/*=============================================================================
	UnGfx.cpp: FGlobalGfx implementation - general-purpose graphics routines

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*------------------------------------------------------------------------------
	Frame functions.
------------------------------------------------------------------------------*/

FMemMark Mark;
BYTE *SavedScreen;
UCamera SavedCamera;
void FGlobalGfx::PreRender( UCamera *Camera )
{
	guard(FGlobalGfx::PreRender);
	Mark.Push(GMem);
	if( Smooth )
	{
		SavedScreen = Camera->Screen;
		Camera->Screen = new(GMem,Camera->ByteStride * Camera->Y)BYTE;
	}
	if( Stretch )
	{
		SavedCamera = *Camera;
		Camera->X /= 2;
		Camera->Stride /= 2;
		Camera->Y /= 2;
		Camera->ByteStride /= 2;
		Camera->PrecomputeRenderInfo(Camera->X,Camera->Y);
		Camera->Screen = new(GMem,Camera->ByteStride * Camera->Y)BYTE;
	}
	if( 0 )
	{
		memcpy(&SavedCamera,Camera,sizeof(UCamera));
		Camera->X      *= 2;
		Camera->Y      *= 2;
		Camera->Stride *= 2;
		Camera->PrecomputeRenderInfo(Camera->X,Camera->Y);
		Camera->Screen  = new(GMem,Camera->ByteStride * Camera->Y)BYTE;
	}
	unguard;
}

//
// Finalize the stuff in the frame based on the current camera mode.
//
void FGlobalGfx::PostRender( UCamera *Camera )
{
	guard(FGlobalGfx::PostRender);

	// Experimental stuff.
	if( 0 )
	{
		DWORD *OldScreen = (DWORD *)Camera->Screen;
		memcpy(Camera,&SavedCamera,sizeof(UCamera));
		DWORD *Src0 = OldScreen;
		DWORD *Src1 = OldScreen + Camera->Stride*2;
		DWORD *Dest = (DWORD*)Camera->Screen + Camera->Stride;
		for( int i=0; i<Camera->Y; i++ )
		{
			for( int i=0; i<Camera->X; i++ )
			{
				*Dest++ = 
				+	((Src0[0] & 0xfcfcfc)>>2)
				+	((Src0[1] & 0xfcfcfc)>>2)
				+	((Src1[0] & 0xfcfcfc)>>2)
				+	((Src1[1] & 0xfcfcfc)>>2);
				Src0 += 2;
				Src1 += 2;
			}
			Src0 += Camera->Stride*2;
			Src1 += Camera->Stride*2;
		}
	}
	if( Stretch )
	{
		RAINBOW_PTR Src = Camera->Screen;
		*Camera = SavedCamera;
		RAINBOW_PTR Dest = Camera->Screen;
		for( int i=0; i<Camera->Y; i+=2 )
		{
			for( int ii=0; ii<2; ii++ )
			{
				if( Camera->ColorBytes==1 )
					for( int j=0; j<Camera->X; j+=2 )
						{Dest.PtrBYTE[0] = Dest.PtrBYTE[1] = *Src.PtrBYTE++; Dest.PtrBYTE+=2;}
				else if( Camera->ColorBytes==2 )
					for( int j=0; j<Camera->X; j+=2 )
						{Dest.PtrWORD[0] = Dest.PtrWORD[1] = *Src.PtrWORD++; Dest.PtrWORD+=2;}
				else if( Camera->ColorBytes==4 )
					for( int j=0; j<Camera->X; j+=2 )
						{Dest.PtrDWORD[0] = Dest.PtrDWORD[1] = *Src.PtrDWORD++; Dest.PtrDWORD+=2;}
				Src.PtrBYTE -= Camera->ByteStride/2;
			}
			Src.PtrBYTE += Camera->ByteStride/2;
		}
	}
	if( Smooth )
	{
		BYTE *OldScreen = Camera->Screen;
		Camera->Screen  = SavedScreen;

		DWORD *Screen = (DWORD *)Camera->Screen;
		DWORD *Frame0 = (DWORD *)OldScreen + 0*Camera->Stride;
		DWORD *Frame1 = (DWORD *)OldScreen + 1*Camera->Stride;
		DWORD *Frame2 = (DWORD *)OldScreen + 2*Camera->Stride;
		for( int i=1; i<Camera->Y-1; i++ )
		{
			for( int i=1; i<Camera->X-2; i++ )
			{
				Screen[i] = 
				+	((Frame0[i+1] & 0xf8f8f8)>>3)
				+	((Frame1[i+0] & 0xf8f8f8)>>3)
				+	((Frame1[i+1] & 0xfefefe)>>1)
				+	((Frame1[i+2] & 0xf8f8f8)>>3)
				+	((Frame2[i+1] & 0xf8f8f8)>>3)
				;
			}
			Screen += Camera->Stride;
			Frame0  = Frame1;
			Frame1  = Frame2;
			Frame2 += Camera->Stride;
		}
	}
	Mark.Pop();
	unguard;
}

