VERSION 5.00
Object = "{BDC217C8-ED16-11CD-956C-0000C04E4C0A}#1.1#0"; "tabctl32.ocx"
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.1#0"; "comctl32.ocx"
Object = "{A8B3B723-0B5A-101B-B22E-00AA0037B2FC}#1.0#0"; "GRID32.OCX"
Begin VB.Form Main 
   Caption         =   "Unreal Gatekeeper"
   ClientHeight    =   5655
   ClientLeft      =   1950
   ClientTop       =   2055
   ClientWidth     =   7710
   Icon            =   "Main.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   5655
   ScaleWidth      =   7710
   Begin VB.CommandButton Command1 
      Caption         =   "&Stop"
      Height          =   255
      Left            =   6900
      TabIndex        =   63
      Top             =   60
      Visible         =   0   'False
      Width           =   795
   End
   Begin VB.ListBox LogList 
      Height          =   645
      Left            =   60
      TabIndex        =   22
      Top             =   4680
      Width           =   7635
   End
   Begin TabDlg.SSTab GateTab 
      Height          =   4575
      Left            =   60
      TabIndex        =   11
      Tag             =   "LEVELLIST"
      Top             =   60
      Width           =   7635
      _ExtentX        =   13467
      _ExtentY        =   8070
      _Version        =   327680
      Style           =   1
      Tabs            =   5
      TabsPerRow      =   5
      TabHeight       =   529
      ShowFocusRect   =   0   'False
      TabCaption(0)   =   "Levels "
      TabPicture(0)   =   "Main.frx":030A
      Tab(0).ControlCount=   7
      Tab(0).ControlEnabled=   -1  'True
      Tab(0).Control(0)=   "LevelsHolder"
      Tab(0).Control(0).Enabled=   0   'False
      Tab(0).Control(1)=   "LevelsAdd"
      Tab(0).Control(1).Enabled=   0   'False
      Tab(0).Control(2)=   "LevelsDelete"
      Tab(0).Control(2).Enabled=   0   'False
      Tab(0).Control(3)=   "LevelsProperties"
      Tab(0).Control(3).Enabled=   0   'False
      Tab(0).Control(4)=   "LevelsUpIt"
      Tab(0).Control(4).Enabled=   0   'False
      Tab(0).Control(5)=   "LevelsDownIt"
      Tab(0).Control(5).Enabled=   0   'False
      Tab(0).Control(6)=   "LevelsMessage"
      Tab(0).Control(6).Enabled=   0   'False
      TabCaption(1)   =   "Gatekeeper "
      TabPicture(1)   =   "Main.frx":0326
      Tab(1).ControlCount=   4
      Tab(1).ControlEnabled=   0   'False
      Tab(1).Control(0)=   "UplinkFrame"
      Tab(1).Control(0).Enabled=   0   'False
      Tab(1).Control(1)=   "GatekeeperMachineFrame"
      Tab(1).Control(1).Enabled=   0   'False
      Tab(1).Control(2)=   "ServerDirectoriesFrame"
      Tab(1).Control(2).Enabled=   0   'False
      Tab(1).Control(3)=   "Frame2"
      Tab(1).Control(3).Enabled=   0   'False
      TabCaption(2)   =   "Properties "
      TabPicture(2)   =   "Main.frx":0342
      Tab(2).ControlCount=   9
      Tab(2).ControlEnabled=   0   'False
      Tab(2).Control(0)=   "Label21"
      Tab(2).Control(0).Enabled=   0   'False
      Tab(2).Control(1)=   "KeyValidGridCaption"
      Tab(2).Control(1).Enabled=   0   'False
      Tab(2).Control(2)=   "ValueEntryCaption"
      Tab(2).Control(2).Enabled=   0   'False
      Tab(2).Control(3)=   "ValueEntryMsg"
      Tab(2).Control(3).Enabled=   0   'False
      Tab(2).Control(4)=   "TopicList"
      Tab(2).Control(4).Enabled=   -1  'True
      Tab(2).Control(5)=   "ValueEntry"
      Tab(2).Control(5).Enabled=   -1  'True
      Tab(2).Control(6)=   "ValueModify"
      Tab(2).Control(6).Enabled=   -1  'True
      Tab(2).Control(7)=   "TopicKeys"
      Tab(2).Control(7).Enabled=   -1  'True
      Tab(2).Control(8)=   "SaveConfig"
      Tab(2).Control(8).Enabled=   -1  'True
      TabCaption(3)   =   "Users "
      TabPicture(3)   =   "Main.frx":035E
      Tab(3).ControlCount=   6
      Tab(3).ControlEnabled=   0   'False
      Tab(3).Control(0)=   "UserBroadcast"
      Tab(3).Control(0).Enabled=   -1  'True
      Tab(3).Control(1)=   "UserMessage"
      Tab(3).Control(1).Enabled=   -1  'True
      Tab(3).Control(2)=   "UserBanish"
      Tab(3).Control(2).Enabled=   -1  'True
      Tab(3).Control(3)=   "UsersHolder"
      Tab(3).Control(3).Enabled=   -1  'True
      Tab(3).Control(4)=   "UserName"
      Tab(3).Control(4).Enabled=   0   'False
      Tab(3).Control(5)=   "Label6"
      Tab(3).Control(5).Enabled=   0   'False
      TabCaption(4)   =   "About "
      Tab(4).ControlCount=   1
      Tab(4).ControlEnabled=   0   'False
      Tab(4).Control(0)=   "AboutHolder"
      Tab(4).Control(0).Enabled=   -1  'True
      Begin VB.CommandButton SaveConfig 
         Caption         =   "&Save Config"
         Height          =   315
         Left            =   -68820
         TabIndex        =   69
         Top             =   4020
         Width           =   1215
      End
      Begin VB.CommandButton LevelsMessage 
         Caption         =   "&Message"
         Height          =   315
         Left            =   1740
         TabIndex        =   62
         Top             =   4140
         Width           =   915
      End
      Begin VB.CommandButton UserBroadcast 
         Caption         =   "&Broadcast"
         Height          =   315
         Left            =   -73800
         TabIndex        =   61
         Top             =   4140
         Width           =   1035
      End
      Begin VB.CommandButton UserMessage 
         Caption         =   "&Message"
         Height          =   315
         Left            =   -74820
         TabIndex        =   60
         Top             =   4140
         Width           =   975
      End
      Begin VB.CommandButton UserBanish 
         Caption         =   "Banish"
         Height          =   315
         Left            =   -68400
         TabIndex        =   59
         Top             =   4140
         Width           =   855
      End
      Begin VB.PictureBox UsersHolder 
         Height          =   3555
         Left            =   -74820
         ScaleHeight     =   3495
         ScaleWidth      =   7215
         TabIndex        =   57
         Top             =   480
         Width           =   7275
         Begin MSGrid.Grid Users 
            Height          =   3495
            Left            =   0
            TabIndex        =   58
            Tag             =   "User;Type;Team;Level;App;Access"
            Top             =   0
            Width           =   7215
            _Version        =   65536
            _ExtentX        =   12726
            _ExtentY        =   6165
            _StockProps     =   77
            BackColor       =   16777215
            BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
               Name            =   "MS Sans Serif"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Enabled         =   0   'False
            BorderStyle     =   0
            Cols            =   7
            FixedCols       =   0
         End
      End
      Begin VB.PictureBox AboutHolder 
         Height          =   4035
         Left            =   -74880
         ScaleHeight     =   3975
         ScaleWidth      =   7335
         TabIndex        =   45
         Top             =   420
         Width           =   7395
         Begin VB.CommandButton AboutWeb 
            Caption         =   "&Gatekeeper Home Page"
            Height          =   315
            Left            =   780
            TabIndex        =   46
            Top             =   3540
            Width           =   1935
         End
         Begin VB.Label RemoteMaker 
            Caption         =   "Unknown"
            Height          =   255
            Left            =   2580
            TabIndex        =   56
            Top             =   2940
            Width           =   4575
         End
         Begin VB.Label RemoteVersion 
            Caption         =   "Unknown"
            Height          =   255
            Left            =   2580
            TabIndex        =   55
            Top             =   2700
            Width           =   4575
         End
         Begin VB.Label Label5 
            Alignment       =   1  'Right Justify
            Caption         =   "Remote protocol version:"
            Height          =   195
            Left            =   180
            TabIndex        =   54
            Top             =   2700
            Width           =   2295
         End
         Begin VB.Label Label9 
            Alignment       =   1  'Right Justify
            Caption         =   "Remote Gatekeeper software:"
            Height          =   195
            Left            =   180
            TabIndex        =   53
            Top             =   2940
            Width           =   2295
         End
         Begin VB.Label About1 
            Alignment       =   2  'Center
            Caption         =   "Gatekeeper"
            BeginProperty Font 
               Name            =   "Arial"
               Size            =   18
               Charset         =   0
               Weight          =   700
               Underline       =   0   'False
               Italic          =   -1  'True
               Strikethrough   =   0   'False
            EndProperty
            Height          =   435
            Left            =   840
            TabIndex        =   52
            Top             =   120
            Width           =   6255
         End
         Begin VB.Label About3 
            Alignment       =   2  'Center
            Caption         =   "Reference implementation."
            Height          =   255
            Left            =   1800
            TabIndex        =   51
            Top             =   840
            Width           =   4275
         End
         Begin VB.Label About4 
            Alignment       =   2  'Center
            Caption         =   "Copyright 1996 Epic MegaGames, Inc."
            Height          =   255
            Left            =   1800
            TabIndex        =   50
            Top             =   1080
            Width           =   4275
         End
         Begin VB.Label About5 
            Alignment       =   2  'Center
            Caption         =   "Initial prototype by Tim Sweeney, 11/8/96."
            Height          =   255
            Left            =   1800
            TabIndex        =   49
            Top             =   1320
            Width           =   4275
         End
         Begin VB.Label About6 
            Caption         =   "http://www.epicgames.com/unrealgate"
            Height          =   255
            Left            =   3120
            TabIndex        =   48
            Top             =   3600
            Width           =   3135
         End
         Begin VB.Label Label4 
            Caption         =   $"Main.frx":037A
            Height          =   795
            Left            =   240
            TabIndex        =   47
            Top             =   1800
            Width           =   6855
         End
      End
      Begin VB.ListBox TopicKeys 
         Height          =   2400
         ItemData        =   "Main.frx":0484
         Left            =   -71220
         List            =   "Main.frx":0486
         Sorted          =   -1  'True
         TabIndex        =   43
         Top             =   780
         WhatsThisHelpID =   2
         Width           =   3615
      End
      Begin VB.CommandButton ValueModify 
         Caption         =   "&Update"
         Height          =   315
         Left            =   -74760
         TabIndex        =   38
         Top             =   4020
         Visible         =   0   'False
         Width           =   855
      End
      Begin VB.TextBox ValueEntry 
         Height          =   285
         Left            =   -74760
         TabIndex        =   36
         Top             =   3600
         Visible         =   0   'False
         Width           =   7155
      End
      Begin VB.ListBox TopicList 
         Height          =   2400
         Left            =   -74760
         Sorted          =   -1  'True
         TabIndex        =   33
         Tag             =   "Databases"
         Top             =   780
         WhatsThisHelpID =   1
         Width           =   3315
      End
      Begin VB.Frame Frame2 
         Caption         =   "Contact information"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   11.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   -1  'True
            Strikethrough   =   0   'False
         EndProperty
         Height          =   1575
         Left            =   -71100
         TabIndex        =   24
         Top             =   2880
         Width           =   3615
         Begin VB.TextBox ContactWeb 
            Height          =   285
            Left            =   1200
            TabIndex        =   28
            Tag             =   "Gate:ContactWeb"
            Top             =   1200
            Width           =   2295
         End
         Begin VB.TextBox ContactOrganization 
            Height          =   285
            Left            =   1200
            TabIndex        =   27
            Tag             =   "Gate:ContactOrganization"
            Top             =   900
            Width           =   2295
         End
         Begin VB.TextBox ContactEmail 
            Height          =   285
            Left            =   1200
            TabIndex        =   26
            Tag             =   "Gate:ContactEmail"
            Top             =   600
            Width           =   2295
         End
         Begin VB.TextBox ContactName 
            Height          =   285
            Left            =   1200
            TabIndex        =   25
            Tag             =   "Gate:ContactName"
            Top             =   300
            Width           =   2295
         End
         Begin VB.Label Label20 
            Alignment       =   1  'Right Justify
            Caption         =   "Email:"
            Height          =   255
            Left            =   60
            TabIndex        =   32
            Top             =   600
            Width           =   1035
         End
         Begin VB.Label Label19 
            Alignment       =   1  'Right Justify
            Caption         =   "Organization:"
            Height          =   255
            Left            =   60
            TabIndex        =   31
            Top             =   900
            Width           =   1035
         End
         Begin VB.Label Label18 
            Alignment       =   1  'Right Justify
            Caption         =   "Web page:"
            Height          =   255
            Left            =   60
            TabIndex        =   30
            Top             =   1200
            Width           =   1035
         End
         Begin VB.Label Text95 
            Alignment       =   1  'Right Justify
            Caption         =   "Name:"
            Height          =   255
            Left            =   60
            TabIndex        =   29
            Top             =   300
            Width           =   1035
         End
      End
      Begin VB.CommandButton LevelsDownIt 
         Caption         =   "&Down It"
         Height          =   315
         Left            =   3960
         TabIndex        =   21
         Top             =   4140
         Width           =   795
      End
      Begin VB.CommandButton LevelsUpIt 
         Caption         =   "&Up It"
         Height          =   315
         Left            =   3240
         TabIndex        =   20
         Top             =   4140
         Width           =   675
      End
      Begin VB.CommandButton LevelsProperties 
         Caption         =   "&Level Properties..."
         Height          =   315
         Left            =   180
         TabIndex        =   19
         Top             =   4140
         Width           =   1515
      End
      Begin VB.Frame ServerDirectoriesFrame 
         Caption         =   "Server's Level Directories"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   11.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   -1  'True
            Strikethrough   =   0   'False
         EndProperty
         Height          =   2355
         Left            =   -71100
         TabIndex        =   18
         Top             =   420
         Width           =   3615
         Begin VB.CommandButton GatekeeperLevelDirsEdit 
            Caption         =   "&Edit"
            Height          =   255
            Left            =   1620
            TabIndex        =   40
            Top             =   1980
            Width           =   615
         End
         Begin VB.ListBox LevelDirectoryList 
            Height          =   1425
            Left            =   120
            Sorted          =   -1  'True
            TabIndex        =   23
            Tag             =   "Gate:LevelDirectoryList"
            Top             =   300
            Width           =   3375
         End
         Begin VB.CommandButton GatekeeperLevelDirsAdd 
            Caption         =   "Add"
            Height          =   255
            Left            =   120
            TabIndex        =   9
            Top             =   1980
            Width           =   615
         End
         Begin VB.CommandButton GatekeeperLevelDirsDelete 
            Caption         =   "Delete"
            Height          =   255
            Left            =   780
            TabIndex        =   10
            Top             =   1980
            Width           =   795
         End
         Begin VB.Label Label3 
            Caption         =   "Example: c:\Unreal\*.unr"
            Height          =   195
            Left            =   120
            TabIndex        =   68
            Top             =   1740
            Width           =   3375
         End
      End
      Begin VB.Frame GatekeeperMachineFrame 
         Caption         =   "Gatekeeper Information"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   11.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   -1  'True
            Strikethrough   =   0   'False
         EndProperty
         Height          =   1755
         Left            =   -74880
         TabIndex        =   15
         Top             =   420
         Width           =   3615
         Begin VB.CheckBox IsUplinked 
            Caption         =   "This Gatekeeper is uplinked."
            Height          =   255
            Left            =   120
            TabIndex        =   41
            Tag             =   "Gate:IsUplinked"
            Top             =   1380
            Width           =   3435
         End
         Begin VB.TextBox Port 
            Height          =   285
            Left            =   1920
            TabIndex        =   8
            Tag             =   "Gate:Port"
            Top             =   960
            Width           =   1575
         End
         Begin VB.TextBox GatekeeperName 
            Height          =   285
            Left            =   1920
            TabIndex        =   7
            Tag             =   "Gate:GatekeeperName"
            Top             =   540
            Width           =   1575
         End
         Begin VB.Label Label10 
            Alignment       =   1  'Right Justify
            Caption         =   "Gatekeeper TCP Port: "
            Height          =   195
            Left            =   120
            TabIndex        =   17
            Top             =   960
            Width           =   1695
         End
         Begin VB.Label Label1 
            Alignment       =   1  'Right Justify
            Caption         =   "Gatekeeper Name:"
            Height          =   195
            Left            =   120
            TabIndex        =   16
            Top             =   540
            Width           =   1635
         End
      End
      Begin VB.Frame UplinkFrame 
         Caption         =   "Uplink search order"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   11.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   -1  'True
            Strikethrough   =   0   'False
         EndProperty
         Height          =   2175
         Left            =   -74880
         TabIndex        =   14
         Top             =   2280
         Width           =   3615
         Begin VB.CommandButton GatekeeperMastersEdit 
            Caption         =   "&Edit"
            Height          =   255
            Left            =   1440
            TabIndex        =   39
            Top             =   1800
            Width           =   615
         End
         Begin VB.CommandButton GatekeeperMastersUp 
            Caption         =   "Up"
            Height          =   255
            Left            =   2280
            TabIndex        =   5
            Top             =   1800
            Width           =   495
         End
         Begin VB.CommandButton GatekeeperMastersDown 
            Caption         =   "Down"
            Height          =   255
            Left            =   2820
            TabIndex        =   6
            Top             =   1800
            Width           =   675
         End
         Begin VB.ListBox UplinkList 
            Height          =   1230
            ItemData        =   "Main.frx":0488
            Left            =   120
            List            =   "Main.frx":048F
            TabIndex        =   2
            Tag             =   "Gate:UplinkList"
            Top             =   540
            Width           =   3375
         End
         Begin VB.CommandButton GatekeeperMastersDelete 
            Caption         =   "Delete"
            Height          =   255
            Left            =   720
            TabIndex        =   4
            Top             =   1800
            Width           =   675
         End
         Begin VB.CommandButton GatekeeperMastersAdd 
            Caption         =   "Add"
            Height          =   255
            Left            =   120
            TabIndex        =   3
            Top             =   1800
            Width           =   555
         End
         Begin VB.Label Label2 
            Caption         =   "Attempt to connect in the following order:"
            Height          =   195
            Left            =   120
            TabIndex        =   42
            Top             =   300
            Width           =   3375
         End
      End
      Begin VB.CommandButton LevelsDelete 
         Caption         =   "Delete Level"
         Height          =   315
         Left            =   6240
         TabIndex        =   1
         Top             =   4140
         Width           =   1215
      End
      Begin VB.CommandButton LevelsAdd 
         Caption         =   "&Add Level"
         Height          =   315
         Left            =   5160
         TabIndex        =   0
         Top             =   4140
         Width           =   1035
      End
      Begin VB.PictureBox LevelsHolder 
         Height          =   3555
         Left            =   180
         ScaleHeight     =   3495
         ScaleWidth      =   7215
         TabIndex        =   12
         TabStop         =   0   'False
         Top             =   480
         Width           =   7275
         Begin MSGrid.Grid Levels 
            Height          =   3495
            Left            =   0
            TabIndex        =   13
            Tag             =   "Level;State;RequestState;Action;Users;Visits"
            Top             =   0
            Width           =   7455
            _Version        =   65536
            _ExtentX        =   13150
            _ExtentY        =   6165
            _StockProps     =   77
            BackColor       =   16777215
            BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
               Name            =   "MS Sans Serif"
               Size            =   8.25
               Charset         =   0
               Weight          =   400
               Underline       =   0   'False
               Italic          =   0   'False
               Strikethrough   =   0   'False
            EndProperty
            Enabled         =   0   'False
            BorderStyle     =   0
            Cols            =   8
            FixedCols       =   0
            ScrollBars      =   2
         End
      End
      Begin VB.Label UserName 
         Caption         =   "Unknown"
         Height          =   195
         Left            =   -71880
         TabIndex        =   67
         Top             =   4200
         Width           =   2295
      End
      Begin VB.Label Label6 
         Alignment       =   1  'Right Justify
         Caption         =   "You are:"
         Height          =   195
         Left            =   -72780
         TabIndex        =   66
         Top             =   4200
         Width           =   795
      End
      Begin VB.Label ValueEntryMsg 
         Caption         =   "Note that some keys are not modifyable. If you attempt to modify one of them, a failure message will appear in the log below."
         Height          =   495
         Left            =   -73680
         TabIndex        =   44
         Top             =   3960
         Visible         =   0   'False
         Width           =   4815
      End
      Begin VB.Label ValueEntryCaption 
         Caption         =   "database.key value (type):"
         Height          =   195
         Left            =   -74760
         TabIndex        =   37
         Top             =   3360
         Visible         =   0   'False
         Width           =   7155
      End
      Begin VB.Label KeyValidGridCaption 
         Caption         =   "Topics keys:"
         Height          =   195
         Left            =   -71220
         TabIndex        =   35
         Top             =   540
         Width           =   3555
      End
      Begin VB.Label Label21 
         Caption         =   "Gatekeeper databases:"
         Height          =   195
         Left            =   -74760
         TabIndex        =   34
         Top             =   540
         Width           =   3195
      End
   End
   Begin ComctlLib.ProgressBar Progress 
      Height          =   270
      Left            =   5550
      TabIndex        =   65
      Top             =   5400
      Visible         =   0   'False
      Width           =   1950
      _ExtentX        =   3440
      _ExtentY        =   476
      _Version        =   327680
      BorderStyle     =   1
      Appearance      =   1
   End
   Begin ComctlLib.StatusBar StatusBar1 
      Align           =   2  'Align Bottom
      Height          =   270
      Left            =   0
      TabIndex        =   64
      Top             =   5385
      Visible         =   0   'False
      Width           =   7710
      _ExtentX        =   13600
      _ExtentY        =   476
      SimpleText      =   ""
      _Version        =   327680
      BeginProperty Panels {0713E89E-850A-101B-AFC0-4210102A8DA7} 
         NumPanels       =   3
         BeginProperty Panel1 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            Object.Width           =   3528
            MinWidth        =   3528
            Text            =   "Not Connected"
            TextSave        =   "Not Connected"
            Object.Tag             =   ""
         EndProperty
         BeginProperty Panel2 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            Object.Width           =   6174
            MinWidth        =   6174
            TextSave        =   ""
            Object.Tag             =   ""
         EndProperty
         BeginProperty Panel3 {0713E89F-850A-101B-AFC0-4210102A8DA7} 
            AutoSize        =   1
            Object.Width           =   3352
            MinWidth        =   1764
            TextSave        =   ""
            Object.Tag             =   ""
         EndProperty
      EndProperty
   End
