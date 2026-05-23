VERSION 5.00
Object = "{0BA686C6-F7D3-101A-993E-0000C0EF6F5E}#1.0#0"; "THREED32.OCX"
Begin VB.Form frmBrushBrowser 
   BorderStyle     =   0  'None
   Caption         =   "Brush Browser"
   ClientHeight    =   6465
   ClientLeft      =   8505
   ClientTop       =   5280
   ClientWidth     =   2415
   Icon            =   "BrBrush.frx":0000
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   6465
   ScaleWidth      =   2415
   ShowInTaskbar   =   0   'False
   Begin Threed.SSPanel SSPanel1 
      Align           =   2  'Align Bottom
      Height          =   960
      Left            =   0
      TabIndex        =   0
      Top             =   5505
      Width           =   2415
      _Version        =   65536
      _ExtentX        =   4260
      _ExtentY        =   1693
      _StockProps     =   15
      BackColor       =   14198960
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Begin VB.CommandButton BroExport 
         Caption         =   "E&xport"
         Height          =   255
         Left            =   1200
         TabIndex        =   1
         Tag             =   "Export textures"
         Top             =   720
         Width           =   1215
      End
      Begin VB.CommandButton BroImport 
         Caption         =   "&Import"
         Height          =   255
         Left            =   0
         TabIndex        =   2
         Tag             =   "Import textures"
         Top             =   720
         Width           =   1215
      End
      Begin VB.CommandButton BroSave 
         Caption         =   "&Save"
         Height          =   255
         Left            =   1200
         TabIndex        =   3
         Tag             =   "Save texture families"
         Top             =   480
         Width           =   1215
      End
      Begin VB.CommandButton BroLoad 
         Caption         =   "&Load"
         Height          =   255
         Left            =   0
         TabIndex        =   4
         Tag             =   "Load texture families"
         Top             =   480
         Width           =   1215
      End
      Begin VB.CommandButton Command1 
         Caption         =   "&Use This Brush"
         Height          =   255
         Left            =   0
         TabIndex        =   5
         Tag             =   "Save texture families"
         Top             =   240
         Width           =   2415
      End
      Begin VB.CommandButton Command2 
         Caption         =   "&Remember Current Brush"
         Height          =   255
         Left            =   0
         TabIndex        =   6
         Tag             =   "Load texture families"
         Top             =   0
         Width           =   2415
      End
   End
End
Attribute VB_Name = "frmBrushBrowser"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'
' Sound Browser: This is a form that implements
' the browser interface.
'
' See ClassBr.frm for an example implementation
'
Option Explicit

'
' Public (Browser Interface)
'

Private Sub Form_Load()
    Call Ed.SetOnTop(Me, "SoundBrowser", TOP_BROWSER)
End Sub

Public Sub BrowserShow()
    Show
End Sub

Public Sub BrowserRefresh()
End Sub

Public Sub BrowserHide()
    Unload Me
End Sub

Public Function GetCurrent() As String
    GetCurrent = ""
End Function

'
' Private
'

