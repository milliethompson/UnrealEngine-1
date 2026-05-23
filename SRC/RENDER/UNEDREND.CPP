/*=============================================================================
	UnEdRend.cpp: Unreal editor rendering functions.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"
#include "UnRaster.h"

#define LINE_NEAR_CLIP_Z 1.0
#define ORTHO_LOW_DETAIL 40000.0
#define AA 0

/*-----------------------------------------------------------------------------
	Line drawing routines.
-----------------------------------------------------------------------------*/

//
// Convenience macros:
//
#define BLEND(Dest,Color)  *(Dest)=GGfx.Blender[(DWORD)*(Dest) + (((DWORD)Color) << 8)]
#define LOGADD(Dest,Color) *(Dest)=GGfx.LogAdd [(DWORD)*(Dest) + (((DWORD)Color) << 8)]
#define SHADE(Color,Shade) GGfx.Shader[Color + (Shade<<8)]

#define MAKELABEL(A,B,C,D) A##B##C##D

//
// Draw a regular line:
//
void FRender::DrawLine
(
	UCamera	*Camera,
	BYTE	Color,
	INT		Dotted,
	FLOAT	X1,
	FLOAT	Y1, 
	FLOAT	G1,
	FLOAT	X2,
	FLOAT	Y2,
	FLOAT	G2
)
{
#if AA
	DrawDepthLine( Camera, Color, Dotted, X1, Y1, G1, X2, Y2, G2 );
#else
	INT NewColor = Color-16;
	if (Camera->ColorBytes==1)
	{
		#define DRAWPIXEL(Dest)  *(Dest)=NewColor
		#define ASMPIXEL		 mov [edi],al
		#define SHIFT 0
		#define LABEL1(X) X##C1
		#include "UnLine1.cpp"
		#undef  LABEL1
		#undef  SHIFT
		#undef  DRAWPIXEL
		#undef  ASMPIXEL
	}
	else if (Camera->ColorBytes==2)
	{
		WORD HiColor;
		GGfx.DefaultPalette->Lock(LOCK_Read);
		if (Camera->Caps & CC_RGB565)	HiColor = GGfx.DefaultPalette(NewColor).HiColor565();
		else							HiColor = GGfx.DefaultPalette(NewColor).HiColor555();
		GGfx.DefaultPalette->Unlock(LOCK_Read);

		#define DRAWPIXEL(Dest)  *(WORD *)(Dest)=HiColor
		#define SHIFT 1
		#define LABEL1(X) X##C2
		#include "UnLine1.cpp"
		#undef  LABEL1
		#undef  SHIFT
		#undef  DRAWPIXEL
	}
	else
	{
		GGfx.DefaultPalette->Lock(LOCK_Read);
		DWORD TrueColor = GGfx.DefaultPalette(NewColor).TrueColor();
		GGfx.DefaultPalette->Unlock(LOCK_Read);
		#define DRAWPIXEL(Dest)  *(DWORD *)(Dest)=TrueColor
		#define SHIFT 2
		#define LABEL1(X) X##C4
		#include "UnLine1.cpp"
		#undef  LABEL1
		#undef  SHIFT
		#undef  DRAWPIXEL
	}
#endif
}

//
// Draw a depth-shaded line
//
#if !AA
void FRender::DrawDepthLine
(
	UCamera		*Camera, 
	BYTE		Color, 
	INT			Dotted,
	FLOAT		X1, 
	FLOAT		Y1, 
	FLOAT		RZ1, 
	FLOAT		X2, 
	FLOAT		Y2, 
	FLOAT		RZ2
)
{
	INT NewColor = Color-16;
	FCacheItem *Item;
	if( Camera->ColorBytes == 1 )
	{
		#define DRAWPIXEL(Dest) *(Dest)=NewColor + (Unfix(FixG1 += FixDG)<<3);
		#define ASMPIXEL \
			__asm{mov ebx,eax}\
			__asm{mov edx,[FixDG]}\
			__asm{shr ebx,13}\
			__asm{add eax,edx}\
			__asm{and ebx,0xf8}\
			__asm{mov edx,[NewColor]}\
			__asm{add ebx,edx}\
			__asm{mov [edi],bl}
		#define DEPTHSHADE
		#define SHIFT 0
		#define LABEL1(X) X##D1
		#include "UnLine1.cpp"
		#undef  LABEL1
		#undef  SHIFT
		#undef  DEPTHSHADE
		#undef  DRAWPIXEL
		#undef  ASMPIXEL
	}
	else if( Camera->ColorBytes == 2 )
	{
		WORD *HiColor = GGfx.DefaultPalette->GetColorDepthPalette(Item,Camera->Texture).PtrWORD;
		#define DRAWPIXEL(Dest) *(WORD *)(Dest)=HiColor[NewColor+(Unfix(FixG1 += FixDG)<<3)];
		#define DEPTHSHADE
		#define SHIFT 1
		#define LABEL1(X) X##D2_565
		#include "UnLine1.cpp"
		#undef  LABEL1
		#undef  SHIFT
		#undef  DEPTHSHADE
		#undef  DRAWPIXEL
		Item->Unlock();
	}
	else
	{
		DWORD *TrueColor = GGfx.DefaultPalette->GetColorDepthPalette(Item,Camera->Texture).PtrDWORD;
		#define DRAWPIXEL(Dest)    *(DWORD *)(Dest)=TrueColor[NewColor+(Unfix(FixG1 += FixDG)<<3)];
		#define DEPTHSHADE
		#define SHIFT 2
		#define LABEL1(X) X##D4
		#include "UnLine1.cpp"
		#undef  SHIFT
		#undef  LABEL1
		#undef  DEPTHSHADE
		#undef  DRAWPIXEL
		Item->Unlock();
	}
}
#endif

/*-----------------------------------------------------------------------------
	Antialiased line drawing.
-----------------------------------------------------------------------------*/

// Floating point library support for antialiased lines.
#include "aafllib.h"
#pragma warning( disable : 4102 )

// Required globals.
FLOAT   LumAdjustX [ 256 ];
FLOAT   LumAdjustY [ 256 ];

// CRT horizontal bias ( 0 on LCD's).
#define CRTBIAS  0.25F

// Gives stronger intensity surpression of horizontal-major-axis
// lines as slope goes down.
// 0.30 ..0.15  is a useful range for CRT's ,
// 0.0 for LCD's  or very advanced (?) crt monitors..

//
// Initialize the custom float2fixed point library and
// intensity correction tables.
//
void LineDrawInit()
{
    // #DEBUG
    // FPU routines init
    // makes sure the right rounding/fractionizing constants are set,
    // depending on current FPU accuracy status (must be 64 or 53 bits)
#if ASM
	InitFlLib();
#endif

    // Direction-dependent intensity correction setup.
    for( int Lum = 0; Lum < 256; Lum++ )
    {
        // Diagonal length correction fix.
        float LumScaled = (float)Lum / 255.f;
        float ThisLumY  = sqrt( 1.0F +  Square(LumScaled)  )* 0.7071067F;
        float ThisLumX  = ThisLumY  * ( 1.0F - (CRTBIAS * (1.0F - LumScaled)) );

        // Index is 0-255 == the slope, from  255*(MinorLen/MajorLen).
        LumAdjustY[Lum] = ThisLumY;
        LumAdjustX[Lum] = ThisLumX;
    }
}


