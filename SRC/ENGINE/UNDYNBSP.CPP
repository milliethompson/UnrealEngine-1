/*=============================================================================
	UnDynBsp.cpp: Unreal dynamic Bsp object support

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	This code handles all moving brushes with a level.  These are objects which move
	once in a while and are added to and maintained within the level's Bsp whenever they 
	move.

Two forms of information are maintained about moving brushes during gameplay:

	1.	Permanent information.  Information that is allocated once for each moving
		brush: Bsp surfaces and vectors.  These records are updated as
		the brush moves, but they are not deleted/reallocated as the brush moves.  This info
		need only be allocated once because each brush requires a constant amount of this
		stuff, and the amount is known in advance.

	2.	Sporadic information.  Information that is deleted/reallocated for a brush each 
		time the brush moves: Bsp nodes, poly points, vertex pools.  This needs to be 
		reallocated dynamically because a brush requires a changing amount of it based on the 
		amount a moving brush is cut by the Bsp in its current position.

	Safely handles overflow conditions.

Implementation:

		Uses unlinked databases for all permanent and sporadic elements; any moving brush
		operation requires that each element of each database be traversed to see what needs
		to be updated.  This is fast because the databases and element sizes (WORDs) are small.
		New element allocation is performed via a roving feeler.

Major optimizations that could be performed:

	Store each brush as a brep with its own point and vector tables.  Then:
	- Preallocate space for all of the brep's points and vectors, so that only
	  points on clipped poly lines need to be sporadically maintained.
	- Point and vector usage will be minimized.
	- Duplicate points and vectors won't be transformed during rendering.
	- VertPools can be added with side linking.
	- Filtering work can be minimized by filtering the entire brep through the
	  bsp in one pass, instead of a per-poly pass.
	Store each brush as a Bsp and use tree merging.

This class uses the following members of other classes for purposes other than what
they are named/intended:

	* UModel::Color - First node index in the linked list of sporadic brush nodes.
	* FBspNode::iBound - Next node in the linked list of sporadic brush nodes.
	* FPoly::iLink - Bsp surface index corresponding to the FPoly
	* FPoly::iBrushPoly - Light mesh index that applies to the FPoly, or INDEX_NONE if none
	* FBspSurf::iActor - Index of actor whose moving brush owns the surface

Revision history:
	* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

#define PRECOMPUTE_FILTER 1	/* Precompute sphere filter for optimization */

/*---------------------------------------------------------------------------------------
	Brush class implementation.
---------------------------------------------------------------------------------------*/

IMPLEMENT_CLASS(ABrush);
void ABrush::Process( FName Message, PMessageParms *Parms )
{
	guard(AMover::Process);
	AActor::Process( Message, Parms );
	unguard;
}

/*---------------------------------------------------------------------------------------
	Globals.
---------------------------------------------------------------------------------------*/

//
// Tracks moving brushes within a level.  One of these structures is kept for each 
// active level.  The structure is not saved with levels, but is rather rebuild at 
// load-time.
//
// Though multiple FMovingBrushTrackers may exist simultaneously, only one may be
// locked at any time.
//
class FMovingBrushTracker : public FMovingBrushTrackerBase
{
	public:

	///////////////////////////////////////
	// FMovingBrushTrackerBase interface //
	///////////////////////////////////////

	// Constructor.
	FMovingBrushTracker() {Initialized=0; Locked=0;};
	
	// Init/Exit.
	void Init( ULevel *ThisLevel );
	void Exit();

	// Lock/Unlock.
	void Lock();
	void Unlock();

	// Moving brush functions.
	void Flush( AActor *Actor );
	void Update( AActor *Actor );
	int  SurfIsDynamic( INDEX iSurf );

	///////////////////////////////////
	// FMovingBrushTracker interface //
	///////////////////////////////////

	// Operations that can be done while locked.
	void SetupAllBrushes();
	void RemoveAllBrushes();
	void UpdateBrushes(AActor **Actors,int Num);

	// Helper functions.
	static int Locked;
	int Initialized;
	void AssertInitialized()	{if( !Initialized )appError("Not initialized");};
	void AssertLocked()			{if( !Initialized )appError("Not initialized"); if( !Locked )appError("Not locked");};

	// Private functions.
	inline int  NewPointIndex(AActor *Actor);
	inline int  NewVectorIndex(AActor *Actor);
	inline int  NewNodeIndex(AActor *Actor,INDEX iParent);
	inline int  NewSurfIndex(AActor *Actor);
	inline int  NewVertPoolIndex(AActor *Actor,int NumVerts);
	inline int	NewBrushMapIndex(AActor *Actor);

	inline void FreePointIndex(INDEX i);
	inline void FreeVectorIndex(INDEX i);
	inline void FreeNodeIndex(INDEX i);
	inline void FreeSurfIndex(INDEX i);
	inline void FreeVertPoolIndex(DWORD i,int NumVerts);
	inline void FreeBrushMapIndex(INDEX i);

	void SetupActorBrush(AActor *Actor);
	void RemoveActorBrush(AActor *Actor);

	void AddActorBrush(AActor *Actor);
	void FlushActorBrush(AActor *Actor,int Group);

	inline void ForceGroupFlush(INDEX iNode);

	void FilterFPoly(INDEX iNode, INDEX iCoplanarParent, FPoly *EdPoly, int Outside);
	void AddPolyFragment(INDEX iNode, INDEX iCoplanarParent, int IsFront, FPoly *EdPoly);

	///////////////////////////////////
	// FMovingBrushTracker internals //
	///////////////////////////////////

private:
	enum {MAX_MOVING_BRUSH_POLYS=2048};  // Maximum moving brush polys per level.
	enum {MAX_MOVING_BRUSH_ACTORS=256};  // Maximum moving brush actors per level.
	enum {MAX_TOUCHING_ACTORS=512};		 // Maximum actors touched by a moving brush during update.

	static ULevel::Ptr Level;
	static FVector FPolyNormal;

	int iTopNode,iTopSurf,iTopPoint,iTopVector,iTopVertPool,iTopBrushMap;
	AActor *AddActor;
	INDEX iAddSurf;

