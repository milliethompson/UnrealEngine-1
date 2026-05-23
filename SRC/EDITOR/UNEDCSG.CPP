/*=============================================================================
	UnEdCSG.cpp: High-level CSG tracking functions for editor

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

//
// Globals:
//
BYTE GFlags1 [MAXWORD+1]; // For fast polygon selection
BYTE GFlags2 [MAXWORD+1];

/*-----------------------------------------------------------------------------
	Level brush tracking
-----------------------------------------------------------------------------*/

//
// Prepare a moving brush.
//
void FGlobalEditor::csgPrepMovingBrush( UModel *Brush )
{
	guard(FGlobalEditor::csgPropMovingBrush);
	debugf("Preparing moving brush %s", Brush->GetName() );

	// Build bounding box.
	Brush->BuildBound(1);

	// Allocate databases.
	Brush->AllocDatabases(0);

	// Build BSP for the brush.
	bspBuild( Brush, BSP_Good, 15, 1 );
	bspRefresh( Brush, 1 );
	bspBuildBounds( Brush );

	unguard;
}

//
// Duplicate the specified brush and make it into a CSG-able level brush.
// Returns new brush, or NULL if the original was empty.
//
UModel *FGlobalEditor::csgDuplicateBrush
(
	ULevel		*Level,
	UModel		*Brush, 
	DWORD		PolyFlags, 
	DWORD		ModelFlags,
	DWORD		ResFlags,
	BOOL		IsMovingBrush
)
{
	guard(FGlobalEditor::csgDuplicateBrush);

	if( Brush->Polys->Num == 0 )
		return NULL;

	// Duplicate the brush and its polys.
	UModel *NewBrush = new("Brush0",           Brush,       DUPLICATE_MakeUnique,ResFlags)UModel;
	NewBrush->Polys  = new(NewBrush->GetName(),Brush->Polys,DUPLICATE_Replace   ,ResFlags)UPolys;
	NewBrush->Polys->Shrink();

	// Init new brush's tables.
	NewBrush->Vectors	= (UVectors	*)NULL;
	NewBrush->Points	= (UVectors	*)NULL;
	NewBrush->Nodes		= (UBspNodes*)NULL;
	NewBrush->Surfs		= (UBspSurfs*)NULL;
	NewBrush->Verts		= (UVerts	*)NULL;
	NewBrush->Bounds	= (UBounds	*)NULL;

	// Set new brush's properties.
	NewBrush->PolyFlags  = PolyFlags;
	NewBrush->ModelFlags = ModelFlags;

	// Update poly textures.
	NewBrush->Lock(LOCK_ReadWrite);
	for( int i=0; i<NewBrush->Polys->Num; i++ )
	{
		FPoly &Poly     = NewBrush->Polys(i);
		Poly.Texture    = Poly.Texture ? Poly.Texture : CurrentTexture;
		Poly.iBrushPoly = INDEX_NONE;
	}
	NewBrush->Unlock(LOCK_ReadWrite);

	// Build bounding box.
	NewBrush->BuildBound(1);

	// If it's a moving brush, prep it.
	if( IsMovingBrush )
		csgPrepMovingBrush(NewBrush);

	return NewBrush;
	unguard;
}

//
// Add a brush to the list of CSG brushes in the level, using a CSG operation, and return 
// a newly-created copy of it.
//
UModel *FGlobalEditor::csgAddOperation
(
	UModel		*Brush,
	ULevel		*Level,
	DWORD		PolyFlags,
	ECsgOper	CsgOper,
	BYTE		ModelFlags
)
{
	guard(FGlobalEditor::csgAddOperation);

	UArray	*BrushArray	= Level->BrushArray;
	UModel  *NewBrush;

	NewBrush = csgDuplicateBrush
	(
		Level,
		Brush,
		PolyFlags,
		ModelFlags,
		RF_NotForClient | RF_NotForServer,
		0
	);
	if( !NewBrush )
		return NULL;

	// Save existing level brush array.
	Level->BrushArray->ModifyAllItems();
	NewBrush->CsgOper = CsgOper;

	Level->BrushArray->AddItem(NewBrush);

	return NewBrush;
	unguard;
}

/*-----------------------------------------------------------------------------
	Misc
-----------------------------------------------------------------------------*/

char *FGlobalEditor::csgGetName(ECsgOper CSG)
{
	guard(FGlobalEditor::csgGetName);
	switch( CSG )
	{
		case CSG_Active:		return "Active Brush";
		case CSG_Add:			return "Add Brush";
		case CSG_Subtract:		return "Subtract Brush";
		case CSG_Intersect:		return "Intersect Brush";
		case CSG_Deintersect:	return "Deintersect Brush";
		default:				return "Unknown Brush";
	}
	unguard;
}

void FGlobalEditor::csgInvalidateBsp(ULevel *Level)
{
	guard(FGlobalEditor::csgInvalidateBsp);
	Level->Model->ModelFlags |= MF_InvalidBsp;
	unguard;
}

/*-----------------------------------------------------------------------------
	CSG Rebuilding
-----------------------------------------------------------------------------*/

//
// Rebuild the level's Bsp from the level's CSG brushes
//
// Note: Needs to be expanded to defragment Bsp polygons as needed (by rebuilding
// the Bsp), so that it doesn't slow down to a crawl on complex levels.
//
void FGlobalEditor::csgRebuild(ULevel *Level)
{
	guard(FGlobalEditor::csgRebuild);

	UModel				*Brush;
	int 				NodeCount,PolyCount,LastPolyCount;
	char				TempStr [80];

	int n = Level->BrushArray->Num;

	GApp->BeginSlowTask ("Rebuilding geometry",1,0);
	FastRebuild=1;

	constraintFinishAllSnaps(Level);
	Level->Model->ModelFlags &= ~MF_InvalidBsp;	// Revalidate the Bsp.
	MapEdit = 0;							// Turn map editing off.

	// Empty the model out.
	Level->Lock(LOCK_Trans);
	Level->Model->EmptyModel(1,1);
	Level->Unlock(LOCK_Trans);

	LastPolyCount = 0;
	for( int i=1; i<n; i++ )
	{
		sprintf(TempStr,"Applying brush %i of %i",i,n);
		GApp->StatusUpdate(TempStr,i,n);

		// See if the Bsp has become badly fragmented and, if so, rebuild.
		PolyCount = Level->Model->Surfs->Num;
		NodeCount = Level->Model->Nodes->Num;
		if( PolyCount>2000 && PolyCount>=3*LastPolyCount )
		{
			strcat (TempStr,": Refreshing Bsp...");
			GApp->StatusUpdate (TempStr,i,n);

			debug 				(LOG_Info,"Map: Rebuilding Bsp");
			bspBuildFPolys		(Level->Model,1);
			bspMergeCoplanars	(Level->Model,0,0);
			bspBuild			(Level->Model,BSP_Lame,25,0);
			debugf				(LOG_Info,"Map: Reduced nodes by %i%%, polys by %i%%",(100*(NodeCount-Level->Model->Nodes->Num))/NodeCount,(100*(PolyCount-Level->Model->Surfs->Num))/PolyCount);

			LastPolyCount = Level->Model->Surfs->Num;
		}

		// Perform this CSG operation.
		Brush = Level->BrushArray->Element(i);
		if( bspBrushCSG( Brush, Level->Model, Brush->PolyFlags, (ECsgOper)Brush->CsgOper, 0 ) > 1 )
			debugf(" Problem was encountered in brush %i",i);
	}

	// Build bounding volumes.
	Level->Lock(LOCK_Trans);
	bspBuildBounds(Level->Model);
	Level->Unlock(LOCK_Trans);

	// Done.
	FastRebuild = 0;
	GApp->EndSlowTask();
	unguard;
}

