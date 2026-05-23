/*=============================================================================
    aalinedo.h : Main include file for depth-shaded linedrawing
                 Included 3 times: for 32, 15 and 16 bit drawing.

    Copyright 1997 Epic MegaGames, Inc. and Evolution Software

    Compiled with Visual C++ 4.2    Best viewed with Tabs=4.

    Revision history:
        * Created by Erik de Neve  March 1997

=============================================================================*/

/* This file has the stuff that gets expanded from inside:
void DrawDepthAALine
(
	UCamera	*Camera, // camera (screen data)
    BYTE	Color,   // selects a colortable
    INT		Dotted,  // !=0 : dots on major axis
    FLOAT	X1,      // starting point
    FLOAT	Y1,      // ,,
    FLOAT	RZ1,     // starting intensity [0-1>
    FLOAT	X2,      // end point
    FLOAT	Y2,      // ,,
    FLOAT	RZ2      // ending intensity [0-1>
)
*/

// These defines add tags to labels to make them local to one of three inclusions
// of this code; needed because all labels have function-wide scope by default.

#define SkipDrawing     LocalTag(SkipDrawing)

#define InLoopSkip0     LocalTag(InLoopSkip0)
#define InLoopSkip1     LocalTag(InLoopSkip1)
#define ReLoop          LocalTag(ReLoop)
#define ReLoop0         LocalTag(ReLoop0)
#define SingleLastPix   LocalTag(SingleLastPix)
#define SingleLastPix0  LocalTag(SingleLastPix0)

#define MidLoop0        LocalTag(MidLoop0)

#define SkipDrawing     LocalTag(SkipDrawing)
#define SinglePixX      LocalTag(SinglePixX)
#define SinglePixY      LocalTag(SinglePixY)
#define BothSingle      LocalTag(BothSingle)
#define AsmInLoop       LocalTag(AsmInLoop)
#define InLoop          LocalTag(InLoop)

// Static because inline assembler needs to access these without EBP.
static int     PixelMinorStep,PixelMajorStep,FullPixels;
static int     Main,Chunk,ColorDev,MainDev,MainDevDev;
static BYTE    *PixelDest,*PixelDestF,*PixelDestL;
static DWORD   TabPixA,TabPixB,TabPixAL,TabPixBL;

// Variables.
float  InvMajorLen,MainF,MajorEndFrac,MajorStartFrac,SlopeF;
float  ColorStart,ColorStartF,ColorEnd,MajorLen;
float  MainFF,MainFL;
float  Y0,X0, Y0F,X0F,  X0L,Y0L,  Y15,Y25,  X15,X25;
float  ColorSlope;

int    LastNeeded;
int    MajorXY;

DWORD ScreenPixA;
DWORD ScreenPixB;
DWORD SumAddA;
DWORD SumAddB;
DWORD MaskA;
DWORD MaskB;

//
// Start of the floating -> fixed point preparations.
//

float  LengthX = X2-X1;
float  LengthY = Y2-Y1;

int    DebugLoopCnt = 0;

