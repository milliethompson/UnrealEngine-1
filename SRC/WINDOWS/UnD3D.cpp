/*=============================================================================
	UnD3D.cpp: Unreal support for the Microsoft Direct3D library.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
		* ported to D3D DrawPrim by Phil Taylor
=============================================================================*/

// Precompiled header.
#include "StdAfx.h"

// Unreal includes.
#include "UnWnCam.h"
#include "UnRender.h"
#include "UnRenDev.h"

// Direct3D includes.
#include "d3d.h"
#include "d3dcaps.h"

// Externs.
extern FWindowsCameraManager CameraManager;
HWND GetCameraWindow( UCamera* Camera );

// Macros.
#define CRITICALCALL(func) {HRESULT hRes=func; if(hRes) appErrorf("%s: %s",#func,CameraManager.ddError(hRes));}
#define SAFERELEASE(x)     {if (x) { x->Release(); x = NULL; }}

// Globals.
char		     szAppName[]="UnrealD3D";
char             DeviceName[128];
IDirectDraw*     dd;

/*-----------------------------------------------------------------------------
	FD3DTexture definition.
-----------------------------------------------------------------------------*/

//
// The Direct3D Texture class.
//
class FD3DTexture
{
public:
	DDSURFACEDESC       ddsd;			// dd surface description, for sys-mem surface		
	IDirectDrawSurface* MemorySurface;  // system memory surface	
	IDirectDrawPalette* Palette;
	
    IDirectDrawSurface* DeviceSurface;  // video memory texture	
	D3DTEXTUREHANDLE    Handle;
    FD3DTexture()
    {
		MemorySurface = 0;
		DeviceSurface = 0;
        Palette = 0;
		Handle = 0;
    }

    D3DTEXTUREHANDLE    GetHandle()    {return Handle;}
    IDirectDrawSurface* GetSurface()   {return MemorySurface;}
    IDirectDrawPalette* GetPalette()   {return Palette;}

    HRESULT InitEmpty(IDirect3DDevice2 *Device,int width,int height,int depth);		
	HRESULT InitEmptyMip(IDirect3DDevice2 *Device,int width,int height,int depth, int numMips);

    HRESULT Release(void);
    BOOL Restore(IDirectDraw* dd,IDirect3DDevice2 *Device);
};

/*-----------------------------------------------------------------------------
	ChooseTextureFormat.
-----------------------------------------------------------------------------*/

// Structure for holding best texture format found.
struct FindTextureData
{
    DWORD           bpp;        // We want a texture format of this bpp.
    DDPIXELFORMAT   ddpf;       // Place the format here.
};

// Texture format enumeration callback.
HRESULT CALLBACK FindTextureCallback( DDSURFACEDESC *DeviceFmt, LPVOID lParam )
{
    FindTextureData * FindData = (FindTextureData *)lParam;
    DDPIXELFORMAT ddpf = DeviceFmt->ddpfPixelFormat;

    // We use GetDC/BitBlt to init textures so we only
    // want to use formats that GetDC will support.
    if( ddpf.dwFlags & (DDPF_ALPHA|DDPF_ALPHAPIXELS) )
        return DDENUMRET_OK;
    if( ddpf.dwRGBBitCount <= 8 && !(ddpf.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4)) )
        return DDENUMRET_OK;
    if( ddpf.dwRGBBitCount > 8 && !(ddpf.dwFlags & DDPF_RGB) )
        return DDENUMRET_OK;

    // Keep the texture format that is nearest to the bitmap we have.
    if (FindData->ddpf.dwRGBBitCount == 0 ||
       (ddpf.dwRGBBitCount >= FindData->bpp &&
       (UINT)(ddpf.dwRGBBitCount - FindData->bpp) < (UINT)(FindData->ddpf.dwRGBBitCount - FindData->bpp)))
    {
        FindData->ddpf = ddpf;
    }

    return DDENUMRET_OK;
}

// Choose an appropriate texture format.
void ChooseTextureFormat( IDirect3DDevice2 *Device, DWORD bpp, DDPIXELFORMAT *pddpf )
{
    FindTextureData FindData;
    ZeroMemory( &FindData, sizeof(FindData) );
    FindData.bpp = bpp;
    Device->EnumTextureFormats( FindTextureCallback, (LPVOID)&FindData );
    *pddpf = FindData.ddpf;
}

/*-----------------------------------------------------------------------------
	PaletteFromUPalette.
-----------------------------------------------------------------------------*/

// Convert a UPalette to an IDirectDrawPalette.
static IDirectDrawPalette *PaletteFromUPalette( IDirectDraw *DirectDraw, UPalette *Pal )
{
    IDirectDrawPalette* Palette = NULL;

    // Convert BGR to RGB.
	DWORD Colors[256];
    for( int i=0; i<256; i++ )
        Colors[i] = RGB( Pal->Element(i).R, Pal->Element(i).G, Pal->Element(i).B );

    // Create a DirectDraw palette for the texture.
    DirectDraw->CreatePalette( DDPCAPS_8BIT, (PALETTEENTRY *)Colors, &Palette, NULL );

    return Palette;
}

/*-----------------------------------------------------------------------------
	InitEmpty.
-----------------------------------------------------------------------------*/

