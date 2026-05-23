/*=============================================================================
	UnTmap.cpp: Unreal span texture mapping subsytem.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	Unreal's main span texture mapping code.  It's responsible for drawing
	linear texture-mapped spans of pixels with Gouraud shading in all
	possible color depths.

Definitions:
	texture span:
		A horizontal span of pixels that must be texture mapped.
	linear texture mapping:
		A fast approximation to texture mapping which is continuous but not
		perspective correct.  Linear texture mapping is a good approximation for
		small polygons and polygons whose z-gradients are small.
	gouraud shading:
		A simple method of polygon shading in which brightness values are set
		at each polygon vertex, and are linear interpolated across the polygon.

	Revision history:
		* Created by Tim Sweeney.
		* Rewritten by Tim Sweeney, 10-27-96, for color depth support and speed.
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Subsystem definition.
-----------------------------------------------------------------------------*/

//
// Per-polygon setup info.
//warning: Mirrored in UnRender.inc.
//
struct FPolySpanTextureInfo : public FPolySpanTextureInfoBase
{
	// Variables.
	QWORD		VMask;
	FDitherSet	*DitherSet;
	INT			AndMask;
	RAINBOW_PTR	TextureData;
	RAINBOW_PTR	BlenderData;

	// Valid while locked.
	UTexture	 *Texture;
	FTextureInfo TextureInfo;

	// Drawing functions.
	void DrawFlat_8P	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);
	void DrawFlat_16P	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);
	void DrawFlat_32P	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);

	void DrawNormal_8P  (QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);
	void DrawNormal_16P (QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);
	void DrawNormal_32P (QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);

	void DrawMasked_8P  (QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);
	void DrawMasked_16P (QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);
	void DrawMasked_32P (QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);

	void DrawBlended_8P (QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);
	void DrawBlended_16P(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);
	void DrawBlended_32P(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line);

	// Table of drawing functions:
	static const DRAW_SPAN DrawFuncTable[4][8];
};

//
// Span texture mapper definition.
//warning: Mirrored in UnRender.inc.
//
class FGlobalSpanTextureMapper : public FGlobalSpanTextureMapperBase
{
public:
	// FGlobalSpanTextureMapperBase overrides.
	FPolySpanTextureInfoBase *SetupForPoly
	(
		UCamera			*Camera,
		UTexture		*ThisTexture,
		AZoneInfo		*ZoneInfo,
		DWORD			ThesePolyFlags,
		DWORD			NotPolyFlags
	);
	void FinishPoly(FPolySpanTextureInfoBase *Info);
};

/*-----------------------------------------------------------------------------
	FGlobalSpanTextureMapper statics.
-----------------------------------------------------------------------------*/

const FPolySpanTextureInfoBase::DRAW_SPAN
	FPolySpanTextureInfo::DrawFuncTable[4][8] =
{
	{	// Flatshaded routines:
		NULL,												// 0-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawFlat_8P,		// 8-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawFlat_16P,		// 16-bit color
		NULL,												// 24-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawFlat_32P		// 32-bit color
	},
	{	// Normal routines:
		NULL,												// 0-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawNormal_8P,		// 8-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawNormal_16P,	// 16-bit color
		NULL,												// 24-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawNormal_32P		// 32-bit color
	},
	{	// Masked routines:
		NULL,												// 0-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawMasked_8P,		// 8-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawMasked_16P,	// 16-bit color
		NULL,												// 24-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawMasked_32P		// 32-bit color
	},
	{	// Blended routines:
		NULL,												// 0-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawBlended_8P,	// 8-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawBlended_16P,	// 16-bit color
		NULL,												// 24-bit color
		(DRAW_SPAN)FPolySpanTextureInfo::DrawBlended_32P	// 32-bit color
	}
};

/*-----------------------------------------------------------------------------
	FGlobalSpanTextureMapper implementation.
-----------------------------------------------------------------------------*/

