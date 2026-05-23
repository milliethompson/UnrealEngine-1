/*=============================================================================
    UnSpan.cpp: Unreal span buffering functions

    Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
    Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

    Revision history:
        * Created by Tim Sweeney

        * Optimized by Erik de Neve - 15 November 1996

        - Box/BoundIsVisible and CopyFromRasterUpdate no longer
          assume that the screen FSpanBuffer starts at 0.

        - Where faster, 'ValidLines' has been reduced to a boolean
          flag, reflecting wether a spanbuffer is empty or not,
          except for CopyFromRasterUpdate where the number of spans
          in the main screen span buffer needs to be maintained.

        - Optimized the logic in Box/BoundIsVisible.

        - CopyFromRasterUpdate now properly handles rasterpolys
          that don't overlap the main screen FSpanBuffer with
          their StartY / EndY.

        - Full-assembler alternatives are called for CalcRectFrom,
          CalcLatticeFrom, CopyFromRasterUpdate, and MergeWith.
          These are currently called from inside their C++ counterparts
          so all Guard/Unguard and validity checks remain effective;
          but they can be called directly, for example:

           C++:  Dest.CopyFromRasterUpdate(Screen,Raster)

           ASM:  CopyFromRasterUpdateMASM(Dest,Screen,Raster);

        - For these routines in UnSpanX.asm, 'STRUCT'ures are used to
          get the relative addresses of the data structures as defined
          in UnRaster.h, UnSpan.h and UnRender.h, so any changes should
          be mirrored in UnspanX.asm.
          The structures are: TRasterLine, FRasterPoly, FSpanBuffer,
          FMemPool, FSpan.

        - The assembler functions assume all span X and Y coordinates
          to be positive.

=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"
#include "UnRaster.h"

//
// Activation of individual external MASM alternatives from UnSpanX.asm.
//

#if ASM
    #define CFRU_ASM   // CopyFromRasterUpdate
    #define RECT_ASM   // CalcRectFrom
    #define LATT_ASM   // CalcLatticeFrom
    #define   MW_ASM   // MergeWith
#endif

//
// Macros used by various span routines
//

#define CUTSTART(x) (int)((x)>>GridXBits)
#define CUTEND(x)   (int)(1+(((x)-1)>>GridXBits))

#define UPDATE_PREVLINK(START,END)\
    {\
    *PrevLink       = TopSpan;\
    TopSpan->Start  = START;\
    TopSpan->End    = END;\
    PrevLink        = &((TopSpan++)->Next);\
    STAT(GStat.SpanTotalChurn++;)\
    ValidLines++;\
    };

#define UPDATE_PREVLINK_ALLOC(START,END)\
    {\
    NewSpan         = (FSpan *)Mem->GetFast4(sizeof(FSpan));\
    *PrevLink       = NewSpan;\
    NewSpan->Start  = START;\
    NewSpan->End    = END;\
    PrevLink        = &(NewSpan->Next);\
    STAT(GStat.SpanTotalChurn++;)\
    ValidLines++;\
    };

/*-----------------------------------------------------------------------------
    Allocation.
-----------------------------------------------------------------------------*/

//
// Allocate a linear span buffer in temporary memory.  Allocates zero bytes
// for the list; must call spanAllocLinear to allocate the proper amount of memory
// for it.
//
// Status: Very seldom called, no need to convert to ASM.
//
void FSpanBuffer::AllocIndex(int AllocStartY, int AllocEndY, FMemPool *MemPool)
{
    GUARD;

    Mem         = MemPool;
    StartY      = AllocStartY;
    EndY        = AllocEndY;
    ValidLines  = 0;

    if( StartY <= EndY )
    {
        Index  = (FSpan **)Mem->Get     ((AllocEndY-AllocStartY) * sizeof(FSpan *));
        List   = (FSpan  *)Mem->GetFast4(0);
    }
    else
    {
        Index = NULL;
        List  = NULL;
    }
    UNGUARD("FSpanBuffer::AllocIndex");
}

//
// Allocate and initialize a span buffer.
//
// Status: Very seldom called, no need to convert to ASM.
//
void FSpanBuffer::AllocAndInitIndex(int AllocStartY, int AllocEndY, FMemPool *MemPool)
{
    guard(FSpanBuffer::AllocAndInitIndex);
    AllocIndex(AllocStartY,AllocEndY,MemPool);
    mymemset(Index,0,(AllocEndY-AllocStartY) * sizeof(FSpan *));
    unguard;
}

//
// Allocate a linear span buffer and initialize it to represent
// the yet-undrawn region of a camera view.
//
// Status: Very seldom called, no need to convert to ASM.
//
void FSpanBuffer::AllocIndexForScreen(int SXR,int SYR, FMemPool *MemPool)
{
    guard(FSpanBuffer::AllocIndexForScreen);
    int  i;

    Mem     = MemPool;
    StartY  = 0;
    EndY    = ValidLines = SYR;

    Index   = (FSpan **)Mem->Get(SYR * sizeof(FSpan *));
    List    = (FSpan  *)Mem->GetFast4(SYR * sizeof(FSpan  ));

    for( i=0; i<SYR; i++ )
    {
        Index[i]        = &List[i];
        List [i].Start  = 0;            // Inclusive
        List [i].End    = SXR;          // Exclusive (half-open interval)
        List [i].Next   = NULL;
    }
    STAT(Churn = SYR);
    STAT(GStat.SpanTotalChurn += SYR;)
    unguard;
}