/*---------------------------------------------------------------------------------------
	Flag setting and searching
---------------------------------------------------------------------------------------*/

//
// Sets and clears all Bsp node flags.  Affects all nodes, even ones that don't
// really exist.
//
void FGlobalEditor::polySetAndClearNodeFlags(UModel *Model, DWORD SetBits, DWORD ClearBits)
	{
	BYTE		NewFlags;
	//
	guard(FGlobalEditor::polySetAndClearNodeFlags);
	for (INDEX i=0; i<Model->Nodes->Num; i++)
		{
		FBspNode &Node = Model->Nodes(i);
		NewFlags = (Node.NodeFlags & ~ClearBits) | SetBits;
		if (NewFlags != Node.NodeFlags)
			{
			Model->Nodes->ModifyItem(i);
			Node.NodeFlags = NewFlags;
			};
		};
	unguard;
	};

//
// Sets and clears all Bsp node flags.  Affects all nodes, even ones that don't
// really exist.
//
void FGlobalEditor::polySetAndClearPolyFlags(UModel *Model, DWORD SetBits, DWORD ClearBits,int SelectedOnly, int UpdateMaster)
	{
	DWORD		NewFlags;
	//
	guard(FGlobalEditor::polySetAndClearPolyFlags);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf &Poly = Model->Surfs(i);
		if ((!SelectedOnly) || (Poly.PolyFlags & PF_Selected))
			{
			NewFlags = (Poly.PolyFlags & ~ClearBits) | SetBits;
			if (NewFlags != Poly.PolyFlags)
				{
				Model->Surfs->ModifyItem(i,UpdateMaster);
				Poly.PolyFlags = NewFlags;
				//
				if (UpdateMaster) polyUpdateMaster (Model,i,0,0);
				};
			};
		};
	unguard;
	};

/*-----------------------------------------------------------------------------
	Polygon searching
-----------------------------------------------------------------------------*/

typedef void (*POLY_CALLBACK)(UModel *Model, INDEX iSurf);

//
// Find the Brush EdPoly corresponding to a given Bsp surface.
//
int FGlobalEditor::polyFindMaster(UModel *Model, INDEX iSurf, FPoly &Poly)
{
	guard(FGlobalEditor::polyFindMaster);

	FBspSurf &Surf = Model->Surfs(iSurf);
	if( !Surf.Brush )
	{
		return 0;
	}
	else
	{
		Surf.Brush->Lock(LOCK_Read);
		Poly = Surf.Brush->Polys->Element(Surf.iBrushPoly);
		Surf.Brush->Unlock(LOCK_Read);
		return 1;
	}
	unguard;
}

//
// Update a the master brush EdPoly corresponding to a newly-changed
// poly to reflect its new properties.
//
// Doesn't do any transaction tracking.  Assumes you've called transSelectedBspSurfs.
//
void FGlobalEditor::polyUpdateMaster(UModel *Model,INDEX iSurf,int UpdateTexCoords,int UpdateBase)
	{
	guard(FGlobalEditor::polyUpdateMaster);
	FPoly			*MasterEdPoly,*BrushEdPolys;
	UModel			*MasterBrush;
	UPolys			*EdPolysHeader;
	FModelCoords	Uncoords;
	//
	FBspSurf &Poly = Model->Surfs(iSurf);
	if (!Poly.Brush) return;
	//
	MasterBrush		= Poly.Brush;
	EdPolysHeader	= MasterBrush->Polys;
	//
	MasterBrush->Polys->Lock(LOCK_ReadWrite);
	BrushEdPolys	= &MasterBrush->Polys->Element(0);
	//
	if (UpdateTexCoords || UpdateBase) Poly.Brush->BuildCoords(NULL,&Uncoords);
	//
	INDEX iEdPoly = Poly.iBrushPoly;
	while (iEdPoly < EdPolysHeader->Num)
		{
		MasterEdPoly = &BrushEdPolys[iEdPoly];
		//
		MasterEdPoly->Texture   = Poly.Texture;
		MasterEdPoly->PanU      = Poly.PanU;
		MasterEdPoly->PanV      = Poly.PanV;
		MasterEdPoly->PolyFlags = Poly.PolyFlags & ~(PF_EdCut|PF_EdProcessed|PF_Selected|PF_Memorized);
		//
		if (UpdateTexCoords || UpdateBase)
			{
			if (UpdateTexCoords)
				{
				MasterEdPoly->TextureU = Model->Vectors(Poly.vTextureU).TransformVectorBy(Uncoords.VectorXform);
				MasterEdPoly->TextureV = Model->Vectors(Poly.vTextureV).TransformVectorBy(Uncoords.VectorXform);
				};
			if (UpdateBase)
				{
				MasterEdPoly->Base
				=	(Model->Points(Poly.pBase) - MasterBrush->Location - MasterBrush->PostPivot)
				.	TransformVectorBy(Uncoords.PointXform)
				+	MasterBrush->PrePivot;
				};
			};
		while (++iEdPoly < EdPolysHeader->Num)
			{
			if (BrushEdPolys[iEdPoly].iLink == Poly.iBrushPoly) break;
			};
		};
	MasterBrush->Polys->Unlock(LOCK_ReadWrite);
	unguard;
	};

//
// Find all Bsp polys with flags such that SetBits are clear or ClearBits are set.
//
void FGlobalEditor::polyFindByFlags(UModel *Model, DWORD SetBits, DWORD ClearBits, POLY_CALLBACK Callback)
	{
	guard(FGlobalEditor::polyFindByFlags);
	FBspSurf *Poly = &Model->Surfs(0);
	//
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		if (((Poly->PolyFlags&SetBits)!=0) || ((Poly->PolyFlags&~ClearBits)!=0))
			{
			Callback (Model,i);
			};
		Poly++;
		};
	unguard;
	};

