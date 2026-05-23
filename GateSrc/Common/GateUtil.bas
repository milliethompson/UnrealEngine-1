Attribute VB_Name = "GateUtil"
'/////////////////////////////////////////////////////////
' GateUtil.frm: Misc Gatekeeper client/server utility
'               functions.
'
' Reference implementation Copyright 1996 Epic MegaGames, Inc.
' Code compiled in Visual Basic 4.0, Professional Edition.
'
' Purpose:
'   Various helper functions common to the Gatekeeper
'   client and server.  This file is shared by both projects.
'
' Revision history:
' * Created by Tim Sweeney
'/////////////////////////////////////////////////////////

Option Explicit

'/////////////////////////////////////////////////////////
' Gatekeeper information common between client and server.
'/////////////////////////////////////////////////////////

' Default Gatekeeper protocol port.
' todo: Change this to a real port, it's the telnet port now for testing.
Global Const DEFAULT_GATE_PORT = 23

'/////////////////////////////////////////////////////////
' Gatekeeper client implementation information.
' The following may differ between various implementations
' of Gatekeeper clients.
'/////////////////////////////////////////////////////////

Global Const GATECLIENT_VERSION = 10
Global Const GATECLIENT_APPLICATION_NAME = "EpicGateAdmin"
Global Const GATECLIENT_APPLICATION_VERSION = 10

'/////////////////////////////////////////////////////////
' Gatekeeper server protocol implementation information.
' The following may differ between various implementations
' of Gatekeeper servers.
'/////////////////////////////////////////////////////////

' Information about the protocol implementor.
Global Const GATESERVER_MAKER = "Reference implementation by Epic MegaGames, Inc."

' Welcome message.
Global Const GATESERVER_WELCOME_1 = "Early prototype version."

'/////////////////////////////////////////////////////////
' Gatekeeper server protocol information.  This is common to
' all Gatekeeper protocol implementations and it shouldn't
' be modified.
'/////////////////////////////////////////////////////////

' String identifying the Gatekeeper protocol; must never change.
Global Const GATESERVER_ID = "Gatekeeper"

' Current protocol version the server implements.  We assume
' that all clients with protocol versions greater than this number
' are backwards-compatible with us.  If a later client is not
' backwards compatible, it's the client's responsibility to
' recognize that and logout.
Global Const GATESERVER_PROTOCOL_VERSION = 10 ' 0.10

' Minimum protocol version we support. We give an error
' GK_Failure_ProtocolNotSupported to anyone who uses the
' CLIENT command with an earlier version of the protocol
' than this.
Global Const GATESERVER_PROTOCOL_MIN = 10 ' 0.10

' Default name for a new Gatekeeper.
Global Const GATESERVER_GATE_NAME = "Unnamed"

' Number of data notify codes.
Global Const GATESERVER_MAX_DATA_NOTIFYS = 199

' Maximum number of server connections.
Global Const GATESERVER_MAX_CONNECTIONS = 256

' Timeouts.
Global Const GATESERVER_LEVEL_STARTUP_TIMEOUT = 300
Global Const GATESERVER_CLIENT_IDLE_TIMEOUT = 300
Global Const GATESERVER_UPLINK_TIMEOUT = 25
Global Const GATESERVER_DOWNLINK_TIMEOUT = 300

' Numeric Gatekeeper server response codes, in the format XYY,
' where:
'    X=response category.
'    YY=specific code, or 00 for the generic response.
'
' Codes supported by Gatekeeper protocol version 0.1:
Global Const GK_Success = 100
Global Const GK_Success_DbSubscribed = 101
Global Const GK_Success_Login = 102
Global Const GK_Success_FileList = 103
Global Const GK_Success_Max = 199
Global Const GK_Failure = 200
Global Const GK_Failure_LoginUnrecongized = 210
Global Const GK_Failure_LoginAccountDisabled = 211
Global Const GK_Failure_LoginFull = 212
Global Const GK_Failure_LoginOnceOnly = 213
Global Const GK_Failure_LoginDuplicate = 213
Global Const GK_Failure_UnrecognizedCommand = 220
Global Const GK_Failure_NoAccess = 230
Global Const GK_Failure_BadParameters = 240
Global Const GK_Failure_NotFound = 250
Global Const GK_Failure_UserNotFound = 251
Global Const GK_Failure_LevelNotFound = 252
Global Const GK_Failure_WrongLevelState = 253
Global Const GK_Failure_InLevel = 254
Global Const GK_Failure_UserNotInLevel = 254
Global Const GK_Failure_LevelPassword = 255
Global Const GK_Failure_LevelFull = 256
Global Const GK_Failure_ProtocolNotSupported = 260
Global Const GK_Failure_DbNotFound = 270
Global Const GK_Failure_DbSubscribeMaxedOut = 271
Global Const GK_Failure_DbNotSubscribed = 272
Global Const GK_Failure_DbRequestKeyUnrecognized = 273
Global Const GK_Failure_DbLimiterKeyUnrecognized = 274
Global Const GK_Failure_DbTypeMismatch = 275
Global Const GK_Failure_DbNonexistantField = 276
Global Const GK_Failure_DbBadKeyfield = 277
Global Const GK_Failure_FileIO = 280
Global Const GK_Failure_NotSupported = 290
Global Const GK_Failure_Max = 399
Global Const GK_Notify = 400
Global Const GK_Notify_Version = 410
Global Const GK_Notify_Maker = 411
Global Const GK_Notify_Time = 412
Global Const GK_Notify_PrintMe = 413
Global Const GK_Notify_DbUnsubscribed = 420
Global Const GK_Notify_EndSession = 430
Global Const GK_Notify_ExecComplete = 440
Global Const GK_Notify_Echo = 441
Global Const GK_Notify_EnterLevel = 450
Global Const GK_Notify_ExitLevel = 451
Global Const GK_Notify_LevelExec = 455
Global Const GK_Notify_Max = 599
Global Const GK_DbNotify = 600
Global Const GK_DbNotify_Max = 799
Global Const GK_Reserved = 800
Global Const GK_Reserved_Max = 999
' Codes supported by Gatekeeper protocol version <insert future version here>:

' Gatekeeper server access flags; the server tracks a set of
' flags for each TCP socket it owns.
' Accesses supported by Gatekeeper 0.10:
Global Const GKACCESS_All = 0 ' Flag mask such that (f&GKACCESS_All)==GKACCESS_All always.
Global Const GKACCESS_Connected = 1 ' The socket is connected to a client.
Global Const GKACCESS_Info = 2 ' The client has informational access privelages.
Global Const GKACCESS_Admin = 4 ' The client has Admin access privelages.
Global Const GKACCESS_LoggedIn = 8 ' The client is logged in with a valid name.
Global Const GKACCESS_InLevel = 16 ' The client is in a level.
Global Const GKACCESS_Game = 32 ' The client is a game server.
Global Const GKACCESS_Gatekeeper = 64 ' The client is another Gatekeeper.
Global Const GKACCESS_Level = 128 ' The client is a game server controlling a level.
Global Const GKACCESS_Config = 4096 ' Option can only be used in a config file.
Global Const GKACCESS_VirtualConnection = 8192 ' This is not a real TCP connection.
Global Const GKACCESS_DbSingleRecord = 16384 ' Database has one record.
Global Const GKACCESS_None = 32768 ' Flag that should never be set.
' Accesses supported by Gatekeeper protocol version <insert future version here>:

'==========================================================
' Gatekeeper protocol
'==========================================================
'
' -------
' Purpose
' -------
'
' The Gatekeeper protocol is design to be a standard for
' interfacing game clients and game servers for non
' performance-critical communictions, such as chatting,
' user account validation, and server administration.
'
' Some capabilities include:
'
' * Enabling game clients to log into game servers, with such
'   features as user account tracking and password validation.
'   The advantage of Gatekeeper over custom solutions is that
'   only one set of user accounts need be maintained for all
'   supported games.
'
' * Enabling game clients to obtain level and player lists
'   from servers, so that utilities like QSpy may support
'   a wide range of Internet based games without knowing
'   the nitty gritty details of each game.
'
' * Enabling server administration clients to log into
'   servers, allowing remote administration of servers
'   across the net.  The becomes useful in complex games
'   like MUDs where remote dungeon masters maintain user
'   accounts.
'
' ------------
' Design goals
' ------------
'
' *. We clearly favor ease of client implementation over
'    ease of server implementation, as the number of different
'    client implementations is expected to far exceed the number
'    of server implemntations.
'
' *. Because Gatekeeper packets will be competing for bandwidth
'    with time-critical gameplay packets, an essential design
'    goal is to minimize the size of Gatekeeper server responses.
'    Minimizing the size of client commands is not as significant,
'    since game communications tend not to require much client-to-
'    server bandwidth.
'
' *. Though the protocol definition must be synchronous (except for
'    the async notify codes), clients which don't require absolute
'    reliability should be implementable asynchronously, sending
'    commands and ignoring the replies, looking only for specific
'    response data types.
'
' *. All aspects of the protocol should be designed for
'    future extendability.
'
' -----------
' Typical use
' -----------
'
' A typical game server will make its Gatekeeper TCP port
' publically available, and use it for administration, sending
' player lists and stats, and for chat.  This enables
' all Gatekeeper-compatible utilities to work will all
' Gatekeeper-compatible servers.  For performance reasons,
' these game servers will typically use a separate port,
' usually a proprietary game-specific protocol, to
' coordinate all in-game action.  GameKeeper knows
' nothing about these in-game protocol details.
'
' Originally designed for games based on the Unreal engine,
' the Gatekeeper interface is simple, standard, and
' extensible enough that it could easily be used for most
' other Internet-based games.
'
' The protocol is text-based and it enables easy
' communication in both numeric machine-recognizable
' and human-readable format.  When a Gatekeeper server is
' initially connected to, all responses are sent in both
' formats. The "Verbose" command turns on machine-only
' format, and "Client" turns back to human and machine format.
'
' The Gatekeeper protocol takes place across a simple TCP
' connection and thus it may be manually used by telneting
' into a Gatekeeper server on the appropriate port.
'
' ------------------------------------
' Client commands and server responses
' ------------------------------------
'
' Clients send commands as ASCII text strings, terminated
' by a cr/lf pair.
'
' Servers send responses as ASCII text strings, where the
' first three characters of every line are a numerical
' response code, so that automated clients may recognize
' the responses easily.  The first 3 characters are
' always numeric and are listed above in the GK_ definitions.
' After the first three characters, a server response may
' be terminated with a cr/lf pair, or it may be followed by
' a space and a line of response-specific text, and terminated
' by a cr/lf pair.
'
' -----------------
' Data format rules
' -----------------
'
' Character set rules:
' *. Gatekeeper servers should terminate all lines of text they
'    send with a cr/lf pair to facilitate Telnet use.
' *. Gatekeeper clients only need terminate lines with cr's.
' *. Gatekeeper clients and servers should recognize lines that
'    are only cr terminated (no lf required).
' *. Servers should send only ASCII 10, 13, and 32-126
'    during normal use, and 8 only when echoing back client input.
' *. Automated clients should send only ASCII 13, and 32-126.
' *. Recognizing that clients may be human and may be hackers,
'    the server must handle (gracefully) anything that clients
'    throw at it.
' *. Here we are only defining a rigid input standard for
'    automated clients sending command lines which contain
'    regular ASCII characters and cr/lf's.  The functionality
'    of additional control codes as well as VT-100 style escape
'    sequences is implementation-defined, and implementations
'    may choose to implement a VT-100 telnet-friendly input
'    system.
'
' Parameter rules:
' *. Parameters (items which are sent from either the client or
'    server to the other party) may be either an unquoted string
'    consisting of the regular ASCII characters with no spaces,
'    or a quoted string containing any ascii characters 32-126.
' *. A quoted string need not have an ending quote.
' *. In quoted strings, \ followed by a character means that
'    the next ASCII character from 32-126 is literal. This is
'    how you can stick quotes in quoted strings.
' *. Any parameter that contains spaces, quotes, or \'s must be
'    sent as a quoted string.
'
' Name rules:
' *. Wherever a name is required, a parameter is fetched and it
'    is checked to verify that it's a valid name. Thus, it is
'    valid to quote a name.
' *. A name must start with a-z or A-Z, and may only contain
'    the characters a-z, A-Z, 0-9, ".", and "_". Names must be
'    between 1 and 31 characters. Servers must not accept or
'    perpetuate any bad names.
' *. For comparison purposes, names are not case sensitive.
'
' Number rules:
' *. Wherever a number is required, a parameter is fetched and it
'    is checked to verify that it's a valid number. Thus it's
'    valid to quote a number.
' *. Number must begin with an optional leading + or -,
'    followed by a digit, followed by multiple digits with
'    one optional decimal point.  Numbers are signed and
'    must contain at least 32 bits of precision.
'
' --------------
' Protocol rules
' --------------
'
' Once a TCP connection is established, the communication
' session goes as follows:
'
' 1. Server sends GK_Notify_Version, GK_Notify_Maker,
'    and GK_Notify_Time. Then optionally sends any other
'    GK_Notify_PrintMe messages that the client should display.
'    Clients who are telnetting in see all of these codes.
'    Automated clients should display all of the
'    GK_Notify_PrintMe codes sent by the server, as these
'    may contain important information (hours, rules, etc).
'
' 2. Client sends a text command line.
'
' 3. Server processes the command line.
'
'    If the command is not valid or generates an error, the server
'    replies with GK_Failure.
'
'    If the command is successful, the server will send zero or
'    more GK_Notify or GK_DbNotify responses answering the
'    client's query, followed by one GK_Success response.
'
' 4. If we're still connected, go back to step 2.
'
' Thus, each client command ultimately generates one and only one of the
' following responses: GK_Failure, GK_Success, or a disconnect.
'
' ----------------------
' Asynchronous responses
' ----------------------
'
' At any time, the server may decide to send an asynchronous
' GK_Notify or GK_DbNotify message to the client.  These
' notifications are for events like chat text arriving, or a new
' level becoming available. The client should receive these and
' process them (or choose not to process them) as it wishes.
'
' At any time, the server may disconnect the client by sending
' GK_Notify_EndSession. However, the client may not receive these as TCP
' often breaks the connection before all queued up data has been sent,
' so clients should always gracefully handler servers disconnecting.
'
' GK_Notify codes are for notifying the client of asynchronous
' events that are not database-oriented, such as chat text arriving.
' The text following the GK_Notify code, if any, is code
' specific.
'
' GK_DbNotify codes are for notifying the client of asynchronous
' events that are database oriented, such as changes, additions,
' and deletions to the player list. The text following the
' GK_DbNotify code is record oriented, in the following
' format:
'
'   GK_DbNotify <+/-> <keyfieldvalue_name> [<field_name>=<value_parm>]...
'
'   Where:
'     + means that keyfieldvalue_name represents a new keyfield that was
'       not in the database prior to the notify, or a modified
'       pre-existing keyfield.
'     - means that keyfieldvalue_name was in the database prior to
'       the notify and is now being deleted; in this case
'       no record information follows.
'     keyfieldvalue_name is the value of the record's key field,
'       such as player name (in the player list) or level name
'       (in the level list).
'     The zero or more optional field_name/value_parm pairs
'       indicate values in the database. Any fields that are
'       not sent are assumed to be empty strings or zero numbers
'       on the client side.
'
' If implemented soundly, a client should be able to
' subscribe to a database (such as a player list or level list),
' immediately receive the current information, and then receive
' async notifications for all additions, deletions, and changes
' so that the client's database is always synchronized with
' the server's minus some time lag.
'
' ---------------
' Gatekeeper time
' ---------------
'
' Each Gatekeeper keeps its own internal time (in seconds)
' which is sent to clients using GK_Notify_Time and can
' be requested via the Time command. The time has a
' completely arbitrary starting point, but counts up at
' exactly one unit per second. Level launch times are tracked
' in Gatekeeper time, so that clients can tell how long a
' level has been up by taking the difference between
' the current Gatekeeper time and the level up-time.
'
' ----------------------
' Protocol extensibility
' ----------------------
'
' The protocol may be extended in the future by adding
' new numeric codes to the above codes.  When a client
' receives a code of the form XYY and his version of
' the protocol doesn't recognize XYY, he must treat it
' exactly as the generic response, X00.  If he doesn't
' recognize X00, he ignores the code entirely.
'
' For this reason, all of the X00 codes, such as GK_Notify
' don't do anything special, and thus future server responses
' have no effect on current clients.
'
' Servers should be designed so that new codes don't
' break old client versions, in accordance with the
' above scheme.
'
' -----------------------------
' Notes on command descriptions
' -----------------------------
'
'   [thing] means that the enclosed thing is optional.
'
'   <something> means that you replace <something> with an
'   actual parameter.
'
'   <something_name> refers to a name. See the name rules above.
'
'   <something_parm> refers to any kind of parameter, usually a string.
'   See the string rules above.
'
'   <something_num> refers to a number. See the number rules above.
'
'   <parm1 | parm2> means that either parm1 or parm2 can be used
'   but not both.
'
'   ... means that the previous item may be repeated zero
'   or more times.
'
'   Extraneous parameters following a command (i.e. more parameters)
'   than the server expects) are ignored, so that all commands may
'   be expanded in the future by adding more parameters.
'
' --------------------
' Gatekeeper datatypes
' --------------------
'
' The following single character codes indicate datatypes:
'
' N: A name, 1-31 characters, must start with a letter and contain
'    only letters, numbers, and "_".
' O: An optional name; may be a name or may be blank.
' P: A password, may either be a name or a blank (zero-length) string.
' I: A 32-bit integer.
' B: A boolean value, either 0 or 1.
' F: A floating point number.
' S: An ASCII string containing only ASCII 32-126.
'
' -------------------
' Gatekeeper commands
' -------------------
'
' Verbose
'    [Access: All, Version: 0.10]
'    Instructs the Gatekeeper server to send verbose
'    responses, with both numeric and human-readable text.
'    Verbose is on by default. The opposite of Verbose is Client.
'    When in verbose mode, commands are echoed back to facilitate
'    telnet usage.
'
' Client <protocol_version_num> [<application_name> [<application_version_num>]]
'    [Access: All, Version: 0.10]
'    Instructs the Gatekeeper server to send machine readable
'    responses only (opposite of Verbose) and adhere to
'    a protocol version of ProtocolVersion or earlier.
'
'    ApplicationName and ApplicationVersion are optional
'    single-word strings identifying the client application
'    and its version, for informational purposes only. The
'    server should not adjust its behaviour according to the
'    client application name or application version.
'
'    When in client mode, commands are not echoed back.
'    Version numbers are integers in the form NNNFF
'    where NNN=integer and FF=decimal. For example,
'    version number 1350 means "13.50".
'
' Login <user_name> [<password_name>]
'    [Access: All, Version: 0.10]
'    Attempts to log in with a specified name.  If that
'    name has no password associated with it, the password is
'    ignored.  When you connect and you first log in,
'    you have Info privelages, which let you get the
'    player lists and level lists, iff the server
'    has a blank Info password. Passwords are not case
'    sensitive. Upon success, sends the player's name.
'
'    The following names are special:
'    Admin<#>: The server administrator. When you log in
'       as Admin, a number will be appended to your name so
'       that your name is unique even if other administrators
'       are online.
'    Not-logged-in: Assiged to users who have not yet logged in.
'
' Exit
'    [Access: All, Version: 0.10]
'    Logs you out.  Note that it's fine for clients to just disconnect
'    without using the Exit command.
'
' Save <db_name>[;<db_name>]... <localfilename_parm>
'     [Access: Admin privelages required, Version: 0.10]
'     Save's the specified databases into a local file.
'     Requires Admin privelages.
'
' Exec <localfilename_parm>
'     [Access: Admin privelages required, Version: 0.10]
'     Executes the commands from the specified file
'     as if they were typed from the console.
'     The response codes resulting from the command execution
'     will be sent, followed by GK_Notify_ExecComplete.
'     The client can count on always receiving a
'     GK_Notify_ExecComplete after all of the Exec result
'     codes, so that it may maintain synchronization.
'
' Rem <remark text up to ending cr/lf>
'     [Access: All, Version: 0.10]
'     A remark command; has no effect at all.
'
' Notify <user_name> <notifycode_num> [<msg_string>]
'     [Access: Admin or level privelages required, Version: 0.10]
'     Sends the specified notification code (and optional)
'     string to the user. The code must be in the range
'     from GK_Notify to GK_Notify_Max.
'
' Echo <echo text up to ending cr/lf>
'     [Access: All, Version: 0.10]
'     Echoes text to the client on the server's
'     GK_Success response line.
'
' Time
'     [Access: All, Version: 0.10]
'     Sends the current Gatekeeper time in seconds,
'     using GK_Notify_Time.  Each Gatekeeper keeps its
'     own time with an arbitrary (meaningless) starting
'     point, which counts up at exactly 1 unit per second.
'
' LevelFiles
'     [Access: Admin privelages required, Version: 0.10]
'     Returns a list of filenames for levels which are in
'     the Gatekeeper's level directories but are not
'     on the level list.
'
' AddLevel
'     [Access: Admin privelages required, Version: 0.10]
'     Adds a specified level file to the Gatekeeper's level list.
'
' DeleteLevel
'     [Access: Admin privelages required, Version: 0.10]
'     Deletes a specified level file to the Gatekeeper's level list.
'
' UpLevel <level_name>
'     [Access: Admin privelages required, Version: 0.10]
'     Attempts to launch the level's level server to bring
'     the level up.
'
' DownLevel <level_name>
'     [Access: Admin privelages required, Version: 0.10]
'     Attempts to shut the level down.
'
' Play/Watch <level_name> <level_password>
'     [Access: LoggedIn privelages required, Version: 0.10]
'     Attempts to enter the specified level for either
'     playing or watching. The player must not already
'     be in a level when he uses this command, and the
'     password must match (if the level has a password).
'
' BeginLevel <app_string> <appversion_num> [<packages_string>]
'     [Access: Level privelages required, Version: 0.10]
'     This command, usable only by a level server, tells
'     the Gatekeeper that the level server's level is up.
'
' Kick <user_name> <msg_string>
'     [Access: Level or Admin privelages required, Version: 0.10]
'     Kicks a player out of the level server's level.
'     If you specify a properties string, that string
'     represents the player's private persistent properties
'     at the time he exits the level. The format is
'     completely up to the level server; the Gatekeeper
'     makes no attempt to figure out its meaning.
'
' UserExtra <user_name> <info_string>
'     [Access: Level privelages required, Version: 0.10]
'     Updates the user's public game-specific information.
'     This information is made available to everyone with
'     Info privelages and is used for stuff like kill
'     counts, score, etc.  This command should be used
'     judiciously, as it often results in a stream of
'     notifications sent out over the network - it makes
'     sense to only update the UserInfo when major events
'     happen (like a user dying).
'
' LevelExtra <extra_string>
'     [Access: Level privelages required, Version: 0.10]
'     Set's the level's "extra" string, a publicly readable
'     string which level servers may use for any internal
'     info they wish to make available publically.
'
' LevelExec <commands>
'     [Access: InLevel privelages required, Version: 0.10]
'     Valid only when the client is in a level, this passes
'     the specified commands to the level server via
'     GK_Notify_LevelExec for execution. This command always
'     succeeds when you're in a level; any data that the
'     level server wants to return must come via async
'     notifications.
'
' ------------------------------------------
' Gatekeeper 0.10 database oriented commands
' ------------------------------------------
'
' Gatekeeper defines a simple interface for single-keyed, flat,
' record-oriented databases such as the player list and level list.
' The interface enables clients to obtain a database's current
' records, and then be notified of all changes that occur.
'
' Here, <db_context> means a database name, optionally followed
' by a colon and a semicolon-delimited list of fields to get.
' For example:
'
'     "Levels" requests the all values of all records of Levels
'         database.
'
'     "Users:Users;Team;Level" requests all of the User, Team, and
'         Level values from the Users database.
'
' Get <db_context> [key_name=value_parm] ...
'     [Access: Requires read access for the specified database, Version: 0.10]
'     Gets all of the records in the specified database, using the
'     GK_DbNotify code, and then sends a success code.  Sends
'     a failure code if any problems occur.  This does not subscribe
'     the client to the database, so no future updates are changed
'     and a nonzero GK_DbNotify channel is not assigned.  This is
'     used for getting a database once where you need no updates.
'     This fails if you have insufficient access to the database.
'
'     If you specify one or more optional search limiters (matching
'     keys/values), then only the records matching all of the
'     limiters are returned.
'
' Sub <db_context> [key_name=value_parm] ...
'     [Access: Requires read access for the specified database, Version: 0.10]
'     Tries to subscribe the client to the named database.
'     If the client doesn't have read access, fails.
'     If successful, first sends a GK_Success_DbSubscribed message
'     with a unique nonzero GK_DbNotify code attached, for example
'     textually, "101 501". This enables the client to subscribe
'     to multiple databases and easily distinguish between GK_DbNotify
'     codes by number. Immediately after the success code is sent,
'     the server sends a dump of all of the database's current records.
'     As the database is updated, the server sends notify messages
'     of added, modified, or deleted records.
'
'     If you specify one or more optional search limiters (matching
'     keys/values), then only the records matching all of the
'     limiters are returned.
'
'     Note that the GK_DbNotify mechanism limits clients to
'     subscribing to 199 databases at a time.
'
'     Note that it's valid for a client to subscribe to a database
'     multiple times, possibly with a different search limiter for
'     each subscription. Each subscription will be assigned a unique
'     GK_DbNotify code.
'
' Unsub <db_notify_code_num>
'     [Access: All, Version: 0.10]
'     Unsubscribe from the database, for example "Unsub 501".
'
' Set <db_name> [<key_name>=<value_parm>]...
'     [Access: Requires write access for the specified database, Version: 0.10]
'     Sets the contents of the specified database record or adds a new
'     record.  If the database is multi-record, one of the keys specified
'     must be the database's keyfield.
'
' The limit on the number of client-subscribed databases is 199,
' based on the number of GK_DbNotify codes available.
'
' Obviously the client is responsible for remembering the
' associations between all of his subscriptions and the
' corresponding GK_DbNotify codes returned by Sub.
'
' There is a special database named "Databases" which is
' simply a list of all of the databases available on the system.
' You get Get and Sub it just like any other database.
'
' Note that there is no way to create a new database or to restructure
' an existing one. The goal of Gatekeeper databases isn't to provide
' an interesting database engine, but to make it easy to reliably
' replicate internal server databases on the client side.
'
' -------------
' Level servers
' -------------
'
' The typical level server is launched as follows:
'
' 1. The level server is either automatically launched
'    at Gatekeeper startup time, or is launched manually
'    by a Gatekeeper admin by using the command
'    "Level Up <level_name>".
'
' 2. The Gatekeeper runs the level server's executable,
'    which it deduces from the file extension of the
'    level name. For example, the ".unr" file extension
'    is registered to Unreal, so launching "Level.unr"
'    automatically spawns Unreal. The Gatekeeper spawns
'    the level server with whatever command line
'    paramteres were specified for it in the level's
'    properties.
'
' 3. Once the level server has initialized itself, it
'    establishes a TCP connection with the Gatekeeper
'    and logs on, typically using the following
'    commands:
'        Client <gk_version_num> <app_name> <app_version>
'        Login <level_name> <level_password>
'        Sub Levels Level=<level_name>
'        Sub Users Level=<level_name>
'        BeginLevel <levelport_num> <app_name> <app_version>
'
'    The first subscription command subscribes the
'    level server to the level list, so that it can
'    get incoming Gatekeeper requests (via RequestState).
'
'    The second subscription command subscribes to the
'    list of players who are in the level server's level.
'    This is how the level server recognizes that the
'    Gatekeeper has allowed new players to enter, or forced
'    existing players to leave. For consistency, the level
'    server should take the Gatekeeper's player lists as
'    absolute truth. If the level server wishes to kick
'    a player out, it should issue a "Level Kick" command
'    to the Gatekeeper.
'
'    The "BeginLevel" command tells that Gatekeeper that
'    the level is up and running, and ready to accept
'    players. It also notifies the Gatekeeper of the
'    level's port which may be either TCP or UDP and which
'    may use whatever protocol the level server desires;
'    the Gatekeeper only uses this port to notify accepted
'    players where they can find the level server.
'
' 4. The level server manages the game in progress,
'    recognizing players as they enter, and continuing
'    until the level server decides to exit, or the
'    Gatekeeper requests it to go down.  During gameplay,
'    the level server uses the "Level UserInfo" command
'    to update publically viewable player stats.
'
' 5. When the level server is ready to go down, it should
'    simply exit. If any players remain in the level, the
'    Gatekeeper handles notifying them that the level is
'    down.
'
' ----------------------------------
' Gatekeeper 0.10 required databases
' ----------------------------------
'
' Users
' Levels
' LevelParms
' Gate, Game, Passwords, TimeLimits
'
' -----------
' Definitions
' -----------
'
' Gatekeeper: A machine on the Internet which controls access
' to a game-specific game server and chatting among players
' worldwide.  Clients make requests to join game levels to the
' Gatekeeper, and the Gatekeeper performs
' high-level player and connection management.
'
' Gatekeeper protocol: A synchronous, text-based protocol built
' on top of TCP which provides a reliable way for clients and
' Gatekeepers to communicate. The protocol is designed to be
' reliably machine-readable, and also human-readable via Telnet.
'
' Gatekeeper client: Any player, utility, or administrator who
' is connected to a Gatekeeper.  A Gatekeeper client may become
' a player by successfully joining a game.
'
' Player: A single game participant.  There are two sides to
' a player, the side that exists on a client machine (where
' the graphics are drawn and the input is taken), and the
' side that exists on a level controlled by a game server,
' where all physics and gameplay decisions are made.  At any
' time, a player will be in either 0 or 1 levels.
'
' Game server: A machine on the Internet which may control one
' or more levels.
'
' Level: An object controlled by a game server which contains players
' in a gaming context.
'
' Master server:
'
' A machine on the Internet that controls high-level player
' accounts, persistent player statistics, and manages
' worldwide chatting.
'
' -----
' Notes
' -----
'
' *. Though we make specific reference to TCP here, the Gatekeeper
'    protocol is equally usable with any other reliable stream
'    based protocol.
'
' ------------
' To expand on
' ------------
'
'todo: Implement timeouts.
'todo: Commands and possible error message map.
'todo: BNF gramar for all of the above.
'todo: Document standard databases and their fields.
'todo: Clean up definitions and explanations.
'todo: Cull error messages.
'todo: ANSI C/C++ source with expandable hooks.
'todo: Figure out chatting: chanops, local/global channels etc.
'todo: Figure out master server commands, topics, and stuff.
'todo: RFC in MS Word for saving as text or html.
'todo: Define Gatekeeper database replication via spanning
' tree where each Gatekeeper can connect to a higher up
' Gatekeeper if the one above him is cut off.
'
' -----------------
' Future directions
' -----------------
'
' - Worldwide, global IRC-style chat across servers and
'   even across multiple games.
' - Generalized in-game stats determination.
' - Global master server architecture with account management.
' - C/C++ Gatekeeper server reference implementation,
'   synchronize protocol names and internal variable names.
' - Teams.

