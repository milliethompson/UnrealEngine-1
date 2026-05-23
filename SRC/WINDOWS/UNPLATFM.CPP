/*=============================================================================
	UnPlatfm.cpp: All generic, platform-specific routines-specific routines.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "StdAfx.h"
#include <float.h>
#include <new.h>
#include <direct.h>
#include <malloc.h>

#include "UnWn.h"
#include "Unreal.h"
#include "UnWnCam.h"
#include "UnPswd.h"
#include "Net.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

#define TRACK_ALLOCS 1

//
// An entry that tracks one allocated memory block.
//
class FTrackedAllocation
{
public:
	void				*Ptr;
	int					Size;
	char				Name[NAME_SIZE];
	FTrackedAllocation	*Next;
} *GTrackedAllocations=NULL; // Global list of all allocations;


int CDECL UnrealAllocationErrorHandler(size_t);
int SlowLog=0,SlowClosed=0;
QWORD FGlobalPlatform_PerformanceFrequency;
FLOAT FGlobalPlatform_CpuSpeed;
HWND FGlobalPlatform_hWndProgressBar=NULL;
HWND FGlobalPlatform_hWndProgressText=NULL;

void FGlobalPlatform_MemoryStatus(FOutputDevice &Out);

/*-----------------------------------------------------------------------------
	FGlobalPlatform Command line.
-----------------------------------------------------------------------------*/

int FGlobalPlatform::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(FGlobalPlatform::Exec);
	const char *Str = Cmd;

	if( GetCMD(&Str,"LAUNCH") )
	{
		if( GetCMD(&Str,"WEB") )
		{
			Out->Log("Spawning Web browser");
			LaunchURL(URL_WEB,"");
			return 1;
		}
		else return 0;
	}
	if( GetCMD(&Str,"EXIT") )
	{
		Out->Log("Closing by request");
		RequestExit();
		return 1;
	}
	else if (GetCMD(&Str,"MEM") )
	{
		FGlobalPlatform_MemoryStatus(*Out);

		int All = GetCMD(&Str,"ALL");
		int Count=0,Size=0;
		FTrackedAllocation *A = GTrackedAllocations;

		while( A )
		{
			if( All )
				Out->Logf("  %s - %i",A->Name,A->Size);
			Count++;
			Size += A->Size;
			A = A->Next;
		}
		Out->Logf("%i allocations (%.3fM)",Count,(FLOAT)Size/1000000.0);
		return 1;
	}
	else if( GetCMD(&Str,"APP") )
	{
		if( GetCMD(&Str,"SET") )
		{
			DWORD hWndParent;
			if( GetDWORD( Str, "HWND=", &hWndParent ) )
				SetParent((DWORD)hWndParent);
			GetDWORD( Str, "PROGRESSBAR=", (DWORD*)&FGlobalPlatform_hWndProgressBar );
			GetDWORD( Str, "PROGRESSTEXT=", (DWORD*)&FGlobalPlatform_hWndProgressText );
			return 1;
		}
		else if( GetCMD(&Str,"OPEN") )
		{
			DWORD hWndParent;
			if (GetDWORD(Str,"HWND=",&hWndParent)) 
				SetParent((DWORD)hWndParent);
			return 1;
		}
		else if( GetCMD(&Str,"MINIMIZE") )
		{
			Minimize();
			return 1;
		}
		else if( GetCMD(&Str,"HIDE") )
		{
			Hide();
			return 1;
		}
		else if( GetCMD(&Str,"SHOW") )
		{
			Show();
			return 1;
		}
		else if( GetCMD(&Str,"SLOWLOG") )
		{
			SlowLog=1;
			return 1;
		}
		else return 0;
	}
	else return 0; // Not executed.

	unguard;
}

/*-----------------------------------------------------------------------------
	Machine state info.
-----------------------------------------------------------------------------*/

//
// Verify that the machine state is valid.  Calls appError if not.
//
void FGlobalPlatform::CheckMachineState()
{
	guard(FGlobalPlatform::CheckMachineState);

	// Check heap.
	switch( _heapchk() )
	{
		case _HEAPOK: 		break;
		case _HEAPBADBEGIN: Errorf("heapchk: _HEAPBADBEGIN"); break;
		case _HEAPBADNODE: 	Errorf("heapchk: _HEAPBADMODE"); break;
		case _HEAPBADPTR: 	Errorf("heapchk: _HEAPBADPTR"); break;
		case _HEAPEMPTY: 	Errorf("heapchk: _HEAPEMPTY"); break;
		default:			Errorf("heapchk: UNKNOWN"); break;
	}
	unguard;
}

//
// See how much memory is in use and is available.  If MemoryAvailable<0,
// the available memory is unknown.
//
void FGlobalPlatform::GetMemoryInfo(int *MemoryInUse,int *MemoryAvailable)
{
	guard(FGlobalPlatform::GetMemoryInfo);

	*MemoryInUse=0;
	*MemoryAvailable=0;

	unguard;
}

//
// Intel CPUID.
//
void FGlobalPlatform_CPUID( int i, DWORD *A, DWORD *B, DWORD *C, DWORD *D )
{
	try
	{
 		__asm
		{			
			mov eax,[i]
			_emit 0x0f
			_emit 0xa2

			mov edi,[A]
			mov [edi],eax

			mov edi,[B]
			mov [edi],ebx

			mov edi,[C]
			mov [edi],ecx

			mov edi,[D]
			mov [edi],edx

			mov eax,0
			mov ebx,0
			mov ecx,0
			mov edx,0
			mov esi,0
			mov edi,0
		}
	}
	catch(...)
	{
		debugf( "CPUID failed!" );
		*A = *B = *C = *D = 0;
	}
}

/*-----------------------------------------------------------------------------
	Polling & Ticking.
-----------------------------------------------------------------------------*/

