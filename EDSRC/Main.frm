VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Object = "{27395F88-0C0C-101B-A3C9-08002B2F49FB}#1.1#0"; "picclp32.ocx"
Begin VB.MDIForm frmMain 
   AutoShowChildren=   0   'False
   BackColor       =   &H00000000&
   Caption         =   "UnrealEd"
   ClientHeight    =   8130
   ClientLeft      =   2085
   ClientTop       =   1845
   ClientWidth     =   11865
   Icon            =   "Main.frx":0000
   LinkTopic       =   "Form1"
   Moveable        =   0   'False
   ScrollBars      =   0   'False
   Visible         =   0   'False
   WindowState     =   2  'Maximized
   Begin Threed.SSPanel MainBar 
      Align           =   1  'Align Top
      Height          =   435
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Visible         =   0   'False
      Width           =   11865
      _Version        =   65536
      _ExtentX        =   20929
      _ExtentY        =   767
      _StockProps     =   15
      ForeColor       =   16777215
      BackColor       =   -2147483633
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      BorderWidth     =   0
      FloodColor      =   0
      ShadowColor     =   1
      Begin VB.TextBox Callback 
         Height          =   345
         Left            =   870
         MultiLine       =   -1  'True
         TabIndex        =   22
         Text            =   "Main.frx":030A
         Top             =   45
         Visible         =   0   'False
         Width           =   345
      End
      Begin VB.ComboBox CalcText 
         BackColor       =   &H00C0C0C0&
         Height          =   315
         HelpContextID   =   328
         Left            =   6840
         TabIndex        =   16
         TabStop         =   0   'False
         Tag             =   "Enter calculation here, ex. 1+2*3"
         Text            =   "0"
         Top             =   3300
         Width           =   2265
      End
      Begin VB.ComboBox ModeCombo 
         BackColor       =   &H00C0C0C0&
         Height          =   315
         HelpContextID   =   111
         Left            =   6480
         Style           =   2  'Dropdown List
         TabIndex        =   12
         TabStop         =   0   'False
         Tag             =   "Editing Mode"
         Top             =   60
         Width           =   2055
      End
      Begin VB.Timer Timer 
         Interval        =   60000
         Left            =   390
         Top             =   0
      End
      Begin VB.ComboBox TextureCombo 
         BackColor       =   &H00C0C0C0&
         Height          =   315
         HelpContextID   =   123
         Left            =   13440
         Style           =   2  'Dropdown List
         TabIndex        =   7
         TabStop         =   0   'False
         Tag             =   "Current texture"
         Top             =   60
         Width           =   1815
      End
      Begin VB.ComboBox GridCombo 
         BackColor       =   &H00C0C0C0&
         Height          =   315
         HelpContextID   =   111
         Left            =   9000
         Style           =   2  'Dropdown List
         TabIndex        =   5
         TabStop         =   0   'False
         Tag             =   "Grid Size"
         Top             =   60
         Width           =   855
      End
      Begin VB.ComboBox ActorCombo 
         BackColor       =   &H00C0C0C0&
         Height          =   315
         Left            =   10920
         Style           =   2  'Dropdown List
         TabIndex        =   2
         TabStop         =   0   'False
         Tag             =   "Actor class for adding new actors"
         Top             =   60
         Width           =   1695
      End
      Begin Threed.SSCommand CalcZero 
         Height          =   285
         HelpContextID   =   328
         Left            =   2715
         TabIndex        =   15
         Tag             =   "Zero the calculator value"
         Top             =   75
         Width           =   210
         _Version        =   65536
         _ExtentX        =   370
         _ExtentY        =   503
         _StockProps     =   78
         Caption         =   "0"
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
      Begin Threed.SSCommand CalcButton 
         Default         =   -1  'True
         Height          =   285
         HelpContextID   =   328
         Left            =   3000
         TabIndex        =   14
         Tag             =   "Perform calculation"
         Top             =   75
         Width           =   495
         _Version        =   65536
         _ExtentX        =   873
         _ExtentY        =   503
         _StockProps     =   78
         Caption         =   "Calc"
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
      Begin Threed.SSCommand HelpButton 
         Height          =   285
         Left            =   1080
         TabIndex        =   11
         Tag             =   "Help"
         Top             =   75
         Width           =   615
         _Version        =   65536
         _ExtentX        =   1085
         _ExtentY        =   503
         _StockProps     =   78
         Caption         =   "Help"
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
         AutoSize        =   1
      End
      Begin Threed.SSCommand EpicButton 
         Height          =   285
         Left            =   1800
         TabIndex        =   10
         Tag             =   "Visit Epic's Web page"
         Top             =   75
         Width           =   615
         _Version        =   65536
         _ExtentX        =   1085
         _ExtentY        =   503
         _StockProps     =   78
         Caption         =   "Epic"
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
      Begin Threed.SSCommand RedoButton 
         Height          =   285
         Left            =   600
         TabIndex        =   9
         Tag             =   "Redo"
         Top             =   75
         Width           =   375
         _Version        =   65536
         _ExtentX        =   661
         _ExtentY        =   503
         _StockProps     =   78
         Caption         =   ">>"
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
      Begin Threed.SSCommand UndoButton 
         Height          =   285
         Left            =   120
         TabIndex        =   8
         Tag             =   "Undo"
         Top             =   75
         Width           =   375
         _Version        =   65536
         _ExtentX        =   661
         _ExtentY        =   503
         _StockProps     =   78
         Caption         =   "<<"
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
      Begin VB.Label ModeLabel 
         Alignment       =   1  'Right Justify
         BackStyle       =   0  'Transparent
         Caption         =   "Mode: "
         Height          =   210
         Left            =   5880
         TabIndex        =   13
         Top             =   105
         Width           =   615
      End
      Begin VB.Label TextureLabel 
         Alignment       =   1  'Right Justify
         Caption         =   "Texture: "
         Height          =   255
         Left            =   12600
         TabIndex        =   6
         Top             =   120
         Width           =   855
      End
      Begin VB.Label GridLabel 
         Alignment       =   1  'Right Justify
         Caption         =   "Grid: "
         Height          =   210
         Left            =   8520
         TabIndex        =   4
         Top             =   105
         Width           =   495
      End
      Begin VB.Label ActorLabel 
         Alignment       =   1  'Right Justify
         Caption         =   "Actor Class: "
         Height          =   195
         Left            =   9960
         TabIndex        =   3
         Top             =   105
         Width           =   975
      End
   End
   Begin Threed.SSPanel Toolbar 
      Align           =   3  'Align Left
      Height          =   7695
      Left            =   0
      TabIndex        =   17
      Top             =   435
      Visible         =   0   'False
      Width           =   2070
      _Version        =   65536
      _ExtentX        =   3651
      _ExtentY        =   13573
      _StockProps     =   15
      BackColor       =   -2147483633
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Begin VB.PictureBox Holder 
         BackColor       =   &H00808080&
         Height          =   4695
         Left            =   0
         ScaleHeight     =   4635
         ScaleWidth      =   1755
         TabIndex        =   19
         Top             =   1080
         Width           =   1815
         Begin Threed.SSRibbon ToolIcons 
            Height          =   615
            Index           =   0
            Left            =   0
            TabIndex        =   20
            Top             =   0
            Visible         =   0   'False
            Width           =   615
            _Version        =   65536
            _ExtentX        =   1085
            _ExtentY        =   1085
            _StockProps     =   65
            BackColor       =   8421504
            PictureDnChange =   0
            RoundedCorners  =   0   'False
            BevelWidth      =   0
            Outline         =   0   'False
         End
         Begin VB.Label StatusText 
            Alignment       =   2  'Center
            BackStyle       =   0  'Transparent
            Caption         =   "Status"
            Height          =   495
            Left            =   0
            TabIndex        =   21
            Top             =   600
            Width           =   1695
         End
      End
      Begin VB.VScrollBar Scroller 
         Height          =   5775
         Left            =   1800
         TabIndex        =   18
         Top             =   0
         Visible         =   0   'False
         Width           =   245
      End
   End
   Begin Threed.SSPanel BrowserPanel 
      Align           =   4  'Align Right
      Height          =   7695
      Left            =   9435
      TabIndex        =   0
      Top             =   435
      Visible         =   0   'False
      Width           =   2430
      _Version        =   65536
      _ExtentX        =   4286
      _ExtentY        =   13573
      _StockProps     =   15
      BackColor       =   -2147483633
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      BorderWidth     =   0
      Begin VB.ComboBox BrowserTopicCombo 
         BackColor       =   &H00C0C0C0&
         Height          =   315
         Left            =   810
         Style           =   2  'Dropdown List
         TabIndex        =   23
         Tag             =   "Various resources you can browse"
         Top             =   45
         Width           =   1605
      End
      Begin VB.Label Label1 
         Caption         =   "Browse"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   9
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   -1  'True
            Strikethrough   =   0   'False
         EndProperty
         Height          =   300
         Left            =   75
         TabIndex        =   24
         Top             =   75
         Width           =   735
      End
   End
   Begin PicClip.PictureClip Pics 
      Left            =   2160
      Top             =   600
      _ExtentX        =   7408
      _ExtentY        =   7408
      _Version        =   327680
      Rows            =   8
      Cols            =   8
      Picture         =   "Main.frx":0313
   End
   Begin PicClip.PictureClip HiPics 
      Left            =   2820
      Top             =   1020
      _ExtentX        =   7408
      _ExtentY        =   7408
      _Version        =   327680
      Rows            =   8
      Cols            =   8
      Picture         =   "Main.frx":139A5
   End
   Begin VB.Menu File 
      Caption         =   "&File"
      Begin VB.Menu New 
         Caption         =   "&New level"
      End
      Begin VB.Menu Open 
         Caption         =   "&Open level"
         Shortcut        =   ^O
      End
      Begin VB.Menu Save 
         Caption         =   "&Save level"
         Shortcut        =   ^L
      End
      Begin VB.Menu SaveAs 
         Caption         =   "Save &As..."
         Shortcut        =   ^E
      End
      Begin VB.Menu ZWOOP 
         Caption         =   "-"
      End
      Begin VB.Menu PlayLevel 
         Caption         =   "&Play level"
         Shortcut        =   ^P
      End
      Begin VB.Menu X 
         Caption         =   "-"
      End
      Begin VB.Menu ImportLevel 
         Caption         =   "&Import Level"
      End
      Begin VB.Menu ExportLevel 
         Caption         =   "&Export level"
      End
      Begin VB.Menu ZSTOS 
         Caption         =   "-"
      End
      Begin VB.Menu Exit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu Edit 
      Caption         =   "&Edit"
      Begin VB.Menu EditCut 
         Caption         =   "Cu&t"
         Visible         =   0   'False
      End
      Begin VB.Menu EditCopy 
         Caption         =   "&Copy"
         Visible         =   0   'False
      End
      Begin VB.Menu EditPaste 
         Caption         =   "&Paste"
         Visible         =   0   'False
      End
      Begin VB.Menu EditDivider1 
         Caption         =   "-"
         Visible         =   0   'False
      End
      Begin VB.Menu EditUndo 
         Caption         =   "&Undo"
         Shortcut        =   ^Z
      End
      Begin VB.Menu EditRedo 
         Caption         =   "&Redo"
         Shortcut        =   ^R
      End
      Begin VB.Menu ZBORE 
         Caption         =   "-"
      End
      Begin VB.Menu EditFind 
         Caption         =   "&Find"
         Shortcut        =   ^F
         Visible         =   0   'False
      End
      Begin VB.Menu EditFindNext 
         Caption         =   "Find &Next"
         Shortcut        =   {F3}
         Visible         =   0   'False
      End
      Begin VB.Menu EditDivider2 
         Caption         =   "-"
         Visible         =   0   'False
      End
      Begin VB.Menu Duplicate 
         Caption         =   "&Duplicate"
         Shortcut        =   ^W
      End
      Begin VB.Menu Delete 
         Caption         =   "Delete"
      End
      Begin VB.Menu XXX 
         Caption         =   "-"
      End
      Begin VB.Menu SelectNone 
         Caption         =   "Select &None"
      End
      Begin VB.Menu SelectAll 
         Caption         =   "Select &All"
      End
      Begin VB.Menu SelectDialog 
         Caption         =   "Select Polys..."
         Begin VB.Menu SelMatchGroups 
            Caption         =   "Matching Groups (Shift-G)"
         End
         Begin VB.Menu SelMatchItems 
            Caption         =   "Matching Items (Shift-I)"
         End
         Begin VB.Menu SelMatchBrush 
            Caption         =   "Matching Brush (Shift-B)"
         End
         Begin VB.Menu SelMatchTex 
            Caption         =   "Matching Texture (Shift-T)"
         End
         Begin VB.Menu WZYRA 
            Caption         =   "-"
         End
         Begin VB.Menu SelAllAdj 
            Caption         =   "All Adjacents (Shift-J)"
         End
         Begin VB.Menu SelCoplAdj 
            Caption         =   "Adjacent Coplanars (Shift-C)"
         End
         Begin VB.Menu SelAdjWalls 
            Caption         =   "Adjacent Walls (Shift-W)"
         End
         Begin VB.Menu SelAdjFloors 
            Caption         =   "Adjacent Floors/Ceils (Shift-F)"
         End
         Begin VB.Menu SelAdjSlants 
            Caption         =   "Adjacent Slants (Shift-S)"
         End
         Begin VB.Menu ZIJWZ 
            Caption         =   "-"
         End
         Begin VB.Menu SelReverse 
            Caption         =   "Reverse (Shift-Q)"
         End
         Begin VB.Menu WIJQZA 
            Caption         =   "-"
         End
         Begin VB.Menu SelMemorize 
            Caption         =   "Memorize Set (Shift-M)"
         End
         Begin VB.Menu SelRecall 
            Caption         =   "Recall Memory (Shift-R)"
         End
         Begin VB.Menu SelIntersection 
            Caption         =   "Or with Memory (Shift-O)"
         End
         Begin VB.Menu SelUnion 
            Caption         =   "And with Memory (Shift-U)"
         End
         Begin VB.Menu SelXor 
            Caption         =   "Xor with Memory (Shift-X)"
         End
      End
      Begin VB.Menu ZEDITZ 
         Caption         =   "-"
      End
      Begin VB.Menu MapEditMode 
         Caption         =   "&Map Edit Mode"
      End
   End
   Begin VB.Menu ScriptMenu 
      Caption         =   "&Script"
      Visible         =   0   'False
      Begin VB.Menu ScriptCompile 
         Caption         =   "&Compile Script"
         Shortcut        =   {F5}
      End
      Begin VB.Menu ScriptMakeChanged 
         Caption         =   "&Make Changed Scripts"
         Shortcut        =   {F6}
      End
      Begin VB.Menu ScriptMakeAll 
         Caption         =   "Make &All Scripts"
         Shortcut        =   {F7}
      End
      Begin VB.Menu ZDAFFODIL 
         Caption         =   "-"
      End
      Begin VB.Menu ScriptNextError 
         Caption         =   "&Next Error"
         Shortcut        =   {F4}
      End
      Begin VB.Menu ScriptResults 
         Caption         =   "&Results"
         Shortcut        =   {F8}
      End
      Begin VB.Menu ScriptEditDefaults 
         Caption         =   "&Edit Default Actor Properties"
         Shortcut        =   {F9}
      End
   End
   Begin VB.Menu Brush 
      Caption         =   "&Brush"
      Begin VB.Menu BrushAdd 
         Caption         =   "&Add"
         Shortcut        =   ^A
      End
      Begin VB.Menu BrushSubtract 
         Caption         =   "&Subtract"
         Shortcut        =   ^S
      End
      Begin VB.Menu BrushIntersect 
         Caption         =   "&Intersect"
         Shortcut        =   ^N
      End
      Begin VB.Menu BrushDeintersect 
         Caption         =   "&Deintersect"
         Shortcut        =   ^D
      End
      Begin VB.Menu AddMovableBrush 
         Caption         =   "&Add Movable Brush"
      End
      Begin VB.Menu AddSpecial 
         Caption         =   "&Add Special..."
      End
      Begin VB.Menu ZRK 
         Caption         =   "-"
      End
      Begin VB.Menu ParametricSolids 
         Caption         =   "&Parametric solids"
         Begin VB.Menu ParSolRect 
            Caption         =   "&Rectangle"
         End
         Begin VB.Menu ParSolTube 
            Caption         =   "Cyllinder/Tube"
         End
         Begin VB.Menu ParSolCone 
            Caption         =   "Cone/Spire"
         End
         Begin VB.Menu ParSolLinearStair 
            Caption         =   "Linear Staircase"
         End
         Begin VB.Menu ParSolSpiralStair 
            Caption         =   "Spiral Staircase"
         End
         Begin VB.Menu CurvedStair 
            Caption         =   "Curved Staircase"
         End
         Begin VB.Menu ParSolSphereDome 
            Caption         =   "Sphere/Dome"
         End
         Begin VB.Menu ParSolHeightMap 
            Caption         =   "Height Map"
            Visible         =   0   'False
         End
      End
      Begin VB.Menu ZGYM 
         Caption         =   "-"
      End
      Begin VB.Menu Resize 
         Caption         =   "&Resize/Move..."
      End
      Begin VB.Menu BrushReset 
         Caption         =   "R&eset"
         Begin VB.Menu ResetRotation 
            Caption         =   "&Rotation"
         End
         Begin VB.Menu ResetScale 
            Caption         =   "&Scale"
         End
         Begin VB.Menu ResetPosition 
            Caption         =   "&Position"
         End
         Begin VB.Menu ZYCLUNT 
            Caption         =   "-"
         End
         Begin VB.Menu ResetAll 
            Caption         =   "&All"
         End
      End
      Begin VB.Menu ZZZ 
         Caption         =   "-"
      End
      Begin VB.Menu LoadBrush 
         Caption         =   "&Load"
         Shortcut        =   ^B
      End
      Begin VB.Menu SaveBrush 
         Caption         =   "&Save"
      End
      Begin VB.Menu SaveBrushAs 
         Caption         =   "Sa&ve As..."
      End
      Begin VB.Menu XCYZ 
         Caption         =   "-"
      End
      Begin VB.Menu BrushImport 
         Caption         =   "&Import..."
      End
      Begin VB.Menu BrushExport 
         Caption         =   "&Export..."
      End
      Begin VB.Menu BrushHull 
         Caption         =   "&Advanced.."
         Visible         =   0   'False
         Begin VB.Menu AddCutaway 
            Caption         =   "&Cutaway Zone"
         End
         Begin VB.Menu BrushSliceTex 
            Caption         =   "No-&Terrain Zone"
         End
         Begin VB.Menu AddNoCut 
            Caption         =   "&No-Cut Zone"
         End
      End
   End
   Begin VB.Menu Camera 
      Caption         =   "&Camera"
      Begin VB.Menu CamAllViews 
         Caption         =   "&All Views"
      End
      Begin VB.Menu CamTwoViews 
         Caption         =   "&Persp + Overhead"
      End
      Begin VB.Menu CamPersp 
         Caption         =   "P&ersp Only"
      End
      Begin VB.Menu CamOvh 
         Caption         =   "&Overhead Only"
      End
      Begin VB.Menu ZILBERT 
         Caption         =   "-"
      End
      Begin VB.Menu CamOpenFree 
         Caption         =   "&Open Free Camera"
      End
      Begin VB.Menu CamCloseAllFree 
         Caption         =   "&Close All Free Cameras"
      End
      Begin VB.Menu CameraResetAll 
         Caption         =   "&Reset All"
      End
   End
   Begin VB.Menu Options 
      Caption         =   "&Options"
      Begin VB.Menu Project 
         Caption         =   "&Level"
      End
      Begin VB.Menu Rebuild 
         Caption         =   "&Rebuild..."
      End
      Begin VB.Menu Preferences 
         Caption         =   "&Preferences"
      End
      Begin VB.Menu ValidateLevel 
         Caption         =   "&Validate Level"
      End
      Begin VB.Menu ViewLevelLinks 
         Caption         =   "&Show Links"
      End
   End
   Begin VB.Menu WIndow 
      Caption         =   "&Window"
      Begin VB.Menu MeshViewer 
         Caption         =   "&Mesh Viewer"
      End
      Begin VB.Menu TwoDee 
         Caption         =   "&2D Shape Editor"
      End
      Begin VB.Menu FloorLofter 
         Caption         =   "Floor Lofter (Experimental)"
      End
      Begin VB.Menu WorldBrowser 
         Caption         =   "Resource &Browser"
      End
      Begin VB.Menu WindowLog 
         Caption         =   "&Log"
      End
      Begin VB.Menu ZFUS 
         Caption         =   "-"
      End
      Begin VB.Menu WinToolbar 
         Caption         =   "&Toolbar"
         Begin VB.Menu WinToolbarLeft 
            Caption         =   "&Left"
         End
         Begin VB.Menu WinToolbarRight 
            Caption         =   "&Right"
         End
      End
      Begin VB.Menu WinPanel 
         Caption         =   "&Panel"
         Visible         =   0   'False
         Begin VB.Menu WinPanelBottom 
            Caption         =   "&Bottom"
         End
         Begin VB.Menu WinPanelTop 
            Caption         =   "&Top"
         End
         Begin VB.Menu WinPanelHide 
            Caption         =   "&Hide"
         End
      End
      Begin VB.Menu WinBrowser 
         Caption         =   "&Browser"
         Begin VB.Menu WinBrowserRight 
            Caption         =   "&Right"
         End
         Begin VB.Menu WinBrowserLeft 
            Caption         =   "&Left"
         End
         Begin VB.Menu WinBrowserHide 
            Caption         =   "&Hide"
         End
      End
   End
   Begin VB.Menu Help 
      Caption         =   "&Help"
      Begin VB.Menu About 
         Caption         =   "&About UnrealEd"
      End
      Begin VB.Menu EpicWeb 
         Caption         =   "&Epic's Web Site"
      End
      Begin VB.Menu QIDJWE 
         Caption         =   "-"
      End
      Begin VB.Menu HelpIndex 
         Caption         =   "&Help Topics"
         Shortcut        =   {F1}
      End
      Begin VB.Menu HelpCam 
         Caption         =   "Help on &Cameras"
         Shortcut        =   {F2}
      End
      Begin VB.Menu RelNotes 
         Caption         =   "&Release Notes"
      End
      Begin VB.Menu ZIELZEB 
         Caption         =   "-"
      End
      Begin VB.Menu UnrealScriptHelp 
         Caption         =   "&UnrealScript help"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim GInitialResized As Integer
