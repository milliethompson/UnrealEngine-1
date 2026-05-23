/*=============================================================================
	UnBsp.cpp: Unreal engine Bsp-related functions.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*---------------------------------------------------------------------------------------
	Globals
---------------------------------------------------------------------------------------*/

//
// Status of filtered polygons:
//
enum EPolyNodeFilter
{
   F_OUTSIDE,              // Leaf is an exterior leaf (visible to viewers).
   F_INSIDE,               // Leaf is an interior leaf (non-visible, hidden behind backface).
   F_COPLANAR_OUTSIDE,     // Poly is coplanar and in the exterior (visible to viewers).
   F_COPLANAR_INSIDE,      // Poly is coplanar and inside (invisible to viewers).
   F_COSPATIAL_FACING_IN,  // Poly is coplanar, cospatial, and facing in.
   F_COSPATIAL_FACING_OUT, // Poly is coplanar, cospatial, and facing out.
};

//
// Generic filter function called by BspFilterEdPolys.  A and B are pointers
// to any integers that your specific routine requires (or NULL if not needed).
//
typedef void (*BSP_FILTER_FUNC) (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly,
   EPolyNodeFilter Leaf, ENodePlace ENodePlace);

//
// Information used by FilterEdPoly
//
class FCoplanarInfo
   {
   public:
   INDEX          iOriginalNode; // INDEX_NONE = no coplanars are tracked
   INDEX          iBackNode;
   int	          BackNodeOutside;
   int	          FrontLeafOutside;
   int            ProcessingBack;
   };

//
// Function to filter an EdPoly through the Bsp, calling a callback
// function for all chunks that fall into leaves.
//
void FilterEdPoly (BSP_FILTER_FUNC FilterFunc, IModel *ModelInfo,
   INDEX iNode, FPoly *EdPoly, FCoplanarInfo CoplanarInfo, int Outside);

//
// Bsp statistics used by link topic function.
//
class FBspStats
	{
	public:
	int   Polys;      // Number of BspSurfs
	int   Nodes;      // Total number of Bsp nodes
	int   MaxNodes;   // Maximum nodes the tree can hold
	int   MaxDepth;   // Maximum tree depth
	int   AvgDepth;   // Average tree depth
	int   Branches;   // Number of nodes with two children
	int   Coplanars;  // Number of nodes holding coplanar polygons (in same plane as parent)
	int   Fronts;     // Number of nodes with only a front child
	int   Backs;      // Number of nodes with only a back child
	int   Leaves;     // Number of nodes with no children
	int   FrontLeaves;// Number of leaf nodes that are in front of their parent
	int   BackLeaves; // Number of leaf nodes that are in back of their parent
	int   DepthCount; // Depth counter (used for finding average depth)
	} GBspStats;

//
// Global variables shared between BspBrushCSG and AddWorldToBrushFunc.  These are very
// tightly tied into the function AddWorldToBrush, not general-purpose.
//
int			GDiscarded;		// Number of polys discarded and not added
INDEX		GNode;          // Node AddBrushToWorld is adding to
INDEX		GLastCoplanar;	// Last coplanar beneath GNode at start of AddWorldToBrush
INDEX		GNumBspNodes;	// Number of Bsp nodes at start of AddWorldToBrush
IModel		*GModelInfo;	// Level map Model we're adding to

/*---------------------------------------------------------------------------------------
	Utility functions
---------------------------------------------------------------------------------------*/

//
// Returns the dot product of the EdPoly's normal with the Bsp node's normal.
// This is useful in determining whether coplanars are facing in same direction
// (Dot>0) or opposite direction (Dot<0)
//
FLOAT bspNormalDot (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly)
   {
   FBspNode    *Node = &ModelInfo->BspNodes[iNode];
   FBspSurf    *Surf = &ModelInfo->BspSurfs[Node->iSurf];
   FVector     *NodeNormal;
   FLOAT       NormalDot;
   //
   NodeNormal=&ModelInfo->FVectors[Surf->vNormal];
   NormalDot =
      (
      NodeNormal->X * EdPoly->Normal.X +
      NodeNormal->Y * EdPoly->Normal.Y +
      NodeNormal->Z * EdPoly->Normal.Z
      );
   return NormalDot;
   };

//
// Returns the dot product of two Bsp nodes' normals.
//
FLOAT bspNodeDot (IModel *ModelInfo, INDEX iNode1, INDEX iNode2)
	{
	FVector     *Node1Normal,*Node2Normal;
	FLOAT       NormalDot;
	//
	Node1Normal = &ModelInfo->FVectors[ModelInfo->BspSurfs[ModelInfo->BspNodes[iNode1].iSurf].vNormal];
	Node2Normal = &ModelInfo->FVectors[ModelInfo->BspSurfs[ModelInfo->BspNodes[iNode2].iSurf].vNormal];
	//
	NormalDot =
		(
		Node1Normal->X * Node2Normal->X +
		Node1Normal->Y * Node2Normal->Y +
		Node1Normal->Z * Node2Normal->Z
		);
	return NormalDot;
	};

/*-----------------------------------------------------------------------------
   Point and Vector table functions
-----------------------------------------------------------------------------*/

//
// Add a new point to the model (preventing duplicates) and return its
// index.
//
INDEX AddThing (FVector *FVectors, INT *Num, INT Max, FVector *V, FLOAT Thresh,int Check)
	{
	FLOAT    Temp;
	FVector  *TableVect;
	INDEX    n = *Num;
	//
	if (Check) // See if this is very close to an existing point/vector:
		{
		TableVect = &FVectors[0];
		for (INDEX i=0; i<n; i++)
			{
			Temp=(V->X - TableVect->X);
			if ((Temp > -Thresh) && (Temp < Thresh))
				{
				Temp=(V->Y - TableVect->Y);
				if ((Temp > -Thresh) && (Temp < Thresh))
					{
					Temp=(V->Z - TableVect->Z);
					if ((Temp > -Thresh) && (Temp < Thresh))
						{
						return i; // Found nearly-matching vector
						};
					};
				};
			TableVect++;
			};
		};
	//
	// Create new point:
	//
	if (n >= Max) appErrorf("Point/Vector table full (%i/%i)!",n,Max);
	//
	FVectors[n]				= *V;
	FVectors[n].Flags 		= 0;
	FVectors[n].iTransform 	= MAXWORD;
	//
	if 		((V->Y==0.0)&&(V->Z==0.0))	FVectors[n].Align = FVA_X;
	else if ((V->X==0.0)&&(V->Z==0.0))	FVectors[n].Align = FVA_Y;
	else if ((V->X==0.0)&&(V->Y==0.0))	FVectors[n].Align = FVA_Z;
	else 								FVectors[n].Align = FVA_None;
	//
	(*Num)++;
	//
	return n;
	};

//
// Add a new vector to the model, merging near-duplicates,  and return its index.
//
INDEX FGlobalEditor::bspAddVector (IModel *ModelInfo, FVector *V, int Normal)
	{
	GUARD;
	FLOAT Thresh;
	//
	if (Normal)  Thresh = THRESH_NORMALS_ARE_SAME;
	else         Thresh = THRESH_VECTORS_ARE_NEAR;
	//
	return AddThing (ModelInfo->FVectors,&ModelInfo->NumVectors,ModelInfo->MaxVectors,V,Thresh,1);
	UNGUARD("bspAddVector");
	};

//
// Add a new point to the model, merging near-duplicates,  and return its index.
//
INDEX FGlobalEditor::bspAddPoint (IModel *ModelInfo, FVector *V, int Exact)
	{
	GUARD;
	//
	FLOAT Thresh;
	if (Exact) Thresh = THRESH_POINTS_ARE_SAME;
	else       Thresh = THRESH_POINTS_ARE_NEAR;
	//
	// Try to find a match quickly from the Bsp. This finds all potential matches
	// except for any dissociated from nodes/surfaces during a rebuild.
	//
	FVector Temp;
	INDEX pVertex;
	FLOAT NearestDist = ModelInfo->FindNearestVertex(*V,Temp,Thresh,pVertex);
	if ((NearestDist >= 0.0) && (NearestDist <= Thresh)) return pVertex;
	//
	// No match found; add it slowly to find duplicates:
	//
	return AddThing
		(
		ModelInfo->FPoints,&ModelInfo->NumPoints,ModelInfo->MaxPoints,V,Thresh,
		!FastRebuild
		);
	UNGUARD("bspAddPoint");
	};

/*-----------------------------------------------------------------------------
	Adding polygons to the Bsp
----------------------------------------------------------------------------*/

//
// Add an editor polygon to the Bsp, and also stick a reference to it
// in the editor polygon's BspNodes list. If the editor polygon has more sides
// than the Bsp will allow, split it up into several sub-polygons.
//
// Returns: Index to newly-created node of Bsp.  If several nodes were created because
// of split polys, returns the parent (highest one up in the Bsp).
//
INDEX FGlobalEditor::bspAddNode (IModel *ModelInfo, INDEX iParent, ENodePlace ENodePlace,
	DWORD NodeFlags, FPoly *EdPoly)
	{
	GUARD;
	FBspNode		*Node,*ParentNode;
	FBspSurf		*Surf;
	FVertPool    	*VertPool;
	FPoly			*EdPoly1, *EdPoly2;
	INDEX			iNode,iTemp;
	//
	if (ENodePlace==NODE_Plane)
		{
		//
		// Make sure coplanars are added at the end of the coplanar list so that 
		// we don't insert NF_NEW nodes with non NF_NEW coplanar children:
		//
		while (ModelInfo->BspNodes[iParent].iPlane != INDEX_NONE)
			{
			iParent = ModelInfo->BspNodes[iParent].iPlane;
			};
		};
	Surf = &ModelInfo->BspSurfs [EdPoly->iLink];
	if (EdPoly->iLink == ModelInfo->NumBspSurfs)
		{
		//
		// This node has a new polygon being added by BspBrushCSG; must set its
		// properties here:
		//
		if (++ModelInfo->NumBspSurfs >= ModelInfo->MaxBspSurfs) appError ("BspSurfs full");
		//
		Surf->pBase     	= bspAddPoint  (ModelInfo,&EdPoly->Base,1);
		Surf->vNormal   	= bspAddVector (ModelInfo,&EdPoly->Normal,1);
		Surf->vTextureU 	= bspAddVector (ModelInfo,&EdPoly->TextureU,0);
		Surf->vTextureV 	= bspAddVector (ModelInfo,&EdPoly->TextureV,0);
		Surf->Texture  		= EdPoly->Texture;
		Surf->iLightMesh  	= INDEX_NONE;
		Surf->iActor		= INDEX_NONE;
		Surf->LastStartY	= 0;
		Surf->LastEndY		= 0;
		//
		Surf->PanU 		 	= EdPoly->PanU;
		Surf->PanV 		 	= EdPoly->PanV;
		Surf->PolyFlags 	= EdPoly->PolyFlags & ~PF_NoAddToBSP;
		Surf->Brush	 		= EdPoly->Brush;
		Surf->iBrushPoly	= EdPoly->iBrushPoly;
		}
	else if (EdPoly->iLink==INDEX_NONE) appError ("BspAddNode: iLink==INDEX_NONE");
	//
	// Set NodeFlags:
	//
	if (Surf->PolyFlags & PF_NotSolid)  NodeFlags |= NF_NotCsg;
	if (Surf->PolyFlags & PF_Invisible) NodeFlags |= NF_NotLightBlocking;
	if (Surf->PolyFlags & PF_Masked)    NodeFlags |= NF_ShootThrough;
	//
	if (EdPoly->NumVertices > FBspNode::MAX_NODE_VERTICES)
		{
		//
		// Split up into two coplanar sub-polygons (one with MAX_NODE_VERTICES vertices and
		// one with all the remaining vertices) and recursively add them.
		//
		if ((ModelInfo->NumFPolys+2) >= ModelInfo->MaxFPolys) appError ("BspAddNode: EdPoly overflow!");
		//
		EdPoly1   = &ModelInfo->FPolys[ModelInfo->NumFPolys++];
		*EdPoly1  = *EdPoly;                       // Copy entire contents
		EdPoly1->NumVertices = FBspNode::MAX_NODE_VERTICES;  // Keep first six verts, drop others
		//
		EdPoly2  = &ModelInfo->FPolys[ModelInfo->NumFPolys++];
		*EdPoly2 = *EdPoly;
		EdPoly2->NumVertices = EdPoly->NumVertices + 2 - FBspNode::MAX_NODE_VERTICES;
		//
		// Copy first vertex then the remaining vertices.
		//
		memmove
			(
			&EdPoly2->Vertex[1],
			&EdPoly->Vertex [FBspNode::MAX_NODE_VERTICES - 1],
			(EdPoly->NumVertices + 1 - FBspNode::MAX_NODE_VERTICES) * sizeof (FVector)
			);
		iNode=bspAddNode(ModelInfo,iParent,ENodePlace,NodeFlags,EdPoly1); // Add this poly first
		iTemp=bspAddNode(ModelInfo,iNode,  NODE_Plane,NodeFlags,EdPoly2); // Then add other (may be bigger)
		//
		ModelInfo->NumFPolys -= 2; // Remove those temp EdPolys
		//
		return iNode; // Return coplanar "parent" node (not coplanar child)
		}
	else
		{
		if ((ModelInfo->NumBspNodes+1) >= ModelInfo->MaxBspNodes) appError ("BspNodes full");
		//
		// Tell transaction tracking system that parent is about to be modified unless this
		// node is the root:		
		//
		if ((ENodePlace != NODE_Root) && (ModelInfo->Trans)) GTrans->NoteBspNode (ModelInfo,iParent);
		//
		// Set node properties:
		//
		iNode				= ModelInfo->NumBspNodes++;
		Node              	= &ModelInfo->BspNodes [iNode];
		Node->iSurf       	= EdPoly->iLink;
		Node->iDynamic[0]	= INDEX_NONE;
		Node->iDynamic[1] 	= INDEX_NONE;
		Node->NodeFlags   	= NodeFlags;
		Node->iBound		= INDEX_NONE;
		Node->iZone[0]		= 0;
		Node->iZone[1]		= 0;
		Node->ZoneMask		= 0;
		//
		// Set node plane:
		//
		Node->Plane.MakePlane(EdPoly->Base,EdPoly->Normal);
		//
		// Set up vertex pool for node:
		//
		Node->iVertPool 	= ModelInfo->NumVertPool;
		VertPool        	= &ModelInfo->VertPool[Node->iVertPool];
		//
		if ((ModelInfo->NumVertPool + (int)Node->NumVertices) >= ModelInfo->MaxVertPool) appErrorf("AddPointToNode: Vertex pool overflow (%i+%i > %i)",ModelInfo->NumVertPool,Node->NumVertices,ModelInfo->MaxVertPool);
		ModelInfo->NumVertPool += EdPoly->NumVertices;
		//
		// Init links:
		//
		Node->iFront		= INDEX_NONE;
		Node->iBack			= INDEX_NONE;
		Node->iPlane		= INDEX_NONE;
		//
		ParentNode=&ModelInfo->BspNodes[iParent];
		//
		// Link parent to this node:
		//
		if      (ENodePlace==NODE_Front)  ParentNode->iFront = iNode;
		else if (ENodePlace==NODE_Back)   ParentNode->iBack  = iNode;
		else if (ENodePlace==NODE_Plane)  ParentNode->iPlane = iNode;
		else if (ENodePlace==NODE_Root)   {};
		//
		// Add all points to point table, merging nearly-overlapping polygon points
		// with other points in the poly to prevent criscrossing vertices from
		// being generated.
		//
		// Must maintain Node->NumVertices on the fly so that bspAddPoint is always
		// called with the Bsp in a clean state.
		//
		BYTE n            = EdPoly->NumVertices;
		Node->NumVertices = 0;
		//
		for (BYTE i=0; i<n; i++)
			{
			int pVertex = bspAddPoint(ModelInfo,&EdPoly->Vertex[i],0);
			if ((Node->NumVertices==0) || (VertPool[Node->NumVertices-1].pVertex!=pVertex))
				{
				VertPool[Node->NumVertices].iSide   = INDEX_NONE; // No side link
				VertPool[Node->NumVertices].pVertex = pVertex;
				Node->NumVertices++;
				};
			};
		if ((Node->NumVertices >= 2) && (VertPool[0].pVertex==VertPool[Node->NumVertices-1].pVertex))
			{
			Node->NumVertices--;
			};
		if (Node->NumVertices < 3)
			{
			debugf(LOG_Bsp,"bspAddNode: Infinitesimal polygon %i (%i)",Node->NumVertices,n);
			Node->NumVertices = 0;
			};
		return iNode;
		};
	UNGUARD("bspAddNode");
	};

