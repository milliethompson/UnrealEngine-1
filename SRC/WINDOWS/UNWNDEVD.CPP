/*
==============================================================================
UnWnDevD.cpp: Input devices dialog class

Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
    This is the CDialog-based class for handling the Unreal input devices dialog.

Revision history:
    * 06/26/96, Created by Mark
==============================================================================
*/

#include "stdafx.h"
#include "unwn.h"
#include "UnWnDevD.h"
#include "UnInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogInputDevices dialog


CDialogInputDevices::CDialogInputDevices(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogInputDevices::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogInputDevices)
	UseJoystick1 = FALSE;
	UseJoystick2 = FALSE;
	UseMouse = FALSE;
	//}}AFX_DATA_INIT
}

BOOL CDialogInputDevices::OnInitDialog()
{
    CDialog::OnInitDialog();
    {
        UseMouse        = GApp->Input->UsingDevice( FPlatformInput::MouseDevice     );
        UseJoystick1    = GApp->Input->UsingDevice( FPlatformInput::Joystick1Device );
        UseJoystick2    = GApp->Input->UsingDevice( FPlatformInput::Joystick2Device );
        UpdateData(FALSE);
    }
    return FALSE; // FALSE tells Windows not to set the input focus. 
                  // We do this because the dialog is expected to be part
                  // of a tab group and we don't want to change the focus.
}

void CDialogInputDevices::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogInputDevices)
	DDX_Check(pDX, IDC_INPUT_USE_JOYSTICK1, UseJoystick1);
	DDX_Check(pDX, IDC_INPUT_USE_JOYSTICK2, UseJoystick2);
	DDX_Check(pDX, IDC_INPUT_USE_MOUSE, UseMouse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogInputDevices, CDialog)
	//{{AFX_MSG_MAP(CDialogInputDevices)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogInputDevices message handlers

//----------------------------------------------------------------------------
//                      Check input and accept changes
//----------------------------------------------------------------------------
BOOL CDialogInputDevices::Accept() 
{
    BOOL Accepted = TRUE; // We always accept the input (it cannot be wrong).
    UpdateData(TRUE);
    GApp->Input->UseDevice( FPlatformInput::MouseDevice     , UseMouse         );
    GApp->Input->UseDevice( FPlatformInput::Joystick1Device , UseJoystick1     );
    GApp->Input->UseDevice( FPlatformInput::Joystick2Device , UseJoystick2     );
    return Accepted;
}