//
// Free a linear span buffer in temporary rendering pool memory.
// Works whether actually saved or not.
//
// Status: Very seldom called, no need to convert to ASM.
//
void FSpanBuffer::Release()
{
    guard(FSpanBuffer::Release);
    Mem->Release(Index);
    unguard;
}

//
// Compute's a span buffer's valid range StartY-EndY range.
// Sets to 0,0 if the span is entirely empty.  You can also detect
// this condition by comparing ValidLines to 0.
//
// Status: Not performance critical, no need to convert to ASM.
//
void FSpanBuffer::GetValidRange(SWORD *ValidStartY,SWORD *ValidEndY)
{
    clockSlow(GStat.GetValidRange);
    if( ValidLines )
    {
        FSpan **TempIndex;
        int NewStartY,NewEndY;

        NewStartY = StartY;
        TempIndex = &Index [0];
        while( *TempIndex==NULL )
		{
			TempIndex++;
			NewStartY++;
		}

        NewEndY   = EndY;
        TempIndex = &Index [EndY-StartY-1];
        while( *TempIndex==NULL )
		{
			TempIndex--;
			NewEndY--;
		}

        *ValidStartY = NewStartY;
        *ValidEndY   = NewEndY;
	}
    else *ValidStartY = *ValidEndY = 0;
    unclockSlow(GStat.GetValidRange);
}

/*-----------------------------------------------------------------------------
    Span occlusion.
-----------------------------------------------------------------------------*/


//
// See if a rectangle is visible.  Returns 1 if all or partially visible,
// 0 if totally occluded.
//
// Status: Performance critical.
//
int FSpanBuffer::BoxIsVisible(int X1, int Y1, int X2, int Y2)
{
    clockSlow(GStat.BoxIsVisible);
    guard(FSpanBuffer::BoxIsVisible);

    FSpan **ScreenIndex, *Span;

    if( Y1 >= EndY )
    {
        unclockSlow(GStat.BoxIsVisible);
        return 0;
    }
    if( Y2 <= StartY )
    {
        unclockSlow(GStat.BoxIsVisible);
        return 0;
    }
    if (Y1 < StartY)    Y1 = StartY;
    if (Y2 > EndY)      Y2 = EndY;

    // Check box occlusion with span buffer.
    ScreenIndex = &Index [Y1-StartY];
    int Count   = Y2-Y1;

    // Start checking last line, then first and the rest.
    Span = *(ScreenIndex + (Count-1) );
    while( --Count >= 0 )
    {
        while( (Span) && (X2 > Span->Start) )
		{
    		if( X1 < Span->End )
    		{
                unclockSlow(GStat.BoxIsVisible);
    			return 1;
    		}
    		Span = Span->Next;
        }
        Span = *ScreenIndex++;
    }
    unclockSlow(GStat.BoxIsVisible);
    return 0;
    unguard;
}

//
// See if a rectangle is visible.  Returns 1 if all or partially visible,
// 0 if totally occluded.
//
// Status: Performance critical.
//
int FSpanBuffer::BoundIsVisible(FScreenBounds &Bound)
{
    clockSlow(GStat.BoundIsVisible);
    guard(FSpanBuffer::BoundIsVisible);

    FSpan   **ScreenIndex, *Span;
    int     MinX=Bound.MinX,MaxX=Bound.MaxX,MinY=Bound.MinY,MaxY=Bound.MaxY;

    if( MinY >= EndY )
    {
        unclockSlow(GStat.BoundIsVisible);
        return 0;
    }
    if( MaxY <= StartY )
	{
        unclockSlow(GStat.BoundIsVisible);
        return 0;
    }
    if (MinY < StartY)  MinY = StartY;
    if (MaxY > EndY)    MaxY = EndY;

    // Check box occlusion with span buffer.
    ScreenIndex = &Index [MinY-StartY];
    int Count   = MaxY-MinY;

    // Start checking last line, then first and the rest.
    Span = *(ScreenIndex + (Count-1) );
    while( --Count >= 0 )
    {
        while( (Span) && (MaxX > Span->Start)  )
		{
    		if( MinX < Span->End )
    		{
                unclockSlow(GStat.BoundIsVisible);
    			return 1;
    		}
    		Span = Span->Next;
        }
        Span = *ScreenIndex++;
    }
    unclockSlow(GStat.BoundIsVisible);
    return 0;
    unguard;
}


/*-----------------------------------------------------------------------------
    Span grabbing and updating.
-----------------------------------------------------------------------------*/

//
// Grind this polygon through the span buffer and:
// - See if the poly is totally occluded.
// - Update the span buffer by adding this poly to it.
// - Build a new, temporary span buffer for raster and span clipping the poly.
//
// Returns 1 if poly is all or partially visible, 0 if completely obscured.
// If 0 was returned, no screen span buffer memory was allocated and the resulting
// span index can be safely freed.
//
// Requires that StartY <= Raster.StartY, EndY >= Raster.EndY;
//
// If the destination FSpanBuffer and the screen's FSpanBuffer are using the same memory
// pool, the newly-allocated screen spans will be intermixed with the destination
// screen spans.  Freeing the destination in this case will overwrite the screen span buffer
// with garbage.
//
// Status: Extremely performance critical.
//
#ifdef  CFRU_ASM
extern "C" int _cdecl CopyFromRasterUpdateMASM (FSpanBuffer *AsmThis,
                                                FSpanBuffer &AsmScreen,
                                          const FRasterPoly &AsmRaster);
#endif


