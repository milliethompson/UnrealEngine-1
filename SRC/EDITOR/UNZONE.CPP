/*=============================================================================
	UnZone.cpp: Zone visibility computation

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	This is all the code needed for assigning visibility zones to nodes
	and filling in all of the zone-related visibility information structures
	in the Bsp.  This zone information is entirely optional during editing
	and rendering, but it accelerates rendering significantly because it allows
	the renderer to discard occluded polygons and regions far more
	efficiently.

Definitions:
	hull:
		The convex bounding volume of a Bsp node, which is guaranteed to be of finite
		size because all geometry in the world has been subtracted out of an initially
		solid space.
	portal:
		An invisible polygon in the world that divides the world's open spaces into 
		watertight zones.
	zone:
		A zone is a logical, possibly nonconvex empty volume of the Bsp which is grouped 
		together for visibility determinated, planned out by the level designer.  A zone 
		consists of one or more adjacent, connected leaf nodes and all of the Bsp node 
		polys which fall into the zone.  A level can have up to MAX_ZONES (64) zones.  
		During rendering, visible zones are tracked and all polys which are known to be 
		in occluded zones are skipped.

Design notes:
 *	Zones are bounded by solid, occluding polygons, and invisibile zone portals.
	Zone portals are added in by level designers and typically inserted in the
	middle of doors and hallways to logically divide space into areas.
 *	This zone-portal scheme bears no resemblance to Quake's PCVS (precomputed visibilty set)
	method, which actually computes node-to-node visibility.  Zone visibility only
	computes zones and their connecting portals, and visibility is computed on
	the fly during rendering.
 *	Any node can contain two leaves: A front leaf (if the node's front is outside
	and iFront is empty), and a back leaf (if the back is outside and iBack is empty).

Revision history:
	Tim: Created
	10/7/96, Tim: Fixed bug with portals coplanar to world polys

=============================================================================*/

#include "Unreal.h"

// Options.
#define DEBUG_HULLS	0	/* Debugging hull code by generating hull brush */

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

// Constants.
#define WORLD_MAX 65536.0		/* Maximum size of the world */
#define MAX_DEPTH 4096			/* Maximum depth of the Bsp from root to the lowest leaf */
enum {MAX_BOUND_POLYS=96};		/* Maximum polys we can handle in a convex volume */

// States during FilterToLeaf routine.
enum EZoneFilterState
{
	FILTERING_DOWN_FRONT,		// First, poly is filtered through the front to leaves.
	FILTERING_DOWN_BACK,		// Then, poly chunks are filtered through the back to leaves.
};

// A remembered node cutting plane during MergeAdjacentZones.
class FZoneFilterCuttingPlane
{
public:
	int		IsFront;
	FVector *Normal;
	FVector	*Base;
};

// Class passed around during zone building operations.
class FZoneFilter
{
public:
	// Variables.
	ULevel					*Level;			// Level this Bsp resides in.
	FZoneFilterCuttingPlane *Planes;		// List of cutting planes in MergeAdjacentZones.
	INDEX					*NodeZones[2];	// Full zone numbers (0-65535), later reduced to byte iZone's.
	int						NumPlanes;		// Number of cutting planes in MergeAdjacentZones.
	int						NumPortals;		// Number of zone portals found on polys with PF_ZONE_PORTAL tag.
	int						PortalChunks;	// Number of portal polys filtered down to leaves.

	// Functions.
	void FilterToLeaf(FPoly *EdPoly,INDEX iOtherNode,INDEX iPrevNode,INDEX iNode,EZoneFilterState State, int Outside, int BackOutside, ENodePlace PrevPlace, int PortalBlocked);
	void AssignUniqueLeafZones(INDEX iPrev,INDEX iNode, INT &iTopZone,int Outside,ENodePlace PrevPlace);
	void MergeAdjacentZones(INDEX iNode, int Outside);
	void PerformMergeAdjacentZones(INDEX iNode, int Outside, FPoly OnceInfiniteEdPoly, int iParentCuttingPlane, int NumPlanes);
	void MergeTwoZones(INDEX iZone1, INDEX iZone2);
	void RemapZones();
	void FillZone(INDEX iNode,INDEX iZone);
	void AssignAllZones(INDEX iNode,int Outside);
	void FindSingleZone(INDEX iPrevNode,INDEX iNode,FPoly *EdPoly,BYTE &iThisZone,int &MultipleZones,int Outside,ENodePlace PrevPlace);
	void FilterMultiZonePoly(INDEX iSourceNode,INDEX iPrevNode,INDEX iNode,FPoly *EdPoly,int Outside,ENodePlace PrevPlace);
	void FilterPortalToLeaves(INDEX iPrevNode,INDEX iNode,FPoly *EdPoly,ENodePlace PrevPlace,int Outside);
	void BuildConnectivity();
	void BuildVisibility();
	void BuildZoneInfo();
	QWORD BuildZoneMasks(INDEX iNode);
};

/*-----------------------------------------------------------------------------
	Non-class functions.
-----------------------------------------------------------------------------*/

//
// Build an FPoly representing an "infinite" plane (which exceeds the maximum
// dimensions of the world in all directions) for a particular Bsp node.
//
void BuildInfiniteFPoly( UModel *Model, INDEX iNode, FPoly &EdPoly )
{
	guard(BuildInfiniteFPoly);

	FBspNode &Node   = Model->Nodes  (iNode       );
	FBspSurf &Poly   = Model->Surfs	 (Node.iSurf  );
	FVector  &Base   = Model->Points (Poly.pBase  );
	FVector  &Normal = Model->Vectors(Poly.vNormal);
	FVector	 Axis1,Axis2;

	// Find two non-problematic axis vectors.
	Normal.FindBestAxisVectors(Axis1,Axis2);

	// Set up the FPoly.
	EdPoly.Init();
	EdPoly.NumVertices = 4;
	EdPoly.Normal      = Normal;
	EdPoly.Base        = Base;
	EdPoly.Vertex[0]   = Base + Axis1*WORLD_MAX + Axis2*WORLD_MAX;
	EdPoly.Vertex[1]   = Base - Axis1*WORLD_MAX + Axis2*WORLD_MAX;
	EdPoly.Vertex[2]   = Base - Axis1*WORLD_MAX - Axis2*WORLD_MAX;
	EdPoly.Vertex[3]   = Base + Axis1*WORLD_MAX - Axis2*WORLD_MAX;

#if CHECK_ALL // Validate the poly
	if( EdPoly.SplitWithNode( Model, iNode, NULL, NULL, 0 )!=SP_Coplanar)
		appError( "BuildInfinitePoly failed" );
#endif
	unguard;
}

