/*=============================================================================
	UnRaster.h: Rasterization template

	Copyright 1995 Epic MegaGames, Inc.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney

	Optimizations needed:
		* Convert textured poly rasterizer to assembly
=============================================================================*/

#ifndef _INC_UNRASTER // Prevent header from being included multiple times
#define _INC_UNRASTER

/*-----------------------------------------------------------------------------
	Generic templates for polygon pipeline
-----------------------------------------------------------------------------*/

template <class TRasterPointT> class TRasterLine
{
public:
	union
	{
		// Explicit start and end points.
		struct {TRasterPointT Start,End;};

		// Start and end points in array form.
		TRasterPointT Point[2];
	};
	// Draw a flatshaded range of pixels.
	inline void DrawFlatRange(const ICamera &Camera,RAINBOW_PTR CameraLine, BYTE Color, INT Start, INT End) const
	{
		INT Pixels = End - Start;
		if( Pixels > 0)
		{
			if( Camera.ColorBytes==1 )
			{
				CameraLine.PtrBYTE += Start;
				while( Pixels-- > 0 )
					*CameraLine.PtrBYTE++ = Color;
			}
			else if( Camera.ColorBytes==2 )
			{
				WORD HiColor = 
					(Camera.Caps & CC_RGB565) 
				?	GGfx.DefaultColors[Color].HiColor565()
				:	GGfx.DefaultColors[Color].HiColor555();

				CameraLine.PtrWORD += Start;
				while( Pixels-- > 0 )
					*CameraLine.PtrWORD++ = HiColor;
			}
			else if( Camera.ColorBytes==4)
			{
				DWORD TrueColor = GGfx.DefaultColors[Color].TrueColor();

				CameraLine.PtrDWORD += Start;
				while( Pixels-- > 0 )
					*CameraLine.PtrDWORD++ = TrueColor;
			}
		}
	}
	// Draw a flatshaded span.
	inline void DrawFlatSpan(const ICamera &Camera,RAINBOW_PTR CameraLine, BYTE Color) const
	{
		DrawFlatRange(Camera,CameraLine,Color,Start.GetX(),End.GetX());
	}
	// Draw a flatshaded clipped span.
	inline void DrawFlatSpanClipped
		(
		const ICamera &Camera,
		RAINBOW_PTR CameraLine,
		BYTE Color,
		FSpan *Span
		) const
	{
		INT StartX	= Start.GetX();
		INT EndX    = End.GetX();

		if ( EndX > StartX )
		{
			// Draw all possibly visible spans.
			while( (Span != NULL) && (Span->Start < EndX) )
			{
				if( Span->End > StartX )
				{
					DrawFlatRange
					(
						Camera,
						CameraLine,
						Color,
						Max(StartX,Span->Start),
						Min(EndX,Span->End)
					);
				}
				Span = Span->Next;
			}
		}
	}


};
template <class TRasterLineT,class TRasterPointT> class TRasterPoly
{
public:
	INT				StartY;
	INT				EndY;
	TRasterLineT	Lines[1];

	// Force the polygon to be facing the viewer.
	inline void ForceForwardFace();

	// Draw the polygon flatshaded.
	inline void DrawFlat(const ICamera &Camera,BYTE Color,FSpanBuffer *SpanBuffer);

	// Draw the polygon using the specified setup info.
	inline void Draw
	(
		const ICamera &Camera,
		FPolySpanTextureInfoBase &Info,
		FSpanBuffer *SpanBuffer
	);
};
template <class TRasterPointT,class TThisT> class TRasterSideSetup
{
public:
	INT					DY;			// Y length
	TThisT				*Next;		// Next side in list, NULL=none
	TRasterPointT		P;			// Value at this point
	TRasterPointT		DP;			// X-derivative at this point
};
template <class TRasterSideSetupT, class TRasterLineT, class TRasterPolyT, class TThisT, class TTransformT> class TRasterSetup
{
public:
	INT					StartY;		// Starting Y value
	INT					EndY;		// Ending Y value + 1
	VOID				*MemTop;	// Top of allocated memory
	TRasterSideSetupT	*LeftSide;	// Left side rasterization setup, NULL = none
	TRasterSideSetupT	*RightSide;	// Right side rasterization setup, NULL = none
	FMemPool			*Mem;		// Memory pool

	// Functions.
	inline void CalcBound(const TTransformT *Pts, INT NumPts, FScreenBounds &BoxBounds);
	inline void Setup(const ICamera &Camera, const TTransformT *Pts, INT NumPts, FMemPool *MemPool);
	inline void SetupCached(const ICamera &Camera, const TTransformT *Pts, INT NumPts, FMemPool *TempPool, FMemPool *CachePool, TRasterSideSetupT **Cache);
	inline void Generate(TRasterPolyT *Raster) const;
	inline void Release()
	{
		GUARD;
		Mem->Release(MemTop);
		UNGUARD("TRasterSetup::Release");
	};
};