	// Mechanism to pair actor indices with Bsp surfaces:
	AActor **BrushMapOwners;
	INDEX  *iBrushMapSurfs;
	INDEX  *iNodeParents;

	// Owner actor indices for things that are deleted and realllocated whenever a brush moves.
	AActor **NodeOwners;
	AActor **PointOwners;
	AActor **VertPoolOwners;

	// Owner actor indices for things that are only updated (not deleted/reallocated) whenever a brush moves.
	AActor **SurfOwners;
	AActor **VectorOwners;

	// List of actors to update in a multi-brush update.
	static AActor *GroupActors[MAX_MOVING_BRUSH_ACTORS];
	static int NumGroupActors;

	static AActor *TouchActors[MAX_MOVING_BRUSH_ACTORS];
	static int NumTouchActors;

	// Used by AddPolyFragment.
	static INDEX *iActorNodePrevLink;
};

// FMovingBrushTracker statics.
int			FMovingBrushTracker::Locked;
ULevel::Ptr	FMovingBrushTracker::Level;
FVector		FMovingBrushTracker::FPolyNormal;
AActor*		FMovingBrushTracker::GroupActors[MAX_MOVING_BRUSH_ACTORS];
int			FMovingBrushTracker::NumGroupActors;
AActor*		FMovingBrushTracker::TouchActors[MAX_MOVING_BRUSH_ACTORS];
int			FMovingBrushTracker::NumTouchActors;
INDEX*		FMovingBrushTracker::iActorNodePrevLink;

/*---------------------------------------------------------------------------------------
	Index management.
---------------------------------------------------------------------------------------*/

//
// Allocate an array of INDEX's for the elements in a database object from
// Num to Max.  These elements of level objects are reserved for use by moving 
// brush pieces.
//
INDEX *AllocDbIndex( UDatabase *Res, char *Descr )
{
	guard(FMovingBrushTracker::AllocDbIndex);

	INDEX *Result = (INDEX *)appMallocArray(Res->Max - Res->Num,INDEX,Descr);

	for( int i=Res->Num; i<Res->Max; i++ )
		Result[i - Res->Num] = INDEX_NONE;

	return Result;

	unguard;
}
AActor **AllocDbActor(UDatabase *Res,char *Descr)
{
	guard(FMovingBrushTracker::AllocDbActor);

	AActor **Result = (AActor **)appMallocArray(Res->Max - Res->Num,AActor *,Descr);

	for( int i=Res->Num; i<Res->Max; i++ )
		Result[i - Res->Num] = NULL;

	return Result;

	unguard;
}

//
// Shorten a database object's maximum element count to prevent
// moving brush data from trashing it as a sparse array.  Returns
// the number of active elements in the object.
//
int inline ExpandDb( UDatabase *Res, int Slack=512 )
{
	guard(ExpandDb);

	Res->Max = Max(Res->Max,Res->Num + Res->Num/2 + Slack);
	Res->Realloc();

	return Res->Num;

	unguard;
}

/*---------------------------------------------------------------------------------------
	Public functions.
---------------------------------------------------------------------------------------*/

//
// Flush an actor's brush.
//
void FMovingBrushTracker::Flush( AActor *Actor )
{
	guard(FMovingBrushTracker::Flush);
	
	if( Initialized )
		FlushActorBrush( Actor, 0 );
	
	unguard;
}

//
// Update an actor's brush.
//
void FMovingBrushTracker::Update( AActor *Actor )
{
	guard(FMovingBrushTracker::Update);
	if( Initialized )
	{
		UModel *Brush = Actor->Brush;
		if( Actor->Brush && (Actor->Brush->Location!=Actor->Location || Actor->Brush->Rotation != Actor->Rotation) )
		{
			//todo: Handle collision response.
			Actor->Brush->Location = Actor->Location;
			Actor->Brush->Rotation = Actor->Rotation;
			UpdateBrushes(&Actor,1);
		}
	}
	unguard;
}

//
// Return whether a surface belongs to a moving brush.
//
int FMovingBrushTracker::SurfIsDynamic( INDEX iSurf )
{
	guard(FMovingBrushTracker::SurfIsDynamic);
	
	if( iSurf < Level->Model->Surfs->Num ) 
		return 0;
	return SurfOwners[iSurf - Level->Model->Surfs->Num] != NULL;

	unguard;
}

/*---------------------------------------------------------------------------------------
	FMovingBrushTracker init & exit.
---------------------------------------------------------------------------------------*/

//
// Initialize or reinitialize everything, and allocate all working tables.  Must be 
// followed by a call to UpdateAllBrushes to actually add moving brushes to the world 
// Bsp.  This function assumes that the Bsp is clean when it is called, i.e. it has no 
// references to dynamic Bsp nodes in it.
//
void FMovingBrushTracker::Init( ULevel *ThisLevel )
{
	guard(FMovingBrushTracker::Init);

	if( Initialized )
		appError("Already initialized");

	Level				= ThisLevel;

	iTopNode			= ExpandDb(Level->Model->Nodes);
	iTopSurf			= ExpandDb(Level->Model->Surfs);
	iTopPoint			= ExpandDb(Level->Model->Points,4096);
	iTopVector			= ExpandDb(Level->Model->Vectors,4096);
	iTopVertPool		= ExpandDb(Level->Model->Verts);
	iTopBrushMap		= 0;

	// Note that all actors are unassimilated.
	for( int i=0; i<Level->Num; i++ )
		if( Level(i) )
			Level(i)->bAssimilated = 0;

	BrushMapOwners		= (AActor **)appMallocArray(MAX_MOVING_BRUSH_POLYS,AActor*,"BrushMapOwners");
	for( i=0; i<MAX_MOVING_BRUSH_POLYS; i++ )
		BrushMapOwners[i] = NULL;

	iBrushMapSurfs		= (INDEX *)appMallocArray(MAX_MOVING_BRUSH_POLYS,INDEX,"BrushMapSurfs");

	VertPoolOwners		= (AActor **)appMallocArray(Level->Model->Verts->Max - Level->Model->Verts->Num,AActor*,"VertPoolOwners");
	for( i=0; i<(Level->Model->Verts->Max - Level->Model->Verts->Num); i++ )
		VertPoolOwners[i] = NULL;

	NodeOwners			= AllocDbActor( Level->Model->Nodes,   "NodeOwners"   );
	iNodeParents		= AllocDbIndex( Level->Model->Nodes,   "NodeParents"  );
	SurfOwners			= AllocDbActor( Level->Model->Surfs,   "SurfOwners"   );
	PointOwners			= AllocDbActor( Level->Model->Points,  "PointOwners"  );
	VectorOwners		= AllocDbActor( Level->Model->Vectors, "VectorOwners" );

	// Successfully initialized structure.
	Locked				= 0;
	Initialized			= 1;

	debugf(LOG_Init,"Initialized moving brush tracker for %s",Level->GetName());

	// Now setup and update all brushes.
	Level->Lock(LOCK_ReadWrite);
	UpdateBrushes(NULL,0);

#if CHECK_ALL
	// Make sure that multiple actors don't share the same brush.
	for( i=0; i<Level->Num; i++ )
	{
		AActor *Actor1 = Level(i);
		if( Actor1 && Actor1->IsMovingBrush() )
		{
			for( int j=0; j<i; j++ )
			{
				AActor *Actor2 = Level(j);
				if( Actor2 && Actor2->IsMovingBrush() )
				{
					if( Actor1->Brush == Actor2->Brush )
						appErrorf("Shared brush %s",Actor1->Brush->GetName());
				}
			}
		}
	}
#endif
	Level->Unlock(LOCK_ReadWrite);

	unguard;
}

