/*=============================================================================
	UnPlatfm.cpp: All generic, platform-specific routines-specific routines.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "StdAfx.h"
#include "UnWn.h"
#include "UnPswd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*-----------------------------------------------------------------------------
	CPasswordDlg dialog.
-----------------------------------------------------------------------------*/

CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	guard(CPasswordDlg::CPasswordDlg);
	//{{AFX_DATA_INIT(CPasswordDlg)
	//}}AFX_DATA_INIT
	unguard;
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	guard(CPasswordDlg::DoDataExchange);
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Control(pDX, IDC_PROMPT, m_Prompt);
	DDX_Control(pDX, IDC_PASSWORD, m_Password);
	DDX_Control(pDX, IDC_NAME, m_Name);
	//}}AFX_DATA_MAP
	unguard;
}

BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*-----------------------------------------------------------------------------
	Custom.
-----------------------------------------------------------------------------*/

BOOL CPasswordDlg::OnInitDialog() 
{
	guard(CPasswordDlg::OnInitDialog);
	CDialog::OnInitDialog();

	// Custom initialization.
	SetWindowText(Title);
	m_Name.ReplaceSel(Name);
	m_Password.ReplaceSel(Password);
	m_Prompt.SendMessage(WM_SETTEXT,0,(LPARAM)Prompt);

	m_Name.SetFocus();

	return FALSE;
	unguard;
}

void CPasswordDlg::OnCancel() 
{
	guard(CPasswordDlg::OnCancel);
	CDialog::OnCancel();
	unguard;
}

void CPasswordDlg::OnOK() 
{
	guard(CPasswordDlg::OnOK);
	char T[256]; CString C;

	m_Name.GetLine(0,T);
	C=T; C.TrimLeft(); C.TrimRight(); strcpy(Name,C);

	m_Password.GetLine(0,T);
	C=T; C.TrimLeft(); C.TrimRight(); strcpy(Password,C);

	strcpy(Encryption,"");

	CDialog::OnOK();
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

