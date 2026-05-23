VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "GateConnectionList"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' GateCLst.cls: Gatekeeper server connection list class.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Maintains a list of GetConnection objects.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

' The array of connections.
Public NumConnections As Long
Public UpCounter As Long
Private Connections(GATESERVER_MAX_CONNECTIONS) As Object

' Held objects.
Private Log As GateLog
Private Keeper As Gatekeeper

'/////////////////////////////////////////////////////////
' Startup/shutdown functions.
'/////////////////////////////////////////////////////////

Public Sub Startup(ThisLog As GateLog, ThisKeeper As Gatekeeper)

    ' Set held objects.
    Set Log = ThisLog
    Set Keeper = ThisKeeper
    
    ' Init connections.
    NumConnections = 0
End Sub

Public Sub Shutdown()

    ' Close all connections.
    CloseAllConnections

    ' Release held objects.
    Set Keeper = Nothing
    Set Log = Nothing
End Sub

'/////////////////////////////////////////////////////////
' Public functions.
'/////////////////////////////////////////////////////////

'
' Tick all connections.  Meant to be called once
' in a while, i.e. once every 10-30 seconds.
'
Public Sub Tick()
    Dim i As Long
    
    ' Tick them all. Note that if a connection
    ' deletes itself during a tick, we'll skip ticking
    ' the following connection, but that is ok since
    ' there is no critical processing in Tick.
    For i = 0 To NumConnections - 1
        Connections(i).Tick
    Next i
End Sub

'
' Accept a new connection that came in.
'
Public Sub AcceptConnection(requestID As Long)
    If NumConnections < GATESERVER_MAX_CONNECTIONS Then
    
        ' Spawn a new connection form to handle the connection.
        Set Connections(NumConnections) = New GateConnection
        
        ' Initialize the new connection form.
        Call Connections(NumConnections).Startup(Me, Log, Keeper, requestID, False)
        
        ' Note that we've just added a new connection to the list.
        NumConnections = NumConnections + 1
    Else
        ' Ignore the connection request, since we would overflow
        ' our maximum. This will cause the request to be immediately
        ' rejected upon return.
    End If
End Sub

'
' Create a new virtual connection.
' Upon success, sets Connection to the new connection and
' returns True.  If failure, returns False.
'
' Guaranteed not to fail if CanCreateConnection is True.
'
Public Function CreateVirtualConnection( _
    ByRef Connection As Object) As Boolean
    
    ' See if there's room.
    If NumConnections < GATESERVER_MAX_CONNECTIONS Then
    
        ' Spawn a new connection form to handle the connection.
        Set Connections(NumConnections) = New GateConnection
        
        ' Initialize the new virtual connection form.
        Call Connections(NumConnections).Startup(Me, Log, Keeper, 0, True)
        
        ' Return the connection.
        Set Connection = Connections(NumConnections)
        
        ' Note that we've just added a new connection to the list.
        NumConnections = NumConnections + 1
        
        ' Success.
        CreateVirtualConnection = True
    End If
End Function

'
' Close all connections.
'
Public Sub CloseAllConnections()
    Dim i As Long
    
    ' Remove all connection forms.
    While NumConnections > 0
        RemoveConnection Connections(0)
    Wend
End Sub

'
' Remove one connection from the list.  Called by a connection form.
'
Public Sub RemoveConnection(ConnectionForm As Object)
    Dim i As Long, j As Long

    ' Remove the connection form from the list.
    For i = 0 To NumConnections - 1
        If Connections(i).hwnd <> ConnectionForm.hwnd Then
            If i <> j Then
                Set Connections(j) = Connections(i)
                Set Connections(i) = Nothing
            End If
            j = j + 1
        Else
            Connections(i).Shutdown
            Unload Connections(i)
            Set Connections(i) = Nothing
        End If
    Next
    
    ' Shorten list if we deleted a form.
    NumConnections = j
End Sub

'
' Return the nth connection.
'
Public Function GetConnection(N As Long) As Object
    Set GetConnection = Connections(N)
End Function

'
' Return the connection exactly matching the named player.
' Returns Nothing if not logged in.
'
Public Function GetPlayerByName(Name As String, Result As Object) As Boolean
    Dim i As Long
    
    ' Default to nothing.
    GetPlayerByName = False
    
    ' Find exact match.
    For i = 0 To NumConnections - 1
        If UCase(Connections(i).UserName) = UCase(Name) Then
            Set Result = Connections(i)
            GetPlayerByName = True
        End If
    Next
End Function

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
