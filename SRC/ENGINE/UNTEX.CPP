/*=============================================================================
	UnTex.cpp: Unreal texture loading/saving/processing functions.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Texture locking and unlocking.
-----------------------------------------------------------------------------*/

//
// Lock a texture for rendering.
//
INT UTexture::Lock
(
	FTextureInfo	&TextureInfo,
	UTexture		*Camera,
	DWORD			InFlags,
	AZoneInfo    	*InZone
)
{
	guard(UTexture::Lock);

	// Lock this texture object.
	INT Result = Lock(LOCK_Read);

	// Get all mips.
	for( int i=0; i<MAX_MIPS; i++ )
		TextureInfo.Mips[i] = &Mips[i];

	// Set locked texture info.
	TextureInfo.Zone	= InZone;
	TextureInfo.Flags	= TextureFlags | InFlags;
	TextureInfo.Palette	= Palette;
	TextureInfo.Colors.PtrVOID = NULL;

	if( InFlags & TL_AnyRender )
	{
		// Call the rendering engine to perform any additional setup/locking.
		GRend->PostLockTexture(TextureInfo,Camera);
	}

	// Success.
	return Result;
	unguard;
}

//
// Unlock a texture for rendering.
//
void UTexture::Unlock( FTextureInfo &TextureInfo )
{
	guard(UTexture::Unlock);

	if( TextureInfo.Flags & TL_AnyRender )
	{
		// Call the rendering engine to perform any additional unlocking.
		GRend->PreUnlockTexture(TextureInfo);
	}

	// Unlock this texture object.
	Unlock(LOCK_Read);
	unguard;
}

/*---------------------------------------------------------------------------------------
	PCX stuff for texture and palette importers.
---------------------------------------------------------------------------------------*/

//
// 128-byte header found at the beginning of a ".PCX" file.
// (Thanks to Ammon Campbell for putting this together)
//
class FPCXFileHeader
{
public:
	BYTE	Manufacturer;		// Always 10.
	BYTE	Version;			// PCX file version.
	BYTE	Encoding;			// 1=run-length, 0=none.
	BYTE	BitsPerPixel;		// 1,2,4, or 8.
	WORD	XMin;				// Dimensions of the image.
	WORD	YMin;				// Dimensions of the image.
	WORD	XMax;				// Dimensions of the image.
	WORD	YMax;				// Dimensions of the image.
	WORD	hdpi;				// Horizontal printer resolution.
	WORD	vdpi;				// Vertical printer resolution.
	BYTE	OldColorMap[48];	// Old colormap info data.
	BYTE	Reserved1;			// Must be 0.
	BYTE	NumPlanes;			// Number of color planes (1, 3, 4, etc).
	WORD	BytesPerLine;		// Number of bytes per scanline.
	WORD	PaletteType;		// How to interpret palette: 1=color, 2=gray.
	WORD	HScreenSize;		// Horizontal monitor size.
	WORD	VScreenSize;		// Vertical monitor size.
	BYTE	Reserved2[54];		// Must be 0.
};

/*--------------------------------------------------
    BMP stuff for texture and palette importers
--------------------------------------------------*/

#pragma pack(push,1)

// Headers found at the beginning of a ".BMP" file.
class MYBITMAPFILEHEADER
{
public:
    unsigned short   bfType;         /* Always "BM" */
    unsigned long    bfSize;         /* Size of file in bytes. */
    unsigned short   bfReserved1;    /* Ignored. */
    unsigned short   bfReserved2;    /* Ignored. */
    unsigned long    bfOffBits;      /* Offset of bitmap in file. */
};

class MYBITMAPINFOHEADER
{
public:
    unsigned long    biSize;            /* Size of header in bytes. */
    unsigned long    biWidth;           /* Width of bitmap in pixels. */
    unsigned long    biHeight;          /* Height of bitmap in pixels. */
    unsigned short   biPlanes;          /* Number of bit planes (always 1). */
    unsigned short   biBitCount;        /* Number of bits per pixel. */
    unsigned long    biCompression;     /* Type of compression (ingored). */
    unsigned long    biSizeImage;       /* Size of pixel array (usually 0). */
    unsigned long    biXPelsPerMeter;   /* Ignored. */
    unsigned long    biYPelsPerMeter;   /* Ignored. */
    unsigned long    biClrUsed;         /* Number of colors (usually 0). */
    unsigned long    biClrImportant;    /* Important colors (usually 0). */
};

#pragma pack(pop)

/*---------------------------------------------------------------------------------------
	UTexture general functions.
---------------------------------------------------------------------------------------*/

//
// Initialize a remap table.
//
void UTexture::InitRemap( BYTE *Remap )
{
	guard(UTexture::InitRemap);
	for( int i=0; i<UPalette::NUM_PAL_COLORS; i++ )
		Remap[i] = 0;
	Remap[UPalette::NUM_PAL_COLORS-1] = UPalette::NUM_PAL_COLORS-1;
	unguard;
}

//
// Remap a texture according to a remap table.
//
void UTexture::DoRemap(BYTE *Remap)
{
	guard(UTexture::DoRemap);
	Lock(LOCK_ReadWrite);

	for( int i=0; i<Max; i++ )
		Element(i) = Remap[Element(i)];

	Unlock(LOCK_ReadWrite);
	unguard;
}

//
// Remap a texture's palette from its current palette to
// the specified destination palette.
//
void UTexture::Remap(UPalette *SourcePalette, UPalette *DestPalette)
{
	guard(UTexture::Remap);
	BYTE Remap[UPalette::NUM_PAL_COLORS];

	// Find all best-match colors.  Remap first and last 10 (Windows) colors to mask.
	// Don't remap any colors to zero (black).
	SourcePalette->Lock(LOCK_Read);

	if (PolyFlags & PF_Masked)	Remap[0] = 0;
	else						Remap[0] = DestPalette->BestMatch(SourcePalette->Element(0));

	for( int i=1; i<UPalette::NUM_PAL_COLORS; i++ )
		Remap[i] = DestPalette->BestMatch(SourcePalette->Element(i));

	SourcePalette->Unlock(LOCK_Read);

	// Perform the remapping.
	DoRemap(Remap);

	unguard;
}

