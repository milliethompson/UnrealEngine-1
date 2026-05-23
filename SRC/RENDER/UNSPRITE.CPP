/*=============================================================================
	UnSprite.cpp: Unreal sprite rendering functions.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"
#include "UnRaster.h"

//
// Parameters
//
#define SPRITE_PROJECTION_FORWARD	32.00 /* Move sprite projection planes forward */
#define NOSORT_SIZE					30.0  /* Don't bother sorting meshmaps smaller than this in either axis*/

//
// Addings sprites to per-node list:
//
#define ADD_START 100000.0
#define ADD_END   0.0

//
// Temporary globals.
//
FSprite *GFirstSprite;

/*------------------------------------------------------------------------------
	Dynamic node contents.
------------------------------------------------------------------------------*/

//
// Init buffer holding all dynamic contents.
//
void dynamicsLock(IModel *ModelInfo)
	{
	GUARD;
	if (GRender.DynamicsLocked) appError ("dynamicsLock: Already locked");
	//
	GRender.NumDynamics 	= 0;
	GRender.NumPostDynamics	= 0;
	GRender.DynamicsLocked	= 1;
	GFirstSprite            = NULL;
	GDynMem.InitPool();
	//
	UNGUARD("dynamicsLock");
	};

//
// Cleanup buffer holding all dynamic contents (call after rendering)
//
void dynamicsUnlock (IModel *ModelInfo)
	{
	GUARD;
	FDynamicsIndex	*DynamicsIndex;
	FBspNode		*Node;
	//
	if (!GRender.DynamicsLocked) appError ("dynamicsUnlocked: Not locked");
	//
	DynamicsIndex = GRender.DynamicsIndex;
	for (INDEX i=0; i<GRender.NumDynamics; i++)
		{
		Node 				= &ModelInfo->BspNodes [DynamicsIndex->iNode];
		Node->iDynamic[0]	= INDEX_NONE;
		Node->iDynamic[1]	= INDEX_NONE;
		DynamicsIndex++;
		};
	GRender.NumDynamics 	= 0;
	GRender.NumPostDynamics = 0;
	//
	GRender.DynamicsLocked  = 0;
	UNGUARD("dynamicsUnlock");
	};

//
// Add an empty entry to dynamic contents and link it up.
// Returns NULL if the dynamics structure is full.
// In this case, the rendering engine must recover gracefully.
//
FDynamicsIndex inline *dynamicsAdd (IModel *ModelInfo, INDEX iNode, INT Type,
	FSprite *Sprite, FRasterPoly *Raster, FLOAT Z,INT IsBack)
{
	GUARD;
	FBspNode		*Node = &ModelInfo->BspNodes [iNode];
	FDynamicsIndex	*Index, *TempIndex,*PrevIndex;
	INT 			iDynamic;

	if (GRender.NumDynamics >= GRender.MaxDynamics) return NULL;

	iDynamic	= GRender.NumDynamics++;
	Index    	= &GRender.DynamicsIndex[iDynamic];

	// Link into dynamics chain.
	if ((Z==ADD_START) || (Node->iDynamic[IsBack] == INDEX_NONE))
	{
		// Add at start of list.
		Index->iNext			= Node->iDynamic[IsBack];
		Node->iDynamic[IsBack]	= iDynamic;
	}
	else if (Z == ADD_END)
	{
		// Add at end of list.
		TempIndex = &GRender.DynamicsIndex [Node->iDynamic[IsBack]];
		while (TempIndex->iNext != INDEX_NONE) 
		{
			TempIndex = &GRender.DynamicsIndex [TempIndex->iNext];
		}
		Index->iNext     = INDEX_NONE;
		TempIndex->iNext = iDynamic;
	}
	else
	{
		// Z sort them it.  Things that must remain on top, such as sprites (not chunks) are
		// assigned a Z value of zero.
		TempIndex = &GRender.DynamicsIndex [Node->iDynamic[IsBack]];
		PrevIndex = NULL;

		while ((TempIndex->iNext != INDEX_NONE) && (TempIndex->Z >= Z))
		{
			PrevIndex = TempIndex;
			TempIndex = &GRender.DynamicsIndex[TempIndex->iNext];
		}
		if (PrevIndex == NULL)
		{
			Index->iNext			= Node->iDynamic[IsBack];
			Node->iDynamic[IsBack]  = iDynamic;
		}
		else
		{
			Index->iNext			= PrevIndex->iNext;
			PrevIndex->iNext		= iDynamic;
		}
	}
	Index->iNode    = iNode;
	Index->Type     = Type;
	Index->Sprite	= Sprite;
	Index->Raster	= Raster;
	Index->Z		= Z;

#if STATS
	switch (Type)
		{
		case DY_SPRITE:		GStat.NumSprites++;			break;
		case DY_CHUNK:		GStat.NumChunks++;			break;
		case DY_FINALCHUNK:	GStat.NumFinalChunks++; 	break;
		};
#endif
	return Index;
	UNGUARD("dynamicsAdd");
};

//
// Add all actors to dynamic contents, optionally excluding the player actor.
// Pass INDEX_NONE to exclude nothing.
//
void dynamicsSetup (ICamera *Camera, INDEX iExclude)
{
	GUARD;
	UActorList 		*Actors    = Camera->Level.Actors; 
	IModel 			*ModelInfo = &Camera->Level.ModelInfo;
	FSprite			*Sprite;

	// Skip if we shouldn't be rendering actors.
	if ((Camera->ShowFlags & SHOW_Actors) && ModelInfo->NumBspNodes)
	{
		// Traverse entire actor list.
		AActor **ActorPtr = &Actors->ActiveActors->First();
		AActor **LastPtr  = &Actors->ActiveActors->Last();
		while( ActorPtr <= LastPtr )
		{
			// Add this actor to dynamics if it's renderable.
			AActor *Actor = *ActorPtr++;
			if 
			(		(Actor->Class != NULL)
				&&	(Actor->iMe   != iExclude)
				&&	((Camera->Level.State==LEVEL_UpPlay)?!Actor->bHidden:!Actor->bHiddenEd) 
			)
			{
				// Allocate a sprite.
				Sprite  		= (FSprite *)GDynMem.GetFast(sizeof(FSprite));
				Sprite->Actor	= Actor;

				// Add the sprite.
				dynamicsAdd( ModelInfo,0,DY_SPRITE,Sprite,NULL,ADD_END,0 );
			}
		}
	}
	UNGUARD("dynamicsSetup");
}

/*------------------------------------------------------------------------------
	Dynamics filtering.
------------------------------------------------------------------------------*/