/*-----------------------------------------------------------------------------
	Flat shaded polygon implementation
-----------------------------------------------------------------------------*/

class FRasterPoint
{
public:
	INT X;
	inline INT GetX() const {return X;};
};
class FRasterLine : public TRasterLine<FRasterPoint>
{
public:
	inline void SetupSpan
	(
		FPolySpanTextureInfoBase &Info
	)
	{
	}
	inline void DrawSpan
	(
		const ICamera &Camera,
		RAINBOW_PTR CameraLine,
		FPolySpanTextureInfoBase &Info,
		INT Line
	) const
	{
		DrawFlatSpan(Camera,CameraLine,SelectColor);
	}
	inline void DrawSpanClipped
	(
		const ICamera &Camera,
		RAINBOW_PTR CameraLine,
		FPolySpanTextureInfoBase &Info,
		INT Line,
		FSpan *Span
	) const
	{
		DrawFlatSpanClipped(Camera,CameraLine,SelectColor,Span);
	}
	inline INT IsBackwards()
	{
		return Start.X > End.X;
	}
};
class FRasterSideSetup : public TRasterSideSetup<FRasterPoint,FRasterSideSetup>
{
public:
	inline VOID SetupSide(const FTransform *P1, const FTransform *P2,INT NewY)
	{
		FLOAT YAdjust	= (FLOAT)(NewY+1) - P1->ScreenY;
		FLOAT FloatDX 	= (P2->ScreenX - P1->ScreenX) / (P2->ScreenY - P1->ScreenY);

		ftoi(DP.X,FloatDX);
		ftoi(P.X,P1->ScreenX + FloatDX * YAdjust);
	}
	inline void GenerateSide(FRasterLine *Line,INT Offset,INT TempDY) const
	{
		INT TempFixX	= P.X;
		INT TempFixDX	= DP.X;
#if ASM
		FRasterPoint *Point = &Line->Point[Offset];
		if ( TempDY > 0 ) __asm 
		{
			// 4-unrolled, 2 cycle rasterizer loop.
			mov eax,[TempFixX]
			push ebp
			mov ecx,[TempFixDX]
			mov ebx,eax
			mov edx,ecx
			sar eax,16
			mov edi,[Point]
			sar ecx,16
			mov esi,[TempDY]
			shl ebx,16
			mov ebp,edi
			shl edx,16
			add ebp,(SIZE FRasterLine)
			jmp RasterLoop

			ALIGN 16
			RasterLoop: ; = 2 cycles per raster side
				add ebx,edx
				mov [edi],eax
				adc eax,ecx
				add ebx,edx

				mov [ebp],eax
				adc eax,ecx
				mov [edi+2*(SIZE FRasterLine)],eax
				add ebx,edx

				adc eax,ecx
				add edi,4*(SIZE FRasterLine)
				mov [ebp+2*(SIZE FRasterLine)],eax
				add ebx,edx

				adc eax,ecx
				add ebp,4*(SIZE FRasterLine)

				sub esi,4
				jg  RasterLoop
			pop ebp				
		}
#else
		while ( TempDY-- > 0 )
		{
			(Line++)->Point[Offset].X  = Unfix(TempFixX);
			TempFixX += TempFixDX;
		}

#endif
	};
};
class FRasterPoly : public TRasterPoly<FRasterLine,FRasterPoint> {};
class FRasterSetup : public TRasterSetup<FRasterSideSetup,FRasterLine,FRasterPoly,class FRasterSetup,FTransform> {};