/*-----------------------------------------------------------------------------
	FZoneFilter: Main zone merging functions and callbacks.
-----------------------------------------------------------------------------*/

//
// Filter a once-infinite poly EdPoly through the Bsp, splitting it as we go, 
// until we get to a leaf, then process the resulting poly chunks based on the 
// filter State.
//
// iPrevNode = index of previous node in chain (guaranteed not INDEX_NONE)
// If FILTERING_DOWN_FRONT: iOtherNode = index of back node to progress down next
// If FILTERING_DOWN_BACK:  iOtherNode = index of original front node's iZone we encountered
//
void FZoneFilter::FilterToLeaf
(
	FPoly				*EdPoly,
	INDEX				iOtherNode,
	INDEX				iPrevNode,
	INDEX				iNode,
	EZoneFilterState	State,
	INT					Outside, 
	INT					BackOutside, 
	ENodePlace			PrevPlace,
	INT					PortalBlocked
)
{
	guard(FZoneFilter::FilterToLeaf);

	FilterLoop:
	if( EdPoly->NumVertices >= FPoly::VERTEX_THRESHOLD )
	{
		FPoly TempEdPoly;
		EdPoly->SplitInHalf(&TempEdPoly);
		FilterToLeaf(&TempEdPoly,iOtherNode,iPrevNode,iNode,State,Outside,BackOutside,PrevPlace,PortalBlocked);
	}
	if( iNode != INDEX_NONE )
	{
		FBspNode &Node   = Level->Model->Nodes( iNode );
		FPoly	 FrontPoly,BackPoly;
		switch( EdPoly->SplitWithNode(Level->Model,iNode,&FrontPoly,&BackPoly,0) )
		{
			case SP_Coplanar:
				debugf(LOG_Info,"FZoneFilter::FilterToLeaf: Got coplanar");

			case SP_Front:
				Outside   = Outside || Node.IsCsg();
				iPrevNode = iNode;
				iNode     = Node.iFront;
				PrevPlace = NODE_Front;
				goto FilterLoop;

			case SP_Back:
				Outside   = Outside && !Node.IsCsg();
				iPrevNode = iNode;
				iNode     = Node.iBack;
				PrevPlace = NODE_Back;
				goto FilterLoop;

			case SP_Split:
				FilterToLeaf
				(
					&FrontPoly,iOtherNode,iNode,Node.iFront,State,
					Outside ||  Node.IsCsg(),BackOutside,NODE_Front,
					PortalBlocked
				);
				FilterToLeaf
				(
					&BackPoly, iOtherNode,iNode,Node.iBack, State,
					Outside && !Node.IsCsg(),BackOutside,NODE_Back,
					PortalBlocked
				);
				break;
			
			default:
				appError("FZoneFilter::FilterToLeaf: Unknown split code");
		}
	}
	else if( State==FILTERING_DOWN_FRONT && Outside )
	{
		// Now iOtherNode = Lowest common parent node.
		//     iPrevNode  = Lowest valid front node.
		//     iNode      = INDEX_NONE.

		if( iOtherNode==INDEX_NONE ) appError( "Error3" );
		if( iPrevNode ==INDEX_NONE ) appError( "Error4" );

		FBspNode &Parent = Level->Model->Nodes(iOtherNode);
		DWORD    Flags   = Level->Model->Nodes(iPrevNode).NodeFlags;

		// On the next iteration:
		//     iOtherNode = The front leaf's zone (=PrevNode.Zone)
		//     iPrevNode  = Lowest common parent (=iOtherNode)
		//     iNode      = (iOtherNode -> iBack)
		FilterToLeaf
		(
			EdPoly,
			NodeZones[PrevPlace][iPrevNode],
			iOtherNode,
			Parent.iBack,
			FILTERING_DOWN_BACK,
			BackOutside,
			BackOutside,
			NODE_Back,
			(Flags & (PrevPlace==NODE_Front ? NF_PortalFront : NF_PortalBack)) != 0
		);
	}
	else if( State==FILTERING_DOWN_BACK && Outside )
	{
		// Here we've reached a destination back leaf and therefore a non-empty polygon has 
		// survived the trip down both the front and the back of the source node, and we
		// know that both original nodes are adjacent and mutually visible.

		// Now iOtherNode  = The front leaf's iZone
		//     iPrevNode   = Lowest valid back node
		//     iNode       = INDEX_NONE
		if( NodeZones[PrevPlace][iPrevNode]==INDEX_NONE )
			appError( "Error2B" );
		
		if
		(	!PortalBlocked 
		||	!(Level->Model->Nodes(iPrevNode).NodeFlags & (PrevPlace==NODE_Front?NF_PortalFront:NF_PortalBack)) )
		{
			MergeTwoZones( iOtherNode, NodeZones[PrevPlace][iPrevNode] );
		}
	}
	unguard;
}

//
// Filter a portal polygon EdPoly down to leaves, tagging all leaves that its
// chunks fall in as NF_PortalX, which blocks adjacency calculations at those
// leaves.  This disregards inside/outside, since the portal FPoly can be
// assumed to be clipped to only the inside of the Bsp.
//
void FZoneFilter::FilterPortalToLeaves
(
	INDEX		iPrevNode,
	INDEX		iNode,
	FPoly		*EdPoly,
	ENodePlace	PrevPlace,
	INT			Outside)
{
	guard(FZoneFilter::FilterPortalToLeaves);

	FilterLoop:
	if( EdPoly->NumVertices >= FPoly::VERTEX_THRESHOLD )
	{
		FPoly TempEdPoly;
		EdPoly->SplitInHalf(&TempEdPoly);
		FilterPortalToLeaves(iPrevNode,iNode,&TempEdPoly,PrevPlace,Outside);
	}
	if( iNode != INDEX_NONE )
	{
		FBspNode &Node = Level->Model->Nodes( iNode );
		FPoly	 FrontPoly,BackPoly;
		switch( EdPoly->SplitWithNode( Level->Model, iNode, &FrontPoly, &BackPoly, 0) )
		{
			case SP_Coplanar:
				debugf(LOG_Info,"FZoneFilter::FilterPortalToLeaves: Got coplanar");
			
			case SP_Front:
				iPrevNode = iNode;
				iNode     = Node.iFront;
				Outside   = Outside || Node.IsCsg();
				PrevPlace = NODE_Front;
				goto FilterLoop;
			
			case SP_Back:
				iPrevNode = iNode;
				iNode     = Node.iBack;
				Outside   = Outside && !Node.IsCsg();
				PrevPlace = NODE_Back;
				goto FilterLoop;
			
			case SP_Split:
				FilterPortalToLeaves(iNode,Node.iFront,&FrontPoly,NODE_Front,Outside || Node.IsCsg());
				iPrevNode = iNode;
				iNode     = Node.iBack;
				Outside   = Outside && !Node.IsCsg();
				PrevPlace = NODE_Back;
				break;
		}
	}
	else
	{
		// At a leaf.
		PortalChunks++;
		
		if( !Outside )
			debugf( LOG_Info, "Portal manifold inconsistency" );
		
		Level->Model->Nodes(iPrevNode).NodeFlags |= (PrevPlace==NODE_Front ? NF_PortalFront : NF_PortalBack);
	}
	unguard;
}

