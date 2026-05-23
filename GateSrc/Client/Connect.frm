VERSION 4.00
Begin VB.Form Connect 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Connection Progress"
   ClientHeight    =   1020
   ClientLeft      =   2895
   ClientTop       =   1455
   ClientWidth     =   4725
   ControlBox      =   0   'False
   Height          =   1380
   Icon            =   "Connect.frx":0000
   Left            =   2835
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1020
   ScaleWidth      =   4725
   ShowInTaskbar   =   0   'False
   Top             =   1155
   Width           =   4845
   Begin VB.Timer ConnectionTimer 
      Enabled         =   0   'False
      Interval        =   100
      Left            =   60
      Top             =   600
   End
   Begin VB.CommandButton Cancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   315
      Left            =   3720
      TabIndex        =   4
      Top             =   60
      Width           =   915
   End
   Begin WINSOCKLib.TCP TCP 
      Left            =   4260
      Top             =   660
      _ExtentX        =   635
      _ExtentY        =   635
      RemoteHost      =   ""
      RemotePort      =   0
      LocalPort       =   0
   End
   Begin VB.Label ConnectHost 
      Caption         =   "Label2"
      Height          =   255
      Left            =   1320
      TabIndex        =   3
      Top             =   120
      Width           =   2235
   End
   Begin VB.Label Action 
      Alignment       =   1  'Right Justify
      Caption         =   "Connecting to:"
      Height          =   255
      Left            =   60
      TabIndex        =   2
      Top             =   120
      Width           =   1155
   End
   Begin VB.Label ConnectStatus 
      Alignment       =   2  'Center
      Caption         =   "Trying..."
      Height          =   195
      Left            =   300
      TabIndex        =   1
      Top             =   780
      Width           =   4035
   End
   Begin ComctlLib.ProgressBar ConnectProgress 
      Height          =   255
      Left            =   60
      TabIndex        =   0
      Top             =   480
      Width           =   4575
      _Version        =   65536
      _ExtentX        =   8070
      _ExtentY        =   450
      _StockProps     =   192
      Appearance      =   1
   End
End
Attribute VB_Name = "Connect"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' Connect.frm: GateClient code to manage the connection
'              with a local or remote GateServer.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Contains all initial-connection code and later
'   sending/receiving and error handling.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Variables.
'/////////////////////////////////////////////////////////

Public TransactionsPending As Long

' List of subscribed topics and forms.
Private SubscriptionDatabases(GATESERVER_MAX_DATA_NOTIFYS) As String
Private SubscriptionParms(GATESERVER_MAX_DATA_NOTIFYS) As String
Private SubscriptionForms(GATESERVER_MAX_DATA_NOTIFYS) As Object
Private SubscriptionSingleRecords(GATESERVER_MAX_DATA_NOTIFYS) As Boolean

' Subscription request in progress.
Private Subscribing As Boolean
Private SubscribingParms As String
Private SubscribingSingleRecord As Boolean
Private SubscribingForm As Object

' Variables for hiding/unhiding progress bar.
Private Const UNHIDE_COUNTDOWN = 10 ' 10ths of seconds
Private UnhideCount As Long

'/////////////////////////////////////////////////////////
' DoModal.
'/////////////////////////////////////////////////////////

'
' Routine to activate this form modally.
'
Public Sub DoModal()
    
    ' Hide this form
    Top = -Me.Height
    UnhideCount = UNHIDE_COUNTDOWN
    
    ' Enable the timer.
    ConnectionTimer.Enabled = True
    
    If LocalGateClient.IsConnected Then
        
        ' Form was activated in order to perform a
        ' transaction.  So, send our data and await
        ' a response.
        
        Action.Caption = "Executing:"
        ConnectHost.Caption = GCmdLine
        ConnectStatus.Caption = "Waiting for response..."
        
        ' Trap TCP errors.
        On Error GoTo SocketError2
    
        ' Send the command line.
        TCP.SendData GCmdLine & Chr(13)
    
        ' Note that we're waiting for a response.
        TransactionsPending = TransactionsPending + 1
    
        ' Continue on with modal form displayed.
        Show 1
        Exit Sub

