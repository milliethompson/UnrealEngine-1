VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "LevelManager"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' GateLev.cls: Gatekeeper server level manager.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Manages list of levels, launching levels, and all
'   stuff linking the Gatekeeper to levels.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Public variables.
'/////////////////////////////////////////////////////////

' Held objects.
Private Log As GateLog
Private DbMan As DbManager
Private Keeper As Gatekeeper

' The database hooks we track.
Private Levels As DbHook ' Level list.
Private LevelParms As DbHook ' Level parameters.

'/////////////////////////////////////////////////////////
' Startup and shutdown.
'/////////////////////////////////////////////////////////

'
' Init the level manager.
'
Public Sub Startup(ThisLog As GateLog, _
    ThisDbMan As DbManager, ThisKeeper As Gatekeeper)

    ' Remember objects.
    Set Log = ThisLog
    Set DbMan = ThisDbMan
    Set Keeper = ThisKeeper

    ' Register Levels hook.
    Set Levels = DbMan.NewDbHook("Levels", _
        "List of all game levels", _
        GKACCESS_Info, GKACCESS_Config, _
        "Level;State;RequestState;Action;App;Ver;Pkgs;Port;Users;Visits;Time;Pid;Extra", _
        "N;N;N;S;S;I;S;I;I;I;I;I;S", _
        False)

    ' Register LevelParms hook.
    Set LevelParms = DbMan.NewDbHook("LevelParms", _
        "Level parameters", _
        GKACCESS_Admin, GKACCESS_Admin, _
        "Level;AutoLaunch;MaxPlayers;MaxWatchers;LevelParms;AdminPassword;PlayerPassword;WatcherPassword;GateInfo;LevelFile;GatePassword", _
        "N;B;I;I;S;P;P;P;S;S;P", _
        False)

    ' Successfully initialized.
    Log.Log "Level manager successfully started."
End Sub

'
' Startup all levels, initializing them to a valid
' state at server start time.  If Autolaunch is
' True, attempts to up each level.
'
Public Sub StartupLevels(Autolaunch As Boolean)
    Dim i As Long
    
    ' Reset all levels to Down.
    For i = 0 To Levels.NumRecords - 1
        Call Levels.NoteDbEditIndex(i, _
            "State=Down RequestState=None Port=0" & _
            " App= Ver=0 Pkgs=" & _
            " Time=0 Pid=0 Extra= Action=" & Quotes("Not yet launched"), _
            False)
    Next
    
    ' Reset all LevelParms
    For i = 0 To LevelParms.NumRecords - 1
        Call LevelParms.NoteDbEditIndex(i, _
            "GateInfo= LevelFile= GatePassword=", _
            False)
    Next
    
    ' Autolaunch all desired levels.
    If Autolaunch Then
        For i = 0 To Levels.NumRecords - 1
            If Val(LevelParms.GetValueByIndexAndKey(i, _
                "AutoLaunch")) Then
                
                ' Up it.
                UpLevel i
            End If
        Next
    End If
End Sub

'
' Shut down the level manager.
'
Public Sub Shutdown()
    
    ' Message.
    Log.Log "Level manager shutting down."

    ' Unregister hooks.
    DbMan.DeleteDbHook LevelParms
    DbMan.DeleteDbHook Levels

    ' Release objects.
    Set Log = Nothing
    Set DbMan = Nothing
    Set Keeper = Nothing
End Sub

'/////////////////////////////////////////////////////////
' Nofification of connections disconnecting.
'/////////////////////////////////////////////////////////

'
' Note that a connection is about to disconnect
' or has disconnected.
'
Public Sub NoteDisconnect(Conn As Object)
    Dim Index As Long
    
    ' See if this connection is a level server.
    If Conn.Access And GKACCESS_Level Then

        ' Get index of this level server's level.
        Index = Levels.GetIndexByKey(Conn.UserName)
        
        ' Level has probably been deleted.
        If Index >= 0 Then
        
            ' Mark this level as down.
            Call NoteLevelDown(Conn.UserName, _
                "Server disconnected")
        End If
    End If
End Sub

'/////////////////////////////////////////////////////////
' Level upping and downing.
'/////////////////////////////////////////////////////////

