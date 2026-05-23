VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "SerRegistrySave"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' SerRSave.cls: Gatekeeper registry saving class.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Performs serialization to support saving properties
'   to the registry.
'
' Notes:
'   When dealing with the registry, we don't care
'   about property access levels.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Public member functions.
'/////////////////////////////////////////////////////////

'
' Save a string to the registry.
'
Public Sub SerializeSTRING(ByRef Value As String, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)
    
    ' Save it to the registry.
    Call SetUserInfo(Key, Value)
End Sub

'
' Save a long to the registry.
'
Public Sub SerializeLONG(ByRef Value As Long, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)
    
    ' Save it to the registry.
    Call SetUserInfo(Key, Trim(Str(Value)))
End Sub

'
' Save a boolean to the registry.
'
Public Sub SerializeBOOLEAN(ByRef Value As Boolean, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)
    
    ' Save it to the registry.
    Call SetUserInfo(Key, Trim(Str(Value)))
End Sub

'/////////////////////////////////////////////////////////
' Public custom functions.
'/////////////////////////////////////////////////////////

'
' Save everything in a topic.
'
Public Sub RegistrySaveTopic(Topic As String, Ob As Object)

    ' Create the registry heading.
    SetUserHeading Topic
    
    ' Have the object serialize everything.
    Call Ob.Serialize(Me, Topic)
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
