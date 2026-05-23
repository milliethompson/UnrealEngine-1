/*=============================================================================
	UnPalTbl.cpp: Unreal palette table manipulation code.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
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
RAINBOW_PTR FRender::GetPaletteLightingTable
(
	UTexture			*Camera,
	UPalette			*Palette,
	AZoneInfo			*ZoneInfo,
	ALevelInfo			*LevelInfo,
	FCacheItem			*&CacheItem
)
{
	guard(GetPaletteLightingTable);
	checkInput(Palette!=NULL);

	// Figure out an appropriate zone number.
	int		iZone;
	if		( ZoneInfo        ) iZone = ZoneInfo->ZoneNumber;
	else if	( LevelInfo       ) iZone = 255;
	else						iZone = 254;

	// Get the palette from the cache.
	int		PaletteCacheID  = MakeCacheID(CID_LightingTable,Palette->GetIndex(),(iZone<<2)+(Camera->ColorBytes-1));
	int		Align			= Camera->ColorBytes * 65536;
	RAINBOW_PTR	Result		= GCache.Get( PaletteCacheID, CacheItem, Align );

	// See if the lighting has changed.
	int LightChanged = 0;
	if( ZoneInfo )
	{
		LightChanged = ZoneInfo->bLightChanged;
	}
	else if( LevelInfo )
	{
		LightChanged = LevelInfo->bLightChanged;
	}

	if( Result.PtrVOID && !LightChanged )
	{
		// Already in cache.
		return Result;
	}

	// We need to regenerate the palette lookup table.
	STAT(clock(GStat.PalTime));

	int IsWaterZone			= ZoneInfo && ZoneInfo->bWaterZone;
	int IsFogZone			= ZoneInfo && ZoneInfo->bFogZone;
	int ZoneScalerCacheID	= MakeCacheID(CID_ZoneScaler,iZone,Camera->ColorBytes);

	FCacheItem	*ZoneScalerCacheItem;
	FVector Fog,FogDelta,Base,Scale,BaseDelta;

	RAINBOW_PTR ZoneScaler = GCache.Get( ZoneScalerCacheID, ZoneScalerCacheItem );

	// Set up one-per-zone palette scaler info for all texture palettes in this zone
	Base  = GMath.ZeroVector;
	Fog   = GMath.ZeroVector;
	Scale = GMath.UnitVector;

	if( ZoneInfo )
	{
		Base.GetHSV( ZoneInfo->AmbientHue, ZoneInfo->AmbientSaturation, ZoneInfo->AmbientBrightness, Camera->ColorBytes );
		Scale.GetHSV( ZoneInfo->RampHue, ZoneInfo->RampSaturation, 255, Camera->ColorBytes );
		if( ZoneInfo->bFogZone )
			Fog.GetHSV( ZoneInfo->FogHue, ZoneInfo->FogSaturation, ZoneInfo->FogThickness, Camera->ColorBytes );
	}
	else if( LevelInfo )
	{
		Fog.GetHSV  (LevelInfo->SkyFogHue,LevelInfo->SkyFogSaturation,LevelInfo->SkyFogBrightness,Camera->ColorBytes);
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
		Fog        += Base * 64.0;
	}
	else
	{
		// Fogged zone.
		FLOAT Gamma = 0.5 + 5.0 * GGfx.GammaLevel / GGfx.NumGammaLevels;
		BaseDelta	= -Gamma * 0x10000 * (Fog * 0.2/62.0);
		Base		= Gamma * 0x10000 * Fog * 0.2 + Gamma * 0x10000 * Base * 0.6;
		FogDelta	= Fog * 0.6 * 0x10000 * 0x120 / 64.0;
		Fog         = GMath.ZeroVector;
	}

	// Prepare palette.
	Palette->Lock(LOCK_Read);
	FColor	*FirstColor		= &Palette->Element(0);
	FColor	*LastColor		= &Palette->Element(256);

	// Handle the specific color depth.
	if( Camera->ColorBytes==1 || Camera->ColorBytes==2 )
	{
		if( !ZoneScaler.PtrVOID || LightChanged )
		{
			// Build one-per-zone palette scaler.
			if ( !ZoneScaler.PtrVOID )
				ZoneScaler = GCache.Create(ZoneScalerCacheID,ZoneScalerCacheItem,64*256*2);

			WORD *Ptr = ZoneScaler.PtrWORD;
			for( int i=0; i<63; i++ )
			{
				int R = Fog.R, DR = Base.R;
				int G = Fog.G, DG = Base.G;
				int B = Fog.B, DB = Base.B;

				// This creates a cool overcast, somewhat radiosity inspired effect:
#if 0
				FLOAT Alpha = i/63.0;
				B = B * (1.0-Alpha) + 0xfff000 * Alpha;
				G = G * (1.0-Alpha) + 0xfff000 * Alpha;
				R = R * (1.0-Alpha) + 0xfff000 * Alpha;
#endif

#if 0
				if( i > 48 && !IsFogZone )
				{
					FLOAT Alpha = Square((i-48.0)/15.0);
					R = R * (1.0-Alpha) + 0xfff000 * Alpha;
					G = G * (1.0-Alpha) + 0xfff000 * Alpha;
					B = B * (1.0-Alpha) + 0xfff000 * Alpha;
				}
#endif

				if( (Camera->CameraCaps&CC_RGB565) || (Camera->ColorBytes==1) )
				{
					// RGB 5-6-5.
					for( int j=0; j<256; j++ )
					{
						*Ptr++ =
						+	((R & 0xf80000) >>  8)
						+	((G & 0xfc0000) >> 13)
						+	((B & 0xf80000) >> 19);

						R+=DR; if (R>0xfff000) {R=0xfff000; DR=0;}
						G+=DG; if (G>0xfff000) {G=0xfff000; DG=0;}
						B+=DB; if (B>0xfff000) {B=0xfff000; DB=0;}
					}
				}
				else
				{
					// RGB 5-5-5.
					for( int j=0; j<256; j++ )
					{
						*Ptr++ = 
						+	((R & 0xf80000) >>  9)
						+	((G & 0xf80000) >> 14)
						+	((B & 0xf80000) >> 19);

						R+=DR; if (R>0xfff000) {R=0xfff000; DR=0;};
						G+=DG; if (G>0xfff000) {G=0xfff000; DG=0;};
						B+=DB; if (B>0xfff000) {B=0xfff000; DB=0;};
					}
				}
				Base     += BaseDelta;
				Fog      += FogDelta;
			}
		}

		// Build texture palette lookup table.
		if( !Result.PtrVOID || LightChanged )
		{
			if ( !Result.PtrVOID )
				Result = GCache.Create(PaletteCacheID,CacheItem,256*64*Camera->ColorBytes,Align);

			if( (Camera->ColorBytes==2) && (Camera->CameraCaps&CC_RGB565) )
			{
				// Build 5-6-5 color lighting lookup table.
				WORD *HiColor = Result.PtrWORD;
				for( int j=0; j<64; j++)
				{
					FColor *C = FirstColor;
					do	{
						*HiColor++ =
						+	(ZoneScaler.PtrWORD[C->R] & FColor::EHiColor565_R )
						+	(ZoneScaler.PtrWORD[C->G] & FColor::EHiColor565_G )
						+	(ZoneScaler.PtrWORD[C->B] & FColor::EHiColor565_B )
						;
					} while( ++C < LastColor );
					ZoneScaler.PtrWORD += 256;
				}
			}
			else if( Camera->ColorBytes==2 )
			{
				// Build 5-5-5 color lighting lookup table.
				WORD *HiColor = Result.PtrWORD;
				for( int j=0; j<64; j++)
				{
					FColor *C = FirstColor;
					do	{
						*HiColor++ =
						+	(ZoneScaler.PtrWORD[C->R] & FColor::EHiColor555_R )
						+	(ZoneScaler.PtrWORD[C->G] & FColor::EHiColor555_G )
						+	(ZoneScaler.PtrWORD[C->B] & FColor::EHiColor555_B )
						;
					} while( ++C < LastColor );
					ZoneScaler.PtrWORD += 256;
				}
			}
			else
			{
				// Build palette index lighting lookup by computing 5-6-5 colors
				// and using the remap table to remap them to their nearest
				// palette indices.
				GGfx.RemapTable->Lock(LOCK_Read);
				BYTE *PalColor = Result.PtrBYTE;
				for( int j=0; j<64; j++)
				{
					FColor *C = FirstColor;
					do	{
						*PalColor++ = GGfx.RemapTable
						(
						+	(ZoneScaler.PtrWORD[C->R] & FColor::EHiColor565_R )
						+	(ZoneScaler.PtrWORD[C->G] & FColor::EHiColor565_G )
						+	(ZoneScaler.PtrWORD[C->B] & FColor::EHiColor565_B )
						);
					} while( ++C < LastColor );
					ZoneScaler.PtrWORD += 256;
				}
				GGfx.RemapTable->Unlock(LOCK_Read);
			}
		}
	}
	else
	{
		// RGB 8-8-8.
		if( !ZoneScaler.PtrVOID || LightChanged )
		{
			// Build one-per-zone palette scaler.
			if( !ZoneScaler.PtrVOID )
				ZoneScaler = GCache.Create(ZoneScalerCacheID,ZoneScalerCacheItem,64*256*4);

			DWORD *Ptr = ZoneScaler.PtrDWORD;
			for( int i=0; i<63; i++ )
			{
				int R = Fog.R, DR = Base.R;
				int G = Fog.G, DG = Base.G;
				int B = Fog.B, DB = Base.B;

				for( int j=0; j<256; j++ )
				{
					*Ptr++ =
					+	((R & 0xff0000) >>  0)
					+	((G & 0xff0000) >>  8)
					+	((B & 0xff0000) >> 16);

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
				Result = GCache.Create(PaletteCacheID,CacheItem,256*64*Camera->ColorBytes,Align);

			DWORD *TrueColor = Result.PtrDWORD;
			for( int j=0; j<64; j++ )
			{
				FColor *C = FirstColor;
				do	{
					*TrueColor++ =
					+	(ZoneScaler.PtrDWORD[C->R] & FColor::ETrueColor_R )
					+	(ZoneScaler.PtrDWORD[C->G] & FColor::ETrueColor_G )
					+	(ZoneScaler.PtrDWORD[C->B] & FColor::ETrueColor_B )
					;
				} while( ++C < LastColor );
				ZoneScaler.PtrDWORD += 256;
			}
		}
	}
	ZoneScalerCacheItem->Unlock();
	Palette->Unlock(LOCK_Read);
	STAT(unclock(GStat.PalTime));

	return Result;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
