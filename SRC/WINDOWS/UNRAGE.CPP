/*=============================================================================
	UnRage.cpp: Unreal ATI Rage support code

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "StdAfx.h"

#include "Unreal.h"
#include "UnRender.h"
#include "UnRaster.h"
#include "UnWnCam.h"
#include "UnRenDev.h"

#include "other\ati3dcif.h" // Note: Requires the library Editor\ati3Dcif.lib.

extern FWindowsCameraManager CameraManager;

/*-----------------------------------------------------------------------------
	Imported functions.
-----------------------------------------------------------------------------*/

// Class containing pointers to all ATI 3D functions from ATI's DLL.
class F3DCIF
{
public:
	C3D_EC  (WINAPI *GetInfo)(PC3D_3DCIFINFO p3DCIFInfo);
	C3D_HRC (WINAPI *ContextCreate)();
	C3D_EC  (WINAPI *Init)();
	C3D_EC  (WINAPI *Term)();
	C3D_EC  (WINAPI *ContextDestroy)(C3D_HRC hRC);
	C3D_EC  (WINAPI *ContextSetState)(C3D_HRC hRC,C3D_ERSID eRStateID, C3D_PRSDATA pRStateData);
	C3D_EC  (WINAPI *RenderBegin)(C3D_HRC hRC);
	C3D_EC  (WINAPI *RenderEnd)();
	C3D_EC  (WINAPI *TextureReg)(C3D_PTMAP ptmapToReg,C3D_PHTX phtmap);
	C3D_EC  (WINAPI *TextureUnreg)(C3D_HTX htxToUnreg);
	C3D_EC  (WINAPI *RenderPrimStrip)(C3D_VSTRIP vStrip,C3D_UINT32 u32NumVert);

	HINSTANCE hModule;
	int Associate();
} ATI3DCIF;

//
// Try to import all of ATI's functions from ATI's DLL.
// Returns 1 if success, 0 if failure.
//
int F3DCIF::Associate()
{
	hModule = LoadLibrary("ATI3DCIF.DLL");
	if (!hModule) return 0;
	debug(LOG_Init,"Found ATI Rage");

	#define ATI_GET(FuncName,ImpName) *(FARPROC *)&FuncName = GetProcAddress(hModule,ImpName); if (!FuncName) GApp->MessageBox(ImpName,ImpName,0);
	ATI_GET(GetInfo,"_ATI3DCIF_GetInfo@4");
	ATI_GET(ContextCreate,"_ATI3DCIF_ContextCreate@0");
	ATI_GET(Init,"_ATI3DCIF_Init@0");
	ATI_GET(Term,"_ATI3DCIF_Term@0");
	ATI_GET(ContextDestroy,"_ATI3DCIF_ContextDestroy@4");
	ATI_GET(ContextSetState,"_ATI3DCIF_ContextSetState@12");
	ATI_GET(RenderBegin,"_ATI3DCIF_RenderBegin@4");
	ATI_GET(RenderEnd,"_ATI3DCIF_RenderEnd@0");
	ATI_GET(TextureReg,"_ATI3DCIF_TextureReg@8");
	ATI_GET(TextureUnreg,"_ATI3DCIF_TextureUnreg@4");
	ATI_GET(RenderPrimStrip,"_ATI3DCIF_RenderPrimStrip@8");

	return GetInfo && ContextCreate && Init && Term && ContextDestroy &&
		ContextSetState && RenderBegin && RenderEnd && TextureReg &&
		TextureUnreg && RenderPrimStrip;
}

/*-----------------------------------------------------------------------------
	FRageRenderDevice definition.
-----------------------------------------------------------------------------*/

// Class that tracks a texture cached by Rage.
class FCachedRageTexture
{
public:
	UTexture			*Texture;		// The Unreal texture object being cached
	LPDIRECTDRAWSURFACE	lpDDSTex;		// DirectDraw surface holding it
	DDSURFACEDESC       ddsd;			// DirectDraw surface description
	C3D_TMAP			TMap;			// Rage API texture definition
	C3D_HTX				hTX;			// Rage API texture handle
	// Maybe we need a mip level here, if mip textures must be cached separately
};

