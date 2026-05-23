/*=============================================================================
	UnCId.h: Cache Id's for all global Unreal objects.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
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
	CID_Extra9				= 0x19000000,
	CID_Extra8				= 0x1A000000,
	CID_Extra7				= 0x1B000000,
	CID_Extra6				= 0x1C000000,
	CID_Extra5				= 0x1D000000,
	CID_Extra4				= 0x1E000000,
	CID_Extra3				= 0x1F000000,
	CID_Extra2				= 0x20000000,
	CID_Extra1				= 0x21000000,
	CID_Extra0				= 0x22000000,
};

/*----------------------------------------------------------------------------
	Functions.
----------------------------------------------------------------------------*/

//
// Make a Cache ID from a base ID, and a unique Byte/Word pair.
//
inline DWORD MakeCacheID(ECacheIDBase Base, DWORD Word=0, DWORD Byte=0)
{
	return (Base) + (Byte << 16) + (Word);
}

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
#endif // _INC_UNCID