//
// Break a sprite into a chunk and set it up to begin filtering down the Bsp
// during rendering.
//
void inline MakeSpriteChunk
(
	IModel *ModelInfo, 
	ICamera *Camera, 
	INDEX iNode,
	FDynamicsIndex *Index
)
{
	GUARD;
	FSprite			*Sprite = Index->Sprite;
	FTransform		*Verts  = Sprite->Verts;

	// Compute four projection-plane points from sprite extents and camera.
	FLOAT FloatX1	= Sprite->X1; 
	FLOAT FloatX2	= Sprite->X2;
	FLOAT FloatY1	= Sprite->Y1; 
	FLOAT FloatY2	= Sprite->Y2;

	// Move closer to prevent actors from slipping into floor.
	FLOAT PlaneZ	= Sprite->Z - SPRITE_PROJECTION_FORWARD;
	FLOAT PlaneZRD	= PlaneZ * Camera->RProjZ;

	FLOAT PlaneX1   = PlaneZRD * (FloatX1 - Camera->FSXR2);
	FLOAT PlaneX2   = PlaneZRD * (FloatX2 - Camera->FSXR2);
	FLOAT PlaneY1   = PlaneZRD * (FloatY1 - Camera->FSYR2);
	FLOAT PlaneY2   = PlaneZRD * (FloatY2 - Camera->FSYR2);

	// Generate four screen-aligned box vertices.
	Verts[0].X = PlaneX1; Verts[0].Y = PlaneY1; Verts[0].Z = PlaneZ; Verts[0].ScreenX = FloatX1; Verts[0].ScreenY = FloatY1;
	Verts[1].X = PlaneX2; Verts[1].Y = PlaneY1; Verts[1].Z = PlaneZ; Verts[1].ScreenX = FloatX2; Verts[1].ScreenY = FloatY1;
	Verts[2].X = PlaneX2; Verts[2].Y = PlaneY2; Verts[2].Z = PlaneZ; Verts[2].ScreenX = FloatX2; Verts[2].ScreenY = FloatY2;
	Verts[3].X = PlaneX1; Verts[3].Y = PlaneY2; Verts[3].Z = PlaneZ; Verts[3].ScreenX = FloatX1; Verts[3].ScreenY = FloatY2;

	// Transform box into worldspace.
	for (INT i=0; i<4; i++) 
		Verts[i].TransformPoint(Camera->Uncoords);

	// Generate a full rasterization for this box, which we'll filter down the Bsp.
	FRasterPoly *Raster;
	Raster			= (FRasterPoly *)GDynMem.Get(sizeof(FRasterPoly) + (Sprite->Y2 - Sprite->Y1)*sizeof(FRasterLine));
	Raster->StartY	= Sprite->Y1;
	Raster->EndY	= Sprite->Y2;

	FRasterLine	*Line = &Raster->Lines[0];
	for ( i=Raster->StartY; i<Raster->EndY; i++ )
	{
		Line->Start.X = Sprite->X1;
		Line->End.X   = Sprite->X2;
		Line++;
	}

	// Add first sprite chunk at end of dynamics list so it will be processed in
	// the current node's linked rendering list (which we can assume is being
	// walked when this is called).
	dynamicsAdd (ModelInfo,iNode,DY_CHUNK,Sprite,Raster,ADD_END,0);

	UNGUARD("MakeSpriteChunk");
}

//
// Filter a chunk from the current Bsp node down to its children.  If this is a leaf,
// leave the chunk here.
//
void inline FilterChunk (IModel *ModelInfo, ICamera *Camera, INDEX iNode, FDynamicsIndex *Index,INT Outside)
{
	FSprite			*Sprite 	= Index->Sprite;
	FRasterPoly		*Raster 	= Index->Raster;
	FTransform		*Verts  	= Sprite->Verts;
	FBspNode		*Node		= &ModelInfo->BspNodes [iNode];
	FBspSurf		*Surf		= &ModelInfo->BspSurfs [Node->iSurf];
	FVector			*Normal		= &ModelInfo->FVectors [Surf->vNormal];
	FVector			*Base		= &ModelInfo->FPoints  [Surf->pBase];

	// Setup.
	FRasterPoly *FrontRaster, *BackRaster;

	// Find point-to-plane distances for all four vertices (side-of-plane classifications).
	INT Front=0, Back=0;
	FLOAT Dist[4];
	{
		for ( INT i=0; i<4; i++ )
		{
			Dist[i] = FPointPlaneDist (Verts[i],*Base,*Normal);
			Front  += Dist[i] > +0.01;
			Back   += Dist[i] < -0.01;
		}
		// Note: Both Front and Back can be zero
	}
	if (Front && Back)
	{	
		//
		// We must split the rasterization and filter it down both sides of this node.
		//

		// Find intersection points.
		FTransform	Intersect[4];
		FTransform	*I  = &Intersect	[0];
		FTransform  *V1 = &Verts		[3]; 
		FTransform  *V2 = &Verts		[0];
		FLOAT       *D1	= &Dist			[3];
		FLOAT		*D2	= &Dist			[0];
		INT			NumInt = 0;

		for ( INT i=0; i<4; i++ )
		{
			if (((*D1) * (*D2)) < 0.0)
			{	
				// At intersection point.
				FLOAT Alpha = *D1 / (*D1 - *D2);

				I->ScreenX = V1->ScreenX + Alpha * (V2->ScreenX - V1->ScreenX);
				I->ScreenY = V1->ScreenY + Alpha * (V2->ScreenY - V1->ScreenY);

				I++;
				NumInt++;
			}
			V1 = V2++;
			D1 = D2++;
		}
		if (NumInt<2) goto NoSplit;

		// Allocate front and back rasters.
		INT	Size	= sizeof (FRasterPoly) + (Raster->EndY - Raster->StartY) * sizeof (FRasterLine);
		FrontRaster	= (FRasterPoly *)GDynMem.Get(Size);
		BackRaster	= (FRasterPoly *)GDynMem.Get(Size);

		// Make sure that first intersection point is on top.
		if (Intersect[0].ScreenY > Intersect[1].ScreenY)
		{
			Intersect[2] = Intersect[0];
			Intersect[0] = Intersect[1];
			Intersect[1] = Intersect[2];
		}
		INT Y0 = ftoi(Intersect[0].ScreenY);
		INT Y1 = ftoi(Intersect[1].ScreenY);

		//
		// Figure out how to split up this raster into two new rasters and copy the right
		// data to each (top/bottom rasters plus left/right rasters).
		//

		// Find TopRaster.
		FRasterPoly *TopRaster = NULL;
		if (Y0 > Raster->StartY)
		{
			if (Dist[0] >= 0) TopRaster = FrontRaster;
			else              TopRaster = BackRaster;
		}

		// Find BottomRaster.
		FRasterPoly *BottomRaster = NULL;
		if (Y1 < Raster->EndY)
		{
			if (Dist[2] >= 0) BottomRaster = FrontRaster;
			else              BottomRaster = BackRaster;
		}

		// Find LeftRaster and RightRaster.
		FRasterPoly *LeftRaster, *RightRaster;
		if (Intersect[1].ScreenX >= Intersect[0].ScreenX)
		{
			if (Dist[1] >= 0.0) {LeftRaster = BackRaster;  RightRaster = FrontRaster;}
			else	   			{LeftRaster = FrontRaster; RightRaster = BackRaster; };
		}
		else // Intersect[1].ScreenX < Intersect[0].ScreenX
		{
			if (Dist[0] >= 0.0) {LeftRaster = FrontRaster; RightRaster = BackRaster; }
			else                {LeftRaster = BackRaster;  RightRaster = FrontRaster;};
		}

		// Get clipped min/max.
		INT ClippedY0 = Clamp(Y0,Raster->StartY,Raster->EndY);
		INT ClippedY1 = Clamp(Y1,Raster->StartY,Raster->EndY);

		// Set left and right raster defaults (may be overwritten by TopRaster or BottomRaster).
		LeftRaster->StartY = ClippedY0; RightRaster->StartY = ClippedY0;
		LeftRaster->EndY   = ClippedY1; RightRaster->EndY   = ClippedY1;

		// Copy TopRaster section.
		if (TopRaster)
		{
			TopRaster->StartY = Raster->StartY;

			FRasterLine	*SourceLine	= &Raster->Lines    [0];
			FRasterLine	*Line		= &TopRaster->Lines [0];

			for (i=TopRaster->StartY; i<ClippedY0; i++)
				*Line++ = *SourceLine++;
		}

		// Copy BottomRaster section.
		if (BottomRaster)
		{
			BottomRaster->EndY = Raster->EndY;

			FRasterLine	*SourceLine	= &Raster->Lines       [ClippedY1 - Raster->StartY];
			FRasterLine	*Line       = &BottomRaster->Lines [ClippedY1 - BottomRaster->StartY];

			for (i=ClippedY1; i<BottomRaster->EndY; i++)
				*Line++ = *SourceLine++;
		}

		// Split middle raster section.
		if (Y1 != Y0)
		{
			FLOAT	FloatYAdjust	= (FLOAT)Y0 + 1.0 - Intersect[0].ScreenY;
			FLOAT	FloatFixDX 		= 65536.0 * (Intersect[1].ScreenX - Intersect[0].ScreenX) / (Intersect[1].ScreenY - Intersect[0].ScreenY);
			INT		FixDX			= FloatFixDX;
			INT		FixX			= 65536.0 * Intersect[0].ScreenX + FloatFixDX * FloatYAdjust;

			if( Raster->StartY > Y0 ) 
			{
				FixX   += (Raster->StartY-Y0) * FixDX;
				Y0		= Raster->StartY;
			}
			if( Raster->EndY < Y1 )
			{
				Y1      = Raster->EndY;
			}
			
			FRasterLine	*SourceLine = &Raster->Lines      [Y0 - Raster->StartY];
			FRasterLine	*LeftLine   = &LeftRaster->Lines  [Y0 - LeftRaster->StartY];
			FRasterLine	*RightLine  = &RightRaster->Lines [Y0 - RightRaster->StartY];

			while( Y0++ < Y1 )
			{
				*LeftLine  = *SourceLine;
				*RightLine = *SourceLine;

				INT X = Unfix(FixX);
				if (X < LeftLine->End.X)    LeftLine->End.X    = X;
				if (X > RightLine->Start.X) RightLine->Start.X = X;

				FixX       += FixDX;
				SourceLine ++;
				LeftLine   ++;
				RightLine  ++;
			}
		}
		// Discard any rasters that are completely empty.
		if (BackRaster ->EndY <= BackRaster ->StartY)
			BackRaster  = NULL;
		if (FrontRaster->EndY <= FrontRaster->StartY) 
			FrontRaster = NULL;
	}
	else
	{
		// Don't have to split the rasterization.
		NoSplit:
		FrontRaster = BackRaster = Raster;
	}

	// Filter it down.
	INT CSG = Node->IsCsg();
	if (Front && FrontRaster)
	{
		if (Node->iFront != INDEX_NONE)	dynamicsAdd (ModelInfo,Node->iFront,DY_CHUNK,Sprite,FrontRaster,ADD_START,0);
		else if (Outside || CSG)		dynamicsAdd (ModelInfo,iNode,DY_FINALCHUNK,Sprite,FrontRaster,Sprite->Z,0);
	}
	if (Back && BackRaster) // Only add if this isn't a back leaf
	{
		if (Node->iBack != INDEX_NONE)	dynamicsAdd (ModelInfo,Node->iBack,DY_CHUNK,Sprite,BackRaster,ADD_START,0);
		else if (Outside && !CSG)		dynamicsAdd (ModelInfo,iNode,DY_FINALCHUNK,Sprite,BackRaster,Sprite->Z,1);
	}
}

