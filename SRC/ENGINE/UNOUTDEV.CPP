/*=============================================================================
	UnOutDev.cpp: Unreal FOutputDevice implementation

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	FOutputDevice implementation.
-----------------------------------------------------------------------------*/

//
// Print a message on the output device using LOG_Info type.
//
int FOutputDevice::Log( const char *Text )
{
	guard(FOutputDevice::Log);

	int Length = strlen(Text);
	Write(Text,Length,LOG_Info);
	return Length;

	unguard;
};

//
// Print a message on the output device using LOG_Info type.
//
int FOutputDevice::Log( ELogType MsgType, const char *Text )
{
	guard(FOutputDevice::Log);

	int Length = strlen(Text);
	Write(Text,Length,MsgType);
	return Length;

	unguard;
};

//
// Print a message on the output device, variable parameters.
//
int VARARGS FOutputDevice::Logf(ELogType Event,const char *Fmt,...)
{
	char TempStr[4096];
	va_list  ArgPtr;

	va_start (ArgPtr,Fmt);
	vsprintf (TempStr,Fmt,ArgPtr);
	va_end   (ArgPtr);

	guard(FOutputDevice::Logf);
	return Log(Event,TempStr);
	unguard;
}

//
// Print a message on the output device, variable parameters.
//
int VARARGS FOutputDevice::Logf(const char *Fmt,...)
{
	char TempStr[4096];
	va_list  ArgPtr;

	va_start (ArgPtr,Fmt);
	vsprintf (TempStr,Fmt,ArgPtr);
	va_end   (ArgPtr);

	guard(FOutputDevice::Logf);
	return Log(LOG_Info,TempStr);
	unguard;
}

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