'/////////////////////////////////////////////////////////
' Global constants.
'/////////////////////////////////////////////////////////

' Primary registry keys.
Global Const SERVER_REGISTRY_KEY = "Gatekeeper Server"
Global Const CLIENT_REGISTRY_KEY = "Gatekeeper Client"

' Hardcoded defaults.
Global Const DEFAULT_PEER_UPLINK_LIST = "unreal.epicgames.com 23"
Global Const DEFAULT_LEVEL_DIRECTORY_LIST = "..\Maps\*.unr"
Global Const DEFAULT_LEVEL_PARMS = "gate=%gateinfo% file=%levelfile%"
Global Const DEFAULT_MAX_LEVEL_PLAYERS = 16
Global Const DEFAULT_MAX_LEVEL_WATCHERS = 16
Global Const DEFAULT_CONTACT_NAME = "Unknown"
Global Const DEFAULT_CONTACT_EMAIL = "Unknown"
Global Const DEFAULT_CONTACT_ORGANIZATION = "Unknown"
Global Const DEFAULT_CONTACT_WEB = "Unknown"

' Log info
Global Const GATE_LOG_1 = "#####################"
Global Const GATE_LOG_2 = "# Gatekeeper server #"
Global Const GATE_LOG_3 = "#####################"

' Client info.
Global Const GATECLIENT_TITLE = "Gatekeeper Client"
Global Const MAX_REMOTE_SERVERS = 64