//
// If there are any dynamics things in this node that need to be filtered down further,
// process them.  This is called while walking the Bsp tree during rendering _before_
// this node's children are walked.
//
// This routine should not draw anything or save the span buffer  The span buffer is not in
// the proper state to draw stuff because it may contain more holes now (since the front
// hasn't been drawn).  However, the span buffer can be used to reject stuff here.
//
// If FilterDown=0, the node should be treated as if it has no children so the contents are
// processed but not filtered further down.  This occurs when a Bsp node's polygons are
// Bound rejected and thus they can't possibly affect the visible results of drawing the actors.
// This saves lots of tree-walking time.
//
void dynamicsFilter (ICamera *Camera, INDEX iNode,INT FilterDown,INT Outside)
{
	GUARD;
	FDynamicsIndex 			*Index;
	IModel 					*ModelInfo	= &Camera->Level.ModelInfo;
	FBspNode				*Node		= &ModelInfo->BspNodes[iNode];
	INDEX					iDynamic	= Node->iDynamic[0];

	while (iDynamic != INDEX_NONE)
	{
		Index = &GRender.DynamicsIndex [iDynamic];
		switch (Index->Type)
		{
			case DY_SPRITE:
			{
				FSprite *Sprite = Index->Sprite;
				if ( GRender.SetupSprite( Camera,Sprite ) )
				{
					MakeSpriteChunk (ModelInfo,Camera,iNode,Index); // May set Index->iNext
				}
				break;
			}
			case DY_CHUNK:
			{
				if (FilterDown) FilterChunk (ModelInfo,Camera,iNode,Index,Outside);
				else            Index->Type = DY_FINALCHUNK;
				break;
			}
			case DY_FINALCHUNK:
			{
				break;
			}
		}
		iDynamic = Index->iNext;
	}
	UNGUARD("dynamicsFilter");
}

/*------------------------------------------------------------------------------
	Dynamics rendering and prerendering.
------------------------------------------------------------------------------*/

//
// This is called for a node's dynamic contents when the contents should be drawn.
// At this instant in time, the span buffer is set up properly for front-to-back rendering.
//
// * Any dynamic contents that can be drawn with span-buffered front-to-back rendering
//   should be drawn now.
//
// * For any dynamic contents that must be drawn transparently (using masking or
//   transparency), the span buffer should be saved.  The dynamic contents can later be
//   drawn back-to-front with the restored span buffer.
//
void dynamicsPreRender
(
	ICamera *Camera, 
	FSpanBuffer *SpanBuffer,
	INDEX iNode, 
	INT IsBack
)
{
	GUARD;
	IModel 			*ModelInfo	= &Camera->Level.ModelInfo;
	FBspNode		*Node 		= &ModelInfo->BspNodes[iNode];
	INDEX			iDynamic	= Node->iDynamic[IsBack];

	if (iDynamic != INDEX_NONE)
	{
		if (GRender.NumPostDynamics < GRender.MaxPostDynamics)
			GRender.PostDynamics[GRender.NumPostDynamics++] = iNode;
	}
	while (iDynamic != INDEX_NONE)
	{
		FDynamicsIndex	*Index	= &GRender.DynamicsIndex [iDynamic];
		FSprite			*Sprite = Index->Sprite;

		switch (Index->Type)
		{
			case DY_FINALCHUNK:
				if( !Index->Sprite->SpanBuffer )
				{
					// Creating a new span buffer for this sprite.
					Sprite->SpanBuffer = (FSpanBuffer *)GDynMem.Get(sizeof(FSpanBuffer));
					Sprite->SpanBuffer->AllocIndex(Index->Raster->StartY,Index->Raster->EndY,&GDynMem);

					if (Sprite->SpanBuffer->CopyFromRaster(*SpanBuffer,*Index->Raster))
					{
						// Span buffer is non-empty, so keep it and put it on the to-draw list.
						STAT(GStat.ChunksDrawn++);
						Sprite->Next	= GFirstSprite;
						GFirstSprite	= Sprite;
					}
					else
					{
						// Span buffer is empty, so ditch it.
						Sprite->SpanBuffer->Release();
						Sprite->SpanBuffer = NULL;
					}
				}
				else 
				{
					// Merging with the sprite's existing span buffer.
					// Creating a temporary span buffer.
					FSpanBuffer *Span = (FSpanBuffer *)GMem.Get(sizeof(FSpanBuffer));
					Span->AllocIndex(Index->Raster->StartY,Index->Raster->EndY,&GMem);

					if (Span->CopyFromRaster(*SpanBuffer,*Index->Raster))
					{
						// Temporary span buffer is non-empty, so merge it into sprite's.
						Sprite->SpanBuffer->MergeWith(*Span);
						STAT(GStat.ChunksDrawn++);
					}
					// Release the temporary memory.
					GMem.Release(Span);
				}
				break;
			case DY_CHUNK:
				break;
			case DY_SPRITE:
				break;
		}
		iDynamic = Index->iNext;
	}
	UNGUARD("dynamicsPreRender");
}

