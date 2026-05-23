/*=============================================================================
	UnD3D.cpp: Unreal support for the Microsoft Direct3D library.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

// Precompiled header.
#include "StdAfx.h"
#include "UnWnCam.h"
#include "UnRender.h"

// Unreal includes.
#include "UnRenDev.h"

// Direct3D includes.
#include "d3d.h"
#include "d3dcaps.h"

// The global camera manager.
extern FWindowsCameraManager CameraManager;

// Macro for calling d3d functions and displaying errors.
#define EXECUTE_ASSERT checkLogic
#define DEBUGCALL(func,tag) {HRESULT hRes=func; if(hRes) debugf("%s: %s",tag,CameraManager.ddError(hRes));}
#define CRITICALCALL(func,tag) {HRESULT hRes=func; if(hRes) appErrorf("%s: %s",tag,CameraManager.ddError(hRes));}
#define USE_DRAWPRIMITIVE 0

/*-----------------------------------------------------------------------------
	Execute buffer assembler.
-----------------------------------------------------------------------------*/

//
// Define the EXECUTE_ASSERT(x) macro to enable overflow checking while
// building execute buffers, at a small cost in performance.
//
#ifndef EXECUTE_ASSERT
#define EXECUTE_ASSERT(x)
#endif

//
// A C++ class for assembling execute buffers easily.
//
class CExecuteBuffer
{
private:
	///////////////////////
	// Private variables //
	///////////////////////

	// Constants.
	enum{ MAX_SIZE = 65536};

	// Direct3D info.
	IDirect3DDevice			*Device;
	IDirect3DExecuteBuffer	*Exec;
	IDirect3DViewport		*Viewport;
	D3DEXECUTEBUFFERDESC	ExecDesc;
	D3DEXECUTEDATA			ExecData;

	// Start and end of buffer.
	BYTE *Start, *End;
	D3DINSTRUCTION *PreviousTop;

	// Pointer to current top of buffer.
	union
	{
		INT					TopINT;
		BYTE				*TopBYTE;
		WORD				*TopWORD;
		DWORD				*TopDWORD;
		D3DINSTRUCTION		*TopINSTRUCTION;
		D3DPOINT			*TopPOINT;
		D3DLINE				*TopLINE;
		D3DTRIANGLE			*TopTRIANGLE;
		D3DMATRIXLOAD		*TopMATRIXLOAD;
		D3DMATRIXMULTIPLY	*TopMATRIXMULTIPLY;
		D3DSTATE			*TopSTATE;
		D3DPROCESSVERTICES	*TopPROCESSVERTICES;
		D3DTEXTURELOAD		*TopTEXTURELOAD;
		D3DBRANCH			*TopBRANCH;
		D3DSPAN				*TopSPAN;
		D3DSTATUS			*TopSTATUS;
	};

	// Most recently written opcode.
	D3DOPCODE PreviousOpcode;

public:
	/////////////////
	// Init & Exit //
	/////////////////

	// Init this execute buffer.
	void Init( IDirect3DDevice *InDevice, IDirect3DViewport *InViewport )
	{
		Start = TopBYTE = new BYTE[MAX_SIZE];
		PreviousOpcode	= (D3DOPCODE)255;
		Device			= InDevice;
		Viewport		= InViewport;
		End				= &Start[MAX_SIZE];
		PreviousTop		= NULL;
		Exec			= NULL;
	}

	// Exit.
	void Exit()
		{if( Exec ) Exec->Release();}

	//////////////////////
	// Helper functions //
	//////////////////////

	// Return number of bytes used in the execute buffer.
	INT GetSize()
		{return Align(TopBYTE - Start,8);}

