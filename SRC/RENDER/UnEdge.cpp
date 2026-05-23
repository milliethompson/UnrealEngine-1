/*=============================================================================
	UnEdge.cpp: Unreal portal and active edge list rendering.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	Unreal active edge list and portal rendering code.  This constitutes
	the full occlusion and rasterization phase of optimized rendering.

Definitions:
	active edge list:
		The list of all edges which intersect with the current scanline.
		Some of the edges are visible and some may be occluded.  The active
		edge list is stored in x-start-sorted order.
	start bucket:
		A bucket of linked lists, with one bucket for each vertical
		line on the screen, which says which edges become active on that
		scanline.  This is built via, guess what, a bucket sort.
	edge:
		A line consisting of a start point and an end point, which
		connects one or two surfaces: an optional left surface, and an
		optional right surface.  The left and right surfaces are determined
		by the edge's corresponding surface backfacing.
	portal:
		A polygon on the boundary between two zones.  All adjacent
		zones are seamlessly separated by portals.  A portal polygon
		links two zones, its front zone, and its back zone.
		Portals are always transparent surfaces.
	span:
		A horizontal range of pixels existing on a scanline.
	surface, boundary:
		A polygon on a boundary between a zone and non-empty space.
		You can never see through boundary surfaces.
	surface, floating:
		A polygon that exists floating in space in the middle of a
		zone, such as a creature polygon or a moving brush polygon. 
		Floating surfaces may be transparent or may be solid.
	surface, solid:
		Any surface that bounds a solid volume; only the fronts of
		solid surfaces can be seen.
	surface, transparent:
		A polygon that can be seen through; transparent surfaces can be
		seen from both the front and back.
	zone:
		A solid, empty region in space, bounded by surfaces and portals.

Notes:
	* X-clip handling of edges will be a bitch.
	* Portal edges will often be shared by more than 2 surfaces, right?

Revision history:
	Created by Tim Sweeney.
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	FGlobalOccluder definition.
-----------------------------------------------------------------------------*/

//
// Active edge rendering class.
//
struct FGlobalOccluder : public FGlobalOccluderBase
{
	//
	// Information about a surface that is associated with an edge.
	//
	struct FEdgeSurf
	{
		// Member variables.
		FSpanBuffer	*SpanBuffer;			// Span buffer to stick stuff visible spans in, NULL if not yet allocated.
		FSpan		**SpanIndex;			// Index into SpanBuffer relative to screen start, valid only if SpanBuffer is non-NULL.
		INDEX		iSurf;					// Bsp surface index.
		int			HorizontalActiveness;	// Activeness count during horizontal pass.
		int			VerticalActiveness;		// Activeness count during vertical pass for x-left-clipped sides.
		int			IsBackfaced;			// Whether it's backfaced.
		int			IsPortal;				// Whether it's a portal.
		int			Reject;					// Whether the surface matters.

		// Functions.
		void Setup(const INDEX iSurf);
	};

	//
	// An edge link.  The edge is either active, meaning that it is
	// exists in the context of the current scanline being processed.
	// Or, it is inactive, meaning that it has either not yet been
	// started (its vertical location is below the current scanline) or
	// it is finished (its vertical location is at or below the current
	// scanline).
	//
	// Note: This could be implemented with virtual functions for:
	//   - Bsp edges
	//   - Creature edges
	//
	struct FEdgeLink
	{
		// Valid for all edges.
		INT			FixX;			// X location interpolant in 16.16 fixed point.
		INT			FixDX;			// X delta per unit change in Y.
		INT			LinesLeft;		// Y countdown of scanlines left, starts >= 1.

		FLOAT		RZ;				// 1.0/Z location interpolant in floating point format.
		FLOAT		DRZ;			// 1.0/Z delta per unit change in Y.

		FEdgeSurf	*LeftSurf;		// Surface on the left, or NULL if none.
		FEdgeSurf	*RightSurf;		// Surface on the right, or NULL if none.

		// For inactive edges, Next refers to the next edge in this scanline bucket.
		// For active edges, Next refers to the next edge in the active edge list.
		FEdgeLink*	Next;			// Next edge.

		// Update interpolants for the next scanline.
		// Returns 0 if this edge link is finished, nonzero if not done.
		inline int Increment()
		{
			RZ		+=	DRZ;
			FixX	+=	FixDX;
			return --LinesLeft;
		}
	};

	//
	// An edge.  A list of these will be stored for each leaf in the Bsp.
	//
	struct FBspEdge
	{
		INDEX			iSurf[2];		// 1 or 2 Bsp surfaces containing the edge.
		INDEX			pVertex[2];		// Vertices of the edge.
	};

