/*============================================================================
UnWnPref.h: Preferences property page dialog
Used by: Properties dialog to present preferences

Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
    This is the CDialog-based MFC-generated class which handles the
    preferences.

Revision history:
    * xx/xx/96, Created by Mark
============================================================================*/

/////////////////////////////////////////////////////////////////////////////
// CDialogPreferences dialog

#include "UnPrefer.h"
class CDialogPreferences : public CDialog
{
// Construction
public:
	CDialogPreferences(CWnd* pParent = NULL);   // standard constructor
    BOOL OnInitDialog();

    BOOL Accept(); 
        // If input is valid, save the changes and return TRUE. Otherwise,
        // notify the user of errors and return FALSE.

// Dialog Data
	//{{AFX_DATA(CDialogPreferences)
	enum { IDD = IDD_PROPERTIES_PREFERENCES };
	BOOL	SwitchFromEmptyWeapon;
	BOOL	SwitchToNewWeapon;
	BOOL	ViewFollowsIncline;
	BOOL	MovingViewBobs;
	BOOL	StillViewBobs;
	BOOL	WeaponsSway;
	BOOL	ReverseUpAndDown;
	BOOL	MouseLookAlwaysOn;
	BOOL	RunAlwaysOn;
	BOOL	ViewRolls;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPreferences)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogPreferences)
	afx_msg void OnUseDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void LoadValuesFrom( const FPreferences & Preferences ); // Load local values.
    void SaveValuesInto( FPreferences & Preferences ); // Save local values.
};