// Determine major axis.
if( Abs(LengthX) > Abs(LengthY) )
{
    // X is major axis.
    MajorXY=1;
    if( X2 < X1 )
    {
		// Reverse to always go from lower to higher X.
        float FTempX = X2;
        float FTempY = Y2;
        X2 = X1;
        Y2 = Y1;
        X1 = FTempX;
        Y1 = FTempY;

        FTempX = RZ1;
        RZ1    = RZ2;
        RZ2    = FTempX;

        LengthX = X2-X1;
        LengthY = Y2-Y1;
	}


    X15 = X1+0.5F;
    X25 = X2+0.5F;

    MajorLen    = LengthX;
    InvMajorLen = 1/LengthX;                  // always positive.

    // Number of pixel boundaries crossed:
    FullPixels  = TruncFL(X25) - TruncFL(X15);

    if( FullPixels == 0 )
		goto SinglePixX; // single pix, X major axis...

    if( LengthY>=0 )
    {
        PixelMinorStep = (Camera->Stride << SHIFT);
        SlopeF         = LengthY * InvMajorLen;      // Ylen/Xlen...

        Y0F            = Y1 + SlopeF*(0.5F * InvcFL(X15));
        Y0             = Y1 + SlopeF*(1.5F - FracFL(X15));   // want Y position in MIDDLE of first full pix !
        Y0L            = Y2 - SlopeF*(0.5F * FracFL(X25));
        PixelDest      = Camera->Screen + ( ( TruncFL(X15) + 1 + (TruncFL(Y0 ))*Camera->Stride) << SHIFT );
        PixelDestF     = Camera->Screen + ( ( TruncFL(X15) +     (TruncFL(Y0F))*Camera->Stride) << SHIFT );
        PixelDestL     = Camera->Screen + ( ( TruncFL(X25) +     (TruncFL(Y0L))*Camera->Stride) << SHIFT );
        MainF          = InvcFL(Y0 );
        MainFF         = InvcFL(Y0F);
        MainFL         = InvcFL(Y0L);
    }
    else
    {
        PixelMinorStep = - (Camera->Stride << SHIFT);
        SlopeF         = - LengthY * InvMajorLen;  // note slope is UNSIGNED.

        Y0F            = Y1 - SlopeF*(0.5F * InvcFL(X15));
        Y0             = Y1 - SlopeF*(1.5F - FracFL(X15));
        Y0L            = Y2 + SlopeF*(0.5F * FracFL(X25));
        PixelDest      = Camera->Screen + ( ( TruncFL(X15) + 1 + (TruncFL(Y0 )+1)*Camera->Stride) << SHIFT );
        PixelDestF     = Camera->Screen + ( ( TruncFL(X15) +     (TruncFL(Y0F)+1)*Camera->Stride) << SHIFT );
        PixelDestL     = Camera->Screen + ( ( TruncFL(X25) +     (TruncFL(Y0L)+1)*Camera->Stride) << SHIFT );
        MainF          = FracFL(Y0 );
        MainFF         = FracFL(Y0F);
        MainFL         = FracFL(Y0L);
    };

    if( Dotted == 0 )
    {
        float LumFactor = LumAdjustX[ RoundFL(SlopeF*255.0F) ];
        RZ1 *=LumFactor;
        RZ2 *=LumFactor;
    }

    // Important safety measure, avoids underflow of the intensity envelope.
    if( RZ1<0.0039F ) RZ1=0.0039F;
    if( RZ2<0.0039F ) RZ2=0.0039F;

    ColorSlope  = ( RZ2 - RZ1 ) * InvMajorLen;

    ColorStartF    = RZ1;
    ColorStart     = RZ1 + ColorSlope*( 1.5F - FracFL(X15) );
    ColorEnd       = RZ2 - ColorSlope*(FracFL(X25)*0.5F);

    PixelMajorStep = 1 << SHIFT;

    MajorStartFrac = InvcFL(X15);
    MajorEndFrac   = FracFL(X25);
}
else
{

    // Y is major axis.
    MajorXY=0;

    if( Y2 < Y1 )
    {
		// reverse to always go from lower to higher Y.
        float FTempY = Y2;
        float FTempX = X2;

        Y2 = Y1;
        X2 = X1;
        Y1 = FTempY;
        X1 = FTempX;

        FTempX = RZ1;
        RZ1    = RZ2;
        RZ2    = FTempX;

        LengthX = X2-X1;
        LengthY = Y2-Y1;
    }

    Y15 = Y1+0.5F;
    Y25 = Y2+0.5F;

    MajorLen    = LengthY;
    InvMajorLen = 1/LengthY;                     // FP divide - always positive.

    // Number of pixel boundaries crossed:
    FullPixels  = TruncFL(Y25) - TruncFL(Y15);   //

    if( FullPixels == 0 )
		goto SinglePixY; // single pix, X major axis...

    if( LengthX>=0 )
    {
        PixelMinorStep = (1 << SHIFT);
        SlopeF         = LengthX * InvMajorLen;  // 1 or smaller...

        X0F            = X1 + SlopeF*(0.5F*InvcFL(Y15));
        X0             = X1 + SlopeF*(1.5F-FracFL(Y15));
        X0L            = X2 - SlopeF*(0.5F*FracFL(Y25));
        int StartPixLoc= TruncFL(Y15) * Camera->Stride;
        PixelDest      = Camera->Screen + ( ( TruncFL(X0 ) + StartPixLoc + Camera->Stride) << SHIFT );
        PixelDestF     = Camera->Screen + ( ( TruncFL(X0F) + StartPixLoc) << SHIFT );
        PixelDestL     = Camera->Screen + ( ( TruncFL(X0L) + (TruncFL(Y25)  ) * Camera->Stride) << SHIFT );
        MainF          = InvcFL(X0 );
        MainFF         = InvcFL(X0F);
        MainFL         = InvcFL(X0L);
    }
    else
    {
        PixelMinorStep = - (1 << SHIFT);
        SlopeF         = - LengthX * InvMajorLen;  // 1 or smaller..

        X0F            = X1 - SlopeF*(0.5F*InvcFL(Y15));
        X0             = X1 - SlopeF*(1.5F-FracFL(Y15));
        X0L            = X2 + SlopeF*(0.5F*FracFL(Y25));
        int StartPixLoc= TruncFL(Y15) * Camera->Stride;
        PixelDest      = Camera->Screen + ( ( TruncFL(X0 ) + 1 + StartPixLoc + Camera->Stride ) << SHIFT );
        PixelDestF     = Camera->Screen + ( ( TruncFL(X0F) + 1 + StartPixLoc ) << SHIFT );
        PixelDestL     = Camera->Screen + ( ( TruncFL(X0L) + 1 + (TruncFL(Y25)  ) * Camera->Stride) << SHIFT );
        MainF          = FracFL(X0 );
        MainFF         = FracFL(X0F);
        MainFL         = FracFL(X0L);
    }

    if( Dotted == 0 )
    {
        float LumFactor = LumAdjustY[ RoundFL(SlopeF*255.0F) ];
        RZ1 *=LumFactor;
        RZ2 *=LumFactor;
    }

    // Important safety measure, avoids underflow of the intensity envelope
    if (RZ1<0.0039F) RZ1=0.0039F;
    if (RZ2<0.0039F) RZ2=0.0039F;

    ColorSlope  = ( RZ2 - RZ1 ) * InvMajorLen;

    ColorStartF    = RZ1;
    ColorStart     = RZ1 + ColorSlope*( 1.5F - FracFL(Y15) );
    ColorEnd       = RZ2 - ColorSlope*(FracFL(Y25)*0.5F);

    PixelMajorStep = Camera->Stride << SHIFT;

    MajorStartFrac = InvcFL(Y15);
    MajorEndFrac   = FracFL(Y25);
}

    Main        = IntFix31( MainF * ColorStart );
    Chunk       = IntFix31( ColorStart );           // Chunk == Intensity Range ofsetting..
    ColorDev    = IntFix31( ColorSlope );
    MainDevDev  = IntFix31( 2.0F * ColorSlope * SlopeF );
    MainDev     = IntFix31( SlopeF * (ColorStart+ColorSlope) - ((MainF) * ColorSlope ) );

    // MainDev == IntFix31( SlopeF * ColorStartF - ((MainF) * ColorSlope ) + (ColorSlope*SlopeF)  );

    // direct COLOR for first & last pixel cases.

    TabPixA     = (DWORD) ColorTab [ Fix32( (   MainFF  ) * ColorStartF * MajorStartFrac ) >> 24 ];
    TabPixB     = (DWORD) ColorTab [ Fix32( ( 1-MainFF  ) * ColorStartF * MajorStartFrac ) >> 24 ];

    TabPixAL    = (DWORD) ColorTab  [ Fix32( (   MainFL  ) * ColorEnd * MajorEndFrac ) >> 24 ];
    TabPixBL    = (DWORD) ColorTab  [ Fix32( ( 1-MainFL  ) * ColorEnd * MajorEndFrac ) >> 24 ];