//
// Rearrange a texture's colors to fit the additive-lighting palette.
//
void UTexture::Fixup()
{
	guard(UTexture::Fixup);

	BYTE Remap[UPalette::NUM_PAL_COLORS];

	InitRemap(Remap);
	for( int iColor=0; iColor<8; iColor++ )
	{
		int iStart = (iColor==0) ? 1 : iColor * 32;
		for( int iShade=0; iShade<28; iShade++ )
			Remap[iStart + iShade] = 32 + iColor + (iShade<<3);
	}
	DoRemap(Remap);

	unguard;
}

/*---------------------------------------------------------------------------------------
	UTexture object implementation.
---------------------------------------------------------------------------------------*/

void UTexture::InitHeader()
{
	guard(UTexture::InitHeader);

	// Init UDatabase info.
	UDatabase::InitHeader();

	// Init UTexture info.
	Palette			= (UPalette *)NULL;

	DiffuseC		= 1.0;
	SpecularC		= 0.0;
	FrictionC		= 0.5;
	ReflectivityC	= 0.5;

	FootstepSound	= NULL;
	HitSound		= NULL;

	PolyFlags		= 0;
	TextureFlags	= 0;

	UBits			= 0;
	VBits			= 0;

	USize			= 0;
	VSize			= 0;
	ColorBytes		= 0;
	CameraCaps		= 0;

	MipZero			= FColor(0,0,0,0);
	MinColor		= FColor(0,0,0,0);
	MaxColor		= FColor(255,255,255,255);

	for( int i=0; i<MAX_MIPS; i++ )
		Mips[i].Offset = MAXDWORD;

	unguard;
}
const char *UTexture::Import(const char *Buffer, const char *BufferEnd,const char *FileType)
{
	guard(UTexture::Import);

	FPCXFileHeader	*PCX = (FPCXFileHeader *)Buffer;
    MYBITMAPINFOHEADER *bmhdr = (MYBITMAPINFOHEADER *)(Buffer + sizeof(MYBITMAPFILEHEADER));
	INT				RunLength;
	BYTE  			Color,*DestEnd,*DestPtr;
    INT             iScanWidth;

	// Validate it.
	int Length = (int)(BufferEnd - Buffer);
	if( Length < sizeof(FPCXFileHeader) )
	{
		// Doesn't contain valid header.
		return NULL;
	}

    // Is this a .BMP or .PCX data stream?
    if( Buffer[0] == 'B' && Buffer[1] == 'M' )
    {
        // This is a .BMP type data stream.
        if( bmhdr->biPlanes!=1 || bmhdr->biBitCount!=8 )
        {
			// Not a 256 color BMP image.
            debugf("BMP is not 8-bit color! planes==%i bits==%i", bmhdr->biPlanes, bmhdr->biBitCount);
            return NULL;
        }

        // Set texture properties.
        USize        = bmhdr->biWidth;
        VSize        = bmhdr->biHeight;

		// Allocate space.
		Num = Max = USize * VSize;
		Realloc();

		// Import it.
		Lock(LOCK_ReadWrite);
		DestPtr	= &Element(0);

        Buffer     += sizeof(MYBITMAPFILEHEADER); // Skip 1st bmp header.
        Buffer     += sizeof(MYBITMAPINFOHEADER); // Skip 2nd bmp header.
        Buffer     += 4 * 256;                    // Skip palette data.
        iScanWidth  = bmhdr->biWidth;

		// Get correct source scanline width.
        while( iScanWidth % 4 )
            iScanWidth++;

		// Copy scanlines.
		for( int y = 0; y < (int)bmhdr->biHeight; y++ )
        {
            // Copy a scanline.
            // Note that scanlines in BMP file are upside-down.
            memcpy
			(
                &DestPtr[(bmhdr->biHeight - 1 - y) * bmhdr->biWidth],
                &Buffer [y * iScanWidth],
                bmhdr->biWidth
			);
        }
		Unlock(LOCK_ReadWrite);
    }
	else if( PCX->Manufacturer == 10 )
	{
		// This is a .PCX.
		if( PCX->BitsPerPixel!=8 || PCX->NumPlanes!=1 )
		{
			// Bad format, must have 8 bits per pixel, 1 plane.
			return NULL;
		}

		// Set texture properties.
		USize		= PCX->XMax + 1 - PCX->XMin;
		VSize		= PCX->YMax + 1 - PCX->YMin;

		// Allocate space.
		Num = Max	= USize * VSize;
		Realloc();

		// Import it.
		Lock(LOCK_ReadWrite);
		DestPtr	= &Element(0);
		DestEnd	= DestPtr + Max;
		Buffer += 128;
		while( DestPtr < DestEnd )
		{
			Color = *Buffer++;
			if( (Color & 0xc0) == 0xc0 )
			{
				RunLength = Color & 0x3f;
				Color     = *Buffer++;
				memset (DestPtr,Color,Min(RunLength,(int)(DestEnd - DestPtr)));
				DestPtr  += RunLength;
			}
			else *DestPtr++ = Color;
		}
		Unlock(LOCK_ReadWrite);
	}
	else
	{
		// Unknown format.
        debugf("Bad image format for texture import.");
        return NULL;
 	}

	// Init info.
	Palette	= (UPalette *)NULL;
    UBits   = FLogTwo(USize);
    VBits   = FLogTwo(VSize);
    Mips[0].Offset = 0;

	// Set tile info.
	TextureFlags = (TextureFlags & ~TF_NoTile);
	if( (USize & (USize-1)) || (VSize & (VSize-1)) || (USize>1024) || (VSize>1024) )
		TextureFlags |= TF_NoTile;

	// Fix it up.
	PostLoadData(0);

	return BufferEnd;
	unguard;
}
void UTexture::Export(FOutputDevice &Out,const char *FileType,int Indent)
{
	guard(UTexture::Export);

	// Export to PCX.
	FColor	*Colors;
	DWORD 	XYSize,i;
	BYTE  	Color,*ScreenPtr;

	// Set all PCX file header properties.
	FPCXFileHeader PCX;
	memset(&PCX,0,sizeof(PCX));
	PCX.Manufacturer	= 10;
	PCX.Version			= 05;
	PCX.Encoding		= 1;
	PCX.BitsPerPixel	= 8;
	PCX.XMin			= 0;
	PCX.YMin			= 0;
	PCX.XMax			= USize-1;
	PCX.YMax			= VSize-1;
	PCX.hdpi			= USize;
	PCX.vdpi			= VSize;
	PCX.NumPlanes		= 1;
	PCX.BytesPerLine	= USize;
	PCX.PaletteType		= 0;
	PCX.HScreenSize		= 0;
	PCX.VScreenSize		= 0;
	Out.Write(&PCX,sizeof(PCX));

	// Special PCX RLE code.
	BYTE RLE=0xc1;

	// Copy all RLE bytes.
	XYSize    = USize * VSize;
	ScreenPtr = &Element(Mips[0].Offset);
	for( i=0; i<XYSize; i++ )
	{
		Color = *(ScreenPtr++);
		if( (Color&0xc0)!=0xc0 )
		{
			// No run length required.
			Out.Write(&Color,1);
		}
		else
		{
			// Run length = 1.
			Out.Write(&RLE,1);
			Out.Write(&Color,1);
		}
	}

	// Build palette.
	BYTE Extra = 12;
	Out.Write(&Extra,1); // Required before palette by PCX format.

	if (!Palette)	Colors = &GGfx.DefaultPalette->Element(0);
	else			Colors = &Palette->Element(0);
	for( i=0; i<UPalette::NUM_PAL_COLORS; i++ )
	{
		Out.Write(&Colors[i].R,1);
		Out.Write(&Colors[i].G,1);
		Out.Write(&Colors[i].B,1);
	}
	unguard;
}
void UTexture::PostLoadData( DWORD PostFlags )
{
	guard(UTexture::PostLoadData);
	Lock(LOCK_Read);

	// Fix up the mipmap info.
	for( int i=0; i<MAX_MIPS; i++ )
	{
		FMipInfo &Mip = Mips[i];
		if( Mip.Offset != MAXDWORD )
		{
			Mip.USize		= USize >> i;
			Mip.VSize		= VSize >> i;
			Mip.MipLevel	= i;
			Mip.UBits		= UBits - i;
			Mip.VBits		= VBits - i;
			Mip.Unused1		= 0;
			Mip.VMask		= Mip.VSize - 1;
			Mip.AndMask		= ((Mip.VSize-1) + ((Mip.USize-1) << (32-Mip.UBits)));
			Mip.Data		= &Element(Mip.Offset);
			Mip.Dither		= NULL;
		}
		else 
		{
			checkState(i>0);
			Mips[i] = Mips[i-1];
			Mips[i].Offset = MAXDWORD;
		}
	}

	// Give the rendering engine the opportunity to do any rendering engine
	// specific texture loading.
	GRend->PostLoadTexture( this, PostFlags );

	Unlock(LOCK_Read);
	unguard;
}
void UTexture::PreKill()
{
	guard(UTexture::PreKill)

	// Give the rendering engine the opportunity to do any rendering engine
	// specific texture killing.
	GRend->PreKillTexture(this);

	unguard;
}
IMPLEMENT_DB_CLASS(UTexture);

