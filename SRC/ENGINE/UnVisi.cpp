/*=============================================================================
	UnVisi.cpp: Unreal visibility computation

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	Experimental visibilty code.

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
void BuildInfiniteFPoly( UModel *Model, INDEX iNode, FPoly &EdPoly );

// Thresholds.
#define VALID_SIDE  0.1   /* A normal must be at laest this long to be valid */
#define VALID_CROSS 0.001 /* A cross product can be safely normalized if this big */

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

// Debugging.
#if DEBUG_PORTALS || DEBUG_WRAPS || DEBUG_BADSHEETS
	static UModel *DEBUG_Brush;
#endif

// A portal.
class FPortal : public FPoly
{
public:
	// Variables.
	INDEX	iFrontLeaf, iBackLeaf;
	FPortal *GlobalNext, *FrontLeafNext, *BackLeafNext, *NodeNext;
	BYTE	IsTesting, ShouldTest, IsPartitioner;
	INT		FragmentCount;

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
		IsPartitioner	(0),
		FragmentCount	(0)
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

// An edge belonging to a leaf.
class FLeafEdge
{
public:
	// Variables.
	INDEX     iEdge;
	FLeafEdge *Next;

	// Constructor.
	FLeafEdge( INDEX iInEdge, FLeafEdge *InNext )
	:	iEdge	(iInEdge),
		Next	(InNext)
	{}
};

// The visibility calculator class.
class FEditorVisibility
{
public:
	// Constants.
	enum {MAX_CLIPS=16384};
	enum {CLIP_BACK_FLAG=0x40000000};

	// Variables.
	FMemAutoMark		Mark;
	UModel				*Model;
	INDEX				Clips[MAX_CLIPS];
	INT					NumPortals, NumLeaves, NumLogicalLeaves, NumLeafEdges;
	INT					NumClips, NumClipTests, NumPassedClips, NumUnclipped;
	INT					NumBspPortals, MaxFragments;
	INT					Extra;
	FBspLeaf			*Leaves;
	FPortal				*FirstPortal;
	UBitMatrix			*Visibility;
	FPortal				**NodePortals;
	FPortal				**LeafPortals;
	FLeafEdge			**LeafEdges;

	// Constructor.
	FEditorVisibility(UModel *InModel, INT InDebug);

	// Destructor.
	~FEditorVisibility();

	// Functions.
	void AddPortal( FPoly &Poly, INDEX iFrontLeaf, INDEX iBackLeaf, INDEX iGeneratingNode );
	void MakePortalsFront( INDEX iBackLeaf, INDEX iParentLeaf, INDEX iNode, INDEX iGeneratingNode, FPoly Poly, INT Outside );
	void MakePortalsBack( INDEX iGeneratingNode, INT GeneratingNodeOutside, INDEX iParentLeaf, INDEX iNode, FPoly Poly, INT Outside );
	void MakePortalsClip( INDEX iNode, FPoly Poly, INT Clip, INT Outside );
	void MakePortals( INDEX iNode, INT Outside );
	void AssignLeaves( INDEX iNode, INT Outside );
	int ClipToMaximalSheetWrapping( FPoly &Poly, const FPoly &A, const FPoly &B, const FLOAT Sign, const FLOAT Phase );
	void CheckVisibility( const INDEX iSourceLeaf, const FPoly &Source, const INDEX iTestLeaf, const FPoly &Clip, const FPortal *ClipPortal );
	void FilterLeafEdge( INDEX iReferenceNode, INDEX iParentNode, INDEX iNode, INT IsFront, FPoly &Poly );
	void BuildLeafEdges( INDEX iNode );
	void TestVisibility();

