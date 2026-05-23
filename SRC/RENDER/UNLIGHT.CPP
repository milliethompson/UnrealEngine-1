/*=============================================================================
	UnLight.cpp: Unreal global lighting subsystem implementation.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
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

todo: / optimize:
 *  All spatial lighting effects interpolate FDistSquared(Vertex,Location) * RRadiusMult, the
    delta of which is known to be a simple quadratic.  Could best interpolate a single fixed
	point integer quadratically instead of a vector, and do start/end rejection to skip 
	known-zero areas.
 *  Tracks span buffers for known-empty spatial lighting and known-empty shadow map.
 *  Time everything and don't bother optimizing the seldom-used functions.  I have only a vague
    idea of where lighting CPU time is being spent...
 *	Prevent the tons of unnecessary matrix inversions per frame by caching or precomputing them.
 *	See 'optimize:' notes for list of what can be optimized.
 *  A signicant percent of lighting activity occurs in fully-shadowed areas (where the shadow
    map value equals 0.0).  When I find a 0.0 shadow map, I skip a lot of the illumination map math
	but it would be nice to find a way to reject large areas of 0.0's with a single check.  Note that
	since the shadow map is generated from a bit mask of binary 1's and 0's filtered with a 5x5 filter,
	it is possible to determine an exact list of all 0.0 shadow maps from just the bit map. Perhaps this
	could be used to build a cached span buffer representing the valid x-spans in the (x,y) shadow maps.
	This would work with the existing loops like spatial_None, but it's complex and may or may not be
	worthwhile.
 *	Don't worry about the bizarre special effects like searchlight which do stuff like multiple atan2's
    per meshel. These will only be used in small rooms where performance isn't an issue.
 *  Feel free to play with additional special effects and techniques; this is set up very modularly and
    it fun to experiment with.
 *	Keep the code clean and simple; it will be extended.

Revision history:
    9-23-96, Tim: Rewritten from the ground up.
=============================================================================*/

#include "float.h"

#include "Unreal.h"
#include "UnRender.h"

#define SHADOW_SMOOTHING 1	/* Smooth shadows (should be 1) */

//
// Testing macros:
//
#if 0
	#define AssertInRange(f,r) {if(f<0.0||f>1.0)appErrorf(r ": Range(%f)",f);}
#else
	#define AssertInRange(f,r)
#endif

/*------------------------------------------------------------------------------------
	Approximate math implementation.
------------------------------------------------------------------------------------*/

FLOAT SqrtManTbl[2<<APPROX_MAN_BITS];
FLOAT DivSqrtManTbl[1<<APPROX_MAN_BITS],DivManTbl[1<<APPROX_MAN_BITS];
FLOAT DivSqrtExpTbl[1<<APPROX_EXP_BITS],DivExpTbl[1<<APPROX_EXP_BITS];
FLOAT AtanTbl[APPROX_ATANS];

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

/*------------------------------------------------------------------------------------
	Subsystem definition
------------------------------------------------------------------------------------*/

//
// Lighting manager definition.
//
class FGlobalLightManager : public FGlobalLightManagerBase
{
public:
	// FGlobalLightManagerBase functions.
	void Init();
	void Exit();
	void SetupForActor(ICamera *Camera, INDEX iActor);
	void SetupForSurf(ICamera *Camera, FVector &Normal, FVector &Base, INDEX iSurf, INDEX iLightMesh, DWORD PolyFlags, int iZone);
	void SetupForNothing(ICamera *Camera);
	void ApplyLatticeEffects(FTexLattice *Start, FTexLattice *End);
	void ReleaseLightBlock();
	void DoDynamicLighting(ILevel *Level);
	void UndoDynamicLighting(ILevel *Level);

	// Forward declarations
	class FLightInfo;

	// Constants.
	enum {RAND_CYCLE=16, N_RANDS=256, RAND_MASK = N_RANDS-1};
	enum {MAX_LIGHTS=16};
	enum {MAX_DYN_LIGHT_POLYS=2048};

	// Function pointer types.
	typedef void (*LIGHT_MERGE_FUNC)   (int Key, int MeshUSize, FLOAT* Src,FLOAT* Dest,FLOAT Scale);
	typedef void (*LIGHT_SPATIAL_FUNC) (int MeshUSize, class FLightInfo* Info, FLOAT* Src, FLOAT* Dest, FVector Vertex, FVector VertexDU);
	typedef void (*LIGHT_LATTICE_FUNC) (FLightInfo &Light, FTexLattice *Start, FTexLattice *End);
	typedef void (*LIGHT_TYPE_FUNC)    (FLightInfo &Light, AActor &Actor);

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
		FLOAT*		IlluminationMap;		// Temporary illumination map pointer.
		INT			IsVolumetric;			// Whether it's volumetric.

		// For volumetric lights.
		FLOAT		VolRadius;				// Volumetric radius.
		FLOAT		VolRadiusSquared;		// VolRadius*VolRadius.
		FLOAT		LocationSizeSquared;	// Location.SizeSqurated().
		FLOAT		VolBrightness;			// Volumetric lighting brightness.

		// Information about the lighting effect.
		FLocalEffectEntry	Effect;