'
' Find the path that a named level resides in.
' Returns an empty string if the level was not found
' in any of our level paths.
'
Private Function FindLevelPath(LevelName As String) As String
    Dim TempList As String, TempFile As String
    Dim Temp As String
    
    ' Go through all directories and try to find file.
    TempList = Keeper.GetLevelDirectoryList
    While TempList <> ""

        ' Get next directory and mask.
        Temp = NextSTRING(TempList, ";")

        ' Go through all files.
        TempFile = Dir(Temp)
        While TempFile <> ""
        
            ' See if this matches.
            If StrComp(LevelName, TempFile, 1) = 0 Then
            
                'todo: Fix this so that it always returns
                ' a proper relative pathname. This code is
                ' not very general.
            
                ' Strip wildcard.
                If InStr(Temp, "*") <> 0 Then
                    Temp = Left(Temp, InStr(Temp, "*") - 1)
                End If
                
                ' Fixup for relative path.
                If Left(Temp, 2) = ".." Then
                    Temp = CurDir$ & "\" & Temp
                End If
                
                ' Add trailing slash.
                If Right(Temp, 1) <> "\" And Right(Temp, 1) <> "/" Then
                    Temp = Temp & "\"
                End If
                FindLevelPath = Temp
                Exit Function
            End If
            
            ' Next file.
            TempFile = Dir()
        Wend
    Wend
End Function

'
' Convert ShellExecute result to error code
'
Public Function ExecuteError(Code As Long) As String
    Select Case Code
        Case 0: ExecuteError = "No Error"
        Case 1: ExecuteError = "Invalid function"
        Case 2: ExecuteError = "File not found"
        Case 3: ExecuteError = "Path not found"
        Case 4: ExecuteError = "Too many open files"
        Case 5: ExecuteError = "Access denied"
        Case 6: ExecuteError = "Invalid handle"
        Case 7: ExecuteError = "Arena trashed"
        Case 8: ExecuteError = "Not enough memory"
        Case 9: ExecuteError = "Invalid block"
        Case 10: ExecuteError = "Bad environment"
        Case 11: ExecuteError = "Bad format"
        Case 12: ExecuteError = "Invalid access"
        Case 13: ExecuteError = "Invalid data"
        Case 14: ExecuteError = "Reserved error"
        Case 15: ExecuteError = "Invalid drive"
        Case 16: ExecuteError = "Current directory"
        Case 17: ExecuteError = "Not same device"
        Case 18: ExecuteError = "No more files"
        Case 19: ExecuteError = "Write protect"
        Case 20: ExecuteError = "Bad unit"
        Case 21: ExecuteError = "Not ready"
        Case 22: ExecuteError = "Bad command"
        Case 23: ExecuteError = "Crc error"
        Case 24: ExecuteError = "Bad length"
        Case 25: ExecuteError = "Seek error"
        Case 26: ExecuteError = "Format error"
        Case 27: ExecuteError = "Sector not found"
        Case 28: ExecuteError = "Out of paper"
        Case 29: ExecuteError = "Write fault"
        Case 30: ExecuteError = "Read fault"
        Case 31: ExecuteError = "General failure"
        Case Else: ExecuteError = "Unknown error"
    End Select
End Function