	//
	// Information about the edges starting on a line.
	//
	struct FLineStarts
	{
		FEdgeLink	*FirstEdge;			// First edge in linked list, or NULL if none.
		FEdgeLink	*LastEdge;			// Last edge in lined list, valid and non-NULL iff FirstEdge!=NULL.
	};

	struct FEdgeSurfCache
	{
		static FEdgeSurf**  Surfs;		// Surface cache entries.
		static FEdgeSurf*** CleanupSurfs; // Surfaces to clean up.
		static FEdgeSurf*** LastSurf;	// Last surface to clean up.

		static void Init();
		static void Reset();
		static inline FEdgeSurf* Get(const INDEX iSurf);
	};

	// Constants.
	enum {MAX_Y	= 1024+512};			// Maximum number of scanlines.

	// Subsystem objects.
	static FGlobalPlatform*	GApp;		// Global platform code.
	static FMemPool*		GMem;		// One memory pool.
	static FMemPool*		GDynMem;	// Another memory pool.

	// Member variables.
	static INT				Y;			// Current scanline.
	static INT				EndY;		// Last scanline.

	static FLineStarts		LineStarts	[MAX_Y]; // List of edges starting on each scanline.
	static FEdgeLink*		ActiveEdges;// Edges that are currently active.
	static FEdgeSurfCache	SurfCache;	// Surface cache.

	// Valid only during OccludeBsp.
	static ICamera*			Camera;		// The camera we're rendering to.
	static ILevel*			Level;		// The level we're rendering.
	static IModel*			Model;		// The level's model.

	// Stats.
	STAT
	(
		static INT			ActiveEdgeCount; // Number of edges processed.
		static INT			PointCount;		 // Number of edges processed.
	);

	// FGlobalRendererBase interface.
	void Init(FGlobalPlatform *GApp,FMemPool *GMem,FMemPool *GDynMem);
	void Exit();
	FBspDrawList *OccludeBsp(ICamera &Camera,FSpanBuffer *Backdrop);

	// Edge bucket functions.
	static inline void AddEdgeToLine( FEdgeLink* Edge, int StartY, int EndY );
	static void AddEdges(FBspEdge* FirstEdge, FBspEdge* LastEdge, int CurrentY);
	static inline void CacheTransform(FTransform *T0, FVector *P0);
};

/*-----------------------------------------------------------------------------
	Statics.
-----------------------------------------------------------------------------*/

// FGlobalOccluder statics.
FGlobalPlatform*				FGlobalOccluder::GApp = NULL;
FMemPool*						FGlobalOccluder::GMem;
FMemPool*						FGlobalOccluder::GDynMem;
INT								FGlobalOccluder::Y;
INT								FGlobalOccluder::EndY;
FGlobalOccluder::FLineStarts	FGlobalOccluder::LineStarts[MAX_Y];
FGlobalOccluder::FEdgeLink*		FGlobalOccluder::ActiveEdges;
FGlobalOccluder::FEdgeSurfCache	FGlobalOccluder::SurfCache;

ICamera*						FGlobalOccluder::Camera;
ILevel*							FGlobalOccluder::Level;
IModel*							FGlobalOccluder::Model;

STAT(INT						FGlobalOccluder::ActiveEdgeCount);
STAT(INT						FGlobalOccluder::PointCount);

// FEdgeSurfCache statics.
FGlobalOccluder::FEdgeSurf**	FGlobalOccluder::FEdgeSurfCache::Surfs;
FGlobalOccluder::FEdgeSurf***	FGlobalOccluder::FEdgeSurfCache::CleanupSurfs;
FGlobalOccluder::FEdgeSurf***	FGlobalOccluder::FEdgeSurfCache::LastSurf;

/*-----------------------------------------------------------------------------
	FEdgeSurf implementation.
-----------------------------------------------------------------------------*/

