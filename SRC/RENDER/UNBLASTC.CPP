/*=============================================================================
	UnBlastC.cpp: C++ Texture blasting code

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Types.
-----------------------------------------------------------------------------*/

enum {LIGHT_XR				= 512};			// Duplicated in UnRender.inc.
enum {LIGHT_X_TOGGLE		= LIGHT_XR*4};	// Duplicated in UnRender.inc.

typedef void (*POST_EFFECT)();
typedef void (*TEX_INNER)( int SkipIn,FTexLattice *T );

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

extern "C"
{
	// Globals in this file.
	DWORD		GLight[LIGHT_XR * 5];
	BYTE		*GBits;
	TEX_INNER	GTexInner=NULL;
	POST_EFFECT GEffect=NULL;
	DWORD		GPolyFlags;

	void PostMask_8P (); void PostBlend_8P (); void PostFire_8P ();
	void PostMask_16P(); void PostBlend_16P(); void PostFire_16P();
	void PostMask_32P(); void PostBlend_32P(); void PostFire_32P();
	POST_EFFECT GEffectTable[5][FRender::DRAWRASTER_MAX] =
	{
		{NULL,	NULL,	NULL,			NULL,			NULL},			// 0-bit
		{NULL,	NULL,	PostMask_8P,	PostBlend_8P,	PostFire_8P},	// 8-bit
		{NULL,	NULL,	PostMask_16P,	PostBlend_16P,	PostFire_16P},	// 16-bit
		{NULL,	NULL,	NULL,			NULL,			NULL},			// 24-bit
		{NULL,	NULL,	NULL,			NULL,			NULL},			// 32-bit
	};

	// Texture mapper inner loop.
	ASMVAR  DWORD   TRO_DitherMask;
	ASMVAR	QWORD	TMI_DTex;
	ASMVAR	DWORD	TMI_DiffLight;
	ASMVAR	DWORD	TMI_DiffDest;
	ASMVAR  DWORD	*TMI_ProcBase;
	ASMVAR  DWORD	*TMI_LineProcBase;
	ASMVAR  FTexLattice *TMI_TopLattice;
	ASMVAR  FTexLattice **TMI_LatticeBase;
	ASMVAR  FTexLattice **TMI_NextLatticeBase;
	ASMVAR	DWORD	TMI_Shader;
	ASMVAR	BYTE	*TMI_Dest;
	ASMVAR	BYTE	*TMI_FinalDest;
	ASMVAR	BYTE	*TMI_RectFinalDest;

	ASMVAR	DWORD	TMI_ProcTable8P[],TMI_ProcTable16P[],TMI_ProcTable32P[];
	ASMVAR	DWORD	TMI_ModTable8P[],TMI_ModTable16P[],TMI_ModTable32P[];

	// Texture mapper outer loop.
	ASMVAR	BYTE	*TMO_Dest;
	ASMVAR	BYTE	*TMO_DestBits;
	ASMVAR	BYTE	**TMO_DestOrDestBitsPtr;
	ASMVAR  FSpan	*TMO_Span,*TMO_OrigSpan;
	ASMVAR  FSpan	*TMO_RectSpan;
	ASMVAR  FSpan	*TMO_NextRectSpan;
	ASMVAR  INT		TMO_Stride;

	// Light mapper inner loop.
	ASMVAR	FLOAT	*TLI_MeshFloat;
	ASMVAR	FLOAT	*TLI_Sinc;
	ASMVAR	DWORD	TLI_AddrMask;
	ASMVAR	DWORD	TLI_Temp;
	ASMVAR	DWORD	*TLI_ProcBase;
	ASMVAR  FTexLattice *TLI_TopLattice;
	ASMVAR	DWORD	*TLI_Dest;
	ASMVAR	DWORD	*TLI_DestEnd;
	ASMVAR	DWORD	TLI_ProcTable[];
	ASMVAR  DWORD	TLI_SkipIn;

	ASMVAR	DWORD	*TLO_TopBase;
	ASMVAR	DWORD	*TLO_BotBase;
	ASMVAR	DWORD	*TLO_FinalDest;
	ASMVAR  FSpan	*TLO_RectSpan;
	ASMVAR  void	(*TLO_LightInnerProc);
	ASMVAR  FTexLattice **TLO_LatticeBase;

	// TexRect outer loop.
	ASMVAR	INT		TRO_Y;
	ASMVAR	INT		TRO_SubRectEndY;
	ASMVAR	DWORD	**TRO_ThisLightBase;
	ASMVAR	FSpan	**TRO_SpanIndex;

	// Light mapper mid loop.
	ASMVAR	DWORD	TLM_GBlitInterX;
	ASMVAR	BYTE	TLM_GBlitInterXBits2;
	ASMVAR	DWORD	TRO_OuterProc;

	void CDECL TLM_8P_Unlit();
	void CDECL TLM_8P_Lit();
	void CDECL TRO_Outer8P();
	void CDECL TRO_Outer16P();
	void CDECL TRO_Outer32P();
	void CDECL LightOuter();

	void CDECL LightVInterpolate_8P_1();
	void CDECL LightVInterpolate_8P_2();
	void CDECL LightVInterpolate_8P_4();

	// Lattice setup loop.
	ASMVAR FMipInfo *TRL_MipPtr[MAX_MIPS];
};

#if !ASM
	void (*GLightInnerProc)();
#endif

/*-----------------------------------------------------------------------------
	Postprocessing effects.
-----------------------------------------------------------------------------*/

//
// 8-bit color.
//

// Blit with no postprocessing effects.
void PostBlit_8P()
	{
	static FSpan *SpanNext;
	FSpan *Span = TMO_OrigSpan;
	while (Span)
		{
		SpanNext	= Span->Next;
		//
		BYTE *Src		= &TMO_DestBits	[Span->Start];
		BYTE *Dest		= &TMO_Dest		[Span->Start];
		BYTE *DestEnd	= &TMO_Dest		[Span->End];
		while (Dest<DestEnd)
			{
			*Dest++ = *Src++;
			};
		Span = SpanNext;
		};
	};

