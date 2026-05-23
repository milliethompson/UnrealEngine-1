/*=============================================================================
	UnSrv.cpp: CUnrealServer's implementation

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "StdAfx.h"
#include "UnWn.h"
#include "UnSrv.h"
#include "Unreal.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnrealServer.
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CUnrealServer, CWnd)

CUnrealServer::CUnrealServer()
{
	EnableAutomation();
	App.UnrealLockApp();
	App.Platform.Log(LOG_Win,"Created CUnrealServer");
}

CUnrealServer::~CUnrealServer()
{
}

void CUnrealServer::OnFinalRelease()
{
	App.Platform.Log(LOG_Win,"OnFinalRelease CUnrealServer");
	App.UnrealUnlockApp();
	CWnd::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CUnrealServer, CWnd)
	//{{AFX_MSG_MAP(CUnrealServer)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CUnrealServer, CWnd)
	//{{AFX_DISPATCH_MAP(CUnrealServer)
	DISP_FUNCTION(CUnrealServer, "Exec", Exec, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CUnrealServer, "GetProp", GetProp, VT_BSTR, VTS_BSTR VTS_BSTR)
	DISP_FUNCTION(CUnrealServer, "Init", Init, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CUnrealServer, "SetProp", SetProp, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

//
// Note: we add support for IID_IUnrealServer to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.
//

// {65033ED8-3939-11D0-B0AE-00C04FD8ED0D}
static const IID IID_IUnrealServer =
{ 0x65033ed8, 0x3939, 0x11d0, { 0xb0, 0xae, 0x0, 0xc0, 0x4f, 0xd8, 0xed, 0xd } };

BEGIN_INTERFACE_MAP(CUnrealServer, CWnd)
	INTERFACE_PART(CUnrealServer, IID_IUnrealServer, Dispatch)
END_INTERFACE_MAP()

// {65033ED9-3939-11D0-B0AE-00C04FD8ED0D}
SINGLEUSE_IMPLEMENT_OLECREATE(CUnrealServer, "Unreal.UnrealServer", 0x65033ed9, 0x3939, 0x11d0, 0xb0, 0xae, 0x0, 0xc0, 0x4f, 0xd8, 0xed, 0xd)

/////////////////////////////////////////////////////////////////////////////
// CUnrealServer message handlers.
/////////////////////////////////////////////////////////////////////////////

void CUnrealServer::Exec(LPCTSTR Cmd) 
{
	static void *MemTop=GMem.Get(0);

	if( !CheckUnrealState("Exec") )
		return;
	
	try	
	{
		App.InOle=1;

		int Leak = (int)GMem.Get(0)-(int)MemTop;
		if (Leak!=0)
			appErrorf("Memory leak: %i",Leak);

		App.Platform.Log(LOG_Cmd,Cmd);

		GUnreal.Exec(Cmd);

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

BSTR CUnrealServer::GetProp(LPCTSTR Topic, LPCTSTR Item) 
{
	if (!CheckUnrealState("GetProp")) 
		return NULL;
	
	try	
	{
		CString	strResult;
		char	szResult[16384];
		App.InOle=1;
		GTopics.Get(NULL,Topic,Item,szResult);
		strResult = szResult;
		App.InOle=0;
		return strResult.AllocSysString();
	}
	catch(...) 
	{
		HandleOleError("GetProp");
	}
	return NULL;
}

void CUnrealServer::Init(long hWndMain, long hWndCallback) 
{
	if( !CheckUnrealState("Init") )
		return;
	
	try	
	{
		App.InOle=1;
		App.hWndMain       = (HWND)hWndMain;
		App.hWndCallback   = (HWND)hWndCallback;
		App.InOle=0;
	}
	catch(...)
	{
		HandleOleError("Init");
	}
}

void CUnrealServer::SetProp(LPCTSTR Topic, LPCTSTR Item, LPCTSTR NewValue) 
{
	if( !CheckUnrealState("SetProp") )
		return;
	
	try	
	{
		App.InOle=1;
		GTopics.Set(NULL,Topic,Item,const_cast<char *>(NewValue));
		App.InOle=0;
	}
	catch(...)
	{
		HandleOleError("SetProp");
	}
}

/////////////////////////////////////////////////////////////////////////////
// The End.
/////////////////////////////////////////////////////////////////////////////