//
// Find all BspSurfs corresponding to a particular editor brush object
// and polygon index. Call with BrushPoly set to INDEX_NONE to find all Bsp 
// polys corresponding to the Brush.
//
void FGlobalEditor::polyFindByBrush(UModel *Model, UModel *Brush, INDEX iBrushPoly, POLY_CALLBACK Callback)
	{
	guard(FGlobalEditor::polyFindByBrush);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf &Poly = Model->Surfs(i);
		if (
			(Poly.Brush == Brush) && 
			((iBrushPoly == INDEX_NONE) || (Poly.iBrushPoly == iBrushPoly))
			)
			{
			Callback (Model,i);
			};
		};
	unguard;
	};

//
// Find all Bsp polys corresponding to a particular editor brush object and polygon index.
// Call with BrushPoly set to INDEX_NONE to find all Bsp polys corresponding to the Brush->
//
void FGlobalEditor::polyFindByBrushGroupItem 
(
	UModel *Model,
	UModel *Brush, INDEX iBrushPoly,
	FName GroupName, FName ItemName,
	POLY_CALLBACK Callback
)
{
	guard(FGlobalEditor::polyFindByBrushGroupItem);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
	{
		FBspSurf *Surf = &Model->Surfs(i);
		FPoly Poly;
		Poly.GroupName = NAME_None;
		Poly.ItemName  = NAME_None;
		polyFindMaster(Model,i,Poly);
		if
		(	(Surf->Brush==Brush)
		&&	(iBrushPoly==INDEX_NONE || iBrushPoly==Surf->iBrushPoly)
		&&	(GroupName==NAME_None   || GroupName==Poly.GroupName)
		&&	(ItemName==NAME_None    || ItemName==Poly.ItemName) )
		{
			Callback (Model,i);
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
   All transactional polygon selection functions
-----------------------------------------------------------------------------*/

void FGlobalEditor::polyResetSelection(UModel *Model)
	{
	guard(FGlobalEditor::polyResetSelection);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		Poly->PolyFlags |= ~(PF_Selected | PF_Memorized);
		Poly++;
		};
	unguard;
	};

void FGlobalEditor::polySelectAll(UModel *Model)
	{
	guard(FGlobalEditor::polySelectAll);
	polySetAndClearPolyFlags(Model,PF_Selected,0,0,0);
	unguard;
	};

void FGlobalEditor::polySelectNone(UModel *Model)
	{
	guard(FGlobalEditor::polySelectNone);
	polySetAndClearPolyFlags(Model, 0, PF_Selected,0,0);
	unguard;
	};

void FGlobalEditor::polySelectMatchingGroups(UModel *Model)
	{
	guard(FGlobalEditor::polySelectMatchingGroups);
	memset(GFlags1,0,sizeof(GFlags1));
	//
	for (INDEX i=0; i<Model->Surfs->Num; i++)
	{
		FBspSurf *Surf = &Model->Surfs(i);
		if (Surf->PolyFlags&PF_Selected)
		{
			FPoly Poly; polyFindMaster(Model,i,Poly);
			GFlags1[Poly.GroupName.GetIndex()]=1;
		}
	}
	for (i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Surf = &Model->Surfs(i);
		FPoly Poly; polyFindMaster(Model,i,Poly);
		if ((GFlags1[Poly.GroupName.GetIndex()]) 
			&& (!(Surf->PolyFlags & PF_Selected)))
			{
			Model->Surfs->ModifyItem(i,0);
			Surf->PolyFlags |= PF_Selected;
			};
		};
	unguard;
	};

void FGlobalEditor::polySelectMatchingItems(UModel *Model)
{
	guard(FGlobalEditor::polySelectMatchingItems);

	memset(GFlags1,0,sizeof(GFlags1));
	memset(GFlags2,0,sizeof(GFlags2));

	for( INDEX i=0; i<Model->Surfs->Num; i++ )
	{
		FBspSurf *Surf = &Model->Surfs(i);
		if( Surf->Brush )
		{
			if( Surf->PolyFlags & PF_Selected )
				GFlags2[Surf->Brush->GetIndex()]=1;
		}
		if( Surf->PolyFlags&PF_Selected )
		{
			FPoly Poly; polyFindMaster(Model,i,Poly);
			GFlags1[Poly.ItemName.GetIndex()]=1;
		}
	}
	for( i=0; i<Model->Surfs->Num; i++ )
	{
		FBspSurf *Surf = &Model->Surfs(i);
		if( Surf->Brush )
		{
			FPoly Poly; polyFindMaster(Model,i,Poly);
			if ((GFlags1[Poly.ItemName.GetIndex()]) &&
				( GFlags2[Surf->Brush->GetIndex()]) &&
				(!(Surf->PolyFlags & PF_Selected)))
			{
				Model->Surfs->ModifyItem(i,0);
				Surf->PolyFlags |= PF_Selected;
			}
		}
	}
	unguard;
}

enum EAdjacentsType
{
	ADJACENT_ALL,		// All adjacent polys
	ADJACENT_COPLANARS,	// Adjacent coplanars only
	ADJACENT_WALLS,		// Adjacent walls
	ADJACENT_FLOORS,	// Adjacent floors or ceilings
	ADJACENT_SLANTS,	// Adjacent slants
};

//
// Select all adjacent polygons (only coplanars if Coplanars==1) and
// return number of polygons newly selected.
//
int TagAdjacentsType(UModel *Model, EAdjacentsType AdjacentType)
	{
	guard(TagAdjacentsType);
	FVert	*VertPool;
	FVector		*Base,*Normal;
	BYTE		b;
	INDEX		i;
	int			Selected,Found;
	//
	Selected = 0;
	memset(GFlags1,0,sizeof(GFlags1));
	//
	// Find all points corresponding to selected vertices:
	//
	for (i=0; i<Model->Nodes->Num; i++)
		{
		FBspNode &Node = Model->Nodes(i);
		FBspSurf &Poly = Model->Surfs(Node.iSurf);
		if (Poly.PolyFlags & PF_Selected)
			{
			VertPool = &Model->Verts(Node.iVertPool);
			//
			for (b=0; b<Node.NumVertices; b++) GFlags1[(VertPool++)->pVertex] = 1;
			};
		};
	//
	// Select all unselected nodes for which two or more vertices are selected:
	//
	for (i=0; i<Model->Nodes->Num; i++)
		{
		FBspNode &Node = Model->Nodes(i);
		FBspSurf &Poly = Model->Surfs(Node.iSurf);
		if (!(Poly.PolyFlags & PF_Selected))
			{
			Found    = 0;
			VertPool = &Model->Verts(Node.iVertPool);
			//
			Base   = &Model->Points (Poly.pBase);
			Normal = &Model->Vectors(Poly.vNormal);
			//
			for (b=0; b<Node.NumVertices; b++) Found += GFlags1[(VertPool++)->pVertex];
			//
			if (AdjacentType == ADJACENT_COPLANARS)
				{
				if (!GFlags2[Node.iSurf]) Found=0;
				}
			else if (AdjacentType == ADJACENT_FLOORS)
				{
				if (Abs(Normal->Z) <= 0.85) Found = 0;
				}
			else if (AdjacentType == ADJACENT_WALLS)
				{
				if (Abs(Normal->Z) >= 0.10) Found = 0;
				}
			else if (AdjacentType == ADJACENT_SLANTS)
				{
				if (Abs(Normal->Z) > 0.85) Found = 0;
				if (Abs(Normal->Z) < 0.10) Found = 0;
				};
			if (Found > 0)
				{
				Model->Surfs->ModifyItem(Node.iSurf,0);
				Poly.PolyFlags |= PF_Selected;
				Selected++;
				};
			};
		};
	return Selected;
	unguard;
	};

void TagCoplanars(UModel *Model)
	{
	guard(TagCoplanars);
	FBspSurf	*SelectedPoly,*Poly;
	FVector		*SelectedBase,*SelectedNormal,*Base,*Normal;
	//
	memset(GFlags2,0,sizeof(GFlags2));
	//
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		SelectedPoly = &Model->Surfs(i);
		if (SelectedPoly->PolyFlags & PF_Selected)
			{
			SelectedBase   = &Model->Points (SelectedPoly->pBase);
			SelectedNormal = &Model->Vectors(SelectedPoly->vNormal);
			//
			for (INDEX j=0; j<Model->Surfs->Num; j++)
				{
				Poly = &Model->Surfs(j);
				Base   = &Model->Points (Poly->pBase);
				Normal = &Model->Vectors(Poly->vNormal);
				//
				if (FCoplanar(*Base,*Normal,*SelectedBase,*SelectedNormal) && (!(Poly->PolyFlags & PF_Selected)))
					{
					GFlags2[j]=1;
					};
				};
			};
		};
	unguard;
	};

void FGlobalEditor::polySelectAdjacents(UModel *Model)
	{
	guard(FGlobalEditor::polySelectAdjacents);
	do {} while (TagAdjacentsType (Model,ADJACENT_ALL) > 0);
	unguard;
	};

void FGlobalEditor::polySelectCoplanars(UModel *Model)
	{
	guard(FGlobalEditor::polySelectCoplanars);
	TagCoplanars(Model);
	do {} while (TagAdjacentsType(Model,ADJACENT_COPLANARS) > 0);
	unguard;
	};

void FGlobalEditor::polySelectMatchingBrush(UModel *Model)
	{
	guard(FGlobalEditor::polySelectMatchingBrush);
	//
	memset(GFlags1,0,sizeof(GFlags1));
	//
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if (Poly->Brush)
			{
			if (Poly->PolyFlags&PF_Selected) GFlags1[Poly->Brush->GetIndex()]=1;
			};
		Poly++;
		};
	for (i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if (Poly->Brush)
			{
			if ((GFlags1[Poly->Brush->GetIndex()])&&(!(Poly->PolyFlags&PF_Selected)))
				{
				Model->Surfs->ModifyItem(i,0);
				Poly->PolyFlags |= PF_Selected;
				};
			};
		Poly++;
		};
	unguard;
	};

