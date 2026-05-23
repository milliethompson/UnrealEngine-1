VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Client"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' GateCli.cls: GateClient class implementation.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Contain all of the public variables and public
'   functions related to the gate client.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Public variables.
'/////////////////////////////////////////////////////////

' List of remote servers we may administer.
Public NumRemoteServers As Integer

' Connect defaults.
Public DefaultRemoteSite As String
Public DefaultConnectLocal As Boolean

' Current connection info.
Public IsLocal As Boolean
Public IsDebugging As Boolean
Public IsConnected As Boolean
Public ConnectSite As String
Public ConnectPort As Long
Public ConnectPassword As String
Public ConnectResultText As String
Public AutoLaunchLocalLevels As Boolean

' Stuff that we received as a result of GK_Success messages.
Public ResultUserName As String
Public ResultFileList As String

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

' List of remote servers for convenience.
Private RemoteServers(MAX_REMOTE_SERVERS) As String

' Count of communication tasks that affect the progress bar.
Private TaskCount

' Local client time minus Gatekeeper server.
Private TimeDelta As Long

'/////////////////////////////////////////////////////////
' Time accessors.
'/////////////////////////////////////////////////////////

'
' Remember the server time at this instant.
' Called when GK_Notify_Time messages come in so
' that our time can be synched with the remote's.
'
Public Sub NoteServerTimeNow(ServerTime As Long)
    TimeDelta = GateTimer - ServerTime
End Sub

'
' Convert server time to local time.
'
Public Function ToLocalTime(ServerTime As Long) As Long
    ToLocalTime = ServerTime + TimeDelta
End Function

'/////////////////////////////////////////////////////////
' Remote Server List Accessors.
'/////////////////////////////////////////////////////////

'
' Get a remote server from the array.
'
Public Function GetRemoteServer(i As Integer) As String
    Dim Temp As String
    
    ' Get encoded string.
    Temp = RemoteServers(i)
    
    ' Get comma-delimited record.
    GetRemoteServer = NextSTRING(Temp, ";")
End Function

'
' Get a remote server's info from the array.
'
Public Sub GetRemoteServerInfo(ByVal Site As String, ByRef Port As Long, ByRef Password As String)
    Dim Temp As String
    Dim i As Integer
    
    ' Add semicolon so we're sure to only find proper sites.
    Site = UCase(Site + ";")
    
    ' Rejig the list.
    For i = 0 To NumRemoteServers - 1
        Temp = RemoteServers(i)
        If InStr(UCase(Temp), Site) <> 0 Then
            
            ' Get comma-delimited record.
            Site = NextSTRING(Temp, ";")
            Port = Val(NextSTRING(Temp, ";"))
            Password = Temp
            Exit Sub
        End If
    Next i
End Sub

'
' Delete a remote server from the array if it exists.
'
Public Sub DeleteRemoteServer(ByVal Site As String)
    Dim i As Integer, j As Integer
    
    ' Add semicolon so we're sure to only find proper sites.
    Site = UCase(Site + ";")
    
    ' Rejig the list.
    j = 0
    For i = 0 To NumRemoteServers - 1
        If InStr(UCase(RemoteServers(i)), Site) = 0 Then
            RemoteServers(j) = RemoteServers(i)
            j = j + 1
        End If
    Next i
    NumRemoteServers = j
End Sub

'
' Add a remote server to the array.
'
Public Sub AddRemoteServer(Site As String, Port As Long, Password As String)
    
    ' Add if there's room.
    If NumRemoteServers < MAX_REMOTE_SERVERS Then
        RemoteServers(NumRemoteServers) = _
            Site + ";" + Trim(Str(Port)) + ";" + Password
        NumRemoteServers = NumRemoteServers + 1
    End If
End Sub

'
' Return whether we can add a new server to the array.
'
Public Function CanAddRemoteServer() As Boolean
    ' Return whether there's free space.
    CanAddRemoteServer = NumRemoteServers < MAX_REMOTE_SERVERS
End Function

'/////////////////////////////////////////////////////////
' Public defaults loading/saving functions.
'/////////////////////////////////////////////////////////

