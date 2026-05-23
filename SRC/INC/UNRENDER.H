/*=============================================================================
	UnRender.h: Rendering functions and structures

	Copyright 1995 Epic MegaGames, Inc.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNRENDER // Prevent header from being included multiple times.
#define _INC_UNRENDER

/*------------------------------------------------------------------------------------
	Other includes.
------------------------------------------------------------------------------------*/

#include "UnSpan.h" // Span buffering.

/*------------------------------------------------------------------------------------
	Tables.
------------------------------------------------------------------------------------*/

static const BYTE OutXMinTab [2] = {0,FVF_OutXMin };
static const BYTE OutXMaxTab [2] = {0,FVF_OutXMax };
static const BYTE OutYMinTab [2] = {0,FVF_OutYMin };
static const BYTE OutYMaxTab [2] = {0,FVF_OutYMax };

/*------------------------------------------------------------------------------------
	Types for rendering engine.
------------------------------------------------------------------------------------*/

//
// Transformed vector with outcode info.
//
struct FOutVector : public FVector
{
public:
	BYTE  Flags;

	FLOAT ClipXM;
	FLOAT ClipXP;
	FLOAT ClipYM;
	FLOAT ClipYP;
};

//
// Transformed and projected vector.
//
class FTransform : public FOutVector
{
public:
	FLOAT ScreenX;
	FLOAT ScreenY;

	INDEX iTransform;
	DWORD iSide;

	// Transform a point.
	void Transform( const UCamera *Camera, INDEX iInTransform )
	{
		FLOAT Factor  = Camera->ProjZ / Z;
		ScreenX       = X * Factor + Camera->FX15;
		ScreenY       = Y * Factor + Camera->FY15;
		iTransform    = iInTransform;
	}

	// Compute the outcode and clipping numbers of an FTransform.
	void ComputeOutcode( const UCamera *Camera )
	{
#if ASM
		__asm
		{
			; 28 cycle clipping number and outcode computation.
			;
			;1
			mov  ecx,[this]					; Get this pointer
			mov  esi,[Camera]				; Get camera pointer
			;
			; Compute clipping numbers:
			;18
			fld  [ecx]FVector.X				; X
			fld  [ecx]FVector.Y				; Y X
			fxch							; X Y
			fmul [esi]UCamera.ProjZRX2		; X*Camera.ProjZRSX2 Y
			fxch							; Y X*Camera.ProjZRSX2
			fmul [esi]UCamera.ProjZRY2		; Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fld  [ecx]FVector.Z				; Z Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fld  [ecx]FVector.Z				; Z Z Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fxch							; Z Z Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fadd st,st(3)					; Z+X*Camera.ProjZRSX2 Z Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fxch							; Z Z+X*Camera.ProjZRSX2 Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fadd st,st(2)					; Z+Y*Camera.ProjZRSY2 Z+X*Camera.ProjZRSX2 Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fxch st(3)						; X*Camera.ProjZRSX2 Z+X*Camera.ProjZRSX2 Y*Camera.ProjZRSY2 Z+Y*Camera.ProjZRSY2
			fsubr [ecx]FVector.Z			; Z-X*Camera.ProjZRSX2 Z+X*Camera.ProjZRSX2 Y*Camera.ProjZRSY2 Z+Y*Camera.ProjZRSY2
			fxch st(2)						; Y*Camera.ProjZRSY2 Z+X*Camera.ProjZRSX2 Z-X*Camera.ProjZRSX2 Z+Y*Camera.ProjZRSY2
			fsubr [ecx]FVector.Z			; Z-Y*Camera.ProjZRSY2 Z+X*Camera.ProjZRSX2 Z-X*Camera.ProjZRSX2 Z+Y*Camera.ProjZRSY2
			fxch							; Z+X*Camera.ProjZRSX2 Z-Y*Camera.ProjZRSY2 Z-X*Camera.ProjZRSX2 Z+Y*Camera.ProjZRSY2
			fstp [ecx]FOutVector.ClipXM		; Z-Y*Camera.ProjZRSY2 Z-X*Camera.ProjZRSX2 Z+Y*Camera.ProjZRSY2
			fxch st(2)						; Z+Y*Camera.ProjZRSY2 Z-X*Camera.ProjZRSX2 Z-Y*Camera.ProjZRSY2 
			fstp [ecx]FOutVector.ClipYM		; Z-X*Camera.ProjZRSX2 Z-Y*Camera.ProjZRSY2 
			fstp [ecx]FOutVector.ClipXP		; Z-Y*Camera.ProjZRSY2 
			fstp [ecx]FOutVector.ClipYP		; (empty fp stack)
			;
			; Compute flags:
			;9
			mov  ebx,[ecx]FOutVector.ClipXM	; ebx = XM clipping number as integer
			mov  edx,[ecx]FOutVector.ClipYM	; edx = YM clipping number as integer
			;
			shr  ebx,31						; ebx = XM: 0 iff clip>=0.0, 1 iff clip<0.0
			mov  edi,[ecx]FOutVector.ClipXP	; edi = XP
			;
			shr  edx,31                     ; edx = YM: 0 or 1
			mov  esi,[ecx]FOutVector.ClipYP	; esi = YP: 0 or 1
			;
			shr  edi,31						; edi = XP: 0 or 1
			mov  al,OutXMinTab[ebx]			; al = 0 or FVF_OutXMin
			;
			shr  esi,31						; esi = YP: 0 or 1
			mov  bl,OutYMinTab[edx]			; bl = FVF_OutYMin
			;
			or   bl,al						; bl = FVF_OutXMin, FVF_OutYMin
			mov  ah,OutXMaxTab[edi]			; ah = FVF_OutXMax
			;
			or   bl,ah						; bl = FVF_OutXMin, FVF_OutYMin, OutYMax
			mov  al,OutYMaxTab[esi]			; bh = FVF_OutYMax
			;
			or   al,bl                      ; al = FVF_OutYMin and FVF_OutYMax
			;
			mov  [ecx]FOutVector.Flags,al	; Store flags
		}
#else
		ClipXM = Z + X * Camera.ProjZRX2;
		ClipXP = Z - X * Camera.ProjZRX2;

		ClipYM = Z + Y * Camera.ProjZRY2;
		ClipYP = Z - Y * Camera.ProjZRY2;

		Flags =
		(	OutXMinTab [ClipXM < 0.0]
		+	OutXMaxTab [ClipXP < 0.0]
		+	OutYMinTab [ClipYM < 0.0]
		+	OutYMaxTab [ClipYP < 0.0]);
#endif
	}
	FTransform inline operator+ (const FTransform &V) const
	{
		FTransform Temp;
		Temp.X = X + V.X; Temp.Y = Y + V.Y; Temp.Z = Z + V.Z;
		return Temp;
	}
	FTransform inline operator- (const FTransform &V) const
	{
		FTransform Temp;
		Temp.X = X - V.X; Temp.Y = Y - V.Y; Temp.Z = Z - V.Z;
		return Temp;
	}
	FTransform inline operator* (FLOAT Scale ) const
	{
		FTransform Temp;
		Temp.X = X * Scale; Temp.Y = Y * Scale; Temp.Z = Z * Scale;
		return Temp;
	}
};

