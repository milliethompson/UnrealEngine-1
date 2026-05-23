VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "SerRegistryLoad"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' SerRLoad.cls: Gatekeeper registry loading class.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Performs serialization to support loading properties
'   from the registry.
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
' Load a string from the registry.
'
Public Sub SerializeSTRING(ByRef Value As String, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)
    
    ' Load it from the registry.
    Value = GetUserInfo(Key, DefaultValue)
End Sub

'
' Load a long from the registry.
'
Public Sub SerializeLONG(ByRef Value As Long, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)

    ' Load it from the registry.
    Value = GetUserInfo(Key, Trim(Str(DefaultValue)))
End Sub

'
' Load a boolean from the registry.
'
Public Sub SerializeBOOLEAN(ByRef Value As Boolean, _
    Key As String, DefaultValue As String, Topic As String, _
    ReadAccess As Long, WriteAccess As Long)

    ' Load it from the registry.
    Value = GetUserInfo(Key, Trim(Str(DefaultValue)))
End Sub

'/////////////////////////////////////////////////////////
' Public custom functions.
'/////////////////////////////////////////////////////////

'
' Load everything in a topic.
'
Public Sub RegistryLoadTopic(Topic As String, Ob As Object)

    ' Create the registry heading.
    SetUserHeading Topic
    
    ' Have the object serialize everything.
    Call Ob.Serialize(Me, Topic)
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
