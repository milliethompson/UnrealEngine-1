VERSION 4.00
Begin VB.Form GateConnection 
   Caption         =   "GateServer Connection"
   ClientHeight    =   2235
   ClientLeft      =   5550
   ClientTop       =   1320
   ClientWidth     =   3840
   Height          =   2595
   Icon            =   "GateConn.frx":0000
   Left            =   5490
   LinkTopic       =   "Form1"
   ScaleHeight     =   2235
   ScaleWidth      =   3840
   Top             =   1020
   Width           =   3960
   Begin WINSOCKLib.TCP TCP 
      Left            =   1560
      Top             =   900
      _ExtentX        =   635
      _ExtentY        =   635
      RemoteHost      =   ""
      RemotePort      =   0
      LocalPort       =   0
   End
End
Attribute VB_Name = "GateConnection"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' GateConn.frm: Gatekeeper server TCP connection handler.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Manages one active TCP connection initiated by
'   the listener form.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Private general variables.
'/////////////////////////////////////////////////////////

' GateLog instance to use for logging.
Private Log As GateLog
Private ConnectionList As GateConnectionList
Private Keeper As Gatekeeper

' Set when a command is executing.
Private IsAtPrompt As Boolean

'/////////////////////////////////////////////////////////
' Per-connection variables.
'/////////////////////////////////////////////////////////

' Connection state; nondefault when Access and GKACCESS_LoggedIn.
Public UserName As String
Public UserType As String
Public Team As String

' Whether this is a real or virtual connection.
Public IsVirtual As Boolean

' Access flags for the connection, always valid.
Public Access As Long

' Optional application info for client.
Public ClientProtocolVersion As Long
Public ClientApplication As String
Public ClientVersion As Long

' Level info; nondefault when Access And GKACCESS_InLevel.
Public Level As String
Public LevelExtra As String

' Connection settings.
Public Verbose As Boolean
Public Extra As String

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

' Connection timing info.
Public ConnectionStartTime As Long
Public LastReceiveTime As Long

' The command line we're accumulating.
Public CmdLine As String

' All databases we're subscribed to.
Private Subscriptions(GATESERVER_MAX_DATA_NOTIFYS) As DbHook
Private Subscribed(GATESERVER_MAX_DATA_NOTIFYS) As Boolean

'/////////////////////////////////////////////////////////
' Database related.
'/////////////////////////////////////////////////////////

'
' Update the database hook for this user if he is
' logged in.
'
Public Sub UpdateUserHook()

    ' Skip if not logged in (name key would be duplicate).
    If (Access And GKACCESS_LoggedIn) And _
        (UserName <> "") Then
       
        ' Update the hook.
        Call Keeper.Users.NoteDbEdit(UserName, _
            "Type=" & UserType & _
            " Team=" & Team & _
            " ProtocolVer=" & ClientProtocolVersion & _
            " App=" & ClientApplication & _
            " AppVer=" & ClientVersion & _
            " Level=" & Level & " LevelExtra=" & Quotes(LevelExtra) & _
            " Time=" & " Account=" & " Extra=" & Quotes(Extra) & _
            " Access=" & Access & " Extra=" & Quotes(Extra))
    End If
End Sub

'
' Create a new subscription code.  This connection will
' think it's subscribed with that code until
' DeleteSubscriptionCode is called.
'
' Returns <0 if a new subscription code couldn't be
' created.
'
Public Function NewSubscriptionCode(Hook As DbHook) As Long
    Dim i As Long

    ' Find an available slot.
    For i = 0 To GATESERVER_MAX_DATA_NOTIFYS - 1
        If Not Subscribed(i) Then

            ' Use this subscription index.
            Set Subscriptions(i) = Hook
            Subscribed(i) = True

            ' Return the corresponding notification code.
            NewSubscriptionCode = GK_DbNotify + i + 1
            Exit Function
        End If
    Next
    
    ' No slots are available; return failure.
    NewSubscriptionCode = -1
End Function

