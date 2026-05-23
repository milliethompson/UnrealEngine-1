/*=============================================================================
	UnPlatfm.h: All generic hooks for platform-specific routines

	This structure is shared between the generic Unreal code base and
	the platform-specific routines.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNPLATFM
#define _INC_UNPLATFM

/*------------------------------------------------------------------------------
	Globals.
------------------------------------------------------------------------------*/

UNENGINE_API extern class FGlobalPlatform *GApp;

/*------------------------------------------------------------------------------
	FOutputDevice.
------------------------------------------------------------------------------*/

// Logging event constants for FOutputDevice.
enum ELogType
{
	LOG_None		= 0,  // Nothing.
	LOG_Info        = 1,  // General info & non-error response to 'Exec'.
	LOG_Critical    = 2,  // Critical engine errors.
	LOG_Actor       = 3,  // Actor debug print.
	LOG_Debug       = 4,  // Internal engine debug messages.
	LOG_Init        = 5,  // Subsystem initializing.
	LOG_Exit        = 6,  // Subsystem exiting.
	LOG_Cmd         = 7,  // Command line echoed.
	LOG_Play        = 8,  // General gameplay messages.
	LOG_Chat        = 9,  // A chat message during gameplay.
	LOG_Whisper     = 10, // A whisper message during gameplay.
	LOG_Session     = 11, // A session-related message from the server.
	LOG_Client      = 12, // A gameplay message from the client.
	LOG_ComeGo      = 13, // A gameplay message regarding players coming and going.
	LOG_Console		= 14, // Console commands.
	LOG_ExecError	= 15, // Error response to Exec.
	LOG_Problem		= 16, // Non-critical problem report.
	LOG_ScriptWarn	= 17, // A script debugging warning.
	LOG_ScriptLog	= 18, // A script logging message.
	LOG_MAX			= 19, // Unused tag representing maximum log value.
};

// An output device.  Player consoles, the debug log, and the script
// compiler result text accumulator are output devices.
class UNENGINE_API FOutputDevice
{
public:
	virtual void Write(const void *Data, int Length, ELogType MsgType=LOG_None)=0;
	virtual int  Log(ELogType MsgType, const char *Text);
	virtual int  Log(const char *Text);
	virtual int  VARARGS Logf(ELogType MsgType, const char *Fmt,...);
	virtual int  VARARGS Logf(const char *Fmt,...);
};

/*----------------------------------------------------------------------------
	File mappings.
----------------------------------------------------------------------------*/

class FFileMapping
{
friend class FGlobalPlatform;
private:
	char   Filename[256];
	HANDLE hFile;
	HANDLE hFileMapping;
	void   *Base;
};

/*----------------------------------------------------------------------------
	Platform features.
----------------------------------------------------------------------------*/

enum EPlatformFeatureFlags
{
	PLAT_MMX		= 1,	// Supports MMX instrutions.
	PLAT_PentiumPro	= 2,	// PentiumPro or better processor.
};

/*----------------------------------------------------------------------------
	FGlobalPlatform.
----------------------------------------------------------------------------*/

// Platform-specific code's communication structure.
class FGlobalPlatform : public FOutputDevice
{
public:

	enum {PLATFORM_VERSION=5};

	///////////////
	// Variables //
	///////////////

	// Platform info.
	DWORD   PlatformVersion;	// Version of the platform specific code.
	DWORD	Features;			// Platform feature bits.

	// Windows.
	DWORD	hWndLog;
	DWORD	hWndParent;
	DWORD	hWndSlowTask;

	// Flags.
	BOOL	Debugging;			// =1 if run in debugger with debug version.
	BOOL	InAppError;			// =1 if program is caught in error handler.
	BOOL	InSlowTask;			// >0 if in a slow task.
	BOOL	ServerLaunched;		// =1 if server has been launched.
	BOOL	ServerAlive;		// =1 if server is up and running.
	BOOL	LogAlive;			// =1 if logging window is up and running.
	BOOL	GuardTrap;			// =1 if error was trapped in guarded code with try/except.
	BOOL	LaunchWithoutLog;	// Launch with log hidden.

	// Logging info.
	BYTE	LogEventEnabled[LOG_MAX];
	CHAR	**LogEventNames;
	FILE	*LogFile;

