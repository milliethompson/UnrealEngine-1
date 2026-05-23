/*=============================================================================
	UnSprite.cpp: Unreal sprite rendering functions.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"
#include "UnRaster.h"
#include "UnRenDev.h"

// Parameters.
#define SPRITE_PROJECTION_FORWARD	32.00 /* Move sprite projection planes forward */
#define ADD_START 100000.0
#define ADD_END   0.0

// Temporary globals.
FSprite *GFirstSprite;
INT GNumSprites;

/*------------------------------------------------------------------------------
	Dynamic node contents.
------------------------------------------------------------------------------*/

//
// Init buffer holding all dynamic contents.
//
void dynamicsLock( UModel *Model )
{
	guard(dynamicsLock);
	checkState(!GRender.DynamicsLocked);

	GRender.NumDynamics 	= 0;
	GRender.NumPostDynamics	= 0;
	GRender.DynamicsLocked	= 1;
	GFirstSprite            = NULL;
	GNumSprites             = 0;

	unguard;
}

//
// Cleanup buffer holding all dynamic contents (call after rendering)
//
void dynamicsUnlock( UModel *Model )
{
	guard(dynamicsUnlock)
	checkState(GRender.DynamicsLocked);

	FDynamicsIndex	*DynamicsIndex;
	FBspNode		*Node;

	DynamicsIndex = GRender.DynamicsIndex;
	for( INDEX i=0; i<GRender.NumDynamics; i++ )
	{
		Node 				= &Model->Nodes(DynamicsIndex->iNode);
		Node->iDynamic[0]	= INDEX_NONE;
		Node->iDynamic[1]	= INDEX_NONE;
		DynamicsIndex++;
	}
	GRender.NumDynamics 	= 0;
	GRender.NumPostDynamics = 0;

	GRender.DynamicsLocked  = 0;
	unguard;
}

//
// Add an empty entry to dynamic contents and link it up.
// Returns NULL if the dynamics structure is full.
// In this case, the rendering engine must recover gracefully.
//
inline FDynamicsIndex* dynamicsAdd
(
	UModel			*Model,
	INDEX			iNode,
	INT				Type,
	FSprite			*Sprite,
	FRasterPoly		*Raster,
	FLOAT			Z,
	INT				IsBack
)
{
	guard(dynamicsAdd);
	
	FBspNode		*Node = &Model->Nodes(iNode);
	FDynamicsIndex	*Index, *TempIndex,*PrevIndex;
	INT 			iDynamic;

	if( GRender.NumDynamics>=GRender.MaxDynamics )
		return NULL;

	iDynamic	= GRender.NumDynamics++;
	Index    	= &GRender.DynamicsIndex[iDynamic];

	// Link into dynamics chain.
	if( Z==ADD_START || Node->iDynamic[IsBack]==INDEX_NONE )
	{
		// Add at start of list.
		Index->iNext			= Node->iDynamic[IsBack];
		Node->iDynamic[IsBack]	= iDynamic;
	}
	else if( Z == ADD_END )
	{
		// Add at end of list.
		TempIndex = &GRender.DynamicsIndex[Node->iDynamic[IsBack]];
		while( TempIndex->iNext != INDEX_NONE )
		{
			TempIndex = &GRender.DynamicsIndex [TempIndex->iNext];
		}
		Index->iNext			= INDEX_NONE;
		TempIndex->iNext		= iDynamic;
	}
	else
	{
		// Z sort them in.  Things that must remain on top, such as sprites (not chunks) are
		// assigned a Z value of zero.
		TempIndex = &GRender.DynamicsIndex[Node->iDynamic[IsBack]];
		PrevIndex = NULL;

		while( TempIndex->iNext!=INDEX_NONE && TempIndex->Z<=Z )
		{
			PrevIndex = TempIndex;
			TempIndex = &GRender.DynamicsIndex[TempIndex->iNext];
		}
		if( PrevIndex == NULL )
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
	}
#endif
	return Index;
	unguard;
}