'
' Unsubscribe this connection from the database
' corresponding to a certain code. Returns True
' if we were subscribed, False if we weren't
' subscribed to the code. Handles invalid
' codes gracefully.
'
' Call with Send=True to send unsubscription
' notification, or False to send nothing.
'
Public Function Unsubscribe(Code As Long, Send As Boolean) As Boolean

    ' Default to failure.
    Unsubscribe = False

    ' Get index into code table.
    Dim Index As Long
    Index = Code - GK_DbNotify - 1

    ' Validate Index.
    If Index < 0 Or Index >= GATESERVER_MAX_DATA_NOTIFYS Then
        Exit Function
    End If

    ' See if we were subscribed.
    If Subscribed(Index) Then
    
        ' Unsubscribe this.
        Call Subscriptions(Index).Unsubscribe(Me, Code, Send)
        Set Subscriptions(Index) = Nothing
        Subscribed(Index) = False
        
        ' Success.
        Unsubscribe = True
    End If
End Function

'
' Unsubscribe this connection from all databases.
'
' Call with Send=True to send unsubscription
' notification, or False to send nothing.
'
Public Sub UnsubscribeAll(Send As Boolean)
    Dim i As Long

    ' Unsubscribe anything we're subscribed to.
    For i = 0 To GATESERVER_MAX_DATA_NOTIFYS - 1
        Call Unsubscribe(i + GK_DbNotify + 1, Send)
    Next
End Sub

'/////////////////////////////////////////////////////////
' Public accessors.
'/////////////////////////////////////////////////////////

'
' Set the user information for this connection.
'
Public Sub SetUser(ThisName As String, _
    ThisType As String, _
    ThisTeam As String, _
    ThisSetAccess As Long, _
    ThisClearAccess As Long, _
    UpdateHook As Boolean)
    
    ' Set the info.
    UserName = ThisName
    UserType = ThisType
    Team = ThisTeam
    Access = (Access Or ThisSetAccess) And Not ThisClearAccess
    
    ' Note db change.
    If UpdateHook Then UpdateUserHook
End Sub

'
' Set the level information for this connection.
' Call with ThisLevel="" for none.
'
Public Sub SetLevel(ThisLevel As String, _
    ThisType As String, Msg As String, _
    UpdateHook As Boolean)
    
    ' See if we're going into a level.
    If ThisLevel = "" Then
        
        ' User is in no level.
        Access = Access And Not GKACCESS_InLevel
        Call Notify(GK_Notify_ExitLevel, Quotes(Msg), "")
    Else
        
        ' User is in a level.
        Access = Access Or GKACCESS_InLevel
        Call Notify(GK_Notify_EnterLevel, ThisLevel & " " & ThisType, "")
    End If

    ' Set info.
    Level = ThisLevel
    UserType = ThisType

    ' Note db change.
    If UpdateHook Then UpdateUserHook
End Sub

'
' Return the host address associated with this connection.
'
Public Function GetHost() As String
    If IsVirtual Then
        GetHost = "virtual"
    Else
        GetHost = TCP.RemoteHostIP & ":" & TCP.RemotePort
    End If
End Function

'
' Veryify that one or more of the specified access flags
' are True.  If it is, returns True. If not, returns
' False and sends error to client. If you specify
' zero (GKACCESS_All) as input, returns True always.
'
Public Function CheckAccess(Flags As Long) As Boolean

    ' Verify access mask.
    If (Access And Flags) = 0 And Access <> 0 Then
    
        ' No access.
        Call Respond(GK_Failure_NoAccess, "", "No access.")
        CheckAccess = False
    Else
        
        ' Success.
        CheckAccess = True
    End If
End Function

'/////////////////////////////////////////////////////////
' Public startup/shutdown.
'/////////////////////////////////////////////////////////

