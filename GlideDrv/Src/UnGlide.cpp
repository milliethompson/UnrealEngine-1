/*=============================================================================
	UnGlide.cpp: Unreal support for the 3dfx Glide library.
	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

// Precompiled header.
#pragma warning( disable:4201 )
#include <windows.h>
#include "Engine.h"
#include "UnRender.h"

// 3dfx Glide includes.
#define __MSC__
#include <Glide.h>

// Globals.
#define PYR(n) ((n)*((n+1))/2)
UBOOL GGlideCheckErrors=1;
_WORD RScale[PYR(128)], GScale[PYR(128)], BScale[PYR(128)];

// Texture upload flags.
enum EGlideFlags
{
	GF_Alpha		= 0x01, // 5551 rgba texture.
	GF_NoPalette    = 0x02, // Non-palettized.
	GF_NoScale      = 0x04, // Scale for precision adjust.
	GF_LightMap		= 0x08,	// Light map.
};

// Pixel formats.
union FGlideColor
{
#if __INTEL__
	struct{ WORD B:5, G:5, R:5, A:1; } Color5551;
	struct{ WORD B:5, G:6, R:5; } Color565;
#else
	struct{ WORD A:1, R:5, G:5, B:5 } Color5551;
	struct{ WORD R:5, G:6, B:5; } Color565;
#endif
};

// Unreal package implementation.
IMPLEMENT_PACKAGE(GlideDrv);

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

//
// Mask a floating point value for Glide.
//
inline FLOAT Mask( FLOAT f )
{
	return f + ( float )( 3 << 18 );
}

/*-----------------------------------------------------------------------------
	Global Glide error handler.
-----------------------------------------------------------------------------*/

//
// Handle a Glide error.
//
void GlideErrorHandler( const char* String, FxBool Fatal )
{
	guard(GlideErrorHandler);
	if( GGlideCheckErrors )
		appErrorf( "Glide error: %s", String );
	unguard;
}

/*-----------------------------------------------------------------------------
	Statistics.
-----------------------------------------------------------------------------*/

//
// Statistics.
//
static struct FGlideStats
{
	// Stat variables.
	INT DownloadsPalette;
	INT Downloads8;
	INT Downloads16;
	INT PolyVTime;
	INT PolyCTime;
	INT PaletteTime;
	INT Download8Time;
	INT Download16Time;
	DWORD Surfs, Polys, Tris;
} Stats;

/*-----------------------------------------------------------------------------
	UGlideRenderDevice definition.
-----------------------------------------------------------------------------*/

//
// The 3dfx Glide rendering device.
//
class DLL_EXPORT UGlideRenderDevice : public URenderDevice
{
	DECLARE_CLASS(UGlideRenderDevice,URenderDevice,CLASS_Config)

	// Constants.
	enum {MAX_TMUS=3};
	enum {ALIGNMENT=8};

	// Variables.
	GrHwConfiguration	hwconfig;
	INT					NumTmu, X, Y;
	FPlane				FlashScale;
	FPlane				FlashFog;
	DWORD				LockFlags;
	DWORD				OldPolyFlags;
	UBOOL				DetailTextures;
	UBOOL				FastUglyRefresh;
	UBOOL				ScreenSmoothing;
	FLOAT				DetailBias;
	BYTE				RefreshRate;

	// UObject interface.
	static void InternalClassInitializer( UClass* Class );
	void PostEditChange();

	// URenderDevice interface.
	UBOOL Init( UViewport* InViewport );
	void Exit();
	void Flush();
	void Lock( FPlane FlashScale, FPlane FlashFog, FPlane ScreenClear, DWORD RenderLockFlags, BYTE* HitData, INT* HitSize );
	void Unlock( UBOOL Blit );
	void DrawComplexSurface( FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet );
	void DrawGouraudPolygon( FSceneNode* Frame, FTextureInfo& Info, FTransTexture** Pts, int NumPts, DWORD PolyFlags, FSpanBuffer* Span );
	void DrawTile( FSceneNode* Frame, FTextureInfo& Info, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, class FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags );
	UBOOL Exec( const char* Cmd, FOutputDevice* Out );
	void Draw2DLine( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2 );
	void Draw2DPoint( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2 );
	void GetStats( char* Result );
	void ClearZ( FSceneNode* Frame );
	void PushHit( const BYTE* Data, INT Count );
	void PopHit( INT Count, UBOOL bForce );
	void ReadPixels( FColor* Pixels );
	void EndFlash();

	// Functions.
	UBOOL TryRes( INT X, INT Y, UBOOL Force, GrScreenRefresh_t Ref );

	// State of a texture map unit.
	struct FGlideTMU
	{
		// Owner.
		UGlideRenderDevice* Glide;

		// Texture map unit.
		INT tmu;

		// Memory cache for the tmu.
		FMemCache Cache;

		// State variables.
		FColor		PaletteMaxColor;
		FColor		MaxColor;
		FCacheItem*	TextureItem;
		QWORD		TextureCacheID;
		QWORD		PaletteCacheID;
		DWORD		GlideFlags;
		FLOAT		UScale, VScale;

		// tmu download functions.
		DWORD DownloadTexture( FTextureInfo& TextureInfo, DWORD Address, DWORD GlideFlags, INT iFirstMip, INT iLastMip, GrTexInfo* texinfo, QWORD CacheID, FCacheItem*& Item, INT USize, INT VSize );
		void DownloadPalette( FTextureInfo& TextureInfo, FColor InPaletteMaxColor );

