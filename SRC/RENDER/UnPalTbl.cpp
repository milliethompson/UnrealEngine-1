/*=============================================================================
	UnPalTbl.cpp: Unreal palette table manipulation code.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	Manages palette lighting lookup tables in non-MMX modes where a ramp
	lighting system is used.

Revision history:
	* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Palette lighting table support.
-----------------------------------------------------------------------------*/

//
// Return the palette lighting table which is appropriate
// for the given palette, zone, and colordepth.
//
// If the CacheItem return is non-NULL, you must Unlock()
// it when you're done using the table.
//
RAINBOW_PTR FGlobalRender::GetPaletteLightingTable
(
	ICamera				&Camera,
	UPalette			*Palette,
	AZoneDescriptor		*ZoneDescriptor,
	ALevelDescriptor	*LevelDescriptor,
	FCacheItem			*&CacheItem
)
{
	guard(GetPaletteLightingTable);

	if( Camera.ColorBytes==1 )
	{
		//
		// 256-color hardware paletized.
		// There is only one palette scaler, and it applies to all zones
		// and palettes.
		//

		CacheItem = NULL;
		return GGfx.ShadeData;
	}

	//
	// Truecolor or hicolor software paletized.
	//

	// Figure out an appropriate zone number.
	int		iZone;
	if		( ZoneDescriptor  ) iZone = ZoneDescriptor->Zone;
	else if	( LevelDescriptor ) iZone = 255;
	else						iZone = 0;

	// Get the palette from the cache.
	int		PaletteCacheID  = MakeCacheID(CID_LightingTable,Palette->Index,(iZone<<2)+(Camera.ColorBytes-1));
	int		Align			= Camera.ColorBytes * 65536;
	RAINBOW_PTR	Result		= GCache.Get( PaletteCacheID, CacheItem, Align );

	// See if the lighting has changed.
	int LightChanged = 0;
	if( ZoneDescriptor )
	{
		LightChanged = ZoneDescriptor->bTempLightChanged;
	}
	else if( LevelDescriptor )
	{
		LightChanged = LevelDescriptor->bTempLightChanged;
	}

	if( Result.PtrVOID && !LightChanged )
	{
		// Already in cache.
		return Result;
	}

	//
	// We need to regenerate the palette lookup table.
	//
	STAT(clock(GStat.PalTime));

	int IsWaterZone			= ZoneDescriptor && ZoneDescriptor->bWaterZone;
	int IsFogZone			= ZoneDescriptor && ZoneDescriptor->bFogZone;
	int ZoneScalerCacheID	= MakeCacheID(CID_ZoneScaler,iZone,Camera.ColorBytes);
	FColor	*FirstColor		= &Palette->Element(0);
	FColor	*LastColor		= &Palette->Element(256);

	FCacheItem	*ZoneScalerCacheItem;
	FVector Fog,FogDelta,Base,Scale,BaseDelta;

	RAINBOW_PTR ZoneScaler = GCache.Get( ZoneScalerCacheID, ZoneScalerCacheItem );

	// Set up one-per-zone palette scaler info for all texture palettes in this zone
	Base  = GMath.ZeroVector;
	Fog   = GMath.ZeroVector;
	Scale = GMath.UnitVector;

	if( ZoneDescriptor )
	{
		GGfx.RGBtoHSV(Base, ZoneDescriptor->AmbientHue,ZoneDescriptor->AmbientSaturation,ZoneDescriptor->AmbientBrightness,Camera.ColorBytes);
		GGfx.RGBtoHSV(Scale,ZoneDescriptor->RampHue,ZoneDescriptor->RampSaturation,255,Camera.ColorBytes);
	}
	else if( LevelDescriptor )
	{
		GGfx.RGBtoHSV(Fog, LevelDescriptor->SkyFogHue,LevelDescriptor->SkyFogSaturation,LevelDescriptor->SkyFogBrightness,Camera.ColorBytes);
	}
	if( IsWaterZone )
	{
		Scale.R = Scale.R * 0.5;
		Scale.G = Scale.G * 0.5;
	}

	if( !IsFogZone )
	{
		// Unfogged zone.
		FLOAT Gamma = 0.5 + 5.0 * GGfx.GammaLevel / GGfx.NumGammaLevels;
		BaseDelta	= Gamma * 0x10000 * (Scale/64.0);
		Base		= Gamma * 0x10000 * Base * 0.6;
		Fog         = Fog   * 0x10000 * 0x120;
		FogDelta	= -Fog  / 64.0;
	}
	else
	{
		// Fogged zone.
		FLOAT Gamma = 0.5 + 5.0 * GGfx.GammaLevel / GGfx.NumGammaLevels;
		BaseDelta	= -Gamma * 0x10000 * (Scale * 0.2/62.0);
		Base		= Gamma * 0x10000 * Scale * 0.2 + Gamma * 0x10000 * Base * 0.6;
		Fog         = GMath.ZeroVector;
		FogDelta	= Scale * 0.6 * 0x10000 * 0x120 / 64.0;
	}

	// Handle the specific color depth.
	if( Camera.ColorBytes==2 )
	{
		if( !ZoneScaler.PtrVOID || LightChanged )
		{
			// Build one-per-zone palette scaler.
			if ( !ZoneScaler.PtrVOID )
			{
				ZoneScaler = GCache.Create(ZoneScalerCacheID,ZoneScalerCacheItem,64*256*3*2);
			}
			WORD *Ptr = ZoneScaler.PtrWORD;

			for( int i=0; i<63; i++ )
			{
				int R = Fog.R, DR = Base.R;
				int G = Fog.G, DG = Base.G;
				int B = Fog.B, DB = Base.B;

				if( Camera.Caps & CC_RGB565 )
				{
					// RGB 5-6-5.
					for( int j=0; j<256; j++ )
					{
						Ptr[0*256*64] = (R & 0xf80000) >> 8;
						Ptr[1*256*64] = (G & 0xfc0000) >> 13;
						Ptr[2*256*64] = (B & 0xf80000) >> 19;
						Ptr++;

						R+=DR; if (R>0xfff000) {R=0xfff000; DR=0;};
						G+=DG; if (G>0xfff000) {G=0xfff000; DG=0;};
						B+=DB; if (B>0xfff000) {B=0xfff000; DB=0;};
					}
				}
				else
				{
					// RGB 5-5-5.
					for( int j=0; j<256; j++ )
					{
						Ptr[0*256*64] = (R & 0xf80000) >> 9;
						Ptr[1*256*64] = (G & 0xf80000) >> 14;
						Ptr[2*256*64] = (B & 0xf80000) >> 19;
						Ptr++;

						R+=DR; if (R>0xfff000) {R=0xfff000; DR=0;};
						G+=DG; if (G>0xfff000) {G=0xfff000; DG=0;};
						B+=DB; if (B>0xfff000) {B=0xfff000; DB=0;};
					}
				}
				Base += BaseDelta;
				Fog  += FogDelta;
			}
		}

		// Build texture palette lookup table.
		if( !Result.PtrVOID || LightChanged )
		{
			if ( !Result.PtrVOID )
			{
				Result = GCache.Create(PaletteCacheID,CacheItem,256*64*2,Align);
			}
			WORD *HiColor = Result.PtrWORD;

			for( int j=0; j<64; j++)
			{
				FColor *C = FirstColor;
				do	{
					*HiColor++ =
					+	ZoneScaler.PtrWORD[C->Red   + 0*256*64]
					+	ZoneScaler.PtrWORD[C->Green + 1*256*64]
					+	ZoneScaler.PtrWORD[C->Blue  + 2*256*64]
					;
				} while( ++C < LastColor );

				ZoneScaler.PtrWORD += 256;
			}
		}
	}
	else if( (Camera.ColorBytes==3) || (Camera.ColorBytes==4) )
	{
		// RGB 8-8-8.
		if( !ZoneScaler.PtrVOID || LightChanged )
		{
			// Build one-per-zone palette scaler.
			if( !ZoneScaler.PtrVOID )
			{
				ZoneScaler = GCache.Create(ZoneScalerCacheID,ZoneScalerCacheItem,64*256*3*4);
			}
			DWORD *Ptr = ZoneScaler.PtrDWORD;

			for( int i=0; i<63; i++ )
			{
				int R = Fog.R, DR = Base.R;
				int G = Fog.G, DG = Base.G;
				int B = Fog.B, DB = Base.B;

				for( int j=0; j<256; j++ )
				{
					Ptr[0*256*64] = (R & 0xff0000) >> 0;
					Ptr[1*256*64] = (G & 0xff0000) >> 8;
					Ptr[2*256*64] = (B & 0xff0000) >> 16;
					Ptr++;

					R+=DR; if (R>0xfff000) {R=0xfff000; DR=0;};
					G+=DG; if (G>0xfff000) {G=0xfff000; DG=0;};
					B+=DB; if (B>0xfff000) {B=0xfff000; DB=0;};
				}
				Base += BaseDelta;
				Fog  += FogDelta;
			}
		}

		// Build texture palette lookup table.
		if( !Result.PtrVOID || LightChanged )
		{
			if( !Result.PtrVOID )
			{
				Result = GCache.Create(PaletteCacheID,CacheItem,256*64*4,Align);
			}
			DWORD *TrueColor = Result.PtrDWORD;

			for( int j=0; j<64; j++ )
			{
				FColor *C = FirstColor;
				do	{
					*TrueColor++ =
					+	ZoneScaler.PtrDWORD[C->Red   + 0*256*64]
					+	ZoneScaler.PtrDWORD[C->Green + 1*256*64]
					+	ZoneScaler.PtrDWORD[C->Blue  + 2*256*64]
					;
				} while( ++C < LastColor );

				ZoneScaler.PtrDWORD += 256;
			}
		}
		else
		{
			appErrorf("Invalid color depth %i",Camera.ColorBytes);
		}
	}
	ZoneScalerCacheItem->Unlock();
	STAT(unclock(GStat.PalTime));

	return Result;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