void FGlobalEditor::polySelectMatchingTexture(UModel *Model)
	{
	guard(FGlobalEditor::polySelectMatchingTexture);
	INDEX		i,Blank=0;
	memset(GFlags1,0,sizeof(GFlags1));
	//
	for (i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if (Poly->Texture && (Poly->PolyFlags&PF_Selected)) GFlags1[Poly->Texture->GetIndex()]=1;
		else if (!Poly->Texture) Blank=1;
		Poly++;
		};
	for (i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if (Poly->Texture && (GFlags1[Poly->Texture->GetIndex()]) && (!(Poly->PolyFlags&PF_Selected)))
			{
			Model->Surfs->ModifyItem(i,0);
			Poly->PolyFlags |= PF_Selected;
			}
		else if (Blank & !Poly->Texture) Poly->PolyFlags |= PF_Selected;
		Poly++;
		};
	unguard;
	};

void FGlobalEditor::polySelectAdjacentWalls(UModel *Model)
	{
	guard(FGlobalEditor::polySelectAdjacentWalls);
	do {} while (TagAdjacentsType  (Model,ADJACENT_WALLS) > 0);
	unguard;
	};

void FGlobalEditor::polySelectAdjacentFloors(UModel *Model)
	{
	guard(FGlobalEditor::polySelectAdjacentFloors);
	do {} while (TagAdjacentsType (Model,ADJACENT_FLOORS) > 0);
	unguard;
	};

void FGlobalEditor::polySelectAdjacentSlants(UModel *Model)
	{
	guard(FGlobalEditor::polySelectAdjacentSlants);
	do {} while (TagAdjacentsType  (Model,ADJACENT_SLANTS) > 0);
	unguard;
	};

void FGlobalEditor::polySelectReverse(UModel *Model)
	{
	guard(FGlobalEditor::polySelectReverse);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		Model->Surfs->ModifyItem(i,0);
		Poly->PolyFlags ^= PF_Selected;
		//
		Poly++;
		};
	unguard;
	};

void FGlobalEditor::polyMemorizeSet(UModel *Model)
	{
	guard(FGlobalEditor::polyMemorizeSet);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if (Poly->PolyFlags & PF_Selected) 
			{
			if (!(Poly->PolyFlags & PF_Memorized))
				{
				Model->Surfs->ModifyItem(i,0);
				Poly->PolyFlags |= (PF_Memorized);
				};
			}
		else
			{
			if (Poly->PolyFlags & PF_Memorized)
				{
				Model->Surfs->ModifyItem(i,0);
				Poly->PolyFlags &= (~PF_Memorized);
				};
			};
		Poly++;
		};
	unguard;
	};

void FGlobalEditor::polyRememberSet(UModel *Model)
	{
	guard(FGlobalEditor::polyRememberSet);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if (Poly->PolyFlags & PF_Memorized) 
			{
			if (!(Poly->PolyFlags & PF_Selected))
				{
				Model->Surfs->ModifyItem(i,0);
				Poly->PolyFlags |= (PF_Selected);
				};
			}
		else
			{
			if (Poly->PolyFlags & PF_Selected)
				{
				Model->Surfs->ModifyItem(i,0);
				Poly->PolyFlags &= (~PF_Selected);
				};
			};
		Poly++;
		};
	unguard;
	};

