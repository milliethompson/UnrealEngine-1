VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "Server"
Attribute VB_Creatable = True
Attribute VB_Exposed = True
'/////////////////////////////////////////////////////////
' GateSrv.cls: Gatekeeper Server class implementation.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   This is the Gatekeeper server object.
'   It is a public, multi-use Ole-creatable class.
'
' Notes:
'   This Ole interface is designed only so that the
'   Gatekeeper server can be launched and shut down, and
'   so that it exposes sufficient information for a local
'   Gatekeeper client to log into it via TCP.
'
'   All functionality is controlled via TCP so that the
'   Gatekeeper server may be administered equally well
'   locally and remotely.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////
Option Explicit

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

'
' We have a Gatekeeper here as an encapsulated object
' so that its internal circular references don't
' inhibit our Class_Terminate member from being called.
'
Private Keeper As Gatekeeper

'/////////////////////////////////////////////////////////
' Class initialization and termination.
'/////////////////////////////////////////////////////////

'
' Initialize the Gatekeeper server.
'
Private Sub Class_Initialize()
    ' We do nothing here, opting to wait for
    ' a call to Init so that we may start up with
    ' a log file in the proper directory.
End Sub

'
' Shutdown the Gatekeeper server.
'
Private Sub Class_Terminate()
    Keeper.Shutdown
    Set Keeper = Nothing
End Sub

'/////////////////////////////////////////////////////////
' Public functions.
'/////////////////////////////////////////////////////////

'
' Init. Must be called before any other functions.
'
Public Sub Init(Directory As String, _
    AutolaunchLevels As Boolean)

    ' Change to the specified place.
    If Directory <> "" Then
        If Mid(Directory, 2, 1) = ":" Then
            ' Change to the proper drive.
            ChDrive Left(Directory, 1)
        End If
        ChDir Directory
    End If
    
    ' Begin.
    Set Keeper = New Gatekeeper
    
    ' Startup.
    Keeper.Startup AutolaunchLevels
End Sub

'
' Return number of logged in users.
'
Public Function NumLoggedInUsers() As Long
    NumLoggedInUsers = Keeper.NumLoggedInUsers
End Function

'
' Return our port for the local client.
'
Public Function GetServerPort() As Long
    GetServerPort = Keeper.GetPort
End Function

'
' Return our master password for the local client.
'
Public Function GetServerAdminPassword() As String
    GetServerAdminPassword = Keeper.GetServerAdminPassword
End Function

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
