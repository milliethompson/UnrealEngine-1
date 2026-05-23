/*=============================================================================
	UnVisi.cpp: Unreal visibility computation

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	Experimental visibility code.

Definitions:

Design notes:

Revision history:
=============================================================================*/

#include "Unreal.h"
#include "UnEdge.h"

/*-----------------------------------------------------------------------------
	Temporary.
-----------------------------------------------------------------------------*/

// Options.
#define DEBUG_PORTALS	0	/* Debugging hull code by generating hull brush */
#define DEBUG_WRAPS		0	/* Debugging sheet wrapping code */
#define DEBUG_BADSHEETS 0   /* Debugging sheet discrepancies */
#define DEBUG_LVS       0   /* Debugging light volumes */
#define WORLD_MAX 65536.0	/* Maximum size of the world */
FPoly BuildInfiniteFPoly( UModel *Model, INDEX iNode );

// Thresholds.
#define VALID_SIDE  0.1   /* A normal must be at laest this long to be valid */
#define VALID_CROSS 0.001 /* A cross product can be safely normalized if this big */

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

//
// Debugging.
//
#if DEBUG_PORTALS || DEBUG_WRAPS || DEBUG_BADSHEETS || DEBUG_LVS
	static UModel *DEBUG_Brush;
#endif

//
// A portal.
//
class FPortal : public FPoly
{
public:
	// Variables.
	INDEX	iFrontLeaf, iBackLeaf;
	FPortal *GlobalNext, *FrontLeafNext, *BackLeafNext, *NodeNext;
	BYTE	IsTesting, ShouldTest;
	INT		FragmentCount;
	INT		ZonePortalCount;

	// Constructor.
	FPortal( FPoly &InPoly, INDEX iInFrontLeaf, INDEX iInBackLeaf, FPortal *InGlobalNext, FPortal *InNodeNext, FPortal *InFrontLeafNext, FPortal *InBackLeafNext )
	:	FPoly			(InPoly),
		iFrontLeaf		(iInFrontLeaf),
		iBackLeaf		(iInBackLeaf),
		GlobalNext		(InGlobalNext),
		NodeNext		(InNodeNext),
		FrontLeafNext	(InFrontLeafNext),
		BackLeafNext	(InBackLeafNext),
		IsTesting		(0),
		ShouldTest		(0),
		FragmentCount	(0),
		ZonePortalCount (0)
	{}
	
	// Get the leaf on the opposite side of the specified leaf.
	INDEX GetNeighborLeafOf( INDEX iLeaf )
	{
		checkLogic( iLeaf==iFrontLeaf || iLeaf==iBackLeaf );
		if     ( iFrontLeaf == iLeaf )	return iBackLeaf;
		else							return iFrontLeaf;
	}

	// Get the next portal for this leaf in the linked list of portals.
	FPortal *Next( INDEX iLeaf )
	{
		checkLogic( iLeaf==iFrontLeaf || iLeaf==iBackLeaf );
		if     ( iFrontLeaf == iLeaf )	return FrontLeafNext;
		else							return BackLeafNext;
	}

	// Return this portal polygon, facing outward from leaf iLeaf.
	void GetPolyFacingOutOf( INDEX iLeaf, FPoly &Poly)
	{
		checkLogic( iLeaf==iFrontLeaf || iLeaf==iBackLeaf );
		Poly = *(FPoly*)this;
		if( iLeaf == iFrontLeaf ) Poly.Reverse();
	}

	// Return this portal polygon, facing inward to leaf iLeaf.
	void GetPolyFacingInto( INDEX iLeaf, FPoly &Poly)
	{
		checkLogic( iLeaf==iFrontLeaf || iLeaf==iBackLeaf );
		Poly = *(FPoly*)this;
		if( iLeaf == iBackLeaf ) Poly.Reverse();
	}
};

//
// The visibility calculator class.
//
class FEditorVisibility
{
public:
	// Constants.
	enum {MAX_CLIPS=16384};
	enum {CLIP_BACK_FLAG=0x40000000};

	// Types.
	typedef void (FEditorVisibility::*PORTAL_FUNC)(FPoly&,INDEX,INDEX,INDEX,INDEX);

	// Variables.
	FMemAutoMark		Mark;
	ULevel*				Level;
	UModel*				Model;
	INDEX				Clips[MAX_CLIPS];
	INT					NumPortals, NumLeaves, NumLogicalLeaves;
	INT					NumClips, NumClipTests, NumPassedClips, NumUnclipped;
	INT					NumBspPortals, MaxFragments, NumZonePortals, NumZoneFragments;
	INT					Extra;
	FBspLeaf*			Leaves;
	FPortal*			FirstPortal;
	UBitMatrix*			Visibility;
	FPortal**			NodePortals;
	FPortal**			LeafPortals;

	// Constructor.
	FEditorVisibility( ULevel* InLevel, UModel* InModel, INT InDebug );

	// Destructor.
	~FEditorVisibility();

	// Portal functions.
	void AddPortal( FPoly &Poly, INDEX iFrontLeaf, INDEX iBackLeaf, INDEX iGeneratingNode, INDEX iGeneratingBase );
	void BlockPortal( FPoly &Poly, INDEX iFrontLeaf, INDEX iBackLeaf, INDEX iGeneratingNode, INDEX iGeneratingBase );
	void TagZonePortalFragment( FPoly &Poly, INDEX iFrontLeaf, INDEX iBackLeaf, INDEX iGeneratingNode, INDEX iGeneratingBase );
	void FilterThroughSubtree( INT Pass, INDEX iGeneratingNode, INDEX iGeneratingBase, INDEX iParentLeaf, INDEX iNode, FPoly Poly, PORTAL_FUNC Func, INDEX iBackLeaf );
	void MakePortalsClip( INDEX iNode, FPoly Poly, INT Clip, PORTAL_FUNC Func );
	void MakePortals( INDEX iNode );
	void AssignLeaves( INDEX iNode, INT Outside );
	int ClipToMaximalSheetWrapping( FPoly &Poly, const FPoly &A, const FPoly &B, const FLOAT Sign, const FLOAT Phase );
	void CheckVolumeVisibility( const INDEX iSourceLeaf, const FPoly &Source, const INDEX iTestLeaf, const FPoly &Clip, const FPortal *ClipPortal );
	int PointToLeaf( FVector Point, INDEX iLeaf );
	void ActorVisibility( AActor* Actor, INDEX* VisibleLeaves, INT& NumVisibleLeaves, INDEX iLeaf=INDEX_NONE, FPoly* Clipper=NULL );

	// Zone functions.
	void FormZonesFromLeaves();
	void AssignAllZones( INDEX iNode, int Outside );
	QWORD BuildZoneMasks( INDEX iNode );
	void BuildConnectivity();
	void BuildZoneInfo();