//
// Transformed sample point.
//
class FTransSample : public FTransform
{
public:
	FVector		Norm;	// Vertex normal at this point.
	FVector		Color;	// Illuminated color of this point.

	// Operators.
	FTransSample inline operator+( const FTransSample &T ) const
	{
		FTransSample Temp;
		Temp.X = X + T.X; Temp.Y = Y + T.Y; Temp.Z = Z + T.Z;
		Temp.Color = Color + T.Color;
		return Temp;
	}
	FTransSample inline operator-( const FTransSample &T ) const
	{
		FTransSample Temp;
		Temp.X = X - T.X; Temp.Y = Y - T.Y; Temp.Z = Z - T.Z;
		Temp.Color = Color - T.Color;
		return Temp;
	}
	FTransSample inline operator*( FLOAT Scale ) const
	{
		FTransSample Temp;
		Temp.X = X * Scale; Temp.Y = Y * Scale; Temp.Z = Z * Scale;
		Temp.Color = Color * Scale;
		return Temp;
	}
};

//
// Transformed texture mapped point.
//
class FTransTexture : public FTransSample
{
public:
	FLOAT U,V; // Texture coordinates * 65536.0.

	// Operators.
	FTransTexture inline operator+( const FTransTexture &F ) const
	{
		FTransTexture Temp;
		Temp.X = X + F.X; Temp.Y = Y + F.Y; Temp.Z = Z + F.Z;
		Temp.U = U + F.U; Temp.V = V + F.V;
		Temp.Color = Color + F.Color;
		return Temp;
	}
	FTransTexture inline operator-( const FTransTexture &F ) const
	{
		FTransTexture Temp;
		Temp.X = X - F.X; Temp.Y = Y - F.Y; Temp.Z = Z - F.Z;
		Temp.U = U - F.U; Temp.V = V - F.V;
		Temp.Color = Color - F.Color;
		return Temp;
	}
	FTransTexture inline operator*( FLOAT Scale ) const
	{
		FTransTexture Temp;
		Temp.X = X * Scale; Temp.Y = Y * Scale; Temp.Z = Z * Scale;
		Temp.U = U * Scale; Temp.V = V * Scale;
		Temp.Color = Color * Scale;
		return Temp;
	}
};

/*------------------------------------------------------------------------------------
	Basic types.
------------------------------------------------------------------------------------*/

//
// A packed MMX data structure.
//
class FMMX
{
public:
	union
	{
		struct {SWORD  B,G,R,A;};		// RGB colors.
		struct {SWORD  U1,V1,U2,V2;};	// Texture coordinates pair.
		struct {INT    U,V;};			// Texture coordinates.
	};
};

//
// Sample lighting and texture values at a lattice point on a rectangular grid.
// 68 bytes.
//
//warning: Mirrored in UnRender.inc.
//
class FTexLattice
{
public:
	union
	{
		struct // 12 bytes.
		{
			// 256-color texture and lighting info.
			FVector	LocNormal;
			FLOAT	Pad0,PointSize,Pad1,Pad2,Pad3;
			INT		U,V,G;
			FLOAT	FloatG;
			FVector Loc;
			FLOAT   W;
		};
		struct // 24 bytes.
		{
			// MMX texture and lighting info.
			FMMX Tex;
			FMMX Color;
			FMMX Fog;
		};
		struct // 64 bytes.
		{
			// Bilinear rectangle setup.
			DWORD L,  H;
			DWORD LY, HY;
			DWORD LX, HX;
			DWORD LXY,HXY;

			// Bilinear subrectangle setup.
			DWORD SubL,  SubH;
			DWORD SubLY, SubHY;
			DWORD SubLX, SubHX;
			DWORD SubLXY,SubHXY;
		};
		struct // 64 bytes.
		{
			// Bilinear rectangle setup.
			QWORD Q;
			QWORD QY;
			QWORD QX;
			QWORD QXY;

