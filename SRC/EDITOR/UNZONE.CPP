/*=============================================================================
	UnZone.cpp: Zone visibility computation

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
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

//
// Options.
//
#define PARANOID	0	/* Turn this on for careful debugging checks */
#define DEBUG_HULLS 0	/* Debugging hull code by generating hull brush */
enum {MAX_BOUND_POLYS=96};

//
// Constants.
//
#define WORLD_MAX 65536.0		/* Maximum size of the world */
#define MAX_DEPTH 4096			/* Maximum depth of the Bsp from root to the lowest leaf */

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
	IModel					ModelInfo;		// Model information for the Bsp.
	ULevel					*Level;			// Level this Bsp resides in.
	FZoneFilterCuttingPlane *Planes;		// List of cutting planes in MergeAdjacentZones.
	INDEX					*NodeZones[2];	// Full zone numbers (0-65535), later reduced to byte iZone's.
	int						NumPlanes;		// Number of cutting planes in MergeAdjacentZones.
	int						NumPortals;		// Number of zone portals found on polys with PF_ZONE_PORTAL tag.
	int						PortalChunks;	// Number of portal polys filtered down to leaves.

	// Functions.
	void FilterToLeaf(FPoly *EdPoly,INDEX iOtherNode,INDEX iPrevNode,INDEX iNode,EZoneFilterState State, int Outside, int BackOutside, ENodePlace PrevPlace, int PortalBlockCount);
	void AssignUniqueLeafZones(INDEX iPrev,INDEX iNode, int &iTopZone,int Outside,ENodePlace PrevPlace);
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
	void BuildVisibility(int SampleDensity, int MinSampleSize);
	void BuildZoneDescriptors();
	QWORD BuildZoneMasks(INDEX iNode);
};

/*-----------------------------------------------------------------------------
	Non-class functions.
-----------------------------------------------------------------------------*/

//
// Build an FPoly representing an "infinite" plane (which exceeds the maximum
// dimensions of the world in all directions) for a particular Bsp node.
//
void BuildInfiniteFPoly(IModel &ModelInfo, INDEX iNode, FPoly &EdPoly)
{
	guard(BuildInfiniteFPoly);

	FBspNode *Node   = &ModelInfo.BspNodes [iNode        ];
	FBspSurf *Poly   = &ModelInfo.BspSurfs [Node->iSurf  ];
	FVector  *Base   = &ModelInfo.FPoints  [Poly->pBase  ];
	FVector  *Normal = &ModelInfo.FVectors [Poly->vNormal];
	FVector	 Axis1,Axis2;

	// Find two non-problematic axis vectors.
	Normal->FindBestAxisVectors(Axis1,Axis2);

	// Set up the FPoly.
	EdPoly.Init();
	EdPoly.NumVertices = 4;
	EdPoly.Normal      = *Normal;
	EdPoly.Base        = *Base;
	EdPoly.Vertex[0]   = *Base + Axis1*WORLD_MAX + Axis2*WORLD_MAX;
	EdPoly.Vertex[1]   = *Base - Axis1*WORLD_MAX + Axis2*WORLD_MAX;
	EdPoly.Vertex[2]   = *Base - Axis1*WORLD_MAX - Axis2*WORLD_MAX;
	EdPoly.Vertex[3]   = *Base + Axis1*WORLD_MAX - Axis2*WORLD_MAX;

#if PARANOID // Validate the poly
	if (EdPoly.SplitWithPlane (*Base,*Normal,NULL,NULL,0)!=SP_Coplanar)
		appError ("BuildInfinitePoly failed");
#endif
	unguard;
}

/*-----------------------------------------------------------------------------
	FZoneFilter: Main zone merging functions and callbacks.
-----------------------------------------------------------------------------*/