//
// Generate setup info for a polygon on the specified memory pool.
//
FPolySpanTextureInfoBase*
FGlobalSpanTextureMapper::SetupForPoly
(
	UCamera		*Camera,
	UTexture	*ThisTexture,
	AZoneInfo	*ZoneInfo,
	DWORD		ThesePolyFlags,
	DWORD		NotPolyFlags
)
{
	guard(FGlobalSpanTextureMapper::SetupForPoly);

	// Get some memory for the resulting setup info.
	FPolySpanTextureInfo* Info;
	Info = new(GMem)FPolySpanTextureInfo;

	// Make sure the texture is valid.
	Info->Texture = ThisTexture ? ThisTexture : GGfx.DefaultTexture;

	// Figure out the texture mipmapping level.
	Info->Texture->Lock( Info->TextureInfo, Camera->Texture, TL_RenderRamp );
	Info->TextureData.PtrBYTE = Info->TextureInfo.Mips[0]->Data;

	// Set up tiling.
	Info->AndMask = GRender.Extra3 ? 0 : ((Info->TextureInfo.Mips[0]->VSize-1) + ((Info->TextureInfo.Mips[0]->USize-1) << (32-Info->Texture->UBits)));
	Info->VMask   = (QWORD)(Info->TextureInfo.Mips[0]->VSize-1) << Info->Texture->UBits;
	Info->UBits   = Info->Texture->UBits;
	Info->VBits	  = Info->Texture->VBits;

	// Set up the poly flags.
	DWORD PolyFlags = (Info->Texture->PolyFlags & ~NotPolyFlags) | ThesePolyFlags;

	// Set up dithering.
	FDitherTable *DitherBase;
	if (GRender.DoDither && !(PolyFlags & PF_NoSmooth )) DitherBase = &GDither256[GRender.TemporalIter&3];
	else DitherBase = &GNoDither256;

	Info->DitherSet = &(*DitherBase)[Info->Texture->UBits];

	// Figure out special effects.
	int DrawKind;
	if( !(PolyFlags & PF_Transparent) )
	{
		if (!(PolyFlags & PF_Masked))	DrawKind = FRender::DRAWRASTER_Normal;
		else							DrawKind = FRender::DRAWRASTER_Masked;
	}
	else DrawKind = FRender::DRAWRASTER_Blended;

	// Get function pointer to perform span texture mapping:
	Info->DrawFunc = 
		(FPolySpanTextureInfoBase::DRAW_SPAN)
		Info->DrawFuncTable[DrawKind][Camera->ColorBytes];

	// Return the setup info.
	return Info;

	unguard;
}

//
// Finish rendering a poly.
//
void FGlobalSpanTextureMapper::FinishPoly
(
	FPolySpanTextureInfoBase *ThisInfo
)
{
	guard(FGlobalSpanTextureMapper::FinishPoly);

	// Get pointer.
	FPolySpanTextureInfo *Info = (FPolySpanTextureInfo *)ThisInfo;

	// Unlock stuff.
	Info->Texture->Unlock(Info->TextureInfo);

	unguard;
}

/*-----------------------------------------------------------------------------
	Texture loops.
-----------------------------------------------------------------------------*/

//
// Flatshaded.
//

void FPolySpanTextureInfo::DrawFlat_8P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
	while ( Pixels-- > 0 )
	{
		*Dest.PtrBYTE++ = TextureInfo.Colors.PtrBYTE
		[
			(int)(Start&0x3f00)
		+	(int)TextureData.PtrBYTE
			[
				((Start >> (64-UBits))        )+
				((Start >> (32-UBits)) & VMask)
			]
		];
		Start += Inc;
	}
}

void FPolySpanTextureInfo::DrawFlat_16P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
	while ( Pixels-- > 0 )
	{
		*Dest.PtrWORD++ = TextureInfo.Colors.PtrWORD
		[
			(int)(Start&0x3f00)
		+	(int)TextureData.PtrBYTE
			[
				((Start >> (64-UBits))        )+
				((Start >> (32-UBits)) & VMask)
			]
		];
		Start += Inc;
	}
}

void FPolySpanTextureInfo::DrawFlat_32P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
	while ( Pixels-- > 0 )
	{
		*Dest.PtrDWORD++ = TextureInfo.Colors.PtrDWORD
		[
			(int)(Start&0x3f00)
		+	(int)TextureData.PtrBYTE
			[
				((Start >> (64-UBits))        )+
				((Start >> (32-UBits)) & VMask)
			]
		];
		Start += Inc;
	}
}

