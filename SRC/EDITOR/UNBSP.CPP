/*=============================================================================
	UnBsp.cpp: Unreal engine Bsp-related functions.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*---------------------------------------------------------------------------------------
	Globals.
---------------------------------------------------------------------------------------*/

// Magic numbers.
#define THRESH_OPTGEOM_COPLANAR			(0.25)		/* Threshold for Bsp geometry optimization */
#define THRESH_OPTGEOM_COSIDAL			(0.25)		/* Threshold for Bsp geometry optimization */

//
// Status of filtered polygons:
//
enum EPolyNodeFilter
{
   F_OUTSIDE				= 0, // Leaf is an exterior leaf (visible to viewers).
   F_INSIDE					= 1, // Leaf is an interior leaf (non-visible, hidden behind backface).
   F_COPLANAR_OUTSIDE		= 2, // Poly is coplanar and in the exterior (visible to viewers).
   F_COPLANAR_INSIDE		= 3, // Poly is coplanar and inside (invisible to viewers).
   F_COSPATIAL_FACING_IN	= 4, // Poly is coplanar, cospatial, and facing in.
   F_COSPATIAL_FACING_OUT	= 5, // Poly is coplanar, cospatial, and facing out.
};

//
// Generic filter function called by BspFilterEdPolys.  A and B are pointers
// to any integers that your specific routine requires (or NULL if not needed).
//
typedef void (*BSP_FILTER_FUNC)
(
	UModel			*Model,
	INDEX			iNode,
	FPoly			*EdPoly,
	EPolyNodeFilter Leaf,
	ENodePlace	ENodePlace
);

//
// Information used by FilterEdPoly.
//
class FCoplanarInfo
{
public:
	INDEX	iOriginalNode;
	INDEX   iBackNode;
	int	    BackNodeOutside;
	int	    FrontLeafOutside;
	int     ProcessingBack;
};

//
// Function to filter an EdPoly through the Bsp, calling a callback
// function for all chunks that fall into leaves.
//
void FilterEdPoly
(
	BSP_FILTER_FUNC FilterFunc, 
	UModel			*Model,
	INDEX			iNode, 
	FPoly			*EdPoly, 
	FCoplanarInfo	CoplanarInfo, 
	int				Outside
);

//
// Bsp statistics used by link topic function.
//
class FBspStats
{
public:
	int   Polys;      // Number of BspSurfs.
	int   Nodes;      // Total number of Bsp nodes.
	int   MaxDepth;   // Maximum tree depth.
	int   AvgDepth;   // Average tree depth.
	int   Branches;   // Number of nodes with two children.
	int   Coplanars;  // Number of nodes holding coplanar polygons (in same plane as parent).
	int   Fronts;     // Number of nodes with only a front child.
	int   Backs;      // Number of nodes with only a back child.
	int   Leaves;     // Number of nodes with no children.
	int   FrontLeaves;// Number of leaf nodes that are in front of their parent.
	int   BackLeaves; // Number of leaf nodes that are in back of their parent.
	int   DepthCount; // Depth counter (used for finding average depth).
} GBspStats;

//
// Global variables shared between bspBrushCSG and AddWorldToBrushFunc.  These are very
// tightly tied into the function AddWorldToBrush, not general-purpose.
//
INT         GErrors;        // Errors encountered in Csg operation.
INT			GDiscarded;		// Number of polys discarded and not added.
INDEX		GNode;          // Node AddBrushToWorld is adding to.
INDEX		GLastCoplanar;	// Last coplanar beneath GNode at start of AddWorldToBrush.
INDEX		GNumNodes;		// Number of Bsp nodes at start of AddWorldToBrush.
UModel		*GModel;		// Level map Model we're adding to.

/*-----------------------------------------------------------------------------
   Point and Vector table functions.
-----------------------------------------------------------------------------*/

//
// Add a new point to the model (preventing duplicates) and return its
// index.
//
INDEX AddThing( UVectors::Ptr Vectors, FVector &V, FLOAT Thresh, int Check )
{
	if( Check )
	{
		// See if this is very close to an existing point/vector.		
		for( INDEX i=0; i<Vectors->Num; i++ )
		{
			const FVector &TableVect = Vectors(i);
			FLOAT Temp=(V.X - TableVect.X);
			if( (Temp > -Thresh) && (Temp < Thresh) )
			{
				Temp=(V.Y - TableVect.Y);
				if( (Temp > -Thresh) && (Temp < Thresh) )
				{
					Temp=(V.Z - TableVect.Z);
					if( (Temp > -Thresh) && (Temp < Thresh) )
					{
						// Found nearly-matching vector.
						return i;
					}
				}
			}
		}
	}

	// Add new vector.
	int Index = Vectors->Add();
	Vectors(Index) = V;
	return Index;
}

//
// Add a new vector to the model, merging near-duplicates,  and return its index.
//
INDEX FGlobalEditor::bspAddVector( UModel *Model, FVector *V, int Normal )
{
	guard(FGlobalEditor::bspAddVector);
	return AddThing
	(
		Model->Vectors,
		*V,
		Normal ? THRESH_NORMALS_ARE_SAME : THRESH_VECTORS_ARE_NEAR,
		1
	);
	unguard;
}