/*------------------------------------------------------------------------------
	Dynamics postrendering.
------------------------------------------------------------------------------*/

void dynamicsFinalize (ICamera *Camera, INT SpanRender)
{
	GUARD;

	if (Camera->Level.ModelInfo.NumBspNodes==0)
	{
		// Can't manage dynamic contents if no nodes.
		GRender.DrawLevelActors (Camera,INDEX_NONE);
	}
	else
	{
		// Draw all presaved sprites.
		FSprite *Sprite = GFirstSprite;
		while( Sprite != NULL )
		{
			GRender.DrawActorChunk (Camera,Sprite);
			Sprite = Sprite->Next;
		}
	}
	UNGUARD("dynamicsFinalize");
}

/*-----------------------------------------------------------------------------
	Scaled sprites.
-----------------------------------------------------------------------------*/

//
// Compute the extent (X1,Y1)-(X2,Y2) of a scaled sprite.
//
void inline CalcScaledSpriteExtent
(
	ICamera *Camera,
	FLOAT ScreenX, FLOAT ScreenY,
	FLOAT XSize, FLOAT YSize,
	INT *X1, INT *Y1, INT *X2, INT *Y2
) 
{
	ScreenX -= XSize * 0.5;
	ScreenY -= YSize * 0.5;

	// Find correctly rounded X and Y.
	*X1 = ftoi(ceil(ScreenX));
	*X2 = ftoi(ceil(ScreenX+XSize));
	if ( *X1 < 0 )
	{
		*X1 = 0;
		if (*X2 < 0) *X2 = 0;
	}
	if ( *X2 > Camera->SXR )
	{
		*X2 = Camera->SXR;
		if (*X1 > Camera->SXR) *X1 = Camera->SXR;
	}

	*Y1 = ftoi(ceil(ScreenY));
	*Y2 = ftoi(ceil(ScreenY+YSize));
	if ( *Y1 < 0 )
	{
		*Y1 = 0;
		if (*Y2 < 0) *Y2 = 0;
	}
	if ( *Y2 > Camera->SYR )
	{
		*Y2 = Camera->SYR;
		if (*Y1 > Camera->SYR) *Y1 = Camera->SYR;
	}
}

//
// Draw a scaled sprite.  Takes care of clipping.
// XSize and YSize are in pixels.
//
void FGlobalRender::DrawScaledSprite
(
	ICamera		*Camera, 
	UTexture	*Texture,
	FLOAT		ScreenX, 
	FLOAT		ScreenY, 
	FLOAT		XSize, 
	FLOAT		YSize, 
	INT			BlitType,
	FSpanBuffer *SpanBuffer,
	INT			Center,
	INT			Highlight
)
{
	FLOAT X1,X2,Y1,Y2;

	GUARD;
	if (!Texture) appError("Null texture");

	if (Center)
	{
		ScreenX -= XSize * 0.5;
		ScreenY -= YSize * 0.5;
	}
	X1 = ScreenX; X2 = ScreenX + XSize;
	Y1 = ScreenY; Y2 = ScreenY + YSize;	

	// Clip.
	if ( X1 < 0.0 ) 			X1 = 0.0;
	if ( X2 > Camera->FSXR ) 	X2 = Camera->FSXR;
	if ( X2 <= X1 )				return;

	if (SpanBuffer)
	{
		if ( Y1 < SpanBuffer->StartY )	Y1 = SpanBuffer->StartY;
		if ( Y2 > SpanBuffer->EndY   )	Y2 = SpanBuffer->EndY;
	}
	else
	{
		if ( Y1 < 0.0 ) 				Y1 = 0.0;
		if ( Y2 > Camera->FSYR ) 		Y2 = Camera->FSYR;
	}
	if ( Y2<=Y1 ) 
		return;

	// Find correctly sampled U and V start and end values.
	FLOAT UScale = 65536.0 * (FLOAT)Texture->USize / XSize;
	FLOAT VScale = 65536.0 * (FLOAT)Texture->VSize / YSize;

	FLOAT U1 = (X1 - ScreenX) * UScale - 0.5;
	FLOAT V1 = (Y1 - ScreenY) * VScale - 0.5;

	FLOAT U2 = (X2 - ScreenX) * UScale - 0.5;
	FLOAT V2 = (Y2 - ScreenY) * VScale - 0.5;

	FLOAT G = Highlight ? (56.0 * 256.0) : (26.0 * 256.0);

	// Build and draw poly.
	FRasterTexPoly *RasterTexPoly = (FRasterTexPoly	*)GMem.Get
	(
		sizeof(FRasterTexPoly)+FGlobalRaster::MAX_RASTER_LINES*sizeof(FRasterTexLine)
	);
	FTransTex P[4];
	P[0].ScreenX = X1; P[0].ScreenY = Y2; P[0].U = U1; P[0].V = V2; P[0].G = G; ftoi(P[0].IntX,P[0].ScreenX-0.5); ftoi(P[0].IntY,P[0].ScreenY-0.5);
	P[1].ScreenX = X1; P[1].ScreenY = Y1; P[1].U = U1; P[1].V = V1; P[1].G = G; ftoi(P[1].IntX,P[1].ScreenX-0.5); ftoi(P[1].IntY,P[1].ScreenY-0.5);
	P[2].ScreenX = X2; P[2].ScreenY = Y1; P[2].U = U2; P[2].V = V1; P[2].G = G; ftoi(P[2].IntX,P[2].ScreenX-0.5); ftoi(P[2].IntY,P[2].ScreenY-0.5);
	P[3].ScreenX = X2; P[3].ScreenY = Y2; P[3].U = U2; P[3].V = V2; P[3].G = G; ftoi(P[3].IntX,P[3].ScreenX-0.5); ftoi(P[3].IntY,P[2].ScreenY-0.5);

	FRasterTexSetup RasterTexSetup;
	RasterTexSetup.Setup	(*Camera,P,4,&GMem);
	RasterTexSetup.Generate	(RasterTexPoly);

	DWORD TheseFlags=0;
	switch (BlitType)
	{
		case BT_None:			TheseFlags = PF_Masked | PF_NoSmooth;		break;
		case BT_Normal:			TheseFlags = PF_Masked | 0;					break;
		case BT_Transparent:	TheseFlags = PF_Masked | PF_Transparent;	break;
	}
	FPolySpanTextureInfoBase *Info
		=	GSpanTextureMapper->SetupForPoly(*Camera,Texture,NULL,TheseFlags,0);
	RasterTexPoly->Draw(*Camera,*Info,SpanBuffer);
	GSpanTextureMapper->FinishPoly(Info);

	GMem.Release(RasterTexPoly);

	UNGUARD_BEGIN;
	UNGUARD_MSGF("FGlobalRender::DrawScaledSprite (Y1=%f Y2=%f)",Y1,Y2);
	UNGUARD_END;
}

/*-----------------------------------------------------------------------------
	Texture block drawing.
-----------------------------------------------------------------------------*/

