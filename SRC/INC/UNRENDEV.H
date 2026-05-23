/*=============================================================================
	UnRenDev.h: 3D rendering device class

	Copyright 1995 Epic MegaGames, Inc.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNRENDEV // Prevent header from being included multiple times
#define _INC_UNRENDEV

/*------------------------------------------------------------------------------------
	FRenderDevice
------------------------------------------------------------------------------------*/

//
// Class representing a low-level 3D rendering device, such as a 3D hardware
// accelerator.
//
class FRenderDevice
{
	public:

	// Init, Exit & Flush.
	int Active,Locked;
	FRenderDevice();
	~FRenderDevice();
	virtual int Init3D( UCamera *Camera, int RequestX, int RequestY)=0;
	virtual void Exit3D()=0;
	virtual void Flush3D()=0;
	virtual int Exec(const char *Cmd,FOutputDevice *Out)=0;

	// Lock & Unlock.
	virtual void Lock3D(UCamera *Camera)=0;
	virtual void Unlock3D(UCamera *Camera,int Blit)=0;

	// Draw a polygon using texture vectors.
	virtual void DrawPolyV( UCamera *Camera,UTexture *Texture,const class FTransform *Pts,int NumPts,
		const FVector &Base, const FVector &Normal, const FVector &U, const FVector &V, FLOAT PanU, FLOAT PanV,
		DWORD PolyFlags)=0;

	// Draw a polygon using texture coordinates.
	virtual void DrawPolyC(UCamera *Camera,UTexture *Texture,const class FTransTexture *Pts,int NumPts,DWORD PolyFlags)=0;

	// Draw a polygon flat-shaded.
	virtual void DrawPolyF(UCamera *Camera,const class FTransform *Pts,int NumPts,FColor Color)=0;
};

/*------------------------------------------------------------------------------------
	The End
------------------------------------------------------------------------------------*/
#endif // _INC_UNRENDEV