//
// Filter a once-infinte EdPoly through the Bsp, cutting it by each parent cutting plane,
// until we get to the desired node.  Then, begin filtering the poly to all leaves.  This
// is a worker function called by MergeAdjacentZones.
//
void FZoneFilter::PerformMergeAdjacentZones
(
	INDEX	iNode,
	INT		Outside, 
	FPoly	OnceInfiniteEdPoly,
	INT		iParentCuttingPlane, 
	INT		NumPlanes
)
{
	guard(FZoneFilter::PerformMergeAdjacentZones);

	// Clip the infinite poly by all parent cutting planes, so that it's
	// restricted to its proper convex hull.
	while( iParentCuttingPlane < NumPlanes )
	{
		if( OnceInfiniteEdPoly.NumVertices >= FPoly::VERTEX_THRESHOLD )
		{
			FPoly TempEdPoly;
			OnceInfiniteEdPoly.SplitInHalf(&TempEdPoly);
			PerformMergeAdjacentZones( iNode, Outside, TempEdPoly, iParentCuttingPlane, NumPlanes );
		}
		FZoneFilterCuttingPlane *Cut = &Planes[iParentCuttingPlane];
		FPoly FrontPoly,BackPoly;
		switch( OnceInfiniteEdPoly.SplitWithPlane(*Cut->Base,*Cut->Normal,&FrontPoly,&BackPoly,0) )
		{
			case SP_Front:
				if (!Cut->IsFront)
					// This piece of the poly was clipped to oblivion.
					return;
				break;
			case SP_Back:
				if (Cut->IsFront)
					// This piece of the poly was clipped to oblivion.
					return;
				break;
			
			case SP_Split:
				// Keep the one piece we want.
				if (Cut->IsFront)
					OnceInfiniteEdPoly = FrontPoly;
				else
					OnceInfiniteEdPoly = BackPoly;
				break;
			
			case SP_Coplanar:
				// Inconsistency due to numerical precision problem.
				debugf( LOG_Info, "FZoneFilter::MergeAdjacentZones: Invalid coplanar" );
				return;
		}
		iParentCuttingPlane++;
	}

	// Filter it through the Bsp down to leaves and tag any leaves that are proven
	// to be adjacent.	
	FBspNode &Node = Level->Model->Nodes(iNode);
	FilterToLeaf
	(
		&OnceInfiniteEdPoly, iNode, iNode, Node.iFront, FILTERING_DOWN_FRONT,
		Outside || Node.IsCsg(), Outside && !Node.IsCsg(), NODE_Front, 0
	);
	unguard;
}

//
// Recurse through the Bsp, merging zones that are adjacent.
//
void FZoneFilter::MergeAdjacentZones( INDEX iNode, int Outside )
{
	guard(FZoneFilter::MergeAdjacentZones);

	FBspNode	&Node   = Level->Model->Nodes(iNode);
	FBspSurf	&Poly	= Level->Model->Surfs(Node.iSurf);
	FVector		Normal  = Node.Plane;

	// Recursively merge adjacents in front and back.
	// Tracks a list of cutting planes from the root down to
	// this node so that the hull bounding polys can be clipped
	// to their appropriate parent volumes.
	Planes[NumPlanes].Normal = &Level->Model->Vectors(Poly.vNormal);
	Planes[NumPlanes].Base   = &Level->Model->Points (Poly.pBase);
	
	if( Node.iFront != INDEX_NONE )
	{
		Planes[NumPlanes++].IsFront = 1;
		MergeAdjacentZones(Node.iFront,Outside || Node.IsCsg());
		NumPlanes--;
	}
	if( Node.iBack != INDEX_NONE )
	{
		Planes[NumPlanes++].IsFront = 0;
		MergeAdjacentZones(Node.iBack, Outside && !Node.IsCsg());
		NumPlanes--;
	}

	// For all coplanar PF_ZonePortal in this node, filter them down to
	// leaves and tag those leaves as NF_PortalFront or NF_PortalBack.  This 
	// causes zone portal polygons to block the adjacency computations.
	for( int i=0; i<Level->Model->Nodes->Num; i++ )
		Level->Model->Nodes(i).NodeFlags &= ~(NF_PortalFront | NF_PortalBack);
	
	INDEX iTempNode = iNode;
	while( iTempNode != INDEX_NONE )
	{
		FBspNode &TempNode = Level->Model->Nodes(iTempNode);
		FBspSurf &TempSurf = Level->Model->Surfs(TempNode.iSurf);
		
		if( TempSurf.PolyFlags & PF_Portal )
		{
			NumPortals++;
			FPoly PortalPoly;
			if( GUnrealEditor.bspNodeToFPoly( Level->Model, iTempNode, &PortalPoly ) )
			{
				INT Forwards = ((PortalPoly.Normal | Normal) > 0.0 );
				if( !Forwards ) PortalPoly.Reverse();
				FilterPortalToLeaves( iNode, Node.iFront, &PortalPoly, Forwards ? NODE_Front : NODE_Back,  Outside ||  Node.IsCsg() );
				FilterPortalToLeaves( iNode, Node.iBack,  &PortalPoly, Forwards ? NODE_Back  : NODE_Front, Outside && !Node.IsCsg() );
			}
		}
		iTempNode = TempNode.iPlane;
	}

	// Build an "infinite" FPoly representing this node's plane.
	FPoly InfiniteEdPoly;
	BuildInfiniteFPoly(Level->Model,iNode,InfiniteEdPoly);

	// Merge all zones which the infinite poly separates.
	PerformMergeAdjacentZones( iNode, Outside, InfiniteEdPoly, 0, NumPlanes );

	unguard;
}

/*-----------------------------------------------------------------------------
	FZoneFilter: Misc zone functions.
-----------------------------------------------------------------------------*/

//
// Assign a unique zone number to every leaf node of the Bsp.
//
void FZoneFilter::AssignUniqueLeafZones
(
	INDEX		iPrev,
	INDEX		iNode,
	INT 		&iTopZone,
	INT			Outside,
	ENodePlace	PrevPlace
)
{
	guard(FZoneFilter::AssignUniqueLeafZones);

	if( iNode != INDEX_NONE )
	{
		FBspNode &Node = Level->Model->Nodes(iNode);
		
		AssignUniqueLeafZones( iNode, Node.iFront, iTopZone, Outside ||  Node.IsCsg(), NODE_Front );
		AssignUniqueLeafZones( iNode, Node.iBack , iTopZone, Outside && !Node.IsCsg(), NODE_Back  );
	}
	else
	{
		if( Outside )
			NodeZones[PrevPlace][iPrev] = iTopZone++;
	}
	unguard;
}