	// Finish filling the execute buffer with code.
	HRESULT Make( DWORD NumVertices, DWORD MaxVertices, DWORD VertexSize )
	{
		// Make Direct3D execute buffer description.
		memset(&ExecDesc,0,sizeof(ExecDesc));
		ExecDesc.dwSize			= sizeof(ExecDesc);
		ExecDesc.dwFlags		= D3DDEB_BUFSIZE | D3DDEB_CAPS;
		ExecDesc.dwCaps			= D3DDEBCAPS_SYSTEMMEMORY;
		ExecDesc.dwBufferSize	= GetSize() + MaxVertices * VertexSize;
		ExecDesc.lpData			= NULL;

		// Create the execute buffer.	
		HRESULT hRes = Device->CreateExecuteBuffer( &ExecDesc, &Exec, NULL );
		if( hRes ) return hRes;

		// Lock the execute buffer.
		memset(&ExecDesc,0,sizeof(ExecDesc));
		ExecDesc.dwSize = sizeof(ExecDesc);
		hRes = Exec->Lock( &ExecDesc );
		if( hRes ) return hRes;

		// Fill the execute buffer with our commands.
		memcpy( ExecDesc.lpData, Start, GetSize() );

		// Get rid of temporary memory.
		delete Start;

		// Unlock the execute buffer.
		hRes = Exec->Unlock();
		if( hRes ) return hRes;

		// Set execute buffer's data info.
		memset(&ExecData,0,sizeof(ExecData));
		ExecData.dwSize					= sizeof(ExecData);
		ExecData.dwVertexOffset			= GetSize();
		ExecData.dwVertexCount			= NumVertices;
		ExecData.dwInstructionOffset	= 0;
		ExecData.dwInstructionLength	= GetSize();

		// Set execute buffer's data.
		return Exec->SetExecuteData(&ExecData);
	}

	// Execute the execute buffer.
	HRESULT Execute( DWORD dwFlags )
		{return Device->Execute( Exec, Viewport, dwFlags );}

	HRESULT Lock( D3DTLVERTEX *&FirstVertex )
	{
		ExecDesc.dwSize = sizeof(ExecDesc);
		HRESULT hRes = Exec->Lock(&ExecDesc);
		if( hRes ) return hRes;
		FirstVertex = (D3DTLVERTEX *)ExecDesc.lpData + GetSize();
		return 0;
	}

	HRESULT Unlock()
		{return Exec->Unlock();}

	///////////////////////////////////////////////////////
	// Functions to write things into the execute buffer //
	///////////////////////////////////////////////////////

	// Raw instruction.
	// EXECUTE_ASSERTs that there is enough space for the opcode and its operands.
	// bOpcode = opcode to write.
	// bSize   = size of the operand array.
	// wCount  = number of operands in the operand array.
	void INSTRUCTION( D3DOPCODE bOpcode, BYTE bSize, WORD wCount )
	{
		if( PreviousOpcode == bOpcode )
		{
			// Tack this onto the end of the previous instruction.
			EXECUTE_ASSERT( TopBYTE + (INT)bSize * (INT)wCount < End );
			TopINSTRUCTION->wCount++;
		}
		else
		{
			// Generate a new instruction.
			EXECUTE_ASSERT( TopBYTE + sizeof(D3DINSTRUCTION) + (INT)bSize * (INT)wCount < End );
			PreviousTop    = TopINSTRUCTION;
			PreviousOpcode = bOpcode;

			TopINSTRUCTION->bOpcode	= bOpcode;
			TopINSTRUCTION->bSize   = bSize;
			TopINSTRUCTION->wCount	= wCount;
			TopINSTRUCTION++;
		}
	}

	// Dealign.
	void DEALIGN()
	{
		if( (TopINT & 7) == 0 )
		{
			EXECUTE_ASSERT( TopBYTE + sizeof(D3DINSTRUCTION) < End );
			PreviousOpcode = (D3DOPCODE)255;

			TopINSTRUCTION->bOpcode	= D3DOP_TRIANGLE;
			TopINSTRUCTION->bSize   = sizeof(D3DTRIANGLE);
			TopINSTRUCTION->wCount	= 0;
			TopINSTRUCTION++;
		}
	}

	// Point.
	// wFirst = index of first point into the execute buffer's vertex array.
	// wCount = number of points to render from the execute buffer's vertex array.
    void POINT( WORD wFirst, WORD wCount )
	{
		INSTRUCTION( D3DOP_POINT, sizeof(D3DPOINT), 1 );
		TopPOINT->wFirst        = wFirst;
		TopPOINT->wCount        = wCount;
		TopPOINT++;
	}

	// Line.
	void LINE( WORD wV1, WORD wV2 )
	{
		DEALIGN();
		INSTRUCTION( D3DOP_LINE, sizeof(D3DLINE), 1 );
		TopLINE->wV1            = wV1;
		TopLINE->wV2            = wV2;
		TopLINE++;
    };