			// Bilinear subrectangle setup.
			QWORD SubQ;
			QWORD SubQY;
			QWORD SubQX;
			QWORD SubQXY;
		};
	};
	DWORD RoutineOfs;
	DWORD AlignPad;
};

//
// Convert a floating point value in the range [0.0,1.0] into a lattice
// lighting value in the range [0x0,0x1800] with a bit of border on both
// sides to prevent underflow.
//
#define UNLIT_LIGHT_VALUE 0.4
#define ToLatticeLight(F) 0x80 + 0x3D80 * F

/*------------------------------------------------------------------------------------
	Dynamics rendering.
------------------------------------------------------------------------------------*/

//
// A temporary rendering object which represents a screen-aligned rectangular
// planar area situated at a certain location.  These are filtered down the Bsp
// to determine the areas where creature meshes and sprites must be rendered.
//
class FSprite
{
public:
	AActor			*Actor;				// Actor the sprite or mesh map is based on.
	AZoneInfo       *Zone;				// Zone it's in or NULL.
	FSpanBuffer		*SpanBuffer;		// Span buffer we're building.
	FSprite			*Next;				// Next sprite in link.
	INT				X1,Y1;				// Top left corner on screen.
	INT				X2,Y2;				// Bottom right corner, X2 must be >= X1, Y2 >= Y1.
	FLOAT 			ScreenX,ScreenY;	// Fractionally-accurate screen location of center.
	FLOAT			DrawScale,Z;		// Screenspace Z location.
	FTransform		Verts[4];			// Vertices of projection plane for filtering.
};

/*------------------------------------------------------------------------------------
	Dynamic Bsp contents.
------------------------------------------------------------------------------------*/

struct FDynamicsIndex
{
	INT 				Type;		// Type of contents.
	FSprite				*Sprite;	// Pointer to dynamic data, NULL=none.
	class FRasterPoly	*Raster;	// Rasterization this is clipped to, NULL=none.
	FLOAT				Z;			// For sorting.
	FDynamicsIndex		*Next;		// Index of next dynamic contents in list or INDEX_NONE.
	FBspNode			*Node;		// Generating node.
};

enum EDynamicsType
{
	DY_NOTHING,			// Nothing.
	DY_SPRITE,			// A sprite.
	DY_CHUNK,			// A sprite chunk.
	DY_FINALCHUNK,		// A non moving chunk.
};

/*------------------------------------------------------------------------------------
	Dynamic node contents.
------------------------------------------------------------------------------------*/

void dynamicsLock 		(UModel *Model);
void dynamicsUnlock		(UModel *Model);
void dynamicsSetup		(UCamera *Camera, AActor *Exclude);
void dynamicsFilter		(UCamera *Camera, INDEX iNode,INT FilterDown,INT Outside);
void dynamicsPreRender	(UCamera *Camera, FSpanBuffer *SpanBuffer, INDEX iNode,INT IsFront);
void dynamicsFinalize	(UCamera *Camera, INT SpanRender);

/*------------------------------------------------------------------------------------
	Bsp & Occlusion.
------------------------------------------------------------------------------------*/

struct FBspDrawList
{
	INT 				iNode,iSurf,iZone,PolyFlags,NumPts,Key;
	FLOAT				MaxZ,MinZ;
	FSpanBuffer			Span;
	FTransform			*Pts;
	UTexture			*Texture;
	FBspDrawList		*Next;
};

/*------------------------------------------------------------------------------------
	Debugging stats.
------------------------------------------------------------------------------------*/

//
// General-purpose statistics:
//
#if STATS
	// Macro to execute an optional statistics-related command.
	#define STAT(cmd) cmd

	// All stats.
	class FRenderStats
	{
	public:

		// Bsp traversal:
		INT NodesDone;			// Nodes traversed during rendering.
		INT NodesTotal;			// Total nodes in Bsp.

		// Occlusion stats
		INT BoxChecks;			// Number of bounding boxes checked.
		INT BoxSkipped;			// Boxes skipped due to nodes being visible on the prev. frame.
		INT BoxBacks;			// Bsp node bounding boxes behind the player.
		INT BoxIn;				// Boxes the player is in.
		INT BoxOutOfPyramid;	// Boxes out of view pyramid.
		INT BoxSpanOccluded;	// Boxes occluded by span buffer.

		// Actor drawing stats:
		INT NumSprites;			// Number of sprites filtered.
		INT NumChunks;			// Number of final chunks filtered.
		INT NumFinalChunks;		// Number of final chunks.
		INT ChunksDrawn;		// Chunks drawn.
		INT MeshesDrawn;		// Meshes drawn.

		// Texture subdivision stats
		INT LatsMade;			// Number of lattices generated.
		INT LatLightsCalc;		// Number of lattice light computations.
		INT DynLightActors;		// Number of actors shining dynamic light.

		// Polygon/rasterization stats
		INT NumSides;			// Number of sides rasterized.
		INT NumSidesCached;		// Number of sides whose setups were cached.
		INT NumRasterPolys;		// Number of polys rasterized.
		INT NumRasterBoxReject;	// Number of polygons whose bounding boxes were span rejected.

		// Span buffer:
		INT SpanTotalChurn;		// Total spans added.
		INT SpanRejig;			// Number of span index that had to be reallocated during merging.

		// Clipping:
		INT ClipAccept;			// Polygons accepted by clipper.
		INT ClipOutcodeReject;	// Polygons outcode-rejected by clipped.
		INT ClipNil;			// Polygons clipped into oblivion.

		// Memory:
		INT GMem;				// Bytes used in global memory pool.
		INT GDynMem;			// Bytes used in dynamics memory pool.

		// Zone rendering:
		INT CurZone;			// Current zone the player is in.
		INT NumZones;			// Total zones in world.
		INT VisibleZones;		// Zones actually processed.
		INT MaskRejectZones;	// Zones that were mask rejected.

		// Illumination cache:
		INT IllumTime;			// Time spent in illumination.
		INT PalTime;			// Time spent in palette regeneration.

		// Pipe:
		INT OcclusionTime;		// Time spent in occlusion code.
		INT InversionTime;		// Matrix inversion time.

		// Lighting:
		INT Lightage,LightMem,MeshPtsGen,MeshesGen;

		// Textures:
		INT UniqueTextures,UniqueTextureMem,CodePatches;

		// Extra:
		INT Extra1,Extra2,Extra3,Extra4;

		// Routine timings:
		INT GetValidRange;
		INT BoxIsVisible;
		INT BoundIsVisible;
		INT CopyFromRasterUpdate;
		INT CopyFromRaster;
		INT CopyIndexFrom;
		INT CopyFromRange;
		INT MergeWith;
		INT MergeFrom;
		INT CalcRectFrom;
		INT CalcLatticeFrom;
		INT Generate;
		INT CalcLattice;
		INT Tmap;
		INT Asm;
		INT TextureMap;
		INT RasterSetup;
		INT RasterSetupCached;
		INT RasterGenerate;
		INT Transform;
		INT Clip;
	};
	extern FRenderStats GStat;
