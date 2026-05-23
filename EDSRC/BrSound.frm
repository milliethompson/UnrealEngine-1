VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Object = "{BE4F3AC8-AEC9-101A-947B-00DD010F7B46}#1.0#0"; "MSOUTL32.OCX"
Begin VB.Form frmSoundFXBrowser 
   BorderStyle     =   0  'None
   Caption         =   "Sound Browser"
   ClientHeight    =   5910
   ClientLeft      =   6945
   ClientTop       =   2535
   ClientWidth     =   2445
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   5910
   ScaleWidth      =   2445
   ShowInTaskbar   =   0   'False
   Begin VB.PictureBox SoundsHolder 
      Height          =   4935
      Left            =   60
      ScaleHeight     =   4875
      ScaleWidth      =   2295
      TabIndex        =   8
      Top             =   60
      Width           =   2355
      Begin MSOutl.Outline SoundsOutline 
         Height          =   4875
         Left            =   0
         TabIndex        =   9
         Top             =   0
         Width           =   2295
         _Version        =   65536
         _ExtentX        =   4048
         _ExtentY        =   8599
         _StockProps     =   77
         BackColor       =   -2147483643
         BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         BorderStyle     =   0
         Style           =   2
      End
   End
   Begin Threed.SSPanel SoundButtonPanel 
      Height          =   975
      Left            =   0
      TabIndex        =   0
      Top             =   4980
      Width           =   2415
      _Version        =   65536
      _ExtentX        =   4260
      _ExtentY        =   1720
      _StockProps     =   15
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Begin VB.CommandButton SoundEdit 
         Caption         =   "&Edit..."
         Height          =   255
         Left            =   1620
         TabIndex        =   7
         Top             =   660
         Width           =   795
      End
      Begin VB.CommandButton SoundExport 
         Caption         =   "&Export"
         Height          =   255
         Left            =   840
         TabIndex        =   6
         Top             =   660
         Width           =   795
      End
      Begin VB.CommandButton SoundImport 
         Caption         =   "&Import"
         Height          =   255
         Left            =   0
         TabIndex        =   5
         Top             =   660
         Width           =   855
      End
      Begin VB.CommandButton SoundSave 
         Caption         =   "&Save Family"
         Height          =   255
         Left            =   1200
         TabIndex        =   2
         Top             =   420
         Width           =   1215
      End
      Begin VB.CommandButton SoundLoad 
         Caption         =   "&Load Family"
         Height          =   255
         Left            =   0
         TabIndex        =   1
         Top             =   420
         Width           =   1215
      End
      Begin VB.CommandButton SoundRefresh 
         Caption         =   "&Refresh"
         Height          =   255
         Left            =   1200
         TabIndex        =   3
         Top             =   180
         Width           =   1215
      End
      Begin VB.CommandButton SoundTest 
         Caption         =   "&Play"
         Height          =   255
         Left            =   0
         TabIndex        =   4
         Top             =   180
         Width           =   1215
      End
   End
End
Attribute VB_Name = "frmSoundFXBrowser"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'
' Sound Browser: This is a form that implements
' the browser interface.
'
' See ClassBr.frm for an example implementation
'
Option Explicit

'
' Public (Browser Interface)
'

Private Sub Form_Load()
    Call Ed.SetOnTop(Me, "SoundBrowser", TOP_BROWSER)
    SoundButtonPanel.Top = Height - SoundButtonPanel.Height
    SoundsHolder.Height = Height - SoundButtonPanel.Height
    SoundsOutline.Height = SoundsHolder.ScaleHeight
    SoundRefresh_Click
End Sub

Public Function GetCurrent() As String
    GetCurrent = CurrentResource()
End Function

Public Sub BrowserShow()
    Show
End Sub

Public Sub BrowserRefresh()
End Sub

Public Sub BrowserHide()
    Unload Me
End Sub


Private Sub Form_Unload(Cancel As Integer)
    Call Ed.EndOnTop(Me)
End Sub

Private Sub LoadSound_Click()
    Ed.Server.Exec "SOUND LOADBUTTONCLICKED"
End Sub


Private Sub SaveSound_Click()
    Ed.Server.Exec "SOUND SAVEBUTTONCLICKED"
End Sub


Private Sub SoundClose_Click()
    ' The user hit the "close" button in the
    ' sound browser window, so hide the sound
    ' browser window.
    Hide
End Sub

Private Sub SoundDelete_Click()
    Dim resname As String
    
    ' For debugging aid
    Ed.Server.Exec "AUDIO DELETEBUTTONCLICKED"
    
    ' Determine which resource to delete, from sounds outline control
    resname = CurrentResource()
    If (resname = "") Then
        ' No resource selected
        Exit Sub
    End If
    
    ' Ask the user if he/she really wants to delete it
    If MsgBox("Delete sound " & resname & "?", 4 + 32, "Delete a sound") = 6 Then
        ' Delete the resource
        Ed.Server.Exec "AUDIO KILL NAME=" & Quotes(resname)
    End If
    
    ' Update browser controls display
    SoundRefresh_Click
End Sub