	// Triangle.
	void TRIANGLE( WORD wV1, WORD wV2, WORD wV3, WORD wFlags=0 )
	{
		DEALIGN();
		INSTRUCTION( D3DOP_TRIANGLE, sizeof(D3DTRIANGLE), 1 );
		TopTRIANGLE->wV1        = wV1;
		TopTRIANGLE->wV2        = wV2;
		TopTRIANGLE->wV3        = wV3;
		TopTRIANGLE->wFlags     = wFlags;
		TopTRIANGLE++;
	}

	// Matrix load.
	void MATRIXLOAD( D3DMATRIXHANDLE hDestMatrix, D3DMATRIXHANDLE hSrcMatrix )
	{
		INSTRUCTION( D3DOP_MATRIXLOAD, sizeof(D3DMATRIXLOAD), 1 );
		TopMATRIXLOAD->hDestMatrix  = hDestMatrix;
		TopMATRIXLOAD->hSrcMatrix   = hSrcMatrix;
		TopMATRIXLOAD++;
	}

	// Matrix multiply.
	void MATRIXMULTIPLY( D3DMATRIXHANDLE hDestMatrix, D3DMATRIXHANDLE hSrcMatrix1, D3DMATRIXHANDLE hSrcMatrix2 )
	{
		INSTRUCTION( D3DOP_MATRIXMULTIPLY, sizeof(D3DMATRIXMULTIPLY), 1 );
		TopMATRIXMULTIPLY->hDestMatrix = hDestMatrix;
		TopMATRIXMULTIPLY->hSrcMatrix1 = hSrcMatrix1;
		TopMATRIXMULTIPLY->hSrcMatrix2 = hSrcMatrix2;
		TopMATRIXMULTIPLY++;
	}

	// State transform.
	void STATETRANSFORM_I( D3DTRANSFORMSTATETYPE dtstTransformStateType, DWORD Arg )
	{
		INSTRUCTION( D3DOP_STATETRANSFORM, sizeof(D3DSTATE), 1 );
		TopSTATE->dtstTransformStateType = dtstTransformStateType;
		TopSTATE->dwArg[0]               = Arg;
		TopSTATE++;
	}
	void STATETRANSFORM_F( D3DTRANSFORMSTATETYPE dtstTransformStateType, D3DVALUE Arg )
	{
		INSTRUCTION( D3DOP_STATETRANSFORM, sizeof(D3DSTATE), 1 );
		TopSTATE->dtstTransformStateType = dtstTransformStateType;
		TopSTATE->dvArg[0]               = Arg;
		TopSTATE++;
	}

	// State light.
	void STATELIGHT_I( D3DLIGHTSTATETYPE dlstLightStateType, DWORD Arg )
	{
		INSTRUCTION( D3DOP_STATELIGHT, sizeof(D3DSTATE), 1 );
		TopSTATE->dlstLightStateType     = dlstLightStateType;
		TopSTATE->dwArg[0]               = Arg;
		TopSTATE++;
	}
	void STATELIGHT_F( D3DLIGHTSTATETYPE dlstLightStateType, D3DVALUE Arg )
	{
		INSTRUCTION( D3DOP_STATELIGHT, sizeof(D3DSTATE), 1 );
		TopSTATE->dlstLightStateType     = dlstLightStateType;
		TopSTATE->dvArg[0]               = Arg;
		TopSTATE++;
	}

	// State render.
	void STATERENDER_I( D3DRENDERSTATETYPE drstRenderStateType, DWORD Arg )
	{
		INSTRUCTION( D3DOP_STATERENDER, sizeof(D3DSTATE), 1 );
		TopSTATE->drstRenderStateType    = drstRenderStateType;
		TopSTATE->dwArg[0]               = Arg;
		TopSTATE++;
	}
	void STATERENDER_F( D3DRENDERSTATETYPE drstRenderStateType, D3DVALUE Arg )
	{
		INSTRUCTION( D3DOP_STATERENDER, sizeof(D3DSTATE), 1 );
		TopSTATE->drstRenderStateType    = drstRenderStateType;
		TopSTATE->dvArg[0]               = Arg;
		TopSTATE++;
	}