//
// Draw a block of texture on the screen.
//
void FGlobalRender::DrawTiledTextureBlock(ICamera *Camera,UTexture *Texture,
	INT X, INT XL, INT Y, INT YL,INT U,INT V)
{
	guard(FGlobalRender::DrawTiledTextureBlock);

	// Get texture data.
	ITexture TextureInfo;
	INT 	MipLevel=0,USize,VSize;
	BYTE	*Data	= Texture->GetData(TextureInfo,&MipLevel,Camera->ColorBytes,&USize,&VSize);
	FCacheItem *Item = NULL;

	// Setup.
	INT UAnd		= USize-1;
	INT VAnd		= VSize-1;
	BYTE VShift		= Texture->UBits;
	BYTE *Dest1		= &Camera->Screen[(X + Y*Camera->Stride)*Camera->ColorBytes];
	INT XE			= X+XL+U;
	INT YE			= Y+YL+V;
	
	X += U; 
	Y += V;

	if( Camera->ColorBytes==1 )
	{
		FColor *Colors = &Texture->Palette->Element(0);
		for (V=Y; V<YE; V++)
		{
			BYTE *Src  = &Data[(V & VAnd) << VShift];
			BYTE *Dest = Dest1;
			for (INT U=X; U<XE; U++) *Dest++ = Src[U&UAnd];
			Dest1 += Camera->Stride;
		}
	}
	else if( Camera->ColorBytes==2 )
	{
		WORD *HiColors = Texture->Palette->GetColorDepthPalette(Item,Camera->Texture).PtrWORD;
		for (V=Y; V<YE; V++)
		{
			BYTE *Src  = &Data[(V & VAnd) << VShift];
			WORD *Dest = (WORD *)Dest1;
			for (INT U=X; U<XE; U++) *Dest++ = HiColors[Src[U&UAnd]];
			Dest1 += Camera->Stride*2;
		}
	}
	else if( Camera->ColorBytes==4 )
	{
		DWORD *TrueColors = Texture->Palette->GetColorDepthPalette(Item,Camera->Texture).PtrDWORD;
		for( V=Y; V<YE; V++ )
		{
			BYTE  *Src  = &Data[(V & VAnd) << VShift];
			DWORD *Dest = (DWORD *)Dest1;
			for (U=X; U<XE; U++) *Dest++ = TrueColors[Src[U&UAnd]];
			Dest1 += Camera->Stride*4;
		}
	}
	else
	{
		appErrorf("Invalid color depth %i",Camera->ColorBytes);
	}

	// Unlock the texture.
	Texture->Unlock(TextureInfo);
	if( Item ) Item->Unlock();
	
	unguard;
}

/*-----------------------------------------------------------------------------
	Actor drawing (sprites and chunks).
-----------------------------------------------------------------------------*/

//
// Draw a Sprite (sprite or scaled sprite) with no span clipping
//
void FGlobalRender::DrawActorSprite( ICamera *Camera,FSprite *Sprite )
{
	GUARD;
	AActor *Actor = Sprite->Actor;
	INT ModeClass = GEditor ? GEditor->edcamModeClass(GEditor->Mode) : EMC_None;

	if (GEditor && GEditor->Scan.Active) 
		GEditor->Scan.PreScan();

	if
	(
			(Actor->DrawType==DT_Sprite) 
		||	(Actor->DrawType==DT_Brush) 
		||	(Camera->ShowFlags & SHOW_ActorIcons)
	)
	{
		if ( Actor->Texture )
		{
			DrawScaledSprite
			(
				Camera,Actor->Texture,
				Sprite->ScreenX,Sprite->ScreenY,
				Sprite->Scale * Actor->Texture->USize, Sprite->Scale * Actor->Texture->VSize,
				Actor->BlitType,0,1,Actor->bSelected
			);
			if ( Camera->Camera->IsOrtho() )
			{
				if ( Camera->ShowFlags & SHOW_ActorRadii )
				{
					if (Actor->bCollideActors)
					{
						DrawCircle(Camera,Actor->Location,Actor->CollisionRadius,ActorArrowColor,1);
					}
					if ( (Actor->LightType!=LT_None) && Actor->bSelected )
					{
						DrawCircle(Camera,Actor->Location,Actor->WorldLightRadius(),ActorArrowColor,1);
					}
				}
				if ( Actor->bDirectional
					&& (ModeClass==EMC_Actor || Actor->IsKindOf("Camera")))
				{
					GGfx.ArrowBrush->Location = Actor->Location;
					GGfx.ArrowBrush->Rotation = Actor->DrawRot;
					GGfx.ArrowBrush->Bound[1].Init(); // Don't bound-reject.
					GRender.DrawBrushPolys (Camera,GGfx.ArrowBrush,ActorArrowColor,0,NULL,1,0,Actor->bSelected,0);
				}
			}
		}
	}
	else if ( (Actor->DrawType==DT_MeshMap) && (Actor->MeshMap) )
	{
		DrawMeshMap
		(
			*Camera,
			*Actor->MeshMap,
			NULL,
			Actor->Location,
			Actor->DrawRot,
			Actor->bSelected,
			Sprite,
			!Actor->bUnlit,
			!Actor->bNoSmooth
		);
	}
	if (GEditor && GEditor->Scan.Active) 
		GEditor->Scan.PostScan(EDSCAN_Actor,Sprite->Actor->iMe,0,0,NULL);

	UNGUARD("FGlobalRender::DrawActorSprite");
}

//
// Draw an actor clipped to a span buffer representing the portion of
// the actor that falls into a particular Bsp leaf.
//
void FGlobalRender::DrawActorChunk (ICamera *Camera,FSprite *Sprite)
{
	GUARD;
	if (GEditor && GEditor->Scan.Active)
		GEditor->Scan.PreScan();

	AActor *Actor = Sprite->Actor;
	if
	(
			(Actor->DrawType==DT_Sprite) 
		||	(Actor->DrawType==DT_Brush) 
		||	(Camera->ShowFlags & SHOW_ActorIcons)
	)
	{
		if (Actor->Texture) DrawScaledSprite
		(
			Camera,Actor->Texture,
			Sprite->ScreenX,Sprite->ScreenY,
			Sprite->Scale * Actor->Texture->USize, Sprite->Scale * Actor->Texture->VSize,
			Actor->BlitType,Sprite->SpanBuffer,1,Actor->bSelected
		);
	}
	else if ((Actor->DrawType==DT_MeshMap) && (Actor->BlitType!=BT_None) && (Actor->MeshMap!=NULL))
	{
		DrawMeshMap
		(
			*Camera,
			*Actor->MeshMap,
			Sprite->SpanBuffer,
			Actor->Location,
			Actor->DrawRot,
			Actor->bSelected,
			Sprite,
			!Actor->bUnlit,
			!Actor->bNoSmooth
		);
	}
	if (GEditor && GEditor->Scan.Active)
		GEditor->Scan.PostScan(EDSCAN_Actor,Sprite->Actor->iMe,0,0,NULL);
	UNGUARD("FGlobalRender::DrawActorChunk");
}

/*-----------------------------------------------------------------------------
	Sprite information.
-----------------------------------------------------------------------------*/