SocketError2:
        ' Fatal error attempting to send.
        MsgBox "Lost connection to the server."
        
        ' Catastrophic failure.
        End
    
    Else
        
        ' Form was activated to perform an initial
        ' connection attempt.
        
        ' Init result.
        LocalGateClient.ConnectResultText = ""
        LocalGateClient.IsConnected = False
        
        ' Set host display.
        ConnectHost.Caption = LocalGateClient.ConnectSite
        
        ' Set up main form's TCP control to make a connection.
        On Error GoTo SocketError1
        TCP.Close
        TCP.LocalPort = 0
        TCP.RemoteHost = LocalGateClient.ConnectSite
        TCP.RemotePort = LocalGateClient.ConnectPort
        TCP.Connect
        
        ' Set initial status.
        ConnectionTimer_Timer
        
        ' Success.
        Show 1
        Exit Sub
        
        ' Handle any socket errors:
SocketError1:
        LocalGateClient.ConnectResultText = "Error initiating socket"
    End If
End Sub

'/////////////////////////////////////////////////////////
' Standard form events.
'/////////////////////////////////////////////////////////

'
' Standard form unload event.
'
Private Sub Form_Unload(Cancel As Integer)

    ' Disconnect Tcp socket.
    On Error GoTo TcpError
    TCP.Disconnect
TcpError:
End Sub

'/////////////////////////////////////////////////////////
' Cancel button.
'/////////////////////////////////////////////////////////

'
' Cancel button - stops connection attempt and fails.
'
Private Sub Cancel_Click()
    
    If LocalGateClient.IsConnected Then
        
        ' User clicked on Cancel during send/receive.
        Dim Result As Long
        Result = MsgBox("Interrupted operation: " & GCmdLine, vbRetryCancel)
        
        ' Exit program if canceled.
        If Result = vbCancel Then End
    
    Else
        ' User clicked on Cancel during initial
        ' connection attempt.
        
        ' Connection attempt failed.
        LocalGateClient.ConnectResultText = "Canceled by user"
    
        ' Deactivate the timer.
        ConnectionTimer.Enabled = False
        
        ' Modal return with failure.
        Hide
    End If
End Sub

'/////////////////////////////////////////////////////////
' Connection timer.
'/////////////////////////////////////////////////////////

'
' Connection timer event.
'
Private Sub ConnectionTimer_Timer()

    ' Update progress bar.
    ConnectProgress.Value = (ConnectProgress.Value + 1) Mod 100

    ' Unhide if connecting is taking a while.
    UnhideCount = UnhideCount - 1
    If UnhideCount = 0 Then
    
        ' Unhide it.
        Top = 0.5 * (Screen.Height - Height)
        Left = 0.5 * (Screen.Width - Width)
    End If
    
    If LocalGateClient.IsConnected Then
        
        ' We are connected and engaged in
        ' a send or receive operation.

    Else
        ' We are making an initial connection attempt.

        ' Get TCP state.
        Dim TcpState As Long
        TcpState = TCP.State

        ' Display message according to TCP progress.
        Select Case TcpState

            ' Informational state changes.
            Case sckConnectionPending
                ConnectStatus.Caption = "Pending..."
            Case sckResolvingHost
                ConnectStatus.Caption = "Resolving host..."
            Case sckConnecting
                ConnectStatus.Caption = "Connecting..."
            Case sckConnected
                ConnectStatus.Caption = "Connected..."

            ' Failure state changes.
            Case sckClosed
                FailedConnectReturn "Socket unexpectedly closed"
            Case sckOpen
                FailedConnectReturn "Socket unexpectedly opened"
            Case sckListening
                FailedConnectReturn "Socket unexpectedly listened"
            Case sckHostResolved
                FailedConnectReturn "Socket unexpectedly resolved"
            Case sckClosing
                FailedConnectReturn "Socket unexpectedly started closing"
            Case sckError
                FailedConnectReturn "Unknown socket error"
            Case Else
                FailedConnectReturn "Unknown socket result"

        End Select
    End If
End Sub

'/////////////////////////////////////////////////////////
' Public functions.
'/////////////////////////////////////////////////////////