	// Process vertices.
	void PROCESSVERTICES( DWORD dwFlags, WORD wStart, WORD wDest, DWORD dwCount, DWORD dwReserved )
	{
		INSTRUCTION( D3DOP_PROCESSVERTICES, sizeof(D3DPROCESSVERTICES), 1 );
		TopPROCESSVERTICES->dwFlags		= dwFlags;
		TopPROCESSVERTICES->wStart		= wStart;
		TopPROCESSVERTICES->wDest		= wDest;
		TopPROCESSVERTICES->dwCount		= dwCount;
		TopPROCESSVERTICES->dwReserved	= dwReserved;
		TopPROCESSVERTICES++;
	}

	// Texture load.
	void TEXTURELOAD( D3DTEXTUREHANDLE hDestTexture, D3DTEXTUREHANDLE hSrcTexture )
	{
		INSTRUCTION( D3DOP_TEXTURELOAD, sizeof(D3DTEXTURELOAD), 1 );
		TopTEXTURELOAD->hDestTexture	= hDestTexture;
		TopTEXTURELOAD->hSrcTexture		= hSrcTexture;
		TopTEXTURELOAD++;
	}

	// Exit.
	void EXIT()
	{
		INSTRUCTION( D3DOP_EXIT, 0, 0 );
	}

	// Branch forward.
	void BRANCHFORWARD( DWORD dwMask, DWORD dwValue, BOOL bNegate, DWORD dwOffset )
	{
		INSTRUCTION( D3DOP_BRANCHFORWARD, sizeof(D3DBRANCH), 1 );
		TopBRANCH->dwMask		= dwMask;
		TopBRANCH->dwValue		= dwValue;
		TopBRANCH->bNegate		= bNegate;
		TopBRANCH->dwOffset		= dwOffset;
		TopBRANCH++;
	}

	// Span.
	void SPAN( WORD wCount, WORD wFirst )
	{
		INSTRUCTION( D3DOP_SPAN, sizeof(D3DSPAN), 1 );
		TopSPAN->wCount         = wCount;
		TopSPAN->wFirst         = wFirst;
		TopSPAN++;
	}

	// Status.
	void SETSTATUS( DWORD dwFlags, DWORD dwStatus, D3DRECT drExtent )
	{
		INSTRUCTION( D3DOP_SETSTATUS, sizeof(D3DSTATUS), 1 );
		TopSTATUS->dwFlags		= dwFlags;
		TopSTATUS->dwStatus		= dwStatus;
		TopSTATUS->drExtent		= drExtent;
		TopSTATUS++;
	}
};

/*-----------------------------------------------------------------------------
	FDirect3DRenderDevice definition.
-----------------------------------------------------------------------------*/

//
// The Direct3D rendering device.
//
class FDirect3DRenderDevice : public FRenderDevice
{
public:
	// DirectX interfaces.
	IDirect3D					*d3d;
	IDirect3DDevice				*d3dDevice;
	IDirect3DViewport			*d3dViewport;
	IDirect3DMaterial			*d3dBackground;

	// Execute buffer.
	CExecuteBuffer				ExecuteBuffer;

	// Variables.
	D3DMATERIALHANDLE			hBackground;
	D3DDEVICEDESC				DeviceDesc;
	D3DVIEWPORT					Viewport;
	GUID						DeviceGUID;
	INT							DevicesFound;

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
	void DrawPolyF(UCamera *Camera,const FTransform *Pts,int NumPts,FColor Color);
	int Exec(const char *Cmd,FOutputDevice *Out);
};

/*-----------------------------------------------------------------------------
	Direct3D device enumeration callback.
-----------------------------------------------------------------------------*/