//
// Clear out the world and free all moving brush data.
//
void FMovingBrushTracker::Exit()
{
	guard(FMovingBrushTracker::Exit);

	AssertInitialized();
	if( Locked ) appError("Locked");

	// Remove all moving brushes.
	Level->Lock(LOCK_ReadWrite);
	RemoveAllBrushes();
	Level->Unlock(LOCK_ReadWrite);

	Initialized=0;

	// Free memory.
	appFree(BrushMapOwners);
	appFree(iBrushMapSurfs);
	appFree(NodeOwners);
	appFree(iNodeParents);
	appFree(SurfOwners);
	appFree(PointOwners);
	appFree(VectorOwners);
	appFree(VertPoolOwners);

	debugf(LOG_Exit,"Shut down moving brush tracker for %s",Level->GetName());

	unguard;
}

/*---------------------------------------------------------------------------------------
	Index functions.
---------------------------------------------------------------------------------------*/

//
// Get an index for a new actor.
//
inline int NewThingActor
(
	int			&TopThing, 
	const int	&NumThings,
	const int	&MaxThings,
	AActor		**ThingOwners,
	AActor		*Actor
)
{
#if CHECK_ALL
	if( (TopThing<NumThings) || (TopThing>=MaxThings) )
		appErrorf ("TopThing inconsistency %i<%i>%i",NumThings,TopThing,MaxThings);
#endif

	int   StartThing    = TopThing;
	AActor **ThingOwner = &ThingOwners[TopThing-NumThings];
	while( TopThing < MaxThings )
	{
		if( *ThingOwner == NULL )
		{
			*ThingOwner = Actor;
			return TopThing;
		}
		TopThing++;
		ThingOwner++;
	}
	TopThing    = NumThings;
	ThingOwner  = &ThingOwners[0];
	while( TopThing < StartThing )
	{
		if (*ThingOwner==NULL)
		{
			*ThingOwner = Actor;
			return TopThing;
		}
		TopThing++;
		ThingOwner++;
	}
#if CHECK_ALL
		appError("NewThingActor overflow");
#endif
	return INDEX_NONE;
}

/*---------------------------------------------------------------------------------------
	Routines to allocate new elements of particular types, for moving brush usage.
	These all call NewThingActor to do their work.
---------------------------------------------------------------------------------------*/

// Get a new Bsp node index.
inline int FMovingBrushTracker::NewNodeIndex(AActor *Actor,INDEX iParent)
{
	guardSlow(FMovingBrushTracker::NewNodeIndex);

	INDEX Result = NewThingActor(iTopNode,Level->Model->Nodes->Num,Level->Model->Nodes->Max,NodeOwners,Actor);

	if( Result!=INDEX_NONE )
	{
#if CHECK_ALL
		if( iNodeParents[Result - Level->Model->Nodes->Num]!=INDEX_NONE )
			appError("Parent duplicate");
#endif
		iNodeParents[Result - Level->Model->Nodes->Num] = iParent;
	}
	return Result;

	unguardSlow;
}

// Get a new Bsp surface index.
inline int FMovingBrushTracker::NewSurfIndex(AActor *Actor)
{
	guardSlow(FMovingBrushTracker::NewSurfIndex);
	return NewThingActor(iTopSurf,Level->Model->Surfs->Num,Level->Model->Surfs->Max,SurfOwners,Actor);
	unguardSlow;
}

// Get a new point index.
inline int FMovingBrushTracker::NewPointIndex(AActor *Actor)
{
	guardSlow(FMovingBrushTracker::NewPointIndex);
	return NewThingActor(iTopPoint,Level->Model->Points->Num,Level->Model->Points->Max,PointOwners,Actor);
	unguardSlow;
}

// Get a new vector index.
inline int FMovingBrushTracker::NewVectorIndex(AActor *Actor)
{
	guardSlow(FMovingBrushTracker::NewVectorIndex);
	return NewThingActor(iTopVector,Level->Model->Vectors->Num,Level->Model->Vectors->Max,VectorOwners,Actor);
	unguardSlow;
}

// Get a new brush-map index.
inline int FMovingBrushTracker::NewBrushMapIndex(AActor *Actor)
{
	guardSlow(FMovingBrushTracker::NewBrushMapIndex);
	return NewThingActor(iTopBrushMap,0,MAX_MOVING_BRUSH_POLYS,BrushMapOwners,Actor);
	unguardSlow;
}