/*-----------------------------------------------------------------------------
	Bsp Splitting
-----------------------------------------------------------------------------*/

//
// Find the best splitting polygon within a pool of polygons, and return its
// index (into the PolyList array).
//
INDEX FindBestSplit (IModel *ModelInfo, int NumPolys,INDEX *PolyList,EBspOptimization Opt,
	int Balance)
	{
	GUARD;
	FPoly   *Poly;
	FLOAT   Score,BestScore;
	int		Best;
	int     i,Index,j,Inc;
	int     Splits,Front,Back,Coplanar,AllSemiSolids;
	//
	if (NumPolys<=0) appError ("FindBestSplit: NumPolys<=0");
	if (NumPolys==1) return 0; // No need to test if only one poly
	//
	if		(Opt==BSP_Optimal)  Inc = 1;					// Test all nodes
	else if (Opt==BSP_Good)		Inc = Max(1,NumPolys/20);	// Test 20 nodes
	else /* BSP_Lame */			Inc = Max(1,NumPolys/4);	// Test 4 nodes
	//
	// See if there are any non-semisolid polygons here:
	//
	for (i=0; i<NumPolys; i++)
		{
		if (!(ModelInfo->FPolys[PolyList[i]].PolyFlags & PF_AddLast)) break;
		};
	AllSemiSolids = (i>=NumPolys);
	//
	// Search through all polygons in the pool and find:
	// A. The number of splits each poly would make.
	// B. The number of front and back nodes the polygon would create.
	// C. Number of coplanars.
	//
	Best      = -1;
	BestScore = 0;
	for (i=0; i<NumPolys; i+=Inc)
		{
		Splits   = 0;
		Front    = 0;
		Back     = 0;
		Coplanar = 0;
		Index    = i-1;
		//
		do	{
			Index++;
			Poly = &ModelInfo->FPolys[PolyList[Index]];
			} while ((Index<(i+Inc)) && (Index<NumPolys) && (Poly->PolyFlags & PF_AddLast) && (!AllSemiSolids));
		if ((Index>=(i+Inc))||(Index>=NumPolys)) continue;
		//
		for (j=0; j<NumPolys; j+=Inc) if (j!=Index)
			{
			FPoly *OtherPoly = &ModelInfo->FPolys[PolyList[j]];
			switch (OtherPoly->SplitWithPlane (Poly->Base,Poly->Normal,NULL,NULL,0))
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
					if (!(Poly->PolyFlags & PF_Portal)) Splits++;
					else Splits += 16;
					break;
				};
			};
		// Highly reward splitters that cleanly partition zones.
		// Disfavor splitters that split zones in half.
		FLOAT Bonus=0;
		/*
		int AllFront=0,AllBack=0;
		for( int i=0; i<UBspNodes::MAX_ZONES; i++)
			{
			AllFront += (SideZones[i]==SIDE_Front);
			AllBack  += (SideZones[i]==SIDE_Back );
			};
		if((AllFront>0)&&(AllBack>0)) Bonus = 100.0 * 1000.0 * (AllFront+AllBack);
		*/
		//
		Score = 
			(FLOAT)(Balance * Abs(Front-Back) +	(100-Balance)*Splits - Bonus) / 
			(0.001 + Poly->Area()
			);
		if ((Score < BestScore) || (Best==-1))
			{
			Best      = Index;
			BestScore = Score;
			};
		};
	if (Best==-1) appError ("Decision failed");
	return Best;
	UNGUARD("FindBestSplit");
	};

//
// Pick a splitter poly then split a pool of polygons into front and back polygons and
// recurse.
//
// iParent = Parent Bsp node, or INDEX_NONE if this is the root node.
// IsFront = 1 if this is the front node of iParent, 0 of back (undefined if iParent==INDEX_NONE)
//
void SplitPolyList (IModel *ModelInfo, INDEX iParent, ENodePlace ENodePlace,
	INDEX NumPolys,INDEX *PolyList,EBspOptimization Opt,int Balance,int RebuildSimplePolys)
	{
	GUARD;
	FPoly    *SplitPoly,*EdPoly,*FrontEdPoly,*BackEdPoly;
	INDEX    *FrontList,*BackList;
	INDEX    iSplitPoly,iOurNode,iPlaneNode;
	int      NumFront,NumBack,NumPolysToAlloc;
	//
	if ((ModelInfo->NumFPolys+2)>=ModelInfo->MaxFPolys) appError ("EdPoly table full!");
	//
	NumPolysToAlloc = NumPolys + 8 + NumPolys/4; // To account for big EdPolys split up
	//
	NumFront=0; FrontList = (INDEX *)GMem.Get(NumPolysToAlloc * sizeof(INDEX));
	NumBack =0; BackList  = (INDEX *)GMem.Get(NumPolysToAlloc * sizeof(INDEX));
	//
	iSplitPoly	= FindBestSplit (ModelInfo,NumPolys,PolyList,Opt,Balance);
	SplitPoly	= &ModelInfo->FPolys[PolyList[iSplitPoly]];
	//
	// Add the splitter poly to the Bsp with either a new BspSurf or an existing one:
	//
	if (RebuildSimplePolys) SplitPoly->iLink = ModelInfo->NumBspSurfs;
	//
	iOurNode	= GUnrealEditor.bspAddNode(ModelInfo,iParent,ENodePlace,0,SplitPoly);
	iPlaneNode  = iOurNode;
	//
	// Now divide all polygons in the pool into (A) polygons that are
	// in front of Poly, and (B) polygons that are in back of Poly.
	// Coplanar polys are inserted immediately, before recursing.
	//
	// If any polygons are split by Poly, we ignrore the original poly,
	// split it into two polys, and add two new polys to the pool.
	//
	for (INDEX i=0; i<NumPolys; i++) if (i != iSplitPoly)
		{
		EdPoly 		= &ModelInfo -> FPolys[PolyList[i]];
		FrontEdPoly	= &ModelInfo -> FPolys[ModelInfo->NumFPolys];
		BackEdPoly  = &ModelInfo -> FPolys[ModelInfo->NumFPolys+1];
		//
		switch (EdPoly->SplitWithPlane
      		(
			SplitPoly -> Base,
			SplitPoly -> Normal,
			FrontEdPoly,
			BackEdPoly,
			0))
			{
			case SP_Coplanar:
	            if (RebuildSimplePolys) EdPoly->iLink = ModelInfo->NumBspSurfs-1;
				iPlaneNode = GUnrealEditor.bspAddNode(ModelInfo,iPlaneNode,NODE_Plane,0,EdPoly);
				break;
			case SP_Front:
	            FrontList[NumFront++]=PolyList[i];
				break;
			case SP_Back:
	            BackList [NumBack++] =PolyList[i];
				break;
			case SP_Split:
	            //
				// This polygon is split.  Ignore this polygon and add in the split
				// versions of it entirely front and entirely back components.
				//
				// Create front & back nodes:
				//
				FrontList[NumFront++]=ModelInfo->NumFPolys++;
				BackList [NumBack ++]=ModelInfo->NumFPolys++;
				//
				// If newly-split polygons have too many vertices, break them up in half:
				//
				if (FrontEdPoly->NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD)
					{
					FrontEdPoly->SplitInHalf(&ModelInfo->FPolys[ModelInfo->NumFPolys]);
					FrontList[NumFront++]=ModelInfo->NumFPolys++;
					};
				if (BackEdPoly->NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD)
					{
					BackEdPoly->SplitInHalf(&ModelInfo->FPolys[ModelInfo->NumFPolys]);
					BackList[NumBack++]=ModelInfo->NumFPolys++;
					};
				break;
			};
		};
	//
	// Recursively split the front and back pools:
	//
	if (NumFront>0) SplitPolyList (ModelInfo,iOurNode,NODE_Front,NumFront,FrontList,Opt,Balance,RebuildSimplePolys);
	if (NumBack>0)  SplitPolyList (ModelInfo,iOurNode,NODE_Back ,NumBack, BackList, Opt,Balance,RebuildSimplePolys);
	//
	GMem.Release (FrontList);
	UNGUARD("SplitPolyList");
	};

/*-----------------------------------------------------------------------------
	High-level Bsp functions
-----------------------------------------------------------------------------*/

//
// Build Bsp from the editor polygon set (EdPolys) of a model.
//
// Opt     = Bsp optimization, Bsp_LAME (fast), Bsp_GOOD (medium), Bsp_OPTIMAL (slow)
// Balance = 0-100, 0=only worry about minimizing splits, 100=only balance tree.
//
void FGlobalEditor::bspBuild (UModel *Model, EBspOptimization Opt, int Balance, int RebuildSimplePolys)
	{
	GUARD;
	IModel		ModelInfo;
	FPoly       *EdPoly;
	INDEX       i,*PolyList;
	int         NumFPolys,OrigEdPolys;
	//
	// Empty the model's tables:
	//
	Model->Lock(&ModelInfo,LOCK_NoTrans);
	if (RebuildSimplePolys)
		{
		// Empty everything
		ModelInfo.Empty(1);
		}
	else
		{
		// Empty node vertices:
		for (int i=0; i<ModelInfo.NumBspNodes; i++) ModelInfo.BspNodes[i].NumVertices=0;
		bspRefresh(&ModelInfo,2);
		// Empty nodes:
		ModelInfo.Empty(0);
		};
	if (!ModelInfo.NumFPolys) goto Done;
	if (ModelInfo.ModelFlags & MF_InvalidBsp) goto Done;
	//
	// Allocate polygon pool:
	//
	PolyList = (INDEX *)GMem.Get(ModelInfo.NumFPolys * sizeof (INDEX));
	//
	// Add all EdPolys to active list
	//
	NumFPolys = 0;
	EdPoly    = &ModelInfo.FPolys[0];
	for (i=0; i<ModelInfo.NumFPolys; i++)
		{
		if (EdPoly->NumVertices!=0) PolyList[NumFPolys++]=i;
		EdPoly++;
		};
	//
	// Now split the entire Bsp by splitting the list of all polygons.
	// This function will add extra polygons to EdPolys, so we compensate
	// by restoring Level->NumFPolys before exiting.
	//
	OrigEdPolys = ModelInfo.NumFPolys;
	SplitPolyList (&ModelInfo,INDEX_NONE,NODE_Root,NumFPolys,PolyList,Opt,Balance,RebuildSimplePolys);
	ModelInfo.NumFPolys = OrigEdPolys; // Restore polygon count to ditch temp EdPolys
	//
	GMem.Release(PolyList);
	//
	// Now build the bounding boxes for all nodes:
	//
	if (!RebuildSimplePolys)
		{
		bspRefresh(&ModelInfo,1); // Remove unreferenced things
		bspBuildBounds (&ModelInfo); // Rebuild all bounding boxes
		};
	Done:
	Model->Unlock(&ModelInfo);
	UNGUARD("BspBuild");
	};