'
' Attempt to bring a level up.
'
Private Sub UpLevel(Index As Long)
    Dim LevelPath As String, LevelName As String
    Dim LevelParmsIndex As Long
    Dim Parms As String
    Dim Conn As Object
    
    ' Get level name.
    LevelName = Levels.GetKeyByIndex(Index)

    ' Skip if the level is not down.
    If StrComp( _
        Levels.GetValueByIndexAndKey(Index, "State"), _
        "Down", 1) <> 0 Then Exit Sub
        
    ' Skip if the level is being upped now.
    If StrComp( _
        Levels.GetValueByIndexAndKey(Index, "RequestState"), _
        "Up", 1) = 0 Then Exit Sub

    ' If a player is logged in with a name matching this
    ' level name, we can't launch the level.
    If Keeper.ConnectionList.GetPlayerByName(LevelName, _
        Conn) Then

        ' Some user is already using this name.
        ' Is it a level server?
        If (Conn.Access And GKACCESS_Level) <> GKACCESS_Level Then
            Call Levels.NoteDbEditIndex(Index, _
                "Action=" & Quotes("Name is in use"), False)
            Exit Sub
        End If
    End If
    
    ' Find the level's path.
    LevelPath = FindLevelPath(LevelName)
    If LevelPath = "" Then
        
        ' Update the database.
        Call Levels.NoteDbEditIndex(Index, _
            "Action=" & Quotes("Can't find file"), False)
    
        ' Write message to log.
        Log.Log "Level up (" & LevelName & ") failed: " & "Can't find file"
        Exit Sub
    End If
        
    ' Use ShellExecute to execute the level file
    ' itself. Windows uses its association to
    ' launch the appropriate game server exe.
    Dim Result As Long, ResultExe As String
    ResultExe = Space(1000)
    Result = FindExecutable( _
        LevelPath & LevelName, _
        LevelPath, _
        ResultExe)
    If Result < 32 Then

        ' Find exe failed.
        Call Levels.NoteDbEditIndex(Index, _
            "Action=" & Quotes(ExecuteError(Result)), False)

        ' Write message to log.
        Log.Log "Level up (" & LevelName & ") executable not found: " & ExecuteError(Result)
        Exit Sub
    End If
        
    ' Build StartupInfo
    Dim StartupInf As STARTUPINFO
    StartupInf.cb = STARTUPINFO_SIZE
    
    ' Build ProcessInfo.
    Dim ProcessInf As PROCESS_INFORMATION
    Dim Password As String
    
    ' Set the appropriate LevelParms.
    Password = RandomName(16)
    Call LevelParms.NoteDbEdit(LevelName, _
        "GateInfo=" & Quotes("name:pswd@localhost:port") & _
        " GatePassword=" & Quotes(Password) & _
        " LevelFile=" & Quotes(LevelPath & LevelName))

    ' Build command line parameters by performing
    ' parameter substitution, combining the
    ' specified command-line parameters with
    ' the substitutable information in this record
    ' of the LevelParms database. This enables
    ' parameters to be specified like:
    '
    '   file=%LevelFile% pswd=%AdminPassword%
    '
    ' Where the values such as %LevelFile% are
    ' replaced with the appropriate value from the
    ' LevelParms record.
    
    ' Get this index in LevelParms.
    LevelParmsIndex = LevelParms.GetIndexByKey(LevelName)

    ' Gate base (unsubstituted) level parameters.
    Parms = LevelParms.GetValueByIndexAndKey( _
        LevelParmsIndex, "LevelParms")
        
    ' If level didn't have any parms, use the
    ' default parms.
    If Parms = "" Then Parms = Keeper.GetDefaultLevelParms
    
    Dim CmdLine As String
    CmdLine = LevelParms.SubstituteParms( _
        LevelParmsIndex, Parms)
    
    ' Attempt to create the process.
    If CreateProcess( _
        ResultExe, _
        CmdLine, _
        0, 0, _
        0, 0, 0, _
        LevelPath, _
        StartupInf, _
        ProcessInf) = 0 Then
        
        ' Launch failed.
        Call Levels.NoteDbEditIndex(Index, _
            "Action=" & Quotes("CreateProcess failed " & GetLastError), _
            False)
    
        ' Write message to log.
        Log.Log "Level up (" & LevelName & "): CreateProcess failed"
        Exit Sub
    End If
    
    ' Launch succeeded thus far.
    Call Levels.NoteDbEditIndex(Index, _
        "RequestState=Up" & _
        " Action=" & Quotes("Launching...") & _
        " Time=" & GateTimer & _
        " Pid=" & ProcessInf.hProcess, _
        False)

    ' Write message to log.
    Log.Log "Level up (" & LevelName & "): Launching"
End Sub