'
' Initialize this new connection and set its logging device.
'
Public Sub Startup(ThisConnectionList As GateConnectionList, _
    ThisLog As GateLog, _
    ThisKeeper As Gatekeeper, _
    requestID As Long, _
    ThisIsVirtual As Boolean)

    ' Set our held objects.
    Set Log = ThisLog
    Set ConnectionList = ThisConnectionList
    Set Keeper = ThisKeeper

    ' Remember virtualness.
    IsVirtual = ThisIsVirtual

    ' Assign a guaranteed unique tag to this form.
    Tag = Str(ConnectionList.UpCounter)
    ConnectionList.UpCounter = ConnectionList.UpCounter + 1

    ' Init times.
    ConnectionStartTime = GateTimer
    LastReceiveTime = GateTimer

    ' Set connection flags.
    Access = GKACCESS_Connected + IIf(Keeper.GetServerInfoPassword = "", GKACCESS_Info, 0)
    
    ' Set names.
    UserName = ""
    UserType = "Unknown"
    Level = ""
    CmdLine = ""
    
    ' Handle whether this is a real TCP connection,
    ' or a virtual connection used internally.
    If IsVirtual Then
    
        ' This is a virtual connection.
        Access = Access Or GKACCESS_VirtualConnection
        Verbose = False
    Else
        ' This is a real connection.
        Verbose = True

        ' Log message for tracking down problems.
        Log.Log "Received request " & requestID
    
        ' Accept the connection.
        TCP.Accept requestID
        
        ' Send welcome message.
        SendWelcome
        
        ' Send time.
        SendTime
    
        ' Send initial prompt.
        SendPrompt
    End If
End Sub

'
' Shutdown this connection and free its logging device.
' Called by Gatekeeper server, never called directly
' by this object.
'
Public Sub Shutdown()
    Dim i As Long
    
    ' Quietly unsubscribe all of our subscriptions.
    UnsubscribeAll (False)
    
    ' Tell the level manager this guy has disconnected.
    Keeper.LevelMan.NoteDisconnect Me

    ' If logged in, this user is in the Users database
    ' and must be deleted.
    If Access And GKACCESS_LoggedIn Then
        
        ' Remove this user from the Users database.
        Keeper.Users.NoteDbDelete (UserName)
    End If
    
    ' Close our TCP socket.
    If Not IsVirtual Then TCP.Close
    
    ' Write a log entry.
    If Not (Access And GKACCESS_VirtualConnection) Then
        Log.Log "End connection"
    End If
    
    ' Release our held objects.
    Set Keeper = Nothing
    Set ConnectionList = Nothing
    Set Log = Nothing

    ' Done.
    Access = Access And Not GKACCESS_Connected
    Unload Me
End Sub

'/////////////////////////////////////////////////////////
' Data sending.
'/////////////////////////////////////////////////////////

'
' Send a string with a trailing cr/lf.
'
Public Sub Send(S As String)
    On Error GoTo SendError
    If Not IsVirtual Then TCP.SendData S & Chr(13) & Chr(10)
SendError:
End Sub

'
' Send a raw string.
'
Public Sub SendRaw(S As String)
    On Error GoTo SendError
    If Not IsVirtual Then TCP.SendData S
SendError:
End Sub

'
' Send a response.
'
' If Verbose=False, only machine codes and data are sent.
' If Verbose=True,  both are sent.
'
Public Sub Respond(MachineCode As Long, Data As String, HumanCode As String)
    On Error GoTo SendError
    
    ' Send machine readable code.
    SendRaw Trim(Str(MachineCode))
    
    ' Send data, if any.
    If Data <> "" Then SendRaw " " & Data
    
    ' Send optional human readable description.
    If Verbose And (HumanCode <> "") Then SendRaw " " + HumanCode
    
    ' Send cr/lf.
    SendRaw Chr(13) & Chr(10)
SendError:
End Sub

'
' Send an async notification.
'
Public Sub Notify(MachineCode As Long, Data As String, HumanCode As String)
    BeginAsyncNotification
    Call Respond(MachineCode, Data, HumanCode)
    EndAsyncNotification
End Sub

'
' Send the prompt string to the receiver.
'
Public Sub SendPrompt()

    ' Set IsAtPrompt so that async notifications
    ' interrupt us properly.
    IsAtPrompt = True
    
    ' We only send a prompt if we're in verbose mode.
    If Verbose Then
        On Error GoTo SendError
        SendRaw Chr(13) & Chr(10) & _
            "(" & UserName & "/" & Level & "> "
SendError:
    End If
End Sub

'
' Begin an async notification.
'
Public Sub BeginAsyncNotification()

    ' If we are in verbose mode, we must send a
    ' leading cr/lf so that the notification starts
    ' on a new line.
    If Verbose And IsAtPrompt Then
        Send "..."
    End If