//
// Init the D3D texture.
//
HRESULT FD3DTexture::InitEmpty( IDirect3DDevice2 *Device, int width, int height, int depth )
{
	guard(FD3DTexture::InitEmpty);

	HRESULT hres;
    IDirectDrawSurface* Target;

    // Get the render target (we need it to get the IDirectDraw).
    if( Device==NULL || Device->GetRenderTarget(&Target) != DD_OK )
        return FALSE;

    // Find the best texture format to use.
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize = sizeof(ddsd);
    ChooseTextureFormat( Device, depth, &ddsd.ddpfPixelFormat );
    ddsd.dwFlags |= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    ddsd.dwWidth  = width;
    ddsd.dwHeight = height;

    // Create a video memory texture.
    D3DDEVICEDESC hal, hel;
    hal.dwSize = sizeof(hal);
    hel.dwSize = sizeof(hel);
    Device->GetCaps( &hal, &hel );

    // Should we check for texture caps?
    if( hal.dcmColorModel )
        ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE | DDSCAPS_ALLOCONLOAD;
    else
        ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY| DDSCAPS_TEXTURE;
  
    hres = dd->CreateSurface( &ddsd, &DeviceSurface, NULL );
	if( hres != DD_OK )
	{
		Release();
		return hres;
	}

    // Create a system memory surface for the texture.
    // We use this system memory surface for the ::Load call
    // and this surface does not get lost.
    if( hal.dcmColorModel )
    {
        ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;  
        hres                = dd->CreateSurface(&ddsd, &MemorySurface, NULL);
		if( hres != DD_OK )
		{
			Release();
			return hres;
		}
    }
    else
    {
        // When dealing with a SW rasterizer we don't need to make two surfaces.
        MemorySurface = DeviceSurface;
        DeviceSurface->AddRef();
    }

    // Get the texture handle.
    IDirect3DTexture2 *	Texture;
    DeviceSurface->QueryInterface(IID_IDirect3DTexture2, (void**)&Texture);
    Texture->GetHandle(Device, &Handle);
    Texture->Release();

    return DD_OK;
	unguard;
}

/*-----------------------------------------------------------------------------
	InitEmptyMip.
-----------------------------------------------------------------------------*/

HRESULT FD3DTexture::InitEmptyMip( IDirect3DDevice2 *Device, int width, int height, int depth, int numMips )
{
	guard(FD3DTexture::InitEmpty);
	HRESULT hres;
    IDirectDrawSurface* Target;

    // Get the render target (We need it to get the IDirectDraw).
    if (Device==NULL || Device->GetRenderTarget(&Target) != DD_OK)
        return FALSE;

    // Find the best texture format to use.
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize = sizeof(ddsd);
    ChooseTextureFormat( Device, depth, &ddsd.ddpfPixelFormat );
    ddsd.dwFlags |= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT;
    ddsd.dwWidth  = width;
    ddsd.dwHeight = height;
    
	// Create a video memory texture.
    D3DDEVICEDESC hal, hel;
    hal.dwSize = sizeof(hal);
    hel.dwSize = sizeof(hel);
    Device->GetCaps( &hal, &hel );

    // Should we check for texture caps?
	if( hal.dcmColorModel )
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_ALLOCONLOAD | DDSCAPS_COMPLEX | DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX | DDSCAPS_SYSTEMMEMORY;  
    ddsd.dwMipMapCount = numMips;

    hres = dd->CreateSurface( &ddsd, &DeviceSurface, NULL );
	if( hres != DD_OK )
	{
		Release();
		return hres;
	}

    // Create a system memory surface for the texture.
    // We use this system memory surface for the ::Load call
    // and this surface does not get lost.
    if( hal.dcmColorModel )
    {
        ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_TEXTURE;
		ddsd.dwMipMapCount  = numMips;
		ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP | DDSCAPS_COMPLEX | DDSCAPS_SYSTEMMEMORY;
		hres                = dd->CreateSurface(&ddsd, &MemorySurface, NULL);
		if( hres != DD_OK )
		{
			Release();
			return hres;
		}
    }
    else
    {
        // When dealing with a software rasterizer we dont need to make two surfaces.
        MemorySurface = DeviceSurface;
        DeviceSurface->AddRef();
    }

    // Get the texture handle.
    IDirect3DTexture2 *	Texture;
    DeviceSurface->QueryInterface(IID_IDirect3DTexture2, (void**)&Texture);
    Texture->GetHandle(Device, &Handle);
    Texture->Release();

    return DD_OK;
	unguard;
}

/*-----------------------------------------------------------------------------
	Restore.
-----------------------------------------------------------------------------*/

// Restore this D3D texture.
BOOL FD3DTexture::Restore( IDirectDraw* dd, IDirect3DDevice2 *Device )
{
	guard(FD3DTexture::Restore);
    HRESULT             err;
    IDirect3DTexture2  *MemoryTexture;
    IDirect3DTexture2  *DeviceTexture;

    if( DeviceSurface == NULL || MemorySurface == NULL )
        return FALSE;

    // We dont need to do this step for system memory surfaces.
    if( DeviceSurface == MemorySurface )
        return TRUE;

    // Restore the video memory texture.
	err = DeviceSurface->Restore();
    if( err != DD_OK )
	{
		debugf( "Restoring video memory surface failed: %s", CameraManager.ddError(err) );
        return FALSE;
	}

    // Call IDirect3DTexture::Load() to copy the texture to the device.
    DeviceSurface->QueryInterface( IID_IDirect3DTexture2, (void**)&DeviceTexture );
    MemorySurface->QueryInterface( IID_IDirect3DTexture2, (void**)&MemoryTexture );

    err = DeviceTexture->Load(MemoryTexture);
	if( err != DD_OK )
	{
		debugf( "Loading video memory surface failed: %s", CameraManager.ddError(err) );
        return FALSE;
	}

    // Get the texture handle.
    DeviceTexture->GetHandle(Device, &Handle);
	if( err !=DD_OK )
	{
		debugf("restore texture handle failed : %s",CameraManager.ddError(err));
        return FALSE;
	}

	// Release temporary interfaces.
    DeviceTexture->Release();
    MemoryTexture->Release();

    return TRUE;
	unguard;
}

