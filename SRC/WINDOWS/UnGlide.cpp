/*=============================================================================
	UnGlide.cpp: Unreal support for the 3dfx Glide library.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

// Precompiled header.
#include "StdAfx.h"
#include "UnWnCam.h"
#include "UnRender.h"

// Unreal includes.
#include "UnRenDev.h"

// 3dfx Glide includes.
#define __MSC__
#include <Glide.h>

// Globals.
BOOL GGlideCheckErrors=1;

// Texture upload flags.
enum EGlideFlags
{
	GF_Alpha		  = 0x01, // 5551 rgba texture.
	GF_NoPalette      = 0x02, // Non-palettized.
	GF_NoScale        = 0x04, // Scale for precision adjust.
	GF_ForceTexture   = 0x08, // Force texture to be uploaded.
	GF_ForcePalette   = 0x10, // Force palette to be uploaded.
};

// Pixel formats.
union FGlideColor
{
	struct
	{
		WORD B:5;
		WORD G:5;
		WORD R:5;
		WORD A:1;
	} Color5551;
	struct
	{
		WORD B:5;
		WORD G:6;
		WORD R:5;
	} Color565;
};

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

//
// Gamma correct a value from 0-255.
//
inline FLOAT GammaCorrect256( FLOAT B, FLOAT Gamma )
{
	return 255.0 * exp( Gamma * log(B/255.0) );
}

/*-----------------------------------------------------------------------------
	Helper for dynalinking to GLIDE.DLL.
-----------------------------------------------------------------------------*/

//
// Class containing pointers to all Glide functions.
//
class FGlide
{
public:
	// From glide.h:
	void   (FX_CALL * grDrawLine)( const GrVertex *v1, const GrVertex *v2 );
	void   (FX_CALL * grDrawPlanarPolygon)( int nverts, const int ilist[], const GrVertex vlist[] );
	void   (FX_CALL * grDrawPlanarPolygonVertexList)( int nverts, const GrVertex vlist[] );
	void   (FX_CALL * grDrawPoint)( const GrVertex *pt );
	void   (FX_CALL * grDrawPolygon)( int nverts, const int ilist[], const GrVertex vlist[] );
	void   (FX_CALL * grDrawPolygonVertexList)( int nverts, const GrVertex vlist[] );
	void   (FX_CALL * grDrawTriangle)( const GrVertex *a, const GrVertex *b, const GrVertex *c );
	void   (FX_CALL * grBufferClear)( GrColor_t color, GrAlpha_t alpha, FxU16 depth );
	int    (FX_CALL * grBufferNumPending)( void );
	void   (FX_CALL * grBufferSwap)( int swap_interval );
	void   (FX_CALL * grRenderBuffer)( GrBuffer_t buffer );
	void   (FX_CALL * grErrorSetCallback)( GrErrorCallbackFnc_t fnc );
	void   (FX_CALL * grSstIdle)(void);
	FxU32  (FX_CALL * grSstVideoLine)( void );
	FxBool (FX_CALL * grSstVRetraceOn)( void );
	FxBool (FX_CALL * grSstIsBusy)( void );
	FxBool (FX_CALL * grSstOpen)( GrScreenResolution_t screen_resolution, GrScreenRefresh_t refresh_rate, GrColorFormat_t color_format, GrOriginLocation_t origin_location, GrSmoothingMode_t smoothing_filter, int num_buffers );
	FxBool (FX_CALL * grSstQueryHardware)( GrHwConfiguration *hwconfig );
	FxBool (FX_CALL * grSstQueryBoards)( GrHwConfiguration *hwconfig );
	void   (FX_CALL * grSstOrigin)(GrOriginLocation_t  origin);
	void   (FX_CALL * grSstSelect)( int which_sst );
	int    (FX_CALL * grSstScreenHeight)( void );
	int    (FX_CALL * grSstScreenWidth)( void );
	FxU32  (FX_CALL * grSstStatus)( void );
	void   (FX_CALL * grSstPassthruMode)( GrPassthruMode_t mode);
	void   (FX_CALL * grSstPerfStats)(GrSstPerfStats_t *pStats);
	void   (FX_CALL * grSstResetPerfStats)(void);
	void   (FX_CALL * grResetTriStats)();
	void   (FX_CALL * grTriStats)(FxU32 *trisProcessed, FxU32 *trisDrawn);
	void   (FX_CALL * grAlphaBlendFunction)( GrAlphaBlendFnc_t rgb_sf, GrAlphaBlendFnc_t rgb_df,GrAlphaBlendFnc_t alpha_sf, GrAlphaBlendFnc_t alpha_df );
	void   (FX_CALL * grAlphaCombine)( GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert );
	void   (FX_CALL * grAlphaControlsITRGBLighting)( FxBool enable );
	void   (FX_CALL * grAlphaTestFunction)( GrCmpFnc_t function );
	void   (FX_CALL * grAlphaTestReferenceValue)( GrAlpha_t value );
	void   (FX_CALL * grChromakeyMode)( GrChromakeyMode_t mode );
	void   (FX_CALL * grChromakeyValue)( GrColor_t value );
	void   (FX_CALL * grClipWindow)( int minx, int miny, int maxx, int maxy );
	void   (FX_CALL * grColorCombine)( GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert );
	void   (FX_CALL * grColorMask)( FxBool rgb, FxBool a );
	void   (FX_CALL * grCullMode)( GrCullMode_t mode );
	void   (FX_CALL * grConstantColorValue)( GrColor_t value );
	void   (FX_CALL * grConstantColorValue4)( float a, float r, float g, float b );
	void   (FX_CALL * grDepthBiasLevel)( FxI16 level );
	void   (FX_CALL * grDepthBufferFunction)( GrCmpFnc_t function );
	void   (FX_CALL * grDepthBufferMode)( GrDepthBufferMode_t mode );
	void   (FX_CALL * grDepthMask)( FxBool mask );
	void   (FX_CALL * grDisableAllEffects)( void );
	void   (FX_CALL * grDitherMode)( GrDitherMode_t mode );
	void   (FX_CALL * grFogColorValue)( GrColor_t fogcolor );
	void   (FX_CALL * grFogMode)( GrFogMode_t mode );
	void   (FX_CALL * grFogTable)( const GrFog_t ft[GR_FOG_TABLE_SIZE] );
	void   (FX_CALL * grGammaCorrectionValue)( float value );
	void   (FX_CALL * grSplash)(void);
	FxU32  (FX_CALL * grTexCalcMemRequired)( GrLOD_t lodmin, GrLOD_t lodmax, GrAspectRatio_t aspect, GrTextureFormat_t fmt );
	FxU32  (FX_CALL * grTexTextureMemRequired)( FxU32 evenOdd, GrTexInfo *info );
	FxU32  (FX_CALL * grTexMinAddress)( GrChipID_t tmu );
	FxU32  (FX_CALL * grTexMaxAddress)( GrChipID_t tmu );
	void   (FX_CALL * grTexNCCTable)( GrChipID_t tmu, GrNCCTable_t table );
	void   (FX_CALL * grTexSource)( GrChipID_t tmu, FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info );
	void   (FX_CALL * grTexClampMode)( GrChipID_t tmu, GrTextureClampMode_t s_clampmode, GrTextureClampMode_t t_clampmode );
	void   (FX_CALL * grTexCombine)( GrChipID_t tmu, GrCombineFunction_t rgb_function, GrCombineFactor_t rgb_factor,  GrCombineFunction_t alpha_function, GrCombineFactor_t alpha_factor, FxBool rgb_invert, FxBool alpha_invert );
	void   (FX_CALL * grTexCombineFunction)( GrChipID_t tmu, GrTextureCombineFnc_t fnc );
	void   (FX_CALL * grTexDetailControl)( GrChipID_t tmu, int lod_bias, FxU8 detail_scale, float detail_max );
	void   (FX_CALL * grTexFilterMode)( GrChipID_t tmu, GrTextureFilterMode_t minfilter_mode, GrTextureFilterMode_t magfilter_mode );
	void   (FX_CALL * grTexLodBiasValue)(GrChipID_t tmu, float bias );
	void   (FX_CALL * grTexDownloadMipMap)( GrChipID_t tmu, FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info );
	void   (FX_CALL * grTexDownloadMipMapLevel)( GrChipID_t tmu, FxU32 startAddress, GrLOD_t thisLod, GrLOD_t largeLod, GrAspectRatio_t aspectRatio, GrTextureFormat_t format, FxU32 evenOdd, void *data );
	void   (FX_CALL * grTexDownloadMipMapLevelPartial)( GrChipID_t tmu, FxU32 startAddress, GrLOD_t thisLod, GrLOD_t largeLod, GrAspectRatio_t aspectRatio, GrTextureFormat_t format, FxU32 evenOdd, void *data, int start, int end );
	void   (FX_CALL * grTexDownloadTable)( GrChipID_t tmu, GrTexTable_t type, void *data );
	void   (FX_CALL * grTexDownloadTablePartial)( GrChipID_t tmu, GrTexTable_t type, void *data, int start, int end );
	void   (FX_CALL * grTexMipMapMode)( GrChipID_t tmu, GrMipMapMode_t mode, FxBool lodBlend );
	void   (FX_CALL * grTexMultibase)( GrChipID_t tmu, FxBool enable );
	void   (FX_CALL * grTexMultibaseAddress)( GrChipID_t tmu, GrTexBaseRange_t range, FxU32 startAddress, FxU32 evenOdd, GrTexInfo *info );
	GrMipMapId_t (FX_CALL * guTexAllocateMemory)( GrChipID_t tmu, FxU8 odd_even_mask, int width, int height, GrTextureFormat_t fmt, GrMipMapMode_t mm_mode, GrLOD_t smallest_lod, GrLOD_t largest_lod, GrAspectRatio_t aspect, GrTextureClampMode_t s_clamp_mode, GrTextureClampMode_t t_clamp_mode, GrTextureFilterMode_t minfilter_mode, GrTextureFilterMode_t magfilter_mode, float lod_bias, FxBool trilinear );
	FxBool (FX_CALL * guTexChangeAttributes)( GrMipMapId_t mmid, int width, int height, GrTextureFormat_t fmt, GrMipMapMode_t mm_mode, GrLOD_t smallest_lod, GrLOD_t largest_lod, GrAspectRatio_t aspect, GrTextureClampMode_t s_clamp_mode, GrTextureClampMode_t t_clamp_mode, GrTextureFilterMode_t minFilterMode, GrTextureFilterMode_t magFilterMode );
	void   (FX_CALL * guTexCombineFunction)( GrChipID_t tmu, GrTextureCombineFnc_t fnc );
	GrMipMapId_t (FX_CALL * guTexGetCurrentMipMap)( GrChipID_t tmu );
	GrMipMapInfo* (FX_CALL * guTexGetMipMapInfo)( GrMipMapId_t mmid );
	FxU32  (FX_CALL * guTexMemQueryAvail)( GrChipID_t tmu );
	void   (FX_CALL * guTexMemReset)( void );
	void   (FX_CALL * guTexDownloadMipMap)( GrMipMapId_t mmid, const void *src, const GuNccTable *table );
	void   (FX_CALL * guTexDownloadMipMapLevel)( GrMipMapId_t mmid, GrLOD_t lod, const void **src );
	void   (FX_CALL * guTexSource)( GrMipMapId_t id );
	void   (FX_CALL * grLfbBegin)( void );
	void   (FX_CALL * grLfbBypassMode)( GrLfbBypassMode_t mode );
	void   (FX_CALL * grLfbConstantAlpha)( GrAlpha_t alpha );
	void   (FX_CALL * grLfbConstantDepth)( FxU16 depth );
	void   (FX_CALL * grLfbEnd)( void );
	const FxU32 * (FX_CALL * grLfbGetReadPtr)( GrBuffer_t buffer );
	void * (FX_CALL * grLfbGetWritePtr)( GrBuffer_t buffer );
	void   (FX_CALL * grLfbOrigin)(GrOriginLocation_t origin);
	void   (FX_CALL * grLfbWriteMode)( GrLfbWriteMode_t mode );
	void   (FX_CALL * grLfbWriteColorFormat)(GrColorFormat_t colorFormat);
	void   (FX_CALL * grLfbWriteColorSwizzle)(FxBool swizzleBytes, FxBool swapWords);
	void   (FX_CALL * grAADrawLine)(const GrVertex *v1, const GrVertex *v2);
	void   (FX_CALL * grAADrawPoint)(const GrVertex *pt );
	void   (FX_CALL * grAADrawPolygon)(const int nverts, const int ilist[], const GrVertex vlist[]);
	void   (FX_CALL * grAADrawPolygonVertexList)(const int nverts, const GrVertex vlist[]);
	void   (FX_CALL * grAADrawTriangle)( const GrVertex *a, const GrVertex *b, const GrVertex *c, FxBool ab_antialias, FxBool bc_antialias, FxBool ca_antialias );
	void   (FX_CALL * grGlideInit)( void );
	void   (FX_CALL * grGlideShutdown)( void );
	void   (FX_CALL * grGlideGetVersion)( char version[80] );
	void   (FX_CALL * grGlideGetState)( GrState *state );
	void   (FX_CALL * grGlideSetState)( const GrState *state );
	void   (FX_CALL * grGlideShamelessPlug)( const FxBool on );
	void   (FX_CALL * grHints)(GrHint_t hintType, FxU32 hintMask);

