/*============================================================================
UnWnAudD.h: Dialog to handle the audio property page
Used by: Properties dialog to display audio page

Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
    This defines the class used to present the audio property page.

Revision history:
    * xx/xx/96, Created by Mark
============================================================================*/

/////////////////////////////////////////////////////////////////////////////
// CDialogAudio dialog

class CDialogAudio : public CDialog
{
// Construction
public:
	CDialogAudio(CWnd* pParent = NULL);   // standard constructor
    BOOL Accept(); 
    BOOL OnInitDialog();
    void CDialogAudio::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        // If input is valid, save the changes and return TRUE. Otherwise,
        // notify the user of errors and return FALSE.

// Dialog Data
	//{{AFX_DATA(CDialogAudio)
	enum { IDD = IDD_AUDIO };
	CSliderCtrl	SoundVolume;
	CSliderCtrl	MusicVolume;
	int		SampleRate;
	BOOL	UseDirectSound;
	BOOL	EnableFilter;
	BOOL	Enable16Bit;
	BOOL	MuteSound;
	BOOL	MuteMusic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogAudio)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogAudio)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