//
// Normal.
//

void FPolySpanTextureInfo::DrawNormal_8P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
	while ( Pixels-- > 0 )
	{
		*Dest.PtrBYTE++ = TextureInfo.Colors.PtrBYTE
		[
			(int)(Start&0x3f00)
		+	(int)TextureData.PtrBYTE
			[
				((Start >> (64-UBits))        )+
				((Start >> (32-UBits)) & VMask)
			]
		];
		Start += Inc;
	}
}

void FPolySpanTextureInfo::DrawNormal_16P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
	while ( Pixels-- > 0 )
	{
		*Dest.PtrWORD++ = TextureInfo.Colors.PtrWORD
		[
			(int)(Start&0x3f00)
		+	(int)TextureData.PtrBYTE
			[
				((Start >> (64-UBits))        )+
				((Start >> (32-UBits)) & VMask)
			]
		];
		Start += Inc;
	}
}

void FPolySpanTextureInfo::DrawNormal_32P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
	while ( Pixels-- > 0 )
	{
		*Dest.PtrDWORD++ = TextureInfo.Colors.PtrDWORD
		[
			(int)(Start&0x3f00)
		+	(int)TextureData.PtrBYTE
			[
				((Start >> (64-UBits))        )+
				((Start >> (32-UBits)) & VMask)
			]
		];
		Start += Inc;
	}
}

//
// Masked.
//

void FPolySpanTextureInfo::DrawMasked_8P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
	while ( Pixels-- > 0 )
	{
		BYTE B = TextureData.PtrBYTE
		[
			((Start >> (64-UBits))        )
		+	((Start >> (32-UBits)) & VMask)
		];
		if( B ) *Dest.PtrBYTE = TextureInfo.Colors.PtrBYTE
		[
			(int)(B)
		+	(int)(Start&0x3f00)
		];
		Dest.PtrBYTE++;
		Start += Inc;
	}
}

void FPolySpanTextureInfo::DrawMasked_16P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
	while ( Pixels-- > 0 )
	{
		BYTE B = TextureData.PtrBYTE
		[
			((Start >> (64-UBits))        )
		+	((Start >> (32-UBits)) & VMask)
		];
		if( B ) *Dest.PtrWORD = TextureInfo.Colors.PtrWORD
		[
			(int)(Start&0x3f00)
		+	(int)(B)
		];
		Dest.PtrWORD++;
		Start += Inc;
	}
}

void FPolySpanTextureInfo::DrawMasked_32P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
	while ( Pixels-- > 0 )
	{
		BYTE B = TextureData.PtrBYTE
		[
			((Start >> (64-UBits))        )
		+	((Start >> (32-UBits)) & VMask)
		];
		if( B ) *Dest.PtrDWORD = TextureInfo.Colors.PtrDWORD
		[
			(int)(Start&0x3f00)
		+	(int)(B)
		];
		Dest.PtrDWORD++;
		Start += Inc;
	}
}

//
// Blended.
//

void FPolySpanTextureInfo::DrawBlended_8P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
	while ( Pixels-- > 0 )
	{
		BYTE B = TextureData.PtrBYTE
		[
			((Start >> (64-UBits))        )
		+	((Start >> (32-UBits)) & VMask)
		];
		if( B ) *Dest.PtrBYTE = BlenderData.PtrBYTE
			[
			((int)*Dest.PtrBYTE << 8)
		+	TextureInfo.Colors.PtrBYTE
			[
				(int)(B)
			+	(int)(Start&0x3f00)
			]
		];		
		Dest.PtrBYTE++;
		Start += Inc;
	}
}

void FPolySpanTextureInfo::DrawBlended_16P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
}

void FPolySpanTextureInfo::DrawBlended_32P
	(QWORD Start,QWORD Inc,RAINBOW_PTR Dest,INT Pixels,INT Line)
{
}

/*-----------------------------------------------------------------------------
	Instantiation.
-----------------------------------------------------------------------------*/

FGlobalSpanTextureMapper     GSpanTextureMapperInstance;
FGlobalSpanTextureMapperBase *GSpanTextureMapper=&GSpanTextureMapperInstance;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
