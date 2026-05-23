VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "DbHook"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' DbHook.cls: Gatekeeper server database hook class.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   A DbHook object is responsible for tracking changes
'   to one server-side database, and notifying clients
'   of all changes to it.  All database are flat,
'   single-unique-keyed, record-oriented collections of
'   text.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Public variables.
'/////////////////////////////////////////////////////////

' Public information about this database.
Public DbName As String
Public DbDescription As String
Public DbReadAccess As Long
Public DbWriteAccess As Long
Public DbFieldKeys As String
Public DbFieldTypes As String
Public DbSingleRecord As Boolean

' Private information determined from the above, set by Init.
Private NumFields As Long
Private FieldTypes() As String
Private FieldKeys() As String
Private UCaseFieldKeys() As String

' Temporary variables for the utility functions.
Private TempValues() As String
Private TempOriginalValues() As String
Private TempChanged() As Boolean
Private TempRequested() As Boolean

' Objects.
Private Keeper As Gatekeeper

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

' Record type.
Private Type DbRecord
    Key As String
    Pairs As String
End Type

' All of the database records.
Private Const DB_RECORDS_BUMP = 256
Private NumDbRecords As Long
Private MaxDbRecords As Long
Private DbRecords() As DbRecord

' Subscriber type.
Private Type DbSubscriber
    Connection As Object
    KeysRequested() As Boolean
    FieldLimiters() As String
    KeysLimited As Boolean
    FieldsLimited As Boolean
    NotifyCode As Long
End Type

' List of subscribers connections.
Private Const MAX_SUBSCRIBERS = 1024
Private NumSubscribers As Long
Private Subscribers(MAX_SUBSCRIBERS) As DbSubscriber

'/////////////////////////////////////////////////////////
' Startup and shutdown.
'/////////////////////////////////////////////////////////

'
' Public initialize function, called by DbManager.
'
Public Sub Startup( _
    ByVal ThisName As String, _
    ByVal ThisDescription As String, _
    ByVal ThisReadAccess As Long, _
    ByVal ThisWriteAccess As Long, _
    ByVal ThisFieldKeys As String, _
    ByVal ThisFieldTypes As String, _
    ByVal ThisSingleRecord As Boolean, _
    ThisKeeper As Gatekeeper)

    ' Remember objects.
    Set Keeper = ThisKeeper

    ' Remember the parameters passed to us.
    DbName = ThisName
    DbDescription = ThisDescription
    DbReadAccess = ThisReadAccess
    DbWriteAccess = ThisWriteAccess
    DbFieldKeys = ThisFieldKeys
    DbFieldTypes = ThisFieldTypes
    
    ' See if this is a key-value-type database.
    DbSingleRecord = ThisSingleRecord

    ' Derive the per-field information.
    Dim TempKeys As String, TempTypes As String, i As Integer
    TempKeys = DbFieldKeys
    While NextSTRING(TempKeys, ";") <> ""
        NumFields = NumFields + 1
    Wend
    
    ' Redim the arrays.
    ReDim FieldTypes(NumFields)
    ReDim FieldKeys(NumFields)
    ReDim UCaseFieldKeys(NumFields)
    ReDim TempValues(NumFields)
    ReDim TempOriginalValues(NumFields)
    ReDim TempChanged(NumFields)
    
    ' Get all field names
    TempKeys = DbFieldKeys
    TempTypes = DbFieldTypes
    For i = 0 To NumFields - 1
        FieldKeys(i) = NextSTRING(TempKeys, ";")
        UCaseFieldKeys(i) = UCase(FieldKeys(i))
        FieldTypes(i) = NextSTRING(TempTypes, ";")
    Next
End Sub

'
' Shut down this class.
'
Public Sub Shutdown()
    Dim i As Integer

    ' Release all connections silently.
    For i = 0 To NumSubscribers - 1
    
        ' Call the connection unsubscription function.
        ' Note that this will result in a call to Me.Unsubscribe,
        ' which will do the dirty work.
        
        Call Subscribers(i).Connection.Unsubscribe( _
            Subscribers(i).NotifyCode, False)
    Next
    
    ' Release held objects.
    Set Keeper = Nothing