	// From glideutl.h.
	void   (FX_CALL * guAADrawTriangleWithClip)( const GrVertex *a, const GrVertex *b, const GrVertex *c);
	void   (FX_CALL * guDrawTriangleWithClip)( const GrVertex *a, const GrVertex *b, const GrVertex *c );
	void   (FX_CALL * guDrawPolygonVertexListWithClip)( int nverts, const GrVertex vlist[] );
	void   (FX_CALL * guAlphaSource)( GrAlphaSource_t mode );
	void   (FX_CALL * guColorCombineFunction)( GrColorCombineFnc_t fnc );
	void   (FX_CALL * guFbReadRegion)( const int src_x, const int src_y, const int w, const int h, const void *dst, const int strideInBytes );
	void   (FX_CALL * guFbWriteRegion)( const int dst_x, const int dst_y, const int w, const int h, const void *src, const int strideInBytes );
	FxU16* (FX_CALL * guTexCreateColorMipMap)( void );
	float  (FX_CALL * guFogTableIndexToW)( int i );
	void   (FX_CALL * guFogGenerateExp)( GrFog_t fogtable[GR_FOG_TABLE_SIZE], float density );
	void   (FX_CALL * guFogGenerateExp2)( GrFog_t fogtable[GR_FOG_TABLE_SIZE], float density );
	void   (FX_CALL * guFogGenerateLinear)( GrFog_t fogtable[GR_FOG_TABLE_SIZE], float nearZ, float farZ );
	FxU32  (FX_CALL * guEndianSwapWords)( FxU32 value );
	FxU16  (FX_CALL * guEndianSwapBytes)( FxU16 value );
	FxBool (FX_CALL * gu3dfGetInfo)( const char *filename, Gu3dfInfo *info );
	FxBool (FX_CALL * gu3dfLoad)( const char *filename, Gu3dfInfo *data );

	// From gump.h.
	void   (FX_CALL * guMPInit)( void );
	void   (FX_CALL * guMPDrawTriangle)( const GrVertex *a, const GrVertex *b, const GrVertex *c );
	//void   (FX_CALL * guMPTexCombineFunction)( GrMPTextureCombineFnc_t tc );
	//void   (FX_CALL * guMPTexSource)( GrChipID_t virtual_tmu, GrMipMapId_t mmid );
	void   (FX_CALL * guMPTexCombineFunction)( int Broken );
	void   (FX_CALL * guMPTexSource)( int Broken );

	// Variables.
	HINSTANCE hModule;
	int Ok;
	FARPROC Find( char *Name, int Num );
	int Associate();
};

//
// Find an entry point into the Glide DLL.
//
FARPROC FGlide::Find( char *Name, int Num )
{
	guard(FGlide::Find);
	char Frack[80];

#if 1
	sprintf(Frack,"_%s@%i",Name,Num);
	FARPROC Result = GetProcAddress(hModule,Frack);
	if( Result ) return Result;
#else
	for (int i=0; i<256; i++ )
	{
		sprintf(Frack,"_%s@%i",Name,i);
		FARPROC Result = GetProcAddress(hModule,Frack);
		if( Result )
		{
			debugf("GLIDE_GET(%s,%i)",Name,i);
			return Result;
		}
	}
#endif

	Ok = 0;
	appErrorf("Glide can't find: %s",Frack);
	return NULL;
	unguard;
}