#else
	#define STAT(x) /* Do nothing */
#endif // STATS

/*------------------------------------------------------------------------------------
	Lighting.
------------------------------------------------------------------------------------*/

//
// Class encapsulating the dynamic lighting subsystem.
//
class FLightManagerBase
{
public:
	// Functions.
	virtual void Init()=0;
	virtual void Exit()=0;
	virtual void SetupForActor(UCamera *Camera, AActor *Actor)=0;
	virtual void SetupForSurf(UCamera *Camera, FVector &Normal, FVector &Base, INDEX iSurf, INDEX iLightMesh, DWORD PolyFlags,AZoneInfo *Zone,UTexture *Texture)=0;
	virtual void ApplyLatticeEffects(FTexLattice *Start, FTexLattice *End)=0;
	virtual void ReleaseLightBlock()=0;
	virtual void DoDynamicLighting(ULevel *Level)=0;
	virtual void UndoDynamicLighting(ULevel *Level)=0;

	// Global variables.
	static FMemMark			Mark;
	static FVector			WorldBase,WorldNormal,Base,Normal,TextureU,TextureV;
	static FVector			InverseUAxis,InverseVAxis,InverseNAxis;
	static UCamera			*Camera;
	static ULevel			*Level;
	static FBspSurf			*Surf;
	static BYTE				*ShadowBase;
	static QWORD			MeshAndMask;
	static DWORD			PolyFlags,MaxSize;
	static INT				MeshUSize,MeshVSize,MeshSpace;
	static INT				MeshUByteSize,MeshByteSpace,LatticeEffects,MeshUSkip;
	static INT				MinXBits,MaxXBits,MinYBits,MaxYBits;
	static INT				NumLights;
	static FLOAT			FMeshSpacing,FTextureUStart,FTextureVStart;
	INT						IsDynamic;
	INT						MeshUTile,MeshVTile;
	INT						TextureUStart,TextureVStart,MeshSpacing;
	INDEX					iLightMesh,iSurf;
	BYTE					MeshUBits,MeshVBits,MeshShift;
	INT						MeshTileSpace;
	RAINBOW_PTR				Mesh;
	FLightMeshIndex			*Index;
};

/*-----------------------------------------------------------------------------
	Span texture mapping.
-----------------------------------------------------------------------------*/

//
// Per polygon setup structure.
//warning: Mirrored in UnRender.inc.
//
struct FPolySpanTextureInfoBase
{
	// Pointer to a member function.
	typedef void (FPolySpanTextureInfoBase::*DRAW_SPAN)
	(
		QWORD			Start,
		QWORD			Inc,
		RAINBOW_PTR		Dest,
		INT				Pixels,
		INT				Line
	);

	// Draw function pointer.
	DRAW_SPAN	DrawFunc;

	// Member variables.
	BYTE		UBits;
	BYTE		VBits;
};

//
// Class encapsualting the span texture mapping subsystem.
//
struct FGlobalSpanTextureMapperBase
{
	// Per polygon setup, uses memory on the specified pool.
	virtual FPolySpanTextureInfoBase* SetupForPoly
	(
		UCamera			*Camera,
		UTexture		*ThisTexture,
		AZoneInfo		*Zone,
		DWORD			ThesePolyFlags,
		DWORD			NotPolyFlags
	) = 0;
	virtual void FinishPoly(FPolySpanTextureInfoBase *Info)=0;

	// Instantiator.
	friend FGlobalSpanTextureMapperBase *newFGlobalSpanTextureMapper(FGlobalPlatform &ThisApp);
};
extern FGlobalSpanTextureMapperBase *GSpanTextureMapper;

/*------------------------------------------------------------------------------------
	Dithering.
------------------------------------------------------------------------------------*/

// Dither offsets: Hand-tuned parameters for texture and illumination dithering.
struct FDitherOffsets
{
	INT U[4][2];
	INT V[4][2];
	INT G[4][2];
};

