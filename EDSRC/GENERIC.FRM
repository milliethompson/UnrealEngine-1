VERSION 5.00
Begin VB.Form frmGeneric 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Temp Junk"
   ClientHeight    =   9690
   ClientLeft      =   2985
   ClientTop       =   3945
   ClientWidth     =   9075
   BeginProperty Font 
      Name            =   "MS Sans Serif"
      Size            =   8.25
      Charset         =   0
      Weight          =   700
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   ForeColor       =   &H80000008&
   HelpContextID   =   330
   Icon            =   "Generic.frx":0000
   LinkTopic       =   "Form8"
   MaxButton       =   0   'False
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   9690
   ScaleWidth      =   9075
   ShowInTaskbar   =   0   'False
End
Attribute VB_Name = "frmGeneric"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Form_Load()
    Call Ed.SetOnTop(Me, "Temp", TOP_NORMAL)
End Sub