//
// Setup this surface cache entry for the specified surface.
// If the surface is solid and backfaced, only sets IsBackfaced=0 and IsPortal=0.
//
void FGlobalOccluder::FEdgeSurf::Setup(const INDEX iThisSurf)
{
	static FBspSurf *Surf;
	static FVector  *Base,*Normal;

	guardSlow(FGlobalOccluder::FEdgeSurf::Setup);
	debugInput(iSurf!=INDEX_NONE);

	iSurf  = iThisSurf;
	Surf   = &FGlobalOccluder::Model->BspSurfs[iSurf];
	Base   = &FGlobalOccluder::Model->FPoints [Surf->pBase];
	Normal = &FGlobalOccluder::Model->FVectors[Surf->vNormal];

	IsBackfaced = ((FGlobalOccluder::Camera->Coords.Origin - *Base) | *Normal) < 1.0;

	if( !(Surf->PolyFlags & PF_NotSolid) )
	{
		IsPortal = 0;

		// Don't bother setting up solid backfaced surfaces since they are never used.
		if( IsBackfaced )
		{
			Reject = 1;
			return;
		}
	}
	else
	{
		IsPortal = (Surf->PolyFlags & PF_Portal) ? 1 : 0;
	}

	SpanBuffer				= NULL;
	SpanIndex				= NULL;
	Reject					= 0;
	HorizontalActiveness	= 0;
	VerticalActiveness		= 0;

	unguardSlow;
}

/*-----------------------------------------------------------------------------
	FEdgeSurfCache implementation.
-----------------------------------------------------------------------------*/

//
// Init the surface cache.
//
inline void FGlobalOccluder::FEdgeSurfCache::Init()
{
	guard(FGlobalOccluder::FEdgeSurfCache::Init);

	Surfs = (FEdgeSurf**)appMalloc(Model->MaxBspSurfs * sizeof(FEdgeSurf*),"SurfCache");
	memset(Surfs,0,Model->MaxBspSurfs * sizeof(FEdgeSurf*));

	LastSurf = CleanupSurfs = (FEdgeSurf***)appMalloc(Model->MaxBspSurfs * sizeof(FEdgeSurf**),"CleanupSurfs");

	unguard;
}

//
// Reset the surface cache. Called once per frame.
//
inline void FGlobalOccluder::FEdgeSurfCache::Reset()
{
	guard(FGlobalOccluder::FEdgeSurfCache::Reset);

	while( LastSurf > &CleanupSurfs[0] )
		**(--LastSurf) = NULL;

	unguard;
}