End Sub

'
' End an async notification.
'
Public Sub EndAsyncNotification()

    ' If we are in verbose mode, resend the
    ' prompt and the command line typed thus far.
    If Verbose And IsAtPrompt Then
        SendPrompt
        On Error GoTo SendError
        SendRaw CmdLine
SendError:
    End If
End Sub

'
' Send the welcome message.
'
Public Sub SendWelcome()
    Call Respond(GK_Notify_Version, GATESERVER_ID & " " & GATESERVER_PROTOCOL_VERSION, "")
    Call Respond(GK_Notify_Maker, GATESERVER_MAKER, "")
    Call Respond(GK_Notify_PrintMe, GATESERVER_WELCOME_1, "")
End Sub

'
' Send the time.
'
Public Sub SendTime()
    Call Respond(GK_Notify_Time, Trim(Str(GateTimer())), "Gatekeeper time.")
End Sub

'/////////////////////////////////////////////////////////
' Command execution.
'/////////////////////////////////////////////////////////

'
' Execute a remote, typed command in this connection's context.
'
Public Sub Exec(CmdLine As String)
    Dim Parm1 As String, Parm2 As String, Parm3 As String
    Dim Topic As String, Key As String, Value As String
    Dim TempFilename As String, Temp As String
    
    ' Handle all connection commands, which are always
    ' valid even when not-logged-in.
    If GetCMD(CmdLine, "VERBOSE") Then
        ' Set verbose (human readable) mode.
        Verbose = True
        Call Respond(GK_Success, "", "Verbose mode is on.")
    ElseIf GetCMD(CmdLine, "CLIENT") Then
        ' Set machine client (non human readable) mode.
        Parm1 = NextSTRING(CmdLine)
        Parm2 = NextSTRING(CmdLine)
        Parm3 = NextSTRING(CmdLine)
        ' Validate parms.
        If IsValidNumber(Parm1) And _
            (Parm2 = "" Or IsValidString(Parm2)) And _
            (Parm3 = "" Or IsValidNumber(Parm3)) Then
            
            ' Validate the protocol version.
            If Val(Parm1) >= GATESERVER_PROTOCOL_MIN Then
            
                ' All parms are valid; set info.
                ClientProtocolVersion = Val(Parm1)
                ClientApplication = Parm2
                ClientVersion = Val(Parm3)
            
                ' Success.
                Call Respond(GK_Success, "", "Automated client mode; use 'Verbose' to return.")
                Verbose = False
                UpdateUserHook
            Else
            
                ' Client version is too old to support.
                Call Respond(GK_Failure_ProtocolNotSupported, "", "Protocol not supported.")
            End If
        Else
            Call Respond(GK_Failure_BadParameters, "", "Bad parameters.")
        End If
    ElseIf GetCMD(CmdLine, "EXIT") Then
        ' Exit.
        Call Respond(GK_Notify_EndSession, "", "Session ended.")
        Call Respond(GK_Success, "", "Exiting.")
        ConnectionList.RemoveConnection Me
    ElseIf GetCMD(CmdLine, "REM") Then
        ' Remark - do absolutely nothing.
        Call Respond(GK_Success, "", "Remark.")
    ElseIf GetCMD(CmdLine, "TIME") Then
        SendTime
    ElseIf GetCMD(CmdLine, "ECHO") Then
        ' Echo the remaining text to the client.
        Call Respond(GK_Success, CmdLine, "")
    ElseIf GetCMD(CmdLine, "VERSION") Then
        ' Resend welcome message.
        SendWelcome
    ElseIf GetCMD(CmdLine, "EXEC") Then
        ' Execute a set of commands.
        If CheckAccess(GKACCESS_Admin) Then
        
            ' Get filename.
            TempFilename = NextSTRING(CmdLine)
            If TempFilename = "" Then
                Call Respond(GK_Failure_BadParameters, "", "Missing filename.")
                Exit Sub
            End If
            
            ' Try to exec the file.
            Call ExecFile(TempFilename)
        End If
    Else
        '
        ' Pass up to the Gatekeeper for processing
        ' in this connection's context.
        '
        If Keeper.Exec(CmdLine, Me) = False Then
            Call Respond(GK_Failure_UnrecognizedCommand, "", "Unrecognized command.")
        End If
    End If