/*-----------------------------------------------------------------------------
	Release.
-----------------------------------------------------------------------------*/

HRESULT FD3DTexture::Release()
{
	HRESULT hres;
    if( MemorySurface ) hres = MemorySurface->Release();   MemorySurface = 0;
    if( DeviceSurface ) hres = DeviceSurface->Release();   DeviceSurface = 0;
    if( Palette       ) hres = Palette->Release();         Palette = 0;
	return hres;
}

/*-----------------------------------------------------------------------------
	FindDeviceCallback.
-----------------------------------------------------------------------------*/

// Callback for finding the D3D device.
BOOL CALLBACK FindDeviceCallback( GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam )
{
    char ach[128];
    char * szFind = (char *)lParam;

    wsprintf(ach,"%s (%s)",szName, szDevice);

    if (lstrcmpi(szFind, szDevice) == 0 || lstrcmpi(szFind, ach) == 0)
    {
        DirectDrawCreate( lpGUID, &dd, NULL );
        return DDENUMRET_CANCEL;
    }
    return DDENUMRET_OK;
}

/*-----------------------------------------------------------------------------
	FDirect3DRenderDevice definition.
-----------------------------------------------------------------------------*/

//
// The Direct3D rendering device.
//
class FDirect3DRenderDevice : public FRenderDevice
{
public:
	// DirectDraw interfaces.
	IDirectDrawSurface			*FrontBuffer;
	IDirectDrawSurface			*BackBuffer;
	IDirectDrawSurface	        *ZBuffer;

	// Direct3D2 Interfaces.
	IDirect3D2					*d3d;
	IDirect3DDevice2			*d3dDevice;
	IDirect3DViewport2			*d3dViewport;
	IDirect3DMaterial2			*d3dBackground;	
	IDirect3DLight				*d3dLight;

	// Variables.
	char						*d3dName;
	SIZE						ScreenSize;	
	D3DMATERIALHANDLE			hBackground;	
	D3DDEVICEDESC				DeviceDesc;
	D3DVIEWPORT					Viewport;
	GUID						DeviceGUID;
	INT							DevicesFound;

	// Texture cache.
	enum {MAX_TEXTURES=256};
	typedef struct tagD3DTexCache
	{		
		UTexture			*Texture;		// The Unreal texture object being cached
		FD3DTexture			D3DTexture;		// d3d texture class
		INT					numMips;		// num mip levels
		BOOL				InUse;			// slot in use
		INT					TexValue;		// w*h = value, a weighting hint for memory fragmentation
		INT					UsageCount;		// slot usage count,for lru'ing
	} D3DTexCache;
	int						NumTexInCache;
	D3DTexCache				TexCache[MAX_TEXTURES];	

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

	// Internal functions.
	BOOL Init3D();
	BOOL DDInit(UCamera *Camera,char *szDevice=NULL, int canvasX=640, int canvasY=400);
	BOOL DDSetMode(int width, int height, int bpp);
	void DDTerm(BOOL fAll=TRUE);	

	// Internal texturing functions.
	D3DTEXTUREHANDLE	LockTexture(UTexture *Texture);
	void				UnlockTexture(UTexture *Texture);
	D3DTEXTUREHANDLE	RegisterTexture(UTexture *Texture);
	void				UnregisterTexture(UTexture *Texture);
	void				BuildTexture(D3DTexCache *CachedTexture,int mipLevel);
};

/*-----------------------------------------------------------------------------
	Init3D.
-----------------------------------------------------------------------------*/

//
// Init Direct3D rendering device.
//
BOOL FDirect3DRenderDevice::Init3D()
{
    HRESULT err = E_FAIL;

    // Create a Direct3D device.
    // first try HAL, then RGB, RAMP.
    d3dName = "HAL";
    err = d3d->CreateDevice(IID_IDirect3DHALDevice, BackBuffer, &d3dDevice);
    if( err != DD_OK )
    {
        d3dName = "RGB";
        err = d3d->CreateDevice(IID_IDirect3DRGBDevice, BackBuffer, &d3dDevice);
    }
    if( err != DD_OK )
    {
        d3dName = "RAMP";
        err = d3d->CreateDevice(IID_IDirect3DRampDevice, BackBuffer, &d3dDevice);
    }
    if( err != DD_OK )
        return FALSE;

    // Now make a Viewport
    err = d3d->CreateViewport(&d3dViewport, NULL);
    if( err != DD_OK )
        return FALSE;

	// Add viewport to device.
    err = d3dDevice->AddViewport(d3dViewport);

    // Setup the viewport for a reasonable viewing area.
    D3DVIEWPORT2 viewData;
    memset( &viewData, 0, sizeof(D3DVIEWPORT2) );
    viewData.dwSize       = sizeof(D3DVIEWPORT2);
    viewData.dwX          = 0;
    viewData.dwY          = 0;
    viewData.dwWidth      = ScreenSize.cx;
    viewData.dwHeight     = ScreenSize.cy;
	viewData.dvClipX      = 0.0f;
    viewData.dvClipY      = 0.0f;
	viewData.dvClipWidth  = ScreenSize.cx;
    viewData.dvClipHeight = ScreenSize.cy;
    viewData.dvMinZ       = 0.0f;
    viewData.dvMaxZ       = 1.0f;//1.0f;//2.0f;

    err = d3dViewport->SetViewport2( &viewData );
    if( err != DD_OK )
		return FALSE;

    err = d3dDevice->SetCurrentViewport( d3dViewport );
    if( err != DD_OK )
        return FALSE;

	// Init the render state.
    d3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,				D3DCULL_NONE);
	d3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE,				D3DSHADE_FLAT);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,				1);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ZFUNC,					D3DCMP_LESSEQUAL);
    d3dDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,			1);
	d3dDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE,			TRUE);
	d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE,	TRUE);
    d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAG,			D3DFILTER_MIPLINEAR);
    d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMIN,			D3DFILTER_MIPLINEAR);    	
    d3dDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE,		FALSE);
	d3dDevice->SetLightState (D3DLIGHTSTATE_FOGMODE,				D3DFOG_NONE);
    d3dDevice->SetLightState (D3DLIGHTSTATE_FOGSTART,				0);
    d3dDevice->SetLightState (D3DLIGHTSTATE_FOGEND,					0);

    return TRUE;
}


