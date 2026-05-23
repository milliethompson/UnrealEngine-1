/*=============================================================================
	UnLight.cpp: Unreal global lighting subsystem implementation.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	Computes all point lighting information and builds surface light meshes 
	based on light actors and shadow maps.

Definitions:
	attenuation:
		The amount by which light diminishes as it travells from a point source
		outward through space.  Physically correct attenuation is propertional to
		1/(distance*distance), but for speed, Unreal uses a lookup table 
		approximation where all light ceases after a light's predefined radius.
	diffuse lighting:
		Viewpoint-invariant lighting on a surface that is the result of a light's
		brightness and a surface texture's diffuse lighting coefficient.
	dynamic light:
		A light that does not move, but has special effects.
	illumination map:
		A 2D array of floating point or MMX Red-Green-Blue-Unused values which 
		represent the illumination that a light applies to a surface. An illumination
		map is the result of combining a light's spatial effects, attenuation,
		incidence factors, and shadow map.
	incidence:
		The angle at which a ray of light hits a point on a surface. Resulting brightness
		is directly proportional to incidence.
    light:
		Any actor whose LightType member has a value other than LT_None.
	meshel:
		A mesh element; a single point in the rectangular NxM mesh containing lighting or
		shadowing values.
	moving light:
		A light that moves. Moving lights do not cast shadows.
	radiosity:
		The process of determining the surface lighting resulting from 
		propagation of light through an environment, accounting for interreflection
		as well as direct light propagation. Radiosity is a computationally
		expensive preprocessing step but generates physically correct lighting.
	raytracing:
		The process of tracing rays through a level between lights and map lattice points
		to precalculate shadow maps, which are later filtered to provide smoothing. 
		Raytracing generates cool looking though physically unrealistic lighting.
	resultant map:
		The final 2D array of floating point or MMX values which represent the total
		illumination resulting from all of the lights (and hence illumination maps) 
		which apply to a surface.
	shadow map:
		A 2D array of floating point values which represent the amount of shadow
		occlusion between a light and a map lattice point, from 0.0 (fully occluded)
		to 1.0 (fully visible).
	shadow hypervolume:
		The six-dimensional hypervolume of space which is not affected by a volume
		lightsource.
	shadow volume:
		The volume of space which is not affected by a point lightsource. The inverse of light
		volume.
	shadow z-buffer:
		A 2D z-buffer representing a perspective projection depth view of the world from a
		lightsource. Often used in dynamic shadowing computations.
	spatial lighting effect:
		A lighting effect that is a function of a location in space, usually relative to
		a light's location.
	specular lighting:
		Viewpoint-varient lighting on a shiny surface that is the result of a
		light's brightness and a surface texture's specular lighting
		coefficient.
	static illumination map:
		An illumination map that represents the total of all static light illumination
		maps that apply to a surface. Static illumination maps do not change in time
		and thus they can be cached.
	static light:
		A light that is constantly on, does not move, and has no special effects.
	surface map:
		Any 2D map that applies to a surface, such as a shadow map or illumination
		map.  Surface maps are always aligned to the surface's U and V texture
		coordinates and are bilinear filtered across the extent of the surface.
	volumetric lighting:
		Lighting that is visible as a result of light interacting with a volume in
		space due to an interacting media such as fog. Volumetric lighting is view
		variant and cannot be associated with a particular surface.

Design notes:
 *	Uses a multi-tiered system for generating the resultant map for a surface,
	where all known constant intermediate and resulting meshes that may be needed 
	in the future are cached, and all known variable intermediate and resulting
	meshes are allocated temporarily.
 *  All floating point light meshes are shadow maps are represented as floating point
    values from 0.0 to 1.0, all-inclusive.  This allows for fast mixing of components A 
	and B via (A+B-A*B), which produces a valid result given valid inputs. However, typical
	lighting values are small (<0.2) so that (A+B-A*B) is approximately linear; nonlinearity
	only occurs with unnaturally bright lights.

Notes:
	No specular lighting support.
	No radiosity.
	No dynamic shadows.
	No shadow hypervolumes.
	No shadow volumes.
	No shadow z-buffers.

Revision history:
    9-23-96, Tim: Rewritten from the ground up.
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"
#include "float.h"

#define SHADOW_SMOOTHING 1 /* Smooth shadows (should be 1) */

/*------------------------------------------------------------------------------------
	Approximate math implementation.
------------------------------------------------------------------------------------*/

FLOAT SqrtManTbl[2<<APPROX_MAN_BITS];
FLOAT DivSqrtManTbl[1<<APPROX_MAN_BITS],DivManTbl[1<<APPROX_MAN_BITS];
FLOAT DivSqrtExpTbl[1<<APPROX_EXP_BITS],DivExpTbl[1<<APPROX_EXP_BITS];
FLOAT AtanTbl[APPROX_ATANS];

/*------------------------------------------------------------------------------------
	Subsystem definition
------------------------------------------------------------------------------------*/

//
// Lighting manager definition.
//
class FLightManager : public FLightManagerBase
{
public:
	// FLightManagerBase functions.
	void Init();
	void Exit();
	void SetupForActor(UCamera *Camera, AActor *iActor);
	void SetupForSurf(UCamera *Camera, FVector &Normal, FVector &Base, INDEX iSurf, INDEX iLightMesh, DWORD PolyFlags, AZoneInfo *Zone, UTexture *Texture);
	void SetupForNothing(UCamera *Camera);
	void ApplyLatticeEffects(FTexLattice *Start, FTexLattice *End);
	void ReleaseLightBlock();
	void DoDynamicLighting(ULevel *Level);
	void UndoDynamicLighting(ULevel *Level);

	// Forward declarations.
	class FLightInfo;

	// Constants.
	enum {MAX_LIGHTS=16};
	enum {MAX_DYN_LIGHT_POLYS=2048};

