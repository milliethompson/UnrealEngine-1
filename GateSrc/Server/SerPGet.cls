VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "SerPropertyGet"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' SerPGet.cls: Gatekeeper property-get serialization class.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Performs serialization to enable remote clients
'   to obtain properties from this Gatekeeper server.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

'
' Valid only during GetProperty:
'

' Connection that's holding us.
Private FindConnection As Object

' The key to find.
Private FindKey As String

' Set to True when a key matching FindKey is found.
Private Found As Boolean

' The value associated with the key, valid only
' after Found is set.
Private FindResult As String

' Access required.
Private FindAccess As Long

' Topic we're looking for.
Private FindTopic As String

'/////////////////////////////////////////////////////////
' Public member functions.
'/////////////////////////////////////////////////////////

'
' Get a string for the remote client.
'
Public Sub SerializeSTRING(ByRef Value As String, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)
    
    ' See if it matches.
    If (FindKey = "*" Or UCase(Key) = FindKey) And _
        ((ReadAccess And FindAccess) = ReadAccess) Then
        
        ' Found it.
        Found = True
        Call FindConnection.Respond(GK_Notify_Property, Topic & " " & Key & " " & Quotes(Value), "")
    End If
End Sub

'
' Get a long for the remote client.
'
Public Sub SerializeLONG(ByRef Value As Long, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)
    
    If (FindKey = "*" Or UCase(Key) = FindKey) And _
        ((ReadAccess And FindAccess) = ReadAccess) Then
        
        ' Found it.
        Found = True
        Call FindConnection.Respond(GK_Notify_Property, Topic & " " & Key & " " & Trim(Str(Value)), "")
    End If
End Sub

'
' Get a boolean for the remote client.
'
Public Sub SerializeBOOLEAN(ByRef Value As Boolean, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)

    ' See if it matches.
    If (FindKey = "*" Or UCase(Key) = FindKey) And _
        ((ReadAccess And FindAccess) = ReadAccess) Then
        
        ' Found it.
        Found = True
        Call FindConnection.Respond(GK_Notify_Property, Topic & " " & Key & " " & IIf(Value, "1", "0"), "")
    End If
End Sub

'/////////////////////////////////////////////////////////
' Public custom functions.
'/////////////////////////////////////////////////////////

'
' Find a key/value in a serialization stream.
' Returns whether the key was found.
'
' Sets Result to the value associated with the
' key (as a string), or "" if nothing found.
'
Public Function GetProperty( _
    Keeper As Gatekeeper, _
    Topic As String, _
    Key As String, _
    Connection As Object, _
    ByRef Result As String) _
    As Boolean
    
    ' Init finding variables.
    FindKey = UCase(Key)
    Set FindConnection = Connection
    FindAccess = Connection.Access
    FindResult = ""
    FindTopic = Topic
    Found = False
    
    ' Handle wildcard key
    If Key = "*" Then
        Found = True ' Zero responses is success for wildcards
    End If
    
    ' Try to find by serializing the object.
    Call Keeper.Serialize(Me, Topic)
        
    ' Handle wildcard data-end.
    If Key = "*" Then
        Call Connection.Respond(GK_Success, "End of property list.", "")
    End If
    
    ' Clear out held objects.
    Set FindConnection = Nothing

    ' Return results.
    Result = FindResult
    GetProperty = Found
    
End Function

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
