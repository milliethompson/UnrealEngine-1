VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form frmTexBrowser 
   BorderStyle     =   0  'None
   Caption         =   "Texture Browser"
   ClientHeight    =   4305
   ClientLeft      =   6045
   ClientTop       =   3900
   ClientWidth     =   2445
   Icon            =   "BrTex.frx":0000
   LinkTopic       =   "Form1"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4305
   ScaleWidth      =   2445
   ShowInTaskbar   =   0   'False
   Begin VB.VScrollBar BrowserScroller 
      Height          =   1650
      Left            =   0
      Max             =   16384
      TabIndex        =   8
      Top             =   720
      Width           =   255
   End
   Begin VB.PictureBox BrowserHolder 
      BackColor       =   &H00C0C0C0&
      Height          =   2415
      Left            =   60
      ScaleHeight     =   157
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   154
      TabIndex        =   7
      Top             =   720
      Width           =   2370
   End
   Begin VB.ComboBox TexSet 
      BackColor       =   &H00C0C0C0&
      Height          =   315
      ItemData        =   "BrTex.frx":030A
      Left            =   60
      List            =   "BrTex.frx":030C
      MousePointer    =   1  'Arrow
      Sorted          =   -1  'True
      Style           =   2  'Dropdown List
      TabIndex        =   0
      Tag             =   "Current texture family"
      Top             =   0
      Width           =   2310
   End
   Begin Threed.SSPanel TexPanel 
      Height          =   735
      Left            =   0
      TabIndex        =   9
      Top             =   3300
      Width           =   2400
      _Version        =   65536
      _ExtentX        =   4233
      _ExtentY        =   1296
      _StockProps     =   15
      ForeColor       =   -2147483640
      BackColor       =   13154717
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "Arial"
         Size            =   9
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   -1  'True
         Strikethrough   =   0   'False
      EndProperty
      Alignment       =   2
      Begin VB.CommandButton BroDelete 
         Caption         =   "&Delete"
         Height          =   255
         Left            =   1560
         TabIndex        =   10
         Tag             =   "Delete this texture"
         Top             =   480
         Width           =   855
      End
      Begin VB.CommandButton BroExport 
         Caption         =   "E&xport"
         Height          =   255
         Left            =   840
         TabIndex        =   11
         Tag             =   "Export textures"
         Top             =   480
         Width           =   735
      End
      Begin VB.CommandButton BroImport 
         Caption         =   "&Import"
         Height          =   255
         Left            =   0
         TabIndex        =   12
         Tag             =   "Import textures"
         Top             =   480
         Width           =   855
      End
      Begin VB.CommandButton BroSaveAll 
         Caption         =   "Save &All"
         Height          =   255
         Left            =   1560
         TabIndex        =   13
         Tag             =   "Save all texture families"
         Top             =   240
         Width           =   855
      End
      Begin VB.CommandButton BroSave 
         Caption         =   "&Save"
         Height          =   255
         Left            =   840
         TabIndex        =   14
         Tag             =   "Save texture families"
         Top             =   240
         Width           =   735
      End
      Begin VB.CommandButton BroLoad 
         Caption         =   "&Load"
         Height          =   255
         Left            =   0
         TabIndex        =   15
         Tag             =   "Load texture families"
         Top             =   240
         Width           =   855
      End
      Begin VB.CommandButton DeleteSet 
         Caption         =   "&Del Set"
         Height          =   255
         Left            =   1560
         TabIndex        =   16
         Tag             =   "Help on texturing"
         Top             =   0
         Width           =   855
      End
      Begin VB.CommandButton BroEdit 
         Caption         =   "&Edit"
         Height          =   255
         Left            =   840
         TabIndex        =   17
         Tag             =   "Edit this texture's properties"
         Top             =   0
         Width           =   735
      End
      Begin VB.CommandButton BroApply 
         Caption         =   "&Apply"
         Height          =   255
         Left            =   0
         TabIndex        =   18
         Tag             =   "Apply this texture to selected polys"
         Top             =   0
         Width           =   855
      End
   End
   Begin Threed.SSRibbon Tex1 
      Height          =   315
      Left            =   825
      TabIndex        =   6
      Tag             =   "View large textures"
      Top             =   360
      Width           =   315
      _Version        =   65536
      _ExtentX        =   556
      _ExtentY        =   556
      _StockProps     =   65
      BackColor       =   12632256
      GroupAllowAllUp =   0   'False
      RoundedCorners  =   0   'False
      BevelWidth      =   1
      PictureUp       =   "BrTex.frx":030E
      PictureDn       =   "BrTex.frx":0420
   End
   Begin Threed.SSRibbon Tex2 
      Height          =   315
      Left            =   1140
      TabIndex        =   5
      Tag             =   "View medium textures"
      Top             =   360
      Width           =   315
      _Version        =   65536
      _ExtentX        =   556
      _ExtentY        =   556
      _StockProps     =   65
      BackColor       =   12632256
      Value           =   -1  'True
      GroupAllowAllUp =   0   'False
      RoundedCorners  =   0   'False
      BevelWidth      =   1
      PictureUp       =   "BrTex.frx":043C
      PictureDn       =   "BrTex.frx":054E
   End
   Begin Threed.SSRibbon Tex4 
      Height          =   315
      Left            =   1455
      TabIndex        =   4
      Tag             =   "View small textures"
      Top             =   360
      Width           =   315
      _Version        =   65536
      _ExtentX        =   556
      _ExtentY        =   556
      _StockProps     =   65
      BackColor       =   12632256
      GroupAllowAllUp =   0   'False
      RoundedCorners  =   0   'False
      BevelWidth      =   1
      PictureUp       =   "BrTex.frx":056A
      PictureDn       =   "BrTex.frx":067C
   End
   Begin Threed.SSRibbon TexSphere 
      Height          =   315
      Left            =   2040
      TabIndex        =   3
      Tag             =   "Show textures as spheres"
      Top             =   360
      Width           =   315
      _Version        =   65536
      _ExtentX        =   556
      _ExtentY        =   556
      _StockProps     =   65
      BackColor       =   12632256
      GroupNumber     =   2
      RoundedCorners  =   0   'False
      BevelWidth      =   1
      PictureUp       =   "BrTex.frx":0698
      PictureDn       =   "BrTex.frx":07AA
   End
   Begin Threed.SSCommand SetPlus 
      Height          =   315
      Left            =   315
      TabIndex        =   2
      Tag             =   "Next texture family"
      Top             =   360
      Width           =   270
      _Version        =   65536
      _ExtentX        =   476
      _ExtentY        =   556
      _StockProps     =   78
      Caption         =   ">"
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      BevelWidth      =   1
      RoundedCorners  =   0   'False
   End
   Begin Threed.SSCommand SetMinus 
      Height          =   315
      Left            =   60
      TabIndex        =   1
      Tag             =   "Previous texture family"
      Top             =   360
      Width           =   270
      _Version        =   65536
      _ExtentX        =   476
      _ExtentY        =   556
      _StockProps     =   78
      Caption         =   "<"
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      BevelWidth      =   1
      RoundedCorners  =   0   'False
   End