'
' Load gate client defaults from the registry.
'
Public Sub LoadDefaults()
    Dim i As Integer

    ' Open registry.
    OpenReg CLIENT_REGISTRY_KEY
    
    ' Load launch defaults.
    SetUserHeading "Launch Defaults"
    DefaultRemoteSite = GetUserInfo("DefaultRemoteSite", "")
    DefaultConnectLocal = GetUserInfo("DefaultConnectLocal", "1")
    NumRemoteServers = Val(GetUserInfo("NumRemoteServers", "0"))
    AutoLaunchLocalLevels = Val(GetUserInfo("AutoLaunchLocalLevels", "0"))
    ' Load remote server list.
    For i = 0 To NumRemoteServers - 1
        RemoteServers(i) = GetUserInfo("RemoteServer" & Trim(Str(i)), "")
    Next

    ' Close the registry.
    CloseReg
End Sub

'
' Save gate client defaults to the registry.
'
Public Sub SaveDefaults()
    Dim i As Integer
    
    ' Open registry.
    OpenReg CLIENT_REGISTRY_KEY
    
    ' Save launch defaults.
    SetUserHeading "Launch Defaults"
    Call SetUserInfo("DefaultRemoteSite", DefaultRemoteSite)
    Call SetUserInfo("DefaultConnectLocal", Str(DefaultConnectLocal))
    Call SetUserInfo("NumRemoteServers", Trim(Str(NumRemoteServers)))
    Call SetUserInfo("AutoLaunchLocalLevels", IIf(AutoLaunchLocalLevels, "1", "0"))
    
    ' Save remote server list.
    For i = 0 To NumRemoteServers - 1
        Call SetUserInfo("RemoteServer" & Trim(Str(i)), RemoteServers(i))
    Next
    
    ' Close the registry.
    CloseReg
End Sub

'/////////////////////////////////////////////////////////
' Public connection functions.
'/////////////////////////////////////////////////////////

'
' Attempt an admin connection to a specified site, port,
' and password. Returns success.
'
Public Function OpenServer(Site As String, _
    Port As Long, _
    Password As String) As Boolean

    ' Set connection params.
    ConnectSite = Site
    ConnectPort = Port
    ConnectPassword = Password
    
    ' Show modal connection dialog and attempt connection.
    Connect.DoModal
    
    ' Return success.
    OpenServer = IsConnected
End Function

'/////////////////////////////////////////////////////////
' Communication.
'/////////////////////////////////////////////////////////

'
' Execute a command on the GateServer and wait for
' a response.
'
Public Function SlowExec(Cmd As String) As Long
    
    ' Execute the command by displaying the modal
    ' connection form, which performs the work then
    ' hides itself.
    GCmdLine = Cmd
    Connect.DoModal
    
    ' Return the result code, which may be success or failure.
    SlowExec = GResultCode
End Function

'
' Execute a command on the GateServer, wait for
' a response, and don't accept failure.
'
Public Sub SafeExec(Cmd As String, _
    Optional ResponseData As Variant, _
    Optional ResponseCode As Variant)
Retry:
    
    ' Execute the command by displaying the modal
    ' connection form, which performs the work then
    ' hides itself.
    GCmdLine = Cmd
    Connect.DoModal
    
    ' Check for success.
    If GResultCode < GK_Success Or GResultCode > GK_Success_Max Then
        
        ' We failed for some reason.
        If IsMissing(ResponseCode) Then
            
            ' Fail.
            Dim Result As Integer
            Result = MsgBox("Operation failed (" & GResultCode & "): " & Cmd, vbAbortRetryIgnore)
            If Result = vbRetry Then
                GoTo Retry
            ElseIf Result = vbAbort Then
                End
            End If
        Else
        
            ' Return the response code to the caller.
            ResponseCode = GResultCode
        End If
    End If
    
    ' Return any result data.
    If Not IsMissing(ResponseData) Then
        ResponseData = GResultData
    End If
End Sub

'
' Executes a command without waiting for a response.
' This is intended for calls which expect notifications
' to come in asynchronously.  When you call this, there's
' not a discernable relationship between commands and
' responses.
'
Public Sub AsyncExec(Cmd As String)

    ' Trap TCP errors.
    On Error GoTo SocketError
    
    ' Send the data.
    Connect.TCP.SendData Cmd & Chr(13)
    
    ' Bump pending transaction count.
    Connect.TransactionsPending = Connect.TransactionsPending + 1

    ' Success exit.
    Exit Sub
    
    ' Errors here are fatal:
SocketError:
    MsgBox "Lost connection to the server."
    End
End Sub

'/////////////////////////////////////////////////////////
' Logging.
'/////////////////////////////////////////////////////////

'
' Output some text on the client log.
'
Public Sub Log(S As String)
    If IsConnected Then
        
        ' The log listbox is only open if we're connected.
        Main.Log (S)
    End If
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