' Timer.
Private PrevTimer As Long
Private TimerBase As Long

'/////////////////////////////////////////////////////////
' Standard Win32 registry related functions.
'/////////////////////////////////////////////////////////

Type STARTUPINFO
    cb As Long
    lpReserved As String
    lpDesktop As String
    lpTitle As String
    dwX As Long
    dwY As Long
    dwXSize As Long
    dwYSize As Long
    dwXCountChars As Long
    dwYCountChars As Long
    dwFillAttribute As Long
    dwFlags As Long
    wShowWindow As Integer
    cbReserved2 As Integer
    lpReserved2 As Byte
    hStdInput As Long
    hStdOutput As Long
    hStdError As Long
End Type
Global Const STARTUPINFO_SIZE = 17 * 4

Type SECURITY_ATTRIBUTES
    nLength As Long
    lpSecurityDescriptor As Long
    bInheritHandle As Boolean
End Type
Global Const SECURITY_ATTRIBUTES = 3 * 4

Type PROCESS_INFORMATION
    hProcess As Long
    hThread As Long
    dwProcessId As Long
    dwThreadId As Long
End Type
Global Const PROCESS_INFORMATION_SIZE = 4 * 4

Declare Function GetExitCodeProcess Lib "kernel32" ( _
    ByVal hProcess As Long, lpExitCode As Long) As Long
