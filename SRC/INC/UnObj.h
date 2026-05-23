/*=============================================================================
	UnObj.h: Standard Unreal object definitions.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNOBJ // Prevent header from being included multiple times.
#define _INC_UNOBJ

/*-----------------------------------------------------------------------------
	Forward declarations.
-----------------------------------------------------------------------------*/

// Foward declaration macro.
#define DECLARE_RES(ResClass) \
	class ResClass; \
	FArchive& operator<<( FArchive&, ResClass*& ); \

// All object classes.
DECLARE_RES(UBuffer);
DECLARE_RES(UArray);
DECLARE_RES(UTextBuffer);
DECLARE_RES(UTexture);
DECLARE_RES(UFont);
DECLARE_RES(UPalette);
DECLARE_RES(UScript);
DECLARE_RES(UClass);
DECLARE_RES(USound);
DECLARE_RES(UStackTree);
DECLARE_RES(UMesh);
DECLARE_RES(UVector);
DECLARE_RES(UBspNodes);
DECLARE_RES(ULightMesh);
DECLARE_RES(UPolys);
DECLARE_RES(UModel);
DECLARE_RES(ULevel);
DECLARE_RES(UReachSpecs);
DECLARE_RES(UCamera);
DECLARE_RES(UProperties);
DECLARE_RES(UPlayer);
DECLARE_RES(UVerts);
DECLARE_RES(UMeshMap);
DECLARE_RES(UBounds);
DECLARE_RES(UEnumDef);
DECLARE_RES(ULinker);
DECLARE_RES(ULinkerLoad);
DECLARE_RES(ULinkerSave);
DECLARE_RES(UFloats);
DECLARE_RES(UMusic);
DECLARE_RES(UDepdenencyList);
DECLARE_RES(UPrimitive);

// Other classes.
class FTextureInfo;
class FSocket;
class FConstraints;
class AActor;
class ABrush;

/*-----------------------------------------------------------------------------
	UBuffer.
-----------------------------------------------------------------------------*/