//
// The ATI Rage 3D rendering device
//
class FRageRenderDevice : public FRenderDevice
{
public:
	// Constants.
	enum {MAX_TEXTURES=256};

	// Variables.
	C3D_HRC hRC;
	int NumTextures;
	FCachedRageTexture Textures[MAX_TEXTURES];

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

	// FRageRenderDevice interface.
	C3D_HTX LockTexture(UTexture *Texture);
	void    UnlockTexture(UTexture *Texture);
	C3D_HTX RegisterTexture(UTexture *Texture);
	void    UnregisterTexture(UTexture *Texture);
	void    BuildTexture(FCachedRageTexture *CachedTexture);
	char *RageError(C3D_EC Code);
};

/*-----------------------------------------------------------------------------
	FRageRenderDevice Init & Exit.
-----------------------------------------------------------------------------*/

//
// Initializes the ATI Rage device.  Can't fail.
//
int FRageRenderDevice::Init3D( UCamera *Camera, int RequestX, int RequestY )
{
	guard(FRageRenderDevice::Init3D);
	checkState(!Active);

	C3D_EC Result;

	// Init ATI lib.
	Result = ATI3DCIF.Init();
	if( Result )
	{
		debugf("Could not initialize ATI 3D driver: %s",RageError(Result));
		return 0;
	}

	// Create rendering context.
	hRC = ATI3DCIF.ContextCreate();
	if( !hRC )
	{
		debugf("Failed to create ATI rendering context");
		ATI3DCIF.Term();
		return 0;
	}

	// Get info.
	C3D_3DCIFINFO Info;
	Info.u32Size = sizeof(Info);
	Result = ATI3DCIF.GetInfo(&Info);
	if( Result )
	{
		debugf("GetInfo failed: %s",RageError(Result));
		ATI3DCIF.ContextDestroy(hRC);
		ATI3DCIF.Term();
		return 0;
	}

	// Init general info.
	Active      = 1;
	Locked      = 0;

	// Init custom info.
	NumTextures = 0;

	// Success.
	debugf( LOG_Init, "Detected: ATI Rage, Heap=%i, Ram=%i",Info.u32OffScreenSize,Info.u32TotalRAM );
	return 1;
	unguard;
}

//
// Shut down the ATI Rage device.
//
void FRageRenderDevice::Exit3D()
{
	guard(FRageRenderDevice::Exit3D);

	if (!Active) appError("Not active");
	if (Locked)  appError("Locked");

	Active = 0;

	// Destroy ATI 3D rendering context.
	C3D_EC Result = ATI3DCIF.ContextDestroy(hRC);
	if (Result) appErrorf("ContextDestroy failed: %s",RageError(Result));

	// Terminate the ATI 3D driver.
	Result = ATI3DCIF.Term();
	if (Result) appErrorf("Term failed: %s",RageError(Result));

	debugf(LOG_Init,"ATI Rage terminated");

	unguard;
};

//
// Flush all cached data.
//
void FRageRenderDevice::Flush3D()
{
	guard(FRageRenderDevice::Flush3D);

	if( !Active ) appError("Not active");

	// Flush all cached textures.
	for( int i=NumTextures-1; i>=0; i-- )
		UnregisterTexture(Textures[i].Texture);

	unguard;
}