	// Experimental.
	void BspCrossVisibility( INDEX iFronyPortalLeaf, INDEX iBackPortalLeaf, INDEX iFrontLeaf, INDEX iBackLeaf, FPoly &FrontPoly, FPoly &ClipPoly, FPoly &BackPoly, INT ValidPolys, INT Pass, INT Tag );
	void BspVisibility( INDEX iNode );
};

/*-----------------------------------------------------------------------------
	Portal building, a simple recursive hierarchy of functions.
-----------------------------------------------------------------------------*/

// Add a portal to the portal list.
// Called by: MakePortalsBack.
void FEditorVisibility::AddPortal
(
	FPoly	&Poly,
	INDEX	iFrontLeaf,
	INDEX	iBackLeaf,
	INDEX	iGeneratingNode
)
{
	guard(FEditorVisibility::AddPortal);
	checkState(iFrontLeaf!=INDEX_NONE);
	checkState(iBackLeaf!=INDEX_NONE);
	checkState(iFrontLeaf<NumLeaves);
	checkState(iBackLeaf<NumLeaves);
	checkState(iFrontLeaf!=iBackLeaf);

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

	unguard;
}

// Filter a portal through the front half of a Bsp subtree.
// Called by: MakePortalsFront.
// Calls:     AddPortal.
void FEditorVisibility::MakePortalsFront
(
	INDEX	iBackLeaf,
	INDEX	iParentLeaf,
	INDEX	iNode,
	INDEX	iGeneratingNode,
	FPoly	Poly,
	INT		Outside 
)
{
	guard(FEditorVisibility::MakePortalsFront);
	while( iNode != INDEX_NONE )
	{
		// Test split.
		FPoly Front,Back;
		int Split = Poly.SplitWithNode(Model,iNode,&Front,&Back,1);

		// Recurse with front.
		if( Split==SP_Front || Split==SP_Split )
			MakePortalsFront
			(
				iBackLeaf,
				Model->Nodes(iNode).iDynamic[1],
				Model->Nodes(iNode).iFront,
				iGeneratingNode,
				Split==SP_Front ? Poly : Front,
				Outside || Model->Nodes(iNode).IsCsg()
			);

		// Consider back.
		if( Split!=SP_Back && Split!=SP_Split )
			return;

		// Loop with back.
		if( Split == SP_Split ) Poly = Back;
		iParentLeaf = Model->Nodes(iNode).iDynamic[0];
		Outside     = Outside && !Model->Nodes(iNode).IsCsg();
		iNode       = Model->Nodes(iNode).iBack;
	}
	if( Outside )
	{
		// We reached a leaf in the front subtree, so Poly is a valid convex volume portal.
		AddPortal(Poly, iParentLeaf, iBackLeaf, iGeneratingNode );
	}
	unguard;
}

// Filter a portal through the back half of a Bsp subtree.
// Called by: MakePortalsClip.
// Calls:     MakePortalsFront.
void FEditorVisibility::MakePortalsBack
(
	INDEX	iGeneratingNode,
	INT		GeneratingNodeOutside,
	INDEX	iParentLeaf,
	INDEX	iNode,
	FPoly	Poly,
	INT		Outside
)
{
	guard(FEditorVisibility::MakePortalsBack);
	while( iNode != INDEX_NONE )
	{
		// Test split.
		FPoly Front,Back;
		int Split = Poly.SplitWithNode(Model,iNode,&Front,&Back,1);

		// Recurse with front.
		if( Split==SP_Front || Split==SP_Split )
			MakePortalsBack
			(
				iGeneratingNode,
				GeneratingNodeOutside,
				Model->Nodes(iNode).iDynamic[1],
				Model->Nodes(iNode).iFront,
				Split==SP_Front ? Poly : Front,
				Outside || Model->Nodes(iNode).IsCsg()
			);

		// Consider back.
		if( Split!=SP_Back && Split!=SP_Split )
			return;

		// Loop with back.		
		if( Split == SP_Split ) Poly = Back;
		iParentLeaf = Model->Nodes(iNode).iDynamic[0];
		Outside     = Outside && !Model->Nodes(iNode).IsCsg();
		iNode       = Model->Nodes(iNode).iBack;
	}
	if( Outside )
	{
		// We reached a leaf in the back subtree, so recurse with the original front.
		MakePortalsFront
		(
			iParentLeaf,
			Model->Nodes(iGeneratingNode).iDynamic[1],
			Model->Nodes(iGeneratingNode).iFront,
			iGeneratingNode,
			Poly,
			GeneratingNodeOutside || Model->Nodes(iGeneratingNode).IsCsg()
		);
	}
	unguard;
}

// Clip a portal by all parent nodes above it.
// Called by: MakePortals.
// Calls:     MakePortalsFront.
void FEditorVisibility::MakePortalsClip
(
	INDEX	iNode,
	FPoly	Poly,
	INT		Clip,
	INT		Outside
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
			MakePortalsClip( iNode, TempPoly, Clip, Outside );
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
	MakePortalsBack
	(
		iNode,
		Outside,
		Model->Nodes(iNode).iDynamic[0],
		Model->Nodes(iNode).iBack,
		Poly,
		Outside && !Model->Nodes(iNode).IsCsg()
	);

	unguard;
}

// Make all portals.
// Called by: TestVisibility.
// Calls:     MakePortalsClip.
void FEditorVisibility::MakePortals
(
	INDEX	iNode,
	INT		Outside
)
{
	guard(FEditorVisibility::MakePortals);
	FPoly Poly;

	// Make an infinite edpoly for this node.
	BuildInfiniteFPoly( Model, iNode, Poly );

	// Filter the portal through this subtree.
	MakePortalsClip( iNode, Poly, 0, Outside );

	// Make portals for front.
	if( Model->Nodes(iNode).iFront != INDEX_NONE )
	{
		Clips[NumClips++] = iNode;
		MakePortals( Model->Nodes(iNode).iFront, Outside || Model->Nodes(iNode).IsCsg() );
		NumClips--;
	}

	// Make portals for back.
	if( Model->Nodes(iNode).iBack != INDEX_NONE )
	{
		Clips[NumClips++] = iNode | CLIP_BACK_FLAG;
		MakePortals( Model->Nodes(iNode).iBack, Outside && !Model->Nodes(iNode).IsCsg() );
		NumClips--;
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Assign leaves.
-----------------------------------------------------------------------------*/

void FEditorVisibility::AssignLeaves( INDEX iNode, INT Outside )
{
	guard(FEditorVisibility::AssignLeaves);
	FBspNode &Node = Model->Nodes(iNode);

	// Process front.
	if( Node.iFront != INDEX_NONE )		AssignLeaves( Node.iFront, Outside || Node.IsCsg() );
	else if( Outside || Node.IsCsg() )	Node.iDynamic[1] = NumLeaves++;

	// Process back.
	if( Node.iBack != INDEX_NONE )		AssignLeaves( Node.iBack, Outside && !Node.IsCsg() );
	else if( Outside && !Node.IsCsg() )	Node.iDynamic[0] = NumLeaves++;

	unguard;
}

/*-----------------------------------------------------------------------------
	Maximal sheet wrapping.
-----------------------------------------------------------------------------*/

// Clip Test to the maximal sheet wrapping of Source and Clip, and store
// the result in Result.  Returns 1 if the result is nonempty, 0 if the
// polygon was clipped to oblivion.
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
#if 0
		INDEX iMIN,iMAX;
		INDEX Found = 0;
		FLOAT Best, Max;
		for( int i=0; i<B.NumVertices; i++ )
		{
			if( !Found )
			{
				FVector Path = B.Vertex[i] - A.Vertex[iA0];
				FLOAT PathSquared = Path.SizeSquared();
				if( PathSquared < Square(VALID_SIDE) )
					continue;

				RefNormal = Side ^ (B.Vertex[i]-A.Vertex[iA0]);
				FLOAT NormalSquared = RefNormal.SizeSquared();
				if( NormalSquared < Square(VALID_CROSS)*SideSquared*PathSquared )
					continue;

				RefNormal *= Sign / sqrt(NormalSquared);
			}
			FLOAT Dot = Phase * (B.Vertex[i] | RefNormal);
			if( !Found || Dot < Best )
			{
				iMIN = i;
				iB0  = i;
				Best = Dot;
			}
			if( !Found || Dot>Max )
			{
				iMAX = i;
				Max  = Dot;
			}
			Found = 1;
		}
		if( !Found ) continue;
		FVector MyRef = RefNormal;
#endif

		for( iB0=0; iB0<B.NumVertices; iB0++ )
		{
			// Compute normal, and don't clip if we don't have enough
			// precision to clip properly.
			FVector Path = B.Vertex[iB0] - A.Vertex[iA0];
			FLOAT PathSquared = Path.SizeSquared();
			if( PathSquared < Square(VALID_SIDE) )
				continue;

			RefNormal = Side ^ Path;
			FLOAT NormalSquared = RefNormal.SizeSquared();
			if( NormalSquared < Square(VALID_CROSS)*SideSquared*PathSquared )
				continue;

			RefNormal *= Sign / sqrt(NormalSquared);

#if 1
			// Test B split to make sure the logic is ok.
			//static const char *Sp[4]={"Plane","Front","Back ","Split"};
			FPoly BB=B; int BSplit = BB.SplitWithPlane(A.Vertex[iA0],Phase*RefNormal,NULL,NULL,1);
			if( BSplit != SP_Back )
			{
				//debugf("Visi: B=%s -- %f : %f/%f",Sp[BSplit],(B.Base-A.Base)|A.Normal,Phase,Sign);
				continue;
			}
#endif
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
	Bsp visibility.
-----------------------------------------------------------------------------*/

// Recursive cross-Bsp visibility.
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
				NewFrontPortal->GetPolyFacingOutOf(iFrontLeaf,NewFrontPoly);
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
			else if( NewFrontPortal->IsPartitioner )
			{
				NewFrontPortal->FragmentCount++;
			}
		}

		// Recurse down the back.
		for( FPortal *NewBackPortal=LeafPortals[iBackLeaf]; NewBackPortal; NewBackPortal=NewBackPortal->Next(iBackLeaf) )
		{
			if( !NewBackPortal->IsTesting )
			{
				INDEX iNewBackLeaf = NewBackPortal->GetNeighborLeafOf(iBackLeaf);
				if( !Visibility->Get(iNewBackLeaf, iBackPortalLeaf) )
					continue;

				FPoly NewBackPoly;
				NewBackPortal->GetPolyFacingInto(iBackLeaf,NewBackPoly);
				if( NewBackPoly.Faces( ClipPoly ) && NewBackPoly.Faces(FrontPoly) )
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
			else if( NewBackPortal->IsPartitioner )
			{
				NewBackPortal->FragmentCount++;
			}
		}
	}
	unguard;
}

// Recursive main Bsp visibility.
void FEditorVisibility::BspVisibility( INDEX iNode )
{
	guard(FEditorVisibility::BspVisibility);
	FBspNode &Node = Model->Nodes(iNode);
	INT FragmentCount = 0;

	// Mark this node's portals as partitioners.
	for( FPortal *ClipPortal = NodePortals[iNode]; ClipPortal; ClipPortal=ClipPortal->NodeNext )
	{
		ClipPortal->IsPartitioner++;
		ClipPortal->IsTesting++;
	}

	// Recurse, so that we can use intersubtree visibility to reject intrasubtree
	// visibility calculations.
	if( Node.iFront != INDEX_NONE ) BspVisibility( Node.iFront );
	if( Node.iBack  != INDEX_NONE ) BspVisibility( Node.iBack  );

	// Unmark partitoners.
	for( ClipPortal = NodePortals[iNode]; ClipPortal; ClipPortal=ClipPortal->NodeNext )
		ClipPortal->IsPartitioner--;

	// Test all portals at this node.
	for( ClipPortal = NodePortals[iNode]; ClipPortal; ClipPortal=ClipPortal->NodeNext )
	{
		GApp->StatusUpdatef("Convolving %i/%i",NumBspPortals,NumPortals,NumBspPortals,NumPortals);

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
		ClipPortal->IsTesting--;
	}
	//debugf("Node %i: %i fragments",iNode,FragmentCount);
	MaxFragments = Max(MaxFragments,FragmentCount);
	unguard;
}

/*-----------------------------------------------------------------------------
	Visibility check.
-----------------------------------------------------------------------------*/

// Recursively check for visibility starting at the source portal Source
// in leaf iSourceLeaf, flowing through the clip portal Clip in 
// leaf iClipLeaf, and terminating in all immediate neighbor leaves of
// ClipLeaf.
void FEditorVisibility::CheckVisibility
(
	const INDEX		iSourceLeaf,
	const FPoly		&Source,
	const INDEX		iTestLeaf,
	const FPoly		&Clip,
	const FPortal	*ClipPortal
)
{
	guard(FEditorVisibility::CheckVisibility);
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
						CheckVisibility( iSourceLeaf, ClippedSource, iNewTestLeaf, NewClip, TestPortal );
						TestPortal->IsTesting--;
					}
				}
			}
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Building leaf/edge associations.
-----------------------------------------------------------------------------*/