// Get a new vertex pool index.
inline int FMovingBrushTracker::NewVertPoolIndex(AActor *Actor,int NumVerts)
{
	guardSlow(FMovingBrushTracker::NewVertPoolIndex);

#if CHECK_ALL
	if( (iTopVertPool<Level->Model->Verts->Num) || (iTopVertPool>=Level->Model->Verts->Max) )
		appError("TopThing inconsistency");
#endif

	int		iStart		= iTopVertPool;
	int		NumFree		= 0;
	AActor	**Owner		= &VertPoolOwners[iTopVertPool - Level->Model->Verts->Num];

	while( iTopVertPool < (Level->Model->Verts->Max - NumVerts) )
	{
		if( *Owner == NULL )
		{
			if( ++NumFree >= NumVerts )
			{
				while (NumFree-- > 0) *Owner-- = Actor;
				return iTopVertPool+1-NumVerts;
			}
		}
		else NumFree=0;

		iTopVertPool++;
		Owner++;
	}

	iTopVertPool	= Level->Model->Verts->Num;
	NumFree			= 0;
	Owner			= &VertPoolOwners[0];

	while( iTopVertPool < (iStart - NumVerts) )
	{
		if( *Owner == NULL )
		{
			if( ++NumFree >= NumVerts )
			{
				while (NumFree-- > 0) *Owner-- = Actor;
				return iTopVertPool+1-NumVerts;
			}
		}
		else NumFree=0;

		iTopVertPool++;
		Owner++;
	}
#if CHECK_ALL
		appError("NewVertPoolIndex overflow");
#endif
	return -1;
	unguardSlow;
}

/*---------------------------------------------------------------------------------------
	Functions to free things.
---------------------------------------------------------------------------------------*/

// Free a point index.
inline void FMovingBrushTracker::FreePointIndex(INDEX i)
{
#if CHECK_ALL
		if( PointOwners[i-Level->Model->Points->Num]==NULL )
			appError("FreePointIndex inconsistency");
#endif
	PointOwners[i-Level->Model->Points->Num] = NULL;
}

// Free a vector index.
inline void FMovingBrushTracker::FreeVectorIndex(INDEX i)
{
#if CHECK_ALL
		if( VectorOwners[i-Level->Model->Vectors->Num]==NULL )
			appError("FreeVectorIndex inconsistency");
#endif
	VectorOwners[i-Level->Model->Vectors->Num] = NULL;
}

// Free a Bsp node index.
inline void FMovingBrushTracker::FreeNodeIndex(INDEX i)
{
#if CHECK_ALL
		if( NodeOwners[i-Level->Model->Nodes->Num]==NULL )
			appError("FreeNodeIndex node inconsistency");
		
		if( iNodeParents[i-Level->Model->Nodes->Num]==NULL )
			appError("FreeNodeIndex parent inconsistency");
#endif
	NodeOwners  [i-Level->Model->Nodes->Num] = NULL;
	iNodeParents[i-Level->Model->Nodes->Num] = INDEX_NONE;
}

// Free a Bsp surface index.
inline void FMovingBrushTracker::FreeSurfIndex(INDEX i)
{
#if CHECK_ALL
		if( SurfOwners[i-Level->Model->Surfs->Num]==NULL )
			appError("FreeSurfIndex inconsistency");
#endif
	SurfOwners[i-Level->Model->Surfs->Num] = NULL;
}

// Free a vertex pool index.
inline void FMovingBrushTracker::FreeVertPoolIndex(DWORD i,int NumVerts)
{
	AActor **VertPoolOwner = &VertPoolOwners[i - Level->Model->Verts->Num];
	for( int j=0; j<NumVerts; j++ )
	{
#if CHECK_ALL
			if( *VertPoolOwner == NULL )
				appError("FreeVertPoolIndex inconsistency");
#endif
		*VertPoolOwner++ = NULL;
	}
}

// Free a brush-map index.
inline void FMovingBrushTracker::FreeBrushMapIndex(INDEX i)
{
#if CHECK_ALL
	if( BrushMapOwners[i]==NULL )
		appError("FreeBrushMapIndex inconsistency");
#endif
	BrushMapOwners[i]=NULL;
}

/*---------------------------------------------------------------------------------------
	Lock & Unlock.
---------------------------------------------------------------------------------------*/

//
// Lock the moving brush tracker for its level.
// Assumes that the tracker is initialized.
//
void FMovingBrushTracker::Lock()
{
	guard(FMovingBrushTracker::Lock);
	
	if( !Initialized ) return;
	checkState(!Locked);

	Locked = 1;

	unguard;
}

//
// Unlock the moving brush tracker for its level.
//
void FMovingBrushTracker::Unlock()
{
	guard(FMovingBrushTracker::Unlock);
	
	if( !Initialized ) return;
	AssertLocked();

	Locked = 0;

	unguard;
}

/*---------------------------------------------------------------------------------------
	Private, permanent per brush operations.
---------------------------------------------------------------------------------------*/

