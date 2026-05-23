/*=============================================================================
	UnSpan.h: Span buffering functions and structures

	Copyright 1995 Epic MegaGames, Inc.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNSPAN
#define _INC_UNSPAN

/*------------------------------------------------------------------------------------
	General span buffer related classes.
------------------------------------------------------------------------------------*/

//
// Screen extents of an axis-aligned bounding box.
//
class FScreenBounds
{
public:
	BOOL    Valid;
	FLOAT	MinX,MinY;
	FLOAT	MaxX,MaxY;
	FLOAT	MinZ,MaxZ;
};

//
// A span buffer linked-list entry representing a free (undrawn) 
// portion of a scanline. 
//
//warning: Mirrored in UnRender.inc.
//
class FSpan
{
public:
	// Variables.
	int Start, End;
	FSpan* Next;

	// Constructors.
	FSpan()
	{}
	FSpan( int InStart, int InEnd )
	:	Start		(InStart)
	,	End			(InEnd)
	{}
};

//
// A span buffer, which represents all free (undrawn) scanlines on
// the screen.
//
class FSpanBuffer
{
public:
	int			StartY;		// Starting Y value.
	int			EndY;		// Last Y value + 1.
	int			ValidLines;	// Number of lines at beginning (for screen).
	int			xChurn;		// Number of lines added since start.
	FSpan**		Index;		// Contains (EndY-StartY) units pointing to first span or NULL.
	FMemStack*	Mem;		// Memory pool everything is stored in.
	FMemMark	Mark;		// Top of memory pool marker.

	// Constructors.
	FSpanBuffer()
	{}
	FSpanBuffer( const FSpanBuffer& Source, FMemStack& InMem )
	:	StartY		(Source.StartY)
	,	EndY		(Source.EndY)
	,	ValidLines	(Source.ValidLines)
	,	xChurn		(0)
	,	Index		(new(InMem,EndY-StartY)FSpan*)
	,	Mem			(&InMem)
	,	Mark		(InMem)
	{
		for( int i=0; i<EndY-StartY; i++ )
		{
			FSpan **PrevLink = &Index[i];
			for( FSpan* Other=Source.Index[i]; Other; Other=Other->Next )
			{
				*PrevLink = new( *Mem, 1, 4 )FSpan( Other->Start, Other->End );
				PrevLink  = &(*PrevLink)->Next;
			}
			*PrevLink = NULL;
		}
	}

	// Allocation.
	void AllocIndex				(int AllocStartY, int AllocEndY, FMemStack *Mem);
	void AllocAndInitIndex		(int AllocStartY, int AllocEndY, FMemStack *Mem);
	void AllocIndexForScreen	(int SXR, int SYR, FMemStack *Mem);
	void Release				();
	void GetValidRange			(SWORD *ValidStartY,SWORD *ValidEndY);

	// Merge/copy/alter operations.
	void CopyIndexFrom			(const FSpanBuffer &Source,								  FMemStack *Mem);
	void MergeFrom				(const FSpanBuffer &Source1, const FSpanBuffer &Source2,  FMemStack *Mem);
	void MergeWith				(const FSpanBuffer &Other);

	// Grabbing and updating from rasterizations.
	int  CopyFromRange			(FSpanBuffer &ScreenSpanBuffer,int Y1, int Y2, FMemStack *Mem);
	int  CopyFromRaster			(FSpanBuffer &ScreenSpanBuffer,class FRasterPoly &Raster);
	int  CopyFromRasterUpdate	(FSpanBuffer &ScreenSpanBuffer,class FRasterPoly &Raster);

	// Rectangle/lattice operations.
	int  CalcRectFrom			(const FSpanBuffer &Source, BYTE GridXBits, BYTE GridYBits, FMemStack *Mem);
	void CalcLatticeFrom		(const FSpanBuffer &Source, FMemStack *Mem);

	// Occlusion.
	int	 BoxIsVisible           (int X1, int Y1, int X2, int Y2);
	int	 BoundIsVisible			(FScreenBounds &Bound);

	// Debugging.
	void AssertEmpty			(char *Name); // Assert it contains no active spans.
	void AssertNotEmpty			(char *Name); // Assert it contains active spans.
	void AssertValid			(char *Name); // Assert its data is ok.
	void AssertGoodEnough		(char *Name); // Check everything but ValidLines.
	void DebugDraw				(UCamera *Camera,BYTE Color);
};

/*------------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------------*/
#endif // _INC_UNSPAN
