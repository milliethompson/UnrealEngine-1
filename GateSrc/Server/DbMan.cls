VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
END
Attribute VB_Name = "DbManager"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
'/////////////////////////////////////////////////////////
' DbMan.cls: Gatekeeper server database manager class.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   DbManager is responsible for managing all of the
'   databases on the Gatekeeper server which clients may
'   want to locally replicate.  DbManager is responsible
'   for creating, deleting, and managing DbHook objects.
'   Each DbHook object is responsible for tracking changes
'   to one database and notifying clients of changes to
'   that one database.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Private variables.
'/////////////////////////////////////////////////////////

' Array of database hooks.
Private Const MAX_DB_HOOKS = 64
Private NumDbHooks As Long
Private DbHooks(MAX_DB_HOOKS) As DbHook

' Hook for 'Databases' database, the list of all databases.
Dim Databases As DbHook

' True after class has been initialized.
Dim StartedUp As Boolean

' Controlling Gatekeeper.
Private Keeper As Gatekeeper

'/////////////////////////////////////////////////////////
' Startup and shutdown.
'/////////////////////////////////////////////////////////

'
' Class initialization.
'
Public Sub Startup(ThisKeeper As Gatekeeper)

    ' Remember objects.
    Set Keeper = ThisKeeper

    ' Create hook for 'Databases', the list of all
    ' databases.
    Set Databases = NewDbHook("Databases", _
        "List of all available Gatekeeper databases", _
        GKACCESS_Info, GKACCESS_None, _
        "Database;Description;ReadAccess;WriteAccess;FieldKeys;FieldTypes;SingleRecord", _
        "N;S;I;I;S;S;S;S;B", _
        False)

    ' Here we have to add DatabasesHook to itself, since
    ' it wasn't set because we haven't startup up.
    Call Databases.NoteDbEdit("Databases", Databases.GetDbSummaryFields)

    ' Note that we've started up.
    StartedUp = True
End Sub

'
' Class termination.
'
Public Sub Shutdown()

    ' Unregister hook for 'Databases'.
    DeleteDbHook Databases
    Set Databases = Nothing
    
    ' Release objects.
    Set Keeper = Nothing
End Sub

'/////////////////////////////////////////////////////////
' Public functions.
'/////////////////////////////////////////////////////////

'
' Create and register a new database hook for
' a specified database name.
'
Function NewDbHook(DbName As String, _
    DbDescription As String, _
    DbReadAccess As Long, _
    DbWriteAccess As Long, _
    DbFieldKeys As String, _
    DbFieldTypes As String, _
    DbSingleRecord) As DbHook
    
    ' Balk if we're out of records.
    If NumDbHooks >= MAX_DB_HOOKS Then
        Keeper.AppError "Too many databases: " & NumDbHooks
    End If
    
    ' Balk if this is a duplicate.
    Dim i As Long
    For i = 0 To NumDbHooks - 1
        If UCase(DbHooks(i).DbName) = UCase(DbName) Then
            Keeper.AppError "Duplicate database: " & DbName
        End If
    Next
    
    ' Create a new hook.
    Set NewDbHook = New DbHook
    
    ' Set all of the hook's properties.
    Call NewDbHook.Startup(DbName, DbDescription, _
        DbReadAccess, DbWriteAccess, _
        DbFieldKeys, DbFieldTypes, _
        DbSingleRecord, Keeper)
    
    ' Add new hook to array.
    Set DbHooks(NumDbHooks) = NewDbHook
    NumDbHooks = NumDbHooks + 1

    ' Add this to the list of all databases.
    If StartedUp Then
        Call Databases.NoteDbEdit(DbName, NewDbHook.GetDbSummaryFields)
    End If

    ' Success, so the Hook is returned!
End Function

'
' Delete a database hook.
'
Public Sub DeleteDbHook(Db As DbHook)
    Dim i As Long, j As Long
    
    ' Find the hook and delete it.
    For i = 0 To NumDbHooks - 1
        If UCase(DbHooks(i).DbName) = UCase(Db.DbName) Then
        
            ' Remove this from the list of all databases.
            Call Databases.NoteDbDelete(Db.DbName)
        
            ' Delete.
            DbHooks(i).Shutdown
            Set DbHooks(i) = Nothing
        Else
        
            ' Reshuffle list without deleting anything.
            If i <> j Then
                Set DbHooks(j) = DbHooks(i)
                Set DbHooks(i) = Nothing
            End If
            j = j + 1
        End If
    Next

    ' Make sure we deleted one item.
    If j <> (NumDbHooks - 1) Then
        Keeper.AppError "Database not found to delete: " & Db.DbName
    End If
    
    ' Shorten the list.
    NumDbHooks = j