End
Attribute VB_Name = "Main"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' Main.frm: GateClient main user interface.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Presents the main user interface for the GateClient,
'   the program responsible for administering local and
'   remote Gatekeeper servers.  The interface takes the
'   form of a tabbed dialog.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

' Flag for preventing recursion while refreshing grids.
Private RefreshingKeys

' Original width of form for resizing.
Private OriginalWidth As Long

' Datatype in ValueEntry textbox.
Private ValueEntryDataType As String
Private ValueEntryOriginal As String

' Maximum number of lines of log history we keep.
Private Const MAX_LOG_ITEMS = 150

'/////////////////////////////////////////////////////////
' Standard form events.
'/////////////////////////////////////////////////////////

'
' Standard load event, occurs once a server connection has
' been established.
'
Private Sub Form_Load()
    
    ' Remeber sizing for Resize.
    OriginalWidth = Width
End Sub

'
' Public startup.
'
Public Sub Startup()

    ' Subscribe to desired topics.
    Call Connect.Subscribe("Gate", Me, True)
    Call Connect.Subscribe("Databases SingleRecord=1", TopicList, False)
    Call Connect.Subscribe("Users:" & Users.Tag, Users, False)
    Call Connect.Subscribe("Levels:" & Levels.Tag, Levels, False)

    ' Set user name.
    UserName.Caption = LocalGateClient.ResultUserName

    ' Refresh tabs.
    LevelsTabRefresh
    UsersTabRefresh

    ' For debugging.
    LocalGateClient.AsyncExec "VERBOSE" 'todo: Comment out.

    ' Show me.
    Show
