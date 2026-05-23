/*=============================================================================
	UnRender.h: Rendering functions and structures

	Copyright 1995 Epic MegaGames, Inc.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNRENDER // Prevent header from being included multiple times
#define _INC_UNRENDER

/*------------------------------------------------------------------------------------
	Other includes.
------------------------------------------------------------------------------------*/

#ifndef _INC_UNSPAN
#include "UnSpan.h"
#endif

/*------------------------------------------------------------------------------------
	Types for rendering engine.
------------------------------------------------------------------------------------*/

// Workaround for Visual C++ internal compiler error when
// accessing the class from an inline member function.
struct FTemp
{
	FLOAT X;
	FLOAT Y;
	FLOAT Z;
	WORD  iTransform;
	BYTE  Flags;
	BYTE  Align;

	FLOAT ClipXM;
	FLOAT ClipXP;
	FLOAT ClipYM;
	FLOAT ClipYP;

	INT	  IntX;
	INT	  IntY;
	DWORD iSide;
	DWORD Pad;

	FLOAT ScreenX;
	FLOAT ScreenY;
	FLOAT RZ;
	DWORD NewIntY;
};
static const BYTE OutXMinTab [2] = {0,FVF_OutXMin };
static const BYTE OutXMaxTab [2] = {0,FVF_OutXMax };
static const BYTE OutYMinTab [2] = {0,FVF_OutYMin };
static const BYTE OutYMaxTab [2] = {0,FVF_OutYMax };

//
// Point/vector transformation cache.
//
class FTransform : public FVector
{
public:
	FLOAT ClipXM;
	FLOAT ClipXP;
	FLOAT ClipYM;
	FLOAT ClipYP;

	INT	  IntX;
	INT	  IntY;
	DWORD iSide;
	DWORD Pad;

	FLOAT ScreenX;
	FLOAT ScreenY;
	FLOAT RZ;
	INT   NewIntY;