// Blit with color 0 = see-through.
void PostMask_8P()
{
	static FSpan *SpanNext;
	FSpan *Span = TMO_OrigSpan;
	while( Span )
	{
		SpanNext	= Span->Next;

		BYTE *Src		= &TMO_DestBits	[Span->Start];
		BYTE *Dest		= &TMO_Dest		[Span->Start];
		BYTE *DestEnd	= &TMO_Dest		[Span->End];
		while( ((int)Dest & 3) && Dest<DestEnd )
		{
			if( *Src )
				*Dest = *Src;
			Src++;
			Dest++;
		}
		while( Dest+3 < DestEnd )
		{
			if( *(DWORD *)Src )
			{
				if( Src[0] && Src[1] && Src[2] && Src[3] )
				{
					*(DWORD *)Dest = *(DWORD *)Src;
				}
				else
				{
					if (Src[0]) Dest[0]=Src[0];
					if (Src[1]) Dest[1]=Src[1];
					if (Src[2]) Dest[2]=Src[2];
					if (Src[3]) Dest[3]=Src[3];
				}
			}
			Src  += 4;
			Dest += 4;
		}
		while( Dest < DestEnd )
		{
			if (*Src) *Dest = *Src; Src++; Dest++;
		}
		Span = SpanNext;
	}
}

// Blit with 64K palette blend table.
void PostBlend_8P()
	{
	static FSpan *SpanNext;
	FSpan *Span = TMO_OrigSpan;
	while (Span)
		{
		SpanNext = Span->Next;
		//
		BYTE *Src		= &TMO_DestBits	[Span->Start];
		BYTE *Dest		= &TMO_Dest		[Span->Start];
		BYTE *DestEnd	= &TMO_Dest		[Span->End];
		while (((int)Dest & 3) && (Dest<DestEnd))
			{
			*Dest = GGfx.BlendTable((int)*Src+((int)*Dest<<8)); Src++; Dest++;
			};
		while ((Dest+3)<DestEnd)
			{
			*Dest = GGfx.BlendTable((int)*Src+((int)*Dest<<8)); Src++; Dest++;
			*Dest = GGfx.BlendTable((int)*Src+((int)*Dest<<8)); Src++; Dest++;
			*Dest = GGfx.BlendTable((int)*Src+((int)*Dest<<8)); Src++; Dest++;
			*Dest = GGfx.BlendTable((int)*Src+((int)*Dest<<8)); Src++; Dest++;
			};
		while (Dest<DestEnd)
			{
			*Dest = GGfx.BlendTable((int)*Src+((int)*Dest<<8)); Src++; Dest++;
			};
		Span = SpanNext;
		};
	};

// Blit with remapping to a specified palette, i.e. for fire.
void PostFire_8P()
	{
	static FSpan *SpanNext;
	FSpan *Span = TMO_OrigSpan;
	while (Span)
		{
		SpanNext	= Span->Next;
		//
		BYTE *Src		= &TMO_DestBits	[Span->Start];
		BYTE *Dest		= &TMO_Dest		[Span->Start];
		BYTE *DestEnd	= &TMO_Dest		[Span->End];
		while (Dest < DestEnd)
			{
			*Dest++ = *Src++;
			};
		Span = SpanNext;
		};
	};

//
// 16-bit color.
//

// Blit with no postprocessing effects.
void PostBlit_16P()
	{
	static FSpan *SpanNext;
	FSpan *Span = TMO_OrigSpan;
	while (Span)
		{
		SpanNext	= Span->Next;
		//
		BYTE *Src		= &TMO_DestBits	[Span->Start];
		BYTE *Dest		= &TMO_Dest		[Span->Start];
		BYTE *DestEnd	= &TMO_Dest		[Span->End];
		while (Dest<DestEnd)
			{
			*Dest++ = *Src++;
			};
		Span = SpanNext;
		};
	};

// Blit with color 0 = see-through.
void PostMask_16P()
{
	static FSpan *SpanNext;
	FSpan *Span = TMO_OrigSpan;
	while( Span )
	{
		SpanNext	= Span->Next;

		WORD *Src		= &((WORD*)TMO_DestBits	)[Span->Start];
		WORD *Dest		= &((WORD*)TMO_Dest		)[Span->Start];
		WORD *DestEnd	= &((WORD*)TMO_Dest		)[Span->End];
		while( ((int)Dest & 3) && Dest<DestEnd )
		{
			if (*Src) *Dest = *Src; Src++; Dest++;
		}
		while( (Dest+3)<DestEnd )
		{
			if( *(DWORD *)Src )
			{
				if( Src[0]&&Src[1]&&Src[2]&&Src[3] )
				{
					*(DWORD *)Dest = *(DWORD *)Src;
				}
				else
				{
					if (Src[0]) Dest[0]=Src[0];
					if (Src[1]) Dest[1]=Src[1];
					if (Src[2]) Dest[2]=Src[2];
					if (Src[3]) Dest[3]=Src[3];
				}
			}
			Src+=4;
			Dest+=4;
		}
		while( Dest < DestEnd )
		{
			if( *Src )
				*Dest = *Src;
			Src++;
			Dest++;
		}
		Span = SpanNext;
	}
}

// Blit with blending effects.
void PostBlend_16P()
	{
	static FSpan *SpanNext;
	FSpan *Span = TMO_OrigSpan;
	while (Span)
		{
		SpanNext	= Span->Next;
		//
		WORD *Src		= (WORD*)(((int)TMO_DestBits+Span->Start)<<1);
		WORD *Dest		= (WORD*)(((int)TMO_Dest    +Span->Start)<<1);
		WORD *DestEnd	= (WORD*)(((int)TMO_Dest    +Span->End  )<<1);
		//
		while (((int)Dest & 6) && (Dest<DestEnd))
			{
			*Dest = *Src & 0xfff0; Src++; Dest++;
			};
		while ((Dest+3)<DestEnd)
			{
			// Can do one DWORD at a time
			*Dest = *Src & 0xfff0; Src++; Dest++;
			*Dest = *Src & 0xfff0; Src++; Dest++;
			*Dest = *Src & 0xfff0; Src++; Dest++;
			*Dest = *Src & 0xfff0; Src++; Dest++;
			};
		while (Dest<DestEnd)
			{
			*Dest = *Src & 0xfff0; Src++; Dest++;
			};
		Span = SpanNext;
		};
	};

// Blit with remapping to a specified palette, i.e. for fire.
void PostFire_16P()
	{
	static FSpan *SpanNext;
	FSpan *Span = TMO_OrigSpan;
	while (Span)
		{
		SpanNext	= Span->Next;
		//
		BYTE *Src		= &TMO_DestBits	[Span->Start];
		BYTE *Dest		= &TMO_Dest		[Span->Start];
		BYTE *DestEnd	= &TMO_Dest		[Span->End];
		while (Dest < DestEnd)
			{
			*Dest++ = *Src++;
			};
		Span = SpanNext;
		};
	};