/*-----------------------------------------------------------------------------
	DDInit.
-----------------------------------------------------------------------------*/

BOOL FDirect3DRenderDevice::DDInit(UCamera *Camera,char *szDevice, int canvasX, int canvasY)
{
	guard(FDirect3DRenderDevice::DDInit);
    HRESULT err;

	// Kill any existing interfaces.
    DDTerm();

	// Enumerate devices in order to find an appropriate one.
    if( szDevice && szDevice[0] )
        DirectDrawEnumerate( FindDeviceCallback, (LPVOID)szDevice );
    if( dd == NULL )
	{
		// Create the default device.
        szDevice = NULL;
        err = DirectDrawCreate( NULL, &dd, NULL );
    }
    if( dd == NULL )
		return FALSE;

	// Go exclusive.
    err = dd->SetCooperativeLevel
	(
		GetCameraWindow(Camera),
		DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_NOWINDOWCHANGES
	);
	if (err != DD_OK)
	{			
		debugf( "SetCooperativeLevel failed: %s", CameraManager.ddError(err) );
        return FALSE;
	}

	// Get Direct3D2 interface.
    err = dd->QueryInterface( IID_IDirect3D2, (void**)&d3d );
    if( err != DD_OK )
    {
		debugf( "This device doesn't support Direct3D2" );
        return FALSE;
    }

	// Set mode to 16-bit color.
    if( !DDSetMode(canvasX,canvasY,16) && !DDSetMode(canvasX,canvasY,32) )
        return FALSE;

    char ach[128];
    wsprintf(ach, "%s - %s %s", szAppName, szDevice ? szDevice : "DISPLAY", d3dName);
	debugf("%s",ach);

    return TRUE;
	unguard;
}

/*-----------------------------------------------------------------------------
	DDSetMode.
-----------------------------------------------------------------------------*/

//
// Set the DirectDraw mode.
//
BOOL FDirect3DRenderDevice::DDSetMode( int width, int height, int bpp )
{
	guard(FDirect3DRenderDevice::DDSetMode);

	// Set the display mode.
    HRESULT err;
    err = dd->SetDisplayMode(width, height, bpp);
    if( err != DD_OK )
        return FALSE;

	// Set globals.
    ScreenSize.cx = width;
    ScreenSize.cy = height;

    // Get rid of any previous surfaces.
    DDTerm( FALSE );

	// Create surfaces.
    DDSURFACEDESC ddsd;
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize            = sizeof(ddsd);
    ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.dwBackBufferCount = 2;
    ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE | DDSCAPS_VIDEOMEMORY;

    // Try to get a tripple-buffered video memory surface.
    err = dd->CreateSurface( &ddsd, &FrontBuffer, NULL );
    if( err != DD_OK )
	{
		// Try a double-buffered surface.
		ddsd.dwBackBufferCount = 1;
        err = dd->CreateSurface( &ddsd, &FrontBuffer, NULL );
		if( err != DD_OK )
		{
			// settle for a main memory surface.
			ddsd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
			err = dd->CreateSurface(&ddsd, &FrontBuffer, NULL);
		}
	}
    if( err != DD_OK )
        return FALSE;

    // Get a pointer to the back buffer.
    DDSCAPS caps;
    caps.dwCaps = DDSCAPS_BACKBUFFER;
    err = FrontBuffer->GetAttachedSurface(&caps, &BackBuffer);

    if( err != DD_OK )
        return FALSE;
    
    // Create the z-buffer.
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize            = sizeof(ddsd);
    ddsd.dwFlags           = DDSD_CAPS
                           | DDSD_WIDTH
                           | DDSD_HEIGHT
                           | DDSD_ZBUFFERBITDEPTH;
    ddsd.ddsCaps.dwCaps    = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
    ddsd.dwWidth           = width;
    ddsd.dwHeight          = height;
    ddsd.dwZBufferBitDepth = bpp;
    err = dd->CreateSurface( &ddsd, &ZBuffer, NULL );
    if( err != DD_OK )
        return err;

	// Attach z-buffer to the rendering target.
    err = BackBuffer->AddAttachedSurface(ZBuffer);
    if( err!= DD_OK )
        return err;

	// Init 3D.
    return Init3D();
    unguard;
}

/*-----------------------------------------------------------------------------
	DDTerm.
-----------------------------------------------------------------------------*/

//
// Terminate DirectDraw.
//
void FDirect3DRenderDevice::DDTerm( BOOL fAll )
{	
	guard(FDirect3DRenderDevice::DDTerm);
    SAFERELEASE(d3dDevice);
    SAFERELEASE(d3dViewport);
    SAFERELEASE(d3dLight);

    SAFERELEASE(BackBuffer);
    SAFERELEASE(FrontBuffer);

    if( fAll )
    {
        SAFERELEASE(d3d);
        SAFERELEASE(dd);
    }
	unguard;
}