#if ASM
	if( Dotted == 0 )
		goto AsmInLoop;
#else
    if( Dotted == 0 )
		goto InLoop;
#endif
    else
    {
		// special case: DOTTED line.
		int DotStart;

		// let start depend on odd/even of the major axis start.
		if ( MajorXY == 1)
			DotStart =  1 & (RoundFL(X1));
        else
			DotStart =  1 & (RoundFL(Y1));

		// figure out if last pixel has to be blank
		LastNeeded =  (DotStart ^ FullPixels) & 1;

		if( DotStart ) goto InLoopSkip1;
        else           goto InLoopSkip0;
    }

    // Special case: line starts and ends inside same pixel ->  2 actual pixels to plot along
    // major axis.
SinglePixX:

    PixelMinorStep = (Camera->Stride << SHIFT); // always positive...
    SlopeF         =  Abs( LengthY * InvMajorLen ); // ONLY needed for lumadjust
    Y0             =  (Y1 + Y2)*0.5F;

    PixelDestL  = Camera->Screen + ( (TruncFL(X15) + TruncFL(Y0) * Camera->Stride) << SHIFT );
    MainF = InvcFL(Y0);

    goto BothSingle;

SinglePixY:

    // Y major axis: Y1<Y2.
    PixelMinorStep = (1 << SHIFT); //always positive...
    SlopeF         =  Abs( LengthX * InvMajorLen );  // ONLY needed for lumadjust
    X0 =  (X1 + X2)*0.5F;

    PixelDestL  = Camera->Screen + ( (TruncFL(X0) + TruncFL(Y15) * Camera->Stride) << SHIFT );
    MainF = InvcFL(X0);