End Sub

'/////////////////////////////////////////////////////////
' Public single-record database functions.
'/////////////////////////////////////////////////////////

'
' Get a value from this single-record database.
'
Public Function GetSingleValue(ByVal Key As String) As String

    ' Validate state.
    If Not DbSingleRecord Then Keeper.AppError "GetSingleValue: " & DbName & " is not single valued"
    If NumDbRecords <> 1 Then Keeper.AppError "GetSingleValue: " & DbName & " contains " & NumDbRecords & " records."
  
    ' Default to blank.
    GetSingleValue = ""
    
    ' Find key.
    Key = UCase(Key)
    Dim Temp As String
    Dim TempKey As String, TempValue As String
    Temp = DbRecords(0).Pairs
    
    ' Check all key=value pairs.
    While Temp <> ""
        TempKey = UCase(NextSTRING(Temp, "="))
        TempValue = NextSTRING(Temp, " ")
        SkipSpaces (Temp)
        
        ' Check for match.
        If TempKey = Key Then
        
            ' Found it!
            GetSingleValue = TempValue
            Exit Function
        End If
    Wend
End Function

'
' Set a value in this single-record database.
'
Public Function SetSingleValue(ByVal Key As String, ByRef Value As String)

    ' Validate state.
    If Not DbSingleRecord Then Keeper.AppError "SetSingleValue: " & DbName & " is not single valued"
    If NumDbRecords <> 1 Then Keeper.AppError "SetSingleValue: " & DbName & " contains " & NumDbRecords & " records."

    ' Change it and notify subscribers.
    Call NoteDbEdit(DbName, Key & "=" & Quotes(Value))
End Function

'
' Attempt to set all of the named key/value pairs
' in the current database.  Either returns True with success,
' or returns False for failure.  In the case of failure, an
' appropriate error message will be sent.
'
Public Function AttemptSet(Connection As Object, _
    ByVal Pairs As String) As Boolean
    
    ' Locals.
    Dim Pair As String, TempPairs As String
    Dim TempKey As String, TempValue As String
    Dim i As Integer
    Dim RegularFieldCount As Integer
    Dim KeyfieldValue As String
    
    ' Default to failure.
    AttemptSet = False
    
    If DbSingleRecord Then
    
        ' Here, the keyfield value is the database's name.
        KeyfieldValue = DbName
    Else
        
        ' Get the keyfield value.
        KeyfieldValue = NextSTRING(Pairs)
        If Not IsValidName(KeyfieldValue) Then
            Call Connection.Respond(GK_Failure_DbBadKeyfield, "", "Missing or invalid keyfield.")
            Exit Function
        End If
    End If
    
    ' Validate all key=value pairs.
    TempPairs = Pairs
    While TempPairs <> ""
        TempKey = UCase(NextSTRING(TempPairs, "="))
        TempValue = NextSTRING(TempPairs, " ")
        SkipSpaces (TempPairs)

        ' Validate this key.
        For i = 1 To NumFields - 1
            If TempKey = UCaseFieldKeys(i) Then
            
                ' Found this key.
                If Not IsValidType(TempValue, FieldTypes(i)) Then
                    
                    ' Type mismatch.
                    Call Connection.Respond(GK_Failure_DbTypeMismatch, "", "Type mismatch in key " & UCaseFieldKeys(i))
                    Exit Function
                End If
                
                RegularFieldCount = RegularFieldCount + 1
                
                ' Skip other keys.
                GoTo NextPair
            End If
        Next

        ' Unrecognized field.
        Call Connection.Respond(GK_Failure_DbNonexistantField, "", "Nonexistant field " & TempKey)
        Exit Function

NextPair:
    Wend
    
    ' Set the values and notify subscribers of any changes.
    Call NoteDbEdit(KeyfieldValue, Pairs)
    
    ' Success.
    AttemptSet = True
End Function

'/////////////////////////////////////////////////////////
' Public change functions, called by database specific code.
'/////////////////////////////////////////////////////////