'
' Called for each numeric code that is received.
' Assumes that IsConnected=True.
'
Private Sub ProcessIncomingCode(Code As Long, ByVal Extra As String)
    Dim Topic As String, Key As String, Value As String
    Dim i As Integer

    ' State assertion.
    If Not LocalGateClient.IsConnected Then
        MsgBox "ProcessIncomingCode inconsistency."
        End
    End If

    If (Code >= GK_Success And Code <= GK_Success_Max) Or _
        (Code >= GK_Failure And Code <= GK_Failure_Max) Then
        
        ' Synchronous success or failure.

        ' State check.
        If TransactionsPending = 0 Then
            ' Server sent unexpected code.
            MsgBox "Server sent unexpected success/failure code " & Code & "."
            Exit Sub
        End If
        
        If Code >= GK_Failure And Code <= GK_Failure_Max Then
            
            ' Display failures.
            LocalGateClient.Log Code & " " & Extra
        ElseIf Code = GK_Success_Login Then
        
            ' Remember user name.
            LocalGateClient.ResultUserName = NextSTRING(Extra)
        
        ElseIf Code = GK_Success_FileList Then
        
            ' Remember file list.
            LocalGateClient.ResultFileList = NextSTRING(Extra)
        
        ElseIf Code = GK_Success_DbSubscribed Then
        
            ' Handle successful database subscription.
            If Subscribing Then
            
                ' Add this to subscription table.
                Dim Num As Long
                Num = Val(Extra) - GK_DbNotify - 1
                If Num < 0 Or Num >= GATESERVER_MAX_DATA_NOTIFYS Then
                    MsgBox "Failed subscription to " & SubscribingParms & "."
                    End
                End If
            
                ' Add to subscription table.
                SubscriptionParms(Num) = SubscribingParms
                SubscriptionSingleRecords(Num) = SubscribingSingleRecord
                Set SubscriptionForms(Num) = SubscribingForm
                
                ' Get raw database name.
                SubscriptionDatabases(Num) = NextSTRING(NextSTRING(SubscribingParms, ":"), " ")
            Else
            
                ' We weren't trying to subscribe.
                MsgBox "Bad subscription received"
                End
            End If
        End If

        ' Set result information.
        GResultCode = Code
        GResultData = Extra
        
        ' Note that we're finished transacting.
        TransactionsPending = TransactionsPending - 1
        
        ' Modal success return.
        If TransactionsPending = 0 Then
            
            ' Shut down the timer.
            ConnectionTimer.Enabled = False
            
            Hide
        End If
        
    ElseIf Code >= GK_DbNotify And Code <= GK_DbNotify_Max Then
    
        ' Turn on IsNotify so that control updates don't recurse.
        IsNotify = True
    
        If Code = GK_DbNotify Then
        
            ' We shouldn't get this in the normal course of events.
            LocalGateClient.Log Code & " " & Extra
        Else
        
            ' Handle an update to a subscribed database.
            '
            ' Code-GK_DbNotify-1 is index into subscription table.
            ' Extra looks like "+ <keyfieldvalue_name> [<key_name>=<value_parm>]...
            '
            ' or like "- <keyfieldvalue_name>".
            
            ' Find out what subscription this corresponds to.
            Dim Index As Integer
            Index = Code - GK_DbNotify - 1
            If SubscriptionDatabases(Index) <> "" Then
                If SubscriptionSingleRecords(Index) Then
                    
                    ' Single-record, so handle each item
                    ' individually:
                    Call SingleSubscriptionNotify( _
                        SubscriptionForms(Index), _
                        SubscriptionDatabases(Index), _
                        Extra)
                Else
                
                    ' Multi-record, so handle as database.
                    Call MultiSubscriptionNotify( _
                        SubscriptionForms(Index), _
                        SubscriptionDatabases(Index), _
                        Extra)
                End If
            End If
        End If
    
        ' Turn off IsNotify to restore user control updates.
        IsNotify = False
    
    ElseIf Code >= GK_Notify And Code <= GK_Notify_Max Then
        ' Handle notification.
        
        ' Turn on IsNotify so that control updates don't recurse.
        IsNotify = True
        
        If Code = GK_Notify_Version Then
            
            ' Display server version.
            LocalGateClient.Log Code & " " & Extra
            Main.RemoteVersion.Caption = Extra
        
        ElseIf Code = GK_Notify_Maker Then
            
            ' Display server software maker.
            LocalGateClient.Log Code & " " & Extra
            Main.RemoteMaker.Caption = Extra
        
        ElseIf Code = GK_Notify_PrintMe Then
            
            ' Display text.
            LocalGateClient.Log Code & " " & Extra
        
        ElseIf Code = GK_Notify_Time Then
        
            ' Synchronize local time with server time.
            LocalGateClient.NoteServerTimeNow Val(Extra)
        End If
        
        ' Turn off IsNotify to restore user control updates.
        IsNotify = False
    End If