/*---------------------------------------------------------------------------------------
	UTextureSet.
---------------------------------------------------------------------------------------*/

IMPLEMENT_DB_CLASS(UTextureSet);

/*---------------------------------------------------------------------------------------
	UTexture mipmap generation.
---------------------------------------------------------------------------------------*/

#define DO_RGB(x) x(R); x(G); x(B); /* Macro for doing the x thing to each RGB component */

//
// Generate all mipmaps for a texture.  Call this after setting the
// texture's palette.
//
void UTexture::CreateMips(int FullMips)
{
	guard(UTexture::CreateMips);

	Lock(LOCK_ReadWrite);
	UPalette *ThisPalette = Palette ? Palette : GGfx.DefaultPalette;
	ThisPalette->Lock(LOCK_Read);
	FColor *Colors = &ThisPalette->Element(0);

	FColor		*SourceColor,ResultColor;
	BYTE		*SourceTex,*DestTex,B;
	INT			ThisUSize,ThisVSize,HalfUSize,HalfVSize,U,V,X,Y,UAnd,VAnd,Red,Green,Blue,MipLevel,ThisSize,i,n;
	static const int BoxC[4][4] =
	{
		{ 5, 8, 5, 0},
		{ 8,12, 8, 0},
		{ 5, 8, 5, 0},
		{ 0, 0, 0, 0}
	};
	const int BoxCSize=3;
	enum {BOX_SHIFT = 6};
	enum {BOX_SUM   = 64};

	// Create average color (lowest mipmap).
	Red  = 0; Green = 0; Blue = 0; 
	for( i=0; i<Max; i++ )
	{
		B      = Element(i);
		Red   += Colors[B].R;
		Green += Colors[B].G;
		Blue  += Colors[B].B;
	}
	MipZero = FColor(Red/i,Green/i,Blue/i,0);
	MipZero.RemapIndex = GGfx.DefaultPalette->BestMatch(MipZero,0);

	if( FullMips && Mips[1].Offset==MAXDWORD)
	{
		// Create the mips.
		Max = 0;
		for( MipLevel=MAX_MIPS-1; MipLevel>=0; MipLevel-- )
		{
			ThisSize = (USize >> MipLevel)*(VSize >> MipLevel);

			if (ThisSize>0) Mips[MipLevel].Offset = Max;
			else			Mips[MipLevel].Offset = MAXDWORD;

			Max += ThisSize;
		}
		Num = Max;
		Realloc();

		if( TextureFlags & TF_Blur )
		{
			FMemMark Mark(GMem);
			FColor *C = &ThisPalette->Element(0);
			BYTE *Temp = new(GMem,USize*VSize)BYTE;
			memcpy(Temp,&Element(0),USize*VSize);
			BYTE *D1 = &Temp[(VSize-1)*USize];
			BYTE *D2 = &Temp[0];
			BYTE *D  = &Element(0);
			for( int vv=0; vv<VSize; vv++ )
			{
				for( int uu=0; uu<USize; uu++ )
				{
					int uuu=(uu+1)&(USize-1);
					FColor T;

					T.R = ((int)C[D1[uu]].R + C[D1[uuu]].R + C[D2[uu]].R + C[D2[uuu]].R )>>2;
					T.G = ((int)C[D1[uu]].G + C[D1[uuu]].G + C[D2[uu]].G + C[D2[uuu]].G )>>2;
					T.B = ((int)C[D1[uu]].B + C[D1[uuu]].B + C[D2[uu]].B + C[D2[uuu]].B )>>2;

					*D++ = ThisPalette->BestMatch(T,0);
				}
				D1  = D2;
				D2 += USize;
			}
			Mark.Pop();
		}

		memmove(&Element(Max - USize*VSize),&Element(0),USize*VSize);
		memset(&Element(0),3,Max - USize*VSize);

		// Build each mip from the next-larger mip.
		ThisUSize		= USize;
		ThisVSize		= VSize;

		for( MipLevel=1; MipLevel<MAX_MIPS && Mips[MipLevel].Offset!=MAXDWORD; MipLevel++ )
		{
			SourceTex	= &Element(Mips[MipLevel-1].Offset);
			DestTex		= &Element(Mips[MipLevel  ].Offset);

			HalfUSize	= ThisUSize>>1;
			HalfVSize	= ThisVSize>>1;

			UAnd		= ThisUSize-1;
			VAnd		= ThisVSize-1;

			if( !(PolyFlags & PF_Masked) )
			{
				// Simple (non masked) mipmap.
				for( U=0; U<HalfUSize; U++) for (V=0; V<HalfVSize; V++ )
				{
					if (U&1) GApp->StatusUpdate ("Creating mipmap",U,HalfUSize);

					Red=0; Green=0; Blue=0;
					for( X=0; X<BoxCSize; X++) for (Y=0; Y<BoxCSize; Y++ )
					{
						SourceColor = &Colors[SourceTex[((V*2+Y-1)&VAnd)*ThisUSize + ((U*2+X-1)&UAnd)]];
						Red   += BoxC[X][Y]*(int)SourceColor->R;
						Green += BoxC[X][Y]*(int)SourceColor->G;
						Blue  += BoxC[X][Y]*(int)SourceColor->B;
					}
					ResultColor = FColor(Red>>BOX_SHIFT,Green>>BOX_SHIFT,Blue>>BOX_SHIFT);
					DestTex[V*HalfUSize+U] = ThisPalette->BestMatch(ResultColor,0);
				}
			}
			else
			{
				// Masked mipmap.
				for( U=0; U<HalfUSize; U++) for (V=0; V<HalfVSize; V++ )
				{
					if (U&1) GApp->StatusUpdate ("Creating mipmap",U,HalfUSize);

					n = 0;
					Red=0; Green=0; Blue=0;
					for( X=0; X<BoxCSize; X++) for (Y=0; Y<BoxCSize; Y++ )
					{
						B = SourceTex[((V*2+Y-1)&VAnd)*ThisUSize + ((U*2+X-1)&UAnd)];
						if( B )
						{
							n     += BoxC[X][Y];
							SourceColor = &Colors[B];
							Red   += BoxC[X][Y]*(int)SourceColor->R;
							Green += BoxC[X][Y]*(int)SourceColor->G;
							Blue  += BoxC[X][Y]*(int)SourceColor->B;
						}
					}
					if( (n*2) >= BOX_SUM )
					{
						// Mostly unmasked - keep it.
						ResultColor = FColor(Red/n,Green/n,Blue/n);
						B           = ThisPalette->BestMatch(ResultColor,0);
					}
					else
					{
						// Mostly masked - remain masked.
						B = 0;
					}

					DestTex[V*HalfUSize+U] = B;
				}
			}
			ThisUSize = ThisUSize >> 1;
			ThisVSize = ThisVSize >> 1;
		}
	}
	ThisPalette->Unlock(LOCK_Read);
	Unlock(LOCK_ReadWrite);

	// Fix it up.
	PostLoadData(0);

	unguard;
}