	// Strings.
	char	StartingPath	[256];	// Starting path.
	char	DataPath		[256];	// Data path, i.e. for CD-Rom play.
	char	CmdLine			[512];	// Command line.
	char	LogFname		[256];	// Name of log file.
	char	ErrorHist		[2048];	// Error history.

	//////////////////////////////////
	// Platform-specific subsystems //
	//////////////////////////////////

	class FCameraManagerBase	*CameraManager;
	// Platform-dependent global audio class goes here
	// Other global platform-specific subsystem classes go here...

	///////////////
	// Functions //
	///////////////

	// Logging.
	virtual void	Write(const void *Data, int Length, ELogType MsgType=LOG_None);
	virtual void	OpenLog(const char *Fname);
	virtual void	CloseLog();

	// Memory allocation.
	virtual void	*VARARGS Malloc(int Size,const char *Fmt,...);
	virtual void	*VARARGS Realloc(void *Ptr,int NewSize,const char *Fmt,...);
	virtual void	Free(void *Ptr);

	// Application init & exit & critical event.
	virtual void	Init(char *CmdLine,char *BaseDir);
	virtual void	Startup();
	virtual void	CheckAllocations();
	virtual void	Exit();
	virtual void	Error(const char *Msg);
	virtual void	VARARGS Errorf(const char *Fmt,...);
	virtual int		VARARGS DebugBoxf(const char *Fmt,...);
	virtual void    CheckMachineState();
	virtual void	ShutdownAfterError();
	virtual void	DebugBreak();
	virtual void	VARARGS GuardMessagef(const char *Fmt,...);

	// Slow task management.
	virtual void		BeginSlowTask(const char *Task,int StatusWindow, int Cancelable);
	virtual void		EndSlowTask  ();
	virtual int			StatusUpdate (const char *Str, int Numerator, int Denominator);
	virtual int VARARGS StatusUpdatef(const char *Fmt, int Numerator, int Denominator, ...);

	// Misc.
	virtual void	Poll();
	virtual QWORD	MicrosecondTime();
	virtual FLOAT	CpuToMilliseconds(INT CpuCycles);
	virtual void	SystemTime(INT *Year,INT *Month,INT *DayOfWeek,INT *Day,INT *Hour,INT *Min,INT *Sec,INT *MSec);
	virtual BYTE*	CreateFileMapping(FFileMapping &File, const char *Name, int MaxSize);
	virtual void    CloseFileMapping(FFileMapping &File,INT Trunc);
	virtual void*	GetProcAddress(const char *ModuleName,const char *ProcName,int Checked);
	virtual void	SetParent(DWORD hWndParent);
	virtual void	RequestExit();
	virtual void	EdCallback(WORD Code,WORD Param);
	virtual void	Show();
	virtual void	Minimize();
	virtual void	Hide();
	virtual int		MessageBox(const char *Text,const char *Title="Message",int YesNo=0);
	virtual void	Enable();
	virtual void	Disable();
	virtual void    LaunchURL(char *URL, char *Extra);
	virtual int     PasswordDialog(const char *Title,const char *Prompt,char *Name,char *Password);
	virtual void	GetMemoryInfo(int *MemoryInUse,int *MemoryAvailable);
	virtual int		Exec(const char *Cmd,FOutputDevice *Out=GApp);
	virtual int		EnableFastMath(int Enable);
	virtual int		IsNan(FLOAT f);
	virtual int     FindFile(const char *In,char *Out);