End Sub

'
' Called for each line that is received.
' Assumes that IsConnected=True.
'
Private Sub ProcessIncomingLine(Line As String)

    ' State assertion.
    If Not LocalGateClient.IsConnected Then
        MsgBox "ProcessIncomingLine inconsistency."
        End
    End If

    ' See if this matches our pattern.
    If Len(Line) = 3 Or Mid(Line, 4, 1) = " " Then
        
        ' See if this is a numeric code.
        Dim Code As Long
        Code = Val(Left(Line, 3))
        If Code >= 100 And Code <= 999 Then
            ' Here we have a valid numeric code.
            
            ' Process this code and its extra data.
            Call ProcessIncomingCode(Code, Mid(Line, 5))
        End If
    End If
End Sub

'
' Connect attempt failed handler.  Performs a modal
' return of information about a failed connection
' attempt.  This is non-fatal.
'
Private Sub FailedConnectReturn(Problem As String)

    ' Set error message.
    LocalGateClient.ConnectResultText = Problem

    ' TCP error.
    ConnectProgress.Value = False
    
    ' Deactivate the timer.
    ConnectionTimer.Enabled = False
    
    ' Modal return with failure.
    Hide

End Sub

'/////////////////////////////////////////////////////////
' TCP events.
'/////////////////////////////////////////////////////////

'
' TCP error event.
'
Private Sub TCP_Error(Number As Integer, Description As String, Scode As Long, Source As String, HelpFile As String, HelpContext As Long, CancelDisplay As Boolean)
    
    ' Handle error in its context.
    If LocalGateClient.IsConnected Then
    
        ' We are connected.  Any TCP errors here are unrecoverable.
        MsgBox "Error communicating with server ( " & _
            Number & "): " & Description & "."
        
        ' Fatal error.
        End
    
    Else
    
        ' We are not yet connected; TCP errors can be
        ' expected while attempting to establish a connection.
        
        FailedConnectReturn Description
    End If
End Sub

'
' TCP close event.
'
Private Sub TCP_Close()

    ' If we're connected, this is Bad.
    If LocalGateClient.IsConnected Then
    
        ' We are connected.  Any TCP errors here are unrecoverable.
        MsgBox "Server disconnected on us."
        
        ' Fatal error.
        End
    
    End If
End Sub

'
' TCP connect event.
' Expects that IsConnected=False.
'
Private Sub TCP_Connect()

    ' State assertion.
    If LocalGateClient.IsConnected Then
        MsgBox "TCP_Connect inconsistency."
        End
    End If

    ' We have just connected successfully!
    LocalGateClient.IsConnected = True

    ' Disable timer.
    ConnectionTimer.Enabled = False

    ' Modal success return.
    Hide
End Sub

'
' TCP data arrival event.
' Expects that IsConnected=True.
'
Private Sub TCP_DataArrival(ByVal bytesTotal As Long)

    ' Grab the incoming string.
    Dim Temp As String
    Call TCP.GetData(Temp, vbString)
    
    If Not LocalGateClient.IsConnected Then
        MsgBox "TCP_DataArrival inconsistency."
        End
    End If
    
    '
    ' Handle the incoming data for an existing connection.
    '
    
    ' Static string accumulator: accumulates incoming
    ' text until a cr is reached.
    Static StringAccumulator As String
    
    ' Process all cr-terminated lines of the incoming data.
    While InStr(Temp, Chr(13)) > 0
        
        ' Remove any leading lf
        If Left$(Temp, 1) = Chr$(10) Then
            Temp = Mid$(Temp, 2)
        End If
        
        ' Get position of cr.
        Dim i As Long
        i = InStr(Temp, Chr(13))
        
        ' Process the completed string up to the cr.
        ProcessIncomingLine StringAccumulator + Left$(Temp, i - 1)
        
        ' Clear the string accumulator.
        StringAccumulator = ""
        
        ' Remove the just-processed text and the cr.
        Temp = Mid$(Temp, i + 1)
    Wend
    
    ' Remove any leading lf
    If Left$(Temp, 1) = Chr$(10) Then
        Temp = Mid$(Temp, 2)
    End If
    
    ' Add the remainder of the text to the string accumulator.
    StringAccumulator = StringAccumulator + Temp
    
