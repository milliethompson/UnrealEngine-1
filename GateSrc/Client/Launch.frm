VERSION 5.00
Object = "{6B7E6392-850A-101B-AFC0-4210102A8DA7}#1.1#0"; "comctl32.ocx"
Begin VB.Form Launch 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Unreal GateKeeper Launcher"
   ClientHeight    =   3750
   ClientLeft      =   1230
   ClientTop       =   8205
   ClientWidth     =   4890
   ControlBox      =   0   'False
   Icon            =   "Launch.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   3750
   ScaleWidth      =   4890
   Begin VB.CommandButton LaunchCancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   315
      Left            =   3780
      TabIndex        =   1
      Top             =   3420
      Width           =   975
   End
   Begin VB.CommandButton LaunchOk 
      Caption         =   "&Ok"
      Height          =   315
      Left            =   2760
      TabIndex        =   0
      Top             =   3420
      Width           =   975
   End
   Begin VB.Frame Frame2 
      Height          =   1770
      Left            =   -420
      TabIndex        =   10
      Top             =   4260
      Visible         =   0   'False
      Width           =   5940
      Begin ComctlLib.ProgressBar ConnectProgress 
         Height          =   195
         Left            =   540
         TabIndex        =   13
         Top             =   480
         Width           =   4575
         _ExtentX        =   8070
         _ExtentY        =   344
         _Version        =   327680
         Appearance      =   1
         MouseIcon       =   "Launch.frx":030A
      End
      Begin VB.Label ConnectStatus 
         Alignment       =   2  'Center
         Caption         =   "Trying..."
         Height          =   195
         Left            =   780
         TabIndex        =   12
         Top             =   720
         Width           =   4035
      End
      Begin VB.Label Label1 
         Alignment       =   2  'Center
         Caption         =   "Connecting to: hostname"
         Height          =   195
         Left            =   780
         TabIndex        =   11
         Top             =   240
         Width           =   3915
      End
   End
   Begin VB.Timer AutoTimer 
      Enabled         =   0   'False
      Interval        =   1000
      Left            =   4335
      Top             =   -165
   End
   Begin VB.Frame Frame1 
      Caption         =   "Startup Options"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   11.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   -1  'True
         Strikethrough   =   0   'False
      EndProperty
      Height          =   3195
      Left            =   120
      TabIndex        =   2
      Top             =   120
      Width           =   4635
      Begin VB.CheckBox AutoLaunchLocalLevels 
         Caption         =   "Automatically launch all local levels upon startup."
         Height          =   255
         Left            =   540
         TabIndex        =   14
         Top             =   660
         Width           =   3915
      End
      Begin VB.CommandButton RemoteServerEdit 
         Caption         =   "&Edit"
         Height          =   315
         Left            =   3600
         TabIndex        =   9
         Top             =   2760
         Width           =   915
      End
      Begin VB.CommandButton RemoteServerDelete 
         Caption         =   "&Delete"
         Height          =   315
         Left            =   3600
         TabIndex        =   8
         Top             =   2400
         Width           =   915
      End
      Begin VB.CommandButton RemoteServerAdd 
         Caption         =   "&Add"
         Height          =   315
         Left            =   3600
         TabIndex        =   7
         Top             =   2040
         Width           =   915
      End
      Begin VB.ListBox RemoteServerList 
         Height          =   1035
         Left            =   540
         Sorted          =   -1  'True
         TabIndex        =   6
         Top             =   2040
         Width           =   2955
      End
      Begin VB.OptionButton LaunchRemote 
         Caption         =   "Administer a remote Gatekeeper on the Internet:"
         Height          =   255
         Left            =   240
         TabIndex        =   5
         Top             =   1680
         Width           =   4335
      End
      Begin VB.OptionButton LaunchLocal 
         Caption         =   "Launch a dedicated Gatekeeper on this machine now."
         Height          =   255
         Left            =   240
         TabIndex        =   3
         Top             =   360
         Value           =   -1  'True
         Width           =   4335
      End
      Begin VB.Label LocalLabel 
         Caption         =   "Note: You can launch a dedicated server automatically by using the command-line parameter ""-launch""."
         Height          =   615
         Left            =   540
         TabIndex        =   4
         Top             =   960
         Width           =   3855
      End
   End