/*-----------------------------------------------------------------------------
	FDirect3DRenderDevice Init & Exit.
-----------------------------------------------------------------------------*/

//
// Initialize D3D. Can't fail.
//
int FDirect3DRenderDevice::Init3D( UCamera *Camera, int RequestX, int RequestY )
{
	guard(FDirect3DRenderDevice::Init3D);
	checkState(!Active);
	debugf("Initializing Direct3D...");

	// read the device name from WIN.INI so we come up on the last device that the user picked.
	// must fix this to use whatever device picking mechanism unreal uses
	//		examine flip3d dx5 sdk sample code to see how it enums,picks & uses a similar mechanism to save choice
	GetProfileString( szAppName, "Device", "", DeviceName, sizeof(DeviceName) );

	if( !DDInit( Camera, DeviceName, RequestX, RequestY ) )
		return FALSE;

	// Create the background material.
    CRITICALCALL( d3d->CreateMaterial( &d3dBackground, NULL ) );
	D3DMATERIAL Background;
    ZeroMemory(&Background, sizeof(Background));
    Background.dwSize        = sizeof(Background);
    Background.dcvDiffuse.r  = D3DVAL(0.0);
    Background.dcvDiffuse.g  = D3DVAL(0.0);
    Background.dcvDiffuse.b  = D3DVAL(0.5);
    Background.dcvAmbient.r  = D3DVAL(0.0);
    Background.dcvAmbient.g  = D3DVAL(0.0);
    Background.dcvAmbient.b  = D3DVAL(0.0);
    Background.dcvSpecular.r = D3DVAL(0.0);
    Background.dcvSpecular.g = D3DVAL(0.0);
    Background.dcvSpecular.b = D3DVAL(0.0);
    Background.dvPower       = D3DVAL(0.0);
    Background.dwRampSize    = 1;
    CRITICALCALL( d3dBackground->SetMaterial( &Background) );
    CRITICALCALL( d3dBackground->GetHandle  ( (IDirect3DDevice2 *)d3dDevice, &hBackground) );

	// Attach the background material to the viewport.
    CRITICALCALL( d3dViewport->SetBackground(hBackground) );

	// Init general info.
	Active      = 1;
	Locked      = 0;

	// Init texture cache.
	NumTexInCache = 0;

	// Success.
	return TRUE;
	unguard;
}

//
// Shut down the Direct3D device.
//
void FDirect3DRenderDevice::Exit3D()
{
	guard(FDirect3DRenderDevice::Exit3D);
	checkState(Active);
	debugf( LOG_Init, "Shutting down Direct3D" );
	HRESULT err;

	// Reset mode.		
	err = dd->SetCooperativeLevel( NULL, DDSCL_NORMAL );
	if( err != DD_OK )
		appErrorf( "SetCooperativeLevel failed: %s", CameraManager.ddError(err) );

	err = dd->RestoreDisplayMode();		
	if( err != DD_OK )
		appErrorf( "RestoreDisplayMode failed: %s", CameraManager.ddError(err) );
	
	WriteProfileString( szAppName, "Device", DeviceName );
	
	// Release objects.	
	DDTerm();

	// Note we're inactive.
	Active = 0;

	// Shut down Direct3D.
	debugf( LOG_Exit, "Direct3D successfully shut down" );
	unguard;
};

//
// Flush all cached data.
//
void FDirect3DRenderDevice::Flush3D()
{
	guard(FDirect3DRenderDevice::Flush3D);
	checkState(Active);
	checkState(!Locked);

	// Flush all cached textures.
	for( int i=NumTexInCache-1; i>=0; i-- )
		UnregisterTexture(TexCache[i].Texture);

	unguard;
}

/*-----------------------------------------------------------------------------
	FDirect3DRenderDevice Lock & Unlock.
-----------------------------------------------------------------------------*/

//
// Lock the Direct3D device.
//
void FDirect3DRenderDevice::Lock3D( UCamera *Camera )
{
	HRESULT err;
	D3DRECT				d3dRect;
	guard(FDirect3DRenderDevice::Lock3D);
	checkState(!Locked);

	d3dRect.lX1 = 0;
	d3dRect.lY1 = 0;
	d3dRect.lX2 = Camera->X;
	d3dRect.lY2 = Camera->Y;
	err = d3dViewport->Clear( 1UL, &d3dRect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER );

    // Lock Direct3D.
	CRITICALCALL( d3dDevice->BeginScene() );
	
	Locked = 1;
	unguard;
};

//
// Unlock the Direct3D rendering device.
//
void FDirect3DRenderDevice::Unlock3D( UCamera *Camera, int Blit )
{
//	HRESULT err;
	guard(FDirect3DRenderDevice::Unlock3D);
	checkState(Locked);

	// We have rendered the backbuffer, sp call flip so we can see it.
    FrontBuffer->Flip( NULL, DDFLIP_WAIT );

    // Unlock Direct3D.
	CRITICALCALL( d3dDevice->EndScene() );

	Locked = 0;
	unguard;
};

/*-----------------------------------------------------------------------------
	FD3DDevice Texture caching.
-----------------------------------------------------------------------------*/