End Sub

'/////////////////////////////////////////////////////////
' Property subscribing and unsubscribing.
'/////////////////////////////////////////////////////////

'
' Subscribe a form to a database.
'
Public Sub Subscribe(DatabaseParms As String, _
    Frm As Object, _
    SingleRecord As Boolean)

    ' Set globals so that incoming parser knows we're
    ' trying to subscribe to a database.
    Subscribing = True
    SubscribingParms = UCase(DatabaseParms)
    SubscribingSingleRecord = SingleRecord
    Set SubscribingForm = Frm

    ' Subscribe, not allowing failure.
    Call LocalGateClient.SafeExec("SUB " & DatabaseParms)

    ' Note that we're no longer waiting for a subscription
    ' response.
    Subscribing = False
    SubscribingParms = ""
    SubscribingSingleRecord = False
    Set SubscribingForm = Nothing
End Sub

'
' Unsubscribe a form from a database.
'
Public Sub Unsubscribe(DatabaseParms As String, Frm As Object)
    Dim i As Integer, j As Integer
    
    ' Remove from list
    For i = 0 To GATESERVER_MAX_DATA_NOTIFYS - 1
        If SubscriptionDatabases(i) <> "" Then
            If SubscriptionParms(i) = UCase(DatabaseParms) And _
                SubscriptionForms(i).hwnd = Frm.hwnd Then
                
                ' Delete it.
                SubscriptionDatabases(i) = ""
                SubscriptionParms(i) = ""
                Set SubscriptionForms(i) = Nothing
                Exit Sub
            End If
        End If
    Next
End Sub

'/////////////////////////////////////////////////////////
' Control notifications.
'/////////////////////////////////////////////////////////

'
' Notify a form of a value change in a single-record
' database.
'
Public Sub SingleSubscriptionNotify(Frm As Object, _
    Database As String, Data As String)
    
    ' Locals.
    Dim i As Integer, j As Integer, Found As Integer
    Dim Keyfield As String
    Dim OriginalData As String, Find As String
    Dim Key As String, Value As String
    Dim Original As String, Item As String
    Dim Duplicate As Boolean
    
    ' Remember original.
    OriginalData = Data
    
    ' See if we're adding or subtracting a record.
    If GetCHAR(Data, "-") Then
    
        ' Subtracting. Just notify subscribing forms.
        Call Frm.NotifyDelete(Database, NextSTRING(Data))
        Exit Sub
    ElseIf Not GetCHAR(Data, "+") Then
    
        ' Not adding - should never happen.
        MsgBox "Bad single record DbNotify: " & OriginalData
        End
    End If
    
    ' Skip Keyfield value.
    If NextSTRING(Data) = "" Then
        MsgBox "Bad DbNotify: " & OriginalData
        End
    End If
    
    ' Process each key=value pair.
    While Data <> ""
    
        ' Get next pair.
        Key = NextSTRING(Data, "=")
        Value = NextSTRING(Data, " ")
        
        ' Note not found.
        Found = False
    
        ' Tag to match.
        Find = UCase(Database) & ":" & UCase(Key)
        
        ' Check all controls on the form to see if one wants
        ' this topic/key.
        For i = 0 To Frm.Controls.Count - 1
            
            ' Check for matching Tag.
            If UCase(Frm.Controls(i).Tag) = Find Then
            
                ' Update found count.
                Found = Found + 1
            
                ' Update the control based on its type.
                If TypeOf Frm.Controls(i) Is TextBox Then
                
                    ' Update TextBox.
                    Frm.Controls(i).Text = Value
                
                ElseIf TypeOf Frm.Controls(i) Is CheckBox Then
                
                    ' Update CheckBox.
                    Frm.Controls(i).Value = Val(Value)
                
                ElseIf TypeOf Frm.Controls(i) Is ListBox Then
                
                    ' Remember current thing.
                    If Frm.Controls(i).ListIndex >= 0 Then
                        Original = UCase(Frm.Controls(i).List(Frm.Controls(i).ListIndex))
                    End If
                    
                    ' Clear it.
                    Frm.Controls(i).Clear
                
                    ' Snag each item.
                    Dim TempValue As String
                    TempValue = Value
                    Item = NextSTRING(TempValue, ";")
                    While Item <> ""
                    
                        ' Add item to ListBox.
                        Frm.Controls(i).AddItem (Item)
                        
                        ' Make this current if it was current before.
                        If UCase(Item) = Original Then
                            Frm.Controls(i).ListIndex = Frm.Controls(i).ListCount - 1
                        End If
                        
                        ' Go to next item.
                        Item = NextSTRING(TempValue, ";")
                    Wend
                    
                    ' If nothing selected, select first.
                    If Frm.Controls(i).ListIndex = -1 And _
                        Frm.Controls(i).ListCount > 0 Then
                        Frm.Controls(i).ListIndex = 0
                    End If
                                    
                ElseIf TypeOf Frm.Controls(i) Is Label Then
                
                    ' Update Label.
                    Frm.Controls(i).Caption = Value
    
                Else
                
                    ' For debugging.
                    'MsgBox "Unknown control for " & Topic & "." & Key
                End If
            End If
        Next
        
        ' For debugging.
        'If Found = 0 Then MsgBox "Not found: " & Database & "." & Key
    Wend