// DitherTable hierarchy: Setup tables for ordered dithering.
struct FDitherPair
{
	QWORD Delta;
	QWORD Offset;
};
struct FDitherUnit
{
	FDitherPair Pair[4][2];
};
struct FDitherSet
{
	FDitherUnit Unit[MAX_MIPS];
};
typedef FDitherSet FDitherTable[12];
extern  FDitherTable GDither256[4],GNoDither256;

void InitDither();

/*------------------------------------------------------------------------------------
	Blit globals.
------------------------------------------------------------------------------------*/
 
//
// Blit globals.
//warning: Mirrored in UnRender.inc.
//
struct UNRENDER_API FGlobalBlit
{
public:
	INT				LatticeX,LatticeY;			// Lattice grid size
	INT				SubX,SubY;					// Sublattice grid size
	INT				InterX,InterY;				// Interlattice size
	INT				LatticeXMask,LatticeXNotMask,LatticeXMask4;
	INT				InterXMask,InterXNotMask;

	BYTE			LatticeXBits,LatticeYBits;	// Lattice shifters, corresponding to LatticeX,LatticeY
	BYTE			SubXBits,SubYBits;			// Sublattice shifters, corresponding to LatticeX,LatticeY
	BYTE			InterXBits,InterYBits;		// LatticeXBits-SubXBits etc

	UTexture		*Texture;
	UPalette		*Palette;
	AZoneInfo		*Zone;
	BYTE			MipRef[MAX_MIPS+1],PrevMipRef[MAX_MIPS+1];

	INT				DrawKind;					// See DRAW_RASTER_ above
	INT				iZone;						// Zone the blitting is occuring in
};
extern "C" extern FGlobalBlit GBlit;

/*------------------------------------------------------------------------------------
	FRender.
------------------------------------------------------------------------------------*/

enum {MAX_XR = 256}; // Maximum subdivision lats horizontally.
enum {MAX_YR = 256}; // Maximum subdivision lats vertically.

// Pure virtual base class of FRender.
class UNRENDER_API FRenderBase
{
public:
	// Init/exit functions.
	virtual void Init()=0;
	virtual void Exit()=0;
	virtual INT Exec(const char *Cmd,FOutputDevice *Out=GApp)=0;

	// Prerender/postrender functions.
	virtual void PreRender(UCamera *Camera)=0;
	virtual void PostRender(UCamera *Camera)=0;

	// Major rendering functions.
	virtual void DrawWorld(UCamera *Camera)=0;
	virtual void DrawActor(UCamera *Camera,AActor *Actor)=0;
	virtual void DrawScaledSprite(UCamera *Camera, UTexture *Texture, FLOAT ScreenX, FLOAT ScreenY, FLOAT XSize, FLOAT YSize, INT BlitType,FSpanBuffer *SpanBuffer,INT Center,INT Highlight,FLOAT Z)=0;
	virtual void DrawTiledTextureBlock(UCamera *Camera,UTexture *Texture,INT X, INT XL, INT Y, INT YL,INT U,INT V)=0;

	// Texture locking/unlocking functions called by the engine only.
	virtual void PostLockTexture(FTextureInfo &TextureInfo, UTexture *Camera)=0;
	virtual void PreUnlockTexture(FTextureInfo &TextureInfo)=0;
	virtual void PostLoadTexture(UTexture *Texture,DWORD PostFlags)=0;
	virtual void PreKillTexture(UTexture *Texture)=0;
};

class UNRENDER_API FEditorRenderBase
{
public:
	// Editor rendering functions.
	virtual void DrawWireBackground(UCamera *Camera)=0;
	virtual void DrawLevelBrushes(UCamera *Camera)=0;
	virtual void DrawMovingBrushWires(UCamera *Camera)=0;
	virtual void DrawActiveBrush(UCamera *Camera)=0;
	virtual void DrawBoundingBox(UCamera *Camera,FBoundingBox *Bound)=0;
	virtual int  Deproject(UCamera *Camera,INT ScreenX,INT ScreenY,FVector *V,INT UseEdScan,FLOAT Radius)=0;
	virtual void InitTransforms(UModel *Model)=0;
	virtual void ExitTransforms()=0;
	virtual FBspDrawList* OccludeBsp(UCamera *Camera, FSpanBuffer *Backdrop)=0;
	virtual void DrawLevelActors(UCamera *Camera, AActor *Exclude)=0;
};