int FSpanBuffer::CopyFromRasterUpdate (FSpanBuffer &Screen, FRasterPoly &Raster)
    {
    clockSlow(GStat.CopyFromRasterUpdate);
    GUARD;

    #ifndef CFRU_ASM

    FRasterLine *Line;
    FSpan       **ScreenIndex,*NewScreenSpan,*NewSpan,*ScreenSpan,**PrevScreenLink;
    FSpan       **TempIndex,**PrevLink;
	int			i,OurStart,OurEnd,Accept=0;

    #endif

    if ((StartY>Raster.StartY)||(EndY<Raster.EndY))
        {
        appErrorf("Illegal span range <%i,%i> <%i,%i>",StartY,EndY,Raster.StartY,Raster.EndY);
        };


    #ifdef CFRU_ASM
    int Accept = CopyFromRasterUpdateMASM (this, Screen, Raster);
    #else

    OurStart = Max(Raster.StartY,Screen.StartY);
    OurEnd   = Min(Raster.EndY,Screen.EndY);

 	TempIndex = &Index [0];

    // Extra check for OurStart>OurEnd = screen and rasterpoly don't
    // overlap, so all-null output.
    if (OurStart>=OurEnd)
        {
    	for (i=StartY; i<EndY; i++) *(TempIndex++) = NULL;
        return 0;
        }

    for (i=StartY; i<OurStart; i++) *(TempIndex++) = NULL;
    //
    Line        = &Raster.Lines [OurStart-Raster.StartY];
    ScreenIndex = &Screen.Index [OurStart-Screen.StartY];

	for (i=OurStart; i<OurEnd; i++)
        {
        PrevScreenLink  = ScreenIndex;
        ScreenSpan      = *(ScreenIndex++);
        PrevLink        = TempIndex++;
        //
        // Skip if this screen span is already full, or if the raster is empty:
        //
        if ((!ScreenSpan) || (Line->End.X <= Line->Start.X)) goto NextLine;
        //
        // Skip past all spans that occur before the raster:
        //
        while (ScreenSpan->End <= Line->Start.X)
            {
            PrevScreenLink  = &(ScreenSpan->Next);
            ScreenSpan      = ScreenSpan->Next;
            if (ScreenSpan == NULL) goto NextLine; // This line is full
            };
        //
        // ASSERT: ScreenSpan->End.X > Line->Start.X
        //
        // See if this span straddles the raster's starting point:
        //
        if (ScreenSpan->Start < Line->Start.X)
            {
            //
            // Add partial chunk to span buffer:
            //
            Accept = 1;
            UPDATE_PREVLINK_ALLOC(Line->Start.X,Min(Line->End.X, ScreenSpan->End));
            //
            // See if span entirely encloses raster; if so, break span
            // up into two pieces and we're done:
            //
            if (ScreenSpan->End > Line->End.X)
                {
                //
                // Get memory for the new span.  Note that this may be drawing from
                // the same memory pool as the destination.
                //
                NewScreenSpan        = (FSpan *)Screen.Mem->GetFast4(sizeof(FSpan));
                NewScreenSpan->Start = Line->End.X;
                NewScreenSpan->End   = ScreenSpan->End;
                NewScreenSpan->Next  = ScreenSpan->Next;
                //
                ScreenSpan->Next     = NewScreenSpan;
                ScreenSpan->End      = Line->Start.X;
                //
                Screen.ValidLines++;
                STAT(Screen.Churn++;)
                STAT(GStat.SpanTotalChurn++;)
                //
                goto NextLine; // Done (everything is clean)
                }
            else
                {
                //
                // Remove partial chunk from the span buffer:
                //
                ScreenSpan->End = Line->Start.X;
                //
                PrevScreenLink  = &(ScreenSpan->Next);
                ScreenSpan      = ScreenSpan->Next;
                if (ScreenSpan == NULL) goto NextLine; // Done (everything is clean)
                };
            };
        //
        // ASSERT: Span->Start >= Line->Start.X
        // if (ScreenSpan->Start < Line->Start.X) appError ("Span2");
        //
        // Process all screen spans that are entirely within the raster:
        //
        while (ScreenSpan->End <= Line->End.X)
            {
            //
            // Add entire chunk to temporary span buffer:
            //
            Accept = 1;
            UPDATE_PREVLINK_ALLOC(ScreenSpan->Start,ScreenSpan->End);
            //
            // Delete this span from the span buffer:
            //
            *PrevScreenLink = ScreenSpan->Next;
            ScreenSpan      = ScreenSpan->Next;
            Screen.ValidLines--;
            if (ScreenSpan==NULL) goto NextLine; // Done (everything is clean)
            };
        //
        // ASSERT: Span->End > Line->End.X
        // if (ScreenSpan->End <= Line->End.X) appError ("Span3");
        //
        // If span overlaps raster's end point, process the partial chunk:
        //
        if (ScreenSpan->Start < Line->End.X)
            {
            //
            // Add chunk from Span->Start to Line->End.X to temp span buffer
            //
            Accept = 1;
            UPDATE_PREVLINK_ALLOC(ScreenSpan->Start,Line->End.X);
            //
            // Shorten this span line by removing the raster:
            //
            ScreenSpan->Start = Line->End.X;
            };
        NextLine:
        *PrevLink = NULL;
        Line ++;
        };
    for (i=OurEnd; i<EndY; i++) *(TempIndex++) = NULL;

    #endif

    unclockSlow(GStat.CopyFromRasterUpdate);
    //
    #if PARANOID
    AssertGoodEnough("CopyFromRasterUpdate - Output spanbuffer");
    Screen.AssertValid("CopyFromRasterUpdate - Screen spanbuffer");
    #endif
    //
    return Accept;
    UNGUARD("FSpanBuffer::CopyFromRasterUpdate");
    };