//
// Convert a rage error code to text:
//
char *FRageRenderDevice::RageError(C3D_EC Code)
{
	guard(FRageRenderDevice::RageError);
	switch( Code )
	{
		case C3D_EC_OK:				return "C3D_EC_OK";
		case C3D_EC_GENFAIL:		return "C3D_EC_GENFAIL";
		case C3D_EC_MEMALLOCFAIL:	return "C3D_EC_MEMALLOCFAIL";
		case C3D_EC_BADPARAM:		return "C3D_EC_BADPARAM";
		case C3D_EC_UNUSED0:		return "C3D_EC_UNUSED0";
		case C3D_EC_BADSTATE:		return "C3D_EC_BADSTATE";
		case C3D_EC_NOTIMPYET:		return "C3D_EC_NOTIMPYET";
		case C3D_EC_UNUSED1:		return "C3D_EC_UNUSED1";
		default:					return "Unknown error code";
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	FRageRenderDevice Lock & Unlock.
-----------------------------------------------------------------------------*/

//
// Lock the ATI Rage rendering device.
//
void FRageRenderDevice::Lock3D(UCamera *Camera)
{
	guard(FRageRenderDevice::Lock);

	if (Locked) appError("Already locked");

	// Tell Rage to begin.
	C3D_EC Result = ATI3DCIF.RenderBegin(hRC);
	if( Result )
		appErrorf("Lock failed: %s",RageError(Result));

	// Set screen surface.
	Result = ATI3DCIF.ContextSetState(hRC,C3D_ERS_SURF_DRAW_PTR,&Camera->Screen); // Frame buf ptr

	Locked = 1;

	unguard;
};

//
// Unlock the ATI Rage rendering device.
//
void FRageRenderDevice::Unlock3D(UCamera *Camera,int Blit)
{
	guard(FRageRenderDevice::Unlock);

	if( !Locked ) appError("Not locked");

	C3D_EC Result = ATI3DCIF.RenderEnd();
	if (Result) appErrorf("Unlock failed: %s",RageError(Result));

	Locked = 0;

	unguard;
};

/*-----------------------------------------------------------------------------
	FRageRenderDevice Texture caching.
-----------------------------------------------------------------------------*/

//
// Build a texture's surface, called on RegisterTexture and in LockTexture
// when a texture's surface has been lost.  Requires that the texture's
// surface is valid and locked.
//
void FRageRenderDevice::BuildTexture(FCachedRageTexture *CachedTexture)
{
	guard(FRageRenderDevice::BuildTexture);

	UTexture *Texture = CachedTexture->Texture;
	debugf(LOG_Info,"Building %s for Rage",Texture->GetName());

	// Convert texture to RGB 5-5-5.
	BYTE   *Src     = &Texture->Element(Texture->Mips[0].Offset);
	WORD   *Dest    = (WORD *)CachedTexture->ddsd.lpSurface;

	int n = Texture->USize * Texture->VSize;
	for( int i=0; i<n; i++ )
		*Dest++ = Texture->Palette(*Src++).HiColor555();

	unguard;
}

//
// Register a texture with the 3D hardware. May cause one or more other textures
// to be flushed.  You can't count on any C3D_PHTX's remaining valid across calls.
// Returns with the texture locked.
//
C3D_HTX FRageRenderDevice::RegisterTexture(UTexture *Texture)
{
	guard(FRageRenderDevice::RegisterTexture);

	// Make sure this texture is cacheable.
	if ((Texture->USize>1024) || (Texture->USize&(Texture->USize-1))) appErrorf("Invalid texture U size: %i (%s)",Texture->USize,Texture->GetName());
	if ((Texture->VSize>1024) || (Texture->VSize&(Texture->VSize-1))) appErrorf("Invalid texture V size: %i (%s)",Texture->USize,Texture->GetName());

	// Make texture entry.
	if (NumTextures>=MAX_TEXTURES) Flush3D(); // Dump all textures (should LRU instead!)
	FCachedRageTexture *CachedTexture = &Textures[NumTextures++];

	CachedTexture->Texture	= Texture;
	CachedTexture->lpDDSTex	= NULL;
	CachedTexture->hTX		= NULL;

	// Create an offscreen DirectDraw surface to cache the texture map.
	ZeroMemory (&CachedTexture->ddsd,sizeof (DDSURFACEDESC));
	CachedTexture->ddsd.dwSize			= sizeof (DDSURFACEDESC);
	CachedTexture->ddsd.dwFlags			= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	CachedTexture->ddsd.ddsCaps.dwCaps	= DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;

	CachedTexture->ddsd.dwWidth			= Texture->USize;
	CachedTexture->ddsd.dwHeight		= Texture->VSize;

	HRESULT DDResult = CameraManager.dd->CreateSurface (&CachedTexture->ddsd,&CachedTexture->lpDDSTex,NULL);
	if( DDResult==DDERR_OUTOFVIDEOMEMORY )
	{
		// Must start flushing stuff!
	}
	if (DDResult) appErrorf("Could not create texture surface for file: %s",CameraManager.ddError(DDResult));

	// Fetch a pointer to the texture surface.
	ZeroMemory(&CachedTexture->ddsd,sizeof(DDSURFACEDESC));
	CachedTexture->ddsd.dwSize = sizeof (DDSURFACEDESC);

	// Lock texture to fill ddsd member, and keep it locked so DirectDraw
	// does not move it. Surface will be unlocked in the UnlockTexture 
	// function.
	if( CachedTexture->lpDDSTex->IsLost() )
		CachedTexture->lpDDSTex->Restore();
	DDResult = CachedTexture->lpDDSTex->Lock
	(
		NULL,
		&CachedTexture->ddsd,
		DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
		NULL
	);
	if( DDResult )
		appErrorf("DirectDraw lock failed: %s",CameraManager.ddError(DDResult));

	BuildTexture(CachedTexture);

	// Fill a C3D_TMAP struct.
	ZeroMemory (&CachedTexture->TMap,sizeof(C3D_TMAP));
	CachedTexture->TMap.u32Size = sizeof (C3D_TMAP);

	// Determine the maximum log2 dimension for mipmapping.
	BYTE *Surface  = (BYTE *)CachedTexture->ddsd.lpSurface;
	int MaxLog2 = FLogTwo(Max(Texture->USize,Texture->VSize));
	for( int i=0; i < MaxLog2; i++ )
	{
		CachedTexture->TMap.apvLevels[i] = Surface;
	}
	CachedTexture->TMap.bMipMap				= FALSE;
	CachedTexture->TMap.u32MaxMapXSizeLg2	= FLogTwo(Texture->USize);
	CachedTexture->TMap.u32MaxMapYSizeLg2	= FLogTwo(Texture->VSize);
	CachedTexture->TMap.eTexFormat			= C3D_ETF_RGB1555;

	SET_CIF_COLOR(CachedTexture->TMap.clrTexChromaKey,0,0,0,0);

	// Register the texture.
	C3D_EC Result = ATI3DCIF.TextureReg(&CachedTexture->TMap,&CachedTexture->hTX);
	if (Result) appErrorf("Error registering %s: %s",Texture->GetName(),RageError(Result));

	return CachedTexture->hTX;

	unguard;
};

//
// Unregister a texture with the 3D hardware.
//
void FRageRenderDevice::UnregisterTexture(UTexture *Texture)
{
	guard(FRageRenderDevice::UnregisterTexture);
	FCachedRageTexture *CachedTexture = NULL;

	// Find the texture in the list and remove it.
	int j=0;
	for( int i=0; i<NumTextures; i++ )
	{
		if (j!=i) Textures[j]=Textures[i];
		if (Textures[i].Texture==Texture)
			CachedTexture = &Textures[i];
		else
			j++;
	}
	if( !CachedTexture )
		appErrorf("Texture %s not found in cache",Texture->GetName());
	NumTextures = j;

	// Unregister the texture with Rage API.
	if( !CachedTexture->hTX )
		appErrorf("Texture %s has an invalid hTX",CachedTexture->Texture->GetName());

 	C3D_EC Result = ATI3DCIF.TextureUnreg(CachedTexture->hTX);
	if( Result )
		appErrorf("TextureUnreg failed: %s",RageError(Result));

	// Release the DirectDraw texture surface.
	HRESULT DDResult = CachedTexture->lpDDSTex->Release();
	if( DDResult )
		appErrorf("Error releasing %s: %s",CachedTexture->Texture->GetName(),CameraManager.ddError(DDResult));

	unguard;
}

//
// Look up a texture from 3D hardware and lock it.  Either returns an existing, cached 
// texture, or registers it and returns it.
//
C3D_HTX FRageRenderDevice::LockTexture(UTexture *Texture)
{
	guard(FRageRenderDevice::LockTexture);
	FCachedRageTexture *CachedTexture = NULL;

	// See if texture exists in cache.
	for( int i=0; i<NumTextures; i++ )
	{
		FCachedRageTexture *CachedTexture = &Textures[i];
		if( CachedTexture->Texture==Texture )
		{
			int MustRebuild = 0;
			if( CachedTexture->lpDDSTex->IsLost() )
			{
				CachedTexture->lpDDSTex->Restore();
				MustRebuild = 1;
			}
			HRESULT DDResult = CachedTexture->lpDDSTex->Lock
			(
				NULL,&CachedTexture->ddsd,
				DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
				NULL
			);
			if( MustRebuild )
				BuildTexture(CachedTexture);
			return CachedTexture->hTX;
		}
	}

	// Create a new texture and return it.
	return RegisterTexture(Texture);
	unguard;
}

//
// Unlock a texture.
//
void FRageRenderDevice::UnlockTexture(UTexture *Texture)
{
	guard(FRageRenderDevice::UnlockTexture);
	for( int i=0; i<NumTextures; i++ )
	{
		FCachedRageTexture *CachedTexture = &Textures[i];
		if( CachedTexture->Texture==Texture )
		{
			HRESULT DDResult = CachedTexture->lpDDSTex->Unlock(NULL);
			if (DDResult) appErrorf("Error unlocking %s: %s",CachedTexture->Texture->GetName(),CameraManager.ddError(DDResult));
			return;
		}
	}
	appErrorf("Texture %s not found for unlock",Texture->GetName());
	unguard;
}

/*-----------------------------------------------------------------------------
	Notes.

C3D_VTCF contains:
	C3D_FLOAT32 x, y, z			Homogeneous screen coordinates
    C3D_FLOAT32 s, t, w			Homogeneous texture coordinates
    C3D_FLOAT32 r, g, b, a		Color

Screen coordinates are like:
	x = k*X/w
	y = k*Y/w
	z = ((k1*Z)+k2)/w
	s = u/w
	t = v/w
	inverse w = 1/w

Current Restrictions
1. Only support EV_VTCF floating point vertex type
2. Only one context may be exist at a time
3. RenderSwitch does not have meaning ( consequence of 2. ) 
4. C3D_ETFILT_MIPTRI_MAG2BY2 is not supported yet.
5. Coordinates should be pre-clipped by software to:
   -2048.0 +2047.0 in X
   -4096.0 +4095.0 in Y

C3D_EPIXFMT    = pixel format of drawing surface
C3D_ETEXFMT    = texture format
C3D_EPRIM      = type of primitive (line, tri, quad)
C3D_ESHADE     = Gouraud shading modes
C3D_ETLIGHT    = texture lighting mode
C3D_ETEXFILTER = no/bi/trilinear mode
C3D_ETEXOP     = texture masking
C3D_ETPERSPCOR = perspective correction
C3D_TMAP       = strictire meeded when regosteromg textires
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	FRageRenderDevice texture vector polygon drawer.
-----------------------------------------------------------------------------*/

//
// Set an ATI vertex structure based on a point and a UV texture vector pair.
//
inline void SetVertV
(
	C3D_VTCF			&VT,
	const FTransform	&P,
	UTexture			*Texture,
	const FVector		&Base,
	const FVector		&Normal,
	const FVector		&U,
	const FVector		&V
)
{
	VT.x = P.ScreenX * (1.0/65536.0);
	VT.y = P.ScreenY;
	VT.z = 0.0;

	FVector PP; PP.X = P.X; PP.Y = P.Y; PP.Z = P.Z;
	FLOAT RZ = 1.0/PP.Z;

	VT.s = RZ * ((PP-Base) | U) * (1.0/65536.0)/(FLOAT)Texture->USize;
	VT.t = RZ * ((PP-Base) | V) * (1.0/65536.0)/(FLOAT)Texture->VSize;
	VT.w = RZ;

	VT.r = 255.0;//255.0*(FLOAT)rand()/(FLOAT)RAND_MAX;
	VT.g = 255.0;//255.0*(FLOAT)rand()/(FLOAT)RAND_MAX;
	VT.b = 255.0;//255.0*(FLOAT)rand()/(FLOAT)RAND_MAX;
}

//
// Set an ATI vertex structure based on a point and vertex texture coordinates.
//
void FRageRenderDevice::DrawPolyV
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
	guard(FRageRenderDevice::DrawPolyV);
	C3D_HTX  hTexture = LockTexture(Texture);
	C3D_EC   Result;
	if( !Locked ) appError("Not locked");

	// Set color.
	INT DoTmap = 1; // 1=texture, 0=no
	Result = ATI3DCIF.ContextSetState(hRC,C3D_ERS_TMAP_EN,&DoTmap);

	Result = ATI3DCIF.ContextSetState(hRC,C3D_ERS_TMAP_SELECT,&hTexture);

	C3D_ETLIGHT Light = C3D_ETL_NONE; // C3D_ETL_NONE C3D_ETL_MODULATE C3D_ETL_ALPHA_DECAL
	Result = ATI3DCIF.ContextSetState(hRC,C3D_ERS_TMAP_LIGHT,&Light);

	C3D_ESHADE Shade = C3D_ESH_SMOOTH; //C3D_ESH_NONE,C3D_ESH_SOLID,C3D_ESH_FLAT,C3D_ESH_SMOOTH
	Result = ATI3DCIF.ContextSetState(hRC,C3D_ERS_SHADE_MODE,&Shade);

	C3D_ETEXFILTER Filter = C3D_ETFILT_MIN2BY2_MAG2BY2;//C3D_ETFILT_MINPNT_MAG2BY2;
	Result = ATI3DCIF.ContextSetState(hRC,C3D_ERS_TMAP_FILTER,&Filter);

	int Persp = 6; // 0-6, 6 is the max and still exhibits temporal discontinuity
	Result = ATI3DCIF.ContextSetState(hRC,C3D_ERS_TMAP_PERSP_COR,&Persp);

	// Build RageVerts by decomposing the arbitrary convex polygon stored in Pts[]
	// into a triangle strip.
	int n=0,m=NumPts>>1;
	C3D_VTCF RageVerts[128];
	for( int i=0; i<m; i++ )
	{
		SetVertV(RageVerts[n++],Pts[i         ],Texture,Base,Normal,U,V);
		SetVertV(RageVerts[n++],Pts[NumPts-i-1],Texture,Base,Normal,U,V);
	}
	if( NumPts&1 )
		SetVertV(RageVerts[n++],Pts[m],Texture,Base,Normal,U,V);

	Result = ATI3DCIF.RenderPrimStrip((C3D_VSTRIP)RageVerts,n);
	if( Result ) appErrorf("RenderPrimStrip failed: %s",RageError(Result));

	UnlockTexture(Texture);
	unguard;
}

/*-----------------------------------------------------------------------------
	FRageRenderDevice texture coordinates polygon drawer.
-----------------------------------------------------------------------------*/

//
// Draw a polygon with texture coordinates.
//
void FRageRenderDevice::DrawPolyC
(
	UCamera*				Camera,
	UTexture*				Texture,
	const FTransTexture*	Pts,
	int						NumPts,
	DWORD					PolyFlags
)
{
	guard(FRageRenderDevice::DrawPolyC);
	if( !Locked ) appError("Not locked");
	// Not yet implemented.
	unguard;
}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/

int FRageRenderDevice::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(FRageRenderDevice::Exec);
	return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Finder.
-----------------------------------------------------------------------------*/

FRageRenderDevice GRageRenDev;

#if 0
FRenderDevice *FindRenderDevice()
	{return ATI3DCIF.Associate() ? &GRageRenDev : NULL;};
#endif

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