		// Functions.
		void inline SetActor			(INDEX iActor,AActor *Actor);
		void inline ComputeFromActor	(ICamera *Camera);
	};

	// Global light effects.
	static void global_None				( class FLightInfo &Light, AActor &Actor );
	static void global_Steady			( class FLightInfo &Light, AActor &Actor );
	static void global_Pulse			( class FLightInfo &Light, AActor &Actor );
	static void global_Blink			( class FLightInfo &Light, AActor &Actor );
	static void global_Flicker			( class FLightInfo &Light, AActor &Actor );
	static void global_Strobe			( class FLightInfo &Light, AActor &Actor );
	static void global_BackdropLight	( class FLightInfo &Light, AActor &Actor );
	static void global_Test				( class FLightInfo &Light, AActor &Actor );

	// Simple lighting functions.
	static void merge_F_None			(int Key, int MeshUSize, FLOAT *Src,FLOAT *Dest,FLOAT Scale);
	static void merge_F_TorchWaver		(int Key, int MeshUSize, FLOAT *Src,FLOAT *Dest,FLOAT Scale);
	static void merge_F_FireWaver		(int Key, int MeshUSize, FLOAT *Src,FLOAT *Dest,FLOAT Scale);
	static void merge_F_WaterShimmer	(int Key, int MeshUSize, FLOAT *Src,FLOAT *Dest,FLOAT Scale);
	static void merge_F_Test			(int Key, int MeshUSize, FLOAT *Src,FLOAT *Dest,FLOAT Scale);

	// Spatial lighting functions.
	static void spatial_None			( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_SearchLight		( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_SlowWave		( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_FastWave		( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_CloudCast		( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_Shock			( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_Disco			( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_Interference	( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_Cylinder		( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_Rotor			( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_Spotlight		( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );
	static void spatial_Test			( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU );

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
	void BuildTemporaryTables();
	void ShadowMapGen_F(BYTE *SrcBits,FLOAT *Dest1);
	static void LightingSpanInit_F(FLOAT *Dest);
	void StaticLightingMapGen(FLOAT *Result);
	void IlluminationMapGen(FLightInfo *Info,FLOAT *Src,FLOAT *Dest);

	// Public lighting variables.
	static FLightInfo*		LastLight;
	static FLightInfo*const FinalLight;
	static FLightInfo		FirstLight[MAX_LIGHTS];
	static ALevelDescriptor	*LevelDescriptor;
	static AZoneDescriptor	*ZoneDescriptor;

	static INT				NumDynLightPolys,TemporaryTablesBuilt;
	static FLOAT*			ActorBrightness;
	static FLOAT			BackdropBrightness;

	// Tables.
	static INDEX			DynLightPolys[MAX_DYN_LIGHT_POLYS];
	static DWORD			FilterTab[8][32];
	static INT				MultiplyTab[256+2+2];

	// Arrays.
	static const FLocalEffectEntry	Effects[LE_MAX];
	static const LIGHT_TYPE_FUNC	GLightTypeFuncs[LT_MAX];

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
// FGlobalLightManager statics.
//
FGlobalLightManager::FLightInfo*		FGlobalLightManager::LastLight;
FGlobalLightManager::FLightInfo*const	FGlobalLightManager::FinalLight = &FirstLight[MAX_LIGHTS];
FGlobalLightManager::FLightInfo			FGlobalLightManager::FirstLight[MAX_LIGHTS];
ALevelDescriptor*						FGlobalLightManager::LevelDescriptor;
AZoneDescriptor*						FGlobalLightManager::ZoneDescriptor;

INT					FGlobalLightManager::NumDynLightPolys;
INT					FGlobalLightManager::TemporaryTablesBuilt;
FLOAT*				FGlobalLightManager::ActorBrightness;
FLOAT				FGlobalLightManager::BackdropBrightness;

INDEX				FGlobalLightManager::DynLightPolys[MAX_DYN_LIGHT_POLYS];
DWORD				FGlobalLightManager::FilterTab[8][32];
INT					FGlobalLightManager::MultiplyTab[256+2+2];

FCacheItem*			FGlobalLightManager::ItemsToUnlock[MAX_UNLOCKED_ITEMS];
FCacheItem**		FGlobalLightManager::TopItemToUnlock;

const FGlobalLightManager::FLocalEffectEntry FGlobalLightManager::Effects[LE_MAX] =
{
// LE_ tag			Simple func			    Spatial func			Lattice func		MinXBit	MaxXBit	MinYBit	MaxYBit	SpacDyn
// ----------------	---------------------	-----------------------	-------------------	------- ------- ------- -------	-------
{/* None         */	merge_F_None,			spatial_None,			NULL,				0,		0,		0,		0,		0		},
{/* TorchWaver   */	merge_F_TorchWaver,		spatial_None,			NULL,				0,		0,		0,		0,		0		},
{/* FireWaver    */	merge_F_FireWaver,		spatial_None,			NULL,				0,		0,		0,		0,		0		},
{/* WateryShimmer*/	merge_F_WaterShimmer,	spatial_None,			NULL,				0,		0,		0,		0,		0		},
{/* Searchlight  */	merge_F_None,			spatial_SearchLight,	NULL,				0,		0,		0,		0,		1		},
{/* SlowWave     */	merge_F_None,			spatial_SlowWave,		NULL,				0,		0,		0,		0,		1		},
{/* FastWave     */	merge_F_None,			spatial_FastWave,		NULL,				0,		0,		0,		0,		1		},
{/* CloudCast    */	merge_F_None,			spatial_CloudCast,		NULL,				0,		0,		0,		0,		1		},
{/* StaticSpot   */	merge_F_None,			spatial_Spotlight,		NULL,				0,		0,		0,		0,		0		},
{/* Shock        */	merge_F_None,			spatial_Shock,			NULL,				0,		0,		0,		0,		1		},
{/* Disco        */	merge_F_None,			spatial_Disco,			NULL,				0,		0,		0,		0,		1		},
{/* Warp         */	merge_F_None,			spatial_None,			lattice_WarpU,		0,		0,		0,		0,		0		},
{/* Spotlight    */	merge_F_None,			spatial_Spotlight,		NULL,				0,		0,		0,		0,		0		},
{/* StolenQuakeWa*/	merge_F_None,			spatial_None,			lattice_CalmWater,	0,		0,		0,		0,		0		},
{/* ChurningWater*/	merge_F_None,			spatial_None,			lattice_ChurningWater,0,	0,		0,		0,		0		},
{/* Satellite    */	merge_F_None,			spatial_None,			lattice_Satellite,	0,		0,		0,		0,		1		},
{/* Interference */	merge_F_None,			spatial_Interference,	NULL,				0,		0,		0,		0,		1		},
{/* Cylinder     */	merge_F_None,			spatial_Cylinder,		NULL,				0,		0,		0,		0,		0		},
{/* Rotor        */	merge_F_None,			spatial_Rotor,			NULL,				0,		0,		0,		0,		1		},
{/* Unused		 */	merge_F_None,			spatial_None,			NULL,				0,		0,		0,		0,		0		},
};

const FGlobalLightManager::LIGHT_TYPE_FUNC FGlobalLightManager::GLightTypeFuncs[LT_MAX] =
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
FVector				FGlobalLightManagerBase::WorldBase;
FVector				FGlobalLightManagerBase::WorldNormal;
FVector				FGlobalLightManagerBase::Base;
FVector				FGlobalLightManagerBase::Normal;
FVector				FGlobalLightManagerBase::TextureU;
FVector				FGlobalLightManagerBase::TextureV;
FVector				FGlobalLightManagerBase::InverseUAxis;
FVector				FGlobalLightManagerBase::InverseVAxis;
FVector				FGlobalLightManagerBase::InverseNAxis;
ICamera*			FGlobalLightManagerBase::Camera;
ILevel*				FGlobalLightManagerBase::Level;
IModel*				FGlobalLightManagerBase::ModelInfo;
FBspSurf*			FGlobalLightManagerBase::Surf;
BYTE*				FGlobalLightManagerBase::ShadowBase;
void*				FGlobalLightManagerBase::MemTop;
VOID*				FGlobalLightManagerBase::MeshVoid;
QWORD				FGlobalLightManagerBase::MeshAndMask;
DWORD				FGlobalLightManagerBase::PolyFlags;
DWORD				FGlobalLightManagerBase::MaxSize;
INT					FGlobalLightManagerBase::MeshUSize;
INT					FGlobalLightManagerBase::MeshVSize;
INT					FGlobalLightManagerBase::MeshSpace;
INT					FGlobalLightManagerBase::MeshUTile;
INT					FGlobalLightManagerBase::MeshVTile;
INT					FGlobalLightManagerBase::MeshTileSpace;
INT					FGlobalLightManagerBase::MeshUByteSize;
INT					FGlobalLightManagerBase::MeshByteSpace;
INT					FGlobalLightManagerBase::MeshSkipSize;
INT					FGlobalLightManagerBase::LatticeEffects;
INT					FGlobalLightManagerBase::MinXBits;
INT					FGlobalLightManagerBase::MaxXBits;
INT					FGlobalLightManagerBase::MinYBits;
INT					FGlobalLightManagerBase::MaxYBits;
INDEX				FGlobalLightManagerBase::iLightMesh;
BYTE				FGlobalLightManagerBase::MeshUBits;
BYTE				FGlobalLightManagerBase::MeshVBits;
BYTE				FGlobalLightManagerBase::MeshShift;
INT					FGlobalLightManagerBase::TextureUStart;
INT					FGlobalLightManagerBase::TextureVStart;
INT					FGlobalLightManagerBase::MeshSpacing;
INT					FGlobalLightManagerBase::NumLights;

/*------------------------------------------------------------------------------------
	Init & Exit
------------------------------------------------------------------------------------*/

//
// Initialize the global lighting subsystem.
//
void FGlobalLightManager::Init()
{
	guard(FGlobalLightManager::Init);

	// Globals.
	NumDynLightPolys = 0;

	// Filtering table.
	int FilterBase[3][3] = 
	{
		{64,24,12},
		{24,20, 8},
		{12, 8, 5}
	};

	// Setup square root tables.
	for (DWORD D=0; D< (1<< APPROX_MAN_BITS ); D++ )
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
	for(int i=0; i<APPROX_ATANS; i++ )
	{
		FLOAT G = (FLOAT)i / ((FLOAT)APPROX_ATANS-(FLOAT)i);
		AtanTbl[APPROX_ATANS-1-i] = atan(G);
	}

	// Generate filter weights.
	int FilterWeight[8][8];
	for( i=0; i<8; i++ ) for( int j=0; j<8; j++ )
	{
#if SHADOW_SMOOTHING
		FilterWeight[i][j]=((i>4)||(j>4))?0:FilterBase[Abs(i-2)][Abs(j-2)];
#else
		FilterWeight[i][j]=(i==2)&&(j==2);
#endif
	}

	// Generate filter lookup table
	int FilterSum=0;
	for( i=0; i<5; i++ ) for( int j=0; j<8; j++ ) FilterSum += FilterWeight[i][j];

	for( i=0; i<32; i++ )
	{
		for( int j=0; j<8; j++ )
		{
			int FilterAccumulator = 0;
			int M=1;
			for( int Bit=0; Bit<5; Bit++ )
			{
				if( i&M ) FilterAccumulator += FilterWeight[j][Bit];
				M = M << 1;
			}
			FilterTab[j][i] = (1024 * FilterAccumulator) / FilterSum;
		}
	}

	// Cache items.
	InitCacheItems();

	// Success.
	debugf(LOG_Init,"Lighting subsystem initialized");
	unguard;
}

//
// Shut down the global lighting system.
//
void FGlobalLightManager::Exit()
{
	guard(FGlobalLightManager::Exit);
	debugf(LOG_Exit,"Lighting subsystem shut down");
	unguard;
}

/*------------------------------------------------------------------------------------
	Table management
------------------------------------------------------------------------------------*/

//
// Function to make sure that the temporary tables for this light mesh have been built.
//
void FGlobalLightManager::BuildTemporaryTables()
{
	guardSlow(FGlobalLightManager::BuildTemporaryTables);
	TemporaryTablesBuilt=1;
	
	// Build saturated multiply table.	
	MultiplyTab[0] = MultiplyTab[1] = 0;
	MultiplyTab[MeshVSize+2] = MultiplyTab[MeshVSize+3] = (MeshVSize-1) * MeshUByteSize;
	
	int Count=0;
	for( int i=0; i<MeshVSize; i++ )
	{
		MultiplyTab[i+2] = Count; 
		Count += MeshUByteSize;
	}

	// Build inverse vectors. Though this may seem slow, it's actually only a teeny 
	// drain on CPU time.
	STAT(clock(GStat.InversionTime));
	InvertVectors(TextureU,TextureV,WorldNormal,InverseUAxis,InverseVAxis,InverseNAxis);
	InverseUAxis *= 65536.0;
	InverseVAxis *= 65536.0;
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
void FGlobalLightManager::ShadowMapGen_F(BYTE *SrcBits,FLOAT *Dest1)
{
	guardSlow(FGlobalLightManager::ShadowMapGen_F);

	static int DirtAccumulator;
	if( !TemporaryTablesBuilt ) BuildTemporaryTables();
	//
	// Smooth and convert to floating point:
	//
	FLOAT *Dest = Dest1;
	for( int V=0; V<MeshVSize; V++ )
	{
#if SHADOW_SMOOTHING
		// Generate smooth shadow map
		// optimize: Convert to assembly. This is called a lot.
		for( int U=0; U<MeshUSize; U+=2 )
		{
			int UPos	= (U-2) >> 3;
			int Shift1	= (U-2) &  7;
			int Shift2	= (U-1) &  7;
			int A		= 0;
			int B		= 0;
			//
			for( int V2=0; V2<5; V2++ )
			{
				int		Pos		= UPos + MultiplyTab[V+V2];
				DWORD	*Filter = FilterTab[V2];

				int		D		= (DWORD)(SrcBits[Pos]) + ((DWORD)(SrcBits[Pos+1]) << 8);

				A     += Filter [(D >> Shift1) & 0x1f];
				B     += Filter [(D >> Shift2) & 0x1f];
			}
			*Dest++ = (FLOAT)A * (1.0/1024.0);
			*Dest++ = (FLOAT)B * (1.0/1024.0);
		}
#else
		// Generate abrupt shadow map, for testing/debugging only
		for( int U=0; U<MeshUSize; U++ )
		{
			int Base = U>>3;
			int Ofs  = U&7;
			*Dest++ = (SrcBits[V*MeshUByteSize + Base] & (1<<Ofs)) ? 1.0 : 0.0;
		}
#endif
	}
	if( PolyFlags & PF_DirtyShadows )
	{
		// Apply noise to shadow map
		Dest = Dest1;
		FLOAT* End = &Dest[MeshSpace];

#if 1 /* Use fractal noise */
		FLOAT *Fractal = (FLOAT *)GMem.Get(MeshSpace*sizeof(FLOAT));
		void MakeUntiledFractal(FLOAT *Dest, int US, int VS, int BasisCell, FLOAT Min, FLOAT Max);
		MakeUntiledFractal(Fractal,MeshUSize,MeshVSize,32,0.70,1.0);

		FLOAT *Src = Fractal;
		while( Dest < End )
			*Dest++ *= *Src++;

		GMem.Release(Fractal);

#else /* Use random noise */
		while( Dest < End )
			*Dest++ *= 0.80 + 0.20 * RandomBases[DirtAccumulator++ & RAND_MASK];
#endif
	}
	unguardSlow;
}

//
// Generate the illumination map for one lightsource that applies to a Bsp surface,
// based on a shadow map and positional actor information.  This illumination map accounts
// for distance from lightsource and incidence, but not the lightsource's brightness, color,
// or lattice effects, which ScaledSpanGen() handle.
//
// Input: Floating point shadow map, light info.
//
// Output: Floating point illumination map.
//
void FGlobalLightManager::IlluminationMapGen(FLightInfo *Info,FLOAT *Src,FLOAT *Dest)
{
	guardSlow(FGlobalLightManager::IlluminationMapGen);

	if( !TemporaryTablesBuilt ) BuildTemporaryTables();
	STAT(GStat.MeshPtsGen+=MeshSpace);
	STAT(GStat.MeshesGen++);

#if 0 /* Test untiled fractal routine */
	void MakeUntiledFractal(FLOAT *Dest, int US, int VS, int BasisCell, FLOAT Min, FLOAT Max);
	MakeUntiledFractal(Dest,MeshUSize,MeshVSize,16,0.4,0.8);
	return;
#endif

	// Compute values for stepping through mesh points:
	FLOAT	U			= (FLOAT)Unfix(TextureUStart);
	FLOAT	V			= (FLOAT)Unfix(TextureVStart);
	FVector	Vertex		= WorldBase + InverseUAxis*U + InverseVAxis*V;
	FVector VertexDU	= InverseUAxis * (FLOAT)MeshSpacing;
	FVector VertexDV	= InverseVAxis * (FLOAT)MeshSpacing;

	for( int VCounter = 0; VCounter < MeshVSize; VCounter++ )
	{
		Info->Effect.SpatialFxFunc(MeshUSize,Info,Src,Dest,Vertex,VertexDU);
		Vertex += VertexDV;
		Src    += MeshUSize;
		Dest   += MeshUSize;
	}
	unguardSlow;
}

//
// Init a lighting span to zero.
//
void FGlobalLightManager::LightingSpanInit_F(FLOAT *Dest)
{
	guardSlow(FGlobalLightManager::LightingSpanInit_F);

	for( int i=MeshUSize; i>0; i-- ) *Dest++ = 0;

	unguardSlow;
}

//
// Build or retrieve a cached light mesh corresponding to all of the static lightsources that affect
// a surface.  This light mesh is time-invariant.
//
void FGlobalLightManager::StaticLightingMapGen(FLOAT *Result)
{
	guard(FGlobalLightManager::StaticLightingMapGen);

	BYTE	*ShadowLoc	= ShadowBase;
	void	*DynMemTop  = GDynMem.Get(0);
	int		Offset		= 0;

	// Go through all lights and generate temporary shadow maps and illumination maps for the static ones:
	for( FLightInfo *Info = FirstLight; Info < LastLight; Info++ )
	{
		if( Info->Opt==ALO_StaticLight ) // This light is static
		{
			// Set up:
			Info->ComputeFromActor  (Camera);

			// Build the light's temporary shadow map from the raytraced shadow bits:
			FLOAT *ShadowMap = (FLOAT *)GMem.Get(MeshSpace * sizeof(FLOAT));
			ShadowMapGen_F(ShadowLoc,ShadowMap);
			
			// Build the light's temporary illumination map:
			Info->IlluminationMap = (FLOAT *)GDynMem.Get(MeshSpace * sizeof(FLOAT));
			IlluminationMapGen(Info,ShadowMap,Info->IlluminationMap);

			// Release that temporary shadow map:
			GMem.Release(ShadowMap);
		}
		ShadowLoc += MeshByteSpace;
	}

	// Go through span-by-span and add all static lights to the cumulative static map:
	FLOAT *Dest = Result;
	int Key = iLightMesh;
	for( int k=MeshVSize; k>0; k-- )
	{
		LightingSpanInit_F(Dest);

		for( Info=FirstLight; Info<LastLight; Info++ )
		{
			if( Info->Opt == ALO_StaticLight ) // This light is static
			{			
				// Scale by brightness and add to destination
				merge_F_None(Key,MeshUSize,Info->IlluminationMap,Dest,Info->Brightness);
				Info->IlluminationMap += MeshUSize;
			}
		}
		Dest += MeshUSize;
		Key  += iLightMesh + k;
	}
	GDynMem.Release(DynMemTop);

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
void FGlobalLightManager::ReleaseLightBlock()
{
	guard(FGlobalLightManager::ReleaseLightBlock);

	// Release working memory.
	GMem.Release(MemTop);

	// Unlock any locked cache items.
	UnlockCacheItems();

	// Update stats.
	STAT(GStat.Lightage += MeshUSize * MeshVSize);
	STAT(GStat.LightMem += MeshUSize * MeshVSize * sizeof(FLOAT) * (LastLight-FirstLight));

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
void FGlobalLightManager::ApplyLatticeEffects( FTexLattice *Start, FTexLattice *End )
{
	guard(FGlobalLightManager::ApplyLatticeEffects);

	FTexLattice *T;

	if( !ZoneDescriptor || !ZoneDescriptor->bFogZone )
	{
		// No fog, init brightnesses.
		if( 1 )//if( FirstLight == LastLight )
		{
			for( T=Start; T<End; T++ )
			{
				T->FloatG = UNLIT_LIGHT_VALUE;
			}
		}
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

	//
	// Apply lattice lighting effect for the particular PolyFlags.
	//

	// Small wavy.
	if( PolyFlags & PF_SmallWavy )
	{
		for( T=Start; T<End; T++ )
		{
			FVector Temp = T->Loc;
			Temp.TransformVector(Camera->Uncoords);
			Temp += Camera->Coords.Origin;
			FLOAT A = Temp.X * 0.045 + LevelDescriptor->Ticks * 0.11;
			FLOAT B = Temp.Y * 0.045 + LevelDescriptor->Ticks * 0.11;
			T->U += 65536.0 * 2.0 * (GMath.CosFloat(A)+GMath.SinFloat(B));
			T->V += 65536.0 * 2.0 * (GMath.SinFloat(A)-GMath.SinFloat(B));
		}
	}

	// Big wavy.
	if( PolyFlags & PF_BigWavy )
	{
		for( T=Start; T<End; T++ )
		{
			FVector Temp = T->Loc;
			Temp.TransformVector(Camera->Uncoords);
			Temp += Camera->Coords.Origin;
			T->U += 65536.0 * 16.0 * GMath.CosFloat(Temp.X * 0.01 + LevelDescriptor->Ticks * 0.1);
			T->V += 65536.0 * 16.0 * GMath.SinFloat(Temp.Y * 0.01 + LevelDescriptor->Ticks * 0.1);
		}
	}

	// Cloud wavy.
	if( PolyFlags & PF_CloudWavy )
	{
		static int Key = 0;
		for( T=Start; T<End; T++ )
		{
			FVector Temp = T->Loc;
			T->FloatG = 0.0;
			Temp.TransformVector(Camera->Uncoords);

			if( LevelDescriptor->bMirrorSky || (T->Loc.Z > 0))
			{
				FLOAT Ofs = PI * LevelDescriptor->Time * LevelDescriptor->SkyWavySpeed * 24.0 * 60.0 * 60.0;
				FLOAT A   = Temp.X * 0.045 + Ofs;
				FLOAT B   = Temp.Y * 0.045 + Ofs;

				T->U += 65536.0 * LevelDescriptor->SkyWavyness * (GMath.CosFloat(A)+GMath.SinFloat(B));
				T->V += 65536.0 * LevelDescriptor->SkyWavyness * (GMath.SinFloat(A)-GMath.CosFloat(B));

				FLOAT Dist = SqrtApprox(T->Loc.SizeSquared());
				T->FloatG  = 0.5 * Clamp
					(
					BackdropBrightness *
					Clamp(200.0/Dist,0.0,1.0)-GRandoms->RandomBase(Key++)*LevelDescriptor->SkyFlicker,0.0,1.0
					);
			}
		}
	}

	// Water wavy.
	if( PolyFlags & PF_WaterWavy )
	{
		for( T=Start; T<End; T++ )
		{
			FVector Temp = T->Loc;
			Temp.TransformVector(Camera->Uncoords);
			T->U += 65536.0 * 3.0 * GMath.CosFloat(Temp.X * 0.065 + LevelDescriptor->Ticks * 0.07);
			T->V += 65536.0 * 3.0 * GMath.SinFloat(Temp.Y * 0.065 + LevelDescriptor->Ticks * 0.11);
			// For testing:
			//T->FloatG *= 
			//	(0.5+0.5*GMath.CosFloat(Temp.X * 0.065 + LevelDescriptor->Ticks * 0.07)) *
			//	(0.5+0.5*GMath.SinFloat(Temp.Y * 0.054 + LevelDescriptor->Ticks * 0.07));
		}
	}

	//
	// Apply lattice lighting effect for each individual lightsource.
	//
	for( FLightInfo *Info=FirstLight; Info<LastLight; Info++ )
	{
		if( Info->Effect.LatticeFxFunc )
		{
			Info->Effect.LatticeFxFunc( *Info, Start, End );
		}
		if( Info->IsVolumetric )
		{
			lattice_Volumetric( *Info, Start, End );
		}
	}

	// Generate final brightness
	for( T=Start; T<End; T++ )
	{
		T->G = ftoi(ToLatticeLight(T->FloatG));
	}

	unguard;
}

/*------------------------------------------------------------------------------------
	All local light effects functions
------------------------------------------------------------------------------------*/

/////////////////////////////
// Simple effect functions //
/////////////////////////////

// No special effects, floating point
void FGlobalLightManager::merge_F_None(int Key, int MeshUSize, FLOAT *Src,FLOAT *Dest,FLOAT Scale)
{
	// Optimize: convert to assembly.
	while( MeshUSize-- > 0 )
	{
		FLOAT Value = Scale * *Src;
		*Dest = Value + *Dest - Value * Dest[0]; 
		AssertInRange(*Dest,"merge_F_None");
		Src++; Dest++;
	}
}

// Torch wavering, floating point
void FGlobalLightManager::merge_F_TorchWaver(int Key, int MeshUSize, FLOAT *Src,FLOAT *Dest,FLOAT Scale)
{
	// Optimize: convert to assembly.
	while( MeshUSize-- > 0 )
	{
		FLOAT Value = Scale * *Src * (0.95 + 0.05 * GRandoms->RandomBase(Key++));
		*Dest = Value + *Dest - Value * Dest[0]; 
		AssertInRange(*Dest,"merge_F_TorchWaver");
		Src++; Dest++;
	}
}

// Fire wavering, floating point
void FGlobalLightManager::merge_F_FireWaver(int Key, int MeshUSize, FLOAT *Src,FLOAT *Dest,FLOAT Scale)
{
	// Optimize: convert to assembly.
	while( MeshUSize-- > 0 )
	{
		FLOAT Value = Scale * *Src * (0.80 + 0.20 * GRandoms->RandomBase(Key++));
		*Dest = Value + *Dest - Value * Dest[0]; 
		AssertInRange(*Dest,"merge_F_FireWaver");
		Src++; Dest++;
	}
}

// Water shimmering, floating point
void FGlobalLightManager::merge_F_WaterShimmer(int Key, int MeshUSize, FLOAT *Src,FLOAT *Dest,FLOAT Scale)
{
	// Optimize: convert to assembly.
	while( MeshUSize-- > 0 )
	{
		FLOAT Value = *Src * Scale * (0.6 + 0.4 * GRandoms->Random(Key++));
		*Dest = Value + *Dest - Value * Dest[0]; 
		AssertInRange(*Dest,"merge_F_WaterShimmer4");
		Src++; Dest++;
	}
}

// Merge routine for testing
void FGlobalLightManager::merge_F_Test(int Key, int MeshUSize, FLOAT *Src,FLOAT *Dest,FLOAT Scale)
{
	while( MeshUSize-- > 0 )
	{
		FLOAT Value = Scale * *Src;
		*Dest = Value + *Dest - Value * Dest[0]; 
		AssertInRange(*Dest,"merge_F_Test");
		Src++; Dest++;
	}
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
#define SPATIAL_BEGIN \
	FVector Location    = Info->Actor->Location; \
	FLOAT	RRadiusMult = Info->RRadiusMult; \
	FLOAT   Diffuse     = Info->Diffuse; \
	for( ; MeshUSize>0; MeshUSize--,Vertex+=VertexDU,Src++,Dest++ ) { \
		if( *Src != 0.0 ) { \
			DWORD SqrtOfs = ftoi( FDistSquared(Vertex,Location) * RRadiusMult ); \
			if( SqrtOfs<16384 ) {
#define SPATIAL_BEGIN1 \
	Vertex  -= Info->Actor->Location; \
	FLOAT	RRadiusMult = Info->RRadiusMult; \
	FLOAT   Diffuse     = Info->Diffuse; \
	for( ; MeshUSize>0; MeshUSize--,Vertex+=VertexDU,Src++,Dest++ ) { \
		if( *Src != 0.0 ) { \
			DWORD SqrtOfs = ftoi( Vertex.SizeSquared() * RRadiusMult ); \
			if( SqrtOfs<16384 ) {
#define SPATIAL_END } else *Dest = 0.0; } else *Dest = 0.0; }

// No effects, floating point
void FGlobalLightManager::spatial_None( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
/*
	// Slower, simpler version:
	SPATIAL_BEGIN1
		*Dest = *Src * Diffuse * GMath.LightSqrt(SqrtOfs);
		AssertInRange(*Dest,"spatial_None");
	SPATIAL_END
*/
	// Forward difference the square of the distance between the points.
	Vertex -= Info->Actor->Location;

	FLOAT A  = (Vertex   | Vertex  ) * Info->RRadiusMult;
	FLOAT B  = (VertexDU | Vertex  ) * Info->RRadiusMult;
	FLOAT C  = (VertexDU | VertexDU) * Info->RRadiusMult;

	INT   D0 = 256.0 * (A          );
	INT   D1 = 256.0 * (2.0 * B + C);
	INT   D2 = 256.0 * (2.0 * C    );

	FLOAT Diffuse = Info->Diffuse;
	for( ; MeshUSize>0; MeshUSize--,D0+=D1,D1+=D2,Src++,Dest++ )
	{
		DWORD SqrtOfs = D0>>8;
		if( (*Src != 0.0) && (SqrtOfs<16384) ) 
		{
			*Dest = *Src * Diffuse * GMath.LightSqrt(SqrtOfs);
			AssertInRange(*Dest,"spatial_None");
		}
		else *Dest = 0.0; 
	}
}

// Yawing searchlight effect
void FGlobalLightManager::spatial_SearchLight( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	SPATIAL_BEGIN1
		FLOAT Angle = fmod
		(
			PI + 4.0 * atan2(Vertex.X,Vertex.Y) 
		+	LevelDescriptor->Ticks * 0.06,
			8.*PI
		);
		if( Angle<PI || Angle > PI*3.)
		{
			*Dest = 0.0;
		}
		else
		{
			FLOAT Scale = 0.5 + 0.5 * GMath.CosFloat(Angle);
			FLOAT D     = 0.00006 * (Square(Vertex.X) + Square(Vertex.Y));
			if (D<1.0) Scale *= D;
			*Dest = *Src * Scale * Diffuse * GMath.LightSqrt(SqrtOfs);
		}
		AssertInRange(*Dest,"spatial_Searchlight");
	SPATIAL_END
}

// Yawing rotor effect
void FGlobalLightManager::spatial_Rotor( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	SPATIAL_BEGIN1
		FLOAT Angle = 6.0 * atan2(Vertex.X,Vertex.Y);
		FLOAT Scale = 0.5 + 0.5 * GMath.CosFloat(Angle + LevelDescriptor->Ticks*0.09);
		FLOAT D     = 0.0001 * (Square(Vertex.X) + Square(Vertex.Y));
		if (D<1.0) Scale = 1.0 - D + Scale * D;
		*Dest		= *Src * Scale * Diffuse * GMath.LightSqrt(SqrtOfs);
		AssertInRange(*Dest,"spatial_Rotor");
	SPATIAL_END
}

// Slow radial waves
void FGlobalLightManager::spatial_SlowWave( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	SPATIAL_BEGIN1
		FLOAT Scale	= 0.7 + 0.3 * GMath.SinTab(((int)SqrtApprox(Vertex.SizeSquared()) - LevelDescriptor->Ticks)<<10);
		*Dest		= *Src * Scale * Diffuse * GMath.LightSqrt(SqrtOfs);
		AssertInRange(*Dest,"spatial_SlowWave");
	SPATIAL_END
}

// Fast radial waves
void FGlobalLightManager::spatial_FastWave( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	SPATIAL_BEGIN1
		FLOAT Scale	= 0.7 + 0.3 * GMath.SinTab((((int)SqrtApprox(Vertex.SizeSquared())>>2) - LevelDescriptor->Ticks)<<11);
		*Dest		= *Src * Scale * Diffuse * GMath.LightSqrt(SqrtOfs);
		AssertInRange(*Dest,"spatial_FastWave");
	SPATIAL_END
}

// Scrolling clouds
void FGlobalLightManager::spatial_CloudCast( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	if( !LevelDescriptor->SkyTexture )
	{
		spatial_None( MeshUSize, Info, Src, Dest, Vertex, VertexDU );
		return;
	}
	BYTE	*Data	= LevelDescriptor->SkyTexture->GetData() + LevelDescriptor->SkyTexture->MipOfs[0];
	BYTE	VShift	= LevelDescriptor->SkyTexture->UBits;
	int		UMask	= LevelDescriptor->SkyTexture->USize-1;
	int		VMask	= LevelDescriptor->SkyTexture->VSize-1;
	int		UPan	= LevelDescriptor->SkyUPanSpeed * 256.0 * LevelDescriptor->Ticks;
	int		VPan	= LevelDescriptor->SkyVPanSpeed * 256.0 * LevelDescriptor->Ticks;

	// optimize: Convert to assembly and optimize reasonably well. This routine is often used
	// for large outdoors areas.
	for( ; MeshUSize>0; MeshUSize--,Vertex+=VertexDU,Src++,Dest++ )
	{
		if( *Src != 0.0 )
		{
			int		FixU	= ftoi((Vertex.X+Vertex.Z) * 256.0 * 0.1) + UPan;
			int		FixV	= ftoi((Vertex.Y+Vertex.Z) * 256.0 * 0.1) + VPan;
			int		U0		= (FixU >> 8) & UMask;
			int		U1		= (U0    + 1) & UMask;
			int		V0		= (FixV >> 8) & VMask;
			int		V1		= (V0    + 1) & VMask;

			FLOAT	Alpha1	= FixU & 255;
			FLOAT	Beta1	= FixV & 255;
			FLOAT	Alpha2	= 256.0 - Alpha1;
			FLOAT	Beta2	= 256.0 - Beta1;

			*Dest = *Src * 
				(
					(FLOAT)Data[U0 + (V0<<VShift)] * Alpha2 * Beta2 +
					(FLOAT)Data[U1 + (V0<<VShift)] * Alpha1 * Beta2 +
					(FLOAT)Data[U0 + (V1<<VShift)] * Alpha2 * Beta1 +
					(FLOAT)Data[U1 + (V1<<VShift)] * Alpha1 * Beta1
				) / (256.0 * 256.0 * 256.0);

			AssertInRange(*Dest,"spatial_CloudCast");
		}
		else *Dest = 0.0;
	}
}

// Shock wave
void FGlobalLightManager::spatial_Shock( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	SPATIAL_BEGIN1
		int Dist = 8.0 * SqrtApprox(Vertex.SizeSquared());
		FLOAT Brightness  = 0.9 + 0.1 * GMath.SinTab(((Dist<<1) - (LevelDescriptor->Ticks << 7))<<4);
		Brightness *= 0.9 + 0.1 * GMath.CosTab(((Dist   ) + (LevelDescriptor->Ticks << 7))<<4);
		Brightness *= 0.9 + 0.1 * GMath.SinTab(((Dist>>1) - (LevelDescriptor->Ticks << 7))<<4);
		*Dest = *Src * Diffuse * GMath.LightSqrt(SqrtOfs) * Brightness;
		AssertInRange(*Dest,"spatial_Shock");
	SPATIAL_END
}

// Disco ball
void FGlobalLightManager::spatial_Disco( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	SPATIAL_BEGIN1
		FLOAT Yaw	= 11.0 * atan2(Vertex.X,Vertex.Y);
		FLOAT Pitch = 11.0 * atan2(SqrtApprox(Square(Vertex.X)+Square(Vertex.Y)),Vertex.Z);

		FLOAT Scale1 = 0.50 + 0.50 * GMath.CosFloat(Yaw   + LevelDescriptor->Ticks*0.15);
		FLOAT Scale2 = 0.50 + 0.50 * GMath.CosFloat(Pitch + LevelDescriptor->Ticks*0.15);

		FLOAT Scale  = Scale1 + Scale2 - Scale1 * Scale2;

		FLOAT D = 0.00005 * (Square(Vertex.X) + Square(Vertex.Y));
		if (D<1.0) Scale *= D;

		*Dest = *Src * (1.0-Scale) * Diffuse * GMath.LightSqrt(SqrtOfs);
		AssertInRange(*Dest,"spatial_Disco");
	SPATIAL_END
}

// Cylinder lightsource
void FGlobalLightManager::spatial_Cylinder( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	Vertex -= Info->Actor->Location;
	for( ; MeshUSize>0; MeshUSize--,Vertex+=VertexDU,Src++,Dest++ )
	{
		*Dest = *Src * Max
		(
			1.0 - (Square(Vertex.X)+Square(Vertex.Y))*Square(Info->RRadius),
			0.0
		);
		AssertInRange(*Dest,"spatial_Cylinder");
	}
}

// Interference pattern
void FGlobalLightManager::spatial_Interference( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	SPATIAL_BEGIN1
		FLOAT Pitch = 11.0 * atan2(SqrtApprox(Square(Vertex.X)+Square(Vertex.Y)),Vertex.Z);
		FLOAT Scale = 0.50 + 0.50 * GMath.CosFloat(Pitch + LevelDescriptor->Ticks*0.15);

		*Dest = *Src * Scale * Diffuse * GMath.LightSqrt(SqrtOfs);
		AssertInRange(*Dest,"spatial_Interference");
	SPATIAL_END
}

// Spotlight lighting
void FGlobalLightManager::spatial_Spotlight( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	// Directional spotlight beam lighting
	FVector View  = Info->Actor->DrawRot.Vector();
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
			*Dest = Dot * *Src * Diffuse * GMath.LightSqrt(SqrtOfs);
			AssertInRange(*Dest,"spatial_None");
		}
		else *Dest = 0.0;
	SPATIAL_END
}

// Spatial routine for testing
void FGlobalLightManager::spatial_Test( int MeshUSize, FLightInfo *Info, FLOAT *Src, FLOAT *Dest, FVector Vertex, FVector VertexDU )
{
	// Dynamic shadow test
	FVector ActorLocation = Camera->Actor->Location;
	//Temp.X += 200.0; Temp.Y += 200.0;
	SPATIAL_BEGIN
		//FVector Temp = Vertex; Temp.TransformPoint(Camera->Uncoords);
		//FLOAT Dist = 0.01 * (Temp - Camera->Actor->Location).Size2D();
		FLOAT Dist = 0.016 * (Vertex - ActorLocation).Size2D();
		if (Dist > 1.0) Dist = 1.0;
		FLOAT V = (Dist > 1.0) ? 1.0 : Dist*Dist;
		*Dest = *Src * Diffuse * GMath.LightSqrt(SqrtOfs) * V;
		AssertInRange(*Dest,"spatial_NegativeLight");
	SPATIAL_END
}

/////////////////////////////
// Lattice warping effects //
/////////////////////////////

// Warping effect.
void FGlobalLightManager::lattice_WarpU( FLightInfo &Light, FTexLattice *Start, FTexLattice *End )
{
	while ( Start < End )
	{
		FVector Temp = Start->Loc;
		Temp.TransformVector(Camera->Uncoords);
		Temp += Camera->Coords.Origin;

		Start->U += 65536.0 * 16.0 * GMath.CosFloat(Temp.X * 0.01 + LevelDescriptor->Ticks * 0.1);
		Start->V += 65536.0 * 16.0 * GMath.CosFloat(Temp.Y * 0.01 + LevelDescriptor->Ticks * 0.1);
		Start++;
	}
}

// Warping effect.
void FGlobalLightManager::lattice_WarpV( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
}

// Wavy watter effect.
void FGlobalLightManager::lattice_CalmWater( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
	while ( Start < End )
	{
		FVector Temp = Start->Loc;
		Temp.TransformVector(Camera->Uncoords);
		Temp += Camera->Coords.Origin;

		FLOAT Dist = SqrtApprox((Start->Loc - Info.Location).SizeSquared());
		if( Dist < Info.Radius )
		{
			Dist = 65536.0 * 6.0 * Square((Info.Radius-Dist)/Info.Radius);
			Start->U += Dist * (0.6*GMath.CosFloat(Temp.X * 0.01 + LevelDescriptor->Ticks * 0.1)+GMath.CosFloat(Temp.X * 0.006 + LevelDescriptor->Ticks * 0.06));
			Start->V += Dist * (0.6*GMath.SinFloat(Temp.Y * 0.01 + LevelDescriptor->Ticks * 0.1)+GMath.SinFloat(Temp.Z * 0.006 + LevelDescriptor->Ticks * 0.06));
		}
		Start++;
	}
}

// Churning water effect.
void FGlobalLightManager::lattice_ChurningWater( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
	while ( Start < End )
	{
		FVector Temp = Start->Loc - Info.Location;
		Temp.TransformVector(Camera->Uncoords);

		FLOAT D = SqrtApprox(Temp.SizeSquared());
		Temp *= 1.0/D;

		Start->U   += 65536.0 * 16.0 * Temp.Y * GMath.SinTab((ftoi(16.0*D) - (LevelDescriptor->Ticks << 4)) << 4);
		Start->V   += 65536.0 * 16.0 * Temp.X * GMath.CosTab((ftoi(16.0*D) - (LevelDescriptor->Ticks << 4)) << 4);

		Start++;
	}
}

// Satellite lattice lighting.
void FGlobalLightManager::lattice_Satellite( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
	FLOAT Radius  = Info.Actor->LightRadius/256.0;
	FLOAT MRadius = 1.0-Radius;
	FLOAT RRadius = 1.0/Radius;
	while ( Start < End )
	{
		FLOAT Dot = Start->Loc|Info.Location * Sgn(Start->Loc.Z);
		if( Dot > 0.0 )
		{
			Dot = (Square(Dot) / Start->Loc.SizeSquared() - MRadius)*RRadius;
			if( Dot > 0.0 )
			{
				Start->FloatG = Min(1.0,Start->FloatG * (1.0 + 2.0 * Info.Brightness * Square(Dot)));
			}
		}
		Start++;
	}
}

inline FLOAT Atan(FLOAT F)
{
	int I;
	if( F >= 0.0 )
	{
		ftoi(I,(APPROX_ATANS-0.01) / (1.0+F));
		return AtanTbl[I];
	}
	else
	{
		ftoi(I,(APPROX_ATANS-0.01) / (1.0-F));
		return -AtanTbl[I];
	}
}

// Lattice volumetric lighting.
void FGlobalLightManager::lattice_Volumetric( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
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

	//optimize: Convert to assembly after all high-level optimizations are done.
	for( FTexLattice *T=Start; T<End; T++ )
	{
		FLOAT c1,c2,d,F,h;
		// d  = distance of shortest line from viewer-to-surface-line to light location.
		// c1 = distance along viewer-to-surface line to surface, relative to nearest point to light location.
		// c2 = distance along viewer-to-surface line to viewer, relative to nearest point to light location.
		c2 = Info.Location | T->LocNormal; // c2<=Info.Location.Size()
		d  = Info.LocationSizeSquared - c2*c2 + 0.001; // d>=0, sqrt(abs(d))=distance from light
		h  = Info.VolRadiusSquared - d; // Out distance squared.
		if( h < 0.0) // viewer-to-surface line does not intersect light's sphere.
			continue; //58%

		h  = SqrtApprox(h);
		d  = DivSqrtApprox(d);

		// Compute c1 and c2 from line clipped to interior of sphere:
		c1 = Max(c2 - T->PointSize,-h);
		c2 = Min(c2,h);
		if( c1 >= c2 ) // Line segment does not reside within sphere.
			continue;

		// Atan is strictly increasing --> F > 0.0.
		// Equivalant: F = Info.Radius * d * (atan(c2*d) - atan(c1*d));
		INT I1,I2;
		c1*=d; c2*=d;
		if( c1 > 0.0 ) // c2 > c1 > 0.0
		{
			ftoi(I1,(APPROX_ATANS-0.01) / (1.0+c1));
			ftoi(I2,(APPROX_ATANS-0.01) / (1.0+c2));
			F = AtanTbl[I2] - AtanTbl[I1];
		}
		else if ( c2 > 0.0 ) // c2 > 0.0 > c1
		{
			ftoi(I1,(APPROX_ATANS-0.01) / (1.0-c1));
			ftoi(I2,(APPROX_ATANS-0.01) / (1.0+c2));
			F = AtanTbl[I2] + AtanTbl[I1];
		}
		else // 0.0 > c2 > c1
		{
			ftoi(I1,(APPROX_ATANS-0.01) / (1.0-c1));
			ftoi(I2,(APPROX_ATANS-0.01) / (1.0-c2));
			F = AtanTbl[I1] - AtanTbl[I2];
		}
		F *= d * Info.VolRadius;
		F *= SqrtApprox(F);
		F *= Info.VolBrightness * 4.0 / (F+4.0);
		if( F < 1.0 )
			T->FloatG = (T->FloatG + F) - (T->FloatG * F);
		else
			T->FloatG = 1.0;
	}
}

// For testing.
void FGlobalLightManager::lattice_Test( FLightInfo &Info, FTexLattice *Start, FTexLattice *End )
{
}

///////////////////////////////////
// Global light effect functions //
///////////////////////////////////

// No global lighting
void FGlobalLightManager::global_None( class FLightInfo &Light, AActor &Actor )
{
	Light.Brightness=0.0;
}

// Steady global lighting
void FGlobalLightManager::global_Steady( class FLightInfo &Light, AActor &Actor )
{
}

// Global light pulsing effect
void FGlobalLightManager::global_Pulse( class FLightInfo &Light, AActor &Actor )
{
	Light.Brightness *= 0.700 + 0.299 * GMath.SinTab
	(
		(LevelDescriptor->Ticks<<16) / Max((int)Actor.LightPeriod,1) + (Actor.LightPhase << 8)
	);
}

// Global blinking effect
void FGlobalLightManager::global_Blink( class FLightInfo &Light, AActor &Actor )
{
	if( ((LevelDescriptor->Ticks<<16)/(Actor.LightPeriod+1) + 
		(Actor.LightPhase << 8)) & 1 )
		Light.Brightness = 0.0;
}

// Global flicker effect
void FGlobalLightManager::global_Flicker( class FLightInfo &Light, AActor &Actor )
{
	FLOAT Random = frand();

	if( Random < 0.5 )	Light.Brightness = 0.0;
	else				Light.Brightness *= Random;
}

// Strobe light.
void FGlobalLightManager::global_Strobe( class FLightInfo &Light, AActor &Actor )
{
	static int LastTicks=0,Toggle=0;
	if( LastTicks != LevelDescriptor->Ticks )
	{
		LastTicks = LevelDescriptor->Ticks;
		Toggle ^= 1;
	}
	if( Toggle ) Light.Brightness = 0.0;
}

// Simulated light emmanating from the backdrop.
void FGlobalLightManager::global_BackdropLight( class FLightInfo &Light, AActor &Actor )
{
}

/*------------------------------------------------------------------------------------
	Implementation of FLightInfo class
------------------------------------------------------------------------------------*/

//
// Set the light info's actor
//
void FGlobalLightManager::FLightInfo::SetActor(INDEX iThisActor,AActor *ThisActor)
{
	//iActor	= iThisActor;
	Actor	= ThisActor;

	if( !ThisActor->LightType || !ThisActor->LightBrightness )
		Opt = ALO_NotLight;
	else if( ThisActor->LightEffect==LE_Satellite )
		Opt = ALO_BackdropLight;
	else if( ThisActor->bTempDynamicLight || !ThisActor->bStatic)
		Opt = ALO_MovingLight;
	else if( ThisActor->LightType==LT_Steady && ThisActor->LightEffect==LE_None )
		Opt = ALO_StaticLight;
	else
		Opt = ALO_DynamicLight;
}

//
// Compute lighting information based on an actor lightsource.
//
void FGlobalLightManager::FLightInfo::ComputeFromActor(ICamera *Camera)
{
	guardSlow(FGlobalLightManager::FLightInfo::ComputeFromActor);

	Radius			= Actor->WorldLightRadius();
	RRadius			= 1.0/Max((FLOAT)1.0,Radius);
	RRadiusMult		= 16383.0 * RRadius * RRadius;
	Location		= Actor->Location;

	if( !Actor->bOnHorizon )
	{
		Location.TransformPoint(Camera->Coords);
	}
	else
	{
		Location.TransformVector(Camera->Coords);
		Location.Normalize();
		LocationSizeSquared = 1.0;
	}

	// Figure out global dynamic lighting effect:
	ELightType Type = (ELightType)Actor->LightType;
	if(!(Camera->ShowFlags&SHOW_PlayerCtrl)) Type=LT_Steady;

	// Only compute global lighting effect once per actor per frame, so that
	// lights with random functions produce consistent lighting on all surfaces they hit.
	FLOAT *CachedBrightness = &ActorBrightness[Actor->iMe];
	if ( *CachedBrightness!=-1.0 )
	{
		Brightness = *CachedBrightness;
	}
	else
	{
		Brightness = (FLOAT)Actor->LightBrightness;
		if( Type<LT_MAX ) GLightTypeFuncs[Type](*this,*Actor);
		*CachedBrightness = Brightness;
	}

	// Find local dynamic lighting effect:
	Effect = Effects[(Actor->LightEffect<LE_MAX) ? Actor->LightEffect : 0];
	LatticeEffects = LatticeEffects || Effect.LatticeFxFunc;

	// Adjust lattice size if needed:
	if( Effect.MaxXBits ) GLightManager->MaxXBits = Min(GLightManager->MaxXBits,Effect.MaxXBits);
	if( Effect.MaxYBits ) GLightManager->MaxYBits = Min(GLightManager->MaxYBits,Effect.MaxYBits);
	GLightManager->MinXBits = Max(GLightManager->MinXBits,Effect.MinXBits);
	GLightManager->MinYBits = Max(GLightManager->MinYBits,Effect.MinYBits);

	// Compute brightness:
	if( Camera->ColorBytes == 1 ) Brightness *= (0.5 + (0.5/256.0) * Actor->LightSaturation);
	Brightness = Clamp(Brightness * (1./256.),0.,1.);
	Brightness *= SqrtApprox(Brightness);

	// Other precomputed info:
	Diffuse = Abs(((Location-Base) | Normal) * RRadius);

	// Needed for volumetric lights only:
	if( IsVolumetric )
	{
		VolRadius			= Actor->WorldVolumetricRadius();
		VolRadiusSquared	= VolRadius * VolRadius;
		LocationSizeSquared = Location.SizeSquared();
		VolBrightness		= Brightness * Actor->VolumeBrightness / 128.0;

		// Figure our project volumetric radius on screen.
		FLOAT ProjRadius = VolRadius * Camera->FSXR / Max(1.0,Location.Z);
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
void FGlobalLightManager::SetupForNothing( ICamera *ThisCamera )
{
	guardSlow(FGlobalLightManager::SetupForNothing);

	// Set overall parameters.
	Camera					= ThisCamera;
	Level					= &Camera->Level;
	ModelInfo				= &Level->ModelInfo;
	LevelDescriptor			= Camera->Level.Descriptor;
	ZoneDescriptor			= NULL;

	// Init variables.
	LatticeEffects			= 0;
	TemporaryTablesBuilt	= 0;	
	MeshVoid				= NULL;
	MemTop					= GMem.Get(0);

	// Init lattice sizing.
	MinXBits = MinYBits = 0;
	MaxXBits = MaxYBits = 8;

	unguardSlow;
}

//
// Compute fast light list for a Bsp polygon:
//
void FGlobalLightManager::SetupForSurf
(
	ICamera		*ThisCamera, 
	FVector		&ThisNormal, 
	FVector		&ThisBase, 
	INDEX		iSurf,
	INDEX		iThisLightMesh,
	DWORD		ThisPolyFlags, 
	INT			iZone
)
{
	guard(FGlobalLightManager::SetupForSurf);
	STAT(clock(GStat.IllumTime));

	static INDEX BackdropLightActors[MAX_LIGHTS];
	INDEX *LightActors=NULL;
	INT Key=0,StaticLights=0,DynamicLights=0,MovingLights=0,StaticLightingChange=0;

	// Set properties.
	Base		= ThisBase;
	Normal		= ThisNormal;
	PolyFlags	= ThisPolyFlags;

	// Empty out the list of lights:
	LastLight		= FirstLight;
	NumLights		= 0;

	SetupForNothing( ThisCamera );

	if( iZone )
	{
		ZoneDescriptor = ThisCamera->Level.GetZoneDescriptor(iZone);
	}

	if( iSurf != INDEX_NONE )
	{
		// Set up a Bsp surface.
		Surf			= &Level->ModelInfo.BspSurfs[iSurf];
		iLightMesh		= iThisLightMesh;
		ShadowBase		= NULL;
				
		// Get light mesh index.
		if( iLightMesh != INDEX_NONE )
		{
			FLightMeshIndex *Index	= &ModelInfo->LightMesh->GetData()[iLightMesh];
			ShadowBase				= &((BYTE *)ModelInfo->LightMesh->GetData())[Index->DataOffset];

			MeshShift		= Index->MeshShift;
			TextureUStart	= Index->TextureUStart;
			TextureVStart	= Index->TextureVStart;
			MeshSpacing		= Index->MeshSpacing;
			MeshUSize		= (Index->MeshUSize+7) & ~7;
			MeshVSize		= Index->MeshVSize;

			// Get vectors.
			TextureU		= ModelInfo->FVectors[Surf->vTextureU];
			TextureV		= ModelInfo->FVectors[Surf->vTextureV];
			WorldNormal		= ModelInfo->FVectors[Surf->vNormal];
			WorldBase		= ModelInfo->FPoints [Surf->pBase];

			// Get light actors.
			NumLights		= Index->NumStaticLights + Index->NumDynamicLights;
			LightActors		= &Index->iLightActor[0];

			// Setup for fog.
			if( ZoneDescriptor && ZoneDescriptor->bFogZone )
			{
				ShadowBase     = NULL;
				NumLights      = 0;
				LatticeEffects = 1;
				LightActors    = &BackdropLightActors[0];
				for( int Which = 0; Which < Level->Actors->ActiveActors->Count(); Which++)
				{
					AActor * const Actor = (*Level->Actors->ActiveActors)[Which];
					if( Actor && Actor->VolumeRadius > 0 )
					{
						BackdropLightActors[NumLights++] = Actor->iMe;
					}
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
		MeshSpacing		= 0;
		MeshUSize		= 0;
		TextureU		= GMath.ZeroVector;
		TextureV		= GMath.ZeroVector;
		WorldNormal		= GMath.ZeroVector;
		WorldBase		= GMath.ZeroVector;

		// Get light actors.
		NumLights = 0;
		LightActors = &BackdropLightActors[0];
		for( int Which = 0; Which < Level->Actors->ActiveActors->Count(); Which++)
		{
			AActor * const Actor = (*Level->Actors->ActiveActors)[Which];
			if( Actor && Actor->bOnHorizon )
			{
				BackdropLightActors[NumLights++] = Actor->iMe;
			}
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
		INDEX  iActor = *LightActors++;
		AActor *Actor = &Camera->Level.Actors->Element(iActor);

		if( Actor->Class && Actor->LightType!=LT_None )
		{
			LastLight->SetActor(iActor,Actor);

			if( LastLight->Opt == ALO_StaticLight )
			{
				StaticLights++;
				StaticLightingChange = StaticLightingChange || Actor->bTempLightChanged;
			}
			DynamicLights += LastLight->Opt == ALO_DynamicLight;
			MovingLights  += LastLight->Opt == ALO_MovingLight;

			if( !ShadowBase )
			{
				// Only performing lattice sampling, so set up the actor here.
				LastLight->ComputeFromActor  (Camera);
			}

			LastLight->IsVolumetric = 
				ZoneDescriptor && ZoneDescriptor->bFogZone 
				&& Actor->VolumeRadius && Actor->VolumeBrightness;

			if( ++LastLight >= &FirstLight[MAX_LIGHTS] ) break;
		}
	}

	// See whether we need to use the lattice lighting/effects function during
	// lattice generation.
	LatticeEffects = LatticeEffects
	||	( PolyFlags & (PF_SmallWavy | PF_BigWavy | PF_CloudWavy | PF_WaterWavy ));
	
	// Temporary cloud wavy setup.
	if( PolyFlags & PF_CloudWavy )
	{
		BackdropBrightness = 
			(
			LevelDescriptor->DayFraction   * LevelDescriptor->SkyDayBright +
			LevelDescriptor->NightFraction * LevelDescriptor->SkyNightBright
			) / (LevelDescriptor->DayFraction + LevelDescriptor->NightFraction);
	}

	// Return if there's no shadow map work to do.
	if( Camera->RendMap != REN_DynLight )
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
	MeshVTile		= 1 << MeshVBits;
	MeshSpace		= MeshUSize * MeshVSize;
	MeshTileSpace	= MeshUTile * MeshVTile;
	MeshByteSpace	= MeshUByteSize * MeshVSize;
	MeshAndMask		= ((MeshVSize-1) + ((MeshUSize-1) << (32-MeshUBits)));
	MeshSkipSize	= 1 << (MeshUBits +  MeshVBits);

	// Generate intermediate meshes for dynamic lights, if any:
	if( DynamicLights )
	{
		BYTE *ShadowLoc = ShadowBase;
		for( FLightInfo* Info=FirstLight; Info<LastLight; Info++ )
		{
			if( Info->Opt == ALO_DynamicLight )
			{	
				// Set up:
				Info->ComputeFromActor  (Camera);

				// Get the light's illumination map:
				if( Info->Effect.IsSpatialDynamic )
				{
					// This light has spatial effects, so we must cache its shadow map since
					// we will be generating its illumination map per frame.
					int CacheID = MakeCacheID(CID_ShadowMap,iLightMesh,Info-FirstLight);
					FLOAT *ShadowMap = (FLOAT *)GCache.Get(CacheID,*TopItemToUnlock++);
					if( !ShadowMap  )
					{
						// Create and generate its shadow map.
						ShadowMap = (FLOAT *)GCache.Create(CacheID,TopItemToUnlock[-1],MeshSpace * sizeof(FLOAT));
						ShadowMapGen_F(ShadowLoc,ShadowMap);
					}

					// Build a temporary illumination map:
					Info->IlluminationMap = (FLOAT *)GDynMem.Get(MeshSpace * sizeof(FLOAT));
					IlluminationMapGen(Info,ShadowMap,Info->IlluminationMap);
				}
				else
				{
					// No spatial lighting. We use a cached illumination map generated from a temporary
					// shadow map. See if the illumination map is already cached:
					int CacheID = MakeCacheID(CID_IlluminationMap,iLightMesh,Info-FirstLight);
					Info->IlluminationMap = (FLOAT *)GCache.Get(CacheID,*TopItemToUnlock++);

					if( !Info->IlluminationMap || Info->Actor->bTempLightChanged )
					{
						// Build a temporary shadow map.
						FLOAT *ShadowMap = (FLOAT *)GDynMem.Get(MeshSpace * sizeof(FLOAT));
						ShadowMapGen_F(ShadowLoc,ShadowMap);

						// Build and cache an illumination map
						if ( !Info->IlluminationMap )
						{
							Info->IlluminationMap = (FLOAT *)GCache.Create(CacheID,TopItemToUnlock[-1],MeshSpace * sizeof(FLOAT));
						}
						IlluminationMapGen(Info,ShadowMap,Info->IlluminationMap);

						// Release the temporary shadow map.
						GDynMem.Release(ShadowMap);
					}
				}
			}
			ShadowLoc += MeshByteSpace;
		}
	}

	// Generate temporary meshes for moving lights:
	if( MovingLights )
	{
		for( FLightInfo* Info=FirstLight; Info<LastLight; Info++ )
		{
			if( Info->Opt == ALO_MovingLight )
			{
				// Set up:
				Info->ComputeFromActor  (Camera);

				// Build a temporary shadow map and fill it with 1.0.
				FLOAT *ShadowMap = (FLOAT *)GMem.Get(MeshSpace * sizeof(FLOAT));
				FLOAT *Temp      = ShadowMap;
				for( int i=MeshSpace; i>0; i--) *Temp++ = 1.0;

				// Build a temporary illumination map.
				Info->IlluminationMap = (FLOAT *)GDynMem.Get(MeshSpace * sizeof(FLOAT));
				IlluminationMapGen(Info,ShadowMap,Info->IlluminationMap);

				// Release the temporary shadow map.
				GMem.Release(ShadowMap);
			}
		}
	}

	// Copy static map to power-of-two area of memory and merge any dynamic light maps in.
	int IsDynamic		= DynamicLights || MovingLights;
	int CacheID			= MakeCacheID(CID_ResultantMap,iLightMesh);

	if( IsDynamic )		MeshVoid = NULL;
	else				MeshVoid = MeshVoid = GCache.Get(CacheID,*TopItemToUnlock++);

	if( !MeshVoid || (Surf && (Surf->PolyFlags & PF_DynamicLight)) || StaticLightingChange)
	{
		if( Surf )
		{
			Surf->PolyFlags &= ~PF_DynamicLight;
			if( IsDynamic ) Surf->PolyFlags |= PF_DynamicLight;
		}

		int CopyLeft	= (MeshUTile-MeshUSize) >> 1; // CopyLeft+USize+CopyRight = USizeTiled.
		int CopyRight	= (MeshUTile-MeshUSize) - CopyLeft;

		// Generate cache entry if needed.
		if( !MeshVoid )
		{
			int Size = ((MeshUTile)*(MeshVTile+1))*sizeof(FLOAT);
			if ( IsDynamic )	MeshVoid = GMem.Get(Size);
			else				MeshVoid = GCache.Create(CacheID,TopItemToUnlock[-1],Size);
		}
		FLOAT *Dest = GetMeshFloat();
		
		// Generate static lighting, if any.
		FLOAT *Static=NULL;
		if( StaticLights )
		{
			// Look up cached static light map.
			int CacheID = MakeCacheID(CID_StaticMap,iLightMesh);
			Static      = (FLOAT *)GCache.Get(CacheID,*TopItemToUnlock++);

			if( !Static || StaticLightingChange )
			{
				// Generate new static light map and cache it.
				if ( !Static )
				{
					Static = (FLOAT *)GCache.Create(CacheID,TopItemToUnlock[-1],MeshSpace * sizeof(FLOAT));
				}
 				StaticLightingMapGen(Static);
			}
		}

		// Loop through the source mesh area:
		FLOAT *Src = Static;
		Key = iLightMesh;
		for( int V=0; V<MeshVSize; V++ )
		{
			FLOAT *StartDest = Dest;

			// Move our static lighting, if any, to the result map:
			if( Static )
			{
				// Copy static lighting to the result map:
				// optimize: Convert to assembly if needed.
				for( int U=MeshUSize; U>0; U-- ) *Dest++ = *Src++;
			}
			else
			{
				// There is no static lighting, so zero out the result map:
				// optimize: Convert to assembly if needed.
				for( int U=MeshUSize; U>0; U-- ) *Dest++ = 0.0;
			}
			
			// Add dynamic lighting to the result map:
			if( DynamicLights || MovingLights )
			{
				for( FLightInfo* Info=FirstLight; Info<LastLight; Info++ )
				{
					if( Info->Opt==ALO_DynamicLight || Info->Opt==ALO_MovingLight)
					{
						Info->Effect.MergeFxFunc( Key,MeshUSize,Info->IlluminationMap,StartDest,Info->Brightness);
						Info->IlluminationMap += MeshUSize;
					}
				}
			}

			// Scale the light mesh values to a range amicable to the mapping inner loop.
			for( int U=0; U<MeshUSize; U++ )
			{
				// optimize: Convert to assembly.
				StartDest[U] = (FLOAT)((3<<22) + 0x10) + Min(2.8*StartDest[U],1.0) * (FLOAT)0x3cf0;
			}

			// Pad the result map's left and right edges for clean wraparound:
			if( CopyRight )
			{
				// optimize: Convert to assembly.
				FLOAT Right = Dest[-1];
				for( int i=0; i<CopyRight; i++ ) *Dest++ = Right;

				FLOAT Left = StartDest[0];
				for ( i=0; i<CopyLeft; i++ ) *Dest++ = Left;
			}
			Key += iLightMesh + V + 73;
		}

		// Fill in the overflow area past the end of the source mesh:
		for( ; V<=MeshVTile; V++ )
		{
			// optimize: Convert to assembly.
			Src = GetMeshFloat();
			for( int U=0; U<MeshUTile; U++ ) *Dest++ = *Src++;
		}
	}
	STAT(unclock(GStat.IllumTime));
	unguard;
}

//
// Compute fast light list for an actor.
//
void FGlobalLightManager::SetupForActor(ICamera* ThisCamera, INDEX iActor)
{
	guard(FGlobalLightManager::SetupForActor);
	
	// Init per actor variables.
	SetupForNothing( ThisCamera );

	LastLight = FirstLight;

	/*
	// Temporarily removed.
	// Dumb stupid o(n^3logn) code! Gack!!
	AActor *Actor = &Camera->Level.Actors->Element(iActor);
	AActor *TestActor = &Camera->Level.Actors->Element(0);

	for( INDEX i=0; i<Camera->Level.Actors->Max; i++ )
	{
		if( TestActor->LightType!=LT_None && i!=iActor)
		{
			FLOAT Radius = TestActor->WorldLightRadius();
			if( !TestActor->LightRadius || FDistSquared(Actor->Location,TestActor->Location)<Radius * Radius)
			{
				if( Camera->Level.ModelInfo.LineClass(Actor->Location,TestActor->Location) )
				{
					LastLight->iActor = i;
					LastLight->Actor  = TestActor;
					LastLight->ComputeFromActor(Camera);
					if( ++LastLight >= FinalLight ) break;
				}
			}
		}
		TestActor++;
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
AActor *GTempLightActor;
void ApplyLightCallback(IModel *ModelInfo, INDEX iNode, int iActor)
{
	guardSlow(ApplyLightCallback);

	while( iNode != INDEX_NONE )
	{
		FBspNode			*Node	= &ModelInfo->BspNodes [iNode];
		FBspSurf			*Poly	= &ModelInfo->BspSurfs [Node->iSurf];
		FLightMeshIndex		*Index	= ModelInfo->GetLightMeshIndex(Node->iSurf);

		if( !(Node->NodeFlags & NF_PolyOccluded )
			&& Index && FGlobalLightManager::NumDynLightPolys < FGlobalLightManager::MAX_DYN_LIGHT_POLYS
			&& (GTempLightActor->bSpecialLit ? (Poly->PolyFlags&PF_SpecialLit) : !(Poly->PolyFlags&PF_SpecialLit)) )
		{
			int n = Index->NumStaticLights + Index->NumDynamicLights;
			if( n < Index->MAX_POLY_LIGHTS )
			{
				for( int i=0; i<n; i++ )
				{
					if( Index->iLightActor[i]==iActor ) goto NextNode; // Don't apply a light twice
				}
				Index->iLightActor[n] = iActor;
				if( !Index->NumDynamicLights++ )
				{
					FGlobalLightManager::DynLightPolys[FGlobalLightManager::NumDynLightPolys++] = Node->iSurf;
				}
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
void FGlobalLightManager::DoDynamicLighting( ILevel *Level )
{
	guard(FGlobalLightManager::DoDynamicLighting);

	// Init cached brightnesses
	FLOAT *Brightness = FGlobalLightManager::ActorBrightness = (FLOAT*)GMem.Get(Level->Actors->Max * sizeof(FLOAT));
	for( INDEX iActor=0; iActor<Level->Actors->Max; iActor++ )
	{
		*Brightness++ = -1.0;
	}

	NumDynLightPolys = 0;
	if ( Level->ModelInfo.NumBspNodes )
	{
		AActor** First = &Level->Actors->ActiveActors->First();
		AActor** Last  = &Level->Actors->ActiveActors->Last();

		while( First <= Last )
			{
			GTempLightActor = *First++;

			if(		(GTempLightActor!=NULL)
				&&	(GTempLightActor->LightType)
				&&	(GTempLightActor->LightEffect!=LE_Satellite)
				&&	(GTempLightActor->LightBrightness)
				&&	(GTempLightActor->bTempDynamicLight || !GTempLightActor->bStatic))
			{
				STAT(GStat.DynLightActors++);
				if( GTempLightActor->LightRadius )
					{
					FLOAT Radius = GTempLightActor->WorldLightRadius();
					Level->ModelInfo.PlaneFilter(GTempLightActor->Location,Radius,ApplyLightCallback,NF_AllOccluded,GTempLightActor->iMe);
				}
			}
		}
	}
	unguard;
}

//
// Remove all dynamic lighting.
//
void FGlobalLightManager::UndoDynamicLighting( ILevel *Level )
{
	guard(FGlobalLightManager::UndoDynamicLighting);

	GMem.Release(ActorBrightness);
	if( !Level->ModelInfo.NumBspNodes ) return;

	for( int i=0; i<NumDynLightPolys; i++ )
	{
		FLightMeshIndex	*Index	= Level->ModelInfo.GetLightMeshIndex(DynLightPolys[i]);
		Index->NumDynamicLights = 0;
	}

	AActor **ActorPtr = &Level->Actors->ActiveActors->First();
	AActor **LastPtr  = &Level->Actors->ActiveActors->Last();
	while( ActorPtr <= LastPtr )
	{
		AActor *Actor = *ActorPtr++;
		if( Actor ) Actor->bTempLightChanged = 0;
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
	void	*MemTop		= GMem.Get(0);
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
			NextCells	= ThisDest = (FLOAT*)GMem.Get( ThisUS * ThisVS * sizeof( FLOAT ) );
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
	GMem.Release(MemTop);
	unguard;
}

//
// Make a random tiled 2D fractal texture of size US*VS.
// Dest is an array of US*VS floats whose values range from 0.0 to 1.0.
//
void MakeTiledFractal(FLOAT *Dest, int Size)
{
	guard(MakeTiledFractal);

	void *MemTop = GMem.Get(0);

	// Make sure Size is a power of two:
	if( Size&(Size-1) ) appError("Size not power of two");

	// Make wraparound table
	INT* WrapU = (INT*)GMem.GetFast((Size+1)*sizeof(INT));
	INT* WrapV = (INT*)GMem.GetFast((Size+1)*sizeof(INT));
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
	GMem.Release(MemTop);
	unguard;
}

/*------------------------------------------------------------------------------------
	Light subsystem instantiation
------------------------------------------------------------------------------------*/

FGlobalLightManager		GLightManagerInstance;
FGlobalLightManagerBase	*GLightManager = &GLightManagerInstance;

/*------------------------------------------------------------------------------------
	The End
------------------------------------------------------------------------------------*/