/*----------------------------------------------------------------------------
   Brush validate
----------------------------------------------------------------------------*/

//
// Validate a brush, and set iLinks on all EdPolys to index of the
// first identical EdPoly in the list, or its index if it's the first.
// Not transactional.
//
void FGlobalEditor::bspValidateBrush (UModel *Brush,int ForceValidate,int DoStatusUpdate)
	{
	GUARD;
	IModel			ModelInfo;
	FPoly 			*EdPoly,*OtherPoly;
	FLOAT			Dist;
	INDEX			i,j,n=0;
	//
	Brush->Lock(&ModelInfo,LOCK_Trans);
	if (ForceValidate || !(ModelInfo.ModelFlags & MF_Linked))
		{
		ModelInfo.ModelFlags |= MF_Linked;
		//
		for (i=0; i<ModelInfo.NumFPolys; i++)
			{
			ModelInfo.FPolys[i].iLink = i;
			};
		for (i=0; i<ModelInfo.NumFPolys; i++)
			{
			if (DoStatusUpdate && !(i&7))
				{
				GApp->StatusUpdate("Validating Brush",i,ModelInfo.NumFPolys);
				};
			EdPoly = &ModelInfo.FPolys[i];
			if (EdPoly->iLink==i)
				{
				for (j=i+1; j<ModelInfo.NumFPolys; j++)
					{
					OtherPoly=&ModelInfo.FPolys[j];
					if ((OtherPoly->iLink == j) &&
						(OtherPoly->Texture == EdPoly->Texture) &&
						((OtherPoly->Normal | EdPoly->Normal)>0.9999)
						)
						{
						Dist = FPointPlaneDist (OtherPoly->Base,EdPoly->Base,EdPoly->Normal);
						if ((Dist>-0.001) && (Dist<0.001))
							{
							OtherPoly->iLink = i;
							n++;
							};
						};
					};
				};
			};
		//debugf (LOG_INFO,"BspValidateBrush linked %i of %i polys",n,ModelInfo.NumFPolys);
		};
	Brush->Unlock(&ModelInfo);
	//
	// Build bounds:
	//
	Brush->BuildBound(0);
	Brush->BuildBound(1);
	//
	UNGUARD("BspValidateBrush");
	};

/*----------------------------------------------------------------------------
   EdPoly building and compacting
----------------------------------------------------------------------------*/

//
// Convert a Bsp node's polygon to an EdPoly, add it to the list, and recurse.
//
void MakeEdPolys(IModel *ModelInfo,INDEX iNode)
   {
   FBspNode *Node   = &ModelInfo->BspNodes[iNode];
   FPoly    *EdPoly = &ModelInfo->FPolys[ModelInfo->NumFPolys];
   //
   if (GUnrealEditor.bspNodeToFPoly(ModelInfo,iNode,EdPoly) >= 3) ModelInfo->NumFPolys++;
   //
   if (Node->iFront!=INDEX_NONE) MakeEdPolys(ModelInfo,Node->iFront);
   if (Node->iBack !=INDEX_NONE) MakeEdPolys(ModelInfo,Node->iBack );
   if (Node->iPlane!=INDEX_NONE) MakeEdPolys(ModelInfo,Node->iPlane);
   };

//
// Trys to merge two polygons.  If they can be merged, replaces Poly1 and emptys Poly2
// and returns 1.  Otherwise, returns 0.
//
int TryToMerge (FPoly *Poly1, FPoly *Poly2)
	{
	GUARD;
	FPoly    NewPoly;
	int      Start1,Start2,Test1,Test2,i,End1,End2,Vertex;
	//
	// Don't even consider merging polys that would result in a merged poly
	// with more sides than we can handle:
	//
	if (((Poly1->NumVertices)+(Poly2->NumVertices)) > FPoly::MAX_FPOLY_VERTICES) return 0;
	//
	// Find one overlapping point:
	//
	for (Start1=0; Start1<Poly1->NumVertices; Start1++)
		for (Start2=0; Start2<Poly2->NumVertices; Start2++)
			if (FPointsAreSame(Poly1->Vertex[Start1],Poly2->Vertex[Start2]))
				goto FoundOverlap;
	return 0; // Couldn't merge, no overlapping points.
	//
	FoundOverlap:
	//
	End1  = Start1;
	End2  = Start2;
	//
	Test1 = Start1+1; if (Test1>=Poly1->NumVertices) Test1 = 0;
	Test2 = Start2-1; if (Test2<0)                   Test2 = Poly2->NumVertices-1;
	//
	if (FPointsAreSame(Poly1->Vertex[Test1],Poly2->Vertex[Test2]))
		{
		End1   = Test1;
		Start2 = Test2;
		}
	else
		{
		Test1 = Start1-1; if (Test1<0)                   Test1=Poly1->NumVertices-1;
		Test2 = Start2+1; if (Test2>=Poly2->NumVertices) Test2=0;
		//
		if (FPointsAreSame(Poly1->Vertex[Test1],Poly2->Vertex[Test2]))
			{
			Start1 = Test1;
			End2   = Test2;
			}
		else return 0; // Only one point overlapped
		};
	//
	// Build a new edpoly containing both polygons merged:
	//
	NewPoly             = *Poly1;
	NewPoly.NumVertices = 0;
	//
	Vertex              = End1;
	for (i=0; i<Poly1->NumVertices; i++)
		{
		NewPoly.Vertex[NewPoly.NumVertices++] = Poly1->Vertex[Vertex];
		if (++Vertex >= Poly1->NumVertices) Vertex=0;
		};
	Vertex               = End2;
	for (i=0; i<(Poly2->NumVertices-2); i++)
		{
		if (++Vertex >= Poly2->NumVertices) Vertex=0;
		NewPoly.Vertex[NewPoly.NumVertices++] = Poly2->Vertex[Vertex];
		};
	//
	// Remove colinear vertices and check convexity:
	//
	if (NewPoly.RemoveColinears())
		{
		if (NewPoly.NumVertices <= FBspNode::MAX_NODE_VERTICES)
			{
			*Poly1				= NewPoly; // Convex
			Poly2->NumVertices	= 0;
			return 1;
			}
		else return 0; // Would result in too many vertices
		}
	else return 0; // Nonconvex
	UNGUARD("TryToMerge");
	};

//
// Merge all polygons in coplanar list that can be merged convexly.
//
void MergeCoplanars (IModel *ModelInfo,INDEX *PolyList, int PolyCount)
	{
	GUARD;
	FPoly    *Poly1,*Poly2;
	int      i,j,MergeAgain;
	//
	MergeAgain=1;
	while (MergeAgain)
		{
		MergeAgain=0;
		for (i=0; i<PolyCount; i++)
			{
			Poly1 = &ModelInfo->FPolys[PolyList[i]];
			if (Poly1->NumVertices>0)
	            {
				for (j=i+1; j<PolyCount; j++)
					{
					Poly2 = &ModelInfo->FPolys[PolyList[j]];
					if (Poly2->NumVertices>0)
						{
						if ((Poly1->NumVertices + Poly2->NumVertices) <= FPoly::MAX_FPOLY_VERTICES)
							{
                  			// If merges, replaces Poly1 and emptys Poly2
                  			if (TryToMerge(Poly1,Poly2)) MergeAgain=1;
							};
						};
					};
				};
			};
		};
	UNGUARD("MergeCoplanars");
	};

//
// Build EdPoly list from a model's Bsp. Not transactional.
//
void FGlobalEditor::bspBuildFPolys (UModel *Model,int iSurfLinks)
	{
	GUARD;
	IModel  ModelInfo;
	//
	Model->Lock(&ModelInfo,LOCK_NoTrans);
	//
	ModelInfo.NumFPolys = 0;
	if (ModelInfo.NumBspNodes != 0) MakeEdPolys(&ModelInfo,0);
	//
	if (!iSurfLinks) for (INDEX i=0; i<ModelInfo.NumFPolys; i++) ModelInfo.FPolys[i].iLink=i;
	//
	Model->Unlock(&ModelInfo);
	UNGUARD("BspBuildEdPolys");
	};

//
// Merge all coplanar EdPolys in a model.  Not transactional.
// Preserves (though reorders) iLinks.
//
void FGlobalEditor::bspMergeCoplanars (UModel *Model,int RemapLinks,int MergeDisparateTextures)
	{
	GUARD;
	IModel		ModelInfo;
	FPoly 		*EdPoly,*OtherPoly;
	FLOAT		Dist;
	INDEX		*PolyList,*Remap;
	INDEX		i,j,n,PolyCount,OriginalNum;
	//
	Model->Lock(&ModelInfo,LOCK_NoTrans);
	OriginalNum = ModelInfo.NumFPolys;
	//
	// Mark all polys as unprocessed:
	//
	for (i=0; i<ModelInfo.NumFPolys; i++) ModelInfo.FPolys[i].PolyFlags &= ~PF_EdProcessed;
	//
	// Find matching coplanars and merge them
	//
	PolyList = (INDEX *)GMem.Get(ModelInfo.MaxFPolys * sizeof (INDEX));
	//
	n=0;
	for (i=0; i<ModelInfo.NumFPolys; i++)
		{
		EdPoly=&ModelInfo.FPolys[i];
		if ((EdPoly->NumVertices > 0) && !(EdPoly->PolyFlags & PF_EdProcessed))
			{
			PolyCount             =  0;
			PolyList[PolyCount++] =  i;
			EdPoly->PolyFlags    |= PF_EdProcessed;
			//
			for (j=i+1; j<ModelInfo.NumFPolys; j++)
	            {
				OtherPoly = &ModelInfo.FPolys[j];
				if (OtherPoly->iLink==EdPoly->iLink)
					{
					OtherPoly->PolyFlags |= PF_EdProcessed;
					Dist = FPointPlaneDist (OtherPoly->Base,EdPoly->Base,EdPoly->Normal);
					if	(
							(Dist>-0.001)
						&&	(Dist<0.001)
						&&	((OtherPoly->Normal | EdPoly->Normal)>0.9999)
						&&	(
								MergeDisparateTextures ||
								(
									FPointsAreNear(OtherPoly->TextureU,EdPoly->TextureU,THRESH_VECTORS_ARE_NEAR)
								&&	FPointsAreNear(OtherPoly->TextureV,EdPoly->TextureV,THRESH_VECTORS_ARE_NEAR)
								)
							)
						)
						{
						PolyList[PolyCount++] = j;
						};
					};
				};
			if (PolyCount > 1)
	            {
				MergeCoplanars (&ModelInfo,PolyList,PolyCount);
				n++;
				};
			};
		};
	GMem.Release(PolyList);
	debugf(LOG_Bsp,"Found %i coplanar sets in %i",n,ModelInfo.NumFPolys);
	//
	// Get rid of empty EdPolys while remapping iLinks:
	//
	j=0;
	Remap = (INDEX *)GMem.Get(ModelInfo.NumFPolys * sizeof(INDEX));
	for (i=0; i<ModelInfo.NumFPolys; i++)
		{
		if (ModelInfo.FPolys[i].NumVertices)
			{
			Remap[i]			= j;
			ModelInfo.FPolys[j]	= ModelInfo.FPolys[i];
			j++;
			};
		};
	ModelInfo.NumFPolys=j;
	if (RemapLinks) for (i=0; i<ModelInfo.NumFPolys; i++)
		{
		if (ModelInfo.FPolys[i].iLink!=INDEX_NONE)
			{
			ModelInfo.FPolys[i].iLink = Remap[ModelInfo.FPolys[i].iLink];
			};
		};
	GMem.Release (Remap);
	debugf(LOG_Bsp,"BspMergeCoplanars reduced %i->%i",OriginalNum,ModelInfo.NumFPolys);
	//
	Model->Unlock(&ModelInfo);
	UNGUARD("BspMergeCoplanars");
	};

/*----------------------------------------------------------------------------
   CSG types & general-purpose callbacks
----------------------------------------------------------------------------*/