	// Visibility functions.
	void BspCrossVisibility( INDEX iFronyPortalLeaf, INDEX iBackPortalLeaf, INDEX iFrontLeaf, INDEX iBackLeaf, FPoly &FrontPoly, FPoly &ClipPoly, FPoly &BackPoly, INT ValidPolys, INT Pass, INT Tag );
	void BspVisibility( INDEX iNode );
	void TestVisibility();
};

/*-----------------------------------------------------------------------------
	Portal building, a simple recursive hierarchy of functions.
-----------------------------------------------------------------------------*/

//
// Tag a zone portal fragment.
//
void FEditorVisibility::TagZonePortalFragment
(
	FPoly	&Poly,
	INDEX	iFrontLeaf,
	INDEX	iBackLeaf,
	INDEX	iGeneratingNode,
	INDEX   iGeneratingBase
)
{
	guard(FEditorVisibility::TagZonePortalFragment);

	// Add this node to the bsp as a coplanar to its generator.
	INDEX iNewNode = GUnrealEditor.bspAddNode( Model, iGeneratingNode, NODE_Plane, Model->Nodes(iGeneratingNode).NodeFlags | NF_IsNew, &Poly );

	// Set the node's zones.
	int Backward = (Poly.Normal | Model->Nodes(iGeneratingBase).Plane) < 0.0;
	Model->Nodes(iNewNode).iZone[Backward^0] = iBackLeaf ==INDEX_NONE ? 0 : Leaves[iBackLeaf ].iZone;
	Model->Nodes(iNewNode).iZone[Backward^1] = iFrontLeaf==INDEX_NONE ? 0 : Leaves[iFrontLeaf].iZone;

	unguard;
}

//
// Mark a portal as blocked.
//
void FEditorVisibility::BlockPortal
(
	FPoly	&Poly,
	INDEX	iFrontLeaf,
	INDEX	iBackLeaf,
	INDEX	iGeneratingNode,
	INDEX   iGeneratingBase
)
{
	guard(FEditorVisibility::BlockPortal);
	if( iFrontLeaf!=INDEX_NONE && iBackLeaf!=INDEX_NONE )
	{
		for( FPortal* Portal=FirstPortal; Portal; Portal=Portal->GlobalNext )
		{
			if
			(	(Portal->iFrontLeaf==iFrontLeaf && Portal->iBackLeaf==iBackLeaf )
			||	(Portal->iFrontLeaf==iBackLeaf  && Portal->iBackLeaf==iFrontLeaf) )
			{
				Portal->ZonePortalCount++;
				NumZoneFragments++;
			}
		}
	}
	unguard;
}

//
// Add a portal to the portal list.
//
void FEditorVisibility::AddPortal
(
	FPoly	&Poly,
	INDEX	iFrontLeaf,
	INDEX	iBackLeaf,
	INDEX	iGeneratingNode,
	INDEX   iGeneratingBase
)
{
	guard(FEditorVisibility::AddPortal);
	if( iFrontLeaf!=INDEX_NONE && iBackLeaf!=INDEX_NONE )
	{
		// Add to linked list of all portals.
		FirstPortal						= 
		LeafPortals[iFrontLeaf]			= 
		LeafPortals[iBackLeaf]			= 
		NodePortals[iGeneratingNode]	= 
			new(GMem)FPortal
			(
				Poly,
				iFrontLeaf,
				iBackLeaf,
				FirstPortal,
				NodePortals[iGeneratingNode],
				LeafPortals[iFrontLeaf],
				LeafPortals[iBackLeaf]
			);
		NumPortals++;

#if DEBUG_PORTALS
		//debugf("AddPortal: %i verts",Poly.NumVertices);
		Poly.PolyFlags |= PF_NotSolid;
		DEBUG_Brush->Polys->AddItem(Poly);
#endif
	}
	unguard;
}

//
// Filter a portal through a front or back subtree.
//
void FEditorVisibility::FilterThroughSubtree
(
	INT			Pass,
	INDEX		iGeneratingNode,
	INDEX		iGeneratingBase,
	INDEX		iParentLeaf,
	INDEX		iNode,
	FPoly		Poly,
	PORTAL_FUNC Func,
	INDEX		iBackLeaf
)
{
	guard(FEditorVisibility::FilterThroughSubtree);
	while( iNode != INDEX_NONE )
	{
		// If overflow.
		if( Poly.NumVertices > FPoly::VERTEX_THRESHOLD )
		{
			FPoly Half;
			Poly.SplitInHalf( &Half );
			FilterThroughSubtree( Pass, iGeneratingNode, iGeneratingBase, iParentLeaf, iNode, Half, Func, iBackLeaf );
		}

		// Test split.
		FPoly Front,Back;
		int Split = Poly.SplitWithNode( Model, iNode, &Front, &Back, 1 );

		// Recurse with front.
		if( Split==SP_Front || Split==SP_Split )
			FilterThroughSubtree
			(
				Pass,
				iGeneratingNode,
				iGeneratingBase,
				Model->Nodes(iNode).iDynamic[1],
				Model->Nodes(iNode).iFront,
				Split==SP_Front ? Poly : Front,
				Func,
				iBackLeaf
			);

		// Consider back.
		if( Split!=SP_Back && Split!=SP_Split )
			return;

		// Loop with back.
		if( Split == SP_Split )
			Poly = Back;
		iParentLeaf = Model->Nodes(iNode).iDynamic[0];
		iNode       = Model->Nodes(iNode).iBack;
	}

	// We reached a leaf in this subtree.
	if( Pass == 0 ) FilterThroughSubtree
	(
		1,
		iGeneratingNode,
		iGeneratingBase,
		Model->Nodes(iGeneratingBase).iDynamic[1],
		Model->Nodes(iGeneratingBase).iFront,
		Poly,
		Func,
		iParentLeaf
	);
	else (this->*Func)( Poly, iParentLeaf, iBackLeaf, iGeneratingNode, iGeneratingBase );
	unguard;
}

//
// Clip a portal by all parent nodes above it.
//
void FEditorVisibility::MakePortalsClip
(
	INDEX		iNode,
	FPoly		Poly,
	INT			Clip,
	PORTAL_FUNC Func
)
{
	guard(FEditorVisibility::MakePortalsClip);

	// Clip by all parents.
	while( Clip < NumClips )
	{
		INDEX    iClipNode = Clips[Clip] & ~CLIP_BACK_FLAG;
		FBspNode &Node     = Model->Nodes(iClipNode);

		// Subdivide if poly vertices overflow.
		if( Poly.NumVertices >= FPoly::VERTEX_THRESHOLD )
		{
			FPoly TempPoly;
			Poly.SplitInHalf( &TempPoly );
			MakePortalsClip( iNode, TempPoly, Clip, Func );
		}

		// Split by parent.
		FPoly Front,Back;
		int Split = Poly.SplitWithNode(Model,iClipNode,&Front,&Back,1);

		// Make sure we generated a useful fragment.
		if(	(Split==SP_Front &&  (Clips[Clip] & CLIP_BACK_FLAG) )
		||	(Split==SP_Back  && !(Clips[Clip] & CLIP_BACK_FLAG) )
		||	(Split==SP_Coplanar))
		{
			// Clipped to oblivion, or useless coplanar.
			return;
		}

		if( Split==SP_Split )
		{
			// Keep the appropriate piece.
			Poly = (Clips[Clip] & CLIP_BACK_FLAG) ? Back : Front;
		}

		// Clip by next parent.
		Clip++;
	}

	// Filter poly down the back subtree.
	FilterThroughSubtree
	(
		0,
		iNode,
		iNode,
		Model->Nodes(iNode).iDynamic[0],
		Model->Nodes(iNode).iBack,
		Poly,
		Func,
		INDEX_NONE
	);
	unguard;
}