void FGlobalEditor::polyXorSet(UModel *Model)
	{
	int			Flag1,Flag2;
	//
	guard(FGlobalEditor::polyXorSet);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		Flag1 = (Poly->PolyFlags & PF_Selected ) != 0;
		Flag2 = (Poly->PolyFlags & PF_Memorized) != 0;
		//
		if (Flag1 ^ Flag2)
			{
			if (!(Poly->PolyFlags & PF_Selected))
				{
				Model->Surfs->ModifyItem(i,0);
				Poly->PolyFlags |= PF_Selected;
				};
			}
		else
			{
			if (Poly->PolyFlags & PF_Selected)
				{
				Model->Surfs->ModifyItem(i,0);
				Poly->PolyFlags &= (~PF_Selected);
				};
			};
		Poly++;
		};
	unguard;
	};

void FGlobalEditor::polyUnionSet(UModel *Model)
	{
	guard(FGlobalEditor::polyUnionSet);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if (!(Poly->PolyFlags & PF_Memorized))
			{
			if (Poly->PolyFlags | PF_Selected)
				{
				Model->Surfs->ModifyItem(i,0);
				Poly->PolyFlags &= (~PF_Selected);
				};
			};
		Poly++;
		};
	unguard;
	};

void FGlobalEditor::polyIntersectSet(UModel *Model)
	{
	guard(FGlobalEditor::polyIntersectSet);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if ((Poly->PolyFlags & PF_Memorized) && !(Poly->PolyFlags & PF_Selected))
			{
			Poly->PolyFlags |= PF_Selected;
			};
		Poly++;
		};
	unguard;
	};

/*---------------------------------------------------------------------------------------
   Brush selection functions
---------------------------------------------------------------------------------------*/

//
// Generic selection routines
//

typedef int (*BRUSH_SEL_FUNC)(UModel *Brush,int Tag);

void MapSelect(ULevel *Level,BRUSH_SEL_FUNC Func,int Tag)
	{
	guard(MapSelect);
	UModel		*Brush;
	//
	Level->Lock(LOCK_Trans);
	//
	int n = Level->BrushArray->Num;
	for (int i=0; i<n; i++)
		{
		Brush = Level->BrushArray->Element(i);
		//
		if (Func (Brush,Tag)) // Select it
			{
			if (!(Brush->ModelFlags & MF_Selected))
				{
				GTrans->NoteResHeader (Brush);
				Brush->ModelFlags |= MF_Selected;
				};
			}
		else // Deselect it
			{
			if (Brush->ModelFlags & MF_Selected)
				{
				GTrans->NoteResHeader (Brush);
				Brush->ModelFlags &= ~MF_Selected;
				};
			};
		};
	Level->Unlock(LOCK_Trans);
	unguard;
	};

//
// Select all
//
int  BrushSelectAllFunc (UModel *Brush,int Tag) {return 1;};
void FGlobalEditor::mapSelectAll(ULevel *Level)
	{
	guard(FGlobalEditor::mapSelectAll);
	MapSelect (Level,BrushSelectAllFunc,0);
	unguard;
	};

//
// Select none
//
int BrushSelectNoneFunc (UModel *Brush,int Tag) {return 0;};
void FGlobalEditor::mapSelectNone(ULevel *Level)
	{
	guard(FGlobalEditor::mapSelectNone);
	MapSelect (Level,BrushSelectNoneFunc,0);
	unguard;
	};

//
// Select by CSG operation
//
int  BrushSelectOperationFunc (UModel *Brush,int Tag) {return ((ECsgOper)Brush->CsgOper == Tag) && !(Brush->PolyFlags & (PF_NotSolid | PF_Semisolid));};
void FGlobalEditor::mapSelectOperation(ULevel *Level,ECsgOper CsgOper)
	{
	guard(FGlobalEditor::mapSelectOperation);
	MapSelect (Level,BrushSelectOperationFunc,CsgOper);
	unguard;
	};

int  BrushSelectFlagsFunc (UModel *Brush,int Tag) {return (Brush->PolyFlags & Tag);};
void FGlobalEditor::mapSelectFlags(ULevel *Level,DWORD Flags)
	{
	guard(FGlobalEditor::mapSelectFlags);					   
	MapSelect (Level,BrushSelectFlagsFunc,(int)Flags);
	unguard;
	};

void MapSelectSeq(ULevel *Level,int Delta)
	{
	UModel			*Brush,*OtherBrush;
	int				i,j,n;
	//
	guard(MapSelectSeq);
	Level->Lock(LOCK_Trans);
	//
	n = Level->BrushArray->Num;
	for (i=1; i<n; i++)
		{
		Brush = Level->BrushArray->Element(i);
		Brush->ModelFlags &= ~MF_Temp;
		j 	  = i+Delta;
		//
		if ((j>=1)&&(j<n))
			{
			OtherBrush = Level->BrushArray->Element(j);
			if (OtherBrush->ModelFlags & MF_Selected) Brush->ModelFlags |= MF_Temp;
			};
		};
	for (i=1; i<n; i++)
		{
		Brush = Level->BrushArray->Element(i);
		//
		if (Brush->ModelFlags & MF_Temp)
			{
			if (!(Brush->ModelFlags & MF_Selected))
				{
				GTrans->NoteResHeader (Brush);
				Brush->ModelFlags |= MF_Selected;
				};
			}
		else
			{
			if (Brush->ModelFlags & MF_Selected)
				{
				GTrans->NoteResHeader (Brush);
				Brush->ModelFlags &= ~MF_Selected;
				};
			};
		};
	Level->Unlock(LOCK_Trans);
	unguard;
	};

//
// Select brushes previous to selected brushes
//
void FGlobalEditor::mapSelectPrevious(ULevel *Level)
	{
	guard(FGlobalEditor::mapSelectPrevious);
	MapSelectSeq (Level,1);
	unguard;
	};

//
// Select brushes after selected brushes
//
void FGlobalEditor::mapSelectNext(ULevel *Level)
	{
	guard(FGlobalEditor::mapSelectNext);
	MapSelectSeq (Level,-1);
	unguard;
	};

//
// Select first or last
//
void FGlobalEditor::mapSelectFirst(ULevel *Level)
	{
	guard(FGlobalEditor::mapSelectFirst);
	UModel *Brush;
	//
	MapSelect (Level,BrushSelectNoneFunc,0);
	Level->Lock(LOCK_Trans);
	//
	if (Level->BrushArray->Num >= 1)
		{
		Brush = Level->BrushArray->Element(1);
		GTrans->NoteResHeader(Brush);
		Brush->ModelFlags |= MF_Selected;
		};
	Level->Unlock(LOCK_Trans);
	unguard;
	};

void FGlobalEditor::mapSelectLast(ULevel *Level)
	{
	guard(FGlobalEditor::mapSelectLast);
	UModel	*Brush;
	int		n;
	//
	MapSelect (Level,BrushSelectNoneFunc,0);
	Level->Lock(LOCK_Trans);
	//
	n = Level->BrushArray->Num;
	if (n >= 1)
		{
		Brush = Level->BrushArray->Element(n-1);
		GTrans->NoteResHeader (Brush);
		Brush->ModelFlags |= MF_Selected;
		};
	Level->Unlock (LOCK_Trans);
	unguard;
	};