//
// Try to import all of ATI's functions from ATI's DLL.
// Returns 1 if success, 0 if failure.
//
int FGlide::Associate()
{
	guard(FGlide::Associate);

	hModule = LoadLibrary("GLIDE.DLL");
	if( !hModule ) return 0;
	debug( LOG_Init, "Found 3dfx GLIDE.DLL" );
	Ok = 1;
	#define GLIDE_GET(FuncName,Num) *(FARPROC *)&FuncName = Find(#FuncName,Num);

	GLIDE_GET(grDrawLine,8);
	GLIDE_GET(grDrawPlanarPolygon,12);
	GLIDE_GET(grDrawPlanarPolygonVertexList,8);
	GLIDE_GET(grDrawPoint,4);
	GLIDE_GET(grDrawPolygon,12);
	GLIDE_GET(grDrawPolygonVertexList,8);
	GLIDE_GET(grDrawTriangle,12);
	GLIDE_GET(grBufferClear,12);
	GLIDE_GET(grBufferNumPending,0);
	GLIDE_GET(grBufferSwap,4);
	GLIDE_GET(grRenderBuffer,4);
	GLIDE_GET(grErrorSetCallback,4);
	GLIDE_GET(grSstIdle,0);
	GLIDE_GET(grSstVideoLine,0);
	GLIDE_GET(grSstVRetraceOn,0);
	GLIDE_GET(grSstIsBusy,0);
	GLIDE_GET(grSstOpen,24);
	GLIDE_GET(grSstQueryHardware,4);
	GLIDE_GET(grSstQueryBoards,4);
	GLIDE_GET(grSstOrigin,4);
	GLIDE_GET(grSstSelect,4);
	GLIDE_GET(grSstScreenHeight,0);
	GLIDE_GET(grSstScreenWidth,0);
	GLIDE_GET(grSstStatus,0);
	GLIDE_GET(grSstPassthruMode,4);
	GLIDE_GET(grSstPerfStats,4);
	GLIDE_GET(grSstResetPerfStats,0);
	GLIDE_GET(grResetTriStats,0);
	GLIDE_GET(grTriStats,8);
	GLIDE_GET(grAlphaBlendFunction,16);
	GLIDE_GET(grAlphaCombine,20);
	GLIDE_GET(grAlphaControlsITRGBLighting,4);
	GLIDE_GET(grAlphaTestFunction,4);
	GLIDE_GET(grAlphaTestReferenceValue,4);
	GLIDE_GET(grChromakeyMode,4);
	GLIDE_GET(grChromakeyValue,4);
	GLIDE_GET(grClipWindow,16);
	GLIDE_GET(grColorCombine,20);
	GLIDE_GET(grColorMask,8);
	GLIDE_GET(grCullMode,4);
	GLIDE_GET(grConstantColorValue,4);
	GLIDE_GET(grConstantColorValue4,16);
	GLIDE_GET(grDepthBiasLevel,4);
	GLIDE_GET(grDepthBufferFunction,4);
	GLIDE_GET(grDepthBufferMode,4);
	GLIDE_GET(grDepthMask,4);
	GLIDE_GET(grDisableAllEffects,0);
	GLIDE_GET(grDitherMode,4);
	GLIDE_GET(grFogColorValue,4);
	GLIDE_GET(grFogMode,4);
	GLIDE_GET(grFogTable,4);
	GLIDE_GET(grGammaCorrectionValue,4);
	GLIDE_GET(grSplash,0);
	GLIDE_GET(grTexCalcMemRequired,16);
	GLIDE_GET(grTexTextureMemRequired,8);
	GLIDE_GET(grTexMinAddress,4);
	GLIDE_GET(grTexMaxAddress,4);
	GLIDE_GET(grTexNCCTable,8);
	GLIDE_GET(grTexSource,16);
	GLIDE_GET(grTexClampMode,12);
	GLIDE_GET(grTexCombine,28);
	GLIDE_GET(grTexCombineFunction,8);
	GLIDE_GET(grTexDetailControl,16);
	GLIDE_GET(grTexFilterMode,12);
	GLIDE_GET(grTexLodBiasValue,8);
	GLIDE_GET(grTexDownloadMipMap,16);
	GLIDE_GET(grTexDownloadMipMapLevel,32);
	GLIDE_GET(grTexDownloadMipMapLevelPartial,40);
	GLIDE_GET(grTexDownloadTable,12);
	GLIDE_GET(grTexDownloadTablePartial,20);
	GLIDE_GET(grTexMipMapMode,12);
	GLIDE_GET(grTexMultibase,8);
	GLIDE_GET(grTexMultibaseAddress,20);
	GLIDE_GET(guTexAllocateMemory,60);
	GLIDE_GET(guTexChangeAttributes,48);
	GLIDE_GET(guTexCombineFunction,8);
	GLIDE_GET(guTexGetCurrentMipMap,4);
	GLIDE_GET(guTexGetMipMapInfo,4);
	GLIDE_GET(guTexMemQueryAvail,4);
	GLIDE_GET(guTexMemReset,0);
	GLIDE_GET(guTexDownloadMipMap,12);
	GLIDE_GET(guTexDownloadMipMapLevel,12);
	GLIDE_GET(guTexSource,4);
	GLIDE_GET(grLfbBegin,0);
	GLIDE_GET(grLfbBypassMode,4);
	GLIDE_GET(grLfbConstantAlpha,4);
	GLIDE_GET(grLfbConstantDepth,4);
	GLIDE_GET(grLfbEnd,0);
	GLIDE_GET(grLfbGetReadPtr,4);
	GLIDE_GET(grLfbGetWritePtr,4);
	GLIDE_GET(grLfbOrigin,4);
	GLIDE_GET(grLfbWriteMode,4);
	GLIDE_GET(grLfbWriteColorFormat,4);
	GLIDE_GET(grLfbWriteColorSwizzle,8);
	GLIDE_GET(grAADrawLine,8);
	GLIDE_GET(grAADrawPoint,4);
	GLIDE_GET(grAADrawPolygon,12);
	GLIDE_GET(grAADrawPolygonVertexList,8);
	GLIDE_GET(grAADrawTriangle,24);
	GLIDE_GET(grGlideInit,0);
	GLIDE_GET(grGlideShutdown,0);
	GLIDE_GET(grGlideGetVersion,4);
	GLIDE_GET(grGlideGetState,4);
	GLIDE_GET(grGlideSetState,4);
	GLIDE_GET(grGlideShamelessPlug,4);
	GLIDE_GET(grHints,8);
	GLIDE_GET(guAADrawTriangleWithClip,12);
	GLIDE_GET(guDrawTriangleWithClip,12);
	GLIDE_GET(guDrawPolygonVertexListWithClip,8);
	GLIDE_GET(guAlphaSource,4);
	GLIDE_GET(guColorCombineFunction,4);
	GLIDE_GET(guFbReadRegion,24);
	GLIDE_GET(guFbWriteRegion,24);
	GLIDE_GET(guTexCreateColorMipMap,0);
	GLIDE_GET(guFogTableIndexToW,4);
	GLIDE_GET(guFogGenerateExp,8);
	GLIDE_GET(guFogGenerateExp2,8);
	GLIDE_GET(guFogGenerateLinear,12);
	GLIDE_GET(guEndianSwapWords,4);
	GLIDE_GET(guEndianSwapBytes,4);
	GLIDE_GET(gu3dfGetInfo,8);
	GLIDE_GET(gu3dfLoad,8);
	GLIDE_GET(guMPInit,0);
	GLIDE_GET(guMPTexCombineFunction,4);
	GLIDE_GET(guMPTexSource,8);
	GLIDE_GET(guMPDrawTriangle,12);

	#undef GLIDE_GET
	return Ok;
	unguard;
}

/*-----------------------------------------------------------------------------
	Global Glide error handler.
-----------------------------------------------------------------------------*/

//
// Handle a Glide error.
//
void GlideErrorHandler( const char *String, FxBool Fatal )
{
	guard(GlideErrorHandler);
	if( GGlideCheckErrors )
		appErrorf("Glide error: %s",String);
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
	int PaletteDownloads;
	int TextureDownloads;
	int LightMeshDownloads;
	SQWORD Microseconds;
	DWORD Time, Polys;

	// Functions.
	void Init()
	{
		memset(this,0,sizeof(*this));
		Microseconds = GApp->MicrosecondTime();
	}
	void Display( FOutputDevice &Out )
	{
#if 0
		Microseconds = GApp->MicrosecondTime() - Microseconds;
		Out.Logf("Glide: ms=%3.1f pal=%i tex=%i ltmesh=%i poly=%i timer=%03.2f",Microseconds/1000.0,PaletteDownloads,TextureDownloads,LightMeshDownloads,Polys,GApp->CpuToMilliseconds(Time));
#endif
	}
} Stats;

/*-----------------------------------------------------------------------------
	FGlideRenderDevice definition.
-----------------------------------------------------------------------------*/

//
// The 3dfx Glide rendering device.
//
class FGlideRenderDevice : public FRenderDevice, public FGlide
{
public:
	// Constants.
	enum {MAX_TMUS=3};
	enum {ALIGNMENT=8};

	// Variables.
	GrHwConfiguration	hwconfig;
	UCamera				SavedCamera;
	INT					FullLight;
	INT					NumTmu;
	INT					ColoredLight;
	
	// State of a texture map unit.
	struct FGlideTMU
	{
		// Owner.
		FGlideRenderDevice *Glide;

		// Texture map unit.
		INT tmu;

		// Memory cache for the tmu.
		FMemCache Cache;

		// State variables.
		UTexture	*Texture;
		UPalette	*Palette;
		FColor		TextureMaxColor;
		FColor		PaletteMaxColor;
		FCacheItem  *TextureItem;
		INT			iLightMesh;
		DWORD		GlideFlags;
		FLOAT		Scale;

		// tmu download functions.
		DWORD DownloadTexture( UTexture *Texture, DWORD GlideFlags, DWORD CacheID, INT iFirstMip, INT iLastMip, GrTexInfo *texinfo, INT UCopies, INT VCopies );
		void DownloadLightMesh( DWORD Base, GrTexInfo *texinfo, INT UCopies, INT VCopies );
		void DownloadPalette( UPalette *Palette, FColor InPaletteMaxColor );