//
// Recursive worker function called by BspCleanup.
//
void CleanupNodes (IModel *ModelInfo, INDEX iNode, INDEX iParent)
	{
	FBspNode	*Node			= &ModelInfo->BspNodes[iNode];
	FVertPool	*VertPool		= &ModelInfo->VertPool[Node->iVertPool];
	FBspNode	*ParentNode,*PlaneNode,*ReplacementNode=NULL;
	INDEX		iReplacementNode;
	//
	if (ModelInfo->NumBspNodes==0) return;
	//
	if ((Node->NodeFlags & NF_TagForEmpty) && (Node->NumVertices != 0))
		{
		if (ModelInfo->Trans) GTrans->NoteBspNode (ModelInfo,iNode);
		Node->NumVertices =  0;
		Node->NodeFlags   &=	~(NF_TagForEmpty);
		};
	Node->NodeFlags &= ~(NF_IsNew | NF_Bounded);
	//
	VertPool[Node->NumVertices].iSide = INDEX_NONE; // No side link
	//
	if (Node->iFront!=INDEX_NONE) CleanupNodes(ModelInfo,Node->iFront,iNode);
	if (Node->iBack !=INDEX_NONE) CleanupNodes(ModelInfo,Node->iBack ,iNode);
	if (Node->iPlane!=INDEX_NONE) CleanupNodes(ModelInfo,Node->iPlane,iNode);
	//
	if ((Node->NumVertices == 0) && (Node->iPlane!=INDEX_NONE))
		{
		if (ModelInfo->Trans) GTrans->NoteBspNode (ModelInfo,Node->iPlane);
		//
		// If this is an empty node with a coplanar (guaranteed non-empty),
		// replace it with the coplanar.
		//
		PlaneNode = &ModelInfo->BspNodes[Node->iPlane];
		//
		// Stick our front, back, and parent nodes on the coplanar.
		//
		if (bspNodeDot(ModelInfo,iNode,Node->iPlane)>=0)
			{
			PlaneNode->iFront  = Node->iFront;
			PlaneNode->iBack   = Node->iBack;
			}
		else
			{
			PlaneNode->iFront  = Node->iBack;
			PlaneNode->iBack   = Node->iFront;
			};
		if (iParent==INDEX_NONE) // This node is the root
			{
			if (ModelInfo->Trans) GTrans->NoteBspNode (ModelInfo,iNode);
			//
			*Node                  = *PlaneNode;   // Replace root
			PlaneNode->NumVertices = 0;            // Mark as unused
			}
		else
			{
			if (ModelInfo->Trans) GTrans->NoteBspNode (ModelInfo,iParent);
			//
			ParentNode = &ModelInfo->BspNodes[iParent];
			//
			if      (ParentNode->iFront==iNode) ParentNode->iFront = Node->iPlane;
			else if (ParentNode->iBack ==iNode) ParentNode->iBack  = Node->iPlane;
			else if (ParentNode->iPlane==iNode) ParentNode->iPlane = Node->iPlane;
			else    appError ("CleanupNodes: Parent and child are unlinked");
			//
			// Node->NumVertices = 0; // Mark original as unused (removed, unneeded)
			}
		}
	else if (
		( Node->NumVertices == 0) &&
		((Node->iFront==INDEX_NONE)||(Node->iBack==INDEX_NONE)))
		{
		//
		// Delete nodes with no fronts or backs.
		// Replace nodes with only fronts.
		// Replace nodes with only backs.
		//
		if       (Node->iFront!=INDEX_NONE) iReplacementNode = Node->iFront;
		else if  (Node->iBack !=INDEX_NONE) iReplacementNode = Node->iBack;
		else                                iReplacementNode = INDEX_NONE;
		//
		if (iReplacementNode!=INDEX_NONE)   ReplacementNode=&ModelInfo->BspNodes[iReplacementNode];
		//
		if (iParent==INDEX_NONE) // Root
			{
			if (iReplacementNode==INDEX_NONE)
				{
         		ModelInfo->NumBspNodes = 0;
				}
			else
				{
				if (ModelInfo->Trans) GTrans->NoteBspNode (ModelInfo,iNode);
         		*Node = *ReplacementNode;
				};
			}
		else // Regular node
			{
			ParentNode = &ModelInfo->BspNodes [iParent];
			//
			if (ModelInfo->Trans) GTrans->NoteBspNode (ModelInfo,iParent);
			//
			if      (ParentNode->iFront==iNode) ParentNode->iFront = iReplacementNode;
			else if (ParentNode->iBack ==iNode) ParentNode->iBack  = iReplacementNode;
			else if (ParentNode->iPlane==iNode) ParentNode->iPlane = iReplacementNode;
			else    appError ("CleanupNodes: Parent and child are unlinked");
			};
		};
   };

//
// Recursive worker function called by BspCleanup.
//
void CleanupPolys (IModel *ModelInfo)
	{
	GUARD;
	FBspSurf *Poly = &ModelInfo->BspSurfs[0];
	for (INDEX i=0; i<ModelInfo->NumBspSurfs; i++)
		{
		(Poly++)->PolyFlags &= ~(PF_IsFront | PF_IsBack);
		};
	UNGUARD("CleanupPolys");
	};

//
// Clean up all nodes after a CSG operation.  Resets temporary bit flags and unlinks
// empty leaves.  Removes zero-vertex nodes which have nonzero-vertex coplanars.
//
void FGlobalEditor::bspCleanup (IModel *ModelInfo)
	{
	GUARD;
	CleanupPolys (ModelInfo);
	CleanupNodes (ModelInfo,0,INDEX_NONE);
	UNGUARD("BspCleanup");
	};

/*----------------------------------------------------------------------------
   CSG leaf filter callbacks
----------------------------------------------------------------------------*/

void AddBrushToWorldFunc (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter, ENodePlace ENodePlace)
	{
	#if 0
	switch (Filter)
		{
		case F_OUTSIDE:					bug(" -> Outside"); break;
		case F_COPLANAR_OUTSIDE:		bug(" -> Coplanar Outside"); break;
		case F_COSPATIAL_FACING_IN:		bug(" -> Cospatial facing in"); break;
		case F_COSPATIAL_FACING_OUT:	bug(" -> Cospatial facing out"); break;
		case F_INSIDE:					bug(" -> Inside"); break; 
		case F_COPLANAR_INSIDE: 		bug(" -> Coplanar inside"); break; 
		};
	#endif
	//
	switch (Filter)
		{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
		case F_COSPATIAL_FACING_OUT:
			GUnrealEditor.bspAddNode (ModelInfo,iNode,ENodePlace,NF_IsNew,EdPoly);
			break;
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
		case F_COSPATIAL_FACING_IN:
			break;
		};
	};

void AddWorldToBrushFunc (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter, ENodePlace ENodePlace)
	{
	switch (Filter)
		{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
			//
			// Only affect the world poly if it has been cut:
			//
			if (EdPoly->PolyFlags & PF_EdCut)
				{            
				GUnrealEditor.bspAddNode (GModelInfo,GLastCoplanar,NODE_Plane,NF_IsNew,EdPoly);
				};
			break;
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
		case F_COSPATIAL_FACING_IN:
		case F_COSPATIAL_FACING_OUT:
			GDiscarded++;
			GModelInfo->BspNodes[GNode].NodeFlags |= NF_TagForEmpty; // Discard original poly
			break;
		};
	};

void SubtractBrushFromWorldFunc (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter, ENodePlace ENodePlace)
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
			GUnrealEditor.bspAddNode (ModelInfo,iNode,ENodePlace,NF_IsNew,EdPoly); // Add to Bsp back
			EdPoly->Reverse();
			break;
		};
	};

void SubtractWorldToBrushFunc (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter, ENodePlace ENodePlace)
	{
	switch (Filter)
		{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
		case F_COSPATIAL_FACING_IN:
			//
			// Only affect the world poly if it has been cut:
			//
			if (EdPoly->PolyFlags & PF_EdCut)
	            {            
				GUnrealEditor.bspAddNode (GModelInfo,GLastCoplanar,NODE_Plane,NF_IsNew,EdPoly);
				};
			break;
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
		case F_COSPATIAL_FACING_OUT:
			GDiscarded++;
			GModelInfo->BspNodes[GNode].NodeFlags |= NF_TagForEmpty; // Discard original poly
			break;
		};
	};

void IntersectBrushWithWorldFunc (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter,ENodePlace ENodePlace)
	{
	switch (Filter)
		{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
		case F_COSPATIAL_FACING_IN:
		case F_COSPATIAL_FACING_OUT:
			// Ignore
			break;
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
			if (EdPoly->Fix()>=3)
	            {
				GModelInfo->FPolys[GModelInfo->NumFPolys++]=*EdPoly;
				};
			break;
		};
	};

void IntersectWorldWithBrushFunc (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter,ENodePlace ENodePlace)
	{
	switch (Filter)
		{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
		case F_COSPATIAL_FACING_IN:
			// Ignore
			break;
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
		case F_COSPATIAL_FACING_OUT:
			if (EdPoly->Fix()>=3)
	            {
				GModelInfo->FPolys[GModelInfo->NumFPolys++]=*EdPoly;
				};
			break;
		};
	};

void DeIntersectBrushWithWorldFunc (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter,ENodePlace ENodePlace)
	{
	switch (Filter)
		{
		case F_INSIDE:
		case F_COPLANAR_INSIDE:
		case F_COSPATIAL_FACING_OUT:
		case F_COSPATIAL_FACING_IN:
			// Ignore
			break;
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
			if (EdPoly->Fix()>=3)
	            {
				GModelInfo -> FPolys[GModelInfo->NumFPolys++]=*EdPoly;
				};
			break;
		};
	};

void DeIntersectWorldWithBrushFunc (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly,
	EPolyNodeFilter Filter,ENodePlace ENodePlace)
	{
	switch (Filter)
		{
		case F_OUTSIDE:
		case F_COPLANAR_OUTSIDE:
		case F_COSPATIAL_FACING_OUT:
			// Ignore
			break;
		case F_COPLANAR_INSIDE:
		case F_INSIDE:
		case F_COSPATIAL_FACING_IN:
			if (EdPoly->Fix()>=3)
	            {
				EdPoly->Reverse();
				GModelInfo->FPolys[GModelInfo->NumFPolys++]=*EdPoly;
				EdPoly->Reverse();
				};
			break;
		};
	};

/*----------------------------------------------------------------------------
   CSG polygon filtering routine (calls the callbacks)
----------------------------------------------------------------------------*/

//
// Handle a piece of a polygon that was filtered to a leaf.
//
void FilterLeaf (BSP_FILTER_FUNC FilterFunc, IModel *ModelInfo,
	INDEX iNode, FPoly *EdPoly, FCoplanarInfo CoplanarInfo, int LeafOutside, ENodePlace ENodePlace)
	{
	GUARD;
	EPolyNodeFilter    FilterType;
	//
	if (CoplanarInfo.iOriginalNode==INDEX_NONE) // Processing regular, non-coplanar polygons:
		{
		FilterType = LeafOutside ? F_OUTSIDE : F_INSIDE;
		FilterFunc (ModelInfo, iNode, EdPoly, FilterType, ENodePlace);
		}
	else if (CoplanarInfo.ProcessingBack) // Finished filtering polygon through tree in back of parent coplanar:
		{
		DoneFilteringBack:
		//
		if      ((!LeafOutside) && (!CoplanarInfo.FrontLeafOutside)) FilterType = F_COPLANAR_INSIDE;
		else if (( LeafOutside) && ( CoplanarInfo.FrontLeafOutside)) FilterType = F_COPLANAR_OUTSIDE;
		else if ((!LeafOutside) && ( CoplanarInfo.FrontLeafOutside)) FilterType = F_COSPATIAL_FACING_OUT;
		else if (( LeafOutside) && (!CoplanarInfo.FrontLeafOutside)) FilterType = F_COSPATIAL_FACING_IN;
		else
			{
			appError ("FilterLeaf: Bad Locs");
			return;
			};
		FilterFunc (ModelInfo, CoplanarInfo.iOriginalNode, EdPoly, FilterType, NODE_Plane);
		}
	else
		{
		CoplanarInfo.FrontLeafOutside = LeafOutside;
		//
		if (CoplanarInfo.iBackNode==INDEX_NONE)
			{
			LeafOutside = CoplanarInfo.BackNodeOutside;
			goto DoneFilteringBack; // Back tree is empty
			}
		else
			{
			//
			// Call FilterEdPoly to filter through the back.  This will result in
			// another call to FilterLeaf with iNode = leaf this falls into in the
			// back tree and EdPoly = the final EdPoly to insert.
			//
			CoplanarInfo.ProcessingBack=1;
			FilterEdPoly (FilterFunc,ModelInfo,CoplanarInfo.iBackNode,EdPoly,CoplanarInfo,CoplanarInfo.BackNodeOutside);
			};
		};
	UNGUARD("FilterLeaf");
	};