End
Attribute VB_Name = "Launch"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' Launch.frm: GateClient initial launch screen.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Presents the user interface for launching either a
'   local server, or administering a remote one.
'   This form is shown automatically at startup.
'   It either shows the main form after a connection
'   has been established, or exits if the user cancels.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

Dim Relisting As Boolean

'/////////////////////////////////////////////////////////
' Standard form events.
'/////////////////////////////////////////////////////////

'
' Form load event.
'
Private Sub Form_Load()
    
    ' Handle command line parameters.
    If Command$ <> "" Then
        On Error GoTo DirError
        ChDrive Left$(Command$, 1)
        ChDir Command$
DirError:
    End If
    
    ' Create the local gate client instance.
    Set LocalGateClient = New GateClient.Client
    
    ' Load defaults.
    LocalGateClient.LoadDefaults
    
    ' Set launch local default.
    AutoLaunchLocalLevels.Value = _
        IIf(LocalGateClient.AutoLaunchLocalLevels, 1, 0)
    
    ' Init UI.
    If App.PrevInstance = 0 Then
        
        ' This is the only running copy, so enable local launch.
        LaunchLocal.Enabled = True
        
        ' Default to either local or remote launch.
        If LocalGateClient.DefaultConnectLocal Then
            LaunchLocal.Value = True
        Else
            LaunchRemote.Value = True
        End If
        
Else
        
        ' Another copy is running, so diable local launch.
        LaunchLocal.Enabled = False
        
        ' Default to remote launch.
        LaunchRemote.Value = True
        
        ' Display another-instance-is-running message.
        LocalLabel.Caption = "Note: Another copy of the server is already running on this machine, so a new one cannot be launched."
    End If

    ' Act on the command line parameters.
    If InStr(UCase(Command), "-LOCAL") <> 0 Then
        
        ' User wants to automatically launch local server.
        If App.PrevInstance = 0 Then
        
            ' Set up to launch automatically.
            AutoTimer.Enabled = True
        Else
        
            ' Can't launch since another copy is running.
            MsgBox "Another copy of the server is already running on this machine, so a new one cannot be launched."
            
            ' Exit program.
            End
        End If
    End If
    
    ' Get remote server list.
    GetRemoteServerList
End Sub

'/////////////////////////////////////////////////////////
' Custom routines.
'/////////////////////////////////////////////////////////

'
' Fill in the list of remote servers.
'
Private Sub GetRemoteServerList()
    Dim i As Integer
    Dim Found As Boolean
    Dim Site As String
    
    ' Prevent RemoteServerList Click events.
    Relisting = True
    
    ' Empty the remote server list.
    RemoteServerList.Clear

    ' Fill in the remote server list.
    For i = 0 To LocalGateClient.NumRemoteServers - 1
        RemoteServerList.AddItem LocalGateClient.GetRemoteServer(i)
    Next
    
    ' Enable or disable the list and its buttons
    ' according to context.
    If LaunchLocal.Value Then
    
        ' Default to launching a local server; can't mess
        ' with remote server list.
        RemoteServerList.ListIndex = -1
        RemoteServerList.Enabled = False
        RemoteServerAdd.Enabled = False
        RemoteServerDelete.Enabled = False
        RemoteServerEdit.Enabled = False
    Else
    
        ' Default to launching a remote server; activate UI.
        RemoteServerList.Enabled = True
        RemoteServerAdd.Enabled = LocalGateClient.CanAddRemoteServer
        RemoteServerDelete.Enabled = True
        RemoteServerEdit.Enabled = True
        
        ' Default to selecting the first item.
        If RemoteServerList.ListCount > 0 Then
            RemoteServerList.ListIndex = 0
        End If
        
        ' Select the default item if it's on the list.
        For i = 0 To RemoteServerList.ListCount - 1
            If UCase(RemoteServerList.List(i)) = UCase(LocalGateClient.DefaultRemoteSite) Then
                
                ' Highlight the default site!
                RemoteServerList.ListIndex = i
            End If
        Next
    End If
    
    ' Enable RemoteServerList Click events.
    Relisting = False
End Sub