Public hwndScript As Long
Public ScriptForm As frmScriptEd

Const BrowserWidth1 = 140
Const BrowserWidth2 = 152

Private Sub About_Click()
    frmDialogs.About.ShowHelp ' WinHelp
End Sub

Private Sub ActorCombo_Click()
    If Ed.Startup Then
        ' Disregard
    ElseIf ActorCombo.ListIndex <> 0 Then
        ActorCombo.ListIndex = 0
        Ed.SetBrowserTopic ("Classes")
    End If
End Sub

Private Sub AddCutFirst_Click()
    Ed.BeginSlowTask "Adding brush to world"
    Ed.Server.SlowExec "BRUSH ADD CUTFIRST"
    Ed.EndSlowTask
End Sub

Private Sub AddCutaway_Click()
    Ed.BeginSlowTask "Adding Cutaway Zone"
    Ed.Server.SlowExec "BRUSH ADD CUTAWAY"
    Ed.EndSlowTask
End Sub

Private Sub AddMovableBrush_Click()
    Ed.BeginSlowTask "Adding movable brush to world"
    Ed.Server.SlowExec "BRUSH ADDMOVABLE"
    Ed.EndSlowTask
End Sub

Private Sub AddNoCut_Click()
    Ed.BeginSlowTask "Adding No-Cut Zone"
    Ed.Server.SlowExec "BRUSH ADD NOCUT"
    Ed.EndSlowTask
    '
    Call MsgBox("A no-cut zone will be added.  This will take effect the next time you rebuild geometry.", 64, "Adding No-Cut Zone")