//
// Filter an EdPoly through the Bsp recursively, calling FilterFunc
// for all chunks that fall into leaves.  FCoplanarInfo is used to
// handle the tricky case of double-recursion for polys that must be
// filtered through a node's front, then filtered through the node's back,
// in order to handle coplanar CSG properly.
//
void FilterEdPoly (BSP_FILTER_FUNC FilterFunc, IModel *ModelInfo,
	INDEX iNode, FPoly *EdPoly, FCoplanarInfo CoplanarInfo, int Outside)
	{
	GUARD;
	FPoly          *TempFrontEdPoly, *TempBackEdPoly;
	FBspNode       *Node;
	FBspSurf       *Poly;
	INDEX          SplitResult,iOurFront,iOurBack;
	int			   NewFrontOutside,NewBackOutside;
	//
	FilterLoop:
	//
	if (EdPoly->NumVertices >= FPoly::FPOLY_VERTEX_THRESHOLD)
		{
		//
		// Must split EdPoly in half to prevent vertices from overflowing
		// in edpolySplitWithPlane:
		//
		TempFrontEdPoly = &ModelInfo->FPolys  [ModelInfo->NumFPolys];
		EdPoly->SplitInHalf(TempFrontEdPoly);
		//
		// Filter other half:
		//
		ModelInfo->NumFPolys++;
		FilterEdPoly (FilterFunc,ModelInfo,iNode,TempFrontEdPoly,CoplanarInfo,Outside);
		ModelInfo->NumFPolys--;
		};
	if (((ModelInfo->NumBspNodes+2)>=ModelInfo->MaxBspNodes) ||
		((ModelInfo->NumFPolys+2)>=ModelInfo->MaxFPolys))
		{
		appError ("FilterEdPoly: EdPoly table is full, world is too big");
		};
	//
	// See if we've precomputed whether this is in front or back
	// based on a bounding sphere:
	//
	Node = &ModelInfo->BspNodes[iNode];
	Poly = &ModelInfo->BspSurfs[Node->iSurf];
	//
	if (0) // Filter is not being precomputed anymore (Poly->PolyFlags & (PF_IsFront | PF_IsBack)) // Was precomputed 
		{
		if (Poly->PolyFlags & PF_IsFront)	SplitResult = SP_Front;
		else								SplitResult = SP_Back;
		}
	else // Wasn't precomputed, must check now:
		{
		TempFrontEdPoly	= &ModelInfo->FPolys [ModelInfo->NumFPolys];
		TempBackEdPoly	= &ModelInfo->FPolys [ModelInfo->NumFPolys+1];
		//
		SplitResult = EdPoly->SplitWithPlane (
			ModelInfo->FPoints  [Poly->pBase],
			ModelInfo->FVectors [Poly->vNormal],
			TempFrontEdPoly,
			TempBackEdPoly,
			0);
		};
	//
	// Process split results
	//
	if (SplitResult==SP_Front)
		{
		Front:
		//
		Outside = Outside || Node->IsCsg();
		if (Node->iFront==INDEX_NONE)
			{
			FilterLeaf (FilterFunc,ModelInfo,iNode,EdPoly,CoplanarInfo,Outside,NODE_Front);
			}
		else
			{
			iNode=Node->iFront;
			goto FilterLoop;
			}
		}
	else if (SplitResult==SP_Back)
		{
		Outside = Outside && !Node->IsCsg();
		//
		if (Node->iBack==INDEX_NONE)
			{
			FilterLeaf (FilterFunc,ModelInfo,iNode,EdPoly,CoplanarInfo,Outside,NODE_Back);
			}
		else
			{
			iNode=Node->iBack;
			goto FilterLoop;
			};
		}
	else if (SplitResult==SP_Coplanar)
		{
		if (CoplanarInfo.iOriginalNode!=INDEX_NONE)
			{
			//
			// This will happen once in a blue moon when a polygon is barely outside the
			// coplanar threshold and is split up into a new polygon that is
			// is barely inside the coplanar threshold.  To handle this, just classify
			// it as front and it will be handled propery.
			//
			debug (LOG_Bsp,"FilterEdPoly: Encountered out-of-place coplanar");
			goto Front;
			};
		CoplanarInfo.iOriginalNode        = iNode;
		CoplanarInfo.iBackNode            = INDEX_NONE;
		CoplanarInfo.ProcessingBack       = 0;
		CoplanarInfo.BackNodeOutside      = Outside; // Default
		NewFrontOutside                   = Outside; // Default
		//
		// See whether Node's iFront or iBack points to the side of the tree on the front
		// of this polygon (will be as expected if this polygon is facing the same
		// way as first coplanar in link, otherwise opposite).
		//
		if (bspNormalDot(ModelInfo,iNode,EdPoly)>=0)
			{
			iOurFront = Node->iFront;
			iOurBack  = Node->iBack;
			if (Node->IsCsg())
	            {CoplanarInfo.BackNodeOutside=0; NewFrontOutside=1;};
			}
		else
			{
			iOurFront = Node->iBack;
			iOurBack  = Node->iFront;
			if (Node->IsCsg())
	            {CoplanarInfo.BackNodeOutside=1; NewFrontOutside=0;};
			};
		//
		// Process front and back:
		//
		if ((iOurFront==INDEX_NONE)&&(iOurBack==INDEX_NONE)) // No front or back
			{
			CoplanarInfo.ProcessingBack		= 1;
			CoplanarInfo.FrontLeafOutside	= NewFrontOutside;
			FilterLeaf (FilterFunc,ModelInfo,iNode,EdPoly,CoplanarInfo,CoplanarInfo.BackNodeOutside,NODE_Plane);
			}
		else if ((iOurFront==INDEX_NONE)&&(iOurBack!=INDEX_NONE)) // Back but no front
			{
			CoplanarInfo.ProcessingBack		= 1;
			CoplanarInfo.iBackNode			= iOurBack;
			CoplanarInfo.FrontLeafOutside	= NewFrontOutside;
			//
			iNode   = iOurBack;
			Outside = CoplanarInfo.BackNodeOutside;
			goto FilterLoop;
			}
		else // Has a front and maybe a back
			{
			//
			// Set iOurBack up to process back on next call to FilterLeaf, and loop
			// to process front.  Next call to FilterLeaf will set FrontLeafOutside.
			//
			CoplanarInfo.ProcessingBack = 0;
			CoplanarInfo.iBackNode      = iOurBack; // May be a node or may be INDEX_NONE
			//
			iNode   = iOurFront;
			Outside = NewFrontOutside;
			goto  FilterLoop;
			};
		}
	else if (SplitResult==SP_Split)
		{
		//
		// This has to be recursive because it relies on creating new EdPolys.
		//
		ModelInfo->NumFPolys += 2;
		//
		// Front half of split:
		//
		if (Node->IsCsg())	{NewFrontOutside=1; NewBackOutside=0;}
		else				{NewFrontOutside=NewBackOutside=Outside;};
		//
		if (Node->iFront==INDEX_NONE)
			FilterLeaf (FilterFunc,ModelInfo,iNode,TempFrontEdPoly,CoplanarInfo,NewFrontOutside,NODE_Front);
		else
			FilterEdPoly (FilterFunc, ModelInfo,Node->iFront, TempFrontEdPoly,CoplanarInfo,NewFrontOutside);
		//
		// Back half of split:
		//
		if (Node->iBack==INDEX_NONE)
			FilterLeaf (FilterFunc,ModelInfo,iNode,TempBackEdPoly,CoplanarInfo,NewBackOutside,NODE_Back);
		else
			FilterEdPoly (FilterFunc,ModelInfo,Node->iBack,TempBackEdPoly,CoplanarInfo,NewBackOutside);
		ModelInfo->NumFPolys -= 2;
		};
	UNGUARD("FilterEdPoly");
	};

//
// Regular entry into FilterEdPoly (so higher-level callers don't have to
// deal with unnecessary info). Filters starting at root.
//
void BspFilterEdPoly (BSP_FILTER_FUNC FilterFunc, IModel *ModelInfo,FPoly *EdPoly,int RootOutside)
	{
	GUARD;
	FCoplanarInfo StartingCoplanarInfo;
	StartingCoplanarInfo.iOriginalNode = INDEX_NONE; // No coplanars
	//
	if (ModelInfo->NumBspNodes==0) // Process root node
		{
		FilterFunc (ModelInfo,0,EdPoly,RootOutside ? F_OUTSIDE : F_INSIDE,NODE_Root);
		}
	else
		{
		FilterEdPoly (FilterFunc,ModelInfo,0,EdPoly,StartingCoplanarInfo,RootOutside);
		};
	UNGUARD("BspFilterEdPoly");
	};

/*----------------------------------------------------------------------------
   Editor fundamentals
----------------------------------------------------------------------------*/

//
// Convert a Bsp node to an EdPoly.  Returns number of vertices in Bsp
// node (0 or 3-MAX_NODE_VERTICES).
//
int FGlobalEditor::bspNodeToFPoly (IModel *ModelInfo, INDEX iNode, FPoly *EdPoly)
	{
	GUARD;
	FBspNode	*Node     	= &ModelInfo->BspNodes [iNode];
	FBspSurf	*Poly     	= &ModelInfo->BspSurfs [Node->iSurf];
	FVertPool	*VertPool	= &ModelInfo->VertPool [Node->iVertPool];
	FVector		*FVectors 	= ModelInfo->FVectors;
	FVector		*FPoints  	= ModelInfo->FPoints;
	FPoly		*MasterEdPoly;
	BYTE		i,j,n,prev;
	//
	EdPoly->Base		= FPoints  [Poly->pBase];
	EdPoly->Normal      = FVectors [Poly->vNormal];
	//
	EdPoly->PolyFlags 	= Poly->PolyFlags & ~(PF_EdCut | PF_EdProcessed | PF_Selected | PF_Memorized);
	EdPoly->iLink     	= Node->iSurf;
	EdPoly->Texture     = Poly->Texture;
	//
	EdPoly->Brush    	= Poly->Brush;
	EdPoly->iBrushPoly  = Poly->iBrushPoly;
	//
	EdPoly->PanU		= Poly->PanU;
	EdPoly->PanV		= Poly->PanV;
	//
	EdPoly->iZone[0]	= Node->iZone[0];
	EdPoly->iZone[1]	= Node->iZone[1];
	//
	MasterEdPoly = polyFindMaster (ModelInfo,Node->iSurf);
	if (MasterEdPoly == NULL)
		{
		EdPoly->GroupName = NAME_NONE;
		EdPoly->ItemName  = NAME_NONE;
		}
	else
		{
		EdPoly->GroupName = MasterEdPoly->GroupName;
		EdPoly->ItemName  = MasterEdPoly->GroupName;
		};
	if (Poly->vTextureU==INDEX_NONE) EdPoly->TextureU.X = MAXSWORD;
	else                             EdPoly->TextureU   = FVectors [Poly->vTextureU];
	//
	if (Poly->vTextureV==INDEX_NONE) EdPoly->TextureV.X = MAXSWORD;
	else                             EdPoly->TextureV   = FVectors [Poly->vTextureV];
	//
	n = Node->NumVertices;
	i=0; j=0; prev=n-1;
	//
	for (i=0; i<n; i++)
		{
		EdPoly->Vertex[j] 		= FPoints[VertPool[i].pVertex];
		EdPoly->Vertex[j].Flags	= 0;
		prev=i;
		j++;
		};
	if (j>=3) EdPoly->NumVertices=j;
	else      EdPoly->NumVertices=0;
	//
	// Remove colinear points and identical points (which will appear
	// if T-joints were eliminated):
	//
	EdPoly->RemoveColinears();
	//
	return EdPoly->NumVertices;
	UNGUARD("BspNodeToEdPoly");
	};

/*---------------------------------------------------------------------------------------
   World filtering
---------------------------------------------------------------------------------------*/

//
// Filter all relevant world polys through the brush.
//
void FilterWorldThroughBrush (
	IModel *ModelInfo, IModel *BrushModelInfo, IModel *BrushInfo,
	FPoly *TempEdPoly, ECsgOper CSGOper, INDEX iNode,
	FVector *BrushSphere)
	{
	GUARD;
	while (iNode != INDEX_NONE) // Loop through all coplanars
		{
		FBspNode		*Node	= &ModelInfo->BspNodes[iNode		];
		FBspSurf		*Poly	= &ModelInfo->BspSurfs[Node->iSurf	];
		FVector			*Base	= &ModelInfo->FPoints [Poly->pBase	];
		FVector			*Normal	= &ModelInfo->FVectors[Poly->vNormal];
		//
		if (ModelInfo->NumBspNodes == 0) return;
		if (Node->NodeFlags & NF_IsNew) return; // Skip new nodes and their children, which are guaranteed new
		//
		// Sphere reject:
		//
		FLOAT Dist		= (*BrushSphere - *Base) | *Normal;
		int   DoFront   = (Dist >= -BrushSphere->W);
		int   DoBack	= (Dist <= +BrushSphere->W);
		//
		// Process only polys that aren't empty:
		//
		if (DoFront && DoBack && (GUnrealEditor.bspNodeToFPoly (ModelInfo,iNode,TempEdPoly)>0))
			{
			TempEdPoly->Brush      = Poly->Brush;
			TempEdPoly->iBrushPoly = Poly->iBrushPoly;
			//
			if ((CSGOper==CSG_Add)||(CSGOper==CSG_Subtract))
				{
				//
				// Add and subtract work the same in this step
				//
				GNode       	= iNode;
				GModelInfo  	= ModelInfo;
				GDiscarded  	= 0;
				GNumBspNodes	= ModelInfo->NumBspNodes;
				//
				//	Find last coplanar in chain:
				//
				GLastCoplanar  = iNode;
				while (ModelInfo->BspNodes[GLastCoplanar].iPlane != INDEX_NONE)
					{
					GLastCoplanar = ModelInfo->BspNodes[GLastCoplanar].iPlane;
					};
				//
				// Do the filter operation:
				//
				if (CSGOper==CSG_Add)
					{
					BspFilterEdPoly(AddWorldToBrushFunc,BrushModelInfo,TempEdPoly,1);
					}
				else // CSG_Subtract
					{
					BspFilterEdPoly(SubtractWorldToBrushFunc,BrushModelInfo,TempEdPoly,1);
					};
				if (GDiscarded == 0)
					{
					//
					// Get rid of all the fragments we added:
					//
					ModelInfo->BspNodes[GLastCoplanar].iPlane = INDEX_NONE;
					ModelInfo->NumBspNodes                    = GNumBspNodes;
					}
				else
					{
					//
					// Tag original poly for deletion; has been deleted or replaced by
					// partial fragments.
					//
					GModelInfo->BspNodes[GNode].NodeFlags |= NF_TagForEmpty;
					};
				}
			else if (CSGOper==CSG_Intersect)
				{
				GModelInfo	= BrushInfo;
				BspFilterEdPoly (IntersectWorldWithBrushFunc,BrushModelInfo,TempEdPoly,1);
				}
			else if (CSGOper==CSG_Deintersect)
				{
				GModelInfo	= BrushInfo;
				BspFilterEdPoly (DeIntersectWorldWithBrushFunc,BrushModelInfo,TempEdPoly,1);
				};
			};
		//
		// Now recurse to filter all of the world's children nodes:
		//
		if (DoFront && (Node->iFront != INDEX_NONE)) FilterWorldThroughBrush
			(
			ModelInfo,BrushModelInfo,BrushInfo,
			TempEdPoly,CSGOper,Node->iFront,
			BrushSphere
			);
		if (DoBack && (Node->iBack != INDEX_NONE)) FilterWorldThroughBrush
			(
			ModelInfo,BrushModelInfo,BrushInfo,
			TempEdPoly,CSGOper,Node->iBack,
			BrushSphere
			);
		iNode = Node->iPlane;
		};
	UNGUARD("FilterWorldThroughBrush");
	};