'
' Note that a database record has been added or modified.
' Any code which adds or modifies a database record is
' responsible for calling this function afterwards.
'
' Record is identified by its index.
'
Public Sub NoteDbEditIndex(Index As Long, _
    FieldData As String, IsNew As Boolean)
    
    ' Locals.
    Dim DeltaFields As String, NewDeltaFields As String
    Dim Key As String
    Dim i As Long
    
    ' Get key.
    Key = DbRecords(Index).Key
    
    ' Build delta between previous and current data, and
    ' see if there are any differences.
    If MergeRecords(Key, _
        DbRecords(Index).Pairs, _
        FieldData, DeltaFields) Or _
        IsNew Then
        
        ' There are differences, so let's update this.
    
        ' Notify all subscribers of the modification.
        For i = 0 To NumSubscribers - 1
        
            ' Send it if it's not field-limited now.
            If Not TempIsFieldLimited(Subscribers(i)) Then

                ' Get just the requested fields.
                NewDeltaFields = GetRequestedTempDeltas(Subscribers(i))

                ' Send them.
                Call Subscribers(i).Connection.Notify( _
                    Subscribers(i).NotifyCode, _
                    "+" & Key & " " & NewDeltaFields, _
                    "")
            Else
            
                ' This is now field limited. So we must
                ' determine whether it was field limited
                ' before the update.
                If Not TempWasFieldLimited(Subscribers(i)) Then
            
                    ' This wasn't field-limited before, but it
                    ' is now, so we have to tell the client to
                    ' delete it:
                    Call Subscribers(i).Connection.Notify( _
                        Subscribers(i).NotifyCode, _
                        "-" & Key, "")
                End If
            End If
        Next
    End If
End Sub

'
' Note that a database record has been added or modified.
' Any code which adds or modifies a database record is
' responsible for calling this function afterwards.
'
' Record is identified by its key.
'
Public Sub NoteDbEdit(Key As String, FieldData As String)
    Dim Index As Long
    Dim IsNew As Boolean
    
    ' Get index.
    Index = GetIndexByKey(Key)
    If Index < 0 Then
    
        ' Create a new index.
        Index = NewDbIndex(Key)
        IsNew = True
    End If
    
    ' Note that the database index was changed.
    Call NoteDbEditIndex(Index, FieldData, IsNew)
End Sub

'
' Note that a database record has been deleted.
' Any code which deletes a database record is responsible
' for calling this function. Returns True if success,
' False if not found.
'
Public Function NoteDbDelete(Key As String) As Boolean
    Dim Index As Long, i As Long
    
    ' Find the index of this key.
    Index = GetIndexByKey(Key)
    If Index >= 0 Then
    
        ' Remember this field, for field limiter checking.
        InitTemp Key
        Call AddToTemp(DbRecords(Index).Pairs, True)
        
        ' Remove this from our internal table.
        Call DeleteDbIndex(Index)
        
        ' Notify all subscribers of key deletion.
        For i = 0 To NumSubscribers - 1
            ' Send notification unless we were field limited.
            'xxx
            If Not TempWasFieldLimited(Subscribers(i)) Then
                Call Subscribers(i).Connection.Notify( _
                    Subscribers(i).NotifyCode, "-" & Key, "")
            End If
        Next
    Else
        
        ' Indicates a problem with the caller.
    End If
End Function

'/////////////////////////////////////////////////////////
' Subscription functions.
'/////////////////////////////////////////////////////////