End Sub

Private Sub AddSpecial_Click()
    frmAddSpecial.Show
End Sub

Private Sub Browser_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If Button = 2 Then
        PopupMenu frmPopups.Browser
    End If
End Sub

Private Sub BrowserHolder_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If Button = 2 Then
        PopupMenu frmPopups.Browser
    End If
End Sub

Private Sub BrowserTopicCombo_Click()
    Ed.SetBrowserTopic (BrowserTopicCombo.Text)
End Sub

Private Sub BrushAdd_Click()
    Ed.BeginSlowTask "Adding brush to world"
    Ed.Server.SlowExec "BRUSH ADD"
    Ed.EndSlowTask
End Sub

Private Sub BrushDeintersect_Click()
    Ed.BeginSlowTask "Deintersecting brush"
    Ed.Server.SlowExec "BRUSH FROM DEINTERSECTION"
    Ed.EndSlowTask
End Sub

Private Sub BrushExport_Click()
    '
    Dim ExportFname As String
    '
    ' Prompt for filename
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.ExportBrush.ShowSave 'Modal Save-As Box
    ExportFname = frmDialogs.ExportBrush.filename
    '
    Call UpdateDialog(frmDialogs.ExportBrush)
    If (ExportFname <> "") Then
        Ed.BeginSlowTask "Exporting brush"
        Ed.Server.SlowExec "BRUSH EXPORT FILE=" & Quotes(ExportFname)
        Ed.EndSlowTask
    End If
Skip: Ed.Server.Enable
End Sub

Private Sub BrushImport_Click()
    '
    ' Dialog for "Brush Import":
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.ImportBrush.filename = ""
    frmDialogs.ImportBrush.DefaultExt = "t3d"
    frmDialogs.ImportBrush.ShowOpen 'Modal File-Open Box
    '
    Call UpdateDialog(frmDialogs.ImportBrush)
    If (frmDialogs.ImportBrush.filename <> "") Then
        frmBrushImp.Show 1
    End If
Skip: Ed.Server.Enable
End Sub

Private Sub BrushIntersect_Click()
    Ed.BeginSlowTask "Intersecting brush"
    Ed.Server.SlowExec "BRUSH FROM INTERSECTION"
    Ed.EndSlowTask
End Sub

Private Sub BrushReset_Click()
    Ed.Server.Exec "BRUSH RESET"
End Sub

Private Sub BrushSliceTex_Click()
    Ed.BeginSlowTask "Adding No-Terrain Zone"
    Ed.Server.SlowExec "BRUSH ADD NOTERRAIN"
    Ed.EndSlowTask
    '
    Call MsgBox("A No-Terrain zone will be added.  This will take effect the next time you rebuild geometry.  See the terrain help for complete information on using No-Terrain zones in UnrealEd.", 64, "Adding No-Terrain Zone")
End Sub

Private Sub BrushSubtract_Click()
    Ed.BeginSlowTask "Subtracting brush from world"
    Ed.Server.SlowExec "BRUSH SUBTRACT"
    Ed.EndSlowTask
End Sub

Private Sub CameraAhead_Click()
    Ed.Server.Exec "CAMERA LOOK AHEAD"
End Sub

Private Sub CameraDown_Click()
    Ed.Server.Exec "CAMERA LOOK DOWN"
End Sub

Private Sub CameraEast_Click()
    Ed.Server.Exec "CAMERA LOOK EAST"
End Sub

Private Sub CameraEntire_Click()
    Ed.Server.Exec "CAMERA LOOK ENTIREMAP"
End Sub

Private Sub CameraNorth_Click()
    Ed.Server.Exec "CAMERA LOOK NORTH"
End Sub

Private Sub CameraSouth_Click()
    Ed.Server.Exec "CAMERA LOOK SOUTH"
End Sub