End Sub

'
' Notify a form of a value change in a multi-record
' database.
'
Public Sub MultiSubscriptionNotify(Ctl As Object, _
    Database As String, Data As String)

    ' Locals.
    Dim i As Integer, j As Integer
    Dim KeyfieldLen As Integer, Comp As Integer
    Dim TempGridKeys As String, TempKey As String
    Dim Key As String, Value As String
    Dim Adding As Boolean, Found As Boolean
    Dim Keyfield As String, OriginalData As String
    Dim Find As String

    ' Remember original.
    OriginalData = Data

    ' See if we're adding or subtracting a record.
    If GetCHAR(Data, "+") Then
        Adding = True
    ElseIf GetCHAR(Data, "-") Then
        Adding = False
    Else
        MsgBox "Unknown DbNotify: " & OriginalData
        Exit Sub
    End If

    ' Get keyfield.
    Keyfield = NextSTRING(Data)
    If Keyfield = "" Then
        MsgBox "Invalid multi keyfield"
        End
    End If
    KeyfieldLen = Len(Keyfield)

    ' Make text to find.
    Find = UCase(Database)

    ' Found the matching control.
    If TypeOf Ctl Is ListBox Then

        ' Update listbox.
        If Adding Then

            ' Add this to listbox.
            Ctl.AddItem Keyfield & _
                Chr(9) & Chr(9) & Chr(9) & _
                Chr(9) & Chr(9) & Chr(9) & _
                Data
                
            ' Set current if not yet set.
            If Ctl.ListIndex < 0 Then
                Ctl.ListIndex = 0
            End If
        Else

            ' Remove this from listbox.
            For i = 0 To Ctl.ListCount - 1
                If UCase(Left(Ctl.List(i), KeyfieldLen)) _
                    = UCase(Keyfield) Then
                    
                    ' Remove this
                    Ctl.RemoveItem i
                    Exit Sub
                End If
            Next
        End If
    ElseIf TypeOf Ctl Is Grid Then

        ' Inhibit grid ui.
        RefreshingGrid = True
        
        ' Update grid.
        If Ctl.Enabled Then
            ' Adding, removing, or updating item in
            ' a nonempty grid.
            
            ' Find this keyfield value in column 0 of
            ' the grid, or find properly sorted position
            ' at which to insert it.
            Found = False
            Ctl.Col = 0
            
            ' Check each item until we find the proper
            ' place for this item.
            For i = 1 To Ctl.Rows - 1
            
                ' Set grid position.
                Ctl.Row = i
                
                ' Perform comparison.
                Comp = StrComp(Ctl.Text, Keyfield, 1)
                If Comp = 0 Then
                
                    ' Exact match.
                    Found = True
                    GoTo HandleGrid
                ElseIf Comp > 0 Then
                
                    ' Passed it.
                    GoTo HandleGrid
                End If
            Next
        Else
            i = 1
        End If