// Filter a polygon down the Bsp and reference its node in all leaves the
// polygon falls into. This is a bit inefficient, as it doesn't bother
// splitting edges during the filtering process.
void FEditorVisibility::FilterLeafEdge
(
	INDEX	iReferenceNode,
	INDEX	iParentNode,
	INDEX	iNode,
	INT		IsFront,
	FPoly	&Poly
)
{
	guard(FEditorVisibility::FilterLeafEdge);
	if( iNode != INDEX_NONE )
	{
		// Split the poly by the node and recurse.
		FBspNode &Node = Model->Nodes(iNode);
		FPoly Front, Back;
		switch( Poly.SplitWithNode( Model, iNode, &Front, &Back, 0 ) )
		{
			case SP_Coplanar:
				debugf("Coplanar leaf edge");
				break;
			case SP_Front:
				FilterLeafEdge( iReferenceNode, iNode, Node.iFront, 1, Poly );
				break;
			case SP_Back:
				FilterLeafEdge( iReferenceNode, iNode, Node.iBack,  0, Poly );
				break;
			case SP_Split:
				FilterLeafEdge( iReferenceNode, iNode, Node.iFront, 1, Front );
				FilterLeafEdge( iReferenceNode, iNode, Node.iBack,  0, Back  );
				break;
		}
	}
	else
	{
		FBspNode &Node = Model->Nodes(iParentNode);
		INDEX    iLeaf = Node.iDynamic[IsFront];
		if( iLeaf != INDEX_NONE )
		{
			// Add all of ReferenceNode's unique edges to leaf edge list.
			FBspNode &Reference = Model->Nodes(iReferenceNode);
			for( int i=0; i<Reference.NumVertices; i++ )
			{
				// Get edge.
				FVert &Vert = Model->Verts(i + Reference.iVertPool);
				checkState(Vert.iSide != INDEX_NONE);

				// See if this edge is already on the list.
				for( FLeafEdge *LeafEdge = LeafEdges[iLeaf]; LeafEdge; LeafEdge=LeafEdge->Next )
					if( LeafEdge->iEdge == Vert.iSide )
						break;
				
				// Add to leaf edge list if it's not already on the list.
				if( LeafEdge == NULL )
				{
					LeafEdges[iLeaf] = new(GMem)FLeafEdge( Vert.iSide, LeafEdges[iLeaf] );
					NumLeafEdges++;
				}
			}
		}
		else
		{
			// A polygon fragment landed outside a leaf.
			// This happens normally with semisolids in the current implementation.
			// When semisolid support is removed, this indicates a real problem.
			//debugf("Detected false leaf");
		}
	}
	unguard;
}