End Sub

'/////////////////////////////////////////////////////////
' Client related function.
'/////////////////////////////////////////////////////////

'
' Execute a command if it's database related.
' Returns True if the command was recongized and
' execution was attempted, False if we're not responsible
' for the commmand here.
'
Public Function Exec(CmdLine As String, _
    Connection As Object) As Boolean
    
    ' Locals.
    Dim KeysRequested As String
    Dim FieldLimiters As String
    Dim Hook As DbHook
    Dim Code As Long
    Dim Temp As String, TempList As String
    Dim TempDb As String, TempFilename As String
    
    ' Default to executing it.
    Exec = True
    
    If GetCMD(CmdLine, "GET") Then
        ' Get a database's current contents without subscribing.
        
        ' Get database name.
        If GetDBNAME(CmdLine, Hook, _
            KeysRequested, FieldLimiters, _
            Connection, True, False) Then
            
            ' Make sure we have access.
            If Connection.CheckAccess(Hook.DbReadAccess) Then
            
                ' Send the appropriate records. If this
                ' fails, it will send the appropriate
                ' error message.
                If Hook.SendAllRecords(Connection, KeysRequested, _
                    FieldLimiters, GK_DbNotify) Then
                    
                    ' Send success message.
                    Call Connection.Respond(GK_Success, "", _
                        "End of database.")
                End If
            End If
        End If
        
    ElseIf GetCMD(CmdLine, "SET") Then
        ' Set the database's contents.
    
        ' Get database name.
        If GetDBNAME(CmdLine, Hook, _
            KeysRequested, FieldLimiters, _
            Connection, False, True) Then
            
            If Hook.AttemptSet(Connection, FieldLimiters) Then
                Call Connection.Respond(GK_Success, "", _
                    "Records set.")
            End If
        End If
    
    ElseIf GetCMD(CmdLine, "SUB") Then
        
        ' Subscribe to a database.
        
        ' Get database name.
        If GetDBNAME(CmdLine, Hook, _
            KeysRequested, FieldLimiters, _
            Connection, True, False) Then
            
            ' Make sure we have access.
            If Connection.CheckAccess(Hook.DbReadAccess) Then
                
                ' Subscribe us.
                Call Hook.Subscribe(Connection, _
                    KeysRequested, FieldLimiters)
            End If
        End If
        
    ElseIf GetCMD(CmdLine, "SAVE") Then
    
        ' Save databases to a file.
        If Connection.CheckAccess(GKACCESS_Admin) Then
        
            ' Get list of databases.
            TempList = NextSTRING(CmdLine)
            
            ' Get filename.
            TempFilename = NextSTRING(CmdLine)
            
            ' Verify parms.
            If TempList <> "" And TempFilename <> "" Then
            
                ' Verify database list.
                Temp = TempList
                While Temp <> ""
                    TempDb = NextSTRING(Temp, ";")
                    
                    If Not GetDBNAME(TempDb, Hook, _
                        KeysRequested, FieldLimiters, _
                        Connection, False, False) Then
                        
                        ' Already sent error message.
                        Exit Function
                    End If
                Wend
                
                ' Save the stuff.
                If SaveDatabases(Connection, _
                    TempList, TempFilename) Then
                    
                    ' Successfully saved.
                    Call Connection.Respond(GK_Success, "", "Saved.")
                Else
                    
                    ' Save failed.
                End If
            Else
                ' Didn't get database list or filename.
                Call Connection.Respond(GK_Failure_BadParameters, "", "Missing database list or filename.")
            End If
        End If
    ElseIf GetCMD(CmdLine, "UNSUB") Then

        If GetCMD(CmdLine, "*") Then
            
            ' Unsubscribe this connection from all databases.
            Connection.UnsubscribeAll (True)
            
            ' This operation can't fail.
            Call Connection.Respond(GK_Success, "", "Unsubscribed all databases.")
        ElseIf NextNUMBER(CmdLine, Code) Then

            ' Unsubscribe from a database.
            If Connection.Unsubscribe(Code, True) Then
            
                ' Successfully unsubscribed.
                Call Connection.Respond(GK_Success, "", "Unsubscribed.")
            Else
            
                ' We weren't subscribed.
                Call Connection.Respond(GK_Failure_DbNotSubscribed, "", "Not subscribed.")
            End If
        Else
        
            ' Didn't get code number.
            Call Connection.Respond(GK_Failure_BadParameters, "", "Missing subscription code.")
        End If
    
    Else
        
        ' Not a recognizable database command.
        Exec = False
    
    End If