/*---------------------------------------------------------------------------------------
   Brush Csg
---------------------------------------------------------------------------------------*/

//
// Perform any CSG operation between the brush and the world.
//
int FGlobalEditor::bspBrushCSG (UModel *Brush, UModel *Model, DWORD PolyFlags, ECsgOper CSGOper, int BuildBounds)
	{
	GUARD;
	IModel			BrushInfo;
	IModel			ModelInfo;
	IModel			TempModelInfo;
	FPoly			EdPoly,*DestEdPoly;
	FLOAT			Orientation;
	FModelCoords	Coords,Uncoords;
	int				NumPolysFromBrush=0,i,j,ReallyBig;
	char			*Descr;
	//
	// See if an overflow is likely:
	//
	if (!Brush) return 0;
	if (GUnrealEditor.MapEdit) return 0; // All operations are non-interactive in map edit mode
	if (Model->ModelFlags & MF_InvalidBsp) return 0;
	//
	if (CSGOper != CSG_Add) PolyFlags &= ~(PF_Semisolid | PF_NotSolid);
	//
	Model->Lock	(&ModelInfo,LOCK_Trans);
	Brush->Lock	(&BrushInfo,LOCK_Trans);
	GUnrealEditor.TempModel->Lock(&TempModelInfo,LOCK_NoTrans);
	//
	ModelInfo.NumZones			= 0;
	TempModelInfo.NumFPolys		= 0;
	TempModelInfo.NumBspNodes	= 0;
	//
	// Build the brush's coordinate system and find orientation of scale
	// transform (if negative, edpolyTransform will reverse the clockness
	// of the EdPoly points and invert the normal):
	//
	Orientation = Brush->BuildCoords(&Coords,&Uncoords);
	//
	// Transform original brush poly into same coordinate system as world
	// so Bsp filtering operations make sense.
	//
	TempModelInfo.NumFPolys  = BrushInfo.NumFPolys;
	DestEdPoly               = &TempModelInfo.FPolys[0];
	BrushInfo.Location      += BrushInfo.PostPivot;
	//
	ReallyBig = (BrushInfo.NumFPolys > 200);
	if (ReallyBig)
		{
		switch (CSGOper)
			{
			case CSG_Add:			Descr = "Adding brush to world"; break;
			case CSG_Subtract: 		Descr = "Subtracting brush from world"; break;
			case CSG_Intersect: 	Descr = "Intersecting brush with world"; break;
			case CSG_Deintersect: 	Descr = "Deintersecting brush with world"; break;
			default:				Descr = "Performing CSG operation"; break;
			};
		GApp->BeginSlowTask (Descr,1,0);
		};
	if (ReallyBig) GApp->StatusUpdate ("Transforming",0,0);
	for (i=0; i<BrushInfo.NumFPolys; i++)
		{
		*DestEdPoly            = BrushInfo.FPolys[i];
		DestEdPoly->Brush      = Brush;
		DestEdPoly->iBrushPoly = i;
		DestEdPoly->PolyFlags |= PolyFlags;
		//
		if (DestEdPoly->iLink==INDEX_NONE) DestEdPoly->iLink = i;
		//
		DestEdPoly->Transform(Coords,&BrushInfo.PrePivot,&BrushInfo.Location,Orientation);
		//
		if (!DestEdPoly->Texture) DestEdPoly->Texture = GUnrealEditor.CurrentTexture;
		//
		DestEdPoly++;
		};
	if (ReallyBig) GApp->StatusUpdate ("Filtering brush",0,0);
	//
	if ((CSGOper==CSG_Intersect)||(CSGOper==CSG_Deintersect))
		{
		BrushInfo.NumFPolys=0; // Empty the brush
		for (i=0; i<TempModelInfo.NumFPolys; i++)
			{
      		if ((BrushInfo.NumFPolys+1) < BrushInfo.MaxFPolys)
				{
         		EdPoly = TempModelInfo.FPolys[i];
				if (EdPoly.iLink==INDEX_NONE) EdPoly.iLink = i;
         		//
				if (CSGOper==CSG_Intersect)
		            {
					GModelInfo = &BrushInfo;
					BspFilterEdPoly (IntersectBrushWithWorldFunc,&ModelInfo,&EdPoly,FBspNode::ROOT_OUTSIDE);
					}
				else if (CSGOper==CSG_Deintersect)
		            {
					GModelInfo = &BrushInfo;
					BspFilterEdPoly (DeIntersectBrushWithWorldFunc,&ModelInfo,&EdPoly,FBspNode::ROOT_OUTSIDE);
					};
				};
			};
		NumPolysFromBrush = BrushInfo.NumFPolys;
		}
	else // Add or subtract
		{
		for (i=0; i<BrushInfo.NumFPolys; i++)
			{
      		if ((ModelInfo.NumFPolys+1) >= ModelInfo.MaxFPolys) appError ("World is too large");
         	EdPoly = TempModelInfo.FPolys[i];
			//
         	// Mark the polygon as non-cut (so that it won't be harmed unless it must
         	// be split), and set iLink so that BspAddNode will know to add its information
         	// if a node is added based on this poly:
         	//
         	EdPoly.PolyFlags &= ~(PF_EdCut);
          	if (EdPoly.iLink==i)
				{
				EdPoly.iLink = ModelInfo.NumBspSurfs;
				TempModelInfo.FPolys[i].iLink = EdPoly.iLink;
				}
			else EdPoly.iLink = TempModelInfo.FPolys[EdPoly.iLink].iLink;
			// 
			// Filter through the Bsp:
         	//
         	if (CSGOper==CSG_Add)			BspFilterEdPoly(AddBrushToWorldFunc,&ModelInfo,&EdPoly,FBspNode::ROOT_OUTSIDE);
			else if (CSGOper==CSG_Subtract) BspFilterEdPoly(SubtractBrushFromWorldFunc,&ModelInfo,&EdPoly,FBspNode::ROOT_OUTSIDE);
			};
		};
	if (!(PolyFlags & (PF_NotSolid | PF_Semisolid)))
		{
		//
		// Quickly build a Bsp for the brush, tending to minimize splits rather than balance
		// the tree.  We only need the cutting planes, though the entire Bsp struct (polys and
		// all) is built.
		//
		GUnrealEditor.TempModel->Unlock(&TempModelInfo); // BspBuild locks this temporarily
		//
		if (ReallyBig) GApp->StatusUpdate ("Building Bsp",0,0);
		bspBuild (GUnrealEditor.TempModel, BSP_Lame,0,1);
		//
		GUnrealEditor.TempModel->Lock(&TempModelInfo,LOCK_NoTrans);
		//
		if (ReallyBig) GApp->StatusUpdate ("Filtering world",0,0);
		//
		TempModelInfo.Model->BuildBound(0);
		FilterWorldThroughBrush
			(
			&ModelInfo,&TempModelInfo,&BrushInfo,
			&EdPoly,CSGOper,0,
			&TempModelInfo.Model->Bound[0].Sphere
			);
		};
	if ((CSGOper==CSG_Intersect) || (CSGOper==CSG_Deintersect))
		{
		if (ReallyBig) GApp->StatusUpdate ("Adjusting brush",0,0);
		//
		// Link polys obtained from the original brush:
		//
		for (i=NumPolysFromBrush-1; i>=0; i--)
			{
			DestEdPoly = &BrushInfo.FPolys[i];
			for (j=0; j<i; j++)
				{
				if (DestEdPoly->iLink == BrushInfo.FPolys[j].iLink)
					{
					DestEdPoly->iLink = j;
					break;
					};
				};
			if (j>=i) DestEdPoly->iLink	= i;
			};
		//
		// Link polys obtained from the world:
		//
		for (i=BrushInfo.NumFPolys-1; i>=NumPolysFromBrush; i--)
			{
			DestEdPoly = &BrushInfo.FPolys[i];
			for (j=NumPolysFromBrush; j<i; j++)
				{
				if (DestEdPoly->iLink == BrushInfo.FPolys[j].iLink)
					{
					DestEdPoly->iLink = j;
					break;
					};
				};
			if (j>=i) DestEdPoly->iLink	= i;
			};
		BrushInfo.ModelFlags |= MF_Linked;
		//
		// Detransform the brush back into its original coordinate system
		//
		for (i=0; i<BrushInfo.NumFPolys; i++)
			{
			DestEdPoly = &BrushInfo.FPolys[i];
			//
			DestEdPoly->Transform (Uncoords,&BrushInfo.Location,&BrushInfo.PrePivot,Orientation);
			DestEdPoly->Fix       ();
			//
			DestEdPoly->Brush		= NULL;
			DestEdPoly->iBrushPoly	= i;
			};
		};
	BrushInfo.Location -= BrushInfo.PostPivot;
	//
	if ((CSGOper==CSG_Add) || (CSGOper==CSG_Subtract))
		{
		if (ReallyBig) GApp->StatusUpdate ("Refreshing Bsp",0,0);
		//
		bspCleanup		(&ModelInfo);	// Clean up nodes, reset node flags
		bspRefresh		(&ModelInfo,0);	// Delete unreferenced stuff if near overflow
		if (BuildBounds)
			{
			bspBuildBounds(&ModelInfo);	// Rebuild bounding volumes
			};
		};
	Brush->Unlock(&BrushInfo);
	Model->Unlock(&ModelInfo);
	GUnrealEditor.TempModel->Unlock(&TempModelInfo);
	//
	if ((CSGOper==CSG_Intersect)||(CSGOper==CSG_Deintersect))
		{
		if (ReallyBig) GApp->StatusUpdate ("Merging",0,0);
		bspMergeCoplanars (Brush,1,0);
		};
	if (ReallyBig) GApp->EndSlowTask();
	return 1;
	//
	UNGUARD("BspBrushCSG");
	};

/*---------------------------------------------------------------------------------------
   Bsp stats
---------------------------------------------------------------------------------------*/

//
// Calculate stats for a node and all its children.  Called recursively.
// IsFront: 1=front node of parent, 0=back node of parent
// Depth:   Depth of node in the Bsp, 0=root.
//
void CalcBspNodeStats (IModel *ModelInfo, INDEX iNode, FBspStats *Stats,
	int IsFront, int Depth)
	{
	FBspNode    *Node = &ModelInfo->BspNodes[iNode];
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
		CalcBspNodeStats(ModelInfo,Node->iFront,Stats,1,Depth+1);
		}
	else if (Node->iFront==INDEX_NONE) // Has back but no front
		{
		Stats->Backs++;
		CalcBspNodeStats(ModelInfo,Node->iBack,Stats,0,Depth+1);
		}
	else // Has both front and back
		{
		Stats->Branches++;
		CalcBspNodeStats(ModelInfo,Node->iFront,Stats,1,Depth+1);
		CalcBspNodeStats(ModelInfo,Node->iBack,Stats,0,Depth+1);
		};
	//
	// Process coplanars:
	//
	i=Node->iPlane;
	while (i!=INDEX_NONE)
		{
		Stats->Coplanars++;
		i=ModelInfo->BspNodes[i].iPlane;
		};
	};

//
// Calculate stats for entire tree:
//
void BspCalcStats (UModel *Model, FBspStats *Stats)
	{
	GUARD;
	IModel ModelInfo;
	//
	Model->Lock(&ModelInfo,LOCK_Read);
	//
	Stats->Polys       = ModelInfo.NumBspSurfs;
	Stats->Nodes       = ModelInfo.NumBspNodes;
	Stats->MaxNodes    = ModelInfo.MaxBspNodes;
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
	if (ModelInfo.NumBspNodes>0) CalcBspNodeStats(&ModelInfo,0,Stats,1,0);
	//
	if (Stats->Leaves>0) Stats->AvgDepth = Stats->DepthCount/Stats->Leaves;
	//
	Model->Unlock(&ModelInfo);
	UNGUARD("BspCalcStats");
	};

/*---------------------------------------------------------------------------------------
   Bsp link topic handler
---------------------------------------------------------------------------------------*/