End
Attribute VB_Name = "frmTexBrowser"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'
' Texture Browser: This is a form that implements
' the browser interface.
'
Option Explicit

Dim OrigSet As String
Dim OrigTexture As String

'
' Public (Browser Interface)
'

Private Sub Form_Load()
    Call Ed.SetOnTop(Me, "TextureBrowser", TOP_BROWSER)
    TexPanel.Top = Height - TexPanel.Height
    BrowserHolder.Height = Height - BrowserHolder.Top - TexPanel.Height
    BrowserScroller.Height = BrowserHolder.Height - 2 * Screen.TwipsPerPixelY
    RefreshTextureSet ("")
End Sub

Public Sub BrowserShow()
    Show
    DoBrowserCam
End Sub

Public Sub BrowserRefresh()
    DoBrowserCam
End Sub

Public Sub BrowserHide()
End Sub

Public Function GetCurrent() As String
    GetCurrent = Ed.Server.GetProp("ED", "CURTEX")
End Function

'
' Private
'

Private Sub BroLoad_Click()
    Dim Fnames As String, Temp As String
    
    ' Dialog for "Load Texture Set".
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.TexFamLoad.filename = ""
    frmDialogs.TexFamLoad.ShowOpen
    '
    Call UpdateDialog(frmDialogs.TexFamLoad)
    If (frmDialogs.TexFamLoad.filename <> "") Then
        Screen.MousePointer = 11
        Fnames = Trim(frmDialogs.TexFamLoad.filename)
        While (Fnames <> "")
            Temp = GrabFname(Fnames)
            Ed.Server.Exec "TEXTURE LOADSET FILE=" & Quotes(Temp)
        Wend
        While InStr(Temp, "\")
            Temp = Mid(Temp, InStr(Temp, "\") + 1)
        Wend
        If InStr(Temp, ".") Then Temp = Left(Temp, InStr(Temp, ".") - 1)
        RefreshTextureSet (Temp)
        Screen.MousePointer = 0
    End If
Skip:
    Ed.Server.Enable
End Sub

Private Sub BroSaveAll_Click()
    Dim Fname As String
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.TexFamSave.DialogTitle = "Save all texture families"
    frmDialogs.TexFamSave.ShowSave
    '
    Call UpdateDialog(frmDialogs.TexFamSave)
    On Error GoTo 0
    '
    Fname = frmDialogs.TexFamSave.filename
    If Fname <> "" Then
        Ed.Server.Exec "TEXTURE SAVESET ALL FILE=" & Quotes(Fname)
    End If
Skip:
    Ed.Server.Enable
End Sub

Private Sub BroSave_Click()
    '
    Dim Fname As String
    Dim SetName As String
    '
    ' Find Set name based on cursor position
    '
    SetName = CurTexSet()
    '
    If SetName = "" Then
        SaveTexFamError
    Else
        If InStr(SetName, " ") > 0 Then
            Fname = Left(SetName, InStr(SetName, " ") - 1)
        Else
            Fname = SetName
        End If
        '
        frmDialogs.TexFamSave.filename = Trim(Fname) & ".utx"
        '
        On Error GoTo BadFilename
        '
TryAgain:
        '
        On Error GoTo Skip
        Ed.Server.Disable
        frmDialogs.TexFamSave.DialogTitle = "Save " & SetName & " texture Set"
        frmDialogs.TexFamSave.ShowSave
        Ed.Server.Enable
        '
        Call UpdateDialog(frmDialogs.TexFamSave)
        On Error GoTo 0
        '
        Fname = frmDialogs.TexFamSave.filename
        '
        If (Fname <> "") Then
            Ed.Server.Exec "TEXTURE SAVESET Set=" & Quotes(SetName) & " FILE=" & Quotes(Fname)
        End If
    End If
Skip:
    Ed.Server.Enable
    Exit Sub
    '
    ' Bad filename handler:
    '
BadFilename:
    '
    Fname = ""
    frmDialogs.TexFamSave.filename = ""
    On Error GoTo 0
    GoTo TryAgain
    '
End Sub

Private Sub BroImport_Click()
    Dim SetName As String

    ' Dialog for "Import Texture".
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.TexImport.filename = ""
    frmDialogs.TexImport.ShowOpen
    
    Call UpdateDialog(frmDialogs.TexImport)
    GString = Trim(frmDialogs.TexImport.filename)
    
    frmTexImport.TexSet = CurTexSet()
    If frmTexImport.TexSet = "" Or frmTexImport.TexSet = AllString Then
        frmTexImport.TexSet = "Untitled"
    End If
    
    ' Modal import accept/cancel box.
    frmTexImport.Show 1
    If GlobalAbortedModal = 0 Then RefreshTextureSet (frmTexImport.TexSet)

Skip:
    Ed.Server.Enable
End Sub

Public Sub BroDelete_Click()
    If MsgBox("Delete texture " & frmMain.TextureCombo.List(0) & "?", 4 + 32, "Delete a texture") = 6 Then
        Ed.Server.Exec "TEXTURE KILL NAME=" & Quotes(frmMain.TextureCombo.List(0))
        RefreshTextureSet ("")
    End If
End Sub

Public Sub BroExport_Click()
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.ExportTex.DialogTitle = "Export texture " & frmMain.TextureCombo.List(0)
    frmDialogs.ExportTex.ShowSave
    '
    Call UpdateDialog(frmDialogs.ExportTex)
    If frmDialogs.ExportTex.filename <> "" Then
        Ed.Server.Exec "RES EXPORT TYPE=TEXTURE NAME=" & Quotes(frmMain.TextureCombo.List(0)) & "FILE=" & Quotes(frmDialogs.ExportTex.filename)
    End If
Skip: Ed.Server.Enable
End Sub

Private Sub RefreshTextureSet(FamName As String)
    Dim Result As String
    Dim Found As Integer
    Dim All As String
    Dim i As Integer
    Dim S As String
    '
    If FamName <> "" Then
        OrigSet = FamName
    ElseIf TexSet.ListIndex >= 0 Then
        OrigSet = TexSet.List(TexSet.ListIndex)
    Else
        OrigSet = ""
    End If
    '
    All = Ed.Server.GetProp("Array", "TextureSets")
    '
    TexSet.Clear
    TexSet.AddItem AllString
    While All <> ""
        S = GrabCommaString(All)
        If Left(S, 1) <> "!" Or TexSphere.Value Then
            TexSet.AddItem S
        End If
    Wend
    '
    If TexSet.ListCount = 0 Then
        TexSet.AddItem NoneString
    Else
        ' Set.
        For i = 0 To TexSet.ListCount - 1
            If TexSet.List(i) = OrigSet Then
                TexSet.ListIndex = i
                Found = 1
            End If
        Next i
        If Found = 0 Then
            If TexSet.ListCount = 1 Then
                TexSet.ListIndex = 0
            Else
                TexSet.ListIndex = 1
            End If
        End If
        DoBrowserCam
    End If
End Sub

Private Function CurTexSet() As String
    '
    Dim Temp As String
    '
    If TexSet.ListIndex < 0 Then
        CurTexSet = ""
    ElseIf TexSet.List(TexSet.ListIndex) = NoneString Then
        CurTexSet = ""
    Else
        CurTexSet = Trim(TexSet.List(TexSet.ListIndex))
    End If
    '
End Function

Public Sub BroApply_Click()
    Ed.Server.Exec "POLY SET TEXTURE=" & frmMain.TextureCombo.List(0)
    Ed.Server.Exec "POLY DEFAULT TEXTURE=" & frmMain.TextureCombo.List(0)
End Sub

Public Sub BrowserEdit_Click()
    '
End Sub

Private Sub SaveTexFamError()
    MsgBox "You must position the cursor on a texture Set name first", 16
End Sub

Private Sub TexSet_Click()
    BrowserScroller.Value = 0
    DoBrowserCam
End Sub

Private Sub BrowserScroller_Scroll()
    DoBrowserCam
End Sub

Private Sub BrowserScroller_Change()
    DoBrowserCam
    TexSet.SetFocus
End Sub

Public Sub BroEdit_Click()
    frmTexProp.SetTexture frmMain.TextureCombo.Text
    frmTexProp.Show
End Sub

Sub DoBrowserCam()
    Dim Size As Integer
    Dim Temp As Long
    '
    If Tex1.Value Then
        Size = 128
    ElseIf Tex2.Value Then
        Size = 64
    ElseIf Tex4.Value Then
        Size = 32
    Else
        Size = 0
    End If
    '
    If (Size = 0) Then
        BrowserScroller.Visible = False
        Ed.Server.Exec "CAMERA CLOSE NAME=BrowserCam"
    Else
        Ed.Server.Exec "CAMERA OPEN X=10 Y=0" & _
            " XR=" & Trim(Str(BrowserHolder.ScaleWidth - 10)) & _
            " YR=" & Trim(Str(BrowserHolder.ScaleHeight)) & _
            " FLAGS=" & Trim(Str(SHOW_NORMAL + SHOW_AS_CHILD + SHOW_NOBUTTONS + SHOW_NOCAPTURE)) & _
            " HWND=" & Trim(Str(BrowserHolder.hwnd)) & _
            " MISC1=" & Trim(Str(Size)) & _
            " MISC2=" & Trim(Str(BrowserScroller.Value)) & _
            " REN=" & Trim(Str(REN_TEXBROWSER)) & _
            " NAME=BrowserCam SET=" & Quotes(TexSet.List(TexSet.ListIndex))
        Temp = Val(Ed.Server.GetProp("ED", "LASTSCROLL"))
        '
        If (Temp <= 32767) Then
            BrowserScroller.Max = Temp
        Else
            BrowserScroller.Max = 32767
        End If
        '
        BrowserScroller.LargeChange = 512
        BrowserScroller.SmallChange = 64
        BrowserScroller.Visible = True
    End If
End Sub

Private Sub SetMinus_Click()
    If TexSet.ListIndex = 0 Then
        TexSet.ListIndex = TexSet.ListCount - 1
    Else
        TexSet.ListIndex = TexSet.ListIndex - 1
    End If
End Sub

Private Sub SetPlus_Click()
    If TexSet.ListIndex = TexSet.ListCount - 1 Then
        TexSet.ListIndex = 0
    Else
        TexSet.ListIndex = TexSet.ListIndex + 1
    End If
End Sub

Private Sub Tex1_Click(Value As Integer)
    DoBrowserCam
End Sub

Private Sub Tex2_Click(Value As Integer)
    DoBrowserCam
End Sub

Private Sub Tex4_Click(Value As Integer)
    DoBrowserCam
End Sub

Private Sub TexSphere_Click(Value As Integer)
    RefreshTextureSet ("")
End Sub

Private Sub TexText_Click(Value As Integer)
    DoBrowserCam
End Sub

Public Sub TextureList_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If Button = 2 Then
        frmPopups.TBProperties.Caption = "&Edit " & frmMain.TextureCombo.List(0) & " properties..."
        PopupMenu frmPopups.TexBrowser
    End If
End Sub

Private Sub DeleteSet_Click()
    If CurTexSet() <> AllString Then
        Ed.Server.Exec "Texture Kill Set=" & CurTexSet()
        RefreshTextureSet ("")
    End If
End Sub


