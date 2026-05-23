/*=============================================================================
	UnWn.h: Main header for UnWn application
	Used by: Log window

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef __AFXWIN_H__
	#error "include 'StdAfx.h' before including this file for PCH"
#endif

#include "Resource.h"		// main symbols

#include "UnWnDlg.h"
#include "UnWnEdSv.h"

extern class CUnrealWnApp App;

/////////////////////////////////////////////////////////////////////////////
// FSimpleOutputDevice.
/////////////////////////////////////////////////////////////////////////////

//
// A simple output device which just encapsulates a CString.
//
class FSimpleOutputDevice : public FOutputDevice
{
public:
	// FOutputDevice interface.
	void Write( const void *ThisData, int Length, ELogType MsgType )
	{
		guard(FSimpleOutputDevice::Write);
		if( Num+Length+1 > Max )
		{
			Max = Max*2 + Length + 2048;
			Str = (char*)realloc( Str, Max );
		}
		memcpy( Str + Num, ThisData, Length+1 );
		Num += Length;
		unguard;
	}
	BSTR AllocSysString()
	{
		return CString(Str).AllocSysString();
	}
	FSimpleOutputDevice()
	:	Num(0), Max(0), Str(NULL)
	{};
private:
	// Variables.
	int Num,Max;
	char *Str;
};

/////////////////////////////////////////////////////////////////////////////
// CUnrealWnApp:
// See UnWn.cpp for the implementation of this class.
/////////////////////////////////////////////////////////////////////////////

class CUnrealWnApp : public CWinApp
{
public:
	// Constructor.
	CUnrealWnApp();

	// Custom.
	DWORD AlwaysOnTop;
	CUnrealWnDlg *Dialog;
	HWND hWndMain;
	HWND hWndCallback;
	void UpdateUI();
	void RegisterFileTypes(char *BaseDir);
	char CmdLine[256];
	char Error[1024];
	char BaseDir[256];
	int InOle,OleCrashed,UsageCount,InError;

	FGlobalPlatform Platform;
	//CUnrealTimer Timer;

	void RouteMessage(void *Msg);
	void MessagePump();
	void InitializeUnreal();
	void UnrealLockApp();
	void UnrealUnlockApp();
	void FinalizeErrorText();

	// Overrides
	//{{AFX_VIRTUAL(CUnrealWnApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL
	//
	// Implementation
	//
	//{{AFX_MSG(CUnrealWnApp)
	afx_msg void OnFileExit();
	afx_msg void OnWindowNewCamera();
	afx_msg void OnHelpAboutUnreal();
	afx_msg void OnHelpEpicsWebSite();
	afx_msg void OnHelpHelpTopics();
	afx_msg void OnHelpOrderingUnreal();
	afx_msg void OnHelpOrderNow();
	afx_msg void OnWindowAlwaysOnTop();
	afx_msg void OnLogCloseLog();
	afx_msg void OnLogOpenUnrealLog();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnFileBeginGame();
	afx_msg void OnFileEndGame();
	afx_msg void OnFileLoadGame();
	afx_msg void OnFileSaveGame();
	afx_msg void OnNetGame();
	afx_msg void OnPropertiesProperties();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
};

/////////////////////////////////////////////////////////////////////////////
// Ole helpers
/////////////////////////////////////////////////////////////////////////////

static int inline CheckUnrealState(char *Descr)
{
	if( App.Platform.GuardTrap )
	{
		char Error[16384];
		strcpy(Error,"Unrecoverable: ");
		strcat(Error,App.Platform.ErrorHist);

		if (App.Platform.Debugging) DebugBreak();
		else AfxThrowOleDispatchException(1,Error);
		return 0;
	}
	else if( !App.Platform.ServerAlive )
	{
		App.Platform.Logf(LOG_Critical,"Ole '%s' after server shutdown",Descr);
		return 0;
	}
	else
	{
		GMem.Tick();
		GDynMem.Tick();
		return 1;
	}
}

static void inline HandleOleError(char *Module)
{
	App.InOle              = 0;
	App.OleCrashed         = 1;
	App.Platform.GuardTrap = 1;

	App.Platform.ShutdownAfterError();

	strcat(App.Platform.ErrorHist," <- Ole call to ");
	strcat(App.Platform.ErrorHist,Module);
	App.FinalizeErrorText();

	App.Platform.Logf("Exiting due to Ole error");
	App.Platform.CloseLog();

	if( App.Platform.Debugging ) DebugBreak();
	else AfxThrowOleDispatchException(1,App.Platform.ErrorHist);
}

/////////////////////////////////////////////////////////////////////////////