//
// Callback for enumerating Direct3D devices.
//
HRESULT WINAPI EnumDevicesCallback
(
	LPGUID			lpGuid,
	LPSTR			lpDeviceDescription,
    LPSTR			lpDeviceName,
	LPD3DDEVICEDESC lpD3DHWDeviceDesc,
    LPD3DDEVICEDESC lpD3DHELDeviceDesc,
	LPVOID			lpUserArg
)
{
	guard(EnumDevicesCallback);
	FDirect3DRenderDevice *RenDev = (FDirect3DRenderDevice*)lpUserArg;
	debugf("   Direct3D device: [%s] [%s]",lpDeviceName,lpDeviceDescription);

	// Make sure this device is useful.
	if( 0 )
		debugf("      Error");
	//else if( lpD3DHWDeviceDesc->dcmColorModel != D3DCOLOR_RGB )
	//	debugf("      Not hardware or not RGB (%i)",lpD3DHWDeviceDesc->dcmColorModel);
	else if( !(lpD3DHWDeviceDesc->dwDevCaps & D3DDEVCAPS_FLOATTLVERTEX ) )
		debugf("      Rejects floating vertices");
	//else if( !(lpD3DHWDeviceDesc->dwDevCaps & D3DDEVCAPS_EXECUTESYSTEMMEMORY ) )
	//	debugf("      No execute in system memory");
	else if( lpD3DHWDeviceDesc->dwDevCaps & (D3DDEVCAPS_SORTDECREASINGZ|D3DDEVCAPS_SORTEXACT|D3DDEVCAPS_SORTINCREASINGZ) )
		debugf("      Nutty sorting requirements");
	else if( !(lpD3DHWDeviceDesc->dwDevCaps & D3DDEVCAPS_TEXTUREVIDEOMEMORY ) )
		debugf("      No video memory textures");
	//else if( !(lpD3DHWDeviceDesc->dwDevCaps & D3DDEVCAPS_TLVERTEXSYSTEMMEMORY ) )
	//	debugf("      No tlvertices in system memory");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwMiscCaps & D3DPRASTERCAPS_ZTEST ) )
		debugf("      No z buffering");
	//else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwMiscCaps & D3DPMISCCAPS_MASKZ ) )
	//	debugf("      No z changes");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_ALWAYS ) )
		debugf("      No z always");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_LESSEQUAL ) )
		debugf("      No z lessequal");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_BOTHSRCALPHA ) )
		debugf("      No source alpha blending");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_SRCCOLOR ) )
		debugf("      No modulation blending");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND ) )
		debugf("      No alpha gouraud blend");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORFLATRGB ) )
		debugf("      No color flat shading");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB ) )
		debugf("      No color gouraud shading");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE ) )
		debugf("      No perspective correction");
	else if( lpD3DHWDeviceDesc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
		debugf("      Square textures only");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHA) )
		debugf("      No alpha textures");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEAR) )
		debugf("      No bilinear filtering");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwTextureBlendCaps & D3DTBLEND_DECAL) )
		debugf("      No decal texturing");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwTextureBlendCaps & D3DPTBLENDCAPS_DECALALPHA ) )
		debugf("      No decal alpha texturing");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwTextureBlendCaps & D3DPTBLENDCAPS_MODULATE ) )
		debugf("      No modulation texturing");
	else if( !(lpD3DHWDeviceDesc->dpcTriCaps.dwTextureBlendCaps & D3DPTBLENDCAPS_MODULATEALPHA ) )
		debugf("      No modulation alpha texturing");
	else if( lpD3DHWDeviceDesc->dwDeviceRenderBitDepth != DDBD_16 )
		debugf("      Not 16-bit color");
	else 
	{
		// This Direct3D driver doesn't suck.
		RenDev->DeviceGUID = *lpGuid;
		RenDev->DeviceDesc = *lpD3DHWDeviceDesc;
		RenDev->DevicesFound++;
	}
	return D3DENUMRET_OK;
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
	HRESULT hRes;
	checkState(!Active);
	debugf("Initializing Direct3D...");

	// Get Direct3D interface.
	d3d  = NULL;
	hRes = CameraManager.dd->QueryInterface( IID_IDirect3D, (void**)&d3d );
	if( hRes )
	{
		debugf("Can't get IDirect3D: %s",CameraManager.ddError(hRes));
		return 0;
	}

	// Find an appropriate device.
	DevicesFound=0;
	hRes = d3d->EnumDevices(EnumDevicesCallback,this);
	if( !DevicesFound )
	{
		debugf("Found no Direct3D devices");
		d3d->Release();
		return 0;
	}

	// Set fullscreen mode.
	if( !CameraManager.ddSetCamera
	(
		Camera,
		RequestX,
		RequestY,
		2,
		CC_Hardware3D
	) )
	{	
		debugf("Setting DirectDraw mode failed");
		d3d->Release();
		return 0;
	}
	checkState(CameraManager.dd!=NULL);

	// Get Direct3D interface.
	d3dDevice = NULL;
	hRes = CameraManager.ddBackBuffer->QueryInterface( DeviceGUID, (void**)&d3dDevice );
	if( hRes )
	{
		debugf("Can't get IDirect3DDevice: %s",CameraManager.ddError(hRes));
		d3d->Release();
		CameraManager.EndFullscreen();
		return 0;
	}

	// Create the background material.
    CRITICALCALL(d3d->CreateMaterial( &d3dBackground, NULL ),"CreateMaterial");
	D3DMATERIAL Background;
    ZeroMemory(&Background, sizeof(Background));
    Background.dwSize        = sizeof(Background);
    Background.dcvDiffuse.r  = D3DVAL(0.0);
    Background.dcvDiffuse.g  = D3DVAL(0.0);
    Background.dcvDiffuse.b  = D3DVAL(0.3);
    Background.dcvAmbient.r  = D3DVAL(0.0);
    Background.dcvAmbient.g  = D3DVAL(0.0);
    Background.dcvAmbient.b  = D3DVAL(0.0);
    Background.dcvSpecular.r = D3DVAL(0.0);
    Background.dcvSpecular.g = D3DVAL(0.0);
    Background.dcvSpecular.b = D3DVAL(0.0);
    Background.dvPower       = D3DVAL(0.0);
    Background.dwRampSize    = 1;
    CRITICALCALL(d3dBackground->SetMaterial(&Background),"Background.SetMaterial");
    CRITICALCALL(d3dBackground->GetHandle( (IDirect3DDevice *)d3dDevice, &hBackground),"Background.GetHandle");

	// Create a viewport.
	CRITICALCALL(d3d->CreateViewport( &d3dViewport, NULL ),"d3d.CreateViewport");

	// Add viewport to device.
	CRITICALCALL(d3dDevice->AddViewport(d3dViewport),"d3dDevice.AddViewport");

	// Attach the background material to the viewport.
    CRITICALCALL(d3dViewport->SetBackground(hBackground),"d3dViewport.SetBackground");

	// Set viewport z-buffer.
	//d3dviewport->SetBackgroundDepth(zbuffer_surf);

	// Set viewport.
	memset(&Viewport,0,sizeof(Viewport));
	Viewport.dwSize		= sizeof(Viewport);
    Viewport.dwX		= 0;
    Viewport.dwY		= 0;
    Viewport.dwWidth	= Camera->X;
    Viewport.dwHeight	= Camera->Y; 
    Viewport.dvScaleX	= Camera->FX2;
    Viewport.dvScaleY	= Camera->FY2;
    Viewport.dvMaxX		= 1.0;
    Viewport.dvMaxY		= 1.0;
    //Viewport.dvMinZ		= 1.0;
    //Viewport.dvMaxZ		= 16383.0;

	CRITICALCALL(d3dViewport->SetViewport(&Viewport),"Viewport.SetViewport");

	// Init execute buffer.
	ExecuteBuffer.Init( d3dDevice, d3dViewport );
	ExecuteBuffer.STATERENDER_I( D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID );
	ExecuteBuffer.STATERENDER_I( D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD );
	//ExecuteBuffer.STATERENDER_I( D3DRENDERSTATE_DITHERENABLE, TRUE );
	ExecuteBuffer.PROCESSVERTICES( D3DPROCESSVERTICES_COPY, 0, 0, 3, 0 );
	//ExecuteBuffer.TRIANGLE( 0, 1, 2, D3DTRIFLAG_EDGEENABLETRIANGLE );
	ExecuteBuffer.LINE( 0, 1 );
	ExecuteBuffer.EXIT();
	ExecuteBuffer.Make( 3, 32, sizeof( D3DTLVERTEX ));


	// Init general info.
	Active      = 1;
	Locked      = 0;

	// Success.
	return 1;
	unguard;
}