Global Const STILL_ACTIVE = &H103

Declare Function GetLastError Lib "kernel32" () As Long

Declare Function CreateProcess Lib "kernel32" _
    Alias "CreateProcessA" ( _
    ByVal lpApplicationName As String, ByVal lpCommandLine As String, _
    ByVal lpProcessAttributes As Long, _
    ByVal lpThreadAttributes As Long, _
    ByVal bInheritHandles As Long, ByVal dwCreationFlags As Long, _
    lpEnvironment As Any, _
    ByVal lpCurrentDriectory As String, _
    lpStartupInfo As STARTUPINFO, _
    lpProcessInformation As PROCESS_INFORMATION) As Long

Declare Function FindExecutable Lib "shell32.dll" _
    Alias "FindExecutableA" ( _
    ByVal lpFile As String, _
    ByVal lpDirectory As String, _
    ByVal lpResult As String) _
    As Long

Declare Function ShellExecute Lib "shell32.dll" _
    Alias "ShellExecuteA" ( _
    ByVal hwnd As Long, _
    ByVal lpOperation As String, _
    ByVal lpFile As String, _
    ByVal lpParameters As String, _
    ByVal lpDirectory As String, _
    ByVal nShowCmd As Long) _
    As Long

' Registry variables
Dim RegUserSoftwareKey As Long
Dim RegUserCompanyKey As Long
Dim RegUserAppKey As Long
Dim RegUserHeadingKey As Long

Dim RegMachineSoftwareKey As Long
Dim RegMachineCompanyKey As Long
Dim RegMachineAppKey As Long
Dim RegMachineHeadingKey As Long

' Registry constants.
Public Const HKEY_CLASSES_ROOT = &H80000000
Public Const HKEY_CURRENT_USER = &H80000001
Public Const HKEY_LOCAL_MACHINE = &H80000002
Public Const HKEY_USERS = &H80000003
Public Const HKEY_PERFORMANCE_DATA = &H80000004
Public Const HKEY_CURRENT_CONFIG = &H80000005
Public Const HKEY_DYN_DATA = &H80000006

Public Const REG_SZ = 1