//
// Build a texture by spraying bits on the sys-mem surf
// called on RegisterTexture   
// Requires that the texture's surface is valid and locked.
//
void FDirect3DRenderDevice::BuildTexture( D3DTexCache *CachedTexture, int MipLevel )
{
	guard(FD3DDevice::BuildTexture);

    IDirectDrawPalette *Palette = NULL;
	UTexture           *Texture = CachedTexture->Texture;
	FMipInfo           &Mip     = Texture->Mips[MipLevel];

	// If palettized, get the palette.
    if( CachedTexture->D3DTexture.ddsd.ddpfPixelFormat.dwRGBBitCount == 8 )
    {
		// Attach to surface and texture.
        Palette = PaletteFromUPalette( dd, Texture->Palette );
        CachedTexture->D3DTexture.MemorySurface->SetPalette( Palette );
        CachedTexture->D3DTexture.DeviceSurface->SetPalette( Palette );
    }

	// Copy stuff to system memory surface.
	BYTE* Src  = &Texture->Element(Mip.Offset);
	BYTE* Dest = (BYTE *)CachedTexture->D3DTexture.ddsd.lpSurface;
	for( int i = 0; i<(int)Mip.VSize; i++ )
	{
		memcpy( Dest, Src, Mip.USize );
		Dest += CachedTexture->D3DTexture.ddsd.lPitch;
		Src  += CachedTexture->D3DTexture.ddsd.lPitch;
	}
	unguard;
}

//
// Register a texture with the 3D hardware. 
// May cause one or more other textures to be flushed.  
// Returns with the system-memory texture surface locked.
//
D3DTEXTUREHANDLE FDirect3DRenderDevice::RegisterTexture( UTexture *Texture )
{
	guard(FDirect3DRenderDevice::RegisterTexture);

	// Make sure this texture is cacheable.
	if( Texture->USize != Texture->VSize )
	{
		//!!
		debugf( "nonsquare texture size: %i %i (%s)", Texture->USize, Texture->VSize, Texture->GetName() );
		return NULL;
	}

	// Validate the texture size.
	if( Texture->USize>1024 || (Texture->USize&(Texture->USize-1)))
		appErrorf( "Invalid texture U size: %i (%s)", Texture->USize, Texture->GetName() );
	if( Texture->VSize>1024 || (Texture->VSize&(Texture->VSize-1)))
		appErrorf( "Invalid texture V size: %i (%s)", Texture->USize, Texture->GetName() );

	// Make texture entry.
	if( NumTexInCache >= MAX_TEXTURES )
	{
		// Dump all textures. !!Should be lru.
		Flush3D(); 
	}

	// Create cache entry.
	D3DTexCache *CachedTexture       = &TexCache[NumTexInCache++];
	CachedTexture->Texture			 = Texture;
	CachedTexture->D3DTexture.Handle = NULL;
	
	// Find number of mipmap levels.
	int i=0, n=0;
	while( i < MAX_MIPS )
	{
		if( Texture->Mips[i].Offset == MAXDWORD )
			break;
		else
			n++;
		i++;
	}

	// Create empty system and video memory surfaces.
	HRESULT DDResult; 
	if( n == 1)
		DDResult = CachedTexture->D3DTexture.InitEmpty(d3dDevice,Texture->USize,Texture->VSize,8);
	else
		DDResult = CachedTexture->D3DTexture.InitEmptyMip(d3dDevice,Texture->USize,Texture->VSize,8,n);
	if( DDResult != DD_OK )
	{
		// Must start flushing stuff!
		appErrorf( "Failed creating texture %s (%ix%i)", Texture->GetName(), Texture->USize, Texture->VSize );
	}

	// Fetch a pointer to the sys-mem texture surface.
	DDResult = CachedTexture->D3DTexture.MemorySurface->Lock
	(
		NULL,
		&CachedTexture->D3DTexture.ddsd,
		DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
		NULL
	);
	if( DDResult )
	{
		appErrorf("DirectDraw lock failed: %s",CameraManager.ddError(DDResult));		
		return NULL;
	}

	//now sys-mem surface locked, spray bits
	BuildTexture(CachedTexture,0);

	// Unlock system memory texture, since can now upload at will.
	DDResult = CachedTexture->D3DTexture.MemorySurface->Unlock( NULL );
	
	// Deal with 1 to NumMips surfaces.
	if( n > 1 )
	{		
		
		LPDIRECTDRAWSURFACE lpDDThisLevel = CachedTexture->D3DTexture.MemorySurface;
		LPDIRECTDRAWSURFACE lpDDNextLevel;
		DDSCAPS ddsCaps;
		i = 1;
		while( i < n )
		{
			ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
			DDResult = lpDDThisLevel->GetAttachedSurface(	&ddsCaps, &lpDDNextLevel);
			if( DDResult != DD_OK )
			{
				break;
			}

			// Fetch a pointer to the ith sys-mem texture surface.
			DDResult = lpDDNextLevel->Lock
			(
				NULL,
				&CachedTexture->D3DTexture.ddsd,
				DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
				NULL
			);			
			if( DDResult )
			{
				appErrorf( "DirectDraw lock failed: %s", CameraManager.ddError(DDResult) );
				return NULL;
			}
			
			// Now ith system-memory surface is locked, so copy data.
			BuildTexture(CachedTexture, i);

			// Unlock it.
			DDResult = lpDDNextLevel->Unlock(NULL);

			// Go to next one.
			lpDDThisLevel = lpDDNextLevel;
			i++;
		}
	}

	// Now download the texture by restoring the surface.
	if( CachedTexture->D3DTexture.Restore( dd, d3dDevice ) )
	{
		CachedTexture->InUse	= TRUE;
		CachedTexture->TexValue = Texture->USize*Texture->VSize;
		CachedTexture->UsageCount++;
		return CachedTexture->D3DTexture.Handle;
	}
	else
	{
		appErrorf( "Failed creating texture %s (%ix%i)", Texture->GetName(), Texture->USize, Texture->VSize );
		return NULL;
	}
	unguard;
}

