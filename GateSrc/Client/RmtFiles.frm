VERSION 5.00
Begin VB.Form RemoteFiles 
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Remote Files"
   ClientHeight    =   4425
   ClientLeft      =   4425
   ClientTop       =   5265
   ClientWidth     =   3870
   Icon            =   "RmtFiles.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   4425
   ScaleWidth      =   3870
   Begin VB.CommandButton RemoteFilesRefresh 
      Caption         =   "&Refresh"
      Height          =   315
      Left            =   60
      TabIndex        =   3
      Top             =   4080
      Width           =   1035
   End
   Begin VB.CommandButton RemoteFilesCancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   315
      Left            =   2760
      TabIndex        =   2
      Top             =   4080
      Width           =   1035
   End
   Begin VB.CommandButton RemoteFilesOk 
      Caption         =   "&Ok"
      Default         =   -1  'True
      Height          =   315
      Left            =   1680
      TabIndex        =   1
      Top             =   4080
      Width           =   1035
   End
   Begin VB.ListBox RemoteList 
      Height          =   3960
      Left            =   60
      Sorted          =   -1  'True
      TabIndex        =   0
      Top             =   60
      Width           =   3735
   End
End
Attribute VB_Name = "RemoteFiles"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' RmtFiles.frm: GateClient remote file lister/picker.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Presents a list of files on a GateServer,
'   and enables the user to select one.  This is used in
'   lieu of the common dialogs, which obviously don't
'   work with remote servers.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

' Command line we use to request the file list.
Dim CmdLine As String
Dim Result As String

'/////////////////////////////////////////////////////////
' Public functions.
'/////////////////////////////////////////////////////////

'
' Present the remote files dialog modally and let the
' user pick a file name.  Returns the filename if
' successful, "" if not.
'
Public Function DoModal(ThisCmdLine As String, _
    ThisCaption As String, ByRef ThisResult) As Boolean
    
    ' Remember the command line.
    CmdLine = ThisCmdLine
    
    ' Set the caption.
    Caption = ThisCaption

    ' Refresh the file list.
    RemoteFilesRefresh_Click
    
    ' Init result.
    Result = ""
    
    ' Show modally.
    Show 1
    
    ' Return result.
    If Result <> "" Then
        DoModal = True
        ThisResult = Result
    End If
End Function

'/////////////////////////////////////////////////////////
' Buttons.
'/////////////////////////////////////////////////////////

'
' Cancel the operation.
'
Private Sub RemoteFilesCancel_Click()
    Hide
End Sub

'
' Accept the selected file.
'
Private Sub RemoteFilesOk_Click()
    If RemoteList.ListIndex >= 0 Then
        Result = RemoteList.List(RemoteList.ListIndex)
    End If
    Hide
End Sub

'
' Double click accepts.
'
Private Sub RemoteList_DblClick()
    RemoteFilesOk_Click
End Sub

'
' Refresh the remote file list.
'
Private Sub RemoteFilesRefresh_Click()
    Dim S As String
    
    ' Init the result file list.
    LocalGateClient.ResultFileList = ""
    
    ' Execute the request command line.
    LocalGateClient.SafeExec (CmdLine)

    ' Empty the listbox.
    RemoteList.Clear
    
    ' Fill the list box with the results.
    S = LocalGateClient.ResultFileList
    While S <> ""
        RemoteList.AddItem NextSTRING(S, ";")
    Wend
    
    ' Select first item.
    If RemoteList.ListCount > 0 Then RemoteList.ListIndex = 0
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
