/*=============================================================================
	UnCamera.h: Unreal camera definitions

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNCAMERA // Prevent header from being included multiple times
#define _INC_UNCAMERA

/*-----------------------------------------------------------------------------
	FCameraConsole.
-----------------------------------------------------------------------------*/

//
// Console keyboard input modes.
//
enum EConsoleKeyState
{
	CK_None			= 0,		// Isn't typing anything.
	CK_Type			= 1,		// Typing a command.
	CK_Menu			= 2,		// Doing stuff in menu; substates are active.
};

//
// Player console information for a camera:
//
class FCameraConsoleBase : public FOutputDevice
{
public:

	// Public Functions.
	virtual void	Init 		(UCamera *Camera)=0;
	virtual void	Exit		()=0;
	virtual int		Key 		(int Key)=0;
	virtual int		IsTyping	()=0;
	virtual void	PreRender	(class ICamera *Camera)=0;
	virtual void	PostRender	(class ICamera *Camera,int XLeft)=0;
	virtual void	Log			(ELogType MsgType, const char *Text)=0;
	virtual int		Exec		(const char *Cmd,FOutputDevice *Out=GApp)=0;
	virtual void	NoteResize	()=0;
    virtual void UpdateActionStatus() = 0;
        // UpdateActionStatus does the following:
        //   - gathers input from polled devices
        //   - sets the state of player actions to reflect the current state of all actions
        //     (the player actions can be retrieved using SetActionStatus())
        //   - does initializations to start a new input cycle
    virtual void SetActionStatus( PPlayerTick & PlayerActions ) = 0;
        // Build a player action packet into PlayerActions based on the current 
        // state of the actions (set by calling UpdateActionStatus). In addition, 
        // the current input movements (mouse, joystick) are cleared so they will not
        // be counted in the next call to SetActionStatus. 
    virtual class FAction * GetActions() = 0;
    virtual class FInput  * GetInput()   = 0;
    virtual void PressSwitch(int Switch) = 0; // Input switch (key, button) was turned on. Switch is actually of type FInput::TSwitch.
    virtual void ReleaseSwitch(int Switch) = 0; // Input switch (key, button) was turned off. Switch is actually of type FInput::TSwitch.
};

/*-----------------------------------------------------------------------------
	UCamera.
-----------------------------------------------------------------------------*/

//
// Information for rendering the camera view (detail level settings):
//
enum ERenderType
{
	REN_None			= 0,	// Hide completely.
	REN_Wire			= 1,	// Wireframe of EdPolys.
	REN_Zones			= 2,	// Show zones and zone portals.
	REN_Polys			= 3,	// Flat-shaded Bsp.
	REN_PolyCuts		= 4,	// Flat-shaded Bsp with normals displayed.
	REN_DynLight		= 5,	// Illuminated texture mapping.
	REN_PlainTex		= 6,	// Plain texture mapping.
	REN_OrthXY			= 13,	// Orthogonal overhead (XY) view.
	REN_OrthXZ			= 14,	// Orthogonal XZ view.
	REN_OrthYZ			= 15,	// Orthogonal YZ view.
	REN_TexView			= 16,	// Viewing a texture (no actor).
	REN_TexBrowser		= 17,	// Viewing a texture browser (no actor).
	REN_MeshView		= 18,	// Viewing a mesh.
	REN_MeshBrowser		= 19,	// Viewing a mesh browser (no actor).
	REN_MAX				= 20
};

enum ECameraCaps
{
	CC_Hardware3D		= 1,	// Hardware 3D rendering.
	CC_RGB565			= 2,	// RGB 565 format (otherwise 555).
};

//
// A camera resource, which associates an actor (which defines
// most view parameters) with a Windows window.
//
class UNREAL_API UCamera : public UResource, public FTask
{
	RESOURCE_CLASS(UCamera,BYTE,RES_Camera)

	enum {MAX_CAMERA_STORED_KEYS=32};

	// Screen & data info.
	DWORD			Locked;			// 1=locked, 0=available.
	DWORD			DataSize; 		// Size of camera data buffer (platform-dependent).
	QWORD			LastUpdateTime;	// Time of last update.
	UTexture		*Texture;		// Texture ID of screen.
	ULevel			*Level;   		// Level the camera is viewing.
	UResource		*MiscRes;		// Resource ID or name ID in modes like EM_TEXVIEW.
	FName			MiscName;		// Used for texture browser.
	INDEX			iActor;			// Index of actor containing location, rotation info for camera.

	// Window info.
	INT				SXR,SYR;   		// Buffer X&Y resolutions.
	INT				ColorBytes;		// 1=256-color, 4=24-bit color.
	INT				Caps;			// Capabilities (CC_).
	INT				OnHold;			// 1=on hold, can't be resized.
	INT				RedMask;		// Red bitmask (if truecolor).
	INT				GreenMask;		// Green bitmask (if truecolor).
	INT				BlueMask;		// Blue bitmask (if truecolor).
	INT				AlphaMask;		// Alpha bitmask (if truecolor).