/*-----------------------------------------------------------------------------
	Setup.
-----------------------------------------------------------------------------*/

void rendDrawAcrossSetup
(
	UCamera		*Camera, 
	UTexture	*Texture, 
	UPalette	*Palette, 
	DWORD		ThesePolyFlags, 
	DWORD		NotPolyFlags
)
{
	guard(rendDrawAcrossSetup);
	checkInput(Texture!=NULL);
	checkInput(Palette!=NULL);

	// Handle poly flags.
	GPolyFlags = (Texture->PolyFlags & ~NotPolyFlags) | ThesePolyFlags;

	if( !(GPolyFlags & PF_Transparent) )
	{
		if( !(GPolyFlags & PF_Masked) )	GBlit.DrawKind = FRender::DRAWRASTER_Normal;
		else							GBlit.DrawKind = FRender::DRAWRASTER_Masked;
	}
	else
	{
		GBlit.DrawKind = FRender::DRAWRASTER_Blended;
	}
	//GEffect = GEffectTable[ Camera->ColorBytes ][ GBlit.DrawKind ];
	GEffect = GEffectTable[ Camera->ColorBytes ][ FRender::DRAWRASTER_Normal ];

	// Handle dithering.
	FDitherTable *DitherBase;
	if( GRender.DoDither && !(GPolyFlags & PF_NoSmooth) )
		DitherBase = &GDither256[GRender.TemporalIter&3];
	else 
		DitherBase = &GNoDither256;

	// Set up mipmapping.
	for( int i=0; i<MAX_MIPS; i++ )
	{
		FMipInfo &Mip	= GBlit.Texture->Mips[i];
		Mip.Dither		= &(*DitherBase)[Texture->UBits].Unit[i];
		TRL_MipPtr[i]	= &GBlit.Texture->Mips[i];
	}
	unguard;
}

void rendDrawAcrossExit()
{
	guard(rendDrawAcrossExit);
	unguard;
}

/*-----------------------------------------------------------------------------
	Lighting vertical interpolators.
-----------------------------------------------------------------------------*/

#if ASM

typedef void (*LIGHT_V_INTERPOLATE_PROC)();

#else

typedef void (*LIGHT_V_INTERPOLATE_PROC)(FSpan *);

// 4 pixels high.
void LightVInterpolate_8P_4(FSpan *SubRectSpan)
	{
	while (SubRectSpan) // Sublattice is 4 pixels high
		{
		for (int i=SubRectSpan->Start; i<=SubRectSpan->End; i++)
			{
			DWORD Mid              = (TLO_TopBase[i]+TLO_BotBase[i])>>1;
			GLight[i + 2*LIGHT_XR] = Mid;
			GLight[i + 1*LIGHT_XR] = (TLO_TopBase[i]+Mid)>>1;
			GLight[i + 3*LIGHT_XR] = (TLO_BotBase[i]+Mid)>>1;
			};
		SubRectSpan = SubRectSpan->Next;
		};
	};

// 2 pixels high.
void LightVInterpolate_8P_2(FSpan *SubRectSpan)
	{
	while (SubRectSpan) // Sublattice is 2 pixels high
		{
		for (int i=SubRectSpan->Start; i<=SubRectSpan->End; i++)
			{
			GLight[i + 1*LIGHT_XR] = (TLO_TopBase[i]+TLO_BotBase[i])>>1;
			};
		SubRectSpan = SubRectSpan->Next;
		};
	};

// 1 pixel high.
void LightVInterpolate_8P_1(FSpan *) {};
#endif

LIGHT_V_INTERPOLATE_PROC LightVInterpolateProcs[3] =
	{
	LightVInterpolate_8P_1,
	LightVInterpolate_8P_2,
	LightVInterpolate_8P_4
	};

/*-----------------------------------------------------------------------------
	Texture setup.
-----------------------------------------------------------------------------*/

FCacheItem *ShaderCacheItem;

//
// Set up the self-modifying code for all versions of the
// texture mapper (MAX_MIPS mips, TMAPPER_DITHER_LINES lines, 4 trilinear phases).
//
void TexSetup( UCamera *Camera )
{
	// Set up palette.
	TMI_Shader = (DWORD)GRender.GetPaletteLightingTable
	(
		Camera->Texture,
		GBlit.Palette,
		GBlit.Zone,
		(GPolyFlags & PF_CloudWavy) ? Camera->Level->Info : NULL,
		ShaderCacheItem
	).PtrVOID;

	// Set colordepth-specific texture mapper info.
	DWORD *ModPtr = 0;
	if( Camera->ColorBytes==1 )
	{
		TMI_ProcBase	= TMI_ProcTable8P;
		TRO_OuterProc	= (DWORD)TRO_Outer8P;
		ModPtr			= &TMI_ModTable8P[1];
	}
	else if( Camera->ColorBytes==2 )
	{
		TMI_ProcBase		= TMI_ProcTable16P;
		TRO_OuterProc		= (DWORD)TRO_Outer16P;
		ModPtr				= &TMI_ModTable16P[1];
		TMI_Shader			= TMI_Shader >> 1;
	}
	else if( Camera->ColorBytes==4 )
	{
		TMI_ProcBase		= TMI_ProcTable32P;
		TRO_OuterProc		= (DWORD)TRO_Outer32P;
		ModPtr				= &TMI_ModTable32P[1];
		TMI_Shader			= TMI_Shader >> 2;
	}
	else
	{
		appError( "Invalid ColorBytes" );
	}

#if ASM

	// Set up all self-modifying code:
	FMipInfo *MipInfo  = &GBlit.Texture->Mips[0];
	FMipInfo *PrevInfo = &GBlit.Texture->Mips[1];

	for( int Mip=0; Mip<MAX_MIPS; Mip++ )
	{
		if( GBlit.MipRef[Mip] && !GBlit.PrevMipRef[Mip] )
		{
			BYTE		*TexBase	= MipInfo->Data;
			BYTE		*PrevBase	= PrevInfo->Data;
			DWORD		AddrMask	= (0xffff >> (16-MipInfo->VBits)) + (0xffff0000 << (16-MipInfo->UBits));
			DWORD		PrevMask	= (0xffff >> (16-MipInfo->VBits)) + (0xffff0000 << (16-PrevInfo->UBits));
			BYTE		UBits		= MipInfo->UBits;
			BYTE		PrevUBits	= PrevInfo->UBits;

			if (GRender.Extra3) AddrMask = PrevMask = 0;

			static DWORD *ModPtr1;
			ModPtr1 = ModPtr;

			STAT(GStat.CodePatches += 16);
			for( DWORD Line=0; Line<=TRO_DitherMask; Line++ )
			{
				FDitherPair *Pair = &MipInfo->Dither->Pair[Line][0];
				__asm
				{
					pushad

					mov eax, [Pair]
					mov ebx, [AddrMask]
					mov cl,  [UBits]
					mov esi, [TexBase]
					mov edi, [PrevBase]
					mov ch,  [PrevUBits]
					mov edx, [PrevMask]

					mov ebp, [ModPtr1] ; Frame base pointer not valid from here on

					call [ebp]

					popad
				}
				ModPtr1++;
			}
			GBlit.PrevMipRef[Mip] = 1;
		}
		MipInfo++;
		if (Mip<7) PrevInfo++;
		ModPtr += 4;
	}
#endif
}