    // Reading/writing profile (.ini) values.
    virtual const char * DefaultProfileFileName() const; // What is the default profile file name?
    virtual const char * FactoryProfileFileName() const; // What is the factory-settings profile file name?
    virtual BOOL GetProfileBoolean // Get the boolean value for Key in Section.
    (
        const char * Section      // The name of the section.
    ,   const char * Key          // The name of the key.
    ,   BOOL         DefaultValue // The default value if a valid boolean profile value is not found.
    ,   const char * FileName = 0 // The name of the profile file. 0 to use the default.
    );
    virtual int GetProfileInteger // Get the integer value for Key in Section.
    (
        const char * Section      // The name of the section.
    ,   const char * Key          // The name of the key.
    ,   int          Default      // The default value if the key is not found.
    ,   const char * FileName = 0 // The name of the profile file. 0 to use the default.
    );
    virtual void GetProfileSection // Get all the values in a section.
    (
        const char * Section      // The name of the section.
    ,   char       * Values       // Where to put Key=Value strings.
    ,   int          Size         // The size of Values.
    ,   const char * FileName = 0 // The name of the profile file. 0 to use the default.
        // All the values in the section are put into *Values in the form
        // of "Key=Value". They are separated by null characters, and the
        // last value is followed by 2 null characters.
    );
    virtual BOOL  GetProfileValue // Get the value associated with a key.
    (
        const char * Section      // The name of the section
    ,   const char * Key          // The name of the key.
    ,   const char * Default      // The default value if the key is not found.
    ,         char * Value        // The output value.
    ,   int          Size         // The size of *Value.
    ,   const char * FileName = 0 // The name of the profile file. 0 to use the default.
        // Notes:
        //   1. If there are no errors, and the key is found (or a default is
        //      specified), TRUE is returned. Otherwise FALSE is returned.
    );	
    virtual void PutProfileSection // Remove all values in the section and write out new values.
    (
        const char * Section      // The name of the section
    ,   const char * Values       // A list of Key=Value strings.
    ,   const char * FileName = 0 // The name of the profile file. 0 to use the default.
        // The Key=Value pairs are separated by nulls, and the last pair is followed by 2 nulls.
    );
    virtual void PutProfileBoolean // Put a boolean value for Key into Section.
    (
        const char * Section      // The name of the section
    ,   const char * Key          // The name of the key.
    ,   BOOL         Value        // The value to put.
    ,   const char * FileName = 0 // The name of the profile file. 0 to use the default.
    );
    virtual void PutProfileInteger // Put an integer value for Key into Section.
    (
        const char * Section      // The name of the section
    ,   const char * Key          // The name of the key.
    ,   int          Value        // The value to put.
    ,   const char * FileName = 0 // The name of the profile file. 0 to use the default.
    );
    virtual void PutProfileValue // Change the value associated with a key in a section.
    (
        const char * Section      // The name of the section
    ,   const char * Key          // The name of the key.
    ,   const char * Value        // The value to use. 0 causes the value to be deleted from the profile.
    ,   const char * FileName = 0 // The name of the profile file. 0 to use the default.
    );	
};

// Editor callback codes.
enum EUnrealEdCallbacks
{
	EDC_None			= 0,	// Nothing.
	EDC_CurTexChange	= 10,	// Change in current texture.
	EDC_CurClassChange	= 11,	// Change in current actor class.
	EDC_SelPolyChange	= 20,	// Poly selection set changed.
	EDC_SelActorChange	= 21,	// Selected actor set changed.
	EDC_SelBrushChange	= 22,	// Selected brush set changed.
	EDC_RtClickTexture	= 23,	// Right clicked on a picture.
	EDC_RtClickPoly		= 24,	// Right clicked on a polygon.
	EDC_RtClickActor	= 25,	// Right clicked on an actor.
	EDC_RtClickWindow	= 26,	// Right clicked on camera window.
	EDC_ModeChange		= 40,	// Mode has changed, Param=new mode index.
	EDC_BrushChange		= 41,	// Brush settings changed.
	EDC_MapChange		= 42,	// Change in map, Bsp.
	EDC_ActorChange		= 43,	// Change in actors.
};

// Help messages sent to main window.
enum EMainWindowCmds
{
	IDC_HELP_ABOUT		= 32783,
	IDC_HELP_ORDER		= 32790,
	IDC_HELP_ORDERNOW	= 32791,
	IDC_HELP_TOPICS		= 32782,
	IDC_HELP_WEB		= 32792,
};

/*-----------------------------------------------------------------------------
	Convenience macros.
-----------------------------------------------------------------------------*/

#define debug				GApp->Log
#define debugf				GApp->Logf
#define appError			GApp->Error
#define appErrorf			GApp->Errorf
#define appMalloc			GApp->Malloc
#define appRealloc			GApp->Realloc
#define appFree				GApp->Free
#define appMessageBox		GApp->MessageBox
#define appDebugBoxf		GApp->DebugBoxf
#define appMallocArray(elements,type,descr) (type *)GApp->Malloc((elements)*sizeof(type),descr)
#define appIsNan(f)         GApp->IsNan(f)

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif /* _INC_PLATFORM */