/*---------------------------------------------------------------------------------------
   Other map brush functions
---------------------------------------------------------------------------------------*/

void CopyBrushEdPolys(UModel *DestBrush,UModel *SourceBrush,int Realloc)
	{
	FPoly *SourceFPolys, *DestFPolys;
	//
	guard(CopyBrushEdPolys);
	//
	// Save all old destination EdPolys for undo
	//
	DestBrush->Lock		(LOCK_Trans);
	DestBrush->Polys->ModifyAllItems();
	DestBrush->Unlock	(LOCK_Trans);
	//
	DestBrush->Polys	= new(DestBrush->GetName(),CREATE_Replace)UPolys(SourceBrush->Polys->Num,1);
	DestFPolys 			= &DestBrush->Polys->Element(0);
	SourceFPolys 		= &SourceBrush->Polys->Element(0);
	//
	memcpy (DestFPolys,SourceFPolys,SourceBrush->Polys->QueryMinSize());
	//
	unguard;
	};

//
// Put the first selected brush into the current Brush->
//
void FGlobalEditor::mapBrushGet(ULevel *Level)
	{
	guard(FGlobalEditor::mapBrushGet);
	UModel		*Brush;
	int			i,Done;
	//
	Level->Lock(LOCK_Trans);
	Level->BrushArray->ModifyAllItems();
	//
	Done = 0;
	for (i=1; i<Level->BrushArray->Num; i++)
		{
		Brush = Level->BrushArray->Element(i);
		//
		if (Brush->ModelFlags & MF_Selected)
			{
			GTrans->NoteResHeader (Brush);
			//
			CopyBrushEdPolys (Level->Brush(),Brush,0);
			Level->Brush()->CopyPosRotScaleFrom(Brush);
			//
			break;
			};
		};
	Level->Unlock(LOCK_Trans);
	unguard;
	};

//
// Replace all selected brushes with the current Brush->
//
void FGlobalEditor::mapBrushPut(ULevel *Level)
	{
	guard(FGlobalEditor::mapBrushPut);
	UModel	*Brush;
	int		i;
	//
	Level->Lock(LOCK_Trans);
	//
	Level->BrushArray->ModifyAllItems();
	//
	for (i=1; i<Level->BrushArray->Num; i++)
		{
		Brush = Level->BrushArray->Element(i);
		if (Brush->ModelFlags & MF_Selected)
			{
			CopyBrushEdPolys(Brush,Level->Brush(),1);
			Brush->CopyPosRotScaleFrom(Level->Brush());
			};
		};
	Level->Unlock(LOCK_Trans);
	unguard;
	};

//
// Delete all selected brushes in a level
//
void FGlobalEditor::mapDelete(ULevel *Level)
	{
	guard(FGlobalEditor::mapDelete);
	UModel			*Brush;
	int				i,n;
	//
	Level->Lock(LOCK_Trans);
	//
	n = Level->BrushArray->Num;
	//
	Level->BrushArray->ModifyAllItems();
	//
	for (i=1; i<n; i++) // Don't delete active brush
		{
		Brush = Level->BrushArray->Element(i);
		if (Brush->ModelFlags & MF_Selected)
			{
			Level->BrushArray->RemoveItem(Brush);
			i--;
			n--;
			};
		};
	Level->Unlock(LOCK_Trans);
	unguard;
	};

//
// Duplicate all selected brushes on a level
//
void FGlobalEditor::mapDuplicate(ULevel *Level)
	{
	guard(FGlobalEditor::mapDuplicate);
	UModel			*Brush,*NewBrush;
	//
	Level->Lock(LOCK_Trans);
	Level->BrushArray->ModifyAllItems();
	//
	int n = Level->BrushArray->Num;
	for (int i=0; i<n; i++)
		{
		Brush = Level->BrushArray->Element(i);
		if (Brush->ModelFlags & MF_Selected)
			{
			NewBrush = csgAddOperation (Brush,Level,Brush->PolyFlags,(ECsgOper)Brush->CsgOper,Brush->ModelFlags);
			//
			GTrans->NoteResHeader (Brush);
			Brush->ModelFlags &= ~MF_Selected;
			//
			NewBrush->Location += FVector(32,32,0);
			};
		};
	Level->Unlock(LOCK_Trans);
	unguard;
	};

//
// Generic private routine for send to front / send to back
//
void SendTo( ULevel *Level, int SendToFirstXOR )
{
	guard(SendTo);

	UModel		*Brush;
	UModel		**BrushArray,**TempArray;
	int			i,Num,Count;

	FMemMark Mark(GMem);
	Level->Lock(LOCK_Trans);
	Level->BrushArray->ModifyAllItems();

	Num			= Level->BrushArray->Num;
	BrushArray	= (UModel **)Level->BrushArray->GetData();
	TempArray	= new(GMem,Num)UModel *;
	Count       = 0;

	TempArray[Count++] = BrushArray[0]; // Active brush

	// Pass 1: Copy stuff to front.
	for( i=1; i<Num; i++ )
	{
		Brush = BrushArray[i];
		if( (Brush->ModelFlags ^ SendToFirstXOR) & MF_Selected )
			TempArray[Count++] = BrushArray[i];
	}

	// Pass 2: Copy stuff to back.
	for( i=1; i<Num; i++ )
	{
		Brush = BrushArray[i];
		if (!((Brush->ModelFlags ^ SendToFirstXOR) & MF_Selected))
			TempArray[Count++] = BrushArray[i];
	}

	// Finish up.
	memcpy( BrushArray, TempArray, Num*sizeof(UModel *) );
	Level->Unlock(LOCK_Trans);

	Mark.Pop();
	unguard;
}

//
// Send all selected brushes in a level to the front of the hierarchy
//
void FGlobalEditor::mapSendToFirst(ULevel *Level)
	{
	guard(FGlobalEditor::mapSendToFirst);
	SendTo(Level,0);
	unguard;
	};

//
// Send all selected brushes in a level to the back of the hierarchy
//
void FGlobalEditor::mapSendToLast(ULevel *Level)
	{
	guard(FGlobalEditor::mapSendToLast);
	SendTo(Level,MF_Selected);
	unguard;
	};