// Renderer globals.
class UNRENDER_API FRender 
:	public FRenderBase,
	public FEditorRenderBase
{
public:
	// Friends.
	friend class FGlobalOccluder;
	friend class FGlobalSpanTextureMapper;
	friend void RenderSubsurface
	(
		UCamera*		Camera,
		AActor*			Owner,
		UTexture*		Texture,
		FSpanBuffer*	Span,
		AZoneInfo*		Zone,
		FTransTexture*	Pts,
		DWORD			PolyFlags,
		INT				SubCount
	);

	// Temporary friends.
	friend void dynamicsLock(UModel *Model);
	friend void dynamicsUnlock(UModel *Model);
	friend FDynamicsIndex *dynamicsAdd(UModel *Model, INDEX iNode, INT Type, FSprite *Sprite, FRasterPoly *Raster, FLOAT Z,INT IsBack );
	friend void dynamicsFilter( UCamera *Camera, INDEX iNode,INT FilterDown,INT Outside );
	friend void dynamicsFinalize( UCamera *Camera, INT SpanRender );
	friend void dynamicsPreRender(UCamera*Camera, FSpanBuffer *SpanBuffer, INDEX iNode, INT IsBack);
	friend void rendDrawAcross(UCamera *Camera,FSpanBuffer *SpanBuffer,FSpanBuffer *RectSpanBuffer, FSpanBuffer *LatticeSpanBuffer,FSpanBuffer *SubRectSpanBuffer, FSpanBuffer *SubLatticeSpanBuffer,int Sampled);
	friend void DrawSoftwareTexturedBspSurf( UCamera* Camera, FBspDrawList* Draw );
	friend void TexSetup( UCamera *Camera );
	friend void rendDrawAcrossSetup(UCamera *Camera, UTexture *Texture, UPalette *Palette, DWORD ThesePolyFlags, DWORD NotPolyFlags);

	// Constants.
	enum EDrawRaster
	{
		DRAWRASTER_Flat				= 0,	// Flat shaded
		DRAWRASTER_Normal			= 1,	// Normal texture mapped
		DRAWRASTER_Masked			= 2,	// Masked texture mapped
		DRAWRASTER_Blended			= 3,	// Blended texture mapped
		DRAWRASTER_Fire				= 4,	// Fire table texture mapped
		DRAWRASTER_MAX				= 5,	// First invalid entry
	};

	// FRenderBase interface.
	void Init();
	void Exit();
	int Exec(const char *Cmd,FOutputDevice *Out=GApp);
	void PreRender(UCamera *Camera);
	void PostRender(UCamera *Camera);
	void DrawWorld(UCamera *Camera);
	void DrawActor(UCamera *Camera,AActor *Actor);
	void DrawScaledSprite(UCamera *Camera, UTexture *Texture, FLOAT ScreenX, FLOAT ScreenY, FLOAT XSize, FLOAT YSize, INT BlitType,FSpanBuffer *SpanBuffer,INT Center,INT Highlight,FLOAT Z);
	void DrawTiledTextureBlock(UCamera *Camera,UTexture *Texture,INT X, INT XL, INT Y, INT YL,INT U,INT V);
	void PostLockTexture(FTextureInfo &TextureInfo, UTexture *Camera);
	void PreUnlockTexture(FTextureInfo &TextureInfo);
	void PostLoadTexture(UTexture *Texture,DWORD PostFlags);
	void PreKillTexture(UTexture *Texture);

	// FEditorRenderBase interface.
	void DrawWireBackground(UCamera *Camera);
	void DrawLevelBrushes(UCamera *Camera);
	void DrawMovingBrushWires(UCamera *Camera);
	void DrawActiveBrush(UCamera *Camera);
	void DrawBoundingBox(UCamera *Camera,FBoundingBox *Bound);
	int	Deproject(UCamera *Camera,INT ScreenX,INT ScreenY,FVector *V,INT UseEdScan,FLOAT Radius);
	void InitTransforms(UModel *Model);
	void ExitTransforms();
	FBspDrawList* OccludeBsp(UCamera *Camera, FSpanBuffer *Backdrop);
	void DrawLevelActors(UCamera *Camera, AActor *Exclude);

	// Public.
	int GetTemporalIter() {return TemporalIter;}

private:
	// Point cache.
	enum {MAX_POINTS = 98304};
	static struct FStampedPoint
	{
		FTransform	*Point;
		DWORD		Stamp;
	} *PointCache;

	// Vector cache.
	enum {MAX_VECTORS = 32768};
	static struct FStampedVector
	{
		FVector		*Vector;
		DWORD		Stamp;
	} *VectorCache;

	// The current timestamp.
	static DWORD	Stamp;

	// Memory stacks.
	static FMemStack PointMem;
	static FMemStack VectorMem;

	// Memory marks.
	static FMemMark PointMark;
	static FMemMark VectorMark;

	// Rendering statistics.
	static class FRenderStats *Stat;

	// Texture lattice pointers.
	static FTexLattice *LatticePtr[MAX_YR][MAX_XR];

	// Variables.
	FGlobalBlit		*GBlitPtr;
	INT 			DynamicsLocked;
	INDEX			NumPostDynamics;
	FBspNode		**PostDynamics;

	INT 			RendIter,TemporalIter,ShowLattice,Antialias;
	INT 			DoDither,ShowChunks,Temporal,Curvy;
	INT 			Toggle,Extra1,Extra2,Extra3,Extra4,LeakCheck;
	INT 			QuickStats,AllStats;
	INT				Pad[6];
	FLOAT			MipMultiplier;

	// Timing.
	DWORD			LastEndTime;	// Time when last sample was ended.
	DWORD			ThisStartTime;	// Time when this sample was started.
	DWORD			ThisEndTime;	// Time when this sample was ended.
	DWORD			NodesDraw;		// Bsp nodes drawn.
	DWORD			PolysDraw;		// Polys drawn.

	// Return the transformed value of a point.
	static inline FTransform &GetPoint( const UCamera *Camera, INDEX pPoint )
	{
		FStampedPoint &S = PointCache[pPoint];
		if( S.Stamp != Stamp )
		{
			S.Stamp = Stamp;
			FTransform *T = S.Point = new(PointMem)FTransform;
			
			const FVector& P = Camera->Level->Model->Points(pPoint);
			T->X = Camera->Uncoords.Origin.X + P.X * Camera->Coords.XAxis.X + P.Y * Camera->Coords.XAxis.Y + P.Z * Camera->Coords.XAxis.Z;
			T->Y = Camera->Uncoords.Origin.Y + P.X * Camera->Coords.YAxis.X + P.Y * Camera->Coords.YAxis.Y + P.Z * Camera->Coords.YAxis.Z;
			T->Z = Camera->Uncoords.Origin.Z + P.X * Camera->Coords.ZAxis.X + P.Y * Camera->Coords.ZAxis.Y + P.Z * Camera->Coords.ZAxis.Z;

			T->iTransform	= pPoint;
			T->ScreenX		= -1.f;
			T->ComputeOutcode( Camera );
		}
		return *S.Point;
	}

	// Return the transformed value of a vector.
	static inline FVector &GetVector( const UModel *Model, const FCoords &Coords, INDEX vVector )
	{
		FStampedVector &S = VectorCache[vVector];
		if( S.Stamp != Stamp )
		{
			S.Stamp = Stamp;
			FVector *T = S.Vector = new(VectorMem)FVector;

			const FVector &V = Model->Vectors(vVector);
			T->X = V.X * Coords.XAxis.X + V.Y * Coords.XAxis.Y + V.Z * Coords.XAxis.Z;
			T->Y = V.X * Coords.YAxis.X + V.Y * Coords.YAxis.Y + V.Z * Coords.YAxis.Z;
			T->Z = V.X * Coords.ZAxis.X + V.Y * Coords.ZAxis.Y + V.Z * Coords.ZAxis.Z;
		}
		return *S.Vector;
	}

	// Functions.
	void DrawBspSurf 		(UCamera *Camera, FBspDrawList *Draw);
	void DrawHighlight		(UCamera *Camera, FSpanBuffer *SpanBuffer, BYTE Color);

	void AntialiasEdge		(UCamera *Camera, FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2);
	
	void DrawFlatPoly		(UCamera *Camera,FSpanBuffer *SpanBuffer, BYTE Color);

	void DrawLatticeGrid	(UCamera *Camera,FSpanBuffer *LatticeSpan);
	void CleanupLattice		(FSpanBuffer &LatticeSpan);

	void DrawRasterOutline  (UCamera *Camera,class FRasterSetup *Raster, BYTE Color);
	void DrawRasterSide		(BYTE *Line,INT SXR,class FRasterSideSetup *Side,BYTE Color);

	void DrawBackdrop		(UCamera *Camera, FSpanBuffer *Backdrop);
	void DrawBrushPolys		(UCamera *Camera, UModel *Model, INT WireColor, INT Dotted, FConstraints *Constraints, INT DrawPivot, INT DrawVertices, INT DrawSelected, INT DoScanWire);
	void DrawLine			(UCamera *Camera, BYTE Color,INT Dotted,FLOAT X1, FLOAT Y1, FLOAT G1, FLOAT X2, FLOAT Y2, FLOAT G2 );
	void DrawDepthLine		(UCamera *Camera, BYTE Color,INT Dotted,FLOAT X1, FLOAT Y1, FLOAT RZ1, FLOAT X2, FLOAT Y2, FLOAT RZ2);
	void DrawFPoly 			(UCamera *Camera, FPoly *Poly, INT WireColor, INT FillColor,INT Dotted);
	void DrawRect 			(UCamera *Camera, BYTE Color, INT X1, INT Y1, INT X2, INT Y2);
	void Draw3DLine 		(UCamera *Camera, const FVector &OrigP, const FVector &OrigQ, INT MustTransform, INT Color,INT DepthShade,INT Dotted);
	void DrawCircle			(UCamera *Camera, FVector &Location, FLOAT Radius, INT Color, INT Dotted);
	void DrawBox			(UCamera *Camera, const FVector &Min, const FVector &Max, INT Color, INT Dotted);

	INT  Project			(UCamera *Camera, FVector *V, FLOAT *ScreenX, FLOAT *ScreenY, FLOAT *Scale);
	INT  BoundVisible 		(UCamera *Camera, FBoundingBox *Bound, FSpanBuffer *SpanBuffer, FScreenBounds *Results);
	INT  OrthoClip			(UCamera *Camera, const FVector &P1, const FVector &P2, FLOAT *ScreenX1, FLOAT *ScreenY1, FLOAT *ScreenX2, FLOAT *ScreenY2);
	void DrawOrthoLine		(UCamera *Camera, const FVector &P1, const FVector &P2, INT Color, INT Dotted, FLOAT Brightness);

	INT inline TransformBspSurf(UModel *Model,UCamera *Camera, INDEX iNode, FTransform **Pts, BYTE &AllCodes);
	INT ClipBspSurf (UModel *Model, UCamera *Camera, INDEX iNode, FTransform *OutPts);
	INT ClipTexPoints (UCamera *Camera, FTransTexture *InPts, FTransTexture *OutPts, INT Num0);

	RAINBOW_PTR GetPaletteLightingTable(UTexture *Camera,UPalette *Palette,AZoneInfo *Zone,
		ALevelInfo *LevelInfo,FCacheItem *&CacheItem);

	INT 	SetupSprite     (UCamera *Camera, FSprite *Sprite);
	void	DrawActorSprite (UCamera *Camera, FSprite *Sprite);
	void	DrawActorChunk  (UCamera *Camera, FSprite *Sprite);
	void	DrawMesh		(UCamera *Camera, AActor *Owner,FSpanBuffer *SpanBuffer,FSprite *Sprite);

	void ShowStat (UCamera *Camera,INT *StatYL,const char *Str);
	void DrawStats(UCamera *Camera);
	void DrawGridSection (UCamera *Camera, INT CameraLocX,
		INT CameraSXR, INT CameraGridY, FVector *A, FVector *B,
		FLOAT *AX, FLOAT *BX,INT AlphaCase);
};