//
// Setup permanenent information (surfaces, vectors, base points) for a moving
// brush.
//
void FMovingBrushTracker::SetupActorBrush( AActor *Actor )
{
	guard(FMovingBrushTracker::SetupActorBrush);

	AssertLocked();
	checkState(!Actor->bAssimilated);
	checkState(Actor->Brush!=NULL);

	UModel *Brush  = Actor->Brush;
	Brush->Lock(LOCK_ReadWrite);

	Actor->bAssimilated = 1;
	*(INDEX *)&Brush->Color=INDEX_NONE;

	// Create permanent maps for all moving brush FPolys.
	FBspSurf *Surf;
	INDEX    iSurf;
	for( int i=0; i<Brush->Polys->Num; i++ )
	{
		FPoly *Poly = &Brush->Polys->Element(i);

		// Create new surface elements.
		int iBrushMap = NewBrushMapIndex(Actor);
		if( iBrushMap==-1 )
			goto Over1; // Safe overflow.

		iSurf = NewSurfIndex(Actor);
		if( iSurf==INDEX_NONE )
			goto Over2;

		iBrushMapSurfs[iBrushMap] = iSurf;
		Surf = &Level->Model->Surfs(iSurf);

		Surf->vNormal = NewVectorIndex(Actor);
		if( Surf->vNormal==INDEX_NONE )
			goto Over3;

		Surf->vTextureU = NewVectorIndex(Actor);
		if( Surf->vTextureU==INDEX_NONE )
			goto Over4;

		Surf->vTextureV = NewVectorIndex(Actor);
		if( Surf->vTextureV==INDEX_NONE )
			goto Over5;

		Surf->pBase = NewPointIndex( Actor );
		if( Surf->pBase==INDEX_NONE )
			goto Over6;

		Surf->iLightMesh = Poly->iBrushPoly; // May be INDEX_NONE.

		// Set all other surface properties.
		Surf->Texture  		= Poly->Texture;
		Surf->PanU 		 	= Poly->PanU;
		Surf->PanV 		 	= Poly->PanV;
		Surf->PolyFlags 	= Poly->PolyFlags & ~PF_NoAddToBSP;
		Surf->Brush	 		= Brush;
		Surf->iBrushPoly	= i;
		Surf->LastStartY	= 0;
		Surf->LastEndY		= 0;
		Surf->Actor			= (ABrush*)Actor;//!!

		// Link FPoly to this Bsp surface.
		Poly->iLink         = iSurf;

		// Go to next FPoly.
		continue;

		// Overflow cleanup.
		Over6:	FreeVectorIndex(Surf->vTextureV);
		Over5:	FreeVectorIndex(Surf->vTextureU);
		Over4:	FreeVectorIndex(Surf->vNormal);
		Over3:	FreeSurfIndex(iSurf);
				Level->Model->Surfs(iSurf).iLightMesh = INDEX_NONE;
		Over2:	FreeBrushMapIndex(iBrushMap);
		Over1:	;
#if CHECK_ALL
			appErrorf("Overflowed",Actor->GetClass()->GetName());
#endif
		break;
	}
#if CHECK_ALL
	debugf("SetupActorBrush for %s",Actor->GetClass()->GetName());
#endif

	Brush->Unlock(LOCK_ReadWrite);
	unguard;
}

//
// Remove all permanent information (surfaces, vectors, base points) for a moving brush from 
// the level.
//
void FMovingBrushTracker::RemoveActorBrush( AActor *Actor )
{
	guard(FMovingBrushTracker::RemoveActorBrush);
	AssertLocked();
	checkState(Actor->bAssimilated);

	// Find all surfaces owned by this actor, and free them and their contents.
	AActor **BrushMapOwner = &BrushMapOwners[0];
	for( int i=0; i<MAX_MOVING_BRUSH_POLYS; i++ )
	{
		if( *BrushMapOwner == Actor )
		{
			// Free all stuff owned by this surface.
			INDEX    iSurf = iBrushMapSurfs[i];
			FBspSurf *Surf = &Level->Model->Surfs(iSurf);
#if CHECK_ALL
				if( Surf->vNormal  ==INDEX_NONE ) appError("Bad vNormal");
				if( Surf->vTextureU==INDEX_NONE ) appError("Bad vTextureU");
				if( Surf->vTextureV==INDEX_NONE ) appError("Bad vTextureV");
				if( Surf->pBase    ==INDEX_NONE ) appError("Bad pBase");
#endif
			FreeSurfIndex		(iSurf);
			FreePointIndex		(Surf->pBase);
			FreeVectorIndex		(Surf->vTextureV);
			FreeVectorIndex		(Surf->vTextureU);
			FreeVectorIndex		(Surf->vNormal);
			FreeBrushMapIndex	(i);
		}
		BrushMapOwner++;
	}
	Actor->bAssimilated = 0;
	unguard;
}

/*---------------------------------------------------------------------------------------
	Polygon filtering.
---------------------------------------------------------------------------------------*/

void FMovingBrushTracker::AddPolyFragment
(
	INDEX	iParent,
	INDEX	iCoplanarParent,
	int		IsFront,
	FPoly	*EdPoly
)
{
	guard(FMovingBrushTracker::AddPolyFragment);

	FBspNode	*Node,*Parent,*ZoneParent,*FirstParent;
	FVert		*VertPool;
	INDEX		iNode;
	int			i,iVertPool;

	int ZoneFront = IsFront;
	Parent = ZoneParent = FirstParent = &Level->Model->Nodes(iParent);

	// If this node is meant to be added as a coplanar, handle it now.
	int Different=0;
	if( iCoplanarParent != INDEX_NONE )
	{
		Different = iParent!=iCoplanarParent;
		iParent = iCoplanarParent;
		IsFront = 2;
	}

	// Find parent.
	Parent = &Level->Model->Nodes(iParent);
	FirstParent = Parent;
	if( IsFront == 2 ) // Coplanar
	{
		while( Parent->iPlane != INDEX_NONE )
		{
			iParent = Parent->iPlane;
			Parent = &Level->Model->Nodes(iParent);
		}
	}

	// Create a new sporadic node.
	iNode = NewNodeIndex(AddActor,iParent);
	if( iNode == INDEX_NONE )
		goto Over1;
	Node = &Level->Model->Nodes(iNode);

	// Set node's info.
	Node->iSurf       	= iAddSurf;
	Node->iDynamic[0]	= INDEX_NONE;
	Node->iDynamic[1] 	= INDEX_NONE;
	Node->NodeFlags   	= NF_IsNew | NF_Sporadic;
	Node->iBound		= INDEX_NONE;
	Node->ZoneMask		= Parent->ZoneMask;
	Node->NumVertices	= EdPoly->NumVertices;

	if( (iCoplanarParent==INDEX_NONE) || Different )
	{
		Node->iZone[0] = Node->iZone[1]	= ZoneParent->iZone[ZoneFront];
	}
	else
	{
		int IsFlipped  = (Node->Plane|Parent->Plane)<0.0;
		Node->iZone[0] = Parent->iZone[IsFlipped];
		Node->iZone[1] = Parent->iZone[1-IsFlipped];
	}
	FirstParent->ZoneMask |= ((QWORD)1 << Node->iZone[0]) | ((QWORD)1 << Node->iZone[1]);

	Node->iFront		= INDEX_NONE;
	Node->iBack			= INDEX_NONE;
	Node->iPlane		= INDEX_NONE;

	// Make the node's plane from the base and normal.
	Node->Plane = FPlane( EdPoly->Base, EdPoly->Normal );

	// Allocate this node's vertex pool and vertices.
	iVertPool = NewVertPoolIndex(AddActor,EdPoly->NumVertices);
	if (iVertPool==MAXDWORD) goto Over2;

	Node->iVertPool = iVertPool;
	VertPool        = &Level->Model->Verts(iVertPool);

	for( i=0; i<EdPoly->NumVertices; i++ )
	{
		INDEX pVertex		= NewPointIndex(AddActor);
		if( pVertex==INDEX_NONE )
			goto Over3;

		VertPool->iSide		          = INDEX_NONE;
		VertPool->pVertex	          = pVertex;
		Level->Model->Points(pVertex) = EdPoly->Vertex[i];

		VertPool++;
	}

	// Success; link this node to its parent and return.
	// (Can't fail past this point).

#if CHECK_ALL
	if		((IsFront==2)&&(Parent->iPlane!=INDEX_NONE)) appError("iPlane exists");
	else if ((IsFront==1)&&(Parent->iFront!=INDEX_NONE)) appError("iFront exists");
	else if ((IsFront==0)&&(Parent->iBack !=INDEX_NONE)) appError("iBack exists");
#endif

	if		(IsFront==2) Parent->iPlane = iNode;
	else if (IsFront==1) Parent->iFront = iNode;
	else				 Parent->iBack  = iNode;

	*iActorNodePrevLink = iNode;
	iActorNodePrevLink  = &Node->iBound;

	return;

	// Overflow handlers.
	Over3:	while(--i >= 0) FreePointIndex((--VertPool)->pVertex);
	Over2:	FreeNodeIndex(iNode);
	Over1:	;
#if CHECK_ALL
		appError("Overflowed");
#endif
	unguard;
}