//
// Set the texture's MaxColor and MinColor so that the texture can be normalized
// when converting to lower color resolutions like RGB 5-5-5 for hardware
// rendering.
//
void UTexture::CreateColorRange()
{
	guard(UTexture::SetMaxColor);
	if( Palette )
	{
		Lock(LOCK_Read);
		Palette->Lock(LOCK_Read);

		// Compute max R, G, B.
		MinColor = FColor(255,255,255,255);
		MaxColor = FColor(0,0,0,0);
		for( int i=0; i<Max; i++ )
		{
			FColor &Color = Palette->Element(Element(i));
			MinColor.R    = ::Min(MinColor.R, Color.R);
			MinColor.G    = ::Min(MinColor.G, Color.G);
			MinColor.B    = ::Min(MinColor.B, Color.B);
			MinColor.A    = ::Min(MinColor.A, Color.A);
			MaxColor.R    = ::Max(MaxColor.R, Color.R);
			MaxColor.G    = ::Max(MaxColor.G, Color.G);
			MaxColor.B    = ::Max(MaxColor.B, Color.B);
			MaxColor.A    = ::Max(MaxColor.A, Color.A);
		}
		Palette->Unlock(LOCK_Read);
		Unlock(LOCK_Read);
	}
	else
	{
		MinColor = FColor(0,0,0,0);
		MaxColor = FColor(255,255,255,255);
	}
	unguard;
}

/*---------------------------------------------------------------------------------------
	Adaptive binary tree quantization routines.
---------------------------------------------------------------------------------------*/

