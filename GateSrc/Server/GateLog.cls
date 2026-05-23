VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "GateLog"
Attribute VB_Creatable = False
Attribute VB_Exposed = True
'/////////////////////////////////////////////////////////
' Log.cls: Gatekeeper server logging class.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Manage the log file with options for opening and
'   closing the log file, and writing log entries.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Private variables
'/////////////////////////////////////////////////////////

' File number of the log file, or 0 if not open.
Private LogFileNum As Long

' Log hooks.
Private Const MAX_LOG_HOOKS = 16
Private NumLogHooks As Long
Private LogHooks(MAX_LOG_HOOKS) As Object

'/////////////////////////////////////////////////////////
' Public logging functions.
'/////////////////////////////////////////////////////////

'
' Write a string to the log.
'
Public Sub Log(S As String)
    Dim i As Long

    ' Write to the log file.
    If LogFileNum <> 0 Then Print #LogFileNum, S
    
    ' Send to all active log hooks.
    For i = 0 To NumLogHooks - 1
        LogHooks(i).Log (S)
    Next
End Sub

'
' Open a log file.
'
Public Sub OpenFile(Filename As String)
    
    ' Close log, if open.
    CloseFile
    
    ' Attempt to open the new log file.
    LogFileNum = FreeFile
    On Error GoTo OpenFailed
    Open Filename For Output As LogFileNum

    ' Print initial log entries.
    Log GATE_LOG_1
    Log GATE_LOG_2
    Log GATE_LOG_3
    Log ""
    Log "Log file open, " & Date & ", " & Time
    Log "Starting " & GATESERVER_ID & " " & GATESERVER_PROTOCOL_VERSION
    Log GATESERVER_MAKER
    ' Success.
    Exit Sub
OpenFailed:
    LogFileNum = 0
End Sub

'
' Close the log file.
'
Public Sub CloseFile()
    If LogFileNum <> 0 Then
        Log "Log file closed: " & Date & ", " & Time
        Close LogFileNum
    End If
    LogFileNum = 0
End Sub

'/////////////////////////////////////////////////////////
' Log hooks.
'/////////////////////////////////////////////////////////

'
' Register a log hooking function. The hook will be
' called for each call to Log.  The object you
' pass must have a function compatible with:
'
'   Public Sub Log(S as String)
'
Public Sub RegisterLogHook(Hook As Object)

    ' Add to hook list if there's space.
    If (NumLogHooks < MAX_LOG_HOOKS) Then
        Set LogHooks(NumLogHooks) = Hook
        NumLogHooks = NumLogHooks + 1
    End If
End Sub

'
' Unregister a log hook.
'
Public Sub UnregisterLogHook(Hook As Object)
    Dim i As Long, j As Long
    
    ' Remove from list of log hooks.
    j = 0
    For i = 0 To NumLogHooks - 1
        If LogHooks(i) <> Hook Then
            If i <> j Then
                Set LogHooks(j) = LogHooks(i)
                Set LogHooks(i) = Nothing
            End If
            j = j + 1
        Else
            Set LogHooks(i) = Nothing
        End If
    Next
    
    ' Shorten list if we found the hook.
    NumLogHooks = j
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