//
// Platform-specific polling routine.  This is in place because there are some
// operating system-dependent things that just don't happen properly in the
// background.
//
void FGlobalPlatform::Poll()
{
	guard(FGlobalPlatform::Poll);
	if( GCameraManager )
		GCameraManager->Poll();
	unguard;
}

/*-----------------------------------------------------------------------------
	Password dialog.
-----------------------------------------------------------------------------*/

//
// Put up a dialog and ask the user for his name and password.  Returns 1 if
// entered, 0 if not.  If entered, sets Name to the name typed in and Password
// to the password.
//
int FGlobalPlatform::PasswordDialog( const char *Title, const char *Prompt, char *Name, char *Password )
{
	guard(FGlobalPlatform::PasswordDialog);

	CPasswordDlg PasswordDlg;
	strcpy(PasswordDlg.Title,		Title);
	strcpy(PasswordDlg.Prompt,		Prompt);
	strcpy(PasswordDlg.Name,		Name);
	strcpy(PasswordDlg.Password,	Password);

	if( PasswordDlg.DoModal()==IDOK )
	{
		strcpy(Name,PasswordDlg.Name);
		strcpy(Password,PasswordDlg.Password);
		return 1;
	}
	else return 0;

	unguard;
}

/*-----------------------------------------------------------------------------
	Globals used by the one and only FGlobalPlatform object.
-----------------------------------------------------------------------------*/

//
// Add a new allocation to the list of tracked allocations.
//
void FGlobalPlatform_AddTrackedAllocation( void *Ptr, int Size, char *Name )
{
	guard(FGlobalPlatform_AddTrackedAllocation);

	FTrackedAllocation *A = new FTrackedAllocation;

	A->Ptr		= Ptr;
	A->Size		= Size;
	A->Next		= GTrackedAllocations;

	strncpy(A->Name,Name,NAME_SIZE); A->Name[31]=0;

	GTrackedAllocations = A;

	unguard;
}

//
// Delete an existing allocation from the list.
// Returns the original (pre-alignment) pointer.
//
void *FGlobalPlatform_DeleteTrackedAllocation( void *Ptr )
{
	guard(FGlobalPlatform_DeleteTrackedAllocation);

	FTrackedAllocation **PrevLink = &GTrackedAllocations;
	FTrackedAllocation *A         = GTrackedAllocations;

	while( A )
	{
		if( A->Ptr == Ptr )
		{
			void *Result = A->Ptr;
			*PrevLink = A->Next;
			delete A;
			return Result;
		}
		PrevLink = &A->Next;
		A        = A->Next;
	}
	appError ("Allocation not found");
	return NULL;
	unguard;
}