		// State functions.
		void Init( int Intmu, FGlideRenderDevice *InGlide )
		{
			guard(FGlideTMU::Init);

			// Init variables.
			Glide			= InGlide;
			tmu				= Intmu;
			Texture			= NULL;
			Palette			= NULL;
			TextureMaxColor	= FColor(255,255,255,255);
			PaletteMaxColor = FColor(255,255,255,255);
			TextureItem		= NULL;
			iLightMesh		= 0;

			// Workaround for Glide bug writing to first 8 bytes of texture memory.
			INT Phudge = 8;

			// Init cache.
			Cache.Init
			(
				Glide->grTexMaxAddress(tmu) - Glide->grTexMinAddress(tmu) - Phudge,
				2048,
				(void*)(Glide->grTexMinAddress(tmu) + Phudge),
				2 * 1024 * 1024
			);

			// Init Glide state.
			unguard;
		}
		void Exit()
		{
			guard(FGlideTMU::Exit);

			// Shut down the texture cache.
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
				Texture		= NULL;
				iLightMesh  = INDEX_NONE;
			}
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
			Cache.Flush();
			unguard;
		}
		void SetLightMesh( INDEX iInLightMesh, BOOL IsDynamic )
		{
			guard(FGlideTMU::SetLightMesh);
			checkInput(iInLightMesh!=INDEX_NONE);
			if( iLightMesh != iInLightMesh )
			{
				// Make a texinfo.
				ResetTexture();
				iLightMesh = iInLightMesh;

				// Make a texinfo.
				GrTexInfo texinfo;
				texinfo.format      = Glide->ColoredLight ? GR_TEXFMT_RGB_565 : GR_TEXFMT_INTENSITY_8;
				texinfo.aspectRatio = GLightManager->MeshVBits + 3 - GLightManager->MeshUBits;
				INT MaxBits			= Max(GLightManager->MeshUBits, GLightManager->MeshVBits);
				texinfo.largeLod    = texinfo.smallLod = GR_LOD_1 - MaxBits;
				checkState(MaxBits<=8);

				// Validate it.
				int UCopies=1, VCopies=1;
				if( texinfo.aspectRatio < GR_ASPECT_8x1 )
				{
					VCopies = VCopies << (GR_ASPECT_8x1 - texinfo.aspectRatio);
					texinfo.aspectRatio = GR_ASPECT_8x1;
				}
				else if( texinfo.aspectRatio > GR_ASPECT_1x8 )
				{
					UCopies = UCopies << (texinfo.aspectRatio - GR_ASPECT_1x8);
					texinfo.aspectRatio = GR_ASPECT_1x8;
				}

				// Generate texture if needed.
				INT   OldTime;
				DWORD CacheID = MakeCacheID( CID_3dfxLightMap, iLightMesh, 0);
				TextureItem   = NULL;
				DWORD Address = (DWORD)Cache.GetEx( CacheID, TextureItem, OldTime, ALIGNMENT );
				if( !Address || (IsDynamic && OldTime!=Cache.GetTime()) )
				{
					if( !Address ) Address = (DWORD)Cache.Create
					(
						CacheID, 
						TextureItem,
						GLightManager->MeshTileSpace * (1 + Glide->ColoredLight) * UCopies * VCopies,
						ALIGNMENT
					);
					DownloadLightMesh( Address, &texinfo, UCopies, VCopies );
				}

				// Make it current.
				Glide->grTexSource( tmu, Address, GR_MIPMAPLEVELMASK_BOTH, &texinfo );
			}
			unguard;
		}
		void SetTexture( UTexture* InTexture, UPalette* InPalette, DWORD InGlideFlags )
		{
			guard(FGlideTMU::SetTexture);
			if( Texture!=InTexture || GlideFlags!=InGlideFlags || (InGlideFlags & GF_ForceTexture) )
			{
				// Set TextureMaxColor.
				if( InGlideFlags & (GF_Alpha|GF_NoPalette|GF_NoScale) )
					TextureMaxColor = FColor(255,255,255,255);
				else
					TextureMaxColor = InTexture->MaxColor;

				// Get the texture into 3dfx memory.
				ResetTexture();
				Texture    = InTexture;
				GlideFlags = InGlideFlags;

				// Make a texinfo.
				GrTexInfo texinfo;
				texinfo.format      = (GlideFlags & GF_Alpha    ) ? GR_TEXFMT_ARGB_1555
									: (GlideFlags & GF_NoPalette) ? GR_TEXFMT_RGB_565
									:                               GR_TEXFMT_P_8;
				
				texinfo.aspectRatio = Texture->VBits + 3 - Texture->UBits;
				int MaxBits			= Max(Texture->UBits, Texture->VBits);
				int iFirstMip		= MaxBits<=8 ? 0                  : MaxBits-8;
				int iLastMip        = MaxBits<=8 ? MaxBits            : 8;
				texinfo.largeLod    = MaxBits<=8 ? GR_LOD_1 - MaxBits : GR_LOD_256;
				texinfo.smallLod    = GR_LOD_1;
				Scale               = 256 >> iFirstMip;

				// Validate it.
				int UCopies=1, VCopies=1;
				if( texinfo.aspectRatio < GR_ASPECT_8x1 )
				{
					VCopies = VCopies << (GR_ASPECT_8x1 - texinfo.aspectRatio);
					texinfo.aspectRatio = GR_ASPECT_8x1;
				}
				else if( texinfo.aspectRatio > GR_ASPECT_1x8 )
				{
					UCopies = UCopies << (texinfo.aspectRatio - GR_ASPECT_1x8);
					texinfo.aspectRatio = GR_ASPECT_1x8;
				}

				// Generate texture if needed.
				DWORD CacheID = MakeCacheID( CID_3dfxTexture, Texture->GetIndex(), GlideFlags );
				DWORD Address = (DWORD)Cache.Get( CacheID, TextureItem, ALIGNMENT );
				if( !Address )
					Address = DownloadTexture( Texture, GlideFlags, CacheID, iFirstMip, iLastMip, &texinfo, UCopies, VCopies );

				// Make it current.
				Glide->grTexSource( tmu, Address, GR_MIPMAPLEVELMASK_BOTH, &texinfo );
			}
			if( !(InGlideFlags & (GF_Alpha|GF_NoPalette)) )
			{
				if( (InGlideFlags & GF_ForcePalette) || Palette!=InPalette || PaletteMaxColor!=TextureMaxColor )
				DownloadPalette( InPalette, TextureMaxColor );
			}
			unguard;
		}
	} States[MAX_TMUS];

	// FRenderDevice interface.
	int Init3D( UCamera *Camera, int RequestX, int RequestY );
	void Exit3D();
	void Flush3D();
	void Lock3D(UCamera *Camera);
	void Unlock3D(UCamera *Camera,int Blit);
	void DrawPolyV(UCamera *Camera,UTexture *Texture,const FTransform *Pts,int NumPts,
		const FVector &Base, const FVector &Normal, const FVector &U, const FVector &V, FLOAT PanU, FLOAT PanV,
		DWORD PolyFlags);
	void DrawPolyC(UCamera *Camera,UTexture *Texture,const FTransTexture *Pts,int NumPts,DWORD PolyFlags);
	int Exec(const char *Cmd,FOutputDevice *Out);

	// State cache.
	INT DepthBuffering;

	// Glide specific functions.
	void SetBlending( DWORD PolyFlags )
	{
		if( !(PolyFlags & (PF_Transparent|PF_Masked|PF_InternalUnused1)) )
		{
			// Normal surface.
			grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ZERO, GR_BLEND_ZERO );
		}
		else if( PolyFlags & PF_Transparent )
		{
			// Transparent surface.
			grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ZERO );
		}
		else if( PolyFlags & PF_InternalUnused1 )
		{
			// More transparent surface.
			FColor C(0,0,0,80);
			grConstantColorValue( *(GrColor_t*)&C );
			guAlphaSource(GR_ALPHASOURCE_CC_ALPHA);
			grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_SRC_ALPHA, GR_BLEND_ZERO, GR_BLEND_ZERO );
			grTexFilterMode( GR_TMU0, GR_TEXTUREFILTER_POINT_SAMPLED, GR_TEXTUREFILTER_POINT_SAMPLED );
		}
		else
		{
			// Masked surface.
			guAlphaSource( GR_ALPHASOURCE_TEXTURE_ALPHA );
			grAlphaBlendFunction( GR_BLEND_SRC_ALPHA, GR_BLEND_ONE_MINUS_SRC_ALPHA, GR_BLEND_ZERO, GR_BLEND_ZERO );
		}
	}
	void ResetBlending( DWORD PolyFlags )
	{
		if( PolyFlags & PF_InternalUnused1 )
		{
			grTexFilterMode( GR_TMU0, GR_TEXTUREFILTER_BILINEAR, GR_TEXTUREFILTER_BILINEAR );
		}
	}
	void ClearZBuffer()
	{
		guard(FGlideRenderDevice::ClearZBuffer);

		grColorMask  ( FXFALSE, FXTRUE );
		grBufferClear( 0x008f8f8f, 0, GR_WDEPTHVALUE_FARTHEST );
		grColorMask  ( FXTRUE, FXTRUE );

		unguard;
	}
	void SetDepthBuffering( int InDepthBuffering )
	{
#if 0 /* grDepthMask is buggy */
		if( InDepthBuffering != DepthBuffering )
		{
			DepthBuffering = InDepthBuffering;
			grDepthMask( DepthBuffering );
		}
#endif
	}
};

/*-----------------------------------------------------------------------------
	FGlideRenderDevice Init & Exit.
-----------------------------------------------------------------------------*/