//
// Add a new point to the model, merging near-duplicates,  and return its index.
//
INDEX FGlobalEditor::bspAddPoint( UModel *Model, FVector *V, int Exact )
{
	guard(FGlobalEditor::bspAddPoint);
	FLOAT Thresh = Exact ? THRESH_POINTS_ARE_SAME : THRESH_POINTS_ARE_NEAR;

	// Try to find a match quickly from the Bsp. This finds all potential matches
	// except for any dissociated from nodes/surfaces during a rebuild.
	FVector Temp;
	INDEX pVertex;
	FLOAT NearestDist = Model->FindNearestVertex(*V,Temp,Thresh,pVertex);
	if( (NearestDist >= 0.0) && (NearestDist <= Thresh) )
	{
		// Found an existing point.
		return pVertex;
	}
	else
	{
		// No match found; add it slowly to find duplicates.
		return AddThing( Model->Points, *V, Thresh, !FastRebuild );
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Adding polygons to the Bsp.
----------------------------------------------------------------------------*/

//
// Add an editor polygon to the Bsp, and also stick a reference to it
// in the editor polygon's BspNodes list. If the editor polygon has more sides
// than the Bsp will allow, split it up into several sub-polygons.
//
// Returns: Index to newly-created node of Bsp.  If several nodes were created because
// of split polys, returns the parent (highest one up in the Bsp).
//
INDEX FGlobalEditor::bspAddNode
(
	UModel		*Model, 
	INDEX		iParent, 
	ENodePlace	ENodePlace,
	DWORD		NodeFlags, 
	FPoly		*EdPoly
)
{
	guard(FGlobalEditor::bspAddNode);
	if( ENodePlace == NODE_Plane )
	{
		// Make sure coplanars are added at the end of the coplanar list so that 
		// we don't insert NF_IsNew nodes with non NF_IsNew coplanar children.
		while( Model->Nodes(iParent).iPlane != INDEX_NONE )
			iParent = Model->Nodes(iParent).iPlane;
	}
	FBspSurf *Surf = NULL;
	if( EdPoly->iLink == Model->Surfs->Num )
	{
		Surf = &Model->Surfs(Model->Surfs->Add());

		// This node has a new polygon being added by bspBrushCSG; must set its
		// properties here.
		Surf->pBase     	= bspAddPoint  (Model,&EdPoly->Base,1);
		Surf->vNormal   	= bspAddVector (Model,&EdPoly->Normal,1);
		Surf->vTextureU 	= bspAddVector (Model,&EdPoly->TextureU,0);
		Surf->vTextureV 	= bspAddVector (Model,&EdPoly->TextureV,0);
		Surf->Texture  		= EdPoly->Texture;
		Surf->iLightMesh	= INDEX_NONE;
		Surf->Actor			= NULL;
		Surf->LastStartY	= 0;
		Surf->LastEndY		= 0;

		Surf->PanU 			= EdPoly->PanU;
		Surf->PanV 			= EdPoly->PanV;
		Surf->PolyFlags 	= EdPoly->PolyFlags & ~PF_NoAddToBSP;
		Surf->Brush	 		= EdPoly->Brush;
		Surf->iBrushPoly	= EdPoly->iBrushPoly;
	}
	else if( EdPoly->iLink != INDEX_NONE )
	{
		checkState( EdPoly->iLink < Model->Surfs->Num );
		Surf = &Model->Surfs(EdPoly->iLink);
	}
	else appError("BspAddNode: iLink==INDEX_NONE");

	// Set NodeFlags.
	if( Surf->PolyFlags & PF_NotSolid              ) NodeFlags |= NF_NotCsg;
	if( Surf->PolyFlags & (PF_Invisible|PF_Portal) ) NodeFlags |= NF_NotVisBlocking;
	if( Surf->PolyFlags & PF_Masked                ) NodeFlags |= NF_ShootThrough;

	if( EdPoly->NumVertices > FBspNode::MAX_NODE_VERTICES )
	{
		// Split up into two coplanar sub-polygons (one with MAX_NODE_VERTICES vertices and
		// one with all the remaining vertices) and recursively add them.

		// Copy first bunch of verts.
		FMemMark Mark(GMem);
		FPoly *EdPoly1 = new(GMem)FPoly;
		*EdPoly1 = *EdPoly;
		EdPoly1->NumVertices = FBspNode::MAX_NODE_VERTICES;

		FPoly *EdPoly2 = new(GMem)FPoly;
		*EdPoly2 = *EdPoly;
		EdPoly2->NumVertices = EdPoly->NumVertices + 2 - FBspNode::MAX_NODE_VERTICES;

		// Copy first vertex then the remaining vertices.
		memmove
		(
			&EdPoly2->Vertex[1],
			&EdPoly->Vertex [FBspNode::MAX_NODE_VERTICES - 1],
			(EdPoly->NumVertices + 1 - FBspNode::MAX_NODE_VERTICES) * sizeof (FVector)
		);
		INDEX iNode = bspAddNode(Model,iParent,ENodePlace,NodeFlags,EdPoly1); // Add this poly first.
		INDEX iTemp = bspAddNode(Model,iNode,  NODE_Plane,NodeFlags,EdPoly2); // Then add other (may be bigger).

		Mark.Pop();
		return iNode; // Return coplanar "parent" node (not coplanar child)
	}
	else
	{
		// Tell transaction tracking system that parent is about to be modified unless this
		// node is the root.
		if( ENodePlace != NODE_Root )
			Model->Nodes->ModifyItem( iParent );

		// Set node properties.
		INDEX iNode			 = Model->Nodes->Add();
		FBspNode &Node       = Model->Nodes(iNode);
		Node.iSurf       	 = EdPoly->iLink;
		Node.iDynamic[0]	 = 0;
		Node.iDynamic[1] 	 = 0;
		Node.NodeFlags   	 = NodeFlags;
		Node.iRenderBound    = INDEX_NONE;
		Node.iCollisionBound = INDEX_NONE;
		Node.iZone[0]		 = 0;
		Node.iZone[1]		 = 0;
		Node.ZoneMask		 = 0;

		// Set node plane.
		Node.Plane = FPlane( EdPoly->Base, EdPoly->Normal );

		// Set up vertex pool for node.
		Node.iVertPool = Model->Verts->Add(EdPoly->NumVertices);
		FVert *VertPool	= &Model->Verts(Node.iVertPool);

		// Init links.
		Node.iFront		= INDEX_NONE;
		Node.iBack		= INDEX_NONE;
		Node.iPlane		= INDEX_NONE;

		FBspNode &ParentNode = Model->Nodes(iParent);

		// Link parent to this node.
		if      (ENodePlace==NODE_Front)  ParentNode.iFront = iNode;
		else if (ENodePlace==NODE_Back)   ParentNode.iBack  = iNode;
		else if (ENodePlace==NODE_Plane)  ParentNode.iPlane = iNode;
		else if (ENodePlace==NODE_Root)   {};

		// Add all points to point table, merging nearly-overlapping polygon points
		// with other points in the poly to prevent criscrossing vertices from
		// being generated.

		// Must maintain Node->NumVertices on the fly so that bspAddPoint is always
		// called with the Bsp in a clean state.
		BYTE n           = EdPoly->NumVertices;
		Node.NumVertices = 0;

		for( BYTE i=0; i<n; i++ )
		{
			int pVertex = bspAddPoint(Model,&EdPoly->Vertex[i],0);
			if( (Node.NumVertices==0) || (VertPool[Node.NumVertices-1].pVertex!=pVertex) )
			{
				VertPool[Node.NumVertices].iSide   = INDEX_NONE; // No side link
				VertPool[Node.NumVertices].pVertex = pVertex;
				Node.NumVertices++;
			}
		}
		if( (Node.NumVertices >= 2) && (VertPool[0].pVertex==VertPool[Node.NumVertices-1].pVertex) )
		{
			Node.NumVertices--;
		}
		if( Node.NumVertices < 3 )
		{
			GErrors++;
			debugf( LOG_Problem, "bspAddNode: Infinitesimal polygon %i (%i)", Node.NumVertices, n );
			Node.NumVertices = 0;
		}
		return iNode;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Bsp Splitting.
-----------------------------------------------------------------------------*/

//
// Find the best splitting polygon within a pool of polygons, and return its
// index (into the PolyList array).
//
FPoly *FindBestSplit
(
	int					NumPolys,
	FPoly				**PolyList,
	EBspOptimization	Opt,
	int					Balance
)
{
	guard(FindBestSplit);
	checkInput(NumPolys>0);

	// No need to test if only one poly.
	if( NumPolys==1 )
		return PolyList[0];

	FPoly   *Poly, *Best=NULL;
	FLOAT   Score, BestScore;
	int     i, Index, j, Inc;
	int     Splits, Front, Back, Coplanar, AllSemiSolids;

	if		(Opt==BSP_Optimal)  Inc = 1;					// Test lots of nodes.
	else if (Opt==BSP_Good)		Inc = Max(1,NumPolys/20);	// Test 20 nodes.
	else /* BSP_Lame */			Inc = Max(1,NumPolys/4);	// Test 4 nodes.

	// See if there are any non-semisolid polygons here.
	for( i=0; i<NumPolys; i++ )
		if( !(PolyList[i]->PolyFlags & PF_AddLast) )
			break;
	AllSemiSolids = (i>=NumPolys);

	// Search through all polygons in the pool and find:
	// A. The number of splits each poly would make.
	// B. The number of front and back nodes the polygon would create.
	// C. Number of coplanars.
	BestScore = 0;
	for( i=0; i<NumPolys; i+=Inc )
	{
		Splits = Front = Back = Coplanar = 0;
		Index = i-1;
		do
		{
			Index++;
			Poly = PolyList[Index];
		} while( Index<(i+Inc) && Index<NumPolys && (Poly->PolyFlags & PF_AddLast) && !AllSemiSolids );
		if( Index>=i+Inc || Index>=NumPolys )
			continue;

		for( j=0; j<NumPolys; j+=Inc ) if( j != Index )
		{
			FPoly *OtherPoly = PolyList[j];
			switch( OtherPoly->SplitWithPlaneFast( FPlane( Poly->Base, Poly->Normal), NULL, NULL ) )
			{
				case SP_Coplanar:
					Coplanar++;
					break;

				case SP_Front:
					Front++;
					break;

				case SP_Back:
					Back++;
					break;

				case SP_Split:
					// Disfavor splitting polys that are zone portals.
					if( !(Poly->PolyFlags & PF_Portal) )
						Splits++;
					else
						Splits += 16;
					break;
			}
		}
		//Score = (Balance * Abs(Front-Back) + (100-Balance)*Splits) / (0.001 + Poly->Area());
		Score = Balance * Abs(Front-Back) + (100-Balance)*Splits;
		if( Score<BestScore || !Best )
		{
			Best      = Poly;
			BestScore = Score;
		}
	}
	checkOutput(Best);
	return Best;
	unguard;
}

//
// Pick a splitter poly then split a pool of polygons into front and back polygons and
// recurse.
//
// iParent = Parent Bsp node, or INDEX_NONE if this is the root node.
// IsFront = 1 if this is the front node of iParent, 0 of back (undefined if iParent==INDEX_NONE)
//
void SplitPolyList
(
	UModel				*Model,
	INDEX				iParent,
	ENodePlace			NodePlace,
	INDEX				NumPolys,
	FPoly				**PolyList,
	EBspOptimization	Opt,
	int					Balance,
	int					RebuildSimplePolys
)
{
	guard(SplitPolyList);
	FMemMark Mark(GMem);

	// To account for big EdPolys split up.
	int NumPolysToAlloc = NumPolys + 8 + NumPolys/4;
	int NumFront=0; FPoly **FrontList = new(GMem,NumPolysToAlloc)FPoly*;
	int NumBack =0; FPoly **BackList  = new(GMem,NumPolysToAlloc)FPoly*;

	FPoly *SplitPoly = FindBestSplit( NumPolys, PolyList, Opt, Balance );

	// Add the splitter poly to the Bsp with either a new BspSurf or an existing one.
	if( RebuildSimplePolys )
		SplitPoly->iLink = Model->Surfs->Num;

	INDEX iOurNode	 = GUnrealEditor.bspAddNode(Model,iParent,NodePlace,0,SplitPoly);
	INDEX iPlaneNode = iOurNode;

	// Now divide all polygons in the pool into (A) polygons that are
	// in front of Poly, and (B) polygons that are in back of Poly.
	// Coplanar polys are inserted immediately, before recursing.

	// If any polygons are split by Poly, we ignrore the original poly,
	// split it into two polys, and add two new polys to the pool.

	FPoly *FrontEdPoly = new(GMem)FPoly;
	FPoly *BackEdPoly  = new(GMem)FPoly;
	for( INDEX i=0; i<NumPolys; i++ )
	{
		FPoly *EdPoly = PolyList[i];
		if( EdPoly == SplitPoly )
			continue;

		switch( EdPoly->SplitWithPlane( SplitPoly->Base, SplitPoly->Normal, FrontEdPoly, BackEdPoly, 0 ) )
		{
			case SP_Coplanar:
	            if( RebuildSimplePolys )
					EdPoly->iLink = Model->Surfs->Num-1;
				iPlaneNode = GUnrealEditor.bspAddNode( Model, iPlaneNode, NODE_Plane, 0, EdPoly );
				break;
			
			case SP_Front:
	            FrontList[NumFront++] = PolyList[i];
				break;
			
			case SP_Back:
	            BackList[NumBack++] = PolyList[i];
				break;
			
			case SP_Split:

				// Create front & back nodes.
				FrontList[NumFront++] = FrontEdPoly;
				BackList [NumBack ++] = BackEdPoly;

				// If newly-split polygons have too many vertices, break them up in half.
				if( FrontEdPoly->NumVertices >= FPoly::VERTEX_THRESHOLD )
				{
					FPoly *Temp = new(GMem)FPoly;
					FrontEdPoly->SplitInHalf(Temp);
					FrontList[NumFront++] = Temp;
				}
				if( BackEdPoly->NumVertices >= FPoly::VERTEX_THRESHOLD )
				{
					FPoly *Temp = new(GMem)FPoly;
					BackEdPoly->SplitInHalf(Temp);
					BackList[NumBack++] = Temp;
				}
				FrontEdPoly = new(GMem)FPoly;
				BackEdPoly  = new(GMem)FPoly;
				break;
		}
	}

	// Recursively split the front and back pools.
	if( NumFront > 0 ) SplitPolyList( Model, iOurNode, NODE_Front, NumFront, FrontList, Opt, Balance, RebuildSimplePolys );
	if( NumBack  > 0 ) SplitPolyList( Model, iOurNode, NODE_Back,  NumBack,  BackList,  Opt, Balance, RebuildSimplePolys );

	Mark.Pop();
	unguard;
}

/*-----------------------------------------------------------------------------
	High-level Bsp functions.
-----------------------------------------------------------------------------*/

//
// Build Bsp from the editor polygon set (EdPolys) of a model.
//
// Opt     = Bsp optimization, BSP_Lame (fast), BSP_Good (medium), BSP_Optimal (slow)
// Balance = 0-100, 0=only worry about minimizing splits, 100=only balance tree.
//
void FGlobalEditor::bspBuild
(
	UModel				*Model, 
	EBspOptimization	Opt, 
	int					Balance, 
	int					RebuildSimplePolys
)
{
	guard(FGlobalEditor::bspBuild);

	// Empty the model's tables.
	Model->Lock(LOCK_ReadWrite);
	if( RebuildSimplePolys )
	{
		// Empty everything but polys.
		Model->EmptyModel( 1, 0 );
	}
	else
	{
		// Empty node vertices.
		for( int i=0; i<Model->Nodes->Num; i++ )
			Model->Nodes(i).NumVertices = 0;

		// Refresh the Bsp.
		bspRefresh(Model,1);
		
		// Empty nodes.
		Model->EmptyModel( 0, 0 );
	}
	if( Model->Polys->Num && !(Model->ModelFlags & MF_InvalidBsp) )
	{
		// Allocate polygon pool.
		FMemMark Mark(GMem);
		FPoly **PolyList = new( GMem, Model->Polys->Num )FPoly*;

		// Add all FPolys to active list.
		for( int i=0; i<Model->Polys->Num; i++ )
			if( Model->Polys(i).NumVertices )
				PolyList[i] = &Model->Polys(i);

		// Now split the entire Bsp by splitting the list of all polygons.
		SplitPolyList
		(
			Model,
			INDEX_NONE,
			NODE_Root,
			Model->Polys->Num,
			PolyList,
			Opt,
			Balance,
			RebuildSimplePolys
		);

		// Now build the bounding boxes for all nodes.
		if( !RebuildSimplePolys )
		{
			// Remove unreferenced things.
			bspRefresh( Model, 1 );

			// Rebuild all bounding boxes.
			bspBuildBounds( Model );
		}
		Mark.Pop();
	}
	Model->Unlock(LOCK_ReadWrite);
	unguard;
}

/*----------------------------------------------------------------------------
   Brush validate.
----------------------------------------------------------------------------*/

//
// Validate a brush, and set iLinks on all EdPolys to index of the
// first identical EdPoly in the list, or its index if it's the first.
// Not transactional.
//
void FGlobalEditor::bspValidateBrush
(
	UModel	*Brush,
	int		ForceValidate,
	int		DoStatusUpdate
)
{
	guard(FGlobalEditor::bspValidateBrush);
	FPoly 			*EdPoly,*OtherPoly;
	FLOAT			Dist;
	INDEX			i,j,n=0;

	Brush->Lock(LOCK_Trans);
	if( ForceValidate || !(Brush->ModelFlags & MF_Linked) )
	{
		Brush->ModelFlags |= MF_Linked;
		for( i=0; i<Brush->Polys->Num; i++ )
		{
			Brush->Polys(i).iLink = i;
		}
		for( i=0; i<Brush->Polys->Num; i++ )
		{
			if( DoStatusUpdate && !(i&7) )
				GApp->StatusUpdate("Validating Brush",i,Brush->Polys->Num);

			EdPoly = &Brush->Polys(i);
			if( EdPoly->iLink==i )
			{
				for( j=i+1; j<Brush->Polys->Num; j++ )
				{
					OtherPoly = &Brush->Polys(j);
					if
					(	OtherPoly->iLink == j
					&&	OtherPoly->Texture == EdPoly->Texture
					&&	(OtherPoly->Normal | EdPoly->Normal)>0.9999 )
					{
						Dist = FPointPlaneDist( OtherPoly->Base, EdPoly->Base, EdPoly->Normal );
						if( Dist>-0.001 && Dist<0.001 )
						{
							OtherPoly->iLink = i;
							n++;
						}
					}
				}
			}
		}
		//debugf (LOG_INFO,"BspValidateBrush linked %i of %i polys",n,Model->Polys->Num);
	}
	Brush->Unlock(LOCK_Trans);

	// Build bounds.
	Brush->BuildBound(0);
	Brush->BuildBound(1);
	unguard;
}

/*----------------------------------------------------------------------------
   EdPoly building and compacting.
----------------------------------------------------------------------------*/

//
// Convert a Bsp node's polygon to an EdPoly, add it to the list, and recurse.
//
void MakeEdPolys(UModel *Model,INDEX iNode)
{
	FBspNode	*Node   = &Model->Nodes(iNode);

	FPoly Temp;
	if (GUnrealEditor.bspNodeToFPoly(Model,iNode,&Temp) >= 3)
		Model->Polys->AddItem(Temp);

	if (Node->iFront!=INDEX_NONE) MakeEdPolys(Model,Node->iFront);
	if (Node->iBack !=INDEX_NONE) MakeEdPolys(Model,Node->iBack );
	if (Node->iPlane!=INDEX_NONE) MakeEdPolys(Model,Node->iPlane);
}

//
// Trys to merge two polygons.  If they can be merged, replaces Poly1 and emptys Poly2
// and returns 1.  Otherwise, returns 0.
//
int TryToMerge( FPoly *Poly1, FPoly *Poly2 )
{
	guard(TryToMerge);
	FPoly    NewPoly;
	int      Start1,Start2,Test1,Test2,i,End1,End2,Vertex;

	// Don't even consider merging polys that would result in a merged poly
	// with more sides than we can handle.
	if( ((Poly1->NumVertices)+(Poly2->NumVertices)) > FPoly::MAX_VERTICES )
		return 0;

	// Find one overlapping point.
	for (Start1=0; Start1<Poly1->NumVertices; Start1++)
		for (Start2=0; Start2<Poly2->NumVertices; Start2++)
			if (FPointsAreSame(Poly1->Vertex[Start1],Poly2->Vertex[Start2]))
				goto FoundOverlap;
	return 0; // Couldn't merge, no overlapping points.

	FoundOverlap:

	End1  = Start1;
	End2  = Start2;

	Test1 = Start1+1; if (Test1>=Poly1->NumVertices) Test1 = 0;
	Test2 = Start2-1; if (Test2<0)                   Test2 = Poly2->NumVertices-1;

	if( FPointsAreSame(Poly1->Vertex[Test1],Poly2->Vertex[Test2]) )
	{
		End1   = Test1;
		Start2 = Test2;
	}
	else
	{
		Test1 = Start1-1; if (Test1<0)                   Test1=Poly1->NumVertices-1;
		Test2 = Start2+1; if (Test2>=Poly2->NumVertices) Test2=0;

		if( FPointsAreSame(Poly1->Vertex[Test1],Poly2->Vertex[Test2]) )
		{
			Start1 = Test1;
			End2   = Test2;
		}
		else return 0; // Only one point overlapped.
	}

	// Build a new edpoly containing both polygons merged.
	NewPoly             = *Poly1;
	NewPoly.NumVertices = 0;

	Vertex              = End1;
	for( i=0; i<Poly1->NumVertices; i++ )
	{
		NewPoly.Vertex[NewPoly.NumVertices++] = Poly1->Vertex[Vertex];
		if( ++Vertex >= Poly1->NumVertices )
			Vertex=0;
	}
	Vertex               = End2;
	for( i=0; i<(Poly2->NumVertices-2); i++ )
	{
		if( ++Vertex >= Poly2->NumVertices )
			Vertex=0;
		NewPoly.Vertex[NewPoly.NumVertices++] = Poly2->Vertex[Vertex];
	}

	// Remove colinear vertices and check convexity.
	if( NewPoly.RemoveColinears() )
	{
		if( NewPoly.NumVertices <= FBspNode::MAX_NODE_VERTICES )
		{
			*Poly1				= NewPoly; // Convex.
			Poly2->NumVertices	= 0;
			return 1;
		}
		else return 0; // Would result in too many vertices.
	}
	else return 0; // Nonconvex
	unguard;
}

//
// Merge all polygons in coplanar list that can be merged convexly.
//
void MergeCoplanars( UModel *Model, INDEX *PolyList, int PolyCount )
{
	guard(MergeCoplanars);

	int MergeAgain = 1;
	while( MergeAgain )
	{
		MergeAgain = 0;
		for( int i=0; i<PolyCount; i++ )
		{
			FPoly &Poly1 = Model->Polys(PolyList[i]);
			if( Poly1.NumVertices > 0 )
	        {
				for( int j=i+1; j<PolyCount; j++ )
				{
					FPoly &Poly2 = Model->Polys(PolyList[j]);
					if( Poly2.NumVertices > 0 )
					{
						if( (Poly1.NumVertices + Poly2.NumVertices) <= FPoly::MAX_VERTICES )
						{
                  			// If merges, replaces Poly1 and emptys Poly2.
                  			if( TryToMerge( &Poly1, &Poly2 ) )
								MergeAgain=1;
						}
					}
				}
			}
		}
	}
	unguard;
}

//
// Build EdPoly list from a model's Bsp. Not transactional.
//
void FGlobalEditor::bspBuildFPolys(UModel *Model,int iSurfLinks)
	{
	guard(FGlobalEditor::bspBuildFPolys);
	Model->Lock(LOCK_ReadWrite);
	//
	Model->Polys->Num = 0;
	if (Model->Nodes->Num != 0) MakeEdPolys(Model,0);
	//
	if (!iSurfLinks) 
		for (INDEX i=0; i<Model->Polys->Num; i++) 
			Model->Polys(i).iLink=i;
	Model->Unlock(LOCK_ReadWrite);
	unguard;
	};

//
// Merge all coplanar EdPolys in a model.  Not transactional.
// Preserves (though reorders) iLinks.
//
void FGlobalEditor::bspMergeCoplanars( UModel *Model, int RemapLinks, int MergeDisparateTextures )
{
	guard(FGlobalEditor::bspMergeCoplanars);

	FPoly 		*EdPoly,*OtherPoly;
	FLOAT		Dist;
	INDEX		*PolyList,*Remap;
	INDEX		i,j,n,PolyCount,OriginalNum;

	Model->Lock(LOCK_ReadWrite);
	OriginalNum = Model->Polys->Num;

	// Mark all polys as unprocessed.
	for( i=0; i<Model->Polys->Num; i++ )
		Model->Polys(i).PolyFlags &= ~PF_EdProcessed;

	// Find matching coplanars and merge them.
	FMemMark Mark(GMem);
	PolyList = new(GMem,Model->Polys->Max)INDEX;

	n = 0;
	for( i=0; i<Model->Polys->Num; i++ )
	{
		EdPoly = &Model->Polys(i);
		if( EdPoly->NumVertices>0 && !(EdPoly->PolyFlags & PF_EdProcessed) )
		{
			PolyCount             =  0;
			PolyList[PolyCount++] =  i;
			EdPoly->PolyFlags    |= PF_EdProcessed;

			for( j=i+1; j<Model->Polys->Num; j++ )
	        {
				OtherPoly = &Model->Polys(j);
				if( OtherPoly->iLink == EdPoly->iLink )
				{
					Dist = (OtherPoly->Base - EdPoly->Base) | EdPoly->Normal;
					if
					(	(Dist>-0.001)
					&&	(Dist<0.001)
					&&	((OtherPoly->Normal | EdPoly->Normal)>0.9999)
					&&	(
							MergeDisparateTextures ||
							(	FPointsAreNear(OtherPoly->TextureU,EdPoly->TextureU,THRESH_VECTORS_ARE_NEAR)
							&&	FPointsAreNear(OtherPoly->TextureV,EdPoly->TextureV,THRESH_VECTORS_ARE_NEAR)
							)
						)
					)
					{
						OtherPoly->PolyFlags |= PF_EdProcessed;
						PolyList[PolyCount++] = j;
					}
				}
			}
			if( PolyCount > 1 )
	        {
				MergeCoplanars( Model, PolyList, PolyCount );
				n++;
			}
		}
	}
	debugf( LOG_Info, "Found %i coplanar sets in %i", n, Model->Polys->Num );
	Mark.Pop();

	// Get rid of empty EdPolys while remapping iLinks.
	j=0;
	Remap = new(GMem,Model->Polys->Num)INDEX;
	for( i=0; i<Model->Polys->Num; i++ )
	{
		if( Model->Polys(i).NumVertices )
		{
			Remap[i]			= j;
			Model->Polys(j)	= Model->Polys(i);
			j++;
		}
	}
	Model->Polys->Num = j;
	if( RemapLinks )
		for( i=0; i<Model->Polys->Num; i++ )
			if( Model->Polys(i).iLink != INDEX_NONE )
				Model->Polys(i).iLink = Remap[Model->Polys(i).iLink];

	debugf( LOG_Info, "BspMergeCoplanars reduced %i->%i", OriginalNum, Model->Polys->Num );

	Mark.Pop();
	Model->Unlock(LOCK_ReadWrite);
	unguard;
}

/*----------------------------------------------------------------------------
   CSG types & general-purpose callbacks.
----------------------------------------------------------------------------*/

//
// Recursive worker function called by BspCleanup.
//
void CleanupNodes( UModel *Model, INDEX iNode, INDEX iParent )
{
	FBspNode *Node = &Model->Nodes(iNode);

	// Transactionally empty vertices of tag-for-empty nodes.
	Node->NodeFlags &= ~(NF_IsNew | NF_IsFront | NF_IsBack);

	// Recursively clean up front, back, and plane nodes.
	if( Node->iFront != INDEX_NONE ) CleanupNodes( Model, Node->iFront, iNode );
	if( Node->iBack  != INDEX_NONE ) CleanupNodes( Model, Node->iBack , iNode );
	if( Node->iPlane != INDEX_NONE ) CleanupNodes( Model, Node->iPlane, iNode );

	// Reload Node since the recusive call aliases it.
	Node = &Model->Nodes(iNode);

	// If this is an empty node with a coplanar, replace it with the coplanar.
	if( Node->NumVertices==0 && Node->iPlane!=INDEX_NONE )
	{
		Model->Nodes->ModifyItem(Node->iPlane);
		FBspNode *PlaneNode = &Model->Nodes(Node->iPlane);

		// Stick our front, back, and parent nodes on the coplanar.
		if( (Node->Plane | PlaneNode->Plane) >= 0.0 )
		{
			PlaneNode->iFront  = Node->iFront;
			PlaneNode->iBack   = Node->iBack;
		}
		else
		{
			PlaneNode->iFront  = Node->iBack;
			PlaneNode->iBack   = Node->iFront;
		}

		if( iParent == INDEX_NONE )
		{
			// This node is the root.
			Model->Nodes->ModifyItem(iNode);

			*Node                  = *PlaneNode;   // Replace root.
			PlaneNode->NumVertices = 0;            // Mark as unused.
		}
		else
		{
			// This is a chil node.
			Model->Nodes->ModifyItem(iParent);
			FBspNode *ParentNode = &Model->Nodes(iParent);

			if      ( ParentNode->iFront == iNode ) ParentNode->iFront = Node->iPlane;
			else if ( ParentNode->iBack  == iNode ) ParentNode->iBack  = Node->iPlane;
			else if ( ParentNode->iPlane == iNode ) ParentNode->iPlane = Node->iPlane;
			else appError( "CleanupNodes: Parent and child are unlinked" );
		}
	}
	else if( Node->NumVertices == 0 && ( Node->iFront==INDEX_NONE || Node->iBack==INDEX_NONE ) )
	{
		// Delete empty nodes with no fronts or backs.
		// Replace empty nodes with only fronts.
		// Replace empty nodes with only backs.
		INDEX iReplacementNode;
		if     ( Node->iFront != INDEX_NONE ) iReplacementNode = Node->iFront;
		else if( Node->iBack  != INDEX_NONE ) iReplacementNode = Node->iBack;
		else                                  iReplacementNode = INDEX_NONE;

		if( iParent == INDEX_NONE )
		{
			// Root.
			if( iReplacementNode == INDEX_NONE )
			{
         		Model->Nodes->Num = 0;
			}
			else
			{
				Model->Nodes->ModifyItem(iNode);
         		*Node = Model->Nodes(iReplacementNode);
			}
		}
		else
		{
			// Regular node.
			FBspNode *ParentNode = &Model->Nodes(iParent);
			Model->Nodes->ModifyItem(iParent);

			if     ( ParentNode->iFront == iNode ) ParentNode->iFront = iReplacementNode;
			else if( ParentNode->iBack  == iNode ) ParentNode->iBack  = iReplacementNode;
			else if( ParentNode->iPlane == iNode ) ParentNode->iPlane = iReplacementNode;
			else  appError ("CleanupNodes: Parent and child are unlinked");
		}
	}
}

//
// Clean up all nodes after a CSG operation.  Resets temporary bit flags and unlinks
// empty leaves.  Removes zero-vertex nodes which have nonzero-vertex coplanars.
//
void FGlobalEditor::bspCleanup( UModel *Model )
{
	guard(FGlobalEditor::bspCleanup);
	if( Model->Nodes->Num > 0 ) 
		CleanupNodes( Model, 0, INDEX_NONE );
	unguard;
}

/*----------------------------------------------------------------------------
   CSG leaf filter callbacks.
----------------------------------------------------------------------------*/

void AddBrushToWorldFunc( UModel *Model, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter, ENodePlace ENodePlace )
{
	switch( Filter )
	{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
		case F_COSPATIAL_FACING_OUT:
			GUnrealEditor.bspAddNode (Model,iNode,ENodePlace,NF_IsNew,EdPoly);
			break;
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
		case F_COSPATIAL_FACING_IN:
			break;
	}
}

void AddWorldToBrushFunc( UModel *Model, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter, ENodePlace ENodePlace )
{
	switch( Filter )
	{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
			// Only affect the world poly if it has been cut.
			if( EdPoly->PolyFlags & PF_EdCut )
				GUnrealEditor.bspAddNode( GModel, GLastCoplanar, NODE_Plane, NF_IsNew, EdPoly );
			break;
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
		case F_COSPATIAL_FACING_IN:
		case F_COSPATIAL_FACING_OUT:
			// Discard original poly.
			GDiscarded++;
			if( GModel->Nodes(GNode).NumVertices )
			{
				GModel->Nodes->ModifyItem(GNode);
				GModel->Nodes(GNode).NumVertices = 0;
			}
			break;
	}
}

void SubtractBrushFromWorldFunc( UModel *Model, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter, ENodePlace ENodePlace )
{
	switch (Filter)
	{
		case F_OUTSIDE:
		case F_COSPATIAL_FACING_OUT:
		case F_COSPATIAL_FACING_IN:
		case F_COPLANAR_OUTSIDE:
			break;
		case F_COPLANAR_INSIDE:
		case F_INSIDE:
			EdPoly->Reverse();
			GUnrealEditor.bspAddNode (Model,iNode,ENodePlace,NF_IsNew,EdPoly); // Add to Bsp back
			EdPoly->Reverse();
			break;
	}
}

void SubtractWorldToBrushFunc( UModel *Model, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter, ENodePlace ENodePlace )
{
	switch (Filter)
	{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
		case F_COSPATIAL_FACING_IN:
			// Only affect the world poly if it has been cut.
			if( EdPoly->PolyFlags & PF_EdCut )
				GUnrealEditor.bspAddNode( GModel, GLastCoplanar, NODE_Plane, NF_IsNew, EdPoly );
			break;
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
		case F_COSPATIAL_FACING_OUT:
			// Discard original poly.
			GDiscarded++;
			if( GModel->Nodes(GNode).NumVertices )
			{
				GModel->Nodes->ModifyItem(GNode);
				GModel->Nodes(GNode).NumVertices = 0;
			}
			break;
	}
}

void IntersectBrushWithWorldFunc( UModel *Model, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter,ENodePlace ENodePlace )
{
	switch (Filter)
	{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
		case F_COSPATIAL_FACING_IN:
		case F_COSPATIAL_FACING_OUT:
			// Ignore.
			break;
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
			if( EdPoly->Fix()>=3 )
				GModel->Polys->AddItem(*EdPoly);
			break;
	}
}

void IntersectWorldWithBrushFunc( UModel *Model, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter,ENodePlace ENodePlace )
{
	switch (Filter)
	{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
		case F_COSPATIAL_FACING_IN:
			// Ignore.
			break;
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
		case F_COSPATIAL_FACING_OUT:
			if( EdPoly->Fix() >= 3 )
				GModel->Polys->AddItem(*EdPoly);
			break;
	}
}

void DeIntersectBrushWithWorldFunc( UModel *Model, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter,ENodePlace ENodePlace )
{
	switch (Filter)
	{
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
		case F_COSPATIAL_FACING_OUT:
		case F_COSPATIAL_FACING_IN:
			// Ignore.
			break;
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
			if( EdPoly->Fix()>=3 )
				GModel->Polys->AddItem(*EdPoly);
			break;
	}
}

void DeIntersectWorldWithBrushFunc( UModel *Model, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter,ENodePlace ENodePlace )
{
	switch (Filter)
	{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
		case F_COSPATIAL_FACING_OUT:
			// Ignore.
			break;
		case F_COPLANAR_INSIDE:
		case F_INSIDE:
		case F_COSPATIAL_FACING_IN:
			if( EdPoly->Fix() >= 3 )
	        {
				EdPoly->Reverse();
				GModel->Polys->AddItem(*EdPoly);
				EdPoly->Reverse();
			}
			break;
	}
}

/*----------------------------------------------------------------------------
   CSG polygon filtering routine (calls the callbacks).
----------------------------------------------------------------------------*/

//
// Handle a piece of a polygon that was filtered to a leaf.
//
void FilterLeaf
(
	BSP_FILTER_FUNC FilterFunc, 
	UModel			*Model,
	INDEX			iNode, 
	FPoly			*EdPoly, 
	FCoplanarInfo	CoplanarInfo, 
	INT				LeafOutside, 
	ENodePlace		ENodePlace
)
{
	guard(FilterLeaf);
	EPolyNodeFilter FilterType;

	if( CoplanarInfo.iOriginalNode == INDEX_NONE )
	{
		// Processing regular, non-coplanar polygons.
		FilterType = LeafOutside ? F_OUTSIDE : F_INSIDE;
		FilterFunc( Model, iNode, EdPoly, FilterType, ENodePlace );
	}
	else if( CoplanarInfo.ProcessingBack )
	{
		// Finished filtering polygon through tree in back of parent coplanar.
		DoneFilteringBack:
		if      ((!LeafOutside) && (!CoplanarInfo.FrontLeafOutside)) FilterType = F_COPLANAR_INSIDE;
		else if (( LeafOutside) && ( CoplanarInfo.FrontLeafOutside)) FilterType = F_COPLANAR_OUTSIDE;
		else if ((!LeafOutside) && ( CoplanarInfo.FrontLeafOutside)) FilterType = F_COSPATIAL_FACING_OUT;
		else if (( LeafOutside) && (!CoplanarInfo.FrontLeafOutside)) FilterType = F_COSPATIAL_FACING_IN;
		else
		{
			appError ("FilterLeaf: Bad Locs");
			return;
		}
		FilterFunc (Model, CoplanarInfo.iOriginalNode, EdPoly, FilterType, NODE_Plane);
	}
	else
	{
		CoplanarInfo.FrontLeafOutside = LeafOutside;

		if( CoplanarInfo.iBackNode == INDEX_NONE )
		{
			// Back tree is empty.
			LeafOutside = CoplanarInfo.BackNodeOutside;
			goto DoneFilteringBack;
		}
		else
		{
			// Call FilterEdPoly to filter through the back.  This will result in
			// another call to FilterLeaf with iNode = leaf this falls into in the
			// back tree and EdPoly = the final EdPoly to insert.
			CoplanarInfo.ProcessingBack=1;
			FilterEdPoly (FilterFunc,Model,CoplanarInfo.iBackNode,EdPoly,CoplanarInfo,CoplanarInfo.BackNodeOutside);
		}
	}
	unguard;
}

//
// Filter an EdPoly through the Bsp recursively, calling FilterFunc
// for all chunks that fall into leaves.  FCoplanarInfo is used to
// handle the tricky case of double-recursion for polys that must be
// filtered through a node's front, then filtered through the node's back,
// in order to handle coplanar CSG properly.
//
void FilterEdPoly
(
	BSP_FILTER_FUNC	FilterFunc, 
	UModel			*Model,
	INDEX			iNode, 
	FPoly			*EdPoly, 
	FCoplanarInfo	CoplanarInfo, 
	INT				Outside
)
{
	guard(FilterEdPoly);

	INDEX          SplitResult,iOurFront,iOurBack;
	INT			   NewFrontOutside,NewBackOutside;

	FilterLoop:
	if( EdPoly->NumVertices >= FPoly::VERTEX_THRESHOLD )
	{
		// Split EdPoly in half to prevent vertices from overflowing.
		FPoly Temp;
		EdPoly->SplitInHalf(&Temp);

		// Filter other half.
		FilterEdPoly( FilterFunc, Model, iNode, &Temp, CoplanarInfo, Outside );
	}

	// Split em.
	FPoly TempFrontEdPoly,TempBackEdPoly;
	SplitResult = EdPoly->SplitWithPlane
	(
		Model->Points (Model->Surfs(Model->Nodes(iNode).iSurf).pBase),
		Model->Vectors(Model->Surfs(Model->Nodes(iNode).iSurf).vNormal),
		&TempFrontEdPoly,
		&TempBackEdPoly,
		0
	);

	// Process split results.
	if( SplitResult == SP_Front )
	{
		Front:

		FBspNode *Node = &Model->Nodes(iNode);
		Outside        = Outside || Node->IsCsg();

		if( Node->iFront == INDEX_NONE )
		{
			FilterLeaf(FilterFunc,Model,iNode,EdPoly,CoplanarInfo,Outside,NODE_Front);
		}
		else
		{
			iNode = Node->iFront;
			goto FilterLoop;
		}
	}
	else if( SplitResult == SP_Back )
	{
		FBspNode *Node = &Model->Nodes(iNode);
		Outside        = Outside && !Node->IsCsg();

		if( Node->iBack == INDEX_NONE )
		{
			FilterLeaf( FilterFunc, Model, iNode, EdPoly, CoplanarInfo, Outside, NODE_Back );
		}
		else
		{
			iNode=Node->iBack;
			goto FilterLoop;
		}
	}
	else if( SplitResult == SP_Coplanar )
	{
		if( CoplanarInfo.iOriginalNode != INDEX_NONE )
		{
			// This will happen once in a blue moon when a polygon is barely outside the
			// coplanar threshold and is split up into a new polygon that is
			// is barely inside the coplanar threshold.  To handle this, just classify
			// it as front and it will be handled propery.
			GErrors++;
			debugf( LOG_Problem, "FilterEdPoly: Encountered out-of-place coplanar" );
			goto Front;
		}
		CoplanarInfo.iOriginalNode        = iNode;
		CoplanarInfo.iBackNode            = INDEX_NONE;
		CoplanarInfo.ProcessingBack       = 0;
		CoplanarInfo.BackNodeOutside      = Outside;
		NewFrontOutside                   = Outside;

		// See whether Node's iFront or iBack points to the side of the tree on the front
		// of this polygon (will be as expected if this polygon is facing the same
		// way as first coplanar in link, otherwise opposite).
		if( (Model->Nodes(iNode).Plane | EdPoly->Normal) >= 0.0 )
		{
			iOurFront = Model->Nodes(iNode).iFront;
			iOurBack  = Model->Nodes(iNode).iBack;
			
			if( Model->Nodes(iNode).IsCsg() )
	        {
				CoplanarInfo.BackNodeOutside = 0;
				NewFrontOutside              = 1;
			}
		}
		else
		{
			iOurFront = Model->Nodes(iNode).iBack;
			iOurBack  = Model->Nodes(iNode).iFront;
			
			if( Model->Nodes(iNode).IsCsg() )
	        {
				CoplanarInfo.BackNodeOutside = 1; 
				NewFrontOutside              = 0;
			}
		}

		// Process front and back.
		if ((iOurFront==INDEX_NONE)&&(iOurBack==INDEX_NONE))
		{
			// No front or back.
			CoplanarInfo.ProcessingBack		= 1;
			CoplanarInfo.FrontLeafOutside	= NewFrontOutside;
			FilterLeaf
			(
				FilterFunc,
				Model,
				iNode,
				EdPoly,
				CoplanarInfo,
				CoplanarInfo.BackNodeOutside,
				NODE_Plane
			);
		}
		else if( iOurFront==INDEX_NONE && iOurBack!=INDEX_NONE )
		{
			// Back but no front.
			CoplanarInfo.ProcessingBack		= 1;
			CoplanarInfo.iBackNode			= iOurBack;
			CoplanarInfo.FrontLeafOutside	= NewFrontOutside;

			iNode   = iOurBack;
			Outside = CoplanarInfo.BackNodeOutside;
			goto FilterLoop;
		}
		else
		{
			// Has a front and maybe a back.

			// Set iOurBack up to process back on next call to FilterLeaf, and loop
			// to process front.  Next call to FilterLeaf will set FrontLeafOutside.
			CoplanarInfo.ProcessingBack = 0;

			// May be a node or may be INDEX_NONE.
			CoplanarInfo.iBackNode = iOurBack;

			iNode   = iOurFront;
			Outside = NewFrontOutside;
			goto FilterLoop;
		}
	}
	else if( SplitResult == SP_Split )
	{
		// Front half of split.
		if( Model->Nodes(iNode).IsCsg() )
		{
			NewFrontOutside = 1; 
			NewBackOutside  = 0;
		}
		else
		{
			NewFrontOutside = Outside;
			NewBackOutside  = Outside;
		}

		if( Model->Nodes(iNode).iFront==INDEX_NONE )
		{
			FilterLeaf
			(
				FilterFunc,
				Model,
				iNode,
				&TempFrontEdPoly,
				CoplanarInfo,
				NewFrontOutside,
				NODE_Front
			);
		}
		else
		{
			FilterEdPoly
			(
				FilterFunc,
				Model,
				Model->Nodes(iNode).iFront,
				&TempFrontEdPoly,
				CoplanarInfo,
				NewFrontOutside
			);
		}

		// Back half of split.
		if( Model->Nodes(iNode).iBack==INDEX_NONE )
		{
			FilterLeaf
			(
				FilterFunc,
				Model,
				iNode,
				&TempBackEdPoly,
				CoplanarInfo,
				NewBackOutside,
				NODE_Back
			);
		}
		else
		{
			FilterEdPoly
			(
				FilterFunc,
				Model,
				Model->Nodes(iNode).iBack,
				&TempBackEdPoly,
				CoplanarInfo,
				NewBackOutside
			);
		}
	}
	unguard;
}

//
// Regular entry into FilterEdPoly (so higher-level callers don't have to
// deal with unnecessary info). Filters starting at root.
//
void BspFilterFPoly( BSP_FILTER_FUNC FilterFunc, UModel *Model, FPoly *EdPoly )
{
	guard(BspFilterFPoly);

	FCoplanarInfo StartingCoplanarInfo;
	StartingCoplanarInfo.iOriginalNode = INDEX_NONE;
	if( Model->Nodes->Num == 0 )
	{
		// If Bsp is empty, process at root.
		FilterFunc( Model, 0, EdPoly, Model->RootOutside ? F_OUTSIDE : F_INSIDE, NODE_Root );
	}
	else
	{
		// Filter through Bsp.
		FilterEdPoly( FilterFunc, Model, 0, EdPoly, StartingCoplanarInfo, Model->RootOutside );
	}
	unguard;
}

/*----------------------------------------------------------------------------
   Editor fundamentals.
----------------------------------------------------------------------------*/

//
// Convert a Bsp node to an EdPoly.  Returns number of vertices in Bsp
// node (0 or 3-MAX_NODE_VERTICES).
//
int FGlobalEditor::bspNodeToFPoly
(
	UModel	*Model,
	INDEX	iNode,
	FPoly	*EdPoly
)
{
	guard(FGlobalEditor::bspNodeToFPoly);
	FPoly		MasterEdPoly;
	BYTE		i,j,n,prev;

	FBspNode &Node     	= Model->Nodes (iNode);
	FBspSurf &Poly     	= Model->Surfs (Node.iSurf);
	FVert	 *VertPool	= &Model->Verts (Node.iVertPool);

	EdPoly->Base		= Model->Points  (Poly.pBase);
	EdPoly->Normal      = Model->Vectors (Poly.vNormal);

	EdPoly->PolyFlags 	= Poly.PolyFlags & ~(PF_EdCut | PF_EdProcessed | PF_Selected | PF_Memorized);
	EdPoly->iLink     	= Node.iSurf;
	EdPoly->Texture     = Poly.Texture;

	EdPoly->Brush    	= Poly.Brush;
	EdPoly->iBrushPoly  = Poly.iBrushPoly;

	EdPoly->PanU		= Poly.PanU;
	EdPoly->PanV		= Poly.PanV;

	EdPoly->iZone[0]	= Node.iZone[0];
	EdPoly->iZone[1]	= Node.iZone[1];

	if( polyFindMaster(Model,Node.iSurf,MasterEdPoly) )
	{
		EdPoly->GroupName = MasterEdPoly.GroupName;
		EdPoly->ItemName  = MasterEdPoly.ItemName;
	}
	else
	{
		EdPoly->GroupName = NAME_None;
		EdPoly->ItemName  = NAME_None;
	}
	EdPoly->TextureU = Model->Vectors(Poly.vTextureU);
	EdPoly->TextureV = Model->Vectors(Poly.vTextureV);

	n = Node.NumVertices;
	i=0; j=0; prev=n-1;

	for( i=0; i<n; i++ )
	{
		EdPoly->Vertex[j] = Model->Points(VertPool[i].pVertex);
		prev = i;
		j++;
	}
	if (j>=3) EdPoly->NumVertices=j;
	else      EdPoly->NumVertices=0;

	// Remove colinear points and identical points (which will appear
	// if T-joints were eliminated).
	EdPoly->RemoveColinears();

	return EdPoly->NumVertices;
	unguard;
}

/*---------------------------------------------------------------------------------------
   World filtering.
---------------------------------------------------------------------------------------*/

//
// Filter all relevant world polys through the brush.
//
void FilterWorldThroughBrush 
(
	UModel		*Model,
	UModel		*Brush,
	ECsgOper	CSGOper,
	INDEX		iNode,
	FPlane		*BrushSphere
)
{
	guard(FilterWorldThroughBrush);

	// Loop through all coplanars.
	while( iNode != INDEX_NONE )
	{
		// Get surface.
		INDEX iSurf = Model->Nodes(iNode).iSurf;

		// Skip new nodes and their children, which are guaranteed new.
		if( Model->Nodes(iNode).NodeFlags & NF_IsNew )
			return;

		// Sphere reject.
		int DoFront = 1, DoBack = 1;
		if( BrushSphere )
		{
			FLOAT Dist = Model->Nodes(iNode).Plane.PlaneDot( *BrushSphere );
			DoFront    = (Dist >= -BrushSphere->W);
			DoBack     = (Dist <= +BrushSphere->W);
		}

		// Process only polys that aren't empty.
		FPoly TempEdPoly;
		if( DoFront && DoBack && (GUnrealEditor.bspNodeToFPoly(Model,iNode,&TempEdPoly)>0) )
		{
			TempEdPoly.Brush      = Model->Surfs(iSurf).Brush;
			TempEdPoly.iBrushPoly = Model->Surfs(iSurf).iBrushPoly;

			if( CSGOper==CSG_Add || CSGOper==CSG_Subtract )
			{
				// Add and subtract work the same in this step.
				GNode       = iNode;
				GModel  	= Model;
				GDiscarded  = 0;
				GNumNodes	= Model->Nodes->Num;

				// Find last coplanar in chain.
				GLastCoplanar = iNode;
				while( Model->Nodes(GLastCoplanar).iPlane != INDEX_NONE )
					GLastCoplanar = Model->Nodes(GLastCoplanar).iPlane;

				// Do the filter operation.
				BspFilterFPoly
				(
					CSGOper==CSG_Add ? AddWorldToBrushFunc : SubtractWorldToBrushFunc,
					Brush,
					&TempEdPoly
				);
				
				if( GDiscarded == 0 )
				{
					// Get rid of all the fragments we added.
					Model->Nodes(GLastCoplanar).iPlane = INDEX_NONE;
					Model->Nodes->Num                  = GNumNodes;
				}
				else
				{
					// Tag original world poly for deletion; has been deleted or replaced by partial fragments.
					if( GModel->Nodes(GNode).NumVertices )
					{
						GModel->Nodes->ModifyItem(GNode);
						GModel->Nodes(GNode).NumVertices = 0;
					}
				}
			}
			else if( CSGOper == CSG_Intersect )
			{
				BspFilterFPoly( IntersectWorldWithBrushFunc, Brush, &TempEdPoly );
			}
			else if( CSGOper == CSG_Deintersect )
			{
				BspFilterFPoly( DeIntersectWorldWithBrushFunc, Brush, &TempEdPoly );
			}
		}

		// Now recurse to filter all of the world's children nodes.
		if( DoFront && (Model->Nodes(iNode).iFront != INDEX_NONE)) FilterWorldThroughBrush
		(
			Model,
			Brush,
			CSGOper,
			Model->Nodes(iNode).iFront,
			BrushSphere
		);
		if( DoBack && (Model->Nodes(iNode).iBack != INDEX_NONE) ) FilterWorldThroughBrush
		(
			Model,
			Brush,
			CSGOper,
			Model->Nodes(iNode).iBack,
			BrushSphere
		);
		iNode = Model->Nodes(iNode).iPlane;
	}
	unguard;
}

/*---------------------------------------------------------------------------------------
   Brush Csg.
---------------------------------------------------------------------------------------*/

//
// Perform any CSG operation between the brush and the world.
//
int FGlobalEditor::bspBrushCSG
(
	UModel		*Brush, 
	UModel		*Model, 
	DWORD		PolyFlags, 
	ECsgOper	CSGOper, 
	int			BuildBounds
)
{
	guard(FGlobalEditor::bspBrushCSG);
	DWORD NotPolyFlags = 0;
	int		NumPolysFromBrush=0,i,j,ReallyBig;
	char	*Descr;

	// Note no errors.
	GErrors = 0;

	// Make sure we're in an acceptable state.
	if( !Brush || GUnrealEditor.MapEdit || (Model->ModelFlags & MF_InvalidBsp) )
		return 0;

	// Non-solid and semisolid stuff can only be added.
	if( CSGOper != CSG_Add )
		NotPolyFlags |= (PF_Semisolid | PF_NotSolid);

	// Prep the model.
	Model->Lock(LOCK_Trans);
	Model->Nodes->NumZones = 0;

	// Prep the brush.
	Brush->Lock(LOCK_Trans);
	Brush->Location += Brush->PostPivot;

	// Prep the temporary model.
	TempModel->Lock(LOCK_ReadWrite);
	TempModel->EmptyModel(1,1);

	// Build the brush's coordinate system and find orientation of scale
	// transform (if negative, edpolyTransform will reverse the clockness
	// of the EdPoly points and invert the normal).
	FModelCoords Coords,Uncoords;
	FLOAT Orientation = Brush->BuildCoords( &Coords, &Uncoords );

	// Update status.
	ReallyBig = (Brush->Polys->Num > 200);
	if( ReallyBig )
	{
		switch( CSGOper )
		{
			case CSG_Add:			Descr = "Adding brush to world"; break;
			case CSG_Subtract: 		Descr = "Subtracting brush from world"; break;
			case CSG_Intersect: 	Descr = "Intersecting brush with world"; break;
			case CSG_Deintersect: 	Descr = "Deintersecting brush with world"; break;
			default:				Descr = "Performing CSG operation"; break;
		}
		GApp->BeginSlowTask( Descr, 1, 0 );
	}

	// Transform original brush poly into same coordinate system as world
	// so Bsp filtering operations make sense.
	if( ReallyBig ) GApp->StatusUpdate ("Transforming",0,0);
	for( i=0; i<Brush->Polys->Num; i++ )
	{
		// Get the brush poly.
		FPoly DestEdPoly = Brush->Polys(i);

		// Set its backward brush link.
		DestEdPoly.Brush       = Brush;
		DestEdPoly.iBrushPoly  = i;

		// Update its flags.
		DestEdPoly.PolyFlags = (DestEdPoly.PolyFlags | PolyFlags) & ~NotPolyFlags;

		// Set its internal link.
		if( DestEdPoly.iLink==INDEX_NONE )
			DestEdPoly.iLink = i;

		// Transform it.
		DestEdPoly.Transform( Coords, Brush->PrePivot, Brush->Location, Orientation );

		// Fix the base if necessary.
		FLOAT Error = (DestEdPoly.Vertex[0] - DestEdPoly.Base) | DestEdPoly.Normal;
		if( Abs(Error) > 0.0001 )
			DestEdPoly.Base += Error * DestEdPoly.Normal;

		// Make sure it has a valid texture.
		if( !DestEdPoly.Texture )
			DestEdPoly.Texture = CurrentTexture;

		// Add poly to the temp model.
		TempModel->Polys->AddItem( DestEdPoly );
	}
	if( ReallyBig ) GApp->StatusUpdate ("Filtering brush",0,0);

	// Pass the brush polys through the world Bsp.
	if( CSGOper==CSG_Intersect || CSGOper==CSG_Deintersect )
	{
		// Empty the brush.
		Brush->EmptyModel(1,1);

		// Intersect and deintersect.
		for( i=0; i<TempModel->Polys->Num; i++ )
		{
         	FPoly EdPoly = TempModel->Polys(i);
			GModel = Brush;
			BspFilterFPoly( CSGOper==CSG_Intersect ? IntersectBrushWithWorldFunc : DeIntersectBrushWithWorldFunc, Model,  &EdPoly );
		}
		NumPolysFromBrush = Brush->Polys->Num;
	}
	else
	{
		// Add and subtract.
		for( i=0; i<Brush->Polys->Num; i++ )
		{
         	FPoly EdPoly = TempModel->Polys(i);

         	// Mark the polygon as non-cut so that it won't be harmed unless it must
         	// be split, and set iLink so that BspAddNode will know to add its information
         	// if a node is added based on this poly.
         	EdPoly.PolyFlags &= ~(PF_EdCut);
          	if( EdPoly.iLink == i )
			{
				EdPoly.iLink = TempModel->Polys(i).iLink = Model->Surfs->Num;
			}
			else
			{
				EdPoly.iLink = TempModel->Polys(EdPoly.iLink).iLink;
			}

			// Filter brush through the world.
			BspFilterFPoly( CSGOper==CSG_Add ? AddBrushToWorldFunc : SubtractBrushFromWorldFunc, Model, &EdPoly );
		}
	}
	if( Model->Nodes->Num && !(PolyFlags & (PF_NotSolid | PF_Semisolid)) )
	{
		// Quickly build a Bsp for the brush, tending to minimize splits rather than balance
		// the tree.  We only need the cutting planes, though the entire Bsp struct (polys and
		// all) is built.

		TempModel->Unlock(LOCK_ReadWrite); // BspBuild locks this temporarily.
		if( ReallyBig ) GApp->StatusUpdate ("Building Bsp",0,0);
		
		bspBuild( TempModel, BSP_Lame, 0, 1 );
		
		if( ReallyBig ) GApp->StatusUpdate( "Filtering world",0,0 );
		GModel = Brush;
		TempModel->BuildBound(0);

		TempModel->Lock(LOCK_ReadWrite);		
		FilterWorldThroughBrush( Model, TempModel, CSGOper, 0, &TempModel->LocalBound.Sphere );
	}
	if( CSGOper==CSG_Intersect || CSGOper==CSG_Deintersect )
	{
		if( ReallyBig ) GApp->StatusUpdate( "Adjusting brush", 0, 0 );

		// Link polys obtained from the original brush.
		for( i=NumPolysFromBrush-1; i>=0; i-- )
		{
			FPoly *DestEdPoly = &Brush->Polys(i);
			for( j=0; j<i; j++ )
			{
				if( DestEdPoly->iLink == Brush->Polys(j).iLink )
				{
					DestEdPoly->iLink = j;
					break;
				}
			}
			if( j >= i ) DestEdPoly->iLink = i;
		}

		// Link polys obtained from the world.
		for( i=Brush->Polys->Num-1; i>=NumPolysFromBrush; i-- )
		{
			FPoly *DestEdPoly = &Brush->Polys(i);
			for( j=NumPolysFromBrush; j<i; j++ )
			{
				if( DestEdPoly->iLink == Brush->Polys(j).iLink )
				{
					DestEdPoly->iLink = j;
					break;
				}
			}
			if( j >= i ) DestEdPoly->iLink = i;
		}
		Brush->ModelFlags |= MF_Linked;

		// Detransform the obtained brush back into its original coordinate system.
		for( i=0; i<Brush->Polys->Num; i++ )
		{
			FPoly *DestEdPoly = &Brush->Polys(i);
			DestEdPoly->Transform( Uncoords, Brush->Location, Brush->PrePivot, Orientation );
			DestEdPoly->Fix();
			DestEdPoly->Brush		= NULL;
			DestEdPoly->iBrushPoly	= i;
		}
	}
	Brush->Location -= Brush->PostPivot;

	if( CSGOper==CSG_Add || CSGOper==CSG_Subtract )
	{
		// Clean up nodes, reset node flags.
		bspCleanup( Model );

		// Rebuild bounding volumes.
		if( BuildBounds )
			bspBuildBounds( Model );
	}

	// Release brush.
	Brush->Unlock(LOCK_Trans);

	// Release model.
	Model->Unlock(LOCK_Trans);

	// Release TempModel.
	TempModel->Unlock(LOCK_ReadWrite);
	TempModel->EmptyModel(1,1);
	
	// Merge coplanars if needed.
	if( CSGOper==CSG_Intersect || CSGOper==CSG_Deintersect )
	{
		if( ReallyBig )
			GApp->StatusUpdate( "Merging", 0, 0 );
		bspMergeCoplanars( Brush, 1, 0 );
	}
	if( ReallyBig )
		GApp->EndSlowTask();

	return 1 + GErrors;
	unguard;
}

/*---------------------------------------------------------------------------------------
   Bsp stats.
---------------------------------------------------------------------------------------*/

//
// Calculate stats for a node and all its children.  Called recursively.
// IsFront: 1=front node of parent, 0=back node of parent
// Depth:   Depth of node in the Bsp, 0=root.
//
void CalcBspNodeStats (UModel *Model, INDEX iNode, FBspStats *Stats,
	int IsFront, int Depth)
	{
	FBspNode    *Node = &Model->Nodes(iNode);
	INDEX       i;
	//
	Stats->DepthCount++;
	//
	if (Depth > Stats->MaxDepth) Stats->MaxDepth = Depth;
	//
	// Process front and back:
	//
	if ((Node->iFront==INDEX_NONE)&&(Node->iBack==INDEX_NONE)) // Leaf
		{
		if ((Depth>0)&&(IsFront==1))      Stats->FrontLeaves++;
		else if ((Depth>0)&&(IsFront==0)) Stats->BackLeaves++;
		Stats->Leaves++;
		}
	else if (Node->iBack==INDEX_NONE) // Has front but no back
		{
		Stats->Fronts++;
		CalcBspNodeStats(Model,Node->iFront,Stats,1,Depth+1);
		}
	else if (Node->iFront==INDEX_NONE) // Has back but no front
		{
		Stats->Backs++;
		CalcBspNodeStats(Model,Node->iBack,Stats,0,Depth+1);
		}
	else // Has both front and back
		{
		Stats->Branches++;
		CalcBspNodeStats(Model,Node->iFront,Stats,1,Depth+1);
		CalcBspNodeStats(Model,Node->iBack,Stats,0,Depth+1);
		};
	//
	// Process coplanars:
	//
	i=Node->iPlane;
	while (i!=INDEX_NONE)
		{
		Stats->Coplanars++;
		i = Model->Nodes(i).iPlane;
		};
	};

//
// Calculate stats for entire tree:
//
void BspCalcStats(UModel *Model, FBspStats *Stats)
	{
	guard(BspCalcStats);
	//
	Model->Lock(LOCK_Read);
	//
	Stats->Polys       = Model->Surfs->Num;
	Stats->Nodes       = Model->Nodes->Num;
	Stats->MaxDepth    = 0; // Will be calculated
	Stats->AvgDepth    = 0;
	Stats->Branches    = 0;
	Stats->Coplanars   = 0;
	Stats->Fronts      = 0;
	Stats->Backs       = 0;
	Stats->Leaves      = 0;
	Stats->FrontLeaves = 0;
	Stats->BackLeaves  = 0;
	Stats->DepthCount  = 0L;
	//
	if (Model->Nodes->Num>0) CalcBspNodeStats(Model,0,Stats,1,0);
	//
	if (Stats->Leaves>0) Stats->AvgDepth = Stats->DepthCount/Stats->Leaves;
	//
	Model->Unlock(LOCK_Read);
	unguard;
	};

/*---------------------------------------------------------------------------------------
   Bsp link topic handler.
---------------------------------------------------------------------------------------*/

//
// Link topic function for Unreal to communicate Bsp information to UnrealEd.
//
AUTOREGISTER_TOPIC("Bsp",BspTopicHandler);
void BspTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(BspTopicHandler::Get);

	// Recalc stats when 'Polys' item is accessed.
	if( (stricmp(Item,"POLYS")==0) )
		BspCalcStats(Level->Model,&GBspStats);

	// Handle item.
	if      (stricmp(Item,"Polys"       )==0) Out.Logf("%i",GBspStats.Polys);
	else if (stricmp(Item,"Nodes"       )==0) Out.Logf("%i",GBspStats.Nodes);
	else if (stricmp(Item,"MaxDepth"    )==0) Out.Logf("%i",GBspStats.MaxDepth);
	else if (stricmp(Item,"AvgDepth"    )==0) Out.Logf("%i",GBspStats.AvgDepth);
	else if (stricmp(Item,"Branches"    )==0) Out.Logf("%i",GBspStats.Branches);
	else if (stricmp(Item,"Coplanars"   )==0) Out.Logf("%i",GBspStats.Coplanars);
	else if (stricmp(Item,"Fronts"      )==0) Out.Logf("%i",GBspStats.Fronts);
	else if (stricmp(Item,"Backs"       )==0) Out.Logf("%i",GBspStats.Backs);
	else if (stricmp(Item,"Leaves"      )==0) Out.Logf("%i",GBspStats.Leaves);
	else if (stricmp(Item,"FrontLeaves" )==0) Out.Logf("%i",GBspStats.FrontLeaves);
	else if (stricmp(Item,"BackLeaves"  )==0) Out.Logf("%i",GBspStats.BackLeaves);

	unguard;
}
void BspTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(BspTopicHandler::Set);
	unguard;
}

/*---------------------------------------------------------------------------------------
   Functions for maintaining linked geometry lists.
---------------------------------------------------------------------------------------*/

//
// A node and vertex number corresponding to a point, used in generating Bsp side links.
//
class FPointVert 
{
public:
	INDEX		iNode;
	INDEX		nVertex;
	FPointVert	*Next;
};

//
// A list of point/vertex links, used in generating Bsp side links.
//
class FPointVertList
{
public:

	// Variables.
	UModel		*Model;
	FPointVert	**Index;
	FMemMark	Mark;

	// Allocate.
	void Alloc( UModel *ThisModel )
	{
		guard(FPointVertList::Alloc);
		Mark.Push(GMem);

		// Allocate working tables.
		Model = ThisModel;
		Index = new(GMem,MEM_Zeroed,Model->Points->Num)FPointVert*;

		unguard;
	}

	// Free.
	void Free()
	{
		guard(FPointVertList::Free);
		Mark.Pop();
		unguard;
	}

	// Add all of a node's vertices to a node-vertex list.
	void AddNode( INDEX iNode )
	{
		guard(FPointVertList::AddNode);

		FBspNode	&Node 		=  Model->Nodes (iNode);
		FVert		*VertPool   =  &Model->Verts (Node.iVertPool);

		for( INDEX i=0; i < Node.NumVertices; i++ )
		{
			INDEX pVertex = VertPool[i].pVertex;

			// Add new point/vertex pair to array, and insert new array entry
			// between index and first entry.
			FPointVert *PointVert	= new(GMem)FPointVert;
			
			PointVert->iNode 		= iNode;
			PointVert->nVertex		= i;
			PointVert->Next			= Index[pVertex];

			Index[pVertex]			= PointVert;
		}
		unguard;
	}

	// Add all nodes' vertices in the model to a node-vertex list.
	void AddAllNodes()
	{
		guard(FPointVertList::AddAllNodes);

		for( INDEX iNode=0; iNode < Model->Nodes->Num; iNode++ )
			AddNode( iNode );

		unguard;
	}

	// Remove all of a node's vertices from a node-vertex list.
	void RemoveNode( INDEX iNode )
	{
		guard(FPointVertList::RemoveNode);

		FBspNode	&Node 		= Model->Nodes(iNode);
		FVert		*VertPool	= &Model->Verts(Node.iVertPool);

		// Loop through all of the node's vertices and search through the
		// corresponding point's node-vert list, and delink this node.
		int Count=0;
		for( INDEX i = 0; i < Node.NumVertices; i++ )
		{
			INDEX pVertex = VertPool[i].pVertex;

			for( FPointVert **PrevLink = &Index[pVertex]; *PrevLink; PrevLink=&(*PrevLink)->Next )
			{
				if( (*PrevLink)->iNode == iNode )
				{
					// Delink this entry from the list.
					*PrevLink = (*PrevLink)->Next;
					Count++;

					if( *PrevLink == NULL )
						break;
				}
			}

			// Node's vertex wasn't found, there's a bug.
			checkLogic(Count>=1);
		}
		unguard;
	}
};

/*---------------------------------------------------------------------------------------
   Geometry optimization.
---------------------------------------------------------------------------------------*/

//
// Add a point to a Bsp node before a specified vertex (between it and the previous one).
// VertexNumber can be from 0 (before first) to Node->NumVertices (after last).
//
// Splits node into two coplanar polys if necessary. If the polygon is split, the
// vertices will be distributed among this node and it's newly-linked iPlane node
// in an arbitrary way, that preserves the clockwise orientation of the vertices.
//
// Maintains node-vertex list, if not NULL.
//
void AddPointToNode
(
	UModel			*Model,
	FPointVertList	*PointVerts,
	INDEX			iNode, 
	INDEX			VertexNumber, 
	INDEX			pVertex 
)
{
	guard(AddPointToNode);

	FBspNode &Node = Model->Nodes(iNode);
	if( (Node.NumVertices+1) >= FBspNode::MAX_NODE_VERTICES )
	{
		// Just refuse to add point: This is a non-fatal problem.
		debugf( LOG_Problem, "Node side limit reached" );
		return;
	}

	// Remove node from vertex list, since vertex numbers will be reordered.
	if( PointVerts )
		PointVerts->RemoveNode( iNode );

	INDEX iOldVert = Node.iVertPool;
	Node.iVertPool = Model->Verts->Add(Node.NumVertices+1);;

	// Make sure this node doesn't already contain the vertex.
	for( INDEX i=0; i<Node.NumVertices; i++ )
		checkLogic( Model->Verts(iOldVert + i).pVertex != pVertex );

	// Copy the old vertex pool to the new one.
	for( i=0; i<VertexNumber; i++ )
		Model->Verts(Node.iVertPool + i) = Model->Verts(iOldVert + i);

	for( i=VertexNumber; i<Node.NumVertices; i++ )
		Model->Verts(Node.iVertPool + i + 1) = Model->Verts(iOldVert + i);

	// Add the new point to the new vertex pool.
	Model->Verts(Node.iVertPool + VertexNumber).pVertex = pVertex;
	Model->Verts(Node.iVertPool + VertexNumber).iSide   = INDEX_NONE;
	
	// Increment number of node vertices.
	Node.NumVertices++;

	// Update the point-vertex list.
	if( PointVerts )
		PointVerts->AddNode( iNode );

	unguard;
}

//
// Add a point to all sides of polygons in which the side intersects with
// this point but doesn't contain it, and has the correct (clockwise) orientation
// as this side.  pVertex is the index of the point to handle, and
// ReferenceVertex defines the direction of this side.
//
int DistributePoint
(
	UModel			*Model,
	FPointVertList	*PointVerts,	
	INDEX			iNode,
	INDEX			pVertex
)
{
	guard(DistributePoint);
	INT Count = 0;

	// Handle front, back, and plane.
	FLOAT Dist = Model->Nodes(iNode).Plane.PlaneDot(Model->Points(pVertex));
	if( Dist < THRESH_OPTGEOM_COPLANAR )
	{
		// Back.
		if( Model->Nodes(iNode).iBack != INDEX_NONE )
			Count += DistributePoint( Model, PointVerts, Model->Nodes(iNode).iBack, pVertex );
	}
	if( Dist > -THRESH_OPTGEOM_COPLANAR )
	{
		// Front.
		if( Model->Nodes(iNode).iFront != INDEX_NONE )
			Count += DistributePoint( Model, PointVerts, Model->Nodes(iNode).iFront, pVertex );
	}
	if( Dist > -THRESH_OPTGEOM_COPLANAR && Dist < THRESH_OPTGEOM_COPLANAR )
	{
		// This point is coplanar with this node, so check point for intersection with
		// this node's sides, then loop with its coplanars.
		for( iNode; iNode!=INDEX_NONE; iNode=Model->Nodes(iNode).iPlane )
		{
			FVert *VertPool = &Model->Verts(Model->Nodes(iNode).iVertPool);

			// Skip this node if it already contains the point in question.
			for( INDEX i=0; i<Model->Nodes(iNode).NumVertices; i++ )
				if( VertPool[i].pVertex == pVertex )
					break;
			if( i != Model->Nodes(iNode).NumVertices )
				continue;

			// Loop through all sides and see if (A) side is colinear with point, and
			// (B) point falls within inside of this side.
			INT FoundSide       = -1;
			INT SkippedColinear = 0;
			INT SkippedInside   = 0;

			for( i=0; i<Model->Nodes(iNode).NumVertices; i++ )
			{
				INDEX j = (i>0) ? (i-1) : (Model->Nodes(iNode).NumVertices-1);

				// Create cutting plane perpendicular to both this side and the polygon's normal.
				FVector Side            = Model->Points(VertPool[i].pVertex) - Model->Points(VertPool[j].pVertex);
				FVector SidePlaneNormal = Side ^ Model->Nodes(iNode).Plane;
				FLOAT   SizeSquared     = SidePlaneNormal.SizeSquared();

				if( SizeSquared>Square(0.001) )
				{
					// Points aren't coincedent.
					Dist = ((Model->Points(pVertex)-Model->Points(VertPool[i].pVertex)) | SidePlaneNormal)/sqrt(SizeSquared);

					if( Dist >= THRESH_OPTGEOM_COSIDAL )
					{
						// Point is outside polygon, can't possibly fall on a side.
						break;
					}
					else if( Dist > -THRESH_OPTGEOM_COSIDAL )
					{
						// The point we're adding falls on this line.
						//
						// Verify that it falls within this side; though it's colinear
						// it may be out of the bounds of the line's endpoints if this side
						// is colinear with an adjacent side.
						//
						// Do this by checking distance from point to side's midpoint and
						// comparing with the side's half-length.

						FVector MidPoint    = (Model->Points(VertPool[i].pVertex) + Model->Points(VertPool[j].pVertex))*0.5;
						FVector MidDistVect = Model->Points(pVertex) - MidPoint;
						if( MidDistVect.SizeSquared() <= Square(0.501)*Side.SizeSquared() )
						{
							FoundSide = i;
						}
						else
						{
							SkippedColinear = 1;
						}
					}
					else
					{
						// Point is inside polygon, so continue checking.
						SkippedInside = 1;
					}
				}
				else
				{
					GErrors++;
					//debugf( LOG_Problem, "Found tiny side" );
					//Poly.PolyFlags |= PF_Selected;
				}
			}
			if( i==Model->Nodes(iNode).NumVertices && FoundSide>=0 )
			{
				// AddPointToNode will reorder the vertices in this node.  This is okay
				// because it's called outside of the vertex loop.
				AddPointToNode( Model, PointVerts, iNode, FoundSide, pVertex );
				Count++;
			}
			else if( SkippedColinear )
			{
				// This happens occasionally because of the fuzzy Dist comparison.  It is
				// not a sign of a problem when the vertex being distributed is colinear
				// with one of this polygon's sides, but slightly outside of this polygon.

				GErrors++;
				//debugf( LOG_Problem, "Skipped colinear" );
				//Poly.PolyFlags |= PF_Selected;
			}
			else if( SkippedInside )
			{
				// Point is on interior of polygon.
				GErrors++;
				//debugf( LOG_Problem, "Skipped interior" );
				//Poly.PolyFlags |= PF_Selected;
			}
		}
	}
	return Count;
	unguard;
}

//
// Merge points that are near.
//
void MergeNearPoints( UModel *Model, FLOAT Dist )
{
	guard(MergeNearPoints);
	FMemMark Mark(GMem);
	INDEX *PointRemap = new(GMem,Model->Points->Num)INDEX;
	int Merged=0,Collapsed=0;

	// Find nearer point for all points.
	for( INDEX i=0; i<Model->Points->Num; i++ )
	{
		PointRemap[i] = i;
		FVector &Point = Model->Points(i);
		for( INDEX j=0; j<i; j++ )
		{
			FVector &TestPoint = Model->Points(j);
			if( (TestPoint - Point).SizeSquared() < Dist*Dist )
			{
				PointRemap[i] = j;
				Merged++;
				break;
			}
		}
	}

	// Remap VertPool.
	for( i=0; i<Model->Verts->Num; i++ )
		if( Model->Verts(i).pVertex>=0 && Model->Verts(i).pVertex<Model->Points->Num )
			Model->Verts(i).pVertex = PointRemap[Model->Verts(i).pVertex];

	// Remap Surfs.
	for( i=0; i<Model->Surfs->Num; i++ )
		if( Model->Surfs(i).pBase>=0 && Model->Surfs(i).pBase<Model->Points->Num )
			Model->Surfs(i).pBase = PointRemap[Model->Surfs(i).pBase];

	// Remove duplicate points from nodes.
	for( i=0; i<Model->Nodes->Num; i++ )
	{
		FBspNode &Node = Model->Nodes(i);
		FVert *Pool = &Model->Verts(Node.iVertPool);
		
		int k=0;
		for( int j=0; j<Node.NumVertices; j++ )
		{
			FVert *A = &Pool[j];
			FVert *B = &Pool[j ? j-1 : Node.NumVertices-1];
			
			if( A->pVertex != B->pVertex )
				Pool[k++] = Pool[j];
		}
		Node.NumVertices = k>=3 ? k : 0;
		if( k < 3 )
			Collapsed++;
	}

	// Success.
	debugf( "MergeNearPoints merged %i/%i, collapsed %i", Merged, Model->Points->Num, Collapsed );
	Mark.Pop();
	unguard;
}

//
// Optimize a level's Bsp, eliminating T-joints where possible, and building side
// links.  This does not always do a 100% perfect job, mainly due to imperfect 
// levels, however it should never fail or return incorrect results.
//
void FGlobalEditor::bspOptGeom( UModel *Model )
{
	guard(FGlobalEditor::bspOptGeom);
	FPointVertList PointVerts;

	debugf( LOG_Info, "BspOptGeom begin" );

	if( GTrans->IsLocked() )
		GTrans->ForceOverflow("Geometry optimization");

	Model->Lock				(LOCK_ReadWrite);
	MergeNearPoints			(Model,0.25);
	bspRefresh				(Model,0);
	PointVerts.Alloc		(Model);
	PointVerts.AddAllNodes	();

	// First four entries are reserved for view-clipped sides.
	Model->Verts->NumSharedSides = 4;
	
	// Mark all sides as unlinked.
	for( int i=0; i<Model->Verts->Num; i++ )
		Model->Verts(i).iSide = INDEX_NONE;

	INT TeesFound = 0, Distributed = 0;

	// Eliminate T-joints on each node by finding all vertices that aren't attached to
	// two shared sides, then filtering them down through the BSP and adding them to
	// the sides they belong on.
	guard(1);
	for( INDEX iNode=0; iNode < Model->Nodes->Num; iNode++ )
	{
		FBspNode &Node = Model->Nodes(iNode);
		
		// Loop through all sides (side := line from PrevVert to ThisVert)		
		for( BYTE ThisVert=0; ThisVert < Node.NumVertices; ThisVert++ )
		{
			BYTE PrevVert = (ThisVert>0) ? (ThisVert - 1) : (Node.NumVertices-1);

			// Count number of nodes sharing this side, i.e. number of nodes for
			// which two adjacent vertices are identical to this side's two vertices.
			for( FPointVert *PV1 = PointVerts.Index[Model->Verts(Node.iVertPool+ThisVert).pVertex]; PV1; PV1=PV1->Next )
				for( FPointVert *PV2 = PointVerts.Index[Model->Verts(Node.iVertPool+PrevVert).pVertex]; PV2; PV2=PV2->Next )
					if( PV1->iNode==PV2->iNode && PV1->iNode!=iNode )
						goto SkipIt;

			// Didn't find another node that shares our two vertices; must add each
			// vertex to all polygons where the vertex lies on the polygon's side.
			// DistributePoint will not affect the current node but may change others
			// and may increase the number of nodes in the Bsp.
			TeesFound++;
			Distributed = 0;
			Distributed += DistributePoint( Model, &PointVerts, 0, Model->Verts(Node.iVertPool+ThisVert).pVertex );
			Distributed += DistributePoint( Model, &PointVerts, 0, Model->Verts(Node.iVertPool+PrevVert).pVertex );
			SkipIt:;
		}
	}
	unguard;

	// Build side links
	// Definition of side: Side (i) links node vertex (i) to vertex ((i+1)%n)
	debugf( LOG_Info, "BspOptGeom building sidelinks" );

	PointVerts.Free			();
	PointVerts.Alloc		(Model);
	PointVerts.AddAllNodes	();

	guard(2);
	for( INDEX iNode=0; iNode < Model->Nodes->Num; iNode++ )
	{
		FBspNode &Node = Model->Nodes(iNode);		
		for( BYTE ThisVert=0; ThisVert < Node.NumVertices; ThisVert++ )
		{
			if( Model->Verts(Node.iVertPool+ThisVert).iSide == INDEX_NONE )
			{
				// See if this node links to another one.
				BYTE PrevVert = (ThisVert>0) ? (ThisVert - 1) : (Node.NumVertices-1);
				for( FPointVert *PV1=PointVerts.Index[Model->Verts(Node.iVertPool+ThisVert).pVertex]; PV1; PV1=PV1->Next )
				{
					for( FPointVert *PV2=PointVerts.Index[Model->Verts(Node.iVertPool+PrevVert).pVertex]; PV2; PV2=PV2->Next )
					{
						if( PV1->iNode==PV2->iNode && PV1->iNode!=iNode )
						{
							// Make sure that the other node's two vertices are adjacent and
							// ordered opposite this node's vertices.
							INDEX		iOtherNode	= PV2->iNode;
							FBspNode	&OtherNode	= Model->Nodes(iOtherNode);

							INT Delta = 
								(OtherNode.NumVertices + PV2->nVertex - PV1->nVertex) % 
								OtherNode.NumVertices;

							if( Delta==1 )
							{
								// Side is properly linked!
								INDEX OtherVert = PV2->nVertex;
								INDEX iSide;
								if( Model->Verts(OtherNode.iVertPool+OtherVert).iSide==INDEX_NONE )
									iSide = Model->Verts->NumSharedSides++;
								else
									iSide = Model->Verts(OtherNode.iVertPool+OtherVert).iSide;
									
								// Link both sides to the shared side.
								Model->Verts( Node.iVertPool + ThisVert ).iSide
								=	Model->Verts(OtherNode.iVertPool+OtherVert).iSide
								=	iSide;
								goto SkipSide;
							}
						}
					}
				}

				// This node doesn't have correct side linking
				//Model->Surfs(Node.iSurf).PolyFlags |= PF_Selected;
				GErrors++;
				//debugf( LOG_Problem, "Failed to link side" );
			}

			// Go to next side.
			SkipSide:;
		}
	}
	unguard;

	// Gather stats.
	i=0; int j=0;
	guard(3);
	for( INDEX iNode=0; iNode < Model->Nodes->Num; iNode++ )
	{
		FBspNode	&Node		= Model->Nodes(iNode);
		FVert		*VertPool	= &Model->Verts(Node.iVertPool);
		
		i += Node.NumVertices;
		for( BYTE ThisVert=0; ThisVert < Node.NumVertices; ThisVert++ )
			if( VertPool[ThisVert].iSide!=INDEX_NONE )
				j++;
	}
	unguard;

	// Done.
	debugf( LOG_Info, "BspOptGeom end" );
	debugf( LOG_Info, "Processed %i T-points, linked: %i/%i sides", TeesFound, j, i );

	PointVerts.Free();
	Model->Unlock(LOCK_ReadWrite);
	unguard;
}

/*---------------------------------------------------------------------------------------
   Bsp point/vector refreshing.
---------------------------------------------------------------------------------------*/

void TagReferencedNodes( UModel *Model, INDEX *NodeRef, INDEX *PolyRef, INDEX iNode )
{
	FBspNode &Node = Model->Nodes(iNode);

	NodeRef[iNode     ] = 0;
	PolyRef[Node.iSurf] = 0;

	if( Node.iFront != INDEX_NONE ) TagReferencedNodes(Model,NodeRef,PolyRef,Node.iFront);
	if( Node.iBack  != INDEX_NONE ) TagReferencedNodes(Model,NodeRef,PolyRef,Node.iBack );
	if( Node.iPlane != INDEX_NONE ) TagReferencedNodes(Model,NodeRef,PolyRef,Node.iPlane);
}

//
// If the Bsp's point and vector tables are nearly full, reorder them and delete
// unused ones:
//
void FGlobalEditor::bspRefresh( UModel *Model, int NoRemapSurfs )
{
	guard(FGlobalEditor::bspRefresh);

	FMemMark Mark(GMem);
	INDEX *VectorRef, *PointRef, *NodeRef, *PolyRef, i;
	BYTE  B;

	// Remove unreferenced Bsp surfs.
	NodeRef		= new(GMem,MEM_Oned,Model->Nodes->Num)INDEX;
	PolyRef		= new(GMem,MEM_Oned,Model->Surfs->Num)INDEX;
	if( Model->Nodes->Num > 0 )
		TagReferencedNodes( Model, NodeRef, PolyRef, 0 );

	if( NoRemapSurfs )
		// Don't remap surfs.
		memset(PolyRef,0,Model->Surfs->Num * sizeof (INDEX));

	// Remap Bsp nodes and surfs.
	int n=0;
	for( i=0; i<Model->Surfs->Num; i++ ) if (PolyRef[i]!=INDEX_NONE)
	{
		Model->Surfs(n) = Model->Surfs(i);
		PolyRef[i]=n++;
	}
	debugf( LOG_Info, "Polys: %i -> %i", Model->Surfs->Num, n );
	Model->Surfs->Num = n;

	n=0;
	for( i=0; i<Model->Nodes->Num; i++ ) if( NodeRef[i]!=INDEX_NONE )
	{
		Model->Nodes(n) = Model->Nodes(i);
		NodeRef[i]=n++;
	}
	debugf( LOG_Info, "Nodes: %i -> %i", Model->Nodes->Num, n );
	Model->Nodes->Num = n;

	// Update Bsp nodes.
	for( i=0; i<Model->Nodes->Num; i++ )
	{
		FBspNode *Node = &Model->Nodes(i);
		Node->iSurf = PolyRef[Node->iSurf];
		if (Node->iFront != INDEX_NONE) Node->iFront = NodeRef[Node->iFront];
		if (Node->iBack  != INDEX_NONE) Node->iBack  = NodeRef[Node->iBack];
		if (Node->iPlane != INDEX_NONE) Node->iPlane = NodeRef[Node->iPlane];
	}

	// Remove unreferenced points and vectors.
	VectorRef = new(GMem,MEM_Oned,Model->Vectors->Num)INDEX;
	PointRef  = new(GMem,MEM_Oned,Model->Points->Num )INDEX;

	// Check Bsp surfs.
	for( i=0; i<Model->Surfs->Num; i++ )
	{
		FBspSurf *Surf = &Model->Surfs(i);
		VectorRef [Surf->vNormal   ] = 0;
		VectorRef [Surf->vTextureU ] = 0;
		VectorRef [Surf->vTextureV ] = 0;
		PointRef  [Surf->pBase     ] = 0;
	}

	// Check Bsp nodes.
	for( i=0; i<Model->Nodes->Num; i++ )
	{
		// Tag all points used by nodes.
		FBspNode	*Node		= &Model->Nodes(i);
		FVert		*VertPool	= &Model->Verts(Node->iVertPool);
		for( B=0; B<Node->NumVertices;  B++ )
		{			
			PointRef[VertPool->pVertex] = 0;
			VertPool++;
		}
		Node++;
	}

	// Remap points.
	n=0; 
	for( i=0; i<Model->Points->Num; i++ ) if( PointRef[i]!=INDEX_NONE )
	{
		Model->Points(n) = Model->Points(i);
		PointRef[i] = n++;
	}
	debugf( LOG_Info, "Points: %i -> %i", Model->Points->Num, n );
	Model->Points->Num = n;

	// Remap vectors.
	n=0; for (i=0; i<Model->Vectors->Num; i++) if (VectorRef[i]!=INDEX_NONE)
	{
		Model->Vectors(n) = Model->Vectors(i);
		VectorRef[i] = n++;
	}
	debugf( LOG_Info, "Vectors: %i -> %i", Model->Vectors->Num, n );
	Model->Vectors->Num = n;

	// Update Bsp surfs.
	for( i=0; i<Model->Surfs->Num; i++ )
	{
		FBspSurf *Surf	= &Model->Surfs(i);
		Surf->vNormal   = VectorRef [Surf->vNormal  ];
		Surf->vTextureU = VectorRef [Surf->vTextureU];
		Surf->vTextureV = VectorRef [Surf->vTextureV];
		Surf->pBase     = PointRef  [Surf->pBase    ];
	}

	// Update Bsp nodes.
	for( i=0; i<Model->Nodes->Num; i++ )
	{
		FBspNode	*Node		= &Model->Nodes(i);
		FVert		*VertPool	= &Model->Verts(Node->iVertPool);
		for( B=0; B<Node->NumVertices;  B++ )
		{			
			VertPool->pVertex = PointRef [VertPool->pVertex];
			VertPool++;
		}
		Node++;
	}

	// Shrink the objects.
	Model->Nodes->Shrink();
	Model->Surfs->Shrink();
	Model->Points->Shrink();
	Model->Vectors->Shrink();
	Model->Verts->Shrink();

	Mark.Pop();
	unguard;
}

/*---------------------------------------------------------------------------------------
   The End.
---------------------------------------------------------------------------------------*/