//
// Finish all texturing.
//
void TexFinish(void)
{
	if( ShaderCacheItem ) ShaderCacheItem->Unlock();
}

/*-----------------------------------------------------------------------------
	Texture inner loops.
-----------------------------------------------------------------------------*/

#if !ASM

// C texture mapper inner loops:
// The C versions of these loops are extremely inefficient. The design goal
// of the C texture loops was to precisely model the inputs and outputs of the 
// assembly language loops rather than to be efficient or readable.

// Light value finder.
inline DWORD LightValAt(BYTE *Dest) {return *(DWORD *)(Dest + TMI_DiffLight);};

void TexInner8(int SkipIn,FTexLattice *T)
{
	FMipInfo *Mip			= &GBlit.Texture->Mips[T->RoutineOfs >> 4];
	DWORD MipLevel			= Mip->MipLevel;
	FDitherUnit *Unit		= &Mip->Dither[MipLevel];
	BYTE *Texture			= Mip->Data;
	BYTE  UBits				= Mip->UBits;
	DWORD VMask				= Mip->VMask << UBits;
	BYTE *Dest				= TMI_Dest;
	BYTE *AlignedDest		= (BYTE *)((int)TMI_Dest & ~3);
	QWORD Tex				= T->Q;
	QWORD DTex				= T->QX;

	if( SkipIn ) Tex += SkipIn * DTex;
	Tex  = (Tex  & ~(QWORD)0xffff) + LightValAt(AlignedDest);
	DTex = (DTex & ~(QWORD)0xffff) + (WORD)((LightValAt(AlignedDest+4) - (Tex&0xffff))>>2);
	if( SkipIn&3 ) Tex += (SkipIn&3) * (WORD)(DTex & 0xffff);

	while( Dest < TMI_FinalDest )
	{
		QWORD ThisTex = Tex + Unit->Pair[(int)Dest&3][TRO_Y&1].Offset;
		*Dest++ = ((WORD *)((int)TMI_Shader*2))
		[
			(int)(ThisTex&0x3f00)+
			(int)Texture
			[
				((ThisTex >> (64-UBits))        ) +
				((ThisTex >> (32-UBits)) & VMask)
			]
		];
		Tex += DTex;
		if (((int)Dest & 3)==0) DTex = (DTex & ~(QWORD)0xffff) + (WORD)((LightValAt(Dest+4) - (Tex&0xffff))>>2);
		STAT(GStat.Extra1++);
	}
}
void TexInner16( int SkipIn,FTexLattice *T )
{
	FMipInfo *Mip		= &GBlit.Texture->Mips[T->RoutineOfs >> 4];
	DWORD MipLevel		= Mip->MipLevel;
	FDitherUnit *Unit	= &Mip->Dither[MipLevel];
	BYTE *Texture		= Mip->Data;
	BYTE  UBits			= Mip->UBits;
	DWORD VMask			= Mip->VMask << UBits;
	BYTE *Dest			= TMI_Dest;
	BYTE *AlignedDest   = (BYTE *)((int)TMI_Dest & ~3);
	QWORD Tex			= T->Q;
	QWORD DTex			= T->QX;

	if( SkipIn ) Tex += SkipIn * DTex;
	Tex  = (Tex  & ~(QWORD)0xffff) + LightValAt(AlignedDest);
	DTex = (DTex & ~(QWORD)0xffff) + (WORD)((LightValAt(AlignedDest+4) - (Tex&0xffff))>>2);
	if( SkipIn& 3) Tex += (SkipIn&3) * (WORD)(DTex & 0xffff);

	while( Dest < TMI_FinalDest )
	{
		QWORD ThisTex = Tex + Unit->Pair[(int)Dest&3][TRO_Y&1].Offset;
		*(WORD *)((int)Dest++ * 2) = ((WORD *)((int)TMI_Shader*2))
		[
			(int)(ThisTex&0x3f00)+
			(int)Texture
			[
				((ThisTex >> (64-UBits))        ) +
				((ThisTex >> (32-UBits)) & VMask)
			]
		];
		Tex += DTex;
		if( ((int)Dest & 3)==0 ) 
			DTex = (DTex & ~(QWORD)0xffff) + (WORD)((LightValAt(Dest+4) - (Tex&0xffff))>>2);
		STAT(GStat.Extra1++);
	}
}
void TexInner32(int SkipIn,FTexLattice *T)
{
	FMipInfo *Mip		= &GBlit.Texture->Mips[T->RoutineOfs >> 4];
	DWORD MipLevel		= Mip->MipLevel;
	FDitherUnit *Unit	= &Mip->Dither[MipLevel];
	BYTE *Texture		= Mip->Data;
	BYTE  UBits			= Mip->UBits;
	DWORD VMask			= Mip->VMask << UBits;
	BYTE *Dest			= TMI_Dest;
	BYTE *AlignedDest   = (BYTE *)((int)TMI_Dest & ~3);
	QWORD Tex			= T->Q;
	QWORD DTex			= T->QX;

	if( SkipIn ) Tex += SkipIn * DTex;
	Tex  = (Tex  & ~(QWORD)0xffff) + LightValAt(AlignedDest);
	DTex = (DTex & ~(QWORD)0xffff) + (WORD)((LightValAt(AlignedDest+4) - (Tex&0xffff))>>2);
	if( SkipIn&3 ) Tex += (SkipIn&3) * (WORD)(DTex & 0xffff);

	while( Dest < TMI_FinalDest )
	{
		QWORD ThisTex = Tex + Unit->Pair[(int)Dest&3][TRO_Y&1].Offset;
		*(DWORD *)((int)Dest++ * 4) = ((DWORD *)((int)TMI_Shader*4))
		[
			(int)(ThisTex&0x3f00)+
			(int)Texture
			[
				((ThisTex >> (64-UBits))        ) +
				((ThisTex >> (32-UBits)) & VMask)
			]
		];
		Tex += DTex;
		if( ((int)Dest & 3)==0 )
			DTex = (DTex & ~(QWORD)0xffff) + (WORD)((LightValAt(Dest+4) - (Tex&0xffff))>>2);
		STAT(GStat.Extra1++);
	}
}
TEX_INNER GTexInnerTable[5]={NULL,TexInner8,TexInner16,NULL,TexInner32};
#endif