//
// Filter a poly through the Bsp, splitting it as we go, until we get to a leaf,
// then process the resulting poly chunks based on the filter State.
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
	INT					PortalBlockCount
)
{
	guard(FZoneFilter::FilterToLeaf);

	FilterLoop:
	if( EdPoly->NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD )
	{
		FPoly TempEdPoly;
		EdPoly->SplitInHalf(&TempEdPoly);
		FilterToLeaf(&TempEdPoly,iOtherNode,iPrevNode,iNode,State,Outside,BackOutside,PrevPlace,PortalBlockCount);
	}
	if( iNode != INDEX_NONE )
	{
		FBspNode *Node   = &ModelInfo.BspNodes [iNode        ];
		FBspSurf *Poly   = &ModelInfo.BspSurfs [Node->iSurf  ];
		FVector  *Base   = &ModelInfo.FPoints  [Poly->pBase  ];
		FVector  *Normal = &ModelInfo.FVectors [Poly->vNormal];
		FPoly	 FrontPoly,BackPoly;

		switch( EdPoly->SplitWithPlane(*Base,*Normal,&FrontPoly,&BackPoly,0) )
		{
			case SP_Coplanar:
				debugf(LOG_Info,"FZoneFilter::FilterToLeaf: Got coplanar");

			case SP_Front:
				Outside   = Outside || Node->IsCsg();
				iPrevNode = iNode;
				iNode     = Node->iFront;
				PrevPlace = NODE_Front;
				goto FilterLoop;

			case SP_Back:
				Outside   = Outside && !Node->IsCsg();
				iPrevNode = iNode;
				iNode     = Node->iBack;
				PrevPlace = NODE_Back;
				goto FilterLoop;

			case SP_Split:
				FilterToLeaf
				(
					&FrontPoly,iOtherNode,iNode,Node->iFront,State,
					Outside ||  Node->IsCsg(),BackOutside,NODE_Front,
					PortalBlockCount
				);
				FilterToLeaf
				(
					&BackPoly, iOtherNode,iNode,Node->iBack, State,
					Outside && !Node->IsCsg(),BackOutside,NODE_Back,
					PortalBlockCount
				);
				break;
			
			default:
				appError("FZoneFilter::FilterToLeaf: Unknown split code");
		}
	}
	else if( (State==FILTERING_DOWN_FRONT) && Outside )
	{
		// Now iOtherNode = Lowest common parent node.
		//     iPrevNode  = Lowest valid front node.
		//     iNode      = INDEX_NONE.

		if (iOtherNode==INDEX_NONE) appError("Error3");
		if (iPrevNode ==INDEX_NONE) appError("Error4");

		FBspNode *Parent = &ModelInfo.BspNodes[iOtherNode];
		DWORD    Flags   = ModelInfo.BspNodes[iPrevNode].NodeFlags;

		// On the next iteration:
		//     iOtherNode = The front leaf's zone (=PrevNode->Zone)
		//     iPrevNode  = Lowest common parent (=iOtherNode)
		//     iNode      = (iOtherNode -> iBack)
		FilterToLeaf
		(
			EdPoly,
			NodeZones[PrevPlace][iPrevNode],
			iOtherNode,
			Parent->iBack,
			FILTERING_DOWN_BACK,
			BackOutside,
			BackOutside,
			NODE_Back,
			Flags &((PrevPlace==NODE_Front)?NF_PortalFront:NF_PortalBack)!=0
		);
	}
	else if( (State==FILTERING_DOWN_BACK) && Outside )
	{
		// Here we've reached a destination back leaf and therefore a non-empty polygon has 
		// survived the trip down both the front and the back of the source node, and we
		// know that both original nodes are adjacent and mutually visible.

		// Now iOtherNode  = The front leaf's iZone
		//     iPrevNode   = Lowest valid back node
		//     iNode       = INDEX_NONE
		if (NodeZones[PrevPlace][iPrevNode]==INDEX_NONE)
			appError ("Error2B");
		
		if
		(
			!PortalBlockCount 
		||	!(ModelInfo.BspNodes[iPrevNode].NodeFlags & ((PrevPlace==NODE_Front)?NF_PortalFront:NF_PortalBack))
		)
		{
			MergeTwoZones( iOtherNode, NodeZones[PrevPlace][iPrevNode] );
		}
	}
	unguard;
}

//
// Filter a portal polygon down to leaves, tagging all leaves that its
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
	if( EdPoly->NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD )
	{
		FPoly TempEdPoly;
		EdPoly->SplitInHalf(&TempEdPoly);
		FilterPortalToLeaves(iPrevNode,iNode,&TempEdPoly,PrevPlace,Outside);
	}
	if( iNode != INDEX_NONE )
	{
		FBspNode *Node   = &ModelInfo.BspNodes [iNode        ];
		FBspSurf *Poly   = &ModelInfo.BspSurfs [Node->iSurf  ];
		FVector  *Base   = &ModelInfo.FPoints  [Poly->pBase  ];
		FVector  *Normal = &ModelInfo.FVectors [Poly->vNormal];
		FPoly	 FrontPoly,BackPoly;

		switch( EdPoly->SplitWithPlane( *Base, *Normal, &FrontPoly, &BackPoly, 0) )
		{
			case SP_Coplanar:
				debugf(LOG_Info,"FZoneFilter::FilterPortalToLeaves: Got coplanar");
			
			case SP_Front:
				iPrevNode = iNode;
				iNode     = Node->iFront;
				Outside   = Outside || Node->IsCsg();
				PrevPlace = NODE_Front;
				goto FilterLoop;
			
			case SP_Back:
				iPrevNode = iNode;
				iNode     = Node->iBack;
				Outside   = Outside && !Node->IsCsg();
				PrevPlace = NODE_Back;
				goto FilterLoop;
			
			case SP_Split:
				FilterPortalToLeaves(iNode,Node->iFront,&FrontPoly,NODE_Front,Outside || Node->IsCsg());
				iPrevNode = iNode;
				iNode     = Node->iBack;
				Outside   = Outside && !Node->IsCsg();
				PrevPlace = NODE_Back;
				break;
		}
	}
	else
	{
		// At a leaf.
		PortalChunks++;
		
		if( !Outside )
			debugf(LOG_Info,"Portal inconsistency");
		
		ModelInfo.BspNodes[iPrevNode].NodeFlags |= 
			(PrevPlace==NODE_Front) ? NF_PortalFront : NF_PortalBack;
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
		if( OnceInfiniteEdPoly.NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD )
		{
			FPoly TempEdPoly;
			OnceInfiniteEdPoly.SplitInHalf(&TempEdPoly);
			PerformMergeAdjacentZones(iNode,Outside,TempEdPoly,iParentCuttingPlane,NumPlanes);
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
				debugf(LOG_Info,"FZoneFilter::MergeAdjacentZones: Invalid coplanar");
				return;
		}
		iParentCuttingPlane++;
	}

	// Filter it through the Bsp down to leaves and tag any leaves that are proven
	// to be adjacent.	
	FBspNode *Node = &ModelInfo.BspNodes[iNode];
	FilterToLeaf
	(
		&OnceInfiniteEdPoly, iNode, iNode, Node->iFront, FILTERING_DOWN_FRONT,
		Outside || Node->IsCsg(), Outside && !Node->IsCsg(), NODE_Front, 0
	);
	unguard;
}