//
// Initializes Glide.  Can't fail.
//
int FGlideRenderDevice::Init3D( UCamera *Camera, int RequestX, int RequestY )
{
	guard(FGlideRenderDevice::Init3D);
	checkState(!Active);
	debugf("Initializing Glide...");

	// Checks.
	checkState(sizeof(FGlideColor)==2);

	// Initialize the Glide library.
	grGlideInit();

	// Set error callback.
	grErrorSetCallback( GlideErrorHandler );

	// Make sure 3Dfx hardware is present.
	GGlideCheckErrors=0;
	if( !grSstQueryHardware( &hwconfig ) )
	{
		grGlideShutdown();
		debugf( "grSstQueryHardware failed" );
		return 0;
    }
	GGlideCheckErrors=1;

	// Check hardware info.
	debugf
	(
		"Glide info: Type=%i, fbRam=%i fbiRev=%i nTexelfx=%i Sli=%i",
		hwconfig.SSTs[0].type,
		hwconfig.SSTs[0].sstBoard.VoodooConfig.fbRam,
		hwconfig.SSTs[0].sstBoard.VoodooConfig.fbiRev,
		hwconfig.SSTs[0].sstBoard.VoodooConfig.nTexelfx,
		(int)hwconfig.SSTs[0].sstBoard.VoodooConfig.sliDetect
	);
	NumTmu = Min(hwconfig.SSTs[0].sstBoard.VoodooConfig.nTexelfx,(int)MAX_TMUS);
	checkState(NumTmu>0);
	for( int tmu=0; tmu<NumTmu; tmu++ )
		debugf
		(
			"Glide tmu %i: tmuRev=%i tmuRam=%i Space=%i", tmu,
			hwconfig.SSTs[0].sstBoard.VoodooConfig.tmuConfig[tmu].tmuRev,
			hwconfig.SSTs[0].sstBoard.VoodooConfig.tmuConfig[tmu].tmuRam,
			grTexMaxAddress(tmu) - grTexMinAddress(tmu)
		);

	// Set variables.
	FullLight = 255;

	// Init state cache.
	DepthBuffering	= 0;

	// Select the first board.
	grSstSelect( 0 );

	// Pick a refresh rate.
	GrScreenRefresh_t Ref;
	int Rate=0; GetINT(GApp->CmdLine,"REFRESH=",&Rate);
	if		( Rate==60						 )	Ref = GR_REFRESH_60Hz;
	if		( Rate==70						 )	Ref = GR_REFRESH_70Hz;
	if		( Rate==72						 )	Ref = GR_REFRESH_72Hz;
	if		( Rate==75						 )	Ref = GR_REFRESH_75Hz;
	if		( Rate==80						 )	Ref = GR_REFRESH_80Hz;
	if		( Rate==85						 )	Ref = GR_REFRESH_85Hz;
	if		( Rate==100						 )	Ref = GR_REFRESH_100Hz;
	if		( Rate==120						 )	Ref = GR_REFRESH_120Hz;
	else										Ref = GR_REFRESH_100Hz;

	// Pick an appropriate resolution.
	GrScreenResolution_t Res;
	if		( RequestX<=320 && RequestY<=200 )	Res = GR_RESOLUTION_320x200;
	else if ( RequestX<=320					 )	Res = GR_RESOLUTION_320x240;
	else if ( RequestX<=400					 )	Res = GR_RESOLUTION_400x256;
	else if ( RequestX<=512					 )	Res = GR_RESOLUTION_512x384;
	else if ( RequestX<=640 && RequestY<=400 )	Res = GR_RESOLUTION_640x400;
	else										Res = GR_RESOLUTION_512x384;

	// Open the display.
	Retry:
	GGlideCheckErrors=0;
	if( !grSstOpen( Res, Ref, GR_COLORFORMAT_ARGB, GR_ORIGIN_UPPER_LEFT, GR_SMOOTHING_ENABLE, 2 ))
    {
		if( Res != GR_RESOLUTION_512x384 )
		{
			// Try again.
			Res = GR_RESOLUTION_512x384;
			goto Retry;
		}
		else if( Ref != GR_REFRESH_72Hz )
		{
			// Try again.
			Ref = GR_REFRESH_72Hz;
			goto Retry;
		}
		grGlideShutdown();
		debugf( "grSstOpen failed (%i, %i)", Ref, Res );
		return 0;
    }
	GGlideCheckErrors = 1;

	// Set depth buffering.
	grDepthBufferMode( GR_DEPTHBUFFER_WBUFFER );
	grDepthMask( 1 );

	// Set dithering.
	grDitherMode( GR_DITHER_4x4 ); // GR_DITHER_DISABLE GR_DITHER_2x2 GR_DITHER_4x4

	// Set gamma.
	grGammaCorrectionValue( 1.4 );

	// Init chroma keying and alpha testing.
	grChromakeyValue(0);
	grChromakeyMode(0);
	grAlphaTestReferenceValue( 127.0 );

	// Fog.
	GrFog_t fog[GR_FOG_TABLE_SIZE];
	for( int i=0; i<GR_FOG_TABLE_SIZE; i++ )
	{
		float W = guFogTableIndexToW(i);
		fog[i]  = Clamp( 0.1f * W, 0.f, 255.f );
	}
	grFogTable(fog);
	grFogColorValue(0); 
	grFogMode(GR_FOG_DISABLE);

	// Init depth buffering.
	grDepthBufferFunction( GR_CMP_LEQUAL ); // GR_CMP_ALWAYS

	for( tmu=0; tmu<NumTmu; tmu++ )
	{
		// Set the default mipmap LOD bias value to the approriate value for 
		// bilinear filtering. 3dfx recommends bilinear=0.5, triliear=0.0.
		// I use a smaller number to improve clarity in exchange for a little bit of aliasing.
		grTexLodBiasValue( tmu, -1.0 );

		// Wrap the textures.
		grTexClampMode( tmu, GR_TEXTURECLAMP_WRAP, GR_TEXTURECLAMP_WRAP ); // GR_TEXTURECLAMP_CLAMP

		// Set up the hardware to render decal textures with no iterated lighting.
		guTexCombineFunction( tmu, GR_TEXTURECOMBINE_DECAL );

		// Enable mipmapping.
		grTexMipMapMode( tmu, GR_MIPMAP_NEAREST, FXFALSE );
		
		// Dithering between mipmaps.
		//grHints( GR_HINT_ALLOW_MIPMAP_DITHER, 1 );
		//grTexMipMapMode( tmu, GR_MIPMAP_NEAREST_DITHER, FXFALSE );

		// Enable bilinear filtering for both minification and magnification.
		grTexFilterMode
		(
			tmu, 
			GR_TEXTUREFILTER_BILINEAR, // min. GR_TEXTUREFILTER_POINT_SAMPLED
			GR_TEXTUREFILTER_BILINEAR  // mag.
		);

		// Init tmu state.
		States[tmu].Init( tmu, this );
	}

	// Init general info.
	Active         = 1;
	Locked         = 0;
	DepthBuffering = 1;

	// Success.
	return 1;
	unguard;
}

//
// Shut down the Glide device.
//
void FGlideRenderDevice::Exit3D()
{
	guard(FGlideRenderDevice::Exit3D);
	checkState(Active);
	checkState(!Locked);
	debugf("Shutting down Glide...");

	// Shut down each texture mapping unit.
	for( int i=0; i<NumTmu; i++ )
		States[i].Exit();

	// Note we're inactive.
	Active = 0;

	// Shut down Glide.
	grGlideShutdown();
	debugf(LOG_Exit,"Glide terminated");

	unguard;
};

//
// Flush all cached data.
//
void FGlideRenderDevice::Flush3D()
{
	guard(FGlideRenderDevice::Flush3D);
	checkState(Active);
	checkState(!Locked);

	for( int i=0; i<NumTmu; i++ )
		States[i].Flush();

	unguard;
}

/*-----------------------------------------------------------------------------
	FGlideRenderDevice Downloaders.
-----------------------------------------------------------------------------*/

//
// Download the texture and all of its mipmaps.
//
DWORD FGlideRenderDevice::FGlideTMU::DownloadTexture
(
	UTexture	*Texture,
	DWORD		GlideFlags,
	DWORD		CacheID,
	INT			iFirstMip,
	INT			iLastMip,
	GrTexInfo	*texinfo,
	INT			UCopies,
	INT			VCopies
)
{
	guard(FGlideRenderDevice::DownloadTexture);
	Stats.TextureDownloads++;

	// Compute size.
	DWORD Size = Glide->grTexCalcMemRequired( texinfo->smallLod, texinfo->largeLod, texinfo->aspectRatio, texinfo->format);

	// Create cache entry.
	DWORD Result = (DWORD)Cache.Create
	(
		CacheID, 
		TextureItem,
		Size,
		ALIGNMENT
	);

	// Create temporary memory.
	FMemMark Mark(GMem);
	INT MaxSize = (Texture->USize>>iFirstMip)*UCopies * (Texture->VSize>>iFirstMip)*VCopies;

	// Make buffer for copying the texture to fix its aspect ratio.
	BYTE *Copy = NULL;
	if( UCopies!=1 || VCopies!=1 )
		Copy = new( GMem, MaxSize )BYTE;

	// Make buffer for alpha conversion.
	FGlideColor *Alpha        = NULL;
	FGlideColor *AlphaPalette = NULL;
	if( GlideFlags & GF_Alpha )
	{
		AlphaPalette = new( GMem, Texture->Palette->Num )FGlideColor;
		Alpha        = new( GMem, MaxSize               )FGlideColor;
		for( int i=0; i<Texture->Palette->Num; i++ )
		{
			AlphaPalette[i].Color5551.R = Texture->Palette(i).R >> (8-5);
			AlphaPalette[i].Color5551.G = Texture->Palette(i).G >> (8-5);
			AlphaPalette[i].Color5551.B = Texture->Palette(i).B >> (8-5);
			AlphaPalette[i].Color5551.A = 1;
		}
		AlphaPalette[0].Color5551.A = 0;
	}
	else if( GlideFlags & GF_NoPalette )
	{
		AlphaPalette = new( GMem, Texture->Palette->Num )FGlideColor;
		Alpha        = new( GMem, MaxSize               )FGlideColor;
		for( int i=0; i<Texture->Palette->Num; i++ )
		{
			AlphaPalette[i].Color565.R = Texture->Palette(i).R >> (8-5);
			AlphaPalette[i].Color565.G = Texture->Palette(i).G >> (8-6);
			AlphaPalette[i].Color565.B = Texture->Palette(i).B >> (8-5);
		}
	}

	// Download the texture's mips.
	//debugf("%s: %i-%i",Texture->GetName(),iFirstMip,iLastMip);
	for( int i=iFirstMip; i<iLastMip; i++ )
	{
		FMipInfo &Mip = Texture->Mips[i];
		BYTE     *Src = &Texture->Element( Mip.Offset );
		if( Copy )
		{
			BYTE *To = Copy;
			for( int j=0; j<VCopies; j++ )
			{
				BYTE *From = Src;
				for( DWORD k=0; k<Mip.VSize; k++ )
				{
					for( int l=0; l<UCopies; l++ )
					{
						memcpy( To, From, Mip.USize );
						To += Mip.USize;
					}
					From += Mip.USize;
				}
			}
			Src = Copy;
		}
		if( Alpha )
		{
			int MipSize = Mip.Size() * UCopies * VCopies;
			for( int i=0; i<MipSize; i++ )
				Alpha[i] = AlphaPalette[Src[i]];
			Src = (BYTE*)Alpha;
		}
		Glide->grTexDownloadMipMapLevel
		(
			tmu,
			Result,
			texinfo->largeLod + i - iFirstMip,
			texinfo->largeLod,
			texinfo->aspectRatio,
			texinfo->format,
			GR_MIPMAPLEVELMASK_BOTH,
			Src
		);
	}
	Mark.Pop();
	return Result;
	unguard;
}