/*------------------------------------------------------------------------------
	FGlobalGfx table functions.
------------------------------------------------------------------------------*/

//
// Lookup all graphics tables from existing objects.
//
void FGlobalGfx::LookupAllTables()
{
	guard(FGlobalGfx::LookupAllTables);

	DefaultTexture		= new("Default",	FIND_Existing) UTexture;
	BkgndTexture		= new("Bkgnd",		FIND_Existing) UTexture;
	BadTexture			= new("Bad",		FIND_Existing) UTexture;

	DefaultPalette		= new("Palette",	FIND_Existing) UPalette;
	HugeFont			= new("f_huge",		FIND_Existing) UFont;
	LargeFont			= new("f_large",	FIND_Existing) UFont;
	MedFont				= new("f_tech",		FIND_Existing) UFont;
	SmallFont			= new("f_small",	FIND_Existing) UFont;

	if( GEditor )
	{
		ArrowBrush		= new("Arrow",		FIND_Existing) UModel;
		RootHullBrush	= new("RootHull",	FIND_Existing) UModel;
	}
	else
	{
		ArrowBrush		= NULL;
		RootHullBrush	= NULL;
	}

	MenuUp				= new("b_menuup",	FIND_Existing) UTexture;
	MenuDn				= new("b_menudn",	FIND_Existing) UTexture;
	CollOn				= new("b_collon",	FIND_Existing) UTexture;
	CollOff				= new("b_colloff",	FIND_Existing) UTexture;
	PlyrOn				= new("b_plyron",	FIND_Existing) UTexture;
	PlyrOff				= new("b_plyroff",	FIND_Existing) UTexture;
	LiteOn				= new("b_liteon",	FIND_Existing) UTexture;
	LiteOff				= new("b_liteoff",	FIND_Existing) UTexture;

	unguard;
}

//
// Lookup all rendering lookup tables.
//
void FGlobalGfx::LookupAllLuts()
{
	guard(FGlobalGfx::LookupAllLuts);
	RemapTable	= new("Remap",		FIND_Existing)UBuffer;
	BlendTable	= new("Blend",		FIND_Existing)UBuffer;
	HueTable	= new("Hue",		FIND_Existing)UVectors;
	SincTable	= new("Sinc",		FIND_Existing)UFloats;
	unguard;
}

/*------------------------------------------------------------------------------
	FGlobalGfx Init & Exit.
------------------------------------------------------------------------------*/

