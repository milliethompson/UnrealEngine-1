VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Gatekeeper"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' Gatekeep.cls: Gatekeeper class implementation.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   This is the Gatekeeper object.
'   It is the internal implementation of all the work that
'   that GateServer.Server object needs done.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Public variables.
'/////////////////////////////////////////////////////////

' The log we write to.
Public Log As GateLog

' The database manager.
Public DbMan As DbManager

' The level manager.
Public LevelMan As LevelManager

' The connection list.
Public ConnectionList As GateConnectionList

' The database hooks we track.
Public Users As DbHook ' Player list.
Public Gate As DbHook ' Gate information.
Public Game As DbHook ' Game information.
Public Passwords As DbHook ' Passwords information.
Public TimeLimits As DbHook ' Time limits.

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

' The form that sits around listening for new TCP connections.
Private Listener As GateListener

' Number of administrators who have logged in.
Private AdminNum As Long

'/////////////////////////////////////////////////////////
' Accessors.
'/////////////////////////////////////////////////////////

' Gate accessors.

Public Function GetPort() As Long
    GetPort = Val(Gate.GetSingleValue("Port"))
End Function

Public Function GetName() As Long
    GetName = Gate.GetSingleValue("GatekeeperName")
End Function

' Passwords accessors.

Public Function GetServerInfoPassword() As String
    GetServerInfoPassword = Passwords.GetSingleValue("Info")
End Function

Public Function GetDefaultLevelParms() As String
    GetDefaultLevelParms = Game.GetSingleValue("DefaultLevelParms")
End Function

Public Function GetServerAdminPassword() As String
    GetServerAdminPassword = Passwords.GetSingleValue("Admin")
End Function

Public Function GetLevelDirectoryList() As String
    GetLevelDirectoryList = Gate.GetSingleValue("LevelDirectoryList")
End Function

'/////////////////////////////////////////////////////////
' Startup and shutdown.
'/////////////////////////////////////////////////////////

'
' Initialize the Gatekeeper.
'
Public Sub Startup(AutolaunchLevels As Boolean)

    ' Init misc globals.
    AdminNum = 0

    ' Setup the log.
    Set Log = New GateLog
    Log.OpenFile "GateSrv.log"
    Log.Log "Gatekeeper initializing."
    
    ' Setup the db manager.
    Set DbMan = New DbManager
    DbMan.Startup Me
            
    ' Setup the level manager.
    Set LevelMan = New LevelManager
    Call LevelMan.Startup(Log, DbMan, Me)
    
    ' Register user list hook.
    Set Users = DbMan.NewDbHook("Users", _
        "List of all logged-in users", _
        GKACCESS_Info, GKACCESS_None, _
        "User;Type;Team;ProtocolVer;App;AppVer;Level;Time;Account;Access;Extra;LevelExtra", _
        "N;N;O;I;S;I;O;I;I;I;S;S", _
        False)
    
    ' Register information hooks.
    Set Gate = DbMan.NewDbHook("Gate", "Gatekeeper settings", _
        GKACCESS_Info, GKACCESS_Admin, _
        "Gate;GatekeeperName;Port;IsUplinked;UplinkList;ContactName;ContactEmail;ContactOrganization;ContactWeb;LevelDirectoryList", _
        "N;N;I;B;S;S;S;S;S;S", _
        True)
    Call Gate.NoteDbEdit("Gate", _
        "GatekeeperName=" & GATESERVER_GATE_NAME & _
        " Port=" & DEFAULT_GATE_PORT & " IsUplinked=0" & _
        " UplinkList=" & Quotes(DEFAULT_PEER_UPLINK_LIST) & _
        " LevelDirectoryList=" & Quotes(DEFAULT_LEVEL_DIRECTORY_LIST))

    Set Game = DbMan.NewDbHook("Game", "Game settings", _
        GKACCESS_Info, GKACCESS_Admin, _
        "Game;DefaultMaxLevelPlayers;DefaultMaxLevelWatchers;DefaultLevelParms", _
        "N;I;I;S", _
        True)
    Call Game.NoteDbEdit("Game", _
        "DefaultMaxLevelPlayers=" & DEFAULT_MAX_LEVEL_PLAYERS & _
        " DefaultMaxLevelWatchers=" & DEFAULT_MAX_LEVEL_WATCHERS & _
        " DefaultLevelParms=" & Quotes(DEFAULT_LEVEL_PARMS))
    
    Set Passwords = DbMan.NewDbHook("Passwords", "Gatekeeper passwords", _
        GKACCESS_Admin, GKACCESS_Admin, _
        "Passwords;Admin;Info;Downlink;DefaultLevelAdmin;DefaultLevelPlayer;DefaultLevelWatcher", _
        "N;P;P;P;P;P;P", _
        True)
    Call Passwords.NoteDbEdit("Passwords", "")

    Set TimeLimits = DbMan.NewDbHook("TimeLimits", "Gatekeeper time limits", _
        GKACCESS_Info, GKACCESS_Admin, _
        "TimeLimits;LevelStartupTimeout;ClientIdleTimeout;UplinkTimeout;DownlinkTimeout", _
        "N;I;I;I;I", _
        True)
    Call TimeLimits.NoteDbEdit("TimeLimits", _
        "LevelStartupTimeout=" & GATESERVER_LEVEL_STARTUP_TIMEOUT & _
        " ClientIdleTimeout=" & GATESERVER_CLIENT_IDLE_TIMEOUT & _
        " UplinkTimeout=" & GATESERVER_UPLINK_TIMEOUT & _
        " DownlinkTimeout=" & GATESERVER_DOWNLINK_TIMEOUT)

    ' Initialize connection list.
    Set ConnectionList = New GateConnectionList
    Call ConnectionList.Startup(Log, Me)
    
    ' Exec the startup files.
    LoadConfig
    LoadAutoexec
    
    ' Initialize the TCP listener.
    Set Listener = New GateListener
    Call Listener.Startup(ConnectionList, Log, Me, GetPort)
    
    ' Successfully initialized.
    Log.Log "Gatekeeper successfully started."
    
    ' Launch all startup levels.
    LevelMan.StartupLevels (AutolaunchLevels)
    
    ' Enable timer.
    Listener.Timer.Enabled = True
