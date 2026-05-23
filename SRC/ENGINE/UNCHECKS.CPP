/*==============================================================================
	UnChecks.cpp: For adding checks to your code

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Description:
	    Refer to the associated header file.

	Revision history:
	    * 03/22/96: Created by Mark
==============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

static const int MaxMessageLength       = 200;
static const int MaxSourceNameLength    = 200;
static const int MaxCombinedMessageLength = MaxMessageLength + MaxSourceNameLength + 50;

/*-----------------------------------------------------------------------------
	Check functions.
-----------------------------------------------------------------------------*/

//
// Report a failed check.
//
void checkFailed( const char *Message, const char *SourceFileName, int LineNumber )
{
    char CombinedMessage[MaxCombinedMessageLength];
    char * Text = CombinedMessage;
    if( Message != 0 )
    {
        Text += sprintf( Text, "%s", Message );
    }
    if( SourceFileName != 0 )
    {
        Text += sprintf( Text, " [File: %s] ", SourceFileName );
    }
    if( LineNumber != 0 )
    {
        Text += sprintf( Text, " [Line: %i] ", LineNumber );
    }
    #if defined( ENGINE_VERSION )
    {
        // Failure implementation for Unreal:
        appError( CombinedMessage );
    }
    #else
    {
        AfxMessageBox( CombinedMessage );
    }
    #endif
}

//
// Report a failed check.
//
extern void checkFailed( const char * SourceFileName, int LineNumber )
{
    checkFailed( 0, SourceFileName, LineNumber );
}

//
// Report a failed check.
//
extern void checkFailed( int LineNumber )
{
    checkFailed( 0, 0, LineNumber );
}

//
// Report a failed check.
//
extern void checkFailed( const char * Message )
{
    checkFailed( Message, 0, 0 );
}

//
// Report a failed check.
//
extern void checkFailed( )
{
    checkFailed( "Failure" );
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