/*-----------------------------------------------------------------------------
	Texture coordinates polygon implementation
-----------------------------------------------------------------------------*/

class FRasterTexPoint
{
public:
	union
	{
		struct
		{
			FLOAT	FloatX;
			FLOAT	FloatU;
			FLOAT	FloatV;
			FLOAT	FloatG;
		};
		struct
		{
			QWORD	Q;
			INT		IntX;
		};
	};
	inline INT GetX() const
	{	
		return ftoi(FloatX);
	}
};
class FRasterTexLine : public TRasterLine<FRasterTexPoint>
{
public:
	inline INT IsBackwards()
	{
		return Start.FloatX > End.FloatX;
	}
	inline void SetupSpan
	(
		FPolySpanTextureInfoBase &Info
	)
	{
		//optimize: Convert to assembly.
		INT StartX = Max(0,ftoi(Start.FloatX));
		INT EndX   = ftoi(End.FloatX);

		FLOAT RLength	= 1.0/(End.FloatX - Start.FloatX);
		FLOAT XAdjust	= (FLOAT)StartX + 0.5 - Start.FloatX;

		FLOAT FloatDU	= (End.FloatU - Start.FloatU) * RLength;
		FLOAT FloatDV	= (End.FloatV - Start.FloatV) * RLength;
		FLOAT FloatDG	= (End.FloatG - Start.FloatG) * RLength;

		//optimize: Use IEEE fadd/fst trick and hand optimize this.
		Start.Q =
			(
			((QWORD)(ftoi(Start.FloatG + XAdjust * FloatDG) & 0x3fff                 )) +
			((QWORD)(ftoi(Start.FloatV + XAdjust * FloatDV) & ((1<<(Info.VBits+16))-1)) << 16) +
			((QWORD)(ftoi(Start.FloatU + XAdjust * FloatDU) & 0xfffffffc              ) << (48-Info.UBits))
			);
		End.Q =
			(
			((QWORD)(ftoi(FloatDG) & 0xffff                 )) +
			((QWORD)(ftoi(FloatDV) & ((1<<(Info.VBits+16))-1)) << 16) +
			((QWORD)(ftoi(FloatDU) & 0xfffffffc              ) << (48-Info.UBits))
			);
		Start.IntX = StartX;
		End.IntX   = EndX;
	}
	inline void DrawSpan
		(
		const ICamera &Camera,
		RAINBOW_PTR CameraLine,
		FPolySpanTextureInfoBase &Info,
		INT Line
		)
	{
		INT Pixels = End.IntX - Start.IntX;
		if ( Pixels > 0 )
		{
			(Info.*Info.DrawFunc)
			(
				Start.Q,
				End.Q, 
				CameraLine.PtrBYTE + Start.IntX * Camera.ColorBytes, 
				Pixels, 
				Line 
			);
		}
	}
	inline void DrawSpanClipped
		(
		const ICamera &Camera,
		RAINBOW_PTR CameraLine,
		FPolySpanTextureInfoBase &Info,
		INT Line,
		FSpan *Span
		) const
	{
		INT StartX = Start.IntX;
		INT EndX   = End.IntX;
		if ( EndX > StartX )
		{
			// Draw all possibly visible spans.
			while( (Span != NULL) && (Span->Start < EndX) )
			{
				if( Span->End > StartX )
				{
					// This span is visible.
					if( Span->Start <= StartX )
					{
						// Not start-clipped.
						(Info.*Info.DrawFunc)
						(
							Start.Q,
							End.Q,
							CameraLine.PtrBYTE + StartX * Camera.ColorBytes,
							Min( EndX, Span->End ) - StartX, 
							Line
						);
					}
					else
					{
						// Start-clipped.
						(Info.*Info.DrawFunc)
						(
							Start.Q + (Span->Start-StartX) * End.Q,
							End.Q,
							CameraLine.PtrBYTE + Span->Start * Camera.ColorBytes,
							Min( EndX, Span->End ) - Span->Start, 
							Line
						);
					}
				}
				Span = Span->Next;
			}
		}
	}
};