'/////////////////////////////////////////////////////////
' Login routine.
'/////////////////////////////////////////////////////////

Private Function Login() As Boolean

    ' Tell the server our Gatekeeper protocol version number
    ' and our application-specific name and version.
    LocalGateClient.SafeExec "Client " & GATECLIENT_VERSION & _
        " " & GATECLIENT_APPLICATION_NAME & _
        " " & GATECLIENT_APPLICATION_VERSION

    ' Log in.
    LocalGateClient.SafeExec "Login Admin* " & LocalGateClient.ConnectPassword

    ' Make sure we got our user name from the above login.
    If LocalGateClient.ResultUserName = "" Then
        MsgBox "Remote server failed to give client a user name."
        End
    End If

    'todo:
    ' For debugging:
    'LocalGateClient.AsyncExec "Verbose"
End Function

'/////////////////////////////////////////////////////////
' Launch routines.
'/////////////////////////////////////////////////////////

'
' Attempt to launch and connect to a local GateServer.
' Returns success.
'
Private Function AttemptLocalLaunch() As Boolean

    ' Load the local gate server.
    Set LocalGateServer = New GateServer.Server

    ' Set the local gate server's working directory
    ' and initialize it.
    Call LocalGateServer.Init(CurDir$, _
        AutoLaunchLocalLevels = 1)

    ' Try to connect.
    If LocalGateClient.OpenServer("localhost", _
        LocalGateServer.GetServerPort, _
        LocalGateServer.GetServerAdminPassword) Then
        
        ' Attempt login.
        Login

        ' Set main form caption.
        Main.Caption = GATECLIENT_TITLE & " - local server"
    
        ' Success!
        LocalGateClient.IsLocal = True
        AttemptLocalLaunch = True
    Else
    
        ' Display diagnostic.
        MsgBox ("Failed connecting to server on this local machine. " & _
            "This indicates either a server failure or a TCP/IP problem. ")

        ' Failed.
        AttemptLocalLaunch = False
    End If
End Function

'
' Attempt to connect to a remote GateServer.
' Returns success.
'
Private Function AttemptRemoteLaunch(Site As String, Port As Long, Password As String) As Boolean
    
    ' Try to connect.
    If LocalGateClient.OpenServer(Site, Port, Password) Then
        
        ' Attempt login.
        Login
        
        ' Indicate that we succeeded.
        AttemptRemoteLaunch = True
        
        ' Set the caption to show the remote site.
        Main.Caption = GATECLIENT_TITLE & " - " & Site
    Else
        
        ' Show diagnostic.
        MsgBox "Connection attempt failed: " & LocalGateClient.ConnectResultText & "."
    
        ' Failure.
        AttemptRemoteLaunch = False
    End If
End Function

'/////////////////////////////////////////////////////////
' Custom user interface events.
'/////////////////////////////////////////////////////////

'
' Cancel button.
'
Private Sub LaunchCancel_Click()
    
    ' Disable timer.
    AutoTimer.Enabled = False
    
    ' Unload with failure.
    Unload Connect
    Unload Me
End Sub

'
' Select launching a local server.
'
Private Sub LaunchLocal_Click()

    ' Update the remote server list.
    GetRemoteServerList
End Sub

'
' Select launching a remote server.
'
Private Sub LaunchRemote_Click()
    
    ' Update the remote server list.
    GetRemoteServerList
End Sub

'
' Called whenever the value of the remote server list
' selection changes.
'
Private Sub RemoteServerList_Click()
    If Not Relisting Then
        If RemoteServerList.ListIndex >= 0 And _
            RemoteServerList.ListCount > 0 Then
            
            ' Remember site name.
            LocalGateClient.DefaultRemoteSite = RemoteServerList.List(RemoteServerList.ListIndex)
        End If
    End If
End Sub

'
' Double click on the remote server list to launch
' a particular server.
'
Private Sub RemoteServerList_DblClick()
    
    ' Try to launch it.
    LaunchOk_Click
End Sub