//
// Unregister a texture with the 3D hardware.
//
void FDirect3DRenderDevice::UnregisterTexture( UTexture *Texture )
{
	guard(FDirect3DRenderDevice::UnregisterTexture);
	D3DTexCache *CachedTexture = NULL;

	// Find the texture in the list and remove it.
	int j=0;
	for( int i=0; i<NumTexInCache; i++ )
	{
		if (j!=i) TexCache[j]=TexCache[i];
		if (TexCache[i].Texture==Texture)
			CachedTexture = &TexCache[i];
		else
			j++;
	}
	if( !CachedTexture )
		appErrorf("Texture %s not found in cache",Texture->GetName());
	NumTexInCache = j;

	// Unregister the texture.
	checkState(CachedTexture->D3DTexture.Handle!=NULL);

	// Release the surfaces.
 	HRESULT DDResult = CachedTexture->D3DTexture.Release();
	if( DDResult )
		appErrorf( "Error releasing %s: %s", CachedTexture->Texture->GetName(), CameraManager.ddError(DDResult) );

	// Clear in-use flag.
	CachedTexture->InUse		= FALSE;
	CachedTexture->UsageCount	= 0;
	CachedTexture->TexValue		= 0;

	unguard;
}

//
// Look up a texture from 3D hardware and lock it.  
// Either returns an existing, cached texture, or registers a new one and returns it.
//
D3DTEXTUREHANDLE FDirect3DRenderDevice::LockTexture( UTexture *Texture )
{
	guard(FDirect3DRenderDevice::LockTexture);
	D3DTexCache *CachedTexture = NULL;

	// See if texture exists in cache.
	for( int i=0; i<NumTexInCache; i++ )
	{
		CachedTexture = &TexCache[i];
		if( CachedTexture->Texture == Texture )
		{
			// Restore from system memory copy if it's lost.
			if( CachedTexture->D3DTexture.DeviceSurface->IsLost() )			
				CachedTexture->D3DTexture.Restore(dd,d3dDevice);
			CachedTexture->UsageCount++;
			return CachedTexture->D3DTexture.Handle;
		}
	}

	// Not found, so create a new texture and return it.
	return RegisterTexture( Texture );
	unguard;
}

//
// Unlock a texture.
//
void FDirect3DRenderDevice::UnlockTexture( UTexture *Texture )
{
	guard(FDirect3DRenderDevice::UnlockTexture);
	for( int i=0; i<NumTexInCache; i++ )
	{
		D3DTexCache *CachedTexture = &TexCache[i];
		if( CachedTexture->Texture == Texture )
		{
			// System memory surface already unlocked.
			return;
		}
	}
	appErrorf( "Texture %s not found for unlock", Texture->GetName() );
	unguard;
}

/*-----------------------------------------------------------------------------
	FDirect3DRenderDevice texture vector polygon drawer.
-----------------------------------------------------------------------------*/

#define C_64K 65536
#define C_32K 32768
static	D3DTEXTUREHANDLE	hLastDecalTexture = NULL;
static  UTexture*			uLastDecalTexture = NULL;
static	D3DTEXTUREHANDLE	hLastLMTexture	= NULL;
static  UTexture*			uLastLMTexture	= NULL;