' Registry API's.
Declare Function RegCloseKey Lib "advapi32.dll" (ByVal hKey As Long) As Long
Declare Function RegCreateKey Lib "advapi32.dll" Alias "RegCreateKeyA" (ByVal hKey As Long, ByVal lpSubKey As String, phkResult As Long) As Long
Declare Function RegDeleteKey Lib "advapi32.dll" Alias "RegDeleteKeyA" (ByVal hKey As Long, ByVal lpSubKey As String) As Long
Declare Function RegEnumKey Lib "advapi32.dll" Alias "RegEnumKeyA" (ByVal hKey As Long, ByVal dwIndex As Long, ByVal lpName As String, ByVal cbName As Long) As Long
Declare Function RegEnumValue Lib "advapi32.dll" Alias "RegEnumValueA" (ByVal hKey As Long, ByVal dwIndex As Long, ByVal lpValueName As String, lpcbValueName As Long, lpReserved As Long, lpType As Long, lpData As Byte, lpcbData As Long) As Long
Declare Function RegLoadKey Lib "advapi32.dll" Alias "RegLoadKeyA" (ByVal hKey As Long, ByVal lpSubKey As String, ByVal lpFile As String) As Long
Declare Function RegOpenKey Lib "advapi32.dll" Alias "RegOpenKeyA" (ByVal hKey As Long, ByVal lpSubKey As String, phkResult As Long) As Long
Declare Function RegOpenKeyEx Lib "advapi32.dll" Alias "RegOpenKeyExA" (ByVal hKey As Long, ByVal lpSubKey As String, ByVal ulOptions As Long, ByVal samDesired As Long, phkResult As Long) As Long
Declare Function RegQueryValue Lib "advapi32.dll" Alias "RegQueryValueA" (ByVal hKey As Long, ByVal lpSubKey As String, ByVal lpValue As String, lpcbValue As Long) As Long

Declare Function RegQueryValueEx Lib "advapi32.dll" Alias "RegQueryValueExA" (ByVal hKey As Long, ByVal lpValueName As String, ByVal lpReserved As Long, lpType As Long, lpData As Any, lpcbData As Long) As Long
    ' Note that if you declare the lpData parameter as
    ' String, you must pass it By Value.

Declare Function RegReplaceKey Lib "advapi32.dll" Alias "RegReplaceKeyA" (ByVal hKey As Long, ByVal lpSubKey As String, ByVal lpNewFile As String, ByVal lpOldFile As String) As Long
Declare Function RegRestoreKey Lib "advapi32.dll" Alias "RegRestoreKeyA" (ByVal hKey As Long, ByVal lpFile As String, ByVal dwFlags As Long) As Long
Declare Function RegSetValue Lib "advapi32.dll" Alias "RegSetValueA" (ByVal hKey As Long, ByVal lpSubKey As String, ByVal dwType As Long, ByVal lpData As String, ByVal cbData As Long) As Long
Declare Function RegSetValueEx Lib "advapi32.dll" Alias "RegSetValueExA" (ByVal hKey As Long, ByVal lpValueName As String, ByVal Reserved As Long, ByVal dwType As Long, lpData As Any, ByVal cbData As Long) As Long         ' Note that if you declare the lpData parameter as String, you must pass it By Value.
Declare Function RegUnLoadKey Lib "advapi32.dll" Alias "RegUnLoadKeyA" (ByVal hKey As Long, ByVal lpSubKey As String) As Long

'
' Open the registry at a particular section.
' This must be called before GetUserInfo, SetUserInfo,
' GetMachineInfo, and SetMachineInfo.  Be sure to call
' CloseReg as soon as you're done.
'
Sub OpenReg(AppName As String)
    
    Call RegOpenKey(HKEY_CURRENT_USER, "Software", RegUserSoftwareKey)
    Call RegCreateKey(RegUserSoftwareKey, "Epic MegaGames", RegUserCompanyKey)
    Call RegCreateKey(RegUserCompanyKey, AppName, RegUserAppKey)
    RegUserHeadingKey = 0
    
    Call RegOpenKey(HKEY_LOCAL_MACHINE, "Software", RegMachineSoftwareKey)
    Call RegCreateKey(RegMachineSoftwareKey, "Epic MegaGames", RegMachineCompanyKey)
    Call RegCreateKey(RegMachineCompanyKey, AppName, RegMachineAppKey)
    RegMachineHeadingKey = 0
End Sub

'
' Close the registry opened by OpenReg.
'
Sub CloseReg()
    
    SetUserHeading ("") ' Flush current heading
    SetMachineHeading ("")
    
    Call RegCloseKey(RegUserAppKey)
    Call RegCloseKey(RegUserCompanyKey)
    Call RegCloseKey(RegUserSoftwareKey)
    
    Call RegCloseKey(RegMachineAppKey)
    Call RegCloseKey(RegMachineCompanyKey)
    Call RegCloseKey(RegMachineSoftwareKey)
End Sub

'
' The the registry heading for user settings.
'
Sub SetUserHeading(Heading As String)
    
    If RegUserHeadingKey <> 0 Then
        Call RegCloseKey(RegUserHeadingKey)
    End If
    
    If Heading = "" Then
        RegUserHeadingKey = 0
    Else
        Call RegCreateKey(RegUserAppKey, Heading, RegUserHeadingKey)
    End If
End Sub

'
' Set user-specific registry information.  Use this
' for saving preferences.
'
Sub SetUserInfo(Name As String, Value As String)
    Dim hKey As Long
    
    hKey = RegUserHeadingKey
    If hKey = 0 Then hKey = RegUserAppKey
    
    If Value <> "" Then
        Call RegSetValueEx(hKey, Name, 0, REG_SZ, ByVal Value, Len(Value))
    Else
        Call RegSetValueEx(hKey, Name, 0, REG_SZ, ByVal " ", 1)
    End If
End Sub

'
' Get user-specific registry information.
'
Function GetUserInfo(Name As String, Default As String) As String
    Dim hKey As Long
    Dim LengthVar As Long
    Dim Temp As String
    
    hKey = RegUserHeadingKey
    If hKey = 0 Then hKey = RegUserAppKey
    
    LengthVar = 1024
    Temp = Space(1024)
    Call RegQueryValueEx(hKey, Name, 0, REG_SZ, _
        ByVal Temp, LengthVar)
        
    If InStr(Temp, Chr(0)) <> 0 Then
        Temp = Left(Temp, InStr(Temp, Chr(0)) - 1)
    End If
    
    Temp = Trim(Temp)
    If Temp = "" Then
        Temp = Default
    Else
        Temp = Left(Temp, LengthVar)
        If Len(Temp) > 0 Then
            If Asc(Right(Temp, 1)) = 0 Then
                Temp = Left(Temp, Len(Temp) - 1)
            End If
        End If
    End If
    GetUserInfo = Temp
End Function

'
' The the registry heading for user settings.
'
Sub SetMachineHeading(Heading As String)
    
    If RegMachineHeadingKey <> 0 Then
        Call RegCloseKey(RegMachineHeadingKey)
    End If
    
    If Heading = "" Then
        RegMachineHeadingKey = 0
    Else
        Call RegCreateKey(RegMachineAppKey, Heading, RegMachineHeadingKey)
    End If
End Sub

'
' Set machine-specific registry information.  Use this
' for installation info.
'
Sub SetMachineInfo(Name As String, Value As String)
    Dim hKey As Long
    
    hKey = RegMachineHeadingKey
    If hKey = 0 Then hKey = RegMachineAppKey
    
    Call RegSetValueEx(hKey, Name, 0, REG_SZ, ByVal Value, Len(Value))