//
// Look up the FEdgeSurf corresponding to a surface index.
// Either looks it up from the cache or creates a new one.
// Iff surface index is INDEX_NONE, returns NULL.
//
inline FGlobalOccluder::FEdgeSurf* FGlobalOccluder::FEdgeSurfCache::Get(const INDEX iSurf)
{
	static FEdgeSurf **CachedResult;
	guardSlow(FGlobalOccluder::FEdgeSurfCache::Get);

	if( iSurf != INDEX_NONE )
	{
		// Look up surface from the cache.
		CachedResult = &Surfs[iSurf];
		if( *CachedResult == NULL )
		{
			// Cache entry doesn't exist, so create it.
			*LastSurf++		= CachedResult;
			*CachedResult	= (FEdgeSurf *)GMem->Get(sizeof(FEdgeSurf));
			(*CachedResult)->Setup(iSurf);
		}
		if( !(*CachedResult)->Reject )
			return *CachedResult;
	}
	return NULL;
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	FGlobalOccluder edge bucket implementation.
-----------------------------------------------------------------------------*/

//
// Add a new edge to the active edge start line list.
//
inline void FGlobalOccluder::AddEdgeToLine( FEdgeLink* Edge, int StartY, int EndY )
{
	FLineStarts* Start	= &LineStarts[StartY];
	Edge->LinesLeft		= EndY - StartY;

	if( Start->FirstEdge == NULL )
		Start->LastEdge = Edge;

	Edge->Next			= Start->FirstEdge;
	Start->FirstEdge	= Edge;
}

//
// Setup a cache transform.
//
inline void FGlobalOccluder::CacheTransform(FTransform *T0, FVector *P0)
{
	// This transform is not cached.
	T0->RZ      = Camera->ProjZ / P0->Z;
	T0->ScreenX = P0->X * T0->RZ + Camera->FSXRM5;
	T0->ScreenY = P0->Y * T0->RZ + Camera->FSYRM5;

	ftoi(T0->NewIntY,T0->ScreenY - 0.5);
	if( T0->NewIntY < 0 )
		T0->NewIntY = 0;
}

//
// Add a list of edges to the start/end buckets.
// Outcode rejects the edges.
// Top- and left-clips the edges.
// Rejects edges that end before CurrentY.
//
BYTE GLSurfCount[65536],GRSurfCount[65536];//!!
void FGlobalOccluder::AddEdges(FBspEdge* Edge, FBspEdge* LastEdge,int CurrentY)
{
	guardSlow(FGlobalOccluder::AddEdges);

	static FTransform   *T0,*T1;
	static FVector		P0,P1;
	static FVector		*V0,*V1;
	static FEdgeLink*	NewEdge;
	static FEdgeSurf	*LeftSurf,*RightSurf;
	static FLOAT		Time,Time0,Time1,TimeLeft,TimeRight;
	static INT			StartY,EndY,Clip0,Clip1;
	static BYTE			AllFlags;
	//static BYTE			LastOutcodes[65536];//!!

	memset(GLSurfCount,0,sizeof(GLSurfCount));//!!
	memset(GRSurfCount,0,sizeof(GRSurfCount));

	for(Edge; Edge < LastEdge; Edge++ )
	{
		// Process this new edge.

		// Previous-frame outcode cache.
		/*
		BYTE &Out1		= LastOutcodes[Edge->pVertex[0]];
		BYTE &Out2		= LastOutcodes[Edge->pVertex[1]];
		BYTE AllOuts	= Out1 & Out2;

		// See if the line was outcode rejected last frame.
		if( AllOuts != 0 )
		{
			// Try to outcode reject the points prior to transformation.
			V0 = &Model->FPoints[Edge->pVertex[0]];
			V1 = &Model->FPoints[Edge->pVertex[1]];

			if(	((AllOuts & FVF_OutYMax) && (Camera->ViewPlanes[0].PlaneDot(*V0)<0.0) && (Camera->ViewPlanes[0].PlaneDot(*V1)<0.0))
			||	((AllOuts & FVF_OutYMin) && (Camera->ViewPlanes[1].PlaneDot(*V0)<0.0) && (Camera->ViewPlanes[1].PlaneDot(*V1)<0.0))
			||	((AllOuts & FVF_OutXMax) && (Camera->ViewPlanes[2].PlaneDot(*V0)<0.0) && (Camera->ViewPlanes[2].PlaneDot(*V1)<0.0))
			)
				// We outcode rejected this in worldspace.
				continue;
		}
		*/

		// Transform the edge's two points using the transformation cache.
		T0 = &GRender.GetPoint(Camera,Edge->pVertex[0]); 
		//Out1 = T0->Flags;

		T1 = &GRender.GetPoint(Camera,Edge->pVertex[1]); 
		//Out2 = T1->Flags;

		STAT(PointCount+=2;)

		// Check for left clip crossings.
		if( (T0->Flags ^ T1->Flags) & FVF_OutXMin )
		{
			TimeLeft		= T0->ClipXM/(T0->ClipXM-T1->ClipXM);
			FTransform T    = *T0 + (*T1-*T0) * TimeLeft;
			T.ComputeOutcode( *Camera );

			if( T.Flags & FVF_OutYMin )
			{
				if( Edge->iSurf[0]!=INDEX_NONE ) 
					GLSurfCount[Edge->iSurf[0]]++;
				if( Edge->iSurf[1]!=INDEX_NONE ) 
					GLSurfCount[Edge->iSurf[1]]++;
			}
		}

		// Check for right clip crossings.
		if( (T0->Flags ^ T1->Flags) & FVF_OutXMax )
		{
			TimeRight		= T0->ClipXP/(T0->ClipXP-T1->ClipXP);
			FTransform T	= *T0 + (*T1-*T0) * TimeRight;
			T.ComputeOutcode( *Camera );

			if( T.Flags & FVF_OutYMin )
			{
				if( Edge->iSurf[0]!=INDEX_NONE ) 
					GRSurfCount[Edge->iSurf[0]]++;
				if( Edge->iSurf[1]!=INDEX_NONE ) 
					GRSurfCount[Edge->iSurf[1]]++;
			}
		}

		// See whether the edge is visible.
		if( (T0->Flags & T1->Flags) == 0 )
		{
			// Get left and right surfaces.
			// We may swap them later if we find the edge is facing the other way.
			RightSurf = SurfCache.Get(Edge->iSurf[0]);
			LeftSurf  = SurfCache.Get(Edge->iSurf[1]);

			// Reject sides for which all surfaces are backfaced.
			if( LeftSurf==NULL && RightSurf==NULL ) 
				continue;

			// The edge is visible.
			STAT(ActiveEdgeCount++;)

			// Perform any clipping (y-min, x-min, x-max).
			AllFlags = T0->Flags | T1->Flags;
			if( AllFlags & (FVF_OutYMin|FVF_OutXMin|FVF_OutXMax) )
			{
				// Compute clip times.
				Clip0 = 0;
				Clip1 = 0;

				// Y-min clip.
				if( AllFlags & FVF_OutYMin )
				{
					Time = T0->ClipYM/(T0->ClipYM-T1->ClipYM);
					if( T0->Flags & FVF_OutYMin )	{Clip0 = 1; Time0 = Time;}
					else							{Clip1 = 1; Time1 = Time;}
				}

				// X-max clip.
				if( AllFlags & FVF_OutXMax )
				{
					if( T0->Flags & FVF_OutXMax )	{if( !Clip0 || TimeRight > Time0 ) {Time0 = TimeRight; Clip0 = 1;}}
					else							{if( !Clip1 || TimeRight < Time1 ) {Time1 = TimeRight; Clip1 = 1;}}
				}

				// X-min clip.
				if( AllFlags & FVF_OutXMin )
				{
					if( T0->Flags & FVF_OutXMin )	{if( !Clip0 || TimeLeft > Time0 ) {Time0 = TimeLeft; Clip0 = 1;}}
					else							{if( !Clip1 || TimeLeft < Time1 ) {Time1 = TimeLeft; Clip1 = 1;}}
				}

				// Time-clip the points.
				if( Clip0 )
				{
					// First point is clipped.
					P0 = *T0 + (*T1-*T0) * Time0;
					P1 = *T1;
					if( Clip1 )
					{
						if( Time0 >= Time1 ) 
							// Clipped to nil - very rare, but it does happen.
							continue;

						// Second point is clipped.
						P1 = *T0 + (*T1-*T0) * Time1;
					}
				}
				else if( Clip1 )
				{
					// Second point is clipped.
					P0 = *T0;
					P1 = *T0 + (*T1-*T0) * Time1;
				}
				else
				{
					// Very narrowly missed the clip (ok).
					P0 = *T0;
					P1 = *T1;
				}

				// Project the points with optional caching.
				if( T0->iTransform != MAXWORD )
				{
					CacheTransform(T0,&P0);
					if ( T0->Flags == 0 ) 
						// Mark this as cached if it's unclipped.
						T0->iTransform = MAXWORD;
				}
				if( T1->iTransform != MAXWORD )
				{
					CacheTransform(T1,&P1);
					if ( T1->Flags == 0 ) 
						// Mark this as cached if it's unclipped.
						T1->iTransform = MAXWORD;
				}
			}
			else
			{
				// Unclipped edge. Project the points with caching.
				if( T0->iTransform != MAXWORD )
					CacheTransform(T0,T0);

				if( T1->iTransform != MAXWORD )
					CacheTransform(T1,T1);
			}

			// Arrange points in Y order. That the left/right surfaces
			// are determined solely by whether the edge is facing up or down.
			if( T1->NewIntY < T0->NewIntY )
			{
				Swap(T0,T1);
				Swap(LeftSurf,RightSurf);
			}

			// Prevent underflow.
			if( T0->NewIntY < 0 )
				T0->NewIntY = 0;

			// Find Y extent of edge.
			StartY = T0->NewIntY;
			EndY   = T1->NewIntY;

			// Make sure this edge is non-nil.
			if( (EndY > StartY) && (StartY < Camera->SYR) )
			{
				// Allocate a new edge link.
				NewEdge				= (FEdgeLink*)GMem->GetFast(sizeof(FEdgeLink));

				// Add this edge link to the line-start table.
				AddEdgeToLine(NewEdge,StartY,EndY);

				// Set X values.
				FLOAT YAdjust	= (FLOAT)(StartY+1) - T0->ScreenY;
				FLOAT RDeltaY	= 1.0 / (T1->ScreenY - T0->ScreenY);
				FLOAT FloatDX 	= (T1->ScreenX - T0->ScreenX) * RDeltaY;

				ftoi(NewEdge->FixDX,65536.0 * FloatDX);
				ftoi(NewEdge->FixX, 65536.0 * (T0->ScreenX + FloatDX * YAdjust));

				// Set Z values.
				NewEdge->DRZ		= (T1->RZ - T0->RZ) * RDeltaY;
				NewEdge->RZ			= T0->RZ + NewEdge->DRZ * YAdjust;

				// Link it in.
				NewEdge->LeftSurf	= LeftSurf;
				NewEdge->RightSurf	= RightSurf;
			}
			//For debugging: GRender.DrawLine(Camera,BrushWireColor,0,T0->ScreenX,T0->ScreenY,T1->ScreenX,T1->ScreenY);
		}
		else if (
			((T0->Flags & T1->Flags) == FVF_OutYMin )
		&&	(((T0->Flags ^ T1->Flags)&FVF_OutXMin) != 0))
		{
			// Must update screen top-left clip effectiveness for this surface.
		}
	}
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	Main occlusion routine.
-----------------------------------------------------------------------------*/

#include "UnRaster.h"
extern FGlobalRaster GRaster;
FBspDrawList *FGlobalOccluder::OccludeBsp
(
	ICamera&			ThisCamera,
	FSpanBuffer			*Backdrop 
)
{
	STAT(clock(GStat.OcclusionTime));
	guard(FGlobalOccluder::PortalOcclusion);

	Camera			= &ThisCamera;
	Level			= &Camera->Level;
	Model			= &Level->ModelInfo;

	FBspDrawList	*DrawList		= NULL;
	FBspDrawList	*TempDrawList	= (FBspDrawList *)GDynMem->Get(sizeof(FBspDrawList));

	//
	// Init active start/end buckets.
	//
	for( int i=0; i<Camera->SYR; i++ )
	{
		LineStarts[i].FirstEdge = NULL;
	}

	//hack:
	enum{NUMHACKS=100000};
	static FBspEdge GEdgeHack[NUMHACKS];
	static int GEdgeHackCount=0;
	static int Hack=0;
	if( !Hack )
	{
		SurfCache.Init();

		Hack=1;

		//
		// Count very near duplicate points
		//
		int DupePoints=0;
		for( int i=0; i<Model->NumPoints; i++ )
		{
			FVector &P1 = Model->FPoints[i];
			for( int j=0; j<i; j++ )
			{
				FVector &P2 = Model->FPoints[j];
				if( (P2-P1).SizeSquared() < 0.04 )
				{
					for( int k=0; k<Model->NumVertPool; k++ )
					{
						FVertPool &Vert = Model->VertPool[k];
						if( Vert.pVertex == i )
							Vert.pVertex = j;
					}
					for( k=0; k<Model->NumBspSurfs; k++ )
					{
						FBspSurf &Surf = Model->BspSurfs[k];
						if( Surf.pBase == i )
							Surf.pBase = j;
					}
					DupePoints++;
					break;
				}
			}
		}
		debugf("Points: %i duplicates, %i total",DupePoints,Model->NumPoints);

		//
		// See about merging surfs.
		// This needs more detailed criteria to merge only if that helps minimize
		// shadow map area.
		//
		int DupeSurfs=0;
		for( i=0; i<Model->NumBspSurfs; i++ )
		{
			FBspSurf &S1 = Model->BspSurfs[i];
			for( int j=0; j<i; j++ )
			{
				FBspSurf &S2 = Model->BspSurfs[j];
				if (Abs(FPointPlaneDist(
					Model->FPoints [S1.pBase],
					Model->FPoints [S2.pBase],
					Model->FVectors[S2.vNormal])) < 0.01)
				{
					if(	(S1.Texture   == S2.Texture)
					&&	(S1.PolyFlags == S2.PolyFlags)
					&&	FPointsAreNear(Model->FVectors[S1.vNormal  ],Model->FVectors[S2.vNormal  ],0.001)
					//&&	FPointsAreNear(Model->FVectors[S1.vTextureU],Model->FVectors[S2.vTextureU],16.0)
					//&&	FPointsAreNear(Model->FVectors[S1.vTextureV],Model->FVectors[S2.vTextureV],16.0)
					)
					{
						DupeSurfs++;

						// Remap node references to surface j to surface i.
						for( int k=0; k<Model->NumBspNodes; k++ )
						{
							FBspNode& Node = Model->BspNodes[k];
							if( Node.iSurf == i )
								Node.iSurf = j;
						}
						break;
					}
				}
			}
		}
		debugf("Surfs: %i duplicates of %i",DupeSurfs,Model->NumBspSurfs);

		//
		// Build list of all edge/polygon associations.
		//

		// Init shared side list.
		for( i=0; i<NUMHACKS; i++ )
		{
			GEdgeHack[i].iSurf  [0]=GEdgeHack[i].iSurf  [1]=INDEX_NONE;
			GEdgeHack[i].pVertex[0]=GEdgeHack[i].pVertex[1]=INDEX_NONE;
		}
		// Fill in sparse shared side list.
		int Unsided = Model->NumSharedSides;
		for( i=0; i<Model->NumBspNodes; i++ )
		{
			FBspNode&  Node     =  Model->BspNodes[i];
			FVertPool* VertPool = &Model->VertPool[Node.iVertPool];

			for( int j=0; j<Node.NumVertices; j++ )
			{
				int		  iSide = VertPool[j].iSide;
				if( iSide == INDEX_NONE )
					iSide = Unsided++;

				FBspEdge& EdgeHack = GEdgeHack[iSide];
				if( EdgeHack.iSurf[0]==INDEX_NONE )
				{
					// Set iSurf[0].
					EdgeHack.iSurf  [0] = Node.iSurf;
					EdgeHack.pVertex[0] = VertPool[j                                      ].pVertex;
					EdgeHack.pVertex[1] = VertPool[(j+Node.NumVertices-1)%Node.NumVertices].pVertex;
				}
				else
				{
					// Set iSurf[1].
					EdgeHack.iSurf  [1] = Node.iSurf;
				}
			}
		}
		// Condense into non-sparse list.
		GEdgeHackCount=0;
		int Identical=0;
		for( i=0; i<NUMHACKS; i++ )
		{
			if (GEdgeHack[i].iSurf[0] != INDEX_NONE)
			{
				if( (GEdgeHack[i].iSurf[0]   != GEdgeHack[i].iSurf  [1])
				&&	(GEdgeHack[i].pVertex[0] != GEdgeHack[i].pVertex[1]))
				{
					// Condense.
					GEdgeHack[GEdgeHackCount++] = GEdgeHack[i];
				}
				else
				{
					Identical++;
				}
			}
		}
		debugf("Hacked %i edges (%i removed)",GEdgeHackCount,Identical);
	}
	//endhack:

	// Init stats.
	STAT
	(
		ActiveEdgeCount=0;
		PointCount=0;
	)

	// Reset the surface cache.
	SurfCache.Reset();

	// Add all edges to the start bucket.
	AddEdges(&GEdgeHack[0],&GEdgeHack[GEdgeHackCount],0);

	// Init the active edge list.
	ActiveEdges = NULL;
	FLineStarts *LineStart = &LineStarts[0];

	//Temp!!
	DWORD TLColor=0,TRColor=0;
	int Found=0;
	for( int k=0; k<Model->NumBspSurfs; k++ )
	{
		if( GLSurfCount[k]&1 )
		{
			FBspSurf *Surf   = &FGlobalOccluder::Model->BspSurfs[k];
			FVector  *Base   = &FGlobalOccluder::Model->FPoints [Surf->pBase];
			FVector  *Normal = &FGlobalOccluder::Model->FVectors[Surf->vNormal];
			if ((Camera->ViewSides[0] | *Normal) <= 0.0)
			{
				//the above should be ^IsBackfaced
				TLColor = k * 12345678;
				Found++;
			}
		}
		if( GRSurfCount[k]&1 )
		{
			FBspSurf *Surf   = &FGlobalOccluder::Model->BspSurfs[k];
			FVector  *Base   = &FGlobalOccluder::Model->FPoints [Surf->pBase];
			FVector  *Normal = &FGlobalOccluder::Model->FVectors[Surf->vNormal];
			if ((Camera->ViewSides[2] | *Normal) <= 0.0)
			{
				TRColor = k * 12345678;
				Found++;
			}
		}
	}
	debugf("Found %i",Found);

	// Process all scanlines.
	for( int Y=0; Y<Camera->SYR; Y++ )
	{
		// Add all edges that start on this scanline to the active edge list.
		if( LineStart->FirstEdge != NULL)
		{
			LineStart->LastEdge->Next = ActiveEdges;
			ActiveEdges               = LineStart->FirstEdge;
		}
		LineStart++;

		// Sort the active edges by x-start.
		{
			FEdgeLink *Edges[1024],**TopEdge=&Edges[0];
			for( FEdgeLink *Edge1 = ActiveEdges; Edge1!=NULL; Edge1=Edge1->Next )
				*TopEdge++ = Edge1;

			for( FEdgeLink** EdgeA = Edges; EdgeA<TopEdge; EdgeA++ )
			{
				for( FEdgeLink** EdgeB = Edges; EdgeB<EdgeA; EdgeB++ )
				{
					if( (*EdgeA)->FixX < (*EdgeB)->FixX )
					{
						Swap( *EdgeA, *EdgeB );
					}
				}
			}

			FEdgeLink **EdgeLink = &ActiveEdges;
			for( EdgeA = Edges; EdgeA<TopEdge; EdgeA++ )
			{
				*EdgeLink = *EdgeA;
				EdgeLink  = &(*EdgeA)->Next;
			}
			*EdgeLink = NULL;
		}

		//Temp!!
		int StartX=0,NextX;
		DWORD Color=TLColor;
		FEdgeSurf* Surf;
		DWORD* Dest = &((DWORD*)Camera->Screen)[Y*Camera->Stride + StartX];

		// Render and increment all active edges.
		FEdgeLink** CurrentEdgePtr = &ActiveEdges;
		while( (*CurrentEdgePtr) != NULL )
		{
			//Temp!!
			NextX	= Unfix((*CurrentEdgePtr)->FixX);

			Surf = (*CurrentEdgePtr)->LeftSurf;
			while( StartX < NextX )
			{
				*Dest++ = Color;
				StartX++;
			}
			Surf = (*CurrentEdgePtr)->RightSurf;
			Color = Surf ? (DWORD)(Surf->iSurf*12345678) : 0;

			// Update the current edge.
			if( (*CurrentEdgePtr)->Increment() )
				// Edge remains active; prepare to go to next.
				CurrentEdgePtr = &(*CurrentEdgePtr)->Next;
			else
				// Retire the current edge.
				*CurrentEdgePtr = (*CurrentEdgePtr)->Next;
		}
		//Temp!!
		while( StartX < Camera->SXR )
		{
			*Dest++ = TRColor;
			StartX++;
		}
	}

	/*
	FSpanBuffer ScreenSpanBuffer;
	ScreenSpanBuffer.AllocIndexForScreen( Camera.SXR,Camera.SYR, GDynMem );

	for( int iNode=0; iNode<Model.NumBspNodes; iNode++ )
	{
		FBspNode &Node = Model.BspNodes[iNode];
		FBspSurf &Surf = Model.BspSurfs[Node.iSurf];

		if( Node.NumVertices > 0 )
		{
			FTransform Pts[FBspNode::MAX_FINAL_VERTICES];
			int NumPts = GRender.ClipBspSurf( &Model,&Camera,iNode,Pts );
			if( NumPts )
			{
				// Rasterize this.
				FRasterSetup WorkingRaster;
				WorkingRaster.Setup(Camera,Pts,NumPts,GMem);
				WorkingRaster.Generate(GRaster.Raster);

				// Allocate a span.
				TempDrawList->Span.AllocIndex(GRaster.Raster->StartY,GRaster.Raster->EndY,GDynMem);
				TempDrawList->Span.CopyFromRaster(ScreenSpanBuffer,*GRaster.Raster);

				// Compute Z range.
				FLOAT MaxZ,MinZ;
				MaxZ = MinZ = Pts[0].Z;
				for( int i=1; i<NumPts; i++ )
				{
					if(			Pts[i].Z > MaxZ ) MaxZ = Pts[i].Z;
					else if(	Pts[i].Z < MinZ ) MinZ = Pts[i].Z;
				}

				// Set draw list info.
				TempDrawList->iNode		= iNode;
				TempDrawList->iZone		= Node.iZone[0];
				TempDrawList->iSurf		= Node.iSurf;
				TempDrawList->PolyFlags	= Surf.PolyFlags;
				TempDrawList->MaxZ		= MaxZ;
				TempDrawList->MinZ		= MinZ;
				TempDrawList->Texture	= Surf.Texture;
				TempDrawList->Next      = DrawList;

				// Next draw list.
				DrawList                = TempDrawList;
				TempDrawList            = (FBspDrawList*)GDynMem->Get(sizeof(FBspDrawList));
			}
		}
	}
	*/
	// Display stats.
	//debugf("Visible %i, Points %i",ActiveEdgeCount,PointCount);

	STAT(unclock(GStat.OcclusionTime));
	//return DrawList;
	return NULL;
	unguard;
}

/*-----------------------------------------------------------------------------
	Subsystem init/exit.
-----------------------------------------------------------------------------*/

void FGlobalOccluder::Init
(
	FGlobalPlatform*	ThisApp,
	FMemPool*			ThisMem,
	FMemPool*			ThisDynMem
)
{
	guard(FGlobalOccluder::Init);

	// Set subsystem objects.
	GApp		= ThisApp;
	GMem		= ThisMem;
	GDynMem		= ThisDynMem;

	// Success.
	logf(LOG_Init,"Edge occluder initialized");

	unguard;	
}

void FGlobalOccluder::Exit()
{
	guard(FGlobalOccluder::Exit);

	// Success.
	logf(LOG_Exit,"Edge occluder shut down");

	unguard;
}

/*-----------------------------------------------------------------------------
	Instantiation.
-----------------------------------------------------------------------------*/

FGlobalOccluder     GEdgeOccluderInstance;
FGlobalOccluderBase *GEdgeOccluder = &GEdgeOccluderInstance;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