/*-----------------------------------------------------------------------------
	Texture outer loops.
-----------------------------------------------------------------------------*/

//
// Texture mapper outer loop.
//
#if !ASM
void TexOuter()
	{
	FSpan *SpanNext;
	int SpanStart,SpanEnd,StartX,EndX;
	//
	// Draw all spans:
	//
	while (TMO_Span)
		{
		SpanStart = TMO_Span->Start;
		SpanEnd   = TMO_Span->End;
		SpanNext  = TMO_Span->Next;
		//
		FTexLattice *T	= TMI_LatticeBase[SpanStart >> GBlit.LatticeXBits];
		if ((SpanStart ^ (SpanEnd-1)) & GBlit.LatticeXNotMask) // Draw multiple rectspans
			{
			StartX = (SpanStart & GBlit.LatticeXNotMask) + GBlit.LatticeX;
			EndX   = StartX + GBlit.LatticeX;
			//
			// Draw first, left-clipped rectspan:
			//
			TMI_Dest		= TMO_DestBits + SpanStart;
			TMI_FinalDest	= TMO_DestBits + StartX;
			GTexInner(SpanStart & GBlit.LatticeXMask,T++);
			//
			// Draw middle, unclipped rectspans:
			//
			while (EndX < SpanEnd)
				{
				TMI_Dest		= TMO_DestBits + StartX;
				TMI_FinalDest	= TMO_DestBits + EndX;
				GTexInner(0,T++);
				StartX  = EndX;
				EndX   += GBlit.LatticeX;
				};
			//
			// Draw last, right-clipped rectspan:
			//
			TMI_Dest		= TMO_DestBits + StartX;
			TMI_FinalDest	= TMO_DestBits + SpanEnd;
			GTexInner (0,T);
			}
		else // Draw single left- and right-clipped span
			{
			if ((SpanStart ^ SpanEnd) & ~3) // Separated
				{
				TMI_Dest		= TMO_DestBits + SpanStart;
				TMI_FinalDest	= TMO_DestBits + SpanEnd;
				GTexInner(SpanStart & GBlit.LatticeXMask,T);
				}
			else // Together
				{
				TMI_Dest		= TMO_DestBits + SpanStart;
				TMI_FinalDest	= TMO_DestBits + SpanEnd;
				GTexInner(SpanStart & GBlit.LatticeXMask,T);
				};
			};
		TMO_Span = SpanNext;
		};
	//
	// Update all rects:
	//
	FSpan *RectSpan = TMO_RectSpan;
	while (RectSpan)
		{
		FTexLattice *T	= TMI_LatticeBase[RectSpan->Start];
		int RectXCount	= RectSpan->End - RectSpan->Start;
		//
		while (RectXCount-- > 0)
			{
			T->Q  += T->QY;
			T->QX += T->QXY;
			T++;
			};
		RectSpan = RectSpan->Next;
		};
	};
#endif

/*-----------------------------------------------------------------------------
	Light mapping.
-----------------------------------------------------------------------------*/

void LightSetup()
{
	GGfx.SincTable->Lock(LOCK_Read);

	TLI_MeshFloat			= GLightManager->Mesh.PtrFLOAT;
	TLI_AddrMask			= (0xffffffff >> (32-GLightManager->MeshVBits)) + (0xffffffff << (32-GLightManager->MeshUBits));
	TLI_Sinc				= &GGfx.SincTable(0);
	TLI_ProcBase			= &TLI_ProcTable[(int)GLightManager->MeshUBits << 6];

	TLM_GBlitInterX			= GBlit.InterX;
	TLM_GBlitInterXBits2	= GBlit.InterXBits+2;
}

void LightFinish()
{
	GGfx.SincTable->Unlock(LOCK_Read);
}

#if !ASM
void LightInner_P_Unlit()
	{
	FTexLattice *T = TLI_TopLattice;
	if ((!T) || (!T->RoutineOfs))
		{
		FTexLattice **LatticeBase = &TLO_LatticeBase[(TLI_Dest - TLO_BotBase) >> GBlit.InterXBits];
		TLI_SkipIn = GBlit.InterX;
		T      = LatticeBase[-1];
		if ((!T) || (!T->RoutineOfs))
			{
			TLI_SkipIn = 0;
			T = LatticeBase[-MAX_XR];
			if ((!T) || (!T->RoutineOfs))
				{
				TLI_SkipIn = GBlit.InterX;
				T = LatticeBase[-MAX_XR-1];
				};
			};
		};
	DWORD	*Dest		= TLI_Dest;
	QWORD	Tex			= T->SubQ;
	QWORD	DTex		= T->SubQX;
	//
	if (TLI_SkipIn) Tex += DTex * TLI_SkipIn;
	//
	while (Dest < TLI_DestEnd)
		{
		*Dest++ = (Tex & 0x3ff8); // 2x2 Precision masked
		Tex += DTex;
		};
	};