'
' Attempt to bring a level down.
'
Private Sub DownLevel(Index As Long)
    Dim LevelName As String
    
    ' Get level name.
    LevelName = Levels.GetKeyByIndex(Index)
    
    ' Skip if the level isn't up.
    If StrComp( _
        Levels.GetValueByIndexAndKey(Index, "State"), _
        "Up", 1) <> 0 Then Exit Sub
    
    ' Skip if the level is already being downed.
    If StrComp( _
        Levels.GetValueByIndexAndKey(Index, "RequestState"), _
        "Down", 1) = 0 Then Exit Sub

    ' Update the database.
    Call Levels.NoteDbEditIndex(Index, _
        "RequestState=Down Action=" & Quotes("Downing..."), _
        False)

    ' Write message to log.
    Log.Log "Level down (" & LevelName & "): Downing"
End Sub

'
' Remove all players from a downed level.
'
Private Sub NoteLevelDown(LevelName As String, _
    Reason As String)
    Dim i As Long
    Dim Conn As Object

    ' Kick all players out of the level.
    For i = 0 To Keeper.ConnectionList.NumConnections - 1
        Set Conn = Keeper.ConnectionList.GetConnection(i)
        Call Conn.SetLevel("", "User", "Level downed.", True)
    Next

    ' Set level state.
    Call Levels.NoteDbEdit(Conn.UserName, _
        "State=Down RequestState=None" & _
        "Port=0 Time=0 Pid=0 Extra=" & _
        " App= Ver=0 Pkgs=" & _
        " Action=" & Quotes(Reason))
End Sub

'
' Check that the specified connection is a level server.
' Silently returns True if it is, otherwise returns False.
'
Private Function CheckLevelServer(Connection As Object, _
    ByRef Index As Long) As Boolean

    ' Make sure this user is a level server.
    If Not Connection.CheckAccess(GKACCESS_Level) Then
        Exit Function
    End If
    
    ' Get index of this level server's level.
    Index = Levels.GetIndexByKey(Connection.UserName)
    
    ' Level has probably been deleted.
    If Index < 0 Then
        Call Connection.Respond(GK_Failure_LevelNotFound, "", "Level not found.")
        Exit Function
    End If
    
    ' Success!
    CheckLevelServer = True
End Function
            

'/////////////////////////////////////////////////////////
' Command line execution.
'/////////////////////////////////////////////////////////