//
// Shut down the Direct3D device.
//
void FDirect3DRenderDevice::Exit3D()
{
	guard(FDirect3DRenderDevice::Exit3D);
	checkState(Active);
	debugf("Shutting down Direct3D...");

	// Release objects.
	ExecuteBuffer.Exit();
	d3dViewport->Release();
	d3dDevice->Release();
	d3d->Release();

	// Note we're inactive.
	Active = 0;

	// Shut down Direct3D.
	debugf(LOG_Exit,"Direct3D terminated");

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
	guard(FDirect3DRenderDevice::Lock3D);
	checkState(!Locked);

	//!!SUCKIT!!
	CameraManager.ddBackBuffer->Unlock(Camera->RealScreen);

	// Clear the z-buffer.
	D3DRECT Rect;
	Rect.x1 = 0; Rect.x2 = Camera->X;
	Rect.y1 = 0; Rect.y2 = Camera->Y;
    DEBUGCALL(d3dViewport->Clear(1, &Rect, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),"Clear");

	// Lock Direct3D.
	DEBUGCALL(d3dDevice->BeginScene(),"BeginScene");

	Locked = 1;
	unguard;
};

//
// Unlock the Direct3D rendering device.
//
void FDirect3DRenderDevice::Unlock3D( UCamera *Camera, int Blit )
{
	guard(FDirect3DRenderDevice::Unlock3D);
	checkState(Locked);

	// Unlock Direct3D.
	DEBUGCALL(d3dDevice->EndScene(),"EndScene");

	//!!SUCKIT!!
	DDSURFACEDESC ddSurfaceDesc;
	ZeroMemory(&ddSurfaceDesc,sizeof(ddSurfaceDesc));
  	ddSurfaceDesc.dwSize = sizeof(ddSurfaceDesc);
	CameraManager.ddBackBuffer->Lock( NULL, &ddSurfaceDesc, DDLOCK_WAIT, NULL );

	Locked = 0;
	unguard;
};