	// Compute the outcode and clipping numbers of an FTransform.
	void inline ComputeOutcode(const ICamera &Camera)
	{
	#ifdef ASM
		__asm
		{
			;
			; 28 cycle clipping number and outcode computation.
			; Set up:
			;
			;1
			mov  ecx,[this]					; Get this pointer
			mov  esi,[Camera]				; Get camera pointer
			;
			; Compute clipping numbers:
			;18
			fld  [ecx]FTemp.X				; X
			fld  [ecx]FTemp.Y				; Y X
			fxch							; X Y
			fmul [esi]ICamera.ProjZRSX2		; X*Camera.ProjZRSX2 Y
			fxch							; Y X*Camera.ProjZRSX2
			fmul [esi]ICamera.ProjZRSY2		; Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fld  [ecx]FTemp.Z				; Z Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fld  [ecx]FTemp.Z				; Z Z Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fxch							; Z Z Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fadd st,st(3)					; Z+X*Camera.ProjZRSX2 Z Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fxch							; Z Z+X*Camera.ProjZRSX2 Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fadd st,st(2)					; Z+Y*Camera.ProjZRSY2 Z+X*Camera.ProjZRSX2 Y*Camera.ProjZRSY2 X*Camera.ProjZRSX2
			fxch st(3)						; X*Camera.ProjZRSX2 Z+X*Camera.ProjZRSX2 Y*Camera.ProjZRSY2 Z+Y*Camera.ProjZRSY2
			fsubr [ecx]FTemp.Z				; Z-X*Camera.ProjZRSX2 Z+X*Camera.ProjZRSX2 Y*Camera.ProjZRSY2 Z+Y*Camera.ProjZRSY2
			fxch st(2)						; Y*Camera.ProjZRSY2 Z+X*Camera.ProjZRSX2 Z-X*Camera.ProjZRSX2 Z+Y*Camera.ProjZRSY2
			fsubr [ecx]FTemp.Z				; Z-Y*Camera.ProjZRSY2 Z+X*Camera.ProjZRSX2 Z-X*Camera.ProjZRSX2 Z+Y*Camera.ProjZRSY2
			fxch							; Z+X*Camera.ProjZRSX2 Z-Y*Camera.ProjZRSY2 Z-X*Camera.ProjZRSX2 Z+Y*Camera.ProjZRSY2
			fstp [ecx]FTemp.ClipXM			; Z-Y*Camera.ProjZRSY2 Z-X*Camera.ProjZRSX2 Z+Y*Camera.ProjZRSY2
			fxch st(2)						; Z+Y*Camera.ProjZRSY2 Z-X*Camera.ProjZRSX2 Z-Y*Camera.ProjZRSY2 
			fstp [ecx]FTemp.ClipYM			; Z-X*Camera.ProjZRSX2 Z-Y*Camera.ProjZRSY2 
			fstp [ecx]FTemp.ClipXP			; Z-Y*Camera.ProjZRSY2 
			fstp [ecx]FTemp.ClipYP			; (empty fp stack)
			;
			; Compute flags:
			;9
			mov  ebx,[ecx]FTemp.ClipXM		; ebx = XM clipping number as integer
			mov  edx,[ecx]FTemp.ClipYM		; edx = YM clipping number as integer
			;
			shr  ebx,31						; ebx = XM: 0 iff clip>=0.0, 1 iff clip<0.0
			mov  edi,[ecx]FTemp.ClipXP		; edi = XP
			;
			shr  edx,31                     ; edx = YM: 0 or 1
			mov  esi,[ecx]FTemp.ClipYP		; esi = YP: 0 or 1
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
			mov  [ecx]FTemp.Flags,al		; Store flags
		}
	#else
		ClipXM = Z + X * Camera.ProjZRSX2;
		ClipXP = Z - X * Camera.ProjZRSX2;
		//
		ClipYM = Z + Y * Camera.ProjZRSY2;
		ClipYP = Z - Y * Camera.ProjZRSY2;
		//
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
// Point plus texture transformation.  First entries must be identical to
// FTransform because FTransTex records are often typecast to FTransform's.
//
class FTransTex : public FTransform // Transform plus texture coordinates
{
public:
	FLOAT		U;  // Texture U value at point * 65536.0.
	FLOAT		V;  // Texture V value at point * 65536.0.
	FLOAT		G;  // Gouraud value at point   * 65536.0.

	FTransTex inline operator+ (const FTransTex &T) const
	{
		FTransTex Temp;
		Temp.X = X + T.X; Temp.Y = Y + T.Y; Temp.Z = Z + T.Z;
		Temp.U = U + T.U; Temp.V = V + T.V; Temp.G = G + T.G;
		return Temp;
	}
	FTransTex inline operator- (const FTransTex &T) const
	{
		FTransTex Temp;
		Temp.X = X - T.X; Temp.Y = Y - T.Y; Temp.Z = Z - T.Z;
		Temp.U = U - T.U; Temp.V = V - T.V; Temp.G = G - T.G;
		return Temp;
	}
	FTransTex inline operator* (FLOAT Scale) const
	{
		FTransTex Temp;
		Temp.X = X * Scale; Temp.Y = Y * Scale; Temp.Z = Z * Scale;
		Temp.U = U * Scale; Temp.V = V * Scale; Temp.G = G * Scale;
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
			FLOAT	PointSize,Pad1,Pad2,Pad3;
			INT		U,V,G;
			FLOAT	FloatG;
			FVector Loc;
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
	AZoneDescriptor	*ZoneDescriptor;	// Zone it's in or NULL.
	FSpanBuffer		*SpanBuffer;		// Span buffer we're building.
	FSprite			*Next;				// Next sprite in link.
	INT				X1,Y1;				// Top left corner on screen.
	INT				X2,Y2;				// Bottom right corner, X2 must be >= X1, Y2 >= Y1.
	INT				iFrame;				// Frame number to draw.
	FLOAT 			ScreenX,ScreenY;	// Fractionally-accurate screen location of center.
	FLOAT			Scale;				// Scaling value, 1.0 = 1 texel to 1 world unit.
	FLOAT			Z;					// Screenspace Z location.
	FTransform		Verts[4];			// Vertices of projection plane for filtering.
};

/*------------------------------------------------------------------------------------
	Dynamic Bsp contents.
------------------------------------------------------------------------------------*/

class FDynamicsIndex
{
public:
	INT 				Type;		// Type of contents.
	FSprite				*Sprite;	// Pointer to dynamic data, NULL=none.
	class FRasterPoly	*Raster;	// Rasterization this is clipped to, NULL=none.
	FLOAT				Z;			// For sorting.
	INDEX				iNode;		// Node the contents correspond to.
	INDEX				iNext;		// Index of next dynamic contents in list or INDEX_NONE.
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

void dynamicsLock 		(IModel *ModelInfo);
void dynamicsUnlock		(IModel *ModelInfo);
void dynamicsSetup		(ICamera *Camera, INDEX iExclude);
void dynamicsFilter		(ICamera *Camera, INDEX iNode,INT FilterDown,INT Outside);
void dynamicsPreRender	(ICamera *Camera, FSpanBuffer *SpanBuffer, INDEX iNode,INT IsFront);
void dynamicsFinalize	(ICamera *Camera, INT SpanRender);

/*------------------------------------------------------------------------------------
	Bsp & Occlusion.
------------------------------------------------------------------------------------*/

struct FBspDrawList
{
	INT 				iNode,iSurf,iZone,PolyFlags,NumPts;
	FLOAT				MaxZ,MinZ;
	FSpanBuffer			Span;
	FTransform			*Pts;
	UTexture			*Texture;
	FBspDrawList		*Next;
};
struct FBspDrawListPtr
{
	public:
	FBspDrawList *Ptr;

	// Comparison function for grouping by texture.
	friend inline INT Compare(const FBspDrawListPtr &A, const FBspDrawListPtr &B)
	{
		return (int)A.Ptr->Texture - (int)B.Ptr->Texture;
	}
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
		INT NodesBehind;		// Nodes totally behind viewer.
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
		INT MeshMapsDrawn;		// Mesh maps drawn.

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
class FGlobalLightManagerBase
{
public:
	// Functions:
	virtual void Init()=0;
	virtual void Exit()=0;
	virtual void SetupForActor(ICamera *Camera, INDEX iActor)=0;
	virtual void SetupForSurf(ICamera *Camera, FVector &Normal, FVector &Base, INDEX iSurf, INDEX iLightMesh, DWORD PolyFlags,int iZone)=0;
	virtual void ApplyLatticeEffects(FTexLattice *Start, FTexLattice *End)=0;
	virtual void ReleaseLightBlock()=0;
	virtual void DoDynamicLighting(ILevel *Level)=0;
	virtual void UndoDynamicLighting(ILevel *Level)=0;

	// Global variables:
	static FVector			WorldBase,WorldNormal,Base,Normal,TextureU,TextureV;
	static FVector			InverseUAxis,InverseVAxis,InverseNAxis;
	static ICamera			*Camera;
	static ILevel			*Level;
	static IModel			*ModelInfo;
	static FBspSurf			*Surf;
	static BYTE				*ShadowBase;
	static VOID				*MemTop;
	static VOID				*MeshVoid;
	static QWORD			MeshAndMask;
	static DWORD			PolyFlags,MaxSize;
	static INT				MeshUSize,MeshVSize,MeshSpace;
	static INT				MeshUTile,MeshVTile,MeshTileSpace;
	static INT				MeshUByteSize,MeshByteSpace,MeshSkipSize,LatticeEffects;
	static INT				MinXBits,MaxXBits,MinYBits,MaxYBits;
	static INT				TextureUStart,TextureVStart,MeshSpacing;
	static INT				NumLights;
	static INDEX			iLightMesh,iSurf;
	static BYTE				MeshUBits,MeshVBits,MeshShift;

	// Inlines:
	inline static FLOAT *&GetMeshFloat() { return (FLOAT *&)MeshVoid; };
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
		ICamera			&Camera,
		UTexture		*ThisTexture,
		AZoneDescriptor	*ZoneDescriptor,
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

//
// Dither offsets: Hand-tuned parameters for texture and illumination dithering.
//
class FDitherOffsets
{
	public:
	INT U[4][2];
	INT V[4][2];
	INT G[4][2];
};

class FDitherPair
{
	public:
	QWORD Delta;
	QWORD Offset;
};

//
// Dither table: Table generated by BuildDitherTable for 256-color
// texture mapper, including dither parameters for all allowable
// texture sizes.
//
class FDitherSet
{
public:
	FDitherPair Pair[8][4][2];
};
typedef FDitherSet FDitherTable[12];
extern  FDitherTable GDither256,GBlur256,GNoDither256;

void InitDither();

/*------------------------------------------------------------------------------------
	Blit globals.
------------------------------------------------------------------------------------*/
 
class UNRENDER_API FBlitMipInfo
{
public:
	DWORD		VMask;
	DWORD		AndMask;
	BYTE		*Data;
	FDitherSet	*Dither;

	BYTE		MipLevel;
	BYTE		UBits;
	BYTE		VBits;
	BYTE		Pad;
};

//
// Blit globals.
//warning: Mirrored in UnRender.inc.
//
class FGlobalBlit
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

	BYTE			UBits,VBits;
	UTexture		*Texture;
	UPalette		*Palette;
	FDitherTable	*DitherBase;
	AZoneDescriptor	*ZoneDescriptor;
	FBlitMipInfo	Mips[8];
	BYTE			MipRef[8+1],PrevMipRef[8+1];

	INT				DrawKind;					// See DRAW_RASTER_ above
	INT				iZone;						// Zone the blitting is occuring in
};
extern "C" extern FGlobalBlit GBlit;

/*------------------------------------------------------------------------------------
	FGlobalRender.
------------------------------------------------------------------------------------*/

enum {MAX_XR = 256}; // Maximum subdivision lats horizontally.
enum {MAX_YR = 256}; // Maximum subdivision lats vertically.

//
// Renderer globals:
//
class UNRENDER_API FGlobalRender
{
public:
	virtual void	Init();
	virtual void	Exit();
	virtual INT 	Exec(const char *Cmd,FOutputDevice *Out=GApp);

	virtual void	PreRender(ICamera *Camera);
	virtual void	PostRender(ICamera *Camera);

	virtual void	DrawWorld(ICamera *Camera);
	virtual void	DrawActor(ICamera *Camera,INDEX iActor);
	virtual void	DrawScaledSprite(ICamera *Camera, UTexture *Texture, FLOAT ScreenX, FLOAT ScreenY, FLOAT XSize, FLOAT YSize, INT BlitType,FSpanBuffer *SpanBuffer,INT Center,INT Highlight);
	virtual void	DrawTiledTextureBlock(ICamera *Camera,UTexture *Texture,INT X, INT XL, INT Y, INT YL,INT U,INT V);

	// Statics.
	static INT 				MaxTransforms;

	static FVector			**VectorTransformFirst;
	static FVector			**VectorTransformLast;
	static FTransform		**PointTransformFirst;
	static FTransform		**PointTransformLast;

	static FTransform		*PointTransform;
	static FVector			*VectorTransform;

	static FTexLattice		*LatticePtr[MAX_YR][MAX_XR];

	static class FRenderStats *Stat;

	// Variables.
	INDEX			NumDynamics;
	INT 			DynamicsLocked;
	INDEX			MaxDynamics;
	INDEX			NumPostDynamics;
	INDEX			MaxPostDynamics;
	FDynamicsIndex	*DynamicsIndex;
	INDEX			*PostDynamics;

	INT 			RendIter,ShowLattice;
	INT 			DoDither,ShowChunks,Unused2;
	INT 			Toggle,Extra1,Extra2,Extra3,Extra4,LeakCheck;
	INT 			QuickStats,AllStats;

	// Timing.
	DWORD			LastEndTime;	// Time when last sample was ended.
	DWORD			ThisStartTime;	// Time when this sample was started.
	DWORD			ThisEndTime;	// Time when this sample was ended.
	DWORD			NodesDraw;		// Bsp nodes drawn.
	DWORD			PolysDraw;		// Polys drawn.

	enum EDrawRaster
	{
		DRAWRASTER_Flat				= 0,	// Flat shaded
		DRAWRASTER_Normal			= 1,	// Normal texture mapped
		DRAWRASTER_Masked			= 2,	// Masked texture mapped
		DRAWRASTER_Blended			= 3,	// Blended texture mapped
		DRAWRASTER_Fire				= 4,	// Fire table texture mapped
		DRAWRASTER_MAX				= 5,	// First invalid entry
	};

	void InitTransforms		(IModel *ModelInfo);
	void ExitTransforms		();

	//
	// Return the transformed and possibly projected value of a point.
	//
	static inline FTransform &GetPoint(const ICamera *Camera, INDEX pPoint)
	{
		FTransform *T = &PointTransform[pPoint];
		if ( T->Align == FVA_Uncached )
		{
			*PointTransformLast++	= T;
			const FVector& P		= Camera->Level.ModelInfo.FPoints[pPoint];

			T->X = Camera->Uncoords.Origin.X + P.X * Camera->Coords.XAxis.X + P.Y * Camera->Coords.XAxis.Y + P.Z * Camera->Coords.XAxis.Z;
			T->Y = Camera->Uncoords.Origin.Y + P.X * Camera->Coords.YAxis.X + P.Y * Camera->Coords.YAxis.Y + P.Z * Camera->Coords.YAxis.Z;
			T->Z = Camera->Uncoords.Origin.Z + P.X * Camera->Coords.ZAxis.X + P.Y * Camera->Coords.ZAxis.Y + P.Z * Camera->Coords.ZAxis.Z;

			T->iTransform	= pPoint;
			T->Align		= 0;
			T->IntX			= MAXINT;

			T->ComputeOutcode(*Camera);
		}
		return *T;
	}

	//
	// Return the transformed value of a vector.
	//
	static inline FVector &GetVector(const IModel *ModelInfo, const FCoords &Coords, INDEX vVector)
	{
		FVector &T = VectorTransform[vVector];
		if (T.Align == FVA_Uncached)
		{
			*VectorTransformLast++	= &T;
			const FVector &V		= ModelInfo->FVectors[vVector];

			T.X = V.X * Coords.XAxis.X + V.Y * Coords.XAxis.Y + V.Z * Coords.XAxis.Z;
			T.Y = V.X * Coords.YAxis.X + V.Y * Coords.YAxis.Y + V.Z * Coords.YAxis.Z;
			T.Z = V.X * Coords.ZAxis.X + V.Y * Coords.ZAxis.Y + V.Z * Coords.ZAxis.Z;

			T.Align = 0;
		}
		return T;
	}

	//
	// Functions.
	//
	FBspDrawList *OccludeBsp(ICamera *Camera, FSpanBuffer *Backdrop);

	void DrawBspSurf 		(ICamera *Camera, FBspDrawList *Draw);
	void DrawHighlight		(ICamera *Camera, FSpanBuffer *SpanBuffer, BYTE Color);

	void DrawFlatPoly		(ICamera *Camera,FSpanBuffer *SpanBuffer, BYTE Color);

	void DrawLatticeGrid	(ICamera *Camera,FSpanBuffer *LatticeSpan);
	void CleanupLattice		(FSpanBuffer &LatticeSpan);

	void DrawRasterOutline  (ICamera *Camera,class FRasterSetup *Raster, BYTE Color);
	void DrawRasterSide		(BYTE *Line,INT SXR,class FRasterSideSetup *Side,BYTE Color);

	void DrawBackdrop		(ICamera *Camera, FSpanBuffer *Backdrop);
	void DrawLevelActors	(ICamera *Camera, INDEX iExclude);
	void DrawMovingBrushWires(ICamera *Camera);
	void DrawLevelBrushes	(ICamera *Camera);
	void DrawActiveBrush	(ICamera *Camera);
	void DrawWireBackground	(ICamera *Camera);
	void DrawBrushPolys		(ICamera *Camera, UModel *Model, INT WireColor, INT Dotted, FConstraints *Constraints, INT DrawPivot, INT DrawVertices, INT DrawSelected, INT DoScan);
	void DrawLine			(ICamera *Camera, BYTE Color,INT Dotted,FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2);
	void DrawDepthLine		(ICamera *Camera, BYTE Color,INT Dotted,FLOAT X1, FLOAT Y1, FLOAT RZ1, FLOAT X2, FLOAT Y2, FLOAT RZ2);
	void DrawFPoly 			(ICamera *Camera, FPoly *Poly, INT WireColor, INT FillColor,INT Dotted);
	void DrawRect 			(ICamera *Camera, BYTE Color, INT X1, INT Y1, INT X2, INT Y2);
	void Draw3DLine 		(ICamera *Camera, const FVector *OrigP, const FVector *OrigQ, INT MustTransform, INT Color,INT DepthShade,INT Dotted);
	void DrawBoundingRect	(ICamera *Camera,FBoundingRect *Bound);
	void DrawCircle			(ICamera *Camera, FVector &Location, INT Radius, INT Color, INT Dotted);

	INT  Deproject			(ICamera *Camera,INT ScreenX,INT ScreenY,FVector *V,INT UseEdScan,FLOAT Radius);
	INT  Project			(ICamera *Camera, FVector *V, FLOAT *ScreenX, FLOAT *ScreenY, FLOAT *Scale);
	INT  BoundVisible 		(ICamera *Camera, FBoundingRect *Bound, FSpanBuffer *SpanBuffer, FScreenBounds *Results,FCoords *Rotation);
	INT  OrthoClip			(ICamera *Camera, const FVector *P1, const FVector *P2, FLOAT *ScreenX1, FLOAT *ScreenY1, FLOAT *ScreenX2, FLOAT *ScreenY2);
	void DrawOrthoLine		(ICamera *Camera, const FVector *P1, const FVector *P2,INT Color,INT Dotted);

	INT inline TransformBspSurf(IModel *ModelInfo,ICamera *Camera, INDEX iNode, FTransform **Pts, BYTE &AllCodes);
	INT ClipBspSurf (IModel *ModelInfo, ICamera *Camera, INDEX iNode, FTransform *OutPts);
	INT ClipTexPoints (ICamera *Camera, FTransTex *InPts, FTransTex *OutPts, INT Num0);

	RAINBOW_PTR GetPaletteLightingTable(ICamera &Camera,UPalette *Palette,AZoneDescriptor *ZoneDescriptor,
		ALevelDescriptor *LevelDescriptor,FCacheItem *&CacheItem);

	INT 	SetupSprite     (ICamera *Camera, FSprite *Sprite);
	void	DrawActorSprite (ICamera *Camera, FSprite *Sprite);
	void	DrawActorChunk  (ICamera *Camera, FSprite *Sprite);
	void	DrawMeshMap		(ICamera &Camera, UMeshMap &MeshMap,FSpanBuffer *SpanBuffer,FVector &Location,FRotation &Rotation,INT Highlight, FSprite *Sprite,INT DrawLit,INT DrawSmooth);

private:
	void ShowStat (ICamera *Camera,INT *StatYL,const char *Str);
	void DrawStats(ICamera *Camera);
	void DrawGridSection (ICamera *Camera, INT CameraLocX,
		INT CameraSXR, INT CameraGridY, FVector *A, FVector *B,
		FLOAT *AX, FLOAT *BX,INT AlphaCase);
};

extern FGlobalRender GRender;
extern FGlobalLightManagerBase *GLightManager;

/*------------------------------------------------------------------------------------
	Random numbers.
------------------------------------------------------------------------------------*/

//
// Random number subsystem.
// Tracks a list of random numbers.
//
class FGlobalRandomsBase
{
protected:
	// Constants.
	enum {RAND_CYCLE = 16       }; // Number of ticks for a complete cycle of Randoms.
	enum {N_RANDS    = 256      }; // Number of random numbers tracked, guaranteed power of two.
	enum {RAND_MASK  = N_RANDS-1}; // Mask so that (i&RAND_MASK) is a valid index into Randoms.

	// Variables.
	static FLOAT RandomBases	[N_RANDS]; // Per-tick discontinuous random numbers.
	static FLOAT Randoms		[N_RANDS]; // Per-tick continuous random numbers.

public:

	// Functions.
	virtual void Init()=0; // Initialize subsystem.
	virtual void Exit()=0; // Shut down subsystem.
	virtual void Tick(int ServerTicks)=0; // Mark one unit of passing time.

	// Inlines.
	FLOAT RandomBase( int i ) {return RandomBases[i & RAND_MASK]; }
	FLOAT Random(     int i ) {return Randoms    [i & RAND_MASK]; }
};
extern FGlobalRandomsBase *GRandoms;

/*-----------------------------------------------------------------------------
	Active edge occluder.
-----------------------------------------------------------------------------*/

//
// Active edge rendering base class.
//
class FGlobalOccluderBase
{
public:
	// Interface.
	virtual void Init(FGlobalPlatform *App,FMemPool *GMem,FMemPool *GDynMem)=0;
	virtual void Exit()=0;
	virtual FBspDrawList *OccludeBsp(ICamera &Camera,FSpanBuffer *Backdrop)=0;
};
extern FGlobalOccluderBase *GEdgeOccluder;

/*------------------------------------------------------------------------------------
	New DrawAcross code.
------------------------------------------------------------------------------------*/

void rendDrawAcrossSetup(ICamera *Camera, UTexture *ThisTexture, UPalette *Palette, DWORD PolyFlags, DWORD NotPolyFlags);
void rendDrawAcross (ICamera *Camera,FSpanBuffer *SpanBuffer,
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