End Function

'/////////////////////////////////////////////////////////
' Saving databases.
'/////////////////////////////////////////////////////////

'
' Save a list of databases to a file.
' Returns True if successful, False if not.
'
Public Function SaveDatabases(Connection As Object, _
    DatabaseList As String, Filename As String)

    ' Locals.
    Dim KeysRequested As String
    Dim FieldLimiters As String
    Dim Hook As DbHook
    Dim FileNum As Long
    Dim TempDb As String

    ' Open file.
    FileNum = FreeFile
    On Error GoTo BadFile
    Open Filename For Output As #FreeFile
    
    ' Save all databases.
    While DatabaseList <> ""
    
        ' Get database name.
        TempDb = NextSTRING(DatabaseList, ";")
        
        If GetDBNAME(TempDb, Hook, _
            KeysRequested, FieldLimiters, _
            Connection, False, False) Then
            
            ' Already sent error message.
            Hook.SaveToFile FileNum
        End If
    Wend
    
    ' Success.
    Close #FileNum
    SaveDatabases = True
    Exit Function

BadFile:
    ' Error opening file.
    Call Connection.Respond(GK_Failure_FileIO, "", "File error.")
End Function

'/////////////////////////////////////////////////////////
' Private functions.
'/////////////////////////////////////////////////////////

'
' Grab a database name from the command line.
' If it's a valid database name, sets the
' DbHook object to the corresponding database hook
' and returns True.  If invalid, sends an appropriate
' error message to the connection and returns False.
'
Private Function GetDBNAME( _
    ByRef CmdLine As String, _
    ByRef Hook As DbHook, _
    ByRef KeysRequested As String, _
    ByRef FieldLimiters As String, _
    Connection As Object, _
    AllowKeyRequests As Boolean, _
    ForWrite As Boolean) _
    As Boolean
    
    ' Default to failure.
    GetDBNAME = False
    
    ' Grab database name.
    Dim FullName As String, DbName As String
    FullName = NextSTRING(CmdLine)
    
    If FullName = "" Then
    
        ' Missing name.
        Call Connection.Respond(GK_Failure_BadParameters, "", "Missing database name.")
    Else
    
        ' Break up into database name and optional
        ' KeysRequested.
        DbName = NextSTRING(FullName, ":")
        KeysRequested = NextSTRING(FullName)
        
        ' See if we want KeysRequested.
        If KeysRequested <> "" And Not AllowKeyRequests Then
        
            ' Missing name.
            Call Connection.Respond(GK_Failure_BadParameters, "", "Field requests not allowed here.")
            Exit Function
        End If
        
        ' Get any field limiters.
        FieldLimiters = CmdLine

        ' Find the database in our list.
        Dim i As Long
        For i = 0 To NumDbHooks - 1
            If UCase(DbHooks(i).DbName) = UCase(DbName) Then
            
                ' Check access.
                If ForWrite Then
                
                    ' Check for write access.
                    If Not Connection.CheckAccess(DbHooks(i).DbWriteAccess) Then
                        Exit Function
                    End If
                Else
                
                    ' Check for read access.
                    If Not Connection.CheckAccess(DbHooks(i).DbReadAccess) Then
                        Exit Function
                    End If
                End If
            
                ' Found it.
                Set Hook = DbHooks(i)
                
                ' Return success.
                GetDBNAME = True
                Exit Function
            End If
        Next
        
        ' Send not-found error message.
        Call Connection.Respond(GK_Failure_DbNotFound, "", "Database not found.")
    End If
    
    ' Failed.
End Function

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