BothSingle:  // LumAdjustX no longer different from Y (lines too short anyway).
{
    float LumFactor = LumAdjustY[ RoundFL(SlopeF*255.0F) ];
    float RZM = (RZ1+RZ2) * LumFactor* 0.5F* MajorLen;

    TabPixAL =  (DWORD) ColorTab [ Fix32( RZM * (   MainF  )) >> 24 ];
    TabPixBL =  (DWORD) ColorTab [ Fix32( RZM * ( 1-MainF  )) >> 24 ];

    goto SingleLastPix;
}

// Drawing: Antialiased, all directions, nondotted, all colordepths.
InLoop:

    ScreenPixA = (DWORD) *(PixCast*)(PixelDestF);
    ScreenPixB = (DWORD) *(PixCast*)(PixelDestF+PixelMinorStep);

    SumAddA = TabPixA + ScreenPixA;   //  sum with carries
    SumAddB = TabPixB + ScreenPixB;   //

    MaskA = ((TabPixA ^ ScreenPixA) ^ SumAddA) & CarryMask; // carryless sums
    MaskB = ((TabPixB ^ ScreenPixB) ^ SumAddB) & CarryMask;

    SumAddA |=  MaskA - (MaskA >> ChannelBits);
    SumAddB |=  MaskB - (MaskB >> ChannelBits);

    *(PixCast*)(PixelDestF)                = (PixCast) SumAddA;
    *(PixCast*)(PixelDestF+PixelMinorStep) = (PixCast) SumAddB;

    if ( !( ++DebugLoopCnt < FullPixels)) goto SingleLastPix;
    // if FullPixels was just 1, goto Last pix now.