End Sub

'
' Execute all of the commands in the file
' using this connection.
'
Public Function ExecFile(Filename As String) As Boolean
    Dim Temp As String, Value As String
    Dim FileNum As Long

    ' Open the file.
    On Error GoTo BadRead
    FileNum = FreeFile
    Open Filename For Input As #FileNum
    
    ' Read all lines.
    Temp = Input(FileLen(Filename), FileNum)
    
    ' Execute each line.
    While Temp <> ""
        Value = NextLINE(Temp)
        If Value <> "" Then Exec (Value)
    Wend

    ' Exec succeeded.
    Close FileNum
    Call Respond(GK_Notify_ExecComplete, "", "Exec completed successfully.")
    ExecFile = True
    Exit Function

BadRead:
    ' Failure.
    Call Respond(GK_Failure_FileIO, "", "Error reading file.")
    Call Respond(GK_Notify_ExecComplete, "", "Exec completed unsuccessfully.")
End Function

'/////////////////////////////////////////////////////////
' Timer tick.
'/////////////////////////////////////////////////////////

'
' Tick function is periodically called, i.e. once every
' 30 seconds, to log off timed-out connections.
'
Public Sub Tick()
    ' Does nothing.
End Sub

'/////////////////////////////////////////////////////////
' TCP events generated by the TCP control.
'/////////////////////////////////////////////////////////

'
' TCP close event: Indicates that this connection has
' been closed by the other party or has timed out.
'
Private Sub TCP_Close()

    ' Tell Gatekeeper server to shut down this connection.
    ConnectionList.RemoveConnection Me
End Sub

'
' TCP data arrival event: Tells us that new data has come in.
' Use the BytesReceived property to figure out how many
' bytes to swallow rather than bytesTotal.
'
Private Sub TCP_DataArrival(ByVal bytesTotal As Long)
    Dim InData As String
    Dim i As Long, N As Long, C As Long, j As Long
    
    ' Get the data.
    Call TCP.GetData(InData, vbString)
    
    ' Process each character in the command line.
    N = Len(InData)
    For i = 1 To N
        
        ' Grab a new character.
        C = Asc(Mid(InData, i, 1))
        
        ' Handle the character.
        If C > 31 And C < 127 Then
            ' Regular alphanumeric.
            CmdLine = CmdLine + Chr(C)
            On Error GoTo SendError1
            If Verbose Then SendRaw Chr(C)
SendError1:
        ElseIf C = 13 Then
            If CmdLine <> "" Then
                ' Carriage return.
                On Error GoTo SendError2
                If Verbose Then SendRaw Chr(13) & Chr(10)
                On Error GoTo 0
    
                ' Set global execing flag for async notify handler.
                IsAtPrompt = False
    
                Exec CmdLine
SendError2:
                CmdLine = ""
                SendPrompt
            End If
        ElseIf C = 8 Or C = 127 Then
            ' Backspace.
            If Len(CmdLine) > 0 Then
                CmdLine = Left(CmdLine, Len(CmdLine) - 1)
                On Error GoTo SendError3
                If Verbose Then SendRaw Chr(8) & " " & Chr(8)
SendError3:
            End If
        ElseIf C = 27 Then
            ' Esc.
            On Error GoTo SendError4
            For j = 1 To Len(CmdLine)
                SendRaw Chr(8) & " " & Chr(8)
            Next
SendError4:
            CmdLine = ""
        End If
    Next

    ' If the above operations closed the form, get out.
    If (Access And GKACCESS_Connected) <> GKACCESS_Connected Then
        Unload Me
    End If
End Sub

'
' TCP error event.  Indicates that some kind of WinSock error
' has occured.
'
Private Sub TCP_Error(Number As Integer, Description As String, Scode As Long, Source As String, HelpFile As String, HelpContext As Long, CancelDisplay As Boolean)
    Log.Log "CRITICAL: TCP_Error: " & Description
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