End Sub

'
' Terminate the Gatekeeper.
'
Public Sub Shutdown()
    
    ' Disable timer.
    Listener.Timer.Enabled = False
    
    ' Save defaults to the registry.
    SaveConfig

    ' Unregister hooks.
    DbMan.DeleteDbHook Users
    DbMan.DeleteDbHook Gate
    DbMan.DeleteDbHook Game
    DbMan.DeleteDbHook Passwords
    DbMan.DeleteDbHook TimeLimits
    
    ' Note that we're shutting down.
    Log.Log "Gatekeeper exiting."

    ' Shut down the listener.
    Listener.Shutdown
    Set Listener = Nothing

    ' Shut down the connection list; this closes all connections.
    ConnectionList.Shutdown
    Set ConnectionList = Nothing

    ' Close the level manager.
    LevelMan.Shutdown
    Set LevelMan = Nothing

    ' Close Db manager.
    DbMan.Shutdown
    Set DbMan = Nothing
    
    ' Successfully shut down.
    Log.Log "Gatekeeper successfully shut down."
    Log.CloseFile
    Set Log = Nothing
    
    ' Unload all loaded forms.
    Dim i As Long
    For i = Forms.Count - 1 To 0 Step -1
        Unload Forms(i)
    Next
End Sub

'/////////////////////////////////////////////////////////
' Defaults loading and saving.
'/////////////////////////////////////////////////////////

'
' Load the defaults from the config file.
'
Public Sub LoadConfig()
    
    Log.Log "Loading defaults from Config.gk."
    AdminExecFile "Config.gk"
End Sub

'
' Execute Autoexec.gk.
'
Public Sub LoadAutoexec()
    AdminExecFile "Autoexec.gk"
End Sub

'
' Save the defaults to the registry.
'
Public Sub SaveConfig()
    Dim FileNum As Long
    
    ' Do it.
    On Error GoTo FileError
    FileNum = FreeFile
    Open "Config.gk" For Output As #FileNum
    
    ' Write commentary.
    Print #FileNum, "rem Config.gk: Gatekeeper-generated configuration file."
    Print #FileNum, "rem Do not edit this! Edit Autoexec.gk instead."
    Print #FileNum, ""
    
    ' Save each database.
    Gate.SaveToFile FileNum
    Game.SaveToFile FileNum
    Passwords.SaveToFile FileNum
    LevelMan.SaveToFile FileNum
    
    ' Close it.
    Close FileNum
    
    ' Success.
    Log.Log "Saved defaults to Config.gk."
    Exit Sub

FileError:
    ' Failure.
    Log.Log "Error saving defaults to Config.gk."
End Sub

'/////////////////////////////////////////////////////////
' Command Line.
'/////////////////////////////////////////////////////////

