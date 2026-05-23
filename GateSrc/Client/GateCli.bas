Attribute VB_Name = "GateCli"
'/////////////////////////////////////////////////////////
' GateCli.bas: GateClient globals.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Globals needed to support the gate client.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Global variables.
'/////////////////////////////////////////////////////////

' Global objects available everywhere.
Public LocalGateServer As GateServer.Server
Public LocalGateClient As GateClient.Client

' Global command to send to server for Connect form.
Public GCmdLine As String
Public GResultCode As Long
Public GResultData As String

' True while a notify is in progress
Public IsNotify As Boolean

' True when notify is refreshing a grid.
Public RefreshingGrid As Long

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