//
// Grind this polygon through the span buffer and:
// - See if the poly is totally occluded
// - Build a new, temporary span buffer for raster and span clipping the poly
//
// Doesn't affect the span buffer no matter what.
// Returns 1 if poly is all or partially visible, 0 if completely obscured.
//
int FSpanBuffer::CopyFromRaster (FSpanBuffer &Screen,FRasterPoly &Raster)
    {
    clockSlow(GStat.CopyFromRaster);
    GUARD;
    FRasterLine *Line;
    FSpan       **ScreenIndex,*ScreenSpan;
    FSpan       **TempIndex,**PrevLink,*NewSpan;
	int			i,OurStart,OurEnd,Accept=0;
    //

    OurStart = Max(Raster.StartY,Screen.StartY);
    OurEnd   = Min(Raster.EndY,Screen.EndY);

 	TempIndex = &Index [0];

    // Extra check for OurStart>OurEnd = screen and rasterpoly don't
    // overlap, so all-null output.
    if (OurStart>=OurEnd)
        {
    	for (i=StartY; i<EndY; i++) *(TempIndex++) = NULL;
        return 0;
        }

    for (i=StartY; i<OurStart; i++) *(TempIndex++) = NULL;
    //
    Line        = &Raster.Lines [OurStart-Raster.StartY];
    ScreenIndex = &Screen.Index [OurStart-Screen.StartY];

    for (i=OurStart; i<OurEnd; i++)
        {
        ScreenSpan      = *(ScreenIndex++);
        PrevLink        = TempIndex++;
        //
        if ((!ScreenSpan) || (Line->End.X <= Line->Start.X)) goto NextLine; // This span is already full, or raster is empty
        //
        // Skip past all spans that occur before the raster:
        //
        while (ScreenSpan->End <= Line->Start.X)
            {
            ScreenSpan = ScreenSpan->Next;
            if (!ScreenSpan) goto NextLine; // This line is full
            };
        //
        // ASSERT: Span->End.X > Line->Start.X
        //
        // See if this span straddles the raster's starting point:
        //
        if (ScreenSpan->Start < Line->Start.X)
            {
            Accept = 1;
            //
            // Add partial chunk to temporary span buffer:
            //
            UPDATE_PREVLINK_ALLOC(Line->Start.X,Min(Line->End.X, ScreenSpan->End));
            ScreenSpan = ScreenSpan->Next;
            if (!ScreenSpan) goto NextLine;
            };
        //
        // ASSERT: Span->Start >= Line->Start.X
        //
        // Process all spans that are entirely within the raster:
        //
        while (ScreenSpan->End <= Line->End.X)
            {
            Accept = 1;
            //
            // Add entire chunk to temporary span buffer:
            //
            UPDATE_PREVLINK_ALLOC(ScreenSpan->Start,ScreenSpan->End);
            ScreenSpan = ScreenSpan->Next;
            if (!ScreenSpan) goto NextLine;
            };
        //
        // ASSERT: Span->End > Line->End.X
        //
        // If span overlaps raster's end point, process the partial chunk:
        //
        if (ScreenSpan->Start < Line->End.X)
            {
            //
            // Add chunk from Span->Start to Line->End.X to temp span buffer
            //
            Accept = 1;
            UPDATE_PREVLINK_ALLOC(ScreenSpan->Start,Line->End.X);
            };
        NextLine:
        *PrevLink = NULL;
        Line++;
        };
    for (i=OurEnd; i<EndY; i++) *(TempIndex++) = NULL;
    //
    #if PARANOID
    AssertGoodEnough("CopyFromRaster");
    #endif
    //
    unclockSlow(GStat.CopyFromRaster);
    return Accept;
    //
    UNGUARD("FSpanBuffer::CopyFromRaster");
    };


//
// Convert the span buffer to a linear span buffer.  Returns 1 if some
// spans were saved, or 0 if nothing was saved.
//
// Status: Not performance critical, very seldom called. No need to optimize.
//
int FSpanBuffer::CopyFromRange (FSpanBuffer &Screen, int Y1, int Y2, FMemPool *Mem)
    {
    clockSlow(GStat.CopyFromRange);
    GUARD;
    FSpan           *ScreenSpan,**ScreenIndex;
    FSpan           **TempIndex,**PrevLink,*TopSpan;
    int             n,Accept;
    //
    AllocIndex(Max(Y1,Screen.StartY), Min(Y2,Screen.EndY), Mem);
    Accept      = 0;
    TempIndex   = &Index [0];
    TopSpan     = &List  [0];
    ScreenIndex = &Screen.Index[0];
    //
    n = EndY - StartY;
    while (n-- > 0)
        {
        ScreenSpan      = *(ScreenIndex++);
        PrevLink        = TempIndex++;
        //
        if (ScreenSpan)
            {
            Accept = 1;
            while (ScreenSpan)
                {
                UPDATE_PREVLINK(ScreenSpan->Start,ScreenSpan->End);
                ScreenSpan = ScreenSpan->Next;
                };
            };
        *PrevLink = NULL;
        };
    Mem->GetFast4((int)TopSpan - (int)List);
    //
    #if PARANOID
    AssertGoodEnough("CopyFromRange");
    #endif
    //
    unclockSlow(GStat.CopyFromRange);
    return Accept;
    //
    UNGUARD("FSpanBuffer::CopyFromRange");
    };

/*-----------------------------------------------------------------------------
    Merging.
-----------------------------------------------------------------------------*/