//
// Make all portals.
//
void FEditorVisibility::MakePortals( INDEX	iNode )
{
	guard(FEditorVisibility::MakePortals);
	INDEX iOriginalNode = iNode;

	// Make an infinite edpoly for this node.
	FPoly Poly = BuildInfiniteFPoly( Model, iNode );

	// Filter the portal through this subtree.
	MakePortalsClip( iNode, Poly, 0, AddPortal );

	// Make portals for front.
	if( Model->Nodes(iNode).iFront != INDEX_NONE )
	{
		Clips[NumClips++] = iNode;
		MakePortals( Model->Nodes(iNode).iFront );
		NumClips--;
	}

	// Make portals for back.
	if( Model->Nodes(iNode).iBack != INDEX_NONE )
	{
		Clips[NumClips++] = iNode | CLIP_BACK_FLAG;
		MakePortals( Model->Nodes(iNode).iBack );
		NumClips--;
	}

	// For all zone portals at this node, mark the matching FPortals as blocked.
	while( iNode != INDEX_NONE )
	{
		FBspNode& Node = Model->Nodes( iNode      );
		FBspSurf& Surf = Model->Surfs( Node.iSurf );
		if( (Surf.PolyFlags & PF_Portal) && GEditor->bspNodeToFPoly( Model, iNode, &Poly ) )
		{
			NumZonePortals++;
			FilterThroughSubtree
			(
				0,
				iNode,
				iOriginalNode,
				Model->Nodes(iOriginalNode).iDynamic[0],
				Model->Nodes(iOriginalNode).iBack,
				Poly,
				BlockPortal,
				INDEX_NONE
			);
		}
		iNode = Node.iPlane;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Assign leaves.
-----------------------------------------------------------------------------*/

//
// Assign contiguous unique numbers to all front and back leaves in the BSP.
// Stores the leaf numbers in FBspNode::iDynamics[2].
//
void FEditorVisibility::AssignLeaves( INDEX iNode, INT Outside )
{
	guard(FEditorVisibility::AssignLeaves);

	FBspNode &Node = Model->Nodes(iNode);
	for( int i=0; i<2; i++ )
		if( Node.iChild[i] != INDEX_NONE )
			AssignLeaves( Node.iChild[i], Node.ChildOutside( i, Outside, NF_NotVisBlocking ) );
		else if( Node.ChildOutside( i, Outside, NF_NotVisBlocking ) )
			Node.iDynamic[i] = NumLeaves++;

	unguard;
}

/*-----------------------------------------------------------------------------
	Point visibility tests.
-----------------------------------------------------------------------------*/

//
// Recursively build a list of leaves visible from a point.
// Uses a recursive shadow volume clipper.
//
void FEditorVisibility::ActorVisibility
(
	AActor*	Actor,
	INDEX*	VisibleLeaves,
	INT&	NumVisibleLeaves,
	INDEX	iLeaf,
	FPoly*	Clipper
)
{
	guard(FEditorVisibility::ActorVisibility);

	// If leaf not specified, find the leaf corresponding to the point.
	if( iLeaf == INDEX_NONE )
	{
		NumVisibleLeaves = 0;
		INDEX iNode=0, iParent=0, IsFront=0;
		while( iNode != INDEX_NONE )
		{
			IsFront = (Model->Nodes(iNode).Plane.PlaneDot(Actor->Location) > 0.0);
			iParent = iNode;
			iNode   = Model->Nodes(iNode).iChild[IsFront];
		}
		iLeaf = Model->Nodes(iParent).iDynamic[IsFront];
		if( iLeaf == INDEX_NONE )
			return;
	}

#if DEBUG_LVS
	if( Clipper )
	{
		Clipper->PolyFlags |= PF_NotSolid;
		DEBUG_Brush->Polys->AddItem( *Clipper );
	}
#endif

	// Add this leaf to the list if it's new.
	for( INDEX i=0; i<NumVisibleLeaves; i++ )
		if( VisibleLeaves[i] == iLeaf )
			break;
	if( i == NumVisibleLeaves )
		VisibleLeaves[NumVisibleLeaves++] = iLeaf;
	checkState(NumVisibleLeaves <= NumLeaves);

	// Recursively check leaves on other side.
	for( FPortal* Portal=LeafPortals[iLeaf]; Portal!=NULL; Portal=Portal->Next(iLeaf) )
	{
		FPoly Poly;
		Portal->GetPolyFacingOutOf( iLeaf, Poly );
		FLOAT PlaneDot = (Actor->Location - Poly.Base) | Poly.Normal;
		if( PlaneDot<0.0 && PlaneDot>-Actor->WorldLightRadius() )
		{
			INDEX iOtherLeaf = Portal->GetNeighborLeafOf( iLeaf );
			if( Clipper )
			{
				// Clip Poly by the specified clipping polygon.
				for( int i=0,j=Clipper->NumVertices-1; i<Clipper->NumVertices; j=i++ )
				{
					if( Poly.NumVertices >= FPoly::VERTEX_THRESHOLD )
						break;
					FPoly Front, Back;
					int Split = Poly.SplitWithPlaneFast( FPlane(Actor->Location,Clipper->Vertex[i],Clipper->Vertex[j]), &Front, &Back );
					if( Split == SP_Back )
						goto Oblivion;
					else if( Split == SP_Split )
						Poly = Front;
				}
			}
			if( Poly.NumVertices > 0 )
				ActorVisibility( Actor, VisibleLeaves, NumVisibleLeaves, iOtherLeaf, &Poly );
			Oblivion:;
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Volume visibility: maximal sheet wrappings.
-----------------------------------------------------------------------------*/

//
// Clip Test to the maximal sheet wrapping of Source and Clip, and store
// the result in Result.  Returns 1 if the result is nonempty, 0 if the
// polygon was clipped to oblivion.
//
int FEditorVisibility::ClipToMaximalSheetWrapping
(
	FPoly       &Poly,
	const FPoly &A,
	const FPoly &B,
	const FLOAT Sign,
	const FLOAT Phase
)
{
	guard(FEditorVisibility::ClipToMaximalSheetWrapping);
	NumClipTests++;
	int AnyClip=0;

	// Traverse all sides of A, with vertex indices iA0, iA1.
	for( int iA1=0,iA0=A.NumVertices-1; iA1<A.NumVertices; iA0=iA1++ )
	{
		// Make vector for measuring furthest cutting point.
		FVector Side = A.Vertex[iA1]-A.Vertex[iA0];
		FLOAT SideSquared = Side.SizeSquared();
		if( SideSquared < Square(VALID_SIDE) )
			continue;

		// Find vertex of B such that the plane formed by it and the first
		// two vertices of A cleanly partitions A and B.
		INDEX iB0 = 0;
		FVector RefNormal;
		for( iB0=0; iB0<B.NumVertices; iB0++ )
		{
			// Compute normal, and don't clip if we don't have enough precision to clip properly.
			FVector Path = B.Vertex[iB0] - A.Vertex[iA0];
			FLOAT PathSquared = Path.SizeSquared();
			if( PathSquared < Square(VALID_SIDE) )
				continue;

			RefNormal = Side ^ Path;
			FLOAT NormalSquared = RefNormal.SizeSquared();
			if( NormalSquared < Square(VALID_CROSS)*SideSquared*PathSquared )
				continue;

			RefNormal *= Sign / sqrt(NormalSquared);

			// Test B split to make sure the logic is ok.
			//static const char *Sp[4]={"Plane","Front","Back ","Split"};
			FPoly BB=B; int BSplit = BB.SplitWithPlane(A.Vertex[iA0],Phase*RefNormal,NULL,NULL,1);
			if( BSplit != SP_Back )
			{
				//debugf("Visi: B=%s -- %f : %f/%f",Sp[BSplit],(B.Base-A.Base)|A.Normal,Phase,Sign);
				continue;
			}

			/*
			debugf("Range = %f < %f > %f",Best,Phase * (B.Vertex[iB0] | MyRef),Max);
			if( iB0==iMAX ) debugf("%f %f Max",Sign,Phase);
			else if( iB0==iMIN ) debugf("%f %f Min",Sign,Phase);
			else debugf("%f %f Other",Sign,Phase);
			*/

			// Clip Poly by plane.
			if( !Poly.Split( A.Vertex[iA0], RefNormal, 1 ) )
			{
				// If back, the poly was clipped to oblivion.
				return 0;
			}
			break;
		}
	}
	NumPassedClips++;
	NumUnclipped += AnyClip==0;
	return 1;
	unguard;
}

/*-----------------------------------------------------------------------------
	Bsp volume visibility.
-----------------------------------------------------------------------------*/

//
// Recursive cross-Bsp visibility.
//
void FEditorVisibility::BspCrossVisibility
(
	INDEX	iFrontPortalLeaf,
	INDEX	iBackPortalLeaf,
	INDEX	iFrontLeaf,
	INDEX	iBackLeaf,
	FPoly	&FrontPoly,
	FPoly	&ClipPoly,
	FPoly	&BackPoly,
	INT		ValidPolys,
	INT		Pass,
	INT		Tag
)
{
	guard(FEditorVisibility::BspCrossVisibility);

	// Clip FrontPoly and BackPoly to each others' visibility volumes through ClipPoly.
	FPoly ClippedFrontPoly = FrontPoly;
	FPoly ClippedBackPoly  = BackPoly;
	if
	(
		(ValidPolys < 3)
	||	(	
			ClipToMaximalSheetWrapping( ClippedBackPoly, ClipPoly, FrontPoly,+1.0, +1.0 )
		&&	ClipToMaximalSheetWrapping( ClippedBackPoly, FrontPoly,ClipPoly, +1.0, -1.0 )
		&&	ClipToMaximalSheetWrapping( ClippedFrontPoly,BackPoly, ClipPoly, -1.0, -1.0 ) 
		&&	ClipToMaximalSheetWrapping( ClippedFrontPoly,ClipPoly, BackPoly, -1.0, +1.0 )
		)
	)
	{
		// Note that front leaf sees back leaf (a precondition of this routine being called).
		Visibility->Set( iFrontLeaf, iBackLeaf, 1 );

		// Recurse down the front.
		if( Pass == 0 ) for( FPortal *NewFrontPortal=LeafPortals[iFrontLeaf]; NewFrontPortal; NewFrontPortal=NewFrontPortal->Next(iFrontLeaf) )
		{
			if( !NewFrontPortal->IsTesting )
			{
				INDEX iNewFrontLeaf = NewFrontPortal->GetNeighborLeafOf(iFrontLeaf);
				if( !Visibility->Get(iNewFrontLeaf, iFrontPortalLeaf) )
					continue;

				FPoly NewFrontPoly;
				NewFrontPortal->GetPolyFacingOutOf( iFrontLeaf, NewFrontPoly );
				if( ClipPoly.Faces( NewFrontPoly ) && BackPoly.Faces( NewFrontPoly ) )
				{
					NewFrontPortal->IsTesting++;
					BspCrossVisibility
					(
						iFrontPortalLeaf,
						iBackPortalLeaf,
						iNewFrontLeaf,
						iBackLeaf,
						NewFrontPoly,
						ClippedFrontPoly,
						ClippedBackPoly,
						ValidPolys+1,
						0,
						Tag
					);
					NewFrontPortal->IsTesting--;
				}
			}
			else NewFrontPortal->FragmentCount++;
		}

		// Recurse down the back.
		for( FPortal *NewBackPortal=LeafPortals[iBackLeaf]; NewBackPortal; NewBackPortal=NewBackPortal->Next(iBackLeaf) )
		{
			if( !NewBackPortal->IsTesting )
			{
				INDEX iNewBackLeaf = NewBackPortal->GetNeighborLeafOf(iBackLeaf);
				if( !Visibility->Get( iNewBackLeaf, iBackPortalLeaf ) )
					continue;

				FPoly NewBackPoly;
				NewBackPortal->GetPolyFacingInto( iBackLeaf, NewBackPoly );
				if( NewBackPoly.Faces( ClipPoly ) && NewBackPoly.Faces( FrontPoly ) )
				{
					NewBackPortal->IsTesting++;
					BspCrossVisibility
					(
						iFrontPortalLeaf,
						iBackPortalLeaf,
						iFrontLeaf,
						iNewBackLeaf,
						ClippedFrontPoly,
						ClippedBackPoly,
						NewBackPoly,
						ValidPolys+1,
						1,
						Tag
					);
					NewBackPortal->IsTesting--;
				}
			}
			else NewBackPortal->FragmentCount++;
		}
	}
	unguard;
}

//
// Recursive main Bsp visibility.
//
void FEditorVisibility::BspVisibility( INDEX iNode )
{
	guard(FEditorVisibility::BspVisibility);
	FBspNode &Node = Model->Nodes(iNode);
	INT FragmentCount = 0;

	// Mark this node's portals as partitioners.
	for( FPortal *ClipPortal = NodePortals[iNode]; ClipPortal; ClipPortal=ClipPortal->NodeNext )
		ClipPortal->IsTesting++;

	// Recurse, so that we can use intersubtree visibility to reject intrasubtree
	// visibility calculations.
	if( Node.iFront != INDEX_NONE ) BspVisibility( Node.iFront );
	if( Node.iBack  != INDEX_NONE ) BspVisibility( Node.iBack  );

	// Test all portals at this node.
	for( ClipPortal = NodePortals[iNode]; ClipPortal; ClipPortal=ClipPortal->NodeNext )
	{
		GApp->StatusUpdatef( "Convolving %i/%i", NumBspPortals, NumPortals, NumBspPortals, NumPortals );

		// Check visibility.
		BspCrossVisibility
		(
			ClipPortal->iFrontLeaf,
			ClipPortal->iBackLeaf,
			ClipPortal->iFrontLeaf,
			ClipPortal->iBackLeaf,
			*ClipPortal,
			*ClipPortal,
			*ClipPortal,
			1,
			0,
			iNode
		);
		NumBspPortals++;
		FragmentCount += ClipPortal->FragmentCount;
	}

	// Unmark testing.
	for( ClipPortal = NodePortals[iNode]; ClipPortal; ClipPortal=ClipPortal->NodeNext )
		ClipPortal->IsTesting--;

	//debugf("Node %i: %i fragments",iNode,FragmentCount);
	MaxFragments = Max(MaxFragments,FragmentCount);
	unguard;
}

/*-----------------------------------------------------------------------------
	Volume visibility check.
-----------------------------------------------------------------------------*/

//
// Recursively check for visibility starting at the source portal Source
// in leaf iSourceLeaf, flowing through the clip portal Clip in 
// leaf iClipLeaf, and terminating in all immediate neighbor leaves of
// ClipLeaf.
//
void FEditorVisibility::CheckVolumeVisibility
(
	const INDEX		iSourceLeaf,
	const FPoly		&Source,
	const INDEX		iTestLeaf,
	const FPoly		&Clip,
	const FPortal	*ClipPortal
)
{
	guard(FEditorVisibility::CheckVolumeVisibility);
	checkInput(iSourceLeaf != iTestLeaf);

	// Note that SourceLeaf can see TestLeaf.
	Visibility->Set(iSourceLeaf, iTestLeaf,1);

	// Check each portal in clip leaf.
	for( FPortal *TestPortal = LeafPortals[iTestLeaf]; TestPortal; TestPortal=TestPortal->Next(iTestLeaf) )
	{
		// Get TestPortal and leaf on far side of TestPortal.
		INDEX iNewTestLeaf = TestPortal->GetNeighborLeafOf(iTestLeaf);

		// Don't recurse on portals we're already testing.
		if
		(
			!TestPortal->IsTesting
		&&	iNewTestLeaf!=iSourceLeaf 
		)
		{
			// Get outward-facing Test polygon.
			FPoly Test;
			TestPortal->GetPolyFacingOutOf(iTestLeaf,Test);

			// Only test if facing.
			if( Source.Faces(Test) && Clip.Faces(Test) )
			{
				// Create ClippedSource by clipping Source to the maximal sheet wrapping
				// volume of Clip and Test.
				FPoly ClippedSource = Source;
				FPoly NewClip		= Test;
				if
				(
					ClipToMaximalSheetWrapping( ClippedSource, Clip,   Test,   +1.0, +1.0 )
				&&	ClipToMaximalSheetWrapping( ClippedSource, Test,   Clip,   +1.0, -1.0 )
				&&	ClipToMaximalSheetWrapping( NewClip,       Source, Clip,   -1.0, -1.0 ) 
				&&	ClipToMaximalSheetWrapping( NewClip,       Clip,   Source, -1.0, +1.0 )
				)
				{
					#if DEBUG_WRAPS
						ClippedSource.PolyFlags |= PF_NotSolid;
						DEBUG_Brush->Polys->AddItem(ClippedSource);

						NewClip.PolyFlags |= PF_NotSolid;
						DEBUG_Brush->Polys->AddItem(NewClip);
					#endif
					if( iNewTestLeaf != iSourceLeaf )
					{
						// Recursively check visibility.
						TestPortal->IsTesting++;
						CheckVolumeVisibility( iSourceLeaf, ClippedSource, iNewTestLeaf, NewClip, TestPortal );
						TestPortal->IsTesting--;
					}
				}
			}
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Zoning.
-----------------------------------------------------------------------------*/

//
// Form zones from the leaves.
//
void FEditorVisibility::FormZonesFromLeaves()
{
	guard(FEditorVisibility::FormZonesFromLeaves);
	FMemMark Mark(GMem);

	// Go through all portals and merge the adjoining zones.
	for( FPortal* Portal=FirstPortal; Portal; Portal=Portal->GlobalNext )
	{
		if( Portal->ZonePortalCount == 0 )
		{
			int Original = Leaves[Portal->iFrontLeaf].iZone;
			int New      = Leaves[Portal->iBackLeaf ].iZone;
			for( int i=0; i<NumLeaves; i++ )
			{
				if( Leaves[i].iZone == Original )
					Leaves[i].iZone = New;
			}
		}
	}
	
	// Renumber the leaves.
	int NumZones=0;
	for( int i=0; i<NumLeaves; i++ )
	{
		if( Leaves[i].iZone >= NumZones )
		{
			for( int j=i+1; j<NumLeaves; j++ )
				if( Leaves[j].iZone == Leaves[i].iZone )
					Leaves[j].iZone = NumZones;
			Leaves[i].iZone = NumZones++;
		}
	}
	debugf( "Found %i zones", NumZones );

	// Confine the zones to 1-63.
	for( i=0; i<NumLeaves; i++ )
		Leaves[i].iZone = (Leaves[i].iZone % 63) + 1;

	// Set official zone count.
	Model->Nodes->NumZones = Clamp(NumZones,1,64);

	Mark.Pop();
	unguard;
}

/*-----------------------------------------------------------------------------
	Assigning zone numbers.
-----------------------------------------------------------------------------*/

//
// Go through the Bsp and assign zone numbers to all nodes.  Prior to this
// function call, only leaves have zone numbers.  The zone numbers for the entire
// Bsp can be determined from leaf zone numbers.
//
void FEditorVisibility::AssignAllZones( INDEX iNode, int Outside )
{
	guard(FEditorVisibility::AssignAllZones);
	INDEX iOriginalNode = iNode;

	// Recursively assign zone numbers to children.
	if( Model->Nodes(iOriginalNode).iFront != INDEX_NONE )
		AssignAllZones( Model->Nodes(iOriginalNode).iFront, Outside || Model->Nodes(iOriginalNode).IsCsg(NF_NotVisBlocking) );
	
	if( Model->Nodes(iOriginalNode).iBack != INDEX_NONE )
		AssignAllZones( Model->Nodes(iOriginalNode).iBack, Outside && !Model->Nodes(iOriginalNode).IsCsg(NF_NotVisBlocking) );

	// Make sure this node's polygon resides in a single zone.  In other words,
	// find all of the zones belonging to outside Bsp leaves and make sure their
	// zone number is the same, and assign that zone number to this node.  Note that
	// if semisolid polygons exist in the world, polygon fragments may actually fall into
	// inside nodes, and these fragments (and their zones) can be disregarded.
	while( iNode != INDEX_NONE )
	{
		FPoly Poly;
		if( !(Model->Nodes(iNode).NodeFlags & NF_IsNew) && GUnrealEditor.bspNodeToFPoly( Model, iNode, &Poly ) )
		{
			// Make sure this node is added to the BSP properly.
			int OriginalNumNodes = Model->Nodes->Num;
			FilterThroughSubtree
			(
				0,
				iNode,
				iOriginalNode,
				Model->Nodes(iOriginalNode).iDynamic[0],
				Model->Nodes(iOriginalNode).iChild  [0],
				Poly,
				TagZonePortalFragment,
				INDEX_NONE
			);

			// See if all of all non-interior added fragments are in the same zone.
			if( Model->Nodes->Num > OriginalNumNodes )
			{
				int CanMerge=1, iZone[2]={0,0};
				for( int i=OriginalNumNodes; i<Model->Nodes->Num; i++ )
					for( int j=0; j<2; j++ )
						if( Model->Nodes(i).iZone[j] != 0 )
							iZone[j] = Model->Nodes(i).iZone[j];
				for( i=OriginalNumNodes; i<Model->Nodes->Num; i++ )
					for( int j=0; j<2; j++ )
						if( Model->Nodes(i).iZone[j]!=0 && Model->Nodes(i).iZone[j]!=iZone[j] )
							CanMerge=0;
				if( CanMerge )
				{
					// All fragments were in the same zone, so keep the original and discard the new fragments.
					for( i=OriginalNumNodes; i<Model->Nodes->Num; i++ )
						Model->Nodes(i).NumVertices = 0;
					for( i=0; i<2; i++ )
						Model->Nodes(iNode).iZone[i] = iZone[i];
				}
				else
				{
					// Keep the multi-zone fragments and remove the original plus any interior unnecessary polys.
					Model->Nodes(iNode).NumVertices = 0;
					Model->Surfs(Model->Nodes(iNode).iSurf).PolyFlags |= PF_NoMerge;
					for( i=OriginalNumNodes; i<Model->Nodes->Num; i++ )
						if( Model->Nodes(i).iZone[0]==0 && Model->Nodes(i).iZone[1]==0 )
							Model->Nodes(i).NumVertices = 0;
				}
			}
		}
		iNode = Model->Nodes(iNode).iPlane;
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
QWORD FEditorVisibility::BuildZoneMasks( INDEX iNode )
{
	guard(FEditorVisibility::BuildZoneMasks);

	FBspNode	&Node		= Model->Nodes(iNode);
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
// Called by: TestVisibility.
//
void FEditorVisibility::BuildConnectivity()
{
	guard(FEditorVisibility::BuildConnectivity);

	for( int i=0; i<64; i++ )
	{
		// Init to identity.
		Model->Nodes->Zones[i].Connectivity = ((QWORD)1)<<i;
	}
	for( i=0; i<Model->Nodes->Num; i++ )
	{
		// Process zones connected by portals.
		FBspNode &Node = Model->Nodes(i);
		FBspSurf &Surf = Model->Surfs(Node.iSurf);

		if( Surf.PolyFlags & PF_Portal )
		{
			Model->Nodes->Zones[Node.iZone[1]].Connectivity |= ((QWORD)1) << Node.iZone[0];
			Model->Nodes->Zones[Node.iZone[0]].Connectivity |= ((QWORD)1) << Node.iZone[1];
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Zone info assignment.
-----------------------------------------------------------------------------*/

//
// Attach ZoneInfo actors to the zones that they belong in.
// ZoneInfo actors are a class of actor which level designers may
// place in UnrealEd in order to specify the properties of the zone they
// reside in, such as water effects, zone name, etc.
//
void FEditorVisibility::BuildZoneInfo()
{
	guard(FEditorVisibility::BuildZoneInfo);
	int Infos=0, Duplicates=0, Zoneless=0;

	for( int i=0; i<64; i++ )
	{
		// By default, the LevelInfo (actor 0) acts as the ZoneInfo
		// for all zones which don't have individual ZoneInfo's.
		Model->Nodes->Zones[i].ZoneActor = NULL;
	}
	for( int iActor=0; iActor<Level->Num; iActor++ )
	{
		AActor *Actor = Level->Element(iActor);
		if( Actor && Actor->IsA("ZoneInfo") && !Actor->IsA("LevelInfo") )
		{
			Actor->ZoneNumber = Model->PointZone(Actor->Location);
			if( Actor->ZoneNumber == 0 )
			{
				Zoneless++;
			}
			else if( Model->Nodes->Zones[Actor->ZoneNumber].ZoneActor )
			{
				Duplicates++;
			}
			else
			{
				Infos++;
				Model->Nodes->Zones[Actor->ZoneNumber].ZoneActor = (AZoneInfo*)Actor;
			}
		}
	}
	debugf(LOG_Info,"BuildZoneInfo: %i ZoneInfo actors, %i duplicates, %i zoneless",Infos,Duplicates,Zoneless);
	unguard;
}

/*-----------------------------------------------------------------------------
	Volume visibility test.
-----------------------------------------------------------------------------*/

//
// Test visibility.
//
void FEditorVisibility::TestVisibility()
{
	guard(FEditorVisibility::TestVisibility);
	SQWORD VisTime = GApp->MicrosecondTime();
	int CountPortals=0;

	GApp->BeginSlowTask("Zoning",1,0);

	// Init Bsp info.
	for( int i=0; i<Model->Nodes->Num; i++ )
	{
		for( int j=0; j<2; j++ )
		{
			Model->Nodes(i).iDynamic[j] = INDEX_NONE;
			Model->Nodes(i).iZone   [j] = 0;
		}
	}

	// Assign leaf numbers to convex outside volumes.
	AssignLeaves( 0, Model->RootOutside );

	// Allocate leaf info.
	LeafPortals  = new(GMem, MEM_Zeroed, NumLeaves        )FPortal*;
	NodePortals  = new(GMem, MEM_Zeroed, Model->Nodes->Num)FPortal*;
	Leaves		 = new(GMem, MEM_Zeroed, NumLeaves        )FBspLeaf;
	for( i=0; i<NumLeaves; i++ )
		Leaves[i].iLogicalLeaf = Leaves[i].iZone = i;

	// Build all portals, with references to their front and back leaves.
	MakePortals( 0 );

	// Form zones.
	FormZonesFromLeaves();
	AssignAllZones( 0, Model->RootOutside );

	// Cleanup the bsp.
	GUnrealEditor.bspCleanup( Model );
	GUnrealEditor.bspRefresh( Model, 0 );
	GUnrealEditor.bspBuildBounds( Model );

	// Build zone interconnectivity info.
	BuildZoneMasks( 0 );
	BuildConnectivity();
	BuildZoneInfo();

	debugf( "Portalized: %i portals, %i zone portals (%i fragments), %i leaves, %i nodes", NumPortals, NumZonePortals, NumZoneFragments, NumLeaves, Model->Nodes->Num );

	// Test visibility of lightsources.
	INT* VisibleLeaves = new(GMem,NumLeaves)INT;
	INT NumLights[2]={0,0};
	for( int Pass=0; Pass<2; Pass++ )
	{
		for( i=0; i<Level->Num; i++ )
		{
			AActor* Actor = Level->Element(i);
			if( Actor && Actor->LightType!=LT_None && Actor->bStatic )
			{
				if( Pass == 1 )
				{
					INT NumVisibleLeaves;
					GApp->StatusUpdate( "Illumination occluding", NumLights[1], NumLights[0] );
					ActorVisibility( Actor, VisibleLeaves, NumVisibleLeaves );
					debugf( "Lightsource %s: %i leaves", Actor->GetName(), NumVisibleLeaves );
				}
				NumLights[Pass]++;
			}
		}
	}

#if 0 /* Test visibility of world */

	// Tag portals which we want to test.
	if( 0 )
	{
		// Test only interzone portals.
		NumLogicalLeaves = Max(1,Model->Nodes->NumZones);
		for( i=0; i<NumLeaves; i++ )
			Leaves[i].iLogicalLeaf = Model->Nodes->NumZones ? Leaves[i].iZone : 0;
	}
	else
	{
		// Test all leaves.
		NumLogicalLeaves = NumLeaves;
		for( i=0; i<NumLeaves; i++ )
			Leaves[i].iLogicalLeaf = i;
	}

	// Test only portals which partition logical leaves.
	for( FPortal *Portal=FirstPortal; Portal; Portal=Portal->GlobalNext )
		if( Leaves[Portal->iFrontLeaf].iLogicalLeaf != Leaves[Portal->iBackLeaf].iLogicalLeaf )
			Portal->ShouldTest++;

	// Allocate visibility.
	Visibility = new(Model->GetName(),CREATE_Replace)UBitMatrix(NumLeaves);
	for( i=0; i<NumLeaves; i++ )
		for( int j=0; i<NumLeaves; i++ )
			checkState(!Visibility->Get(i,j));

	// Each leaf can see itself.
	for( i=0; i<NumLeaves; i++ )
		Visibility->Set(i,i,1);

	// Perform Bsp-based visibility test.
	BspVisibility(0);
	checkLogic(NumBspPortals==NumPortals);

	// Merge logical leaf visibility results into physical leaves.
	UBitMatrix *NewVisibility = new("TempVisibility",CREATE_Unique)UBitMatrix(NumLogicalLeaves);
	for( i=0; i<NumLeaves; i++ )
		for( int j=0; j<NumLeaves; j++ )
			NewVisibility->Set( Leaves[i].iLogicalLeaf, Leaves[j].iLogicalLeaf, Visibility->Get(i,j) );
	for( i=0; i<NumLeaves; i++ )
		for( int j=0; j<NumLeaves; j++ )
			Visibility->Set(i,j,NewVisibility->Get(Leaves[i].iLogicalLeaf,Leaves[j].iLogicalLeaf));
	NewVisibility->Kill();

	// Check for portal test leaks.
	for( Portal=FirstPortal; Portal; Portal=Portal->GlobalNext )
		checkLogic(!Portal->IsTesting);

	// Compute stats.
	int VisiCount=0,VisiMax=0;
	for( i=0; i<NumLeaves; i++ )
	{
		int VisiLeaf = 0;
		for( int j=0; j<NumLeaves; j++ )
			VisiLeaf += Visibility->Get(i,j);
		VisiCount += VisiLeaf;
		VisiMax    = Max(VisiMax,VisiLeaf);
	}

	int Bytes=0;
	for( i=0; i<NumLeaves; i++ )
	{
		int Value=0, RunLength=0;
		for( int j=0; j<i; j++ )
		{
			if( Visibility->Get(j,i) == Value )
			{
				// No change in value.
				RunLength++;
				if( RunLength==16383 )
				{
					// Overflow, so emit 01 + RunLength[0-16384] + 00+RunLength[0]
					Bytes += 3;
					RunLength=0;
				}
			}
			else
			{
				if( j+1==i || Visibility->Get(j+1,i)!=Value )
				{
					// Permanent change in value.
					if( RunLength<=63 ) Bytes += 1; // 00+RunLength[0-63]
					else Bytes += 2; // 01+RunLength[0-16383]
					Value = !Value;
					RunLength=0;
				}
				else
				{
					// Temporary 1-bit change in value.
					if( RunLength<=63 ) Bytes += 1; // 10+RunLength[0-63]
					else Bytes += 2; // 11+RunLength[0-16383]
					RunLength=0;
				}
			}
		}
	}
	
	// Stats.
	VisTime = GApp->MicrosecondTime() - VisTime;
	debugf("Visibility: %i portals, %i leaves (%i logical), %i nodes",NumPortals,NumLeaves,NumLogicalLeaves,Model->Nodes->Num);

	debugf("Visibility: %i avg vis, %i max vis, %iK (%f%% ratio)",VisiCount/(NumLeaves+1),VisiMax,Bytes/1024,100.0*8*Bytes/(NumLeaves*(NumLeaves+1)/2));
	debugf("Visibility: %i clip tests (%i%% passed, %i%% unclipped), %i per leaf",NumClipTests,100*NumPassedClips/(NumClipTests+1),100*NumUnclipped/(NumClipTests+1),NumClipTests/(NumLogicalLeaves+1));
	debugf("Visibility: %i max fragments, %f seconds",MaxFragments,VisTime/1000000.0);
#endif

	// Cleanup Bsp info.
	for( i=0; i<Model->Nodes->Num; i++ )
	{
		Model->Nodes(i).iDynamic[0] = 0;
		Model->Nodes(i).iDynamic[1] = 0;
	}

	GApp->EndSlowTask();
	unguard;
}

/*-----------------------------------------------------------------------------
	Visibility constructor/destructor.
-----------------------------------------------------------------------------*/

//
// Constructor.
//
FEditorVisibility::FEditorVisibility( ULevel *InLevel, UModel *InModel, INT InExtra )
:	Mark			(GMem),
	Level			(InLevel),
	Model			(InModel),
	NumPortals		(0),
	NumLeaves		(0),
	NumClips		(0),
	NumClipTests	(0),
	NumPassedClips	(0),
	NumUnclipped	(0),
	NumBspPortals	(0),
	MaxFragments	(0),
	NumZonePortals	(0),
	NumZoneFragments(0),
	Extra			(InExtra),
	Leaves			(NULL),
	FirstPortal		(NULL),
	Visibility		(NULL),
	NodePortals		(NULL),
	LeafPortals		(NULL)
{
	guard(FEditorVisibility::FEditorVisibility);

#if DEBUG_PORTALS || DEBUG_WRAPS || DEBUG_BADSHEETS || DEBUG_LVS
	// Init brush for debugging.
	DEBUG_Brush=new("Brush",FIND_Existing)UModel;
	DEBUG_Brush->Polys->Num=0;
	DEBUG_Brush->Location=DEBUG_Brush->PrePivot=DEBUG_Brush->PostPivot=FVector(0,0,0);
	DEBUG_Brush->Rotation = FRotation(0,0,0);
#endif

	unguard;
}

//
// Destructor.
//
FEditorVisibility::~FEditorVisibility()
{
	guard(FEditorVisibility::FEditorVisibility);
	//!!Visibility->~FSymmetricBitArray;
	unguard;
}

/*-----------------------------------------------------------------------------
	Main function.
-----------------------------------------------------------------------------*/

//
// Perform visibility testing within the level.
//
void FGlobalEditor::TestVisibility( ULevel* Level, UModel* Model, int A, int B )
{
	guard(FGlobalEditor::TestVisibility);
	if( Model->Nodes->Num )
	{
		// Test visibility.
		FEditorVisibility Visi( Level, Model, A );
		Visi.TestVisibility();
	}
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
void UpdateBoundWithPolys( FBoundingBox &Bound, FPoly **PolyList, int nPolys )
{
	guard(UpdateBoundWithPolys);
	for( int i=0; i<nPolys; i++ )
		for( int j=0; j<PolyList[i]->NumVertices; j++ )
			Bound += PolyList[i]->Vertex[j];
	unguard;
}

//
// Update a convolution hull with a list of polys.
//
void UpdateConvolutionWithPolys( UModel *Model, INDEX iNode, FPoly **PolyList, int nPolys )
{
	guard(UpdateConvolutionWithPolys);
	FBoundingBox Box(0);

	FBspNode &Node = Model->Nodes(iNode);
	Node.iCollisionBound = Model->LeafHulls->Num;
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
			Box += PolyList[i]->Vertex[j];
	}
	Model->LeafHulls->AddItem(INDEX_NONE);

	// Add bounds.
	Model->LeafHulls->AddItem( *(INT*)&Box.Min.X );
	Model->LeafHulls->AddItem( *(INT*)&Box.Min.Y );
	Model->LeafHulls->AddItem( *(INT*)&Box.Min.Z );
	Model->LeafHulls->AddItem( *(INT*)&Box.Max.X );
	Model->LeafHulls->AddItem( *(INT*)&Box.Max.Y );
	Model->LeafHulls->AddItem( *(INT*)&Box.Max.Z );

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
				// May occasionally happen.
				debugf("FilterBound: Got inficoplanar");
				break;
			
			case SP_Front:
				// Shouldn't happen if hull is correct.
				debugf("FilterBound: Got infifront");
				return;
			
			case SP_Split:
				InfiniteEdPoly = BackPoly;
				break;
			
			case SP_Back:
				break;
		}

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
	FBoundingBox	*ParentBound,
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
	FBoundingBox	Bound;

	Bound.Min.X = Bound.Min.Y = Bound.Min.Z = +65536.0;
	Bound.Max.X = Bound.Max.Y = Bound.Max.Z = -65536.0;

	// Split bound into front half and back half.
	FPoly **FrontList = new(GMem,nPolys*2+16)FPoly*; int nFront=0;
	FPoly **BackList  = new(GMem,nPolys*2+16)FPoly*; int nBack=0;

	FPoly *FrontPoly  = new(GMem)FPoly;
	FPoly *BackPoly   = new(GMem)FPoly;

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
	}
	if( nFront && nBack )
	{
		// Add partitioner plane to front and back.
		FPoly InfiniteEdPoly = BuildInfiniteFPoly( Model, iNode );
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
		{
			FilterBound( Model, &Bound, Node.iFront, FrontList, nFront, Outside || Node.IsCsg() );
		}
		else
		{
			if( Outside || Node.IsCsg() )
				UpdateBoundWithPolys( Bound, FrontList, nFront );
			else
				UpdateConvolutionWithPolys( Model, iNode, FrontList, nFront );
		}
	}
	if( nBack > 0 )
	{
		if( Node.iBack != INDEX_NONE)
		{
			FilterBound( Model, &Bound,Node.iBack, BackList, nBack, Outside && !Node.IsCsg() );
		}
		else
		{
			if( Outside && !Node.IsCsg() )
				UpdateBoundWithPolys( Bound, BackList, nBack );
			else
				UpdateConvolutionWithPolys( Model, iNode, BackList, nBack );
		}
	}

	// Apply this bound to this node.
	if( Node.iRenderBound == INDEX_NONE )
	{
		Node.iRenderBound = Model->Bounds->Add();
		Model->Bounds(Node.iRenderBound) = Bound;
	}

	// Update parent bound to enclose this bound.
	if( ParentBound )
		*ParentBound += Bound.Min;

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
void FGlobalEditor::bspBuildBounds( UModel *Model )
{
	guard(FGlobalEditor::bspBuildBounds);

	if( Model->Nodes->Num==0 )
		return;

#if DEBUG_HULLS
	DEBUG_Brush=new("Brush",FIND_Existing)UModel;
	DEBUG_Brush->Polys->Num=0;
	DEBUG_Brush->Location=DEBUG_Brush->PrePivot=DEBUG_Brush->PostPivot=FVector(0,0,0);
	DEBUG_Brush->Rotation=FRotation(0,0,0);
#endif

	FPoly *PolyList[6];
	GGfx.RootHullBrush->Lock(LOCK_Read);
	checkState(GGfx.RootHullBrush->Polys->Num==6);
	for( int i=0; i<6; i++ )
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
		Model->Nodes(i).iRenderBound     = INDEX_NONE;
		Model->Nodes(i).iCollisionBound  = INDEX_NONE;
	}

	FilterBound( Model, NULL, 0, PolyList, 6, Model->RootOutside );
	Model->Bounds->Shrink();

	debugf( "bspBuildBounds: Generated %i bounds, %i hulls", Model->Bounds->Num, Model->LeafHulls->Num );
	unguard;
}

/*-----------------------------------------------------------------------------
	Non-class functions.
-----------------------------------------------------------------------------*/

//
// Build an FPoly representing an "infinite" plane (which exceeds the maximum
// dimensions of the world in all directions) for a particular Bsp node.
//
FPoly BuildInfiniteFPoly( UModel *Model, INDEX iNode )
{
	guard(BuildInfiniteFPoly);

	FBspNode &Node   = Model->Nodes  (iNode       );
	FBspSurf &Poly   = Model->Surfs	 (Node.iSurf  );
	FVector  &Base   = Model->Points (Poly.pBase  );
	FVector  &Normal = Model->Vectors(Poly.vNormal);
	FVector	 Axis1,Axis2;

	// Find two non-problematic axis vectors.
	Normal.FindBestAxisVectors( Axis1, Axis2 );

	// Set up the FPoly.
	FPoly EdPoly;
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

	return EdPoly;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
