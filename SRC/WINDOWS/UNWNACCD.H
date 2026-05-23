/*============================================================================
UnWnAccD.h: Action dialog selection dialog
Used by: Action dialog for selection which actions to show

Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
    This is the class for a small dialog which lets the user
    select the kinds of actions to show on the actions property
    page.

Revision history:
    * xx/xx/96, Created by Mark
============================================================================*/

/////////////////////////////////////////////////////////////////////////////
// CDialogSelectActionClasses dialog

class CDialogSelectActionClasses : public CDialog
{
// Construction
public:
	CDialogSelectActionClasses(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogSelectActionClasses)
	enum { IDD = IDD_ACTION_SELECTION };
	BOOL	SelectAdministrativeActions;
	BOOL	SelectAdvancedActions;
	BOOL	SelectMiscellaneousActions;
	BOOL	SelectMoveActions;
	BOOL	SelectTurnActions;
	BOOL	SelectWeaponActions;
	//}}AFX_DATA

    BOOL ShowAllActions; // When the dialog is done, this is TRUE if all actions are to be
                         // shown, FALSE if selected actions are to be shown.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogSelectActionClasses)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogSelectActionClasses)
	afx_msg void OnShowAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