//
// Link topic function for Unreal to communicate Bsp information to UnrealEd.
//
AUTOREGISTER_TOPIC("Bsp",BspTopicHandler);
void BspTopicHandler::Get(ULevel *Level, const char *Topic, const char *Item, char *Data)
	{
	GUARD;
	if (!Level->Model) return;
	//
	// Recalc stats when 'Polys' item is accessed:
	//
	if ((stricmp(Item,"POLYS")==0)) BspCalcStats(Level->Model,&GBspStats);
	//
	// Handle item:
	//
	if      (stricmp(Item,"Polys"       )==0) itoa (GBspStats.Polys       ,Data,10);
	else if (stricmp(Item,"Nodes"       )==0) itoa (GBspStats.Nodes       ,Data,10);
	else if (stricmp(Item,"MaxNodes"    )==0) itoa (GBspStats.MaxNodes    ,Data,10);
	else if (stricmp(Item,"MaxDepth"    )==0) itoa (GBspStats.MaxDepth    ,Data,10);
	else if (stricmp(Item,"AvgDepth"    )==0) itoa (GBspStats.AvgDepth    ,Data,10);
	else if (stricmp(Item,"Branches"    )==0) itoa (GBspStats.Branches    ,Data,10);
	else if (stricmp(Item,"Coplanars"   )==0) itoa (GBspStats.Coplanars   ,Data,10);
	else if (stricmp(Item,"Fronts"      )==0) itoa (GBspStats.Fronts      ,Data,10);
	else if (stricmp(Item,"Backs"       )==0) itoa (GBspStats.Backs       ,Data,10);
	else if (stricmp(Item,"Leaves"      )==0) itoa (GBspStats.Leaves      ,Data,10);
	else if (stricmp(Item,"FrontLeaves" )==0) itoa (GBspStats.FrontLeaves ,Data,10);
	else if (stricmp(Item,"BackLeaves"  )==0) itoa (GBspStats.BackLeaves  ,Data,10);
	else strcpy(Data,"-1"); // Unknown
	//
	UNGUARD("BspTopicHandler::Get");
	};