extern FRender GRender;
extern UNRENDER_API FLightManagerBase *GLightManager;

/*------------------------------------------------------------------------------------
	Random numbers.
------------------------------------------------------------------------------------*/

// Random number subsystem.
// Tracks a list of set random numbers.
class FGlobalRandomsBase
{
public:
	// Functions.
	virtual void Init()=0; // Initialize subsystem.
	virtual void Exit()=0; // Shut down subsystem.
	virtual void Tick(FLOAT TimeSeconds)=0; // Mark one unit of passing time.

	// Inlines.
	FLOAT RandomBase( int i ) {return RandomBases[i & RAND_MASK]; }
	FLOAT Random(     int i ) {return Randoms    [i & RAND_MASK]; }

protected:
	// Constants.
	enum {RAND_CYCLE = 16       }; // Number of ticks for a complete cycle of Randoms.
	enum {N_RANDS    = 256      }; // Number of random numbers tracked, guaranteed power of two.
	enum {RAND_MASK  = N_RANDS-1}; // Mask so that (i&RAND_MASK) is a valid index into Randoms.

	// Variables.
	static FLOAT RandomBases	[N_RANDS]; // Per-tick discontinuous random numbers.
	static FLOAT Randoms		[N_RANDS]; // Per-tick continuous random numbers.
};
extern FGlobalRandomsBase *GRandoms;