//
// Set the Sprite structure corresponding to an actor;  returns 1 if the actor
// is visible, or 0 if the actor is occluded.  If occluded, the Sprite isn't
// visible and should be ignored.
//
INT FGlobalRender::SetupSprite( ICamera *Camera, FSprite *Sprite )
{
	GUARD;

	// Setup.
	AActor	*Actor		= Sprite->Actor;
	UClass	*Class		= Actor->Class;
	FVector Location	= Actor->Location - Camera->Coords.Origin;

	// Init sprite info.
	Sprite->Z			= Location | Camera->Coords.ZAxis;
	Sprite->SpanBuffer	= NULL;

	// Figure out what to do with this.
	if ((Sprite->Z < -SPRITE_PROJECTION_FORWARD) && !Camera->Camera->IsWire())
	{
		// This is not possibly visible.
		return 0;
	}

	// Get the zone descriptor.
	Sprite->ZoneDescriptor = Camera->Level.GetZoneDescriptor(Actor->Zone);

	// Handle the actor based on its type.
	if ( (Actor->DrawType==DT_Sprite) 
		||	(Actor->DrawType==DT_Brush) 
		||	(Camera->ShowFlags & SHOW_ActorIcons) )
	{
		// This is a sprite.
		if ((Actor->DrawType!=DT_Brush) || (GEditor!=NULL))
		{
			// Make sure we have something to draw.
			if (!Actor->Texture) 
				return 0;

			// See if this is occluded.
			if (!GRender.Project (Camera,&Actor->Location,&Sprite->ScreenX,&Sprite->ScreenY,&Sprite->Scale)) 
				return 0;

			// Compute sprite extent.
			Sprite->Scale *= Actor->DrawScale;
			CalcScaledSpriteExtent
			(
				Camera,
				Sprite->ScreenX,Sprite->ScreenY,
				Sprite->Scale * Actor->Texture->USize,Sprite->Scale * Actor->Texture->VSize,
				&Sprite->X1,&Sprite->Y1,&Sprite->X2,&Sprite->Y2
			);
		}
		// Return whether the sprite is visible.
		return ((Sprite->Y1 < Camera->SYR) && (Sprite->Y2 > 0));
	}
	else if ((Actor->DrawType==DT_MeshMap) && Actor->MeshMap)
	{
		// This is a meshmap.
		UMeshMap *MeshMap	= Actor->MeshMap;
		UMesh *Mesh			= MeshMap->Mesh;
		Location			= Actor->Location;

		// Get information about the meshmap.
		IMesh MeshInfo;
		Mesh->GetInfo(&MeshInfo);

		// Set and validate the meshmap's frame.
		if (Actor->AnimSeq>=Mesh->NumAnimSeqs)
		{
			Sprite->iFrame=0;
		}
		else
		{
			Sprite->iFrame = 
				MeshInfo.AnimSeqs[Actor->AnimSeq].SeqStartFrame
			+	((int)Actor->AnimBase%(MeshInfo.AnimSeqs[Actor->AnimSeq].SeqNumFrames));
		}
		if (Sprite->iFrame>=Mesh->NumAnimFrames) 
			Sprite->iFrame=0;

		// Setup rendering info for the meshmap.
		if (Camera->Camera->IsWire())
		{
			// Setup wireframe rendering.
			if (!GRender.Project (Camera,&Location,&Sprite->ScreenX,&Sprite->ScreenY,NULL)) 
				return 0;

			Sprite->Scale = Actor->DrawScale;
			Sprite->X1	  = Sprite->ScreenX; Sprite->X2 = Sprite->ScreenX;
			Sprite->Y1	  = Sprite->ScreenY; Sprite->Y2 = Sprite->ScreenY;
		}
		else
		{
			// Setup 3D rendering.
			Sprite->Scale = Actor->DrawScale;

			// Get pointer to mesh's per-frame bounding Bound.
			FBoundingRect Bounds = MeshInfo.Bound[Sprite->iFrame];

			// Transform Bound by scale and origin.
			Bounds.Min.X = -1.0 + Location.X + MeshMap->Scale.X * (Bounds.Min.X - Mesh->Origin.X) * Actor->DrawScale;
			Bounds.Min.Y = -1.0 + Location.Y + MeshMap->Scale.Y * (Bounds.Min.Y - Mesh->Origin.Y) * Actor->DrawScale;
			Bounds.Min.Z = -1.0 + Location.Z + MeshMap->Scale.Z * (Bounds.Min.Z - Mesh->Origin.Z) * Actor->DrawScale;

			Bounds.Max.X = +1.0 + Location.X + MeshMap->Scale.X * (Bounds.Max.X - Mesh->Origin.X) * Actor->DrawScale;
			Bounds.Max.Y = +1.0 + Location.Y + MeshMap->Scale.Y * (Bounds.Max.Y - Mesh->Origin.Y) * Actor->DrawScale;
			Bounds.Max.Z = +1.0 + Location.Z + MeshMap->Scale.Z * (Bounds.Max.Z - Mesh->Origin.Z) * Actor->DrawScale;

			FCoords BoxCoords = GMath.UnitCoords;
			BoxCoords.DeTransformByRotation(Actor->DrawRot);
			BoxCoords.DeTransformByRotation(Mesh->RotOrigin);
			BoxCoords.Origin = Location;

			// Try to view pyramid and span reject it.
			FScreenBounds ScreenBounds;
			if (!GRender.BoundVisible (Camera,&Bounds,NULL,&ScreenBounds,&BoxCoords)) 
				return 0;

			// Set extent.
			if (ScreenBounds.Valid)
			{
				// Screen bounds are valid, so set the extent.
				Sprite->X1 = Max ((INT)0,ScreenBounds.MinX); Sprite->X2 = Min (Camera->SXR,ScreenBounds.MaxX);
				Sprite->Y1 = Max ((INT)0,ScreenBounds.MinY); Sprite->Y2 = Min (Camera->SYR,ScreenBounds.MaxY);
			}
			else
			{
				// Screen bounds aren't valid, so the extent is potentially the whole screen.
				Sprite->X1 = 0; Sprite->X2 = Camera->SXR;
				Sprite->Y1 = 0; Sprite->Y2 = Camera->SYR;
			}
		}
		// This is potential visible.
		return 1;
	}
	// Not potentially visible.
	return 0;
	UNGUARD("FGlobalRender::SetupSprite");
}

/*-----------------------------------------------------------------------------
	Wireframe view drawing.
-----------------------------------------------------------------------------*/

//
// Draw all moving brushes in the level as wireframes.
//
void FGlobalRender::DrawMovingBrushWires( ICamera *Camera )
{
	guard(FGlobalRender::DrawMovingBrushWires);

	UActorList *Actors = Camera->Level.Actors;
	AActor **ActorPtr  = &Actors->ActiveActors->First();
	AActor **LastPtr   = &Actors->ActiveActors->Last();
	while( ActorPtr <= LastPtr )
	{
		AActor *Actor = *ActorPtr++;

		if (Actor->Class && Actor->IsMovingBrush())
		{
			if (GEditor && GEditor->Scan.Active)
				GEditor->Scan.PreScan();

			if (!GEditor)
				Actor->UpdateBrushPosition(&Camera->Level,Actor->iMe,0);

			DrawBrushPolys(Camera,Actor->Brush,MoverColor,0,NULL,Actor->bSelected,Actor->bSelected,Actor->bSelected,0);

			if (GEditor && GEditor->Scan.Active) 
				GEditor->Scan.PostScan(EDSCAN_Actor,Actor->iMe,0,0,NULL);
		}
	}
	unguard;
}

//
// Just draw an actor, no span occlusion
//
void FGlobalRender::DrawActor( ICamera *Camera,INDEX iActor )
{
	GUARD;
	FSprite	Sprite;
	Sprite.Actor = &Camera->Level.Actors->Element(iActor);
	if ( SetupSprite( Camera, &Sprite) )
	{
		DrawActorSprite( Camera, &Sprite );
	}
	UNGUARD("FGlobalRender::DrawActor");
}