		// State functions.
		void Init( INT Intmu, UGlideRenderDevice* InGlide )
		{
			guard(FGlideTMU::Init);

			// Init variables.
			Glide			= InGlide;
			tmu				= Intmu;
			MaxColor	    = FColor(255,255,255,255);
			PaletteMaxColor = FColor(255,255,255,255);
			TextureItem		= NULL;
			PaletteCacheID	= 0;

			// Reset remembered info.
			ResetTexture();

			unguard;
		}
		void Exit()
		{
			guard(FGlideTMU::Exit);
			Cache.Exit(0);
			unguard;
		}
		void ResetTexture()
		{
			guard(FGlideTMU::ResetTexture);
			if( TextureItem != NULL )
			{
				TextureItem->Unlock();
				TextureItem = NULL;
			}
			TextureCacheID = 0;
			unguard;
		}
		void Tick()
		{
			guard(FGlideTMU::Tick);

			// Unlock and reset the texture.
			ResetTexture();

			// Update the texture cache.
			Cache.Tick();
			unguard;
		}
		void Flush()
		{
			guard(FGlideTMU::Flush);
			debugf( NAME_Log, "Flushed Glide TMU %i", tmu );
			Cache.Flush();
			unguard;
		}
		void SetTexture( FTextureInfo& Info, DWORD GlideFlags )
		{
			guard(FGlideTMU::SetTexture);
			QWORD TestID = Info.CacheID + (((QWORD)GlideFlags) << 60);
			if( TestID!=TextureCacheID )
			{
				// Get the texture into 3dfx memory.
				ResetTexture();
				TextureCacheID = TestID;

				// Make a texinfo.
				GrTexInfo texinfo;
				texinfo.format      = (GlideFlags & GF_Alpha    ) ? GR_TEXFMT_ARGB_1555
									: (GlideFlags & GF_NoPalette) ? GR_TEXFMT_RGB_565
									:                               GR_TEXFMT_P_8;
				INT GlideUBits      = Max( (INT)Info.Mips[0]->UBits, (INT)Info.Mips[0]->VBits-3);
				INT GlideVBits      = Max( (INT)Info.Mips[0]->VBits, (INT)Info.Mips[0]->UBits-3);
				INT MaxDim          = Max(GlideUBits,GlideVBits);
				INT FirstMip        = Max(MaxDim,8) - 8;
				INT LastMip			= Min(FirstMip+8,Info.NumMips-1);
				if( FirstMip >= Info.NumMips )
					appErrorf( "Encountered texture over 256x256 without sufficient mipmaps" );
				texinfo.aspectRatio = GlideVBits + 3 - GlideUBits;
				texinfo.largeLod    = Max( 8-MaxDim, 0 );
				texinfo.smallLod    = texinfo.largeLod + LastMip - FirstMip;

				// Download texture if needed.
				DWORD Address = (DWORD)Cache.Get( TestID, TextureItem, ALIGNMENT );
				if
				(	!Address
				||	(Info.TextureFlags & TF_RealtimeChanged) 
				||	((GlideFlags&GF_LightMap)&&(Info.MaxColor->D==0xffffffff)) )
					Address = DownloadTexture( Info, Address, GlideFlags, FirstMip, LastMip, &texinfo, TestID, TextureItem, 1<<(GlideUBits-FirstMip), 1<<(GlideVBits-FirstMip) );

				// Make it current.
				grTexSource( tmu, Address-ALIGNMENT, GR_MIPMAPLEVELMASK_BOTH, &texinfo );

				// Set MaxColor.
				MaxColor
				=	(GlideFlags & GF_LightMap) ? FColor(Info.MaxColor->B*2,Info.MaxColor->G*2,Info.MaxColor->R*2)
				:   (GlideFlags & (GF_Alpha|GF_NoPalette|GF_NoScale)) ? FColor(255,255,255,255)
				:	MaxColor = *Info.MaxColor;

				// Set scale.
				FLOAT Scale = (256 >> FirstMip) / (FLOAT)Min(256, Max(Info.USize, Info.VSize));
				UScale      = Scale / Info.UScale;
				VScale      = Scale / Info.VScale;

				// Handle palette.
				if( !(GlideFlags & (GF_Alpha|GF_NoPalette)) )
					if( Info.PaletteCacheID!=PaletteCacheID || PaletteMaxColor!=MaxColor )
						DownloadPalette( Info, MaxColor );
			}
			unguard;
		}
	} States[MAX_TMUS];

	// Glide specific functions.
	void UpdateModulation( FColor& FinalColor, FColor Color, INT& Count )
	{
		FinalColor.R = (FinalColor.R * Color.R) >> 8;
		FinalColor.G = (FinalColor.G * Color.G) >> 8;
		FinalColor.B = (FinalColor.B * Color.B) >> 8;
		if( --Count == 0 )
		{
			grConstantColorValue( *(GrColor_t*)&FinalColor );
			grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_LOCAL, GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, FXFALSE );
		}
	}
	void SetBlending( DWORD PolyFlags )
	{
		// Types.
		if( PolyFlags & PF_Translucent )
		{
			grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ONE_MINUS_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );
			if( !(PolyFlags & PF_Occlude) )
				grDepthMask( 0 );
		}
		else if( PolyFlags & PF_Modulated )
		{
			grAlphaBlendFunction( GR_BLEND_DST_COLOR, GR_BLEND_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );
			if( !(PolyFlags & PF_Occlude) )
				grDepthMask( 0 );
		}
		else if( PolyFlags & PF_Masked )
		{
			guAlphaSource( GR_ALPHASOURCE_TEXTURE_ALPHA );
			grAlphaBlendFunction( GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA, GR_BLEND_ZERO, GR_BLEND_ZERO );
			grAlphaTestFunction( GR_CMP_GREATER );
		}
		else
		{
			grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ZERO, GR_BLEND_ZERO );
		}

		// Flags.
		if( PolyFlags & PF_Invisible )
		{
			grColorMask( FXFALSE, FXTRUE );
		}
		if( PolyFlags & PF_NoSmooth )
		{
			grTexFilterMode( GR_TMU0, GR_TEXTUREFILTER_POINT_SAMPLED, GR_TEXTUREFILTER_POINT_SAMPLED );
		}
		if( (LockFlags & LOCKR_LightDiminish) && !(PolyFlags & PF_Unlit) )
		{
			grFogMode( GR_FOG_WITH_TABLE );
		}

		// Remember flags.
		OldPolyFlags = PolyFlags;
	}
	void ResetBlending( DWORD PolyFlags )
	{
		// Types.
		if( PolyFlags & PF_Invisible )
		{
			grColorMask( FXTRUE, FXTRUE );
		}
		if( PolyFlags & PF_Masked )
		{
			grAlphaTestFunction( GR_CMP_ALWAYS );
		}
		if( PolyFlags & (PF_Translucent|PF_Modulated) )
		{
			if( !(PolyFlags & PF_Occlude) )
				grDepthMask( 1 );
		}

		// Flags.
		if( PolyFlags & PF_NoSmooth )
		{
			grTexFilterMode( GR_TMU0, GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
		}
		if( (LockFlags & LOCKR_LightDiminish) && !(PolyFlags & PF_Unlit) )
		{
			grFogMode(GR_FOG_DISABLE);
		}
	}
};
IMPLEMENT_CLASS(UGlideRenderDevice);

/*-----------------------------------------------------------------------------
	UGlideRenderDevice Init & Exit.
-----------------------------------------------------------------------------*/