void LightInner_P_Lit()
	{
	//
	// Make sure we're in a valid rect.  We may not be, as this inner loop will often
	// be called to render the first lightel of the next rect, when only the previous
	// rect's definition is available.  This could be solved by storing h & v deltas for
	// all lattices where they are valid, but this is not convenient due to the rol texture
	// coordinate encoding format, so we just adjust the rect here.  Adjustment is required,
	// on average, 8% of the time.
	//
	FTexLattice *T = TLI_TopLattice;
	//
	if ((!T) || (!T->RoutineOfs))
		{
		FTexLattice **LatticeBase = &TLO_LatticeBase[(TLI_Dest - TLO_BotBase) >> GBlit.InterXBits];
		T = LatticeBase[-1];
		if ((!T) || (!T->RoutineOfs))
			{
			T = LatticeBase[-MAX_XR];
			if ((!T) || (!T->RoutineOfs))
				{
				T = LatticeBase[-MAX_XR-1];
#if CHECK_ALL
				if (!T)				appError("LightInner inconsistency 1");
				if (!T->RoutineOfs) appError("LightInner inconsistency 2");
#endif
				TLI_SkipIn += GBlit.InterX;
				};
			}
		else TLI_SkipIn += GBlit.InterX;
		};
	DWORD	*Dest		= TLI_Dest;
	QWORD	Tex			= T->SubQ;
	QWORD	DTex		= T->SubQX;
	BYTE	UBits		= GLightManager->MeshUBits;
	DWORD	VMask		= ((1 << GLightManager->MeshVBits)-1) << UBits;
	DWORD	USize		= 1<<UBits;
	//
	if (TLI_SkipIn) Tex += DTex * TLI_SkipIn;
	//
	QWORD Ofs = (QWORD)1 << (64-UBits);
	while (Dest < TLI_DestEnd)
	{
		FLOAT *Addr1 = &GLightManager->Mesh.PtrFLOAT
		[
			((Tex >> (64-UBits))        ) +
			((Tex >> (32-UBits)) & VMask)
		];
		FLOAT *Addr2 = &GLightManager->Mesh.PtrFLOAT
		[
			(((Tex+Ofs) >> (64-UBits))  ) +
			((Tex >> (32-UBits)) & VMask)
		];
		FLOAT Alpha = GGfx.SincTable((Tex>>(56-UBits))&0xff);

		FLOAT A		= Addr1[0];
		FLOAT C		= Addr1[USize];
		FLOAT AB	= A+(Addr2[0    ]-A)*Alpha;
		FLOAT CD	= C+(Addr2[USize]-C)*Alpha;

		*Dest++ = ((int)(AB + (CD-AB)*GGfx.SincTable((Tex>>24)&0xff)) & 0x3ff8);
		Tex += DTex;
	}
}
#endif

//
// Generate lighting across an entire row of sublattices based on
// the setup info contained in the specified row of rects.
//
#if !ASM
inline void LightOuter(FSpan *Span)
	{
	//
	// Light this span
	//
	while (Span)
		{
		FSpan *SpanNext = Span->Next;
		//
		TLI_TopLattice	= TLO_LatticeBase[Span->Start >> GBlit.InterXBits];
		TLI_Dest		= TLO_BotBase + Span->Start;
		TLO_FinalDest	= TLO_BotBase + Span->End;
		TLI_SkipIn		= Span->Start & GBlit.InterXMask;
		//
		if ((Span->Start ^ (Span->End-1)) & GBlit.InterXNotMask) // Draw multiple rectspans
			{
			//
			// Light first, left-clipped rectspan:
			//
			TLI_DestEnd	= TLO_BotBase + (Span->Start & GBlit.InterXNotMask) + GBlit.InterX;
			GLightInnerProc();
			TLI_TopLattice++;
			//
			// Light middle, unclipped rectspans:
			//
			TLI_SkipIn	 = 0;
			TLI_Dest	 = TLI_DestEnd;
			TLI_DestEnd	+= GBlit.InterX;
			//
			while (TLI_DestEnd < TLO_FinalDest)
				{
				GLightInnerProc();
				TLI_Dest = TLI_DestEnd;
				TLI_DestEnd += GBlit.InterX;
				TLI_TopLattice++;
				};
			};
		//
		// Light last, right-clipped rectspan:
		//
		TLI_DestEnd = TLO_FinalDest;
		GLightInnerProc();
		//
		Span = SpanNext;
		};
	//
	// Update all rects:
	//
	FSpan *RectSpan = TLO_RectSpan;
	while (RectSpan)
		{
		FTexLattice *T	= TLO_LatticeBase[RectSpan->Start];
		int Count		= RectSpan->End - RectSpan->Start;
		//
		while (Count-- > 0)
			{
			T->SubQ		+= T->SubQY;
			T->SubQX	+= T->SubQXY;
			T++;
			};
		RectSpan = RectSpan->Next;
		};
	};
#endif

/*-----------------------------------------------------------------------------
	Main texture mapper.
-----------------------------------------------------------------------------*/