//
// Draw all actors in a level without doing any occlusion checking or sorting.
// For wireframe views.
//
void FGlobalRender::DrawLevelActors(ICamera *Camera, INDEX iExclude)
{
	GUARD;
	UActorList *Actors = Camera->Level.Actors;

	AActor **ActorPtr = &Actors->ActiveActors->First();
	AActor **LastPtr  = &Actors->ActiveActors->Last();
	while( ActorPtr <= LastPtr )
	{
		AActor *Actor = *ActorPtr++;
		if
		(
				Actor->Class 
			&&	(Actor->iMe != iExclude) 
			&&	((Camera->Level.State==LEVEL_UpPlay)?!Actor->bHidden:!Actor->bHiddenEd)
		)
		{

			// If this actor is an event source, draw event lines connecting it to
			// all corresponding event sinks.
			if ((Actor->Event!=NAME_NONE) && GEditor && 
				(GEditor->edcamModeClass(GEditor->Mode)==EMC_Actor))
			{
				AActor **OtherActorPtr = &Actors->ActiveActors->First();
				AActor **OtherLastPtr  = &Actors->ActiveActors->Last();
				while( OtherActorPtr <= OtherLastPtr )
				{
					AActor *OtherActor = *OtherActorPtr++;
					if
					(
							OtherActor->Class 
						&&	(OtherActor->Name == Actor->Event) 
						&&	((Camera->Level.State==LEVEL_UpPlay)?!Actor->bHidden:!Actor->bHiddenEd))
					{
						GRender.Draw3DLine
						(
							Camera,
							&Actor->Location, 
							&OtherActor->Location,
							1,ActorArrowColor,0,1
						);
					}
				}
			}
			
			// Draw this actor.
			FSprite Sprite;
			Sprite.Actor = Actor;
			
			if( SetupSprite(Camera,&Sprite) )
			{
				DrawActorSprite (Camera,&Sprite);
			}
		}
	}
	UNGUARD("FGlobalRender::DrawLevelActors");
}

/*------------------------------------------------------------------------------
	Mesh sprites.
------------------------------------------------------------------------------*/

//
// Structure used by DrawMeshMap for sorting triangles.
//
struct FMeshTriSort
{
	FTransTex*	FirstPoint;	// First point of clipped triangle point list.
	FLOAT		MinZ;		// Z minima on clipped triangle.
	INT 		Index;		// Index of triangle in triangle table.
	INT 		NumPts;		// Number of points in clipped triangle, >= 3.
	friend inline INT Compare(const FMeshTriSort &A, const FMeshTriSort &B)
	{
		return 1 - 2*(A.MinZ > B.MinZ);
	}
};