//
// Download the current light mesh.
//
void FGlideRenderDevice::FGlideTMU::DownloadLightMesh( DWORD Base, GrTexInfo *texinfo, INT UCopies, INT VCopies )
{
	guard(FGlideRenderDevice::DownloadLightMesh);

	FMemMark Mark(GDynMem);
	Stats.LightMeshDownloads++;

	RAINBOW_PTR Data;
	if( Glide->ColoredLight )
	{
		// Find max lighting value.
		BYTE        Max = 1;
		RAINBOW_PTR Src = GLightManager->Mesh;
		for( int i=0; i<GLightManager->MeshTileSpace; i++ )
		{
			if( Src.PtrBYTE[0] > Max ) Max = Src.PtrBYTE[0];
			if( Src.PtrBYTE[1] > Max ) Max = Src.PtrBYTE[1];
			if( Src.PtrBYTE[2] > Max ) Max = Src.PtrBYTE[2];
			Src.PtrDWORD++;
		}
		GLightManager->Index->InternalByte = Max;
		INT Scale[256];
		for( i=0; i<=Max; i++ )
			Scale[i] = (i * 255) / Max;

		// Convert 8-8-8 light maps to 5-6-5.
		Data.PtrWORD = new( GDynMem, GLightManager->MeshTileSpace * UCopies * VCopies )WORD;
		WORD  *Dest  = Data.PtrWORD;
		Src			 = GLightManager->Mesh;
		for( i=0; i<GLightManager->MeshVTile; i++ )
		{
			for( int j=0; j<GLightManager->MeshUTile; j++ )
			{
				*Dest++ = 
				+	((Scale[Src.PtrBYTE[2]] >> 3 ) & 0x001f) // Blue.
				+	((Scale[Src.PtrBYTE[1]] << 3 ) & 0x07e0) // Green.
				+	((Scale[Src.PtrBYTE[0]] << 8 ) & 0xf800) // Red.
				;
				Src.PtrDWORD++;
			}
			Dest += GLightManager->MeshUTile * (UCopies-1);
		}
	}
	else
	{
		// Find max lighting value.
		FLOAT Max  = 0.0;
		FLOAT *Src = GLightManager->Mesh.PtrFLOAT;
		for( int i=0; i<GLightManager->MeshTileSpace; i++ )
		{
			if( *Src > Max ) Max = *Src;
			Src++;
		}
		Max                                -= 3<<22;
		GLightManager->Index->InternalByte  = (Max * 255.0) / 0x4000;
		FLOAT Factor                        = 255.0 / Max;

		// Convert floating point light map to 8-bit intensity.
		Data.PtrBYTE = new(GMem,GLightManager->MeshTileSpace)BYTE;
		BYTE  *Dest  = Data.PtrBYTE;
		Src			 = GLightManager->Mesh.PtrFLOAT;
		for( i=0; i<GLightManager->MeshVTile; i++ )
		{
			for( int j=0; j<GLightManager->MeshUTile; j++ )
			{
				*Dest++ = Min(255,ftoi((*Src++ - (3<<22)) * Factor));
			}
			Dest += GLightManager->MeshUTile * (UCopies-1);
		}
	}

	// Download it.
	guard(grTexDownloadMipMapLevel);
	Glide->grTexDownloadMipMapLevel
	(
		tmu,
		Base,
		texinfo->largeLod,
		texinfo->largeLod,
		texinfo->aspectRatio,
		texinfo->format,
		GR_MIPMAPLEVELMASK_BOTH,
		Data.PtrVOID
	);
	unguard;

	Mark.Pop();
	unguard;
}

//
// Download the palette and all of its mipmaps.
//
void FGlideRenderDevice::FGlideTMU::DownloadPalette
(
	UPalette	*InPalette,
	FColor		InPaletteMaxColor
)
{
	guard(FGlideRenderDevice::DownloadPalette);

	FLOAT ScaleR, ScaleG, ScaleB;
	Stats.PaletteDownloads++;

	// Update state.
	Palette           = InPalette;
	PaletteMaxColor   = InPaletteMaxColor;

	// Set up scaling.
	ScaleR = 255.0 / Max(PaletteMaxColor.R, (BYTE)1);
	ScaleG = 255.0 / Max(PaletteMaxColor.G, (BYTE)1);
	ScaleB = 255.0 / Max(PaletteMaxColor.B, (BYTE)1);

	// A 3dfx palette entry is in the form of 0x00rrggbb.
	Palette->Lock(LOCK_Read);
	struct {BYTE B,G,R,A;} GlidePal[256];
	for( int i=0; i<Palette->Num; i++ )
	{
		GlidePal[i].R = Palette->Element(i).R * ScaleR;
		GlidePal[i].G = Palette->Element(i).G * ScaleG;
		GlidePal[i].B = Palette->Element(i).B * ScaleB;
		GlidePal[i].A = 0;
	}

	// Send the palette.
	Glide->grTexDownloadTablePartial
	(
		tmu,
		GR_TEXTABLE_PALETTE,
		GlidePal,
		0,
		Palette->Num-1
	);
	Palette->Unlock(LOCK_Read);

	unguard;
}

/*-----------------------------------------------------------------------------
	FGlideRenderDevice Lock & Unlock.
-----------------------------------------------------------------------------*/

//
// Lock the Glide device.
//
void FGlideRenderDevice::Lock3D( UCamera *Camera )
{
	guard(FGlideRenderDevice::Lock3D);
	checkState(!Locked);

	// Update camera sizing.
	SavedCamera    = *Camera;
	Camera->Stride = 1024;
	Camera->PrecomputeRenderInfo( grSstScreenWidth(), grSstScreenHeight() );

	// Note that we support colored lighting.
	Camera->Caps |= CC_ColoredLight;
	ColoredLight = (Camera->Caps & CC_ColoredLight) ? 1 : 0;

	// Clear the Z-buffer.
	ClearZBuffer();

	// Init stats.
	Stats.Init();

	Locked = 1;
	unguard;
};

//
// Unlock the Glide rendering device.
//
void FGlideRenderDevice::Unlock3D( UCamera *Camera, int Blit )
{
	guard(FGlideRenderDevice::Unlock3D);
	checkState(Locked);

	// Tick each of the states.
	for( int i=0; i<NumTmu; i++ )
		States[i].Tick();

	// Blit it.
	if( Blit )
	{
		// Flip pages.
		guard(grBufferSwap);
		grBufferSwap( 1 );
		unguard;

		// Display stats.
		guard(Stats);
		Stats.Display(*GApp);
		unguard;

		// Throttle if we are rendering faster than the refresh rate.
		if( grBufferNumPending() > 2 )
		{
			QWORD Time = GApp->MicrosecondTime();
			while( GApp->MicrosecondTime()-Time<100000 && grBufferNumPending() > 2 );
		}
	}

	// Restore old camera.
	*Camera = SavedCamera;

	Locked = 0;
	unguard;
};

/*-----------------------------------------------------------------------------
	FGlideRenderDevice texture vector polygon drawer.
-----------------------------------------------------------------------------*/