//
// Recurse through the Bsp, merging zones that are adjacent.
//
void FZoneFilter::MergeAdjacentZones(INDEX iNode, int Outside)
{
	guard(FZoneFilter::MergeAdjacentZones);

	FBspNode	*Node   = &ModelInfo.BspNodes[iNode];
	FBspSurf	*Poly	= &ModelInfo.BspSurfs[Node->iSurf];

	// Recursively merge adjacents in front and back.
	// Tracks a list of cutting planes from the root down to
	// this node so that the hull bounding polys can be clipped
	// to their appropriate parent volumes.
	
	Planes[NumPlanes].Normal = &ModelInfo.FVectors[Poly->vNormal];
	Planes[NumPlanes].Base   = &ModelInfo.FPoints [Poly->pBase];
	
	if( Node->iFront != INDEX_NONE )
	{
		Planes[NumPlanes++].IsFront = 1;
		MergeAdjacentZones(Node->iFront,Outside || Node->IsCsg());
		NumPlanes--;
	}
	if( Node->iBack != INDEX_NONE )
	{
		Planes[NumPlanes++].IsFront = 0;
		MergeAdjacentZones(Node->iBack, Outside && !Node->IsCsg());
		NumPlanes--;
	}

	// For all coplanar PF_ZONE_PORTALS in this node, filter them down to
	// leaves and tag those leaves as NF_PortalX.  This causes zone portal
	// polygons to block the adjacency computations.
	for( int i=0; i<ModelInfo.NumBspNodes; i++ )
	{
		ModelInfo.BspNodes[i].NodeFlags &= ~(NF_PortalFront | NF_PortalBack);
	}
	
	INDEX iTempNode = iNode;
	while( iTempNode != INDEX_NONE )
	{
		FBspNode *TempNode = &ModelInfo.BspNodes[iTempNode];
		FBspSurf *TempPoly = &ModelInfo.BspSurfs[TempNode->iSurf];
		
		if( TempPoly->PolyFlags & PF_Portal )
		{
			NumPortals++;
			FPoly PortalPoly;
			if( GUnrealEditor.bspNodeToFPoly(&ModelInfo,iTempNode,&PortalPoly) )
			{
				FilterPortalToLeaves(iNode,Node->iFront,&PortalPoly,NODE_Front,Outside ||  Node->IsCsg());
				FilterPortalToLeaves(iNode,Node->iBack, &PortalPoly,NODE_Back, Outside && !Node->IsCsg());
			}
		}
		iTempNode = TempNode->iPlane;
	}

	// Build an "infinite" FPoly representing this node's plane.
	FPoly InfiniteEdPoly;
	BuildInfiniteFPoly(ModelInfo,iNode,InfiniteEdPoly);

	// Merge all zones which the infinite poly separates.
	PerformMergeAdjacentZones(iNode,Outside,InfiniteEdPoly,0,NumPlanes);

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
	INT			&iTopZone,
	INT			Outside,
	ENodePlace	PrevPlace
)
{
	guard(FZoneFilter::AssignUniqueLeafZones);

	if( iNode != INDEX_NONE )
	{
		FBspNode *Node = &ModelInfo.BspNodes[iNode];
		
		AssignUniqueLeafZones(iNode,Node->iFront,iTopZone,Outside ||  Node->IsCsg(),NODE_Front);
		AssignUniqueLeafZones(iNode,Node->iBack ,iTopZone,Outside && !Node->IsCsg(),NODE_Back);
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
	{
		for( int i=0; i<ModelInfo.NumBspNodes; i++ )
		{
			for( int j=0; j<2; j++ )
			{
				if( NodeZones[j][i] == iZone2 )
				{
					NodeZones[j][i]=iZone1;
				}
			}
		}
	}
	unguard;
}

//
// Renumber all zones in the Bsp so that zone numbers go from 1 to the maximum
// with no gaps.
//
void FZoneFilter::RemapZones()
{
	guard(FZoneFilter::RemapZones);

	BYTE	*ZoneRemap  = (BYTE *)GMem.GetZeroed(ModelInfo.NumZones * sizeof(BYTE));
	int		NewNumZones = 0;

	FBspNode *Node = &ModelInfo.BspNodes[0];
	for( int i=0; i<ModelInfo.NumBspNodes; i++ )
	{
		for( int j=0; j<2; j++ )
		{
			if( NodeZones[j][i] != INDEX_NONE )
			{
				if (ZoneRemap[NodeZones[j][i]]==0)
					ZoneRemap[NodeZones[j][i]] = 1+((NewNumZones++)%63);
				
				Node->iZone[j] = ZoneRemap[NodeZones[j][i]];
			}
			else Node->iZone[j] = 0;
		}
		Node++;
	}
	debugf(LOG_Info,"visBuild reduced %i zones down to %i (%i)",ModelInfo.NumZones,NewNumZones+1,Min(64,NewNumZones+1));
	ModelInfo.NumZones = Min(64,NewNumZones+1);
	GMem.Release(ZoneRemap);
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
void FZoneFilter::FindSingleZone(INDEX iPrevNode,INDEX iNode,FPoly *EdPoly,BYTE &iThisZone,int &MultipleZones,int Outside,ENodePlace PrevPlace)
{
	guard(FZoneFilter::FindSingleZone);

	FilterLoop:
	if( EdPoly->NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD )
	{
		FPoly TempEdPoly;
		EdPoly->SplitInHalf(&TempEdPoly);
		FindSingleZone(iPrevNode,iNode,&TempEdPoly,iThisZone,MultipleZones,Outside,PrevPlace);
	}
	if( iNode!=INDEX_NONE )
	{
		// Filter through.
		FBspNode *Node   = &ModelInfo.BspNodes [iNode        ];
		FBspSurf *Poly   = &ModelInfo.BspSurfs [Node->iSurf  ];
		FVector  *Base   = &ModelInfo.FPoints  [Poly->pBase  ];
		FVector  *Normal = &ModelInfo.FVectors [Poly->vNormal];
		FPoly	 FrontPoly,BackPoly;

		switch( EdPoly->SplitWithPlane (*Base,*Normal,&FrontPoly,&BackPoly,0) )
		{
			case SP_Coplanar:
				debugf(LOG_Info,"Coplanar");
				// Continue on as if it's front.
			
			case SP_Front:
				iPrevNode	= iNode;
				iNode		= Node->iFront;
				Outside     = Outside || Node->IsCsg();
				PrevPlace   = NODE_Front;
				goto FilterLoop;
				break;
			
			case SP_Back:
				iPrevNode	= iNode;
				iNode		= Node->iBack;
				Outside     = Outside && !Node->IsCsg();
				PrevPlace   = NODE_Back;
				goto FilterLoop;
				break;

			case SP_Split:
				FindSingleZone(iNode,Node->iFront,&FrontPoly,iThisZone,MultipleZones,Outside ||  Node->IsCsg(),NODE_Front);
				FindSingleZone(iNode,Node->iBack, &BackPoly, iThisZone,MultipleZones,Outside && !Node->IsCsg(),NODE_Back);
				break;
		}
	}
	else
	{
		// iPrevNode is a leaf.
		if( Outside )
		{
			FBspNode *PrevNode = &ModelInfo.BspNodes[iPrevNode];
			INDEX    iPrevZone = PrevNode->iZone[PrevPlace==NODE_Front];

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
void FZoneFilter::FilterMultiZonePoly(INDEX iSourceNode,INDEX iPrevNode,INDEX iNode,FPoly *EdPoly,int Outside,ENodePlace PrevPlace)
{
	guard(FZoneFilter::FilterMultiZonePoly);

	FilterLoop:
	if( EdPoly->NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD )
	{
		FPoly TempEdPoly;
		EdPoly->SplitInHalf(&TempEdPoly);
		FilterMultiZonePoly(iSourceNode,iPrevNode,iNode,&TempEdPoly,Outside,PrevPlace);		
	}
	if( iNode != INDEX_NONE )
	{
		// Filter through.
		FBspNode *Node   = &ModelInfo.BspNodes [iNode        ];
		FBspSurf *Poly   = &ModelInfo.BspSurfs [Node->iSurf  ];
		FVector  *Base   = &ModelInfo.FPoints  [Poly->pBase  ];
		FVector  *Normal = &ModelInfo.FVectors [Poly->vNormal];
		FPoly	 FrontPoly,BackPoly;

		switch( EdPoly->SplitWithPlane (*Base,*Normal,&FrontPoly,&BackPoly,0) )
		{
			case SP_Coplanar:
				// Happens occastionally due to precision errors.
				// Arbitrarily treat coplanars as front so they're most likely to get some zone
				// assigned to them.
			
			case SP_Front:
				iPrevNode	= iNode;
				iNode		= Node->iFront;
				Outside     = Outside || Node->IsCsg();
				PrevPlace	= NODE_Front;
				goto FilterLoop;
			
			case SP_Back:
				iPrevNode	= iNode;
				iNode		= Node->iBack;
				Outside     = Outside && !Node->IsCsg();
				PrevPlace	= NODE_Back;
				goto FilterLoop;
			
			case SP_Split:
				FilterMultiZonePoly(iSourceNode,iNode,Node->iFront,&FrontPoly,Outside ||  Node->IsCsg(),NODE_Front);
				FilterMultiZonePoly(iSourceNode,iNode,Node->iBack, &BackPoly, Outside && !Node->IsCsg(),NODE_Back);
				break;
		}
	}
	else
	{
		// iPrevNode is a leaf.
		if( Outside )
		{
			FBspNode	*SourceNode = &ModelInfo.BspNodes[iSourceNode];
			FBspNode	*PrevNode	= &ModelInfo.BspNodes[iPrevNode  ];
			INDEX       iPrevZone   = PrevNode->iZone[PrevPlace==NODE_Front];

			if( iPrevZone == 0 )
				appError("FZoneFilter::FilterMultiZonePoly: Zone inconsistency");

			INDEX iNewNode = GUnrealEditor.bspAddNode
			(
				&ModelInfo, iSourceNode, NODE_Plane,
				SourceNode->NodeFlags | NF_IsNew,
				EdPoly
			);
			ModelInfo.BspNodes[iNewNode].iZone[1] = iPrevZone;
			ModelInfo.BspNodes[iNewNode].iZone[0] = iPrevZone;

			INDEX iSurf = SourceNode->iSurf;
			FBspNode *Node = &ModelInfo.BspNodes[0];
			for( INDEX iNode=0; iNode<ModelInfo.NumBspNodes; iNode++)
			{
				if( Node->iSurf == iSurf ) Node->NodeFlags |= NF_NoMerge;
				Node++;
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
	
	INDEX		iOriginalNode	= iNode;
	FBspNode	*OriginalNode	= &ModelInfo.BspNodes[iOriginalNode];
	FBspSurf	*OriginalPoly	= &ModelInfo.BspSurfs[OriginalNode->iSurf];
	FVector		*OriginalNormal	= &ModelInfo.FVectors[OriginalPoly->vNormal];
	FPoly		EdPoly;

	// Recursively assign zone numbers to children.
	if( OriginalNode->iFront != INDEX_NONE )
		AssignAllZones(OriginalNode->iFront, Outside ||  OriginalNode->IsCsg());
	
	if( OriginalNode->iBack != INDEX_NONE )
		AssignAllZones(OriginalNode->iBack,  Outside && !OriginalNode->IsCsg());

	// Make sure this node's polygon resides in a single zone.  In other words,
	// find all of the zones belonging to outside Bsp leaves and make sure their
	// zone number is the same, and assign that zone number to this node.  Note that
	// if semisolid polygons exist in the world, polygon fragments may actually fall into
	// inside nodes, and these fragments (and their zones) can be disregarded.
	while( iNode != INDEX_NONE )
	{
		FBspNode	*Node		= &ModelInfo.BspNodes[iNode];
		FBspSurf	*Poly		= &ModelInfo.BspSurfs[Node->iSurf];

		if (Node->NodeFlags & NF_IsNew)
			break;

		if( GUnrealEditor.bspNodeToFPoly (&ModelInfo,iNode,&EdPoly) )
		{
			BYTE	ThisZone;
			int		MultipleZones = 0;

			int Forwards = ((*OriginalNormal | ModelInfo.FVectors[Poly->vNormal])>=0.0);
			if( Poly->PolyFlags & (PF_TwoSided|PF_Portal) )
			{
				// Two-sided's and portals aren't allowed to be split.
				ThisZone = 0;
				FindSingleZone
				(
					iOriginalNode,OriginalNode->iFront,&EdPoly,ThisZone,MultipleZones,
					Outside || OriginalNode->IsCsg(),NODE_Front
				);
				
				// If 0, this is probably an interior semisolid poly.
				Node->iZone[Forwards] = ThisZone;

				ThisZone = 0;
				FindSingleZone
					(
					iOriginalNode,OriginalNode->iBack,&EdPoly,ThisZone,MultipleZones,
					Outside && !OriginalNode->IsCsg(),NODE_Back
					);
				// If 0, this is probably an interior semisolid poly.
				Node->iZone[1-Forwards] = ThisZone;
			}
			else
			{
				ThisZone      = 0;
				MultipleZones = 0;
				if( Forwards )
				{
					FindSingleZone
					(
						iOriginalNode,OriginalNode->iFront,&EdPoly,ThisZone,MultipleZones,
						Outside || OriginalNode->IsCsg(),NODE_Front
					);
					if( MultipleZones )
					{
						FilterMultiZonePoly
						(
							iNode,iOriginalNode,OriginalNode->iFront,
							&EdPoly,Outside || OriginalNode->IsCsg(),NODE_Front
						);
						Node->NodeFlags |= NF_TagForEmpty;
					}
					else
					{
						// If 0, this is probably an interior semisolid poly.
						Node->iZone[1] = ThisZone;
					}
				}
				else
				{
					FindSingleZone
					(
						iOriginalNode,OriginalNode->iBack,&EdPoly,ThisZone,MultipleZones,
						Outside && !OriginalNode->IsCsg(),NODE_Back
					);
					if( MultipleZones )
					{
						FilterMultiZonePoly
						(
							iNode,iOriginalNode,OriginalNode->iBack,
							&EdPoly,Outside && !OriginalNode->IsCsg(),NODE_Back
						);
						Node->NodeFlags |= NF_TagForEmpty;
					}
					else
					{
						// If 0, this is probably an interior semisolid poly.
						Node->iZone[1] = ThisZone;
					}
				}
			}
		}
		iNode = Node->iPlane;
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

	FBspNode	*Node		= &ModelInfo.BspNodes[iNode];
	QWORD		ZoneMask	= 0;

	if (Node->iZone[1]!=0) ZoneMask |= ((QWORD)1) << Node->iZone[1];
	if (Node->iZone[0]!=0) ZoneMask |= ((QWORD)1) << Node->iZone[0];

	if (Node->iFront != INDEX_NONE)	ZoneMask |= BuildZoneMasks(Node->iFront);
	if (Node->iBack  != INDEX_NONE)	ZoneMask |= BuildZoneMasks(Node->iBack );
	if (Node->iPlane != INDEX_NONE)	ZoneMask |= BuildZoneMasks(Node->iPlane);

	Node->ZoneMask = ZoneMask;

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
		ModelInfo.BspNodesResource->Zones[i].Connectivity = ((QWORD)1)<<i;
	}
	for( i=0; i<ModelInfo.NumBspNodes; i++ )
	{
		// Process zones connected by portals.
		FBspNode *Node = &ModelInfo.BspNodes[i];
		FBspSurf *Poly = &ModelInfo.BspSurfs[Node->iSurf];

		if( Poly->PolyFlags & PF_Portal )
		{
			ModelInfo.BspNodesResource->Zones[Node->iZone[1]].Connectivity |= ((QWORD)1) << Node->iZone[0];
			ModelInfo.BspNodesResource->Zones[Node->iZone[0]].Connectivity |= ((QWORD)1) << Node->iZone[1];
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Zone-to-zone visibility.
-----------------------------------------------------------------------------*/

//
// Go through all portals in the world and sample their visibility.
// Call with:
//
// SampleDensity = maximum world-unit distance between lattice-based
//                 samples (i.e. 16 = sample size of 16x16 world units).
//
// MinSampleSize = minimum number of samples to take in each direction (i.e. 8 = don't
//                 sample a lattice less than 8x8).
//
// Visibility is generated by rendering two sets of views at each portal, one set in the
// in-facing direction, and one in the out-facing direction.  At these portals, the
// world is span-buffer rendered and the zones inside of all other visible portals in the
// world are tagged as visible.  This also explicitly tags Entry(i,i) as visible, and
// tags Entry(i,j) as visible if Entry(i,j) is connected.
//
void FZoneFilter::BuildVisibility(int SampleDensity, int MinSampleSize)
{
	guard(FZoneFilter::BuildVisibility);
	for( int i=0; i<64; i++ )
	{
		ModelInfo.BspNodesResource->Zones[i].Visibility = ModelInfo.BspNodesResource->Zones[i].Connectivity;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	FZoneFilter: Zone descriptors.
-----------------------------------------------------------------------------*/

//
// Attach ZoneDescriptor actors to the zones that they belong in.
// ZoneDescriptor actors are a class of actor which level designers may
// place in UnrealEd in order to specify the properties of the zone they
// reside in, such as water effects, zone name, etc.
//
void FZoneFilter::BuildZoneDescriptors()
{
	guard(FZoneFilter::BuildZoneDescriptors);
	int Descriptors=0, Duplicates=0, Zoneless=0;

	for( int i=0; i<64; i++ )
	{
		ModelInfo.BspNodesResource->Zones[i].iZoneActor = INDEX_NONE;
	}
	for( i=0; i<Level->ActorList->Max; i++ )
	{
		AActor *Actor = &Level->ActorList->Element(i);
		if( Actor->IsKindOf("ZoneDescriptor") )
		{
			Actor->Zone = ModelInfo.PointZone(Actor->Location);
			if( Actor->Zone==0 )
			{
				Zoneless++;
			}
			else if( ModelInfo.BspNodesResource->Zones[Actor->Zone].iZoneActor != INDEX_NONE )
			{
				Duplicates++;
			}
			else
			{
				Descriptors++;
				ModelInfo.BspNodesResource->Zones[Actor->Zone].iZoneActor = i;
			}
		}
	}
	debugf(LOG_Info,"visBuild: %i ZoneDescriptors, %i duplicates, %i zoneless",Descriptors,Duplicates,Zoneless);
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
void FGlobalEditor::visBuild(ULevel *Level)
{
	guard(FGlobalEditor::visBuild);

	UModel *Model = Level->Model;
	FZoneFilter Filter;

	Model->Lock(&Filter.ModelInfo,LOCK_NoTrans);
	Filter.Level = Level;
	Filter.ModelInfo.NumZones = 0;

	int NumPortals=0;
	for( int i=0; i<Filter.ModelInfo.NumBspSurfs; i++ )
	{
		if (Filter.ModelInfo.BspSurfs[i].PolyFlags & PF_Portal) 
			NumPortals++;
	}
	if( Filter.ModelInfo.NumBspNodes )
	{
		debug(LOG_Info,"visBuild begin");

		// Allocate working tables.
		Filter.Planes		= (FZoneFilterCuttingPlane *)GMem.Get(MAX_DEPTH * sizeof(FZoneFilterCuttingPlane));
		Filter.NodeZones[0] = (INDEX                   *)GMem.Get(Filter.ModelInfo.NumBspNodes * sizeof (INDEX));
		Filter.NodeZones[1] = (INDEX                   *)GMem.Get(Filter.ModelInfo.NumBspNodes * sizeof (INDEX));

		// Give each leaf in the world a unique zone number.
		for( int i=0; i<Filter.ModelInfo.NumBspNodes; i++ )
		{
			Filter.NodeZones[1][i]					= INDEX_NONE;
			Filter.NodeZones[0][i]					= INDEX_NONE;
			Filter.ModelInfo.BspNodes[i].iZone[0]	= 0;
			Filter.ModelInfo.BspNodes[i].iZone[1]	= 0;
			Filter.ModelInfo.BspNodes[i].NodeFlags &= ~(NF_PortalFront | NF_PortalBack | NF_IsNew);
		}

		// Don't assign 0 to any zone.
		Filter.ModelInfo.NumZones = 1;
		
		Filter.AssignUniqueLeafZones(0,0,Filter.ModelInfo.NumZones,FBspNode::ROOT_OUTSIDE,NODE_Root);
		debugf(LOG_Info,"visBuild assigned %i starting zones",Filter.ModelInfo.NumZones);

		// Go through the entire tree, merging zones which are spatially connected.
		Filter.NumPortals   = 0;
		Filter.PortalChunks = 0;
		Filter.NumPlanes    = 0;
		Filter.MergeAdjacentZones(0,FBspNode::ROOT_OUTSIDE);

		// Renumber the zones so we're left with a smaller, more manageable set.
		Filter.RemapZones();

		// Assign zone numbers to all nodes in the world (previous to this, only leaves
		// have zone numbers).
		Filter.AssignAllZones(0,FBspNode::ROOT_OUTSIDE);
		Filter.BuildConnectivity();
		Filter.BuildVisibility(0,0);
		Filter.BuildZoneDescriptors();

		// Clean up Bsp.  Required since node polys may have been filtered, leaving zero-
		// vertex node polys with possibly non-empty coplanars.
		int NodesMissed=0;
		for( i=0; i<Filter.ModelInfo.NumBspNodes; i++ )
		{
			FBspNode *Node = &Filter.ModelInfo.BspNodes[i];

			if ((Node->iZone == 0) && !(Node->NodeFlags&NF_TagForEmpty))
				NodesMissed++;
			
			Filter.ModelInfo.BspNodes[i].NodeFlags &= ~(NF_PortalFront | NF_PortalBack);
		}
		GMem.Release(Filter.Planes);

		GUnrealEditor.bspCleanup		(&Filter.ModelInfo);	// Clean up newly-emptied nodes.
		GUnrealEditor.bspRefresh		(&Filter.ModelInfo,0);	// Delete unreferenced stuff if near overflow.
		GUnrealEditor.bspBuildBounds	(&Filter.ModelInfo);	// Build bounding box structure.

		// Build 64-entry rejection mask for each node.  Must call this after
		// bspCleanup.
		Filter.BuildZoneMasks(0);

		// Summary stats.
		debugf(LOG_Info,"visBuild end, portals=%i/%i, missed=%i",
			Filter.NumPortals,
			Filter.PortalChunks,			
			NodesMissed);
	}
	else debug(LOG_Info,"visBuild built no zones");
	Model->Unlock(&Filter.ModelInfo);
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
void UpdateBoundWithPolys(FBoundingRect *Bound,FPoly **PolyList,int nPolys)
{
	guard(UpdateBoundWithPolys);
	for( int i=0; i<nPolys; i++ )
	{
		FPoly   *Poly = PolyList[i];
		FVector *V    = &Poly->Vertex[0];
		
		for( int j=0; j<nPolys; j++ )
		{
			UpdateMin(&Bound->Min,V);
			UpdateMax(&Bound->Max,V);
			V++;
		}
#if DEBUG_HULLS
		DEBUG_Brush->Polys->Element(DEBUG_Brush->Polys->Num++)=*Poly;
#endif
	}
	unguard;
}

//
// Cut a partitioning poly by a list of polys, and add the resulting inside pieces to the
// front list and back list.
//
void SplitPartitioner
(
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
		if( InfiniteEdPoly.NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD )
		{
			FPoly Half;
			InfiniteEdPoly.SplitInHalf(&Half);
			SplitPartitioner(PolyList,FrontList,BackList,n,nPolys,nFront,nBack,Half);
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
		if( (nFront >= MAX_BOUND_POLYS) || (nBack >= MAX_BOUND_POLYS) )
			appError("Convex volume is too complex");

		n++;
	}
	FPoly *New = (FPoly *)GMem.Get(sizeof(FPoly));
	*New = InfiniteEdPoly;
	New->Reverse();
	FrontList[nFront++] = New;

	New = (FPoly *)GMem.Get(sizeof(FPoly));
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
	IModel			*ModelInfo,
	FBoundingRect	*ParentBound,
	INDEX			iNode,
	FPoly			**PolyList,
	INT				nPolys,
	INT				Outside
)
{
	FBspNode		*Node		= &ModelInfo->BspNodes [iNode];
	FBspSurf		*Surf		= &ModelInfo->BspSurfs [Node->iSurf];
	FVector			*Base		= &ModelInfo->FPoints  [Surf->pBase];
	FVector			*Normal		= &ModelInfo->FVectors [Surf->vNormal];
	VOID			*MemTop		= GMem.Get(0);
	FBoundingRect	Bound;

	Bound.Min.X = Bound.Min.Y = Bound.Min.Z = +65536.0;
	Bound.Max.X = Bound.Max.Y = Bound.Max.Z = -65536.0;

	// Split bound into front half and back half.
	FPoly *FrontList[MAX_BOUND_POLYS]; int nFront=0;
	FPoly *BackList [MAX_BOUND_POLYS]; int nBack=0;

	FPoly *FrontPoly = (FPoly *)GMem.Get(sizeof(FPoly));
	FPoly *BackPoly  = (FPoly *)GMem.Get(sizeof(FPoly));

	for( int i=0; i<nPolys; i++ )
	{
		FPoly *Poly = PolyList[i];
		switch( Poly->SplitWithPlane(*Base,*Normal,FrontPoly,BackPoly,0) )
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
				if( FrontPoly->NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD )
				{
					FPoly *Half = (FPoly *)GMem.Get(sizeof(FPoly));
					FrontPoly->SplitInHalf(Half);
					FrontList[nFront++] = Half;
				}
				FrontList[nFront++] = FrontPoly;

				if( BackPoly->NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD )
				{
					FPoly *Half = (FPoly *)GMem.Get(sizeof(FPoly));
					BackPoly->SplitInHalf(Half);
					BackList[nBack++] = Half;
				}
				BackList [nBack++] = BackPoly;

				FrontPoly           = (FPoly *)GMem.Get(sizeof(FPoly));
				BackPoly            = (FPoly *)GMem.Get(sizeof(FPoly));
				break;

			default:
				appError("FZoneFilter::FilterToLeaf: Unknown split code");
			}
		
		if( (nFront >= MAX_BOUND_POLYS)||(nBack >= MAX_BOUND_POLYS) )
			appError("Convex area is too complex");
	}
	if( nFront && nBack )
	{
		// Add partitioner plane to front and back.
		FPoly InfiniteEdPoly;
		BuildInfiniteFPoly(*ModelInfo,iNode,InfiniteEdPoly);

		SplitPartitioner(PolyList,FrontList,BackList,0,nPolys,nFront,nBack,InfiniteEdPoly);
	}
	else
	{
		if( !nFront ) debugf("FilterBound: Empty fronthull");
		if( !nBack  ) debugf("FilterBound: Empty backhull");
	}

	// Recursively update all our childrens' bounding volumes.
	if( nFront > 0 )
	{
		if( Node->iFront != INDEX_NONE )
			FilterBound
			(
				ModelInfo,&Bound,Node->iFront,
				FrontList, nFront,
				Outside ||  Node->IsCsg()
			);
		else if( Outside || Node->IsCsg() )
			UpdateBoundWithPolys( &Bound,FrontList,nFront );
	}
	if( nBack > 0 )
	{
		if( Node->iBack != INDEX_NONE)
			FilterBound
			(
				ModelInfo,&Bound,Node->iBack, 
				BackList, nBack,
				Outside && !Node->IsCsg()
			);
		else if( Outside && !Node->IsCsg() )
			UpdateBoundWithPolys(&Bound,BackList,nBack);
	}

	// Apply this bound to this node.
	Node->NodeFlags |= NF_Bounded;
	Node->iBound     = ModelInfo->NumBounds++;

	if (ModelInfo->NumBounds > ModelInfo->MaxBounds)
		appError ("Bound table full");

	ModelInfo->Bounds[Node->iBound] = Bound;

	// Update parent bound to enclose this bound.
	if( ParentBound )
	{
		UpdateMin(&ParentBound->Min,&Bound.Min);
		UpdateMax(&ParentBound->Max,&Bound.Max);
	}
	GMem.Release(MemTop);
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
void FGlobalEditor::bspBuildBounds(IModel *ModelInfo)
{
	guard(FGlobalEditor::bspBuildBounds);
	
	if (ModelInfo->NumBspNodes==0)
		return;

#if DEBUG_HULLS
	DEBUG_Brush=new("Brush",FIND_Existing)UModel;
	DEBUG_Brush->Polys->Num=0;
	DEBUG_Brush->Location=DEBUG_Brush->PrePivot=DEBUG_Brush->PostPivot=GMath.ZeroVector;
	DEBUG_Brush->Rotation=GMath.ZeroRotation;
#endif

	FPoly *PolyList[6];
	for (int i=0; i<6; i++)
		PolyList[i] = &GGfx.RootHullBrush->Polys->Element(i);

	ModelInfo->NumBounds=0;
	FilterBound(ModelInfo,NULL,0,PolyList,6,FBspNode::ROOT_OUTSIDE);

	debugf("bspBuildBounds: Generated %i bounds",ModelInfo->NumBounds);
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