//
// Display a list of all tracked allocations that haven't been freed.
//
void FGlobalPlatform_DumpTrackedAllocations()
{
	guard(FGlobalPlatform_DumpTrackedAllocations);
	FTrackedAllocation *A = GTrackedAllocations;
	while( A )
	{
		App.Platform.Logf(LOG_Exit,"Unfreed: %s",A->Name);
		A = A->Next;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalPlatform init/exit.
-----------------------------------------------------------------------------*/

//
// Initialize the platform-specific subsystem.
//
// This code is not guarded because it will be called before
// the error trapping mechanism is set up.
//
void FGlobalPlatform::Init( char *ThisCmdLine, char *BaseDir )
{
	try
	{
		strcpy(CmdLine,ThisCmdLine);

		static char *OurLogEventNames[LOG_MAX] =
		{
			"None",		"Info",		"Critical",	"Actor",
			"Debug",	"Init",		"Exit",		"Cmd",
			"Play",		"Chat",		"Whisper",	"Session",
			"Client",	"ComeGo",	"Console",	"ExecError",
			"Problem",	"ScriptWarn","ScriptLog"
		};
		PlatformVersion	= PLATFORM_VERSION;
		hWndLog			= 0;
		hWndParent		= 0;
		Debugging		= 0;
		InAppError		= 0;
		InSlowTask		= 0;
		LaunchWithoutLog= 0;
		ServerLaunched	= 0;
		ServerAlive		= 0;
		LogAlive		= 0;
		GuardTrap		= 0;
		LogFile			= NULL;

		// Features present.
		Features		= 0;

		if (mystrstr(CmdLine,"-DEBUG")) Debugging=1;
		if (mystrstr(CmdLine,"-debug")) Debugging=1;
		if (mystrstr(CmdLine,"-Debug")) Debugging=1;

		strcpy(ErrorHist,"Unreal has encountered a protection fault!  ");
		memset(LogEventEnabled,1,sizeof(LogEventEnabled));
		LogEventNames = OurLogEventNames;

		// Strings.
		strcpy(StartingPath,	BaseDir);
		strcpy(DataPath,		BaseDir);
		strcpy(LogFname,		LOG_PARTIAL);

		// Math.
		EnableFastMath(0);
	}
	catch(...)
	{
		AfxMessageBox("Unreal Windows initialization has failed.");
		ExitProcess(1);
	}
}

//
// Startup routine, to be called after ::Init.  This does all heavy
// initialization of platform-specific information, and is intended
// to be called after logging and guarding are in place.
//
void FGlobalPlatform::Startup()
{
	guard(FGlobalPlatform::Startup);

	// Command line.
	Logf(LOG_Init,"Command line: %s",CmdLine);

	// Check Windows version.
	OSVERSIONINFO Version; Version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&Version);
	if( Version.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		Logf(LOG_Init, "Detected: Microsoft Windows NT %u.%u (Build: %u)",
			Version.dwMajorVersion,Version.dwMinorVersion,Version.dwBuildNumber);
	}
	else if( Version.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		Logf(LOG_Init, "Detected: Microsoft Windows 95 %u.%u (Build: %u)",
			Version.dwMajorVersion,Version.dwMinorVersion,Version.dwBuildNumber);
	}
	else
	{
		Logf(LOG_Init,"Detected: Windows %u.%u (Build: %u)",
			Version.dwMajorVersion,Version.dwMinorVersion,Version.dwBuildNumber);
		appError ("Unreal requires Windows 95 or Windows NT");
	}

	// Init windows floating point control.
	_fpreset();
	char StateDescr[512]="Detected: Coprocessor, ";
	unsigned int FPState = _controlfp (0,0);

	if (FPState&_IC_AFFINE)					strcat(StateDescr," Affine"); 
	else									strcat(StateDescr," Projective");

	if ((FPState&_RC_CHOP)==_RC_CHOP)		strcat(StateDescr," Chop");
	else if ((FPState&_RC_UP)==_RC_UP)		strcat(StateDescr," Up");
	else if ((FPState&_RC_DOWN)==_RC_DOWN)	strcat(StateDescr," Down");
	else strcat(StateDescr," Near");

	if ((FPState&_MCW_PC)==_PC_24)			strcat(StateDescr," 24-bit");
	else if ((FPState&_MCW_PC)==_PC_53)		strcat(StateDescr," 53-bit");
	else									strcat(StateDescr," 64-bit");

	Log(LOG_Init,StateDescr);

	// Performance counter frequency.
	LARGE_INTEGER lFreq;
	QueryPerformanceFrequency(&lFreq);
	FGlobalPlatform_PerformanceFrequency = ((QWORD)lFreq.LowPart) + ((QWORD)lFreq.HighPart<<32); 
	checkState(FGlobalPlatform_PerformanceFrequency!=0);

	INT Cycles=0;
	DWORD Q1 = MicrosecondTime();
	clock(Cycles);
	Sleep(100);
	DWORD Q2 = MicrosecondTime();
	unclock(Cycles);

	FGlobalPlatform_CpuSpeed = 1000000.0 * (FLOAT)Cycles/(FLOAT)(Q2-Q1);
	debugf("CPU SPEED = %f",FGlobalPlatform_CpuSpeed / 1000000.0);

	SYSTEM_INFO SI;
	GetSystemInfo(&SI);
	Logf(LOG_Init,"CPU Page size=%i, Processors=%i",SI.dwPageSize,SI.dwNumberOfProcessors);

	// Check processor version with CPUID.
	DWORD A=0,B=0,C=0,D=0;

	FGlobalPlatform_CPUID(0,&A,&B,&C,&D);
	char Brand[13]="", *Model, FeatStr[256]="";

	Brand[ 0] = B;
	Brand[ 1] = B>>8;
	Brand[ 2] = B>>16;
	Brand[ 3] = B>>24;
	Brand[ 4] = D;
	Brand[ 5] = D>>8;
	Brand[ 6] = D>>16;
	Brand[ 7] = D>>24;
	Brand[ 8] = C;
	Brand[ 9] = C>>8;
	Brand[10] = C>>16;
	Brand[11] = C>>24;
	Brand[12] = 0;
	
	FGlobalPlatform_CPUID( 1, &A, &B, &C, &D );
	switch( (A>>8) & 0x000f )
	{
		case 4:  Model="486-class processor";        break;
		case 5:  Model="Pentium-class processor";    break;
		case 6:  Model="PentiumPro-class processor"; break;
		case 7:  Model="P7-class processor";         break;
		default: Model="Unknown processor";          break;
	}
	if( D & 0x00800000 )
	{
		strcat( FeatStr, "MMX " );
		Features |= PLAT_MMX;
	}
	if( D & 0x00008000 )
	{
		Features |= PLAT_PentiumPro;
		strcat( FeatStr, "CMov " );
	}
	if( D & 0x00000001 ) strcat( FeatStr, "FPU " );
	if( D & 0x00000010 ) strcat( FeatStr, "TimeStamp " );
	Logf( LOG_Init, "CPU Detected: %s (%s)", Model, Brand );
	Logf( LOG_Init, "CPU Features: %s", FeatStr );

	// Keyboard layout.
	char KL[KL_NAMELENGTH];
	checkState(GetKeyboardLayoutName(KL)!=0);
	debugf( "Keyboard layout: %s", KL );

	// Handle operator new allocation errors.
	_set_new_handler(UnrealAllocationErrorHandler);

	// Handle malloc allocation errors.
	_set_new_mode (1);	

	unguard;
}

//
// Set low precision mode.
//
int FGlobalPlatform::EnableFastMath(int Enable)
{
	guard(FGlobalPlatform::EnableFastMath);
	if( Enable )
	{
		// Fast, low precision, round down (for rendering).
		unsigned int FPState = _controlfp (_PC_24, _MCW_PC);
	}
	else
	{
		// Slow, high precision, round to nearest (for geometry).
		unsigned int FPState = _controlfp (_PC_64, _MCW_PC);
	}
	return Enable;
	unguard;
}

//
// See whether a floating point number is a not-a-number.
//
int FGlobalPlatform::IsNan( FLOAT f )
{
	guard(FGlobalPlatform::IsNan);
	return _isnan(f);
	unguard;
}

//
// Shut down the platform-specific subsystem.
// Not guarded.
//
void FGlobalPlatform::Exit()
{
	Log(LOG_Exit,"FGlobalPlatform exit");
}

//
// Check all memory allocations to make sure everything has been freed properly.
//
void FGlobalPlatform::CheckAllocations()
{
	guard(FGlobalPlatform::CheckAllocations);

	Log(LOG_Exit,"FGlobalPlatform CheckAllocations");
	FGlobalPlatform_DumpTrackedAllocations();

	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalPlatform misc.
-----------------------------------------------------------------------------*/

//
// Lookup the address of a DLL function
//
void *FGlobalPlatform::GetProcAddress( const char *ModuleName, const char *ProcName,int Checked )
{
	guard(FGlobalPlatform::GetProcAddress);

	char TempName[256];
	void *Result;

	HMODULE			hModule = GetModuleHandle(ModuleName);
	if( !hModule )	hModule = LoadLibrary    (ModuleName);

	if( !hModule)  
	{
		if( Checked )
			Errorf("Couldn't load module %s",ModuleName);
		return NULL;
	}
	Result = (void *)::GetProcAddress(hModule,ProcName);
	if( Result )
		return Result;

	// Actor processing function pointer.
	sprintf(TempName,"?%s@@3VUClass@@A",ProcName);
	Result = (void *)::GetProcAddress(hModule,TempName);
	if( Result )
		return Result;

	if( Checked )
		Errorf( "Couldn't find address of %s in %s",ProcName,ModuleName );
	return NULL;
	unguard;
}

//
// Break the debugger.
//
void FGlobalPlatform::DebugBreak()
{
	guard(FGlobalPlatform::DebugBreak);
	::DebugBreak();
	unguard;
}

/*-----------------------------------------------------------------------------
	High resolution timer.
-----------------------------------------------------------------------------*/

//
// Result is in microseconds.  Starting time is arbitrary.
// This should only be used for time deltas, not absolute events,
// since it wraps around.
//
QWORD FGlobalPlatform::MicrosecondTime()
{
	guard(FGlobalPlatform::MicrosecondTime);

	LARGE_INTEGER Numerator;

	if( !QueryPerformanceCounter(&Numerator) )
		App.Platform.Error( "FGlobalPlatform::Time: No performance counter exists" );

	QWORD N = (QWORD)Numerator.LowPart + (((QWORD)Numerator.HighPart)<<32);

	return (N*(QWORD)1000000)/FGlobalPlatform_PerformanceFrequency;

	unguard;
}

//
// Return the system time.
//
void FGlobalPlatform::SystemTime( INT *Year,INT *Month,INT *DayOfWeek,
	INT *Day,INT *Hour,INT *Min,INT *Sec,INT *MSec )
{
	guard(FGlobalPlatform::SystemTime);

	SYSTEMTIME st;
	GetLocalTime (&st);

	*Year		= st.wYear;
	*Month		= st.wMonth;
	*DayOfWeek	= st.wDayOfWeek;
	*Day		= st.wDay;
	*Hour		= st.wHour;
	*Min		= st.wMinute;
	*Sec		= st.wSecond;
	*MSec		= st.wMilliseconds;

	unguard;
}

//
// Convert milliseconds to CPU cycles.
//
FLOAT FGlobalPlatform::CpuToMilliseconds( INT CpuCycles )
{
	return 1000.0 * CpuCycles / FGlobalPlatform_CpuSpeed;
};

/*-----------------------------------------------------------------------------
	Windows functions.
-----------------------------------------------------------------------------*/

//
// Enable all windows.
//
void FGlobalPlatform::Enable()
{
	guard(FGlobalPlatform::Enable);

	CameraManager->EnableCameraWindows(0,1);
	App.Dialog->EnableWindow(1);

	unguard;
}

//
// Disable all windows, preventing them from accepting any input.
//
void FGlobalPlatform::Disable()
{
	guard(FGlobalPlatform::Disable);

	GCameraManager->EnableCameraWindows(0,0);
	App.Dialog->EnableWindow(0);

	unguard;
}

/*-----------------------------------------------------------------------------
	Link functions.
-----------------------------------------------------------------------------*/

//
// Launch a uniform resource locator (i.e. http://www.epicgames.com/unreal).
// This is expected to return immediately as the URL is launched by another
// task.
//
void FGlobalPlatform::LaunchURL( char *URL, char *Extra )
{
	guard(FGlobalPlatform::LaunchURL);

	Logf("LaunchURL %s",URL);

	if( ShellExecute(App.Dialog->m_hWnd,"open",URL_WEB,"","",SW_SHOWNORMAL)<=(HINSTANCE)32 )
	{
		MessageBox
		(
			"To visit Epic's Web site, you must have a Windows 95 Web browser installed.",
			"Can't visit the Web",
			0
		);
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	File mapping functions.
-----------------------------------------------------------------------------*/

//
// Create a file mapping object, set its address, and return its handle.
// Call with MaxSize=0 to open a read-only.
// Returns NULL if file not found.
//
BYTE *FGlobalPlatform::CreateFileMapping( FFileMapping &File, const char *Name, int MaxSize )
{
	guard(FGlobalPlatform::CreateFileMapping);

	// Init file mapping struct.
	strcpy( File.Filename, Name );
	File.hFile        = NULL;
	File.hFileMapping = NULL;
	File.Base         = NULL;

	// Create or open the file.
	// Note: Win32 bug prevents using read-only file mappings.
	guard(CreateFile);
	File.hFile = CreateFile
	(
		Name,
		MaxSize ? GENERIC_READ | GENERIC_WRITE   : GENERIC_READ,
		MaxSize ? 0                              : FILE_SHARE_READ,
		NULL,
		MaxSize ? CREATE_ALWAYS                  : OPEN_EXISTING,
		MaxSize ? FILE_FLAG_SEQUENTIAL_SCAN      : FILE_FLAG_RANDOM_ACCESS,
		NULL
	);
	unguard;
	if( File.hFile == NULL )
	{
		guard(CloseFile);
		CloseFileMapping( File, MaxSize ? -1 : 0 );
		return NULL;
		unguard;
	}

	// Make the file into a file mapping.
	guard(CreateFileMapping);
	File.hFileMapping = ::CreateFileMapping
	( 
		File.hFile,
		NULL, 
		MaxSize ? PAGE_READWRITE : PAGE_READONLY,
		0, 
		MaxSize, 
		NULL
	);
	unguard;
	if( File.hFileMapping == NULL  )
	{
		guard(CloseFileMapping);
		CloseFileMapping( File, MaxSize ? -1 : 0 );
		return NULL;
		unguard;
	}

	// Map the file into memory.
	guard(MapViewOfFile);
	File.Base = MapViewOfFile
	(
		File.hFileMapping,
		MaxSize ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ,
		0,
		0,
		0
	);
	unguard;
	if( File.Base == NULL )
	{
		guard(CloseFileMapping);
		CloseFileMapping( File, MaxSize ? -1 : 0 );
		return NULL;
		unguard;
	}

	// Success.
	checkOutput(((int)File.Base&15)==0);
	return (BYTE*)File.Base;
	unguard;
}

//
// Close a file mapping object.
// If Trunc > 0, truncate the file.
// If Trunc = 0, delete the file.
//
void FGlobalPlatform::CloseFileMapping( FFileMapping &File, INT Trunc )
{
	guard(FGlobalPlatform::CloseFileMapping);

	if( File.Base         ) UnmapViewOfFile( File.Base         ); File.Base         = NULL;
	if( File.hFileMapping ) CloseHandle    ( File.hFileMapping ); File.hFileMapping = NULL;
	if( Trunc > 0         ) SetFilePointer ( File.hFile, Trunc, 0, FILE_BEGIN );
	if( Trunc > 0         ) SetEndOfFile   ( File.hFile        );
	if( File.hFile        ) CloseHandle    ( File.hFile        ); File.hFile        = NULL;
	if( Trunc == 0        ) DeleteFile     ( File.Filename     );

	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalPlatform file finding.
-----------------------------------------------------------------------------*/

int FGlobalPlatform::FindFile( const char *In, char *Out )
{
	guard(FGlobalPlatform::FindFile);
	static const char *Paths[] = FIND_PATHS;

	// Try file as specified.
	strcpy( Out, In );
	if( fsize( Out ) >= 0 )
		return 1;

	// Try all of the predefined paths.
	for( int i=0; i<ARRAY_COUNT(Paths); i++ )
	{
		strcpy( Out, Paths[i] );
		strcat( Out, In       );
		if( fsize( Out ) >= 0 )
			return 1;
	}

	// Not found.
	return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalPlatform Log routines.
-----------------------------------------------------------------------------*/

//
// Print a message on the debugging log.
//
// This code is unguarded because trapped errors will just try
// to log more errors, resulting in a recursive mess.
//
void FGlobalPlatform::Write( const void *Data,int Length, ELogType Event )
{
	if (!LogAlive) 
		return;

	char C[256];

	Length = min (255,Length);
	strncpy (C,(char *)Data,Length);
	C[Length]=0;

	if (strchr(C,'\r')) *strchr(C,'\r')=0;
	if (strchr(C,'\n')) *strchr(C,'\n')=0;

	CString S = (CString)(LogEventNames[Event])+": "+C;

	if( SlowLog && SlowClosed )
	{
		LogFile = fopen(LogFname,"a+t");
	}
	if (Debugging) OutputDebugString(S+"\r\n");
	if (LogAlive && App.Dialog) App.Dialog->Log(S);
	if (LogFile) fputs(LPCSTR(S+"\n"),LogFile);

	if( SlowLog )
	{
		fclose(LogFile);
		SlowClosed=1;
	}
}

//
// Close the log file.
// Not guarded.
//
void FGlobalPlatform::CloseLog()
{
	CTime	T = CTime::GetCurrentTime();
	CString	S = (CString) "Log file closed, " + T.Format("%#c");

	if( LogFile )
	{
		Log(LOG_Info,LPCSTR(S));
		fclose(LogFile);
		LogFile = NULL;
	}
}

//
// Open the log file.
// Not guarded.
//
void FGlobalPlatform::OpenLog( const char *Fname )
{
	CTime		T = CTime::GetCurrentTime();
	CString	S = (CString) "Log file open, " + T.Format("%#c");

	if (LogFile!=NULL)	CloseLog();
	if (Fname!=NULL)	strcpy(LogFname,Fname);

	// Create new text file for read/write.
	LogFile = fopen(LogFname,"w+t");

	if( LogFile == NULL )
	{
		Log(LOG_Info,"Failed to open log");
	}
	else
	{
		fputs(
			"\n"
			"###############################################\n"
			"# Unreal, Copyright 1997 Epic MegaGames, Inc. #\n"
			"###############################################\n"
			"\n",
			LogFile);
		Log(LOG_Info,S);
	}
}

/*-----------------------------------------------------------------------------
	FGlobalPlatform windows specific routines.
-----------------------------------------------------------------------------*/

//
// Show memory status.
//
void FGlobalPlatform_MemoryStatus( FOutputDevice &Out )
{
	MEMORYSTATUS B; B.dwLength = sizeof(B);
	GlobalMemoryStatus(&B);
	Out.Log("Memory status:");
	Out.Logf
	(
		"Total: Phys=%iK Pagef=%iK Virt=%iK",
		B.dwTotalPhys/1024,
		B.dwTotalPageFile/1024,
		B.dwTotalVirtual/1024
	);
	Out.Logf
	(
		"Avail: Phys=%iK Pagef=%iK Virt=%iK",
		B.dwTotalPhys/1024,
		B.dwTotalPageFile/1024,
		B.dwTotalVirtual/1024
	);
	Out.Logf("Load = %i%%",B.dwMemoryLoad);
}

//
// Set the parent of the main server/log window.
//
void FGlobalPlatform::SetParent( DWORD hWndNewParent )
{
	guard(FGlobalPlatform::SetParent);
	hWndParent = hWndNewParent;
	unguard;
}

//
// Exit at the engine's request.
//
// This doesn't cause an immediate exit, but rather waits till the
// message queue settles and execution can unwind naturally.
//
void FGlobalPlatform::RequestExit()
{
	guard(FGlobalPlatform::RequestExit);
	App.Dialog->Exit();
	unguard;
}

//
// Message box.  YesNo: 1=yes/no, 0=ok.
// Not guarded.
//
int FGlobalPlatform::MessageBox( const char *Text, const char *Title, int YesNo )
{
	guard(FGlobalPlatform::MessageBox);

	if (!YesNo) debugf(LOG_Info,Text);
	return ::MessageBox(NULL,Text,Title,(YesNo?MB_YESNO:MB_OK) | MB_APPLMODAL)==IDYES;

	unguard;
}

//
// Put up a message box for debugging.
// Not guarded.
//
int VARARGS FGlobalPlatform::DebugBoxf( const char *Fmt, ... )
{
	char TempStr[4096];
	va_list  ArgPtr;

	va_start (ArgPtr,Fmt);
	vsprintf (TempStr,Fmt,ArgPtr);
	va_end   (ArgPtr);

	return MessageBox(TempStr,"Unreal DebugBoxf",0);
}

//
// Send a callback value to the UnrealEd client.
//
void FGlobalPlatform::EdCallback( WORD Code, WORD Param )
{
	guard(FGlobalPlatform::EdCallback);

	if( App.hWndCallback!=NULL )
		PostMessage( App.hWndCallback, WM_CHAR, 32+Code, 0 );
	
	unguard;
}

//
// Show all Unreal windows
//
void FGlobalPlatform::Show()
{
	guard(FGlobalPlatform::Show);

	App.UpdateUI();
	App.Dialog->ShowMe();
	
	unguard;
}

//
// Minimize all Unreal windows
// 
void FGlobalPlatform::Minimize()
{
	guard(FGlobalPlatform::Minimize);
	App.Dialog->ShowWindow(SW_SHOWMINIMIZED);
	unguard;
}

//
// Hide all Unreal windows.
//
void FGlobalPlatform::Hide()
{
	guard(FGlobalPlatform::Hide);
	App.Dialog->ShowWindow(SW_HIDE);
	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalPlatform memory allocation.
-----------------------------------------------------------------------------*/

// Allocate memory. Tracks all memory allocations.
void *VARARGS FGlobalPlatform::Malloc( int Size, const char *Fmt, ... )
{
#if CHECK_ALLOCS
	char TempStr[4096];
	va_list  ArgPtr;

	va_start (ArgPtr,Fmt);
	vsprintf (TempStr,Fmt,ArgPtr);
	va_end   (ArgPtr);

	// Can't enclose variable arguments in GUARD/UNGUARD block.
	guard(FGlobalPlatform::Malloc);
	checkInput(Size>=0);

	if( Size == 0 )
	{
		return NULL;
	}
	else
	{
		void *Ptr = malloc(Size);
		if( !Ptr ) Errorf("Out of memory: %s",TempStr);
		FGlobalPlatform_AddTrackedAllocation(Ptr,Size,TempStr);
		return Ptr;
	}
	unguardf(("(%i %s)",Size,TempStr));
#else
	guard(FGlobalPlatform::Malloc);
	return malloc(Size);
	unguard;
#endif
}

//
// Reallocate memory.
//
void *VARARGS FGlobalPlatform::Realloc( void *Ptr, int NewSize, const char *Fmt,... )
{
#if CHECK_ALLOCS
	char TempStr[4096];
	va_list  ArgPtr;

	va_start (ArgPtr,Fmt);
	vsprintf (TempStr,Fmt,ArgPtr);
	va_end   (ArgPtr);

	guard(FGlobalPlatform::Realloc);
	checkInput(NewSize>=0);

	if( Ptr == NULL )
	{
		if( NewSize == 0 )
		{
			return NULL;
		}
		else
		{
			Ptr = malloc(NewSize);
			if( !Ptr ) Errorf( "Out of memory: %s", TempStr );
			FGlobalPlatform_AddTrackedAllocation( Ptr, NewSize, TempStr );
			return Ptr;
		}
	}
	else
	{
		if( NewSize == 0 )
		{
			FGlobalPlatform_DeleteTrackedAllocation( Ptr );
			return NULL;
		}
		else
		{
			for( FTrackedAllocation *A = GTrackedAllocations; A; A=A->Next )
			{
				if( A->Ptr == Ptr )
					break;
			}
			if( !A ) appErrorf( "Allocation %s not found ",TempStr);
			A->Ptr = realloc( Ptr, NewSize );
			A->Size = NewSize;
			return A->Ptr;
		}
	}
	unguardf(("(%i %s)",NewSize,TempStr));
#else
	guard(FGlobalPlatform::Realloc);
	return realloc( Ptr, NewSize );
	unguard;
#endif
}

//
// Free memory.
//
void FGlobalPlatform::Free( void *Ptr )
{
#if CHECK_ALLOCS
	guard(FGlobalPlatform::Free);
	if( Ptr ) free(FGlobalPlatform_DeleteTrackedAllocation(Ptr));
	unguard;
#else
	guard(FGlobalPlatform::Free);
	free(Ptr);
	unguard;
#endif
}

/*-----------------------------------------------------------------------------
	FGlobalPlatform error handling.
-----------------------------------------------------------------------------*/

//
// Allocation error handler.
//
int CDECL UnrealAllocationErrorHandler( size_t )
{
	appError
	(
		"Unreal has run out of virtual memory. "
		"To prevent this condition, you must free up more space "
		"on your primary hard disk."
	);
	return 0;
}

//
// Shutdown all vital subsystems after an error occurs.  This makes sure that
// vital support such as DirectDraw is shut down before exiting.
// Not guarded.
//
void FGlobalPlatform::ShutdownAfterError()
{
	try
	{
		GuardTrap = 1;

		if( ServerAlive )
		{
			ServerAlive = 0;
			debug(LOG_Exit,"FGlobalPlatform::ShutdownAfterError");
			GCameraManager->ShutdownAfterError();
		}
	}
	catch(...)
	{
		try
		{
			// Double fault.
	  		Log(LOG_Critical,"Double fault in FGlobalPlatform::ShutdownAfterError");
		}
		catch(...)
		{
			// Triple fault. We are hosed.
			ExitProcess(1);
		}
	}
}

//
// Handle a critical error, and unwind the stack, dumping out the
// calling stack for debugging.
// Not guarded.
//
void FGlobalPlatform::Error( const char *Msg )
{
	if( !App.Dialog )
	{
		Log(LOG_Critical,"Error without dialog");
	}
	else if( InAppError )
	{
		Logf(LOG_Critical,"Error reentered: %s",Msg);
	}
	else if( Debugging )
	{
		CameraManager->EndFullscreen();
  		Log(LOG_Critical,"Breaking debugger");

		// First time doesn't seem to trigger break in NT 4.0.
		DebugBreak();
		DebugBreak(); 
	}
	else
	{
		ShutdownAfterError();

	  	Log(LOG_Critical,"appError triggered:");
	  	Log(LOG_Critical,Msg);

		strcpy(ErrorHist,Msg);
		strcat(ErrorHist,"\r\n\r\nHistory:  ");

		InAppError  = 1;
		strcpy(App.Error,Msg);
	}
	throw (1);
}

//
// Global error handler with a formatted message.
// Not guarded.
//
void VARARGS FGlobalPlatform::Errorf( const char *Fmt, ... )
{
	char 	TempStr[4096];
	va_list ArgPtr;

	va_start (ArgPtr,Fmt);
	vsprintf (TempStr,Fmt,ArgPtr);
	va_end   (ArgPtr);

	Error(TempStr);
}

void VARARGS FGlobalPlatform::GuardMessagef( const char *Fmt, ... )
{
	char 	TempStr[4096];
	va_list ArgPtr;

	va_start (ArgPtr,Fmt);
	vsprintf (TempStr,Fmt,ArgPtr);
	va_end   (ArgPtr);

	Log(LOG_Critical,TempStr);
	if( GuardTrap ) strcat(ErrorHist," <- ");
	strcat(ErrorHist,TempStr);
	GuardTrap = 1; 
}

/*-----------------------------------------------------------------------------
	Slow task and progress bar functions.
-----------------------------------------------------------------------------*/

//
// Begin a slow task, optionally bringing up a progress bar.  Nested calls may be made
// to this function, and the dialog will only go away after the last slow task ends.
//
void FGlobalPlatform::BeginSlowTask( const char *Task, int StatusWindow, int Cancelable )
{
	guard(FGlobalPlatform::BeginSlowTask);
	if( FGlobalPlatform_hWndProgressBar && FGlobalPlatform_hWndProgressText )
	{
		SendMessage( FGlobalPlatform_hWndProgressBar, PBM_SETRANGE, (WPARAM)0, MAKELPARAM(0, 100) );
		SendMessage( FGlobalPlatform_hWndProgressText, WM_SETTEXT, (WPARAM)0, (LPARAM)Task );
	}
	InSlowTask++;
	unguard;
}

//
// End the slow task.
//
void FGlobalPlatform::EndSlowTask()
{
	guard(FGlobalPlatform::EndSlowTask);
	
	if( InSlowTask>0 )
	{
		InSlowTask--;
	}
	else debug( LOG_Info, "EndSlowTask: Not begun" );
	unguard;
}

//
// Update the progress bar with a message and percent complete.
//
int FGlobalPlatform::StatusUpdate( const char *Str, int Numerator, int Denominator )
{
	guard(FGlobalPlatform::StatusUpdate);

	if( InSlowTask && FGlobalPlatform_hWndProgressBar && FGlobalPlatform_hWndProgressText )
	{
		SendMessage( FGlobalPlatform_hWndProgressText, WM_SETTEXT, (WPARAM)0, (LPARAM)Str );
		SendMessage( FGlobalPlatform_hWndProgressBar, PBM_SETPOS, (WPARAM)(Denominator ? 100*Numerator/Denominator : 0), (LPARAM)0 );
	}

	// Should return 0 if cancel is desired, but cancel isn't implemented.
	return 1;
	unguard;
}

//
// Update the progress bar.
//
int VARARGS FGlobalPlatform::StatusUpdatef( const char *Fmt, int Numerator, int Denominator, ... )
{
	guard(FGlobalPlatform::StatusUpdatef);

	char		TempStr[4096];
	va_list		ArgPtr;

	va_start (ArgPtr,Denominator);
	vsprintf (TempStr,Fmt,ArgPtr);
	va_end   (ArgPtr);

	return StatusUpdate( TempStr,Numerator, Denominator );
	
	unguard;
}

//----------------------------------------------------------------------------
//                The default configuration file name.
//----------------------------------------------------------------------------

const char * FGlobalPlatform::DefaultProfileFileName() const
{
    static char FileName[_MAX_PATH+1] = { 0 }; //+1 for trailing null.
    // Determine the name only once:
    if( FileName[0] == 0 )
    {
		if( !GetSTRING(GDefaults.CmdLine,"INI=",FileName,_MAX_PATH) )
        {
	        strcpy(FileName,StartingPath); 
            strcat(FileName,PROFILE_RELATIVE_FNAME);
        }
        debugf( LOG_Init, "Profile: %s", FileName );
    }
    return FileName;
}

//----------------------------------------------------------------------------
//                The "factory-settings" configuration file name.
//----------------------------------------------------------------------------

const char * FGlobalPlatform::FactoryProfileFileName() const
{
    static char FileName[_MAX_PATH+1] = { 0 }; //+1 for trailing null.
    // Determine the name only once:
    if( FileName[0] == 0 )
    {
        strcpy(FileName,StartingPath); 
        strcat(FileName,FACTORY_PROFILE_RELATIVE_FNAME);
        debugf( LOG_Init, "Factory Profile: %s", FileName );
    }
    return FileName;
}

//----------------------------------------------------------------------------
//       Profile operation: Get the integer value for Key in Section.
//----------------------------------------------------------------------------

int FGlobalPlatform::GetProfileInteger 
(
    const char * Section      // The name of the section
,   const char * Key          // The name of the key.
,   int          Default      // The default value if the key is not found.
,   const char * FileName     // The name of the profile file. 0 to use the default.
)
{
    if( FileName == 0 )
    {
        FileName = DefaultProfileFileName();
    }
    int Value = GetPrivateProfileInt(Section,Key,Default,FileName);
    //Logf( LOG_Debug, "GetProfileInteger(%s,%s,%i,%s) => %i", Section, Key, Default, FileName, Value );
    return Value;
}

//----------------------------------------------------------------------------
//       Profile operation: Get a boolean value.
//----------------------------------------------------------------------------

BOOL FGlobalPlatform::GetProfileBoolean
(
    const char * Section      // The name of the section
,   const char * Key          // The name of the key.
,   BOOL         DefaultValue // The default value if a valid boolean profile value is not found.
,   const char * FileName     // The name of the profile file. 0 to use the default.
)
{
    char Text[30]; 
    BOOL Result = DefaultValue;
    if( GetProfileValue(Section,Key,0,Text,sizeof(Text),FileName) )
    {
        if( stricmp(Text,"true")==0 )
        {
            Result = TRUE;
        }
        else if( stricmp(Text,"false")==0 )
        {
            Result = FALSE;
        }
        else
        {
            //tba? As a courtesy, we should probably display an error message.
        }
        //Logf( LOG_Debug, "GetProfileBoolean(%s,%s,%i,%s) => %i", Section, Key, DefaultValue, FileName, Result );
    }
    return Result;
}

//----------------------------------------------------------------------------
//       Profile operation: Put a boolean value.
//----------------------------------------------------------------------------

void FGlobalPlatform::PutProfileBoolean
(
    const char * Section      // The name of the section
,   const char * Key          // The name of the key.
,   BOOL         Value        // The value to put.
,   const char * FileName     // The name of the profile file. 0 to use the default.
)
{
    char Text[30]; 
    strcpy( Text, Value ? "True" : "False" );
    PutProfileValue(Section,Key,Text,FileName);
    //Logf( LOG_Debug, "PutProfileBoolean(%s,%s,%i,%s)", Section, Key, Value, FileName );
}

//----------------------------------------------------------------------------
//       Profile operation: Put an integer value.
//----------------------------------------------------------------------------

void FGlobalPlatform::PutProfileInteger
(
    const char * Section      // The name of the section
,   const char * Key          // The name of the key.
,   int          Value        // The value to put.
,   const char * FileName     // The name of the profile file. 0 to use the default.
)
{
    char Text[30]; // Big enough to hold any integer.
    sprintf( Text, "%i", Value );
    PutProfileValue(Section,Key,Text,FileName);
    //Logf( LOG_Debug, "PutProfileInteger(%s,%s,%i,%s)", Section, Key, Value, FileName );
}

//----------------------------------------------------------------------------
//       Profile operation: Get all the values in a section.
//----------------------------------------------------------------------------

void FGlobalPlatform::GetProfileSection 
(
    const char * Section      // The name of the section
,   char       * Values       // Where to put Key=Value strings.
,   int          Size         // The size of Values.
,   const char * FileName     // The name of the profile file. 0 to use the default.
)
{
    if( FileName == 0 )
    {
        FileName = DefaultProfileFileName();
    }
    debugf( LOG_Init, "Reading profile section '%s' from %s", Section, FileName );
    // Double-terminate the list initially. //tbi? Assumes Size >= 2
    Values[0] = 0;
    Values[1] = 0; 
    GetPrivateProfileSection(Section,Values,Size,FileName);
    //Logf( LOG_Debug, "GetProfileSection(%s,%s) => %s...", Section, FileName, Values );
}	

//----------------------------------------------------------------------------
//       Profile operation: Get the value associated with a key.
//----------------------------------------------------------------------------

BOOL FGlobalPlatform::GetProfileValue 
(
    const char * Section      // The name of the section
,   const char * Key          // The name of the key.
,   const char * Default      // The default value if the key is not found.
,         char * Value        // The output value.
,   int          Size         // The size of *Value.
,   const char * FileName     // The name of the profile file. 0 to use the default.
)
{
    Value[0] = 0;
    if( FileName == 0 )
    {
        FileName = DefaultProfileFileName();
    }
    const int Count = GetPrivateProfileString
    (
        Section
    ,   Key
    ,   Default==0 ? "" : Default
    ,   Value
    ,   Size
    ,   FileName
    );  
    //Logf( LOG_Debug, "GetProfileValue(%s,%s,%s,%s) => %s...", Section, Key, Default==0?"":Default,FileName,Value );
	return Count > 0 && Value[0] != 0;
}

//----------------------------------------------------------------------------
// Profile operation: Remove all values in the section and write out new values.
//----------------------------------------------------------------------------

void FGlobalPlatform::PutProfileSection 
(
    const char * Section      // The name of the section
,   const char * Values       // A list of Key=Value strings.
,   const char * FileName     // The name of the profile file. 0 to use the default.
)
{
    if( FileName == 0 )
    {
        FileName = DefaultProfileFileName();
    }
    const BOOL Okay = WritePrivateProfileSection(Section,Values,FileName);
    debugf( LOG_Info, "Writing profile section '%s' into %s", Section, FileName );
    //Logf( LOG_Debug, "PutProfileSection(%s,%s...,%s)", Section, Values, FileName );
    if( !Okay )
    {
        // For robust error notificiation, we could interpret GetLastError()
        // and show a message box. 
    }
}	

//----------------------------------------------------------------------------
//       Profile operation:
//----------------------------------------------------------------------------

void FGlobalPlatform::PutProfileValue // Change the value associated with a key in a section.
(
    const char * Section,     // The name of the section
    const char * Key,         // The name of the key.
    const char * Value,       // The value to use. 0 causes the value to be deleted from the profile.
    const char * FileName     // The name of the profile file. 0 to use the default.
)
{
    if( FileName == 0 )
    {
        FileName = DefaultProfileFileName();
    }
    Logf( LOG_Debug, "PutProfileValue(%s,%s,%s,%s)", Section, Key, Value, FileName );
    const BOOL Okay = WritePrivateProfileString( Section, Key, Value, FileName );
    if( !Okay )
    {
        // For robust error notificiation, we could interpret GetLastError()
        // and show a message box. 
    }
}	

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
