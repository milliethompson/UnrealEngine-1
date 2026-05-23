/*==========================================================================
FILENAME:     UnWnAudD.cpp
DESCRIPTION:  Implementation of "Audio Properties" dialog panel for
              Unreal "Properties" dialog.
AUTHOR:       Mark Randell, Ammon R. Campbell
COPYRIGHT:    (C) Copyright 1996 Epic MegaGames, Inc.  All rights
              reserved.
NOTICE:       This computer software contains trade secrets and/or
              proprietary information of Epic MegaGames, Inc., and may
              not be disclosed without the express consent of an officer
              of Epic MegaGames, Inc.
==========================================================================*/

/********************************* INCLUDES *****************************/

#include "stdafx.h"
#include "Unreal.h"
#include "unwn.h"
#include "UnWnAudD.h"
#include "UnFGAud.h"

/********************************* CONSTANTS ****************************/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/********************************* VARIABLES ****************************/

	/* NONE */

/********************************* FUNCTIONS ****************************/

/////////////////////////////////////////////////////////////////////////////
// CDialogAudio dialog


CDialogAudio::CDialogAudio(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAudio::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogAudio)
//	SampleRate = -1;
	UseDirectSound = FALSE;
	EnableFilter = FALSE;
	//}}AFX_DATA_INIT
}


void CDialogAudio::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogAudio)
	DDX_Control(pDX, IDC_SOUND_VOLUME, SoundVolume);
	DDX_Control(pDX, IDC_MUSIC_VOLUME, MusicVolume);
//	DDX_CBIndex(pDX, IDC_SAMPLE_RATE_LIST, SampleRate);
	DDX_Check(pDX, IDC_USE_DIRECT_SOUND, UseDirectSound);
	DDX_Check(pDX, IDC_ENABLE_FILTER, EnableFilter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogAudio, CDialog)
	//{{AFX_MSG_MAP(CDialogAudio)
		// NOTE: the ClassWizard will add message map macros here
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogAudio message handlers

//----------------------------------------------------------------------------
//                      Dialog initialization.
//----------------------------------------------------------------------------
BOOL CDialogAudio::OnInitDialog() 
{
    CDialog::OnInitDialog();
    SoundVolume.SetRange(0,MAX_SFX_VOLUME);
    SoundVolume.SetPos( GAudio.SfxVolumeGet() );
    MusicVolume.SetRange(0,MAX_MUSIC_VOLUME);
    MusicVolume.SetPos( GAudio.MusicVolumeGet() );
    UseDirectSound = GAudio.DirectSoundFlagGet() != 0 ? TRUE : FALSE;
    EnableFilter = GAudio.FilterFlagGet() != 0 ? TRUE : FALSE;

//todo: Set sampling rate
//  SamplingRate.??? GAudio.MixingRateGet()

    UpdateData(FALSE);
    return FALSE; // FALSE tells Windows not to set the input focus. 
                  // We do this because the dialog is expected to be part
                  // of a tab group and we don't want to change the focus.
}

//
// CDialogAudio::OnHScroll:
//    Update volume levels dynamically from slider controls
//
void CDialogAudio::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Update sound engine volume levels from slider controls
	GAudio.MusicVolumeSet(MusicVolume.GetPos());
	GAudio.SfxVolumeSet(SoundVolume.GetPos());

	// Allow scrollbar/slider to update	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//----------------------------------------------------------------------------
//                      Check input and accept changes
//----------------------------------------------------------------------------
BOOL CDialogAudio::Accept() 
{
    BOOL Accepted = TRUE; // We always accept the input (it cannot be wrong).
    UpdateData(TRUE);
    GAudio.MusicVolumeSet( MusicVolume.GetPos() );
    GAudio.SfxVolumeSet( SoundVolume.GetPos() );
    GAudio.DirectSoundFlagSet( UseDirectSound );
    GAudio.FilterFlagSet( EnableFilter );

// todo:  Set mixing rate
//    GAudio.MixingRateSet( SampleRate );

    return Accepted;
}


void CDialogAudio::PostNcDestroy() 
{
    CDialog::PostNcDestroy();
    delete this;
}

/*
==========================================================================
End UnWndAudD.cpp
==========================================================================
*/
