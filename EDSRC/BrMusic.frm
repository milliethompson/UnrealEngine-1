VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form frmMusicBrowser 
   BorderStyle     =   0  'None
   Caption         =   "Music Browser"
   ClientHeight    =   5865
   ClientLeft      =   1305
   ClientTop       =   2820
   ClientWidth     =   2490
   LinkTopic       =   "Form2"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   5865
   ScaleWidth      =   2490
   ShowInTaskbar   =   0   'False
   Begin VB.PictureBox MusicHolder 
      Height          =   4815
      Left            =   60
      ScaleHeight     =   4755
      ScaleWidth      =   2295
      TabIndex        =   8
      Top             =   60
      Width           =   2355
      Begin VB.ListBox SongList 
         Height          =   4755
         IntegralHeight  =   0   'False
         Left            =   0
         TabIndex        =   9
         Top             =   0
         Width           =   2295
      End
   End
   Begin Threed.SSPanel MusicButtonPanel 
      Height          =   915
      Left            =   0
      TabIndex        =   0
      Top             =   4920
      Width           =   2415
      _Version        =   65536
      _ExtentX        =   4260
      _ExtentY        =   1614
      _StockProps     =   15
      BackColor       =   12632256
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Begin VB.CommandButton MusicDelete 
         Caption         =   "&Delete"
         Height          =   255
         Left            =   1620
         TabIndex        =   7
         Top             =   600
         Width           =   735
      End
      Begin VB.CommandButton MusicExport 
         Caption         =   "&Export"
         Height          =   255
         Left            =   900
         TabIndex        =   6
         Top             =   600
         Width           =   735
      End
      Begin VB.CommandButton MusicImport 
         Caption         =   "&Import"
         Height          =   255
         Left            =   60
         TabIndex        =   5
         Top             =   600
         Width           =   855
      End
      Begin VB.CommandButton MusicSave 
         Caption         =   "&Save All"
         Height          =   255
         Left            =   1260
         TabIndex        =   4
         Top             =   360
         Width           =   1095
      End
      Begin VB.CommandButton MusicLoad 
         Caption         =   "&Load"
         Height          =   255
         Left            =   60
         TabIndex        =   3
         Top             =   360
         Width           =   1215
      End
      Begin VB.CommandButton MusicRefresh 
         Caption         =   "&Refresh"
         Height          =   255
         Left            =   1260
         TabIndex        =   2
         Top             =   120
         Width           =   1095
      End
      Begin VB.CommandButton MusicTest 
         Caption         =   "&Play"
         Height          =   255
         Left            =   60
         TabIndex        =   1
         Top             =   120
         Width           =   1215
      End
   End
End
Attribute VB_Name = "frmMusicBrowser"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'
' Music Browser: This is a form that implements
' the browser interface.
'
' See ClassBr.frm for an example implementation
'
Option Explicit




'
' Public (Browser Interface)
'
Private Sub Form_Load()
    Call Ed.SetOnTop(Me, "MusicBrowser", TOP_BROWSER)
    MusicButtonPanel.Top = Height - MusicButtonPanel.Height
    MusicHolder.Height = Height - MusicButtonPanel.Height
    SongList.Height = MusicHolder.ScaleHeight
    MusicRefresh_Click
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call Ed.EndOnTop(Me)
End Sub


Private Sub MusicDelete_Click()
    Dim resname As String
    
    ' For debugging aid
    Ed.Server.Exec "MUSIC DELETEBUTTONCLICKED"
    
    ' Determine which resource to delete, from music listbox control
    resname = CurrentResource()
    If (resname = "") Then
        ' No resource selected
        Exit Sub
    End If
    
    ' Ask the user if he/she really wants to delete it
    If MsgBox("Delete song " & resname & "?", 4 + 32, "Delete a song") = 6 Then
        ' Delete the resource
        Ed.Server.Exec "MUSIC KILL NAME=" & Quotes(resname)
    End If
    
    ' Update browser controls display
    MusicRefresh_Click

End Sub


Private Sub MusicExport_Click()
    Dim resname As String
    
    ' For debugging
    Ed.Server.Exec "MUSIC EXPORTBUTTONCLICKED"
    
    ' get name of currently selected song resource
    resname = CurrentResource()
    If (resname = "") Then Exit Sub
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.MusicExportDlg.DialogTitle = "Export music " & resname
    frmDialogs.MusicExportDlg.filename = resname & ".S3M"
    frmDialogs.MusicExportDlg.DefaultExt = "S3M"
    frmDialogs.MusicExportDlg.ShowSave
    '
    Call UpdateDialog(frmDialogs.MusicExportDlg)
    If frmDialogs.MusicExportDlg.filename <> "" Then
        Ed.Server.Exec "RES EXPORT TYPE=Music NAME=" & Quotes(resname) & "FILE=" & Quotes(frmDialogs.MusicExportDlg.filename)
    End If
Skip:
    Ed.Server.Enable

End Sub