//
// Draw a mesh map.
//
void FGlobalRender::DrawMeshMap
(
	ICamera &Camera, UMeshMap &MeshMap,FSpanBuffer *SpanBuffer,
	FVector &Location, FRotation &Rotation, INT Highlight, FSprite *Sprite,
	INT DrawLit, INT DrawSmooth
)
{
	guard(FGlobalRender::DrawMeshMap);

	FVector				VertexNormal,Side1,Side2,*FVectPool;
	FPoly				EdPoly;
	FCoords				Coords;
	FTransTex			*Pts,*PtsPool,*P;
	FTransform			*Verts,*Vert,*V1,*V2,*V3;
	FMeshTriSort 		*TriPool,*TriPtr,*TriTop;
	FMeshVertLinkIndex	*VertLinkIndex;
	FRasterTexPoly		*RasterTexPoly;
	FRasterTexSetup		RasterTexSetup;
	INT 				*VertexShade;
	WORD				Color,*VertLink;
	BYTE				Outcode = FVF_OutReject;
	INT 				i,j,k,n,FrameOffset,NumPts,VisibleTriangles;
	INT 				Temp=0,TotalPts,iVertex;

	// Process special poly flags.
	DWORD ExtraFlags = DrawSmooth ? 0 : PF_NoSmooth;

	// Lock the mesh map.
	IMeshMap MeshMapInfo;
	MeshMap.Lock(&MeshMapInfo);

	// Grab all memory needed for drawing the meshmap.
	Verts 		  = (FTransform		*)GMem.Get(MeshMapInfo.NumVertices * sizeof  (FTransform));
	VertexShade	  = (INT			*)GMem.Get(MeshMapInfo.NumVertices * sizeof  (INT));
	TriPool		  = (FMeshTriSort	*)GMem.Get(64 * MeshMapInfo.NumTriangles * sizeof (FMeshTriSort)); // Way overestimate
	Pts 		  = (FTransTex		*)GMem.Get(32 * sizeof (FTransTex));
	RasterTexPoly = (FRasterTexPoly	*)GMem.Get(sizeof(FRasterTexPoly)+FGlobalRaster::MAX_RASTER_LINES*sizeof(FRasterTexLine));
	PtsPool		  = (FTransTex		*)GMem.Get(0); // Will be expanded	

	// Compute the scale.
	FVector Scale = MeshMapInfo.Scale * Sprite->Scale;

	// Figure out what frame we're drawing.
	FrameOffset	= Sprite->iFrame * MeshMapInfo.NumVertices;

	// Render a wireframe view or textured view.
	if (Camera.Camera->IsWire() || Camera.Camera->IsOrtho())
	{
		//
		// Render a wireframe view.
		//

		// Setup coordinate system.
		Coords      = GMath.UnitCoords;
		Coords.TransformByRotation (Rotation);
		Coords.TransformByRotation (MeshMapInfo.RotOrigin);

		// Set up color.
		if (Highlight)	Color = ActorHiWireColor;
		else			Color = ActorWireColor;

		// Transform all points.
		FVectPool = (FVector *)GMem.Get(MeshMapInfo.NumVertices*sizeof(FVector));
		for (i=0; i<MeshMapInfo.NumVertices; i++) // Transform all points
		{
			FMeshVertex &MeshVertex = MeshMapInfo.Vertex[FrameOffset + i];

			FVectPool[i].X	= Scale.X * (MeshVertex.X - MeshMapInfo.Origin.X);
			FVectPool[i].Y	= Scale.Y * (MeshVertex.Y - MeshMapInfo.Origin.Y);
			FVectPool[i].Z	= Scale.Z * (MeshVertex.Z - MeshMapInfo.Origin.Z);

			FVectPool[i].TransformVector(Coords);	 
			FVectPool[i] += Location;
		}

		// Render each triangle.
		FMeshTriangle *Triangle = &MeshMapInfo.Triangles[0]; 
		for (i=0; i<MeshMapInfo.NumTriangles; i++)
			{
			// Build an FPoly from this triangle.
			EdPoly.Init();
			EdPoly.NumVertices = 3;
			for (j=0; j<3; j++) EdPoly.Vertex[j] = FVectPool[Triangle->iVertex[j]];
			
			// Validate the FPoly.
			if (EdPoly.Finalize(1)==0)
			{
				// Set FPoly's flags.
				if (Triangle->Type==MT_Masked) EdPoly.PolyFlags |= PF_Masked | PF_NotSolid;
				else if (Triangle->Type==MT_Transparent) EdPoly.PolyFlags |= PF_Transparent | PF_NotSolid;
				EdPoly.PolyFlags |= PF_NotSolid;

				// Render it.
				GRender.DrawFPoly(&Camera,&EdPoly,Color,1,0);
			}
			Triangle++;
		}
	}
	else
	{
		//
		// 3D textured view: Build coordinate system to get creature into screenspace.
		//

		// Setup coordinate system.
		Coords = Camera.Coords;
		Coords.TransformByRotation	(Rotation);
		Coords.TransformByRotation	(MeshMapInfo.RotOrigin);
		FVector TempVector = Location - Camera.Coords.Origin;
		TempVector.TransformVector (Camera.Coords);

		// Transform all points into screenspace and compute outcodes.
		FrameOffset				= Sprite->iFrame * MeshMapInfo.NumVertices;
		FMeshVertex *MeshVertex = &MeshMapInfo.Vertex	[FrameOffset];
		Vert					= &Verts 				[0];

		for (i=0; i<MeshMapInfo.NumVertices; i++)
		{
			Vert->X = Scale.X * (MeshVertex->X - MeshMapInfo.Origin.X);
			Vert->Y = Scale.Y * (MeshVertex->Y - MeshMapInfo.Origin.Y);
			Vert->Z = Scale.Z * (MeshVertex->Z - MeshMapInfo.Origin.Z);

			Vert->TransformVector(Coords);
			*Vert += TempVector;

			Vert->ComputeOutcode(Camera);
			Outcode &= Vert->Flags;

			MeshVertex++;
			Vert++;
		}
		if( !Outcode )
		{
			// Some of the vertices are in the view frustrum.

			// Set up list for triangle sorting, adding all non-backfaced triangles.
			FMeshTriangle	*Triangle	= &MeshMapInfo.Triangles [0]; 		
			TriTop   			= &TriPool               [0];
			VisibleTriangles	= 0;

			for (i=0; i<MeshMapInfo.NumTriangles; i++)
			{
				V1 = &Verts [Triangle->iVertex[0]];
				V2 = &Verts [Triangle->iVertex[1]];
				V3 = &Verts [Triangle->iVertex[2]];

				// Try to outcode-reject the triangle.
				if (!(V1->Flags & V2->Flags & V3->Flags))
				{
					//
					// See if this triangle is either two-sided or front-faced:
					//
					if (	(Triangle->Type==MT_Masked)
						||	(Triangle->Type==MT_Transparent)
						||	((*V1 | (*V2 ^ *V3))>0.0))
					{
						TriTop->Index = i;
						TriTop->MinZ =
							Verts [Triangle->iVertex[0]].Z +
							Verts [Triangle->iVertex[1]].Z +
							Verts [Triangle->iVertex[2]].Z;
						VisibleTriangles++;
						TriTop++;
					}
				}
				Triangle++;
			}
			if (VisibleTriangles>0)
			{
				//
				// One or more triangles is visible.
				//

				// If creature is large on screen, sort it.
				if (((Sprite->X2-Sprite->X1)>=NOSORT_SIZE) && ((Sprite->Y2-Sprite->Y1)>=NOSORT_SIZE))
				{
					QSort(TriPool,VisibleTriangles);
				}

				// Build list of all incident lights on the creature.
				// Considers only the entire creature and does not
				// differentiate between individual creature polys.
				GLightManager->SetupForActor(&Camera,Sprite->Actor->iMe);

				for (i=0; i<MeshMapInfo.NumVertices; i++)
				{
					// Tag as uncomputed.
					VertexShade[i] = 0;
				}

				// Perform all vertex lighting.
				TriPtr = &TriPool[0];
				for (i=0; i<VisibleTriangles; i++)
				{
					FMeshTriangle &Triangle = MeshMapInfo.Triangles[TriPtr->Index];

					for (j=0; j<3; j++)
					{
						iVertex = Triangle.iVertex[j];
						if (VertexShade[iVertex]==0)
						{
							FTransform *P = &Verts[iVertex];
							if (!P->Flags)
							{
								// Transformation is done, don't generate again.
								P->iTransform = 0;
								FLOAT Factor  = Camera.ProjZ / P->Z;
								P->ScreenX    = P->X * Factor + Camera.FSXR15;
								P->ScreenY    = P->Y * Factor + Camera.FSYR15;
							}
							else
							{
								// Must transform after clipping.
								P->iTransform = MAXWORD;
							}

							VertexNormal 	= GMath.ZeroVector;

							VertLinkIndex	= &MeshMapInfo.VertLinkIndex [iVertex];
							VertLink     	= &MeshMapInfo.VertLinks     [VertLinkIndex->TriangleListOffset];
							n            	= VertLinkIndex->NumVertTriangles;

							for (k=0; k<n; k++)
							{
								FMeshTriangle &OtherTriangle = MeshMapInfo.Triangles[*VertLink++];

								Side1      = Verts[OtherTriangle.iVertex[1]] - Verts[OtherTriangle.iVertex[0]];
								Side2      = Verts[OtherTriangle.iVertex[2]] - Verts[OtherTriangle.iVertex[0]];

								VertexNormal += (Side2 ^ Side1).Normal();
							}
							VertexNormal.Normalize();

							// Compute effect of each lightsource on this vertex.
							FTexLattice Lattice;
							if( DrawLit )
							{
								// Temporarily disabled:
								//GRender.LightList.CalcRectSample(1,0,*P,VertexNormal,Lattice,(Camera->RendMap==REN_DynLight)?2:0);				
								Lattice.FloatG = 0.5 - Min(0.5,0.00035*P->Z);
							}
							else
							{
								Lattice.FloatG = 0.5 - Min(0.5,0.00035*P->Z);
							}
							if( Highlight ) Lattice.FloatG = 0.6 + (0.4 * Lattice.FloatG);
							VertexShade[iVertex] = Min(256*61,256.0 + 
								60.0 * 256.0 * Lattice.FloatG + (Sprite->Actor->InherentGlow<<6));
						}
					}
					TriPtr++;
				}

				// Clip the triangles.
				TriPtr   = &TriPool[0];
				TotalPts = 0;

				for( i=0; i<VisibleTriangles; i++ )
				{
					FMeshTriangle &Triangle = MeshMapInfo.Triangles[TriPtr->Index];

					P = &Pts [0];
					for (j=0; j<3; j++)
					{
						*(FTransform *)P = Verts[Triangle.iVertex[j]];

						P->U = Triangle.Tex[j].U * 65536.0;
						P->V = Triangle.Tex[j].V * 65536.0;
						P->G = VertexShade [Triangle.iVertex[j]];

						P++;
					}
					NumPts = GRender.ClipTexPoints(&Camera,Pts,&PtsPool[TotalPts],3);

					TriPtr->FirstPoint   = &PtsPool [TotalPts];
					TriPtr->NumPts       = NumPts;

					TotalPts             += NumPts;
					TriPtr				 ++;
				}
				GMem.Get(TotalPts * sizeof(FTransTex)); // Reserve memory for PtsPool.

				// Draw all visible triangles.
				TriPtr = &TriPool[0];
				for (i=0; i<VisibleTriangles; i++)
				{
					FMeshTriangle &Triangle = MeshMapInfo.Triangles[TriPtr->Index];
					NumPts   = TriPtr->NumPts;

					if (NumPts>0)
					{
						P = TriPtr->FirstPoint;

						RasterTexSetup.Setup	(Camera,P,NumPts,&GMem);
						RasterTexSetup.Generate	(RasterTexPoly);
						if ( Triangle.Type==MT_Masked || Triangle.Type==MT_Transparent )
						{
							RasterTexPoly->ForceForwardFace();
						}
						if ((Camera.RendMap!=REN_Polys)&&(Camera.RendMap!=REN_PolyCuts)&&
							(Camera.RendMap!=REN_Zones)&&(Triangle.Type!=MT_Flat)&&(Camera.RendMap!=REN_Wire))
						{
							FPolySpanTextureInfoBase *Info
							= GSpanTextureMapper->SetupForPoly
							(
								Camera,
								MeshMapInfo.Textures[Triangle.TextureNum],
								Sprite->ZoneDescriptor,
								ExtraFlags | ((Triangle.Type==MT_Masked) ? PF_Masked : 0),
								PF_Masked
							);
							RasterTexPoly->Draw(Camera,*Info,SpanBuffer);
							GSpanTextureMapper->FinishPoly(Info);
						}
						else if (Triangle.Type==MT_Flat)
						{
							RasterTexPoly->DrawFlat
							(
								Camera,
								BlackColor,
								SpanBuffer
							);
						}
						else 
						{
							RasterTexPoly->DrawFlat
							(
								Camera,
								COLOR(P_BROWN,4+(TriPtr->Index)&15),
								SpanBuffer
							);
						}
						RasterTexSetup.Release();
					}
					TriPtr++;
				}
			}
		}
	}
	GMem.Release(Verts);
	STAT(GStat.MeshMapsDrawn++);
	MeshMap.Unlock	(&MeshMapInfo);

	unguardf(("(%s)",MeshMap.Name));
}

/*------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------*/