//
// Add all actors to dynamic contents, optionally excluding the player actor.
// Pass INDEX_NONE to exclude nothing.
//
void dynamicsSetup( UCamera *Camera, AActor *Exclude )
{
	guard(dynamicsSetup);
	ULevel::Ptr 	Actors  = Camera->Level;
	FSprite			*Sprite;

	// Skip if we shouldn't be rendering actors.
	if( !(Camera->Actor->ShowFlags & SHOW_Actors) || !Camera->Level->Model->Nodes->Num )
		return;

	// Traverse entire actor list.
	for( INDEX iActor=0; iActor<Actors->Num; iActor++ )
	{
		// Add this actor to dynamics if it's renderable.
		AActor *Actor = Actors(iActor);
		if 
		(	(Actor)
		&&	(Actor != Exclude)
		&&	((Camera->Level->GetState()==LEVEL_UpPlay)?!Actor->bHidden:!Actor->bHiddenEd)
		&&	(!Actor->bOnlyOwnerSee || Actor->IsOwnedBy(Camera->Actor)) )
		{
			// Allocate a sprite.
			Sprite  		= new(GDynMem)FSprite;
			Sprite->Actor	= Actor;

			// Add the sprite.
			dynamicsAdd( Camera->Level->Model, 0, DY_SPRITE, Sprite, NULL, ADD_END, 0 );
		}
	}
	unguard;
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
	UModel			*Model, 
	UCamera			*Camera, 
	INDEX			iNode,
	FDynamicsIndex	*Index
)
{
	guard(MakeSpriteChunk);
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

	FLOAT PlaneX1   = PlaneZRD * (FloatX1 - Camera->FX2);
	FLOAT PlaneX2   = PlaneZRD * (FloatX2 - Camera->FX2);
	FLOAT PlaneY1   = PlaneZRD * (FloatY1 - Camera->FY2);
	FLOAT PlaneY2   = PlaneZRD * (FloatY2 - Camera->FY2);

	// Generate four screen-aligned box vertices.
	Verts[0].X = PlaneX1; Verts[0].Y = PlaneY1; Verts[0].Z = PlaneZ; Verts[0].ScreenX = FloatX1; Verts[0].ScreenY = FloatY1;
	Verts[1].X = PlaneX2; Verts[1].Y = PlaneY1; Verts[1].Z = PlaneZ; Verts[1].ScreenX = FloatX2; Verts[1].ScreenY = FloatY1;
	Verts[2].X = PlaneX2; Verts[2].Y = PlaneY2; Verts[2].Z = PlaneZ; Verts[2].ScreenX = FloatX2; Verts[2].ScreenY = FloatY2;
	Verts[3].X = PlaneX1; Verts[3].Y = PlaneY2; Verts[3].Z = PlaneZ; Verts[3].ScreenX = FloatX1; Verts[3].ScreenY = FloatY2;

	// Transform box into worldspace.
	for( INT i=0; i<4; i++ )
		*(FVector*)&Verts[i] = Verts[i].TransformPointBy(Camera->Uncoords);

	// Generate a full rasterization for this box, which we'll filter down the Bsp.
	FRasterPoly *Raster;
	Raster			= (FRasterPoly *)new(GDynMem,sizeof(FRasterPoly) + (Sprite->Y2 - Sprite->Y1)*sizeof(FRasterLine))BYTE;
	Raster->StartY	= Sprite->Y1;
	Raster->EndY	= Sprite->Y2;

	FRasterLine	*Line = &Raster->Lines[0];
	for( i=Raster->StartY; i<Raster->EndY; i++ )
	{
		Line->Start.X = Sprite->X1;
		Line->End.X   = Sprite->X2;
		Line++;
	}

	// Add first sprite chunk at end of dynamics list so it will be processed in
	// the current node's linked rendering list (which we can assume is being
	// walked when this is called).
	dynamicsAdd( Model, iNode, DY_CHUNK, Sprite, Raster, ADD_END, 0 );

	unguard;
}