//
// Try a resolution.
//
UBOOL UGlideRenderDevice::TryRes( INT X, INT Y, UBOOL Force, GrScreenRefresh_t Ref )
{
	guard(UGlideRenderDevice::TryRes);
	UBOOL Result = 1;

	// Pick an appropriate resolution.
	INT Res;
	if      ( X<=512		   ) Res=GR_RESOLUTION_512x384;
	else if ( X<=640 && Y<=400 ) Res=GR_RESOLUTION_640x400;
	else if ( X<=640 && Y<=480 ) Res=GR_RESOLUTION_640x480;
	else if ( X<=800 && Y<=600 ) Res=GR_RESOLUTION_800x600;
	else						 Res=GR_RESOLUTION_1024x768;

	// Open the display.
	Retry:
	GGlideCheckErrors=0;
	if( !grSstWinOpen
	( 
		(INT)Viewport->GetWindow(),
		hwconfig.SSTs[0].type!=GR_SSTTYPE_SST96 ? Res : GR_RESOLUTION_NONE,
		Ref,
		GR_COLORFORMAT_ABGR, GR_ORIGIN_UPPER_LEFT,
		3,
		1
	) )
    {
		if( Res != GR_RESOLUTION_512x384 )
		{
			// Try again.
			debugf( NAME_Init, "Glide: Resolution %i failed, falling back...", Res );
			Res = GR_RESOLUTION_512x384;
			goto Retry;
		}
		else if( Ref!=GR_REFRESH_72Hz )
		{
			// Try again.
			debugf( NAME_Init, "Glide: Refresh %i failed, falling back...", Ref );
			Ref = GR_REFRESH_72Hz;
			goto Retry;
		}
		grGlideShutdown();
		debugf( NAME_Init, "grSstOpen failed (%i, %i)", Ref, Res );
		Result = 0;
    }
	GGlideCheckErrors = 1;
	if( Result )
	{
		// Set parameters.
		grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
		grDepthMask( 1 );
		grDitherMode( GR_DITHER_2x2 );
		grChromakeyValue(0);
		grChromakeyMode(0);
		grAlphaTestReferenceValue( 127.0 );
		grDepthBiasLevel(16);
		grDepthBufferFunction( GR_CMP_LEQUAL );

		// Fog.
		GrFog_t fog[GR_FOG_TABLE_SIZE];
		for( INT i=0; i<GR_FOG_TABLE_SIZE; i++ )
		{
			float W = guFogTableIndexToW(i);
			fog[i]  = Clamp( 0.1f * W, 0.f, 255.f );
		}
		grFogTable(fog);
		grFogColorValue(0); 
		grFogMode(GR_FOG_DISABLE);

		// Init all TMU's.
		for( INT tmu=0; tmu<NumTmu; tmu++ )
		{
			grTexLodBiasValue( tmu, Clamp( DetailBias, -3.f, 3.f ) );
			grTexClampMode( tmu, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP );
			guTexCombineFunction( tmu, GR_TEXTURECOMBINE_DECAL );
			grTexMipMapMode( tmu, GR_MIPMAP_NEAREST, FXFALSE );
			grTexFilterMode( tmu, GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
			States[tmu].Init( tmu, this );
		}
	}
	return Result;
	unguard;
}

//
// Initializes Glide.  Can't fail.
//
UBOOL UGlideRenderDevice::Init( UViewport* InViewport )
{
	guard(UGlideRenderDevice::Init);

	// Remember variables.
	OldPolyFlags		= 0;
	Viewport			= InViewport;

	// Driver flags.
	SpanBased			= 0;
	FrameBuffered		= 0;
	SupportsFogMaps		= 1;
	SupportsDistanceFog	= 1;

	// Log message.
	debugf( NAME_Init, "Initializing Glide..." );

	// Verify that hardware exists.
	char GlideVer[80];
	grGlideGetVersion(GlideVer);
	debugf( NAME_Init, "Found Glide: %s", GlideVer );
	if( !grSstQueryBoards(&hwconfig) )
		return 0;

	// Checks.
	check(sizeof(FGlideColor)==2);

	// Initialize the Glide library.
	grGlideInit();

	// Set error callback.
	grErrorSetCallback( GlideErrorHandler );

	// Make sure 3Dfx hardware is present.
	GGlideCheckErrors=0;
	if( !grSstQueryHardware( &hwconfig ) )
	{
		grGlideShutdown();
		debugf( NAME_Init, "grSstQueryHardware failed" );
		return 0;
    }
	GGlideCheckErrors=1;

	// Init pyramic-compressed scaling tables.
	for( INT A=0; A<128; A++ )
	{
		for( INT B=0; B<=A; B++ )
		{
			INT M=Max(A,1);
			RScale[PYR(A)+B] = Min(((B*0x10000)/M),0xf800) & 0xf800;
			GScale[PYR(A)+B] = Min(((B*0x00800)/M),0x07e0) & 0x07e0;
			BScale[PYR(A)+B] = Min(((B*0x00020)/M),0x001f) & 0x001f;
		}
	}

	// Check hardware info.
	debugf
	(
		NAME_Init,
		"Glide info: Type=%i, fbRam=%i fbiRev=%i nTexelfx=%i Sli=%i",
		hwconfig.SSTs[0].type,
		hwconfig.SSTs[0].sstBoard.VoodooConfig.fbRam,
		hwconfig.SSTs[0].sstBoard.VoodooConfig.fbiRev,
		hwconfig.SSTs[0].sstBoard.VoodooConfig.nTexelfx,
		(int)hwconfig.SSTs[0].sstBoard.VoodooConfig.sliDetect
	);
	NumTmu = Min(hwconfig.SSTs[0].sstBoard.VoodooConfig.nTexelfx,(int)MAX_TMUS);
	check(NumTmu>0);
	for( INT tmu=0; tmu<NumTmu; tmu++ )
		debugf
		(
			NAME_Init,
			"Glide tmu %i: tmuRev=%i tmuRam=%i Space=%i", tmu,
			hwconfig.SSTs[0].sstBoard.VoodooConfig.tmuConfig[tmu].tmuRev,
			hwconfig.SSTs[0].sstBoard.VoodooConfig.tmuConfig[tmu].tmuRam,
			grTexMaxAddress(tmu) - grTexMinAddress(tmu)
		);

	// Select the first board.
	grSstSelect( 0 );

	// Try it.
	UBOOL Result = TryRes( Viewport->SizeX, Viewport->SizeY, 0, RefreshRate );
	if( Result )
	{
		for( INT tmu=0; tmu<NumTmu; tmu++ )
		{
			States[tmu].Cache.Init
			(
				grTexMaxAddress(tmu) - grTexMinAddress(tmu),
				2048,
				(void*)(grTexMinAddress(tmu) + ALIGNMENT),
				2 * 1024 * 1024
			);
		}
	}

	// Go.
	Flush();
	Viewport->MakeFullscreen( grSstScreenWidth(), grSstScreenHeight(), 1 );
	Viewport->SetDrag( 1 );
	Viewport->SetMouseCapture( 1, 1 );

	return Result;
	unguard;
}

//
// Register configurable properties.
//
void UGlideRenderDevice::InternalClassInitializer( UClass* Class )
{
	guard(UGlideRenderDevice::InternalClassInitializer);
	if( appStricmp( Class->GetName(), "GlideRenderDevice" )==0 )
	{
		UEnum* RefreshRates=new(Class,"RefreshRates")UEnum( NULL );
		new(RefreshRates->Names)FName( "60Hz"  );
		new(RefreshRates->Names)FName( "70Hz"  );
		new(RefreshRates->Names)FName( "72Hz"  );
		new(RefreshRates->Names)FName( "75Hz"  );
		new(RefreshRates->Names)FName( "80Hz"  );
		new(RefreshRates->Names)FName( "90Hz"  );
		new(RefreshRates->Names)FName( "100Hz" );
		new(RefreshRates->Names)FName( "85Hz"  );
		new(RefreshRates->Names)FName( "120Hz" );

		new(Class,"DetailTextures",  RF_Public)UBoolProperty ( CPP_PROPERTY(DetailTextures  ), "Options", CPF_Config );
		new(Class,"FastUglyRefresh", RF_Public)UBoolProperty ( CPP_PROPERTY(FastUglyRefresh ), "Options", CPF_Config );
		new(Class,"ScreenSmoothing", RF_Public)UBoolProperty ( CPP_PROPERTY(ScreenSmoothing ), "Options", CPF_Config );
		new(Class,"DetailBias",      RF_Public)UFloatProperty( CPP_PROPERTY(DetailBias      ), "Options", CPF_Config );
		new(Class,"RefreshRate",     RF_Public)UByteProperty ( CPP_PROPERTY(RefreshRate     ), "Options", CPF_Config, RefreshRates );
	}
	unguard;
}

//
// Validate configuration changes.
//
void UGlideRenderDevice::PostEditChange()
{
	guard(UGlideRenderDevice::PostEditChange);
	RefreshRate = Clamp((INT)RefreshRate,0,GR_REFRESH_120Hz);
	DetailBias  = Clamp(DetailBias,-3.f,3.f);
	unguard;
}

//
// Shut down the Glide device.
//
void UGlideRenderDevice::Exit()
{
	guard(UGlideRenderDevice::Exit);
	debugf( NAME_Exit, "Shutting down Glide..." );

	// Shut down windowing.
	grSstWinClose();

	// Shut down each texture mapping unit.
	for( INT i=0; i<NumTmu; i++ )
		States[i].Exit();

	// Shut down Glide.
	grGlideShutdown();

	debugf( NAME_Exit, "Glide shut down" );
	unguard;
};

//
// Flush all cached data.
//
void UGlideRenderDevice::Flush()
{
	guard(UGlideRenderDevice::Flush);

	for( INT i=0; i<NumTmu; i++ )
		States[i].Flush();
	grGammaCorrectionValue( 0.5 + 1.5*Viewport->Client->Brightness );

	unguard;
}

/*-----------------------------------------------------------------------------
	UGlideRenderDevice Downloaders.
-----------------------------------------------------------------------------*/

//
// Download the texture and all of its mipmaps.
//
DWORD UGlideRenderDevice::FGlideTMU::DownloadTexture
(
	FTextureInfo&	Info,
	DWORD			Address,
	DWORD			GlideFlags,
	INT				iFirstMip,
	INT				iLastMip,
	GrTexInfo*		texinfo,
	QWORD			TestID,
	FCacheItem*&	Item,
	INT				USize,
	INT				VSize
)
{
	guard(UGlideRenderDevice::DownloadTexture);
	FMemMark Mark(GMem);
	INT MaxSize = USize * VSize;

	// Create cache entry.
	INT GlideSize = grTexCalcMemRequired( texinfo->smallLod, texinfo->largeLod, texinfo->aspectRatio, texinfo->format );
	if( !Address )
		Address = (DWORD)Cache.Create( TestID, Item, GlideSize, ALIGNMENT );//!!
	//!!
	DWORD Tmp=Address-ALIGNMENT;
	if( Tmp<grTexMinAddress(0) || Tmp+GlideSize>grTexMaxAddress(0) )
		appErrorf( "Texture miscalc: %08X %08X %08X %08X, %i %i %i %i", Tmp, GlideSize, grTexMinAddress(0), grTexMaxAddress(0), texinfo->smallLod, texinfo->largeLod, texinfo->aspectRatio, texinfo->format );
	if( GlideFlags & GF_LightMap )
	{
		Stats.Downloads16++;
		clock(Stats.Download16Time);

		// Find max brightness if not yet computed.
		if( Info.MaxColor->D==0xffffffff )
		{
			DWORD* Tmp = (DWORD*)Info.Mips[0]->DataPtr;
			DWORD  Max = 0x01010101;
			for( INT i=0; i<Info.VClamp; i++ )
			{
				for( INT j=0; j<Info.UClamp; j++ )
				{
					DWORD Flow = (Max - *Tmp) & 0x80808080;
					if( Flow )
					{
						DWORD MaxMask = Flow - (Flow >> 7);
						Max = (*Tmp & MaxMask) | (Max & (0x7f7f7f7f - MaxMask)) ;
					}
					Tmp++;
				}
				Tmp += Info.USize - Info.UClamp;
			}
			Info.MaxColor->D = Max;
			check(!(Max&0x00808080));
		}

		// Convert 8-8-8 light maps to 5-6-5.
		_WORD* RPtr  = RScale + PYR(Info.MaxColor->B);
		_WORD* GPtr  = GScale + PYR(Info.MaxColor->G);
		_WORD* BPtr  = BScale + PYR(Info.MaxColor->R);
		_WORD* Space = New<_WORD>(GMem,MaxSize), *Ptr=Space;
		for( INT c=0; c<VSize; c+=Info.VSize )
		{
			FRainbowPtr Src = Info.Mips[0]->DataPtr;
			for( INT i=0; i<Info.VClamp; i++ )
			{
				for( INT d=0; d<USize; d+=Info.USize )
				{
					FRainbowPtr InnerSrc=Src;
					for( INT j=0; j<Info.UClamp; j++ )
					{
						*Ptr++
						=	BPtr[InnerSrc.PtrBYTE[0]]
						+	GPtr[InnerSrc.PtrBYTE[1]]
						+	RPtr[InnerSrc.PtrBYTE[2]];
						InnerSrc.PtrDWORD++;
					}
					Ptr += Info.USize - Info.UClamp;
				}
				Src.PtrDWORD += Info.USize;
			}
			Ptr += (Info.VSize - Info.VClamp) * Info.USize;
		}
		grTexDownloadMipMapLevelPartial
		(
			tmu,
			Address-ALIGNMENT,
			texinfo->largeLod,
			texinfo->largeLod,
			texinfo->aspectRatio,
			texinfo->format,
			GR_MIPMAPLEVELMASK_BOTH,
			Space,
			0,
			Info.VClamp-1
		);
		unclock(Stats.Download16Time);
	}
	else
	{
		// Create temporary memory.
		clock(Stats.Download8Time);

		// Make buffer for copying the texture to fix its aspect ratio.
		BYTE* Copy=NULL;
		if( USize!=Info.Mips[iFirstMip]->USize || VSize!=Info.Mips[iFirstMip]->VSize )
			Copy = New<BYTE>( GMem, MaxSize );

		// Make buffer for alpha conversion.
		FGlideColor* Alpha        = NULL;
		FGlideColor* AlphaPalette = NULL;
		if( GlideFlags & GF_Alpha )
		{
			AlphaPalette = New<FGlideColor>( GMem, NUM_PAL_COLORS );
			Alpha        = New<FGlideColor>( GMem, MaxSize        );
			for( INT i=0; i<NUM_PAL_COLORS; i++ )
			{
				AlphaPalette[i].Color5551.R = Info.Palette[i].R >> (8-5);
				AlphaPalette[i].Color5551.G = Info.Palette[i].G >> (8-5);
				AlphaPalette[i].Color5551.B = Info.Palette[i].B >> (8-5);
				AlphaPalette[i].Color5551.A = 1;
			}
			AlphaPalette[0].Color5551.R = 0;
			AlphaPalette[0].Color5551.G = 0;
			AlphaPalette[0].Color5551.B = 0;
			AlphaPalette[0].Color5551.A = 0;
			Stats.Downloads16++;
		}
		else if( GlideFlags & GF_NoPalette )
		{
			AlphaPalette = New<FGlideColor>( GMem, NUM_PAL_COLORS );
			Alpha        = New<FGlideColor>( GMem, MaxSize        );
			for( INT i=0; i<NUM_PAL_COLORS; i++ )
			{
				AlphaPalette[i].Color565.R = Info.Palette[i].R >> (8-5);
				AlphaPalette[i].Color565.G = Info.Palette[i].G >> (8-6);
				AlphaPalette[i].Color565.B = Info.Palette[i].B >> (8-5);
			}
			Stats.Downloads16++;
		}
		else
		{
			Stats.Downloads8++;
		}

		// Download the texture's mips.
		for( INT iMip=iFirstMip; iMip<=iLastMip; iMip++,MaxSize/=4 )
		{
			FMipmap*  Mip = Info.Mips[iMip];
			BYTE*     Src = Mip->DataPtr;
			if( Copy )
			{
				BYTE* To = Copy;
				for( INT j=0; j<VSize; j+=Mip->VSize )
				{
					BYTE* From = Src;
					for( INT k=0; k<Mip->VSize; k++ )
					{
						for( INT l=0; l<USize; l+=Mip->USize )
						{
							memcpy( To, From, Mip->USize );
							To += Mip->USize;
						}
						From += Mip->USize;
					}
				}
				Src = Copy;
			}
			if( Alpha )
			{
				for( INT i=0; i<MaxSize; i++ )
					Alpha[i] = AlphaPalette[Src[i]];
				Src = (BYTE*)Alpha;
			}
			grTexDownloadMipMapLevel
			(
				tmu,
				Address-ALIGNMENT,
				texinfo->largeLod + iMip - iFirstMip,
				texinfo->largeLod,
				texinfo->aspectRatio,
				texinfo->format,
				GR_MIPMAPLEVELMASK_BOTH,
				Src
			);
		}
		unclock(Stats.Download8Time);
	}
	Mark.Pop();
	return Address;
	unguardf(( "(size=%ix%i, (%i/%i)", Info.USize, Info.VSize, iFirstMip, iLastMip ));
}

//
// Download the palette and all of its mipmaps.
//
void UGlideRenderDevice::FGlideTMU::DownloadPalette
(
	FTextureInfo&	TextureInfo,
	FColor			InMaxColor
)
{
	guard(UGlideRenderDevice::DownloadPalette);
	clock(Stats.PaletteTime);
	Stats.DownloadsPalette++;

	// Update state.
	PaletteCacheID = TextureInfo.PaletteCacheID;
	PaletteMaxColor = InMaxColor;
	QWORD NewCacheID = (TextureInfo.PaletteCacheID & ~(QWORD)255) + CID_GlidePal + ((QWORD)InMaxColor.D<<32);
	FCacheItem* Item;
	struct FGlidePal {BYTE B,G,R,A;}* GlidePal = (FGlidePal*)GCache.Get( NewCacheID, Item );
	if( !GlidePal )
	{
		// Create it.
		GlidePal = (FGlidePal*)GCache.Create( NewCacheID, Item, 1024 );
		FLOAT ScaleR = 255.0 / Max(PaletteMaxColor.R, (BYTE)1);
		FLOAT ScaleG = 255.0 / Max(PaletteMaxColor.G, (BYTE)1);
		FLOAT ScaleB = 255.0 / Max(PaletteMaxColor.B, (BYTE)1);
		for( INT i=0; i<NUM_PAL_COLORS; i++ )
		{
			GlidePal[i].R = appFloor(TextureInfo.Palette[i].R * ScaleR);
			GlidePal[i].G = appFloor(TextureInfo.Palette[i].G * ScaleG);
			GlidePal[i].B = appFloor(TextureInfo.Palette[i].B * ScaleB);
			GlidePal[i].A = 0;
		}
	}

	// Send the palette.
	grTexDownloadTable( tmu, GR_TEXTABLE_PALETTE, GlidePal );
	Item->Unlock();

	unclock(Stats.PaletteTime);
	unguard;
}

/*-----------------------------------------------------------------------------
	UGlideRenderDevice Lock & Unlock.
-----------------------------------------------------------------------------*/

//
// Lock the Glide device.
//
void UGlideRenderDevice::Lock( FPlane InFlashScale, FPlane InFlashFog, FPlane ScreenClear, DWORD InLockFlags, BYTE* HitData, INT* HitSize )
{
	guard(UGlideRenderDevice::Lock);

	// Remember parameters.
	LockFlags  = InLockFlags;
	FlashScale = InFlashScale;
	FlashFog   = InFlashFog;

	// Clear the Z-buffer.
	grColorMask( (LockFlags & LOCKR_ClearScreen) ? FXTRUE : FXFALSE, FXTRUE );
	grBufferClear( FColor(ScreenClear).TrueColor(), 0, GR_WDEPTHVALUE_FARTHEST );
	grColorMask( FXTRUE, FXTRUE );

	// Init stats.
	memset(&Stats,0,sizeof(Stats));

	unguard;
};

//
// Clear the Z-buffer.
//
void UGlideRenderDevice::ClearZ( FSceneNode* Frame )
{
	guard(UGlideRenderDevice::ClearZ);

	// Clear only the Z-buffer.
	grColorMask( FXFALSE, FXTRUE );
	grBufferClear( 0, 0, GR_WDEPTHVALUE_FARTHEST );
	grColorMask( FXTRUE, FXTRUE );

	unguard;
}

//
// Perform screenflashes.
//
void UGlideRenderDevice::EndFlash()
{
	guard(UGlideRenderDevice::EndFlash);
	if( FlashScale!=FVector(.5,.5,.5) || FlashFog!=FVector(0,0,0) )
	{
		// Setup color.
		FColor GlideColor = FColor(FPlane(FlashFog.R,FlashFog.G,FlashFog.B,Min(FlashScale.R*2.f,1.f)));
		grConstantColorValue( *(GrColor_t*)&GlideColor );

		// Set up verts.
		GrVertex Verts[4];
		Verts[0].x=0;               Verts[0].y=0;               Verts[0].oow=0.5;
		Verts[1].x=0;               Verts[1].y=Viewport->SizeY; Verts[1].oow=0.5;
		Verts[2].x=Viewport->SizeX; Verts[2].y=Viewport->SizeY; Verts[2].oow=0.5;
		Verts[3].x=Viewport->SizeX; Verts[3].y=0;               Verts[3].oow=0.5;

		// Draw it.
		grDepthMask( 0 );
		grDepthBufferFunction( GR_CMP_ALWAYS );
		guColorCombineFunction( GR_COLORCOMBINE_CCRGB );
		guAlphaSource( GR_ALPHASOURCE_CC_ALPHA );
		grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_SRC_ALPHA, GR_BLEND_ZERO, GR_BLEND_ZERO );
		grDrawPlanarPolygonVertexList( 4, Verts );
		grDepthMask( 1 );
		grDepthBufferFunction( GR_CMP_LEQUAL );
	}
	unguard;
}