void FMovingBrushTracker::FilterFPoly
(
	INDEX	iNode, 
	INDEX	iCoplanarParent,
	FPoly	*EdPoly, 
	int		Outside
)
{
	FPoly		*TempFrontEdPoly, *TempBackEdPoly;
	FBspNode	*Node;
	FBspSurf	*Surf;
	int			SplitResult;

	TempFrontEdPoly	= new(GMem)FPoly;
	TempBackEdPoly	= new(GMem)FPoly;

	FilterLoop:
	Node  = &Level->Model->Nodes(iNode);
	Surf  = &Level->Model->Surfs(Node->iSurf);

	if( EdPoly->NumVertices >= FPoly::VERTEX_THRESHOLD )
	{
		// Must split to avoid vertex overflow.
		TempFrontEdPoly = new(GMem)FPoly;
		EdPoly->SplitInHalf(TempFrontEdPoly);
		FilterFPoly(iNode,iCoplanarParent,TempFrontEdPoly,Outside);
	}
#if CHECK_ALL
#if PRECOMPUTE_FILTER
	if( Surf->PolyFlags & (PF_IsFront | PF_IsBack) )
	{
		// Filter status was precomputed based on bounding sphere.
		if (Surf->PolyFlags & PF_IsFront) goto Front;
		else goto Back;
	}
#endif
#endif
	SplitResult = EdPoly->SplitWithPlaneFast
	(
		Level->Model->Points  (Surf->pBase),
		Level->Model->Vectors (Surf->vNormal),
		TempFrontEdPoly,
		TempBackEdPoly
	);
	if( SplitResult == SP_Front )
		{
#if CHECK_ALL
		if (Surf->PolyFlags & PF_IsBack) appError("Precompute error 1");
#endif
		Front:
		Outside = Outside || Node->IsCsg();
		if( Node->iFront != INDEX_NONE )
		{
			iNode = Node->iFront;
			goto FilterLoop;
		}
		else if (Outside) AddPolyFragment(iNode,iCoplanarParent,1,EdPoly);
	}
	else if( SplitResult == SP_Back )
	{
#if CHECK_ALL
		if (Surf->PolyFlags & PF_IsFront) appError("Precompute error 2");
#endif
		Back:
		Outside = Outside && !Node->IsCsg();
		if( Node->iBack != INDEX_NONE )
		{
			iNode = Node->iBack;
			goto FilterLoop;
		}
		else if (Outside) AddPolyFragment(iNode,iCoplanarParent,0,EdPoly);
	}
	else if( SplitResult == SP_Coplanar )
	{
#if CHECK_ALL
		if (Surf->PolyFlags & (PF_IsFront | PF_IsBack)) appError("Precompute error 3");
#endif
		if ((Level->Model->Vectors(Surf->vNormal) | FPolyNormal) >= 0.0) iCoplanarParent = iNode;
		goto Front;
	}
	else if( SplitResult == SP_Split )
	{
#if CHECK_ALL
		if (Surf->PolyFlags & (PF_IsFront | PF_IsBack)) appError("Precompute error 4");
#endif

		// Handle front fragment.
		if (Node->iFront != INDEX_NONE) FilterFPoly(Node->iFront,iCoplanarParent,TempFrontEdPoly,Outside || Node->IsCsg());
		else if (Outside || Node->IsCsg()) AddPolyFragment(iNode,iCoplanarParent,1,TempFrontEdPoly);

		// Handle back fragment.
		EdPoly			= TempBackEdPoly;
		TempBackEdPoly	= new(GMem)FPoly;
		goto Back;
	}
}

/*---------------------------------------------------------------------------------------
	Private, sporadic per brush operations.
---------------------------------------------------------------------------------------*/

