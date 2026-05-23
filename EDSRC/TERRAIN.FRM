VERSION 5.00
Begin VB.Form frmTerrain 
   Caption         =   "Terrain Properties"
   ClientHeight    =   5205
   ClientLeft      =   3570
   ClientTop       =   2865
   ClientWidth     =   5685
   HelpContextID   =   315
   Icon            =   "Terrain.frx":0000
   LinkTopic       =   "Form1"
   MDIChild        =   -1  'True
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   5205
   ScaleWidth      =   5685
   ShowInTaskbar   =   0   'False
   Begin VB.Frame Frame2 
      Caption         =   "TerraCon"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   11.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   -1  'True
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4575
      Left            =   120
      TabIndex        =   5
      Top             =   120
      Width           =   1455
   End
   Begin VB.CommandButton Help 
      Caption         =   "&Help"
      Height          =   375
      Left            =   120
      TabIndex        =   4
      Top             =   4800
      Width           =   975
   End
   Begin VB.CommandButton Close 
      Caption         =   "&Close"
      Height          =   375
      Left            =   4800
      TabIndex        =   3
      Top             =   4800
      Width           =   855
   End
   Begin VB.Frame Frame1 
      Caption         =   "Level Terrain Support"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   11.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   -1  'True
         Strikethrough   =   0   'False
      EndProperty
      Height          =   975
      Left            =   1680
      TabIndex        =   0
      Top             =   120
      Width           =   3975
      Begin VB.OptionButton Option2 
         Caption         =   "Disabled"
         Height          =   255
         Left            =   240
         TabIndex        =   2
         Top             =   600
         Value           =   -1  'True
         Width           =   975
      End
      Begin VB.OptionButton Option1 
         Caption         =   "Enabled"
         Height          =   255
         Left            =   240
         TabIndex        =   1
         Top             =   360
         Width           =   975
      End
   End
   Begin VB.Label Label1 
      Caption         =   "This feature is not yet implemented"
      Height          =   255
      Left            =   1680
      TabIndex        =   6
      Top             =   2040
      Width           =   3255
   End
End
Attribute VB_Name = "frmTerrain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Close_Click()
    Unload Me
End Sub

Private Sub Form_Load()
    Call Ed.SetOnTop(Me, "TerrainToolbar", TOP_PANEL)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call Ed.EndOnTop(Me)
End Sub

Private Sub Help_Click()
    SendKeys "{F1}"
End Sub