Public Function Exec(CmdLine As String, _
    Connection As Object) As Boolean
    
    ' Locals.
    Dim TempList As String, Temp As String
    Dim TempResult As String, TempFile As String
    Dim TempExtList As String, LevelName As String
    Dim NewState As String, Action As String
    Dim Index As Long, Port As Long, Ver As Long, ParmsIndex As Long
    Dim App As String, Pkgs As String
    Dim Conn As Object, Role As String, Password As String
    Dim Num As Long, Max As Long, i As Long
    
    ' Default to success.
    Exec = True
    
    ' Find command.
    If GetCMD(CmdLine, "LEVELFILES") Then
        
        ' Get list of files in the level directories
        ' which aren't already in the level list.
        If Connection.CheckAccess(GKACCESS_Admin) Then
        
            ' Go through all directories and collect filenames.
            TempList = Keeper.GetLevelDirectoryList
            While TempList <> ""
            
                ' Get next directory and mask.
                Temp = NextSTRING(TempList, ";")
                
                ' Go through all files.
                TempFile = Dir(Temp)
                While TempFile <> ""
                
                    ' Make sure this name is valid and it
                    ' isn't already in the level list.
                    If IsValidName(TempFile) And _
                        Levels.GetIndexByKey(TempFile) < 0 Then
                        
                        If TempResult <> "" Then TempResult = TempResult & ";"
                        TempResult = TempResult & TempFile
                    End If
                    
                    ' Next file.
                    TempFile = Dir()
                Wend
            Wend
            Call Connection.Respond(GK_Success_FileList, TempResult, "")
        End If
    
    ElseIf GetCMD(CmdLine, "ADDLEVEL") Then
    
        ' Add a level to the list.
        If Connection.CheckAccess(GKACCESS_Admin) Then
            Temp = NextSTRING(CmdLine)
            If IsValidName(Temp) Then
                Call Levels.NoteDbEdit(Temp, "State=Down RequestState=None Action=" & Quotes("Newly created"))
                Call LevelParms.NoteDbEdit(Temp, "")
                Call Connection.Respond(GK_Success, "", "Level added.")
            
                ' Write message to log.
                Log.Log "Added level " & Temp
            Else
                Call Connection.Respond(GK_Failure_BadParameters, "", "Bad or missing level name.")
            End If
        End If
    
    ElseIf GetCMD(CmdLine, "DELETELEVEL") Then
    
        ' Delete a level from the list.
        If Connection.CheckAccess(GKACCESS_Admin) Then
            'todo: If running, down it.
            Temp = NextSTRING(CmdLine)
            If Levels.GetIndexByKey(Temp) >= 0 Then
                Call Levels.NoteDbDelete(Temp)
                Call LevelParms.NoteDbDelete(Temp)
                Call Connection.Respond(GK_Success, "", "Level deleted.")
            
                ' Write message to log.
                Log.Log "Deleted level " & Temp
            Else
                Call Connection.Respond(GK_Failure_LevelNotFound, "", "Level not found.")
            End If
        End If
    
    ElseIf GetCMD(CmdLine, "UPLEVEL") Then
    
        ' Bring the level up.
        If Connection.CheckAccess(GKACCESS_Admin) Then
            If GetLevelName(Connection, CmdLine, LevelName, Index) Then
                Call UpLevel(Index)
                Call Connection.Respond(GK_Success, "", "Upping level.")
            End If
        End If
    
    ElseIf GetCMD(CmdLine, "DOWNLEVEL") Then
    
        ' Bring the level down.
        If Connection.CheckAccess(GKACCESS_Admin) Then
            If GetLevelName(Connection, CmdLine, LevelName, Index) Then
                Call DownLevel(Index)
                Call Connection.Respond(GK_Success, "", "Downing level.")
            End If
        End If

    ElseIf GetCMD(CmdLine, "BEGINLEVEL") Then

        ' Begin the level.
        If CheckLevelServer(Connection, Index) Then
        
            ' Get level port.
            If Not NextNUMBER(CmdLine, Port) Then
                Call Connection.Respond(GK_Failure_BadParameters, "", "Missing level port.")
                Exit Function
            End If
            
            ' Get app name.
            App = NextSTRING(CmdLine)
            If App = "" Then
                Call Connection.Respond(GK_Failure_BadParameters, "", "Missing level app name.")
                Exit Function
            End If
            
            ' Get app version.
            If Not NextNUMBER(CmdLine, Ver) Then
                Call Connection.Respond(GK_Failure_BadParameters, "", "Missing level app version.")
                Exit Function
            End If

            ' Get optional packages.
            Pkgs = NextSTRING(CmdLine)

            ' Mark this level as up.
            Call Levels.NoteDbEdit( _
                Connection.UserName, _
                "State=Up RequestState=None" & _
                " Port=" & Port & _
                " App=" & Quotes(App) & _
                " Ver=" & Ver & _
                " Pkgs=" & Quotes(Pkgs) & _
                " Action=" & Quotes("Upped by level server"))

            ' Success.
            Call Connection.Respond(GK_Success, "", "Level is up.")
        End If
    
    ElseIf GetCMD(CmdLine, "KICK") Then

        ' Kick a player out of his level.
        If Not Connection.CheckAccess( _
            GKACCESS_Level Or GKACCESS_Admin) Then
            Exit Function
        End If

        ' Get user connection.
        If Not Keeper.CheckActiveUser( _
            NextSTRING(CmdLine), 0, Conn) Then
            
            ' User not found.
            Call Connection.Respond(GK_Failure_UserNotFound, "", "User not found.")
            Exit Function
        End If

        ' Make sure user is in a level.
        If Conn.Level = "" Then
            Call Connection.Respond(GK_Failure_UserNotInLevel, "", "User not in level.")
            Exit Function
        End If
        
        ' Get Kick message.
        Temp = NextSTRING(CmdLine)
        If Temp = "" Then Temp = "Kicked out by " & Connection.UserName

        ' Kick the user out.
        Call Conn.SetLevel("", "User", Temp, True)
        Call Connection.Respond(GK_Success, "", "Kicked user out of level.")

    ElseIf GetCMD(CmdLine, "USEREXTRA") Then
    
        ' Set user's public game specific information.
        If CheckLevelServer(Connection, Index) Then
        
            ' Get user connection.
            If Not Keeper.CheckActiveUser( _
                NextSTRING(CmdLine), 0, Conn) Then
                
                ' User not found.
                Call Connection.Respond(GK_Failure_UserNotFound, "", "User not found.")
                Exit Function
            End If
            
            ' Update user's LevelExtra.
            Conn.LevelExtra = NextSTRING(CmdLine)
            Conn.UpdateUserHook
            Call Connection.Respond(GK_Success, "", "Set user extra.")
        End If

    ElseIf GetCMD(CmdLine, "LEVELEXTRA") Then
    
        ' Set level's public game specific information.
        If CheckLevelServer(Connection, Index) Then
            Call Levels.NoteDbEdit(Connection.UserName, _
                "Extra=" & Quotes(NextSTRING(CmdLine)))
            Call Connection.Respond(GK_Success, "", "Set level extra.")
        End If
    
    ElseIf GetCMD(CmdLine, "WATCH") Then
    
        ' Enter a level as a spectator.
        Role = "Watcher"
        GoTo TryEnterLevel
    
    ElseIf GetCMD(CmdLine, "PLAY") Then

        ' Play a level.
        Role = "Player"

        ' Try to enter a level:
