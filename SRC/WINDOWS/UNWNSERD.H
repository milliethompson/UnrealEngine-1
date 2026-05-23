/*============================================================================
UnWnSerD.h: Dialog to handle the server property page
Used by: Properties dialog to display server page

Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
    This defines the class used to present the server property page.

Revision history:
    * xx/xx/96, Created by Mark
============================================================================*/

/////////////////////////////////////////////////////////////////////////////
// CDialogServerProperties dialog

class CDialogServerProperties : public CDialog
{
// Construction
public:
	CDialogServerProperties(CWnd* pParent = NULL);   // standard constructor
    BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CDialogServerProperties)
	enum { IDD = IDD_SERVER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogServerProperties)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogServerProperties)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