void FGlobalEditor::mapSetBrush(ULevel *Level,EMapSetBrushFlags PropertiesMask,WORD BrushColor,FName GroupName,
	DWORD SetPolyFlags,DWORD ClearPolyFlags)
	{
	guard(FGlobalEditor::mapSetBrush);
	UModel		*Brush;
	//
	Level->Lock(LOCK_Trans);
	for (int i=1; i<Level->BrushArray->Num; i++)
		{
		Brush = Level->BrushArray->Element(i);
		if (Brush->ModelFlags & MF_Selected)
			{
			GTrans->NoteResHeader (Brush);
			if (PropertiesMask & MSB_BrushColor)
				{
				if (BrushColor==65535) // Remove color
					{
					Brush->ModelFlags &= ~MF_Color;
					}
				else // Set color
					{
					Brush->Color		= BrushColor;
					Brush->ModelFlags  |= MF_Color;
					};
				};
			if (PropertiesMask & MSB_Group)
				{
				Brush->Lock(LOCK_Trans);
				Brush->Polys->ModifyAllItems();
				for (INDEX j=0; j<Brush->Polys->Num; j++)
					{
					Brush->Polys(j).GroupName = GroupName;
					};
				Brush->Unlock(LOCK_Trans);
				};
			if (PropertiesMask & MSB_PolyFlags)
				{
				Brush->PolyFlags = (Brush->PolyFlags & ~ClearPolyFlags) | SetPolyFlags;
				};
			};
		};
	Level->Unlock(LOCK_Trans);
	unguard;
	};

/*---------------------------------------------------------------------------------------
   Poly texturing operations
---------------------------------------------------------------------------------------*/

//
// Pan textures on selected polys.  Doesn't do transaction tracking.
//
void FGlobalEditor::polyTexPan(UModel *Model,int PanU,int PanV,int Absolute)
	{
	guard(FGlobalEditor::polyTexPan);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if (Poly->PolyFlags & PF_Selected)
			{
			if (Absolute)
				{
				Poly->PanU = PanU;
				Poly->PanV = PanV;
				}
			else // Relative
				{
				Poly->PanU += PanU;
				Poly->PanV += PanV;
				};
			polyUpdateMaster (Model,i,0,0);
			};
		Poly++;
		};
	unguard;
	};

//
// Scale textures on selected polys. Doesn't do transaction tracking.
//
void FGlobalEditor::polyTexScale(UModel *Model,FLOAT UU,FLOAT UV, FLOAT VU, FLOAT VV,int Absolute)
	{
	FVector		OriginalU,OriginalV;
	FVector		NewU,NewV;
	//
	guard(FGlobalEditor::polyTexScale);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if (Poly->PolyFlags & PF_Selected)
			{
			OriginalU = Model->Vectors (Poly->vTextureU);
			OriginalV = Model->Vectors (Poly->vTextureV);
			//
			if (Absolute)
				{
				OriginalU *= 65536.0/OriginalU.Size();
				OriginalV *= 65536.0/OriginalV.Size();
				};
			//
			// Calc new vectors:
			//
			NewU = OriginalU * UU + OriginalV * UV;
			NewV = OriginalU * VU + OriginalV * VV;
			//
			// Update Bsp poly:
			//
			Poly->vTextureU = bspAddVector (Model,&NewU,0); // Add U vector
			Poly->vTextureV = bspAddVector (Model,&NewV,0); // Add V vector
			//
			// Update generating brush poly:
			//
			polyUpdateMaster	(Model,i,1,0);
			//
			Poly->iLightMesh	= INDEX_NONE;
			};
		Poly++;
		};
	unguard;
	};

//
// Align textures on selected polys.  Doesn't do any transaction tracking.
//
void FGlobalEditor::polyTexAlign(UModel *Model,ETexAlign TexAlignType,DWORD Texels)
	{
	FPoly			EdPoly;
	FVector			Base,Normal,U,V,Temp,OldBase;
	FModelCoords	Coords,Uncoords;
	FLOAT			BaseZ,Orientation,k;
	//
	guard(FGlobalEditor::polyTexAlign);
	for (INDEX i=0; i<Model->Surfs->Num; i++)
		{
		FBspSurf *Poly = &Model->Surfs(i);
		if (Poly->PolyFlags & PF_Selected)
			{
			polyFindMaster (Model,i,EdPoly);
			Normal = Model->Vectors(Poly->vNormal);
			//
			switch (TexAlignType)
				{
				case TEXALIGN_Default:
					//
					Orientation = Poly->Brush->BuildCoords (&Coords,&Uncoords);
					//
					EdPoly.TextureU  = GMath.ZeroVector;
					EdPoly.TextureV  = GMath.ZeroVector;
					EdPoly.Base      = EdPoly.Vertex[0];
					EdPoly.PanU      = 0;
					EdPoly.PanV      = 0;
					EdPoly.Finalize  (0);
					EdPoly.Transform (Coords,&GMath.ZeroVector,&GMath.ZeroVector,Orientation);
					//
		      		Poly->vTextureU 	= bspAddVector (Model,&EdPoly.TextureU,0);
	      			Poly->vTextureV 	= bspAddVector (Model,&EdPoly.TextureV,0);
					Poly->PanU			= EdPoly.PanU;
					Poly->PanV			= EdPoly.PanV;
					Poly->iLightMesh	= INDEX_NONE;
					//
					polyUpdateMaster	(Model,i,1,1);
					break;
				case TEXALIGN_Floor:
					//
					if (Abs(Normal.Z) > 0.05)
						{
						//
						// Shouldn't change base point, just base U,V
						//
						Base           	= Model->Points(Poly->pBase);
						OldBase         = Base;
						BaseZ          	= (Base | Normal) / Normal.Z;
						Base       		= GMath.ZeroVector;
						Base.Z     		= BaseZ;
			      		Poly->pBase 	= bspAddPoint (Model,&Base,1);
						//
						Temp.X 			= 65536.0;
						Temp.Y			= 0.0;
						Temp.Z			= 0.0;
						Temp			= Temp - Normal * (Temp | Normal);
						Poly->vTextureU	= bspAddVector (Model,&Temp,0);
						//
						Temp.X			= 0.0;
						Temp.Y 			= 65536.0;
						Temp.Z			= 0.0;
						Temp			= Temp - Normal * (Temp | Normal);
						Poly->vTextureV	= bspAddVector (Model,&Temp,0);
						//
						Poly->PanU       = 0;
						Poly->PanV       = 0;
						Poly->iLightMesh = INDEX_NONE;
						};
					polyUpdateMaster (Model,i,1,1);
					break;
				case TEXALIGN_WallDir:
					//
					// Align texture U,V directions for walls
					// U = (Nx,Ny,0)/sqrt(Nx^2+Ny^2)
					// V = (U dot N) normalized and stretched so Vz=1
					//
					if (Abs(Normal.Z)<0.95)
						{
						U.X = +Normal.Y;
						U.Y = -Normal.X;
						U.Z = 0.0;
						U  *= 65536.0/U.Size();
						V   = (U ^ Normal);
						V  *= 65536.0/V.Size();
						//
						if (V.Z > 0.0)
							{
							V *= -1.0;
							U *= -1.0;
							};
						Poly->vTextureU = bspAddVector (Model,&U,0);
						Poly->vTextureV = bspAddVector (Model,&V,0);
						//
						Poly->PanU			= 0;
						Poly->PanV			= 0;
						Poly->iLightMesh	= INDEX_NONE;
						//
						polyUpdateMaster (Model,i,1,0);
						};
					break;
				case TEXALIGN_WallPan:
					Base = Model->Points  (Poly->pBase);
					U    = Model->Vectors (Poly->vTextureU);
					V    = Model->Vectors (Poly->vTextureV);
					//
					if ((Abs(Normal.Z)<0.95) && (Abs(V.Z)>0.05))
						{
						k     = -Base.Z/V.Z;
						V    *= k;
						Base += V;
			      		Poly->pBase = bspAddPoint (Model,&Base,1);
						Poly->iLightMesh = INDEX_NONE;
						//
						polyUpdateMaster(Model,i,1,1);
						};
					break;
				case TEXALIGN_OneTile:
					Poly->iLightMesh = INDEX_NONE;
					polyUpdateMaster (Model,i,1,1);
					break;
				};
			};
		Poly++;
		};
	unguardf(("(Type=%i,Texels=%i",TexAlignType,Texels));
	};