'
' Subscribe a connection to this database, using
' the specified field mask and subscriber code.
' Note that multiple subscriptions from a single
' subscriber are ok.
'
' Returns the subscription's GK_DbNotify code if the
' subscription occurred successfully, a negative number
' if it failed.
'
' Sends success or failure code to the remote.
'
' Called by: DbManager (NOT Connections).
'
Public Function Subscribe(Connection As Object, _
    KeysRequested As String, FieldLimiters As String) As Long
    
    ' Get index of where we expect to add this subscriber.
    Dim Index As Integer
    Index = NumSubscribers
    
    ' Make sure there's room in the subscriber table.
    If Index >= MAX_SUBSCRIBERS Then
    
        ' Subscription table is full.
        Call Connection.Respond(GK_Failure_DbSubscribeMaxedOut, "", "Database is oversubscribed.")
        Subscribe = -1
        Exit Function
    End If
    
    ' Set subscription filters.
    If Not SetSubscriberFilters(Connection, Subscribers(Index), _
        KeysRequested, FieldLimiters) Then
        
        ' Bad filters; error has already been sent.
        Exit Function
    End If
    
    ' Get subscription code from the connection.
    Subscribe = Connection.NewSubscriptionCode(Me)
    
    ' Handle connection subscriptions maxed out.
    If Subscribe < 0 Then
        Call Connection.Respond(GK_Failure_DbSubscribeMaxedOut, "", "Database is oversubscribed.")
        Exit Function
    End If

    ' Add subscriber to table.
    NumSubscribers = NumSubscribers + 1
    
    ' Init the subscriber's info.
    Call InitSubscriberInfo(Subscribers(Index), _
        Connection, Subscribe)

    ' Send success code so that the client knows the data
    ' notify code.
    Call Connection.Respond(GK_Success_DbSubscribed, Trim(Str(Subscribe)), "Subscribed.")

    ' Send all initial records.
    Call SendAll(Subscribers(Index))
End Function

'
' Unsubscribe a connection from this database with
' the specified subscriber code.  Returns True if
' unsubscribed, False if this connection was not
' subscribed to begin with.  The later is not an
' error condition.
'
' Call with Send=True to notify the connection of the
' unsubscription, or Send=False to send nothing.  If
' the unsubscription is occuring because of a disconnect,
' Send must be False.
'
' Called by: Connections (NOT the DbManager).
'
Public Function Unsubscribe(Connection As Object, _
    NotifyCode As Long, Send As Boolean) As Boolean

    ' See if this Connection and NotifyCode exists in
    ' this database's subscription list:
    Dim Index As Long
    For Index = 0 To NumSubscribers - 1
    
        ' See if it matches.
        If Subscribers(Index).NotifyCode = NotifyCode And _
            Subscribers(Index).Connection.Tag = Connection.Tag Then
            
            ' Found it!
            GoTo Found
        End If
    Next
    
    ' This was not found.
    Unsubscribe = False
    Exit Function
    
Found:
    ' Found this subscription at position Index.
    Unsubscribe = True

    ' Remove this subscriber from the list.
    RemoveSubscriberIndex (Index)

    ' Notify client of the unsubscription.
    If Send Then
        Call Connection.Respond(GK_Notify_DbUnsubscribed, _
            Trim(Str(NotifyCode)), "Unsubscribed.")
    End If
End Function

'/////////////////////////////////////////////////////////
' Sending functions, called by database manager.
'/////////////////////////////////////////////////////////

'
' Send all records to a connection, using the specified
' code.
'
Private Sub SendAll(Subscriber As DbSubscriber)
    
    ' Locals.
    Dim Original As String, Delta As String

    ' Send all.
    Dim i As Long
    For i = 0 To NumDbRecords - 1
    
        ' Get rid of empty properties.
        Original = DbRecords(i).Pairs
        Call MergeRecords(DbRecords(i).Key, "", Original, Delta)
        
        ' Skip it if it's field-limited.
        If Not TempIsFieldLimited(Subscriber) Then

            ' Grab only the desired fields.
            Delta = GetRequestedTempDeltas(Subscriber)

            ' Send it if it's not blank.
            If Delta <> "" Or Subscriber.KeysRequested(0) Then
                Call Subscriber.Connection.Respond( _
                    Subscriber.NotifyCode, "+" & _
                    DbRecords(i).Key & " " & Delta, _
                    "")
            End If
        End If
    Next
End Sub

'
' Send all records meeting the specified criteria.
' Returns True if records were successfully sent, or
' False if there was an error in the filters.  In the
' case of an error, the message has already been sent.
'
Public Function SendAllRecords(Connection As Object, _
    KeysRequested As String, FieldLimiters As String, _
    ByVal Code As Long) As Boolean
    
    ' Create a temporary subscriber to perform
    ' key and field limiting.
    Dim TempSubscriber As DbSubscriber
    
    ' Set the subscriber's filters.
    SendAllRecords = SetSubscriberFilters( _
        Connection, TempSubscriber, _
        KeysRequested, FieldLimiters)
        
    ' If subsciber filter parsing error, the error code has
    ' already been sent, so exit.
    If Not SendAllRecords Then Exit Function
    
    ' Init the subscriber's info.
    Call InitSubscriberInfo(TempSubscriber, _
        Connection, Code)

    ' Send the results.
    Call SendAll(TempSubscriber)