ReLoop:
    TabPixA = (DWORD) ColorTab [          (Main)  >> 23 ];
    TabPixB = (DWORD) ColorTab [  (Chunk - Main)  >> 23 ];

    ScreenPixA = (DWORD) *(PixCast*)(PixelDest);
    ScreenPixB = (DWORD) *(PixCast*)(PixelDest+PixelMinorStep);

    SumAddA = TabPixA + ScreenPixA;   //  sum with carries
    SumAddB = TabPixB + ScreenPixB;   //

    MaskA = ((TabPixA ^ ScreenPixA) ^ SumAddA) & CarryMask; // carryless sums
    MaskB = ((TabPixB ^ ScreenPixB) ^ SumAddB) & CarryMask;

    SumAddA |=  MaskA - (MaskA >> ChannelBits);
    SumAddB |=  MaskB - (MaskB >> ChannelBits);

    *(PixCast*)(PixelDest)                = (PixCast) SumAddA;
    *(PixCast*)(PixelDest+PixelMinorStep) = (PixCast) SumAddB;

    PixelDest += PixelMajorStep;

    Main    -= MainDev;
    Chunk   += ColorDev;

    if( Main < 0 )
    {
        Main      += Chunk;
        MainDev   -= ColorDev;
        PixelDest += PixelMinorStep;
    }

    MainDev += MainDevDev;

    if( ++DebugLoopCnt < FullPixels)
		goto ReLoop;

// do single pixel, TabPixA/B prepared AND position prepared.
SingleLastPix:
    ScreenPixA = (DWORD) *(PixCast*)(PixelDestL);
    ScreenPixB = (DWORD) *(PixCast*)(PixelDestL+PixelMinorStep);

    SumAddA = TabPixAL + ScreenPixA; // sum with carries
    SumAddB = TabPixBL + ScreenPixB; //

    MaskA = ((TabPixAL ^ ScreenPixA) ^ SumAddA) & CarryMask; // carryless sums
    MaskB = ((TabPixBL ^ ScreenPixB) ^ SumAddB) & CarryMask;

    SumAddA |=  MaskA - (MaskA >> ChannelBits);
    SumAddB |=  MaskB - (MaskB >> ChannelBits);

    *(PixCast*)(PixelDestL)                = (PixCast) SumAddA;
    *(PixCast*)(PixelDestL+PixelMinorStep) = (PixCast) SumAddB;

	goto SkipDrawing;

// Dotted drawing start.
// skips all even pixels including the first partial one.

InLoopSkip0:

    if( ++DebugLoopCnt >= FullPixels)
		goto SingleLastPix0;

ReLoop0:
    TabPixA = (DWORD) ColorTab [          (Main)  >> 23 ];
    TabPixB = (DWORD) ColorTab [  (Chunk - Main)  >> 23 ];

    ScreenPixA = (DWORD) *(PixCast*)(PixelDest);
    ScreenPixB = (DWORD) *(PixCast*)(PixelDest+PixelMinorStep);

    SumAddA = TabPixA + ScreenPixA;   //  sum with carries
    SumAddB = TabPixB + ScreenPixB;   //

    MaskA = ((TabPixA ^ ScreenPixA) ^ SumAddA) & CarryMask; // carryless sums
    MaskB = ((TabPixB ^ ScreenPixB) ^ SumAddB) & CarryMask;

    SumAddA |=  MaskA - (MaskA >> ChannelBits);
    SumAddB |=  MaskB - (MaskB >> ChannelBits);

    *(PixCast*)(PixelDest)                = (PixCast) SumAddA;
    *(PixCast*)(PixelDest+PixelMinorStep) = (PixCast) SumAddB;

    PixelDest += PixelMajorStep;

    Main    -= MainDev;
    Chunk   += ColorDev;

    if( Main < 0 )
    {
        Main      += Chunk;
        MainDev   -= ColorDev;
        PixelDest += PixelMinorStep;
    }

    MainDev += MainDevDev;

MidLoop0:
    if( ++DebugLoopCnt >= FullPixels)
		goto SingleLastPix0;

    // Non-drawing step.
    PixelDest += PixelMajorStep;

    Main    -= MainDev;
    Chunk   += ColorDev;

    if( Main < 0 )
    {
        Main      += Chunk;
        MainDev   -= ColorDev;
        PixelDest += PixelMinorStep;
    }

    MainDev += MainDevDev;

    if( ++DebugLoopCnt < FullPixels)
		goto ReLoop0;

SingleLastPix0:
	if( LastNeeded )
		goto SingleLastPix;
	goto SkipDrawing;

// Dotted drawing start.
// skips all odd pixels.

InLoopSkip1:

    ScreenPixA = (DWORD) *(PixCast*)(PixelDestF);
    ScreenPixB = (DWORD) *(PixCast*)(PixelDestF+PixelMinorStep);

    SumAddA = TabPixA + ScreenPixA;   // sum with carries
    SumAddB = TabPixB + ScreenPixB;

    MaskA = ((TabPixA ^ ScreenPixA) ^ SumAddA) & CarryMask; // carryless sums
    MaskB = ((TabPixB ^ ScreenPixB) ^ SumAddB) & CarryMask;

    SumAddA |=  MaskA - (MaskA >> ChannelBits);
    SumAddB |=  MaskB - (MaskB >> ChannelBits);

    *(PixCast*)(PixelDestF)                = (PixCast) SumAddA;
    *(PixCast*)(PixelDestF+PixelMinorStep) = (PixCast) SumAddB;

    goto MidLoop0;

#if ASM

// General inner drawing loop. Draws the first partial pixel pair,
// then all full pixel pairs, then the last partial pair.
// Destinations and pixel values for the start-and endpixels have
// been precomputed.

AsmInLoop:

// force labels to be local.
#define LoopMajorASM LocalTag(LoopMajorASM)
#define IntoMajorASM LocalTag(IntoMajorASM)
#define LastPixelASM LocalTag(LastPixelASM)