//
// Unlock the Glide rendering device.
//
void UGlideRenderDevice::Unlock( UBOOL Blit )
{
	guard(UGlideRenderDevice::Unlock);

	// Tick each of the states.
	for( INT i=0; i<NumTmu; i++ )
		States[i].Tick();

	// Blit it.
	if( Blit )
	{
		// Throttle if we are rendering much faster than the refresh rate.
		DOUBLE Seconds = appSeconds();
		while( grBufferNumPending()>0 && appSeconds()-Seconds<0.1 );

		// Flip pages.
		guard(grBufferSwap);
		grBufferSwap( !FastUglyRefresh );
		unguard;
	}
	unguard;
};

/*-----------------------------------------------------------------------------
	UGlideRenderDevice texture vector polygon drawer.
-----------------------------------------------------------------------------*/

//
// Draw a textured polygon using surface vectors.
//
#define VERTS(poly) ((GrVertex*)(poly)->User)
#define MASTER_S r
#define MASTER_T g
void UGlideRenderDevice::DrawComplexSurface( FSceneNode* Frame, FSurfaceInfo& Surface, FSurfaceFacet& Facet )
{
	guard(UGlideRenderDevice::DrawComplexSurface);
	const FLOAT NearZ = 200.0;
	clock(Stats.PolyVTime);
	FMemMark Mark(GMem);
	Stats.Surfs++;

	// Mutually exclusive effects.
	if( Surface.DetailTexture && Surface.FogMap )
		Surface.DetailTexture = NULL;

	// Flags.
	DWORD GlideFlags
	=	((Surface.PolyFlags & PF_Masked) ? GF_Alpha : 0)
	|	((Surface.PolyFlags & (PF_Modulated|PF_Translucent))||(Surface.Texture->TextureFlags&TF_Realtime) ? GF_NoScale : 0);

	// Set up all poly vertices.
	for( FSavedPoly* Poly=Facet.Polys; Poly; Poly=Poly->Next )
	{
		// Set up vertices.
		Poly->User = New<GrVertex>(GMem,Poly->NumPts);
		for( INT i=0; i<Poly->NumPts; i++ )
		{
			VERTS(Poly)[i].MASTER_S = Facet.MapCoords.XAxis | (*(FVector*)Poly->Pts[i] - Facet.MapCoords.Origin);
			VERTS(Poly)[i].MASTER_T = Facet.MapCoords.YAxis | (*(FVector*)Poly->Pts[i] - Facet.MapCoords.Origin);
			VERTS(Poly)[i].x        = Mask(Poly->Pts[i]->ScreenX + Frame->XB);
			VERTS(Poly)[i].y	    = Mask(Poly->Pts[i]->ScreenY + Frame->YB);
			VERTS(Poly)[i].z	    = Poly->Pts[i]->Point.Z;
			VERTS(Poly)[i].oow      = Poly->Pts[i]->RZ * Frame->RProj.Z;
		}
	}

	// Init precision adjustment.
	FColor FinalColor(255,255,255,0);

	// Count things to draw.
	INT ModulateThings = (Surface.Texture!=NULL) + (Surface.LightMap!=NULL) + (Surface.MacroTexture!=NULL) + (Surface.BumpMap!=NULL);
	if( ModulateThings > 1 )
		guColorCombineFunction( GR_COLORCOMBINE_DECAL_TEXTURE );

	// Draw normal texture.
	if( Surface.Texture )
	{
		// Setup texture.
		SetBlending( Surface.PolyFlags );
		States[GR_TMU0].SetTexture( *Surface.Texture, GlideFlags );
		UpdateModulation( FinalColor, States[0].MaxColor, ModulateThings ); 
		for( Poly=Facet.Polys; Poly; Poly=Poly->Next )
		{
			for( INT i=0; i<Poly->NumPts; i++ )
			{
				VERTS(Poly)[i].tmuvtx[0].sow = VERTS(Poly)[i].oow * (VERTS(Poly)[i].MASTER_S - Surface.Texture->Pan.X) * States[GR_TMU0].UScale;
				VERTS(Poly)[i].tmuvtx[0].tow = VERTS(Poly)[i].oow * (VERTS(Poly)[i].MASTER_T - Surface.Texture->Pan.Y) * States[GR_TMU0].VScale;
			}
			grDrawPolygonVertexList( Poly->NumPts, VERTS(Poly) );
			Stats.Polys++;
		}
		ResetBlending( Surface.PolyFlags );
	}

	// Handle depth buffering the appropriate areas of masked textures.
	if( Surface.PolyFlags & PF_Masked )
		grDepthBufferFunction( GR_CMP_EQUAL );

	// Modulation blend the rest of the textures.
	if( ModulateThings>0 || (Surface.DetailTexture && DetailTextures) )
		grAlphaBlendFunction( GR_BLEND_DST_COLOR, GR_BLEND_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );

	// Bump map.
	if( Surface.BumpMap )
	{
		States[GR_TMU0].SetTexture( *Surface.BumpMap, GF_NoScale );
		UpdateModulation( FinalColor, States[0].MaxColor, ModulateThings ); 
		for( Poly=Facet.Polys; Poly; Poly=Poly->Next )
		{
			for( int i=0; i<Poly->NumPts; i++ )
			{
				VERTS(Poly)[i].tmuvtx[0].sow = VERTS(Poly)[i].oow * (VERTS(Poly)[i].MASTER_S - Surface.Texture->Pan.X) * States[GR_TMU0].UScale;
				VERTS(Poly)[i].tmuvtx[0].tow = VERTS(Poly)[i].oow * (VERTS(Poly)[i].MASTER_T - Surface.Texture->Pan.Y) * States[GR_TMU0].VScale;
			}
			grDrawPolygonVertexList( Poly->NumPts, VERTS(Poly) );
		}
	}

	// Macrotexture.
	if( Surface.MacroTexture )
	{
		// Set the light mesh.
		States[GR_TMU0].SetTexture( *Surface.MacroTexture, GF_NoPalette );
		UpdateModulation( FinalColor, States[0].MaxColor, ModulateThings ); 
		for( Poly=Facet.Polys; Poly; Poly=Poly->Next )
		{
			for( INT i=0; i<Poly->NumPts; i++ )
			{
				VERTS(Poly)[i].tmuvtx[0].sow = VERTS(Poly)[i].oow * (VERTS(Poly)[i].MASTER_S - Surface.MacroTexture->Pan.X) * States[GR_TMU0].UScale;
				VERTS(Poly)[i].tmuvtx[0].tow = VERTS(Poly)[i].oow * (VERTS(Poly)[i].MASTER_T - Surface.MacroTexture->Pan.Y) * States[GR_TMU0].VScale;
			}
			grDrawPolygonVertexList( Poly->NumPts, VERTS(Poly));
		}
	}

	// Light map.
	if( Surface.LightMap )
	{
		// Set the light map.
		States[GR_TMU0].SetTexture( *Surface.LightMap, GF_NoPalette|GF_LightMap );
		UpdateModulation( FinalColor, States[0].MaxColor, ModulateThings ); 
		for( Poly=Facet.Polys; Poly; Poly=Poly->Next )
		{
			for( INT i=0; i<Poly->NumPts; i++ )
			{
				VERTS(Poly)[i].tmuvtx[0].sow = VERTS(Poly)[i].oow * (VERTS(Poly)[i].MASTER_S - Surface.LightMap->Pan.X + 0.5*Surface.LightMap->UScale) * States[GR_TMU0].UScale;
				VERTS(Poly)[i].tmuvtx[0].tow = VERTS(Poly)[i].oow * (VERTS(Poly)[i].MASTER_T - Surface.LightMap->Pan.Y + 0.5*Surface.LightMap->VScale) * States[GR_TMU0].VScale;
			}
			grDrawPolygonVertexList( Poly->NumPts, VERTS(Poly) );
		}
	}

	// Draw detail texture overlaid.
	if( Surface.DetailTexture && DetailTextures )
	{
		for( Poly=Facet.Polys; Poly; Poly=Poly->Next )
		{
			UBOOL IsNear[32], CountNear=0;
			for( int i=0; i<Poly->NumPts; i++ )
			{
				IsNear[i] = VERTS(Poly)[i].z<NearZ;
				CountNear += IsNear[i];
			}
			if( CountNear )
			{
				INT NumNear=0;
				States[GR_TMU0].SetTexture( *Surface.DetailTexture, GF_NoPalette | GF_NoScale );
				GrVertex Near[32];
				for( INT i=0,j=Poly->NumPts-1; i<Poly->NumPts; j=i++ )
				{
					if( IsNear[i] ^ IsNear[j] )
					{
						FLOAT G           = (VERTS(Poly)[i].z - NearZ) / (VERTS(Poly)[i].z - VERTS(Poly)[j].z);
						FLOAT F           = 1.0 - G;
						Near[NumNear].z   = NearZ;
						Near[NumNear].oow = 1.0 / NearZ;
						Near[NumNear].x   = Mask( (F*Poly->Pts[i]->ScreenX*VERTS(Poly)[i].z + G*Poly->Pts[j]->ScreenX*VERTS(Poly)[j].z) * Near[NumNear].oow + Frame->XB);
						Near[NumNear].y   = Mask( (F*Poly->Pts[i]->ScreenY*VERTS(Poly)[i].z + G*Poly->Pts[j]->ScreenY*VERTS(Poly)[j].z) * Near[NumNear].oow + Frame->YB);
						Near[NumNear].tmuvtx[0].sow = Near[NumNear].oow*( F*VERTS(Poly)[i].MASTER_S + G*VERTS(Poly)[j].MASTER_S - Surface.DetailTexture->Pan.X ) * States[GR_TMU0].UScale;
						Near[NumNear].tmuvtx[0].tow = Near[NumNear].oow*( F*VERTS(Poly)[i].MASTER_T + G*VERTS(Poly)[j].MASTER_T - Surface.DetailTexture->Pan.Y ) * States[GR_TMU0].VScale;
						NumNear++;
					}
					if( IsNear[i] )
					{
						Near[NumNear].z   = VERTS(Poly)[i].z;
						Near[NumNear].oow = VERTS(Poly)[i].oow;
						Near[NumNear].x   = VERTS(Poly)[i].x;
						Near[NumNear].y   = VERTS(Poly)[i].y;
						Near[NumNear].tmuvtx[0].sow = Near[NumNear].oow*( VERTS(Poly)[i].MASTER_S - Surface.DetailTexture->Pan.X ) * States[GR_TMU0].UScale;
						Near[NumNear].tmuvtx[0].tow = Near[NumNear].oow*( VERTS(Poly)[i].MASTER_T - Surface.DetailTexture->Pan.Y ) * States[GR_TMU0].VScale;
						NumNear++;
					}
				}
				for( i=0; i<NumNear; i++ )
					Near[i].a = Min( 100.f * (NearZ / Near[i].z - 1), 255.f );
				grDepthBiasLevel(0);
				grAlphaCombine( GR_COMBINE_FUNCTION_LOCAL_ALPHA, GR_COMBINE_FACTOR_ONE, GR_COMBINE_LOCAL_ITERATED, GR_COMBINE_OTHER_NONE, FXFALSE );
				grConstantColorValue( 0x7f7f7f );
				grColorCombine( GR_COMBINE_FUNCTION_BLEND, GR_COMBINE_FACTOR_LOCAL_ALPHA, GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, FXFALSE );
				grDrawPolygonVertexList( NumNear, Near );
				grDepthBiasLevel(16);
			}
		}
	}
	check(ModulateThings==0);

	// Fog map.
	if( Surface.FogMap )
	{
		States[GR_TMU0].SetTexture( *Surface.FogMap, GF_NoPalette|GF_LightMap );
		FinalColor.R = States[GR_TMU0].MaxColor.R;
		FinalColor.G = States[GR_TMU0].MaxColor.G;
		FinalColor.B = States[GR_TMU0].MaxColor.B;
		grConstantColorValue( *(GrColor_t*)&FinalColor );
		grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ONE_MINUS_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );
		grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_LOCAL, GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, FXFALSE );
		for( Poly=Facet.Polys; Poly; Poly=Poly->Next )
		{
			for( int i=0; i<Poly->NumPts; i++ )
			{
				VERTS(Poly)[i].tmuvtx[0].sow = VERTS(Poly)[i].oow * (VERTS(Poly)[i].MASTER_S - Surface.FogMap->Pan.X + 0.5*Surface.FogMap->UScale) * States[GR_TMU0].UScale;
				VERTS(Poly)[i].tmuvtx[0].tow = VERTS(Poly)[i].oow * (VERTS(Poly)[i].MASTER_T - Surface.FogMap->Pan.Y + 0.5*Surface.FogMap->VScale) * States[GR_TMU0].VScale;
			}
			grDrawPolygonVertexList( Poly->NumPts, VERTS(Poly) );
		}
	}

	// Finish mask handling.
	if( Surface.PolyFlags & PF_Masked )
		grDepthBufferFunction( GR_CMP_LEQUAL );

	Mark.Pop();
	unclock(Stats.PolyVTime);
	unguard;
}