End Function

'/////////////////////////////////////////////////////////
' Private database functions.
'/////////////////////////////////////////////////////////

'
' Find a named key.  Returns index into table if found,
' or a negative number if not found.
'
Public Function GetIndexByKey(Key As String) As Long
    Dim i As Long
    
    ' See if it exists.
    For i = 0 To NumDbRecords - 1
        If StrComp(DbRecords(i).Key, Key, 1) = 0 Then
        
            ' Found it!
            GetIndexByKey = i
            Exit Function
        End If
    Next
    
    ' Not found.
    GetIndexByKey = -1
End Function

'
' Delete a database index.
'
Private Function DeleteDbIndex(Index As Long)
    Dim i As Long

    ' Move all later records back.
    For i = Index + 1 To NumDbRecords - 1
        DbRecords(i - 1) = DbRecords(i)
    Next

    ' Decrease count.
    NumDbRecords = NumDbRecords - 1
End Function

'
' Create a new database record and return its index.
'
Private Function NewDbIndex(Key As String) As Long

    ' If we're about to overflow, redimension the database.
    If NumDbRecords >= MaxDbRecords Then
        MaxDbRecords = MaxDbRecords + DB_RECORDS_BUMP
        ReDim Preserve DbRecords(MaxDbRecords)
    End If

    ' Get new index.
    NewDbIndex = NumDbRecords
    
    ' Init record.
    DbRecords(NewDbIndex).Key = Key
    DbRecords(NewDbIndex).Pairs = ""
    
    ' Bump count.
    NumDbRecords = NumDbRecords + 1
End Function

'
' Return the number of records in the database.
'
Public Function NumRecords() As Long
    NumRecords = NumDbRecords
End Function

'
' Return the database key corresponding to a certain
' index.
'
Public Function GetKeyByIndex(Index As Long)
    GetKeyByIndex = DbRecords(Index).Key
End Function

'
' Return the database value corresponding to a certain
' index and key.
'
Public Function GetValueByIndexAndKey(Index As Long, _
    Key As String)
    
    Dim Pairs As String, TempKey As String, TempValue As String
    Pairs = DbRecords(Index).Pairs
    
    While Pairs <> ""
        TempKey = NextSTRING(Pairs, "=")
        TempValue = NextSTRING(Pairs, " ")
        SkipSpaces (Pairs)
        
        If StrComp(TempKey, Key, 1) = 0 Then
            GetValueByIndexAndKey = TempValue
            Exit Function
        End If
    Wend
End Function

'/////////////////////////////////////////////////////////
' Private subscriber functions.
'/////////////////////////////////////////////////////////

'
' Validate and set all of the KeysRequested and FieldLimiters
' for a subscriber. Returns True if they were successfully
' set, or False if one or more were in error.
'
Private Function SetSubscriberFilters( _
    Connection As Object, _
    ByRef Subscriber As DbSubscriber, _
    KeysRequested As String, _
    FieldLimiters As String) As Boolean
    
    ' Default to failure.
    SetSubscriberFilters = False
    
    ' Locals.
    Dim TempList As String
    Dim LimitKey As String, LimitValue As String
    Dim i As Integer
    
    ' Dimension the request and limiters arrays.
    ReDim Subscriber.KeysRequested(NumFields)
    ReDim Subscriber.FieldLimiters(NumFields)
    
    ' Init FieldLimiters.
    For i = 0 To NumFields
        Subscriber.FieldLimiters(i) = "*"
    Next

    ' Build FieldLimiters.
    Subscriber.FieldsLimited = (FieldLimiters <> "")
    If Subscriber.FieldsLimited Then
    
        ' Fill in all field limiters.
        TempList = FieldLimiters
        While TempList <> ""
        
            ' Break limiter up into key and value.
            LimitKey = UCase(NextSTRING(TempList, "="))
            LimitValue = NextSTRING(TempList, " ")
            SkipSpaces (TempList)
            
            ' Add limiter to table if not meaningless.
            If LimitKey <> "" Then
            
                ' See if this limits a valid field.
                For i = 0 To NumFields - 1
                
                    ' Check for match.
                    If UCaseFieldKeys(i) = LimitKey Then
                                        
                        ' This field was requested.
                        Subscriber.FieldLimiters(i) = UCase(LimitValue)
                        GoTo NextLimiter
                    End If
                Next
            End If
            
            ' The specified limiter was not found.
            Call Connection.Respond(GK_Failure_DbLimiterKeyUnrecognized, "", "Unrecognized field limiter " & LimitKey & ".")
            Exit Function