	// Function pointer types.
	typedef void (*LIGHT_MERGE_FUNC)   ( BYTE Caps, int Key, FLightInfo &Light, RAINBOW_PTR Stream, RAINBOW_PTR Dest );
	typedef void (*LIGHT_SPATIAL_FUNC) ( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	typedef void (*LIGHT_LATTICE_FUNC) ( FLightInfo &Light, FTexLattice *Start, FTexLattice *End );
	typedef void (*LIGHT_TYPE_FUNC)    ( FLightInfo &Light, AActor *Actor );
	typedef DWORD FILTER_TAB[4];

	// Information about one special lighting effect.
	struct FLocalEffectEntry
	{
		LIGHT_MERGE_FUNC	MergeFxFunc;		// Function to perform merge lighting
		LIGHT_SPATIAL_FUNC	SpatialFxFunc;		// Function to perform spatial lighting
		LIGHT_LATTICE_FUNC	LatticeFxFunc;		// Function to perform lattice warping
		int					MinXBits,MaxXBits;	// Horizontal lattice size bounds, 0=n/a
		int					MinYBits,MaxYBits;	// Vertical lattice size bounds, 0=n/a
		int					IsSpatialDynamic;	// Indicates whether light spatiality changes over time
	};
	enum ELightKind
	{
		// Level of optimization that can be applied to a lightsource, ranging from 0 (the most)
		// to 3 (the least).
		ALO_NotLight 		= 0,	// Actor is not a lightsource
		ALO_BackdropLight	= 1,	// Actor is a backdrop light only
		ALO_StaticLight		= 2,	// Actor is a non-moving, non-changing lightsource
		ALO_DynamicLight	= 3,	// Actor is a non-moving, changing lightsource
		ALO_MovingLight		= 4,	// Actor is a moving, changing lightsource
	};

	// Information about a lightsource.
	class FLightInfo
	{
	public:
		// For all lights.
		AActor		*Actor;					// All actor drawing info.
		ELightKind	Opt;					// Light type.
		FVector		Location;				// Transformed screenspace location of light.
		FLOAT		Radius;					// Maximum effective radius.
		FLOAT		RRadius;				// 1.0 / Radius.
		FLOAT		RRadiusMult;			// 16383.0 / (Radius * Radius).
		FLOAT		Brightness;				// Center brightness at this instance, 1.0=max, 0.0=none.
		FLOAT		Diffuse;				// BaseNormalDelta * RRadius.
		BYTE*		IlluminationMap;		// Temporary illumination map pointer.
		INT			IsVolumetric;			// Whether it's volumetric.

		// For volumetric lights.
		FLOAT		VolRadius;				// Volumetric radius.
		FLOAT		VolRadiusSquared;		// VolRadius*VolRadius.
		FLOAT		LocationSizeSquared;	// Location.SizeSqurated().
		FLOAT		VolBrightness;			// Volumetric lighting brightness.

		// Information about the lighting effect.
		FLocalEffectEntry	Effect;

		// Coloring.
		FLOAT		FloatScale;				// Floating point mono light scaling.
		FVector		FloatColor;				// Floating point color values.
		FColor		ByteColor;				// Byte color values.

		// Functions.
		void inline SetActor			(AActor *Actor);
		void inline ComputeFromActor	(UCamera *Camera);
	};

	// Global light effects.
	static void global_None				( FLightInfo &Light, AActor *Actor );
	static void global_Steady			( FLightInfo &Light, AActor *Actor );
	static void global_Pulse			( FLightInfo &Light, AActor *Actor );
	static void global_Blink			( FLightInfo &Light, AActor *Actor );
	static void global_Flicker			( FLightInfo &Light, AActor *Actor );
	static void global_Strobe			( FLightInfo &Light, AActor *Actor );
	static void global_BackdropLight	( FLightInfo &Light, AActor *Actor );
	static void global_Test				( FLightInfo &Light, AActor *Actor );

	// Simple lighting functions.
	static void merge_None				( BYTE Caps, INT Key, FLightInfo &Light, RAINBOW_PTR Stream, RAINBOW_PTR Dest );
	static void merge_TorchWaver		( BYTE Caps, INT Key, FLightInfo &Light, RAINBOW_PTR Stream, RAINBOW_PTR Dest );
	static void merge_FireWaver			( BYTE Caps, INT Key, FLightInfo &Light, RAINBOW_PTR Stream, RAINBOW_PTR Dest );
	static void merge_WaterShimmer		( BYTE Caps, INT Key, FLightInfo &Light, RAINBOW_PTR Stream, RAINBOW_PTR Dest );
	static void merge_Test				( BYTE Caps, INT Key, FLightInfo &Light, RAINBOW_PTR Stream, RAINBOW_PTR Dest );

	// Spatial lighting functions.
	static void spatial_None			( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_SearchLight		( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_SlowWave		( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_FastWave		( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_CloudCast		( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_Shock			( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_Disco			( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_Interference	( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_Cylinder		( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_Rotor			( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_Spotlight		( FLightInfo *Info, BYTE *Src, BYTE *Dest );
	static void spatial_Test			( FLightInfo *Info, BYTE *Src, BYTE *Dest );

	// Lattice lighting functions.
	static void lattice_WarpU			( FLightInfo &Light, FTexLattice *Start, FTexLattice *End );
	static void lattice_WarpV			( FLightInfo &Light, FTexLattice *Start, FTexLattice *End );
	static void lattice_CalmWater		( FLightInfo &Light, FTexLattice *Start, FTexLattice *End );
	static void lattice_ChurningWater	( FLightInfo &Light, FTexLattice *Start, FTexLattice *End );
	static void lattice_Satellite		( FLightInfo &Light, FTexLattice *Start, FTexLattice *End );
	static void lattice_Test			( FLightInfo &Light, FTexLattice *Start, FTexLattice *End );

	// Special lattice lighting functions.
	static void lattice_Volumetric		( FLightInfo &Light, FTexLattice *Start, FTexLattice *End );

	// Functions.
	inline static void BuildTemporaryTables();
	void ShadowMapGen_B( BYTE *SrcBits, BYTE *Dest1 );
	void StaticLightingMapGen( BYTE Caps, RAINBOW_PTR Result );

	// Public lighting variables.
	static FLightInfo*		LastLight;
	static FLightInfo*const FinalLight;
	static FLightInfo		FirstLight[MAX_LIGHTS];
	static ALevelInfo    	*LevelInfo;
	static AZoneInfo		*Zone;

	static INT				NumDynLightPolys,TemporaryTablesBuilt;
	static FLOAT			BackdropBrightness;
	static FLOAT            LightSqrt[4096];

	static UTexture::Ptr	Texture;
	static UPalette::Ptr	BumpPalette;
	static UPalette::Ptr	TempColorPalette;

	// Tables.
	static INDEX			DynLightPolys[MAX_DYN_LIGHT_POLYS];
	static FILTER_TAB		FilterTab[128];

	// Arrays.
	static const FLocalEffectEntry	Effects[LE_MAX];
	static const LIGHT_TYPE_FUNC	GLightTypeFuncs[LT_MAX];
	static BYTE ByteClamp[512];
	static BYTE ByteMul  [256][256];

	// Memory cache info.
	enum {MAX_UNLOCKED_ITEMS=128};
	static FCacheItem *ItemsToUnlock[MAX_UNLOCKED_ITEMS];
	static FCacheItem **TopItemToUnlock;
	void InitCacheItems()
	{
		TopItemToUnlock = &ItemsToUnlock[0];
	}
	void UnlockCacheItems()
	{
		while( TopItemToUnlock > &ItemsToUnlock[0])
			(*--TopItemToUnlock)->Unlock();
	}
};

//
// FLightManager statics.
//
FLightManager::FLightInfo*		FLightManager::LastLight;
FLightManager::FLightInfo*const	FLightManager::FinalLight = &FirstLight[MAX_LIGHTS];
FLightManager::FLightInfo		FLightManager::FirstLight[MAX_LIGHTS];
FLightManager::FILTER_TAB		FLightManager::FilterTab[128];
ALevelInfo*						FLightManager::LevelInfo;
AZoneInfo*						FLightManager::Zone;
BYTE							FLightManager::ByteClamp[512];
BYTE							FLightManager::ByteMul  [256][256];

UTexture::Ptr					FLightManager::Texture;
UPalette::Ptr					FLightManager::BumpPalette;
UPalette::Ptr					FLightManager::TempColorPalette;

INT								FLightManager::MeshUSkip;
INT								FLightManager::NumDynLightPolys;
INT								FLightManager::TemporaryTablesBuilt;
FLOAT							FLightManager::BackdropBrightness;
FLOAT							FLightManager::LightSqrt[4096];

INDEX							FLightManager::DynLightPolys[MAX_DYN_LIGHT_POLYS];

FCacheItem*						FLightManager::ItemsToUnlock[MAX_UNLOCKED_ITEMS];
FCacheItem**					FLightManager::TopItemToUnlock;

const FLightManager::FLocalEffectEntry FLightManager::Effects[LE_MAX] =
{
// LE_ tag			Simple func			    Spatial func			Lattice func		MinXBit	MaxXBit	MinYBit	MaxYBit	SpacDyn
// ----------------	---------------------	-----------------------	-------------------	------- ------- ------- -------	-------
{/* None         */	merge_None,				spatial_None,			NULL,				0,		0,		0,		0,		0		},
{/* TorchWaver   */	merge_TorchWaver,		spatial_None,			NULL,				0,		0,		0,		0,		0		},
{/* FireWaver    */	merge_FireWaver,		spatial_None,			NULL,				0,		0,		0,		0,		0		},
{/* WateryShimmer*/	merge_WaterShimmer,		spatial_None,			NULL,				0,		0,		0,		0,		0		},
{/* Searchlight  */	merge_None,				spatial_SearchLight,	NULL,				0,		0,		0,		0,		1		},
{/* SlowWave     */	merge_None,				spatial_SlowWave,		NULL,				0,		0,		0,		0,		1		},
{/* FastWave     */	merge_None,				spatial_FastWave,		NULL,				0,		0,		0,		0,		1		},
{/* CloudCast    */	merge_None,				spatial_CloudCast,		NULL,				0,		0,		0,		0,		1		},
{/* StaticSpot   */	merge_None,				spatial_Spotlight,		NULL,				0,		0,		0,		0,		0		},
{/* Shock        */	merge_None,				spatial_Shock,			NULL,				0,		0,		0,		0,		1		},
{/* Disco        */	merge_None,				spatial_Disco,			NULL,				0,		0,		0,		0,		1		},
{/* Warp         */	merge_None,				spatial_None,			lattice_WarpU,		0,		0,		0,		0,		0		},
{/* Spotlight    */	merge_None,				spatial_Spotlight,		NULL,				0,		0,		0,		0,		0		},
{/* StolenQuakeWa*/	merge_None,				spatial_None,			lattice_CalmWater,	0,		0,		0,		0,		0		},
{/* ChurningWater*/	merge_None,				spatial_None,			lattice_ChurningWater,0,	0,		0,		0,		0		},
{/* Satellite    */	merge_None,				spatial_None,			lattice_Satellite,	0,		0,		0,		0,		1		},
{/* Interference */	merge_None,				spatial_Interference,	NULL,				0,		0,		0,		0,		1		},
{/* Cylinder     */	merge_None,				spatial_Cylinder,		NULL,				0,		0,		0,		0,		0		},
{/* Rotor        */	merge_None,				spatial_Rotor,			NULL,				0,		0,		0,		0,		1		},
{/* Unused		 */	merge_None,				spatial_None,			NULL,				0,		0,		0,		0,		0		},
};

const FLightManager::LIGHT_TYPE_FUNC FLightManager::GLightTypeFuncs[LT_MAX] =
{
	global_None,
	global_Steady,
	global_Pulse,
	global_Blink,
	global_Flicker,
	global_Strobe,
	global_BackdropLight,
};

//
// FLightManagerBase statics.
//
FVector		FLightManagerBase::WorldBase;
FVector		FLightManagerBase::WorldNormal;
FVector		FLightManagerBase::Base;
FVector		FLightManagerBase::Normal;
FVector		FLightManagerBase::TextureU;
FVector		FLightManagerBase::TextureV;
FVector		FLightManagerBase::InverseUAxis;
FVector		FLightManagerBase::InverseVAxis;
FVector		FLightManagerBase::InverseNAxis;
UCamera*	FLightManagerBase::Camera;
ULevel*		FLightManagerBase::Level;
FBspSurf*	FLightManagerBase::Surf;
BYTE*		FLightManagerBase::ShadowBase;
FMemMark	FLightManagerBase::Mark;
QWORD		FLightManagerBase::MeshAndMask;
DWORD		FLightManagerBase::PolyFlags;
DWORD		FLightManagerBase::MaxSize;
INT			FLightManagerBase::MeshUSize;
INT			FLightManagerBase::MeshVSize;
INT			FLightManagerBase::MeshSpace;
INT			FLightManagerBase::MeshUByteSize;
INT			FLightManagerBase::MeshByteSpace;
INT			FLightManagerBase::LatticeEffects;
INT			FLightManagerBase::MinXBits;
INT			FLightManagerBase::MaxXBits;
INT			FLightManagerBase::MinYBits;
INT			FLightManagerBase::MaxYBits;
INT			FLightManagerBase::NumLights;
FLOAT		FLightManagerBase::FMeshSpacing;
FLOAT		FLightManagerBase::FTextureUStart;
FLOAT		FLightManagerBase::FTextureVStart;

/*------------------------------------------------------------------------------------
	Init & Exit
------------------------------------------------------------------------------------*/

//
// Set up the tables required for fast square root computation.
//
void SetupTable(FLOAT *ManTbl,FLOAT *ExpTbl,FLOAT Power)
{
	union {FLOAT F; DWORD D;} Temp;

	Temp.F = 1.0;
	for( DWORD i=0; i<(1<<APPROX_EXP_BITS); i++ )
	{
		Temp.D = (Temp.D & 0x007fffff ) + (i << (32-APPROX_EXP_BITS));
		ExpTbl[ i ]=exp(Power*log(Abs(Temp.F)));
		if(_isnan(ExpTbl[ i ])) ExpTbl[ i ]=0.0;
		//debugf("exp [%f] %i = %f",Power,i,ExpTbl[i]);
	}

	Temp.F = 1.0;
	for( i=0; i<(1<<APPROX_MAN_BITS); i++ )
	{
		Temp.D = (Temp.D & 0xff800000 ) + (i << (32-APPROX_EXP_BITS-APPROX_MAN_BITS));
		ManTbl[ i ]=exp(Power*log(Abs(Temp.F)));
		if(_isnan(ManTbl[ i ])) ManTbl[ i ]=0.0;
		//debugf("man [%f] %i = %f",i,Power,ManTbl[i]);
	}
}

//
// Initialize the global lighting subsystem.
//
void FLightManager::Init()
{
	guard(FLightManager::Init);

	// Globals.
	NumDynLightPolys = 0;

	// Multiplier.
	for( int i=0; i<256; i++ )
		for( int j=0; j<256; j++ )
			ByteMul[i][j] = (i*j)/255;

	// Clamper.
	for( i=0; i<ARRAY_COUNT(ByteClamp); i++ )
		ByteClamp[i] = Clamp( i, 0, 255 );

	// Filtering table.
	int FilterWeight[8][8] = 
	{
		{ 0,24,40,24,0,0,0,0},
		{ 0,40,64,40,0,0,0,0},
		{ 0,24,40,24,0,0,0,0},
		{ 0, 0, 0, 0,0,0,0,0},
		{ 0, 0, 0, 0,0,0,0,0},
		{ 0, 0, 0, 0,0,0,0,0},
		{ 0, 0, 0, 0,0,0,0,0},
		{ 0, 0, 0, 0,0,0,0,0}
	};

	// Setup square root tables.
	for( DWORD D=0; D< (1<< APPROX_MAN_BITS ); D++ )
	{
		union {FLOAT F; DWORD D;} Temp;
		Temp.F = 1.0;
		Temp.D = (Temp.D & 0xff800000 ) + (D << (23 - APPROX_MAN_BITS));
		Temp.F = sqrt(Temp.F);
		Temp.D = (Temp.D - ( 64 << 23 ) );   // exponent bias re-adjust
		SqrtManTbl[ D ] = (FLOAT) (Temp.F * sqrt(2.0)); // for odd exponents
		SqrtManTbl[ D + (1 << APPROX_MAN_BITS) ] =  (FLOAT) (Temp.F * 2.0);
	}
	SetupTable(DivSqrtManTbl,DivSqrtExpTbl,-0.5);
	SetupTable(DivManTbl,    DivExpTbl,    -1.0);
	
	// Setup arctangent table.
	for( i=0; i<APPROX_ATANS; i++ )
	{
		FLOAT G = (FLOAT)i / ((FLOAT)APPROX_ATANS-(FLOAT)i);
		AtanTbl[APPROX_ATANS-1-i] = atan(G);
	}

	// Init square roots.
	for( i=0; i<ARRAY_COUNT(LightSqrt); i++ )
	{
		FLOAT S		 = sqrt((FLOAT)(i+1) * (1.0/ARRAY_COUNT(LightSqrt)));

		// This function gives a more luminous, specular look to the lighting.
		FLOAT Temp	 = (2*S*S*S-3*S*S+1); // Or 1.0-S.

		// This function makes surfaces look more matte.
		//FLOAT Temp = (1.0-S);
		LightSqrt[i] = Temp/S;
	}

	// Generate filter lookup table
	int FilterSum=0;
	for( i=0; i<8; i++ )
		for( int j=0; j<8; j++ )
			FilterSum += FilterWeight[i][j];

	// Iterate through all filter table indices 0x00-0x3f.
	for( i=0; i<128; i++ )
	{
		// Iterate through all vertical filter weights 0-3.
		for( int j=0; j<4; j++ )
		{
			// Handle all four packed values.
			FilterTab[i][j] = 0;
			for (int Pack=0; Pack<4; Pack++ )
			{
				// Accumulate filter weights in FilterTab[i][j] according to which Bits are set in i.
				INT Acc = 0;
				for( int Bit=0; Bit<8; Bit++ )
					if( i & (1<<(Pack + Bit)) )
						Acc += FilterWeight[j][Bit];

				// Add to sum.
				DWORD Result = (Acc * 255) / FilterSum;
				checkState(Result>=0 && Result<=255);
				FilterTab[i][j] += (Result << (Pack*8));
			}
		}
	}

	// Cache items.
	InitCacheItems();

	// Create temporary bump map palette.
	TempColorPalette = new("TempBumpPalette",CREATE_Unique)UPalette(256,1);
	GObj.AddToRoot(TempColorPalette);

	// Success.
	debugf(LOG_Init,"Lighting subsystem initialized");
	unguard;
}

//
// Shut down the global lighting system.
//
void FLightManager::Exit()
{
	guard(FLightManager::Exit);

	GObj.RemoveFromRoot(TempColorPalette);
	debugf(LOG_Exit,"Lighting subsystem shut down");

	unguard;
}

/*------------------------------------------------------------------------------------
	Table management
------------------------------------------------------------------------------------*/

//
// Function to make sure that the temporary tables for this light mesh have been built.
//
void FLightManager::BuildTemporaryTables()
{
	guardSlow(FLightManager::BuildTemporaryTables);
	STAT(clock(GStat.InversionTime));
	TemporaryTablesBuilt=1;
	
	// Build inverse vectors. Though this may seem slow, it's actually only a teeny drain on CPU time.
	FCoords Temp = FCoords( FVector(0,0,0), TextureU, TextureV, WorldNormal ).Inverse();
	InverseUAxis = Temp.XAxis * 65536.0;
	InverseVAxis = Temp.YAxis * 65536.0;
	InverseNAxis = Temp.ZAxis;

	STAT(unclock(GStat.InversionTime));
	unguardSlow;
}

/*------------------------------------------------------------------------------------
	Intermediate map generation code
------------------------------------------------------------------------------------*/

//
// Generate the shadow map for one lightsource that applies to a Bsp surface.
//
// Input: 1-bit-deep occlusion bitmask.
//
// Output: Floating point representation of the fraction of occlusion
// between each point and the lightsource, and range from 0.0 (fully occluded) to
// 1.0 (fully unoccluded).  These values refer only to occlusion and say nothing
// about resultant lighting (brightness, incidence, etc) which IlluminationMapGen handles.
//
void FLightManager::ShadowMapGen_B( BYTE *SrcBits, BYTE *Dest1 )
{
	guardSlow(FLightManager::ShadowMapGen_B);
#if SHADOW_SMOOTHING
	debugInput(((int)Dest1 & 3)==0);

	// Generate smooth shadow map by convolving the shadow bitmask with a smoothing filter.
	// optimize: Convert to assembly. This is called a lot.
	INT Size4 = MeshUSize/4;
	memset( Dest1, 0, MeshSpace );
	DWORD *Dests[3] = { (DWORD*)Dest1, (DWORD*)Dest1, (DWORD*)Dest1 + Size4 };
	for( int V=0; V<MeshVSize; V++ )
	{
		// Offset of shadow map relative to convolution filter left edge.
		#define PHUDGE 2

		// Get initial bits, with low bit shifted in.
		BYTE *Src = SrcBits;
		DWORD D   = (DWORD)*Src++ << (8+PHUDGE);
		if( D & 0x200 ) D |= 0x0100;

		// Filter everything.
		for( int U=0; U<MeshUByteSize; U++ )
		{
			D = (D >> 8) | (((DWORD)*Src++) << (8+PHUDGE));

			FILTER_TAB &Tab1 = FilterTab[D & 0x7f];
			*Dests[0]++     += Tab1[0];
			*Dests[1]++     += Tab1[1];
			*Dests[2]++     += Tab1[2];

			FILTER_TAB &Tab2 = FilterTab[(D>>4) & 0x7f];
			*Dests[0]++     += Tab2[0];
			*Dests[1]++     += Tab2[1];
			*Dests[2]++     += Tab2[2];
		}
		SrcBits += MeshUByteSize;
		if( V == 0           ) Dests[0] -= Size4;
		if( V == MeshVSize-2 ) Dests[2] -= Size4;
	}
#else
	BYTE *Dest = Dest1;
	for( int V=0; V<MeshVSize; V++ )
	{
		// Generate abrupt shadow map, for testing/debugging only
		for( int U=0; U<MeshUSize; U++ )
		{
			int Base = U>>3;
			int Ofs  = U&7;
			*Dest++ = (SrcBits[V*MeshUByteSize + Base] & (1<<Ofs)) ? 255 : 0;
		}
	}
#endif
	if( PolyFlags & PF_DirtyShadows )
	{
		// Apply noise to shadow map
		BYTE *Dest = Dest1;
		BYTE *End = &Dest[MeshSpace];

#if 0 /* Use fractal noise, slow */
		FMemMark Mark(GMem);
		FLOAT *Fractal = new(GMem,MeshSpace)FLOAT;
		void MakeUntiledFractal(FLOAT *Dest, int US, int VS, int BasisCell, FLOAT Min, FLOAT Max);
		MakeUntiledFractal(Fractal,MeshUSize,MeshVSize,32,0.70,1.0);

		FLOAT *Src = Fractal;
		do *Dest = ftoi(*Dest * *Src++);
		while( ++Dest < End );

		Mark.Pop();

#else /* Use random noise */
		static int DirtAccumulator;
		do *Dest = ftoi(*Dest * (0.80 + 0.20 * GRandoms->RandomBase(DirtAccumulator++)));
		while( ++Dest < End );
#endif
	}
	unguardSlow;
}

//
// Build or retrieve a cached light mesh corresponding to all of the static lightsources that affect
// a surface.  This light mesh is time-invariant.
//
void FLightManager::StaticLightingMapGen( BYTE Caps, RAINBOW_PTR Result )
{
	guard(FLightManager::StaticLightingMapGen);
	FMemMark Mark(GMem);

	// Go through all lights and generate temporary shadow maps and illumination maps for the static ones:
	RAINBOW_PTR Stream = NULL;
	BYTE *ShadowLoc = ShadowBase;
	for( FLightInfo *Info = FirstLight; Info < LastLight; Info++ )
	{
		if( Info->Opt == ALO_StaticLight )
		{
			// Set up:
			Info->ComputeFromActor( Camera );

			// Build the light's temporary shadow map from the raytraced shadow bits.
			Info->IlluminationMap = new( GMem, MeshSpace )BYTE;
			ShadowMapGen_B( ShadowLoc, Info->IlluminationMap );

			// Build the illumination map, overwriting the shadow map.
			Info->Effect.SpatialFxFunc( Info, Info->IlluminationMap, Info->IlluminationMap );

			// Merge it in.
			merge_None( Caps, 0, *Info, Stream, Result );
			Stream = Result;

			// Release that temporary shadow map.
			Mark.Pop();
		}
		ShadowLoc += MeshByteSpace;
	}
	debugState(Stream.PtrVOID!=NULL);
	unguard;
}

/*------------------------------------------------------------------------------------
	Light blocks
------------------------------------------------------------------------------------*/

//
// Release any temporary memory that was allocated by calls to SetupForSurf().  Temporary
// memory will typically have been allocated for the final light map when there are dynamic
// components to the lighting.
//
void FLightManager::ReleaseLightBlock()
{
	guard(FLightManager::ReleaseLightBlock);

	// Release working memory.
	Mark.Pop();

	// Unlock any locked cache items.
	UnlockCacheItems();

	// Fix up after bump mapping.
	if( BumpPalette )
		Texture->Palette = BumpPalette;

	// Update stats.
	STAT(GStat.Lightage += MeshUSize * MeshVSize);
	STAT(GStat.LightMem += MeshUSize * MeshVSize * sizeof(FLOAT));

	unguard;
}

/*------------------------------------------------------------------------------------
	Applying lattice effects
------------------------------------------------------------------------------------*/

//
// Apply all lattice special effects to a span of texture lattices. Called after
// lattice screenspace and texture coordinates are generated and before texture 
// rectangles are generated.
//
void FLightManager::ApplyLatticeEffects( FTexLattice *Start, FTexLattice *End )
{
	guard(FLightManager::ApplyLatticeEffects);
	STAT(clock(GStat.IllumTime));
	FTexLattice *T;

	// Init FloatG.
	if( !Zone || !Zone->bFogZone )
	{
		// No fog, init brightnesses.
		for( T=Start; T<End; T++ )
			T->FloatG = UNLIT_LIGHT_VALUE;
	}
	else
	{
		// Fog setup.
		for( T=Start; T<End; T++ )
		{
			T->PointSize	= SqrtApprox(T->Loc.SizeSquared()); // PointRSize >= 0.0
			T->LocNormal	= T->Loc / T->PointSize;
			T->FloatG		= 0.0;
		}
	}

	// Small wavy.
	if( PolyFlags & PF_SmallWavy )
	{
		for( T=Start; T<End; T++ )
		{
			FVector Temp = T->Loc.TransformVectorBy(Camera->Uncoords) + Camera->Coords.Origin;
			FLOAT   A    = Temp.X * 0.045 + LevelInfo->TimeSeconds * 3.63;
			FLOAT   B    = Temp.Y * 0.045 + LevelInfo->TimeSeconds * 3.63;
			T->U        += 65536.0 * 2.0 * (GMath.CosFloat(A)+GMath.SinFloat(B));
			T->V        += 65536.0 * 2.0 * (GMath.SinFloat(A)-GMath.SinFloat(B));
		}
	}

	// Big wavy.
	if( PolyFlags & PF_BigWavy )
	{
		for( T=Start; T<End; T++ )
		{
			FVector Temp = T->Loc.TransformVectorBy(Camera->Uncoords) + Camera->Coords.Origin;
			T->U        += 65536.0 * 16.0 * GMath.CosFloat(Temp.X * 0.01 + LevelInfo->TimeSeconds * 3.5);
			T->V        += 65536.0 * 16.0 * GMath.SinFloat(Temp.Y * 0.01 + LevelInfo->TimeSeconds * 3.5);
		}
	}

	// Cloud wavy.
	if( PolyFlags & PF_CloudWavy )
	{
		static int Key = 0;
		for( T=Start; T<End; T++ )
		{
			T->FloatG = 0.0;
			FVector Temp = T->Loc.TransformVectorBy(Camera->Uncoords);
			if( LevelInfo->bMirrorSky || (T->Loc.Z > 0))
			{
				FLOAT Ofs = PI * LevelInfo->TimeDays * LevelInfo->SkyWavySpeed * 24.0 * 60.0 * 60.0;
				FLOAT A   = Temp.X * 0.045 + Ofs;
				FLOAT B   = Temp.Y * 0.045 + Ofs;

				T->U += 65536.0 * LevelInfo->SkyWavyness * (GMath.CosFloat(A)+GMath.SinFloat(B));
				T->V += 65536.0 * LevelInfo->SkyWavyness * (GMath.SinFloat(A)-GMath.CosFloat(B));

				FLOAT Dist = SqrtApprox(T->Loc.SizeSquared());
				T->FloatG  = 0.5 * Clamp
				(
					BackdropBrightness *
					Clamp(200.0/Dist,0.0,1.0)-GRandoms->RandomBase(Key++)*LevelInfo->SkyFlicker,0.0,1.0
				);
			}
		}
	}

	// Water wavy.
	if( PolyFlags & PF_WaterWavy )
	{
		for( T=Start; T<End; T++ )
		{
			FVector Temp = T->Loc.TransformVectorBy(Camera->Uncoords);
			T->U += 65536.0 * 3.0 * GMath.CosFloat(Temp.X * 0.065 + LevelInfo->TimeSeconds * 2.5);
			T->V += 65536.0 * 3.0 * GMath.SinFloat(Temp.Y * 0.065 + LevelInfo->TimeSeconds * 3.1);
		}
	}

	// Apply lattice lighting effect for each individual lightsource.
	for( FLightInfo *Info=FirstLight; Info<LastLight; Info++ )
	{
		if( Info->Effect.LatticeFxFunc )
			Info->Effect.LatticeFxFunc( *Info, Start, End );

		if( Info->IsVolumetric )
			lattice_Volumetric( *Info, Start, End );
	}

	// Generate final brightness
	for( T=Start; T<End; T++ )
		T->G = ftoi( ToLatticeLight( T->FloatG ) );

	STAT(unclock(GStat.IllumTime));
	unguard;
}

/*------------------------------------------------------------------------------------
	All local light effects functions
------------------------------------------------------------------------------------*/

/////////////////////////////
// Simple effect functions //
/////////////////////////////

void Test( BYTE *Src, RAINBOW_PTR Stream, RAINBOW_PTR Dest, DWORD InCount, FColor ByteColor )
{
	static DWORD SavedESP, SavedEBP, Count;
	static BYTE *MulR, *MulG, *MulB;
	Count = InCount;
	MulR  = &FLightManager::ByteMul[ByteColor.R][0];
	MulG  = &FLightManager::ByteMul[ByteColor.G][0];
	MulB  = &FLightManager::ByteMul[ByteColor.B][0];
	__asm
	{
		// esi = illumination map pointer.
		// ebp = stream to merge with.
		// edi = destination.

		// Init.
		mov [SavedESP],esp
		mov [SavedEBP],ebp

		mov esi,[Src]
		mov edi,[Dest]
		mov ebp,[Stream]
		sub ebp,edi

		// Pre.
		sub edi,4

		// Loop.
	InnerLoop:

		xor eax,eax        ; eax = 0
		add edi,4          ; edi = new dest

		mov al,[esi]       ; eax = illumination map value
		inc esi            ; esi = updated ilumination map pointer

		mov esp,[MulR]     ; esp = R multiplier base address
		mov edx,[MulG]     ; edx = G multiplier base address

		mov bl,[ebp+edi+0] ; bl  = R from stream
		mov cl,[ebp+edi+1] ; cl  = G from stream

		mov ch,[esp+eax]   ; ch  = scaled R
		mov esp,[MulB]     ; esp = B multiplier base address

		mov bh,[edx+eax]   ; bh  = scaled G
		mov dl,[ebp+edi+2] ; dl  = B from stream

		add bl,ch          ; bl  = resultant R
		jc  CarryR         ; overflow
	NoCarryR:

		mov [edi+0],bl     ; store R
		mov bl,[esp+eax]   ; bl  = scaled B

		add bh,cl          ; bh = resultant G
		jc  CarryG         ; overflow
	NoCarryG:

		add bl,dl          ; bl = resultant B
		jc  CarryB         ; overflow
	NoCarryB:

		mov [edi+1],bh    ; store G
		mov esp,[Count]   ; esp = count

		mov [edi+2],bl    ; store B
		dec esp           ; dec count

		mov [Count],esp   ; store count
		jg  InnerLoop

		jmp ExitIt

		// Overflow handlers.
	CarryR:
		mov bl,255
		jmp NoCarryR
	CarryG:
		mov bh,255
		jmp NoCarryG
	CarryB:
		mov bl,255
		jmp NoCarryB

		// Exit.
		ExitIt:
		mov esp,[SavedESP]
		mov ebp,[SavedEBP]
	}
}

#define AUTOMERGE(expr,simple) \
	if( Caps & CC_ColoredLight ) \
	{ \
		if( Stream.PtrVOID == NULL ) \
		{ \
			Stream = Dest; \
			DWORD *Temp = Stream.PtrDWORD; \
			for( int i=0; i<MeshVSize; i++ ) \
			{ \
				memset( Temp, 0, MeshUSize * sizeof(DWORD) ); \
				Temp += MeshUSize + MeshUSkip; \
			} \
		} \
		BYTE    *Src      = Info.IlluminationMap; \
		BYTE    *MulR     = &ByteMul[Info.ByteColor.R][0]; \
		BYTE    *MulG     = &ByteMul[Info.ByteColor.G][0]; \
		BYTE    *MulB     = &ByteMul[Info.ByteColor.B][0]; \
		for( int i=0; i<MeshVSize; i++ ) \
		{ \
			if( ASM /*&& simple*/ ) \
			{ \
				Test( Src, Stream, Dest, MeshUSize, Info.ByteColor ); \
				Src             += MeshUSize; \
				Stream.PtrDWORD += MeshUSize + MeshUSkip; \
				Dest.PtrDWORD   += MeshUSize + MeshUSkip; \
			} \
			else \
			{ \
				for( int j=0; j<MeshUSize; j++ ) \
				{ \
					BYTE Light = ftoi(expr); \
					Dest.PtrBYTE[0] = ByteClamp[(int)Stream.PtrBYTE[0] + MulR[Light]]; \
					Dest.PtrBYTE[1] = ByteClamp[(int)Stream.PtrBYTE[1] + MulG[Light]]; \
					Dest.PtrBYTE[2] = ByteClamp[(int)Stream.PtrBYTE[2] + MulB[Light]]; \
					Stream.PtrDWORD++; \
					Dest.PtrDWORD++; \
				} \
				Stream.PtrDWORD += MeshUSkip; \
				Dest.PtrDWORD   += MeshUSkip; \
			} \
		} \
	} \
	else \
	{ \
		if( Stream.PtrVOID == NULL ) \
		{ \
			Stream = Dest; \
			FLOAT *Temp = Stream.PtrFLOAT; \
			for( int i=0; i<MeshVSize; i++ ) \
			{ \
				for( int j=0; j<MeshUSize; j++ ) \
					*Temp++ = (FLOAT)((3<<22) + 0x10); \
				Temp += MeshUSkip; \
			} \
		} \
		BYTE  *Src  = Info.IlluminationMap; \
		FLOAT Scale = Info.FloatScale; \
		for( int i=0; i<MeshVSize; i++ ) \
		{ \
			for( int j=0; j<MeshUSize; j++ ) \
				*Dest.PtrFLOAT++ = *Stream.PtrFLOAT++ + Scale * (expr); \
			Dest.PtrFLOAT   += MeshUSkip; \
			Stream.PtrFLOAT += MeshUSkip; \
		} \
	}

// No special effects, floating point
void FLightManager::merge_None( BYTE Caps, INT Key, FLightInfo &Info, RAINBOW_PTR Stream, RAINBOW_PTR Dest )
{
	guardSlow(merge_None);
	AUTOMERGE(*Src++,1)
	unguardSlow;
}

// Torch wavering, floating point
void FLightManager::merge_TorchWaver( BYTE Caps, INT Key, FLightInfo &Info, RAINBOW_PTR Stream, RAINBOW_PTR Dest )
{
	guardSlow(FLightManager::merge_TorchWaver);
	AUTOMERGE(*Src++ * (0.95 + 0.05 * GRandoms->RandomBase(Key++)),0);
	unguardSlow;
}

// Fire wavering, floating point
void FLightManager::merge_FireWaver( BYTE Caps, INT Key, FLightInfo &Info, RAINBOW_PTR Stream, RAINBOW_PTR Dest )
{
	guardSlow(FLightManager::merge_FireWaver);
	AUTOMERGE(*Src++ * (0.80 + 0.20 * GRandoms->RandomBase(Key++)),0);
	unguardSlow;
}

// Water shimmering, floating point
void FLightManager::merge_WaterShimmer( BYTE Caps, INT Key, FLightInfo &Info, RAINBOW_PTR Stream, RAINBOW_PTR Dest )
{
	guardSlow(FLightManager::merge_WaterShimmer);
	AUTOMERGE(*Src++ * (0.6 + 0.4 * GRandoms->Random(Key++)),0);
	unguardSlow;
}

// Merge routine for testing
void FLightManager::merge_Test( BYTE Caps, INT Key, FLightInfo &Info, RAINBOW_PTR Stream, RAINBOW_PTR Dest )
{
	guardSlow(FLightManager::merge_Test);
	AUTOMERGE(*Src++,0);
	unguardSlow;
}

//////////////////////////////
// Spatial effect functions //
//////////////////////////////

//
// Convenience macros that give you access to the following parameters easily:
// Info			= FLightInfo pointer
// Vertex		= This point in space
// Location		= Location of light in space
// RRadiusMult	= Inverse radius multiplier
//
#define SPATIAL_PRE \
	STAT(GStat.MeshPtsGen+=MeshSpace); \
	STAT(GStat.MeshesGen++); \
	if( !TemporaryTablesBuilt ) \
		BuildTemporaryTables(); \
	\
	/* Compute values for stepping through mesh points */ \
	FVector	Vertex1		= WorldBase + InverseUAxis*FTextureUStart + InverseVAxis*FTextureVStart; \
	FVector VertexDU	= InverseUAxis * FMeshSpacing; \
	FVector VertexDV	= InverseVAxis * FMeshSpacing; \
    \
	for( int VCounter = 0; VCounter < MeshVSize; VCounter++ ) {

#define SPATIAL_POST \
		Vertex1 += VertexDV; }

#define SPATIAL_BEGIN \
	SPATIAL_PRE \
	FVector Vertex      = Vertex1; \
	FVector Location    = Info->Actor->Location; \
	FLOAT	RRadiusMult = Info->RRadiusMult; \
	FLOAT   Diffuse     = Info->Diffuse; \
	for( int i=0; i<MeshUSize; i++,Vertex+=VertexDU,Src++,Dest++ ) { \
		if( *Src != 0.0 ) { \
			DWORD SqrtOfs = ftoi( FDistSquared(Vertex,Location) * RRadiusMult ); \
			if( SqrtOfs<4096 ) {

#define SPATIAL_BEGIN1 \
	SPATIAL_PRE \
	FVector Vertex = Vertex1 - Info->Actor->Location; \
	FLOAT	RRadiusMult = Info->RRadiusMult; \
	FLOAT   Diffuse     = Info->Diffuse; \
	for( int i=0; i<MeshUSize; i++,Vertex+=VertexDU,Src++,Dest++ ) { \
		if( *Src != 0.0 ) { \
			DWORD SqrtOfs = ftoi( Vertex.SizeSquared() * RRadiusMult ); \
			if( SqrtOfs<4096 ) {

#define SPATIAL_END } else *Dest = 0.0; } else *Dest = 0.0; } SPATIAL_POST

// No effects, floating point.
void FLightManager::spatial_None( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_None);
	STAT(GStat.MeshPtsGen+=MeshSpace);
	STAT(GStat.MeshesGen++);

	// Variables.
	static FVector VertexDU,VertexDV;
	static FLOAT   Scale,Diffuse;
	static INT     Dist,DistU,DistV,DistUU,DistVV,DistUV;
	static INT     Interp00,Interp10,Interp20,Interp01,Interp11,Interp02;
	static DWORD   Inner0,Inner1;
	static INT     Hecker;

	// Init.
	if( !TemporaryTablesBuilt )
		BuildTemporaryTables();

	// Compute values for stepping through mesh points:
	VertexDU = InverseUAxis * FMeshSpacing;
	VertexDV = InverseVAxis * FMeshSpacing;

	FVector	Vertex
	=	WorldBase
	+	InverseUAxis * FTextureUStart
	+	InverseVAxis * FTextureVStart
	-	Info->Actor->Location;

	Diffuse  = Info->Diffuse;
	Scale    = Info->RRadiusMult * 4096.0;
	Dist     = (Vertex   | Vertex  ) * Scale;
	DistU    = (Vertex   | VertexDU) * Scale;
	DistV    = (Vertex   | VertexDV) * Scale;
	DistUU   = (VertexDU | VertexDU) * Scale;
	DistVV   = (VertexDV | VertexDV) * Scale;
	DistUV   = (VertexDU | VertexDV) * Scale;

	Interp00 = ftoi(Dist              );
	Interp10 = ftoi(2 * DistV + DistVV);
	Interp20 = ftoi(2 * DistVV        );
	Interp01 = ftoi(2 * DistU + DistUU);
	Interp11 = ftoi(2 * DistUV        );
	Interp02 = ftoi(2 * DistUU        );

	for( int VCounter = 0; VCounter < MeshVSize; VCounter++ )
	{
		// Forward difference the square of the distance between the points.
		Inner0 = Interp00;
		Inner1 = Interp01;

		// Interpolate.
		for( int U=0; U<MeshUSize; U++ )
		{
			if( *Src!=0 && Inner0<4096*4096 ) 
			{
				*(FLOAT*)&Hecker = *Src * Diffuse * LightSqrt[Inner0>>12] + (2<<22);
				*Dest = Hecker;
			}
			else *Dest = 0;
			Src++;
			Dest++;
			Inner0 += Inner1;
			Inner1 += Interp02;
		}
		Interp00 += Interp10;
		Interp10 += Interp20;
		Interp01 += Interp11;
	}
	unguardSlow;
}

// Yawing searchlight effect
void FLightManager::spatial_SearchLight( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_SearchLight);
	FLOAT Offset
	=	(2.0 * PI)
	+	(Info->Actor->LightPhase * (8.0 * PI / 256.0))
	+	(Info->Actor->LightPeriod ? 35.0 * LevelInfo->TimeSeconds / Info->Actor->LightPeriod : 0);
	SPATIAL_BEGIN1
		FLOAT Angle = fmod( Offset + 4.0 * atan2( Vertex.X, Vertex .Y), 8.*PI );
		if( Angle<PI || Angle>PI*3.0 )
		{
			*Dest = 0.0;
		}
		else
		{
			FLOAT Scale = 0.5 + 0.5 * GMath.CosFloat(Angle);
			FLOAT D     = 0.00006 * (Square(Vertex.X) + Square(Vertex.Y));
			if( D < 1.0 )
				Scale *= D;
			*Dest = *Src * Scale * Diffuse * LightSqrt[SqrtOfs];
		}
	SPATIAL_END
	unguardSlow;
}

// Yawing rotor effect
void FLightManager::spatial_Rotor( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_Rotor);
	SPATIAL_BEGIN1
		FLOAT Angle = 6.0 * atan2(Vertex.X,Vertex.Y);
		FLOAT Scale = 0.5 + 0.5 * GMath.CosFloat(Angle + LevelInfo->TimeSeconds*3.5);
		FLOAT D     = 0.0001 * (Square(Vertex.X) + Square(Vertex.Y));
		if (D<1.0) Scale = 1.0 - D + Scale * D;
		*Dest		= *Src * Scale * Diffuse * LightSqrt[SqrtOfs];
	SPATIAL_END
	unguardSlow;
}

// Slow radial waves
void FLightManager::spatial_SlowWave( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_SlowWave);
	SPATIAL_BEGIN1
		FLOAT Scale	= 0.7 + 0.3 * GMath.SinTab(((int)SqrtApprox(Vertex.SizeSquared()) - LevelInfo->TimeSeconds*35.0) * 1024.0);
		*Dest		= *Src * Scale * Diffuse * LightSqrt[SqrtOfs];
	SPATIAL_END
	unguardSlow;
}

// Fast radial waves
void FLightManager::spatial_FastWave( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_FastWave);
	SPATIAL_BEGIN1
		FLOAT Scale	= 0.7 + 0.3 * GMath.SinTab((((int)SqrtApprox(Vertex.SizeSquared())>>2) - LevelInfo->TimeSeconds*35.0) * 2048.0);
		*Dest		= *Src * Scale * Diffuse * LightSqrt[SqrtOfs];
	SPATIAL_END
	unguardSlow;
}

// Scrolling clouds
void FLightManager::spatial_CloudCast( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_CloudCast);
	if( !LevelInfo->SkyTexture )
	{
		spatial_None( Info, Src, Dest );
		return;
	}
	LevelInfo->SkyTexture->Lock(LOCK_Read);
	BYTE	*Data	= &LevelInfo->SkyTexture->Element(0) + LevelInfo->SkyTexture->Mips[0].Offset;
	BYTE	VShift	= LevelInfo->SkyTexture->UBits;
	int		UMask	= LevelInfo->SkyTexture->USize-1;
	int		VMask	= LevelInfo->SkyTexture->VSize-1;
	int		UPan	= LevelInfo->SkyUPanSpeed * 256.0 * 40.0 * LevelInfo->TimeSeconds;
	int		VPan	= LevelInfo->SkyVPanSpeed * 256.0 * 40.0 * LevelInfo->TimeSeconds;

	// optimize: Convert to assembly and optimize reasonably well. This routine is often used
	// for large outdoors areas.
	SPATIAL_PRE
		FVector Vertex = Vertex1;
		for( int i=0; i<MeshUSize; i++,Vertex+=VertexDU,Src++,Dest++ )
		{
			*Dest = 0.0;
			if( *Src != 0.0 )
			{
				DWORD SqrtOfs = ftoi( FDistSquared(Vertex,Info->Actor->Location) * Info->RRadiusMult );
				if( SqrtOfs<4096 )
				{
					int		FixU	= ftoi((Vertex.X+Vertex.Z) * (256.0 / 12.0)) + UPan;
					int		FixV	= ftoi((Vertex.Y+Vertex.Z) * (256.0 / 12.0)) + VPan;
					int		U0		= (FixU >> 8) & UMask;
					int		U1		= (U0    + 1) & UMask;
					int		V0		= (FixV >> 8) & VMask;
					int		V1		= (V0    + 1) & VMask;

					FLOAT	Alpha1	= FixU & 255;
					FLOAT	Beta1	= FixV & 255;
					FLOAT	Alpha2	= 256.0 - Alpha1;
					FLOAT	Beta2	= 256.0 - Beta1;

					*Dest = *Src * 3.0 * LightSqrt[SqrtOfs] * Info->Diffuse *
					(
						Data[U0 + (V0<<VShift)] * Alpha2 * Beta2 +
						Data[U1 + (V0<<VShift)] * Alpha1 * Beta2 +
						Data[U0 + (V1<<VShift)] * Alpha2 * Beta1 +
						Data[U1 + (V1<<VShift)] * Alpha1 * Beta1
					) / (256.0 * 256.0 * 256.0);
				}
			}
		}
	SPATIAL_POST
	LevelInfo->SkyTexture->Unlock(LOCK_Read);
	unguardSlow;
}

// Shock wave
void FLightManager::spatial_Shock( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_Shock);
	SPATIAL_BEGIN1
		int Dist = 8.0 * SqrtApprox(Vertex.SizeSquared());
		FLOAT Brightness  = 0.9 + 0.1 * GMath.SinTab(((Dist<<1) - (LevelInfo->TimeSeconds * 4000.0))*16.0);
		Brightness       *= 0.9 + 0.1 * GMath.CosTab(((Dist   ) + (LevelInfo->TimeSeconds * 4000.0))*16.0);
		Brightness       *= 0.9 + 0.1 * GMath.SinTab(((Dist>>1) - (LevelInfo->TimeSeconds * 4000.0))*16.0);
		*Dest = *Src * Diffuse * LightSqrt[SqrtOfs] * Brightness;
	SPATIAL_END
	unguardSlow;
}

// Disco ball
void FLightManager::spatial_Disco( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_Disco);
	SPATIAL_BEGIN1
		FLOAT Yaw	= 11.0 * atan2(Vertex.X,Vertex.Y);
		FLOAT Pitch = 11.0 * atan2(SqrtApprox(Square(Vertex.X)+Square(Vertex.Y)),Vertex.Z);

		FLOAT Scale1 = 0.50 + 0.50 * GMath.CosFloat(Yaw   + LevelInfo->TimeSeconds*5.0);
		FLOAT Scale2 = 0.50 + 0.50 * GMath.CosFloat(Pitch + LevelInfo->TimeSeconds*5.0);

		FLOAT Scale  = Scale1 + Scale2 - Scale1 * Scale2;

		FLOAT D = 0.00005 * (Square(Vertex.X) + Square(Vertex.Y));
		if (D<1.0) Scale *= D;

		*Dest = *Src * (1.0-Scale) * Diffuse * LightSqrt[SqrtOfs];
	SPATIAL_END
	unguardSlow;
}

// Cylinder lightsource
void FLightManager::spatial_Cylinder( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_Cylinder);
	SPATIAL_PRE
		FVector Vertex = Vertex1 - Info->Actor->Location;
		for( int i=0; i<MeshUSize; i++,Vertex+=VertexDU,Src++,Dest++ )
		{
			*Dest = *Src * Max
			(
				1.0 - ( Square(Vertex.X) + Square(Vertex.Y) ) * Square(Info->RRadius),
				0.0
			);
		}
	SPATIAL_POST
	unguardSlow;
}

// Interference pattern
void FLightManager::spatial_Interference( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_Interference);
	SPATIAL_BEGIN1
		FLOAT Pitch = 11.0 * atan2(SqrtApprox(Square(Vertex.X)+Square(Vertex.Y)),Vertex.Z);
		FLOAT Scale = 0.50 + 0.50 * GMath.CosFloat(Pitch + LevelInfo->TimeSeconds*5.0);

		*Dest = *Src * Scale * Diffuse * LightSqrt[SqrtOfs];
	SPATIAL_END
	unguardSlow;
}

// Spotlight lighting
void FLightManager::spatial_Spotlight( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_Spotlight);
	// Directional spotlight beam lighting
	FVector View  = Info->Actor->Rotation.Vector();
	FLOAT   Sine  = 1.0 - Info->Actor->LightCone / 256.0;
	FLOAT   RSine = 1.0 / (1.0 - Sine);
	SPATIAL_BEGIN1
		FLOAT Size = SqrtApprox(Vertex.SizeSquared());
		//
		FLOAT Dot = Max(0.f,(Vertex | View) - Sine * Size) * RSine;
		if( Dot > 0.0 )
		{
			Dot /= Size;
			Dot *= Dot;
			*Dest = Dot * *Src * Diffuse * LightSqrt[SqrtOfs];
		}
		else *Dest = 0.0;
	SPATIAL_END
	unguardSlow;
}

// Spatial routine for testing
void FLightManager::spatial_Test( FLightInfo *Info, BYTE *Src, BYTE *Dest )
{
	guardSlow(FLightManager::spatial_Test);
	// Dynamic shadow test
	FVector ActorLocation = Camera->Actor->Location;
	//Temp.X += 200.0; Temp.Y += 200.0;
	SPATIAL_BEGIN
		//FVector Temp = Vertex.TransformPointBy(Camera->Uncoords);
		//FLOAT Dist = 0.01 * (Temp - Camera->Actor->Location).Size2D();
		FLOAT Dist = 0.016 * (Vertex - ActorLocation).Size2D();
		if (Dist > 1.0) Dist = 1.0;
		FLOAT V = (Dist > 1.0) ? 1.0 : Dist*Dist;
		*Dest = *Src * Diffuse * LightSqrt[SqrtOfs] * V;
	SPATIAL_END
	unguardSlow;
}

/////////////////////////////
// Lattice warping effects //
/////////////////////////////

// Warping effect.
void FLightManager::lattice_WarpU( FLightInfo &Light, FTexLattice *Start, FTexLattice *End )
{
	guardSlow(FLightManager::lattice_WarpU);
	while ( Start < End )
	{
		FVector Temp = Start->Loc.TransformVectorBy(Camera->Uncoords) + Camera->Coords.Origin;
		Start->U    += 65536.0 * 16.0 * GMath.CosFloat(Temp.X * 0.01 + LevelInfo->TimeSeconds * 3.5);
		Start->V    += 65536.0 * 16.0 * GMath.CosFloat(Temp.Y * 0.01 + LevelInfo->TimeSeconds * 3.5);
		Start++;
	}
	unguardSlow;
}

// Warping effect.
void FLightManager::lattice_WarpV( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
	guardSlow(FLightManager::lattice_WarpV);
	unguardSlow;
}

// Wavy watter effect.
void FLightManager::lattice_CalmWater( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
	guardSlow(FLightManager::lattice_CalmWater);
	while ( Start < End )
	{
		FVector Temp = Start->Loc.TransformVectorBy(Camera->Uncoords) + Camera->Coords.Origin;
		FLOAT   Dist = SqrtApprox((Start->Loc - Info.Location).SizeSquared());
		if( Dist < Info.Radius )
		{
			Dist = 65536.0 * 6.0 * Square((Info.Radius-Dist)/Info.Radius);
			Start->U += Dist * (0.6*GMath.CosFloat(Temp.X * 0.01 + LevelInfo->TimeSeconds * 3.5)+GMath.CosFloat(Temp.X * 0.006 + LevelInfo->TimeSeconds * 2.0));
			Start->V += Dist * (0.6*GMath.SinFloat(Temp.Y * 0.01 + LevelInfo->TimeSeconds * 3.5)+GMath.SinFloat(Temp.Z * 0.006 + LevelInfo->TimeSeconds * 2.0));
		}
		Start++;
	}
	unguardSlow;
}

// Churning water effect.
void FLightManager::lattice_ChurningWater( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
	guardSlow(FLightManager::lattice_ChurningWater);
	while ( Start < End )
	{
		FVector Temp = (Start->Loc - Info.Location).TransformVectorBy(Camera->Uncoords);
		FLOAT   D    = SqrtApprox(Temp.SizeSquared());
		Temp        *= 1.0/D;

		Start->U   += 65536.0 * 16.0 * Temp.Y * GMath.SinTab((16.0*D - (LevelInfo->TimeSeconds * 600.0)) * 16.0);
		Start->V   += 65536.0 * 16.0 * Temp.X * GMath.CosTab((16.0*D - (LevelInfo->TimeSeconds * 600.0)) * 16.0);

		Start++;
	}
	unguardSlow;
}

// Satellite lattice lighting.
void FLightManager::lattice_Satellite( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
	guardSlow(FLightManager::lattice_Satellite);
	FLOAT Radius  = Info.Actor->LightRadius/256.0;
	FLOAT MRadius = 1.0-Radius;
	FLOAT RRadius = 1.0/Radius;
	while ( Start < End )
	{
		FLOAT Dot = (Start->Loc | Info.Location) * Sgn(Start->Loc.Z);
		if( Dot > 0.0 )
		{
			Dot = (Square(Dot) / Start->Loc.SizeSquared() - MRadius)*RRadius;
			if( Dot > 0.0 )
				Start->FloatG += Info.Brightness * Square(Dot);
		}
		Start++;
	}
	unguardSlow;
}

inline FLOAT Atan(FLOAT F)
{
	if( F >= 0.0 )	return +AtanTbl[ftoi((APPROX_ATANS-0.01) / (1.0+F))];
	else			return -AtanTbl[ftoi((APPROX_ATANS-0.01) / (1.0-F))];
}

// Lattice volumetric lighting.
void FLightManager::lattice_Volumetric( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
	guardSlow(FLightManager::lattice_Volumetric);
	//optimize: High level:
	// Per vol light per frame
	//	Invalid reject (no zone).
	//	In-accept.
	//	Behind reject.
	//	View frustrum reject.
	//  Compute maximum acceptable subdivision size based on light prop.
	// Per poly
	//  y distance poly reject
	//  x distance poly reject
	// Per line
	//  y distance reject
	//  x out reject

	for( FTexLattice *T=Start; T<End; T++ )
	{
		FLOAT c1,c2,d,F,h;
		// d  = distance of shortest line from viewer-to-surface-line to light location.
		// c1 = distance along viewer-to-surface line to surface, relative to nearest point to light location.
		// c2 = distance along viewer-to-surface line to viewer, relative to nearest point to light location.
		// F  = fog line-integral.
		c2 = Info.Location | T->LocNormal;             // c2<=Info.Location.Size().
		d  = Info.LocationSizeSquared - c2*c2 + 0.001; // d>=0, sqrt(abs(d))=distance from light.
		h  = Info.VolRadiusSquared - d;                // Out distance squared.
		if( h < 0.0) // If viewer-to-surface line does not intersect light's sphere.
			continue; // Taken 58% of the time.

		// Compute c1 and c2 from line clipped to interior of sphere.
		h  = SqrtApprox(h);
		c1 = Max(c2 - T->PointSize,-h);
		c2 = Min(c2,h);
		if( c1 >= c2 ) // If line segment does not reside within sphere.
			continue;

		// Atan is strictly increasing --> F > 0.0.
		d  = DivSqrtApprox(d);
#if 0
		F = atan(c2*d) - atan(c1*d);
#else
		c1*=d; c2*=d;
		if( c1 > 0.0 ) // c2 > c1 > 0.0.
		{
			F = AtanTbl[ftoi(APPROX_ATANS / (1.001+c2))] 
			-	AtanTbl[ftoi(APPROX_ATANS / (1.001+c1))];
		}
		else if( c2 > 0.0 ) // c2 > 0.0 > c1.
		{
			F = AtanTbl[ftoi(APPROX_ATANS / (1.001+c2))]
			+	AtanTbl[ftoi(APPROX_ATANS / (1.001-c1))];
		}
		else // 0.0 > c2 > c1.
		{
			F =	AtanTbl[ftoi(APPROX_ATANS / (1.001-c1))]
			-	AtanTbl[ftoi(APPROX_ATANS / (1.001-c2))];
		}
#endif
		F *= d * Info.VolRadius;
		F *= F;
		F *= Info.VolBrightness * 4.0 / (F+4.0);
		if( F < 1.0 )
			T->FloatG += F - T->FloatG * F;
		else
			T->FloatG = 1.0;
	}
	unguardSlow;
}

// For testing.
void FLightManager::lattice_Test( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
	guardSlow(FLightManager::lattice_Test);
	unguardSlow;
}

///////////////////////////////////
// Global light effect functions //
///////////////////////////////////

// No global lighting
void FLightManager::global_None( class FLightInfo &Light, AActor *Actor )
{
	guardSlow(FLightManager::global_None);
	Light.Brightness=0.0;
	unguardSlow;
}

// Steady global lighting
void FLightManager::global_Steady( class FLightInfo &Light, AActor *Actor )
{
	guardSlow(FLightManager::global_Steady);
	unguardSlow;
}

// Global light pulsing effect
void FLightManager::global_Pulse( class FLightInfo &Light, AActor *Actor )
{
	guardSlow(FLightManager::global_Pulse);
	Light.Brightness *= 0.700 + 0.299 * GMath.SinTab
	(
		(LevelInfo->TimeSeconds * 35.0 * 65536.0) / Max((int)Actor->LightPeriod,1) + (Actor->LightPhase << 8)
	);
	unguardSlow;
}

// Global blinking effect
void FLightManager::global_Blink( class FLightInfo &Light, AActor *Actor )
{
	guardSlow(FLightManager::global_Blink);
	if( (int)((LevelInfo->TimeSeconds * 35.0 * 65536.0)/(Actor->LightPeriod+1) + (Actor->LightPhase << 8)) & 1 )
		Light.Brightness = 0.0;
	unguardSlow;
}

// Global flicker effect
void FLightManager::global_Flicker( class FLightInfo &Light, AActor *Actor )
{
	guardSlow(FLightManager::global_Flicker);

	FLOAT Random = GRandoms->RandomBase(Actor->GetIndex());
	if( Random < 0.5 )	Light.Brightness = 0.0;
	else				Light.Brightness *= Random;
	unguardSlow;
}

// Strobe light.
void FLightManager::global_Strobe( class FLightInfo &Light, AActor *Actor )
{
	guardSlow(FLightManager::global_Strobe);
	static float LastTimeSeconds=0; static int Toggle=0;
	if( LastTimeSeconds != LevelInfo->TimeSeconds )
	{
		LastTimeSeconds = LevelInfo->TimeSeconds;
		Toggle ^= 1;
	}
	if( Toggle ) Light.Brightness = 0.0;
	unguardSlow;
}

// Simulated light emmanating from the backdrop.
void FLightManager::global_BackdropLight( class FLightInfo &Light, AActor *Actor )
{
	guardSlow(FLightManager::global_BackdropLight);
	unguardSlow;
}

/*------------------------------------------------------------------------------------
	Implementation of FLightInfo class
------------------------------------------------------------------------------------*/

//
// Set the light info's actor.
//
void FLightManager::FLightInfo::SetActor( AActor *InActor )
{
	Actor = InActor;

	if( !Actor->LightType || !Actor->LightBrightness )
		Opt = ALO_NotLight;
	else if( Actor->LightEffect==LE_Satellite )
		Opt = ALO_BackdropLight;
	else if( Actor->bDynamicLight || !Actor->bStatic)
		Opt = ALO_MovingLight;
	else if( Actor->LightType==LT_Steady && Actor->LightEffect==LE_None )
		Opt = ALO_StaticLight;
	else
		Opt = ALO_DynamicLight;
}

//
// Compute lighting information based on an actor lightsource.
//
void FLightManager::FLightInfo::ComputeFromActor( UCamera *Camera )
{
	guardSlow(FLightManager::FLightInfo::ComputeFromActor);

	Radius			= Actor->WorldLightRadius();
	RRadius			= 1.0/Max((FLOAT)1.0,Radius);
	RRadiusMult		= 4093.0 * RRadius * RRadius;
	Location		= Actor->Location;

	if( !Actor->bOnHorizon )
	{
		Location = Location.TransformPointBy(Camera->Coords);
	}
	else
	{
		Location = Location.TransformVectorBy(Camera->Coords).Normal();
		LocationSizeSquared = 1.0;
	}

	// Figure out global dynamic lighting effect:
	ELightType Type = (ELightType)Actor->LightType;
	if( !(Camera->Actor->ShowFlags&SHOW_PlayerCtrl) )
		Type = LT_Steady;

	// Only compute global lighting effect once per actor per frame, so that
	// lights with random functions produce consistent lighting on all surfaces they hit.
	Brightness = (FLOAT)Actor->LightBrightness;
	if( Type<LT_MAX )
		GLightTypeFuncs[Type]( *this, Actor );

	// Find local dynamic lighting effect:
	Effect = Effects[(Actor->LightEffect<LE_MAX) ? Actor->LightEffect : 0];
	LatticeEffects = LatticeEffects || Effect.LatticeFxFunc;

	// Adjust lattice size if needed:
	if( Effect.MaxXBits ) GLightManager->MaxXBits = Min(GLightManager->MaxXBits,Effect.MaxXBits);
	if( Effect.MaxYBits ) GLightManager->MaxYBits = Min(GLightManager->MaxYBits,Effect.MaxYBits);
	GLightManager->MinXBits = Max(GLightManager->MinXBits,Effect.MinXBits);
	GLightManager->MinYBits = Max(GLightManager->MinYBits,Effect.MinYBits);

	// Compute brightness:
	Brightness = Clamp(Brightness * (1./256.),0.,1.);

	// Other precomputed info:
	Diffuse = Abs(((Location-Base) | Normal) * RRadius);

	// Coloring.
	if( Camera->Caps & CC_ColoredLight )
	{
		FloatColor.GetHSV( Actor->LightHue, Actor->LightSaturation, Brightness*255.f, Camera->ColorBytes );
		ByteColor.R = FloatColor.R * 255;
		ByteColor.G = FloatColor.G * 255;
		ByteColor.B = FloatColor.B * 255;
		FloatColor *= Brightness * 2.8;
	}
	else
	{
		Brightness *= SqrtApprox(Brightness);
		FloatScale  = Brightness * 2.8 * 0x3cf0 / 255.0;
	}

	// Needed for volumetric lights only.
	if( IsVolumetric )
	{
		VolRadius			= Actor->WorldVolumetricRadius();
		VolRadiusSquared	= VolRadius * VolRadius;
		LocationSizeSquared = Location.SizeSquared();
		VolBrightness		= Brightness * Actor->VolumeBrightness / 128.0;

		// Figure our project volumetric radius on screen.
		FLOAT ProjRadius = VolRadius * Camera->FX / Max(1.f,Location.Z);
		int MinLattice = Clamp((int)FLogTwo(ProjRadius * 0.065),2,5);

		GLightManager->MaxXBits = Min(GLightManager->MaxXBits,MinLattice);
		GLightManager->MaxYBits = Min(GLightManager->MaxYBits,MinLattice);
	}
	unguardSlow;
}

/*------------------------------------------------------------------------------------
	Implementation of FLightList class
------------------------------------------------------------------------------------*/

//
// Init basic properties.
//
void FLightManager::SetupForNothing( UCamera *ThisCamera )
{
	guardSlow(FLightManager::SetupForNothing);

	// Set overall parameters.
	Camera					= ThisCamera;
	Level					= Camera->Level;
	LevelInfo				= Camera->Level->Info;
	Zone					= NULL;

	// Init variables.
	IsDynamic				= 0;
	LatticeEffects			= 0;
	TemporaryTablesBuilt	= 0;	
	Mesh.PtrVOID			= NULL;

	// Texture info.
	Texture					= (UTexture *)NULL;
	BumpPalette				= (UPalette *)NULL;

	// Init lattice sizing.
	MinXBits = MinYBits = 0;
	MaxXBits = MaxYBits = 8;

	// Mark start of memory.
	Mark.Push(GMem);

	unguardSlow;
}

//
// Compute fast light list for a surface.
//
void FLightManager::SetupForSurf
(
	UCamera			*ThisCamera, 
	FVector			&ThisNormal, 
	FVector			&ThisBase, 
	INDEX			iSurf,
	INDEX			iThisLightMesh,
	DWORD			ThisPolyFlags, 
	AZoneInfo		*ThisZone,
	UTexture		*ThisTexture
)
{
	guard(FLightManager::SetupForSurf);
	STAT(clock(GStat.IllumTime));

	static AActor *BackdropLightActors[MAX_LIGHTS];
	AActor **LightActors=NULL;
	INT Key=0, StaticLights=0, DynamicLights=0, MovingLights=0, StaticLightingChange=0;

	// Set properties.
	Base			= ThisBase;
	Normal			= ThisNormal;
	PolyFlags		= ThisPolyFlags;

	// Empty out the list of lights:
	LastLight		= FirstLight;
	NumLights		= 0;

	SetupForNothing( ThisCamera );

	// Set zone.
	Zone			= ThisZone;
	INT iZone		= Zone ? Zone->ZoneNumber : 255;

	if( iSurf != INDEX_NONE )
	{
		// Set up a Bsp surface.
		Surf			= &Level->Model->Surfs(iSurf);
		iLightMesh		= iThisLightMesh;
		ShadowBase		= NULL;

		// Get light mesh index.
		if( iLightMesh != INDEX_NONE )
		{
			Index			= &Level->Model->LightMesh(iLightMesh);
			ShadowBase		= &Level->Model->LightMesh->Bits(Index->DataOffset);

			MeshShift		= Index->MeshShift;
			TextureUStart	= Index->TextureUStart;
			TextureVStart	= Index->TextureVStart;
			FTextureUStart	= Unfix(Index->TextureUStart);
			FTextureVStart	= Unfix(Index->TextureVStart);
			MeshSpacing		= Index->MeshSpacing;
			FMeshSpacing    = Index->MeshSpacing;
			MeshUSize		= (Index->MeshUSize+7) & ~7;
			MeshVSize		= Index->MeshVSize;

			// Get vectors.
			TextureU		= Level->Model->Vectors(Surf->vTextureU);
			TextureV		= Level->Model->Vectors(Surf->vTextureV);
			WorldNormal		= Level->Model->Vectors(Surf->vNormal);
			WorldBase		= Level->Model->Points (Surf->pBase);

			// Get light actors.
			NumLights		= Index->NumStaticLights + Index->NumDynamicLights;
			LightActors		= &Index->LightActor[0];

			// Setup for fog.
			if( Zone && Zone->bFogZone )
			{
				ShadowBase     = NULL;
				NumLights      = 0;
				LatticeEffects = 1;
				LightActors    = &BackdropLightActors[0];

				for( INDEX iActor=0; iActor<Level->Num; iActor++ )
				{
					AActor *Actor = Level->Element(iActor);
					if( Actor && Actor->VolumeRadius > 0 )
						BackdropLightActors[NumLights++] = Actor;
				}
			}

		}
	}
	else
	{
		// Set up for the backdrop.
		Surf			= NULL;
		iLightMesh		= INDEX_NONE;
		ShadowBase		= NULL;
		MeshShift		= 0;
		TextureUStart	= 0;
		TextureVStart	= 0;
		FTextureUStart	= 0;
		FTextureVStart	= 0;
		MeshSpacing		= 0;
		MeshUSize		= 0;
		TextureU		= GMath.ZeroVector;
		TextureV		= GMath.ZeroVector;
		WorldNormal		= GMath.ZeroVector;
		WorldBase		= GMath.ZeroVector;

		// Get light actors.
		NumLights   = 0;
		LightActors = &BackdropLightActors[0];

		for( INDEX iActor=0; iActor<Level->Num; iActor++ )
		{
			AActor *Actor = Level->Element(iActor);
			if( Actor && Actor->bOnHorizon )
				BackdropLightActors[NumLights++] = Actor;
		}
	}

	// Adjust lattice size for surface effects.
	if( PolyFlags & PF_BigWavy )
	{
		GLightManager->MaxXBits = Min(GLightManager->MaxXBits,5);
		GLightManager->MaxYBits = Min(GLightManager->MaxYBits,3);
	}
	if( PolyFlags & PF_SmallWavy )
	{
		GLightManager->MaxXBits = Min(GLightManager->MaxXBits,4);
		GLightManager->MaxYBits = Min(GLightManager->MaxYBits,3);
	}
	if( PolyFlags & PF_BigWavy )
	{
		GLightManager->MaxXBits = Min(GLightManager->MaxXBits,4);
		GLightManager->MaxYBits = Min(GLightManager->MaxYBits,3);
	}

	// Set up parameters for each light and count lights of each type:
	for( int i=0; i<NumLights; i++ )
	{
		AActor *Actor = *LightActors++;
		if( Actor && Actor->LightType!=LT_None )
		{
			LastLight->SetActor( Actor );
			if( LastLight->Opt == ALO_StaticLight )
			{
				StaticLights++;
				StaticLightingChange = StaticLightingChange || Actor->bLightChanged;
			}
			DynamicLights += LastLight->Opt == ALO_DynamicLight;
			MovingLights  += LastLight->Opt == ALO_MovingLight;

			if( !ShadowBase )
			{
				// Only performing lattice sampling, so set up the actor here.
				LastLight->ComputeFromActor( Camera );
			}

			LastLight->IsVolumetric = 
			(	Zone
			&&	Zone->bFogZone
			&&	Actor->Zone==Zone
			&& Actor->VolumeRadius
			&& Actor->VolumeBrightness );

			if( ++LastLight >= &FirstLight[MAX_LIGHTS] ) break;
		}
	}

#if 0
	// To regenerate all lighting every frame, uncomment this.
	GCache.Flush(MakeCacheID(CID_ResultantMap   ,0,0),MakeCacheID(CID_MAX,0,0));
	GCache.Flush(MakeCacheID(CID_StaticMap      ,0,0),MakeCacheID(CID_MAX,0,0));
	GCache.Flush(MakeCacheID(CID_ShadowMap      ,0,0),MakeCacheID(CID_MAX,0,0));
	GCache.Flush(MakeCacheID(CID_IlluminationMap,0,0),MakeCacheID(CID_MAX,0,0));
#endif

	// See whether we need to use the lattice lighting/effects function during
	// lattice generation.
	LatticeEffects = LatticeEffects
	||	( PolyFlags & (PF_SmallWavy | PF_BigWavy | PF_CloudWavy | PF_WaterWavy ));
	
	// Temporary cloud wavy setup.
	if( PolyFlags & PF_CloudWavy )
	{
		BackdropBrightness = 
		(
			LevelInfo->DayFraction   * LevelInfo->SkyDayBright +
			LevelInfo->NightFraction * LevelInfo->SkyNightBright
		) / (LevelInfo->DayFraction + LevelInfo->NightFraction);
	}

	// Set texture info.
	Texture = ThisTexture;
	if( !(Texture->TextureFlags & TF_BumpMap) )
	{
		// Set up for no bump mapping.
		BumpPalette	= (UPalette*)NULL;
	}
	else
	{
		// Set up for bump mapping.
		BumpPalette = Texture->Palette;
		GBlit.Palette = Texture->Palette = TempColorPalette;

		// Flush existing bump palette from cache.
		GCache.Flush(MakeCacheID(CID_LightingTable,Texture->Palette->GetIndex(),(iZone<<2)+(Camera->ColorBytes-1)));

		// Temporary hacked bump lighting pattern.
		static FLOAT Count=0.0;
		Count += 0.04;

		FLOAT CX = 0.5*sin(2*Count);
		FLOAT CY = 0.5*cos(2*Count);

		FLOAT DX = sin(1.5*Count+2)*0.3;
		FLOAT DY = 0.0*cos(1.5*Count+2)*0.3;

		BumpPalette->Lock(LOCK_ReadWrite);
		for( int i=0; i<BumpPalette->Num; i++ )
		{
			FLOAT X = BumpPalette(i).NormalU/128.0; 
			FLOAT Y = BumpPalette(i).NormalV/128.0; 

			Texture->Palette(i).R = 200.0*Square(Clamp(1.0-sqrt(Square(X-CX)+Square(Y-CY)),0.0,1.0));
			Texture->Palette(i).B = 255.0*Square(Clamp(1.0-sqrt(Square(X-DX)+Square(Y-DY)),0.0,1.0));
			Texture->Palette(i).G = Texture->Palette(i).R/2;
		}
		BumpPalette->Unlock(LOCK_ReadWrite);
	}

	// Return if there's no shadow map work to do.
	if( Camera->Actor->RendMap != REN_DynLight )
	{
		ShadowBase = NULL;
		LastLight  = FirstLight;
		NumLights  = 0;
	}
	if( !ShadowBase )
	{
		STAT(unclock(GStat.IllumTime));
		return;
	}

	// Set global values:
	MeshUByteSize	= MeshUSize >> 3;
	MeshUBits		= FLogTwo(MeshUSize);
	MeshVBits		= FLogTwo(MeshVSize);
	MeshUTile		= 1 << MeshUBits;
	MeshUSkip       = MeshUTile - MeshUSize;
	MeshVTile		= 1 << MeshVBits;
	MeshSpace		= MeshUSize * MeshVSize;
	MeshTileSpace	= MeshUTile * MeshVTile;
	MeshByteSpace	= MeshUByteSize * MeshVSize;
	MeshAndMask		= ((MeshVSize-1) + ((MeshUSize-1) << (32-MeshUBits)));

	// Copy static map to power-of-two area of memory and merge any dynamic light maps in.
	IsDynamic	= DynamicLights || MovingLights;
	int CacheID	= MakeCacheID(CID_ResultantMap,iLightMesh,Camera->Caps & CC_Mask);

	Mesh = IsDynamic ? NULL : GCache.Get(CacheID,*TopItemToUnlock++);
	if( !Mesh.PtrVOID || (Surf && (Surf->PolyFlags & PF_DynamicLight)) || StaticLightingChange)
	{
		// Clear dynamic light status.
		if( Surf )
		{
			Surf->PolyFlags &= ~PF_DynamicLight;
			if( IsDynamic ) Surf->PolyFlags |= PF_DynamicLight;
		}

		// Generate cache entry if needed.
		if( !Mesh.PtrVOID )
		{
			int Size     = ((MeshUTile)*(MeshVTile+1)+1)*sizeof(FLOAT);
			Mesh.PtrVOID = IsDynamic ? new(GMem,Size)BYTE : GCache.Create(CacheID,TopItemToUnlock[-1],Size);
		}

		// Generate static lighting, if any.
		RAINBOW_PTR Stream = NULL;
		if( StaticLights )
		{
			// Look up cached static light map.
			int CacheID = MakeCacheID( CID_StaticMap, iLightMesh, Camera->Caps & CC_Mask );
			Stream      = GCache.Get(CacheID,*TopItemToUnlock++);

			if( !Stream.PtrVOID || StaticLightingChange )
			{
				// Generate new static light map and cache it.
				if( !Stream.PtrVOID )
					Stream = GCache.Create( CacheID, TopItemToUnlock[-1],MeshTileSpace * sizeof(FLOAT) );
 				StaticLightingMapGen( Camera->Caps, Stream );
			}
		}

		// Merge in the dynamic lights.
		BYTE *ShadowLoc = ShadowBase;
		for( FLightInfo* Info=FirstLight; Info<LastLight; Info++ )
		{
			if( Info->Opt==ALO_DynamicLight || Info->Opt==ALO_MovingLight )
			{	
				// Set up:
				BYTE *ShadowMap;
				FMemMark Mark( GMem );
				Info->ComputeFromActor( Camera );
				if( Info->Opt==ALO_MovingLight )
				{
					// Build a temporary shadow map and fill it with 1.0.
					ShadowMap = new(GMem,MEM_Oned,MeshSpace)BYTE;

					// Build a temporary illumination map.
					Info->IlluminationMap = new(GDynMem,MeshSpace)BYTE;
					Info->Effect.SpatialFxFunc( Info, ShadowMap, Info->IlluminationMap );
				}
				else if( Info->Effect.IsSpatialDynamic )
				{
					// This light has spatial effects, so we must cache its shadow map since
					// we will be generating its illumination map per frame.
					int CacheID = MakeCacheID(CID_ShadowMap,iLightMesh,Info-FirstLight);
					ShadowMap = (BYTE *)GCache.Get(CacheID,*TopItemToUnlock++);
					if( !ShadowMap  )
					{
						// Create and generate its shadow map.
						ShadowMap = (BYTE *)GCache.Create(CacheID,TopItemToUnlock[-1],MeshSpace * sizeof(BYTE));
						ShadowMapGen_B( ShadowLoc, ShadowMap );
					}

					// Build a temporary illumination map:
					Info->IlluminationMap = new(GDynMem,MeshSpace)BYTE;
					Info->Effect.SpatialFxFunc( Info, ShadowMap, Info->IlluminationMap );
				}
				else
				{
					// No spatial lighting. We use a cached illumination map generated from a temporary
					// shadow map. See if the illumination map is already cached:
					int CacheID = MakeCacheID( CID_IlluminationMap, iLightMesh, Info - FirstLight );
					Info->IlluminationMap = (BYTE *)GCache.Get( CacheID, *TopItemToUnlock++ );
					if( !Info->IlluminationMap || Info->Actor->bLightChanged )
					{
						// Build a temporary shadow map.
						ShadowMap = new(GMem,MeshSpace)BYTE;
						ShadowMapGen_B( ShadowLoc, ShadowMap );

						// Build and cache an illumination map
						if( !Info->IlluminationMap )
							Info->IlluminationMap = (BYTE *)GCache.Create( CacheID, TopItemToUnlock[-1], MeshSpace * sizeof(BYTE) );
						Info->Effect.SpatialFxFunc( Info, ShadowMap, Info->IlluminationMap );
					}
				}

				// Merge the illumination map in.
				Info->Effect.MergeFxFunc( Camera->Caps, Key, *Info, Stream, Mesh );
				Stream = Mesh;
				Mark.Pop();
			}
			ShadowLoc += MeshByteSpace;
		}
		
		// If no lights at all, zero-fill the mesh.
		if( !Stream.PtrVOID )
		{
			DWORD Value;
			if( Camera->Caps & CC_ColoredLight ) Value           = 0;
			else                                 *(FLOAT*)&Value = (FLOAT)((3<<22) + 0x10);
			RAINBOW_PTR Dest = Mesh;
			for( int i=0; i<MeshVSize; i++ )
			{
				for( int i=0; i<MeshUSize; i++ )
					*Dest.PtrDWORD++ = Value;
				Dest.PtrDWORD += MeshUSkip;
			}
			Stream = Mesh;
		}

		// Copy static lighting mesh if not yet copied.
		if( Stream.PtrVOID != Mesh.PtrVOID )
			memcpy( Mesh.PtrVOID, Stream.PtrVOID, MeshTileSpace * sizeof(FLOAT) );

		// Fill in the mesh's gaps.
		Key                = iLightMesh;
		RAINBOW_PTR Dest   = Mesh.PtrFLOAT;
		int CopyLeft       = (MeshUTile-MeshUSize) >> 1;
		int CopyRight      = (MeshUTile-MeshUSize) - CopyLeft;
		for( int V=0; V<MeshVSize; V++ )
		{
			RAINBOW_PTR StartDest = Dest;
			if( !(Camera->Caps & CC_ColoredLight) )
			{
				// Scale the light mesh values to a range amicable to the mapping inner loop.
				for( int U=0; U<MeshUSize; U++ )
				{
					if( *Dest.PtrDWORD > 0x4B403cf0 ) 
						*Dest.PtrDWORD = 0x4B403cf0;
					Dest.PtrDWORD++;
				}
			}
			else Dest.PtrDWORD += MeshUSize;

			// Pad the result map's left and right edges for clean wraparound.
			// Note: CopyLeft + USize + CopyRight = USizeTiled.
			if( CopyRight )
			{
				DWORD Right = Dest.PtrDWORD[-1];
				for( int i=0; i<CopyRight; i++ ) *Dest.PtrDWORD++ = Right;

				DWORD Left = StartDest.PtrDWORD[0];
				for ( i=0; i<CopyLeft; i++ ) *Dest.PtrDWORD++ = Left;
			}
			Key += iLightMesh + V + 73;
		}

		// Fill in the overflow area past the end of the source mesh:
		for( ; V<=MeshVTile; V++ )
		{
			DWORD *Src = &Dest.PtrDWORD[-MeshUTile];
			for( int U=0; U<MeshUTile; U++ ) *Dest.PtrDWORD++ = *Src++;
		}
		*Dest.PtrDWORD++ = Dest.PtrDWORD[-MeshUTile];
	}
	STAT(unclock(GStat.IllumTime));
	unguardf(("(%i:%i/%i)",iSurf,ThisCamera->Level->Model->Surfs->Num,ThisCamera->Level->Model->Surfs->Max));
}

//
// Compute fast light list for an actor.
//
void FLightManager::SetupForActor( UCamera* ThisCamera, AActor *Actor )
{
	guard(FLightManager::SetupForActor);
	
	// Init per actor variables.
	SetupForNothing( ThisCamera );

	LastLight = FirstLight;

	/*
	// Temporarily removed.
	// Dumb stupid o(n^3logn) code! Gack!!
	AActor *Actor = &Camera->Level.Actors->Element(iActor);
	AActor *TestActor = &Camera->Level.Actors->Element(0);

	for( INDEX i=0; i<Camera->Level.Actors->Num; i++ )
	{
		if( TestActor->LightType!=LT_None && i!=iActor)
		{
			FLOAT Radius = TestActor->WorldLightRadius();
			if( !TestActor->LightRadius || FDistSquared(Actor->Location,TestActor->Location)<Radius * Radius)
			{
				if( Camera->Level.Model.LineClass(Actor->Location,TestActor->Location) )
				{
					LastLight->iActor = i;
					LastLight->Actor  = TestActor;
					LastLight->ComputeFromActor(Camera);
					if( ++LastLight >= FinalLight ) break;
				}
			}
		}
		iTestActor++;
	}
	*/
	unguard;
}

/*-----------------------------------------------------------------------------
	Per-frame dynamic lighting pass
-----------------------------------------------------------------------------*/

//
// Callback for applying a dynamic light to a node.
//
void ApplyLightCallback( UModel *Model, INDEX iNode, int ActorPtr )
{
	guardSlow(ApplyLightCallback);
	AActor *Actor = (AActor*)ActorPtr;

	while( iNode != INDEX_NONE )
	{
		FBspNode			*Node	= &Model->Nodes (iNode);
		FBspSurf			*Poly	= &Model->Surfs (Node->iSurf);
		FLightMeshIndex		*Index	=  Model->GetLightMeshIndex(Node->iSurf);

		if( !(Node->NodeFlags & NF_PolyOccluded )
			&& Index && FLightManager::NumDynLightPolys < FLightManager::MAX_DYN_LIGHT_POLYS
			&& (Actor->bSpecialLit ? (Poly->PolyFlags&PF_SpecialLit) : !(Poly->PolyFlags&PF_SpecialLit)) )
		{
			int n = Index->NumStaticLights + Index->NumDynamicLights;
			if( n < Index->MAX_POLY_LIGHTS )
			{
				// Don't apply a light twice.
				for( int i=0; i<n; i++ )
					if( Index->LightActor[i] == Actor )
						goto NextNode;

				Index->LightActor[n] = Actor;
				if( !Index->NumDynamicLights++ )
					FLightManager::DynLightPolys[FLightManager::NumDynLightPolys++] = Node->iSurf;
			}
		}
		NextNode:
		iNode = Node->iPlane;
	}
	unguardSlow;
}

//
// Apply all dynamic lighting.
//
void FLightManager::DoDynamicLighting( ULevel *Level )
{
	guard(FLightManager::DoDynamicLighting);

	// Perform dynamic lighting.
	NumDynLightPolys = 0;
	if( Level->Model->Nodes->Num )
	{
		for( INDEX iActor=0; iActor<Level->Num; iActor++ )
		{
			AActor *Actor = Level->Element(iActor);
			if
			(	(Actor)
			&&	(Actor->LightType)
			&&	(Actor->LightEffect!=LE_Satellite)
			&&	(Actor->LightBrightness)
			&&	(Actor->LightRadius)
			&&	(Actor->bDynamicLight || !Actor->bStatic) )
			{
				Level->Model->PlaneFilter
				(
					Actor->Location,
					Actor->WorldLightRadius(),
					ApplyLightCallback,
					NF_AllOccluded,
					(INT)Actor
				);
				STAT(GStat.DynLightActors++);
			}
		}
	}
	unguard;
}

//
// Remove all dynamic lighting.
//
void FLightManager::UndoDynamicLighting( ULevel *Level )
{
	guard(FLightManager::UndoDynamicLighting);

	if( !Level->Model->Nodes->Num )
		return;

	for( int i=0; i<NumDynLightPolys; i++ )
	{
		FLightMeshIndex	*Index	= Level->Model->GetLightMeshIndex(DynLightPolys[i]);
		Index->NumDynamicLights = 0;
	}

	for( INDEX iActor=0; iActor<Level->Num; iActor++ )
	{
		AActor *Actor = Level->Element(iActor);
		if( Actor )
			Actor->bLightChanged = 0;
	}
	unguard;
}

/*------------------------------------------------------------------------------------
	Fractal generator
------------------------------------------------------------------------------------*/

//
// Make a random untiled 2D fractal texture of size US*VS.
// Dest is an array of US*VS floats whose values range from 0.0 to 1.0.
//
void MakeUntiledFractal(FLOAT *Dest, int US, int VS, int BasisCell, FLOAT Min, FLOAT Max)
{
	guard(MakeUntiledFractal);

	// Check params.
	if ( US<=0 || VS<=0 ) appError( "Zero size" );
	if ( BasisCell & (BasisCell-1) ) appError( "Invalid basis cell" );

	// Locels.
	FMemMark Mark(GMem);
	int		Cell		= BasisCell;
	FLOAT	PowerFactor = 0.5;
	FLOAT   Median		= 0.5*(Min+Max);
	FLOAT	Magnitude   = 0.5*(Max-Min);
	int		ThisUS, ThisVS;
	int		PrevUS=0, PrevVS=0,iRand=0;
	FLOAT	*Cells=NULL, *NextCells, *ThisDest;

	// Generate progressively larger meshes until we reach the final size.
	while( Cell )
	{
		if( Cell > 1 )
		{
			// Creating a temporary mesh that we'll be sampling.
			ThisUS		= 1 + (US + Cell - 1) / Cell;
			ThisVS		= 1 + (VS + Cell - 1) / Cell;
			NextCells	= ThisDest = new(GMem,ThisUS * ThisVS)FLOAT;
		}
		else
		{
			// Creating the final mesh.
			ThisUS		= (US + Cell - 1) / Cell;
			ThisVS		= (VS + Cell - 1) / Cell;
			NextCells	= ThisDest = Dest;
		}

		if( Cell != BasisCell )
		{
			// Create smaller mesh based on previous mesh
			FLOAT *StartSrc1	= &Cells[0     ];
			FLOAT *StartSrc2	= &Cells[PrevUS];

			for( int V=0; V<ThisVS; V++ )
			{
				if( V & 1 )
				{
					// Odd vertical row
					FLOAT *Src1 = StartSrc1;
					FLOAT *Src2 = StartSrc2;
					
					for( int U=ThisUS; U>0; U-- )
					{
						*ThisDest++ = (Src1[0] + Src2[0]) * 0.5 + 
							Magnitude*(1-2*GRandoms->Random(iRand+0));
						if( --U>0 )
							*ThisDest++ = (Src1[0]+Src1[1]+Src2[0]+Src2[1]) * 0.25 + 
								Magnitude*(1-2*GRandoms->Random(iRand+1));
						Src1++;
						Src2++;
						iRand+=3;
					}
					StartSrc1  = StartSrc2;
					StartSrc2 += PrevUS;
					iRand     += ThisVS;
				}
				else
				{
					// Even vertical row
					FLOAT *Src = StartSrc1;

					for( int U=ThisUS; U>0; U-- )
					{
						*ThisDest++ = Src[0];
						if( --U>0 )
							*ThisDest++ = (Src[0] + Src[1]) * 0.5 + 
								Magnitude*(1-2*GRandoms->Random(iRand));
						Src++;
						iRand++;
					}
				}
			}
		}
		else
		{
			// Create initial mesh
			for( int V=ThisVS; V>0; V-- )
			{
				for( int U=ThisUS; U>0; U-- )
				{
					*ThisDest++ = Median + 
						Magnitude*(1.0-2.0*GRandoms->Random(iRand++));
				}
				iRand += ThisUS+7;
			}
		}
		Cell   /= 2;
		Cells   = NextCells;
		PrevUS  = ThisUS;
		PrevVS  = ThisVS;
		Magnitude *= PowerFactor;
	}

	// Done.
	Mark.Pop();
	unguard;
}

//
// Make a random tiled 2D fractal texture of size US*VS.
// Dest is an array of US*VS floats whose values range from 0.0 to 1.0.
//
void MakeTiledFractal( FLOAT *Dest, int Size )
{
	guard(MakeTiledFractal);
	FMemMark Mark(GMem);

	// Make sure Size is a power of two:
	if( Size&(Size-1) ) appError("Size not power of two");

	// Make wraparound table
	INT* WrapU = new(GMem,Size+1)INT;
	INT* WrapV = new(GMem,Size+1)INT;
	for( int i=0; i<Size; i++)
	{
		WrapU[i]=i;
		WrapV[i]=i*Size;
	}
	WrapU[Size] = WrapV[Size] = 0;

	// Init random index
	int iRand = 0;

	// Init base
	Dest[0] = 0.6 * 0.5;

	FLOAT Range = 0.6 * 0.5;
	int Speed   = Size;

	// Descend through mesh
	for( Speed=Size/2; Speed; (Speed /= 2, Range *= 0.5) )
	{
		FLOAT *Dest0 = &Dest[0];
		FLOAT *Dest1 = &Dest[Speed*Size];
		for( int v=Speed; v<Size; v+=Speed+Speed )
		{
			FLOAT *Dest2 = &Dest[WrapV[v+Speed]];
			for( int u=Speed; u<Size; u+=Speed+Speed )
			{
				FLOAT Base = 
					(
					+	Dest0[      u-Speed ]
					+	Dest0[WrapU[u+Speed]]
					+	Dest2[      u-Speed ]
					+	Dest2[WrapU[u+Speed]]
					) * 0.25;
				Dest1[u-Speed] = Base + Range * (-1.0 + 2.0 * GRandoms->Random(iRand+0));
				Dest0[u      ] = Base + Range * (-1.0 + 2.0 * GRandoms->Random(iRand+1));
				Dest1[u      ] = Base + Range * (-1.0 + 2.0 * GRandoms->Random(iRand+2));
				iRand += 3;
			}
			Dest0 += (Speed + Speed) * Size;
			Dest1 += (Speed + Speed) * Size;
		}
	}
	Mark.Pop();
	unguard;
}

/*------------------------------------------------------------------------------------
	Light subsystem instantiation
------------------------------------------------------------------------------------*/

FLightManager					GLightManagerInstance;
UNRENDER_API FLightManagerBase	*GLightManager = &GLightManagerInstance;

/*------------------------------------------------------------------------------------
	The End
------------------------------------------------------------------------------------*/