HandleGrid:
        If Adding Then
            ' Adding or updating item.
            
            If Not Found Then
                
                ' Add item and set keyfield value.
                Ctl.AddItem Keyfield, i
            End If
            
            ' Here we are modifying an existing row
            ' which may have just been added.
            
            ' Set all grid cells in this row.
            Ctl.Row = i
            While Data <> ""
                
                ' Grab a key-value pair.
                Key = NextSTRING(Data, "=")
                Value = NextSTRING(Data, " ")
                
                ' See if this key exists in the grid heading.
                TempGridKeys = Ctl.Tag
                j = 0
                While TempGridKeys <> ""
                    TempKey = NextSTRING(TempGridKeys, ";")
                    If StrComp(Key, TempKey) = 0 Then
                        Ctl.Col = j
                        Ctl.Text = Value
                    End If
                    j = j + 1
                Wend
            Wend
                        
            If Adding And (Not Found) And _
                (Not Ctl.Enabled) Then
            
                ' Added to empty grid; remove bogus
                ' element.
                Ctl.RemoveItem 2
                Ctl.Enabled = True
            End If
        ElseIf Found Then
            
            ' Delete an existing item.
            If Ctl.Rows > 2 Then
            
                ' Remove a normal item.
                Ctl.RemoveItem i
            Else
                
                ' Remove the last item.
                Ctl.Enabled = False
                Ctl.AddItem ""
                Ctl.RemoveItem i
            End If
        End If
    
        ' Reenable grid ui.
        RefreshingGrid = False
        
        ' Change the column in order to trigger a
        ' RowColChange event so the grid can update
        ' itself.
        Ctl.Col = IIf(Ctl.Col = 0, 1, 0)
    End If
End Sub

'/////////////////////////////////////////////////////////
' Control updating.
'/////////////////////////////////////////////////////////

'
' Note that a control has changed value.
'
Public Sub UpdateControl(Ctl As Object, _
    IsValid As Boolean, ByVal KeyValue As String)

    ' Locals.
    Dim S As String, i As Integer, Temp As String
    Dim Database As String, Key As String
    
    ' Fix up key value.
    If KeyValue <> "" Then KeyValue = KeyValue & " "
    KeyValue = " " & KeyValue
    
    Temp = Ctl.Tag
    Database = NextSTRING(Temp, ":")
    Key = NextSTRING(Temp)

    ' Don't do this if we're being notified.
    If IsNotify Then Exit Sub

    ' Figure out what type of control this is.
    If TypeOf Ctl Is TextBox Then
    
        ' Update TextBox.
        If IsValid Then LocalGateClient.AsyncExec "SET " & Database & KeyValue & Key & "=" & Quotes(Ctl.Text)

    ElseIf TypeOf Ctl Is CheckBox Then
    
        ' Update CheckBox.
        If IsValid Then LocalGateClient.AsyncExec "SET " & Database & KeyValue & Key & "=" & IIf(Ctl.Value, "1", "0")
    
    ElseIf TypeOf Ctl Is ListBox Then
    
        ' Accumulate settings.
        For i = 0 To Ctl.ListCount - 1
            If S <> "" Then S = S + ";"
            S = S + Ctl.List(i)
        Next
        
        ' Update it.
        If IsValid Then LocalGateClient.AsyncExec "SET " & Database & KeyValue & Key & "=" & Quotes(S)

    ElseIf TypeOf Ctl Is Label Then
    
        ' Update Label.
        If IsValid Then LocalGateClient.AsyncExec "SET " & Database & KeyValue & Key & "=" & Quotes(Ctl.Caption)
    Else
    
        ' For debugging.
        'MsgBox "Unknown control for " & Topic & "." & Key
    
    End If
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