// Traverse all Bsp node polygons and generate their leaf edge references.
void FEditorVisibility::BuildLeafEdges( INDEX iNode )
{
	guard(FEditorVisibility::BuildLeafEdges);

	// Save this node.
	INDEX    iParent = iNode;
	FBspNode &Parent = Model->Nodes(iParent);

	// Process all coplanars.
	while( iNode != INDEX_NONE )
	{
		// Handle the Bsp node polygon at this node.
		FPoly Poly;
		if( GUnrealEditor.bspNodeToFPoly( Model, iNode, &Poly ) >= 3 )
		{
			// Handle node poly being flipped relative to parent.
			INDEX iFront, iBack, IsFlipped;
			if( (Parent.Plane | Model->Nodes(iNode).Plane) >= 0.0 )
			{
				iFront    = Parent.iFront;
				iBack     = Parent.iBack;
				IsFlipped = 0;
			}
			else
			{
				iFront    = Parent.iBack;
				iBack     = Parent.iFront;
				IsFlipped = 1;
			}

			// Filter down front.
			FilterLeafEdge( iNode, iParent, iFront, 1 ^ IsFlipped, Poly );

			// Filter down back if two-sided.
			if( Poly.PolyFlags & PF_TwoSided )
				FilterLeafEdge( iNode, iParent, iBack, 0 ^ IsFlipped, Poly );
		}
		// Go to coplanar.
		iNode = Model->Nodes(iNode).iPlane;
	}
	// Recurse.
	if( Parent.iFront != INDEX_NONE ) BuildLeafEdges( Parent.iFront );
	if( Parent.iBack  != INDEX_NONE ) BuildLeafEdges( Parent.iBack  );
	unguard;
}