End Sub

'
' Standard unload event.  We handle GateClient application
' shutdown here.
'
Private Sub Form_Unload(Cancel As Integer)

    If LocalGateClient.IsConnected Then
        ' We are connected to a server.
    
        ' Query the user if there are users logged in.
        If LocalGateClient.IsLocal Then
            If LocalGateServer.NumLoggedInUsers > 1 Then
                If MsgBox("There are currently " & Trim(Str(LocalGateServer.NumLoggedInUsers - 1)) & _
                    " user(s) logged in or logging in to the local GateServer (other than yourself). " & _
                    "Are you sure you want to exit and kick them off?", vbOKCancel) = vbCancel Then
                    Cancel = 1
                    Exit Sub
                    End If
            End If
        End If

        ' Unload all forms.
        Dim i As Integer
        For i = Forms.Count - 1 To 0 Step -1
            Unload Forms(i)
        Next
            
        ' Unload the local gate client.
        Set LocalGateClient = Nothing
    End If

    ' Unload the local gate server.
    Set LocalGateServer = Nothing
    
    ' Close the TCP control.
    On Error GoTo SocketError
    Close
SocketError:

    ' Successfully unloaded.
End Sub

'
' Handle form resizing.
'
Private Sub Form_Resize()
    
    ' Don't mess with it if minimized.
    If WindowState <> 0 Then Exit Sub
    
    ' Prevent recursion.
    Static Resizing As Boolean
    If Not Resizing Then
        Resizing = True
        
        ' Prevent width from changing.
        Width = OriginalWidth
        
        ' Figure out new height of log listbox.
        Dim NewLogHeight As Integer
        NewLogHeight = ScaleHeight - LogList.Top
        If NewLogHeight <= 0 Then
            LogList.Visible = False
            If ScaleHeight < LogList.Top Then
                Height = Height - ScaleHeight + LogList.Top
            End If
        Else
            LogList.Visible = True
            LogList.Height = NewLogHeight
            Height = Height - ScaleHeight + LogList.Top + LogList.Height
        End If
        Resizing = False
    End If