/*-----------------------------------------------------------------------------
	Active edge occluder.
-----------------------------------------------------------------------------*/

#if 0
// Active edge rendering base class.
class FGlobalOccluderBase
{
public:
	// Interface.
	virtual void Init(FGlobalPlatform *App,FMemStack *GMem,FMemStack *GDynMem)=0;
	virtual void Exit()=0;
	virtual FBspDrawList *OccludeBsp(UCamera *Camera,FSpanBuffer *Backdrop)=0;
};
extern FGlobalOccluderBase *GEdgeOccluder;
#endif

/*------------------------------------------------------------------------------------
	New DrawAcross code.
------------------------------------------------------------------------------------*/

void rendDrawAcrossSetup(UCamera *Camera, UTexture *ThisTexture, UPalette *Palette, DWORD PolyFlags, DWORD NotPolyFlags);
void rendDrawAcross (UCamera *Camera,FSpanBuffer *SpanBuffer,
	FSpanBuffer *RectSpanBuffer, FSpanBuffer *LatticeSpanBuffer,
	FSpanBuffer *SubRectSpanBuffer, FSpanBuffer *SubLatticeSpanBuffer,
	int Sampled);
void rendDrawAcrossExit();

/*------------------------------------------------------------------------------------
	Fast approximate math code.
------------------------------------------------------------------------------------*/

#define APPROX_MAN_BITS 10		/* Number of bits of approximate square root mantissa, <=23 */
#define APPROX_EXP_BITS 9		/* Number of bits in IEEE exponent */
#define APPROX_ATANS    1024	/* Number of entries in approximate arctan table */

extern FLOAT SqrtManTbl[2<<APPROX_MAN_BITS];
extern FLOAT DivSqrtManTbl[1<<APPROX_MAN_BITS],DivManTbl[1<<APPROX_MAN_BITS];
extern FLOAT DivSqrtExpTbl[1<<APPROX_EXP_BITS],DivExpTbl[1<<APPROX_EXP_BITS];
extern FLOAT AtanTbl[APPROX_ATANS];

//
// Macro to look up from a power table.
//
#define POWER_ASM(ManTbl,ExpTbl)\
	__asm\
	{\
		/* Here we use the identity sqrt(a*b) = sqrt(a)*sqrt(b) to perform\
		** an approximate floating point square root by using a lookup table\
		** for the mantissa (a) and the exponent (b), taking advantage of the\
		** ieee floating point format.\
		*/\
		__asm mov  eax,[F]									/* get float as int                   */\
		__asm shr  eax,(32-APPROX_EXP_BITS)-APPROX_MAN_BITS	/* want APPROX_MAN_BITS mantissa bits */\
		__asm mov  ebx,[F]									/* get float as int                   */\
		__asm shr  ebx,32-APPROX_EXP_BITS					/* want APPROX_EXP_BITS exponent bits */\
		__asm and  eax,(1<<APPROX_MAN_BITS)-1				/* keep lowest 9 mantissa bits        */\
		__asm fld  DWORD PTR ManTbl[eax*4]					/* get mantissa lookup                */\
		__asm fmul DWORD PTR ExpTbl[ebx*4]					/* multiply by exponent lookup        */\
		__asm fstp [F]										/* store result                       */\
	}\
	return F;

//
// Fast floating point power routines.
// Pretty accurate to the first 10 bits.
// About 12 cycles on the Pentium.
//
inline FLOAT DivSqrtApprox(FLOAT F) {POWER_ASM(DivSqrtManTbl,DivSqrtExpTbl);}
inline FLOAT DivApprox    (FLOAT F) {POWER_ASM(DivManTbl,    DivExpTbl    );}
inline FLOAT SqrtApprox   (FLOAT F)
{
	__asm
	{
		mov  eax,[F]                        // get float as int.
		shr  eax,(23 - APPROX_MAN_BITS) - 2 // shift away unused low mantissa.
		mov  ebx,[F]						// get float as int.
		and  eax, ((1 << (APPROX_MAN_BITS+1) )-1) << 2 // 2 to avoid "[eax*4]".
		and  ebx, 0x7F000000				// 7 bit exp., wipe low bit+sign.
		shr  ebx, 1							// exponent/2.
		mov  eax,DWORD PTR SqrtManTbl [eax]	// index hi bit is exp. low bit.
		add  eax,ebx						// recombine with exponent.
		mov  [F],eax						// store.
	}
	return F;								// compiles to fld [F].
}

/*------------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------------*/
#endif // _INC_UNRENDER