//
// Macro for copying a span.
//
#define COPY_SPAN(SRC_INDEX)\
    {\
    PrevLink         = DestIndex++;\
    Span             = *(SRC_INDEX++);\
    while (Span)\
        {\
        UPDATE_PREVLINK(Span->Start,Span->End);\
        Span = Span->Next;\
        };\
    *PrevLink = NULL;\
    };

//
// Merge two span buffers into a new (newly-allocated) span buffer.
//
// Status: Very seldom-called; doesn't need to be optimized.
//
void FSpanBuffer::MergeFrom (const FSpanBuffer &Source1,const FSpanBuffer &Source2, FMemPool *Mem)
    {
    clockSlow(GStat.MergeFrom);
    GUARD;
    FSpan   **DestIndex,*TopSpan,**PrevLink;
    FSpan   *Span,*PrevSpan,*Span1,*Span2,**Index1,**Index2;
    int     Y;
    //
    StartY = Min(Source1.StartY,Source2.StartY);
    EndY   = Max(Source1.EndY,Source2.EndY);
    Index  = (FSpan **)Mem->Get((EndY - StartY) * sizeof (FSpan *));
    List   = (FSpan  *)Mem->GetFast4(0);
    //
    DestIndex = &Index [0];
    TopSpan   = &List  [0];
    //
    Index1    = &Source1.Index[0];
    Index2    = &Source2.Index[0];
    //
    Y = StartY;
    //
    while ((Y < Source2.EndY) && (Y < Source1.StartY)) // Copy just buffer 2
        {
        COPY_SPAN(Index2);
        Y++;
        };
    while ((Y < Source1.EndY) && (Y < Source2.StartY)) // Copy just buffer 1
        {
        COPY_SPAN(Index1);
        Y++;
        };
    if ((Y >= Source1.StartY) && (Y >= Source2.StartY)) // Merge overlapping areas
        {
        while ((Y < Source1.EndY) && (Y < Source2.EndY)) // Merge both buffers
            {
            Span1          = *(Index1++);
            Span2          = *(Index2++);
            PrevSpan       = NULL;
            PrevLink       = DestIndex++;
            //
            while (Span1 || Span2)
                {
                if ((!Span2) || (Span1 && (Span1->Start <= Span2->Start)))
                    {
                    // Process Span1
                    if (PrevSpan && (Span1->Start <= PrevSpan->End))
                        {
                        PrevSpan->End = Max (PrevSpan->End,Span1->End); // Merge
                        }
                    else
                        {
                        PrevSpan = TopSpan;
                        UPDATE_PREVLINK(Span1->Start,Span1->End);
                        };
                    Span1 = Span1->Next;
                    }
                else
                    {
                    // Process Span2
                    if (PrevSpan && (Span2->Start <= PrevSpan->End))
                        {
                        PrevSpan->End = Max (PrevSpan->End,Span2->End); // Merge
                        }
                    else
                        {
                        PrevSpan = TopSpan;
                        UPDATE_PREVLINK(Span2->Start,Span2->End);
                        };
                    Span2 = Span2->Next;
                    };
                };
            *PrevLink = NULL;
            Y++;
            };
        }
    else // Copy empty section
        {
        while ((Y < Source1.StartY) || (Y < Source2.StartY))
            {
            *(DestIndex++) = NULL;
            Y++;
            };
        };
    while ((Y >= Source1.StartY) && (Y < Source1.EndY)) // Copy just buffer 1
        {
        COPY_SPAN(Index1);
        Y++;
        };
    while ((Y >= Source2.StartY) && (Y < Source2.EndY)) // Copy just buffer 2
        {
        COPY_SPAN(Index2);
        Y++;
        };
    Mem->GetFast4((int)TopSpan - (int)List);
    //
    #if PARANOID
    AssertGoodEnough("MergeFrom");
    #endif
    //
    UNGUARD("FSpanBuffer::MergeFrom");
    unclockSlow(GStat.MergeFrom);
    };

//
// Merge this existing span buffer with another span buffer.  Overwrites the appropriate
// parts of this span buffer.  If this span buffer's index isn't large enough
// to hold everything, reallocates the index.
//
// This is meant to be called with this span buffer using GDynMem and the other span
// buffer using GMem.
//
// Status: This is currently unused and doesn't need to be optimized.
//



#ifdef  MW_ASM

extern "C" int _cdecl MergeWithMASM (FSpanBuffer *AsmThis,
                                     const FSpanBuffer &AsmOther);

#endif