Private Sub MusicImport_Click()
    ' For debugging
    Ed.Server.Exec "MUSIC IMPORTBUTTONCLICKED"
    
    '
    ' Use dialog for "Import Song":
    '
    Ed.Server.Disable
    On Error GoTo Skip
    
    frmDialogs.MusicImportDlg.filename = ""
    frmDialogs.MusicImportDlg.ShowOpen
    
    Call UpdateDialog(frmDialogs.MusicImportDlg)
    GString = Trim(frmDialogs.MusicImportDlg.filename)

    '
    Ed.Server.Disable
    frmMusicImportDlg.Show 1 ' Modal import accept/cancel box
    
    ' Update browser controls display
    MusicRefresh_Click

Skip:
    Ed.Server.Enable

End Sub


Private Sub MusicLoad_Click()
    Dim Fnames As String
    
    ' For debugging aid
    Ed.Server.Exec "MUSIC LOADBUTTONCLICKED"
    
    ' Dialog for "Load Music".
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.MusicLoadDlg.filename = ""
    frmDialogs.MusicLoadDlg.ShowOpen
    
    Call UpdateDialog(frmDialogs.MusicLoadDlg)
    If (frmDialogs.MusicLoadDlg.filename <> "") Then
        Screen.MousePointer = 11
        Fnames = Trim(frmDialogs.MusicLoadDlg.filename)
        While (Fnames <> "")
            Ed.Server.Exec "MUSIC LOAD FILE=" & Quotes(GrabFname(Fnames))
        Wend
        Screen.MousePointer = 0
    End If
        ' Update broswer controls display
    MusicRefresh_Click
Skip:
    Ed.Server.Enable

End Sub



Private Sub MusicRefresh_Click()
    Dim stmp As String
    Dim resname As String
    Dim Index As Integer
    
    ' For debugging aid
    Ed.Server.Exec "MUSIC REFRESHBUTTONCLICKED"
    
    ' Build entries for music browswer listbox control
    ' from queries of the unreal engine about song
    ' resources.
    SongList.Clear
    Ed.Server.Exec "MUSIC QUERY"
    Index = 0
        
    ' Get music resources in this family
    Ed.Server.Exec "MUSIC QUERY"
    Do
        resname = Ed.Server.GetProp("Music", "QueryMusic")
        If (resname = "") Then Exit Do
        SongList.List(Index) = resname
        Index = Index + 1
    Loop

    
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
    
    If SongList.ListCount > 0 Then SongList.ListIndex = 0
    SongList.Refresh


End Sub


Private Sub MusicSave_Click()
    Dim Fname As String
    
    ' For debugging aid
    Ed.Server.Exec "MUSICSAVEBUTTONCLICKED"
    
    ' Build default filename
    Fname = "music"
    frmDialogs.MusicSaveDlg.filename = Trim(Fname) & ".umx"
    
    ' Use common dialog to get filename from user
    On Error GoTo BadFilename
TryAgain:
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.MusicSaveDlg.DialogTitle = "Save Music"
    frmDialogs.MusicSaveDlg.DefaultExt = "umx"
    frmDialogs.MusicSaveDlg.ShowSave
    Ed.Server.Enable

    Call UpdateDialog(frmDialogs.MusicSaveDlg)
    ' Save the resource(s), using the name that was
    ' entered in the common dialog.
    On Error GoTo 0
    Fname = frmDialogs.MusicSaveDlg.filename
    If (Fname <> "") Then
        ' Tell unreal engine to actually do the saving for us.
        Ed.Server.Exec "MUSIC SAVE ALL FILE=" & Quotes(Fname)
    End If
    
Skip:
    Ed.Server.Enable
    Exit Sub
    
'
' Bad filename handler:
'
BadFilename:
    Fname = ""
    frmDialogs.MusicSaveDlg.filename = ""
    On Error GoTo 0
    GoTo TryAgain


End Sub


Private Sub MusicTest_Click()
    Dim resname As String
    
    ' For debugging
    Ed.Server.Exec "MUSIC TESTBUTTONCLICKED"
    
    ' get name of currently selected song resource
    resname = CurrentResource()
    If (resname = "") Then Exit Sub
    
    ' play the music
    Ed.Server.Exec "MUSIC TEST NAME=" & Quotes(resname)
    
    ' wait for user
'    MsgBox "Playing " & Quotes(resname), vbOKOnly, "Test Music"

    ' stop the music
'    Ed.Server.Exec "MUSIC TESTOFF"

End Sub



Private Sub SongList_Click()
    Ed.Server.Exec "MUSIC LISTBOXCLICK"
End Sub



Public Function CurrentResource() As String
    Dim Index As Integer

    Index = SongList.ListIndex
    If (Index < 0) Then
        ' No hilite in list.
        CurrentResource = ""
        Exit Function
    End If
    
    ' Caller gets current resource name.
    CurrentResource = SongList.List(Index)

End Function

Public Function GetCurrent() As String
    GetCurrent = CurrentResource()

End Function

Public Sub BrowserHide()
    Unload Me
End Sub

Public Sub BrowserRefresh()
End Sub

Public Sub BrowserShow()
    Show
End Sub