//
// Template containing one set of multidimensional items that are being quantized.
//
template <class TQuantizant> class TQuantizantSet
{
public:
	// Constants.
	enum {NUM_COMPONENTS = TQuantizant::NUM_COMPONENTS};

	// Variables.
	int				NumQuantizants;				// Number of things in this set.
	int				MaxQuantizants;				// Maximum quantizants that can fit here.
	TQuantizant		*Quantizants;				// Pointer to a pool of sets.
	FLOAT			Averages[NUM_COMPONENTS];	// Averages per component.
	INT				IntAverages[NUM_COMPONENTS];// Integer averages per component.
	INT				RmsErrors[NUM_COMPONENTS];	// RMS error per component.
	TQuantizant		Average;					// Average quantizant.

	// Constructor.
	TQuantizantSet(FMemStack &Mem,int InMax)
	:	MaxQuantizants	(InMax),
		NumQuantizants	(0),
		Quantizants		(new(Mem,InMax)TQuantizant)
	{}

	// Add an item to this set.
	void AddItem( TQuantizant &Item )
	{
		checkLogic(NumQuantizants<MaxQuantizants);
		Quantizants[NumQuantizants++] = Item;
	}

	// Compute the RMS error values of each component in this set.
	void ComputeRmsErrors()
	{
		guard(TQuantizantSet::ComputeRmsErrors);
		checkInput(NumQuantizants>0);

		// Init averages and RmsErrors.
		for( int i=0; i<NUM_COMPONENTS; i++ )
		{
			IntAverages[i] = RmsErrors[i] = 0;
			Averages[i] = 0.0;
		}


		// Compute averages.
		for( i=0; i<NumQuantizants; i++ )
			for( int j=0; j<NUM_COMPONENTS; j++ )
				IntAverages[j] += Quantizants[i].GetComponent(j);

		// Finalize averages.
		for( int j=0; j<NUM_COMPONENTS; j++ )
		{
			Averages[j] = (FLOAT)IntAverages[j]/NumQuantizants;
			IntAverages[j] = Averages[j];
			Average.SetComponent(j,IntAverages[j]);
		}

		// Compute RMS errors.
		for( i=0; i<NumQuantizants; i++ )
			for( int j=0; j<NUM_COMPONENTS; j++ )
				RmsErrors[j] += Abs(Quantizants[i].GetComponent(j) - IntAverages[j]);

		unguard;
	}

	// Subdivide the set by the specified component into two subsets,
	// minimizing the total RMS error of the subsets. ComputeRmsErrors must
	// have already been called.
	void SplitInHalf(int iComponent, TQuantizantSet &A, TQuantizantSet &B)
	{
		guard(TQuantizantSet::SplitInHalf);
		checkLogic(NumQuantizants>1);

		// Split up into two subsets.
		for( int i=0; i<NumQuantizants; i++ )
		{
			if( Quantizants[i].GetComponent(iComponent) <= Averages[iComponent] )
				A.AddItem( Quantizants[i] );
			else
				B.AddItem( Quantizants[i] );
		}
		checkLogic(A.NumQuantizants>0);
		checkLogic(B.NumQuantizants>0);

		// Compute each subset's errors.
		A.ComputeRmsErrors();
		B.ComputeRmsErrors();
		unguard;
	}
};

//
// Template to perform adaptive binary tree quantization on a class which
// exposes NUM_COMPONENTS and INT Component(int i). This continually
// subdivides the input set into halves, opting to split the set of
// maximum RMS error at each decision point.
//
template <class TQuantizant, int MAX_SETS> class TQuantizer
{
public:
	// Variables.
	TQuantizantSet<TQuantizant> *Sets[MAX_SETS];
	int NumSets;

	// Constructor. Quantizes the set. Upon return, NumSets and Sets are valid
	// and the caller is responsible for turning them into meaningful results, i.e.
	// a quantized palette.
	TQuantizer( FMemStack &Mem, TQuantizant *Items, int NumItems )
	:	NumSets(0)
	{
		guard(TQuantizer::TQuantizer);

		// Create the starting set.
		Sets[NumSets++] = new(Mem)TQuantizantSet<TQuantizant>( Mem, NumItems );
		for( int i=0; i<NumItems; i++ )
			Sets[0]->AddItem( Items[i] );
		Sets[0]->ComputeRmsErrors();

		// Keep splitting the most erroneous set until either all sets are
		// maximally split or we have no more splittable sets.
		while( NumSets < MAX_SETS )
		{
			// Find the worst set.
			INT   WorstRmsError   = 0;
			INT   iWorstSet       = 0;
			INT   iWorstComponent = 0;

			for( i=0; i<NumSets; i++ )
			{
				for( int j=0; j<TQuantizant::NUM_COMPONENTS; j++ )
				{
					// See if this is a splittable set with the worst RMS error.
					if
					(
						Sets[i]->RmsErrors[j]   > WorstRmsError 
					&&	Sets[i]->NumQuantizants > 1
					)
					{
						WorstRmsError   = Sets[i]->RmsErrors[j];
						iWorstSet       = i;
						iWorstComponent = j;
					}
				}
			}

			// See if we found a splittable set.
			if( WorstRmsError )
			{
				// We found a splittable set, so create two subsets and split it up.
				TQuantizantSet<TQuantizant> *Set = Sets[iWorstSet];
				TQuantizantSet<TQuantizant> &A   = *new(Mem)TQuantizantSet<TQuantizant>(Mem,Set->NumQuantizants);
				TQuantizantSet<TQuantizant> &B   = *new(Mem)TQuantizantSet<TQuantizant>(Mem,Set->NumQuantizants);
				Set->SplitInHalf(iWorstComponent,A,B);
				//debugf("Split %i -> %i,%i",Set->NumQuantizants,A.NumQuantizants,B.NumQuantizants);

				// Add the two subsets to the list of sets (replace the split up one).
				Sets[iWorstSet] = &A;
				Sets[NumSets++] = &B;
			}
			else
			{
				// We are done; we've exausted all sets.
				break;
			}
		}
		unguard;
	}

	// Return the nearest item matching the specified one.
	int GetNearest( TQuantizant &Q )
	{
		INT Min   = MAXINT;
		INT iBest = 0;
		for( int i=0; i<NumSets; i++ )
		{
			INT Error = Q.Distance(Sets[i]->Average);
			if( Error < Min )
			{
				Min   = Error;
				iBest = i;
			}
		}
		return iBest;
	}
};

/*---------------------------------------------------------------------------------------
	UTexture bump mapping functions.
---------------------------------------------------------------------------------------*/