void FSpanBuffer::MergeWith(const FSpanBuffer &Other)
    {
    clockSlow(GStat.MergeWith);
    GUARD;

    #ifdef MW_ASM
    MergeWithMASM( this, Other ); // validlines: 1944claimed, 162 really !!
    #else

    //
    // See if the existing span's index is large enough to hold the merged result:
    //
    if ((Other.StartY < StartY) || (Other.EndY > EndY)) // Must reallocate and copy index
        {
        int NewStartY = Min(StartY,Other.StartY);
        int NewEndY   = Max(EndY,  Other.EndY);
        int NewNum    = NewEndY - NewStartY;
        FSpan **NewIndex = (FSpan **)Mem->Get(NewNum*sizeof(FSpan *));
        //
        mymemset(&NewIndex[0                    ],0,    (StartY-NewStartY)*sizeof(FSpan *));
        memcpy(&NewIndex[StartY-NewStartY     ],Index,(EndY     -StartY)*sizeof(FSpan *));
        mymemset(&NewIndex[NewNum-(NewEndY-EndY)],0,    (NewEndY  -EndY  )*sizeof(FSpan *));
        //
        StartY = NewStartY;
        EndY   = NewEndY;
        Index  = NewIndex;
        //
        STAT(GStat.SpanRejig++);
        };
    //
    // Now merge other span into this one:
    //
    FSpan **ThisIndex  = &Index       [Other.StartY - StartY];
    FSpan **OtherIndex = &Other.Index [0];
    FSpan *ThisSpan,*OtherSpan,*TempSpan,**PrevLink;
    //
    for (int i=Other.StartY; i<Other.EndY; i++)
        {
        PrevLink    = ThisIndex;
        ThisSpan    = *(ThisIndex++);
        OtherSpan   = *(OtherIndex++);
        //
        // Do everything relative to ThisSpan:
        //
        while (ThisSpan && OtherSpan)
            {
            if (OtherSpan->End < ThisSpan->Start) // Link OtherSpan in completely before ThisSpan:
                {
                *PrevLink = TempSpan= (FSpan *)Mem->GetFast4(sizeof(FSpan));
                TempSpan->Start     = OtherSpan->Start;
                TempSpan->End       = OtherSpan->End;
                TempSpan->Next      = ThisSpan;
                PrevLink            = &TempSpan->Next;
                //
                OtherSpan           = OtherSpan->Next;
                //
                ValidLines++;
                }
            else
            if (OtherSpan->Start <= ThisSpan->End) // Merge OtherSpan into ThisSpan:
                {
                *PrevLink           = ThisSpan;
                ThisSpan->Start     = Min(ThisSpan->Start,OtherSpan->Start);
                ThisSpan->End       = Max(ThisSpan->End,  OtherSpan->End);
                TempSpan            = ThisSpan; // For maintaining End and Next
                //
                PrevLink            = &ThisSpan->Next;
                ThisSpan            = ThisSpan->Next;
                OtherSpan           = OtherSpan->Next;
                //
                while (1)
                    {
                    if (ThisSpan&&(ThisSpan->Start <= TempSpan->End))
                        {
                        TempSpan->End = Max(ThisSpan->End,TempSpan->End);
                        ThisSpan      = ThisSpan->Next;
                        ValidLines--;
                        }
                    else if (OtherSpan&&(OtherSpan->Start <= TempSpan->End))
                        {
                        TempSpan->End = Max(TempSpan->End,OtherSpan->End);
                        OtherSpan     = OtherSpan->Next;
                        }
                    else break;
                    };
                }
            else // This span is entirely before the other span; keep it.
                {
                *PrevLink           = ThisSpan;
                PrevLink            = &ThisSpan->Next;
                ThisSpan            = ThisSpan->Next;
                };
            };
        while (OtherSpan) // Just append spans from OtherSpan:
            {
            *PrevLink = TempSpan    = (FSpan *)Mem->GetFast4(sizeof(FSpan));
            TempSpan->Start         = OtherSpan->Start;
            TempSpan->End           = OtherSpan->End;
            PrevLink                = &TempSpan->Next;
            //
            OtherSpan               = OtherSpan->Next;
            //
            ValidLines++;
            };
        *PrevLink = ThisSpan;
        };

    #endif


    #if PARANOID
    AssertGoodEnough("MergeWith");
    #endif
    //
    UNGUARD("FSpanBuffer::MergeWith");
    unclockSlow(GStat.MergeWith);
    };

/*-----------------------------------------------------------------------------
    Duplicating.
-----------------------------------------------------------------------------*/

//
// Copy the index from one span buffer to another span buffer.
//
// Status: Seldom called, no need to optimize.
//
void FSpanBuffer::CopyIndexFrom (const FSpanBuffer &Source, FMemPool *Mem)
    {
    clockSlow(GStat.CopyIndexFrom);
    GUARD;
    //
    StartY   = Source.StartY;
    EndY     = Source.EndY;
    //
    Index = (FSpan **)Mem->Get((Source.EndY-Source.StartY)*sizeof(FSpan *));
    memcpy(&Index[0],&Source.Index[0],(Source.EndY-Source.StartY)*sizeof(FSpan *));
    UNGUARD("FSpanBuffer::CopyIndexFrom");
	//
    unclockSlow(GStat.CopyIndexFrom);
    };

/*-----------------------------------------------------------------------------
    Lattice span downsizing.
-----------------------------------------------------------------------------*/

//
// Create a new span buffer by downsizing an existing one to a given lattice
// size.  Each 'pixel' in the destination span buffer if and only if there is
// one or more 'pixels' set in the corresponding region of the source span buffer.
//
// Status: Extremely performance critical.
//


#ifdef  RECT_ASM
extern "C" int _cdecl CalcRectFromMASM (FSpanBuffer *AsmThis,
                                  const FSpanBuffer &Source,
                                               BYTE GridXBits,
                                               BYTE GridYBits,
                                           FMemPool *Mem);
#endif