	// Temporal info.
	DWORD			Current;		// 1 if this is the current input camera.
	DWORD			ClickFlags;		// Click flags used by move and click functions.

	// Things that have an effect when the camera is first opened (pOpenCameraWindow).
	INT				OpenX;			// Screen X location to open window at.
	INT				OpenY;			// Screen X location to open window at.
	DWORD			ParentWindow;	// hWnd of parent window.

	// Player console.
	FCameraConsoleBase *Console;

	// Remembered actor (for ReconcileActors).
	APawn RememberedActor;

	// Platform-specific info to store with camera (hWnd, etc).
	union
	{
		char Generic[256];
#ifdef PLATFORM_CAMERA_CLASS
		PLATFORM_CAMERA_CLASS Win;
#endif
	};

	// Constructors.
	UCamera(ULevel *Level);

	// Resource functions.
	void Register				(FResourceType *Type);
	void InitHeader				();
	void FreeData				();
	void QueryHeaderReferences	(FResourceCallback &Callback);
	void PreKill				();
	void Flip					();

	// Task functions.
	void TaskTick();
	void TaskExit();
	char *TaskStatus(char *Name,char *Desc);

	// Custom functions.
	int		IsGame();
	int		IsEditor();
	inline APawn &GetActor()
	{
		if ( iActor==INDEX_NONE ) 
			appError ("UCamera::GetActor: INDEX_NONE");
		
		APawn *Result = Level->ActorList->Element(iActor).GetPlayer();
		
		if( !Result ) 
			appErrorf("UCamera::GetActor: Camera actor is %s!",Result->Class?Result->Class->Name:"NULL");
		
		return *Result;
	}
	int 	IsOrtho();
	int		IsWire();
	int		IsRealWire();
	int		IsBrowser();
	int		IsInvalidBsp();
	int		IsRealtime();
	int		WireMode();

	int		Lock	(class ICamera *CameraInfo);
	void	Unlock	(class ICamera *CameraInfo,int Blit);
	int		Move  	(BYTE Buttons, SWORD Dx, SWORD Dy, int Shift, int Ctrl);
	int		Click 	(BYTE Buttons, SWORD MouseX, SWORD MouseY, int Shift, int Ctrl);
	void	Draw  	(int Scan);
	int		Key		(int Key);
	void	Hold	();
	void	Unhold	();
	void	OpenWindow(DWORD ParentWindow,int Temporary);
	void	UpdateWindow();
	int		Exec	(const char *Cmd,FOutputDevice *Out=GApp);
};

//
// Camera's working structure.  Locking a camera resource sets up all transformations
// and returns this.
//
class UNREAL_API ICamera
{
public:

	// Members copied directly from UCamera upon lock.
	UCamera			*Camera;		// The camera this corresponds to.
	UTexture		*Texture;		// Texture representing the screen.
	INT				SXR,SYR;   		// Buffer X&Y resolutions.
	INT				Stride;			// Stride in pixels.
	INT				ByteStride;		// Stride in bytes = SXStride * ColorBytes.
	INT				ColorBytes;		// 1=256-color, 4=24-bit color.
	INT				Caps;			// Capabilities flags (CC_).
	INDEX			iActor;			// Actor controlling the camera.
	ILevel			Level;			// Locked level.

	// Custom info.
	BYTE			*Data;			// Pointer to data, not necessarily the screen.
	BYTE			*RealScreen;	// Pointer to real screen of size SXStride,SYR.
	BYTE			*Screen;		// Pointer to screen, may be offset into Realscreen.
	APawn			*Actor;			// Actor representing camera.

	// Precomputed stuff for optimization.
	INT				FovAngle;   	// X field of view angle in degrees (normally 100).
	INT				SXR2,SYR2;		// SXR / 2.
	INT				FixSXR;			// FIX(SXR).

	DWORD			ExtraPolyFlags;	// Additional poly flags associated with camera.
	DWORD			ShowFlags;		// Stuff to show.
	DWORD			RendMap;		// Way to render map.

	FCoords			Coords;     	// Transformation coordinates   (World -> Screen).
	FCoords			Uncoords;		// Detransformation coordinates (Screen -> World).

	FLOAT			FSXR,FSYR;		// Floating point SXR.
	FLOAT			FSXR15,FSYR15;	// (Floating point SXR + 1.0001)/2.0.
	FLOAT			FSXR1,FSYR1;	// Floating point SXR-1.
	FLOAT			FSXR2,FSYR2;	// Floating point SXR / 2.0.
	FLOAT			FSXRM5,FSYRM5;	// Floating point SXR,SYR - 0.5.

	FLOAT			Zoom;			// Zoom value, based on OrthoZoom and size.
	FLOAT			RZoom;			// 1.0/OrthoZoom.
	FLOAT			OrthoZoom;    	// Zoom factor for orthogonal views.