//
// Filter a chunk from the current Bsp node down to its children.  If this is a leaf,
// leave the chunk here.
//
void inline FilterChunk( UModel *Model, UCamera *Camera, INDEX iNode, FDynamicsIndex *Index, INT Outside )
{
	FSprite			*Sprite 	= Index->Sprite;
	FRasterPoly		*Raster 	= Index->Raster;
	FTransform		*Verts  	= Sprite->Verts;
	FBspNode		&Node		= Model->Nodes  (iNode);
	FBspSurf		&Surf		= Model->Surfs  (Node.iSurf);
	FVector			&Normal		= Model->Vectors(Surf.vNormal);
	FVector			&Base		= Model->Points (Surf.pBase);

	// Setup.
	FRasterPoly *FrontRaster, *BackRaster;

	// Find point-to-plane distances for all four vertices (side-of-plane classifications).
	INT Front=0, Back=0;
	FLOAT Dist[4];
	{
		for ( INT i=0; i<4; i++ )
		{
			Dist[i] = FPointPlaneDist (Verts[i],Base,Normal);
			Front  += Dist[i] > +0.01;
			Back   += Dist[i] < -0.01;
		}
		// Note: Both Front and Back can be zero
	}
	if( Front && Back )
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

		for( INT i=0; i<4; i++ )
		{
			if( (*D1)*(*D2) < 0.0 )
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
		if( NumInt < 2 )
			goto NoSplit;

		// Allocate front and back rasters.
		INT	Size	= sizeof (FRasterPoly) + (Raster->EndY - Raster->StartY) * sizeof (FRasterLine);
		FrontRaster	= (FRasterPoly *)new(GDynMem,Size)BYTE;
		BackRaster	= (FRasterPoly *)new(GDynMem,Size)BYTE;

		// Make sure that first intersection point is on top.
		if( Intersect[0].ScreenY > Intersect[1].ScreenY )
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
		if( Y0 > Raster->StartY )
		{
			if (Dist[0] >= 0) TopRaster = FrontRaster;
			else              TopRaster = BackRaster;
		}

		// Find BottomRaster.
		FRasterPoly *BottomRaster = NULL;
		if( Y1 < Raster->EndY )
		{
			if (Dist[2] >= 0) BottomRaster = FrontRaster;
			else              BottomRaster = BackRaster;
		}

		// Find LeftRaster and RightRaster.
		FRasterPoly *LeftRaster, *RightRaster;
		if( Intersect[1].ScreenX >= Intersect[0].ScreenX )
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
		if( TopRaster )
		{
			TopRaster->StartY = Raster->StartY;

			FRasterLine	*SourceLine	= &Raster->Lines    [0];
			FRasterLine	*Line		= &TopRaster->Lines [0];

			for (i=TopRaster->StartY; i<ClippedY0; i++)
				*Line++ = *SourceLine++;
		}

		// Copy BottomRaster section.
		if( BottomRaster )
		{
			BottomRaster->EndY = Raster->EndY;

			FRasterLine	*SourceLine	= &Raster->Lines       [ClippedY1 - Raster->StartY];
			FRasterLine	*Line       = &BottomRaster->Lines [ClippedY1 - BottomRaster->StartY];

			for (i=ClippedY1; i<BottomRaster->EndY; i++)
				*Line++ = *SourceLine++;
		}

		// Split middle raster section.
		if( Y1 != Y0 )
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
		if( BackRaster ->EndY <= BackRaster ->StartY )
			BackRaster  = NULL;
		if( FrontRaster->EndY <= FrontRaster->StartY )
			FrontRaster = NULL;
	}
	else
	{
		// Don't have to split the rasterization.
		NoSplit:
		FrontRaster = BackRaster = Raster;
	}

	// Filter it down.
	INT CSG = Node.IsCsg();
	if( Front && FrontRaster )
	{
		// Only add if not solid space.
		if     ( Node.iFront != INDEX_NONE ) dynamicsAdd( Model, Node.iFront, DY_CHUNK, Sprite, FrontRaster, ADD_START, 0 );
		else if( Outside || CSG            ) dynamicsAdd( Model, iNode, DY_FINALCHUNK, Sprite, FrontRaster, Sprite->Z, 0 );
	}
	if( Back && BackRaster )
	{
		// Only add if not solid space.
		if     ( Node.iBack != INDEX_NONE  ) dynamicsAdd( Model, Node.iBack, DY_CHUNK, Sprite, BackRaster, ADD_START, 0 );
		else if( Outside && !CSG           ) dynamicsAdd( Model, iNode, DY_FINALCHUNK, Sprite, BackRaster, Sprite->Z, 1 );
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
void dynamicsFilter( UCamera *Camera, INDEX iNode,INT FilterDown,INT Outside )
{
	guard(dynamicsFilter);
	UModel		*Model		= Camera->Level->Model;
	FBspNode	*Node		= &Model->Nodes(iNode);
	INDEX		iDynamic	= Node->iDynamic[0];

	while( iDynamic != INDEX_NONE )
	{
		FDynamicsIndex *Index = &GRender.DynamicsIndex[iDynamic];
		switch( Index->Type )
		{
			case DY_SPRITE:
			{
				FSprite *Sprite = Index->Sprite;
				if( GRender.SetupSprite( Camera, Sprite ) )
				{
					// Note: May set Index->iNext.
					MakeSpriteChunk( Model, Camera, iNode, Index );
				}
				break;
			}
			case DY_CHUNK:
			{
				if( FilterDown ) FilterChunk( Model, Camera, iNode, Index, Outside );
				else             Index->Type = DY_FINALCHUNK;
				break;
			}
			case DY_FINALCHUNK:
			{
				break;
			}
		}
		iDynamic = Index->iNext;
	}
	unguard;
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
	UCamera		*Camera, 
	FSpanBuffer *SpanBuffer,
	INDEX		iNode, 
	INT			IsBack
)
{
	guard(dynamicsPreRender);
	UModel 			*Model		= Camera->Level->Model;
	FBspNode		*Node 		= &Model->Nodes(iNode);
	INDEX			iDynamic	= Node->iDynamic[IsBack];

	if( iDynamic != INDEX_NONE )
	{
		if (GRender.NumPostDynamics < GRender.MaxPostDynamics)
			GRender.PostDynamics[GRender.NumPostDynamics++] = iNode;
	}
	while( iDynamic != INDEX_NONE )
	{
		FDynamicsIndex	*Index	= &GRender.DynamicsIndex [iDynamic];
		FSprite			*Sprite = Index->Sprite;

		switch (Index->Type)
		{
			case DY_FINALCHUNK:
				if( !Index->Sprite->SpanBuffer )
				{
					// Creating a new span buffer for this sprite.
					Sprite->SpanBuffer = new(GDynMem)FSpanBuffer;
					Sprite->SpanBuffer->AllocIndex(Index->Raster->StartY,Index->Raster->EndY,&GDynMem);

					if( Sprite->SpanBuffer->CopyFromRaster(*SpanBuffer,*Index->Raster) )
					{
						// Span buffer is non-empty, so keep it and put it on the to-draw list.
						STAT(GStat.ChunksDrawn++);
						Sprite->Next	= GFirstSprite;
						GFirstSprite	= Sprite;
						GNumSprites++;
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
					FMemMark Mark(GMem);
					FSpanBuffer *Span = new(GMem)FSpanBuffer;
					Span->AllocIndex(Index->Raster->StartY,Index->Raster->EndY,&GMem);

					if (Span->CopyFromRaster(*SpanBuffer,*Index->Raster))
					{
						// Temporary span buffer is non-empty, so merge it into sprite's.
						Sprite->SpanBuffer->MergeWith(*Span);
						STAT(GStat.ChunksDrawn++);
					}

					// Release the temporary memory.
					Mark.Pop();
				}
				break;
			case DY_CHUNK:
				break;
			case DY_SPRITE:
				break;
		}
		iDynamic = Index->iNext;
	}
	unguard;
}

/*------------------------------------------------------------------------------
	Dynamics postrendering.
------------------------------------------------------------------------------*/

void dynamicsFinalize( UCamera *Camera, INT SpanRender )
{
	guard(dynamicsFinalize);
	if( Camera->Level->Model->Nodes->Num == 0 )
	{
		// Can't manage dynamic contents if no nodes.
		GRender.DrawLevelActors( Camera, NULL );
	}
	else
	{
		// Draw all presaved sprites.
		for( FSprite *Sprite = GFirstSprite; Sprite; Sprite=Sprite->Next )
			GRender.DrawActorChunk( Camera, Sprite );
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Scaled sprites.
-----------------------------------------------------------------------------*/

//
// Compute the extent (X1,Y1)-(X2,Y2) of a scaled sprite.
//
void inline CalcScaledSpriteExtent
(
	UCamera	*Camera,
	FLOAT	ScreenX,
	FLOAT	ScreenY,
	FLOAT	XSize,
	FLOAT	YSize,
	INT		*X1,
	INT		*Y1,
	INT		*X2,
	INT		*Y2
) 
{
	checkInput(XSize>=0.0);
	checkInput(YSize>=0.0);

	ScreenX -= XSize * 0.5;
	ScreenY -= YSize * 0.5;

	// Find correctly rounded X and Y.
	*X1 = ftoi(ceil(ScreenX));
	*X2 = ftoi(ceil(ScreenX+XSize));
	if( *X1 < 0 )
	{
		*X1 = 0;
		if( *X2 < 0 )
			*X2 = 0;
	}
	if( *X2 > Camera->X )
	{
		*X2 = Camera->X;
		if( *X1 > Camera->X )
			*X1 = Camera->X;
	}

	*Y1 = ftoi(ceil(ScreenY));
	*Y2 = ftoi(ceil(ScreenY+YSize));
	if( *Y1 < 0 )
	{
		*Y1 = 0;
		if( *Y2 < 0 )
			*Y2 = 0;
	}
	if( *Y2 > Camera->Y )
	{
		*Y2 = Camera->Y;
		if( *Y1 > Camera->Y )
			*Y1 = Camera->Y;
	}
}

//
// Draw a scaled sprite.  Takes care of clipping.
// XSize and YSize are in pixels.
//
void FRender::DrawScaledSprite
(
	UCamera		*Camera, 
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
	checkInput( Texture != NULL );
	FLOAT X1=0, X2=0, Y1=0, Y2=0;
	guard( FRender::DrawScaledSprite );

	if (Center)
	{
		ScreenX -= XSize * 0.5;
		ScreenY -= YSize * 0.5;
	}
	X1 = ScreenX; X2 = ScreenX + XSize;
	Y1 = ScreenY; Y2 = ScreenY + YSize;	

	// Clip.
	if ( X1 < 0.0 ) 		X1 = 0.0;
	if ( X2 > Camera->FX ) 	X2 = Camera->FX;
	if ( X2 <= X1 )			return;

	if (SpanBuffer)
	{
		if ( Y1 < SpanBuffer->StartY )	Y1 = SpanBuffer->StartY;
		if ( Y2 > SpanBuffer->EndY   )	Y2 = SpanBuffer->EndY;
	}
	else
	{
		if ( Y1 < 0.0 ) 			Y1 = 0.0;
		if ( Y2 > Camera->FY ) 		Y2 = Camera->FY;
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

	// Build poly.
	FTransTexture P[4];
	P[0].ScreenX = X1; P[0].ScreenY = Y2; P[0].U = U1; P[0].V = V2; P[0].Color = FVector(G,G,G); P[0].Z=1000.0;
	P[1].ScreenX = X1; P[1].ScreenY = Y1; P[1].U = U1; P[1].V = V1; P[1].Color = FVector(G,G,G); P[1].Z=1000.0;
	P[2].ScreenX = X2; P[2].ScreenY = Y1; P[2].U = U2; P[2].V = V1; P[2].Color = FVector(G,G,G); P[2].Z=1000.0;
	P[3].ScreenX = X2; P[3].ScreenY = Y2; P[3].U = U2; P[3].V = V2; P[3].Color = FVector(G,G,G); P[3].Z=1000.0;

	// Set flags.
	DWORD TheseFlags=0;
	switch( BlitType )
	{
		case BT_None:			TheseFlags = PF_Masked | PF_NoSmooth;		break;
		case BT_Normal:			TheseFlags = PF_Masked | 0;					break;
		case BT_Transparent:	TheseFlags = PF_Masked | PF_Transparent;	break;
	}

	// Draw it.
	if( GCameraManager->RenDev )
	{
		// Hardware render it.
		GCameraManager->RenDev->DrawPolyC( Camera, Texture, P, 4, TheseFlags );
	}
	else
	{
		//!! This makes a VC++ 5.0 code generator bug go away.
		__asm{}

		// Software render it.
		FMemMark Mark(GMem);
		FRasterTexPoly *RasterTexPoly = (FRasterTexPoly	*)new(GMem,
			sizeof(FRasterTexPoly)+FGlobalRaster::MAX_RASTER_LINES*sizeof(FRasterTexLine))BYTE;
		FRasterTexSetup RasterTexSetup;
		RasterTexSetup.Setup( Camera, P, 4, &GMem );
		RasterTexSetup.Generate( RasterTexPoly );
		FPolySpanTextureInfoBase *Info = GSpanTextureMapper->SetupForPoly( Camera, Texture, NULL, TheseFlags, 0 );
		RasterTexPoly->Draw( Camera, *Info, SpanBuffer );
		GSpanTextureMapper->FinishPoly(Info);
		Mark.Pop();
	}
	unguardf(("(Y1=%f Y2=%f)",Y1,Y2));
}

/*-----------------------------------------------------------------------------
	Texture block drawing.
-----------------------------------------------------------------------------*/

//
// Draw a block of texture on the screen.
//
void FRender::DrawTiledTextureBlock
(
	UCamera		*Camera,
	UTexture	*Texture,
	INT			X, 
	INT			XL, 
	INT			Y, 
	INT			YL,
	INT			U,
	INT			V
)
{
	guard(FRender::DrawTiledTextureBlock);

	if( GCameraManager->RenDev )
	{
		// Build poly.
		FTransTexture P[4];
		FLOAT G = 63*256;
		P[0].ScreenX = X;    P[0].ScreenY = Y+YL; P[0].U = (U   )*65536.0; P[0].V = (V+YL)*65536.0; P[0].Color = FVector(G,G,G); P[0].Z=1000.0;
		P[1].ScreenX = X;    P[1].ScreenY = Y;    P[1].U = (U   )*65536.0; P[1].V = (V   )*65536.0; P[1].Color = FVector(G,G,G); P[1].Z=1000.0;
		P[2].ScreenX = X+XL; P[2].ScreenY = Y;    P[2].U = (U+XL)*65536.0; P[2].V = (V   )*65536.0; P[2].Color = FVector(G,G,G); P[2].Z=1000.0;
		P[3].ScreenX = X+XL; P[3].ScreenY = Y+YL; P[3].U = (U+XL)*65536.0; P[3].V = (V+YL)*65536.0; P[3].Color = FVector(G,G,G); P[3].Z=1000.0;

		// Draw it.
		GCameraManager->RenDev->DrawPolyC( Camera, Texture, P, 4, 0 );
	}
	else
	{
		// Software render it.
		FTextureInfo TextureInfo;
		Texture->Lock(TextureInfo,Camera->Texture,TL_RenderPalette);
		RAINBOW_PTR Colors = TextureInfo.Colors;

		// Setup.
		BYTE *Data		= TextureInfo.Mips[0]->Data;
		INT UAnd		= TextureInfo.Mips[0]->USize-1;
		INT VAnd		= TextureInfo.Mips[0]->VSize-1;
		BYTE VShift		= Texture->UBits;
		BYTE *Dest1		= &Camera->Screen[(X + Y*Camera->Stride)*Camera->ColorBytes];
		INT XE			= X+XL+U;
		INT YE			= Y+YL+V;

		X += U; 
		Y += V;

		if( Camera->ColorBytes==1 )
		{
			for (V=Y; V<YE; V++)
			{
				BYTE *Src  = &Data[(V & VAnd) << VShift];
				BYTE *Dest = Dest1;
				for (INT U=X; U<XE; U++) *Dest++ = Colors.PtrBYTE[Src[U&UAnd]];
				Dest1 += Camera->Stride;
			}
		}
		else if( Camera->ColorBytes==2 )
		{
			for (V=Y; V<YE; V++)
			{
				BYTE *Src  = &Data[(V & VAnd) << VShift];
				WORD *Dest = (WORD *)Dest1;
				for (INT U=X; U<XE; U++) *Dest++ = Colors.PtrWORD[Src[U&UAnd]];
				Dest1 += Camera->Stride*2;
			}
		}
		else if( Camera->ColorBytes==4 )
		{
			for( V=Y; V<YE; V++ )
			{
				BYTE  *Src  = &Data[(V & VAnd) << VShift];
				DWORD *Dest = (DWORD *)Dest1;
				for (U=X; U<XE; U++) *Dest++ = Colors.PtrDWORD[Src[U&UAnd]];
				Dest1 += Camera->Stride*4;
			}
		}
		else appErrorf("Invalid color depth %i",Camera->ColorBytes);

		// Unlock the texture.
		Texture->Unlock(TextureInfo);
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Actor drawing (sprites and chunks).
-----------------------------------------------------------------------------*/

//
// Draw an actor with no span clipping.
//
void FRender::DrawActorSprite( UCamera *Camera,FSprite *Sprite )
{
	guard(FRender::DrawActorSprite);
	
	AActor *Actor = Sprite->Actor;
	INT ModeClass = GEditor ? GEditor->edcamModeClass(GEditor->Mode) : EMC_None;

	if( GEditor && GEditor->Scan.Active )
		GEditor->Scan.PreScan();

	// Draw the actor.
	if
	(	(Actor->DrawType==DT_Sprite) 
	||	(Actor->DrawType==DT_Brush) 
	||	(Camera->Actor->ShowFlags & SHOW_ActorIcons) )
	{
		if( Actor->Texture ) DrawScaledSprite
		(
			Camera,Actor->Texture,
			Sprite->ScreenX,
			Sprite->ScreenY,
			Actor->DrawScale * Actor->Texture->USize,
			Actor->DrawScale * Actor->Texture->VSize,
			Actor->BlitType,
			0,
			1,
			Actor->bSelected && Camera->Level->GetState()!=LEVEL_UpPlay
		);
	}
	else if( Actor->DrawType==DT_Mesh )
	{
		if( Actor->Mesh ) DrawMesh ( Camera, Actor, NULL, Sprite );
	}

	// Draw frills.
	if( Camera->IsOrtho() )
	{
		// Radii.
		if( Camera->Actor->ShowFlags & SHOW_ActorRadii )
		{
			if( !Actor->Brush || !Actor->Brush->bCheckCollision )
			{
				// Show collision radius
				if( Actor->bCollideActors && Camera->Actor->RendMap==REN_OrthXY )
					DrawCircle( Camera, Actor->Location, Actor->CollisionRadius, ActorArrowColor, 1 );

				// Show collision height.
				FVector Ext(Actor->CollisionRadius,Actor->CollisionRadius,Actor->CollisionHeight);
				FVector Min(Actor->Location - Ext);
				FVector Max(Actor->Location + Ext);
				if( Actor->bCollideActors && Camera->Actor->RendMap!=REN_OrthXY )
					DrawBox( Camera, Min, Max, ActorArrowColor, 1 );
			}
			else
			{
				FVector Min,Max;
				Actor->GetCollisionExtent( Min, Max );
				DrawBox( Camera, Min, Max, ActorArrowColor, 1 );
			}

			// Show light radius.
			if( (Actor->LightType!=LT_None) && Actor->bSelected  && Camera->Level->GetState()!=LEVEL_UpPlay )
				DrawCircle( Camera, Actor->Location, Actor->WorldLightRadius(), ActorArrowColor, 1 );
		}

		// Direction arrow.
		if
		(	(Actor->bDirectional)
		&&	(ModeClass==EMC_Actor || Actor->IsA("View")))
		{
			GGfx.ArrowBrush->Location = Actor->Location;
			GGfx.ArrowBrush->Rotation = Actor->Rotation;
			GGfx.ArrowBrush->TransformedBound.Init(); // Don't bound-reject.
			GRender.DrawBrushPolys(Camera,GGfx.ArrowBrush,ActorArrowColor,0,NULL,1,0,Actor->bSelected,0);
		}
	}

	if( GEditor && GEditor->Scan.Active )
		GEditor->Scan.PostScan( EDSCAN_Actor, (INDEX)Sprite->Actor, 0, 0, NULL );

	unguard;
}

//
// Draw an actor clipped to a span buffer representing the portion of
// the actor that falls into a particular Bsp leaf.
//
void FRender::DrawActorChunk( UCamera *Camera,FSprite *Sprite )
{
	guard(FRender::DrawActorChunk);
	if (GEditor && GEditor->Scan.Active)
		GEditor->Scan.PreScan();

	AActor *Actor = Sprite->Actor;
	if
	(
			(Actor->DrawType==DT_Sprite) 
		||	(Actor->DrawType==DT_Brush) 
		||	(Camera->Actor->ShowFlags & SHOW_ActorIcons)
	)
	{
		if (Actor->Texture) DrawScaledSprite
		(
			Camera,Actor->Texture,
			Sprite->ScreenX,
			Sprite->ScreenY,
			Actor->DrawScale * Actor->Texture->USize,
			Actor->DrawScale * Actor->Texture->VSize,
			Actor->BlitType,
			Sprite->SpanBuffer,
			1,
			Actor->bSelected && Camera->Level->GetState()!=LEVEL_UpPlay
		);
	}
	else if( Actor->DrawType==DT_Mesh && Actor->BlitType!=BT_None && Actor->Mesh!=NULL )
	{
		DrawMesh( Camera, Actor, Sprite->SpanBuffer, Sprite );
	}
	if( GEditor && GEditor->Scan.Active )
		GEditor->Scan.PostScan( EDSCAN_Actor, (INDEX)Sprite->Actor, 0, 0, NULL );

	unguard;
}

/*-----------------------------------------------------------------------------
	Sprite information.
-----------------------------------------------------------------------------*/

//
// Set the Sprite structure corresponding to an actor;  returns 1 if the actor
// is visible, or 0 if the actor is occluded.  If occluded, the Sprite isn't
// visible and should be ignored.
//
INT FRender::SetupSprite( UCamera *Camera, FSprite *Sprite )
{
	guard(FRender::SetupSprite);

	// Setup.
	AActor	*Actor		= Sprite->Actor;
	UClass	*Class		= Actor->GetClass();
	FVector Location	= Actor->Location - Camera->Coords.Origin;

	// Init sprite info.
	Sprite->Z			= Location | Camera->Coords.ZAxis;
	Sprite->SpanBuffer	= NULL;

	// Figure out what to do with this.
	if( Sprite->Z<-SPRITE_PROJECTION_FORWARD && !Camera->IsWire() )
	{
		// This is not possibly visible.
		return 0;
	}

	// Get the zone info.
	Sprite->Zone = Actor->Zone;

	// Handle the actor based on its type.
	if
	(	(Actor->DrawType==DT_Sprite) 
	||	(Actor->DrawType==DT_Brush) 
	||	(Camera->Actor->ShowFlags & SHOW_ActorIcons) )
	{
		// This is a sprite.
		if( Actor->DrawType!=DT_Brush || GEditor!=NULL )
		{
			// Make sure we have something to draw.
			if( !Actor->Texture )
				return 0;

			// See if this is occluded.
			if( !GRender.Project( Camera, &Actor->Location, &Sprite->ScreenX, &Sprite->ScreenY, &Actor->DrawScale ))  
				return 0;

			// Compute sprite extent.
			CalcScaledSpriteExtent
			(
				Camera,
				Sprite->ScreenX,Sprite->ScreenY,
				Actor->DrawScale * Actor->Texture->USize,
				Actor->DrawScale * Actor->Texture->VSize,
				&Sprite->X1,
				&Sprite->Y1,
				&Sprite->X2,
				&Sprite->Y2
			);
		}
		else
		{
			Sprite->X1	  = Sprite->ScreenX;
			Sprite->X2	  = Sprite->ScreenX;
			Sprite->Y1	  = Sprite->ScreenY;
			Sprite->Y2    = Sprite->ScreenY;
		}

		// Return whether the sprite is visible.
		debugOutput(Sprite->X1<=Sprite->X2);
		debugOutput(Sprite->Y1<=Sprite->Y2);
		return Sprite->Y1<Camera->Y && Sprite->Y2>0;
	}
	else if( Actor->DrawType==DT_Mesh && Actor->Mesh )
	{
		// Setup rendering info for the mesh.
		if( Camera->IsWire() )
		{
			// Setup wireframe rendering.
			if( !GRender.Project (Camera,&Location,&Sprite->ScreenX,&Sprite->ScreenY,NULL) )
				return 0;

			Sprite->X1	  = Sprite->ScreenX;
			Sprite->X2	  = Sprite->ScreenX;
			Sprite->Y1	  = Sprite->ScreenY;
			Sprite->Y2    = Sprite->ScreenY;
		}
		else
		{
			// Get mesh's per-frame bounding Bound.
			FBoundingRect Bounds    = Actor->Mesh->GetBoundingRect( Actor );
			FCoords       BoxCoords = GMath.UnitCoords / Actor->Rotation / Actor->Mesh->RotOrigin;
			BoxCoords.Origin        = Actor->Location;

			// Try to view pyramid and span reject it.
			FScreenBounds ScreenBounds;
			if( !GRender.BoundVisible( Camera, &Bounds, NULL, &ScreenBounds, &BoxCoords ) )
				return 0;

			// Set extent.
			if( ScreenBounds.Valid )
			{
				// Screen bounds are valid, so set the extent.
				Sprite->X1 = Max( 0.f,        ScreenBounds.MinX );
				Sprite->X2 = Min( Camera->FX, ScreenBounds.MaxX );
				Sprite->Y1 = Max( 0.f,        ScreenBounds.MinY );
				Sprite->Y2 = Min( Camera->FY, ScreenBounds.MaxY );
			}
			else
			{
				// Screen bounds aren't valid, so the extent is potentially the whole screen.
				Sprite->X1 = 0;
				Sprite->Y1 = 0;
				Sprite->X2 = Camera->X;
				Sprite->Y2 = Camera->Y;
			}
		}

		// This is potential visible.
		debugOutput(Sprite->X1<=Sprite->X2);
		debugOutput(Sprite->Y1<=Sprite->Y2);
		return 1;
	}

	// Not potentially visible.
	return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Wireframe view drawing.
-----------------------------------------------------------------------------*/

//
// Draw all moving brushes in the level as wireframes.
//
void FRender::DrawMovingBrushWires( UCamera *Camera )
{
	guard(FRender::DrawMovingBrushWires);

	for( INDEX iActor=0; iActor<Camera->Level->Num; iActor++ )
	{
		AActor *Actor = Camera->Level->Element(iActor);
		if( Actor && Actor->IsMovingBrush() )
		{
			if (GEditor && GEditor->Scan.Active)
				GEditor->Scan.PreScan();

			if (!GEditor)
				Actor->UpdateBrushPosition(Camera->Level,0);

			DrawBrushPolys(Camera,Actor->Brush,MoverColor,0,NULL,Actor->bSelected,Actor->bSelected,Actor->bSelected,0);

			if (GEditor && GEditor->Scan.Active) 
				GEditor->Scan.PostScan( EDSCAN_Actor, (INDEX)Actor, 0, 0, NULL );
		}
	}
	unguard;
}

//
// Just draw an actor, no span occlusion
//
void FRender::DrawActor( UCamera *Camera, AActor *Actor)
{
	guard(FRender::DrawActor);

	FSprite	Sprite;
	Sprite.Actor = Actor;
	if( SetupSprite( Camera, &Sprite) )
	{
		DrawActorSprite( Camera, &Sprite );
	}
	unguard;
}

//
// Draw all actors in a level without doing any occlusion checking or sorting.
// For wireframe views.
//
void FRender::DrawLevelActors( UCamera *Camera, AActor *Exclude )
{
	guard(FRender::DrawLevelActors);
	ULevel::Ptr Actors = Camera->Level;

	for( INDEX iActor=0; iActor<Actors->Num; iActor++ )
	{
		AActor *Actor = Actors(iActor);
		if
		(	(Actor)
		&&	(Actor != Exclude) 
		&&	((Camera->Level->GetState()==LEVEL_UpPlay)?!Actor->bHidden:!Actor->bHiddenEd)
		&&	(!Actor->bOnlyOwnerSee || Actor->IsOwnedBy(Camera->Actor)) )
		{
			// If this actor is an event source, draw event lines connecting it to
			// all corresponding event sinks.
			if( Actor->Event!=NAME_None && GEditor && 
				GEditor->edcamModeClass(GEditor->Mode)==EMC_Actor)
			{
				for( INDEX iOther=0; iOther<Actors->Num; iOther++ )
				{
					AActor *OtherActor = Actors(iOther);
					if
					(	(OtherActor)
					&&	(OtherActor->GetFName() == Actor->Event) 
					&&	((Camera->Level->GetState()==LEVEL_UpPlay)?!Actor->bHidden:!Actor->bHiddenEd)
					&&  (!Actor->bOnlyOwnerSee || Actor->IsOwnedBy(Camera->Actor)) )
					{
						GRender.Draw3DLine
						(
							Camera,
							Actor->Location, 
							OtherActor->Location,
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
	unguard;
}

/*------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------*/
