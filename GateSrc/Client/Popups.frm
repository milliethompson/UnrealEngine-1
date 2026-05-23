VERSION 4.00
Begin VB.Form Popups 
   Caption         =   "Form1"
   ClientHeight    =   525
   ClientLeft      =   3420
   ClientTop       =   2805
   ClientWidth     =   5145
   Height          =   1125
   Icon            =   "Popups.frx":0000
   Left            =   3360
   LinkTopic       =   "Form1"
   ScaleHeight     =   525
   ScaleWidth      =   5145
   Top             =   2265
   Width           =   5265
   Begin VB.Menu Level 
      Caption         =   "Level"
      Begin VB.Menu LevelProperties 
         Caption         =   "&Level Properties..."
      End
      Begin VB.Menu LevelDivider 
         Caption         =   "-"
      End
      Begin VB.Menu LevelMessage 
         Caption         =   "&Message"
      End
      Begin VB.Menu LevelToggleAuto 
         Caption         =   "&Toggle Autolaunch"
      End
      Begin VB.Menu LevelDivider2 
         Caption         =   "-"
      End
      Begin VB.Menu LevelUpIt 
         Caption         =   "&Up It"
      End
      Begin VB.Menu LevelDownIt 
         Caption         =   "&Down It"
      End
      Begin VB.Menu LevelDivider3 
         Caption         =   "-"
      End
      Begin VB.Menu LevelDelete 
         Caption         =   "Delete Level"
      End
   End
   Begin VB.Menu User 
      Caption         =   "User"
      Begin VB.Menu UserMessage 
         Caption         =   "&Message"
      End
      Begin VB.Menu UserBroadcast 
         Caption         =   "&Broadcast"
      End
      Begin VB.Menu UserDivider 
         Caption         =   "-"
      End
      Begin VB.Menu UserBanish 
         Caption         =   "Banish"
      End
   End
End
Attribute VB_Name = "Popups"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' Popups.frm: GateClient popup menus.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Just contains a menu with a bunch of popups.
'   This form is never displayed/activated.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Level routers.
'/////////////////////////////////////////////////////////

'
' Route all Level popup menu items to the appropriate
' buttons on the Main form.
'

Private Sub LevelDelete_Click()
    Main.LevelsDelete_Click
End Sub

Private Sub LevelDownIt_Click()
    Main.LevelsDownIt_Click
End Sub

Private Sub LevelProperties_Click()
    Main.LevelsProperties_Click
End Sub

Private Sub LevelMessage_Click()
    Main.LevelsMessage_Click
End Sub

Private Sub LevelUpIt_Click()
    Main.LevelsUpIt_Click
End Sub

'/////////////////////////////////////////////////////////
' User routers.
'/////////////////////////////////////////////////////////

Private Sub UserMessage_Click()
    Main.UserMessage_Click
End Sub

Private Sub UserBanish_Click()
    Main.UserBanish_Click
End Sub

Private Sub UserBroadcast_Click()
    Main.UserBroadcast_Click
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