'
' Ok button.
'
Private Sub LaunchOk_Click()

    ' Disable timer.
    AutoTimer.Enabled = False

    ' Try to launch.
    If LaunchLocal.Value Then
        
        ' Try starting a local server.
        If AttemptLocalLaunch Then
        
            ' Remember defaults.
            LocalGateClient.DefaultConnectLocal = True
            LocalGateClient.AutoLaunchLocalLevels = AutoLaunchLocalLevels.Value
            
            ' Save defaults.
            LocalGateClient.SaveDefaults
            
            ' Go into the main client form.
            Main.Startup
            Hide
        End If
    Else
        
        ' Try connecting to a remote server.
        Dim Site As String, Port As Long, Password As String
        
        ' Get site name from list.
        Site = RemoteServerList.List(RemoteServerList.ListIndex)
        
        ' Get remote server info.
        Call LocalGateClient.GetRemoteServerInfo(Site, Port, Password)

        If AttemptRemoteLaunch(Site, Port, Password) Then
        
            ' Remember defaults.
            LocalGateClient.DefaultConnectLocal = False
            LocalGateClient.DefaultRemoteSite = Site
            LocalGateClient.AutoLaunchLocalLevels = AutoLaunchLocalLevels.Value
            
            ' Save defaults.
            LocalGateClient.SaveDefaults
            
            ' Go into the main client form.
            Main.Startup
            Hide
        End If
    End If
End Sub

'
' Add a new remote server to the list.
'
Private Sub RemoteServerAdd_Click()
    
    ' Pop up site entry dialog.
    Dim SiteName As String, SitePort As Long, SitePassword As String
    SitePort = DEFAULT_GATE_PORT
    
    If SiteEntry.GetRemoteSite(SiteName, SitePort, SitePassword) Then
        
        ' Add to list.
        Call LocalGateClient.AddRemoteServer(SiteName, SitePort, SitePassword)
    
        ' Default to this new item.
        LocalGateClient.DefaultRemoteSite = SiteName
    
        ' Update the remote server list.
        GetRemoteServerList
        
        ' Save defaults.
        LocalGateClient.SaveDefaults
    End If
End Sub

'
' Delete the selected remote server from the list.
'
Private Sub RemoteServerDelete_Click()
    Dim Site As String

    If RemoteServerList.ListIndex >= 0 And _
        RemoteServerList.ListCount > 0 Then
        
        ' Get site name.
        Site = RemoteServerList.List(RemoteServerList.ListIndex)
        
        ' Confirm deletion.
        If MsgBox("Are you sure you want to delete server " & _
            Site & " from the list?", vbYesNo) = vbYes Then
            
            ' Delete it from the list.
            LocalGateClient.DeleteRemoteServer Site
            
            ' Update the remote server list.
            GetRemoteServerList
            
            ' Save configuration.
            LocalGateClient.SaveDefaults
            End If
        End If
End Sub

'
' Edit the selected remote server.
'
Private Sub RemoteServerEdit_Click()
    Dim Site As String, Port As Long, Password As String
    
    If RemoteServerList.ListIndex >= 0 And _
        RemoteServerList.ListCount > 0 Then
        
        ' Get site name.
        Site = RemoteServerList.List(RemoteServerList.ListIndex)
    
        ' Get the remote server's info.
        Call LocalGateClient.GetRemoteServerInfo(Site, Port, Password)
        
        ' Delete it from the list.
        Call LocalGateClient.DeleteRemoteServer(Site)

        ' Edit the site info. Only changes info if
        ' success.
        Call SiteEntry.GetRemoteSite(Site, Port, Password)

        ' Add it back to the list.
        Call LocalGateClient.AddRemoteServer(Site, Port, Password)

        ' Update the remote server list.
        GetRemoteServerList
        
        ' Save defaults.
        LocalGateClient.SaveDefaults
    End If
End Sub

'/////////////////////////////////////////////////////////
' Timer.
'/////////////////////////////////////////////////////////

'
' Timer for automatic local launch.
'
Private Sub AutoTimer_Timer()
    
    ' Attempt local launch.
    If AttemptLocalLaunch Then
    
        ' Successfully launched via command line.
        Main.Startup
        Unload Me
    Else
    
        ' Failed command line launch, already displayed diagnostic.
        Unload Connect
        Unload Me
    End If
End Sub

'/////////////////////////////////////////////////////////
' The end.
'/////////////////////////////////////////////////////////