Public Function Exec(CmdLine As String, Connection As Object) As Boolean
    Dim User As String, BaseUser As String, UserType As String
    Dim Password As String, Level As String, Team As String
    Dim Topic As String, Key As String, Value As String, Args As String
    Dim Conn As Object
    Dim i As Long, SetAccess As Long
    Dim Code As Long

    ' Default to success.
    Exec = True

    If GetCMD(CmdLine, "LOGIN") Then
        
        ' Login command.
        If Connection.Access And GKACCESS_LoggedIn Then
            
            ' Prevent multiple logins.
            Call Connection.Respond(GK_Failure_LoginOnceOnly, "", "You can only log in once.")
            Exit Function
        End If
            
        ' Attempt login.
    
        ' Grab name and password.
        User = NextSTRING(CmdLine)
        Password = NextSTRING(CmdLine)

        ' Expand and check user name.
        If Not ExpandUser(User, BaseUser) Then
        
            ' Invalid user name.
            Call Connection.Respond(GK_Failure_BadParameters, "", "Invalid name.")
            Exit Function
        End If
        
        ' See if user is already logged on.
        If CheckActiveUser(User, 0, Conn) Then
                
            ' Login name is already logged in.
            Call Connection.Respond(GK_Failure_LoginDuplicate, "", "Name is already logged in.")
            Log.Log "Duplicate login attempt by " & Connection.GetHost & ": " & User
        End If
        
        ' Check password.
        If Not CheckPassword(BaseUser, Password, _
            SetAccess, UserType) Then
        
            ' Bad password.
            Call Connection.Respond(GK_Failure_LoginUnrecongized, "", "Bad name or password.")
            Log.Log "Failed login attempt from " & Connection.GetHost & ": " & User
            Exit Function
        End If
        
        ' Log in the user.
        Call Connection.SetUser(User, UserType, _
            "", GKACCESS_LoggedIn Or SetAccess, _
            0, True)

        ' Tell the user his name.
        Call Connection.Respond(GK_Success_Login, User, "Welcome.")
        Log.Log "Successful login from " & Connection.GetHost & ": " & User
    
    ElseIf GetCMD(CmdLine, "BANISH") Then
        
        ' Banish <user>
        If Connection.CheckAccess(GKACCESS_Admin) Then
            User = NextSTRING(CmdLine)
            If CheckActiveUser(User, 0, Conn) Then
                ' Banish him.
                ConnectionList.RemoveConnection Conn
                Call Connection.Respond(GK_Success, "", "Banished.")
            Else
                ' User not found.
                Call Connection.Respond(GK_Failure_UserNotFound, "", "User missing or not found.")
            End If
        End If
    
    ElseIf GetCMD(CmdLine, "TELL") Then
        
        ' Tell <user_name> <message_str>
        If Connection.CheckAccess(GKACCESS_LoggedIn) Then
            'todo: Implement.
            Call Connection.Respond(GK_Success, "", "Told.")
        End If
    
    ElseIf GetCMD(CmdLine, "BROADCAST") Then
        
        ' Broadcast <message_str>
        If Connection.CheckAccess(GKACCESS_LoggedIn) Then
            'todo: Implement.
            Call Connection.Respond(GK_Success, "", "Broadcasted.")
        End If
    
    ElseIf GetCMD(CmdLine, "SAVECONFIG") Then
        
        ' Save configuration.
        If Connection.CheckAccess(GKACCESS_Admin) Then
            SaveConfig
            Call Connection.Respond(GK_Success, "", "Saved configuration.")
        End If
        
    ElseIf GetCMD(CmdLine, "NOTIFY") Then
    
        ' Send a notify to a user.
        If Connection.CheckAccess( _
            GKACCESS_Level Or GKACCESS_Admin) Then
        End If
    
        ' Get user connection.
        User = NextSTRING(CmdLine)
        If Not CheckActiveUser( _
            User, 0, Conn) Then
            
            ' User not found.
            Call Connection.Respond(GK_Failure_UserNotFound, "", "User not found.")
            Exit Function
        End If
        
        ' Get notify code.
        If Not NextNUMBER(CmdLine, Code) Then
            Call Connection.Respond(GK_Failure_BadParameters, "", "Missing notify code.")
            Exit Function
        End If
        
        ' Verify notify code.
        If Code < GK_Notify Or Code > GK_Notify_Max Then
            Call Connection.Respond(GK_Failure_BadParameters, "", "Bad notify code.")
            Exit Function
        End If

        ' Send notify.
        Call Conn.Respond(Code, "", NextSTRING(CmdLine))

        ' Success.
        Call Connection.Respond(GK_Success, "", "Notification sent.")
    
    ElseIf LevelMan.Exec(CmdLine, Connection) Then
        
        ' Level code handled it successfully.
    
    ElseIf DbMan.Exec(CmdLine, Connection) Then
        
        ' Database code handled it successfully.
    
    Else
        
        ' Unrecognized command.
        Exec = False
    
    End If
End Function

'
' Return number of logged in users.
'
Public Function NumLoggedInUsers() As Long
    NumLoggedInUsers = ConnectionList.NumConnections