//
// Add all sporadic information (nodes, poly points) for a moving brush to the
// level.  Assumes that no Bsp nodes coming from this actor's brush already exist in the 
// level (those Bsp nodes, if any, must have already been cleaned out by FlushActorBrush).
//
void FMovingBrushTracker::AddActorBrush( AActor *Actor )
{
	guard(FMovingBrushTracker::AddActorBrush);
	checkState(Actor->bAssimilated);
	checkState(Actor->Brush!=NULL);
	AssertLocked();

	FMemMark Mark(GMem);

	iActorNodePrevLink = (INDEX *)(&Actor->Brush->Color);
	AddActor           = Actor;
	UModel *Brush      = Actor->Brush;

	Brush->Lock(LOCK_ReadWrite);

#if CHECK_ALL
	{
		// Verify that no Bsp nodes coming from this actor's brush already exist in the level.
		for( int i=Level->Model->Nodes->Num; i<Level->Model->Nodes->Max; i++ )
		{
			if( NodeOwners[i-Level->Model->Nodes->Num]==Actor )
				appError( "Brush nodes already exist" );
		}

		// Verify that that each FPoly in the brush is referenced once and only once by BrushMapOwners.
		for( i=0; i<Brush->Polys->Num; i++ )
		{
			int Found=0;
			for( int j=0; j<MAX_MOVING_BRUSH_POLYS; j++ )
			{
				if( BrushMapOwners[j]==Actor )
				{
					FBspSurf *Surf = &Level->Model->Surfs(iBrushMapSurfs[j]);
					if (Surf->iBrushPoly == i) Found++;
				}
			}
			if( Found != 1 )
				appErrorf("Surf/FPoly dissociation %i",Found);
		}
	}
#endif

	// Build coordinate system to transform Brush's original polygons into the world according
	// to the Brush's current rotation:
	FModelCoords Coords;
	FLOAT    Orientation = Brush->BuildCoords(&Coords,NULL);
	FVector  Location    = Brush->Location + Brush->PostPivot;

	int Total=0;
	for( int i=0; i<Brush->Polys->Num; i++ )
		Total += Brush->Polys(i).NumVertices;

	FPoly	*TransformedPolys = new( GMem, Brush->Polys->Num )FPoly;
	FVector *Points           = new( GMem, Total )FVector;

	int Count=0;
	for( i=0; i<Brush->Polys->Num; i++ )
	{
		TransformedPolys[i] = Brush->Polys(i);
		TransformedPolys[i].Transform( Coords, &Brush->PrePivot, &Location, Orientation );
#if PRECOMPUTE_FILTER
		for( int j=0; j<TransformedPolys[i].NumVertices; j++ )
			Points[Count++] = TransformedPolys[i].Vertex[j];
#endif
	}
#if PRECOMPUTE_FILTER
	checkState(Count==Total);
	FBoundingVolume Bound;
	Bound.Init( Points, Total );
	Level->Model->PrecomputeSphereFilter( Bound.Sphere );
#endif

	// Go through list of all brush FPolys and update their corresponding Bsp surfaces.
	FPoly *Poly = &TransformedPolys[0];
	for( i=0; i<Brush->Polys->Num; i++ )
	{
		INDEX    iSurf  = Poly->iLink;
		FBspSurf *Surf  = &Level->Model->Surfs(iSurf);

#if CHECK_ALL
		if (SurfOwners[iSurf-Level->Model->Surfs->Num]!=Actor) appError( "iSurfOwner mismatch" );
#endif

		FPolyNormal = Poly->Normal;
		Level->Model->Points (Surf->pBase    ) = Poly->Base;
		Level->Model->Vectors(Surf->vNormal  ) = Poly->Normal;
		Level->Model->Vectors(Surf->vTextureU) = Poly->TextureU;
		Level->Model->Vectors(Surf->vTextureV) = Poly->TextureV;

		// Filter the brush's FPoly through the Bsp, creating new sporadic Bsp nodes (and their 
		// corresponding VertPools and points) for all outside leaves the FPoly fragments fall into:
		guard(Filtering);
		{
			// Scrub precompute info from surface.
			Surf->PolyFlags &= ~(PF_IsFront | PF_IsBack);

			iAddSurf = iSurf;
			if (Level->Model->Nodes->Num > 0 )
				FilterFPoly( 0, INDEX_NONE, Poly, 1 );

		}
		unguard;
		Poly++;
	}
	*iActorNodePrevLink = INDEX_NONE;

	// Tag all newly-added nodes as non-new.
	INDEX iNode = *(INDEX *)&Brush->Color;
	while( iNode != INDEX_NONE )
	{
		FBspNode *Node   = &Level->Model->Nodes(iNode);

#if CHECK_ALL
		if (NodeOwners[iNode-Level->Model->Nodes->Num]!=Actor) appError( "Add node inconsistency" );
#endif

		Node->NodeFlags &= ~NF_IsNew;
		iNode            = Node->iBound;
	}

	Brush->Unlock(LOCK_ReadWrite);
	Mark.Pop();
	unguard;
}

//
// Force the brush that owns a specified Bsp node to be flushed and later
// updated as part of a group flushing operation.
//
void FMovingBrushTracker::ForceGroupFlush(INDEX iNode)
{
	guard(FMovingBrushTracker::ForceGroupFlush);

	AActor *OwnerActor = NodeOwners[iNode - Level->Model->Nodes->Num];
	if( OwnerActor == NULL )
		// Already flushed it
		return;

	for( int i=0; i<NumGroupActors; i++ )
	{
		if (GroupActors[i]==OwnerActor) 
			// Already in flush list.
			return;
	}
	if( NumGroupActors < MAX_MOVING_BRUSH_ACTORS )
	{
		GroupActors[NumGroupActors++] = OwnerActor;
	}
	unguard;
}

