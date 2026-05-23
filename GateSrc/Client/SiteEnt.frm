VERSION 4.00
Begin VB.Form SiteEntry 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Remote Gatekeeper Information"
   ClientHeight    =   1860
   ClientLeft      =   1470
   ClientTop       =   6105
   ClientWidth     =   4245
   ControlBox      =   0   'False
   Height          =   2220
   Icon            =   "SiteEnt.frx":0000
   Left            =   1410
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1860
   ScaleWidth      =   4245
   ShowInTaskbar   =   0   'False
   Top             =   5805
   Width           =   4365
   Begin VB.TextBox RemotePort 
      Height          =   315
      Left            =   1200
      TabIndex        =   1
      Text            =   "Text1"
      Top             =   600
      Width           =   2895
   End
   Begin VB.CommandButton Cancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   315
      Left            =   2940
      TabIndex        =   4
      Top             =   1500
      Width           =   1155
   End
   Begin VB.CommandButton Ok 
      Caption         =   "&Ok"
      Default         =   -1  'True
      Height          =   315
      Left            =   60
      TabIndex        =   3
      Top             =   1500
      Width           =   1275
   End
   Begin VB.TextBox RemotePassword 
      Height          =   315
      Left            =   1200
      PasswordChar    =   "*"
      TabIndex        =   2
      Text            =   "Text1"
      Top             =   1020
      Width           =   2895
   End
   Begin VB.TextBox RemoteSite 
      Height          =   315
      Left            =   1200
      TabIndex        =   0
      Text            =   "Text1"
      Top             =   180
      Width           =   2895
   End
   Begin VB.Label Label3 
      Alignment       =   1  'Right Justify
      Caption         =   "Port:"
      Height          =   255
      Left            =   0
      TabIndex        =   7
      Top             =   660
      Width           =   1035
   End
   Begin VB.Label Label2 
      Alignment       =   1  'Right Justify
      Caption         =   "Password:"
      Height          =   255
      Left            =   0
      TabIndex        =   6
      Top             =   1080
      Width           =   1035
   End
   Begin VB.Label Label1 
      Alignment       =   1  'Right Justify
      Caption         =   "Site:"
      Height          =   255
      Left            =   0
      TabIndex        =   5
      Top             =   240
      Width           =   1095
   End
End
Attribute VB_Name = "SiteEntry"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' OpenRmt.frm: Information about a remote site.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Presents a user interface for editing a remote
'   site's info.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

Dim Success As Boolean

'/////////////////////////////////////////////////////////
' Public Functions.
'/////////////////////////////////////////////////////////

'
' Pop up the entry form to get a remote site name.
' Returns True if success, False if canceled.
'
Public Function GetRemoteSite( _
    ByRef SiteName As String, _
    ByRef SitePort As Long, _
    ByRef SitePassword As String) As Boolean
    
    ' Set text.
    RemoteSite.Text = SiteName
    RemotePort.Text = Trim(Str(SitePort))
    RemotePassword.Text = SitePassword
    
    ' Show modally.
    Show 1

    ' Handle result.
    If Success Then
        SiteName = RemoteSite.Text
        SitePort = Val(RemotePort.Text)
        SitePassword = RemotePassword.Text
    End If
    
    GetRemoteSite = Success
End Function
    

'/////////////////////////////////////////////////////////
' Buttons.
'/////////////////////////////////////////////////////////

'
' Cancel; don't change current connection status.
'
Private Sub Cancel_Click()
    
    ' Failed.
    Success = False
    Hide
End Sub

'
' Ok; validate and return results in the global strings.
'
Private Sub Ok_Click()

    ' Validate the text.
    RemoteSite.Text = Trim(RemoteSite.Text)
    RemotePort.Text = Trim(RemotePort.Text)
    RemotePassword.Text = Trim(RemotePassword.Text)
    
    If Not ValidateListItem(RemoteSite.Text, "site name") Then
        RemoteSite.SetFocus
        Exit Sub
    End If
    
    If Not ValidateNumber(RemotePort.Text, "port number") Then
        RemotePort.SetFocus
        Exit Sub
    End If
    
    If Not ValidateOptionalName(RemotePassword.Text, "password") Then
        RemotePassword.SetFocus
        Exit Sub
    End If
    
    If False Then
    
        ' Duplicate site entered.
        'todo: Recognize duplicates.
        MsgBox "This site name is a duplicate of an existing site."
    Else
    
        ' Acceptable results.
        Success = True
        Hide
    End If
End Sub

'/////////////////////////////////////////////////////////
' Text boxes.
'/////////////////////////////////////////////////////////

'
' Highlight text boxes when they get the focus.
'

Private Sub RemoteSite_GotFocus()
    RemoteSite.SelStart = 0
    RemoteSite.SelLength = Len(RemoteSite.Text)
End Sub

Private Sub RemotePort_GotFocus()
    RemotePort.SelStart = 0
    RemotePort.SelLength = Len(RemotePort.Text)
End Sub

Private Sub RemotePassword_GotFocus()
    RemotePassword.SelStart = 0
    RemotePassword.SelLength = Len(RemotePassword.Text)
End Sub

'/////////////////////////////////////////////////////////
' Standard form events.
'/////////////////////////////////////////////////////////

'
' When shown.
'
Private Sub Form_Activate()
    RemoteSite.SetFocus
End Sub

'/////////////////////////////////////////////////////////
' The end.
'/////////////////////////////////////////////////////////