//
// A bumpmap normal.
//
class FBumpMapNormal : public FColor
{
public:
	// Quantization constants.
	enum {NUM_COMPONENTS=2};

	INT GetComponent(int i)
	{
		return i ? NormalV : NormalU;
	}
	void SetComponent(int i,INT F)
	{
		if( i )	NormalV = Clamp(F,-128,+127);
		else	NormalU = Clamp(F,-128,+127);
	}
	INT Distance( FBumpMapNormal &Other )
	{
		return
		+	Abs((int)NormalU-(int)Other.NormalU)
		+	Abs((int)NormalV-(int)Other.NormalV);
	}
};

//
// Build a power-of-two rectangular bump map.
//
static void BuildBumpMapFrom
(
	UPalette::Ptr	Pal, 
	FBumpMapNormal	*Dest, 
	BYTE			*Source, 
	int				USize, 
	int				VSize
)
{
	guard(BuildBumpMapFrom);

	for( int V=0; V<VSize; V++ )
	{
		for( int U=0; U<USize; U++ )
		{
			// Get three source texels.
			FLOAT A  = Pal(Source[U                 + V                *USize]).FBrightness();
			FLOAT B  = Pal(Source[((U+1)&(USize-1)) + V                *USize]).FBrightness();
			FLOAT C  = Pal(Source[U                 + ((V+1)&(VSize-1))*USize]).FBrightness();
			FLOAT D  = Pal(Source[((U+1)&(USize-1)) + ((V+1)&(VSize-1))*USize]).FBrightness();

			// Get the gradients.
			FLOAT DU = 0.5 * (B-A + D-C);
			FLOAT DV = 0.5 * (C-A + D-B);

			// Nonlinear bias the gradients.
			DU       = Sgn(DU) * sqrt(Abs(DU));
			DV       = Sgn(DV) * sqrt(Abs(DV));

			// Set the bump map normal, -128 to +127.
			Dest->NormalU = 127.0 * DU;
			Dest->NormalV = 127.0 * DV;
			//debugf("%i %i",Dest->NormalU,Dest->NormalV);
			Dest++;
		}
	}
	unguard;
}

//
// Turn this texture into a mipmapped bump map based on
// Source, which is assumed to be a 256-value heightmap
// texture.
//
void UTexture::CreateBumpMap()
{
	guard(UTexture::CreateBumpMapFrom);
	checkState(Palette!=NULL);

	GApp->StatusUpdate ("Creating bump map",0,0);

	// Remember the original data.
	Lock(LOCK_ReadWrite);
	FMemMark Mark(GMem);

	// Allocate bump map normals.
	FBumpMapNormal *Normals = new(GMem,Max)FBumpMapNormal;

	// Make each mip.
	for( int i=0; i<MAX_MIPS; i++ )
	{
		if( Mips[i].Offset != MAXDWORD )
		{
			BuildBumpMapFrom
			(
				Palette,
				&Normals[Mips[i].Offset],
				&Element(Mips[i].Offset),
				USize >> i,
				VSize >> i
			);
		}
	}

	// Quantize the bump map normals.
	TQuantizer<FBumpMapNormal,256> Quantizer(GMem,Normals,Max);
	debugf("Quantized %i bump map normals",Quantizer.NumSets);

	// Build the texture's bump map normal palette based on the quantizer's results.
	Palette->Num = Quantizer.NumSets;
	for( i=0; i<Quantizer.NumSets; i++ )
		Palette(i) = Quantizer.Sets[i]->Average;

	// Remap the texture to the new bump map normal palette.
	for( i=0; i<Max; i++ )
		Element(i) = Quantizer.GetNearest(Normals[i]);

	// Set mip flag.
	TextureFlags |= TF_BumpMap;

	Unlock(LOCK_ReadWrite);
	Mark.Pop();
	GCache.Flush();

	// Fix it up.
	PostLoadData(0);

	unguard;
}

//
// Darken a rectangular area of the texture.
//
void UTexture::BurnRect
(
	int			X1,
	int			X2,
	int			Y1,
	int			Y2,
	int			Bright
)
{
	guard(UTexture::BurnRect);
	BYTE			*Data = &Element(0);
	BYTE			*Dest1,*Dest;
	DWORD			*DDest1,*DDest;
	WORD			*WDest1,*WDest;
	int				i,j;

	X1 = Clamp(X1,0,USize);
	X2 = Clamp(X2,0,USize);
	Y1 = Clamp(Y1,0,VSize);
	Y2 = Clamp(Y2,0,VSize);

	if( ColorBytes==1 )
	{
		Dest1  = &Data [X1 + Y1*USize];
		Bright = Bright?0x00:0x40;
		for( i=Y1; i<Y2; i++ )
		{
			Dest = Dest1;
			for( j=X1; j<X2; j++ )
			{
				*Dest = 0x10 + Bright + (*Dest & 7) + ((*Dest & 0xf0) >> 1);
				Dest++;
			}
			Dest1 += USize;
		}
	}
	else if( ColorBytes==2 )
	{
		WDest1    = (WORD *)&Data[2*(X1 + Y1*USize)];
		for( i=Y1; i<Y2; i++ )
		{
			WDest = WDest1;
			if( CameraCaps & CC_RGB565 )
			{
				WORD Temp = Bright ? (0x7800+0x03e0+0x000f) : 0;
				for (j=X1; j<X2; j++) {*WDest = Temp + ((*WDest & (0x001e + 0x07c0 + 0x0f000))>>1); WDest++;};
			}
			else
			{
				WORD Temp = Bright ? (0x3C00+0x01e0+0x000f) : 0;
				for (j=X1; j<X2; j++) {*WDest = Temp + ((*WDest & (0x001e + 0x03c0 + 0x07800))>>1); WDest++;};
			}
			WDest1 += USize;
		}
	}
	else
	{
		DDest1  = (DWORD *)&Data[4*(X1 + Y1*USize)];
		for( i=Y1; i<Y2; i++ )
		{
			DDest = DDest1;
			DWORD Temp = Bright ? 0x7f7f7f7f : 0;
			for( j=X1; j<X2; j++ )
			{
				*DDest = Temp + ((*DDest & 0xfefefefe)>>1);
				DDest++;
			}
			DDest1 += USize;
		}
	}
	unguard;
}