	FLOAT			ProjZ;      	// Distance to projection plane, screenspace units.
	FLOAT			RProjZ;			// 1.0/ProjZ.
	FLOAT			ProjZRSX2;		// ProjZ / FSXR2 for easy view pyramid reject.
	FLOAT			ProjZRSY2;		// ProjZ / FSYR2 for easy view pyramid reject.
	FVector			ViewSides[4];	// 4 normal vectors indicating view frustrum extent lines.
	FVector			ViewPlanes[4];	// 4 planes indicating view frustrum extent planes.

	// Standard functions.
	void	BuildCoords();
	void	PrecomputeRenderInfo(int SXR, int SYR);
	FVector *GetOrthoNormal();
};

//
// ShowFlags for camera
//
enum ECameraShowFlags
{
	SHOW_Frame     		= 0x00000001, 	// Show world bounding cube.
	SHOW_ActorRadii		= 0x00000002, 	// Show actor collision radii.
	SHOW_Backdrop  		= 0x00000004, 	// Show background scene.
	SHOW_Actors    		= 0x00000008,	// Show actors.
	SHOW_Coords    		= 0x00000010,	// Show brush/actor coords.
	SHOW_ActorIcons		= 0x00000020,	// Show actors as icons.
	SHOW_Brush			= 0x00000040,	// Show the active brush.
	SHOW_StandardView	= 0x00000080,	// Camera is a standard view.
	SHOW_Menu			= 0x00000100,	// Show menu on camera.
	SHOW_ChildWindow	= 0x00000200,	// Show as true child window.
	SHOW_MovingBrushes	= 0x00000400,	// Show moving brushes.
	SHOW_PlayerCtrl		= 0x00000800,	// Player controls are on.
	SHOW_NoButtons		= 0x00002000,	// No menu/view buttons.
	SHOW_RealTime		= 0x00004000,	// Update window in realtime.
	SHOW_NoCapture		= 0x00008000,	// No mouse capture.
};

//
// Mouse buttons and commands passed to edcamDraw.
//
enum EMouseButtons	
{
	BUT_LEFT			= 0x01,		// Left mouse button.
	BUT_RIGHT			= 0x02,		// Right mouse button.
	BUT_MIDDLE 			= 0x04,		// Middle mouse button.
	BUT_FIRSTHIT		= 0x08,		// Sent when a mouse button is initially hit.
	BUT_LASTRELEASE		= 0x10,		// Sent when last mouse button is released.
	BUT_SETMODE			= 0x20,		// Called when a new camera mode is first set.
	BUT_EXITMODE		= 0x40,		// Called when the existing mode is changed.
	BUT_LEFTDOUBLE		= 0x80,		// Left mouse button double click.
};

/*-----------------------------------------------------------------------------
	FCameraManager.
-----------------------------------------------------------------------------*/

//
// Global camera manager class.  Tracks all active cameras and their
// relationships to levels.
//
class UNREAL_API FCameraManager : public FTask
{
public:
	TArray<UCamera>	*CameraArray;		// Array of cameras.
	UCamera			*FullscreenCamera;	// Current fullscreen camera, NULL=not fullscreen.
	int				DrawTime;			// Time consumed by draw/flip.

	// Init/Exit functions.
	FCameraManager() {Initialized=0;};
	virtual void Init()=0;
	virtual void Exit()=0;

	// Task functions.
	virtual void TaskExit()=0;
	virtual void TaskTick()=0;
	virtual char *TaskStatus(char *Name,char *Desc)=0;

	// Platform-specific camera manager functions.
	virtual void	SetPalette			(UPalette *Palette, UPalette *GammaPalette)=0;
	virtual void	SetModeCursor		(UCamera *Camera)=0;
	virtual void	UpdateCameraWindow	(UCamera *Camera)=0;
	virtual void	InitCameraWindow	(UCamera *Camera)=0;
	virtual void	OpenCameraWindow	(UCamera *Camera,DWORD ParentWindow,int Temporary)=0;
	virtual void	CloseCameraWindow	(UCamera *Camera)=0;
	virtual void	ShowCameraWindows	(DWORD ShowFlags,int DoShow)=0;
	virtual void	EnableCameraWindows	(DWORD ShowFlags,int DoEnable)=0;
	virtual void	EndFullscreen		()=0;
	virtual void	Poll				()=0;
	virtual int 	LockCameraWindow	(UCamera *Camera,ICamera *CameraInfo)=0;
	virtual void	UnlockCameraWindow	(UCamera *Camera,ICamera *CameraInfo,int Blit)=0;
	virtual void	ShutdownAfterError	()=0;
	virtual void	GetStoredMove		(UCamera *Camera,FVector *Move,FFloatRotation *Rot)=0;
	virtual void	ResetModes			()=0;
	virtual UCamera *CurrentCamera		()=0;
	virtual void	MakeCurrent			(UCamera *Camera)=0;
	virtual void	MakeFullscreen		(UCamera *Camera)=0;

	// Standard functions.
	virtual void	RedrawLevel			(ULevel *Level);
	virtual void	CloseWindowChildren	(DWORD ParentWindow);
	virtual void	UpdateActorUsers	();
	virtual int		Exec				(const char *Cmd,FOutputDevice *Out=GApp)=0;

	protected:
	int Initialized;
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNCAMERA
