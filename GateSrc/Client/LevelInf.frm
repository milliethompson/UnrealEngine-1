VERSION 5.00
Object = "{BDC217C8-ED16-11CD-956C-0000C04E4C0A}#1.1#0"; "tabctl32.ocx"
Begin VB.Form LevelInfo 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Level Properties"
   ClientHeight    =   4080
   ClientLeft      =   2430
   ClientTop       =   2175
   ClientWidth     =   6690
   Icon            =   "LevelInf.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4080
   ScaleWidth      =   6690
   Begin TabDlg.SSTab LevelTab 
      Height          =   3975
      Left            =   60
      TabIndex        =   7
      Top             =   60
      Width           =   6555
      _ExtentX        =   11562
      _ExtentY        =   7011
      _Version        =   327680
      Style           =   1
      Tab             =   2
      TabHeight       =   529
      ShowFocusRect   =   0   'False
      TabCaption(0)   =   "Settings "
      Tab(0).ControlCount=   10
      Tab(0).ControlEnabled=   0   'False
      Tab(0).Control(0)=   "AutoLaunch"
      Tab(0).Control(0).Enabled=   -1  'True
      Tab(0).Control(1)=   "ServerParms"
      Tab(0).Control(1).Enabled=   -1  'True
      Tab(0).Control(2)=   "MaxWatchers"
      Tab(0).Control(2).Enabled=   -1  'True
      Tab(0).Control(3)=   "MaxPlayers"
      Tab(0).Control(3).Enabled=   -1  'True
      Tab(0).Control(4)=   "Close1"
      Tab(0).Control(4).Enabled=   -1  'True
      Tab(0).Control(5)=   "Label4"
      Tab(0).Control(5).Enabled=   0   'False
      Tab(0).Control(6)=   "Label5"
      Tab(0).Control(6).Enabled=   0   'False
      Tab(0).Control(7)=   "Label3"
      Tab(0).Control(7).Enabled=   0   'False
      Tab(0).Control(8)=   "Label1"
      Tab(0).Control(8).Enabled=   0   'False
      Tab(0).Control(9)=   "Label2"
      Tab(0).Control(9).Enabled=   0   'False
      TabCaption(1)   =   "Passwords "
      Tab(1).ControlCount=   9
      Tab(1).ControlEnabled=   0   'False
      Tab(1).Control(0)=   "WatcherPassword"
      Tab(1).Control(0).Enabled=   -1  'True
      Tab(1).Control(1)=   "PlayerPassword"
      Tab(1).Control(1).Enabled=   -1  'True
      Tab(1).Control(2)=   "AdminPassword"
      Tab(1).Control(2).Enabled=   -1  'True
      Tab(1).Control(3)=   "Close2"
      Tab(1).Control(3).Enabled=   -1  'True
      Tab(1).Control(4)=   "Label7"
      Tab(1).Control(4).Enabled=   0   'False
      Tab(1).Control(5)=   "Label14"
      Tab(1).Control(5).Enabled=   0   'False
      Tab(1).Control(6)=   "Label13"
      Tab(1).Control(6).Enabled=   0   'False
      Tab(1).Control(7)=   "Label15"
      Tab(1).Control(7).Enabled=   0   'False
      Tab(1).Control(8)=   "Label6"
      Tab(1).Control(8).Enabled=   0   'False
      TabCaption(2)   =   "Read-only "
      Tab(2).ControlCount=   7
      Tab(2).ControlEnabled=   -1  'True
      Tab(2).Control(0)=   "Label8"
      Tab(2).Control(0).Enabled=   0   'False
      Tab(2).Control(1)=   "Label9"
      Tab(2).Control(1).Enabled=   0   'False
      Tab(2).Control(2)=   "Label10"
      Tab(2).Control(2).Enabled=   0   'False
      Tab(2).Control(3)=   "GateInfo"
      Tab(2).Control(3).Enabled=   0   'False
      Tab(2).Control(4)=   "LevelInfo"
      Tab(2).Control(4).Enabled=   0   'False
      Tab(2).Control(5)=   "GatePassword"
      Tab(2).Control(5).Enabled=   0   'False
      Tab(2).Control(6)=   "Close3"
      Tab(2).Control(6).Enabled=   0   'False
      Begin VB.CommandButton Close3 
         Caption         =   "&Close"
         Height          =   315
         Left            =   5400
         TabIndex        =   26
         Top             =   3540
         Width           =   1035
      End
      Begin VB.TextBox GatePassword 
         Height          =   285
         Left            =   2220
         Locked          =   -1  'True
         TabIndex        =   25
         Tag             =   "LevelParms:GatePassword"
         Top             =   2220
         Width           =   2775
      End
      Begin VB.TextBox LevelInfo 
         Height          =   285
         Left            =   2220
         Locked          =   -1  'True
         TabIndex        =   24
         Tag             =   "Levels:Extra"
         Top             =   1860
         Width           =   2775
      End
      Begin VB.TextBox GateInfo 
         Height          =   285
         Left            =   2220
         Locked          =   -1  'True
         TabIndex        =   23
         Tag             =   "LevelParms:GateInfo"
         Top             =   1500
         Width           =   2775
      End
      Begin VB.CheckBox AutoLaunch 
         Caption         =   "Automatically launch this level at startup time."
         Height          =   255
         Left            =   -73980
         TabIndex        =   19
         Tag             =   "LevelParms:AutoLaunch"
         Top             =   780
         Width           =   3675
      End
      Begin VB.TextBox WatcherPassword 
         Height          =   285
         IMEMode         =   3  'DISABLE
         Left            =   -72120
         PasswordChar    =   "*"
         TabIndex        =   5
         Tag             =   "LevelParms:WatcherPassword"
         Top             =   2220
         Width           =   2955
      End
      Begin VB.TextBox PlayerPassword 
         Height          =   285
         IMEMode         =   3  'DISABLE
         Left            =   -72120
         PasswordChar    =   "*"
         TabIndex        =   4
         Tag             =   "LevelParms:PlayerPassword"
         Top             =   1860
         Width           =   2955
      End
      Begin VB.TextBox AdminPassword 
         Height          =   285
         IMEMode         =   3  'DISABLE
         Left            =   -72120
         PasswordChar    =   "*"
         TabIndex        =   3
         Tag             =   "LevelParms:AdminPassword"
         Top             =   1500
         Width           =   2955
      End
      Begin VB.CommandButton Close2 
         Caption         =   "&Close"
         Height          =   315
         Left            =   -69600
         TabIndex        =   6
         Top             =   3540
         Width           =   1035
      End
      Begin VB.TextBox ServerParms 
         Height          =   735
         Left            =   -74880
         TabIndex        =   2
         Tag             =   "LevelParms:LevelParms"
         Top             =   2640
         Width           =   6315
      End
      Begin VB.TextBox MaxWatchers 
         Height          =   285
         Left            =   -72600
         TabIndex        =   1
         Tag             =   "LevelParms:MaxWatchers"
         Text            =   "0"
         Top             =   1560
         Width           =   1935
      End
      Begin VB.TextBox MaxPlayers 
         Height          =   285
         Left            =   -72600
         TabIndex        =   0
         Tag             =   "LevelParms:MaxPlayers"
         Text            =   "0"
         Top             =   1200
         Width           =   1935
      End
      Begin VB.CommandButton Close1 
         Cancel          =   -1  'True
         Caption         =   "&Close"
         Height          =   315
         Left            =   -69600
         TabIndex        =   8
         Top             =   3540
         Width           =   1035
      End
      Begin VB.Label Label10 
         Alignment       =   1  'Right Justify
         Caption         =   "LevelExtra:"
         Height          =   195
         Left            =   840
         TabIndex        =   22
         Top             =   1920
         Width           =   1275
      End
      Begin VB.Label Label9 
         Alignment       =   1  'Right Justify
         Caption         =   "GatePassword:"
         Height          =   195
         Left            =   840
         TabIndex        =   21
         Top             =   2280
         Width           =   1275
      End
      Begin VB.Label Label8 
         Alignment       =   1  'Right Justify
         Caption         =   "GateInfo:"
         Height          =   195
         Left            =   840
         TabIndex        =   20
         Top             =   1560
         Width           =   1275
      End
      Begin VB.Label Label7 
         Alignment       =   2  'Center
         Caption         =   "(Leave blank to use default)"
         Height          =   255
         Left            =   -72120
         TabIndex        =   18
         Top             =   2580
         Width           =   2955
      End
      Begin VB.Label Label4 
         Alignment       =   2  'Center
         Caption         =   "(0 to use default)"
         Height          =   255
         Left            =   -72600
         TabIndex        =   17
         Top             =   1920
         Width           =   1935
      End
      Begin VB.Label Label14 
         Alignment       =   1  'Right Justify
         Caption         =   "Level Watcher Password:"
         Height          =   255
         Left            =   -74280
         TabIndex        =   16
         Top             =   2220
         Width           =   2055
      End
      Begin VB.Label Label13 
         Alignment       =   1  'Right Justify
         Caption         =   "Level Player Password:"
         Height          =   255
         Left            =   -74280
         TabIndex        =   15
         Top             =   1860
         Width           =   2055
      End
      Begin VB.Label Label15 
         Alignment       =   1  'Right Justify
         Caption         =   "Level Admin Password:"
         Height          =   255
         Left            =   -74280
         TabIndex        =   14
         Top             =   1500
         Width           =   2055
      End
      Begin VB.Label Label6 
         Caption         =   "Note: Changes made to levels that are already up are applied the next time the level is brought up."
         Height          =   435
         Left            =   -74760
         TabIndex        =   13
         Top             =   3420
         Width           =   5055
      End
      Begin VB.Label Label5 
         Caption         =   "Level command-line parameters (leave blank to use default level parameters):"
         Height          =   255
         Left            =   -74880
         TabIndex        =   12
         Top             =   2340
         Width           =   6315
      End
      Begin VB.Label Label3 
         Alignment       =   1  'Right Justify
         Caption         =   "Watchers per level:"
         Height          =   255
         Left            =   -74100
         TabIndex        =   11
         Top             =   1620
         Width           =   1395
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         Caption         =   "Players per level:"
         Height          =   255
         Left            =   -74100
         TabIndex        =   10
         Top             =   1260
         Width           =   1395
      End
      Begin VB.Label Label2 
         Caption         =   "Note: Changes made to levels that are already up are applied the next time the level is brought up."
         Height          =   435
         Left            =   -74760
         TabIndex        =   9
         Top             =   3420
         Width           =   5055
      End
   End
End
Attribute VB_Name = "LevelInfo"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' LevelInf.frm: GateClient form with information about
'               one game server level.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Presents the user interface for modifying the properties
'   of one game server level.  The level may be either
'   local or remote, and it may be either active or
'   inactive.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Standard form events.
'/////////////////////////////////////////////////////////

'
' Standard form load event.
'
Private Sub Form_Load()
    
    ' Most setup work is perform in the Startup sub,
    ' which is called after our level name has been set.
End Sub

'
' Standard form unload event.
'
Private Sub Form_Unload(Cancel As Integer)
    
    ' Unsubscribe.
    Call Connect.Unsubscribe("LevelParms Level=" & Tag, Me)
    Call Connect.Unsubscribe("Levels Level=" & Tag, Me)
    
    ' Save the configuration.
    LocalGateClient.AsyncExec "SaveConfig"
End Sub

'
' Close buttons (one per page).
'

Private Sub Close1_Click()
    Unload Me
End Sub

Private Sub Close2_Click()
    Unload Me
End Sub

Private Sub Close3_Click()
    Unload Me
End Sub

'/////////////////////////////////////////////////////////
' Public functions.
'/////////////////////////////////////////////////////////

'
' Startup. Called after this form's tag has been set
' to the name of the level we're referencing.
'
Public Sub Startup()
    Dim i As Integer

    ' Set up the current level tab:
    LevelTab_Click 0
    
    ' Set caption.
    Caption = Tag
    
    ' Set all control tags to reflect the caption.
    For i = 0 To Controls.Count - 1
        If Left(Controls(i).Tag, 1) = ":" Then
            Controls(i).Tag = Tag & Controls(i).Tag
        End If
    Next
    
    ' Subscribe to the appropriate level.
    Call Connect.Subscribe("Levels Level=" & Tag, Me, True)
    Call Connect.Subscribe("LevelParms Level=" & Tag, Me, True)
End Sub

Private Sub AutoLaunch_Click()
    Call Connect.UpdateControl(AutoLaunch, True, Tag)
End Sub

'/////////////////////////////////////////////////////////
' Tab control implementation.
'/////////////////////////////////////////////////////////

'
' The tab was changed.
'
Private Sub LevelTab_Click(PreviousTab As Integer)

    ' Skip out Startup hasn't been called yet.
    If Tag = "" Then Exit Sub
    
    ' Refresh the new tab.
    Select Case LevelTab.Tab
        Case 0 ' Settings tab.
        Case 1 ' Passwords tab.
    End Select
End Sub

'/////////////////////////////////////////////////////////
' All change notifications.
'/////////////////////////////////////////////////////////

Private Sub MaxPlayers_LostFocus()
    Call Connect.UpdateControl(MaxPlayers, True, Tag)
End Sub

Private Sub MaxWatchers_LostFocus()
    Call Connect.UpdateControl(MaxWatchers, True, Tag)
End Sub

Private Sub ServerParms_LostFocus()
    Call Connect.UpdateControl(ServerParms, True, Tag)
End Sub

Private Sub AdminPassword_LostFocus()
    Call Connect.UpdateControl(AdminPassword, True, Tag)
End Sub

Private Sub PlayerPassword_LostFocus()
    Call Connect.UpdateControl(PlayerPassword, True, Tag)
End Sub

Private Sub WatcherPassword_LostFocus()
    Call Connect.UpdateControl(WatcherPassword, True, Tag)
End Sub

'/////////////////////////////////////////////////////////
' Subscription notifications.
'/////////////////////////////////////////////////////////

'
' One of the subscribed topics was deleted. This means
' that the level whose properties this form is editing
' was deleted.
'
Public Sub NotifyDelete(Database As String, Key As String)
    If StrComp(Tag, Key) = 0 Then Unload Me
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