/*---------------------------------------------------------------------------------------
   Map geometry link topic handler
---------------------------------------------------------------------------------------*/

AUTOREGISTER_TOPIC("Map",MapTopicHandler);
void MapTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(MapTopicHandler::Get);

	if( !Level || !Level->BrushArray )
		return;

	int NumBrushes  = Level->BrushArray->Num;
	int NumAdd	    = 0;
	int NumSubtract	= 0;
	int NumSpecial  = 0;
	int NumPolys    = 0;

	for( int i=1; i<NumBrushes; i++ )
	{
		UModel	*Brush        = Level->BrushArray->Element(i);
		UPolys	*BrushEdPolys = Brush->Polys;

		if      (Brush->CsgOper == CSG_Add)			NumAdd++;
		else if (Brush->CsgOper == CSG_Subtract)	NumSubtract++;
		else										NumSpecial++;

		NumPolys += BrushEdPolys->Num;
	}
	Level->Lock(LOCK_Read);
	if     ( stricmp(Item,"Brushes"     )==0 ) Out.Logf("%i",NumBrushes-1);
	else if( stricmp(Item,"Add"         )==0 ) Out.Logf("%i",NumAdd);
	else if( stricmp(Item,"Subtract"    )==0 ) Out.Logf("%i",NumSubtract);
	else if( stricmp(Item,"Special"     )==0 ) Out.Logf("%i",NumSpecial);
	else if( stricmp(Item,"AvgPolys"    )==0 ) Out.Logf("%i",NumPolys/Max(1,NumBrushes-1));
	else if( stricmp(Item,"TotalPolys"  )==0 ) Out.Logf("%i",NumPolys);
	else if( stricmp(Item,"Points"		)==0 ) Out.Logf("%i",Level->Model->Points->Num);
	else if( stricmp(Item,"Vectors"		)==0 ) Out.Logf("%i",Level->Model->Vectors->Num);
	else if( stricmp(Item,"Sides"		)==0 ) Out.Logf("%i",Level->Model->Verts->NumSharedSides);
	else if( stricmp(Item,"Zones"		)==0 ) Out.Logf("%i",Level->Model->Nodes->NumZones-1);
	else if( stricmp(Item,"Bounds"		)==0 ) Out.Logf("%i",Level->Model->Bounds->Num);
	else if( stricmp(Item,"DuplicateBrush")==0 )
	{
		// Make a unique copy of the current brush and return its name.
		UModel *PlaceAt = NULL;
		GetUModel(Item,"PLACEAT=",PlaceAt);

		// Make it a moving brush.
		//todo: Expand to handle moving and non moving brushes.
		UModel *NewBrush = GEditor->csgDuplicateBrush(Level,Level->BrushArray->Element(0),0,0,0,1);
		if( PlaceAt )
		{
			NewBrush->Location  = PlaceAt->Location;
			NewBrush->Rotation  = PlaceAt->Rotation;
			NewBrush->PrePivot  = PlaceAt->PrePivot;
			NewBrush->PostPivot = PlaceAt->PostPivot;
		}
		debugf( "Duplicate %s at %s", NewBrush ? NewBrush->GetName() : "NULL", PlaceAt ? PlaceAt->GetName() : "NULL" );
	}
	Level->Unlock(LOCK_Read);

	unguard;
}
void MapTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{}

/*---------------------------------------------------------------------------------------
   Polys link topic handler
---------------------------------------------------------------------------------------*/

AUTOREGISTER_TOPIC("Polys",PolysTopicHandler);
void PolysTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(PolysTopicHandler::Get);
	DWORD		OnFlags,OffFlags;

	Level->Lock(LOCK_Read);

	int n=0, StaticLights=0, DynamicLights=0, Meshels=0, MeshU=0, MeshV=0;
	OffFlags = (DWORD)~0;
	OnFlags  = (DWORD)~0;
	for( INDEX i=0; i<Level->Model->Surfs->Num; i++ )
	{
		FBspSurf *Poly = &Level->Model->Surfs(i);
		if( Poly->PolyFlags&PF_Selected )
		{
			OnFlags  &=  Poly->PolyFlags;
			OffFlags &= ~Poly->PolyFlags;
			n++;
			if( Poly->iLightMesh != INDEX_NONE )
			{
				FLightMeshIndex &Index = Level->Model->LightMesh->Element(Poly->iLightMesh);
				StaticLights	+= Index.NumStaticLights;
				DynamicLights	+= Index.NumDynamicLights;
				Meshels			+= Index.MeshUSize * Index.MeshVSize;
				MeshU            = Index.MeshUSize;
				MeshV            = Index.MeshVSize;
			}
		}
	}
	if      (!stricmp(Item,"NumSelected"))			Out.Logf("%i",n);
	else if (!stricmp(Item,"StaticLights"))			Out.Logf("%i",StaticLights);
	else if (!stricmp(Item,"DynamicLights"))		Out.Logf("%i",DynamicLights);
	else if (!stricmp(Item,"Meshels"))				Out.Logf("%i",Meshels);
	else if (!stricmp(Item,"SelectedSetFlags"))		Out.Logf("%u",OnFlags  & ~PF_NoEdit);
	else if (!stricmp(Item,"SelectedClearFlags"))	Out.Logf("%u",OffFlags & ~PF_NoEdit);
	else if (!stricmp(Item,"MeshSize") && n==1)		Out.Logf("%ix%i",MeshU,MeshV);
	Level->Unlock(LOCK_Read);

	unguard;
}
void PolysTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(PolysTopicHandler::Set);
	unguard;
}