//
// Draw a textured polygon using surface vectors.
//
void FGlideRenderDevice::DrawPolyV
(
	UCamera				*Camera,
	UTexture			*Texture,
	const FTransform	*Pts,
	INT					NumPts,
	const FVector		&Base,
	const FVector		&Normal,
	const FVector		&U,
	const FVector		&V,
	FLOAT				PanU,
	FLOAT				PanV,
	DWORD				PolyFlags
)
{
	guard(FGlideRenderDevice::DrawPolyV);
	Stats.Polys++;

	// Count things to draw.
	int DrawThings = 1;

	// Check lighting.
	BOOL  Lit        = 0;
	FLOAT LightScale = 0.0;
	if( GLightManager->Mesh.PtrVOID!=NULL && !(PolyFlags & PF_Unlit) /*&& !(PolyFlags & PF_NoOcclude)*/ )
	{
		Lit         = 1;
		LightScale  = 256.0 / (Min(256,Max(GLightManager->MeshUTile, GLightManager->MeshVTile)) * GLightManager->MeshSpacing);
		DrawThings++;
	}

	BOOL Phog = 0;

	// Check bump mapping.
	if( Texture->BumpMap )
		DrawThings++;

	if( PolyFlags & PF_FakeBackdrop )
		DrawThings++;

	// Set state.
	SetDepthBuffering( !(PolyFlags & PF_NoOcclude) );

	// Setup texture.
	States[GR_TMU0].SetTexture( Texture, Texture->Palette, ((PolyFlags & PF_Masked) ? GF_Alpha : 0) | ((PolyFlags & PF_Transparent) ? GF_NoScale : 0) );

	// Get texture scale.
	FLOAT Scale  = States[GR_TMU0].Scale / Min(256, Max(Texture->USize, Texture->VSize));
	FLOAT UStart = GLightManager->TextureUStart/65536.0;
	FLOAT VStart = GLightManager->TextureVStart/65536.0;

	// Alloc verts.
	FMemMark Mark(GMem);
	GrVertex *Verts = new(GMem,NumPts)GrVertex, *FarVerts=Verts, *NearVerts=NULL;
	INT      NumFarVerts=NumPts, NumNearVerts=0;

	// Near-clipping factor.
	const FLOAT NearZ = 200.0;
	BYTE IsNear[64];
	BYTE AllNear=1, AnyNear=0;

	// Set up verts.
	for( int i=0; i<NumPts; i++ )
	{
		const FTransform &Pt	= Pts		 [i];
		GrVertex		 &Vert	= Verts      [i];

		// Compute S,T.
		FLOAT S					= U | (*(FVector*)&Pt - Base);
		FLOAT T					= V | (*(FVector*)&Pt - Base);

		// Set up vertex.
		Vert.x	 				= Mask(Camera->FXB + Pt.ScreenX / 65536.0);
		Vert.y	 				= Mask(Camera->FYB + Pt.ScreenY);
		Vert.z					= Pt.Z;
		Vert.a					= 64;
		Vert.oow 				= 1.0/Vert.z;
		Vert.tmuvtx[0].sow		= Vert.oow * (S + PanU) * Scale;
		Vert.tmuvtx[0].tow		= Vert.oow * (T + PanV) * Scale;

		// Is it near?
		IsNear[i] = Pt.Z < NearZ;
		AllNear  &= IsNear[i];
		AnyNear  |= IsNear[i];

		if( Lit )
		{
			Vert.tmuvtx[1].sow	= Vert.oow * (S - UStart) * LightScale;
			Vert.tmuvtx[1].tow	= Vert.oow * (T - VStart) * LightScale;
		}
	}

	// Setup for first rendering.
	if( DrawThings > 1 )
	{
		guColorCombineFunction( GR_COLORCOMBINE_DECAL_TEXTURE );
		//grFogMode(GR_FOG_DISABLE);
	}

	if( (PolyFlags & PF_FakeBackdrop) || !Texture->DetailTexture )
		AnyNear = AllNear = 0;

	// Init precision adjustment.
	struct {BYTE B,G,R,A;} FinalColor = {FullLight,FullLight,FullLight,0};

	// Near clip to assist the detail texturing.
	if( AllNear )
	{
		NearVerts    = Verts;
		NumNearVerts = NumPts;
		NumFarVerts  = 0;
	}
	else if( AnyNear )
	{
		// Clip the points so that we get a new poly with just the near ones.
		NumNearVerts = 0;
		NumFarVerts  = 0;
		NearVerts    = new(GMem,NumPts+6)GrVertex;
		FarVerts     = new(GMem,NumPts+6)GrVertex;

		GrVertex *A   = &Verts[0       ];
		GrVertex *B   = &Verts[NumPts-1];
		for( int i=0,j=NumPts-1; i<NumPts; j=i++,B=A++ )
		{
			if( IsNear[j] ^ IsNear[i] )
			{
				GrVertex Vert;
				FLOAT G            = (A->z - NearZ) / (A->z - B->z);
				FLOAT F            = 1.0 - G;
				Vert.z             = F*A->z + G*B->z;
				Vert.oow           = 1.0 / Vert.z;
				Vert.x             = Mask(Camera->FXB + (F*Pts[i].ScreenX*A->z + G*Pts[j].ScreenX*B->z)*Vert.oow/65536.0);
				Vert.y             = Mask(Camera->FYB + (F*Pts[i].ScreenY*A->z + G*Pts[j].ScreenY*B->z)*Vert.oow);
				Vert.tmuvtx[0].sow = (F*A->tmuvtx[0].sow*A->z + G*B->tmuvtx[0].sow*B->z) * Vert.oow;
				Vert.tmuvtx[0].tow = (F*A->tmuvtx[0].tow*A->z + G*B->tmuvtx[0].tow*B->z) * Vert.oow;

				if( Lit )
				{
					Vert.tmuvtx[1].sow = (F*A->tmuvtx[1].sow*A->z + G*B->tmuvtx[1].sow*B->z) * Vert.oow;
					Vert.tmuvtx[1].tow = (F*A->tmuvtx[1].tow*A->z + G*B->tmuvtx[1].tow*B->z) * Vert.oow;
				}

				NearVerts [NumNearVerts++] = Vert;
				FarVerts  [NumFarVerts++ ] = Vert;
			}
			if( IsNear[i] )
			{
				GrVertex &Vert     = NearVerts[NumNearVerts++];
				Vert               = *A;
			}
			else
			{
				GrVertex &Vert     = FarVerts[NumFarVerts++];
				Vert               = *A;
			}
		}
	}

	// Update precision adjustment.
	FinalColor.R = (FinalColor.R * States[0].TextureMaxColor.R) >> 8;
	FinalColor.G = (FinalColor.G * States[0].TextureMaxColor.G) >> 8;
	FinalColor.B = (FinalColor.B * States[0].TextureMaxColor.B) >> 8;
	if( --DrawThings == 0 )
	{
		grConstantColorValue( *(GrColor_t*)&FinalColor );
		grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_LOCAL, GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, FXFALSE );
		//grFogMode(GR_FOG_WITH_TABLE);
	}

	// Draw normal texture.
	SetBlending( PolyFlags );
	if( Lit && (PolyFlags & PF_Masked) ) grAlphaTestFunction( GR_CMP_GREATER );
	if( NumFarVerts ) grDrawPlanarPolygonVertexList( NumFarVerts,  FarVerts );
	if( PolyFlags & PF_Transparent )
	{
		for( i=0; i<NumNearVerts; i++ )
			NearVerts[i].a = 255.f * NearVerts[i].z / NearZ;
		guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ALPHA);
	}
	if( NumNearVerts ) grDrawPlanarPolygonVertexList( NumNearVerts, NearVerts);
	if( Lit && (PolyFlags & PF_Masked) ) grAlphaTestFunction( GR_CMP_ALWAYS );

	// Handle depth buffering the appropriate areas of masked textures.
	if( PolyFlags & PF_Masked )
		grDepthBufferFunction( GR_CMP_EQUAL );

	// Draw detail texture.
	if( NumNearVerts )
	{
		// Draw the detail texture, scaled by the regular texture.
		States[GR_TMU0].SetTexture( Texture->DetailTexture, NULL, GF_NoPalette );
		grAlphaCombine( GR_COMBINE_FUNCTION_LOCAL_ALPHA, GR_COMBINE_FACTOR_ONE, GR_COMBINE_LOCAL_ITERATED, GR_COMBINE_OTHER_NONE, FXFALSE );
		guColorCombineFunction( GR_COLORCOMBINE_DIFF_SPEC_A );		
		grAlphaBlendFunction( GR_BLEND_ZERO, GR_BLEND_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );
		for( i=0; i<NumNearVerts; i++ )
		{
			FLOAT Alpha = Max( (256.f+50.f) - 50.f * (NearZ / NearVerts[i].z), 0.f );
			NearVerts[i].r = NearVerts[i].g = NearVerts[i].b = Alpha;
			NearVerts[i].a = 255.0 - Alpha;
			NearVerts[i].tmuvtx[0].sow *= 8.0;
			NearVerts[i].tmuvtx[0].tow *= 8.0;
		}
		grDrawPlanarPolygonVertexList( NumNearVerts, NearVerts );
		if( DrawThings > 0 )
			guColorCombineFunction( GR_COLORCOMBINE_DECAL_TEXTURE );
	}
	else if( PolyFlags & PF_FakeBackdrop )
	{
		// Update precision adjustment.
		FinalColor.R = (FinalColor.R * States[0].TextureMaxColor.R) >> 8;
		FinalColor.G = (FinalColor.G * States[0].TextureMaxColor.G) >> 8;
		FinalColor.B = (FinalColor.B * States[0].TextureMaxColor.B) >> 8;
		if( --DrawThings == 0 )
		{
			grConstantColorValue( *(GrColor_t*)&FinalColor );
			grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_LOCAL, GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, FXFALSE );
			//grFogMode(GR_FOG_WITH_TABLE);
		}
		else guColorCombineFunction( GR_COLORCOMBINE_DECAL_TEXTURE );

		// Draw backdrop.
		for( i=0; i<NumFarVerts; i++ )
		{
			FarVerts[i].tmuvtx[0].sow *= 0.5;
			FarVerts[i].tmuvtx[0].tow *= 0.5;
		}
		grAlphaBlendFunction( GR_BLEND_ZERO, GR_BLEND_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );
		grDrawPlanarPolygonVertexList( NumFarVerts, FarVerts );
	}

	// Modulation blend the light and bump map textures.
	if( DrawThings > 0 )
		grAlphaBlendFunction( GR_BLEND_ZERO, GR_BLEND_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );

	// Bump map.
	if( Texture->BumpMap )
	{
		// Set the bump map.
		static UPalette* BumpPalette = new("TempBumpPalette",FIND_Existing)UPalette;
		States[GR_TMU0].SetTexture( Texture->BumpMap, Texture->BumpMap->Palette, GF_NoScale );

		// Update precision adjustment.
		if( --DrawThings == 0 )
		{
			grConstantColorValue( *(GrColor_t*)&FinalColor );
			grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_LOCAL, GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, FXFALSE );
			//grFogMode(GR_FOG_WITH_TABLE);
		}
		else guColorCombineFunction( GR_COLORCOMBINE_DECAL_TEXTURE );

		// Draw bump map.
		if( NumNearVerts ) grDrawPlanarPolygonVertexList( NumNearVerts, NearVerts);
		if( NumFarVerts  ) grDrawPlanarPolygonVertexList( NumFarVerts,  FarVerts );

		// Setup for next rendering.
		if( DrawThings > 0 ) grAlphaBlendFunction( GR_BLEND_ZERO, GR_BLEND_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );
	}

	// Light map.
	if( Lit )
	{
		// Set the light mesh.
		States[GR_TMU0].SetLightMesh( GLightManager->iLightMesh, GLightManager->IsDynamic || (PolyFlags&PF_DynamicLight) );
		
		// Update precision adjustment.
		FinalColor.R = ((INT)FinalColor.R * GLightManager->Index->InternalByte) >> 8;
		FinalColor.G = ((INT)FinalColor.G * GLightManager->Index->InternalByte) >> 8;
		FinalColor.B = ((INT)FinalColor.B * GLightManager->Index->InternalByte) >> 8;
		if( --DrawThings == 0 )
		{
			grConstantColorValue( *(GrColor_t*)&FinalColor );
			grColorCombine( GR_COMBINE_FUNCTION_SCALE_OTHER, GR_COMBINE_FACTOR_LOCAL, GR_COMBINE_LOCAL_CONSTANT, GR_COMBINE_OTHER_TEXTURE, FXFALSE );
			//grFogMode(GR_FOG_WITH_TABLE);
		}

		// Draw light nap.
		if( NumNearVerts )
		{
			for( int i=0; i<NumNearVerts; i++ )
				NearVerts[i].tmuvtx[0] = NearVerts[i].tmuvtx[1];
			grDrawPlanarPolygonVertexList( NumNearVerts, NearVerts);
		}
		if( NumFarVerts )
		{
			for( i=0; i<NumFarVerts; i++ )
				FarVerts[i].tmuvtx[0] = FarVerts[i].tmuvtx[1];
			grDrawPlanarPolygonVertexList( NumFarVerts,  FarVerts );
		}

		// Setup for next rendering.
		if( DrawThings > 0 )
			grAlphaBlendFunction( GR_BLEND_ZERO, GR_BLEND_SRC_COLOR, GR_BLEND_ZERO, GR_BLEND_ZERO );
	}

	// Phog.
	if( Phog )
	{
		// Set the fog mesh.
		static UTexture *Backdrop = new("Backdrop",FIND_Existing)UTexture;
		States[GR_TMU0].SetTexture( Backdrop, NULL, GF_NoPalette );

		grAlphaBlendFunction( GR_BLEND_ONE, GR_BLEND_ONE, GR_BLEND_ZERO, GR_BLEND_ZERO );
		guColorCombineFunction( GR_COLORCOMBINE_DECAL_TEXTURE );
		//grFogMode(GR_FOG_DISABLE);

		// Draw phog mesh.
		if( NumNearVerts )
		{
			for( int i=0; i<NumNearVerts; i++ )
				NearVerts[i].tmuvtx[0] = NearVerts[i].tmuvtx[1];
			grDrawPlanarPolygonVertexList( NumNearVerts, NearVerts);
		}
		if( NumFarVerts )
		{
			for( i=0; i<NumFarVerts; i++ )
				FarVerts[i].tmuvtx[0] = FarVerts[i].tmuvtx[1];
			grDrawPlanarPolygonVertexList( NumFarVerts,  FarVerts );
		}
		DrawThings--;
	}

	if( PolyFlags & PF_Masked )
		grDepthBufferFunction( GR_CMP_LEQUAL );

	ResetBlending( PolyFlags );
	debugState(DrawThings==0);
	Mark.Pop();
	unguard;
}