void BspTopicHandler::Set(ULevel *Level, const char *Topic, const char *Item, const char *Data)
	{
	GUARD;
	UNGUARD("BspTopicHandler::Set");
	};

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
	IModel		*ModelInfo;	// Model.
	FPointVert	**Index;	// Starting link.

	// Allocate.
	void Alloc( IModel *ThisModelInfo )
	{
		guard(FPointVertList::Alloc);

		// Allocate working tables.
		ModelInfo = ThisModelInfo;
		Index     = (FPointVert **)GMem.GetZeroed( ModelInfo->NumPoints * sizeof (FPointVert*) );

		unguard;
	}

	// Free.
	void Free()
	{
		guard(FPointVertList::Free);
		GMem.Release(Index);
		unguard;
	}

	// Add all of a node's vertices to a node-vertex list.
	void AddNode( INDEX iNode )
	{
		guard(FPointVertList::AddNode);

		FBspNode	*Node 		= &ModelInfo->BspNodes [iNode];
		FVertPool	*VertPool   = &ModelInfo->VertPool [Node->iVertPool];

		for( INDEX i=0; i < Node->NumVertices; i++ )
		{
			INDEX pVertex = VertPool[i].pVertex;

			// Add new point/vertex pair to array, and insert new array entry
			// between index and first entry.

			FPointVert *PointVert	= (FPointVert *)GMem.Get( sizeof(FPointVert) );
			
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

		for( INDEX iNode=0; iNode < ModelInfo->NumBspNodes; iNode++ )
			AddNode( iNode );

		unguard;
	}

	// Remove all of a node's vertices from a node-vertex list.
	void RemoveNode( INDEX iNode )
	{
		guard(FPointVertList::RemoveNode);

		FBspNode	*Node 		= &ModelInfo->BspNodes [iNode];
		FVertPool	*VertPool	= &ModelInfo->VertPool [Node->iVertPool];

		// Loop through all of the node's vertices and search through the
		// corresponding point's node-vert list, and delink this node.
		int Count=0;
		for( INDEX i = 0; i < Node->NumVertices; i++ )
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
	IModel			*ModelInfo,
	FPointVertList	*PointVerts,
	INDEX			iNode, 
	INDEX			VertexNumber, 
	INDEX			pVertex 
)
{
	guard(AddPointToNode);

	FBspNode	*Node 	  = &ModelInfo->BspNodes [iNode];
	FBspSurf	*Poly 	  = &ModelInfo->BspSurfs [Node->iSurf];
	FVertPool	*VertPool = &ModelInfo->VertPool [Node->iVertPool];
	FVertPool	*NewVertPool;

	if( (Node->NumVertices+1) >= FBspNode::MAX_NODE_VERTICES )
	{
		// Just refuse to add point: This is a non-fatal problem.
		debugf(LOG_Bsp,"Node side limit reached");
		return;
	}

	// Make sure vertex pool is big enough.
	if( (ModelInfo->NumVertPool + (int)(Node->NumVertices+1)) >= ModelInfo->MaxVertPool )
	{
		appError("Vertex pool overflow");
	}

	// Remove node from vertex list, since vertex numbers will be reordered.
	if( PointVerts )
		PointVerts->RemoveNode( iNode );

	Node->iVertPool	= ModelInfo->NumVertPool;
	NewVertPool		= &ModelInfo->VertPool[Node->iVertPool];

	// Make sure this node doesn't already contain the vertex.
	for( INDEX i=0; i<Node->NumVertices; i++ )
		checkLogic(VertPool[i].pVertex!=pVertex);
	
	// Copy the old vertex pool to the new one.
	for( i=0; i<VertexNumber; i++ )
		NewVertPool[i] = VertPool[i];
	
	for( i=VertexNumber; i<Node->NumVertices; i++ )
		NewVertPool[i+1]	= VertPool[i];

	// Add the new point to the new vertex pool.
	NewVertPool[VertexNumber].pVertex = pVertex;
	NewVertPool[VertexNumber].iSide   = INDEX_NONE;

	Node->NumVertices++;
	ModelInfo->NumVertPool += Node->NumVertices;

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
	IModel			*ModelInfo,
	FPointVertList	*PointVerts,	
	INDEX			iNode,
	INDEX			pVertex
)
{
	guard(DistributePoint);

	FBspNode		*Node 		= &ModelInfo->BspNodes [iNode];
	FVector			*FPoints	= ModelInfo->FPoints;
	FVector			*FVectors	= ModelInfo->FVectors;
	FVertPool		*VertPool;
	FVector			Side,SidePlaneNormal,MidPoint,MidDistVect;
	FLOAT			Size;
	int				FoundSide,SkippedColinear,SkippedInside;
	int				Count = 0;

	// Make a local copy of iFront and iBack because Node may change within the routine.
	INDEX iFront = Node->iFront;
	INDEX iBack  = Node->iBack;

	FLOAT Dist = Node->Plane.PlaneDot(FPoints[pVertex]);

	if( Dist < -THRESH_OPTGEOM_COPLANAR )
	{
		// Back.
		if( iBack != INDEX_NONE )
			Count += DistributePoint( ModelInfo, PointVerts, iBack, pVertex );
	}
	else if( Dist >= THRESH_OPTGEOM_COPLANAR )
	{
		// Front.
		if (iFront != INDEX_NONE)
			Count += DistributePoint( ModelInfo, PointVerts, iFront, pVertex );
	}
	else
	{
		// This point is coplanar with this node, so check point for intersection with
		// this node's sides, then loop with its coplanars.
		while( iNode != INDEX_NONE )
		{
			Node 		= &ModelInfo->BspNodes [iNode];
			VertPool	= &ModelInfo->VertPool [Node->iVertPool];

			// Skip this node if it already contains the point in question.
			for( INDEX i=0; i<Node->NumVertices; i++ )
				if( VertPool[i].pVertex==pVertex )
					goto Skip;

			// Loop through all sides and see if (A) side is colinear with point, and
			// (B) point falls within inside of this side.

			FoundSide       = -1;
			SkippedColinear = 0;
			SkippedInside   = 0;

			for( i=0; i<Node->NumVertices; i++ )
			{
				INDEX j = (i>0) ? (i-1) : (Node->NumVertices-1);

				// Create cutting plane perpendicular to both this side and the polygon's normal.
				Side            = FPoints [VertPool[i].pVertex] - FPoints [VertPool[j].pVertex];
				SidePlaneNormal = Side ^ Node->Plane;

				Size = SidePlaneNormal.Size();
				if( Size>0.00001 )
				{
					// Points aren't coincedent.
					Dist = ((FPoints[pVertex]-FPoints[VertPool[i].pVertex]) | SidePlaneNormal)/Size;

					if( Dist >= THRESH_OPTGEOM_COSIDAL )
					{
						// Point is outside polygon, can't possibly fall on a side.
						goto Skip;
					}
					else if( Dist > -THRESH_OPTGEOM_COSIDAL )
					{
						// This point we're adding falls on this line.
						//
						// Verify that it falls within this side; though it's colinear
						// it may be out of the bounds of the line's endpoints if this side
						// is colinear with an adjacent side.
						//
						// Do this by checking distance from point to side's midpoint and
						// comparing with the side's half-length.

						MidPoint    = (FPoints [VertPool[i].pVertex] + FPoints [VertPool[j].pVertex])*0.5;
						MidDistVect = FPoints[pVertex] - MidPoint;

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
					debugf(LOG_Bsp,"Found tiny side");
					//Poly->PolyFlags |= PF_Selected;
				}
			}
			if( FoundSide >= 0 )
			{
				// AddPointToNode will reorder the vertices in this node.  This is okay
				// because it's called outside of the vertex loop.
				AddPointToNode (ModelInfo,PointVerts,iNode,FoundSide,pVertex);
				Count++;
			}
			else if( SkippedColinear )
			{
				// This happens occasionally because of the fuzzy Dist comparison.  It is
				// not a sign of a problem when the vertex being distributed is colinear
				// with one of this polygon's sides, but slightly outside of this polygon.

				debug(LOG_Bsp,"Skipped colinear");
				//Poly->PolyFlags |= PF_Selected;
			}
			else if( SkippedInside )
			{
				// Point is on interior of polygon.
				debug(LOG_Bsp,"Skipped interior");
				//Poly->PolyFlags |= PF_Selected;
			}

			// Go to next coplanar node.
			Skip:
			iNode = Node->iPlane;
		}

		// Now recurse with both front and back.  Since the point is coplanar with this
		// plane, it may also be coplanar with planes in either front (or back). (This is
		// true with points and lines, but not planes).

		if (iBack  != INDEX_NONE) Count += DistributePoint (ModelInfo,PointVerts,iBack, pVertex);
		if (iFront != INDEX_NONE) Count += DistributePoint (ModelInfo,PointVerts,iFront,pVertex);
	}
	return Count;
	unguard;
}

//
// Merge points that are near.
//
void MergeNearPoints( IModel *Model, FLOAT Dist )
{
	guard(Heal);
	INDEX *PointRemap = (INDEX *)GMem.Get(Model->NumPoints * sizeof(INDEX));
	int Merged=0,Collapsed=0;

	// Find nearer point for all points.
	for( INDEX i=0; i<Model->NumPoints; i++ )
	{
		PointRemap[i] = i;
		FVector *Point = &Model->FPoints[i];
		for( INDEX j=0; j<i; j++ )
		{
			FVector *TestPoint = &Model->FPoints[j];
			if( (*TestPoint-*Point).SizeSquared() < Dist*Dist )
			{
				PointRemap[i] = j;
				Merged++;
				break;
			}
		}
	}

	// Remap VertPool.
	for( i=0; i<Model->NumVertPool; i++ )
		Model->VertPool[i].pVertex = PointRemap[Model->VertPool[i].pVertex];

	// Remap Surfs.
	for( i=0; i<Model->NumBspSurfs; i++ )
		Model->BspSurfs[i].pBase = PointRemap[Model->BspSurfs[i].pBase];

	// Fix nodes.
	for( i=0; i<Model->NumBspNodes; i++ )
	{
		FBspNode  *Node = &Model->BspNodes[i];
		FVertPool *Pool = &Model->VertPool[Node->iVertPool];
		
		int k=0;
		for( int j=0; j<Node->NumVertices; j++ )
		{
			FVertPool *A = &Pool[j];
			FVertPool *B = &Pool[j ? j-1 : Node->NumVertices-1];
			
			if( A->pVertex != B->pVertex )
				Pool[k++]=Pool[j];
		}
		Node->NumVertices = k>=3 ? k : 0;
		if(k<3) Collapsed++;
	}

	// Success.
	debugf("MergeNearPoints merged %i/%i, collapsed %i",Merged,Model->NumPoints,Collapsed);
	GMem.Release(PointRemap);
	unguard;
}

//
// Optimize a level's Bsp, eliminating T-joints where possible, and building side
// links.  This does not always do a 100% perfect job, mainly due to imperfect 
// levels, however it should never fail or return incorrect results.
//
void FGlobalEditor::bspOptGeom(UModel *Model)
{
	guard(FGlobalEditor::bspOptGeom);
	IModel ModelInfo;
	FPointVertList PointVerts;

	debug (LOG_Bsp,"BspOptGeom begin");

	Model->Lock				(&ModelInfo,LOCK_NoTrans);
	//MergeNearPoints		(&ModelInfo,0.5);
	bspRefresh				(&ModelInfo,1);
	PointVerts.Alloc		(&ModelInfo);
	PointVerts.AddAllNodes	();

	// First four entries are reserved for view-clipped sides.
	ModelInfo.NumSharedSides=4;
	
	// Mark all sides as unlinked.
	for( int i=0; i<ModelInfo.NumVertPool; i++ )
		ModelInfo.VertPool[i].iSide = INDEX_NONE;

	INT TeesFound = 0, Distributed = 0;

	// Eliminate T-joints on each node by finding all vertices that aren't attached to
	// two shared sides, then filtering them down through the BSP and adding them to
	// the sides they belong on.
	
	for( INDEX iNode=0; iNode < ModelInfo.NumBspNodes; iNode++ )
	{
		FBspNode *Node = &ModelInfo.BspNodes[iNode];
		
		// Loop through all sides (side := line from PrevVert to ThisVert)		
		FVertPool *VertPool = &ModelInfo.VertPool[Node->iVertPool];
		for( BYTE ThisVert=0; ThisVert < Node->NumVertices; ThisVert++ )
		{
			BYTE PrevVert = (ThisVert>0) ? (ThisVert - 1) : (Node->NumVertices-1);

			// Count number of nodes sharing this side, i.e. number of nodes for
			// which two adjacent vertices are identical to this side's two vertices.
			for( FPointVert *PV1 = PointVerts.Index[VertPool[ThisVert].pVertex]; PV1; PV1=PV1->Next )
				for( FPointVert *PV2 = PointVerts.Index[VertPool[PrevVert].pVertex]; PV2; PV2=PV2->Next )
					if( PV1->iNode==PV2->iNode && PV1->iNode!=iNode )
						goto SkipIt;

			// Didn't find another node that shares our two vertices; must add each
			// vertex to all polygons where the vertex lies on the polygon's side.
			// DistributePoint will not affect the current node but may change others
			// and may increase the number of nodes in the Bsp.

			TeesFound++;
			Distributed = 0;
			Distributed += DistributePoint( &ModelInfo, &PointVerts, 0, VertPool[ThisVert].pVertex );
			Distributed += DistributePoint( &ModelInfo, &PointVerts, 0, VertPool[PrevVert].pVertex );
			//if (Distributed != 1) debugf("Dist %i",Distributed);
			SkipIt:;
		}
	}

	// Build side links
	// Definition of side: Side (i) links node vertex (i) to vertex ((i+1)%n)
	debug(LOG_Bsp,"BspOptGeom building sidelinks");

	PointVerts.Free			();
	PointVerts.Alloc		(&ModelInfo);
	PointVerts.AddAllNodes	();

	for( iNode=0; iNode < ModelInfo.NumBspNodes; iNode++ )
	{
		FBspNode *Node = &ModelInfo.BspNodes[iNode];
		
		FVertPool *VertPool = &ModelInfo.VertPool [Node->iVertPool];
		for( BYTE ThisVert=0; ThisVert < Node->NumVertices; ThisVert++ )
		{
			if( VertPool[ThisVert].iSide == INDEX_NONE )
			{
				BYTE PrevVert = (ThisVert>0) ? (ThisVert - 1) : (Node->NumVertices-1);

				// See if this node links to another one.
				for( FPointVert *PV1=PointVerts.Index[VertPool[ThisVert].pVertex]; PV1; PV1=PV1->Next )
				{
					for( FPointVert *PV2=PointVerts.Index[VertPool[PrevVert].pVertex]; PV2; PV2=PV2->Next )
					{
						if( PV1->iNode==PV2->iNode && PV1->iNode!=iNode )
						{
							// Make sure that the other node's two vertices are adjacent and
							// ordered opposite this node's vertices.

							INDEX		iOtherNode		= PV2->iNode;
							FBspNode	*OtherNode		= &ModelInfo.BspNodes [iOtherNode];
							FVertPool	*OtherVertPool	= &ModelInfo.VertPool [OtherNode->iVertPool];

							INT Delta = 
								(OtherNode->NumVertices + PV2->nVertex - PV1->nVertex) % 
								OtherNode->NumVertices;

							if( Delta==1 )
							{
								// Side is properly linked!
								INDEX OtherVert = PV2->nVertex;
								if( OtherVertPool[OtherVert].iSide==INDEX_NONE )
								{
									// Create a new 'shared side' for these two sides:
									VertPool      [ThisVert ].iSide = ModelInfo.NumSharedSides;
									OtherVertPool [OtherVert].iSide = ModelInfo.NumSharedSides;
									ModelInfo.NumSharedSides++;

									// Validation check.
									checkLogic(VertPool[(ThisVert+Node->NumVertices-1)%Node->NumVertices].pVertex==OtherVertPool[OtherVert].pVertex);
									checkLogic(VertPool[ThisVert].pVertex==OtherVertPool[(OtherVert+OtherNode->NumVertices-1)%OtherNode->NumVertices].pVertex);
								}
								else
								{
									// This side is shared by >2 polys (OK, can happen in valid maps)
									//debugf(LOG_Bsp,"Multiply shared side");
									VertPool[ThisVert].iSide = OtherVertPool[OtherVert].iSide;
								}
								goto SkipSide;
							}
						}
					}
				}
				// This node doesn't have correct side linking
				//ModelInfo.BspSurfs[Node->iSurf].PolyFlags |= PF_Selected;
				debug(LOG_Bsp,"Failed to link side");
				//ModelInfo.BspSurfs[Node->iSurf].PolyFlags |= PF_Selected;
			}
			// Go to next side.
			SkipSide:;
		}
	}
	i=0; int j=0;
	for( iNode=0; iNode < ModelInfo.NumBspNodes; iNode++ )
	{
		FBspNode  *Node     = &ModelInfo.BspNodes [iNode];
		FVertPool *VertPool = &ModelInfo.VertPool [Node->iVertPool];
		
		for( BYTE ThisVert=0; ThisVert < Node->NumVertices; ThisVert++ )
		{
			i++;
			if (VertPool[ThisVert].iSide!=INDEX_NONE) j++;
		}
	}

	// Done.
	debug  (LOG_Bsp,"BspOptGeom end");
	debugf (LOG_Bsp,"Processed %i T-points, linked: %i/%i sides",TeesFound,j,i);

	PointVerts.Free	();
	Model->Unlock   (&ModelInfo);
	unguard;
}

/*---------------------------------------------------------------------------------------
   Bsp point/vector refreshing
---------------------------------------------------------------------------------------*/

void TagReferencedNodes (IModel *ModelInfo,INDEX *NodeRef,INDEX *PolyRef,INDEX iNode)
	{
	FBspNode *Node = &ModelInfo->BspNodes[iNode];
	//
	NodeRef[iNode      ] = 0;
	PolyRef[Node->iSurf] = 0;
	//
	if (Node->iFront!=INDEX_NONE)	TagReferencedNodes(ModelInfo,NodeRef,PolyRef,Node->iFront);
	if (Node->iBack !=INDEX_NONE)	TagReferencedNodes(ModelInfo,NodeRef,PolyRef,Node->iBack );
	if (Node->iPlane!=INDEX_NONE)	TagReferencedNodes(ModelInfo,NodeRef,PolyRef,Node->iPlane);
	};

//
// If the Bsp's point and vector tables are nearly full, reorder them and delete
// unused ones:
//
void FGlobalEditor::bspRefresh (IModel *ModelInfo,int ForceRefresh)
	{
	GUARD;
	FBspNode	*Node;
	FBspSurf	*Poly;
	FVertPool	*VertPool;
	INDEX		*VectorRef,*PointRef,*NodeRef,*PolyRef,i,n;
	BYTE		B;
	//
	// Remove unreferenced Bsp surfs:
	//
	if ((((int)ModelInfo->NumVectors *8) >= ((int)ModelInfo->MaxVectors *7)) ||
		(((int)ModelInfo->NumPoints  *8) >= ((int)ModelInfo->MaxPoints  *7)) ||
		(((int)ModelInfo->NumBspSurfs*8) >= ((int)ModelInfo->MaxBspSurfs*7)) ||
		(((int)ModelInfo->NumBspNodes*8) >= ((int)ModelInfo->MaxBspNodes*7)) ||
		(ForceRefresh))
		{
		if (GTrans->Locked) GTrans->ForceOverflow("Bsp Refresh");
		//
		NodeRef		= (INDEX *)GMem.GetOned(ModelInfo->NumBspNodes * sizeof (INDEX));
		PolyRef		= (INDEX *)GMem.GetOned(ModelInfo->NumBspSurfs * sizeof (INDEX));
		if (ModelInfo->NumBspNodes>0) TagReferencedNodes (ModelInfo,NodeRef,PolyRef,0);
		//
		if (ForceRefresh==2) memset(PolyRef,0,ModelInfo->NumBspSurfs * sizeof (INDEX)); // Don't remap surfs
		//
		// Remap Bsp nodes and surfs:
		//
		n=0; for (i=0; i<ModelInfo->NumBspSurfs; i++) if (PolyRef[i]!=INDEX_NONE)
			{
			ModelInfo->BspSurfs[n] = ModelInfo->BspSurfs[i];
			PolyRef[i]=n++;
			};
		debugf (LOG_Bsp,"Polys: %i -> %i",ModelInfo->NumBspSurfs,n);
		ModelInfo->NumBspSurfs = n;
		//
		n=0; for (i=0; i<ModelInfo->NumBspNodes; i++) if (NodeRef[i]!=INDEX_NONE)
			{
			ModelInfo->BspNodes[n] = ModelInfo->BspNodes[i];
			NodeRef[i]=n++;
			};
		debugf (LOG_Bsp,"Nodes: %i -> %i",ModelInfo->NumBspNodes,n);
		ModelInfo->NumBspNodes = n;
		//
		// Update Bsp nodes:
		//
		for (i=0; i<ModelInfo->NumBspNodes; i++)
			{
			Node = &ModelInfo->BspNodes[i];
			//
			Node->iSurf = PolyRef[Node->iSurf];
			if (Node->iFront != INDEX_NONE) Node->iFront = NodeRef[Node->iFront];
			if (Node->iBack  != INDEX_NONE) Node->iBack  = NodeRef[Node->iBack];
			if (Node->iPlane != INDEX_NONE) Node->iPlane = NodeRef[Node->iPlane];
			};
		//
		// Remove unreferenced points and vectors:
		//
		if ((((int)ModelInfo->NumVectors*4) >= ((int)ModelInfo->MaxVectors*3)) ||
			(((int)ModelInfo->NumPoints *4) >= ((int)ModelInfo->MaxPoints *3)) ||
			(ForceRefresh))
			{	
			VectorRef = (INDEX *)GMem.GetOned(ModelInfo->NumVectors * sizeof(INDEX));
			PointRef  = (INDEX *)GMem.GetOned(ModelInfo->NumPoints  * sizeof(INDEX));
			//
			// Check Bsp surfs
			//
			Poly = &ModelInfo->BspSurfs[0];
			for (i=0; i<ModelInfo->NumBspSurfs; i++)
				{
				VectorRef [Poly->vNormal   ] = 0;
				VectorRef [Poly->vTextureU ] = 0;
				VectorRef [Poly->vTextureV ] = 0;
				PointRef  [Poly->pBase     ] = 0;
				Poly++;
				};
			//
			// Check Bsp nodes
			//
			Node = &ModelInfo->BspNodes[0];
			for (i=0; i<ModelInfo->NumBspNodes; i++) // Tag all points used by nodes
				{
				VertPool = &ModelInfo->VertPool [Node->iVertPool];
				for (B=0; B<Node->NumVertices;  B++)
					{			
					PointRef [VertPool->pVertex] = 0;
					VertPool++;
					};
				Node++;
				};
			//
			// Remap points
			//
			n=0; for (i=0; i<ModelInfo->NumPoints; i++) if (PointRef[i]!=INDEX_NONE)
				{
				ModelInfo->FPoints[n] = ModelInfo->FPoints[i];
				PointRef[i] = n++;
				};
			debugf (LOG_Bsp,"Points: %i -> %i",ModelInfo->NumPoints,n);
			ModelInfo->NumPoints = n;
			//
			// Remap vectors
			//
			n=0; for (i=0; i<ModelInfo->NumVectors; i++) if (VectorRef[i]!=INDEX_NONE)
				{
				ModelInfo->FVectors[n] = ModelInfo->FVectors[i];
				VectorRef[i] = n++;
				};
			debugf (LOG_Bsp,"Vectors: %i -> %i",ModelInfo->NumVectors,n);
			ModelInfo->NumVectors = n;
			//
			// Update Bsp surfs
			//
			Poly = &ModelInfo->BspSurfs[0];
			for (i=0; i<ModelInfo->NumBspSurfs; i++)
				{
				Poly->vNormal   = VectorRef [Poly->vNormal  ];
				Poly->vTextureU = VectorRef [Poly->vTextureU];
				Poly->vTextureV = VectorRef [Poly->vTextureV];
				Poly->pBase     = PointRef  [Poly->pBase    ];
				Poly++;
				};
			//
			// Update Bsp nodes
			//
			Node = &ModelInfo->BspNodes[0];
			for (i=0; i<ModelInfo->NumBspNodes; i++)
				{
				VertPool = &ModelInfo->VertPool [Node->iVertPool];
				for (B=0; B<Node->NumVertices;  B++)
					{			
					VertPool->pVertex = PointRef [VertPool->pVertex];
					VertPool++;
					};
				Node++;
				};
			GMem.Release(VectorRef);
			};
		GMem.Release(NodeRef);
		};
	UNGUARD("BspRefresh");
	};

/*---------------------------------------------------------------------------------------
   The End
---------------------------------------------------------------------------------------*/