//
// A database that holds raw byte data.
//
class UNENGINE_API UBuffer : public UDatabase, public FOutputDevice
{
	DECLARE_DB_CLASS(UBuffer,UDatabase,BYTE,NAME_Buffer,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_ScriptWritable | CLASS_Swappable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UBuffer(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}

	// UObject interface.
	const char *Import(const char *Buffer, const char *BufferEnd,const char *FileType);
	void Export(UTextBuffer &Buffer,const char *FileType,int Indent);

	// FOutputDevice interface.
	void Write(const void *Data, int Length, ELogType MsgType=LOG_None);
};

/*-----------------------------------------------------------------------------
	UTextBuffer.
-----------------------------------------------------------------------------*/

//
// A database that holds a bunch of text.  The text is contiguous and, if
// of nonzero length, is terminated by a NULL at the very last position.
//
class UNENGINE_API UTextBuffer : public UDatabase, public FOutputDevice
{
	DECLARE_DB_CLASS(UTextBuffer,UDatabase,char,NAME_TextBuffer,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_ScriptWritable | CLASS_Swappable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Variables.
	INT Pos; // Cursor position for editing.
	INT Top; // Top of screen pointer for editing.

	// Constructors.
	UTextBuffer(int InMax, int InOccupy=0) : UDatabase(InMax,InOccupy)
	{
		guard(UTextBuffer::UTextBuffer);
		checkState(InMax>0);
		Num=::Max(InMax,1);
		Element(0)=0;
		unguard;
	}

	// UObject interface.
	void InitHeader();
	const char *Import(const char *Buffer, const char *BufferEnd,const char *FileType);
	void Export(FOutputDevice &Out,const char *FileType,int Indent);
	void SerializeHeader(FArchive& Ar)
	{
		guard(UTextBuffer::SerializeHeader);
		UDatabase::SerializeHeader(Ar);
		Ar << Pos << Top;
		unguardobj;
	}

	// FOutputDevice interface.
	void Write(const void *Data, int Length, ELogType MsgType=LOG_None);

	// UDatabase interface.
	void Empty();
	void Shrink();
};

/*-----------------------------------------------------------------------------
	UWords object class.
-----------------------------------------------------------------------------*/

//
// A database of 16-bit words.
//
class UNENGINE_API UWords : public UDatabase
{
	DECLARE_DB_CLASS(UWords,UDatabase,WORD,NAME_Words,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Swappable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UWords(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UInts object class.
-----------------------------------------------------------------------------*/

//
// A database of 32-bit Ints.
//
class UNENGINE_API UInts : public UDatabase
{
	DECLARE_DB_CLASS(UInts,UDatabase,INT,NAME_Ints,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Swappable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UInts(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UArray.
-----------------------------------------------------------------------------*/

//
// A database that holds references to untyped objects.
//
class UNENGINE_API UArray : public UDatabase
{
	DECLARE_DB_CLASS(UArray,UDatabase,UObject *,NAME_Array,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UArray(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

//
// A template that deals with object arrays type-safely.
//
template<class TClassPtr> class TArray : public UArray
{
public:
	// Constructors.
	TArray<TClassPtr>( int MaxElements, int Occupy=0 )
	{
		guard(TArray::TArray);
		Num = 0;
		Max = MaxElements;
		Realloc();
		if( Occupy ) Num = Max;
		unguardobj;
	}
	TArray<TClassPtr>() {}

	// UDatabase interface.
	      TClassPtr &Element(int i)       {return ((TClassPtr *)GetData())[i];}
	const TClassPtr &Element(int i) const {return ((TClassPtr *)GetData())[i];}

	// TArray interface.
	struct UNENGINE_API Ptr
	{
		TArray *Res;
		Ptr() {}
		Ptr(TArray *Set) {Res = Set;}
		      TArray& operator*()        {return *Res;}
		const TArray& operator*()  const {return *Res;}
		      TArray* operator->()       {return Res;}
		const TArray* operator->() const {return Res;}
		operator       TArray* ()        {return Res;}
		operator const TArray* ()  const {return Res;}
		      TClassPtr& operator() (int i)       {return (TClassPtr &)Res->Element(i);}
		const TClassPtr& operator() (int i) const {return (TClassPtr &)Res->Element(i);}
	};
};

/*-----------------------------------------------------------------------------
	UEnumDef.
-----------------------------------------------------------------------------*/

//
// An enumeration, a database of names.  Used for enumerationsin
// Unreal scripts.
//
class UNENGINE_API UEnumDef : public UDatabase
{
	DECLARE_DB_CLASS(UEnumDef,UDatabase,FName,NAME_EnumDef,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UEnumDef(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}

	// UObject interface.
	const char *Import(const char *Buffer, const char *BufferEnd,const char *FileType);
	void Export(FOutputDevice &Out,const char *FileType,int Indent);

	// UEnumDef interface.
	int AddName(const char *NewName);
	int FindNameIndex(const char *Name, INDEX &Result) const;
};

/*-----------------------------------------------------------------------------
	UPalette.
-----------------------------------------------------------------------------*/

//
// A truecolor value.
//
class UNENGINE_API FColor
{
public:
	// Variables.
	union
	{
		struct
		{
			BYTE R,G,B;
			union {BYTE Flags, RemapIndex, A;};
		};
		struct
		{
			CHAR NormalU,NormalV;
		};
		DWORD D;
	};

	// Constants.
	enum
	{
		EHiColor565_R = 0xf800,
		EHiColor565_G = 0x07e0,
		EHiColor565_B = 0x001f,

		EHiColor555_R = 0x7c00,
		EHiColor555_G = 0x03e0,
		EHiColor555_B = 0x001f,

		ETrueColor_R  = 0x00ff0000,
		ETrueColor_G  = 0x0000ff00,
		ETrueColor_B  = 0x000000ff,
	};

	// Constructors.
	FColor() {}
	FColor( BYTE InR, BYTE InG, BYTE InB )
	:	R(InR), G(InG), B(InB) {}
	FColor( BYTE InR, BYTE InG, BYTE InB, BYTE InFlags )
	:	R(InR), G(InG), B(InB), Flags(InFlags) {}
	FColor( DWORD InD )
	:	D(InD) {}
	FColor( const FVector &V )
	:	R(ftoi(Clamp(V.R,0.f,255.f))),
		G(ftoi(Clamp(V.G,0.f,255.f))),
		B(ftoi(Clamp(V.B,0.f,255.f))) {}

	// Serializer.
	friend FArchive& operator<< (FArchive &Ar, FColor &Color )
	{
		return Ar << Color.R << Color.G << Color.B << Color.RemapIndex;
	}

	// Operators.
	int operator==( const FColor &C ) const
	{
		return R==C.R && G==C.G && B==C.B;
	}
	int operator!=( const FColor &C ) const
	{
		return R!=C.R || G!=C.G || B!=C.B;
	}

	// Greyscale 0-255 brightness.
	int Brightness() const
	{
		return (2*(int)R + 3*(int)G + 1*(int)B)>>3;
	}
	// Floating point 0.0 to 1.0 brightness.
	FLOAT FBrightness() const
	{
		return (2.0*R + 3.0*G + 1.0*B)/(6.0*256.0);
	}
	DWORD TrueColor() const
	{
		return ((D&0xff)<<16) + (D&0xff00) + ((D&0xff0000)>>16);
	}
	WORD HiColor565() const
	{
		return ((D&0xf8) << 8) + ((D&0xfC00) >> 5) + ((D&0xf80000) >> 19);
	}
	WORD HiColor555() const
	{
		return ((D&0xf8) << 7) + ((D&0xf800) >> 6) + ((D&0xf80000) >> 19);
	}
	FVector Vector() const
	{
		return FVector(R,G,B);
	}
};

//
// A palette object.  Holds NUM_PAL_COLORS unique FColor values, 
// forming a 256-color palette which can be referenced by textures.
//
class UNENGINE_API UPalette : public UDatabase
{
	DECLARE_DB_CLASS(UPalette,UDatabase,FColor,NAME_Palette,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_ScriptWritable | CLASS_Swappable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constants.
	enum{NUM_PAL_COLORS=256};	// Number of colors in a standard palette.

	// Constructors.
	UPalette( int InNum, int InOccupy=0 ) : UDatabase( InNum, InOccupy ) {}

	// UObject interface.
	const char *Import(const char *Buffer, const char *BufferEnd,const char *FileType);
	void InitHeader();

	// UPalette interface.
	BYTE BestMatch(FColor Color,int SystemPalette=1);
	UPalette* ReplaceWithExisting();
	void BuildPaletteRemapIndex(int Masked);
	void Smooth();
	RAINBOW_PTR GetColorDepthPalette(FCacheItem *&CacheItem, UTexture *CameraTexture);
	void FixPalette();
};

/*-----------------------------------------------------------------------------
	UTexture and FTextureInfo.
-----------------------------------------------------------------------------*/

// Maximum number of mipmaps that a texture can have.
enum {MAX_MIPS=12};
enum {MAX_TEXTURE_SIZE=1024};

// Flags for normal textures.
enum ETextureFlags
{
	// General info about the texture.
	TF_NoTile			= 0x00000001,	// Texture size isn't a power of two.
	TF_BumpMap			= 0x00000002,	// This texture is a normal-discretized bumpmap.
	TF_Blur				= 0x00000004,	// Blur this texture on import.
	TF_Realtime         = 0x00000008,   // Texture data (not animation) changes in realtime.

	// Special flags.
	TF_Temp				= 0x08000000,	// Temporary.

	// Special info about a locked texture, relative to the camera it was locked for.
	TL_Render			= 0x10000000,	// Suitable for polygon rendering to the camera.
	TL_RenderPalette	= 0x20000000,	// Colors is a valid palette for the camera.
	TL_RenderRamp		= 0x40000000,	// Colors is a valid ramp palette for the camera.

	// Flag groups.
	TL_AnyRender = TL_Render | TL_RenderPalette | TL_RenderRamp,
};

//
// Information about one of the texture's mipmaps.
//warning: Mirrored in UnRender.inc.
//
struct UNENGINE_API FMipInfo
{
	// Always valid.
	DWORD		Offset;			// Offset of mipmap into texture's data.

	// Valid in memory only, set by engine in PostLoadHeader.
	DWORD		USize;			// U dimension.
	DWORD		VSize;			// V dimension.
	BYTE		MipLevel;		// MipLevel (actual).
	BYTE		UBits;			// UBits.
	BYTE		VBits;			// VBits.
	BYTE		Unused1;		// Available.

	// Valid in memory only, set by rendering subsystem in PostLoadData.
	DWORD		VMask;			// VMask.
	DWORD		AndMask;		// AndMask.
	BYTE		*Data;			// Pointer to start, set in PostLoadData.
	struct FDitherUnit *Dither;	// Pointer to dither table, set in PostLoadData.

	// Inlines.
	INT Size()
	{
		return USize * VSize;
	}
};

//
// A 256-color texture map.  Texture maps reference an optional palette,
// and can either be a simple rectangular array of pixels, or a hierarchy of mipmaps
// where each successive mipmap is half the dimensions of the mipmap above it.
//
class UNENGINE_API UTexture : public UDatabase
{
	DECLARE_DB_CLASS(UTexture,UDatabase,BYTE,NAME_Texture,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_ScriptWritable | CLASS_Swappable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Subtextures.
	UTexture*	BumpMap;			// Bump map to illuminate this texture with.
	UTexture*	DetailTexture;		// Detail texture to apply.
	UTexture*	MacroTexture;		// Macrotexture to apply, not currently used.
	UTexture*	AnimNext;			// Next texture in looped animation sequence.

	// The palette.
	UPalette::Ptr	Palette;		// Custom texture palette, NULL = use default palette.

	// Surface lighting properties.
	FLOAT		DiffuseC;			// Diffuse lighting coefficient (0.0-1.0).
	FLOAT		SpecularC;			// Specular lighting coefficient (0.0-1.0).
	FLOAT		ReflectivityC;		// Reflectivity (0.0-0.1).

	// Surface physics properties.
	FLOAT		FrictionC;			// Surface friction coefficient, 1.0=none, 0.95=some.

	// Sounds.
	USound		*FootstepSound;		// Footstep sound.
	USound		*HitSound;			// Sound when the texture is hit with a projectile.

	// Flags.
	DWORD		PolyFlags;			// Polygon flags to be applied to Bsp polys with texture (See PF_*).
	DWORD		TextureFlags;		// Texture flags, see TF_*.

	// Misc info.
	BYTE		UBits;				// # of bits in USize, i.e. 8 for 256.
	BYTE		VBits;				// # of bits in VSize.

	// Internal information.
	INT			USize;				// Width, must be power of 2.
	INT			VSize;				// Height, must be power of 2.
	INT			ColorBytes;			// Bytes per texel, must be 1 for regular textures.
	DWORD		CameraCaps;			// If a Camera, its caps flags, otherwise 0.
	FColor		MipZero;			// Overall average color of texture.
	FColor		MinColor;			// Minimum color for normalization.
	FColor		MaxColor;			// Maximum color for normalization.

	// Padding.
	DWORD		Pad[8];				// For expansion.

	// Table of mipmaps.
	FMipInfo    Mips[MAX_MIPS];		// Offset of mipmaps into texture's data, MAXDWORD=not avail.

	// Constructors.
	UTexture(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}

	// UObject interface.
	void InitHeader();
	const char *Import(const char *Buffer, const char *BufferEnd,const char *FileType);
	void Export(FOutputDevice &Out,const char *FileType,int Indent);
	void PostLoadData(DWORD PostFlags);
	void PreKill();
	void SerializeHeader(FArchive &Ar)
	{
		guard(UTexture::SerializeHeader);

		// UObject references.
		UDatabase::SerializeHeader(Ar);

		// UTexture references.
		Ar << BumpMap << DetailTexture << MacroTexture << AnimNext;
		Ar << Palette;
		Ar << DiffuseC << SpecularC << ReflectivityC;
		Ar << FrictionC;
		Ar << FootstepSound << HitSound;
		Ar << PolyFlags << TextureFlags;
		Ar << UBits << VBits;
		Ar << USize << VSize << ColorBytes;
		Ar << CameraCaps << MipZero;
		Ar << MinColor << MaxColor;

		for( int i=0; i<MAX_MIPS; i++ )
			Ar << Mips[i].Offset;

		unguardobj;
	}

	// UTexture interface.
	void Remap(UPalette *SourcePalette, UPalette *DestPalette);
	void BuildPaletteRemapIndex(int Masked);
	void CreateMips(int FullMips);
	void CreateBumpMap();
	void Fixup();
	void CreateColorRange();
	void BurnRect(int X1,int X2,int Y1,int Y2,int Bright);
	void Clearscreen(BYTE DefaultPalColor);

	// UTexture locking.
	INT Lock(DWORD LockType)
	{
		return UDatabase::Lock(LockType);
	}
	INT Lock(FTextureInfo &TextureInfo, UTexture *Camera, DWORD TextureLockFlags, class AZoneInfo *Zone=NULL);

	// UTexture unlocking.
	void Unlock(DWORD OldLockType)
	{
		UDatabase::Unlock(OldLockType);
	}
	void Unlock(FTextureInfo &TextureInfo);
private:
	void UTexture::DoRemap (BYTE *Remap);
	void InitRemap(BYTE *Remap);
};

//
// Information about a locked texture. Used for ease of rendering.
//
class FTextureInfo
{
public:
	// General variables.
	DWORD			Flags;				// From ETextureFlags.
	RAINBOW_PTR		Colors;				// Palette for the camera's specific colordepth, if requested.
	FMipInfo        *Mips[MAX_MIPS];	// Pointer to each mipmap's info.
	class AZoneInfo *Zone;				// Zone info for the zone the texture is in, or NULL.
	UPalette		*Palette;			// Optional palette.

	// Platform-specific data.
	BYTE			Platform[256];		// Space reserved for platform-specific locked texture info.
};

/*-----------------------------------------------------------------------------
	UTextureSet.
-----------------------------------------------------------------------------*/

//
// A database that holds references to untyped objects.
//
class UNENGINE_API UTextureSet : public UDatabase
{
	DECLARE_DB_CLASS(UTextureSet,UDatabase,UTexture *,NAME_TextureSet,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_ScriptWritable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UTextureSet(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UBspNode.
-----------------------------------------------------------------------------*/

// Flags associated with a Bsp node.
enum EBspNodeFlags
{
	// Flags.
	NF_NotCsg			= 0x01, // Node is not a Csg splitter, i.e. is a transparent poly.
	NF_ShootThrough		= 0x02, // Can shoot through (for projectile solid ops).
	NF_NotVisBlocking   = 0x04, // Node does not block visibility, i.e. is an invisible collision hull.
	NF_PolyOccluded		= 0x08, // Node's poly was occluded on the previously-drawn frame.
	NF_AllOccluded		= 0x10, // Node and all its children were occluded on previously-drawn frame.
	NF_IsNew 		 	= 0x20, // Editor: Node was newly-added.
	NF_IsFront     		= 0x40, // Filter operation bounding-sphere precomputed and guaranteed to be front.
	NF_IsBack      		= 0x80, // Guaranteed back.

	// Combinations of flags.
	NF_NeverMove		= 0, // Bsp cleanup must not move nodes with these tags.
};

//
// FBspNode defines one node in the Bsp, including the front and back
// pointers and the polygon data itself.  A node may have 0 or 3 to (MAX_NODE_VERTICES-1)
// vertices. If the node has zero vertices, it's only used for splitting and
// doesn't contain a polygon (this happens in the editor).
//
// vNormal, vTextureU, vTextureV, and others are indices into the level's
// vector table.  iFront,iBack should be INDEX_NONE to indicate no children.
//
// If iPlane==INDEX_NONE, a node has no coplanars.  Otherwise iPlane
// is an index to a coplanar polygon in the Bsp.  All polygons that are iPlane
// children can only have iPlane children themselves, not fronts or backs.
//
class FBspNode // 64 bytes
{
public:
	enum {MAX_NODE_VERTICES=16};	// Max vertices in a Bsp node, pre clipping
	enum {MAX_FINAL_VERTICES=24};	// Max vertices in a Bsp node, post clipping
	enum {MANY_CHILDREN=8};			// A Bsp Node with this many children is occlusion-rejected carefully

	// Persistent information.
	FPlane			Plane;			// 16 Plane the node falls into (X, Y, Z, W).
	QWORD			ZoneMask;		// 8  Bit mask for all zones at or below this node (up to 64).
	INDEX			iVertPool;		// 4  Index of first vertex in vertex pool, =iTerrain if NumVertices==0 and NF_TerrainFront.
	INDEX			iSurf;			// 4  Index to surface information.
	union
	{
		struct
		{
			INDEX	iBack;			// 4  Index to node in front (in direction of Normal).
			INDEX	iFront;			// 4  Index to node in back  (opposite direction as Normal).
			INDEX	iPlane;			// 4  Index to next coplanar poly in coplanar list.
		};
		struct
		{
			INDEX	iChild[3];		// 12 Index representation of children.
		};
	};
	INDEX			iCollisionBound;// 4  Collision bound.
	INDEX			iRenderBound;	// 4  Rendering bound.
	BYTE			iZone[2];		// 2  Visibility zone in 1=front, 0=back.
	BYTE			NumVertices;	// 1  Number of vertices in node.
	BYTE			NodeFlags;		// 1  Node flags.

	// Valid in memory only.
	INDEX			iDynamic[2];	// 4  Index to dynamic contents in 1=front, 0=Back. !!old!!

	// Functions.
	int IsCsg( DWORD ExtraFlags=0 ) const
	{
		return (NumVertices>0) && !(NodeFlags & (NF_IsNew | NF_NotCsg | ExtraFlags));
	}
	int ChildOutside( int iChild, int Outside, DWORD ExtraFlags=0 ) const
	{
		return iChild ? (Outside || IsCsg(ExtraFlags)) : (Outside && !IsCsg(ExtraFlags));
	}
	struct FDynamicsIndex* GetDynamic( int i )
	{
		return (FDynamicsIndex*)iDynamic[i];
	}
	void SetDynamic( int i, struct FDynamicsIndex* Value )
	{
		iDynamic[i] = (int)Value;
	}
	friend FArchive& operator<< (FArchive &Ar, FBspNode &N )
	{
		guard(FBspNode<<);
		Ar << N.Plane << N.ZoneMask << N.NodeFlags << N.iVertPool << N.iSurf;
		Ar << N.iChild[0] << N.iChild[1] << N.iChild[2];
		Ar << N.iCollisionBound << N.iRenderBound << N.iZone[0] << N.iZone[1];
		Ar << N.NumVertices;
		return Ar;
		unguard;
	}
};

//
// Properties of a zone.
//
class UNENGINE_API FZoneProperties
{
public:
	// General zone properties.
	AZoneInfo*	ZoneActor;		// Optional actor defining the zone's property.

	// Connectivity and visibility bit masks.
	QWORD		Connectivity;	// (Connect[i]&(1<<j))==1 if zone i is adjacent to zone j.
	QWORD		Visibility;		// (Connect[i]&(1<<j))==1 if zone i can see zone j.

	// Serializer.
	friend FArchive& operator<< (FArchive& Ar, FZoneProperties &P)
	{
		guard(FZoneProperties<<);
		return Ar << AR_OBJECT(P.ZoneActor) << P.Connectivity << P.Visibility;
		unguard;
	}
};

//
// A database of Bsp nodes associated with a model.
//
class UNENGINE_API UBspNodes : public UDatabase
{
	DECLARE_DB_CLASS(UBspNodes,UDatabase,FBspNode,NAME_BspNodes,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	enum {MAX_ZONES=64};				// Maximum zones in a Bsp, limited by QWORD bitmask size

	// Variables.
	INT				NumZones;			// Number of rendering zones
	FZoneProperties	Zones[MAX_ZONES];	// Properties of each zone.

	// Constructors.
	UBspNodes(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}

	// UObject interface.
	void InitHeader();
	void SerializeHeader(FArchive& Ar)
	{
		guard(UBspNodes::SerializeHeader);
		UDatabase::SerializeHeader(Ar);
		Ar << NumZones;
		for( int i=0; i<NumZones; i++ )
			Ar << Zones[i];
		unguardobj;
	}

	// UDatabase interface.
	void PostLoadItem(int Index,DWORD PostFlags);
};

/*-----------------------------------------------------------------------------
	UBspSurf.
-----------------------------------------------------------------------------*/

//
// One Bsp polygon.  Lists all of the properties associated with the
// polygon's plane.  Does not include a point list; the actual points
// are stored along with Bsp nodes, since several nodes which lie in the
// same plane may reference the same poly.
//
class FBspSurf
{
public:

	// Persistent info.
	UTexture	*Texture;	// 4 Texture map.
	UModel		*Brush;		// 4 Editor brush. !!to be eliminated later
	DWORD		PolyFlags;  // 4 Polygon flags.
	INDEX		pBase;      // 4 Polygon & texture base point index (where U,V==0,0).
	INDEX		vNormal;    // 4 Index to polygon normal.
	INDEX		vTextureU;  // 4 Texture U-vector index.
	INDEX		vTextureV;  // 4 Texture V-vector index.
	INDEX		iLightMesh;	// 4 Light mesh.
	INDEX		iBrushPoly; // 4 Editor brush polygon index.
	SWORD		PanU;		// 2 U-Panning value.
	SWORD		PanV;		// 2 V-Panning value.
	ABrush		*Actor;		// 4 Brush actor owning this Bsp surface.

	// Valid in memory only.
	SWORD		LastStartY;	// 2 Last span buffer starting Y value or 0.
	SWORD		LastEndY;	// 2 Last span buffer ending Y value or 0.

	// Functions.
	friend FArchive& operator<< (FArchive &Ar, FBspSurf &Surf)
	{
		guard(FBspSurf<<);
		Ar << Surf.Texture;
		Ar << Surf.Brush;
		Ar << Surf.PolyFlags << Surf.pBase << Surf.vNormal;
		Ar << Surf.vTextureU << Surf.vTextureV;
		Ar << Surf.iLightMesh << Surf.iBrushPoly;
		Ar << Surf.PanU << Surf.PanV;
		Ar << AR_OBJECT(Surf.Actor);
		return Ar;
		unguard;
	}
};

// Flags describing effects and properties of a Bsp polygon.
enum EPolyFlags
{
	// Regular in-game flags.
	PF_Invisible		= 0x00000001,	// Poly is invisible.
	PF_Masked			= 0x00000002,	// Poly should be drawn masked.
	PF_Transparent	 	= 0x00000004,	// Poly is transparent.
	PF_NotSolid			= 0x00000008,	// Poly is not solid, doesn't block.
	PF_Environment   	= 0x00000010,	// Poly should be drawn environment mapped.
	PF_Semisolid	  	= 0x00000020,	// Poly is semi-solid = collision solid, Csg nonsolid.
	PF_Hurt 			= 0x00000040,	// Floor hurts player.
	PF_FakeBackdrop		= 0x00000080,	// Poly looks exactly like backdrop.
	PF_TwoSided			= 0x00000100,	// Poly is visible from both sides.
	PF_AutoUPan		 	= 0x00000200,	// Automatically pans in U direction.
	PF_AutoVPan 		= 0x00000400,	// Automatically pans in V direction.
	PF_NoSmooth			= 0x00000800,	// Don't smooth textures.
	PF_BigWavy 			= 0x00001000,	// Poly has a big wavy pattern in it.
	PF_SmallWavy		= 0x00002000,	// Small wavy pattern (for water/enviro reflection).
	PF_WaterWavy		= 0x00004000,	// Poly is ghost (transparent nocolorized) 1-sided.
	PF_LowShadowDetail	= 0x00008000,	// Low detaul shadows.
	PF_NoMerge			= 0x00010000,	// Don't merge poly's nodes before lighting when rendering.
	PF_CloudWavy		= 0x00020000,	// Polygon appears wavy like clouds.
	PF_DirtyShadows		= 0x00040000,	// Dirty shadows.
	PF_HighLedge		= 0x00080000,	// High ledge, blocks player.
	PF_SpecialLit		= 0x00100000,	// Only speciallit lights apply to this poly.
	PF_Gouraud			= 0x00200000,	// Gouraud shaded.
	PF_Unlit			= 0x00400000,	// Unlit.
	PF_HighShadowDetail	= 0x00800000,	// High detail shadows.
	PF_Portal			= 0x04000000,	// Portal between iZones.
	PF_NoLook			= 0x08000000,	// Player shouldn't automatically look up/down these surfaces.

	// Editor flags.
	PF_Memorized     	= 0x01000000,	// Editor: Poly is remembered.
	PF_Selected      	= 0x02000000,	// Editor: Poly is selected.
	PF_InternalUnused1	= 0x40000000,	// FPoly has been split by SplitPolyWithPlane.   
	PF_DynamicLight		= 0x80000000,	// Polygon is dynamically lit.

	// FPoly flags.
	PF_EdProcessed 		= 0x40000000,	// FPoly was already processed in editorBuildFPolys.
	PF_EdCut       		= 0x80000000,	// FPoly has been split by SplitPolyWithPlane.  

	// Combinations of flags.
	PF_NoOcclude		= PF_Masked | PF_Transparent | PF_Invisible,
	PF_NoEdit			= PF_Memorized | PF_Selected | PF_EdProcessed | PF_NoMerge | PF_EdCut,
	PF_NoImport			= PF_NoEdit | PF_NoMerge | PF_Memorized | PF_Selected | PF_DynamicLight | PF_EdProcessed | PF_EdCut,
	PF_AddLast			= PF_Semisolid | PF_NotSolid,
	PF_NoAddToBSP		= PF_EdCut | PF_EdProcessed | PF_Selected | PF_Memorized,
	PF_NoShadows		= PF_Unlit | PF_Invisible | PF_Environment | PF_FakeBackdrop | PF_Portal,
};

//
// A database of Bsp polygons associated with a model.
//
class UNENGINE_API UBspSurfs : public UDatabase
{
	DECLARE_DB_CLASS(UBspSurfs,UDatabase,FBspSurf,NAME_BspSurfs,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UBspSurfs(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}

	// UObject interface.
	void ModifySelected(int UpdateMaster);
	void ModifyAllItems(int UpdateMaster);
	void ModifyItem(int Index, int UpdateMaster);
};

/*-----------------------------------------------------------------------------
	UBounds.
-----------------------------------------------------------------------------*/

//
// A bonuding volume object is a database of bounding box associated with
// node in a model's Bsp.
//
class UNENGINE_API UBounds : public UDatabase
{
	DECLARE_DB_CLASS(UBounds,UDatabase,FBoundingBox,NAME_Bounds,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UBounds(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	ULightMesh.
-----------------------------------------------------------------------------*/

//
// Describes the mesh-based lighting applied to a Bsp poly.
//
class FLightMeshIndex
{
public:
	// Constants.
	enum {MAX_POLY_LIGHTS=16};	// Number of lights that may be applied to a single poly

	// Variables.
	INT		DataOffset;			// Data offset for first shadow mesh.
	INT		TextureUStart;  	// Minimum texture U coordinate that is visible.
	INT		TextureVStart;  	// Minimum texture U coordinate that is visible.
	INT     MeshUSize;   	  	// Lighting mesh U-size = (USize+15)/16.
	INT	    MeshVSize;   	  	// Lighting mesh V-size = (VSize+15)/16.
	INT		MeshSpacing;    	// Mesh spacing, usually 32.
	INT		NumStaticLights;	// Number of static, shadowing lights attached.
	INT		NumDynamicLights;	// Number of dynamic, non-shadowing lights attached.
	BYTE	MeshShift;		  	// Mesh shifting value, LogTwo(MeshSpacing).
	BYTE	MeshUBits;			// FLogTwo(MeshUSize).
	BYTE	MeshVBits;			// FLogTwo(MeshVSize).
	BYTE    InternalByte;		// Reserved for rendering use.
	INT		InternalInt;		// Reserved for rendering use.
	AActor  *LightActor[MAX_POLY_LIGHTS]; // Actors controlling the lights.

	// Serializer.
	friend FArchive& operator<< (FArchive& Ar, FLightMeshIndex& I)
	{
		guard(FLightMeshIndex<<);
		Ar << I.DataOffset;
		Ar << I.TextureUStart << I.TextureVStart;
		Ar << I.MeshUSize     << I.MeshVSize;
		Ar << I.MeshSpacing;
		Ar << I.NumStaticLights << I.NumDynamicLights;
		Ar << I.MeshShift << I.MeshUBits << I.MeshVBits;
		for( int i=0; i<(I.NumStaticLights+I.NumDynamicLights); i++ )
			Ar << AR_OBJECT(I.LightActor[i]);
		return Ar;
		unguard;
	}
};

//
// A light mesh object associated with a level.
//
class UNENGINE_API ULightMesh : public UDatabase
{
	DECLARE_DB_CLASS(ULightMesh,UDatabase,FLightMeshIndex,NAME_LightMesh,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Variables.
	UBuffer::Ptr Bits;

	// Constructor.
	ULightMesh(int InNum, int InFill )
	:	UDatabase(InNum,InFill)
	{
		guard(ULightMesh::ULightMesh);
		Bits = new(GetName(),CREATE_Replace)UBuffer(0);
		unguardobj;
	}

	// UObject interface.
	INT Lock(DWORD NewLockType)
	{
		guard(ULightMesh::Lock);
		if( Bits ) Bits->Lock(NewLockType);
		return UDatabase::Lock(NewLockType);
		unguardobj;
	}
	void Unlock(DWORD OldLockType)
	{
		guard(ULightMesh::Unlock);
		if( Bits ) Bits->Unlock(OldLockType);
		UDatabase::Unlock(OldLockType);
		unguardobj;
	}
	void InitHeader()
	{	
		guard(ULightMesh::InitHeader);

		// Init parent.
		UDatabase::InitHeader();

		// Init light mesh info.
		Bits = (UBuffer *)NULL;

		unguardobj;
	}
	void SerializeHeader(FArchive& Ar)
	{
		guard(UDatabase::SerializeHeader);
		UDatabase::SerializeHeader(Ar);
		Ar << Bits;
		unguardobj;
	}
};

/*-----------------------------------------------------------------------------
	UPolys.
-----------------------------------------------------------------------------*/

// Results from FPoly.SplitWithPlane, describing the result of splitting
// an arbitrary FPoly with an arbitrary plane.
enum ESplitType
{
	SP_Coplanar		= 0, // Poly wasn't split, but is coplanar with plane
	SP_Front		= 1, // Poly wasn't split, but is entirely in front of plane
	SP_Back			= 2, // Poly wasn't split, but is entirely in back of plane
	SP_Split		= 3, // Poly was split into two new editor polygons
};

//
// A general-purpose polygon used by the editor.  An FPoly is a free-standing
// class which exists independently of any particular level, unlike the polys
// associated with Bsp nodes which rely on scads of other objects.  FPolys are
// used in UnrealEd for internal work, such as building the Bsp and performing
// boolean operations.
//
class UNENGINE_API FPoly
{
public:
	enum {MAX_VERTICES=16}; // Maximum vertices an FPoly may have.
	enum {VERTEX_THRESHOLD=MAX_VERTICES-2}; // Threshold for splitting into two.

	FVector     Base;        	// Base point of polygon.
	FVector     Normal;			// Normal of polygon.
	FVector     TextureU;		// Texture U vector.
	FVector     TextureV;		// Texture V vector.
	FVector     Vertex[MAX_VERTICES]; // Actual vertices.
	DWORD       PolyFlags;		// FPoly & Bsp poly bit flags (PF_).
	UModel		*Brush;			// Brush where this originated, or NULL.
	UTexture	*Texture;		// Texture map.
	FName		GroupName;		// Group name.
	FName		ItemName;		// Item name.
	INDEX       NumVertices;	// Number of vertices.
	INDEX		iLink;			// iBspSurf, or brush fpoly index of first identical polygon, or MAXWORD.
	INDEX		iBrushPoly;		// Index of editor solid's polygon this originated from.
	SWORD		PanU,PanV;		// Texture panning values.
	BYTE		iZone[2];		// Zones in front and back.

	// Custom functions.
	void  Init				();
	void  Reverse			();
	void  SplitInHalf		(FPoly *OtherHalf);
	void  Transform			(const FModelCoords &Coords, const FVector &PreSubtract,const FVector &PostAdd, FLOAT Orientation);
	int   Fix				();
	int   CalcNormal		();
	int   SplitWithPlane	(const FVector &Base,const FVector &Normal,FPoly *FrontPoly,FPoly *BackPoly,int VeryPrecise) const;
	int   SplitWithNode		(const UModel *Model,INDEX iNode,FPoly *FrontPoly,FPoly *BackPoly,int VeryPrecise) const;
	int   SplitWithPlaneFast(const FPlane Plane,FPoly *FrontPoly,FPoly *BackPoly) const;
	int   Split				(const FVector &Normal, const FVector &Base, int NoOverflow=0 );
	int   RemoveColinears	();
	int   Finalize			(int NoError);
	int   Faces				(const FPoly &Test) const;
	FLOAT Area				();

	// Serializer.
	friend FArchive& operator<< (FArchive& Ar, FPoly &Poly)
	{
		guard(FPoly<<);
		Ar << Poly.NumVertices;
		Ar << Poly.Base << Poly.Normal << Poly.TextureU << Poly.TextureV;
		for( int i=0; i<Poly.NumVertices; i++ )
			Ar << Poly.Vertex[i];
		Ar << Poly.PolyFlags;
		Ar << Poly.Brush << Poly.Texture << Poly.GroupName << Poly.ItemName;
		Ar << Poly.iLink << Poly.iBrushPoly << Poly.PanU << Poly.PanV;
		Ar << Poly.iZone[0] << Poly.iZone[1];
		return Ar;
		unguard;
	}

	// Inlines.
	int IsBackfaced( const FVector &Point ) const
		{return ((Point-Base) | Normal) < 0.0;}
	int IsCoplanar( const FPoly &Test ) const
		{return Abs((Base - Test.Base)|Normal)<0.01 && Abs(Normal|Test.Normal)>0.9999;}
};

//
// List of FPolys.
//
class UNENGINE_API UPolys : public UDatabase
{
	DECLARE_DB_CLASS(UPolys,UDatabase,FPoly,NAME_Polys,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Swappable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UPolys(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}

	// UObject interface.
	const char *Import(const char *Buffer, const char *BufferEnd,const char *FileType);
	void Export(FOutputDevice &Out,const char *FileType,int Indent);

	// UPolys interface.
	void ParseFPolys(const char **Stream,int More, int CmdLine);
};

/*-----------------------------------------------------------------------------
	UFloats.
-----------------------------------------------------------------------------*/

//
// A table of floating point numbers.
//
class UNENGINE_API UFloats : public UDatabase
{
	DECLARE_DB_CLASS(UFloats,UDatabase,FLOAT,NAME_Floats,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UFloats(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UVectors.
-----------------------------------------------------------------------------*/

//
// A table of floating point vectors.  Used within levels to store all points
// and vectors in the world.
//
class UNENGINE_API UVectors : public UDatabase
{
	DECLARE_DB_CLASS(UVectors,UDatabase,FVector,NAME_Vectors,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UVectors(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UVerts.
-----------------------------------------------------------------------------*/

//
// One vertex associated with a Bsp node's polygon.  Contains a vertex index
// into the level's FPoints table, and a unique number which is common to all
// other sides in the level which are cospatial with this side.
//
class FVert
{
public:
	// Variables.
	INDEX 	pVertex;	// Index of vertex.
	INDEX	iSide;		// If shared, index of unique side. Otherwise INDEX_NONE.

	// Functions.
	friend FArchive& operator<< (FArchive &Ar, FVert &Vert)
	{
		guard(FVert<<);
		return Ar << Vert.pVertex << Vert.iSide;
		unguard;
	}
};

//
// Vertex pool object, containing all point lists referenced by Bsp polygons,
// as well as connectivity information linking sides of adjacent polys.
//
class UNENGINE_API UVerts : public UDatabase
{
	DECLARE_DB_CLASS(UVerts,UDatabase,FVert,NAME_Verts,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Variables.
	INT NumSharedSides; // Number of unique iSideIndex's.

	// Constructors.
	UVerts(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}

	// UObject interface.
	void InitHeader();
	void SerializeHeader(FArchive& Ar)
	{
		guard(UVerts::SerializeHeader);
		UDatabase::SerializeHeader(Ar);
		Ar << NumSharedSides;
		unguardobj;
	}
};

/*-----------------------------------------------------------------------------
	UFont.
-----------------------------------------------------------------------------*/

//
// Information about one font character which resides in a texture.
//
class UNENGINE_API FFontCharacter
{
public:
	// Variables.
	int	StartU;	// Starting coordinate of character in texture.
	int	StartV;	// Ending coordinate of character in texture.
	int	USize;	// U-length of character, 0=none.
	int	VSize;	// V-length of character.

	// Serializer.
	friend FArchive& operator<< (FArchive &Ar, FFontCharacter &Ch )
	{
		guard(FFontCharacter<<);
		return Ar << Ch.StartU << Ch.StartV << Ch.USize << Ch.VSize;
		unguard;
	}
};

//
// A font object, containing information about a set of characters.
// The character bitmaps are stored in the contained textures, while
// the font database only contains the coordinates of the individual
// characters.
//
class UNENGINE_API UFont : public UDatabase
{
	DECLARE_DB_CLASS(UFont,UDatabase,FFontCharacter,NAME_Font,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_ScriptWritable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constants.
	enum {NUM_FONT_CHARS=256};

	// Variables.
	UTexture::Ptr Texture;

	// Constructors.
	UFont(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}

	// UObject interface.
	void InitHeader();
	void SerializeHeader(FArchive &Ar)
	{
		guard(UFont::SerializeHeader);
		UDatabase::SerializeHeader(Ar);
		Ar << Texture;
		unguardobj;
	}

	// UFont interface.
	UFont(UTexture *Texture);
	void StrLen(int &XL, int &YL, int XSpace, int YSpace, const char *Text,int iStart=0,int NumChars=MAXINT);
	void WrappedStrLen(int &XL, int &YL, int XSpace, int YSpace, int Width, const char *Text);
	void VARARGS Printf(UTexture *DestTexture,int X, int Y, int XSpace, const char *Fmt,...);
	void VARARGS WrappedPrintf(UTexture *DestTexture,int X, int Y, int XSpace, int YSpace, int Width, int Center, const char *Fmt,...);
	void Cout(UTexture *DestTexture,int X, int Y, int XSpace, char C,RAINBOW_PTR Colors);
};

/*-----------------------------------------------------------------------------
	Bit arrays.
-----------------------------------------------------------------------------*/

//
// A bit array.
//
class UNENGINE_API UBitArray : public UBuffer
{
	DECLARE_DB_CLASS(UBitArray,UBuffer,DWORD,NAME_BitArray,NAME_UnEngine)

	// Variables.
	DWORD NumBits;

	// Constructor.
	UBitArray( DWORD InNumBits )
	:	UBuffer((InNumBits+31)/32,1),
		NumBits(InNumBits)
	{}

	// Serializer.
	void SerializeHeader(FArchive &Ar)
	{
		guard(UBitArray::SerializeHeader);
		UBuffer::SerializeHeader(Ar);
		Ar << NumBits;
		unguardobj;
	}
	void SerializeData(FArchive &Ar);

	// Bit getter.
	BOOL Get( DWORD i )
	{
		if( i>=(DWORD)NumBits ) appErrorf("UBitArray::Get: %i/%i",i,NumBits);
		return (Element(i/32) & (1 << (i&31))) != 0;
	}

	// Bit setters.
	void Set( DWORD i, BOOL Value )
	{
		if( i>=(DWORD)NumBits ) appErrorf("UBitArray::Set: %i/%i",i,NumBits);
		if( Value ) Element(i/32) |= 1 << (i&31);
		else        Element(i/32) &= ~(1 << (i&31));
	}
};

//
// An nxn symmetric bit array.
//
class UNENGINE_API UBitMatrix : public UBitArray
{
	DECLARE_DB_CLASS(UBitMatrix,UBitArray,DWORD,NAME_BitMatrix,NAME_UnEngine)

	// Variables.
	DWORD Side;

	// Constructor.
	UBitMatrix( DWORD InSide )
	:	UBitArray( InSide * (InSide + 1)/2 ),
		Side(InSide)
	{}

	// Serializer.
	void SerializeHeader(FArchive &Ar)
	{
		guard(UBitMatrix::SerializeHeader);
		UBitArray::SerializeHeader(Ar);
		Ar << Side;
		unguardobj;
	}

	// Bit getter.
	BOOL Get( DWORD i, DWORD j )
	{
		//debugf("Get %i,%i -> %i",i,j,(i<=j) ? (i + j*(j+1)/2) : (j + i*(i+1)/2));
		if( i>=Side || j>=Side ) appErrorf("UBitMatrix::Get: %i,%i/%i",i,j,Side);
		return UBitArray::Get( (i<=j) ? (i + j*(j+1)/2) : (j + i*(i+1)/2) );
	}

	// Bit setters.
	void Set( DWORD i, DWORD j, BOOL Value )
	{
		//debugf("Set %i,%i -> %i",i,j,(i<=j) ? (i + j*(j+1)/2) : (j + i*(i+1)/2));
		if( i>=Side || j>=Side ) appErrorf("UBitMatrix::Set: %i,%i/%i",i,j,Side);
		UBitArray::Set( (i<=j) ? (i + j*(j+1)/2) : (j + i*(i+1)/2), Value );
	}
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNOBJ
