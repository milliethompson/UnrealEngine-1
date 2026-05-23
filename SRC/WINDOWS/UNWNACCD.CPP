/*============================================================================
UnWnAccD.cpp: Action selection dialog class

Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
    Refer to the associated header file.

Revision history:
    * xx/xx/96: Created by Mark
============================================================================*/

#include "stdafx.h"
#include "unwn.h"
#include "UnWnAcCD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogSelectActionClasses dialog


CDialogSelectActionClasses::CDialogSelectActionClasses(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSelectActionClasses::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSelectActionClasses)
	SelectAdministrativeActions = FALSE;
	SelectAdvancedActions = FALSE;
	SelectMiscellaneousActions = FALSE;
	SelectMoveActions = FALSE;
	SelectTurnActions = FALSE;
	SelectWeaponActions = FALSE;
	//}}AFX_DATA_INIT

    ShowAllActions = FALSE;
}


void CDialogSelectActionClasses::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSelectActionClasses)
	DDX_Check(pDX, IDC_SELECT_ADMIN_ACTIONS, SelectAdministrativeActions);
	DDX_Check(pDX, IDC_SELECT_ADVANCED_ACTIONS, SelectAdvancedActions);
	DDX_Check(pDX, IDC_SELECT_MISC_ACTIONS, SelectMiscellaneousActions);
	DDX_Check(pDX, IDC_SELECT_MOVE_ACTIONS, SelectMoveActions);
	DDX_Check(pDX, IDC_SELECT_TURN_ACTIONS, SelectTurnActions);
	DDX_Check(pDX, IDC_SELECT_WEAPON_ACTIONS, SelectWeaponActions);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogSelectActionClasses, CDialog)
	//{{AFX_MSG_MAP(CDialogSelectActionClasses)
	ON_BN_CLICKED(IDSHOWALL, OnShowAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogSelectActionClasses message handlers

void CDialogSelectActionClasses::OnShowAll() 
{
    ShowAllActions = TRUE;
    OnOK();	
}