NextLimiter:
        ' Process the next limiter.
        Wend
    End If
    
    ' Build KeysRequested.
    Subscriber.KeysLimited = KeysRequested <> ""
    If Subscriber.KeysLimited Then
    
        ' Want specific keys.
        TempList = KeysRequested
        While TempList <> ""
    
            ' Get key.
            Dim Temp As String
            Temp = UCase(NextSTRING(TempList, ";"))
            
            If Temp = "*" Then
            
                ' Enable all fields.
                For i = 0 To NumFields - 1
                    Subscriber.KeysRequested(i) = True
                Next
                GoTo NextRequest
            End If
            
            ' See if this is a valid field.
            For i = 0 To NumFields - 1
                            
                If Temp = UCaseFieldKeys(i) Then
                                    
                    ' This field was requested.
                    Subscriber.KeysRequested(i) = True
                    GoTo NextRequest
                End If
            Next

            ' The specified request field was not found.
            Call Connection.Respond(GK_Failure_DbRequestKeyUnrecognized, "", "Unrecognized field request " & Temp & ".")
            Exit Function

NextRequest:
        ' Process the next requested field.
        Wend
    Else
        
        ' Want all keys.
        For i = 0 To NumFields
            Subscriber.KeysRequested(i) = True
        Next
    End If
    
    ' Return success.
    SetSubscriberFilters = True
End Function

'
' Initialize the information about a database
' subscriber.  The subscriber may be permanent and
' correspond to an entry in the Subscribers table,
' or it may have been created temporarily in order
' to service a Get call.
'
Private Sub InitSubscriberInfo( _
    ByRef Subscriber As DbSubscriber, _
    Connection As Object, _
    NotifyCode As Long)
    
    ' Add this.
    Set Subscriber.Connection = Connection
    Subscriber.NotifyCode = NotifyCode
End Sub

'
' Remove a subscriber from the subscriber list at a certain
' index position.
'
Private Sub RemoveSubscriberIndex(Index As Long)
    Dim i As Long

    ' Remove this index from the subscriber list.
    For i = Index + 1 To NumSubscribers - 1
        Subscribers(i - 1) = Subscribers(i)
    Next
    
    ' Release reference to deleted connection object.
    Set Subscribers(NumSubscribers - 1).Connection = Nothing
    
    ' Shrink list.
    NumSubscribers = NumSubscribers - 1
End Sub

'/////////////////////////////////////////////////////////
' Database summary information.
'/////////////////////////////////////////////////////////

'
' Return the database summary fields as a string.
'
Public Function GetDbSummaryFields() As String
    GetDbSummaryFields = _
        "Description=" & Quotes(DbDescription) & _
        " ReadAccess=" & DbReadAccess & _
        " WriteAccess=" & DbWriteAccess & _
        " FieldKeys=" & Quotes(DbFieldKeys) & _
        " FieldTypes=" & Quotes(DbFieldTypes) & _
        " SingleRecord=" & IIf(DbSingleRecord, "1", "0")
End Function

'/////////////////////////////////////////////////////////
' Database utility functions.
'/////////////////////////////////////////////////////////

'
' Init the temporary value table.
'
Private Sub InitTemp(KeyName As String)
    Dim i As Long
    
    ' Init all values and changed entries.
    For i = 0 To NumFields
    
        ' Init to blank according to its datatype.
        If FieldTypes(i) = "I" Or _
            FieldTypes(i) = "B" Then
            TempValues(i) = "0"
            TempOriginalValues(i) = "0"
        Else
            TempValues(i) = ""
            TempOriginalValues(i) = ""
        End If
        
        ' Note that it hasn't changed.
        TempChanged(i) = False
    Next
    
    ' Set the key.
    TempValues(0) = KeyName
    TempOriginalValues(0) = KeyName