//
// Flush all sporadic information (nodes, poly points) for a moving brush from 
// the level.  If Group is true, expects that NumGroupActors and GroupActors 
// are valid.
//
void FMovingBrushTracker::FlushActorBrush( AActor *Actor, int Group )
{
	guard(FMovingBrushTracker::FlushActorBrush);
	AssertLocked();
	checkState(Actor->bAssimilated);

	// Go through all sporadic nodes in the Bsp and find ones owned by this actor.
	INDEX iNode = Actor->Brush->Color;
	while( iNode != INDEX_NONE )
	{
		FBspNode *Node   = &Level->Model->Nodes(iNode);
		INDEX iParent    = iNodeParents[iNode-Level->Model->Nodes->Num];
		FBspNode *Parent = &Level->Model->Nodes(iParent);

#if CHECK_ALL
		if (NodeOwners[iNode-Level->Model->Nodes->Num]!=Actor) appError( "Flush node inconsistency" );
#endif

		// Remove references to this node from its parents.
		if		(Parent->iFront==iNode) Parent->iFront=INDEX_NONE;
		else if (Parent->iBack ==iNode) Parent->iBack =INDEX_NONE;
		else if (Parent->iPlane==iNode) Parent->iPlane=INDEX_NONE;
#if CHECK_ALL
		else appError("Parent mismatch");
#endif
		// If those node has any children belonging to moving brushes that
		// aren't in our group list, tag them for subsequent flushing. This
		// prevents them from being orphaned.
		if( Group )
		{
			if (Node->iFront!=INDEX_NONE)		ForceGroupFlush(Node->iFront);
			if (Node->iBack !=INDEX_NONE)		ForceGroupFlush(Node->iBack );
			if (Node->iPlane!=INDEX_NONE)		ForceGroupFlush(Node->iPlane);
		}

		// Free all sporadic data.
		FVert *VertPool = &Level->Model->Verts(Node->iVertPool);
		for( DWORD j=0; j<Node->NumVertices; j++ )
		{
			FreePointIndex(VertPool->pVertex);
			VertPool++;
		}
		FreeVertPoolIndex    (Node->iVertPool,Node->NumVertices);
		FreeNodeIndex        (iNode);
		iNode = Node->iBound;
	}
	unguard;
}

/*---------------------------------------------------------------------------------------
	Public operations.
---------------------------------------------------------------------------------------*/


//
// Setup all brushes in the level.  Cleans out all permanent and sporadic data, then
// sets up permanent data (surfaces, vectors) for all moving brushes.  Sporadic data
// isn't added until the next call to UpdateAllBrushes.
//
void FMovingBrushTracker::SetupAllBrushes()
{
	guard(FMovingBrushTracker::SetupAllBrushes);
	AssertLocked();

	RemoveAllBrushes();

	for( int i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level(i);
		if( Actor && Actor->IsMovingBrush() )
			SetupActorBrush( Actor );
	}
	unguard;
}

//
// Remove all permanent and sporadic moving brush data.
// Note that this must be called before a level is saved, or else Bsp nodes that
// should be leaves will reference nodes in the range from Num-Max that don't exist 
// in the saved file resulting.
//
void FMovingBrushTracker::RemoveAllBrushes()
{
	guard(FMovingBrushTracker::RemoveAllBrushes);
	AssertLocked();

	// Flush all sporadic data.
	guard(FlushSporadics);
	for( int i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level(i);
		if( Actor && Actor->IsMovingBrush() && Actor->bAssimilated )
			FlushActorBrush( Actor, 0 );
	}
	unguard;

	// Verify that all dynamic nodes were actually removed.
#if CHECK_ALL
	guard(CheckSporadics);
	int n = Level->Model->Nodes->Num;
	for( int i=0; i<n; i++ )
	{
		FBspNode *Node = &Level->Model->Nodes(i);

		if ( (Node->iFront!=INDEX_NONE) && (Node->iFront >= n) )	appErrorf( "Bad iFront %i",i );
		if ( (Node->iBack !=INDEX_NONE) && (Node->iBack  >= n) )	appErrorf( "Bad iBack %i", i );
		if ( (Node->iPlane!=INDEX_NONE) && (Node->iPlane >= n) )	appErrorf( "Bad iPlane %i",i );
	}
	unguard;
#endif

	// Remove all permanent data.
	guard(RemoveSporadics);
	for( int i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level(i);
		if( Actor && Actor->IsMovingBrush() && Actor->bAssimilated )
			RemoveActorBrush(Actor);
	}
	unguard;
	unguard;
}

//
// Update certain moving brushes in the level.  Call with Actors=NULL to update all moving
// brushes, or with a list of specific brushes to be updated in lock-step.  All of the
// specified moving brushes (and brushes interwoved with their Bsp's) are cleaned out of the 
// Bsp, and then re-added.
//
void FMovingBrushTracker::UpdateBrushes( AActor **Actors, int Num )
{
	guard(FMovingBrushTracker::UpdateBrushes);

	AssertLocked();
	int Group;

	if( Actors == NULL )
	{
		// Update all actor brushes.
		Group = 0;
		NumGroupActors = 0;

		for( int iActor=0; iActor<Level->Num; iActor++ )
		{
			AActor *Actor = Level(iActor);
			if( Actor && Actor->IsMovingBrush() )
			{
				GroupActors[NumGroupActors++] = Actor;
				if (NumGroupActors >= MAX_MOVING_BRUSH_ACTORS) 
					break;
			}
		}
	}
	else
	{
		// Update specified actor brushes.
		Group = 1;
		NumGroupActors = Min(Num,(int)MAX_MOVING_BRUSH_ACTORS);
		memcpy( GroupActors, Actors, Num * sizeof(AActor *) );
	}

	// Init touch actors so that they can be added back later.
	NumTouchActors = 0;

	// Flush all actor brushes.  Calls to FlushActorBrush with Group set to true
	// cause GroupActors to be expanded to include all brushes interwoven with the
	// specified brushes.
	for( int i=0; i<NumGroupActors; i++ )
	{
		AActor *Actor = GroupActors[i];
		if( Actor->GetClass() && Actor->IsMovingBrush() && Actor->bAssimilated )
			FlushActorBrush(Actor,Group);
	}

	// Add all moving brush sporadic data to the world.
	for( i=0; i<NumGroupActors; i++ )
	{
		AActor *Actor = GroupActors[i];
		if( Actor->GetClass() && Actor->IsMovingBrush() )
		{
			if( !Actor->bAssimilated )
				SetupActorBrush(Actor);
			AddActorBrush(Actor);
		}
	}
	unguard;
}

/*---------------------------------------------------------------------------------------
	Instantiation.
---------------------------------------------------------------------------------------*/

UNENGINE_API class FMovingBrushTracker GBrushTrackerInstance;
UNENGINE_API class FMovingBrushTrackerBase &GBrushTracker = GBrushTrackerInstance;

/*---------------------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------------------*/
