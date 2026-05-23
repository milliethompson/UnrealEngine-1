/*=============================================================================
	UnGfx.h: Graphics functions.

	Copyright 1995 Epic MegaGames, Inc.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNGFX // Prevent header from being included multiple times.
#define _INC_UNGFX

/*------------------------------------------------------------------------------------
	Color information.
------------------------------------------------------------------------------------*/

//
// Editor colors, set by FGlobalGraphics::Init.
//
enum {MAX_COLORS=64};
#define WorldBoxColor			GGfx.Colors[0 ]
#define GroundPlaneColor		GGfx.Colors[1 ]
#define GroundPlaneHighlight	GGfx.Colors[2 ]
#define NormalColor				GGfx.Colors[3 ]
#define BrushFillColor			GGfx.Colors[4 ]
#define BrushWireColor			GGfx.Colors[5 ]
#define BspWireColor			GGfx.Colors[6 ]
#define BspFillColor			GGfx.Colors[7 ]
#define PivotColor				GGfx.Colors[8 ]
#define SelectColor				GGfx.Colors[9 ]
#define CurrentColor			GGfx.Colors[10]
#define AddWireColor			GGfx.Colors[11]
#define SubtractWireColor		GGfx.Colors[12]
#define GreyWireColor			GGfx.Colors[15]
#define BrushVertexColor		GGfx.Colors[16]
#define BrushSnapColor			GGfx.Colors[17]
#define InvalidColor			GGfx.Colors[18]
#define ActorDotColor			GGfx.Colors[19]
#define ActorWireColor			GGfx.Colors[20]
#define ActorHiWireColor		GGfx.Colors[21]
#define ActorFillColor			GGfx.Colors[22]
#define BlackColor				GGfx.Colors[23]
#define WhiteColor				GGfx.Colors[24]
#define MaskColor				GGfx.Colors[25]
#define SelectBorderColor		GGfx.Colors[27]
#define SemiSolidWireColor		GGfx.Colors[28]
#define NonSolidWireColor       GGfx.Colors[30]
#define WireBackground			GGfx.Colors[32]
#define WireGridAxis			GGfx.Colors[35]
#define ActorArrowColor			GGfx.Colors[36]
#define ScaleBoxColor			GGfx.Colors[37]
#define ScaleBoxHiColor			GGfx.Colors[38]
#define ZoneWireColor			GGfx.Colors[39]
#define MoverColor				GGfx.Colors[40]
#define OrthoBackground			GGfx.Colors[41]

//
// Standard colors.
//
enum EStandardColors
{
	P_GREY		= 0,
	P_BROWN		= 1,
	P_FLESH		= 2,
	P_WOOD		= 3,
	P_GREEN		= 4,
	P_RED		= 5,
	P_BLUE		= 6,
	P_FIRE		= 7,
};

#define BRIGHTNESS(b) (BYTE)((b)<<3)
#define COLOR(c,b)    (BYTE)((c)+((b)<<3)+32) /* c=color 0-7, b=brightness 0-27 */

#define FIRSTCOLOR 16
#define LASTCOLOR  240

/*------------------------------------------------------------------------------------
	FGlobalGfx.
------------------------------------------------------------------------------------*/

//
// Graphics globals.
//
class UNENGINE_API FGlobalGfx
{
public:
	// Parameters.
	DWORD	DefaultCameraFlags;	// Camera-show flags when opening a new camera.
	DWORD	DefaultRendMap;		// Camera map rendering flags when opening a new camera.

	// Standard colors.
	BYTE Colors[MAX_COLORS];

	// Objects.
	UArray*			Graphics;			// Graphics array object.
	UArray*			Tables;				// Stored/regenerated tables of textures & things.
	UArray*			Luts;				// Lookup tables.

	UTexture		*DefaultTexture;	// Default texture for untextured polygons.
	UTexture		*BkgndTexture;		// Background texture for viewers.
	UTexture		*BadTexture;		// Invalid texture picture.

	UModel			*ArrowBrush;		// Brush that shows actor directions.
	UModel			*RootHullBrush;		// Brush that encloses the world.

	UPalette::Ptr	DefaultPalette;		// Palette to use at startup.

	UBuffer::Ptr	RemapTable;			// 64K shade table.
	UBuffer::Ptr	BlendTable;			// 64K blend table.
	UVectors::Ptr	HueTable;			// 24-bit color 256-entry hue table.
	UFloats::Ptr	SincTable;			// Pseudo 2D sinc interpolation table.

	UFont			*HugeFont;
	UFont			*LargeFont;
	UFont			*MedFont;
	UFont			*SmallFont;

	UTexture		*MenuUp,*MenuDn;
	UTexture		*CollOn,*CollOff;
	UTexture		*PlyrOn,*PlyrOff;
	UTexture		*LiteOn,*LiteOff;

	// Gamma correction parameters.
	WORD	NumGammaLevels;
	WORD	GammaLevel;

	// Effects/Postprocessing.
	int Smooth;
	int Stretch;

	// Inlines.
	BYTE Blend256(BYTE A, BYTE B) {return BlendTable(A+((INT)B<<8));}

	// FGlobalGfx interface.
	void Init();
	void Exit();
	void PreRender(UCamera *Camera);
	void PostRender(UCamera *Camera);

	// Private functions.
private:
	void LookupAllTables();
	void LookupAllLuts();
};

/*------------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------------*/
#endif // _INC_UNGFX
