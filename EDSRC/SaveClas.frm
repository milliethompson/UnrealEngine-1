VERSION 5.00
Begin VB.Form frmSaveClass 
   Caption         =   "Save Actor Classes"
   ClientHeight    =   1425
   ClientLeft      =   2820
   ClientTop       =   10140
   ClientWidth     =   5220
   Icon            =   "SaveClas.frx":0000
   LinkTopic       =   "Form1"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   1425
   ScaleWidth      =   5220
   ShowInTaskbar   =   0   'False
   Begin VB.Frame Frame2 
      Caption         =   "File Type"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   11.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   -1  'True
         Strikethrough   =   0   'False
      EndProperty
      Height          =   1275
      Left            =   120
      TabIndex        =   3
      Top             =   60
      Width           =   3975
      Begin VB.OptionButton SaveH 
         Caption         =   "C++ Header file (.h)"
         Height          =   195
         Left            =   120
         TabIndex        =   6
         Top             =   900
         Width           =   2355
      End
      Begin VB.OptionButton SaveU 
         Caption         =   "Text Actor Classes (.u)"
         Height          =   255
         Left            =   120
         TabIndex        =   5
         Top             =   540
         Width           =   2355
      End
      Begin VB.OptionButton SaveUCX 
         Caption         =   "Unreal Actor Classes (.ucx)"
         Height          =   255
         Left            =   120
         TabIndex        =   4
         Top             =   300
         Value           =   -1  'True
         Width           =   2355
      End
   End
   Begin VB.CommandButton Cancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   375
      Left            =   4200
      TabIndex        =   1
      Top             =   600
      Width           =   975
   End
   Begin VB.CommandButton Save 
      Caption         =   "&Save As..."
      Default         =   -1  'True
      Height          =   375
      Left            =   4200
      TabIndex        =   0
      Top             =   180
      Width           =   975
   End
   Begin VB.Label ClassName 
      Caption         =   "ClassName (invisible)"
      Height          =   735
      Left            =   4200
      TabIndex        =   2
      Top             =   1020
      Visible         =   0   'False
      Width           =   975
   End
End
Attribute VB_Name = "frmSaveClass"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'
' Modal save actor class form called from frmClassBrowser
'
Option Explicit

Private Sub Cancel_Click()
    GlobalAbortedModal = 1
    Unload Me
End Sub

Private Sub Form_Load()
    Call Ed.MakeFormFit(Me)
End Sub

Private Sub Save_Click()
    GResult = 0
    '
    If SaveUCX.Value Then
        frmDialogs.ClassSave.FilterIndex = 1
    ElseIf SaveU.Value Then
        frmDialogs.ClassSave.FilterIndex = 2
    Else ' SaveH.Value
        frmDialogs.ClassSave.FilterIndex = 3
    End If
    '
    GResult = 1
    GlobalAbortedModal = 0
    Unload Me
End Sub
