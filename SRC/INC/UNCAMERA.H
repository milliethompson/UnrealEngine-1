/*=============================================================================
	UnCamera.h: Unreal camera object.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNCAMERA // Prevent header from being included multiple times.
#define _INC_UNCAMERA

/*-----------------------------------------------------------------------------
	FCameraConsole.
-----------------------------------------------------------------------------*/

// Console keyboard input modes.
enum EConsoleKeyState
{
	CK_None			= 0,		// Isn't typing anything.
	CK_Type			= 1,		// Typing a command.
	CK_Menu			= 2,		// Doing stuff in menu; substates are active.
};

// Player console information for a camera:
class FCameraConsoleBase : public FOutputDevice
{
public:

	// Public Functions.
	virtual void	Init 		(UCamera *Camera)=0;
	virtual void	Exit		()=0;
	virtual int		IsTyping	()=0;
	virtual void	PreRender	(UCamera *Camera)=0;
	virtual void	PostRender	(UCamera *Camera,int XLeft)=0;
	virtual void	Write		(const void *Data, int Length, ELogType MsgType = LOG_Info)=0;
	virtual int		Exec		(const char *Cmd,FOutputDevice *Out=GApp)=0;
	virtual void	NoteResize	()=0;
	virtual int		Key			(int Key)=0;
	virtual int		Process		(EInputKey iKey, EInputState State, FLOAT Delta=0.0 )=0;
	virtual void	PostReadInput(PPlayerTick &Move,FLOAT DeltaSeconds,FOutputDevice *Out)=0;
};

/*-----------------------------------------------------------------------------
	UCamera.
-----------------------------------------------------------------------------*/

// Information for rendering the camera view (detail level settings).
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
	CC_ColoredLight		= 4,	// Colored lighting.
	CC_Mask				= 0xff, // Caps mask which affects cached rendering info.
};

// ShowFlags for camera.
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

// Mouse buttons and commands passed to edcamDraw.
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

//
// A camera object, which associates an actor (which defines
// most view parameters) with a Windows window.
//
class UNENGINE_API UCamera : public UObject, public FOutputDevice
{
	DECLARE_CLASS(UCamera,UObject,NAME_Camera,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Transient | CLASS_PreKill};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Screen & data info.
	ULevel::Ptr		Level;   		// Level the camera is viewing.
	UTexture*		Texture;		// Texture ID of screen.
	UObject*		MiscRes;		// Used in in modes like EM_TEXVIEW.
	QWORD			LastUpdateTime;	// Time of last update.

	// Window info.
	INT				X,Y;   			// Buffer X & Y resolutions.
	INT				ColorBytes;		// 1=256-color, 4=24-bit color.
	INT				Caps;			// Capabilities (CC_).
	INT				OnHold;			// 1=on hold, can't be resized.

	// Temporal info.
	DWORD			Current;		// If this is the current input camera.
	DWORD			ClickFlags;		// Click flags used by move and click functions.

	// Things that have an effect when the camera is first opened.
	INT				OpenX;			// Screen X location to open window at.
	INT				OpenY;			// Screen X location to open window at.
	DWORD			ParentWindow;	// hWnd of parent window.

	// Player console.
	class FCameraConsoleBase *Console;

	// Input system.
	class FInputBase *Input;

	// Lock info: Valid only while locked.
	APawn*			Actor;			// Actor containing location, rotation info for camera.
	INT				Stride;			// Stride in pixels.
	INT				ByteStride;		// Stride in bytes = SXStride * ColorBytes.
	BYTE*			RealScreen;		// Pointer to real screen of size SXStride,SYR.
	BYTE*			Screen;			// Pointer to screen, may be offset into Realscreen.
	INT             FXB,FYB;        // Offset of top left active viewport.
	INT				X2,Y2;			// SXR/2, SYR/2.
	INT				FixX;			// Fix(X).
	DWORD			ExtraPolyFlags;	// Additional poly flags associated with camera.
	FCoords			Coords;     	// Transformation coordinates   (World -> Screen).
	FCoords			Uncoords;		// Detransformation coordinates (Screen -> World).
	FLOAT			FX,FY;			// Floating point X,Y.
	FLOAT			FX15,FY15;		// (Floating point SXR + 1.0001)/2.0.
	FLOAT			FX1,FY1;		// Floating point SXR-1.
	FLOAT			FX2,FY2;		// Floating point SXR / 2.0.
	FLOAT			FXM5,FYM5;		// Floating point SXR,SYR - 0.5.
	FLOAT			Zoom;			// Zoom value, based on OrthoZoom and size.
	FLOAT			RZoom;			// 1.0/OrthoZoom.
	FLOAT			ProjZ;      	// Distance to projection plane, screenspace units.
	FLOAT			RProjZ;			// 1.0/ProjZ.
	FLOAT			ProjZRX2;		// ProjZ / FSXR2 for easy view pyramid reject.
	FLOAT			ProjZRY2;		// ProjZ / FSYR2 for easy view pyramid reject.
	FVector			ViewSides[4];	// 4 unit vectors indicating view frustrum extent lines.
	FPlane			ViewPlanes[4];	// 4 planes indicating view frustrum extent planes.