__asm
{
    push edi
    push esi
    push ebx
    push ebp

    // plot first pixel.
    mov     edi, PixelDestF
    mov     ebx, PixelMinorStep

    mov     eax,TabPixA ;; get 32-bit shaded color pixel 1
    mov     esi,TabPixB ;; get 32-bit shaded color pixel 2

#if SHIFT == 2
	// use 32-bit regs only for 24/32 bit gfx buffers
    mov     ebp,[edi]     ;; existing pixel 1 from bitmap
    mov     edx,[edi+ebx] ;; existing pixel 2 from bitmap
#else
	// use 16- bit regs for 15/16 bit gfx buffers
    mov     bp,[edi]     ;; existing pixel 1 from bitmap
    mov     dx,[edi+ebx] ;; existing pixel 2 from bitmap
#endif

    nop     ;; mov     MainDev,ecx
    lea     ecx,[eax+ebp]  ;; copy/add color 1

    lea     ebx,[esi+edx]  ;; copy/add color 2
    xor     esi,edx     ;; carry-less sum 1st pixel

    xor     eax,ebp     ;; carry-less sum 2nd pixel
    xor     esi,ebx     ;; diff both 1st pixel results

    xor     eax,ecx     ;; diff both 2nd pixel results
    and     esi,CarryMask

    and     eax,CarryMask
    mov     ebp,esi     ;; copy result pixel 1

    shr     esi,ChannelBits  ;; u  ChannelBits
    mov     edx,eax     ;; copy result pixel 2

    shr     eax,ChannelBits  ;; u  ChannelBits
    sub     ebp,esi     ;; expand masks  1

    sub     edx,eax     ;; expand masks  2
    mov     esi,PixelMinorStep

    or      ebx,ebp     ;; saturate the one we summed earlier
    or      ecx,edx     ;; saturate the one we summed earlier

#if SHIFT == 2  // use 32-bit regs only for 24/32 bit gfx buffers
    mov     [edi+esi],ebx  ;; store....  pixel 2
    mov     ebx,FullPixels
    mov     [edi],ecx      ;; store....  pixel 1
#else
    mov     [edi+esi],bx   ;; store....  pixel 2
    mov     ebx,FullPixels
    mov     [edi],cx       ;; store....  pixel 1
#endif

    mov     ecx,MainDev

    dec     ebx          // decrease FullPixels.
    mov     edx,Chunk

    mov     eax,Main
    mov     edi,PixelDest

    push    ebx
    jnz     IntoMajorASM

    pop     ebx
    jmp     LastPixelASM

    ;;;;;;;;;;;;;;;;;;;;

    // MAIN LOOP.

	align 16

LoopMajorASM:   ; update all the fixed-point vars

    ;;;;    EAX==main (minor subpixel position)   ESI==minorstep

    push    ecx            ;; push FULLPIXELS
    mov     ecx,MainDev    ;;

    mov     ebx,ColorDev
    sub     eax,ecx        ;; MAIN -= MAINDEV

    mov     edx,Chunk      ;; restore CHUNK
    add     eax,0x80000000  ;; make sign into carry (sign flipped!)

    sbb     ebp,ebp        ;; u    log overflow into a 0 / 0xFFFFFFFF mask
    add     edx,ebx        ;; Chunk += ColorDev

    and     esi,ebp        ;; mask Ypitch/PixelMinorStep
    and     ebx,ebp        ;; mask ColorDev!

    and     ebp,edx        ;; mask Chunk but preserve it too
    add     edi,esi        ;; Dest +=PixelInc, += masked- Y step

    sub     ecx,ebx        ;; MainDev -= masked-ColorDev
    mov     esi,PixelMajorStep

    add     eax,ebp        ;; Main    += masked-chunk
    mov     ebp,MainDevDev

    add     edi,esi  ;; Dest + PixelMajorStep
    add     ecx,ebp  ;; Maindev += MainDevDev ;;

    add     eax,0x80000000;   // undo flipped sign from above
    mov     Chunk,edx

IntoMajorASM: ; make sure FullPixels got pushed. ecx==MainDev

    ;;; ecx=MainDev... to store...
    ;;; EAX=main EDX=chunk  EDI=screenbuffer

    sub     edx, eax       ;; chunk-maincolor
    mov     Main,eax      ;; main color

    shr     eax,23        ;; u   get 8 msbits     1st pixel
    mov     esi,ColorTab  ;; can be WORD[] or DWORD[] array ptr !

    shr     edx,23        ;; u   get 8 msbits     2nd pixel
    mov     ebx,PixelMinorStep    ;;

#if SHIFT == 2  // use 32-bit regs only for 24/32 bit gfx buffers

    mov     ebp,[edi]     ;; existing pixel 1 from bitmap ;;;;
    mov     eax,[esi+eax*4] ;; get 32-bit shaded color pixel 1

    mov     ebx,[edi+ebx] ;; existing pixel 2 from bitmap
    mov     esi,[esi+edx*4] ;; get 32-bit shaded color pixel 2

#else // use 16- bit regs for 15/16 bit gfx buffers


    mov      bp,word ptr [edi]        ;; U pipe existing pixel 1 from bitmap
    mov     eax,[esi+eax*4] ;; get 16-bit shaded color pixel 1

    mov      bx,word ptr [edi+ebx]    ;; existing pixel 2 from bitmap
    mov     esi,[esi+edx*4] ;; get 16-bit shaded color pixel 2

#endif
    ;;;;;;;;;;;

    mov     MainDev,ecx
    lea     ecx,[eax+ebp]  ;; copy/add color 1

    lea     edx,[esi+ebx]  ;; copy/add color 2
    xor     esi,ebx     ;; carry-less sum 1st pixel

    xor     eax,ebp     ;; carry-less sum 2nd pixel
    xor     esi,edx     ;; diff both 1st pixel results

    xor     eax,ecx     ;; diff both 2nd pixel results
    and     esi,CarryMask

    and     eax,CarryMask
    mov     ebp,esi     ;; copy result pixel 1

    shr     esi,ChannelBits  ;; u  ChannelBits
    mov     ebx,eax     ;; copy result pixel 2

    shr     eax,ChannelBits  ;; u  ChannelBits
    sub     ebp,esi     ;; expand masks  1

    sub     ebx,eax     ;; expand masks  2
    mov     esi,PixelMinorStep

    or      edx,ebp     ;; saturate the one we summed earlier
    or      ecx,ebx     ;; saturate the one we summed earlier

#if SHIFT == 2  // use 32-bit regs only for 24/32 bit gfx buffers
    mov     [edi],ecx      ;; store....  pixel 1
    pop     ecx
    mov     [edi+esi],edx  ;; store....  pixel 2
#else
    mov     [edi],cx      ;;U store....  pixel 1
    pop     ecx
    mov     [edi+esi],dx  ;;U store....  pixel 2
#endif

    mov     eax,Main       ;; get Main

    dec     ecx
    jnz     LoopMajorASM ;;don't waste last pixel with fixed-point updates!

	;;; last partial pixel: send TabPixAL/BL  to  [PixDestL]

LastPixelASM:

	mov     edi,PixelDestL
	mov     ebx,PixelMinorStep

#if SHIFT == 2  // use 32-bit regs only for 24/32 bit gfx buffers
    mov     ebp,[edi]     ;; U existing pixel 1 from bitmap ;;;;
    mov     eax,TabPixAL ;; get 32-bit shaded color pixel 1

    mov     ebx,[edi+ebx] ;; U existing pixel 2 from bitmap
    mov     esi,TabPixBL ;; get 32-bit shaded color pixel 2
#else // use 16- bit regs for 15/16 bit gfx buffers
    mov     bp,[edi]        ;; U pipe existing pixel 1 from bitmap ;;;;
    mov     eax,TabPixAL ;; get 32-bit shaded color pixel 1

    mov     bx,[edi+ebx]    ;; U existing pixel 2 from bitmap
    mov     esi,TabPixBL ;; get 32-bit shaded color pixel 2
#endif

    ;;;;;;

    nop ;;mov     MainDev,ecx
    lea     ecx,[eax+ebp]  ;; copy/add color 1

    lea     edx,[esi+ebx]  ;; copy/add color 2
    xor     esi,ebx     ;; carry-less sum 1st pixel

    xor     eax,ebp     ;; carry-less sum 2nd pixel
    xor     esi,edx     ;; diff both 1st pixel results

    xor     eax,ecx     ;; diff both 2nd pixel results
    and     esi,CarryMask

    and     eax,CarryMask
    mov     ebp,esi     ;; copy result pixel 1

    shr     esi,ChannelBits  ;; u  ChannelBits
    mov     ebx,eax     ;; copy result pixel 2

    shr     eax,ChannelBits  ;; u  ChannelBits
    sub     ebp,esi     ;; expand masks  1

    sub     ebx,eax     ;; expand masks  2
    mov     esi,PixelMinorStep

    or      edx,ebp     ;; saturate the one we summed earlier
    or      ecx,ebx     ;; saturate the one we summed earlier

#if SHIFT == 2  // use 32-bit regs only for 24/32 bit gfx buffers
    mov     [edi],ecx      ;; store....  pixel 1
    mov     [edi+esi],edx  ;; store....  pixel 2
#else
    mov     [edi],cx      ;;U store....  pixel 1
    nop
    mov     [edi+esi],dx  ;;U store....  pixel 2
#endif

    ;;;;;;;

    pop ebp
    pop ebx
    pop esi
    pop edi
}
#endif

SkipDrawing:;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