//
// Fill a region of memory with a DWORD.
//
void __inline FillDWORD( DWORD *Dest, DWORD D, int n )
{
#if ASM
	if( n > 0 ) __asm
	{
		mov edi,[Dest]
		mov esi,[n]
		mov eax,[D]

		FillLoop:
		mov [edi],eax
		add edi,4
		dec esi
		jg  FillLoop
	}
#else
	while (n-- > 0) *Dest++ = D;
#endif
}

//
// Clear the screen to a default palette color.
//
void UTexture::Clearscreen( BYTE c )
{
	guard(UTexture::Clearscreen);
	int	Fill, Size4 = (USize * ColorBytes) >> 2;
	Lock(LOCK_ReadWrite);
	Palette->Lock(LOCK_Read);
	if( ColorBytes==1 )
	{
		Fill = (DWORD)c + (((DWORD)c)<<8) + (((DWORD)c)<<16) + (((DWORD)c)<<24);
	}
	else if( ColorBytes==2 && (CameraCaps & CC_RGB565) )
	{
		int W = Palette(c).HiColor565();
		Fill = (W) + (W<<16);
	}
	else if( ColorBytes==2 )
	{
		int W = Palette(c).HiColor555();
		Fill = (W) + (W<<16);
	}
	else
	{
		Fill = Palette(c).TrueColor();
	}
	DWORD *Dest = (DWORD *)&Element(0);
	for( int i=0; i<VSize; i++ )
	{
		FillDWORD(Dest,Fill,Size4);
		Dest += (USize * ColorBytes) >> 2;
	}
	Palette->Unlock(LOCK_Read);
	Unlock(LOCK_ReadWrite);
	unguard;
}

/*---------------------------------------------------------------------------------------
	UPalette implementation.
---------------------------------------------------------------------------------------*/

//
// UObject interface.
//
const char *UPalette::Import(const char *Buffer, const char *BufferEnd,const char *FileType)
{
	guard(UPalette::Import);

	FPCXFileHeader	*PCX		 = (FPCXFileHeader *)Buffer;
	BYTE			*PCXPalette  = (BYTE *)(BufferEnd - NUM_PAL_COLORS * 3);
	int				BufferLength = (int)(BufferEnd-Buffer);
    MYBITMAPINFOHEADER *bmhdr    = (MYBITMAPINFOHEADER *)(Buffer + sizeof(MYBITMAPFILEHEADER));
    BYTE            *bmpal       = (BYTE *)(Buffer + sizeof(MYBITMAPINFOHEADER) + sizeof(MYBITMAPFILEHEADER));

	// Validate stuff in header.
	if( BufferLength < sizeof(FPCXFileHeader) )
		return NULL; // Doesn't contain valid header and palette.
	
	// Set the size.
	Num = Max = NUM_PAL_COLORS;
	Realloc();

    // Is this a .BMP or .PCX data stream?
    if( Buffer[0] == 'B' && Buffer[1] == 'M' )
    {
        // This is a .BMP type data stream.
        if( bmhdr->biPlanes != 1 || bmhdr->biBitCount != 8 )
        {
            debugf("BMP is not 8-bit color!  planes==%i bits==%i", bmhdr->biPlanes, bmhdr->biBitCount);
            return NULL; // Not a 256 color BMP image!
        }

		// Import all records.
		Lock(LOCK_ReadWrite);
		for( int i=0; i<NUM_PAL_COLORS; i++ )
        {
			Element(i).B = *bmpal++;
			Element(i).G = *bmpal++;
			Element(i).R = *bmpal++;
            *bmpal++;
        }
		Unlock(LOCK_ReadWrite);
    }
    else if( PCX->Manufacturer == 10 )
    {
		// This is a .PCX data stream.
		if( (PCX->BitsPerPixel!=8) || (PCX->NumPlanes!=1) )
		{
			debugf("Bad PCX format: must have 8 bits per pixel, 1 plane");
			return NULL;
		}

		// Import all records.
		Lock(LOCK_ReadWrite);
		for( int i=0; i<NUM_PAL_COLORS; i++ )
		{
			Element(i).R = *PCXPalette++;
			Element(i).G = *PCXPalette++;
			Element(i).B = *PCXPalette++;
		}
		Unlock(LOCK_ReadWrite);
	}
    else
    {
        // Unrecognized image format.
        debugf("Bad image format for palette import.");
        return NULL;
    }

	// Setup remapping table.
	BuildPaletteRemapIndex(0);

	// Success.
	return BufferEnd;
	unguard;
}
void UPalette::InitHeader()
{
	guard(UPalette::InitHeader);

	// Init parent class.
	UObject::InitHeader();

	// Set hardcoded number of colors.
	Num = Max = NUM_PAL_COLORS;
	unguard;
}
IMPLEMENT_DB_CLASS(UPalette);

/*-----------------------------------------------------------------------------
	UPalette general functions.
-----------------------------------------------------------------------------*/

//
// Adjust a regular (imported) palette.
//
void UPalette::FixPalette()
{
	guard(UPalette::FixPalette);
	Lock(LOCK_Read);

	FColor TempColors[256];
	for( int i=0; i<256; i++ )
		TempColors[i] = Element(0);

	for( int iColor=0; iColor<8; iColor++ )
	{
		int iStart = (iColor==0) ? 1 : 32*iColor;
		for( int iShade=0; iShade<28; iShade++ )
			TempColors[16 + iColor + (iShade<<3)] = Element(iStart + iShade);

	}
	for( i=0; i<256; i++ )
	{
		Element(i) = TempColors[i];
		Element(i).RemapIndex = i+0x10;
	}
	Element(0).RemapIndex=0;

	Unlock(LOCK_Read);
	unguard;
}