//
// Antialiased, additive blended, dotted & nondotted,  15,16,32 bit depth.
//
#if AA
void FRender::DrawDepthLine
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
{
	static int Inited=0;
	GApp->EnableFastMath(0);
	if( !Inited) { Inited=1; LineDrawInit(); }

	X1 += 0.5; X2 += 0.5;
	Y1 += 0.5; Y2 += 0.5;

	// Check boundaries - NOTE +1 pixel stricter bound required compared
	//                    to the old code because of double-pixel AAliased lines.
	//                  - Strict checks for RZ1/RZ2 color range added.
	//  #DEBUG   These checks may be obsolete if the preparations
	//           in  FGlobalRender::Draw3DLine  are reliable enough.
	if
	(	(X1 <  0.0         ) || (X2 < 0.0          )
	||	(Y1 <  0.0         ) || (Y2 < 0.0          )
	||	(X1 >= Camera->FX-1) || (X2 >= Camera->FX-1)
	||	(Y1 >= Camera->FY-1) || (Y2 >= Camera->FY-1) )
		return;

	static DWORD *ColorTab;
	DWORD CacheID = MakeCacheID( CID_AALineTable, Camera->ColorBytes, Camera->Caps, Color );
	FCacheItem *Item;
	ColorTab = (DWORD*)GCache.Get( CacheID, Item );
	if( ColorTab == NULL )
	{
		ColorTab = (DWORD*)GCache.Create( CacheID, Item, 256*4 );
		FVector C = GGfx.DefaultPalette(Color).Vector();
		FLOAT GammaCorrection=0.56;
		for( int i=0; i<256; i++ )
		{
			FLOAT GFactor  = pow( (float)i/256.0F , GammaCorrection );
			if( Camera->ColorBytes==2 && (Camera->Caps & CC_RGB565) )
				ColorTab[i]
				=	( ftoi(C.B * GFactor * 0.125f) << 0 )
				+	( ftoi(C.G * GFactor * 0.250f) << 5 )
				+	( ftoi(C.R * GFactor * 0.125f) << 11);
			else if( Camera->ColorBytes==2 )
				ColorTab[i]
				=	( ftoi(C.B * GFactor * 0.125f) << 0 )
				+	( ftoi(C.G * GFactor * 0.125f) << 5 )
				+	( ftoi(C.R * GFactor * 0.125f) << 10);
			else if( Camera->ColorBytes==4 )
				ColorTab[i]
				=	( ftoi(C.B * GFactor) << 0 )
				+	( ftoi(C.G * GFactor) << 8 )
				+	( ftoi(C.R * GFactor) << 16);
		}
	}

	//  #DEBUG  Adapt the code to get 32, 15 or 16 bit depth from Camera
	//
	//  eg.   replace ColorBits  with   Camera->ColorBytes
	//
	//
	//  #DEBUG  ColorRamp needs to resolve to a pointer to a color array
	//          with element size 'ColCast'.
	//          Which table is selected depends on both color depth and
	//          the number passed in the 'Color' parameter.
	//          For 15 and 16 bit modes, both 16 and 32-bit color tables
	//          can be used; just set the 'ColCast' type WORD or DWORD.

	if( Camera->ColorBytes==2 && (Camera->Caps & CC_RGB565) )
	{
		// HiColor 5:6:5
		#define     LocalTag(n) n##Hi565     /* ID to localize labels                */
		#define     SHIFT 1                  /* destination address shift            */
		#define     CarryMask   0x00010820
		#define     ChannelBits 5            /* bits per color channel               */
		#define     PixCast     WORD         /* data type for pixel destination      */
		#include    "AALineDo.h"
		#undef      PixCast
		#undef      ChannelBits
		#undef      CarryMask
		#undef      SHIFT
		#undef      LocalTag
	}
	else if( Camera->ColorBytes==2 )
	{
		// HiColor 5:5:5
		#define     LocalTag(n) n##Hi555     /* ID to localize labels                */
		#define     SHIFT 1                  /* destination address shift            */
		#define     CarryMask   0x00008420
		#define     ChannelBits 5            /* bits per color channel               */
		#define     PixCast     WORD         /* data type for pixel destination      */
		#include    "AALineDo.h"
		#undef      PixCast
		#undef      ChannelBits
		#undef      CarryMask
		#undef      SHIFT
		#undef      LocalTag
    }
    else if( Camera->ColorBytes==4 )
    {
		// TrueColor 8:8:8:8  32-bit
		#define     LocalTag(n) n##Tru32     /* ID to localize labels                 */
		#define     SHIFT 2                  /* destination address shift             */
		#define     CarryMask   0x01010100
		#define     ChannelBits 8            /* bits per color channel                */
		#define     PixCast     DWORD        /* data type for pixel destination       */
		#include    "AALineDo.h"
		#undef      PixCast
		#undef      ChannelBits
		#undef      CarryMask
		#undef      SHIFT
		#undef      LocalTag
	}
	Item->Unlock();
}
#endif

/*-----------------------------------------------------------------------------
	Low-level graphics drawing primitives.
-----------------------------------------------------------------------------*/

//
// Draw a clipped rectangle, assumes X1<X2 and Y1<Y2:
//
void FRender::DrawRect( UCamera *Camera, BYTE Color, int X1, int Y1, int X2, int Y2 )
{
	guard(FRender::DrawRect);

	if ((X2<0)||(Y2<0)) return;

	const int SXR      = Camera->X;
	const int SYR      = Camera->Y;
	const int SXStride = Camera->Stride;

	if ((X1>=SXR) || (Y1>=SYR)) return;

	BYTE *Dest1,*Dest;
	int X,Y,XL,YL;

	if (X1<0)				X1=0;
	if (Y1<0)				Y1=0;
	if (++X2>Camera->X)	X2=Camera->X; 
	if (++Y2>Camera->Y)	Y2=Camera->Y;

	Color -= 16;

	YL     = Y2-Y1;
	XL     = X2-X1;

	if( Camera->ColorBytes==1 )
	{
		Dest1 = &Camera->Screen[X1 + Y1*SXStride];
#if ASM
		__asm
		{
			mov edi,[Dest1]		// Destination address
			mov ecx,[SXStride]	// Screen resolution
			mov edx,[YL]		// Number of lines to draw
			mov ebx,[XL]		// Loop counter
			mov al, [Color]		// Color
			sub ecx,ebx			// Stride skip

			ALIGN 16
			Outer:				// Outer loop entry point
			Inner:				// Inner loop entry point
			mov [edi],al		// Store color onto screen
			inc edi				// Go to next screen pixel
			dec ebx				// Next pixel
			jg  Inner

			add edi,ecx			// Skip SXR-XL pixels
			mov ebx,[XL]		// Get inner loop counter
			dec edx				// Next line
			jg  Outer
		}
		#else
		for( Y=0; Y<YL; Y++ )
		{
			Dest = Dest1;
			for (X=0; X<XL; X++) *Dest++ = Color;
			Dest1 += SXStride;
		}
		#endif
	}
	else if( Camera->ColorBytes==2 )
	{
		WORD HiColor;
		GGfx.DefaultPalette->Lock(LOCK_Read);
		if (Camera->Caps & CC_RGB565)	HiColor = GGfx.DefaultPalette(Color).HiColor565();
		else							HiColor = GGfx.DefaultPalette(Color).HiColor555();
		GGfx.DefaultPalette->Unlock(LOCK_Read);

		Dest1 = &Camera->Screen[(X1 + Y1*SXStride)*2];
		for( Y=0; Y<YL; Y++ )
		{
			Dest = Dest1;
			for( X=0; X<XL; X++ )
			{
				*(WORD *)Dest  = HiColor;
				Dest          += 2;
			}
			Dest1 += SXStride << 1;
		}
	}
	else
	{
		GGfx.DefaultPalette->Lock(LOCK_Read);
		DWORD TrueColor = GGfx.DefaultPalette(Color).TrueColor();
		GGfx.DefaultPalette->Unlock(LOCK_Read);
		Dest1 = &Camera->Screen[(X1 + Y1 * SXStride) << 2];
		for( Y=0; Y<YL; Y++ )
		{
			Dest = Dest1;
			for( X=0; X<XL; X++ )
			{
				*(DWORD *)Dest  = TrueColor;
				Dest           += 4;
			}
			Dest1 += SXStride << 2;
		}
	}
	unguard;
}