/*-----------------------------------------------------------------------------
	UGlideRenderDevice texture coordinates polygon drawer.
-----------------------------------------------------------------------------*/

//
// Draw a polygon with texture coordinates.
//
void UGlideRenderDevice::DrawGouraudPolygon
(
	FSceneNode*		Frame,
	FTextureInfo&	Texture,
	FTransTexture**	Pts,
	INT				NumPts,
	DWORD			PolyFlags,
	FSpanBuffer*	Span
)
{
	guard(UGlideRenderDevice::DrawGouraudPolygon);
	clock(Stats.PolyCTime);
	Stats.Tris++;

	// Set up verts.
	static GrVertex Verts[32];
	States[GR_TMU0].SetTexture( Texture, GF_NoScale | ((PolyFlags&PF_Masked)?GF_Alpha:0) );
	for( INT i=0; i<NumPts; i++ )
	{
		Verts[i].x	 			= Mask(Pts[i]->ScreenX + Frame->XB);
		Verts[i].y	 			= Mask(Pts[i]->ScreenY + Frame->YB);
		Verts[i].r				= Pts[i]->Light.R*255.f;
		Verts[i].g				= Pts[i]->Light.G*255.f;
		Verts[i].b				= Pts[i]->Light.B*255.f;
		Verts[i].oow 			= Pts[i]->RZ * Frame->RProj.Z;
		Verts[i].tmuvtx[0].sow	= Verts[i].oow * Pts[i]->U * States[GR_TMU0].UScale;
		Verts[i].tmuvtx[0].tow	= Verts[i].oow * Pts[i]->V * States[GR_TMU0].VScale;
	}

	// Draw it.
	SetBlending( PolyFlags );
	guColorCombineFunction( (PolyFlags & PF_Modulated) ? GR_COLORCOMBINE_DECAL_TEXTURE : GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB );
	grDrawPolygonVertexList( NumPts, Verts );
	ResetBlending( PolyFlags );

	// Fog.
	if( (PolyFlags & (PF_RenderFog|PF_Translucent|PF_Modulated))==PF_RenderFog )
	{
		for( INT i=0; i<NumPts; i++ )
		{
			Verts[i].r = Pts[i]->Fog.R*255.f;
			Verts[i].g = Pts[i]->Fog.G*255.f;
			Verts[i].b = Pts[i]->Fog.B*255.f;
		}
		grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ONE_MINUS_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );
		guColorCombineFunction( GR_COLORCOMBINE_ITRGB );
		grDrawPolygonVertexList( NumPts, Verts );
		grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ZERO, GR_BLEND_ZERO );
		if( PolyFlags & (PF_Translucent|PF_Modulated) )
			grDepthMask( 1 );
	}

	// Unlock.
	unclock(Stats.PolyCTime);
	unguard;
}