	// Platform-specific info to store with camera (hWnd, etc).
	char PlatformSpecific[256];
	class FWinCamera &Win() {return *(FWinCamera*)PlatformSpecific;}

	// Constructor.
	UCamera(ULevel *Level);

	// UObject interface.
	void InitHeader();
	void PreKill();
	INT Lock(DWORD LockType);
	void Unlock(DWORD OldLockType, int Blit=0);
	void SerializeHeader(FArchive &Ar)
	{
		guard(UCamera::SerializeHeader);
		UObject::SerializeHeader(Ar);
		Ar << Texture;
		unguard;
	}

	// FOutputDevice interface.
	void Write(const void *Data, int Length, ELogType MsgType=LOG_Info);

	// UCamera inlines.
	int		IsGame()		{return Level && Level->GetState()==LEVEL_UpPlay;}
	int		IsEditor()		{return Level && Level->GetState()==LEVEL_UpEdit;}
	int 	IsOrtho()		{return Actor && (Actor->RendMap==REN_OrthXY||Actor->RendMap==REN_OrthXZ||Actor->RendMap==REN_OrthYZ);}
	int		IsRealWire()	{return IsOrtho() || (Actor && Actor->RendMap==REN_Wire);}
	int		IsBrowser()		{return Actor && (Actor->RendMap==REN_TexView||Actor->RendMap==REN_TexBrowser||Actor->RendMap==REN_MeshBrowser);}
	int		IsInvalidBsp()	{return Level && (Level->Model->ModelFlags&MF_InvalidBsp)==MF_InvalidBsp;}
	int		IsRealtime()	{return Actor && (Actor->ShowFlags&(SHOW_RealTime | SHOW_PlayerCtrl));}
	int		WireMode()		{return IsRealWire() ? Actor->RendMap : REN_Wire;}
	int		IsWire();

	// UCamera functions.
	int		Process	(EInputKey iKey, EInputState State, FLOAT Delta=0.0 );
	int		Key		(int Key);
	int		Move  	(BYTE Buttons, SWORD Dx, SWORD Dy, int Shift, int Ctrl);
	int		Click 	(BYTE Buttons, SWORD MouseX, SWORD MouseY, int Shift, int Ctrl);
	void	Draw  	(int Scan);
	void	Hold	();
	void	Unhold	();
	void	OpenWindow(DWORD ParentWindow,int Temporary);
	void	UpdateWindow();
	int		Exec	(const char *Cmd,FOutputDevice *Out=GApp);
	void	ExecMacro(const char *Filename,FOutputDevice *Out=GApp);

	// UCamera functions valid only when rendering.
	void	BuildCoords();
	void	PrecomputeRenderInfo(int SXR, int SYR);
	FVector GetOrthoNormal();

	// Input related functions.
	void ReadInput( PPlayerTick &Move, FLOAT DeltaSeconds, FOutputDevice *Out );
};

/*-----------------------------------------------------------------------------
	FCameraManagerBase.
-----------------------------------------------------------------------------*/

//
// Global camera manager class.  Tracks all active cameras and their
// relationships to levels.
//
class UNENGINE_API FCameraManagerBase
{
public:
	TArray<UCamera*> *CameraArray;		// Array of cameras.
	UCamera			*FullscreenCamera;	// Current fullscreen camera, NULL=not fullscreen.
	class FRenderDevice *RenDev;		// Rendering device.
	int				DrawTime;			// Time consumed by draw/flip.

	// Init/Exit functions.
	FCameraManagerBase() {Initialized=0;};
	virtual void Init()=0;
	virtual void Exit()=0;

	// Platform-specific camera manager functions.
	virtual void	SetPalette			(UPalette *Palette)=0;
	virtual void	SetModeCursor		(UCamera *Camera)=0;
	virtual void	UpdateCameraWindow	(UCamera *Camera)=0;
	virtual void	InitCameraWindow	(UCamera *Camera)=0;
	virtual void	OpenCameraWindow	(UCamera *Camera,DWORD ParentWindow,int Temporary)=0;
	virtual void	CloseCameraWindow	(UCamera *Camera)=0;
	virtual void	ShowCameraWindows	(DWORD ShowFlags,int DoShow)=0;
	virtual void	EnableCameraWindows	(DWORD ShowFlags,int DoEnable)=0;
	virtual void	EndFullscreen		()=0;
	virtual void	Poll				()=0;
	virtual int 	LockCameraWindow	(UCamera *Camera)=0;
	virtual void	UnlockCameraWindow	(UCamera *Camera,int Blit)=0;
	virtual void	ShutdownAfterError	()=0;
	virtual void    UpdateCameraInput   (UCamera *Camera)=0;
	virtual UCamera *CurrentCamera		()=0;
	virtual void	MakeCurrent			(UCamera *Camera)=0;
	virtual void	MakeFullscreen		(UCamera *Camera)=0;
	virtual void	Tick                ()=0;

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