//
// Merge two zones.  Goes through all nodes in the world and replaces
// references to the second zone with the first.
//
void FZoneFilter::MergeTwoZones( INDEX iZone1, INDEX iZone2 )
{
	guard(FZoneFilter::MergeTwoZones);
	
	if( iZone1 != iZone2 )
		for( int i=0; i<Level->Model->Nodes->Num; i++ )
			for( int j=0; j<2; j++ )
				if( NodeZones[j][i] == iZone2 )
					NodeZones[j][i] = iZone1;

	unguard;
}

//
// Renumber all zones in the Bsp so that zone numbers go from 1 to the maximum
// with no gaps.
//
void FZoneFilter::RemapZones()
{
	guard(FZoneFilter::RemapZones);

	FMemMark Mark(GMem);
	BYTE *ZoneRemap = new(GMem,MEM_Zeroed,Level->Model->Nodes->NumZones)BYTE;

	int NewNumZones = 0;
	for( int i=0; i<Level->Model->Nodes->Num; i++ )
	{
		FBspNode &Node = Level->Model->Nodes(i);
		for( int j=0; j<2; j++ )
		{
			if( NodeZones[j][i] != INDEX_NONE )
			{
				if( ZoneRemap[NodeZones[j][i]]==0 )
					ZoneRemap[NodeZones[j][i]] = 1+((NewNumZones++)%63);
				
				Node.iZone[j] = ZoneRemap[NodeZones[j][i]];
			}
			else Node.iZone[j] = 0;
		}
	}

	debugf( LOG_Info, "zoneBuild reduced %i zones down to %i (%i)", Level->Model->Nodes->NumZones, NewNumZones+1, Min(64,NewNumZones+1) );
	Level->Model->Nodes->NumZones = Min(64,NewNumZones+1);
	Mark.Pop();
	unguard;
}

/*-----------------------------------------------------------------------------
	FZoneFilter: Assigning zone numbers.
-----------------------------------------------------------------------------*/

//
// Filter a poly through the Bsp, figure out which zone it lies in, and set iThisZone to
// its number.  This routine hopes that all chunks of the poly lie in the same zone, but
// it sets MultipleZones to 1 if this isn't the case.
//
void FZoneFilter::FindSingleZone
(
	INDEX	iPrevNode,
	INDEX	iNode,
	FPoly	*EdPoly,
	BYTE	&iThisZone,
	INT		&MultipleZones,
	INT		Outside,
	ENodePlace PrevPlace
)
{
	guard(FZoneFilter::FindSingleZone);

	FilterLoop:
	if( EdPoly->NumVertices >= FPoly::VERTEX_THRESHOLD )
	{
		FPoly TempEdPoly;
		EdPoly->SplitInHalf(&TempEdPoly);
		FindSingleZone( iPrevNode, iNode, &TempEdPoly, iThisZone, MultipleZones, Outside, PrevPlace );
	}
	if( iNode != INDEX_NONE )
	{
		// Filter through.
		FBspNode &Node = Level->Model->Nodes( iNode );
		FPoly	 FrontPoly,BackPoly;
		switch( EdPoly->SplitWithNode( Level->Model, iNode, &FrontPoly, &BackPoly, 0 ) )
		{
			case SP_Coplanar:
				debugf( LOG_Info, "Coplanar" );
				// Continue on as if it's front.
			
			case SP_Front:
				iPrevNode	= iNode;
				iNode		= Node.iFront;
				Outside     = Outside || Node.IsCsg();
				PrevPlace   = NODE_Front;
				goto FilterLoop;
				break;
			
			case SP_Back:
				iPrevNode	= iNode;
				iNode		= Node.iBack;
				Outside     = Outside && !Node.IsCsg();
				PrevPlace   = NODE_Back;
				goto FilterLoop;
				break;

			case SP_Split:
				FindSingleZone( iNode, Node.iFront, &FrontPoly, iThisZone, MultipleZones, Outside ||  Node.IsCsg(), NODE_Front );
				FindSingleZone( iNode, Node.iBack,  &BackPoly,  iThisZone, MultipleZones, Outside && !Node.IsCsg(), NODE_Back  );
				break;
		}
	}
	else
	{
		// iPrevNode is a leaf.
		if( Outside )
		{
			FBspNode &PrevNode = Level->Model->Nodes(iPrevNode);
			INDEX    iPrevZone = PrevNode.iZone[PrevPlace==NODE_Front];

			if( iPrevZone == 0 )
				appError("FZoneFilter::FindSingleZone: Unzoned leaf");

			if		(iThisZone==0)			iThisZone     = iPrevZone;
			else if (iThisZone!=iPrevZone)	MultipleZones = 1;
		}
	}
	unguard;
}

