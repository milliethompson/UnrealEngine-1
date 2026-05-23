VERSION 4.00
Begin VB.Form Editor 
   Caption         =   "Viewer/Editor"
   ClientHeight    =   3975
   ClientLeft      =   1770
   ClientTop       =   2730
   ClientWidth     =   7410
   Height          =   4575
   Icon            =   "Editor.frx":0000
   Left            =   1710
   LinkTopic       =   "Form1"
   ScaleHeight     =   3975
   ScaleWidth      =   7410
   Top             =   2190
   Width           =   7530
   Begin VB.TextBox EditorText 
      Height          =   3975
      Left            =   0
      MultiLine       =   -1  'True
      TabIndex        =   0
      Text            =   "Editor.frx":030A
      Top             =   0
      Width           =   7395
   End
   Begin VB.Menu EditorFile 
      Caption         =   "&File"
      Begin VB.Menu EditorSave 
         Caption         =   "&Save"
      End
      Begin VB.Menu EditorSaveAsLocal 
         Caption         =   "Save &As (Local)"
      End
      Begin VB.Menu EditorDivider1 
         Caption         =   "-"
      End
      Begin VB.Menu EditorExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu EditorEdit 
      Caption         =   "&Edit"
      Begin VB.Menu EditorCut 
         Caption         =   "Cu&t"
      End
      Begin VB.Menu EditorCopy 
         Caption         =   "&Copy"
      End
      Begin VB.Menu EditorPaste 
         Caption         =   "&Paste"
      End
   End
End
Attribute VB_Name = "Editor"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' Editor.frm: GateClient text editor/viewer that
'             handles both local and remote files.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Presents a standard interface for editing or viewing
'   local or remote files.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Standard form events
'/////////////////////////////////////////////////////////

Private Sub Form_Resize()
    EditorText.Width = ScaleWidth
    EditorText.Height = ScaleHeight
End Sub

'/////////////////////////////////////////////////////////
' The End
'/////////////////////////////////////////////////////////