int FSpanBuffer::CalcRectFrom(const FSpanBuffer &Source,BYTE GridXBits,BYTE GridYBits, FMemPool *Mem)
    {
    clockSlow(GStat.CalcRectFrom);
    GUARD;

    #ifdef RECT_ASM
    int NonEmpty = CalcRectFromMASM (this,Source,GridXBits,GridYBits,Mem);

    #else

    FSpan   **DestIndex,*TopSpan,**SourceIndex,**PrevLink;
    int     V,GY,SpanY1,Y,SpanYL,Updated,NonEmpty=0;
    int     TestStart,TestEnd,DestStart,DestEnd;
    //
    SourceIndex = (FSpan **)Mem->Get(sizeof(FSpan *)<<GridYBits);
    AllocIndex (Source.StartY>>GridYBits,1+((Source.EndY-1)>>GridYBits), Mem);
    //
    DestIndex   = &Index [0];
    TopSpan     = &List  [0];
    GY          = (StartY << GridYBits) - Source.StartY;
    //
    for (V=StartY; V<EndY; V++)
        {
        SpanY1      = Max(GY,0);
        SpanYL      = Min(GY+(1<<GridYBits),Source.EndY-Source.StartY) - SpanY1;
        PrevLink    = DestIndex++;
        //
        for (Y=0; Y<SpanYL; Y++) SourceIndex[Y] = Source.Index[Y + SpanY1];
        //
        FindMinLoop:
        //
        DestStart = 9999; // Arbitrary maximum
        DestEnd   = 0;
        for (Y=0; Y<SpanYL; Y++)
            {
            if (SourceIndex[Y])
                {
                TestStart = CUTSTART(SourceIndex[Y]->Start);
                if (TestStart <= DestStart)
                    {
                    DestStart       = TestStart;
                    DestEnd         = CUTEND(SourceIndex[Y]->End);
                    };
                };
            };
        if (DestEnd)
            {
            NonEmpty = 1;
            do
                {
                Updated=0;
                for (Y=0; Y<SpanYL; Y++)
                    {
                    if (SourceIndex[Y] && (CUTSTART(SourceIndex[Y]->Start) <= DestEnd))
                        {
                        TestEnd = CUTEND(SourceIndex[Y]->End);
                        if (TestEnd > DestEnd) DestEnd = TestEnd;
                        Updated = 1;
                        SourceIndex[Y] = SourceIndex[Y]->Next;
                        };
                    };
                } while (Updated);
            UPDATE_PREVLINK(DestStart,DestEnd);
            goto FindMinLoop;
            };
        *PrevLink  = NULL;
        GY        += 1<<GridYBits;
        };
    Mem->GetFast4((int)TopSpan - (int)List);
    //

    #endif

    #if PARANOID
    AssertGoodEnough("CalcRectFrom");
    #endif
    //
    unclockSlow(GStat.CalcRectFrom);
    return NonEmpty;
    //
    UNGUARD("FSpanBuffer::CalcRectFrom");
    };

//
// Create a new span buffer in that indicates which lattice points
// must be computed assuming Source is a span buffer indicating visiblity
// of lattice rectangles.  pixel(x,y) should be set if
// pixel(x,y), pixel(x-1,y), pixel(x,y-1), or pixel(x-1,y-1) is set.
//
// Status: Extremely performance critical.
//


#ifdef  LATT_ASM
extern "C" int _cdecl CalcLatticeFromMASM (FSpanBuffer *AsmThis,
                                     const FSpanBuffer &Source,
                                              FMemPool *Mem);
#endif



void FSpanBuffer::CalcLatticeFrom(const FSpanBuffer &Source, FMemPool *Mem)
    {
    clockSlow(GStat.CalcLatticeFrom);
    GUARD;

    #ifdef LATT_ASM
    CalcLatticeFromMASM(this,Source,Mem);
    #else


    FSpan       *Span1,*Span2,*TopSpan,**PrevLink;
    int         i,n,DestStart,DestEnd;
    //
    AllocIndex(Source.StartY,Source.EndY+1,Mem);
    n           = Source.EndY - Source.StartY;
    TopSpan     = &List[0];
    //
    Span1       = Source.Index[0];
    PrevLink    = &Index[0];
    while (Span1)
        {
        DestStart = Span1->Start;
        DestEnd   = Span1->End + 1;
        Span1     = Span1->Next;
        //
        while (Span1 && (Span1->Start <= DestEnd))
            {
            DestEnd = Span1->End+1;
            Span1   = Span1->Next;
            };
        UPDATE_PREVLINK(DestStart,DestEnd);
        };
    *PrevLink = NULL;
    //
    for (i=1; i<n; i++)
        {
        Span1    = Source.Index[i-1];
        Span2    = Source.Index[i];
        PrevLink = &Index[i];
        //
        while (Span1 || Span2)
            {
            if ((!Span2) || (Span1 && (Span1->Start<=Span2->Start)))
                {
                DestStart = Span1->Start;
                DestEnd   = Span1->End + 1;
                Span1     = Span1->Next;
                //
                while (1)
                    {
                    if (Span2 && (Span2->Start<=DestEnd))
                        {
                        if ((Span2->End+1)>DestEnd) DestEnd = Span2->End+1;
                        Span2   = Span2->Next;
                        }
                    else if (Span1 && (Span1->Start<=DestEnd))
                        {
                        if ((Span1->End+1)>DestEnd) DestEnd = Span1->End+1;
                        Span1   = Span1->Next;
                        }
                    else break;
                    };
                UPDATE_PREVLINK(DestStart,DestEnd);
                }
            else
                {
                DestStart = Span2->Start;
                DestEnd   = Span2->End + 1;
                Span2     = Span2->Next;
                //
                while (1)
                    {
                    if (Span1 && (Span1->Start<=DestEnd))
                        {
                        if ((Span1->End+1)>DestEnd) DestEnd = Span1->End+1;
                        Span1   = Span1->Next;
                        }
                    else if (Span2 && (Span2->Start<=DestEnd))
                        {
                        if ((Span2->End+1)>DestEnd) DestEnd = Span2->End+1;
                        Span2   = Span2->Next;
                        }
                    else break;
                    };
                UPDATE_PREVLINK(DestStart,DestEnd);
                };
            };
        *PrevLink = NULL;
        };
    Span1       = Source.Index[n-1];
    PrevLink    = &Index[n];
    while (Span1)
        {
        DestStart = Span1->Start;
        DestEnd   = Span1->End + 1;
        Span1     = Span1->Next;
        //
        while (Span1 && (Span1->Start <= DestEnd))
            {
            DestEnd = Span1->End+1;
            Span1   = Span1->Next;
            };
        UPDATE_PREVLINK(DestStart,DestEnd);
        };
    *PrevLink = NULL;
    Mem->GetFast4((int)TopSpan - (int)List);
    //

    #endif

    #if PARANOID
    AssertGoodEnough("CalcLatticeFrom");
    #endif
    //
    UNGUARD("FSpanBuffer::CalcLatticeFrom");
    unclockSlow(GStat.CalcLatticeFrom);
    };