End Sub

'
' Return whether the record in the temporary tables is
' relevant to the Subscriber based on the Subscriber's
' FieldLimiters.
'
Private Function TempIsFieldLimited(ByRef Subscriber As DbSubscriber)
    Dim i As Integer

    ' If there are no field limiters, we're not field limited.
    TempIsFieldLimited = Subscriber.FieldsLimited
    
    ' Check all field limiters.
    If TempIsFieldLimited Then
    
        ' Check for match with all field limiters.
        For i = 0 To NumFields - 1
            If Subscriber.FieldLimiters(i) <> "*" And _
            UCase(Subscriber.FieldLimiters(i)) <> UCase(TempValues(i)) Then
            
                ' This field is not wanted.
                TempIsFieldLimited = True
                Exit Function
            End If
        Next
        
        ' We passed all of the field limiter criteria.
        TempIsFieldLimited = False
    End If
End Function

'
' Return whether the record in the temporary tables was
' field-limited before it was changed.
'
Private Function TempWasFieldLimited(ByRef Subscriber As DbSubscriber)
    Dim i As Integer

    ' If there are no field limiters, we were not field limited.
    TempWasFieldLimited = Subscriber.FieldsLimited
    
    ' Check all field limiters.
    If TempWasFieldLimited Then
    
        ' Check for match with all field limiters.
        For i = 0 To NumFields - 1
            If Subscriber.FieldLimiters(i) <> "*" And _
            UCase(Subscriber.FieldLimiters(i)) <> UCase(TempOriginalValues(i)) Then

                ' This field was not wanted.
                TempWasFieldLimited = True
                Exit Function
            End If
        Next
        
        ' We passed all of the field limiter criteria.
        TempWasFieldLimited = False
    End If
End Function

'
' Return the key/value pairs in the temporary tables
' which are listed in KeysRequested and whose TempChanged record
' is set.
'
' Returns them in the order specified by KeysRequested, and if duplicates
' appear in KeysRequested, duplicates will appear in the resulting string.
'
' If KeysRequested is blank, returns all changed fields.
'
Private Function GetRequestedTempDeltas(ByRef Subscriber As DbSubscriber) As String
    Dim i As Integer
    
    ' Add all requested keys to the result.
    For i = 1 To NumFields - 1
    
        ' See if this key is requested.
        If Subscriber.KeysRequested(i) And _
            TempChanged(i) Then
            ' This key is requested.
            
            ' Add a separating space.
            If GetRequestedTempDeltas <> "" Then
                GetRequestedTempDeltas = GetRequestedTempDeltas & " "
            End If
                
            ' Add the key/value pair.
            GetRequestedTempDeltas = GetRequestedTempDeltas & _
                FieldKeys(i) & "=" & Quotes(TempValues(i))
        End If
    Next
End Function

'
' Add all of the key/value pairs in Record to the
' temporary value table, updating TempChanged.
'
' Returns whether any changes were made.
'
Private Function AddToTemp(ByVal Record As String, _
    Original As Boolean) As Boolean

    ' Locals.
    Dim TempKey As String, TempValue As String
    Dim i As Integer
    
    ' Default to no changes.
    AddToTemp = False
    
    ' Merge all Record key/value pairs in, counting and noting changes.
    ' Process each space-separated key=value pair.
    While Record <> ""
        
        ' Get key and value.
        TempKey = UCase(NextSTRING(Record, "="))
        TempValue = NextSTRING(Record, " ")
        SkipSpaces (Record)
        
        ' See if this key exists in the database.
        For i = 0 To NumFields - 1
            If UCaseFieldKeys(i) = TempKey Then

                ' Update this existing key if it has changed.
                If TempValues(i) <> TempValue Then
                
                    ' Validate this record according to its type.
                    If IsValidType(TempValue, FieldTypes(i)) Then
                        TempChanged(i) = Not Original
                        TempValues(i) = TempValue
                        AddToTemp = True
                        
                        ' Add to OriginalValues if first pass.
                        If Original Then
                            TempOriginalValues(i) = TempValue
                        End If
                    End If
                End If
                
                ' Process next key/value pair.
                GoTo NextPair
            End If
        Next