class FRasterTexSideSetup : public TRasterSideSetup<FRasterTexPoint,FRasterTexSideSetup>
{
public:
	inline void SetupSide(const FTransTex *P1, const FTransTex *P2,INT NewY)
	{
		FLOAT FloatRDY		= 1.0 / (P2->ScreenY - P1->ScreenY);
		FLOAT FloatYAdjust	= (FLOAT)(NewY+1) - P1->ScreenY;

		DP.FloatX 			= (P2->ScreenX  - P1->ScreenX ) * FloatRDY;
		DP.FloatU			= (P2->U        - P1->U       ) * FloatRDY;
		DP.FloatV			= (P2->V        - P1->V       ) * FloatRDY;
		DP.FloatG			= (P2->G        - P1->G       ) * FloatRDY;

		P.FloatX			= P1->ScreenX  + DP.FloatX * FloatYAdjust;
		P.FloatU			= P1->U        + DP.FloatU * FloatYAdjust;
		P.FloatV			= P1->V        + DP.FloatV * FloatYAdjust;
		P.FloatG			= P1->G        + DP.FloatG * FloatYAdjust;
	}
	inline void GenerateSide(FRasterTexLine *Line,INT Offset,INT TempDY) const
	{
#if ASM
		FRasterTexPoint *Point		= &Line->Point[Offset];
		const FRasterTexPoint *LP   = &P;
		const FRasterTexPoint *LDP  = &DP;
		__asm
		{
			// Optimization ideas by Erik de Nieve
            mov edx,[TempDY]
            mov eax,[Point]
            mov ebx,[LP]
            mov ecx,[LDP]
            cmp edx,0
            jle Done

            fld [ecx]LDP.FloatG ; DG
            fld [ecx]LDP.FloatV ; DV
            fld [ecx]LDP.FloatU ; DU
            fld [ecx]LDP.FloatX ; DX

            fld [ebx]LDP.FloatG ; Values
            fld [ebx]LDP.FloatV
            fld [ebx]LDP.FloatU
            fld [ebx]LDP.FloatX

            RasterLoop:  ; Stack 0-7 :  X U V G - dX dU dV dG

            fst     [eax]LDP.FloatX		; X			; XUVG
            fxch	st(1)
            fst     [eax]LDP.FloatU		; U			; UXVG
            fxch	st(2)
            fst     [eax]LDP.FloatV		; V			; VXUG
            fxch	st(3)
            fst     [eax]LDP.FloatG		; G			; GXUV

            fadd	st,st(7)			; G += DG    ; GXUV
            fxch	st(3)
            fadd	st,st(6)			; V += DV    ; VXUG
            fxch	st(2)
            fadd	st,st(5)			; U += DU    ; UXVG
            fxch	st(1)
            fadd	st,st(4)			; X += DX    ; XUVG

            add eax,SIZE FRasterTexLine
            dec edx

            jne RasterLoop

            fcompp ; pop 8 registers in 4 cycles
            fcompp
            fcompp
            fcompp

            Done:
		}
#else
		FLOAT TempFloatX	= P.FloatX;
		FLOAT TempFloatU	= P.FloatU;
		FLOAT TempFloatV	= P.FloatV;
		FLOAT TempFloatG	= P.FloatG;

		while ( TempDY-- > 0 )
		{
			Line->Point[Offset].FloatX = TempFloatX; TempFloatX += DP.FloatX;
			Line->Point[Offset].FloatU = TempFloatU; TempFloatU += DP.FloatU;
			Line->Point[Offset].FloatV = TempFloatV; TempFloatV += DP.FloatV;
			Line->Point[Offset].FloatG = TempFloatG; TempFloatG += DP.FloatG;
			Line++;
		}
#endif
	}
};
class FRasterTexPoly  : public TRasterPoly<FRasterTexLine,FRasterTexPoint> {};
class FRasterTexSetup : public TRasterSetup<FRasterTexSideSetup,FRasterTexLine,FRasterTexPoly,class FRasterTexSetup,FTransTex> {};

class FGlobalRaster
{
public:
	enum {MAX_RASTER_LINES	= 1024}; // Maximum scanlines in rasterization

	class FRasterPoly *Raster; // Polygon rasterization data

	void Init();
	void Exit();
};

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
#endif // _INC_UNRASTER