End Sub

'
' Get machine-specific registry information.
'
Function GetMachineInfo(Name As String, Default As String) As String
    Dim hKey As Long
    Dim LengthVar As Long
    Dim Temp As String
    
    hKey = RegMachineHeadingKey
    If hKey = 0 Then hKey = RegMachineAppKey
    
    LengthVar = 1024
    Temp = Space(1024)
    Call RegQueryValueEx(hKey, Name, 0, REG_SZ, ByVal Temp, LengthVar)
    
    If InStr(Temp, Chr(0)) <> 0 Then
        Temp = Left(Temp, InStr(Temp, Chr(0)) - 1)
    End If

    GetMachineInfo = Trim(Temp)
    If (GetMachineInfo = "") Then
        GetMachineInfo = Default
    Else
        GetMachineInfo = Left(GetMachineInfo, LengthVar)
        If Len(GetMachineInfo) > 0 Then
            If Asc(Right(GetMachineInfo, 1)) = 0 Then
                GetMachineInfo = Left(GetMachineInfo, Len(GetMachineInfo) - 1)
            End If
        End If
    End If
End Function

'/////////////////////////////////////////////////////////
' Non-wraparount timer.
'/////////////////////////////////////////////////////////

'
' Return a seconds timer that never wraps around.
'
Public Function GateTimer() As Long
    Dim NewTimer As Long
    
    ' Get the time in seconds since midnight.
    NewTimer = Timer + TimerBase
    
    ' Prevent wraparound.
    If (NewTimer < PrevTimer) Then
        NewTimer = NewTimer - TimerBase
        TimerBase = TimerBase + PrevTimer - NewTimer
        NewTimer = NewTimer + TimerBase
    End If
    PrevTimer = NewTimer
    
    ' Return the new time.
    GateTimer = NewTimer
End Function

'/////////////////////////////////////////////////////////
' Command line processing.
'/////////////////////////////////////////////////////////

'
' Try to pull a command word from the beginning of CmdLine.
' Returns True if found (and removed), False otherwise.
'
' Match must be uppercase.  CmdLine may be mixed case.
'
Public Function GetCMD(ByRef CmdLine As String, Match As String) As Boolean
    Dim Temp As String
    Dim C As Long
    
    ' Default to not found.
    GetCMD = False
    
    ' Eat up leading spaces.
    While Left(CmdLine, 1) = " "
        CmdLine = Mid(CmdLine, 2)
    Wend
    
    ' See if we have a match.
    If UCase(Left(CmdLine, Len(Match))) = Match Then
        Temp = Mid(CmdLine, Len(Match) + 1)
        If Temp <> "" Then
            C = Asc(Left(Temp, 1))
            If C = 32 Or C = 10 Or C = 13 Then
                GetCMD = True
                CmdLine = Temp
                
                ' Eat up any trailing spaces.
                While Left(CmdLine, 1) = " "
                    CmdLine = Mid(CmdLine, 2)
                Wend
            End If
        Else
            GetCMD = True
            CmdLine = Temp
        End If
    End If
End Function

'
' Try to pull a character from the beginning of CmdLine.
' Returns True if found (and removed), False otherwise.
'
Public Function GetCHAR(ByRef CmdLine As String, Match As String) As Boolean
    Dim Temp As String
    Dim C As Long
    
    ' Default to not found.
    GetCHAR = False
    
    ' Eat up leading spaces.
    While Left(CmdLine, 1) = " "
        CmdLine = Mid(CmdLine, 2)
    Wend
    
    ' See if we have a match.
    If Left(CmdLine, 1) = Match Then
        GetCHAR = True
        CmdLine = Mid(CmdLine, 2)
        
        ' Eat up any trailing spaces.
        While Left(CmdLine, 1) = " "
            CmdLine = Mid(CmdLine, 2)
        Wend
    End If
End Function

'
' Skip spaces.
'
Sub SkipSpaces(ByRef S As String)
    While Left(S, 1) = " "
        S = Mid(S, 2)
    Wend
End Sub

'
' Grab and remove the first element from a string.
' Elements may be separated by spaces or tabs.
'
Function NextSTRING(ByRef S As String, Optional Separator As Variant) As String
    Dim Result As String, C As String
    Dim Quoted As Boolean
    
    If IsMissing(Separator) Then
        ' Separator is a space.
        Separator = " "
    
        ' Skip leading spaces; spaces are nonrigid unlike other separators.
        SkipSpaces (S)
    End If

    While S <> ""
        ' Grab first character.
        C = Left(S, 1)
        S = Mid(S, 2)
        
        ' Handle the character.
        If (C = Separator) And (Not Quoted) Then
            ' Got to unquoted separator; we're done.
            GoTo Done
        ElseIf C = Chr(34) Then
            ' Toggle quoting.
            Quoted = Not Quoted
        ElseIf Quoted Then
            ' Quoted characters.
            If C <> "\" Then
                ' Normal quoted character.
                Result = Result + C
            Else
                ' Literal character, even if separator.
                Result = Result + Left(S, 1)
                S = Mid(S, 2)
            End If
        Else
            ' Normal character.
            Result = Result + C
        End If
    Wend
    
Done:
    NextSTRING = Result
    
    ' Absorb trailing separator.
    If Left(S, 1) = Separator Then S = Mid(S, 2)

End Function

'
' Grab the next cr/lf-terminated line from the string.
'
Function NextLINE(ByRef S As String) As String
    Dim i As Long
    
    ' Find cr/lf.
    i = InStr(S, Chr(13))
    If i = 0 Then
    
        ' No cr, so get whole thing.
        NextLINE = S
        S = ""
    Else
    
        ' Get up to cr.
        NextLINE = Left(S, i - 1)
        S = Mid(S, i + 1)
    End If
    
    ' Get rid of any lf's in the result.
    i = InStr(NextLINE, Chr(10))
    While i <> 0
        NextLINE = Left(NextLINE, i - 1) + Mid(NextLINE, i + 1)
        i = InStr(NextLINE, Chr(10))
    Wend
End Function

'
' Grab and remove the first element from a string,
' expecting that it's a number. Returns True if success,
' False if not gotten.
'
Function NextNUMBER(ByRef S As String, ByRef N As Long, _
    Optional Separator As Variant) As Boolean
    
    ' Locals.
    Dim Original As String, MaybeNum As String

    ' Save original
    Original = S
    
    ' Try to get number.
    MaybeNum = NextSTRING(S)
    If MaybeNum = "" Then
    
        ' Empty.
        NextNUMBER = False
    ElseIf IsValidNumber(MaybeNum) Then
    
        ' Got a number.
        N = Val(MaybeNum)
        NextNUMBER = True
    Else
    
        ' Not a valid number.
        S = Original
        NextNUMBER = False
    End If
End Function

'
' Return whether a string is a valid name.
' Names must 1-31 characters, must
' start with A-Z or a-z, and must only contain
' A-Z, a-z, and _ internally.
'
Public Function IsValidName(S As String)
    Dim i As Long, C As String

    ' Check length.
    If Len(S) > 31 Or Len(S) < 1 Then
        IsValidName = False
    Else
        ' Default to failure.
        IsValidName = False
        
        ' Verify first character.
        C = Left(S, 1)
        If (C < "a" Or C > "z") And (C < "A" Or C > "Z") Then Exit Function
        
        ' Verify each character.
        For i = 2 To Len(S)
            C = Mid(S, i, 1)
            If _
                (C < "a" Or C > "z") And _
                (C < "A" Or C > "Z") And _
                (C < "0" Or C > "9") And _
                (C <> "_") And (C <> ".") Then
                Exit Function
            End If
        Next i
        ' Success.
        IsValidName = True
    
    End If