Private Sub SoundEdit_Click()
    Dim resname As String
    
    ' For debugging
    Ed.Server.Exec "AUDIO EDITBUTTONCLICKED"
    
    ' get name of currently selected sound resource
    resname = CurrentResource()
    If (resname = "") Then Exit Sub
    
    ' edit the sound
    Ed.Server.Exec "AUDIO EDIT NAME=" & Quotes(resname)

End Sub

Private Sub SoundExport_Click()
    Dim resname As String
    
    ' For debugging
    Ed.Server.Exec "AUDIO EXPORTBUTTONCLICKED"
    
    ' get name of currently selected sound resource
    resname = CurrentResource()
    If (resname = "") Then Exit Sub
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.SoundExportDlg.DialogTitle = "Export sound " & resname
    frmDialogs.SoundExportDlg.filename = resname & ".UFX"
    frmDialogs.SoundExportDlg.DefaultExt = "UFX"
    frmDialogs.SoundExportDlg.ShowSave
    '
    Call UpdateDialog(frmDialogs.SoundExportDlg)
    If frmDialogs.SoundExportDlg.filename <> "" Then
        Ed.Server.Exec "RES EXPORT TYPE=Sound NAME=" & Quotes(resname) & "FILE=" & Quotes(frmDialogs.SoundExportDlg.filename)
    End If
Skip:
    Ed.Server.Enable
End Sub

Private Sub SoundImport_Click()
    Dim Family As String

    ' For debugging
    Ed.Server.Exec "AUDIO IMPORTBUTTONCLICKED"
    
    '
    ' Use dialog for "Import Sound":
    '
    Ed.Server.Disable
    On Error GoTo Skip
    
    frmDialogs.SoundImportDlg.filename = ""
    frmDialogs.SoundImportDlg.ShowOpen
    
    Call UpdateDialog(frmDialogs.SoundImportDlg)
    GString = Trim(frmDialogs.SoundImportDlg.filename)

    If frmSoundImportDlg.SoundFamily = "" Then
        frmSoundImportDlg.SoundFamily = CurrentFamily()
    End If
    If frmSoundImportDlg.SoundFamily = "" Then
        frmSoundImportDlg.SoundFamily = "General"
    End If
    '
    Ed.Server.Disable
    frmSoundImportDlg.Show 1 ' Modal import accept/cancel box
    
    ' Update browser controls display
    SoundRefresh_Click
Skip:
    Ed.Server.Enable
End Sub

Private Sub SoundLoad_Click()
    Dim Fnames As String
    
    ' For debugging aid
    Ed.Server.Exec "AUDIO LOADBUTTONCLICKED"
    
    '
    ' Dialog for "Load Sound Family":
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.SoundLoadFamilyDlg.filename = ""
    frmDialogs.SoundLoadFamilyDlg.ShowOpen
    '
    Call UpdateDialog(frmDialogs.SoundLoadFamilyDlg)
    If (frmDialogs.SoundLoadFamilyDlg.filename <> "") Then
        Screen.MousePointer = 11
        Fnames = Trim(frmDialogs.SoundLoadFamilyDlg.filename)
        While (Fnames <> "")
            Ed.Server.Exec "AUDIO LOADFAMILY FILE=" & Quotes(GrabFname(Fnames))
        Wend
        Screen.MousePointer = 0
    End If
        ' Update broswer controls display
    SoundRefresh_Click
Skip:
    Ed.Server.Enable
End Sub



Private Sub SoundRefresh_Click()
    Dim stmp As String
    Dim resname As String
    Dim Index As Integer
    
    ' For debugging aid
    Ed.Server.Exec "AUDIO REFRESHBUTTONCLICKED"
    
    ' Build entries for sounds browswer outline control
    ' from queries of the unreal engine about sound
    ' resources.
    SoundsOutline.Clear
    Ed.Server.Exec "AUDIO QUERY"
    Index = 0
    Do
        ' Get next audio family in families list
        stmp = Ed.Server.GetProp("Audio", "QueryFam")
        If (stmp <> "" And stmp <> AllString) Then
            ' Add family name to browser outline control
            SoundsOutline.List(Index) = stmp
            SoundsOutline.Indent(Index) = 1
            Index = Index + 1
        
            ' Get audio resources in this family
            Ed.Server.Exec "AUDIO QUERY FAMILY=" & Quotes(stmp)
            Do
                resname = Ed.Server.GetProp("Audio", "QueryAudio")
                If (resname = "") Then Exit Do
                SoundsOutline.List(Index) = resname
                SoundsOutline.Indent(Index) = 2
                Index = Index + 1
            Loop
        End If
    Loop Until stmp = ""

    
    ' Tell Ed that we want a list of all sound resources
'    Ed.Server.Exec "RESOURCE QUERY TYPE=Sound"
    
    ' Loop through the list of sound resources
'    i = 0
'    Do
        ' Get resource name from Ed
'        stmp = Trim(Ed.Server.GetProp("Res", "QueryRes"))
'        If stmp = "" Then Exit Do
'        Debug.Print "<" & stmp & ">"
'        stmp = Trim(Mid(stmp, InStr(stmp, " ")))
'        stmp = Trim(Left(stmp, InStr(stmp, "|") - 1))
        
        ' Add resource name to browser list control