/*-----------------------------------------------------------------------------
	FDirect3DRenderDevice texture vector polygon drawer.
-----------------------------------------------------------------------------*/

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

#if USE_DRAWPRIMITIVE
	D3DTLVERTEX Verts[32];
#else
	D3DTLVERTEX *Verts;
	CRITICALCALL(ExecuteBuffer.Lock(Verts),"ExecuteBuffer.Lock");
#endif

	// Translate the points to D3DTLVERTEX's.
	for( int i=0; i<NumPts; i++ )
	{
		const FTransform  &Pt   = Pts  [i];
		D3DTLVERTEX       &Vert = Verts[i];

		// Set screen coords.
		Vert.sx			= Pt.ScreenX/65536.0;
		Vert.sy			= Pt.ScreenY;
		Vert.sz			= Pt.Z;
		Vert.rhw		= 1.0 / Pt.Z;
		Vert.color		= RGBA_MAKE(rand()%255, rand()%255, rand()%255, 255);
		Vert.specular	= RGBA_MAKE(rand()%255, rand()%255, rand()%255, 255);
		Vert.tu			= 0;
		Vert.tv			= 0;
	}

	// Draw it.
#if USE_DRAWPRIMITIVE
	// Draw with DrawPrimitive.
	CRITICALCALL(d3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DVT_TLVERTEX, Verts, 3), "DrawPrimitive");
#else
	CRITICALCALL( ExecuteBuffer.Unlock(), "ExecuteBuffer.Unlock" );
	CRITICALCALL( ExecuteBuffer.Execute( D3DEXECUTE_UNCLIPPED ), "Execute" );
#endif

	unguard;
}

/*-----------------------------------------------------------------------------
	FDirect3DRenderDevice texture coordinates polygon drawer.
-----------------------------------------------------------------------------*/

//
// Draw a polygon with texture coordinates.
//
void FDirect3DRenderDevice::DrawPolyC
(
	UCamera				*Camera,
	UTexture			*Texture,
	const FTransTexture	*Pts,
	int					NumPts,
	DWORD				PolyFlags
)
{
	guard(FDirect3DRenderDevice::DrawPolyC);
	checkState(Locked);

	// Not yet implemented.
	unguard;
}

/*-----------------------------------------------------------------------------
	FDirect3DRenderDevice flat shaded polygon drawer.
-----------------------------------------------------------------------------*/

//
// Draw a flat-shaded poly.
//
void FDirect3DRenderDevice::DrawPolyF
(
	UCamera				*Camera,
	const FTransform	*Pts,
	int					NumPts,
	FColor				Color
)
{
	guard(FDirect3DRenderDevice::DrawPolyF);
	checkState(Locked);

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

	// Found it?
	if( FoundDirect3D )
	{
		debugf(LOG_Init,"Found Direct3D");
		return &GDirect3DRenDev;
	}
	else
	{
		debugf(LOG_Init,"No Direct3D detected");
		return NULL;
	}
}
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
