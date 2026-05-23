/*=============================================================================
	UnCId.h: Cache Id's for all global Unreal objects.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNCID
#define _INC_UNCID

/*----------------------------------------------------------------------------
	Cache ID's.
----------------------------------------------------------------------------*/

//
// 8-bit base Cache ID's. These exist so that objects owned by various 
// subsystems who use the memory cache don't collide.
//
enum ECacheIDBase
{
	CID_ColorDepthPalette	= 0x11000000, // A colordepth palette.
	CID_RemappedTexture		= 0x12000000, // A remapped texture.
	CID_LightingTable		= 0x13000000, // A zone/colordepth lighting table.
	CID_ZoneScaler			= 0x14000000, // A zone/colordepth scaling table.
	CID_ShadowMap			= 0x15000000, // A lightmesh shadow map.
	CID_IlluminationMap		= 0x16000000, // A lightmesh illumination map.
	CID_ResultantMap		= 0x17000000, // A surface resulting illumination map.
	CID_StaticMap			= 0x18000000, // A static lighting map.
	CID_MemStackChunk		= 0x19000000, // A chunk allocated by FMemStack.
	CID_3dfxTexture			= 0x1A000000, // A cached texture in the 3dfx code.
	CID_3dfxLightMap		= 0x1B000000, // A cached light map in the 3dfx code.
	CID_AALineTable			= 0x1C000000, // A cached antialiased line drawing table.
	CID_TweenAnim			= 0x1D000000, // A cached animation tween.
	CID_Extra4				= 0x1E000000,
	CID_Extra3				= 0x1F000000,
	CID_Extra2				= 0x20000000,
	CID_Extra1				= 0x21000000,
	CID_Extra0				= 0x22000000,
	CID_MAX					= 0xff000000, // Cache ID mask.
};

/*----------------------------------------------------------------------------
	Functions.
----------------------------------------------------------------------------*/

//
// Make a Cache ID from a base ID, and low part of a DWORD.
//
inline DWORD MakeCacheID(ECacheIDBase Base, DWORD Int24=0 )
{
	return (Base) + (Int24 << 8);
}

//
// Make a Cache ID from a base ID, and a unique Byte/Word pair.
//
inline DWORD MakeCacheID(ECacheIDBase Base, DWORD Word, DWORD Byte)
{
	return (Base) + (Byte << 16) + (Word);
}

//
// Make a CacheID from a base ID and three bytes.
//
inline DWORD MakeCacheID(ECacheIDBase Base, BYTE A, BYTE B, BYTE C)
{
	return (Base) + ((INT)A<<8) + ((INT)B<<16) + ((INT)C<<24);
}

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
#endif // _INC_UNCID