'        SoundsOutline.List(i) = stmp
'        SoundsOutline.Indent(i) = 1
'        i = i + 1
'    Loop
    
    If SoundsOutline.ListCount > 0 Then SoundsOutline.ListIndex = 0
    SoundsOutline.Refresh

End Sub

Private Sub SoundSave_Click()
    Dim Fname As String
    Dim FamilyName As String
    
    ' For debugging aid
    Ed.Server.Exec "AUDIO SAVEBUTTONCLICKED"
    
    ' Get current family name from outline control
    FamilyName = CurrentFamily()
    If FamilyName = "" Then
        ' No family hilited in outline control
        Exit Sub
    End If
        
    ' Build filename from family name
    If InStr(FamilyName, " ") > 0 Then
        Fname = Left(FamilyName, InStr(FamilyName, " ") - 1)
    Else
        Fname = FamilyName
    End If
    frmDialogs.SoundSaveFamilyDlg.filename = Trim(Fname) & ".uax"
    
    ' Use common dialog to get filename from user
    On Error GoTo BadFilename
TryAgain:
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.SoundSaveFamilyDlg.DialogTitle = "Save " & FamilyName & " sound family"
    frmDialogs.SoundSaveFamilyDlg.DefaultExt = "uax"
    frmDialogs.SoundSaveFamilyDlg.ShowSave
    Ed.Server.Enable

    Call UpdateDialog(frmDialogs.SoundSaveFamilyDlg)
    ' Save the resource family, using the name that was
    ' entered in the common dialog.
    On Error GoTo 0
    Fname = frmDialogs.SoundSaveFamilyDlg.filename
    If (Fname <> "") Then
        ' Tell unreal engine to actually do the saving for us.
        Ed.Server.Exec "AUDIO SAVEFAMILY FAMILY=" & Quotes(FamilyName) & " FILE=" & Quotes(Fname)
    End If
    
Skip:
    Ed.Server.Enable
    Exit Sub
    
'
' Bad filename handler:
'
BadFilename:
    Fname = ""
    frmDialogs.SoundSaveFamilyDlg.filename = ""
    On Error GoTo 0
    GoTo TryAgain

End Sub

Private Sub SoundsOutline_Click()
    Ed.Server.Exec "SOUND OUTLINECLICK"
End Sub


Private Sub SoundsOutline_DblClick()
'old:
'   Ed.Server.Exec "SOUND OUTLINEDBLCLICK"

'new:
    Dim resname As String
    
    ' For debugging
    Ed.Server.Exec "AUDIO TESTBUTTONCLICKED"
    
    ' get name of currently selected sound resource
    resname = CurrentResource()
    If (resname = "") Then Exit Sub
    
    ' play the sound
    Ed.Server.Exec "AUDIO TEST NAME=" & Quotes(resname)
End Sub


Private Sub SoundsOutline_Expand(ListIndex As Integer)
    Ed.Server.Exec "SOUND OUTLINEEXPAND"
End Sub


Private Sub SoundsOutline_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    Ed.Server.Exec "SOUND OUTLINEMOUSEDOWN"
End Sub


Private Sub SoundTest_Click()
    Dim resname As String
    
    ' For debugging
    Ed.Server.Exec "AUDIO TESTBUTTONCLICKED"
    
    ' get name of currently selected sound resource
    resname = CurrentResource()
    If (resname = "") Then Exit Sub
    
    ' play the sound
    Ed.Server.Disable
'    Ed.Server.Exec "AUDIO TEST NAME=" & Quotes(resname)
    Ed.BeginSlowTask "Audio Preview"
    Screen.MousePointer = 0
    Ed.Server.SlowExec "AUDIO TEST NAME=" & Quotes(resname)
    Ed.EndSlowTask
    Ed.Server.Enable
    
    ' wait for user
'    MsgBox "Playing " & Quotes(resname), vbOKOnly, "Test Sound Effect"

    ' stop the sound
'    Ed.Server.Exec "AUDIO TESTOFF"
End Sub

' Determine which family is currently hilited in
' the sound browser outline control
Public Function CurrentFamily() As String
    Dim Index As Integer

    Index = SoundsOutline.ListIndex
    While (Index >= 0 And SoundsOutline.Indent(Index) <> 1)
        Index = Index - 1
    Wend
    If (Index < 0) Then
        CurrentFamily = ""
        Exit Function
    End If
    CurrentFamily = SoundsOutline.List(Index)
End Function

Public Function CurrentResource() As String
    Dim Index As Integer

    Index = SoundsOutline.ListIndex
    If (Index < 1) Then
        ' No hilite in list.
        CurrentResource = ""
        Exit Function
    End If
    If (SoundsOutline.Indent(Index) <> 2) Then
        ' Hilite is not on a sound resource (probably on a family instead).
        CurrentResource = ""
        Exit Function
    End If
    ' Caller gets current resource name.
    CurrentResource = SoundsOutline.List(Index)
End Function