//
// Filter a polygon down the Bsp and add it to leaves, assigning the proper zone numbers
// to it.  This is called when a Bsp node poly's chunks are found to reside in more than
// one zone.  To render it properly, it must be split up so that each piece lies in one
// and only one zone.
//
void FZoneFilter::FilterMultiZonePoly
(
	INDEX		iSourceNode,
	INDEX		iPrevNode,
	INDEX		iNode,
	FPoly		*EdPoly,
	int			Outside,
	ENodePlace	PrevPlace
)
{
	guard(FZoneFilter::FilterMultiZonePoly);

	FilterLoop:
	if( EdPoly->NumVertices >= FPoly::VERTEX_THRESHOLD )
	{
		FPoly TempEdPoly;
		EdPoly->SplitInHalf(&TempEdPoly);
		FilterMultiZonePoly(iSourceNode,iPrevNode,iNode,&TempEdPoly,Outside,PrevPlace);
	}
	if( iNode != INDEX_NONE )
	{
		// Filter through.
		INT   IsCsg = Level->Model->Nodes(iNode).IsCsg();
		FPoly FrontPoly,BackPoly;
		switch( EdPoly->SplitWithNode( Level->Model, iNode, &FrontPoly, &BackPoly, 0 ) )
		{
			case SP_Coplanar:
				// Happens occastionally due to precision errors.
				// Arbitrarily treat coplanars as front so they're most likely to get some zone
				// assigned to them.
			
			case SP_Front:
				iPrevNode	= iNode;
				iNode		= Level->Model->Nodes(iNode).iFront;
				Outside     = Outside || IsCsg;
				PrevPlace	= NODE_Front;
				goto FilterLoop;
			
			case SP_Back:
				iPrevNode	= iNode;
				iNode		= Level->Model->Nodes(iNode).iBack;
				Outside     = Outside && !IsCsg;
				PrevPlace	= NODE_Back;
				goto FilterLoop;
			
			case SP_Split:
				FilterMultiZonePoly( iSourceNode, iNode, Level->Model->Nodes(iNode).iFront, &FrontPoly, Outside ||  IsCsg, NODE_Front );
				FilterMultiZonePoly( iSourceNode, iNode, Level->Model->Nodes(iNode).iBack,  &BackPoly,  Outside && !IsCsg, NODE_Back  );
				break;
		}
	}
	else
	{
		// iPrevNode is a leaf.
		if( Outside )
		{
			INDEX iPrevZone = Level->Model->Nodes(iPrevNode).iZone[PrevPlace==NODE_Front];
			if( iPrevZone == 0 )
				appError("FZoneFilter::FilterMultiZonePoly: Zone inconsistency");

			INDEX iNewNode = GUnrealEditor.bspAddNode
			(
				Level->Model, 
				iSourceNode, 
				NODE_Plane,
				Level->Model->Nodes(iSourceNode).NodeFlags | NF_IsNew,
				EdPoly
			);
			Level->Model->Nodes(iNewNode).iZone[1] = iPrevZone;
			Level->Model->Nodes(iNewNode).iZone[0] = iPrevZone;

			INDEX iSurf = Level->Model->Nodes(iSourceNode).iSurf;
			for( INDEX iNode=0; iNode<Level->Model->Nodes->Num; iNode++)
			{
				FBspNode &Node = Level->Model->Nodes(iNode);
				if( Node.iSurf == iSurf )
					Node.NodeFlags |= NF_NoMerge;
			}
		}
	}
	unguard;
}