void rendDrawAcross
(
	UCamera *Camera,FSpanBuffer *SpanBuffer,
	FSpanBuffer *RectSpanBuffer, FSpanBuffer *LatticeSpanBuffer,
	FSpanBuffer *SubRectSpanBuffer, FSpanBuffer *SubLatticeSpanBuffer,
	int Sampled
)
{
	guard(rendDrawAcross);
	STAT(clock(GStat.TextureMap));

	static int RectEndY,EndY;
	static FSpan*const NullSpan = NULL;
	static FSpan*const* RectSpanDecisionList[4] =
	{
		&NullSpan,&TMO_RectSpan,&TMO_NextRectSpan,&TMO_RectSpan
	};
	static FTexLattice**const* LatticeBaseDecisionList[4] =
	{
		&TMI_NextLatticeBase,&TMI_LatticeBase,&TMI_NextLatticeBase,&TMI_LatticeBase
	};
	static int*const SubRectDecisionList[2] =
	{
		&TRO_SubRectEndY,
		&EndY
	};
	static int*const RectDecisionList[2] =
	{
		&RectEndY,
		&EndY
	};
	static DWORD*const LightAlternator = (DWORD *)((int)&GLight[0] + (int)&GLight[LIGHT_X_TOGGLE]);
	static DWORD*const OriginalLightBase[4]={&GLight[LIGHT_XR*1],&GLight[LIGHT_XR*1],&GLight[LIGHT_XR*2],&GLight[LIGHT_XR*3]};

	static DWORD* LightBase[4];
	static LIGHT_V_INTERPOLATE_PROC LightVInterpolateProc;
	static FSpan **RectIndex,**SubLatticeIndex,**SubRectIndex;

	LightVInterpolateProc = LightVInterpolateProcs[GBlit.SubYBits];
	memcpy(LightBase,OriginalLightBase,sizeof(LightBase));
#if ASM
		TLO_LightInnerProc = Sampled ? TLM_8P_Lit : TLM_8P_Unlit;
#else
		GLightInnerProc = Sampled ? LightInner_P_Lit : LightInner_P_Unlit;
		GTexInner = GTexInnerTable[Camera->ColorBytes];
		void (*TexOuterProc)() = TexOuter;
#endif

	// Set up texture mapper inner globals.
	TMI_LatticeBase		= &GRender.LatticePtr[RectSpanBuffer->StartY+1][1];
	TMI_NextLatticeBase	= &TMI_LatticeBase[MAX_XR];

	// Set up texture mapper outer globals.
	TMO_Stride = Camera->Stride;
	if( !GEffect )
	{
		TMO_DestBits          = NULL;
		TMO_DestOrDestBitsPtr = &TMO_DestBits;
	}
	else
	{
		TMO_DestBits = new( GMem, 4096 )BYTE;
		TMO_DestOrDestBitsPtr = &TMO_Dest;
	}
	TexSetup(Camera);
	LightSetup();

	// Set up.
	TRO_SpanIndex	= &SpanBuffer->Index[0];
	RectIndex		= &RectSpanBuffer->Index[0];
	SubLatticeIndex	= &SubLatticeSpanBuffer->Index[0];
	SubRectIndex	= &SubRectSpanBuffer->Index[0];

	TRO_Y			= SpanBuffer->StartY;
	EndY			= SpanBuffer->EndY;
	TRO_SubRectEndY	= (SubRectSpanBuffer->StartY + 1) << GBlit.SubYBits;
	RectEndY		= (RectSpanBuffer->StartY    + 1) << GBlit.LatticeYBits;

	// Skip into top rect.
	int SkipY = TRO_Y - (RectSpanBuffer->StartY << GBlit.LatticeYBits);
	if( SkipY > 0 )
	{
		int SubSkipY    = SkipY >> GBlit.SubYBits;
		FSpan *RectSpan = *RectSpanBuffer->Index;

		while( RectSpan )
		{
			FTexLattice *T = TMI_LatticeBase[RectSpan->Start];
			if( SubSkipY ) for (int i=RectSpan->Start; i<RectSpan->End; i++)
			{
				T->Q		+= SkipY    * T->QY;
				T->QX		+= SkipY    * T->QXY;
				T->SubQ		+= SubSkipY * T->SubQY;
				T->SubQX	+= SubSkipY * T->SubQXY;
				T++;
			}
			else for (int i=RectSpan->Start; i<RectSpan->End; i++)
			{
				T->Q		+= SkipY * T->QY;
				T->QX		+= SkipY * T->QXY;
				T++;
			}
			RectSpan = RectSpan->Next;
		}
	}

	// Prepare to traverse all lattice rows.
	TMO_Dest		= (BYTE *)((DWORD)Camera->Screen / Camera->ColorBytes) + SpanBuffer->StartY * TMO_Stride;
	TMO_RectSpan	= *RectIndex++;

	// Set up for first lighting sublattice.
	TLO_BotBase		= &GLight[0];
	TLO_RectSpan	= *RectSpanBuffer->Index;
	TLO_LatticeBase = TMI_LatticeBase;

#if ASM
	__asm
	{
		pushad
		;
		; Generate first row's lighting:
		;
		mov eax,[SubLatticeIndex]
		mov esi,[eax]
		add eax,4
		mov [SubLatticeIndex],eax
		call LightOuter
		;
		; Lead into MainYLoop:
		;
		mov		eax,[TRO_Y]				; eax = TRO_Y
		mov		ebx,[EndY]				; ebx = EndY
		mov		ecx,[RectEndY]			; ecx = RectEndY
		xor		edx,edx					; Zero edx
		;
		MainYLoop:
		;
		mov		edi,[RectIndex]			; edi = RectIndex
		cmp		ecx,ebx					; RectEndY <?> EndY
		;
		setg	dl						; dl = RectEndY > EndY
		;
		mov		esi,[edi]				; esi = *RectIndex
		add		edi,4					; edi = RectIndex+1
		;
		mov		[TMO_NextRectSpan],esi	; TMO_NextRectSpan = *RectIndex
		mov		esi,RectDecisionList[edx*4] ; edi = RectDecisionList[RectEndY > EndY]
		;
		mov		[RectIndex],edi			; RectIndex++
		mov		edi,[GBlit].SubY		; edi = GBlit.SubY
		;
		mov		edx,[esi]				; ecx = *RectDecisionList[RectEndY > EndY]
		add		edi,eax					; edi = TRO_Y + GBlit.SubY
		;
		mov		[RectEndY],edx			; RectEndY = *RectDecisionList[RectEndY > EndY]
		;
		; Traverse all sublattices rows in lattice row:
		;
		RectYLoop:
		;
		mov		edx,[TLO_BotBase]		; edx = TLO_BotBase
		mov		ebx,[LightAlternator]	; ebx = LightAlternator
		;
		mov		[TLO_TopBase],edx		; TLO_TopBase = TLO_BotBase
		sub		ebx,edx					; ebx = LightAlternator - TLO_BotBase
		;
		mov		[TLO_BotBase],ebx		; TLO_BotBase = LightAlternator - TLO_BotBase
		mov		esi,[EndY]				; esi = EndY
		;
		mov		[LightBase],edx			; LightBase[0] = TLO_BotBase
		cmp		edi,esi					; (TRO_Y + GBlit.SubY) <?> EndY
		;
		setl	dl						; dl = (TRO_Y + GBlit.SubY) < EndY
		;
		shl		dl,1					; dl = ((TRO_Y + GBlit.SubY) < EndY) * 2
		cmp		edi,ecx					; (TRO_Y + GBlit.SubY) <?> RectEndY
		;
		setl	cl						; cl = (TRO_Y + GBlit.SubY) < RectEndY
		;
		add		cl,dl					; cl = choice index
		xor		ebx,ebx					; ebx = 0
		;
		mov		bl,cl					; ebx = choice index
		mov		ecx,[TRO_SubRectEndY]	; ecx = TRO_SubRectEndY
		;
		xor		eax,eax					; Clear out eax
		cmp		ecx,esi					; TRO_SubRectEndY <?> EndY
		;
		mov		edx,LatticeBaseDecisionList[ebx*4] ; edx = LatticeBaseDecisionList[Choice]
		mov		ecx,RectSpanDecisionList[ebx*4] ; ecx = RectSpanDecisionList[Choice]
		;
		setg	al						; eax = TRO_SubRectEndY > EndY
		;
		mov		esi,[edx]				; esi = *LatticeBaseDecisionList[Choice]
		mov		ebx,[ecx]				; ebx = *RectSpanDecisionList[Choice]
		;
		mov		[TLO_LatticeBase],esi	; TLO_LatticeBase = *LatticeBaseDecisionList[Choice]
		mov		edx,[SubLatticeIndex]	; edx = SubLatticeIndex
		;
		mov		esi,SubRectDecisionList[eax*4] ; esi = SubRectDecisionList[TRO_SubRectEndY > EndY]
		mov		[TLO_RectSpan],ebx		; TLO_RectSpan = *RectSpanDecisionList[Choice]
		;
		mov		ecx,[esi]				; ecx = *SubRectDecisionList[TRO_SubRectEndY > EndY]
		mov		esi,[edx]				; esi = *SubLatticeIndex
		;
		sub		edi,ecx					; edi = TRO_Y + GBlit.SubY - TRO_SubRectEndY
		add		edx,4					; edx = SubLatticeIndex+1
		;
		mov		[TRO_SubRectEndY],ecx	; TRO_SubRectEndY = *SubRectDecisionList[TRO_SubRectEndY > EndY]
		mov		[SubLatticeIndex],edx	; SubLatticeIndex++
		;
		lea		edx,LightBase[edi*4]	; ecx = &LightBase[TRO_Y + GBlit.SubY - TRO_SubRectEndY]
		;
		mov		[TRO_ThisLightBase],edx	; TRO_ThisLightBase	= &LightBase[TRO_Y + GBlit.SubY - TRO_SubRectEndY]
		;
		; Perform sublattice lighting:
		;
		call	LightOuter
		;
		; Interpolate the sublattice lighting:
		;
		mov		eax,[SubRectIndex]
		mov		ebx,4
		;
		add		ebx,eax
		;
		mov		esi,[eax]
		mov		[SubRectIndex],ebx
		;
		test	esi,esi
		jz		SkipLighting
		;
		call	[LightVInterpolateProc]
		SkipLighting:
		;
		; Texture map these 1/2/4 lines:
		;
		mov eax,[TRO_Y]
		call [TRO_OuterProc]
		;
		; Next subrect:
		;
		mov edi,[GBlit].SubY
		mov eax,[TRO_Y]
		;
		add edi,eax
		mov ecx,[RectEndY]
		;
		mov [TRO_SubRectEndY],edi
		;
		cmp eax,ecx ; TRO_Y <?> RectEndY
		jl  RectYLoop
		;
		; Next rect:
		;
		mov edi,[TMO_NextRectSpan]		; edi = TMO_NextRectSpan
		mov	esi,[TMI_NextLatticeBase]	; esi = TMI_NextLatticeBase
		;
		mov ecx,[GBlit].LatticeY		; edx = GBlit.LatticeY
		mov [TMO_RectSpan],edi			; TMO_RectSpan = TMO_NextRectSpan;
		;
		mov [TMI_LatticeBase],esi		; TMI_LatticeBase = TMI_NextLatticeBase
		add ecx,eax						; ecx = TRO_Y + GBlit.LatticeY
		;
		add	esi,MAX_XR*4				; esi = TMI_NextLatticeBase + MAX_XR
		mov [RectEndY],ecx				; RectEndY = TRO_Y + GBlit.LatticeY
		;
		mov ebx,[EndY]					; ebx = EndY
		mov [TMI_NextLatticeBase],esi	; TMI_NextLatticeBase += MAX_XR
		;
		xor		edx,edx					; Zero edx
		;
		cmp eax,ebx						; TRO_Y <?> TRO_EndY
		jl MainYLoop					; Next rect
		;
		popad
	}
#else
	LightOuter(*SubLatticeIndex++);
	while( TRO_Y < EndY )
	{
		RectEndY = *RectDecisionList[RectEndY > EndY]; //Logic version: if (RectEndY > EndY) RectEndY = EndY;
		TMO_NextRectSpan = *RectIndex++;

		while( TRO_Y < RectEndY )
		{
			TLO_TopBase		= LightBase[0] = TLO_BotBase;
			TLO_BotBase		= (DWORD *)((int)LightAlternator - (int)TLO_BotBase);

			// No-branch logic to set up pointers required by LightOuter, equivalant to the following:
			//
			//if (TempY<RectEndY)	{TLO_RectSpan = TMO_RectSpan;		TLO_LatticeBase = TMI_LatticeBase;}
			//else if (TempY<EndY)	{TLO_RectSpan = TMO_NextRectSpan;   TLO_LatticeBase = TMI_NextLatticeBase;}
			//else					{TLO_RectSpan = NULL;				TLO_LatticeBase = TMI_NextLatticeBase;};
			//
			//if (TRO_SubRectEndY > EndY) TRO_SubRectEndY = EndY;
			int TempY = TRO_Y + GBlit.SubY;
			int Choice = (TempY<RectEndY) + (TempY<EndY)*2;

			TLO_RectSpan		= *RectSpanDecisionList[Choice];
			TLO_LatticeBase		= *LatticeBaseDecisionList[Choice];

			TRO_SubRectEndY		= *SubRectDecisionList[TRO_SubRectEndY > EndY];
			TRO_ThisLightBase	= &LightBase[TRO_Y + GBlit.SubY - TRO_SubRectEndY];

			LightOuter(*SubLatticeIndex++);
			LightVInterpolateProc(*SubRectIndex++);

			do{
				*TMO_DestOrDestBitsPtr	= TMO_Dest; //Translation: if (!GEffect) TMO_DestBits = TMO_Dest;
				TMI_DiffLight			= (int)*TRO_ThisLightBase - (int)TMO_DestBits;
				TMI_DiffDest			= (int)TMO_Dest - (int)TMO_DestBits;
				TMI_LineProcBase		= &TMI_ProcBase[(TRO_Y&TRO_DitherMask)*2];

				// Perform all texture mapping and rect updating.
				TMO_OrigSpan = TMO_Span = *TRO_SpanIndex++;
				TexOuterProc();
				if (GEffect) GEffect();

				// Next line.
				TMO_Dest += TMO_Stride;
				TRO_ThisLightBase++;
				} while (++TRO_Y < TRO_SubRectEndY);
			// Next subrect.
			TRO_SubRectEndY   = TRO_Y + GBlit.SubY;
		}
		// Next rect:
		TMO_RectSpan		 = TMO_NextRectSpan;
		TMI_LatticeBase		 = TMI_NextLatticeBase;
		TMI_NextLatticeBase += MAX_XR;
		RectEndY			 = TRO_Y + GBlit.LatticeY;
	}
#endif
	TexFinish();
	LightFinish();
	STAT(unclock(GStat.TextureMap));
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