//
// Initialize the graphics engine and allocate all stuff.
// Calls appError if failure.
//
void FGlobalGfx::Init()
{
	guard(FGlobalGfx::Init);

	FColor			Color,*C1,*C2;
	int 			ShadeWeight[32],i,j;

	// Init stuff required for importing.
	DefaultPalette = (UPalette*)NULL;

	// Init color table using non-Windows colors.
	BlackColor				= 233;	// Pure non-Windows black
	WhiteColor				= 16;	// Pure non-Windows white
	MaskColor				= 0;	// Invisible color for sprites and masked textures

	// All UnrealEd colors.
	WorldBoxColor			= COLOR(P_BLUE,10);
	GroundPlaneHighlight	= COLOR(P_BLUE,8);
	GroundPlaneColor		= COLOR(P_BLUE,16);
	NormalColor				= COLOR(P_GREY,0);
	BrushFillColor			= COLOR(P_FIRE,0);
	BrushWireColor			= COLOR(P_RED,0);
	AddWireColor			= COLOR(P_BLUE,0);
	SubtractWireColor		= COLOR(P_WOOD,0);
	GreyWireColor			= COLOR(P_GREY,4);
	InvalidColor			= COLOR(P_GREY,4);
	BspWireColor			= COLOR(P_FLESH,6);
	BspFillColor			= COLOR(P_FLESH,16);
	SelectColor				= COLOR(P_BLUE,8);
	SelectBorderColor		= COLOR(P_FLESH,4);
	PivotColor				= COLOR(P_WOOD,0);
	ActorDotColor			= COLOR(P_BROWN,0);
	ActorWireColor			= COLOR(P_BROWN,8);
	ActorHiWireColor		= COLOR(P_BROWN,2);
	ActorFillColor			= COLOR(P_BROWN,24);
	NonSolidWireColor		= COLOR(P_GREY,4);
	SemiSolidWireColor		= COLOR(P_GREEN,4);
	WireBackground			= BlackColor;
	WireGridAxis			= COLOR(P_GREY,12);
	ActorArrowColor			= COLOR(P_RED,4);
	ScaleBoxColor			= COLOR(P_FIRE,6);
	ScaleBoxHiColor			= COLOR(P_FIRE,0);
	ZoneWireColor			= COLOR(P_BLUE,0);
	MoverColor				= COLOR(P_GREEN,2);
	OrthoBackground			= COLOR(P_BLUE,23); //COLOR(P_GREY,4);

	// Allocate global graphics array.
	Graphics = new("Graphics",CREATE_Unique)UArray(0);

	// Init misc.
	DefaultCameraFlags = SHOW_Frame | SHOW_Actors | SHOW_MovingBrushes;

	// Gamma parameters.
	NumGammaLevels  = 8;
	GammaLevel		= 4;

	// Add graphics array to root.
	GObj.AddToRoot(Graphics);
	debug( LOG_Init, "Graphics initialized" );

	// Load graphics tables if present, otherwise regenerate.
	GObj.AddFile("Unreal.gfx",NULL,1);
	Tables = new("GfxTables",FIND_Optional)UArray;
	if( !Tables )
	{
		GApp->BeginSlowTask("Building Unreal.gfx",1,0);
		Tables = new("GfxTables",CREATE_Unique)UArray(0);
		debug (LOG_Init,"Building Unreal.gfx");

		//
		// Regenerate all graphics tables.
		//

		DefaultPalette = new("Palette",DEFAULT_PALETTE_FNAME,IMPORT_Replace)UPalette;
		if( !DefaultPalette )
			appError ("Error loading default palette");

		DefaultPalette->FixPalette();
		Tables->AddItem(DefaultPalette);

		// Call import script to load editor stuff (marker sprites, fonts, etc).
		guard(GFX_BOOTSTRAP_FNAME)
		if (!GEditor) appError("Can't find graphics objects");
		GEditor->Exec("MACRO PLAY NAME=Startup FILE=" GFX_BOOTSTRAP_FNAME);
		unguard;

		// Finish & save new table to disk.
		GApp->StatusUpdate( "Saving", 0, 0 );
		GObj.SaveDependent( Tables, "Unreal.gfx" );
		GApp->EndSlowTask();
	}
	LookupAllTables();
	Graphics->AddItem(Tables);

	// Load lookup tables.
	GObj.AddFile("Unreal.tab",NULL,1);
	Luts = new("GfxLuts",FIND_Optional)UArray;
	if (!Luts)
	{
		GApp->BeginSlowTask("Building Unreal.tab",1,0);
		Luts = new("GfxLuts",CREATE_Unique)UArray(0);
		debug (LOG_Init,"Building Unreal.tab");

		RemapTable   = new("Remap",      CREATE_Replace)UBuffer(65536,1);
		BlendTable   = new("Blend",      CREATE_Replace)UBuffer(256*256,1);
		HueTable	 = new("Hue",        CREATE_Replace)UVectors(256,1);
		SincTable	 = new("Sinc",		 CREATE_Replace)UFloats(256,1);

		Luts->AddItem(RemapTable);
		Luts->AddItem(BlendTable);
		Luts->AddItem(HueTable);
		Luts->AddItem(SincTable);

		LookupAllLuts();

		// Shade weight table.
		ShadeWeight[31] = 0;
		for( i=1; i<32; i++ )
			ShadeWeight[i] = (int)(65536.0 * exp (0.95 * log ((FLOAT)(31-i)/31.0)));

		// Remap table - table of nearest palette index for all 5-6-5 colors.
		RemapTable->Lock(LOCK_ReadWrite);
		BYTE *Ptr = &RemapTable->Element(0);
		union
		{
			struct{DWORD B:5; DWORD G:6; DWORD R:5;};
			DWORD D;
		} Count;
		for( Count.D=0; Count.D<65536; Count.D++ )
		{
			if( (Count.D & 255)==0 )
				GApp->StatusUpdate ("Generating remap table",Count.D,65536);
			FColor Color(Count.R<<3,Count.G<<2,Count.B<<3);
			*Ptr++ = GGfx.DefaultPalette->BestMatch(Color);
		}
		RemapTable->Unlock(LOCK_ReadWrite);

		// Hue table (for HSV color).
		HueTable->Lock(LOCK_ReadWrite);
		for( i=0; i<86; i++ )
		{
			FLOAT F = (FLOAT)i/85.0;

			HueTable(i		).X = 1.0 - F; // XYZ=RGB
			HueTable(i		).Y = F;
			HueTable(i		).Z = 0;

			HueTable(i+85	).X = 0;
			HueTable(i+85	).Y = 1.0 - F;
			HueTable(i+85	).Z = F;

			HueTable(i+170	).X = F;
			HueTable(i+170	).Y = 0;
			HueTable(i+170	).Z = 1.0 - F;
		}
		HueTable->Unlock(LOCK_ReadWrite);

		// Sinc tables.
		SincTable->Lock(LOCK_ReadWrite);
		for (int i=0; i<256; i++)
		{
			// This is now just a bilinear scaling table.
			//SincData[i] = 0.5 - 0.5 * cos(PI * i / 256.0);
			//SincData[i] = 0.6 * i/255.0 + 0.4 * (0.5 - 0.5 * cos(PI * i / 256.0));
			SincTable(i) = i/255.0;
		}
		SincTable->Unlock(LOCK_ReadWrite);

		// Blend table.
		BlendTable->Lock(LOCK_ReadWrite);
		for( i=0; i<256; i++ )
		{
			GApp->StatusUpdate ("Generating blend tables",i,256);
			DefaultPalette->Lock(LOCK_Read);
			C1 = &DefaultPalette(i);
			DefaultPalette->Unlock(LOCK_Read);
			for( j=0; j<256; j++ )
			{
				C2 = &DefaultPalette(j);
				Color.R = ((int)C1->R + (int)C2->R) / 2;
				Color.G = ((int)C1->G + (int)C2->G) / 2;
				Color.B = ((int)C1->B + (int)C2->B) / 2;
				//BlendTable(i*256+j) = DefaultPalette->BestMatchInRange(Color,j);
				BlendTable(i*256+j) = DefaultPalette->BestMatch(Color);
			}
		}
		for (i=0; i<256; i++)
		{
			// Force blending tables to leave masked color as-is.
			BlendTable(i)=0;
			BlendTable(i*256)=i;
		}
		BlendTable->Unlock(LOCK_ReadWrite);

		GApp->StatusUpdate( "Saving", 0, 0 );
		GObj.SaveDependent( Luts, "Unreal.tab" );
		GApp->EndSlowTask();
	}
	LookupAllLuts();
	Graphics->AddItem(Luts);
	GCameraManager->SetPalette(DefaultPalette);

	unguard;
}

//
// Shut down graphics.
//
void FGlobalGfx::Exit()
{
	guard(FGlobalGfx::Exit);
	GObj.RemoveFromRoot(Graphics);
	debug(LOG_Exit,"Graphics closed");
	unguard;
}

/*------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------*/
