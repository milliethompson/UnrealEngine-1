Attribute VB_Name = "Start"
'/////////////////////////////////////////////////////////
' Start.bas: Gatekeeper server startup.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Does nothing in release version; some debugging code
'   may be uncommented for development versions.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

Sub Main()
    If InStr(UCase(Command), "-TEST") <> 0 Then
        ChDrive "f"
        ChDir "\Unreal\Gate"
        Dim Keeper As New Gatekeeper
        Keeper.Startup (False)
        GateConnection.Show 1 ' Show any modal form that has no effect
        Keeper.Shutdown
    End If
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