Private Sub CameraUp_Click()
    Ed.Server.Exec "CAMERA LOOK UP"
End Sub

Private Sub CameraWest_Click()
    Ed.Server.Exec "CAMERA LOOK WEST"
End Sub

Private Sub CalcButton_Click()
    Dim Result As Double
    Dim OrigStr As String
    '
    OrigStr = CalcText.Text
    If Eval(CalcText.Text, Result) Then
        CalcText.ForeColor = &H80000008
        If Trim(OrigStr) <> "0" Then
            If Trim(OrigStr) <> CalcText.List(0) Then
                CalcText.AddItem OrigStr, 0
            End If
        End If
        '
        If Result <> 0 Then
            If Trim(Str(Result)) <> CalcText.List(0) Then
                CalcText.AddItem Trim(Str(Result)), 0
            End If
        End If
        CalcText.Text = Trim(Str(Result))
        CalcText.SetFocus
        SendKeys "{HOME}+{END}" ' Select all
    Else
        CalcText.ForeColor = &HC0&
        CalcText.SetFocus
        SendKeys "{End}"
    End If
End Sub

Private Sub CalcText_Click()
    If CalcText.Text = "Reset" Then
        CalcText.Clear
        CalcText.AddItem "Reset"
        CalcText.Text = "0"
        CalcText.SetFocus
        SendKeys "{HOME}+{END}" ' Select all
    End If
    CalcText.ForeColor = &H80000008
End Sub

Private Sub CalcZero_Click()
    CalcText.Text = "0"
    CalcText.ForeColor = &H80000008
    CalcText.SetFocus
    SendKeys "{HOME}+{END}" ' Select all
End Sub

Private Sub ClassBrows_Click()
    frmClassBrowser.Show
End Sub


Private Sub CamAllViews_Click()
    Ed.CameraVertRatio = 0.66
    Ed.CameraLeftRatio = 0.5
    Ed.CameraRightRatio = 0.5
    ResizeAll (True)
End Sub

Private Sub CamCloseAllFree_Click()
    Ed.Server.Exec "CAMERA CLOSE FREE"
End Sub


Private Sub CameraResetAll_Click()
    Ed.Server.Exec "CAMERA CLOSE ALL"
    Ed.CameraVertRatio = 0.66
    Ed.CameraLeftRatio = 0.5
    Ed.CameraRightRatio = 0.5
    ResizeAll (False)
End Sub

Private Sub CamOpenFree_Click()
    Ed.OpenFreeCamera
End Sub

Private Sub CamOvh_Click()
    Ed.CameraVertRatio = 1#
    Ed.CameraLeftRatio = 1#
    ResizeAll (True)
End Sub

Private Sub CamPersp_Click()
    Ed.CameraVertRatio = 1#
    Ed.CameraLeftRatio = 0#
    ResizeAll (True)
End Sub

Private Sub CamTwoViews_Click()
    Ed.CameraVertRatio = 1#
    Ed.CameraLeftRatio = 0.4
    ResizeAll (True)
End Sub

Private Sub Command1_Click()
    ToolHelp (123)
End Sub

Private Sub CurvedStair_Click()
    frmParSolCurvedStair.Show
End Sub

Private Sub Delete_Click()
    Ed.Server.Exec "DELETE"
End Sub

Private Sub Directories_Click()
   'Ed.Server.Disable
   'frmDirectories.Show 1
   'Ed.Server.Enable
End Sub

Private Sub Duplicate_Click()
    Ed.Server.Exec "DUPLICATE"
End Sub

Private Sub EditCopy_Click()
    If hwndScript <> 0 Then
        ScriptForm.EditCopy_Click
    End If
End Sub

Private Sub EditCut_Click()
    If hwndScript <> 0 Then
        ScriptForm.EditCut_Click
    End If
End Sub

Private Sub EditFind_Click()
    If hwndScript <> 0 Then
        ScriptForm.EditFind_Click
    End If
End Sub

Private Sub EditFindNext_Click()
    If hwndScript <> 0 Then
        ScriptForm.EditFindNext_Click
    End If
End Sub

Private Sub EditPaste_Click()
    If hwndScript <> 0 Then
        ScriptForm.EditPaste_Click
    End If
End Sub

Private Sub EpicButton_Click()
    Ed.Server.Exec "LAUNCH WEB"
End Sub

Private Sub EpicWeb_Click()
    Ed.Server.Exec "LAUNCH WEB"
End Sub

Private Sub Exit_Click()
   Unload Me
End Sub

Private Sub ExportLevel_Click()
    '
    Dim ExportFname As String
    '
    ' Prompt for filename
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.ExportMap.ShowSave
    ExportFname = frmDialogs.ExportMap.filename
    '
    Call UpdateDialog(frmDialogs.ExportMap)
    If (ExportFname <> "") Then
        PreSaveAll
        Ed.BeginSlowTask "Exporting map"
        Ed.Server.SlowExec "MAP EXPORT FILE=" & Quotes(ExportFname)
        Ed.EndSlowTask
    End If
    '
Skip: Ed.Server.Enable
End Sub

Private Sub FloorLofter_Click()
    frmFloorLofter.Show
End Sub

Private Sub GridCombo_Click()
    Dim S As Integer
    '
    If Ed.Startup Then
        ' Disregard
    ElseIf GridCombo.Text = "Off" Then
        Call Ed.SetGridMode(0)
    ElseIf GridCombo.Text = "Custom" Then
        frmGrid.Show
        Call Ed.SetGridMode(1)
    Else
        S = Val(GridCombo.Text)
        Call Ed.SetGridSize(S, S, S)
        Call Ed.SetGridMode(1)
    End If
End Sub

Private Sub HelpButton_Click()
    frmMain.PopupMenu frmPopups.Help
End Sub

Private Sub HelpCam_Click()
    '
    ' Bring up camera help specific to the
    ' current editor mode.
    '
    Call Ed.Tools.Handlers(Ed.ToolMode).DoHelp(Ed.ToolMode, Ed)
    '
End Sub

Private Sub HelpIndex_Click()
    frmDialogs.HelpContents.HelpFile = App.Path + "\\help\\unrealed.hlp"
    frmDialogs.HelpContents.ShowHelp ' Run WinHelp
End Sub

Private Sub Holder_KeyDown(KeyCode As Integer, Shift As Integer)
    If KeyCode = &H70& Then ' Intercept F1
       KeyCode = 0
       Call Ed.Tools.Handlers(Ed.MRUTool).DoHelp(Ed.MRUTool, Ed)
    End If
End Sub

Private Sub ImportLevel_Click()
    '
    ' Dialog for "Map Import"
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.ImportMap.filename = ""
    frmDialogs.ImportMap.ShowOpen 'Modal File-Open Box
    '
    Call UpdateDialog(frmDialogs.ImportMap)
    If (frmDialogs.ImportMap.filename <> "") Then
        '
        frmImportMap.Show 1
        '
        If GResult Then
            Ed.BeginSlowTask "Importing map"
            If GImportExisting Then ' Import new map
                Ed.Server.SlowExec "MAP IMPORTADD FILE=" & Quotes(frmDialogs.ImportMap.filename)
            Else ' Add to existing map
                Ed.Server.SlowExec "MAP IMPORT FILE=" & Quotes(frmDialogs.ImportMap.filename)
            End If
            Ed.EndSlowTask
            If Ed.MapEdit Then Call Ed.Tools.Click("MAPEDIT")
            PostLoad
        End If
    End If
Skip: Ed.Server.Enable
End Sub

Private Sub LoadBrush_Click()
    '
    ' Dialog for "Load Brush":
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.BrushOpen.filename = ""
    frmDialogs.BrushOpen.ShowOpen 'Modal Brush-Open Box
    '
    Call UpdateDialog(frmDialogs.BrushOpen)
    If (frmDialogs.BrushOpen.filename <> "") Then
        '
        ' Load the brush
        '
        Call UpdateDialog(frmDialogs.BrushOpen)
        Ed.BrushFname = frmDialogs.BrushOpen.filename
        Ed.Server.Exec "BRUSH LOAD FILE=" & Quotes(Ed.BrushFname)
    End If
Skip: Ed.Server.Enable
End Sub

Private Sub Map320x200_Click()
    Ed.Server.Exec "CAMERA SIZE XR=320 YR=200"
End Sub

Private Sub Map400x300_Click()
    Ed.Server.Exec "CAMERA SIZE XR=400 YR=300"
End Sub

Private Sub Map480x360_Click()
    Ed.Server.Exec "CAMERA SIZE XR=480 YR=360"
End Sub

Private Sub Map560x420_Click()
    Ed.Server.Exec "CAMERA SIZE XR=560 YR=420"
End Sub

Private Sub Map640x480_Click()
    Ed.Server.Exec "CAMERA SIZE XR=640 YR=480"
End Sub

Private Sub MapFlat_Click()
    Ed.Server.Exec "CAMERA SET MODE=FLAT"
End Sub

Private Sub MapFlatNorms_Click()
    Ed.Server.Exec "CAMERA SET MODE=FLATNORMS"
End Sub

Private Sub MapIllum_Click()
    Ed.Server.Exec "CAMERA SET MODE=ILLUM"
End Sub

Private Sub MapLight_Click()
    Ed.Server.Exec "CAMERA SET MODE=SHADE"
End Sub

Private Sub MapPersp_Click()
    Ed.Server.Exec "CAMERA SET MODE=MAP3D"