TryEnterLevel:

        ' Check access.
        If Not Connection.CheckAccess(GKACCESS_LoggedIn) Then
            Exit Function
        End If

        ' Make sure player isn't already in a level.
        If Connection.Level <> "" Then
            Call Connection.Respond(GK_Failure_InLevel, "", "Already in level.")
            Exit Function
        End If
        
        ' Get level.
        LevelName = NextSTRING(CmdLine)
        Index = Levels.GetIndexByKey(LevelName)
        ParmsIndex = LevelParms.GetIndexByKey(LevelName)
        If Index < 0 Or ParmsIndex < 0 Then
            Call Connection.Respond(GK_Failure_LevelNotFound, "", "Level not found.")
            Exit Function
        End If

        ' Get Max.
        Max = Val(LevelParms.GetValueByIndexAndKey(ParmsIndex, "Max" & Role & "s"))
        If Max = 0 Then Max = Val(Keeper.Game.GetSingleValue("DefaultMaxLevel" & Role & "s"))

        ' Get Num
        For i = 0 To Keeper.ConnectionList.NumConnections - 1
            Set Conn = Keeper.ConnectionList.GetConnection(i)
            If StrComp(Conn.Level, LevelName, 1) = 0 And _
                StrComp(Conn.UserType, Role, 1) = 0 Then
                Num = Num + 1
            End If
        Next
        
        ' Get password.
        Password = LevelParms.GetValueByIndexAndKey(ParmsIndex, Role & "Password")
        If Password = "" Then Password = Keeper.Passwords.GetSingleValue("DefaultLevel" & Role)
        
        ' Check password.
        If StrComp(NextSTRING(CmdLine), _
            Password, 1) <> 0 Then
            
            ' Bad password.
            Call Connection.Respond(GK_Failure_LevelPassword, "", "Bad level password.")
            Exit Function
        End If
        
        ' Make sure level is up.
        If StrComp( _
            Levels.GetValueByIndexAndKey(Index, "State"), _
            "Up", 1) <> 0 Then
            
            Call Connection.Respond(GK_Failure_WrongLevelState, "", "Level is not up.")
            Exit Function
        End If
        
        ' Make sure level isn't full.
        If Num >= Max Then
            Call Connection.Respond(GK_Failure_LevelFull, "", "Level is full (" & Num & "/" & Max & ").")
            Exit Function
        End If
        
        ' Add player to level.
        Call Connection.SetLevel( _
            Levels.GetKeyByIndex(Index), _
            Role, "", True)
        Call Connection.Respond(GK_Success, "", "Entered level.")
    
    ElseIf GetCMD(CmdLine, "LEVELEXEC") Then
    
        ' Send a command to the level server.
        If Connection.CheckAccess(GKACCESS_InLevel) Then
            If Keeper.ConnectionList.GetPlayerByName( _
                Connection.Level, Conn) Then
                
                    ' Send message to level.
                    Call Conn.Notify(GK_Notify_LevelExec, _
                        "", Quotes(CmdLine))
                End If

            ' This always succeeds if we're in a level, since
            ' the level's response, if any, will come
            ' asynchronously via a notify.
            Call Connection.Respond(GK_Success, "", "Level command issued.")
        End If
    
    Else
    
        ' Command not found.
        Exec = False
    End If