/*-----------------------------------------------------------------------------
	Textured tiles.
-----------------------------------------------------------------------------*/

void UGlideRenderDevice::DrawTile( FSceneNode* Frame, FTextureInfo& Texture, FLOAT X, FLOAT Y, FLOAT XL, FLOAT YL, FLOAT U, FLOAT V, FLOAT UL, FLOAT VL, class FSpanBuffer* Span, FLOAT Z, FPlane Color, FPlane Fog, DWORD PolyFlags )
{
	guard(UGlideRenderDevice::DrawTile);
	clock(Stats.PolyCTime);
	Stats.Tris++;

	// Setup color.
	FColor GlideColor = FColor(Color);
	grConstantColorValue( *(GrColor_t*)&GlideColor );

	// Set up verts.
	GrVertex Verts[4];
	States[GR_TMU0].SetTexture( Texture, GF_NoScale | ((PolyFlags&PF_Masked)?GF_Alpha:0) );
	FLOAT RZ    = 1.0 / Z;
	X          += Frame->XB + 0.25;
	Y          += Frame->YB + 0.25;
	Verts[0].x=Mask(X   ); Verts[0].y=Mask(Y   ); Verts[0].oow=RZ; Verts[0].tmuvtx[0].sow=(U   )*RZ*States[GR_TMU0].UScale; Verts[0].tmuvtx[0].tow=(V   )*RZ*States[GR_TMU0].VScale;
	Verts[1].x=Mask(X   ); Verts[1].y=Mask(Y+YL); Verts[1].oow=RZ; Verts[1].tmuvtx[0].sow=(U   )*RZ*States[GR_TMU0].UScale; Verts[1].tmuvtx[0].tow=(V+VL)*RZ*States[GR_TMU0].VScale;
	Verts[2].x=Mask(X+XL); Verts[2].y=Mask(Y+YL); Verts[2].oow=RZ; Verts[2].tmuvtx[0].sow=(U+UL)*RZ*States[GR_TMU0].UScale; Verts[2].tmuvtx[0].tow=(V+VL)*RZ*States[GR_TMU0].VScale;
	Verts[3].x=Mask(X+XL); Verts[3].y=Mask(Y   ); Verts[3].oow=RZ; Verts[3].tmuvtx[0].sow=(U+UL)*RZ*States[GR_TMU0].UScale; Verts[3].tmuvtx[0].tow=(V   )*RZ*States[GR_TMU0].VScale;

	// Draw it.
	SetBlending( PolyFlags );
	guColorCombineFunction( (PolyFlags & PF_Modulated) ? GR_COLORCOMBINE_DECAL_TEXTURE : GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB );
	grDrawPlanarPolygonVertexList( 4, Verts );
	ResetBlending( PolyFlags );

	// Fog.
	if( PolyFlags & PF_RenderFog )
	{
		if( PolyFlags & (PF_Translucent|PF_Modulated) )
			grDepthMask( 0 );
		GlideColor = FColor(Fog);
		grConstantColorValue( *(GrColor_t*)&GlideColor );
		grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ONE_MINUS_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );
		guColorCombineFunction( GR_COLORCOMBINE_TEXTURE_TIMES_CCRGB );
		grDrawPlanarPolygonVertexList( 4, Verts );
		grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ZERO, GR_BLEND_ZERO );
		if( PolyFlags & (PF_Translucent|PF_Modulated) )
			grDepthMask( 1 );
	}

	// Unlock.
	unclock(Stats.PolyCTime);

	unguard;
}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/

