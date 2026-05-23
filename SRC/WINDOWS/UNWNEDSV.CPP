/*=============================================================================
	UnEdSrv.cpp: CUnrealEdServer implementation

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "StdAfx.h"
#include "UnWn.h"
#include "Unreal.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Globals.
/////////////////////////////////////////////////////////////////////////////

enum {MAX_RESULTS_LENGTH=16384};

/////////////////////////////////////////////////////////////////////////////
// CUnrealEdServer.
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CUnrealEdServer, CWnd)

CUnrealEdServer::CUnrealEdServer()
{
	EnableAutomation();
	App.UnrealLockApp();
	App.Platform.Log(LOG_Info,"Created CUnrealEdServer");
}

CUnrealEdServer::~CUnrealEdServer()
{
}

void CUnrealEdServer::OnFinalRelease()
{
	App.Platform.Log(LOG_Info,"OnFinalRelease CUnrealEdServer");
	App.UnrealUnlockApp();
	CWnd::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CUnrealEdServer, CWnd)
	//{{AFX_MSG_MAP(CUnrealEdServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CUnrealEdServer, CWnd)
	//{{AFX_DISPATCH_MAP(CUnrealEdServer)
	DISP_FUNCTION(CUnrealEdServer, "Exec", Exec, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CUnrealEdServer, "SlowExec", SlowExec, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CUnrealEdServer, "GetProp", GetProp, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CUnrealEdServer, "SetProp", SetProp, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CUnrealEdServer, "Enable", Enable, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CUnrealEdServer, "Disable", Disable, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CUnrealEdServer, "Init", Init, VT_EMPTY, VTS_I4 VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

//
// Note: we add support for IID_IUnrealEdServer to support typesafe binding
// from VBA.  This IID must match the GUID that is attached to the 
// dispinterface in the .ODL file.
//

// {D0EB88E6-2016-11CF-98C0-0000C06958A7}
static const IID IID_IUnrealEdServer =
{ 0xd0eb88e6, 0x2016, 0x11cf, { 0x98, 0xc0, 0x0, 0x0, 0xc0, 0x69, 0x58, 0xa7 } };

BEGIN_INTERFACE_MAP(CUnrealEdServer, CWnd)
	INTERFACE_PART(CUnrealEdServer, IID_IUnrealEdServer, Dispatch)
END_INTERFACE_MAP()

// {F936C3A7-1FF8-11CF-98C0-0000C06958A7}
SINGLEUSE_IMPLEMENT_OLECREATE(CUnrealEdServer, "Unreal.UnrealEdServer", 0xf936c3a7, 0x1ff8, 0x11cf, 0x98, 0xc0, 0x0, 0x0, 0xc0, 0x69, 0x58, 0xa7)

/////////////////////////////////////////////////////////////////////////////
// CUnrealEdServer OLE handlers.
/////////////////////////////////////////////////////////////////////////////

void CUnrealEdServer::Exec(LPCTSTR Cmd) 
{
	if( !CheckUnrealState("Exec") )
		return;
	
	try
	{
		App.InOle=1;

		App.Platform.Log(LOG_Cmd,Cmd);
		GUnreal.Exec(Cmd);
		CheckUnrealState("PostExec");
		App.InOle=0;
	}
	catch(...)
	{
		char C[256]="Exec (";
		char *S=&C[strlen(C)];
		strncpy(S,Cmd,160);
		S[160]=0;
		strcat(S,")");
		HandleOleError(C);
	}
}

void CUnrealEdServer::SlowExec(LPCTSTR Cmd) 
{
	if( !CheckUnrealState("SlowExec") )
		return;
	
	try
	{
		App.InOle=1;
		HCURSOR SavedCursor = SetCursor(LoadCursor(NULL,IDC_WAIT));
		App.Platform.Log(LOG_Cmd,Cmd);
		GUnreal.Exec(Cmd);
		CheckUnrealState("PostSlowExec");
		SetCursor(SavedCursor);
		App.InOle=0;
	}
	catch(...)
	{
		HandleOleError("SlowExec");
	}
}

BSTR CUnrealEdServer::GetProp(LPCTSTR Topic, LPCTSTR Item) 
{
	if( !CheckUnrealState("GetProp") )
		return NULL;
	
	try
	{
		FSimpleOutputDevice Out;
		App.InOle=1;
		GTopics.Get(NULL,Topic,Item,Out);
		CheckUnrealState("PostGetProp");
		App.InOle=0;
		return Out.AllocSysString();
	}
	catch(...)
	{
		HandleOleError("GetProp");
	}
	return NULL;
}

void CUnrealEdServer::SetProp(LPCTSTR Topic, LPCTSTR Item, LPCTSTR NewValue)
{
	if( !CheckUnrealState("SetProp") )
		return;
	
	try
	{
		App.InOle=1;
		GTopics.Set(NULL,Topic,Item,const_cast<char *>(NewValue));
		CheckUnrealState("PostSetProp");
		App.InOle=0;
	}
	catch(...)
	{
		HandleOleError("SetProp");
	}
}

void CUnrealEdServer::Init(long hWndMain, long hWndEdCallback)
{
	if( !CheckUnrealState("Init") )
		return;
	
	try
	{
		App.InOle=1;
		App.hWndMain       = (HWND)hWndMain;
		App.hWndCallback   = (HWND)hWndEdCallback;
		App.InOle=0;
	}
	catch(...)
	{
		HandleOleError("Init");
	}
}

void CUnrealEdServer::Enable() 
{
	if (!CheckUnrealState("Enable")) 
		return;
	
	App.Platform.Enable();
}

void CUnrealEdServer::Disable() 
{
	if (!CheckUnrealState("Disable")) 
		return;
	
	App.Platform.Disable();
}

/////////////////////////////////////////////////////////////////////////////
// The End.
/////////////////////////////////////////////////////////////////////////////
