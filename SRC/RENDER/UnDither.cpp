/*=============================================================================
	UnDither.cpp: Unreal texture and illumination dithering setup.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
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

FDitherTable GDither256[4],GNoDither256;

void BuildDitherTable
(
	FDitherSet		*G, 
	INT				UBits, 
	FDitherOffsets	&Offsets,
	FDitherOffsets	&OtherOffsets
)
{
	FDitherOffsets *Ofs = &Offsets;
	int Mip,Line,Pixel;
	for( Mip=0; Mip<8; Mip++ )
	{
		for( Line=0; Line<4; Line++ )
		{
			for( Pixel=0; Pixel<2; Pixel++ )
			{
				G->Unit[Mip].Pair[Line][Pixel].Offset =	
					((QWORD)(((Ofs->G[Line][Pixel]-0x0000) >> (8           )) & (0x0000ffff))   ) +
					((QWORD)(((Ofs->V[Line][Pixel]-0x8000) << (16-Mip      )) & (0xffff0000))   ) +
					((QWORD)(((Ofs->V[Line][Pixel]-0x8000) >> (16+Mip      )) & 0x3ff) << 32) +
					((QWORD)(((Ofs->U[Line][Pixel]-0x8000) << (16-Mip-UBits)) &~0x3ff) << 32);
			}
			for( Pixel=0; Pixel<2; Pixel++ )
			{
				G->Unit[Mip].Pair[Line][Pixel].Delta =
					G->Unit[Mip].Pair[Line][(Pixel+1)&1].Offset - 
					G->Unit[Mip].Pair[Line][Pixel].Offset;
			}
		}
		//todo: Decide whether to dither mipmaps.
		Ofs = &OtherOffsets; /* To inhibit dithering on mipmaps */
	}
}

void InitDither()
{
	guard(InitDither);
	FDitherOffsets NoDitherOffsets = // Table for no texture dithering, only light dithering:
	{
		{	// U256Offsets:
			{+0x8000,+0x8000},
			{+0x8000,+0x8000},
			{+0x8000,+0x8000},
			{+0x8000,+0x8000}
		},
		{	// V256Offsets:	
			{+0x8000,+0x8000},
			{+0x8000,+0x8000},
			{+0x8000,+0x8000},
			{+0x8000,+0x8000}
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
			{+0x8000,+0x8000},
			{+0x8000,+0x8000},
			{+0x8000,+0x8000},
			{+0x8000,+0x8000}
		},
		{	// V256Offsets:	
			{+0x8000,+0x8000},
			{+0x8000,+0x8000},
			{+0x8000,+0x8000},
			{+0x8000,+0x8000}
		},		
		{	// G256Offsets:
			{+0x0000,+0x0000},
			{+0x0000,+0x0000},
			{+0x0000,+0x0000},
			{+0x0000,+0x0000}
		},
	};
	FDitherOffsets DitherOffsets[4] = // Regular texture and light dithering:
	{
		{	// 1
			{	// U256Offsets:
				{+0xA000,+0x2000},
				{+0xE000,+0x6000},
				{+0x8000,+0x0000},
				{+0xC000,+0x4000},
			},		
			{	// V256Offsets:
				{+0x0000,+0x4000},
				{+0x8000,+0xC000},
				{+0x2000,+0x6000},
				{+0xA000,+0xE000},
			},
			{	// G256Offsets:
				//{+0x2000,+0x8000},
				//{+0xC000,+0x6000},
				//{+0x0000,+0xA000},
				//{+0xE000,+0x4000}
				{+0x04000,+0x10000},
				{+0x18000,+0x0C000},
				{+0x00000,+0x14000},
				{+0x1C000,+0x08000}
			},
		},
		{	// 2
			{	// U256Offsets:
				{+0x0000,+0x4000},
				{+0x8000,+0xC000},
				{+0x2000,+0x6000},
				{+0xA000,+0xE000},
			},
			{	// V256Offsets:
				{+0xA000,+0x2000},
				{+0xE000,+0x6000},
				{+0x8000,+0x0000},
				{+0xC000,+0x4000},
			},		
			{	// G256Offsets:
				{+0x5000,+0xF000},
				{+0xB000,+0x1000},
				{+0x7000,+0xD000},
				{+0x9000,+0x3000}
			}
		},
		{	// 3
			{	// U256Offsets:
				{+0xA000,+0x2000},
				{+0xE000,+0x6000},
				{+0x8000,+0x0000},
				{+0xC000,+0x4000},
			},		
			{	// V256Offsets:
				{+0x0000,+0x4000},
				{+0x8000,+0xC000},
				{+0x2000,+0x6000},
				{+0xA000,+0xE000},
			},
			{	// G256Offsets:
				{+0x2000,+0x8800},
				{+0xC000,+0x6800},
				{+0x0000,+0xA800},
				{+0xE000,+0x4800}
			},
		},
		{	// 4
			{	// U256Offsets:
				{+0x0000,+0x4000},
				{+0x8000,+0xC000},
				{+0x2000,+0x6000},
				{+0xA000,+0xE000},
			},
			{	// V256Offsets:
				{+0xA000,+0x2000},
				{+0xE000,+0x6000},
				{+0x8000,+0x0000},
				{+0xC000,+0x4000},
			},		
			{	// G256Offsets:
				{+0x9800,+0x3000},
				{+0x7800,+0xD000},
				{+0xB800,+0x1000},
				{+0x5800,+0xF000}
			}
		}
	};

	// Build dither tables.
	for( int UBits=0; UBits<16; UBits++ )
	{
		BuildDitherTable(&GNoDither256[UBits],UBits,NoDitherOffsets,NoDitherOffsets);
		for( int i=0; i<4; i++ )
			BuildDitherTable(&GDither256[i][UBits],UBits,DitherOffsets[i],NoDitherOffsets);
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