End Sub

Private Sub MapTextures_Click()
    Ed.Server.Exec "CAMERA SET MODE=TEXTURES"
End Sub

Private Sub MapXY_Click()
    Ed.Server.Exec "CAMERA SET MODE=MAPXY"
End Sub

Private Sub MapXZ_Click()
    Ed.Server.Exec "CAMERA SET MODE=MAPXZ"
End Sub

Private Sub MapYZ_Click()
    Ed.Server.Exec "CAMERA SET MODE=MAPYZ"
End Sub

Private Sub MDIForm_Load()
    '
    Dim i
    Dim S As String, T As String
    Dim Temp As String
    Dim Highlight As Boolean
    '
    ' Inhibit registry reading if desired.
    '
    If InStr(UCase(Command$), "RESET") Then
        MsgBox "Your UnrealEd settings have been reset."
        NoReadRegistry = True
    End If
    '
    ' Init App object properties.
    '
    Call InitApp
    '
    ' Create global UnrealEdApp object.
    '
    Set Ed = New UnrealEdApp
    '
    ' Show startup screen
    '
    Ed.Startup = 1
    App.Title = Ed.EditorAppName
    frmMain.Caption = Ed.EditorAppName
    frmMain.Show
    '
    ' Stick values in combo boxes
    '
    If Screen.Width <= 640 * Screen.TwipsPerPixelX Then
        EpicButton.Visible = False
        ModeLabel.Visible = False
        ModeCombo.Visible = False
    End If
    '
    If Screen.Width < 1024 * Screen.TwipsPerPixelX Then
        CalcButton.Visible = False
        CalcText.Visible = False
        CalcZero.Visible = False
    End If
    '
    ModeCombo.AddItem "Move Camera/Brush"
    ModeCombo.AddItem "Zoom Camera/Brush"
    ModeCombo.AddItem "Brush Rotate"
    ModeCombo.AddItem "Brush Scale"
    ModeCombo.AddItem "Brush Sheer"
    ModeCombo.AddItem "Brush Stretch"
    ModeCombo.AddItem "Brush SnapScale"
    ModeCombo.AddItem "Add Actor"
    ModeCombo.AddItem "Add Light"
    ModeCombo.AddItem "Move Actor/Light"
    ModeCombo.AddItem "Set/Pan/Scale Textures"
    ModeCombo.AddItem "Rotate Textures"
    ModeCombo.AddItem "Terraform"
    ModeCombo.ListIndex = 0
    '
    GridCombo.AddItem "Off"
    GridCombo.AddItem "1"
    GridCombo.AddItem "2"
    GridCombo.AddItem "4"
    GridCombo.AddItem "8"
    GridCombo.AddItem "16"
    GridCombo.AddItem "32"
    GridCombo.AddItem "64"
    GridCombo.AddItem "128"
    GridCombo.AddItem "256"
    GridCombo.AddItem "Custom"
    GridCombo.ListIndex = 5
    '
    ActorCombo.AddItem "Light"
    ActorCombo.AddItem "Show Browser..."
    ActorCombo.ListIndex = 0
    '
    TextureCombo.AddItem "Default"
    TextureCombo.AddItem "Show Browser..."
    TextureCombo.ListIndex = 0
    '
    ' Launch UnrealServer.  There is only
    ' one of these per instance.
    '
    Ed.GetProfile
    Ed.LaunchServer (frmMain.hwnd)
    If Ed.Licensed = 0 Then frmLicense.Show 1 ' Show license info
    Call Ed.Server.Init(frmMain.hwnd, frmMain.Callback.hwnd) ' Initialize the server
    '
    ' Set help file dirs:
    '
    frmDialogs.ToolHelp.HelpFile = App.HelpFile
    frmDialogs.RelNotes.HelpFile = App.HelpFile
    frmDialogs.HelpContents.HelpFile = App.HelpFile
    frmDialogs.About.HelpFile = App.HelpFile
    '
    ' Initialize tools
    '
    Call Ed.Tools.InitTools(Ed)
    '
    Ed.Startup = 0
    GInitialResized = 1
    '
    Ed.Server.Exec "APP HIDE"
    '
    ' Set server parameters to the defaults the
    ' editor expects:
    '
    Ed.Server.Exec "MAP GRID X=16 Y=16 Z=16 BASE=ABSOLUTE SHOW2D=ON SHOW3D=OFF"
    Ed.Server.Exec "MAP ROTGRID PITCH=4 YAW=4 ROLL=4"
    Ed.Server.Exec "MODE CAMERAMOVE GRID=ON ROTGRID=ON SHOWVERTICES=ON SNAPTOPIVOT=ON SNAPDIST=10"
    '
    ResizeAll (False)
    '
    ' Init and show toolbar (must be drawn after camera in order
    ' for palette to come out right):
    '
    InitToolbar
    '
    ' Load initial resources, if any
    '
    If Ed.InitialFiles <> "" Then
        Open "UnrealEd.tmp" For Append As #1
        If LOF(1) <> 0 Then
            Close #1
            Call MsgBox("It appears that UnrealEd did not start up successfully the last time it was run.  This may be due to invalid startup files that were specified.  You may want to remove them from the Preferences dialog.", _
                0, "Possible UnrealEd startup problem")
            Kill "UnrealEd.tmp"
            frmPreferences.Show 1
        Else
            Print #1, "UnrealEd"
            Close #1
            S = Ed.InitialFiles
            Do
                If InStr(S, " ") Then
                    T = Trim(Left(S, InStr(S, " ") - 1))
                    S = Trim(Mid(S, InStr(S, " ") + 1))
                Else
                    T = S
                    S = ""
                End If
                If T <> "" Then
                    Ed.BeginSlowTask "Loading file"
                    Ed.Server.SlowExec "RES LOAD FILE=" & Quotes(T)
                    Ed.EndSlowTask
                End If
            Loop Until T = ""
            Kill "UnrealEd.tmp"
        End If
    End If
    
    Ed.StatusText "UnrealEd is ready to go"
    '
    Call Ed.RegisterBrowserTopic(frmTexBrowser, "Textures")
    Call Ed.RegisterBrowserTopic(frmClassBrowser, "Classes")
    Call Ed.RegisterBrowserTopic(frmSoundFXBrowser, "SoundFX")
    Call Ed.RegisterBrowserTopic(frmMusicBrowser, "Music")
    Call Ed.SetBrowserTopic(Ed.InitialBrowserTopic)
    '
    ' Browsers that no longer exist:
    'Call Ed.RegisterBrowserTopic(frmBrushBrowser, "Brushes")
    'Call Ed.RegisterBrowserTopic(frmAmbientBrowser, "Ambient")
    '
    If CalcText.Visible Then
        CalcText.AddItem "Reset"
        CalcText.Text = "0"
        CalcButton.SetFocus
        SendKeys "{HOME}+{END}" ' Select all
    End If
    
    PreferencesChange
    
    ' Enable registry reading.
    NoReadRegistry = False

    ' Load command-line level, if any.
    If GetString(Command$, "FILE=", Temp) Then
        If InStr(Temp, ":") = 0 And (Left(Temp, 1) <> "\") Then
            Temp = App.Path + "\" + Temp
        End If
        Ed.BeginSlowTask "Loading " & Temp
        Ed.Server.Exec "MAP LOAD FILE=" & Quotes(Temp)
        Ed.EndSlowTask
        '
        Ed.MapFname = Temp
        Caption = Ed.EditorAppName + " - " + Ed.MapFname
        Ed.LoadParamsFromLevel
        ResizeAll (True)
    End If

End Sub

Private Sub MDIForm_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If (Button And 2) <> 0 Then ' Right click
        PopupMenu frmPopups.WIndow
    End If
End Sub

Private Sub MDIForm_Resize()
    ResizeAll (True)
End Sub

Private Sub MDIForm_Unload(Cancel As Integer)
    Dim N As Integer
    
    ' Unload browser.
    Ed.UnloadBrowser
   
    ' Unload all forms
    Dim i As Long
    For i = Forms.Count - 1 To 0 Step -1
        Unload Forms(i)
    Next
   
    ' Save profile now that all forms have
    ' called their EndOnTop's.
    Ed.SaveProfile
   
   ' End the program, in case any stray objects
   ' are still hanging around in memory.
   End
End Sub

Private Sub MapEditMode_Click()
    Ed.Tools.Click "MAPEDIT"
End Sub

Private Sub MeshViewer_Click()
    frmMeshViewer.Show
End Sub

Private Sub ModeCombo_Click()
    If Ed.Startup <> 0 Or GSettingMode <> 0 Then
        ' Disregard
    Else
        GSettingMode = 1
        Select Case ModeCombo.List(ModeCombo.ListIndex)
        Case "Move Camera/Brush": Ed.Tools.Click "CAMERAMOVE"
        Case "Zoom Camera/Brush": Ed.Tools.Click "CAMERAZOOM"
        Case "Brush Rotate": Ed.Tools.Click "BRUSHROTATE"
        Case "Brush Scale": Ed.Tools.Click "BRUSHSCALE"
        Case "Brush Sheer": Ed.Tools.Click "BRUSHSHEER"
        Case "Brush Stretch": Ed.Tools.Click "BRUSHSTRETCH"
        Case "Brush SnapScale": Ed.Tools.Click "BRUSHSNAP"
        Case "Add Actor": Ed.Tools.Click "ADDACTOR"
        Case "Add Light": Ed.Tools.Click "ADDLIGHT"
        Case "Move Actor/Light": Ed.Tools.Click "MOVEACTOR"
        Case "Set/Pan/Scale Textures": Ed.Tools.Click "TEXTUREPAN"
        Case "Rotate Textures": Ed.Tools.Click "TEXTUREROTATE"
        Case "Terraform": Ed.Tools.Click "TERRAFORM"
        End Select
        GSettingMode = 0
    End If
    Call Ed.StatusText("Set mode to " & ModeCombo.List(ModeCombo.ListIndex))
End Sub

Private Sub New_Click()
    Ed.Server.Disable
    If MsgBox("Are you sure you want to create a new map?", _
        vbOKCancel) = vbOK Then
       
        ' New map.
        Ed.MapFname = ""
        frmMain.Caption = Ed.EditorAppName
        Ed.Server.Exec "MAP NEW"
        PostLoad
    End If
    Ed.Server.Enable
End Sub

Private Sub ObjectProperties_Click()
   frmActorProperties.Show
End Sub

Private Sub Open_Click()
    '
    ' Dialog for "Open Map":
    '
    On Error GoTo Skip
    Ed.Server.Disable
    frmDialogs.MapOpen.filename = ""
    frmDialogs.MapOpen.ShowOpen
    '
    Call UpdateDialog(frmDialogs.MapOpen)
    If (frmDialogs.MapOpen.filename <> "") Then
        '
        ' Load the map, inhibiting redraw since we're
        ' about to resize everything anyway.
        '
        Ed.MapFname = frmDialogs.MapOpen.filename
        Caption = Ed.EditorAppName + " - " + Ed.MapFname
        '
        Ed.BeginSlowTask "Loading map"
        Ed.Server.SlowExec "MAP LOAD FILE=" & _
            Quotes(Ed.MapFname) & " REDRAW=OFF"
        Ed.EndSlowTask
        '
        If Ed.MapEdit Then Call Ed.Tools.Click("MAPEDIT")
        Ed.LoadParamsFromLevel
        ResizeAll (True)
        PostLoad
        '
    End If
    '
Skip: Ed.Server.Enable
End Sub

Private Sub PanelHolder_Mousedown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    If Button = 2 Then
        PopupMenu frmPopups.Panel
    End If
End Sub

Private Sub ParSolCone_Click()
    frmParSolCone.Show
End Sub

Private Sub ParSolHeightMap_Click()
    frmParSolHeightMap.Show
End Sub

Private Sub ParSolLinearStair_Click()
    frmParSolLinearStair.Show
End Sub

Private Sub ParSolRect_Click()
    frmParSolCube.Show
End Sub

Private Sub ParSolSphereDome_Click()
    frmParSolSphere.Show
End Sub

Private Sub ParSolSpiralStair_Click()
    frmParSolSpiralStair.Show
End Sub

Private Sub ParSolTube_Click()
    frmParSolTube.Show
End Sub

Private Sub PolygonProperties_Click()
   frmSurfaceProps.Show
End Sub

Private Sub Preferences_Click()
   Ed.Server.Disable
   frmPreferences.Show 1
   Ed.Server.Enable
End Sub

Private Sub Project_Click()
    frmActorProperties.GetLevelProperties
End Sub

Private Sub Rebuild_Click()
    frmRebuilder.Show ' Rebuild dialog
End Sub

Private Sub EditRedo_Click()
    If hwndScript <> 0 Then
        ScriptForm.EditRedo_Click
    Else
        Ed.Server.Exec "TRANSACTION REDO"
    End If
End Sub

Private Sub RedoButton_Click()
    Ed.Tools.Click "TRANSACTION REDO"
End Sub

Private Sub RelNotes_Click()
    frmDialogs.RelNotes.ShowHelp ' Run WinHelp
End Sub

Private Sub ResetAll_Click()
    Ed.Server.Exec "BRUSH RESET"
End Sub

Private Sub ResetPosition_Click()
    Ed.Server.Exec "BRUSH MOVETO X=0 Y=0 Z=0"
End Sub

Private Sub ResetRotation_Click()
    Ed.Server.Exec "BRUSH ROTATETO PITCH=0 YAW=0 ROLL=0"
End Sub

Private Sub ResetScale_Click()
    Ed.Server.Exec "BRUSH SCALE X=1 Y=1 Z=1 SHEER=0"
End Sub

Private Sub Resize_Click()
    frmBrush.Show
End Sub

Private Sub Save_Click()
    On Error GoTo Skip
    If Ed.MapFname = "" Then
        '
        ' Prompt for filename
        '
        Ed.Server.Disable
        frmDialogs.MapSaveAs.ShowSave 'Modal Save-As Box
        Ed.MapFname = frmDialogs.MapSaveAs.filename
        Ed.Server.Enable
        Call UpdateDialog(frmDialogs.MapSaveAs)
    End If
    '
    If Ed.MapFname <> "" Then
        '
        ' Save the map
        '
        PreSaveAll
        Caption = Ed.EditorAppName + " - " + Ed.MapFname
        Ed.BeginSlowTask ("Saving map")
        Ed.SaveParamsToLevel
        Ed.Server.SlowExec "MAP SAVE FILE=" & Quotes(Ed.MapFname)
        Ed.EndSlowTask
    End If
Skip: Ed.Server.Enable
End Sub

Private Sub SaveAs_Click()
    '
    ' Just set the default filename to empty and
    ' call Save_Click to do the normal "save" procedure.
    '
    Ed.MapFname = ""
    Save_Click
End Sub

Private Sub SaveBrush_Click()
    On Error GoTo Skip
    If Ed.BrushFname = "" Then
        '
        ' Prompt for filename
        '
        Ed.Server.Disable
        frmDialogs.BrushSave.ShowSave
        Ed.BrushFname = frmDialogs.BrushSave.filename
        Call UpdateDialog(frmDialogs.BrushSave)
    End If
    '
    If Ed.BrushFname <> "" Then
        '
        ' Save the brush
        '
        Ed.Server.Exec "BRUSH SAVE FILE=" & Quotes(Ed.BrushFname)
    End If
Skip: Ed.Server.Enable
End Sub

Private Sub SaveBrushAs_Click()
    '
    ' Just set the default filename to empty and
    ' call Save_Click to do the normal "save" procedure.
    '
    Ed.BrushFname = ""
    SaveBrush_Click
End Sub

Private Sub ScriptCompile_Click()
    ScriptForm.ScriptCompile_Click
End Sub

Private Sub ScriptEditDefaults_Click()
    ScriptForm.ScriptEditDefaults_Click
End Sub

Private Sub ScriptMakeAll_Click()
    ScriptForm.ScriptMakeAll_Click
End Sub

Private Sub ScriptMakeChanged_Click()
    ScriptForm.ScriptMakeChanged_Click
End Sub

Private Sub ScriptNextError_Click()
    ScriptForm.ScriptNextError_Click
End Sub

Private Sub ScriptResults_Click()
    ScriptForm.ScriptResults_Click
End Sub

Private Sub SelAdjFloors_Click()
    Ed.Server.Exec "POLY SELECT ADJACENT FLOORS"
End Sub

Private Sub SelAdjSlants_Click()
    Ed.Server.Exec "POLY SELECT ADJACENT SLANTS"
End Sub

Private Sub SelAdjWalls_Click()
    Ed.Server.Exec "POLY SELECT ADJACENT WALLS"
End Sub

Private Sub SelAllAdj_Click()
    Ed.Server.Exec "POLY SELECT ADJACENT ALL"
End Sub

Private Sub SelCoplAdj_Click()
    Ed.Server.Exec "POLY SELECT ADJACENT COPLANARS"
End Sub

Private Sub SelectAll_Click()
    Ed.Server.Exec "SELECT ALL"
End Sub

Private Sub SelectNone_Click()
    Ed.Server.Exec "SELECT NONE"
End Sub

Private Sub SelIntersection_Click()
    Ed.Server.Exec "POLY SELECT MEMORY INTERSECTION"
End Sub

Private Sub SelMatchBrush_Click()
    Ed.Server.Exec "POLY SELECT MATCHING BRUSH"
End Sub

Private Sub SelMatchGroups_Click()
    Ed.Server.Exec "POLY SELECT MATCHING GROUPS"
End Sub

Private Sub SelMatchItems_Click()
    Ed.Server.Exec "POLY SELECT MATCHING ITEMS"
End Sub

Private Sub SelMatchTex_Click()
    Ed.Server.Exec "POLY SELECT MATCHING TEXTURE"
End Sub

Private Sub SelMemorize_Click()
    Ed.Server.Exec "POLY SELECT MEMORY SET"
End Sub

Private Sub SelRecall_Click()
    Ed.Server.Exec "POLY SELECT MEMORY RECALL"
End Sub

Private Sub SelReverse_Click()
    Ed.Server.Exec "POLY SELECT REVERSE"
End Sub

Private Sub SelUnion_Click()
    Ed.Server.Exec "POLY SELECT MEMORY UNION"
End Sub

Private Sub SelXor_Click()
    Ed.Server.Exec "POLY SELECT MEMORY XOR"
End Sub

Private Sub ShowBackdrop_Click()
    Ed.Server.Exec "CAMERA SET BACKDROP=TOGGLE"
End Sub

Private Sub ShowBrush_Click()
    Ed.Server.Exec "CAMERA SET BRUSH=TOGGLE"
End Sub

Private Sub ShowGrid_Click()
    Ed.Server.Exec "CAMERA SET GRID=TOGGLE"
End Sub

Private Sub ShowOcclusion_Click()
    Ed.Server.Exec "CAMERA SET OCCLUSION=TOGGLE"
End Sub

Private Sub SSPanel5_Click()

End Sub
Private Sub TexBrows_Click()
    Ed.BrowserPos = 0
    ResizeAll (True)
End Sub

Private Sub TexPalette_Click()
    Ed.BrowserPos = 0
    ResizeAll (False)
End Sub

Private Sub TextureCombo_Click()
    If Ed.Startup Then
        ' Disregard
    ElseIf TextureCombo.ListIndex = 1 Then
        If Ed.BrowserPos = 2 Then
            Ed.BrowserPos = 0
            ResizeAll (True)
        End If
        Call Ed.SetBrowserTopic("Textures")
        TextureCombo.ListIndex = 0
    End If
End Sub

Private Sub Timer_Timer()
    Dim Name As String
    Ed.AutoSaveCountup = Ed.AutoSaveCountup + 1
    If (Ed.AutoSaveCountup > Ed.AutoSaveTime) And _
        (Ed.AutoSaveTime <> 0) Then
        '
        Ed.AutoSaveCountup = 0
        Ed.BeginSlowTask ("Saving map")
        '
        If Ed.AutoUnique <> 0 Then
            Name = "Auto" & Trim(Str(Ed.AutoCounter))
        Else
            Name = "Autosave"
        End If
        '
        Ed.Server.SlowExec "MAP SAVE FILE=" & Quotes(Ed.BaseDir + Ed.MapDir + "\" + Name + ".unr")
        Ed.EndSlowTask
        '
        If Ed.AutoUnique <> 0 Then
            Ed.AutoCounter = (Ed.AutoCounter + 1) Mod 20
            Ed.SaveProfile
        End If
    End If
End Sub

Private Sub TWODEE_Click()
    frmTwoDee.Show
End Sub

Private Sub EditUndo_Click()
    If hwndScript <> 0 Then
        ScriptForm.EditUndo_Click
    Else
        Ed.Server.Exec "TRANSACTION UNDO"
    End If
End Sub

Private Sub UndoButton_Click()
    Ed.Tools.Click "TRANSACTION UNDO"
End Sub

Private Sub UnrealScriptHelp_Click()
    '!!
End Sub

Private Sub ValidateLevel_Click()
    Call frmResults.UpdateStatus("Validating level:")
    Ed.Server.Exec "LEVEL VALIDATE"
    frmResults.UpdateResults
    frmResults.Results_DblClick
End Sub

Public Sub ViewLevelLinks_Click()
    Call frmResults.UpdateStatus("Level links:")
    Ed.Server.Exec "LEVEL LINKS"
    frmResults.UpdateResults
    frmResults.Results_DblClick
End Sub

Private Sub WinBrowserHide_Click()
    Ed.BrowserPos = 2
    ResizeAll (True)
End Sub

Private Sub WinBrowserLeft_Click()
    Ed.BrowserPos = 1
    ResizeAll (True)
End Sub

Private Sub WinBrowserRight_Click()
    Ed.BrowserPos = 0
    ResizeAll (True)
End Sub

Private Sub WindowLog_Click()
    Ed.Server.Exec "APP SHOW"
End Sub

Private Sub WinToolbarLeft_Click()
    Ed.ToolbarPos = 0 ' left
    ResizeAll (True)
End Sub

Private Sub WinToolbarRight_Click()
    Ed.ToolbarPos = 1 ' right
    ResizeAll (True)
End Sub

Private Sub WorldBrowser_Click()
    frmResBrowse.Show
End Sub

'---------------------------------'
' All code related to the toolbar '
'---------------------------------'

Private Sub MDIForm_KeyDown(KeyCode As Integer, Shift As Integer)
    If KeyCode = &H70& Then ' Intercept F1
       KeyCode = 0
       Call Ed.Tools.Handlers(Ed.MRUTool).DoHelp(Ed.MRUTool, Ed)
    End If
End Sub

Private Sub ResizeToolbar()
    Dim W As Integer, H As Integer, MaxH As Integer
    '
    ToolbarCount = ToolGridX * ToolGridY
    '
    W = (ToolGridX * 35 + 6) * Screen.TwipsPerPixelX
    H = (ToolGridY * 35 + 32) * Screen.TwipsPerPixelY
    MaxH = Toolbar.Height - 32 * Screen.TwipsPerPixelY
    '
    Holder.Top = 1 * Screen.TwipsPerPixelY
    Holder.Height = H + 5 * Screen.TwipsPerPixelY
    Holder.Width = W - 3 * Screen.TwipsPerPixelX
    '
    If (H > MaxH) Then ' Must use scrollbar
        If Ed.ToolbarPos = 0 Then ' Left
            Holder.Left = 2 * Screen.TwipsPerPixelX
            Scroller.Left = W - 2 * Screen.TwipsPerPixelX
        Else ' Right
            Holder.Left = Scroller.Width + 2 * Screen.TwipsPerPixelX
            Scroller.Left = 1 * Screen.TwipsPerPixelX
        End If
        Toolbar.Width = W + 13 * Screen.TwipsPerPixelX
        Scroller.Height = Toolbar.Height
        Scroller.Min = 0
        Scroller.Max = H - MaxH ' - 14 * Screen.TwipsPerPixelY
        Scroller.Value = Scroller.Min
        Scroller.LargeChange = MaxH
        Scroller.SmallChange = MaxH / 8
        Scroller.Visible = True
    Else ' No scrollbar, everything fits nicely
        Holder.Left = 2 * Screen.TwipsPerPixelX
        Toolbar.Width = W
        Scroller.Visible = False
    End If
    '
    StatusText.Top = Holder.Height - 33 * Screen.TwipsPerPixelY
    StatusText.Left = 2 * Screen.TwipsPerPixelX
    StatusText.Width = Holder.Width - 10 * Screen.TwipsPerPixelX
End Sub

Private Sub InitToolbar()
    Dim i, j, N, V As Integer
    Dim Highlight As Boolean
    Dim Temp As String
    '
    ' Init defaults
    '
    Ed.GridMode = 1
    Ed.RotGridMode = 1
    Ed.SpeedMode = 1
    Ed.SnapVertex = 1
    Ed.ShowVertices = 1
    '
    ' Build grid
    '
    For N = 0 To ToolbarCount - 1
        '
        i = Int(N / 3)
        j = N Mod 3
        '
        If N <> 0 Then
            Load ToolIcons(N)
        End If
        '
        ToolIcons(N).Left = (j * 35) * Screen.TwipsPerPixelX
        ToolIcons(N).Top = (i * 35) * Screen.TwipsPerPixelY
        ToolIcons(N).Width = 35 * Screen.TwipsPerPixelX
        ToolIcons(N).Height = 35 * Screen.TwipsPerPixelY
        ToolIcons(N).GroupNumber = N
        '
    Next N
    '
    ' Set all tool names
    '
    ToolIcons(0).Tag = "CAMERAMOVE"
    ToolIcons(1).Tag = "CAMERAZOOM"
    '
    ToolIcons(3).Tag = "BRUSHROTATE"
    ToolIcons(4).Tag = "BRUSHSHEER"
    '
    ToolIcons(6).Tag = "BRUSHSCALE"
    ToolIcons(7).Tag = "BRUSHSTRETCH"
    '
    ToolIcons(9).Tag = "BRUSHSNAP"
    ToolIcons(10).Tag = "MOVEACTOR"
    '
    ToolIcons(12).Tag = "ADDLIGHT"
    ToolIcons(13).Tag = "ADDACTOR"
    '
    ToolIcons(15).Tag = "SELECT ALL"
    ToolIcons(16).Tag = "SELECT NONE"
    '
    ToolIcons(18).Tag = "TRANSACTION UNDO"
    ToolIcons(19).Tag = "TRANSACTION REDO"
    '
    ToolIcons(21).Tag = "TEXTURE RESET"
    ToolIcons(22).Tag = "BRUSH RESET"
    '
    ToolIcons(24).Tag = "TEXTUREPAN"
    ToolIcons(25).Tag = "TEXTUREROTATE"
    '
    ToolIcons(27).Tag = "BROKEN"
    ToolIcons(28).Tag = "BRUSH MIRROR"
    '
    ToolIcons(30).Tag = "MAPEDIT"
    ToolIcons(31).Tag = "TERRAFORM"
    '
    ToolIcons(33).Tag = "SHOWVERTICES"
    ToolIcons(34).Tag = "SNAPVERTEX"
    '
    ToolIcons(36).Tag = "HELP"
    ToolIcons(37).Tag = "SPEED"
    '
    ToolIcons(39).Tag = "GRID"
    ToolIcons(40).Tag = "ROTGRID"
    '
    ToolIcons(42).Tag = "ACTBROWSE"
    ToolIcons(43).Tag = "TEXBROWSE"
    '
    ' Brush tools:
    '
    ToolIcons(2).Tag = "BRUSH ADD"
    ToolIcons(5).Tag = "BRUSH SUBTRACT"
    ToolIcons(8).Tag = "BRUSH FROM INTERSECTION"
    ToolIcons(11).Tag = "BRUSH FROM DEINTERSECTION"
    ToolIcons(14).Tag = "BRUSH ADD SPECIAL"
    ToolIcons(17).Tag = "BRUSH ADDMOVER"
    ToolIcons(20).Tag = "CUBE"
    ToolIcons(23).Tag = "SPHERE"
    ToolIcons(26).Tag = "CYLINDER"
    ToolIcons(29).Tag = "CONE"
    ToolIcons(32).Tag = "STAIR"
    ToolIcons(35).Tag = "SPIRAL"
    ToolIcons(38).Tag = "CURVEDSTAIR"
    ToolIcons(41).Tag = "SHEET"
    ToolIcons(44).Tag = "LOAD"
    '
    GToolClicking = 1
    For N = 0 To ToolbarCount - 1
        '
        ' Get picture
        '
        Call Ed.Tools.GetPicture(ToolIcons(N).Tag, ToolIcons(N))
        '
        ' Set highlighting
        '
        Call Ed.Tools.Handlers(ToolIcons(N).Tag).GetStatus(ToolIcons(N).Tag, Ed, Temp, Highlight)
        Call Ed.Tools.Highlight(ToolIcons(N).Tag, Highlight)
        '
        ' Make visible
        '
        ToolIcons(N).Visible = True
    Next N
    StatusText.ZOrder
    GToolClicking = 0
    '
    ' Set initial mode to first tool, and show picture:
    '
    Call Ed.Tools.Handlers("CAMERAMOVE").DoClick("CAMERAMOVE", Ed)
    '
End Sub

Private Sub Scroller_Scroll()
    Holder.Top = -Scroller.Value
End Sub

Private Sub Scroller_Change()
    Scroller_Scroll
End Sub

Private Sub ToolIcons_Click(index As Integer, Value As Integer)
    Dim i As Integer
    Dim Tool As String
    '
    If GToolClicking = 0 Then
        GToolClicking = 1
        Tool = ToolIcons(index).Tag
        Call Ed.Tools.Handlers(Tool).DoClick(Tool, Ed)
        Ed.MRUTool = Tool
        GToolClicking = 0
    End If
    '
End Sub

Private Sub ToolIcons_MouseDown(index As Integer, Button As Integer, Shift As Integer, X As Single, Y As Single)
    '
    Dim i As Integer
    Dim Icon As Integer
    Dim Temp As String
    Dim Highlight As Integer
    '
    If (Button And 2) <> 0 Then ' Left click
        Set PopupToolControl = ToolIcons(index)
        PopupToolMoveable = False
        PopupToolIndex = index
        Call LeftClickTool(ToolIcons(index).Tag, frmMain)
    End If
    '
End Sub

'
' Resize the entire screen: toolbar, panel,
' and browser.
'
Public Sub ResizeAll(Reopen As Boolean)
    Dim MustExit As Integer
    
    If GInitialResized = 0 Then Exit Sub ' Just starting up
    If WindowState = 1 Then Exit Sub ' Minimized
    
    If ScaleWidth < 480 * Screen.TwipsPerPixelX Then
        Width = Width - ScaleWidth + 480 * Screen.TwipsPerPixelX
        MustExit = True
    End If
    If ScaleHeight < 280 * Screen.TwipsPerPixelY Then
        Height = Height - ScaleHeight + 280 * Screen.TwipsPerPixelY
        MustExit = True
    End If
    If MustExit Then Exit Sub
    
    GResizingAll = 1
    
    ' Set visibility and positions.
    frmCameraHolder.Visible = False
    MainBar.Visible = True
        
    If Ed.BrowserPos = 2 Then ' Hide.
        BrowserPanel.Visible = False
    Else
        If BrowserPanel.Visible = False Then
            BrowserPanel.Align = 0
        End If
        BrowserPanel.Visible = True ' Must do before align
        '
        If Ed.BrowserPos = 0 Then ' Right.
            BrowserPanel.Align = 4
        Else ' Left.
            BrowserPanel.Align = 3
        End If
        '
        If BrowserTopicCombo.Text <> "" Then
            Ed.ReloadBrowser
        End If
    End If
    
    If Ed.ToolbarPos = 0 Then ' Left
        Toolbar.Align = 3
    Else ' Right
        Toolbar.Align = 4
    End If
    
    Toolbar.Visible = True
    ResizeToolbar
    
    ' Position combo boxes.
    X = MainBar.Width - (TextureCombo.Left + _
        TextureCombo.Width + _
        8 * Screen.TwipsPerPixelX)
    CalcText.Left = CalcText.Left + X
    CalcButton.Left = CalcButton.Left + X
    CalcZero.Left = CalcZero.Left + X
    ModeLabel.Left = ModeLabel.Left + X
    ModeCombo.Left = ModeCombo.Left + X
    TextureLabel.Left = TextureLabel.Left + X
    TextureCombo.Left = TextureCombo.Left + X
    ActorLabel.Left = ActorLabel.Left + X
    ActorCombo.Left = ActorCombo.Left + X
    GridLabel.Left = GridLabel.Left + X
    GridCombo.Left = GridCombo.Left + X
    
    ' Camera holder.
    frmCameraHolder.SetPos
    frmCameraHolder.OpenCameras (Reopen)
    frmCameraHolder.Show

    ' Force all forms to be clipped to the newly-sized
    ' window and forced in front of cameras.
    Ed.NoteResize

End Sub

'
' UnrealEdServer callback dispatcher
'
Private Sub Callback_KeyPress(KeyAscii As Integer)
    '
    Dim N As Integer
    Dim S As String
    '
    Select Case KeyAscii - 32
    Case EDC_GENERAL:
    Case EDC_CURTEXCHANGE:
        frmMain.TextureCombo.List(0) = Ed.Server.GetProp("ED", "CURTEX")
        frmMain.TextureCombo.ListIndex = 0
        frmTexBrowser.BrowserRefresh
    Case EDC_CURCLASSCHANGE:
        frmMain.ActorCombo.List(0) = Ed.Server.GetProp("ED", "CURCLASS")
        frmMain.ActorCombo.ListIndex = 0
    Case EDC_SELPOLYCHANGE:
        If GPolyPropsAction = 1 Then
            frmSurfaceProps.GetSelectedPolys
        End If
    Case EDC_SELACTORCHANGE:
        If GActorPropsAction = AP_Selected Then
            frmActorProperties.GetSelectedActors
        ElseIf GActorPropsAction = AP_Class Then
            frmActorProperties.NoteClassChange
        End If
    Case EDC_SELBRUSHCHANGE:
    Case EDC_RTCLICKTEXTURE:
        frmMain.TextureCombo.List(0) = Ed.Server.GetProp("ED", "CURTEX")
        frmMain.TextureCombo.ListIndex = 0
        frmTexBrowser.BrowserRefresh
        Call frmTexBrowser.TextureList_MouseDown(2, 0, 0, 0)
    Case EDC_RTCLICKPOLY:
        frmPopups2.prProperties.Caption = "Surface &Properties (" & Ed.Server.GetProp("Polys", "NumSelected") & " selected)..."
        frmPopups2.prApplyTex.Caption = "Apply &Texture " & frmMain.TextureCombo.List(0)
        PopupMenu frmPopups2.PolyRtClick
    Case EDC_RTCLICKACTOR:
        N = Val(Ed.Server.GetProp("Actor", "NumSelected"))
        GPopupActorClass = Ed.Server.GetProp("Actor", "ClassSelected")
        '
        frmPopups2.arMoverKeyframe.Visible = _
            (Val(Ed.Server.GetProp("Actor", "ISKINDOF CLASS=MOVER")) = 1)
        If GPopupActorClass <> "" Then
            frmPopups2.arProps.Caption = GPopupActorClass & " &Properties (" & Trim(Str(N)) & " selected)..."
            frmPopups2.arSelectAllOfType.Caption = "Select all " & GPopupActorClass & " actors"
            frmPopups2.arSelectAllOfType.Visible = True
            frmPopups2.arRememberClass.Visible = True
            frmPopups2.arScriptMenu.Visible = True
        Else
            frmPopups2.arProps.Caption = "Actor &Properties (" & Trim(Str(N)) & " selected)..."
            frmPopups2.arSelectAllOfType.Visible = False
            frmPopups2.arRememberClass.Visible = False
            frmPopups2.arScriptMenu.Visible = False
        End If
        '
        PopupMenu frmPopups2.ActorRtClick
    Case EDC_MODECHANGE:
    Case EDC_BRUSHCHANGE:
    Case EDC_MAPCHANGE:
    Case EDC_ACTORCHANGE:
    Case EDC_RTCLICKWINDOW:
        PopupMenu frmPopups.WIndow
    End Select
    '
End Sub

Public Sub PreferencesChange()
    WorldBrowser.Visible = Ed.GodMode
End Sub

'
' Play level.
'

Private Sub PlayLevel_Click()
    PreSaveAll
    Ed.BeginSlowTask ("Saving map for play")
    Ed.Server.SlowExec "MAP SAVE FILE=" & Quotes(Ed.BaseDir & Ed.MapDir & "\Autoplay.unr")
    Ed.EndSlowTask
    Call Shell(Ed.BaseDir + "\Unreal " & _
        "FILE=" & Quotes(Ed.BaseDir & Ed.MapDir & "\Autoplay.unr") & _
        " HWND=" & Trim(Str(hwnd)), _
        vbNormalFocus)
End Sub