NextPair:
    Wend
End Function

'
' Attempt to merge all of the keys and values from Update
' into Original.  Upon return:
'
' - Original has been updated to contain the result of
'     applying Update to it.
'
' - Update is unaffected.
'
' - Delta contains key/value pairs for only the keys
'     whose values have changed in Original.
'
' Returns True if Update is non-blank, i.e. one or more
' values have been changed, False otherwise.
'
' Field names are case insensitive.
'
' Handles Original being garbled or maliciously designed.
' Handles Update being garbled or maliciously designed.
'
Public Function MergeRecords( _
    ByRef Key As String, _
    ByRef Original As String, _
    ByVal Update As String, _
    ByRef Delta As String) As Boolean
    
    ' Locals.
    Dim i As Long
    
    ' Init temporary tables.
    InitTemp (Key)

    ' Add all Original key/value pairs to table.
    Call AddToTemp(Original, True)
    
    ' Merge all updated records in.
    MergeRecords = AddToTemp(Update, False)

    ' Rebuild Original and Update from the table.
    Original = ""
    Delta = ""
    For i = 1 To NumFields - 1
        
        ' Add to Original.
        If Original <> "" Then Original = Original & " "
        Original = Original & FieldKeys(i) & "=" & Quotes(TempValues(i))
        
        ' Add to Delta if changed.
        If TempChanged(i) Then
            If Delta <> "" Then Delta = Delta & " "
            Delta = Delta & FieldKeys(i) & "=" & Quotes(TempValues(i))
        End If
    Next i
End Function

'/////////////////////////////////////////////////////////
' Parameter substitution.
'/////////////////////////////////////////////////////////

'
' Perform parameter substitution.  For example, if
' the database record at position Index is:
'
'    Animal=Fish IQ=100
'
' And Parms is:
'
'    -dosomething -genus %Animal% -smartness %IQ%
'
' Then the result is:
'
'    -dosometing -genus Fish - smartness 100
'
' In the resulting text, quotes are placed around any
' substituted parameters who textually require them.
'
' Assumes Index is valid.
'
Public Function SubstituteParms(Index As Long, _
    ByVal Parms As String) As String
    
    ' Locals.
    Dim Temp As String, Key As String
    Dim i As Integer
    
    ' Add stuff to temp.
    InitTemp (True)
    TempValues(0) = DbRecords(Index).Key
    Call AddToTemp(DbRecords(Index).Pairs, True)

    ' Go through Parms and perform any substitution.
    While InStr(Parms, "%") <> 0
        SubstituteParms = SubstituteParms & Left(Parms, InStr(Parms, "%") - 1)
        Parms = Mid(Parms, InStr(Parms, "%") + 1)
        If InStr(Parms, "%") = 0 Then
            Parms = ""
        Else
            Key = UCase(Left(Parms, InStr(Parms, "%") - 1))
            Parms = Mid(Parms, InStr(Parms, "%") + 1)
            For i = 0 To NumFields - 1
                If Key = UCaseFieldKeys(i) Then
                    SubstituteParms = SubstituteParms & TempValues(i)
                End If
            Next
        End If
    Wend
End Function

'/////////////////////////////////////////////////////////
' Saving.
'/////////////////////////////////////////////////////////

'
' Save the contents of this database to a specified file.
'
Public Sub SaveToFile(FileNum As Long)
    Dim i As Integer
    
    ' Handle the appropriate database type.
    If DbSingleRecord Then
    
        ' Save a single record in executable form.
        Print #FileNum, _
            "Set " & DbName & " " & _
            DbRecords(i).Pairs
    Else
    
        ' Save multiple records in executable form.
        For i = 0 To NumDbRecords - 1
    
            ' Save it.
            Print #FileNum, _
                "Set " & DbName & " " & _
                DbRecords(i).Key & " " & _
                DbRecords(i).Pairs
        Next
    End If
End Sub

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