/*-----------------------------------------------------------------------------
	FGlideRenderDevice texture coordinates polygon drawer.
-----------------------------------------------------------------------------*/

//
// Draw a polygon with texture coordinates.
//
void FGlideRenderDevice::DrawPolyC
(
	UCamera*				Camera,
	UTexture*				Texture,
	const FTransTexture*	Pts,
	int						NumPts,
	DWORD					PolyFlags
)
{
	guard(FGlideRenderDevice::DrawPolyC);
	checkState(Locked);

	// Setup texture.
	States[GR_TMU0].SetTexture( Texture, Texture->Palette, GF_NoScale | ((PolyFlags&PF_Masked)?GF_Alpha:0));

	// Get texture scale.
	FLOAT Scale = States[GR_TMU0].Scale / (65536.0 * Min(256, Max(Texture->USize, Texture->VSize)));

	// Alloc verts.
	FMemMark Mark(GMem);
	GrVertex *Verts = new(GMem,NumPts)GrVertex;

	// Set up verts.
	for( int i=0; i<NumPts; i++ )
	{
		const FTransTexture &Pt		= Pts[i];
		GrVertex			&Vert	= Verts[i];

		// Set up vertex.
		Vert.x	 					= Mask(Camera->FXB + Pt.ScreenX);
		Vert.y	 					= Mask(Camera->FYB + Pt.ScreenY);
		Vert.z						= Pt.Z;
		Vert.a						= 64;
		Vert.r						= Min(Pt.Color.R * (2.0f/256.f),255.f);
		Vert.g						= Min(Pt.Color.G * (2.0f/256.f),255.f);
		Vert.b						= Min(Pt.Color.B * (2.0f/256.f),255.f);
		Vert.oow 					= 1.0/Vert.z;
		Vert.tmuvtx[0].sow			= Vert.oow * Pt.U * Scale;
		Vert.tmuvtx[0].tow			= Vert.oow * Pt.V * Scale;
	}

	// Set state.
	SetDepthBuffering( !(PolyFlags & PF_NoOcclude) );

	// Update precision adjustment.
	struct {BYTE B,G,R,A;} FinalColor = {FullLight,FullLight,FullLight,0};
	FinalColor.R = (FinalColor.R * States[0].TextureMaxColor.R) >> 8;
	FinalColor.G = (FinalColor.G * States[0].TextureMaxColor.G) >> 8;
	FinalColor.B = (FinalColor.B * States[0].TextureMaxColor.B) >> 8;

	guColorCombineFunction(GR_COLORCOMBINE_TEXTURE_TIMES_ITRGB);

	// Draw it.
	SetBlending(PolyFlags);
	grDrawPlanarPolygonVertexList( NumPts, Verts );
	ResetBlending( PolyFlags );

	Mark.Pop();
	unguard;
}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/

//
// Execute a command.
//
int FGlideRenderDevice::Exec( const char *Cmd,FOutputDevice *Out )
{
	guard(FGlideRenderDevice::Exec);
	const char *Str = Cmd;
	
	if( GetCMD(&Str,"SET") )
	{
		int Got=0;
		FLOAT Gamma;
		if( GetFLOAT(Str,"GAMMA=",&Gamma) )
		{
			grGammaCorrectionValue( Gamma );
			Out->Logf("Gamma %f",Gamma);
			Got=1;
		}
		INT Dither;
		if( GetINT(Str,"DITHER=",&Dither) && Dither==0 || Dither==2 || Dither==4 )
		{
			// Set dithering.
			grDitherMode( Dither==0 ? GR_DITHER_DISABLE : Dither==2 ? GR_DITHER_2x2 : GR_DITHER_4x4 );
			Out->Logf("Dither %i",Dither);
			Got=1;
		}
		if( GetINT(Str,"FULLLIGHT=",&FullLight) )
		{
			Out->Logf("Full light %i",FullLight);
			Got=1;
		}
		return Got;
	}
	else if( GetCMD(&Str,"MIPDITHER") )
	{
		grHints( GR_HINT_ALLOW_MIPMAP_DITHER, 1 );
		grTexMipMapMode( GR_TMU0, GR_MIPMAP_NEAREST_DITHER, FXFALSE );
		Out->Logf("Dithered mipmaps");
		return 1;
	}
	else if( GetCMD(&Str,"NOMIPDITHER") )
	{
		grTexMipMapMode( GR_TMU0, GR_MIPMAP_NEAREST, FXFALSE );
		Out->Logf("Undithered mipmaps");
		return 1;
	}
	else
	{
		Out->Logf("Unrecognized RenDev command");
		return 1;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Finder.
-----------------------------------------------------------------------------*/

FGlideRenderDevice GGlideRenDev;

#if 1
//
// Return the Glide render device if it's available.
//
FRenderDevice *FindRenderDevice()
{
	// Fix up the environment variables.
	_putenv( "SST_RGAMMA=" );
	_putenv( "SST_GGAMMA=" );
	_putenv( "SST_BGAMMA=" );

	// Try dynalinking the DLL.
	int Found3dfx = 0;
	if( !GetParam(GApp->CmdLine,"NOGLIDE") && GGlideRenDev.Associate() )
	{
		// Make sure 3Dfx hardware is present.
		Found3dfx = GGlideRenDev.grSstQueryBoards(&GGlideRenDev.hwconfig);
	}

	// Found it?
	if( Found3dfx )
	{
		char GlideVer[80];
		GGlideRenDev.grGlideGetVersion(GlideVer);
		debugf( LOG_Init, "Found a 3dfx: %s", GlideVer );
		return &GGlideRenDev;
	}
	else
	{
		debugf( LOG_Init, "No 3dfx detected" );
		return NULL;
	}
}
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