End Sub

'/////////////////////////////////////////////////////////
' Logging.
'/////////////////////////////////////////////////////////

'
' Output some text in the log list.
'
Public Sub Log(S As String)

    ' Add to the log listbox.
    LogList.AddItem S
    
    ' Prevent overflowing.
    If LogList.ListCount > MAX_LOG_ITEMS Then
        LogList.RemoveItem 0
    End If
    
    ' Go to last item if we were previously at the last.
    If LogList.ListIndex = LogList.ListCount - 2 Then
        LogList.ListIndex = LogList.ListCount - 1
    End If
End Sub

'/////////////////////////////////////////////////////////
' The tab control.
'/////////////////////////////////////////////////////////

'
' Handle the user clicking on the tab control.
'
Private Sub GateTab_Click(PrevTab As Integer)
    Select Case GateTab.Tab
        Case 0 ' Levels
            FixLevelSelection
        Case 1 ' Gatekeeper
        Case 2 ' Properties
        Case 3 ' Users
            FixUserSelection
        Case 4 ' About
    End Select
End Sub

'/////////////////////////////////////////////////////////
' Levels tab accessors.
'/////////////////////////////////////////////////////////

'
' Return the number of levels in the grid.
'
Public Function NumLevels() As Long
    If Levels.Enabled Then
        NumLevels = Levels.Rows - 1
    Else
        NumLevels = 0
    End If
End Function

'
' Return whether there is a valid current level
' in the Levels grid.
'
Private Function GetLevelValidity() As Boolean
    GetLevelValidity = NumLevels > 0
End Function

'
' Return the current level's name.
' Only valid if GetLevelValidity() is true.
'
Private Function GetLevelName() As String
    
    ' Inhibit FixLevelSelection.
    RefreshingGrid = RefreshingGrid + 1
    
    ' Grab the level name.
    Levels.Col = 0
    GetLevelName = Levels.Text

    ' Refresh the grid selection.
    RefreshingGrid = RefreshingGrid - 1
    FixLevelSelection
End Function

'
' Return the current level's player count.
' Only valid if GetLevelValidity() is true.
'
Private Function GetLevelPlayerCount() As Long
    
    ' Inhibit FixLevelSelection.
    RefreshingGrid = RefreshingGrid + 1
    
    ' Grab the level name.
    Levels.Col = 4
    GetLevelPlayerCount = Val(Levels.Text)

    ' Refresh the grid selection.
    RefreshingGrid = RefreshingGrid - 1
    FixLevelSelection