/*-----------------------------------------------------------------------------
	Visibility test.
-----------------------------------------------------------------------------*/

// Test visibility.
void FEditorVisibility::TestVisibility()
{
	guard(FEditorVisibility::TestVisibility);
	SQWORD VisTime = GApp->MicrosecondTime();
	int TestedPortals=0, CountPortals=0;

	GApp->BeginSlowTask("Testing visibility",1,0);
	debugf("Testing visibility");

	// Init Bsp info.
	for( int i=0; i<Model->Nodes->Num; i++ )
	{
		Model->Nodes(i).iDynamic[0] = INDEX_NONE;
		Model->Nodes(i).iDynamic[1] = INDEX_NONE;
	}

	// Assign leaf numbers to convex outside volumes.
	AssignLeaves( 0, Model->RootOutside );

	// Allocate leaf info.
	LeafPortals  = new(GMem, MEM_Zeroed, NumLeaves)FPortal*;
	NodePortals  = new(GMem, MEM_Zeroed, Model->Nodes->Num)FPortal*;
	LeafEdges    = new(GMem, MEM_Zeroed, NumLeaves)FLeafEdge*;
	Leaves		 = new(GMem, MEM_Zeroed, NumLeaves)FBspLeaf;

	// Set leaf zones.
	for( i=0; i<Model->Nodes->Num; i++ )
		for( int j=0; j<2; j++ )
			if( Model->Nodes(i).iDynamic[j] != INDEX_NONE )
				Leaves[Model->Nodes(i).iDynamic[j]].iZone = Model->Nodes(i).iZone[j];

	// Build all portals, with references to their front and back leaves.
	MakePortals( 0, Model->RootOutside );

	// Tag portals which we want to test.
	if( 1 )
	{
		// Test all portals.
		NumLogicalLeaves = NumLeaves;
		for( i=0; i<NumLeaves; i++ )
			Leaves[i].iLogicalLeaf = i;
	}
	else if( 0 )
	{
		// Test only interzone portals.
		NumLogicalLeaves = Max(1,Model->Nodes->NumZones);
		for( i=0; i<NumLeaves; i++ )
			Leaves[i].iLogicalLeaf = Model->Nodes->NumZones ? Leaves[i].iZone : 0;
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
	debugf("Visibility: %i portals (%i sources), %i leaves (%i logical), %i nodes",NumPortals,TestedPortals,NumLeaves,NumLogicalLeaves,Model->Nodes->Num);
	debugf("Visibility: %i avg vis, %i max vis, %iK (%f%% ratio)",VisiCount/(NumLeaves+1),VisiMax,Bytes/1024,100.0*8*Bytes/(NumLeaves*(NumLeaves+1)/2));
	debugf("Visibility: %i clip tests (%i%% passed, %i%% unclipped), %i per leaf",NumClipTests,100*NumPassedClips/(NumClipTests+1),100*NumUnclipped/(NumClipTests+1),NumClipTests/(NumLogicalLeaves+1));
	debugf("Visibility: %i max fragments, %f seconds",MaxFragments,VisTime/1000000.0);

	///////////////////////////////////
	// Temporary edge rendering hack //
	///////////////////////////////////

	// Init edges.
	/*
	enum{NUMHACKS=100000};
	GEdgeHack.Edges = new FBspEdge[NUMHACKS];
	for( i=0; i<NUMHACKS; i++ )
	{
		FBspEdge &Edge = GEdgeHack.Edges[i];
		Edge.iSurf  [0]=Edge.iSurf  [1]=INDEX_NONE;
		Edge.pVertex[0]=Edge.pVertex[1]=INDEX_NONE;
		Edge.iFrame = 0;
	}
	*/

	// Build list of all edges.
	/*
	for( i=0; i<Model->Nodes->Num; i++ )
	{
		FBspNode&  Node =  Model->Nodes(i);
		FVert* VertPool = &Model->Verts(Node.iVertPool);

		for( int j=0; j<Node.NumVertices; j++ )
		{
			if( VertPool[j].iSide == INDEX_NONE )
				VertPool[j].iSide = Model->Verts->NumSharedSides++;

			FBspEdge& EdgeHack = GEdgeHack.Edges[VertPool[j].iSide];
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
				EdgeHack.iSurf[1] = Node.iSurf;
			}
		}
	}*/

	// Condense edges into non-sparse list.
	// No longer sensible in the current context, because intrasurfaces edges
	// are relevant for rendering iff they are interleaf.
	/*
	int Identical=0;
	for( i=0; i<NUMHACKS; i++ )
	{
		if (GEdgeHack.Edges[i].iSurf[0] != INDEX_NONE)
		{
			if( (GEdgeHack.Edges[i].iSurf[0]   != GEdgeHack.Edges[i].iSurf  [1])
			&&	(GEdgeHack.Edges[i].pVertex[0] != GEdgeHack.Edges[i].pVertex[1]))
			{
				// Condense.
				GEdgeHack.Edges[GEdgeHack.NumEdges++] = GEdgeHack.Edges[i];
			}
			else
			{
				Identical++;
			}
		}
	}
	debugf("Hacked %i edges (%i removed)",GEdgeHack.NumEdges,Identical);
	*/

	// Build leaf edges.
	BuildLeafEdges( 0 );

	// Allocate edge pool.
	/*
	GEdgeHack.EdgePool = new INT[NumLeafEdges];
	*/

	// Condense temporary LeafEdges into global leaf edges.
	/*
	GEdgeHack.Visibility = Visibility;
	GEdgeHack.NumLeaves  = NumLeaves;
	GEdgeHack.Leaves     = new FBspLeaf[NumLeaves];
	memcpy(GEdgeHack.Leaves,Leaves,NumLeaves*sizeof(FBspLeaf));
	int iEdge            = 0;
	for( i=0; i<NumLeaves; i++ )
	{
		FBspLeaf &Leaf     = GEdgeHack.Leaves[i];
		Leaf			   = Leaves[i];
		Leaf.iLeafEdgePool = iEdge;
		Leaf.NumLeafEdges  = 0;

		// Add all leaf edges to permanent list.
		for( FLeafEdge *LeafEdge=LeafEdges[i]; LeafEdge; LeafEdge=LeafEdge->Next )
		{
			GEdgeHack.EdgePool[iEdge++] = LeafEdge->iEdge;
			Leaf.NumLeafEdges++;
		}
	}
	checkLogic(iEdge == NumLeafEdges);
	*/

	// Edge stats.
	debugf("Visibility: %i edges, %i leaf edges",Model->Verts->NumSharedSides,NumLeafEdges);

	//////////////
	// End hack //
	//////////////

	// Disable zone rendering since we're appropriating their ZoneMask.
	Model->Nodes->NumZones = 0;

	// Cleanup Bsp info.
	for( i=0; i<Model->Nodes->Num; i++ )
	{
		Model->Nodes(i).ZoneMask = 
			((QWORD)(DWORD)Model->Nodes(i).iDynamic[0] <<  0) +
			((QWORD)(DWORD)Model->Nodes(i).iDynamic[1] << 32);
		Model->Nodes(i).iDynamic[0] = INDEX_NONE;
		Model->Nodes(i).iDynamic[1] = INDEX_NONE;
	}
	for( i=0; i<Model->Surfs->Num; i++ )
	{
		Model->Surfs(i).PolyFlags &= ~PF_Selected;
	}

	GApp->EndSlowTask();
	unguard;
}

/*-----------------------------------------------------------------------------
	Visibility constructor/destructor.
-----------------------------------------------------------------------------*/

// Constructor.
FEditorVisibility::FEditorVisibility( UModel *InModel, INT InExtra )
:	Mark			(GMem),
	Model			(InModel),
	NumPortals		(0),
	NumLeaves		(0),
	NumLeafEdges	(0),
	NumClips		(0),
	NumClipTests	(0),
	NumPassedClips	(0),
	NumUnclipped	(0),
	NumBspPortals	(0),
	MaxFragments	(0),
	Extra			(InExtra),
	Leaves			(NULL),
	FirstPortal		(NULL),
	Visibility		(NULL),
	NodePortals		(NULL),
	LeafPortals		(NULL),
	LeafEdges		(NULL)
{
	guard(FEditorVisibility::FEditorVisibility);

#if DEBUG_PORTALS || DEBUG_WRAPS || DEBUG_BADSHEETS
	// Init brush for debugging.
	DEBUG_Brush=new("Brush",FIND_Existing)UModel;
	DEBUG_Brush->Polys->Num=0;
	DEBUG_Brush->Location=DEBUG_Brush->PrePivot=DEBUG_Brush->PostPivot=GMath.ZeroVector;
	DEBUG_Brush->Rotation=GMath.ZeroRotation;
#endif

	unguard;
}

// Destructor.
FEditorVisibility::~FEditorVisibility()
{
	guard(FEditorVisibility::FEditorVisibility);
	//!!Visibility->~FSymmetricBitArray;
	unguard;
}

/*-----------------------------------------------------------------------------
	Main function.
-----------------------------------------------------------------------------*/

// Perform visibility testing within the level.
void FGlobalEditor::TestVisibility( UModel *Model, int A, int B )
{
	guard(FGlobalEditor::TestVisibility);
	if( Model->Nodes->Num )
	{
		// Test visibility.
		FEditorVisibility Visi(Model,A);
		Visi.TestVisibility();
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