//
// Draw a textured polygon using surface vectors.
//
void FDirect3DRenderDevice::DrawPolyV
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
	guard(FDirect3DRenderDevice::DrawPolyV);

	D3DTEXTUREHANDLE  hTexture; 
	D3DTEXTUREHANDLE  hLMTexture; 
	
	// Set the rendering state.
	d3dDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT );	
	d3dDevice->SetRenderState( D3DRENDERSTATE_BLENDENABLE, FALSE );
	d3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECAL );

	// Setup first-pass texture, decal mode	
	if( Texture != uLastDecalTexture )
	{		
		hTexture = LockTexture(Texture);
		CRITICALCALL( d3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREHANDLE, hTexture ) );
	}
	else CRITICALCALL( d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,hLastDecalTexture) );

	// Translate the points to D3DTLVERTEX's.
	D3DTLVERTEX Verts[32];
	for( int i=0; i<NumPts; i++ )
	{
		const FTransform  &Pt   = Pts  [i];
		D3DTLVERTEX       &Vert = Verts[i];

		// Set screen coords.
		Vert.sx			= Pt.ScreenX/C_64K;
		Vert.sy			= Pt.ScreenY;
		Vert.sz			= Pt.Z/C_32K;
		Vert.rhw		= 1.0f/Pt.Z;	
		Vert.color		= RGB_MAKE(0,0,0);
		Vert.tu			= (((FVector&)Pt - Base) | U) / (Texture->USize);
		Vert.tv			= (((FVector&)Pt - Base) | V) / (Texture->VSize);
	}

	// Draw with DrawPrimitive.
	CRITICALCALL( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX, Verts, NumPts, 0 ) );

	// Remember texture.
	if( Texture != uLastDecalTexture )
	{
		hLastDecalTexture = hTexture;
		uLastDecalTexture = Texture;
	}

	// Light maps.
	d3dDevice->SetRenderState(D3DRENDERSTATE_BLENDENABLE, TRUE);
	d3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,    D3DBLEND_ZERO);
	d3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,   D3DBLEND_SRCCOLOR);
	if( Texture != uLastLMTexture )
	{		
		hLMTexture = LockTexture(Texture);
		CRITICALCALL( d3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREHANDLE, hLMTexture ) );
	}
	else CRITICALCALL( d3dDevice->SetRenderState( D3DRENDERSTATE_TEXTUREHANDLE, hLastLMTexture ) );

	// Translate the points to D3DTLVERTEX's.
	for( i=0; i<NumPts; i++ )
	{
		const FTransform  &Pt   = Pts  [i];
		D3DTLVERTEX       &Vert = Verts[i];

		// Set screen coords.
		Vert.sx			= Pt.ScreenX/C_64K;
		Vert.sy			= Pt.ScreenY;
		Vert.sz			= Pt.Z/C_32K;
		Vert.rhw		= 1.0f/Pt.Z;	
		Vert.color		= RGB_MAKE(255,255,255);
		Vert.specular	= RGB_MAKE(0,0,0);
		Vert.tu			= (((FVector&)Pt - Base) | U) / (Texture->USize) / 16;
		Vert.tv			= (((FVector&)Pt - Base) | V) / (Texture->VSize) / 16;
	}

	// Draw with DrawPrimitive.
	CRITICALCALL( d3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX, Verts, NumPts, 0) );

	// Remember light map.
	if( Texture != uLastLMTexture )
	{
		hLastLMTexture = hLMTexture;
		uLastLMTexture = Texture;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	FDirect3DRenderDevice texture coordinates polygon drawer.
-----------------------------------------------------------------------------*/

static D3DTEXTUREHANDLE	hCLastDecalTexture = NULL;
static UTexture*		uCLastDecalTexture = NULL;
static D3DTEXTUREHANDLE	hCLastLMTexture	= NULL;
static UTexture*		uCLastLMTexture	= NULL;

//
// Draw a polygon with texture coordinates.
//
void FDirect3DRenderDevice::DrawPolyC
(
	UCamera*			Camera,
	UTexture*			Texture,
	const FTransTexture*Pts,
	int					NumPts,
	DWORD				PolyFlags
)
{	
	guard(FDirect3DRenderDevice::DrawPolyC);
	checkState(Locked);	
	D3DTEXTUREHANDLE  hCTexture;

	// Setup creature texture, shade & blend mode.
	d3dDevice->SetRenderState(D3DRENDERSTATE_BLENDENABLE,        FALSE);
	d3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE,			 D3DSHADE_GOURAUD);	
	d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND,	 D3DTBLEND_MODULATE);
	if ( Texture != uCLastDecalTexture )
	{		
		hCTexture = LockTexture(Texture);
		CRITICALCALL( d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,hCTexture) );
	}
	else CRITICALCALL( d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,hCLastDecalTexture) );

	// Translate the points to D3DTLVERTEX's.
	D3DTLVERTEX Verts[32];
	for( int i=0; i<NumPts; i++ )
	{
		const FTransTexture &Pt = Pts[i];
		D3DTLVERTEX &Vert = Verts[i];

		// Set screen coords.
		Vert.sx			= Pt.ScreenX;
		Vert.sy			= Pt.ScreenY;
		Vert.sz			= Pt.Z/C_32K;
		Vert.rhw		= 1.0 / Pt.Z;
		BYTE r			= Min(Pt.Color.R * (2.0f/256.f),255.f);
		BYTE g			= Min(Pt.Color.G * (2.0f/256.f),255.f);
		BYTE b			= Min(Pt.Color.B * (2.0f/256.f),255.f);
		Vert.color		= RGB_MAKE(r, g, b);
		Vert.specular	= RGB_MAKE(0, 0, 0);
		Vert.tu			= Pt.U / (Texture->USize * 65536.0);
		Vert.tv			= Pt.V / (Texture->VSize * 65536.0);
	}

	// Draw with DrawPrimitive.
	CRITICALCALL( d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX, Verts, NumPts, 0) );

	//last used caching, creature
	if ( Texture != uCLastDecalTexture )
	{
		hCLastDecalTexture = hCTexture;
		uCLastDecalTexture = Texture;
	}

	// Revert to polyv 1st pass blend mode.
	d3dDevice->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECAL);
	unguard;
}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/

//
// Execute a command.
//
int FDirect3DRenderDevice::Exec( const char *Cmd,FOutputDevice *Out )
{
	guard(FDirect3DRenderDevice::Exec);
	const char *Str = Cmd;

	return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Finder.
-----------------------------------------------------------------------------*/

FDirect3DRenderDevice GDirect3DRenDev;

#if 0
//
// Return the Direct3D render device if it's available.
//
FRenderDevice *FindRenderDevice()
{
	int FoundDirect3D = 0;
	debugf("%s","found d3d render device");
	if( CameraManager.dd )
	{
		// See if the DirectDraw device has a Direct3D.
		IDirect3D2 *d3d = NULL;
		CameraManager.dd->QueryInterface( IID_IDirect3D2, (void**)&d3d );
		if( d3d )
		{
			FoundDirect3D = 1;
			d3d->Release();
		}
	}
	else
	{   
		IDirectDraw *dd;
        HRESULT err = DirectDrawCreate( NULL, &dd, NULL );
		if( dd != NULL )
		{
			IDirect3D2 *d3d;
			dd->QueryInterface( IID_IDirect3D2, (void**)&d3d );
			if( d3d )
			{
				FoundDirect3D = 1;
				d3d->Release();
			}
			dd->Release();
		}
	} 

	// Found it?
	if( FoundDirect3D )
	{
		debugf( LOG_Init, "Found Direct3D" );
		return &GDirect3DRenDev;
	}
	else
	{
		debugf( LOG_Init, "No Direct3D detected" );
		return NULL;
	}
}
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