End Function

'/////////////////////////////////////////////////////////
' Levels tab implementation.
'/////////////////////////////////////////////////////////

'
' Send a message to all Users in the level.
'
Private Sub LevelMessage_Click()

End Sub

'
' Refresh the level list.
'
Private Sub LevelsTabRefresh()
    Dim Unit As Long, i As Integer
    
    ' Inhibit FixLevelSelection.
    RefreshingGrid = RefreshingGrid + 1
    
    ' Compute sizing info.
    Levels.Width = LevelsHolder.ScaleWidth + 1200
    Levels.Height = LevelsHolder.ScaleHeight
    Unit = LevelsHolder.ScaleWidth / 8
    
    ' Set all column headings and sizes.
    Levels.Row = 0
    '
    Levels.Col = 0
    Levels.ColWidth(0) = LevelsHolder.ScaleWidth - 6 * Unit - 4 * Screen.TwipsPerPixelX
    Levels.Text = "Level"
    '
    Levels.Col = 1
    Levels.ColWidth(1) = Unit
    Levels.Text = "State"
    '
    Levels.Col = 2
    Levels.ColWidth(2) = Unit
    Levels.Text = "Request"
    '
    Levels.Col = 3
    Levels.ColWidth(3) = Unit * 2
    Levels.Text = "Action"
    '
    Levels.Col = 4
    Levels.ColWidth(4) = Unit
    Levels.Text = "Users"
    '
    Levels.Col = 5
    Levels.ColWidth(5) = Unit
    Levels.Text = "Visits"
    '
    Levels.Col = 6
    Levels.ColWidth(6) = 100

    ' Reactivate FixLevelSelection.
    RefreshingGrid = RefreshingGrid - 1
    FixLevelSelection
End Sub

'
' Update the selected region of the grid.  This is called
' in response to the user messing with the grid with the
' mouse or keyboard.
'
Private Sub FixLevelSelection()
    Dim State As String, RequestState As String
    
    If RefreshingGrid = 0 Then
        RefreshingGrid = 1
        
        ' Set rows.
        If NumLevels > 0 Then
        
            ' There are levels in the list.
            Levels.SelStartCol = 0
            Levels.SelEndCol = 6
            
            ' Set row.
            If Levels.SelStartRow < Levels.Row Then
                Levels.Row = Levels.SelStartRow
            Else
                Levels.Row = Levels.SelEndRow
            End If
            
            ' Set selection
            If Levels.Row > 0 Then
                Levels.SelStartRow = Levels.Row
                Levels.SelEndRow = Levels.Row
            Else
                Levels.SelStartRow = 1
                Levels.SelEndRow = 1
            End If
            
            ' Get state and request state.
            Levels.Col = 1: State = Levels.Text
            Levels.Col = 2: RequestState = Levels.Text
        Else

            ' The list is empty.
            Levels.SelStartCol = 6
            Levels.SelEndCol = 6
        End If

        ' Set position.
        Levels.Col = 6
        RefreshingGrid = 0

        ' Set level related buttons.
        LevelsProperties.Enabled = NumLevels > 0
        LevelsMessage.Enabled = NumLevels > 0
        LevelsDelete.Enabled = UCase(State) = "DOWN" And UCase(RequestState) <> "UP"
        LevelsUpIt.Enabled = UCase(State) = "DOWN" And UCase(RequestState) <> "UP"
        LevelsDownIt.Enabled = UCase(State) = "UP" And UCase(RequestState) <> "DOWN"
    End If
End Sub

'
' Mouse click on the levels grid.
'
Private Sub Levels_Click()
    FixLevelSelection
End Sub

Private Sub Levels_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    FixLevelSelection
    If Button And vbRightButton And GetLevelValidity Then
        ' Set popup menu properties.
        
        ' Set level name.
        Popups.LevelProperties.Caption = GetLevelName & " &Properties..."
        
        ' Show popup menu.
        PopupMenu Popups.Level
        
        ' Note that the code in the Popups form will
        ' route popup menu commands to the appropriate
        ' button Click handlers in this form.
    End If
End Sub

'
' Mouse move above the levels grid.
'
Private Sub Levels_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    FixLevelSelection
End Sub

'
' Mouse button release above the levels grid.
'
Private Sub Levels_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    FixLevelSelection
End Sub

'
' Row/column change above the levels grid.
'
Private Sub Levels_RowColChange()
    FixLevelSelection
End Sub

'
' Selection change above the levels grid.
'
Private Sub Levels_SelChange()
    FixLevelSelection
End Sub

'
' Attempt to add a new level to the server's
' level list.
'
Public Sub LevelsAdd_Click()
    Dim NewLevel As String
    
    ' Use remote file dialog to show the user a
    ' list of remote levels.
    If RemoteFiles.DoModal("LevelFiles", _
        "Add a new level", NewLevel) Then
        
        ' Add it.
        Call LocalGateClient.AsyncExec("AddLevel " & NewLevel)
        
        ' Save configuration.
        LocalGateClient.AsyncExec "SaveConfig"
    End If
End Sub

'
' Delete the level.
'
Public Sub LevelsDelete_Click()
    If GetLevelValidity Then
        
        If MsgBox("Are you sure you want to delete level " & _
            GetLevelName & "?", vbOKCancel) = vbOK Then
            
            ' Delete it.
            Call LocalGateClient.AsyncExec("DeleteLevel " & GetLevelName)
        
            ' Save configuration.
            LocalGateClient.AsyncExec "SaveConfig"
        End If
    End If
End Sub

'
' Shut the level down.
'
Public Sub LevelsDownIt_Click()
    If GetLevelValidity Then
        If GetLevelPlayerCount > 0 Then
            
            ' Prompt before downing level with players.
            If MsgBox("Level " & GetLevelName & " contains " & _
                GetLevelPlayerCount & " players. Are you sure you want " & _
                "to shut the level down and kick the players out?", _
                vbOKCancel) <> vbOK Then
                
                ' Aborted.
                Exit Sub
            End If
        End If
        LocalGateClient.AsyncExec "DownLevel " & GetLevelName
    End If
End Sub

'
' Send a message to everyone in a level.
'
Public Sub LevelsMessage_Click()
    Dim Message As String
    
    If GetLevelValidity Then
        If ThingEntry.GetString( _
            "Send a message to all users in " & GetLevelName, _
            "Enter message to broadcast to all users in " & GetLevelName & ":", _
            Message) Then
            
            ' Send it.
            If Message <> "" Then
                LocalGateClient.AsyncExec "Tell " & _
                    GetLevelName & Quotes(Message)
            End If
        End If
    End If
End Sub

'
' View/edit the level's properties.
'
Public Sub LevelsProperties_Click()
    If GetLevelValidity Then
        
        ' See if a level properties form already exists
        ' for this level.
        Dim i As Integer
        For i = 0 To Forms.Count - 1
            If Forms(i).Tag = GetLevelName Then
                
                ' This form is already open.
                Forms(i).SetFocus
                Exit Sub
            End If
        Next
        
        ' Open up a new form.
        Dim NewForm As New LevelInfo
        NewForm.Tag = GetLevelName
        NewForm.Startup
        NewForm.Show
        
    End If
End Sub

'
' Bring a level up.
'
Public Sub LevelsUpIt_Click()
    If GetLevelValidity Then
        LocalGateClient.AsyncExec "UpLevel " & GetLevelName
    End If
End Sub

'/////////////////////////////////////////////////////////
' Gatekeeper tab implementation.
'/////////////////////////////////////////////////////////

'
' All TextBox exchangers.
'

Private Sub GatekeeperName_LostFocus()
    Call Connect.UpdateControl(GatekeeperName, ValidateName(GatekeeperName.Text, "name"), "")
End Sub

Private Sub Port_LostFocus()
    Call Connect.UpdateControl(Port, ValidateNumber(Port.Text, "port number"), "")
End Sub

Private Sub ContactEmail_LostFocus()
    Call Connect.UpdateControl(ContactEmail, True, "")
End Sub

Private Sub ContactName_LostFocus()
    Call Connect.UpdateControl(ContactName, True, "")
End Sub

Private Sub ContactOrganization_LostFocus()
    Call Connect.UpdateControl(ContactOrganization, True, "")
End Sub

Private Sub ContactWeb_LostFocus()
    Call Connect.UpdateControl(ContactWeb, True, "")
End Sub

'
' TextBox Changers
'
Private Sub IsUplinked_Click()
    Call Connect.UpdateControl(IsUplinked, True, "")
End Sub

'
' Master server list
'

' Parse the current item.
Private Sub GatekeeperGetCurrent(ByRef Site As String, _
    ByRef Port As Long, ByRef Password As String)
    
    ' Get current item string.
    Dim S As String
    S = UplinkList.List(UplinkList.ListIndex)
    
    ' Grab the appropriate parts.
    Site = NextSTRING(S)
    Port = Val(NextSTRING(S))
    Password = NextSTRING(S)
End Sub

' Add a master server button.
Private Sub GatekeeperMastersAdd_Click()
    ' Add it.
    Dim NewSite As String, NewPort As Long, NewPassword As String
    NewPort = DEFAULT_GATE_PORT
    If SiteEntry.GetRemoteSite(NewSite, NewPort, NewPassword) Then
    
        ' Add to list.
        UplinkList.AddItem NewSite & " " & NewPort & " " & NewPassword
        
        ' Update the remote.
        Call Connect.UpdateControl(UplinkList, True, "")
    
        ' Save configuration.
        LocalGateClient.AsyncExec "SaveConfig"
    End If
End Sub

' Delete a master server button.
Private Sub GatekeeperMastersDelete_Click()
    If _
        UplinkList.ListIndex >= 0 And _
        UplinkList.ListCount > 0 Then
        
        ' Get info.
        Dim Site As String, Port As Long, Password As String
        Call GatekeeperGetCurrent(Site, Port, Password)
        
        ' Confirm deletion.
        If MsgBox("Are you sure you want to delete uplink " & _
            Site & " from the list?", vbYesNo) = vbYes Then
        
            ' Get new list index.
            Dim NewIndex As Integer
            NewIndex = UplinkList.ListIndex - 1
            If NewIndex < 0 And UplinkList.ListCount > 1 Then NewIndex = 0
            
            ' Delete it.
            UplinkList.RemoveItem UplinkList.ListIndex
            UplinkList.ListIndex = NewIndex
        
            ' Update the list.
            Call Connect.UpdateControl(UplinkList, True, "")
        
            ' Save configuration.
            LocalGateClient.AsyncExec "SaveConfig"
        End If
    End If
End Sub

' Edit this master server list item.
Private Sub GatekeeperMastersEdit_Click()
    If _
        UplinkList.ListIndex >= 0 And _
        UplinkList.ListCount > 0 Then

        ' Get info.
        Dim Site As String, Port As Long, Password As String
        Call GatekeeperGetCurrent(Site, Port, Password)
    
        ' Edit with the remote site entry dialog.
        If SiteEntry.GetRemoteSite(Site, Port, Password) Then
        
            ' Update list.
            UplinkList.List(UplinkList.ListIndex) = _
                Site & " " & Port & " " & Password
            
            ' Update the remote.
            Call Connect.UpdateControl(UplinkList, True, "")
        
            ' Save configuration.
            LocalGateClient.AsyncExec "SaveConfig"
        End If
    End If
End Sub

' Move this master server down button.
Private Sub GatekeeperMastersDown_Click()
    If _
        UplinkList.ListIndex >= 0 And _
        UplinkList.ListCount > 0 And _
        UplinkList.ListIndex < (UplinkList.ListCount - 1) Then
        
        ' Move it down.
        Dim Temp As String
        Temp = UplinkList.List(UplinkList.ListIndex)
        UplinkList.List(UplinkList.ListIndex) = UplinkList.List(UplinkList.ListIndex + 1)
        UplinkList.List(UplinkList.ListIndex + 1) = Temp
        UplinkList.ListIndex = UplinkList.ListIndex + 1
    
        ' Update the list.
        Call Connect.UpdateControl(UplinkList, True, "")
    
        ' Save configuration.
        LocalGateClient.AsyncExec "SaveConfig"
    End If
End Sub

' Move this master server up button.
Private Sub GatekeeperMastersUp_Click()
    If _
        UplinkList.ListIndex > 0 And _
        UplinkList.ListCount > 0 Then
        
        ' Move it up.
        Dim Temp As String
        Temp = UplinkList.List(UplinkList.ListIndex)
        UplinkList.List(UplinkList.ListIndex) = UplinkList.List(UplinkList.ListIndex - 1)
        UplinkList.List(UplinkList.ListIndex - 1) = Temp
        UplinkList.ListIndex = UplinkList.ListIndex - 1
        
        ' Update the list.
        Call Connect.UpdateControl(UplinkList, True, "")
        
        ' Save configuration.
        LocalGateClient.AsyncExec "SaveConfig"
    End If
End Sub

'
' Level directories list.
'

' Add a directory to the list.
Private Sub GatekeeperLevelDirsAdd_Click()
    
    ' Add it.
    Dim NewDir As String
    If ThingEntry.GetListItem("Add a level directory", _
        "Enter full path of the new directory, (for example, c:\Unreal\Maps):", _
        "directory name", NewDir) Then
        
        ' Add to list.
        LevelDirectoryList.AddItem NewDir
        
        ' Update remote.
        Call Connect.UpdateControl(LevelDirectoryList, True, "")
    
        ' Save configuration.
        LocalGateClient.AsyncExec "SaveConfig"
    End If
End Sub

' Delete a directory from the list.
Private Sub GatekeeperLevelDirsDelete_Click()
    If _
        LevelDirectoryList.ListIndex >= 0 And _
        LevelDirectoryList.ListCount > 0 Then
        
        ' Confirm deletion.
        If MsgBox("Are you sure you want to delete directory " & _
            LevelDirectoryList.List(LevelDirectoryList.ListIndex) & _
            " from the list?", vbYesNo) = vbYes Then
        
            ' Get new list index.
            Dim NewIndex As Integer
            NewIndex = LevelDirectoryList.ListIndex - 1
            If NewIndex < 0 And LevelDirectoryList.ListCount > 1 Then NewIndex = 0
            
            ' Delete it.
            LevelDirectoryList.RemoveItem LevelDirectoryList.ListIndex
            LevelDirectoryList.ListIndex = NewIndex
        
            ' Update the list.
            Call Connect.UpdateControl(LevelDirectoryList, True, "")
        
            ' Save configuration.
            LocalGateClient.AsyncExec "SaveConfig"
        End If
    End If
End Sub

' Edit a directory entry.
Private Sub GatekeeperLevelDirsEdit_Click()

    If _
        LevelDirectoryList.ListIndex >= 0 And _
        LevelDirectoryList.ListCount > 0 Then
        
        ' Edit it.
        Dim NewDir As String
        NewDir = LevelDirectoryList.List(LevelDirectoryList.ListIndex)
        If ThingEntry.GetListItem("Add a level directory", _
            "Enter full path of the new directory, (for example, c:\Unreal\Maps):", _
            "directory name", NewDir) Then
    
            ' Update the item.
            LevelDirectoryList.List(LevelDirectoryList.ListIndex) = NewDir
    
            ' Update the list.
            Call Connect.UpdateControl(LevelDirectoryList, True, "")
        
            ' Save configuration.
            LocalGateClient.AsyncExec "SaveConfig"
        End If
    End If
End Sub

'/////////////////////////////////////////////////////////
' Properties tab implementation.
'/////////////////////////////////////////////////////////

'
' Update visibility the value entry box.
'
Private Sub UpdateEntryBox()
    Dim DoShow As Boolean
    Dim Temp As String
    Dim Key As String

    If RefreshingKeys Then Exit Sub

    ' Figure out whether to show or hide.
    DoShow = _
        TopicKeys.ListIndex >= 0 And _
        TopicList.ListIndex >= 0

    ' Show or hide them.
    ValueEntryCaption.Visible = DoShow
    ValueEntry.Visible = DoShow
    ValueModify.Visible = DoShow
    ValueEntryMsg.Visible = DoShow

    If DoShow Then
    
        ' Parse Key and Datatype.
        Temp = TopicKeys.List(TopicKeys.ListIndex)
        Key = NextSTRING(Temp, Chr(9))
        While Left(Temp, 1) = Chr(9)
            Temp = Mid(Temp, 2)
        Wend
        ValueEntryDataType = UCase(NextSTRING(Temp))
        
        ' Handle datatype
        If ValueEntryDataType = "P" Then
            ' Password entry.
            ValueEntry.PasswordChar = "*"
        Else
            ' Any other entry.
            ValueEntry.PasswordChar = ""
        End If
    
        ' Update the message.
        ValueEntryCaption.Caption = _
            Key & " (" & _
            TypeDescription(ValueEntryDataType) & "):"
            
        ' Remember the datatype.
    End If
End Sub

'
' The topic we're viewing has changed, so update
' the list of keys for this topic.
'
Private Sub TopicList_Click()
    Dim NewTopic As String, Temp As String, Pair As String
    Dim Keys As String, Types As String, Key As String
    Dim Value As String
    Dim i As Integer
    
    ' Inhibit ui during update.
    RefreshingKeys = True
    
    If TopicList.ListCount > 0 And _
        TopicList.ListIndex >= 0 Then
        
        ' Get the newly selected topic name.
        Temp = TopicList.List(TopicList.ListIndex)
        NewTopic = NextSTRING(Temp, Chr(9))
        
        ' Only update if topic name has changed.
        If UCase(NewTopic) <> UCase(TopicKeys.Tag) Then
            
            ' Update keys list box caption.
            KeyValidGridCaption.Caption = NewTopic & " keys:"
            
            ' Get field keys and types.
            
            ' Remove leading tabs.
            While Left(Temp, 1) = Chr(9)
                Temp = Mid(Temp, 2)
            Wend
            
            ' Check all key=value pairs.
            While Temp <> ""
                Key = UCase(NextSTRING(Temp, "="))
                Value = NextSTRING(Temp, " ")
                If Key = "FIELDKEYS" Then
                    Keys = Value
                ElseIf Key = "FIELDTYPES" Then
                    Types = Value
                End If
            Wend
            
            ' Fill in keys listbox.
            TopicKeys.Clear
            
            Call NextSTRING(Keys, ";")
            Call NextSTRING(Types, ";")
            While Keys <> ""
                TopicKeys.AddItem NextSTRING(Keys, ";") & _
                    Chr(9) & Chr(9) & Chr(9) & _
                    Chr(9) & Chr(9) & Chr(9) & _
                    NextSTRING(Types, ";")
            Wend
        End If
    End If
    
    ' Update ui.
    RefreshingKeys = False
    UpdateEntryBox
End Sub

'
' Clicked on a key name, meaning the user wants to
' view/modify the key value.
'
Private Sub TopicKeys_Click()
    Dim NewKey As String, Temp As String
    Dim NewDatabase As String
    
    If TopicKeys.ListCount > 0 And _
        TopicKeys.ListIndex >= 0 And _
        TopicList.ListIndex >= 0 Then
        
        ' Get database name.
        Temp = TopicList.List(TopicList.ListIndex)
        NewDatabase = NextSTRING(Temp, Chr(9))
        
        ' Get key name.
        Temp = TopicKeys.List(TopicKeys.ListIndex)
        NewKey = NextSTRING(Temp, Chr(9))

        ' Get new datatype.
        Dim NewType As String
        While Left(Temp, 1) = Chr(9)
            Temp = Mid(Temp, 2)
        Wend
        NewType = UCase(NextSTRING(Temp))

        ' Get new tag.
        Dim NewTag As String
        NewTag = NewDatabase & ":" & NewKey
        
        ' Only update if key name has changed.
        If UCase(NewTag) <> UCase(ValueEntry.Tag) Then
            
            ' Unsubscribe from old.
            If Tag <> "" Then
                Call Connect.Unsubscribe(ValueEntry.Tag, Me)
            End If

            ' Set the entry box tag.
            ValueEntry.Tag = NewTag
            
            ' Empty it.
            ValueEntryOriginal = _
                IIf(NewType = "I" Or _
                NewType = "F" Or _
                NewType = "B", "0", "")
            ValueEntry.Text = ValueEntryOriginal
            
            ' Subscribe to new.
            Call Connect.Subscribe(NewTag, Me, True)
        
            ' Set focus to it.
            ValueEntry.Visible = True
            ValueEntry.SetFocus
        End If
    End If

    ' Update value entry box.
    UpdateEntryBox
End Sub

'
' Handle the value changing somehow.
'
Private Sub ValueEntry_Change()

    ' See if this was changed by user or remote.
    If IsNotify Then
    
        ' Changed by remote, so remember it.
        ValueEntryOriginal = ValueEntry.Text
    End If
End Sub

'
' Handle the user changing the value in the entry box.
'
Private Sub ValueEntry_Click()

    ' See if this was changed by user or remote.
    If Not IsNotify Then
    
        ' Changed by user, so contemplate sending it.
        ValueModify_Click
    End If
End Sub

'
' Handle gaining focus.
'
Private Sub ValueEntry_GotFocus()
    ValueEntry.SelStart = 0
    ValueEntry.SelLength = Len(ValueEntry.Text)
End Sub

Private Sub ValueEntry_LostFocus()
    ModifyValue False
End Sub

'
' Clicked on value.
'
Private Sub ValueModify_Click()
    ModifyValue True
End Sub

'
' Send the changed value.
'
Private Sub ModifyValue(Focus As Boolean)

    ' Make sure we're ready to send.
    If Not IsNotify Then
    
        ' Validate the datatype.
        If ValidateType(ValueEntry.Text, ValueEntryDataType) Then
        
            ' Update it.
            Call Connect.UpdateControl(ValueEntry, True, "")
        Else
        
            ' Restore original value.
            IsNotify = True
            ValueEntry.Text = ValueEntryOriginal
            IsNotify = False
        End If

        ' Set focus to it.
        If Focus Then ValueEntry.SetFocus
    End If
End Sub

'
' Save the server configuration.
'
Private Sub SaveConfig_Click()
    LocalGateClient.AsyncExec "SaveConfig"
End Sub


'/////////////////////////////////////////////////////////
' User grid accessors.
'/////////////////////////////////////////////////////////

'
' Return the number of Users in the grid.
'
Private Function NumUsers() As Long
    If Users.Enabled Then
        NumUsers = Users.Rows - 1
    Else
        NumUsers = 0
    End If
End Function


'
' Return whether there is a valid current User
' in the Users grid.
'
Private Function GetUserValidity() As Boolean
    GetUserValidity = NumUsers > 0
End Function

'
' Return the current User's name.
' Only valid if GetLevelValidity() is true.
'
Private Function GetUserName() As String
    
    ' Inhibit FixGridSelection.
    RefreshingGrid = RefreshingGrid + 1
    
    ' Grab the user name.
    Users.Col = 0
    GetUserName = Users.Text

    ' Refresh the grid selection.
    RefreshingGrid = RefreshingGrid - 1
    FixUserSelection
End Function

'
' Return the current User's level.
' Only valid if GetLevelValidity() is true.
'
Private Function GetUserLevel() As String
    
    ' Inhibit FixGridSelection.
    RefreshingGrid = RefreshingGrid + 1
    
    ' Grab the user name.
    Users.Col = 3
    GetUserLevel = Users.Text

    ' Refresh the grid selection.
    RefreshingGrid = RefreshingGrid - 1
    FixUserSelection
End Function

'/////////////////////////////////////////////////////////
' Users tab implementation.
'/////////////////////////////////////////////////////////

'
' Update all information in the Users tab.
'
Sub UsersTabRefresh()
    Dim Unit As Long, i As Integer
    
    ' Inhibit FixLevelSelection.
    RefreshingGrid = RefreshingGrid + 1
    
    ' Compute sizing info.
    Users.Width = UsersHolder.ScaleWidth + 500
    Users.Height = UsersHolder.ScaleHeight
    Unit = UsersHolder.ScaleWidth / 11
    
    ' Set all column headings and sizes.
    Users.Row = 0
    Users.Col = 0
    Users.ColWidth(0) = UsersHolder.ScaleWidth - 9 * Unit - 4 * Screen.TwipsPerPixelX
    Users.Text = "User"
    '
    Users.Col = 1
    Users.ColWidth(1) = Unit * 2
    Users.Text = "Type"
    '
    Users.Col = 2
    Users.ColWidth(2) = Unit * 2
    Users.Text = "Team"
    '
    Users.Col = 3
    Users.ColWidth(3) = Unit * 2
    Users.Text = "Level"
    '
    Users.Col = 4
    Users.ColWidth(4) = Unit * 2
    Users.Text = "Application"
    '
    Users.Col = 5
    Users.ColWidth(5) = Unit
    Users.Text = "Access"
    '
    Users.Col = 6
    Users.ColWidth(6) = 100

    ' Reactivate FixUserSelection.
    RefreshingGrid = RefreshingGrid - 1
    FixUserSelection
End Sub

'
' Update the selected region of the grid.  This is called
' in response to the user messing with the grid with the
' mouse or keyboard.
'
Private Sub FixUserSelection()
    If RefreshingGrid = 0 Then
        RefreshingGrid = 1
        
        ' Set column.
        Users.Col = 6
        
        ' Set rows.
        If NumUsers > 0 Then
        
            ' There are Users in the list.
            Users.SelStartCol = 0
            Users.SelEndCol = 5
            
            ' Set row.
            If Users.SelStartRow < Users.Row Then
                Users.Row = Users.SelStartRow
            Else
                Users.Row = Users.SelEndRow
            End If
            
            ' Set selection
            If Users.Row > 0 Then
                Users.SelStartRow = Users.Row
                Users.SelEndRow = Users.Row
            Else
                Users.SelStartRow = 1
                Users.SelEndRow = 1
            End If
        Else
        
            ' The list is empty.
            Users.SelStartCol = 6
            Users.SelEndCol = 6
        End If
        
        RefreshingGrid = 0
    End If

    ' Set level related buttons.
    UserMessage.Enabled = NumUsers > 0
    UserBanish.Enabled = NumUsers > 0
End Sub

'
' Mouse click on the Users grid.
'
Private Sub Users_Click()
    FixUserSelection
End Sub

Private Sub Users_MouseDown(Button As Integer, Shift As Integer, X As Single, Y As Single)
    FixUserSelection
    If Button And vbRightButton And GetUserValidity Then
        ' Set popup menu properties.
        
        ' Show popup menu.
        PopupMenu Popups.User
        
        ' Note that the code in the Popups form will
        ' route popup menu commands to the appropriate
        ' button Click handlers in this form.
    End If
End Sub

'
' Mouse move above the Users grid.
'
Private Sub Users_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    FixUserSelection
End Sub

'
' Mouse button release above the Users grid.
'
Private Sub Users_MouseUp(Button As Integer, Shift As Integer, X As Single, Y As Single)
    FixUserSelection
End Sub

'
' Row/column change above the users grid.
'
Private Sub Users_RowColChange()
    FixUserSelection
End Sub

'
' Selection change above the users grid.
'
Private Sub Users_SelChange()
    FixUserSelection
End Sub

'
' Send a message to a user.
'
Public Sub UserMessage_Click()
    Dim Message As String
    
    If GetUserValidity Then
        If ThingEntry.GetString( _
            "Send a message to " & GetUserName, _
            "Enter message for " & GetUserName & ":", _
            Message) Then
            
            'Send it.
            If Message <> "" Then
                LocalGateClient.AsyncExec "Tell " & _
                    GetUserName & Quotes(Message)
            End If
        End If
    End If
End Sub

'
' Broadcast a message to all users.
'
Public Sub UserBroadcast_Click()
    Dim Message As String
    
    If NumUsers > 0 Then
        If ThingEntry.GetString( _
            "Broadcast to all users", _
            "Enter message to broadcast:", _
            Message) Then
            
            'Broadcast it.
            If Message <> "" Then
                LocalGateClient.AsyncExec "Broadcast " & _
                    Quotes(Message)
            End If
        End If
    End If
End Sub

'
' Banish a User from this Gatekeeper.
'
Public Sub UserBanish_Click()
    If GetUserValidity Then
        If MsgBox("Are you sure you want to forcefully disconnect " & _
            GetUserName & "?", vbOKCancel) = vbOK Then
            
            ' Banish him
            LocalGateClient.AsyncExec "Banish " & GetUserName
        End If
    End If
End Sub

'/////////////////////////////////////////////////////////
' Subscription notifications.
'/////////////////////////////////////////////////////////

'
' One of the subscribed topics was deleted.
' This shouldn't occur.
'
Public Sub NotifyDelete(Database As String)
    MsgBox "Unexpected notify of deleted " & Database & "."
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