End Function

'/////////////////////////////////////////////////////////
' Timer tick.
'/////////////////////////////////////////////////////////

'
' Timer tick: Updates the status of all levels.
' Detects crashed levels.
'
Public Sub Tick()
    Dim i As Long, Pid As Long, ExitCode As Long
    Dim Level As String, State As String
        
    ' Check each level.
    For i = 0 To Levels.NumRecords - 1
    
        ' Get level name and state state.
        Level = Levels.GetKeyByIndex(i)
        State = Levels.GetValueByIndexAndKey(i, "State")
        If StrComp(State, "Up", 1) = 0 Then

            ' Get process exit code.
            Pid = Val(Levels.GetValueByIndexAndKey(i, "Pid"))
            If GetExitCodeProcess(Pid, ExitCode) = 0 Then
            
                ' Error getting exit code:
                ' something is f****d.
                Call NoteLevelDown(Level, _
                    "Vanished")
            ElseIf ExitCode <> STILL_ACTIVE Then
            
                ' Process has terminated.
                Call NoteLevelDown(Level, _
                    "Crashed")
            End If
        End If
    Next
End Sub

'/////////////////////////////////////////////////////////
' Helpers.
'/////////////////////////////////////////////////////////

'
' Get a level name from the command line.
' Silently returns True and sets LevelName if success.
' Returns false and sends an error message if failure.
'
Private Function GetLevelName(Connection As Object, _
    ByRef CmdLine As String, _
    ByRef LevelName As String, _
    ByRef Index As Long) As Boolean
    
    ' Get name.
    LevelName = NextSTRING(CmdLine)
    
    ' Make sure it's a valid name.
    If IsValidName(LevelName) Then
    
        ' Make sure it exists in the level database.
        Index = Levels.GetIndexByKey(LevelName)
        If Index >= 0 Then
        
            ' Success.
            GetLevelName = True
        Else
        
            ' Level not found.
            Call Connection.Respond(GK_Failure_LevelNotFound, "", "Level not found.")
        End If
    Else
    
        ' Invalud name.
        Call Connection.Respond(GK_Failure_BadParameters, "", "Bad or missing level name.")
    End If
End Function

'/////////////////////////////////////////////////////////
' Password checking.
'/////////////////////////////////////////////////////////

'
' See if the specified name corresponds to a level.
'
' Returns True if the name is a level name, False
' if not.
'
' Sets PasswordIsOk to True if the specified password
' is correct and the level server user can log in, or
' False if either the password is incorrect, or the level
' is not in a state that allows the level server to log in.
'
Public Function CheckLevelUser( _
    LevelName As String, _
    LevelPassword As String, _
    ByRef PasswordIsOk As Boolean) As Boolean
    
    ' Locals
    Dim State As String, Password As String
    Dim Index As Long, ParmsIndex As Long
    
    ' Get index of this level name in the Levels database.
    Index = Levels.GetIndexByKey(LevelName)
    ParmsIndex = LevelParms.GetIndexByKey(LevelName)
    If Index >= 0 Then
    
        ' This is a valid level name.
        CheckLevelUser = True
        
        If ParmsIndex >= 0 Then
            
            ' Make sure this is a valid state for loggin in.
            State = Levels.GetValueByIndexAndKey(Index, "State")
            Password = LevelParms.GetValueByIndexAndKey(ParmsIndex, "GatePassword")
            
            ' Now check the password.
            PasswordIsOk = (StrComp(Password, LevelPassword, 1) = 0)
        End If
    End If
End Function

'/////////////////////////////////////////////////////////
' Saving.
'/////////////////////////////////////////////////////////

'
' Save persistent info to file.
'
Public Sub SaveToFile(FileNum As Long)
    Levels.SaveToFile FileNum
    LevelParms.SaveToFile FileNum
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