End Function

'
' Return whether a string is a valid number.
'
Public Function IsValidNumber(S As String)
    Dim i As Long, C As String
    Dim GotDigits As Boolean
    Dim GotDecimal As Boolean

    ' Default to fail.
    IsValidNumber = False
    If Len(S) < 1 Then Exit Function

    ' Check all digits.
    For i = 1 To Len(S)
        C = Mid(S, i, 1)
        If C >= "0" And C <= "9" Then
        
            ' A valid digit.
            GotDigits = True
        Else

            If C = "+" Or C = "-" Then
                
                ' Allow + or - only as first character.
                If i <> 1 Then
                    Exit Function
                End If
            ElseIf C = "." And GotDigits Then
                
                ' Allow one decimal after a digit.
                If Not GotDecimal Then
                    GotDecimal = True
                Else
                    Exit Function
                End If
            Else
                ' Got an invalid character.
                Exit Function
            End If
        End If
    Next
    
    ' Success.
    IsValidNumber = True
End Function

'
' Return whether a string is valid.
'
Function IsValidString(S As String)

    ' All strings are valid.
    IsValidString = True
End Function

'
' Return whether an item is valid according to its
' Gatekeeper one-character datatype.
'
Function IsValidType(ByVal Item As String, ByVal Datatype As String)

    ' Handle this particular data type.
    Select Case Datatype
        Case "N"
            ' Name
            IsValidType = IsValidName(Item)
        Case "O"
            ' Optional name
            IsValidType = (Item = "") Or IsValidName(Item)
        Case "P"
            ' Password
            IsValidType = (Item = "") Or IsValidName(Item)
        Case "I"
            ' Integer
            IsValidType = IsValidNumber(Item)
        Case "F"
            ' Floating point number
            IsValidType = IsValidNumber(Item)
        Case "S"
            ' String
            IsValidType = True
        Case "B"
            ' Boolean
            IsValidType = (Item = "0") Or (Item = "1")
        Case "T"
            ' Type
            Item = UCase(Item)
            IsValidType = _
                Item = "N" Or Item = "O" Or _
                Item = "P" Or Item = "I" Or _
                Item = "F" Or Item = "S" Or _
                Item = "T" Or Item = "B"
        Case Else
            ' Unknown data type
            IsValidType = False
    End Select
End Function

'
' Return textual description of a datatype.
'
Function TypeDescription(Datatype As String)

    ' Handle this particular data type.
    Select Case Datatype
        Case "N": TypeDescription = "name"
        Case "O": TypeDescription = "optional name"
        Case "P": TypeDescription = "password"
        Case "I": TypeDescription = "integer"
        Case "F": TypeDescription = "floating point number"
        Case "S": TypeDescription = "string"
        Case "B": TypeDescription = "boolean"
        Case "T": TypeDescription = "type"
        Case Else: TypeDescription = "bad"
    End Select
End Function

'
' Return a quoted version of a string which can be
' reliably processed by GetNEXT.  This requires quoting
' the string if it contains quotes, spaces, or literals, and
' adding in literal characters if it contains quotes or literals.
'
Function Quotes(S As String)
    If InStr(S, " ") = 0 And InStr(S, Chr(34)) = 0 _
        And InStr(S, "\") = 0 Then
        
        ' No literal char or quotes required.
        Quotes = S
    Else
        If InStr(S, Chr(34)) = 0 And InStr(S, "\") = 0 Then
            
            ' No literal required
            Quotes = Chr(34) & S & Chr(34)
        Else
        
            ' Literals required.
            Dim Result As String, C As String
            Dim i As Long
            For i = 1 To Len(S)
                C = Mid(S, i, 1)
                If C <> Chr(34) And C <> "\" Then
                
                    ' Normal char.
                    Result = Result + C
                Else
                
                    ' Must be literaled.
                    Result = Result + "\" + C
                End If
            Next
            Quotes = Chr(34) & Result & Chr(34)
        End If
    End If
End Function

'
' Validate a name with user interface.
'
Function ValidateName(S As String, Tag As String) As Boolean
    ValidateName = IsValidName(S)
    If Not ValidateName Then MsgBox "'" & S & "' is not a valid " & Tag & ". " & _
        "It must start with a letter, must contain from 2-31 characters, " & _
        "and may only contain letters, numbers and the '_' character."
End Function

'
' Validate an optional name with user interface.
'
Function ValidateOptionalName(S As String, Tag As String) As Boolean
    ValidateOptionalName = (S = "") Or IsValidName(S)
    If Not ValidateOptionalName Then MsgBox "'" & S & "' is not a valid " & Tag & ". " & _
        "It must start with a letter, must contain from 2-31 characters, " & _
        "and may only contain letters, numbers and the '_' character."
End Function

'
' Validate a number with user interface.
'
Function ValidateNumber(S As String, Tag As String) As Boolean
    ValidateNumber = IsValidNumber(S)
    If Not ValidateNumber Then
        If S = "" Then
            MsgBox "You must enter a " & Tag & "."
        Else
            MsgBox "'" & S & "' is not a valid " & Tag & "."
        End If
    End If
End Function

'
' Validate a list item with user interface.
' List items may contain anything except for spaces,
' commas, semicolons.
'
Function ValidateListItem(S As String, Tag As String) As Boolean
    Dim i As Long
    Dim C As String
    
    ' Balk at zero-length.
    If Len(S) = 0 Then
        MsgBox "This " & Tag & " must not be blank."
        ValidateListItem = False
        Exit Function
    End If
    
    ' Validate all letters.
    For i = 1 To Len(S)
        C = Mid(S, i, 1)
        If C = "," Or C = ";" Or C = " " Then
            MsgBox "'" & S & "' is not a valid " & Tag & ". They may contain " & _
                "anything but spaces, commas, double quotes, semicolons, and backslashes."
            ValidateListItem = False
            Exit Function
        End If
    Next
    
    ' Success.
    ValidateListItem = True
End Function

'
' Validate a specific type.
'
Function ValidateType(S As String, ByVal Datatype As String)
    ValidateType = IsValidType(S, Datatype)
    If Not ValidateType Then
        MsgBox "'" & S & "' is not a valid " & _
            TypeDescription(Datatype) & "."
    End If
End Function

'/////////////////////////////////////////////////////////
' Make a random name of a specified length.
'/////////////////////////////////////////////////////////

Public Function RandomName(Size As Long) As String
    Randomize
    RandomName = Chr(Asc("A") + 25 * Rnd)
    While Size > 1
        RandomName = RandomName + Chr(Asc("A") + 25 * Rnd)
        Size = Size - 1
    Wend
End Function

'/////////////////////////////////////////////////////////
' Gatekeeper functions.
'/////////////////////////////////////////////////////////

'
' Convert a Gatekeeper error code to a human readable string.
'
Public Function GatekeeperError(Code As Long, Extra As String) As String

    'todo: Parse this once the codes are finalized.
    GatekeeperError = Str(Code)
End Function

'/////////////////////////////////////////////////////////
' The End.
'/////////////////////////////////////////////////////////