//
// Draw a circle.
//
void FRender::DrawCircle
(
	UCamera *Camera,
	FVector &Location,
	FLOAT	Radius,
	INT		Color,
	INT		Dotted
)
{
	FLOAT F = 0.0;
	FVector A,B,P1,P2;
	if	   ( Camera->Actor->RendMap==REN_OrthXY )	{A=FVector(1,0,0); B=FVector(0,1,0);}
	else if( Camera->Actor->RendMap==REN_OrthXZ )	{A=FVector(1,0,0); B=FVector(0,0,1);}
	else											{A=FVector(0,1,0); B=FVector(0,0,1);};

	Dotted=0;
	int Subdivide=8;
	for( FLOAT Thresh = Camera->Actor->OrthoZoom/Radius; Thresh<2048 && Subdivide<256; Thresh *= 2,Subdivide*=2 );

	P1 = Location + Radius * (A * cos(F) + B * sin(F));
	for( int i=0; i<Subdivide; i++ )
	{
		F += 2.0*PI / Subdivide;
		P2 = Location + Radius * (A * cos(F) + B * sin(F));
		DrawOrthoLine( Camera, P1, P2, Color, Dotted, 1.0 );
		P1 = P2;
	}
}

//
// Draw a box centered about a location.
//
void FRender::DrawBox
(
	UCamera			*Camera,
	const FVector	&Min,
	const FVector	&Max,
	const INT	    Color,
	const INT		Dotted 
)
{
	guard(FRender::DrawBox);

	FVector A,B;
	FVector Location = Min+Max;
	if	   ( Camera->Actor->RendMap==REN_OrthXY )	{A=FVector(Max.X-Min.X,0,0); B=FVector(0,Max.Y-Min.Y,0);}
	else if( Camera->Actor->RendMap==REN_OrthXZ )	{A=FVector(Max.X-Min.X,0,0); B=FVector(0,0,Max.Z-Min.Z);}
	else											{A=FVector(0,Max.Y-Min.Y,0); B=FVector(0,0,Max.Z-Min.Z);}

	DrawOrthoLine( Camera, (Location+A+B)/2, (Location+A-B)/2, Color, Dotted, 1.0 );
	DrawOrthoLine( Camera, (Location-A+B)/2, (Location-A-B)/2, Color, Dotted, 1.0 );
	DrawOrthoLine( Camera, (Location+A+B)/2, (Location-A+B)/2, Color, Dotted, 1.0 );
	DrawOrthoLine( Camera, (Location+A-B)/2, (Location-A-B)/2, Color, Dotted, 1.0 );

	unguard;
}

/*-----------------------------------------------------------------------------
	Misc.
-----------------------------------------------------------------------------*/

//
// Clip a line in an orthogonal view and return 1 if the line is visible,
// 2 if it's visible as a point (parallel to line of sight), or 0 if it's obscured.
//
int FRender::OrthoClip
(
	UCamera			*Camera,
	const FVector	&P1, 
	const FVector	&P2,
	FLOAT			*ScreenX1, 
	FLOAT			*ScreenY1, 
	FLOAT			*ScreenX2, 
	FLOAT			*ScreenY2
)
{
	guard(FRender::OrthoClip);

	const FVector *Origin = &Camera->Coords.Origin;
	FLOAT   X1,X2,Y1,Y2,Temp;
	FLOAT   SX = Camera->FX-1, SY=Camera->FY-1;
	int     Status=1;

	// Get unscaled coordinates for whatever axes we're using.
	switch( Camera->Actor->RendMap )
	{
		case REN_OrthXY:
			X1=P1.X - Origin->X; Y1=P1.Y - Origin->Y;
			X2=P2.X - Origin->X; Y2=P2.Y - Origin->Y;
			break;
		case REN_OrthXZ:
			X1=P1.X - Origin->X; Y1=Origin->Z - P1.Z;
			X2=P2.X - Origin->X; Y2=Origin->Z - P2.Z;
			break;
		case REN_OrthYZ:
			X1=P1.Y - Origin->Y; Y1=Origin->Z - P1.Z;
			X2=P2.Y - Origin->Y; Y2=Origin->Z - P2.Z;
			break;
		default:
			appError ("OrthoClip: Bad RendMap");
			return 0;
	}

	// See if points for a line that's parallel to our line of sight (i.e. line appears as a dot).
	if( (Abs(X2-X1)+Abs(Y1-Y2))<0.2 )
		Status=2;

	// Zoom.
	X1 = (X1 * Camera->RZoom) + Camera->FX2;
	X2 = (X2 * Camera->RZoom) + Camera->FX2;
	Y1 = (Y1 * Camera->RZoom) + Camera->FY2;
	Y2 = (Y2 * Camera->RZoom) + Camera->FY2;

	// X-Clip.
	if( X1 > X2 )
	{
		// Arrange so X1<X2.
		Temp = X1; X1 = X2; X2 = Temp;
		Temp = Y1; Y1 = Y2; Y2 = Temp;
	}
	if( X2<0 || X1>SX )
		return 0;
	if( X1<0 )
	{
		if( Abs(X2-X1)<0.001 )
			return 0;
		Y1 += (0-X1)*(Y2-Y1)/(X2-X1);
		X1  = 0;
	}
	if( X2>=SX )
	{
		if( Abs(X2-X1)<0.001 )
			return 0;
		Y2 += ((SX-1.0)-X2)*(Y2-Y1)/(X2-X1);
		X2  = SX-1.0;
	}

	// Y-Clip.
	if( Y1 > Y2 )
	{
		// Arrange so Y1<Y2.
		Temp=X1; X1=X2; X2=Temp;
		Temp=Y1; Y1=Y2; Y2=Temp;
	}
	if( Y2 < 0 || Y1 > SY )
		return 0;
	if( Y1 < 0 )
	{
		if( Abs(Y2-Y1)<0.001 )
			return 0;
		X1 += (0-Y1)*(X2-X1)/(Y2-Y1);
		Y1  = 0;
	}
	if( Y2 >= SY )
	{
		if( Abs(Y2-Y1)<0.001 )
			return 0;
		X2 += ((SY-1.0)-Y2)*(X2-X1)/(Y2-Y1);
		Y2  = SY-1.0;
	}

	// Return.
	*ScreenX1=X1;
	*ScreenY1=Y1;
	*ScreenX2=X2;
	*ScreenY2=Y2;

	return Status;
	unguard;
}