End Function

'/////////////////////////////////////////////////////////
' Executing files.
'/////////////////////////////////////////////////////////

'
' Execute a file with Admin privelages.
' Returns True if executed, False if not.
'
Public Function AdminExecFile(Filename As String)
    Dim Connection As Object

    ' Create a virtual connection to exec this on.
    If ConnectionList.CreateVirtualConnection(Connection) Then
    
        ' Set the connection user.
        Call Connection.SetUser("Exec", "Admin", "", _
            GKACCESS_Admin Or GKACCESS_Info Or GKACCESS_Config, _
            0, False)
        
        ' Execute the commands.
        AdminExecFile = Connection.ExecFile(Filename)

        ' Remove the virtual connection.
        ConnectionList.RemoveConnection Connection
    End If
    
    ' Log success message.
    Log.Log "Exec " & Filename & IIf(AdminExecFile, " succeeded.", " failed.")
End Function

'/////////////////////////////////////////////////////////
' User related functions.
'/////////////////////////////////////////////////////////

'
' Verify that User is in the active user list and that he
' has all of the specified Access flags set.  Returns True
' if he is and sets Conn to his connection. Returns False
' if not.
'
Public Function CheckActiveUser(User As String, _
    Access As Long, Conn As Object) As Boolean
    
    ' See if this player is on the connection list.
    If ConnectionList.GetPlayerByName(User, Conn) Then
    
        ' The user is connected, so check his access.
        CheckActiveUser = _
            ((Conn.Access And Access) = Access)
    End If
End Function

'
' Expand a user name. If User contains a trailing *,
' replaces that * with a number such that the resulting
' user name is unique and valid. Returns True if the
' returned user name is valid, False otherwise.
'
' Sets BaseName to the user name without the trailing
' number, so that standard names like Admin and Info are
' preserved.  Returns True if the result is a
' valid, unique name.  Returns False if not.
'
Public Function ExpandUser(User As String, _
    BaseUser As String) As Boolean
    
    ' Locals.
    Dim i As Integer
    Dim Conn As Object
    
    ' See if this is wildcarded.
    If Right(User, 1) = "*" Then
    
        ' Unwildcard this.
        BaseUser = Left$(User, Len(User) - 1)
        
        ' See if we can make this into a unique name.
        For i = 0 To GATESERVER_MAX_CONNECTIONS
        
            ' Form expanded user name.
            User = BaseUser & Trim$(Str$(i))
            
            ' Validate the name.
            If Not IsValidName(User) Then
            
                ' This expansion isn't a valid name.
                Exit Function
            End If
        
            ' See if this name is in the active list.
            If Not ConnectionList.GetPlayerByName(User, Conn) Then
            
                ' Success!
                ExpandUser = True
                Exit Function
            End If
        Next
        ' Failed.
    Else
    
        ' Only return whether this user is valid.
        BaseUser = User
        ExpandUser = IsValidName(User)
    End If
End Function

'
' Return whether the specified password is valid for the
' specified user. The specified user name must not be
' wildcarded.  If the password is accepted, sets Access
' to the user's access flags which should explicitly be
' set and returns True. Otherwise returns False.
'
Public Function CheckPassword( _
    BaseUser As String, _
    Password As String, _
    ByRef SetAccess As Long, _
    ByRef UserType As String) As Boolean

    ' Simplistic password check.
    If LevelMan.CheckLevelUser(BaseUser, Password, _
        CheckPassword) Then

        ' This user is a level server.
        SetAccess = GKACCESS_Level Or GKACCESS_Info Or GKACCESS_Admin
        UserType = "Level"

    ElseIf StrComp(BaseUser, "Admin", 1) = 0 Then

        ' This user is an administrator.
        CheckPassword = StrComp(Password, GetServerAdminPassword, 1) = 0
        SetAccess = GKACCESS_Admin + GKACCESS_Info
        UserType = "Admin"

    Else

        ' This user is a player.
        CheckPassword = StrComp(Password, GetServerInfoPassword, 1) = 0
        SetAccess = GKACCESS_Info
        UserType = "Player"

    End If
End Function

'/////////////////////////////////////////////////////////
' Timer tick.
'/////////////////////////////////////////////////////////

'
' Timer tick. Called by GateListener's timer.
' Only called while we're up and running.
'
Public Sub Tick()

    ' Tick the level manager.
    LevelMan.Tick
End Sub

'/////////////////////////////////////////////////////////
' Critical error handler.
'/////////////////////////////////////////////////////////

'
' Handle an unrecoverable error.
'
Sub AppError(Msg As String)
    Log.Log "Critical error:"
    Log.Log Msg
    Log.CloseFile
    MsgBox Msg
    End
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
