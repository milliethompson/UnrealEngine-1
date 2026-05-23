/*=============================================================================
	UnDither.cpp: Unreal texture and illumination dithering setup.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	Sets up Unreal's global dithering tables.

Definitions:
	texture dithering:
		The process of adding an ordered dither offset to the (u,v) coordinates
		of a texture while performing texture mapping in order to eliminate the
		Large Square Pixel Syndrome.  Unreal's ordered dither kernel is 2x4.
		The result of texture dithering is a random discrete blurry appearance.
		This can be viewed as a statistically good approximation to bilinear 
		interpolation.
	illumination dithering:
		Adding an ordered dither offset to the lighting value of each pixel
		during texture mapping.

	Revision history:
		* Created by Tim Sweeney.
		* Rewritten by Tim Sweeney, 10-27-96, for color depth support and speed.
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Dither variables & functions
-----------------------------------------------------------------------------*/

FDitherTable GDither256,GBlur256,GNoDither256;

void BuildDitherTable (FDitherSet *G, int UBits, FDitherOffsets &Offsets,FDitherOffsets &OtherOffsets)
	{
	FDitherOffsets *Ofs = &Offsets;
	int Mip,Line,Pixel;
	for (Mip=0; Mip<8; Mip++)
		{
		for (Line=0; Line<4; Line++)
			{
			for (Pixel=0; Pixel<2; Pixel++)
				{
				G->Pair[Mip][Line][Pixel].Offset =	
					((QWORD)((Ofs->G[Line][Pixel] >> (8           )) & (0x0000ffff))   ) +
					((QWORD)((Ofs->V[Line][Pixel] << (16-Mip      )) & (0xffff0000))   ) +
					((QWORD)((Ofs->V[Line][Pixel] >> (16+Mip      )) & 0x3ff) << 32) +
					((QWORD)((Ofs->U[Line][Pixel] << (16-Mip-UBits)) &~0x3ff) << 32);
				};
			for (Pixel=0; Pixel<2; Pixel++)
				{
				G->Pair[Mip][Line][Pixel].Delta =
					G->Pair[Mip][Line][(Pixel+1)&1].Offset - 
					G->Pair[Mip][Line][Pixel].Offset;
				};
			};
		//Was: Ofs = &OtherOffsets; /* To inhibit dithering on mipmaps */
		};
	};

void InitDither()
	{
	guard(InitDither);
	FDitherOffsets NoDitherOffsets = // Table for no texture dithering, only light dithering:
		{
			{	// U256Offsets:
				{+0x0000,+0x0000},
				{+0x0000,+0x0000},
				{+0x0000,+0x0000},
				{+0x0000,+0x0000}
			},
			{	// V256Offsets:	
				{+0x0000,+0x0000},
				{+0x0000,+0x0000},
				{+0x0000,+0x0000},
				{+0x0000,+0x0000}
			},		
			{	// G256Offsets:
				{+0x2000,+0x8000},
				{+0xC000,+0x6000},
				{+0x0000,+0xA000},
				{+0xE000,+0x4000}
			},
		};
	FDitherOffsets AbsolutelyNoDitherOffsets = // Table for absolutely no dithering
		{
			{	// U256Offsets:
				{+0x0000,+0x0000},
				{+0x0000,+0x0000},
				{+0x0000,+0x0000},
				{+0x0000,+0x0000}
			},
			{	// V256Offsets:	
				{+0x0000,+0x0000},
				{+0x0000,+0x0000},
				{+0x0000,+0x0000},
				{+0x0000,+0x0000}
			},		
			{	// G256Offsets:
				{+0x0000,+0x0000},
				{+0x0000,+0x0000},
				{+0x0000,+0x0000},
				{+0x0000,+0x0000}
			},
		};
	FDitherOffsets DitherOffsets = // Regular texture and light dithering:
		{		
/* Old: Overly messy due to 4-line texture dither pattern
			{	// U256Offsets:
				{+0x0000,+0x6200},
				{+0xEC00,+0x5000},
				{+0x1300,+0x7300},
				{+0xD200,+0x2A00}
			},		
			{	// V256Offsets:
				{+0xD700,+0x3200},
				{+0x0020,+0x7300},
				{+0xA500,+0x4300},
				{+0x0E00,+0x6F00}
			},
			{	// G256Offsets:
				{+0x2000,+0x8000},
				{+0xC000,+0x6000},
				{+0x0000,+0xA000},
				{+0xE000,+0x4000}
			},
*/
			{	// U256Offsets:
				{+0x0000,+0x6200},
				{+0xEC00,+0x5000},
				{+0x0000,+0x6200},
				{+0xEC00,+0x5000},
			},		
			{	// V256Offsets:
				{+0xD700,+0x3200},
				{+0x0020,+0x7300},
				{+0xD700,+0x3200},
				{+0x0020,+0x7300},
			},
			{	// G256Offsets:
				{+0x2000,+0x8000},
				{+0xC000,+0x6000},
				{+0x0000,+0xA000},
				{+0xE000,+0x4000}
			},
		};
	FDitherOffsets BlurOffsets = // Texture blurring and light dithering:
		{
			{	// U256Offsets:
				{+16*0x0000,+16*0x6200},
				{+16*0xEC00,+16*0x5000},
				{+16*0x1300,+16*0x7300},
				{+16*0xD200,+16*0x2A00}
			},		
			{	// V256Offsets:
				{+16*0xD700,+16*0x3200},
				{+16*0x0020,+16*0x7300},
				{+16*0xA500,+16*0x4300},
				{+16*0x0E00,+16*0x6F00}
			},		
			{	// G256Offsets:
				{+0x2000,+0x8000},
				{+0xC000,+0x6000},
				{+0x0000,+0xA000},
				{+0xE000,+0x4000}
			},
		};
	//
	// Build dither tables:
	//
	for (int UBits=0; UBits<16; UBits++)
		{
		BuildDitherTable(&GDither256	[UBits],UBits,DitherOffsets,	NoDitherOffsets);
		BuildDitherTable(&GBlur256		[UBits],UBits,BlurOffsets,		BlurOffsets);
		BuildDitherTable(&GNoDither256	[UBits],UBits,NoDitherOffsets,	NoDitherOffsets);
		};
	unguard;
	};

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