//
// Go through the Bsp and assign zone numbers to all nodes.  Prior to this
// function call, only leaves have zone numbers.  The zone numbers for the entire
// Bsp can be determined from leaf zone numbers.
//
void FZoneFilter::AssignAllZones(INDEX iNode,int Outside)
{
	guard(FZoneFilter::AssignAllZones);
	
	INDEX	iOriginalNode	= iNode;
	INDEX	iOriginalSurf	= Level->Model->Nodes(iNode).iSurf;
	INT		IsCsg			= Level->Model->Nodes(iOriginalNode).IsCsg();
	FPoly	EdPoly;

	// Recursively assign zone numbers to children.
	if( Level->Model->Nodes(iOriginalNode).iFront != INDEX_NONE )
		AssignAllZones
		(
			Level->Model->Nodes(iOriginalNode).iFront, 
			Outside || IsCsg
		);
	
	if( Level->Model->Nodes(iOriginalNode).iBack != INDEX_NONE )
		AssignAllZones
		(
			Level->Model->Nodes(iOriginalNode).iBack,
			Outside && !IsCsg
		);

	// Make sure this node's polygon resides in a single zone.  In other words,
	// find all of the zones belonging to outside Bsp leaves and make sure their
	// zone number is the same, and assign that zone number to this node.  Note that
	// if semisolid polygons exist in the world, polygon fragments may actually fall into
	// inside nodes, and these fragments (and their zones) can be disregarded.
	while( iNode != INDEX_NONE )
	{
		if( Level->Model->Nodes(iNode).NodeFlags & NF_IsNew )
			break;

		INDEX iSurf = Level->Model->Nodes(iNode).iSurf;
		if( GUnrealEditor.bspNodeToFPoly( Level->Model, iNode, &EdPoly ) )
		{
			BYTE	ThisZone;
			int		MultipleZones = 0;

			int Forward = 
			(	 Level->Model->Vectors(Level->Model->Surfs(iOriginalSurf).vNormal)
			|	 Level->Model->Vectors(Level->Model->Surfs(iSurf        ).vNormal)) >= 0.0;

			if( Level->Model->Surfs(iSurf).PolyFlags & (PF_TwoSided|PF_Portal) )
			{
				// Two-sided's and portals aren't allowed to be split.
				ThisZone = 0;//!!why?? want multisplit portals!! This is screwed.
				FindSingleZone
				(
					iOriginalNode,
					Level->Model->Nodes(iOriginalNode).iFront,
					&EdPoly,
					ThisZone,
					MultipleZones,
					Outside || IsCsg,
					NODE_Front
				);
				// If 0, this is probably an interior semisolid poly.
				Level->Model->Nodes(iNode).iZone[Forward] = ThisZone;

				ThisZone = 0;
				FindSingleZone
				(
					iOriginalNode,
					Level->Model->Nodes(iOriginalNode).iBack,
					&EdPoly,ThisZone,
					MultipleZones,
					Outside && !IsCsg,
					NODE_Back
				);
				// If 0, this is probably an interior semisolid poly.
				Level->Model->Nodes(iNode).iZone[1-Forward] = ThisZone;
			}
			else
			{
				ThisZone = 0;
				MultipleZones = 0;
				if( Forward )
				{
					FindSingleZone
					(
						iOriginalNode,
						Level->Model->Nodes(iOriginalNode).iFront,
						&EdPoly,
						ThisZone,
						MultipleZones,
						Outside || IsCsg,
						NODE_Front
					);
					if( MultipleZones )
					{
						FilterMultiZonePoly
						(
							iNode,
							iOriginalNode,
							Level->Model->Nodes(iOriginalNode).iFront,
							&EdPoly,
							Outside || IsCsg,
							NODE_Front
						);
						Level->Model->Nodes(iNode).NodeFlags |= NF_TagForEmpty;
					}
					else
					{
						// If 0, this is probably an interior semisolid poly.
						Level->Model->Nodes(iNode).iZone[1] = ThisZone;
					}
				}
				else
				{
					FindSingleZone
					(
						iOriginalNode,
						Level->Model->Nodes(iOriginalNode).iBack,
						&EdPoly,
						ThisZone,
						MultipleZones,
						Outside && !IsCsg,
						NODE_Back
					);
					if( MultipleZones )
					{
						FilterMultiZonePoly
						(
							iNode,
							iOriginalNode,
							Level->Model->Nodes(iOriginalNode).iBack,
							&EdPoly,
							Outside && !IsCsg,
							NODE_Back
						);
						Level->Model->Nodes(iNode).NodeFlags |= NF_TagForEmpty;
					}
					else
					{
						// If 0, this is probably an interior semisolid poly.
						Level->Model->Nodes(iNode).iZone[1] = ThisZone;
					}
				}
			}
		}
		iNode = Level->Model->Nodes(iNode).iPlane;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Bsp zone structure building.
-----------------------------------------------------------------------------*/

//
// Build a 64-bit zone mask for each node, with a bit set for every
// zone that's referenced by the node and its children.  This is used
// during rendering to reject entire sections of the tree when it's known
// that none of the zones in that section are active.
//
QWORD FZoneFilter::BuildZoneMasks(INDEX iNode)
{
	guard(FZoneFilter::BuildZoneMasks);

	FBspNode	&Node		= Level->Model->Nodes(iNode);
	QWORD		ZoneMask	= 0;

	if (Node.iZone[1]!=0) ZoneMask |= ((QWORD)1) << Node.iZone[1];
	if (Node.iZone[0]!=0) ZoneMask |= ((QWORD)1) << Node.iZone[0];

	if (Node.iFront != INDEX_NONE)	ZoneMask |= BuildZoneMasks(Node.iFront);
	if (Node.iBack  != INDEX_NONE)	ZoneMask |= BuildZoneMasks(Node.iBack );
	if (Node.iPlane != INDEX_NONE)	ZoneMask |= BuildZoneMasks(Node.iPlane);

	Node.ZoneMask = ZoneMask;

	return ZoneMask;
	unguard;
}

//
// Build 64x64 zone connectivity matrix.  Entry(i,j) is set if node i is connected
// to node j.  Entry(i,i) is always set by definition.  This structure is built by
// analyzing all portals in the world and tagging the two zones they connect.
//
void FZoneFilter::BuildConnectivity()
{
	guard(FZoneFilter::BuildConnectivity);

	for( int i=0; i<64; i++ )
	{
		// Init to identity.
		Level->Model->Nodes->Zones[i].Connectivity = ((QWORD)1)<<i;
	}
	for( i=0; i<Level->Model->Nodes->Num; i++ )
	{
		// Process zones connected by portals.
		FBspNode &Node = Level->Model->Nodes(i);
		FBspSurf &Poly = Level->Model->Surfs(Node.iSurf);

		if( Poly.PolyFlags & PF_Portal )
		{
			Level->Model->Nodes->Zones[Node.iZone[1]].Connectivity |= ((QWORD)1) << Node.iZone[0];
			Level->Model->Nodes->Zones[Node.iZone[0]].Connectivity |= ((QWORD)1) << Node.iZone[1];
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Zone-to-zone visibility.
-----------------------------------------------------------------------------*/

//
// Init visibility. Real visibility computation isn't implemented here.
//
void FZoneFilter::BuildVisibility()
{
	guard(FZoneFilter::BuildVisibility);
	for( int i=0; i<64; i++ )
	{
		Level->Model->Nodes->Zones[i].Visibility = Level->Model->Nodes->Zones[i].Connectivity;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	FZoneFilter: Zone info.
-----------------------------------------------------------------------------*/

//
// Attach ZoneInfo actors to the zones that they belong in.
// ZoneInfo actors are a class of actor which level designers may
// place in UnrealEd in order to specify the properties of the zone they
// reside in, such as water effects, zone name, etc.
//
void FZoneFilter::BuildZoneInfo()
{
	guard(FZoneFilter::BuildZoneInfo);
	int Infos=0, Duplicates=0, Zoneless=0;

	for( int i=0; i<64; i++ )
	{
		// By default, the LevelInfo (actor 0) acts as the ZoneInfo
		// for all zones which don't have individual ZoneInfo's.
		Level->Model->Nodes->Zones[i].ZoneActor = NULL;
	}
	for( int iActor=0; iActor<Level->Num; iActor++ )
	{
		AActor *Actor = Level->Element(iActor);
		if( Actor && Actor->IsA("ZoneInfo") && !Actor->IsA("LevelInfo") )
		{
			Actor->ZoneNumber = Level->Model->PointZone(Actor->Location);
			if( Actor->ZoneNumber == 0 )
			{
				Zoneless++;
			}
			else if( Level->Model->Nodes->Zones[Actor->ZoneNumber].ZoneActor )
			{
				Duplicates++;
			}
			else
			{
				Infos++;
				Level->Model->Nodes->Zones[Actor->ZoneNumber].ZoneActor = (AZoneInfo*)Actor;
			}
		}
	}
	debugf(LOG_Info,"zoneBuild: %i ZoneInfo actors, %i duplicates, %i zoneless",Infos,Duplicates,Zoneless);
	unguard;
}

/*-----------------------------------------------------------------------------
	FZoneFilter: Main routine.
-----------------------------------------------------------------------------*/

//
// Build all visibility zones for the Bsp.  Assigns iZone's and zone reject bitmasks
// to each node, sets up zone-to-zone connectivity information for sound propagation, 
// and sets up all visibility portals for proper visibility zone occlusion checking.
//
void FGlobalEditor::zoneBuild( ULevel *Level )
{
	guard(FGlobalEditor::zoneBuild);
	FMemMark Mark(GMem);

	FZoneFilter Filter;
	Filter.Level = Level;

	Filter.Level->Lock(LOCK_ReadWrite);
	Filter.Level->Model->Nodes->NumZones = 0;

	int NumPortals=0;
	for( int i=0; i<Filter.Level->Model->Surfs->Num; i++ )
	{
		if( Filter.Level->Model->Surfs(i).PolyFlags & PF_Portal )
			NumPortals++;
	}
	if( Filter.Level->Model->Nodes->Num )
	{
		debug(LOG_Info,"zoneBuild begin");

		// Allocate working tables.
		FMemMark Mark(GMem);
		Filter.Planes		= new(GMem,MAX_DEPTH)FZoneFilterCuttingPlane;
		Filter.NodeZones[0] = new(GMem,Filter.Level->Model->Nodes->Num)INDEX;
		Filter.NodeZones[1] = new(GMem,Filter.Level->Model->Nodes->Num)INDEX;

		// Give each leaf in the world a unique zone number.
		for( int i=0; i<Filter.Level->Model->Nodes->Num; i++ )
		{
			Filter.NodeZones[0][i]			 = INDEX_NONE;
			Filter.NodeZones[1][i]			 = INDEX_NONE;
			Filter.Level->Model->Nodes(i).iZone[0]	 = 0;
			Filter.Level->Model->Nodes(i).iZone[1]	 = 0;
			Filter.Level->Model->Nodes(i).NodeFlags &= ~(NF_PortalFront | NF_PortalBack | NF_IsNew);
		}

		// Don't assign 0 to any zone.
		Filter.Level->Model->Nodes->NumZones = 1;
		
		Filter.AssignUniqueLeafZones
		(
			0,
			0,
			Filter.Level->Model->Nodes->NumZones,
			Level->Model->RootOutside,
			NODE_Root
		);
		debugf( LOG_Info, "zoneBuild assigned %i starting zones", Filter.Level->Model->Nodes->NumZones );

		// Go through the entire tree, merging zones which are spatially connected.
		Filter.NumPortals   = 0;
		Filter.PortalChunks = 0;
		Filter.NumPlanes    = 0;
		Filter.MergeAdjacentZones( 0, Level->Model->RootOutside );

		// Renumber the zones so we're left with a smaller, more manageable set.
		Filter.RemapZones();

		// Assign zone numbers to all nodes in the world (previous to this, only leaves
		// have zone numbers).
		Filter.AssignAllZones( 0, Level->Model->RootOutside );
		Filter.BuildConnectivity();
		Filter.BuildVisibility();
		Filter.BuildZoneInfo();

		// Clean up Bsp.  Required since node polys may have been filtered, leaving zero-
		// vertex node polys with possibly non-empty coplanars.
		int NodesMissed=0;
		for( i=0; i<Filter.Level->Model->Nodes->Num; i++ )
		{
			FBspNode &Node = Filter.Level->Model->Nodes(i);

			if ((Node.iZone == 0) && !(Node.NodeFlags&NF_TagForEmpty))
				NodesMissed++;
			
			Node.NodeFlags &= ~(NF_PortalFront | NF_PortalBack);
		}

		GUnrealEditor.bspCleanup	(Filter.Level->Model);		// Clean up newly-emptied nodes.
		GUnrealEditor.bspRefresh	(Filter.Level->Model,0);	// Delete unreferenced stuff if near overflow.
		GUnrealEditor.bspBuildBounds(Filter.Level->Model);		// Build bounding box structure.

		// Build 64-entry rejection mask for each node.  Must call this after
		// bspCleanup.
		Filter.BuildZoneMasks(0);

		// Summary stats.
		debugf
		(
			LOG_Info,
			"zoneBuild end, portals=%i, chunks=%i, missed=%i",
			Filter.NumPortals,
			Filter.PortalChunks,			
			NodesMissed
		);
		Mark.Pop();
	}
	else debug(LOG_Info,"zoneBuild built no zones");
	Filter.Level->Unlock(LOCK_ReadWrite);
	Mark.Pop();
	unguard;
}

/*-----------------------------------------------------------------------------
	Bsp node bounding volumes.
-----------------------------------------------------------------------------*/

#if DEBUG_HULLS
	UModel *DEBUG_Brush;
#endif

//
// Update a bounding volume by expanding it to enclose a list of polys.
//
void UpdateBoundWithPolys( FBoundingRect *Bound, FPoly **PolyList, int nPolys )
{
	guard(UpdateBoundWithPolys);
	for( int i=0; i<nPolys; i++ )
	{
		FPoly &Poly = *PolyList[i];
		for( int j=0; j<Poly.NumVertices; j++ )
		{
			FVector &V = Poly.Vertex[j];
			Bound->Min.UpdateMinWith(V);
			Bound->Max.UpdateMaxWith(V);
		}
#if DEBUG_HULLS
		DEBUG_Brush->Polys->Element(DEBUG_Brush->Polys->Num++)=*Poly;
#endif
	}
	unguard;
}

//
// Update a convolution hull with a list of polys.
//
void UpdateConvolutionWithPolys( UModel *Model, INDEX iNode, FPoly **PolyList, int nPolys )
{
	guard(UpdateConvolutionWithPolys);

	FVector Min(+65536.0,+65536.0,+65536.0);
	FVector Max(-65536.0,-65536.0,-65536.0);

	FBspNode &Node = Model->Nodes(iNode);
	Node.NodeFlags |= NF_SolidLeaf;
	Node.iBound     = Model->LeafHulls->Num;
	for( int i=0; i<nPolys; i++ )
	{
		if( PolyList[i]->iBrushPoly != INDEX_NONE )
		{
			for( int j=0; j<i; j++ )
				if( PolyList[j]->iBrushPoly == PolyList[i]->iBrushPoly )
					break;
			if( j >= i )
				Model->LeafHulls->AddItem(PolyList[i]->iBrushPoly);
		}
		for( int j=0; j<PolyList[i]->NumVertices; j++ )
		{
			Min.UpdateMinWith(PolyList[i]->Vertex[j]);
			Max.UpdateMaxWith(PolyList[i]->Vertex[j]);
		}
	}
	Model->LeafHulls->AddItem(INDEX_NONE);

	// Add bounds.
	Model->LeafHulls->AddItem(*(INT*)&Min.X);
	Model->LeafHulls->AddItem(*(INT*)&Min.Y);
	Model->LeafHulls->AddItem(*(INT*)&Min.Z);
	Model->LeafHulls->AddItem(*(INT*)&Max.X);
	Model->LeafHulls->AddItem(*(INT*)&Max.Y);
	Model->LeafHulls->AddItem(*(INT*)&Max.Z);

	unguard;
}

//
// Cut a partitioning poly by a list of polys, and add the resulting inside pieces to the
// front list and back list.
//
void SplitPartitioner
(
	UModel  *Model,
	FPoly	**PolyList,
	FPoly	**FrontList,
	FPoly	**BackList,
	int		n,
	int		nPolys,
	int		&nFront, 
	int		&nBack, 
	FPoly	InfiniteEdPoly
)
{
	FPoly FrontPoly,BackPoly;
	while( n < nPolys )
	{
		if( InfiniteEdPoly.NumVertices >= FPoly::VERTEX_THRESHOLD )
		{
			FPoly Half;
			InfiniteEdPoly.SplitInHalf(&Half);
			SplitPartitioner(Model,PolyList,FrontList,BackList,n,nPolys,nFront,nBack,Half);
		}
		FPoly *Poly = PolyList[n];
		switch( InfiniteEdPoly.SplitWithPlane(Poly->Base,Poly->Normal,&FrontPoly,&BackPoly,0) )
		{
			case SP_Coplanar:
				debugf("FilterBound: Got inficoplanar"); // May occasionally happen
				break;
			
			case SP_Front:
				debugf("FilterBound: Got infifront"); // Shouldn't happen if hull is correct
				return;
			
			case SP_Split:
				InfiniteEdPoly = BackPoly;
				break;
			
			case SP_Back:
				break;
		}
		if( nFront>=MAX_BOUND_POLYS || nBack>=MAX_BOUND_POLYS )
			appError("Convex volume is too complex");

		n++;
	}

	FPoly *New = new(GMem)FPoly;
	*New = InfiniteEdPoly;
	New->Reverse();
	New->iBrushPoly |= 0x40000000;
	FrontList[nFront++] = New;

	New = new(GMem)FPoly;
	*New = InfiniteEdPoly;
	BackList[nBack++] = New;
}

//
// Recursively filter a set of polys defining a convex hull down the Bsp,
// splitting it into two halves at each node and adding in the appropriate
// face polys at splits.
//
void FilterBound
(
	UModel			*Model,
	FBoundingRect	*ParentBound,
	INDEX			iNode,
	FPoly			**PolyList,
	INT				nPolys,
	INT				Outside
)
{
	FMemMark Mark(GMem);
	FBspNode		&Node		= Model->Nodes  (iNode);
	FBspSurf		&Surf		= Model->Surfs  (Node.iSurf);
	FVector			&Base		= Model->Points (Surf.pBase);
	FVector			&Normal		= Model->Vectors(Surf.vNormal);
	FBoundingRect	Bound;

	Bound.Min.X = Bound.Min.Y = Bound.Min.Z = +65536.0;
	Bound.Max.X = Bound.Max.Y = Bound.Max.Z = -65536.0;

	// Split bound into front half and back half.
	FPoly *FrontList[MAX_BOUND_POLYS]; int nFront=0;
	FPoly *BackList [MAX_BOUND_POLYS]; int nBack=0;

	FPoly *FrontPoly = new(GMem)FPoly;
	FPoly *BackPoly  = new(GMem)FPoly;

	for( int i=0; i<nPolys; i++ )
	{
		FPoly *Poly = PolyList[i];
		switch( Poly->SplitWithPlane( Base, Normal, FrontPoly, BackPoly, 0 ) )
		{
			case SP_Coplanar:
				debugf("FilterBound: Got coplanar");
				FrontList[nFront++] = Poly;
				BackList[nBack++] = Poly;
				break;
			
			case SP_Front:
				FrontList[nFront++] = Poly;
				break;
			
			case SP_Back:
				BackList[nBack++] = Poly;
				break;
			
			case SP_Split:
				if( FrontPoly->NumVertices >= FPoly::VERTEX_THRESHOLD )
				{
					FPoly *Half = new(GMem)FPoly;
					FrontPoly->SplitInHalf(Half);
					FrontList[nFront++] = Half;
				}
				FrontList[nFront++] = FrontPoly;

				if( BackPoly->NumVertices >= FPoly::VERTEX_THRESHOLD )
				{
					FPoly *Half = new(GMem)FPoly;
					BackPoly->SplitInHalf(Half);
					BackList[nBack++] = Half;
				}
				BackList [nBack++] = BackPoly;

				FrontPoly = new(GMem)FPoly;
				BackPoly  = new(GMem)FPoly;
				break;

			default:
				appError( "FZoneFilter::FilterToLeaf: Unknown split code" );
			}
		
		if( (nFront >= MAX_BOUND_POLYS)||(nBack >= MAX_BOUND_POLYS) )
			appError("Convex area is too complex");
	}
	if( nFront && nBack )
	{
		// Add partitioner plane to front and back.
		FPoly InfiniteEdPoly;
		BuildInfiniteFPoly(Model,iNode,InfiniteEdPoly);
		InfiniteEdPoly.iBrushPoly = iNode;

		SplitPartitioner(Model,PolyList,FrontList,BackList,0,nPolys,nFront,nBack,InfiniteEdPoly);
	}
	else
	{
		if( !nFront ) debugf("FilterBound: Empty fronthull");
		if( !nBack  ) debugf("FilterBound: Empty backhull");
	}

	// Recursively update all our childrens' bounding volumes.
	if( nFront > 0 )
	{
		if( Node.iFront != INDEX_NONE )
			FilterBound
			(
				Model,&Bound,Node.iFront,
				FrontList, nFront,
				Outside ||  Node.IsCsg()
			);
		else if( Outside || Node.IsCsg() )
			UpdateBoundWithPolys( &Bound, FrontList, nFront );
		else
			UpdateConvolutionWithPolys( Model, iNode, FrontList, nFront );
	}
	if( nBack > 0 )
	{
		if( Node.iBack != INDEX_NONE)
			FilterBound
			(
				Model,&Bound,Node.iBack, 
				BackList, nBack,
				Outside && !Node.IsCsg()
			);
		else if( Outside && !Node.IsCsg() )
			UpdateBoundWithPolys( &Bound, BackList, nBack );
		else
			UpdateConvolutionWithPolys( Model, iNode, BackList, nBack );
	}

	// Apply this bound to this node.
	if( Node.iBound == INDEX_NONE )
	{
		Node.NodeFlags |= NF_Bounded;
		Node.iBound     = Model->Bounds->Add();
		Model->Bounds(Node.iBound) = Bound;
	}

	// Update parent bound to enclose this bound.
	if( ParentBound )
	{
		ParentBound->Min.UpdateMinWith(Bound.Min);
		ParentBound->Max.UpdateMaxWith(Bound.Max);
	}
	Mark.Pop();
}

//
// Build bounding volumes for all Bsp nodes.  The bounding volume of the node
// completely encloses the "outside" space occupied by the nodes.  Note that 
// this is not the same as representing the bounding volume of all of the 
// polygons within the node.
//
// We start with a practically-infinite cube and filter it down the Bsp,
// whittling it away until all of its convex volume fragments land in leaves.
//
void FGlobalEditor::bspBuildBounds(UModel *Model)
{
	guard(FGlobalEditor::bspBuildBounds);

	if( Model->Nodes->Num==0 )
		return;

#if DEBUG_HULLS
	DEBUG_Brush=new("Brush",FIND_Existing)UModel;
	DEBUG_Brush->Polys->Num=0;
	DEBUG_Brush->Location=DEBUG_Brush->PrePivot=DEBUG_Brush->PostPivot=GMath.ZeroVector;
	DEBUG_Brush->Rotation=GMath.ZeroRotation;
#endif

	FPoly *PolyList[6];
	GGfx.RootHullBrush->Lock(LOCK_Read);
	for (int i=0; i<6; i++)
	{
		PolyList[i]             = &GGfx.RootHullBrush->Polys->Element(i);
		PolyList[i]->iBrushPoly = INDEX_NONE;
	}
	GGfx.RootHullBrush->Unlock(LOCK_Read);

	// Empty bounds and hulls.
	Model->Bounds->Empty();
	Model->LeafHulls->Empty();
	for( i=0; i<Model->Nodes->Num; i++ )
	{
		Model->Nodes(i).NodeFlags &= ~(NF_Bounded | NF_SolidLeaf);
		Model->Nodes(i).iBound     = INDEX_NONE;
	}

	FilterBound
	(
		Model,
		NULL,
		0,
		PolyList,
		6,
		Model->RootOutside
	);
	Model->Bounds->Shrink();

	debugf("bspBuildBounds: Generated %i bounds, %i hulls",Model->Bounds->Num,Model->LeafHulls->Num);
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
