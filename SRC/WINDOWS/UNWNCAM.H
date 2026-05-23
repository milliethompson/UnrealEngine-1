/*=============================================================================
	UnWnCam.h: Unreal Windows-platform camera manager
	Used by: Windows code

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNWNCAM
#define _INC_UNWNCAM

#include "ddraw.h"

/////////////////////////////////////////////////////////////////////////////
// FWindowsCameraManager.
/////////////////////////////////////////////////////////////////////////////

enum EWindowsBlitType
{
	BLIT_NONE			= 0,
	BLIT_DIBSECTION		= 1,
	BLIT_DIRECTDRAW		= 2,
	BLIT_DEFAULT		= BLIT_DIBSECTION,
};

class FWindowsCameraManager : public FCameraManagerBase
{
public:
	//////////////////////////////////
	// FCameraManagerBase Interface //
	//////////////////////////////////

	// Constants.
	enum {MAX_CAMERA_STORED_KEYS=64};
	enum {MAX_DD=4};

	// Init/Exit.
	FWindowsCameraManager();
	void Init();
	void Exit();

	// Platform-specific camera manager functions.
	void	SetPalette			(UPalette *Palette);
	void	SetModeCursor		(UCamera *);
	void	UpdateCameraWindow	(UCamera *);
	void	InitCameraWindow	(UCamera *Camera);
	void	OpenCameraWindow	(UCamera *Camera,DWORD ParentWindow,int Temporary);
	void	CloseCameraWindow	(UCamera *Camera);
	void	ShowCameraWindows	(DWORD ShowFlags,int DoShow);
	void	EnableCameraWindows	(DWORD ShowFlags,int DoEnable);
	void	EndFullscreen		();
	void	Poll				();
	int 	LockCameraWindow	(UCamera *Camera);
	void	UnlockCameraWindow	(UCamera *Camera,int Blit);
	void	ShutdownAfterError	();
	UCamera *CurrentCamera		();
	void    UpdateCameraInput   (UCamera *Camera);
	void	MakeCurrent			(UCamera *Camera);
	void	MakeFullscreen		(UCamera *Camera);
	int		Exec				(const char *Cmd,FOutputDevice *Out=GApp);
	void	Tick                ();

	/////////////////////////////////////
	// Windows specific implementation //
	/////////////////////////////////////

	// Regular Windows support.
	void		InitFullscreen();
	void		AllocateCameraDIB(UCamera *Camera, int BlitType);
	void		FreeCameraStuff(UCamera *Camera);
	BITMAPINFO	*AllocateDIB(int XR, int YR, int AllocateData, int Direction, int ColorBytes, DWORD *Size);
	void		SetCameraBufferSize (UCamera *Camera, int NewWidth, int NewHeight, int NewColorBytes);
	void		SetCameraClientSize(UCamera *Camera, int NewWidth, int NewHeight, int UpdateProfile);
	void		ResizeCameraFrameBuffer(UCamera *Camera,int NewSXR, int NewSYR, int NewColorBytes, int BlitType, int Redraw);
	void		SetOnTop(HWND hWnd);
	void		SetSize(HWND hWnd, int NewWidth, int NewHeight, int HasMenu);
	void		StopClippingCursor(UCamera *Camera, int RestorePos);
	LRESULT		CameraWndProc (HWND hWnd, UINT iMessage, WPARAM wParam,LPARAM lParam);
	inline int	PalFlags() {return (GDefaults.LaunchEditor ? 0 : PC_NOCOLLAPSE);};
	int			CursorIsCaptured();
	void		SaveFullscreenWindowRect(UCamera *Camera);
	int			Toggle(HMENU hMenu,int Item);
	void		FindAvailableModes(UCamera *Camera);
	void		SetColorDepth(UCamera *Camera,int NewColorBytes);
	int			IsHardware3D(UCamera *Camera);

	LOGPALETTE	*LogicalPalette;	// Game's logical palette data.
	HPALETTE	hLogicalPalette;	// Logical palette handle.
	WNDCLASS 	CameraWndClass;		// Class for camera windows.
	HDC			hMemScreenDC;		// A memory DC compatible with the screen's DC.
	HWND		FullscreenhWndDD;	// Window that has DirectDraw control, or NULL=none.
	HANDLE		DMouseHandle;		// Handle for DirectMouse, if it's active.
	int			UseDirectMouse;		// Whether DirectMouse usage is allowed.
	int			UseDirectDraw;		// Whether DirectDraw usage is allowed.
	int 		InMenuLoop;			// Whether we're in a modal menu loop state.
	INT			NormalMouseInfo[3];	// Mouse info from SystemParametersInfo.
	INT			CaptureMouseInfo[3];// Mouse info when captured.
	GUID		ddGUIDs[MAX_DD];	// Avaiable DirectDraw GUIDs.
	int			NumDD;				// Number of valid ddGUIDs.

	// DirectDraw support.
	enum {DD_POLL_TIME=16}; // Milliseconds between mandatory lock/unlock ~ 60 polls per second.
	enum {DD_MAX_MODES=10};
	typedef HRESULT (WINAPI *DD_CREATE_FUNC)(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);
	typedef HRESULT (WINAPI *DD_ENUM_FUNC  )(LPDDENUMCALLBACK lpCallback,LPVOID lpContext);

	IDirectDraw2		*dd;
	IDirectDrawSurface  *ddFrontBuffer;
	IDirectDrawSurface  *ddBackBuffer;
	IDirectDrawPalette  *ddPalette;
	DDSURFACEDESC 		ddSurfaceDesc;
	DD_CREATE_FUNC		ddCreateFunc;
	DD_ENUM_FUNC		ddEnumFunc;
	int					ddModeWidth[DD_MAX_MODES],ddModeHeight[DD_MAX_MODES];
	int					ddNumModes;

	int			ddInit();
	void		ddExit();
	char		*ddError(HRESULT Result);
	int			ddSetMode(HWND hWndOwner, int Width, int Height, int ColorBytes,int &Caps);
	void		ddEndMode();
	int			ddSetCamera(UCamera *Camera,int Width, int Height, int ColorBytes, int RequestedCaps);

	// DirectMouse support.
	QWORD		StoredMouseTime;
	void		dmStart();
	void		dmEnd();
};

/////////////////////////////////////////////////////////////////////////////
// The End.
/////////////////////////////////////////////////////////////////////////////
#endif // _INC_UNWNCAM