//
// Find closest palette color matching a given RGB value.
//
BYTE UPalette::BestMatch( FColor Color,int SystemPalette )
{
	guard(UPalette::BestMatch);
	Lock(LOCK_Read);

	int BestDelta = MAXINT;
	int BestColor = FIRSTCOLOR;

	int First = SystemPalette ? FIRSTCOLOR : 1;
	int Last  = SystemPalette ? LASTCOLOR  : NUM_PAL_COLORS;

	for( int TestColor=First; TestColor<Last; TestColor++ )
	{
		FColor *ColorPtr = &Element(TestColor);
		int Delta =
		(
			Square((int)ColorPtr->R - (int)Color.R) +
			Square((int)ColorPtr->G - (int)Color.G) +
			Square((int)ColorPtr->B - (int)Color.B)
		);
		if( Delta < BestDelta )
		{
			BestColor = TestColor;
			BestDelta = Delta;
		}
	}
	Unlock(LOCK_Read);
	return BestColor;
	unguard;
}

//
// Find closest palette color matching a given RGB value that
// lies within a particular one of the 8 palette ranges.
//
BYTE UPalette::BestMatchInRange( FColor MatchColor, BYTE RangeColor )
{
	guard(UPalette::BestMatchInRange);
	Lock(LOCK_Read);

	FColor		*ColorPtr;
	int 		Delta,BestDelta,BestColor,TestColor;

	RangeColor		= RangeColor & 7;
	BestDelta  		= MAXINT;
	BestColor  		= FIRSTCOLOR;

	for( TestColor=FIRSTCOLOR; TestColor<LASTCOLOR; TestColor++ )
	{
		ColorPtr = &Element(TestColor);
		if( RangeColor == (TestColor&7) )
		{
			Delta =
				Square((int)ColorPtr->R - (int)MatchColor.R) +
				Square((int)ColorPtr->G - (int)MatchColor.G) +
				Square((int)ColorPtr->B - (int)MatchColor.B);
			if( Delta < BestDelta )
			{
				BestColor = TestColor;
				BestDelta = Delta;
			}
		}
		ColorPtr++;
	}
	Unlock(LOCK_Read);
	return BestColor;
	unguard;
}

//
// Smooth out a ramp palette by averaging adjacent colors.
//
void UPalette::Smooth()
{
	guard(UPalette::Smooth);

	FColor *C1=&Element(0), *C2=&Element(1);

	for( int i=1; i<NUM_PAL_COLORS; i++ )
	{
		C2->R = ((int)C1->R + (int)C2->R)>>1;
		C2->G = ((int)C1->G + (int)C2->G)>>1;
		C2->B = ((int)C1->B + (int)C2->B)>>1;

		C1++;
		C2++;
	}
	unguard;
}

//
// Return a pointer to a cached color depth-specific palette.
// Size of returned array is Camera->ColorBytes.
//
RAINBOW_PTR UPalette::GetColorDepthPalette( FCacheItem *&Item, UTexture *DestTexture )
{
	guard(UPalette::GetColorDepthPalette);

	int CacheID = MakeCacheID(CID_ColorDepthPalette,GetIndex(),DestTexture->ColorBytes);
	RAINBOW_PTR	Result = GCache.Get(CacheID,Item);

	if( !Result.PtrBYTE )
	{
		Lock(LOCK_Read);
		Result      = GCache.Create(CacheID,Item,NUM_PAL_COLORS * DestTexture->ColorBytes);
		FColor *Src = &Element(0);
		if( DestTexture->ColorBytes==1 )
		{			
			for( int i=0; i<NUM_PAL_COLORS; i++ )
				Result.PtrBYTE[i] = (Src++)->RemapIndex;
		}
		else if( (DestTexture->ColorBytes==2) && (DestTexture->CameraCaps & CC_RGB565) )
		{
			for( int i=0; i<NUM_PAL_COLORS; i++ )
				Result.PtrWORD[i] = (Src++)->HiColor565();
		}
		else if( DestTexture->ColorBytes==2 )
		{
			for( int i=0; i<NUM_PAL_COLORS; i++ )
				Result.PtrWORD[i] = (Src++)->HiColor555();
		}
		else if( DestTexture->ColorBytes==4 )
		{
			for( int i=0; i<NUM_PAL_COLORS; i++ )
				Result.PtrDWORD[i] = (Src++)->TrueColor();
		}
		else
		{
			appError( "Invalid ColorBytes" );
		}
		Unlock(LOCK_Read);
	}
	return Result;
	unguard;
}

//
// Build the palette remap index.
//
void UPalette::BuildPaletteRemapIndex( int Masked )
{
	guard(UPalette::BuildPaletteRemapIndex);
	Lock(LOCK_Read);

	if( GGfx.DefaultPalette && this!=(UPalette*)GGfx.DefaultPalette)
	{
		// Build remap index for a regular palette.
		for( int i=0; i<NUM_PAL_COLORS; i++ )
			Element(i).RemapIndex = GGfx.DefaultPalette->BestMatch(Element(i));		
	}
	else for( int i=0; i<NUM_PAL_COLORS; i++ )
	{
		// This is the default palette.
		Element(i).RemapIndex = i;
	}

	if( Masked )
		Element(0).RemapIndex = 0;

	Unlock(LOCK_Read);
	unguard;
}

//
// Sees if this palette is a duplicate of an existing palette.
// If it is, deletes this palette and returns the existing one.
// If not, returns this palette.
//
UPalette *UPalette::ReplaceWithExisting()
{
	guard(UPalette::ReplaceWithExisting);
	Lock(LOCK_Read);

	UPalette* TestPalette;
	FOR_ALL_TYPED_OBJECTS(TestPalette,UPalette)
	{
		if( TestPalette != this )
		{
			TestPalette->Lock(LOCK_Read);
			FColor *C1 = &Element(0);
			FColor *C2 = &TestPalette->Element(0);
			for( int i=0; i<NUM_PAL_COLORS; i++ )
			{
				if( *C1 != *C2 ) break;
				C1++; 
				C2++;
			}
			TestPalette->Unlock(LOCK_Read);
			if( i == NUM_PAL_COLORS )
			{
				debugf(LOG_Info,"Replaced palette %s with %s",GetName(),TestPalette->GetName());
				Unlock(LOCK_Read);
				Kill();
				return TestPalette;
			}
		}
	}
	END_FOR_ALL_OBJECTS;

	Unlock(LOCK_Read);
	return this;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
