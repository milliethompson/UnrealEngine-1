VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "SerPropertySet"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' SerPSet.cls: Gatekeeper property-set serialization class.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Performs serialization to support remote Gatekeepers
'   setting properties.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

'
' Valid only during SetProperty:
'

' The key to find.
Private FindKey As String

' The value to set it to.
Private FindValue As String

' Access of the caller.
Private FindAccess As Long

' Set to True when a key matching FindKey is found.
Private Found As Boolean

'/////////////////////////////////////////////////////////
' Public member functions.
'/////////////////////////////////////////////////////////

'
' Set a string from the remote client.
'
Public Sub SerializeSTRING(ByRef Value As String, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)
    
    ' See if it matches.
    If (FindKey = "*" Or UCase(Key) = FindKey) And _
        ((WriteAccess And FindAccess) = WriteAccess) Then
        
        ' Found it.
        Found = True
        Value = FindValue
    End If
End Sub

'
' Set a long from the remote client.
'
Public Sub SerializeLONG(ByRef Value As Long, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)

    ' See if it matches.
    If (FindKey = "*" Or UCase(Key) = FindKey) And _
        ((WriteAccess And FindAccess) = WriteAccess) Then
        
        ' Found it.
        Found = True
        Value = Val(FindValue)
    End If
End Sub

'
' Set a boolean from the remote client.
'
Public Sub SerializeBOOLEAN(ByRef Value As Boolean, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)

    ' See if it matches.
    If (FindKey = "*" Or UCase(Key) = FindKey) And _
        ((WriteAccess And FindAccess) = WriteAccess) Then
        
        ' Found it.
        Found = True
        Value = Val(FindValue)
    End If
End Sub

'/////////////////////////////////////////////////////////
' Public custom functions.
'/////////////////////////////////////////////////////////

'
' Set one or more key/value pairs in a serialization
' stream.  Returns whether the value was successfully
' set.
'
Public Function SetProperty( _
    Keeper As Gatekeeper, _
    Topic As String, _
    Key As String, _
    Value As String, _
    Access As Long) _
    As Boolean
    
    ' Init.
    Found = False
    FindKey = UCase(Key)
    FindValue = Value
    FindAccess = Access
    
    ' Try to find by serializing the object.
    Call Keeper.Serialize(Me, Topic)
        
    ' Return whether we found it.
    SetProperty = Found
End Function

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
