VERSION 5.00
Begin VB.Form ThingEntry 
   Caption         =   "Thing Entry"
   ClientHeight    =   1560
   ClientLeft      =   4215
   ClientTop       =   7365
   ClientWidth     =   4350
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1560
   ScaleWidth      =   4350
   Begin VB.CommandButton Ok 
      Caption         =   "&Ok"
      Default         =   -1  'True
      Height          =   315
      Left            =   60
      TabIndex        =   2
      Top             =   1200
      Width           =   1095
   End
   Begin VB.CommandButton Cancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   315
      Left            =   3180
      TabIndex        =   1
      Top             =   1200
      Width           =   1095
   End
   Begin VB.TextBox Entry 
      Height          =   285
      Left            =   60
      TabIndex        =   0
      Text            =   "Text1"
      Top             =   780
      Width           =   4215
   End
   Begin VB.Label Prompt 
      Caption         =   "Please enter the thing:"
      Height          =   495
      Left            =   120
      TabIndex        =   3
      Top             =   180
      Width           =   4095
   End
End
Attribute VB_Name = "ThingEntry"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' ThingEnt.frm: Gatekeeper client text entry form.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Presents the user interface for typing in something.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

Private Success As Boolean
Private Thing As String

' Types of things to get.
Private Const GET_Name = 0
Private Const GET_OptionalName = 1
Private Const GET_Password = 2
Private Const GET_ListItem = 3
Private Const GET_String = 4
Private GetType As Long

'/////////////////////////////////////////////////////////
' Public functions.
'/////////////////////////////////////////////////////////

'
' Get a name. Returns True if success, False is failure.
'
Public Function GetName(ThisCaption As String, _
    ThisPrompt As String, ThisThing As String, _
    Value As String) As Boolean
    
    ' Show modally.
    Thing = ThisThing
    Caption = ThisCaption
    Prompt.Caption = ThisPrompt
    Entry.Text = Value
    Entry.SelStart = 0
    Entry.SelLength = Len(Value)
    Entry.PasswordChar = ""
    GetType = GET_Name
    Show 1
    
    ' Handle results.
    If Success Then Value = Entry.Text
    GetName = Success
End Function

'
' Get a password. Returns True if success, False is failure.
'
Public Function GetPassword(ThisCaption As String, _
    ThisPrompt As String, ThisThing As String, _
    Value As String) As Boolean
    
    ' Show modally.
    Thing = ThisThing
    Caption = ThisCaption
    Prompt.Caption = ThisPrompt
    Entry.Text = Value
    Entry.SelStart = 0
    Entry.SelLength = Len(Value)
    Entry.PasswordChar = "*"
    GetType = GET_Password
    Show 1
    
    ' Handle results.
    If Success Then Value = Entry.Text
    GetPassword = Success
End Function

'
' Get an optional name. Returns True if success, False
' is failure.
'
Public Function GetOptionalName(ThisCaption As String, _
    ThisPrompt As String, ThisThing As String, _
    Value As String) As Boolean
    
    ' Show modally.
    Thing = ThisThing
    Caption = ThisCaption
    Prompt.Caption = ThisPrompt
    Entry.Text = Value
    Entry.SelStart = 0
    Entry.SelLength = Len(Value)
    Entry.PasswordChar = ""
    GetType = GET_OptionalName
    Show 1
    
    ' Handle results.
    If Success Then Value = Entry.Text
    GetOptionalName = Success
End Function

'
' Get a list item. Returns True if success, False is failure.
'
Public Function GetListItem(ThisCaption As String, _
    ThisPrompt As String, ThisThing As String, _
    Value As String) As Boolean
    
    ' Show modally.
    Thing = ThisThing
    Caption = ThisCaption
    Prompt.Caption = ThisPrompt
    Entry.Text = Value
    Entry.SelStart = 0
    Entry.SelLength = Len(Value)
    Entry.PasswordChar = ""
    GetType = GET_ListItem
    Show 1
    
    ' Handle results.
    If Success Then Value = Entry.Text
    GetListItem = Success
End Function

'
' Get a string. Returns True if success, False is failure.
'
Public Function GetString(ThisCaption As String, _
    ThisPrompt As String, _
    Value As String) As Boolean
    
    ' Show modally.
    Thing = "string"
    Caption = ThisCaption
    Prompt.Caption = ThisPrompt
    Entry.Text = Value
    Entry.SelStart = 0
    Entry.SelLength = Len(Value)
    Entry.PasswordChar = ""
    GetType = GET_String
    Show 1
    
    ' Handle results.
    If Success Then Value = Entry.Text
    GetString = Success
End Function

'/////////////////////////////////////////////////////////
' Standard form events.
'/////////////////////////////////////////////////////////

'
' Form has been activated.
'
Private Sub Form_Activate()
    Entry.SetFocus
End Sub

'/////////////////////////////////////////////////////////
' Buttons.
'/////////////////////////////////////////////////////////

'
' Ok button.
'
Private Sub Ok_Click()
    
    ' Default to success.
    Success = True
    
    ' Validate the text being entered.
    Select Case GetType
        Case GET_Name
            If ValidateName(Entry.Text, Thing) Then Hide
        Case GET_OptionalName
            If ValidateOptionalName(Entry.Text, Thing) Then Hide
        Case GET_Password
            If ValidateOptionalName(Entry.Text, Thing) Then Hide
        Case GET_ListItem
            If ValidateListItem(Entry.Text, Thing) Then Hide
        Case GET_String
            Hide
    End Select
End Sub

'
' Cancel button.
'
Private Sub Cancel_Click()
    Success = False
    Hide
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