//
// Figure out the unclipped screen location of a 3D point taking into account either
// a perspective or orthogonal projection.  Returns 1 if view is orthogonal or point 
// is visible in 3D view, 0 if invisible in 3D view (behind the viewer).
//
// Scale = scale of one world unit (at this point) relative to screen pixels,
// for example 0.5 means one world unit is 0.5 pixels.
//
int FRender::Project(UCamera *Camera, FVector *V, FLOAT *ScreenX, FLOAT *ScreenY, FLOAT *Scale)
{
	guard(FRender::Project);

	FVector	Temp = *V - Camera->Coords.Origin;
	if( Camera->Actor->RendMap == REN_OrthXY )
	{
		*ScreenX = +Temp.X * Camera->RZoom + Camera->FX2;
		*ScreenY = +Temp.Y * Camera->RZoom + Camera->FY2;
		if( Scale )
			*Scale = Camera->RZoom;
		return 1;
	}
	else if (Camera->Actor->RendMap==REN_OrthXZ)
	{
		*ScreenX = +Temp.X * Camera->RZoom + Camera->FX2;
		*ScreenY = -Temp.Z * Camera->RZoom + Camera->FY2;
		if( Scale )
			*Scale = Camera->RZoom;
		return 1;
	}
	else if (Camera->Actor->RendMap==REN_OrthYZ)
	{
		*ScreenX = +Temp.Y * Camera->RZoom + Camera->FX2;
		*ScreenY = -Temp.Z * Camera->RZoom + Camera->FY2;
		if( Scale )
			*Scale = Camera->RZoom;
		return 1;
	}
	else
	{
		Temp     = Temp.TransformVectorBy( Camera->Coords );
		FLOAT Z  = Temp.Z; if (Abs (Z)<0.01) Z+=0.02;
		FLOAT RZ = Camera->ProjZ / Z;
		*ScreenX = Temp.X * RZ + Camera->FX2;
		*ScreenY = Temp.Y * RZ + Camera->FY2;

		if( Scale  )
			*Scale = RZ;

		return Z > 1.0;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
   Screen to world functions (inverse projection).
-----------------------------------------------------------------------------*/

//
// Convert a particular screen location to a world location.  In ortho views,
// sets non-visible component to zero.  In persp views, places at camera location
// unless UseEdScan=1 and the user just clicked on a wall (a Bsp polygon).
// Sets V to location and returns 1, or returns 0 if couldn't perform conversion.
//
int FRender::Deproject(UCamera *Camera,int ScreenX,int ScreenY,FVector *V,int UseEdScan,FLOAT Radius)
	{
	guard(FRender::Deproject);
	//
	FVector	*Origin = &Camera->Coords.Origin;
	FLOAT	SX		= (FLOAT)ScreenX - Camera->FX2;
	FLOAT	SY		= (FLOAT)ScreenY - Camera->FY2;
	//
	switch (Camera->Actor->RendMap)
		{
		case REN_OrthXY:
			V->X = +SX / Camera->RZoom + Origin->X;
			V->Y = +SY / Camera->RZoom + Origin->Y;
			V->Z = 0;
			return 1;
		case REN_OrthXZ:
			V->X = +SX / Camera->RZoom + Origin->X;
			V->Y = 0.0;
			V->Z = -SY / Camera->RZoom + Origin->Z;
			return 1;
		case REN_OrthYZ:
			V->X = 0.0;
			V->Y = +SX / Camera->RZoom + Origin->Y;
			V->Z = -SY / Camera->RZoom + Origin->Z;
			return 1;
		default: // 3D view
			if (UseEdScan && GEditor)
				{
				FBspNode	*Node;
				FBspSurf	*Poly;
				FVector		*PlaneBase,*PlaneNormal;
				FVector		SightVector,SightX,SightY,SightDest;
				//
				if (GEditor->Scan.Type==EDSCAN_BspNodePoly)
					{
					UModel *Model	= Camera->Level->Model;
					Node  			= &Model->Nodes  (GEditor->Scan.Index);
					Poly  			= &Model->Surfs  (Node->iSurf);
					PlaneBase 		= &Model->Points (Poly->pBase);
					PlaneNormal		= &Model->Vectors(Poly->vNormal);
					//
					// Find line direction vector of line-of-sight starting at camera
					// location:
					//
					SightVector = Camera->Coords.ZAxis;
					SightX      = Camera->Coords.XAxis * SX * Camera->RProjZ;
					SightY      = Camera->Coords.YAxis * SY * Camera->RProjZ;
					//
					SightVector += SightX;
					SightVector += SightY;
					SightDest    = *Origin + SightVector;
					//
					// Find intersection of line-of-sight and plane:
					//
					*V = FLinePlaneIntersection (*Origin,SightDest,*PlaneBase,*PlaneNormal);
					*V += *PlaneNormal * Radius; // Move destination point out of plane:
					//
					return 1;
					};
				}
			else
				{
				*V = *Origin;
				};
			return 0; // 3D not supported yet
		};
	unguard;
	};

/*-----------------------------------------------------------------------------
   High-level graphics primitives.
-----------------------------------------------------------------------------*/

void FRender::DrawOrthoLine
(
	UCamera			*Camera,
	const FVector	&P1,
	const FVector	&P2,
	INT				Color,
	INT				Dotted,
	FLOAT			Brightness
)
{
	guard(FRender::DrawOrthoLine);

	FLOAT   X1,Y1,X2,Y2;
	int		Status;

	Status = OrthoClip( Camera, P1, P2, &X1, &Y1, &X2, &Y2 );

	if( Status==1 )
	{
		// Line is visible as a line.
		DrawLine( Camera, Color, Dotted, X1, Y1, Brightness, X2, Y2, Brightness );
	}
	else if( Status==2 )
	{
		// Line is visible as a point.
		if( Camera->Actor->OrthoZoom < ORTHO_LOW_DETAIL )
			DrawRect( Camera, Color, X1-1, Y1-1, X1+1, Y1+1 );
	}
	unguard;
}

void FRender::Draw3DLine
(
	UCamera			*Camera,
	const FVector	&OrigP,
	const FVector	&OrigQ,
	int				MustTransform, 
	int				Color,
	int				DepthShade,
	int				Dotted
)
{
	guard(FRender::Draw3DLine);

	FLOAT   SX	= Camera->FX-1;
	FLOAT	SY  = Camera->FY-1;
	FLOAT   SX2 = Camera->FX2;
	FLOAT	SY2 = Camera->FY2;
	FLOAT   X1,Y1,RZ1,X2,Y2,RZ2;
	FVector P,Q;
	FLOAT	Temp,Alpha;

	if( Camera->IsOrtho() )
	{
		DrawOrthoLine( Camera, OrigP, OrigQ, Color, Dotted, 1.0 );
		return;
	}

	P = OrigP; 
	Q = OrigQ;
	if( MustTransform )
	{
		// Transform into screenspace.
   		P = P.TransformPointBy( Camera->Coords );
   		Q = Q.TransformPointBy( Camera->Coords );
	}

	// Calculate delta, discard line if points are identical.
	FVector D = Q-P;

	if( D.X<0.01 && D.X>-0.01
	&&	D.Y<0.01 && D.Y>-0.01
	&&	D.Z<0.01 && D.Z>-0.01 )
	{
		return; // Same point, divide would fail.
	}

	// Clip to near clipping plane.
	if( P.Z <= LINE_NEAR_CLIP_Z )
	{
		// Clip P to NCP.
		if( Q.Z<(LINE_NEAR_CLIP_Z-0.01) )
			// Prevent divide by zero when P-Q is tiny.
			return;
		P.X +=  (LINE_NEAR_CLIP_Z-P.Z) * D.X/D.Z;
		P.Y +=  (LINE_NEAR_CLIP_Z-P.Z) * D.Y/D.Z;
		P.Z  =  (LINE_NEAR_CLIP_Z);
	}
	else if( Q.Z<(LINE_NEAR_CLIP_Z-0.01) )
	{
		// Clip Q to NCP.
		Q.X += (LINE_NEAR_CLIP_Z-Q.Z) * D.X/D.Z;
		Q.Y += (LINE_NEAR_CLIP_Z-Q.Z) * D.Y/D.Z;
		Q.Z =  (LINE_NEAR_CLIP_Z);
	}

	// Calculate perspective.
	RZ1 = 1.0/P.Z; X1=P.X * Camera->ProjZ * RZ1 + SX2; Y1=P.Y * Camera->ProjZ * RZ1 + SY2; 
	RZ2 = 1.0/Q.Z; X2=Q.X * Camera->ProjZ * RZ2 + SX2; Y2=Q.Y * Camera->ProjZ * RZ2 + SY2; 

	// Arrange for X-clipping
	if( X2 < X1 )
	{
		// Flip so X2>X1.
		Temp=X1;  X1 =X2;  X2 =Temp;
		Temp=Y1;  Y1 =Y2;  Y2 =Temp;
		Temp=RZ1; RZ1=RZ2; RZ2=Temp;
	}
	else if( X2-X1 < 0.01 )
	{
		// Special case vertical line.
		if( X1<0 || X1>=SX )
			return;

		if( Y1<0 )
		{
			if( Y2<0 )
				return;
			if( DepthShade )
			{			
				Alpha = (0-Y1)/(Y2-Y1);
				RZ1   = RZ1 + Alpha * (RZ2-RZ1);
			}
			Y1=0;
		}
		else if( Y1>=SY )
		{
			if( Y2>=SY )
				return;
			if( DepthShade )
			{			
				Alpha = (SY-1-Y1)/(Y2-Y1);
				RZ1   = RZ1 + Alpha * (RZ2-RZ1);
			}
			Y1=SY-1;
		}
		if( Y2<0 )
		{
			if( DepthShade )
			{			
				Alpha = (0-Y1)/(Y2-Y1);
				RZ2   = RZ1 + Alpha * (RZ2-RZ1);
			}
      		Y2=0;
      	}
		else if( Y2 >= SY )
		{
			if( DepthShade )
			{			
				Alpha = (SY-1-Y1)/(Y2-Y1);
				RZ2   = RZ1 + Alpha * (RZ2-RZ1);
			}
			Y2=SY-1;
		}
		goto Draw;
	}

	// X-clip it.  X2>X1.
	if( X2<0.0 || X1>=SX )
		return;

	if( X1<0 )
	{
		// Bound X1 and calculate new Y1 for later Y-clipping.
		Alpha = (0-X1)/(X2-X1);
		Y1    = Y1 + Alpha * (Y2-Y1);

		if( DepthShade )
			RZ1 = RZ1 + Alpha * (RZ2-RZ1);

		X1=0;
	}
	if( X2>=SX )
	{
		// Bound X2 and calculate new Y2 for later Y-clipping.
		Alpha = (SX-1-X1)/(X2-X1);
		Y2    = Y1 + Alpha * (Y2-Y1);

		if( DepthShade )
			RZ2 = RZ1 + Alpha * (RZ2-RZ1);

		X2 = SX-1;
	}

	// Arrange for Y-clipping.
	if( Y2<Y1 )
	{
		// Flip so Y2>Y1.
		Temp=X1;  X1=X2;   X2=Temp;
		Temp=Y1;  Y1=Y2;   Y2=Temp;
		Temp=RZ1; RZ1=RZ2; RZ2=Temp;
	}
	else if( Y2-Y1 < 0.01 )
	{
		// -0.01 to 0.01 = horizontal line.
		// Special case horizontal line (already x-clipped).
		if( Y1<0.0 || Y2>=SY )
			return;
		else goto Draw;
	}

	// Y-clip it.  Y2>Y1.
	if( Y2<0 || Y1>=SY )
		return;

	if( Y1<0 )
	{
		// Bound Y1 and calculate new X1, discard if out of range.
		Alpha = (0-Y1)/(Y2-Y1);
		X1    = X1 + Alpha * (X2-X1);

		if 	   ( X1<0 || X1>=SX ) 	return;
		else if( DepthShade     ) 	RZ1 = RZ1 + Alpha * (RZ2-RZ1);

		Y1=0;
	}
	if( Y2>=SY )
	{
		// Bound Y2 and calculate new X2, discard if out of range.
		Alpha 	= (SY-1-Y1)/(Y2-Y1);
		X2       = X1 + Alpha * (X2-X1);

		if 	   ( X2<0 || X2>=SX ) 	return;
		else if( DepthShade     ) 	RZ2 = RZ1 + Alpha * (RZ2-RZ1);

		Y2=SY-1;
	}

	// Draw it.
	Draw:
	if( DepthShade==0 )
	{
		DrawLine( Camera, Color, Dotted, X1, Y1, 1.0, X2, Y2, 1.0 );
	}
	else if( DepthShade==1 )
	{
		DrawDepthLine
		(
			Camera,
			Color,
			Dotted,
			X1,
			Y1,
			Clamp( RZ1 * 400.0, 0.01, 0.99 ),
			X2,
			Y2,
			Clamp( RZ2 * 400.0, 0.01, 0.99 )
		);
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Edpoly drawers.
-----------------------------------------------------------------------------*/

//
// Draw an editor polygon
//
void FRender::DrawFPoly(UCamera *Camera, FPoly *EdPoly, int WireColor, int FillColor,int Dotted)
{
	guard(FRender::DrawFPoly);

	const FVector	*Origin = &Camera->Coords.Origin;
	FVector			*Verts	= &EdPoly->Vertex[0];
	FVector			VPoly,*V1,*V2;
	INDEX			NumPts,i;

	if( Camera->IsOrtho() )
	{
		// Orthogonal view.
		NumPts	= EdPoly->NumVertices;
		V1		= &EdPoly->Vertex[0];
		V2      = &EdPoly->Vertex[EdPoly->NumVertices-1];
		for( i=0; i<NumPts; i++ )
		{
			if ((EdPoly->PolyFlags & PF_NotSolid) || (V1->X >= V2->X))
			{
        		DrawOrthoLine( Camera, *V1, *V2, WireColor, Dotted, 1.0 );
			}
			V2=V1++;
		}
	}
	else
	{
		// Perspective view.
		if( FillColor && !(EdPoly->PolyFlags & PF_NotSolid) )
		{
			// Backface rejection.
			VPoly.X = Verts[0].X - Origin->X;
			VPoly.Y = Verts[0].Y - Origin->Y;
			VPoly.Z = Verts[0].Z - Origin->Z;
			if(
	        (	VPoly.X*EdPoly->Normal.X
			+	VPoly.Y*EdPoly->Normal.Y
			+	VPoly.Z*EdPoly->Normal.Z) >= 0.0 )
			{
				// Backfaced: Normal and view vector are facing same way.
				return;
			}
		}
		V1	= &EdPoly->Vertex[0];
		V2	= &EdPoly->Vertex[EdPoly->NumVertices-1];
		for( i=0; i<EdPoly->NumVertices; i++ )
		{
			if( (EdPoly->PolyFlags & PF_NotSolid) || (V1->X >= V2->X) )
				Draw3DLine( Camera, *V1, *V2, 1, WireColor, 0, Dotted );

			V2 = V1++;
		}
	}
	unguard;
}

//
// Draw a brush (with rotation/translation):
//
void FRender::DrawBrushPolys
(
	UCamera			*Camera,
	UModel			*Brush, 
	int				WireColor, 
	int				Dotted, 
	FConstraints	*Constraints, 
	int				DrawPivot,
	int				DrawVertices, 
	int				DrawSelected, 
	int				DoScan
)
{
	guard(FRender::DrawBrushPolys);
	FMemMark Mark(GMem);

	// See if we can reject the brush.
	if( Brush->TransformedBound.IsValid )
		if( !BoundVisible( Camera, &Brush->TransformedBound, NULL, NULL ) )
			return;

	APawn		*Actor = Camera->Actor;
	FCoords     Coords;
	FVector		Vertex,*VertPtr,*V1,*V2;
	FPoly       *TransformedEdPolys;
	FLOAT       X,Y;
	BYTE		DrawColor,VertexColor,PivColor;
	INDEX       i,j;
	FVector     OrthoNormal = Camera->GetOrthoNormal();

	// Get model.
	Brush->Lock(LOCK_Read);
	TransformedEdPolys = new(GMem,Brush->Polys->Num)FPoly;

	// Figure out brush movement constraints.
	FVector   Location = Brush->Location;
	FRotation Rotation = Brush->Rotation;
	BOOL      Snapped  = Constraints && GEditor && GEditor->constraintApply( Camera->Level->Model, Brush, &Location, &Rotation, Constraints );

	// Make coordinate system from camera.
	Coords        = GMath.UnitCoords * Brush->PostScale * Rotation * Brush->Scale;
	Coords.Origin = Camera->Coords.Origin;

	// Setup colors.
	if( Snapped < 0 ) DrawColor = InvalidColor;
	if( DrawSelected) DrawColor = WireColor + BRIGHTNESS(4);
	else              DrawColor = WireColor + BRIGHTNESS(10);

	VertexColor = WireColor + BRIGHTNESS(2);
	PivColor    = WireColor + BRIGHTNESS(0);

	// Transform and draw all FPolys.
	int NumTransformed = 0;
	FPoly *EdPoly = &TransformedEdPolys[0];
	for( i=0; i<Brush->Polys->Num; i++ )
	{
		*EdPoly = Brush->Polys(i);
		EdPoly->Normal = EdPoly->Normal.TransformVectorBy(Coords);
		//
		if
		(	!Camera->IsOrtho()
		||	(Camera->Actor->OrthoZoom<ORTHO_LOW_DETAIL)
		||	(EdPoly->PolyFlags & PF_NotSolid)
		||	(OrthoNormal | EdPoly->Normal) != 0.0 )
		{
			// Transform it.
			VertPtr = &EdPoly->Vertex[0];
			for( j=0; j<EdPoly->NumVertices; j++ )
				*VertPtr++ = (*VertPtr - Brush->PrePivot).TransformVectorBy(Coords) + Brush->PostPivot + Location;

			// Draw this brush's EdPoly's.
			if (DoScan && GEditor && GEditor->Scan.Active) GEditor->Scan.PreScan();
			DrawFPoly (Camera,EdPoly,DrawColor,0,Dotted);
			if (DoScan && GEditor && GEditor->Scan.Active) GEditor->Scan.PostScan (EDSCAN_BrushSide,(int)Brush,i,0,NULL);

			NumTransformed++;
			EdPoly++;
		}
	}
	//
	// Draw all vertices:
	//
	if (DrawVertices && (Brush->Polys->Num>0))
		{
		for (i=0; i<NumTransformed; i++)
			{
			EdPoly = &TransformedEdPolys[i];
			//
			V1 = &EdPoly->Vertex[0];
			V2 = &EdPoly->Vertex[EdPoly->NumVertices-1];
			for (j=0; j<EdPoly->NumVertices; j++)
				{
      			if (Project (Camera,V1,&X,&Y,NULL))
					{
					if ( GEditor && GEditor->Scan.Active) GEditor->Scan.PreScan();
         			DrawRect (Camera,VertexColor, X-1, Y-1, X+1, Y+1);
					if ( GEditor && GEditor->Scan.Active) GEditor->Scan.PostScan (EDSCAN_BrushVertex,(int)Brush,i,j,&EdPoly->Vertex[j]);
         			};
				V2 = V1++;
				};
			};
		//
		// Draw the origin:
		//
		Vertex = -Brush->PrePivot.TransformVectorBy(Coords) + Brush->PostPivot + Location;
		if (Project (Camera,&Vertex,&X,&Y,NULL))
			{
			if ( GEditor && GEditor->Scan.Active) GEditor->Scan.PreScan();
			//
			DrawRect (Camera,VertexColor,X-1, Y-1, X+1, Y+1);
			if (memcmp(&Brush->Scale,&GMath.UnitScale,sizeof(FScale)) || 
				memcmp(&Brush->PostScale,&GMath.UnitScale,sizeof(FScale)))
				{
				DrawRect (Camera,BlackColor, X-3, Y-3, X-1, Y-1);
				DrawRect (Camera,BlackColor, X+1, Y-3, X+3, Y-1);
				DrawRect (Camera,BlackColor, X-3, Y+1, X-1, Y+3);
				DrawRect (Camera,BlackColor, X+1, Y+1, X+3, Y+3);
				};
			if ( GEditor && GEditor->Scan.Active) GEditor->Scan.PostScan(EDSCAN_BrushVertex,(int)Brush,-1,0,&Vertex); // -1 = origin
			};
		};
	//
	// Draw the current pivot:
	//
	if (DrawPivot && (Brush->Polys->Num>0))
		{
		Vertex = Brush->PostPivot + Location;
		if (Project (Camera,&Vertex,&X,&Y,NULL))
			{
			if ( GEditor && GEditor->Scan.Active) GEditor->Scan.PreScan();
			if (Snapped==0)
				{
         		DrawRect (Camera,PivColor, X-1, Y-1, X+1, Y+1);
        		DrawRect (Camera,PivColor, X,   Y-4, X,   Y+4);
         		DrawRect (Camera,PivColor, X-4, Y,   X+4, Y);
				}
			else
         		{
         		DrawRect (Camera,PivColor, X-1, Y-1, X+1, Y+1);
         		DrawRect (Camera,PivColor, X-4, Y-4, X+4, Y-4);
         		DrawRect (Camera,PivColor, X-4, Y+4, X+4, Y+4);
         		DrawRect (Camera,PivColor, X-4, Y-4, X-4, Y+4);
         		DrawRect (Camera,PivColor, X+4, Y-4, X+4, Y+4);
				};
			if ( GEditor && GEditor->Scan.Active) GEditor->Scan.PostScan(EDSCAN_BrushVertex,(int)Brush,-2,0,&Vertex); // -2 = pivot
			};
		};
	Mark.Pop();
	Brush->Unlock(LOCK_Read);
	unguard;
	};

void FRender::DrawLevelBrushes(UCamera *Camera)
	{
	guard(FRender::DrawLevelBrushes);
	//
	ULevel			*Level = Camera->Level;
	UModel			*Brush;
	FConstraints	*Constraints;
	BYTE			WireColor;
	int				i,ShowVerts,Selected;
	//
	// Draw all regular brushes:
	//
	for (i=1; i<Level->BrushArray->Num; i++)
		{
		Brush = Level->BrushArray->Element(i);
		//
		if (Brush->ModelFlags & MF_Color)
			{
			WireColor = COLOR(Brush->Color,4);
			}
		else if (Brush->PolyFlags & PF_Portal)
			{
			WireColor = ScaleBoxHiColor;
			}
		else switch (Brush->CsgOper)
			{
			case CSG_Add:
				if		(Brush->PolyFlags & PF_Semisolid)	WireColor = SemiSolidWireColor;
				else if	(Brush->PolyFlags & PF_NotSolid)	WireColor = NonSolidWireColor;
				else										WireColor = AddWireColor;
				break;
			case CSG_Subtract:
	            WireColor = SubtractWireColor;
				break;
			default:
	            WireColor = GreyWireColor;
				break;
			};
		Selected  = GEditor && GEditor->MapEdit && (Brush->ModelFlags & MF_Selected);
		ShowVerts = Selected;
		//
		if (GEditor && GEditor->MapEdit && (Brush->ModelFlags & MF_ShouldSnap))
			{
			Constraints = &GEditor->Constraints;
			}
		else Constraints = NULL;
		//
		DrawBrushPolys(Camera,Brush,WireColor,0,Constraints,ShowVerts,ShowVerts,Selected,1);
	}
	unguard;
}

void FRender::DrawActiveBrush(UCamera *Camera)
	{
	guard(FRender::DrawActiveBrush);
	//
	int	Selected = Camera->Level->Brush()->ModelFlags & MF_Selected;
	DrawBrushPolys
		(
		Camera,Camera->Level->Brush(),
		BrushWireColor,
		1,
		GEditor ? &GEditor->Constraints : NULL,1,GEditor ? GEditor->ShowVertices : 0,
		Selected, 1
		);
	unguard;
	};

//
// Draw the brush's bounding box and pivots:
//
void FRender::DrawBoundingBox( UCamera *Camera, FBoundingBox *Bound )
{
	guard(FRender::DrawBoundingBox);

	FVector B[2],P,Q;
	FLOAT SX,SY;
	int i,j,k;

	B[0]=Bound->Min;
	B[1]=Bound->Max;

	for( i=0; i<2; i++ ) for( j=0; j<2; j++ )
	{
		P.X=B[i].X; Q.X=B[i].X;
		P.Y=B[j].Y; Q.Y=B[j].Y;
		P.Z=B[0].Z; Q.Z=B[1].Z;
		Draw3DLine( Camera, P, Q, 1, ScaleBoxColor, 0, 1 );

		P.Y=B[i].Y; Q.Y=B[i].Y;
		P.Z=B[j].Z; Q.Z=B[j].Z;
		P.X=B[0].X; Q.X=B[1].X;
		Draw3DLine( Camera, P, Q, 1, ScaleBoxColor, 0, 1 );

		P.Z=B[i].Z; Q.Z=B[i].Z;
		P.X=B[j].X; Q.X=B[j].X;
		P.Y=B[0].Y; Q.Y=B[1].Y;
		Draw3DLine( Camera, P, Q, 1, ScaleBoxColor, 0, 1 );
	}
	for( i=0; i<2; i++ ) for( j=0; j<2; j++ ) for( k=0; k<2; k++ )
	{
		P.X=B[i].X; P.Y=B[j].Y; P.Z=B[k].Z;
		if( Project( Camera, &P, &SX, &SY, NULL ) )
		{
			if( GEditor && GEditor->Scan.Active )
				GEditor->Scan.PreScan();
			DrawRect( Camera, ScaleBoxHiColor, SX-1, SY-1, SX+1, SY+1 );
			if( GEditor && GEditor->Scan.Active) GEditor->Scan.PostScan(EDSCAN_BrushVertex,0,0,0,&P);
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Misc gfx.
-----------------------------------------------------------------------------*/

//
// Draw a piece of an orthogonal grid (arbitrary axes):
//
void FRender::DrawGridSection
(
	UCamera *Camera,
	INT		CameraLocX,
	INT		CameraSXR,
	INT		CameraGridY,
	FVector *A,
	FVector *B,
	FLOAT	*AX,
	FLOAT	*BX,
	INT		AlphaCase
)
{
	guard(FRender::DrawGridSection);

	if( !CameraGridY ) return;
	if( !Camera->IsOrtho() ) appError("Ortho camera error");

	FLOAT	Start = (int)((CameraLocX - (CameraSXR>>1) * Camera->Zoom)/CameraGridY) - 1.0;
	FLOAT	End   = (int)((CameraLocX + (CameraSXR>>1) * Camera->Zoom)/CameraGridY) + 1.0;
	int     Dist  = (int)(Camera->X * Camera->Zoom / CameraGridY);
	int		i,Color,IncBits,GMax,iStart,iEnd;
	FLOAT	Alpha,Ofs;

	IncBits = 0;
	GMax    = Camera->X >> 2;

	if( Dist+Dist >= GMax )
	{
		while( (Dist>>IncBits) >= GMax )
			IncBits++;
		Alpha = (FLOAT)Dist / (FLOAT)((1<<IncBits) * GMax);
	}
	else Alpha = 0.0;

	iStart  = Max((int)Start,-32768/CameraGridY) >> IncBits;
	iEnd    = Min((int)End,  +32768/CameraGridY) >> IncBits;

	for( i=iStart; i<iEnd; i++ )
	{
		*AX = (i * CameraGridY) << IncBits;
		*BX = (i * CameraGridY) << IncBits;

		if ((i<<IncBits)&7)	Ofs = 6.9; // Normal.
		else				Ofs = 8.9; // Highlight 8-aligned.

		if( (i&1) != AlphaCase )
		{
#if AA
			if (i&1) Ofs += Alpha * (4.0-Ofs);
			Color = COLOR(P_GREY,27-(int)Ofs);
			DrawOrthoLine( Camera, *A, *B, Color, 0, 0.5 );
#else
			if (i&1) Ofs += Alpha * (4.0-Ofs);
			Color = COLOR(P_GREY,4+(int)Ofs);
			DrawOrthoLine( Camera, *A, *B, Color, 1, 1.0 );
#endif
		}
	}
	unguard;
}

//
// Draw worldbox and groundplane lines, if desired.
//
void FRender::DrawWireBackground( UCamera *Camera )
{
	guard(FRender::DrawWireBackground);
	if( !GEditor )
		return;

	// Vector defining worldbox lines.
	FVector	*Origin = &Camera->Coords.Origin;
	FVector B1( 32768.0, 32767.0, 32767.0);
	FVector B2(-32768.0, 32767.0, 32767.0);
	FVector B3( 32768.0,-32767.0, 32767.0);
	FVector B4(-32768.0,-32767.0, 32767.0);
	FVector B5( 32768.0, 32767.0,-32767.0);
	FVector B6(-32768.0, 32767.0,-32767.0);
	FVector B7( 32768.0,-32767.0,-32767.0);
	FVector B8(-32768.0,-32767.0,-32767.0);
	FVector A,B;
	int     i,j,Color;

	if( Camera->IsOrtho() )
	{
		if( Camera->Actor->ShowFlags & SHOW_Frame )
		{
			// Draw grid.
			for( int AlphaCase=0; AlphaCase<=1; AlphaCase++ )
			{
				if( Camera->Actor->RendMap==REN_OrthXY )
				{
					// Do Y-Axis lines.
					A.Y=+32767.0; A.Z=0.0;
					B.Y=-32767.0; B.Z=0.0;
					DrawGridSection( Camera, Origin->X, Camera->X, GEditor->Constraints.Grid.X, &A, &B, &A.X, &B.X, AlphaCase );

					// Do X-Axis lines.
					A.X=+32767.0; A.Z=0.0;
					B.X=-32767.0; B.Z=0.0;
					DrawGridSection( Camera, Origin->Y, Camera->Y, GEditor->Constraints.Grid.Y, &A, &B, &A.Y, &B.Y, AlphaCase );
				}
				else if (Camera->Actor->RendMap==REN_OrthXZ)
				{
					// Do Z-Axis lines.
					A.Z=+32767.0; A.Y=0.0;
					B.Z=-32767.0; B.Y=0.0;
					DrawGridSection( Camera, Origin->X, Camera->X, GEditor->Constraints.Grid.X, &A, &B, &A.X, &B.X, AlphaCase );

					// Do X-Axis lines.
					A.X=+32767.0; A.Y=0.0;
					B.X=-32767.0; B.Y=0.0;
					DrawGridSection( Camera, Origin->Z, Camera->Y, GEditor->Constraints.Grid.Z, &A, &B, &A.Z, &B.Z, AlphaCase );
				}
				else if (Camera->Actor->RendMap==REN_OrthYZ)
				{
					// Do Z-Axis lines.
					A.Z=+32767.0; A.X=0.0;
					B.Z=-32767.0; B.X=0.0;
					DrawGridSection( Camera, Origin->Y, Camera->X, GEditor->Constraints.Grid.Y, &A, &B, &A.Y, &B.Y, AlphaCase );

					// Do Y-Axis lines.
					A.Y=+32767.0; A.X=0.0;
					B.Y=-32767.0; B.X=0.0;
					DrawGridSection( Camera, Origin->Z, Camera->Y, GEditor->Constraints.Grid.Z, &A, &B, &A.Z, &B.Z, AlphaCase );
				}
			}

			// Draw axis lines.
			if( Camera->IsOrtho() )	Color = WireGridAxis;
			else					Color = GroundPlaneHighlight;

			A.X=+32767.0;  A.Y=0; A.Z=0;
			B.X=-32767.0;  B.Y=0; B.Z=0;
        	DrawOrthoLine( Camera, A, B, Color, 0, 1.0 );

			A.X=0; A.Y=+32767.0; A.Z=0;
			B.X=0; B.Y=-32767.0; B.Z=0;
        	DrawOrthoLine( Camera, A, B, Color, 0, 1.0 );

			A.X=0; A.Y=0; A.Z=+32767.0;
			B.X=0; B.Y=0; B.Z=-32767.0;
	       	DrawOrthoLine( Camera, A, B, Color, 0, 1.0 );
		}

		// Draw orthogonal worldframe.
    	DrawOrthoLine( Camera, B1, B2, WorldBoxColor, 1, 1.0 );
     	DrawOrthoLine( Camera, B3, B4, WorldBoxColor, 1, 1.0 );
     	DrawOrthoLine( Camera, B5, B6, WorldBoxColor, 1, 1.0 );
     	DrawOrthoLine( Camera, B7, B8, WorldBoxColor, 1, 1.0 );
     	DrawOrthoLine( Camera, B1, B3, WorldBoxColor, 1, 1.0 );
     	DrawOrthoLine( Camera, B5, B7, WorldBoxColor, 1, 1.0 );
		DrawOrthoLine( Camera, B2, B4, WorldBoxColor, 1, 1.0 );
     	DrawOrthoLine( Camera, B6, B8, WorldBoxColor, 1, 1.0 );
     	DrawOrthoLine( Camera, B1, B5, WorldBoxColor, 1, 1.0 );
     	DrawOrthoLine( Camera, B2, B6, WorldBoxColor, 1, 1.0 );
     	DrawOrthoLine( Camera, B3, B7, WorldBoxColor, 1, 1.0 );
     	DrawOrthoLine( Camera, B4, B8, WorldBoxColor, 1, 1.0 );
		return;
	}

	// Draw worldbox.
	if(  (Camera->Actor->ShowFlags & SHOW_Frame) &&
		!(Camera->Actor->ShowFlags & SHOW_Backdrop) )
	{
		Draw3DLine( Camera, B1, B2, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B1, B2, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B3, B4, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B5, B6, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B7, B8, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B1, B3, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B5, B7, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B2, B4, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B6, B8, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B1, B5, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B2, B6, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B3, B7, 1, WorldBoxColor, 1, 0 );
		Draw3DLine( Camera, B4, B8, 1, WorldBoxColor, 1, 0 );

		// Index of middle line (axis).
		j=(63-1)/2;
		for( i=0; i<63; i++ )
		{
			A.X=32767.0*(-1.0+2.0*i/(63-1));	B.X=A.X;

			A.Y=32767;                          B.Y=-32767.0;
			A.Z=0.0;							B.Z=0.0;
			Draw3DLine( Camera, A, B, 1, (i==j)?GroundPlaneHighlight:GroundPlaneColor, 1, 0 );

			A.Y=A.X;							B.Y=B.X;
			A.X=32767.0;						B.X=-32767.0;
			Draw3DLine( Camera, A, B, 1, (i==j)?GroundPlaneHighlight:GroundPlaneColor, 1, 0 );
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