//
// Get stats.
//
void UGlideRenderDevice::GetStats( char* Result )
{
	guard(UGlideRenderDevice::GetStats);
	appSprintf
	(
		Result,
		"pal=%03i (%04.1f) down8=%03i (%04.1f) down16=%03i (%04.1f) surfs=%03i poly=%03i tris=%03i pv=%04.1f pc=%04.1f",
		Stats.DownloadsPalette,
		GSecondsPerCycle*1000 * Stats.PaletteTime,
		Stats.Downloads8,
		GSecondsPerCycle*1000 * Stats.Download8Time,
		Stats.Downloads16,
		GSecondsPerCycle*1000 * Stats.Download16Time,
		Stats.Surfs,
		Stats.Polys,
		Stats.Tris,
		GSecondsPerCycle*1000 * Stats.PolyVTime,
		GSecondsPerCycle*1000 * Stats.PolyCTime
	);
	unguard;
}

//
// Execute a command.
//
UBOOL UGlideRenderDevice::Exec( const char* Cmd, FOutputDevice* Out )
{
	guard(UGlideRenderDevice::Exec);
	if( ParseCommand(&Cmd,"GetRes") )
	{
		Out->Logf( "512x384 640x480 800x600 1024x768" );
		return 1;
	}
	else if( ParseCommand(&Cmd,"SetRes") )
	{
		INT X=appAtoi(Cmd), Y=appAtoi(appStrchr(Cmd,'x') ? appStrchr(Cmd,'x')+1 : appStrchr(Cmd,'X') ? appStrchr(Cmd,'X')+1 : "");
		if( X && Y )
		{
			grSstWinClose();
			if( !TryRes( X, Y, 1, RefreshRate ) )
				appErrorf( "Failed setting resolution" );
			Viewport->SizeX = Viewport->Client->ViewportX = grSstScreenWidth();
			Viewport->SizeY = Viewport->Client->ViewportY = grSstScreenHeight();
			Viewport->Client->SaveConfig();
			Flush();
		}
		return 1;
	}
	else return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Unimplemented.
-----------------------------------------------------------------------------*/

void UGlideRenderDevice::Draw2DLine( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FVector P1, FVector P2 )
{
	guard(UGlideRenderDevice::Draw2DLine);
	// Not implemented (not needed for Unreal I).
	unguard;
}
void UGlideRenderDevice::Draw2DPoint( FSceneNode* Frame, FPlane Color, DWORD LineFlags, FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2 )
{
	guard(UGlideRenderDevice::Draw2DPoint);
	// Not implemented (not needed for Unreal I).
	unguard;
}
void UGlideRenderDevice::PushHit( const BYTE* Data, INT Count )
{
	guard(UGlideRenderDevice::PushHit);
	// Not implemented (not needed for Unreal I).
	unguard;
}
void UGlideRenderDevice::PopHit( INT Count, UBOOL bForce )
{
	guard(UGlideRenderDevice::PopHit);
	// Not implemented (not needed for Unreal I).
	unguard;
}

/*-----------------------------------------------------------------------------
	Pixel reading.
-----------------------------------------------------------------------------*/

void UGlideRenderDevice::ReadPixels( FColor* InPixels )
{ 
	guard(ReadPixels);
	INT X=grSstScreenWidth();
	INT Y=grSstScreenHeight();
	BYTE* Pixels=(BYTE*)InPixels;
	DWORD* Data;

	// Lock the frame buffer.  
	GrLfbInfo_t lfbInfo;
	lfbInfo.size = sizeof( GrLfbInfo_t );
	grLfbLock
	(
		GR_LFB_READ_ONLY,
        GR_BUFFER_FRONTBUFFER,
        GR_LFBWRITEMODE_ANY,
        GR_ORIGIN_UPPER_LEFT,
        FXFALSE,
        &lfbInfo
	);

	// Read the frame buffer.
	FMemMark Mark(GMem);
	WORD* Buffer = New<_WORD>(GMem,X*Y);
	FxU16* Src = (WORD*)lfbInfo.lfbPtr;
	FxU16* Dest = Buffer;
	for( INT y=0; y<Y; y++ )
	{
		WORD* End = &Dest[X];
		while( Dest < End )
			*Dest++ = *Src++;
		Src += lfbInfo.strideInBytes/2 - X;
	}

	// Unlock the frame buffer.
	grLfbUnlock( GR_LFB_READ_ONLY, GR_BUFFER_FRONTBUFFER );
	for( INT i=0; i<X*Y; i++ )
	{
		Pixels[i*4+0] = (Buffer[i] & 0xf800) >> 8;
		Pixels[i*4+1] = (Buffer[i] & 0x07e0) >> 3;
		Pixels[i*4+2] = (Buffer[i] & 0x001f) << 3;
	}

	// Expand to truecolor.
	Data = (DWORD*)Pixels;
    for( y=0; y < Y; y++ )
	{
        for( INT x=0; x < X; x++ )
		{
            DWORD rgb = *Data;
            DWORD r = (rgb >> 16) & 0xFF;
            DWORD g = (rgb >> 8) & 0xFF;
            DWORD b = rgb & 0xFF;
            r += r>>5;
            g += g>>6;
            b += b>>5;
            if( r > 255 ) r = 255;
            if( g > 255 ) g = 255;
            if( b > 255 ) b = 255;
            rgb = (r<<16) | (g<<8) | b;
            *Data++ = rgb;
        }
    }

	// Init the gamma table.
	FLOAT Gamma=1.5*Viewport->Client->Brightness;
    DWORD GammaTable[256];
    for( INT x=0; x<256; x++ )
        GammaTable[x] = appFloor(appPow(x/255.,1.0/Gamma) * 255.0 + 0.5);

	// Gamma correct.
	Data = (DWORD*)Pixels;
    for( y=0; y<Y; y++ )
	{
        for( INT x=0; x<X; x++ )
		{
            DWORD r = GammaTable[(*Data >> 16) & 0xFF];
            DWORD g = GammaTable[(*Data >> 8 ) & 0xFF];
            DWORD b = GammaTable[(*Data >> 0 ) & 0xFF];
            *InPixels++ = FColor(r,g,b);
			Data++;
        }
    }
	
	Mark.Pop();
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