/*-----------------------------------------------------------------------------
    Debugging.
-----------------------------------------------------------------------------*/

//
// These debugging functions are available while writing span buffer code.
// They perform various checks to make sure that span buffers don't become
// corrupted.  They don't need optimizing, of course.
//

//
// Make sure that a span buffer is completely empty.
//
void FSpanBuffer::AssertEmpty (char *Name)
    {
    GUARD;
    FSpan **TempIndex,*Span;
    int i;
    //
    TempIndex = Index;
    for (i=StartY; i<EndY; i++)
        {
        Span = *(TempIndex++);
        while (Span!=NULL)
            {
            appErrorf("%s not empty, line=%i<%i>%i, start=%i, end=%i",Name,StartY,i,EndY,Span->Start,Span->End);
            Span=Span->Next;
            };
        };
    UNGUARD("FSpanBuffer::AssertEmpty");
    };

//
// Assure that a span buffer isn't empty.
//
void FSpanBuffer::AssertNotEmpty (char *Name)
    {
    GUARD;
    FSpan **TempIndex,*Span;
    int i,NotEmpty=0;
    //
    TempIndex = Index;
    for (i=StartY; i<EndY; i++)
        {
        Span = *(TempIndex++);
        while (Span!=NULL)
            {
            if (Span->Start>=Span->End) appErrorf("%s contains %i-length span",Name,Span->End-Span->Start);
            NotEmpty=1;
            Span=Span->Next;
            };
        };
    if (!NotEmpty) appErrorf ("%s is empty",Name);
    UNGUARD("FSpanBuffer::AssertNotEmpty");
    };

//
// Make sure that a span buffer is valid.  Performs the following checks:
// - Make sure there are no zero-length spans
// - Make sure there are no negative-length spans
// - Make sure there are no overlapping spans
// - Make sure all span pointers are valid (otherwise GPF's)
//

void FSpanBuffer::AssertValid (char *Name)
    {
    GUARD;
    FSpan **TempIndex,*Span;
    int i,PrevEnd,c=0;
    //
    TempIndex = Index;
    for (i=StartY; i<EndY; i++)
        {
        PrevEnd = -1000;
        Span = *(TempIndex++);
        while (Span)
            {
            if ((i==StartY)||(i==(EndY-1)))
                {
                if ((PrevEnd!=-1000) && (PrevEnd >= Span->Start)) appErrorf("%s contains %i-length overlap, line %i/%i",Name,PrevEnd-Span->Start,i-StartY,EndY-StartY);
                if (Span->Start>=Span->End) appErrorf("%s contains %i-length span, line %i/%i",Name,Span->End-Span->Start,i-StartY,EndY-StartY);
                PrevEnd = Span->End;
                };
            Span=Span->Next;
            c++;
            };
        };
    if (c!=ValidLines) appErrorf ("%s bad ValidLines: claimed=%i, correct=%i",Name,ValidLines,c);
    UNGUARD("FSpanBuffer::AssertValid");
    };

//
// Like AssertValid, but 'ValidLines' checked for zero/nonzero only.
//
void FSpanBuffer::AssertGoodEnough (char *Name)
    {
    GUARD;
    FSpan **TempIndex,*Span;
    int i,PrevEnd,c=0;
    //
    TempIndex = Index;
    for (i=StartY; i<EndY; i++)
        {
        PrevEnd = -1000;
        Span = *(TempIndex++);
        while (Span)
            {
            if ((i==StartY)||(i==(EndY-1)))
                {
                if ((PrevEnd!=-1000) && (PrevEnd >= Span->Start)) appErrorf("%s contains %i-length overlap, line %i/%i",Name,PrevEnd-Span->Start,i-StartY,EndY-StartY);
                if (Span->Start>=Span->End) appErrorf("%s contains %i-length span, line %i/%i",Name,Span->End-Span->Start,i-StartY,EndY-StartY);
                PrevEnd = Span->End;
                };
            Span=Span->Next;
            c++;
            };
        };
    if ((c==0)!=(ValidLines==0)) appErrorf ("%s bad ValidLines: claimed=%i, correct=%i",Name,ValidLines,c);
    UNGUARD("FSpanBuffer::AssertValid");
    };

//
// Draw a span buffer on the screen for debugging
//
void FSpanBuffer::DebugDraw (ICamera *Camera,BYTE Color)
    {
    GUARD;
    FSpan   **TempIndex = Index;
    BYTE    *Dest       = &Camera->Screen[StartY * Camera->SXR];
    for (int i=StartY; i<EndY; i++)
        {
        FSpan *Span = *(TempIndex++);
        while (Span)
            {
            mymemset(Dest+Span->Start,Color,Span->End-Span->Start);
            Span = Span->Next;
            };
        Dest += Camera->SXR;
        };
    UNGUARD("FSpanBuffer::DebugDraw");
    };

/*-----------------------------------------------------------------------------
    The End.
-----------------------------------------------------------------------------*/
