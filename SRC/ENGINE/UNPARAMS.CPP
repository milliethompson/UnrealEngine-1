/*=============================================================================
	UnParams.cpp: Functions to help parse commands.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	What's happening: When the Visual Basic level editor is being used,
	this code exchanges messages with Visual Basic.  This lets Visual Basic
	affect the world, and it gives us a way of sending world information back
	to Visual Basic.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	Getters.
	All of these functions return 1 if the appropriate item was
	fetched, or 0 if not.
-----------------------------------------------------------------------------*/

//
// Get a byte (0-255).
//
int UNENGINE_API GetBYTE
(
	const char		*Stream, 
	const char		*Match,
	BYTE			*Value
)
{
	guard(GetBYTE);

	const char *Temp=mystrstr(Stream,Match);
	if( Temp==NULL ) return 0; // didn't match.

	Temp   += strlen( Match );
	*Value  = (BYTE)atoi( Temp );

	return *Value!=0 || isdigit(Temp[0]);
	unguard;
}

//
// Get a signed byte (-128 to 127).
//
int UNENGINE_API GetCHAR
(
	const char		*Stream, 
	const char		*Match,
	CHAR			*Value
)
{
	guard(GetCHAR);

	const char *Temp=mystrstr(Stream,Match);
	if (Temp==NULL) return 0; // didn't match.

	Temp   += strlen( Match );
	*Value  = (CHAR)atoi( Temp );

	return *Value!=0 || isdigit(Temp[0]);
	unguard;
}

//
// Get a word (0-65536).
//
int UNENGINE_API GetWORD
(
	const char		*Stream, 
	const char		*Match,
	WORD			*Value
)
{
	guard(GetWORD);

	const char *Temp=mystrstr(Stream,Match);
	if (Temp==NULL) return 0; // didn't match.

	Temp   += strlen( Match );
	*Value  = (WORD)atol( Temp );

	return *Value!=0 || isdigit(Temp[0]);
	unguard;
}

//
// Get an INDEX (0-65536, or None).
//
int UNENGINE_API GetINDEX
(
	const char		*Stream, 
	const char		*Match,
	INDEX			*Value
)
{
	guard(GetINDEX);

	const char *Temp=mystrstr(Stream,Match);
	if (Temp==NULL) return 0; // didn't match.

	Temp += strlen(Match);

	if( !strnicmp(Temp,"NONE",4) ) *Value = INDEX_NONE;
	else *Value=(WORD)atol( Temp );

	return *Value!=0 || isdigit(Temp[0]);
	unguard;
}

//
// Get a signed word (-32768 to 32767).
//
int UNENGINE_API GetSWORD
(
	const char		*Stream, 
	const char		*Match,
	SWORD			*Value
)
{
	guard(GetSWORD);

	const char *Temp = mystrstr( Stream, Match );
	if( Temp==NULL ) return 0; // didn't match.

	Temp   += strlen( Match );
	*Value  =(SWORD)atol( Temp );

	return *Value!=0 || isdigit(Temp[0]);
	unguard;
}

//
// Get a floating-point number.
//
int UNENGINE_API GetFLOAT
(
	const char		*Stream, 
	const char		*Match,
	FLOAT			*Value
)
{
	guard(GetFLOAT);

	const char *Temp=mystrstr(Stream,Match);
	if (Temp==NULL) return 0; // didn't match.
	*Value=(FLOAT) atof (Temp+strlen(Match));

	return 1;
	unguard;
}

//
// Get a floating-point vector (X=, Y=, Z=), return number of components parsed (0-3).
//
int UNENGINE_API GetFVECTOR
(
	const char		*Stream, 
	FVector			*Value
)
{
	guard(GetFVECTOR);
	int NumVects = 0;

	// Support for old format.
	NumVects += GetFLOAT (Stream,"X=",&Value->X); //oldver.
	NumVects += GetFLOAT (Stream,"Y=",&Value->Y); //oldver.
	NumVects += GetFLOAT (Stream,"Z=",&Value->Z); //oldver.

	// New format.
	if( NumVects==0 )
	{
		Value->X = atof(Stream);

		Stream = strchr(Stream,',');
		if (!Stream) return 0;
		Value->Y = atof(++Stream);

		Stream = strchr(Stream,',');
		if (!Stream) return 0;
		Value->Z = atof(++Stream);

		NumVects=3;
	}
	return NumVects;
	unguard;
}

//
// Get a string enclosed in parenthesis.
//
int UNENGINE_API GetSUBSTRING
(
	const char		*Stream, 
	const char		*Match,
	char			*Value,
	int				MaxLen
)
{
	guard(GetSUBSTRING);

	const char *Found = mystrstr(Stream,Match);
	const char *Start;

	if( Found == NULL ) return 0; // didn't match.

	Start = Found+strlen(Match);
	if (*Start != '(') return 0;

	strncpy (Value,Start+1,MaxLen);
	Value[MaxLen-1]=0;
	char *Temp=strchr(Value,')');
	if (Temp!=NULL) *Temp=0;
	return 1;

	unguard;
}

//
// Get a floating-point vector (X=, Y=, Z=), return number of components parsed (0-3).
//
int UNENGINE_API GetFVECTOR
(
	const char		*Stream, 
	const char		*Match, 
	FVector			*Value
)
{
	guard(GetFVECTOR);

	char Temp[80];
	if (!GetSUBSTRING(Stream,Match,Temp,80)) return 0;
	return GetFVECTOR(Temp,Value);

	unguard;
}

//
// Get a floating-point vector (X=, Y=, Z=), return number of components parsed (0-3).
//
int UNENGINE_API GetFIXFVECTOR
(
	const char *Stream, 
	FVector *Value
)
{
	guard(GetFIXFVECTOR);

	int NumVects = GetFVECTOR(Stream,Value);
	*Value *= 65536.0;
	return NumVects;

	unguard;
}

//
// Get a floating-point scale value.
//
int UNENGINE_API GetFSCALE
(
	const char		*Stream,
	FScale			*FScale
)
{
	guard(GetFSCALE);

	if (GetFVECTOR 	(Stream,&FScale->Scale)!=3) 				return 0;
	if (!GetFLOAT  	(Stream,"S=",&FScale->SheerRate)) 			return 0;
	if (!GetINT     (Stream,"AXIS=",(int *)&FScale->SheerAxis)) return 0;
	return 1;

	unguard;
}

//
// Get a set of rotations (PITCH=, YAW=, ROLL=), return number of components parsed (0-3).
//
int UNENGINE_API GetFROTATION
(
	const char		*Stream, 
	FRotation		*Rotation,
	int				ScaleFactor
)
{
	guard(GetFROTATION);

	FLOAT	Temp;
	int 	N = 0;

	// Old format.
	if (GetFLOAT (Stream,"PITCH=",&Temp)) {Rotation->Pitch = Temp * ScaleFactor; N++;}; //oldver.
	if (GetFLOAT (Stream,"YAW=",  &Temp)) {Rotation->Yaw   = Temp * ScaleFactor; N++;}; //oldver.
	if (GetFLOAT (Stream,"ROLL=", &Temp)) {Rotation->Roll  = Temp * ScaleFactor; N++;}; //oldver.

	// New format.
	if( N == 0 )
	{
		Rotation->Pitch = atof(Stream) * ScaleFactor;

		Stream = strchr(Stream,',');
		if (!Stream) return 0;
		Rotation->Yaw = atof(++Stream) * ScaleFactor;

		Stream = strchr(Stream,',');
		if (!Stream) return 0;
		Rotation->Roll = atof(++Stream) * ScaleFactor;

		N=3;
	}
	return N;
	unguard;
}

//
// Get a rotation value, return number of components parsed (0-3).
//
int UNENGINE_API GetFROTATION
(
	const char		*Stream, 
	const char		*Match, 
	FRotation		*Value,
	int				ScaleFactor
)
{
	guard(GetFROTATION);

	char Temp[80];
	if (!GetSUBSTRING(Stream,Match,Temp,80)) return 0;
	return GetFROTATION(Temp,Value,ScaleFactor);

	unguard;
}

//
// Get a double word (4 bytes).
//
int UNENGINE_API GetDWORD
(
	const char		*Stream, 
	const char		*Match,
	DWORD			*Value
)
{
	guard(GetDWORD);

	const char *Temp=mystrstr(Stream,Match);
	if (Temp==NULL) return 0; // didn't match
	*Value=(DWORD)atol(Temp+strlen(Match));
	return 1;

	unguard;
}

//
// Get a signed double word (4 bytes).
//
int UNENGINE_API GetINT
(
	const char		*Stream, 
	const char		*Match,
	INT				*Value
)
{
	guard(GetINT);

	const char *Temp=mystrstr(Stream,Match);
	if (Temp==NULL) return 0; // didn't match
	*Value=atoi(Temp+strlen(Match));
	return 1;

	unguard;
}

//
// Get a string.
//
int UNENGINE_API GetSTRING
(
	const char		*Stream, 
	const char		*Match,
	char			*Value,
	int				MaxLen
)
{
	guard(GetSTRING);

	int i=strlen(Stream);
	const char *Found = mystrstr(Stream,Match);
	const char *Start;

	if( Found )
	{
		Start = Found+strlen(Match);
		if( *Start == '\x22' )
		{
			// Quoted string with spaces.
			strncpy (Value,Start+1,MaxLen);
			Value[MaxLen-1]=0;
			char *Temp=strchr(Value,'\x22');
			if (Temp!=NULL) *Temp=0;
		}
		else
		{
			// Non-quoted string without spaces.
			strncpy(Value,Start,MaxLen);
			Value[MaxLen-1]=0;
			char *Temp;
			Temp=strchr(Value,' ' ); if (Temp) *Temp=0;
			Temp=strchr(Value,'\r'); if (Temp) *Temp=0;
			Temp=strchr(Value,'\n'); if (Temp) *Temp=0;
			Temp=strchr(Value,'\t'); if (Temp) *Temp=0;
		}
		return 1;
	}
	else return 0;
	unguard;
}

//
// Sees if Stream starts with the named command.  If it does,
// skips through the command and blanks past it.  Returns 1 of match,
// 0 if not.
//
int UNENGINE_API GetCMD
(
	const char		**Stream, 
	const char		*Match
)
{
	guard(GetCMD);

	while( (**Stream==' ')||(**Stream==9) )
		(*Stream)++;

	if( strnicmp(*Stream,Match,strlen(Match))==0 )
	{
		*Stream += strlen(Match);
		if( !isalnum(**Stream) )
		{
			while ((**Stream==' ')||(**Stream==9)) (*Stream)++;
			return 1; // Success.
		}
		else
		{
			*Stream -= strlen(Match);
			return 0; // Only found partial match.
		}
	}
	else return 0; // No match.
	unguard;
}

//
// See if Stream starts with the named command.  Returns 1 of match,
// 0 if not.  Does not affect the stream.
//
int UNENGINE_API PeekCMD
(
	const char		*Stream, 
	const char		*Match
)
{
	guard(PeekCMD);
	
	while( (*Stream==' ')||(*Stream==9) )
		Stream++;

	if( strnicmp(Stream,Match,strlen(Match))==0 )
	{
		Stream += strlen(Match);
		if (!isalnum(*Stream)) return 1; // Success.
		else return 0; // Only found partial match.
	}
	else return 0; // No match.
	unguard;
}

//
// Get a line of Stream (everything up to, but not including, CR/LF.
// Returns 0 if ok, nonzero if at end of stream and returned 0-length string.
//
int UNENGINE_API GetLINE
(
	const char		**Stream,
	char			*Result,
	int				MaxLen,
	int				Exact
)
{
	guard(GetLINE);
	int GotStream=0;
	int IsQuoted=0;
	int Ignore=0;

	*Result=0;
	while( **Stream!=0 && **Stream!=10 && **Stream!=13 && --MaxLen>0 )
	{
		if( !IsQuoted && **Stream==';' )
			// Start of comments.
			Ignore = 1;
		
		if( !IsQuoted && **Stream=='|' )
			// Command chaining.
			break;

		// Check quoting.
		IsQuoted = IsQuoted ^ (**Stream==34);
		GotStream=1;

		if( !Ignore )
			// Got stuff.
			*(Result++) = *((*Stream)++);
		else
			(*Stream)++;
	}
	if( Exact )
	{
		// Eat up exactly one CR/LF.
		if( **Stream == 13 )
			(*Stream)++;
		if( **Stream == 10 )
			(*Stream)++;
	}
	else
	{
		// Eat up all CR/LF's.
		while( **Stream==10 || **Stream==13 || **Stream=='|' )
			(*Stream)++;
	}

	*Result=0;
	if ( **Stream!=0 || GotStream ) return 0; // Keep going.
	return 1; // At end of stream, and returned zero-length string.
	
	unguard;
}

//
// Get a boolean value.
//
int UNENGINE_API GetONOFF
(
	const char		*Stream, 
	const char		*Match, 
	int				*OnOff
)
{
	guard(GetONOFF);

	char TempStr[16];
	if( GetSTRING(Stream,Match,TempStr,16) )
	{
		if ((!stricmp(TempStr,"ON"))||(!stricmp(TempStr,"TRUE"))||(!stricmp(TempStr,"1"))) *OnOff = 1;
		else *OnOff = 0;
		return 1;
	}
	else return 0;
	unguard;
}

//
// Get an object.
//
int UNENGINE_API GetOBJ( const char *Stream, const char *Match, UClass *Type, UObject **DestRes )
{
	guard(GetOBJ);

	char TempStr[NAME_SIZE];
	if( !GetSTRING( Stream, Match, TempStr, NAME_SIZE ) )
		return 0; // Match not found.

	if( stricmp(TempStr,"NONE") != 0 )
	{		
		// Look this object up.
		UObject *Res;
		Res = GObj.FindObject( TempStr, Type, FIND_Optional );
		if( !Res )
			return 0;

		*DestRes = Res;
	}
	else
	{
		// Object name "None" was explicitly specified.
		*DestRes = NULL;
	}
	return 1;
	unguard;
}

//
// Get a name.
//
int UNENGINE_API GetNAME
(
	const char		*Stream, 
	const char		*Match, 
	FName			*Name
)
{
	guard(GetNAME);
	char TempStr[NAME_SIZE];

	if( !GetSTRING(Stream,Match,TempStr,NAME_SIZE) )
		return 0; // Match not found.

	*Name = FName( TempStr, FNAME_Add );

	return 1;
	unguard;
}

//
// Gets a "BEGIN" string.  Returns 1 if gotten, 0 if not.
// If not gotten, doesn't affect anything.
//
int UNENGINE_API GetBEGIN
(
	const char	**Stream, 
	const char	*Match
)
{
	guard(GetBEGIN);

	const char *Original = *Stream;
	if (GetCMD (Stream,"BEGIN") && GetCMD (Stream,Match)) return 1; // Gotten.
	*Stream = Original;
	return 0;

	unguard;
}

//
// Gets an "END" string.  Returns 1 if gotten, 0 if not.
// If not gotten, doesn't affect anything.
//
int UNENGINE_API GetEND
(
	const char	**Stream, 
	const char	*Match
)
{
	guard(GetEND);

	const char *Original = *Stream;
	if (GetCMD (Stream,"END") && GetCMD (Stream,Match)) return 1; // Gotten.
	*Stream = Original;
	return 0;

	unguard;
}

//
// Get next command.  Skips past comments and cr's
//
void UNENGINE_API GetNEXT
(
	const char **Stream
)
{
	guard(GetNEXT);

	// Skip over spaces, tabs, cr's, and linefeeds.
	SkipJunk:
	while ((**Stream==' ')||(**Stream==9)||(**Stream==13)||(**Stream==10)) (*Stream)++;

	if( **Stream==';' )
	{
		// Skip past comments.
		while ((**Stream!=0)&&(**Stream!=10)&&(**Stream!=13)) (*Stream)++;
		goto SkipJunk;
	}

	// Upon exit, *Stream either points to valid Stream or a zero character.
	unguard;
}

//
// See if a command-line parameter exists in the stream.
//
int UNENGINE_API GetParam( const char *Stream,const char *Param )
{
	guard(GetParam);
	const char *Start = mystrstr(Stream,Param);
	if( Start && (Start>Stream) )
		return (Start[-1]=='-') || (Start[-1]=='/');
	else return 0;
	unguard;
}

//
// Skip to the end of this line.
//
void UNENGINE_API SkipLINE( const char **Stream )
{
	guard(SkipLINE);

	// Skip over spaces, tabs, cr's, and linefeeds.
	while ((**Stream!=10)&&(**Stream!=13)&&(**Stream!=0)) (*Stream)++;
	GetNEXT(Stream);
	unguard;
}

//
// Grab the next space-delimited string from the input stream.
// If quoted, gets entire quoted string.
//
int UNENGINE_API GrabSTRING( const char *&Str,char *Result, int MaxLen )
{
	guard(GrabSTRING);
	int Len=0;

	// Skip spaces and tabs.
	while( *Str==' ' || *Str==9 )
		Str++;

	if( *Str == 34 )
	{
		// Get quoted string.
		Str++;
		while( *Str && *Str!=34 && (Len+1)<MaxLen )
		{
			char c = *Str++;
			if( c == '\\' )
			{
				// Get escaped character.
				c = *Str++;
				if( !c )
					break;
			}
			if( (Len+1)<MaxLen )
				Result[Len++] = c;
		}
		if( *Str==34 )
			Str++;
	}
	else
	{
		// Get unquoted string.
		while( *Str && *Str!=' ' && *Str!=9 )
			if( (Len+1)<MaxLen )
				Result[Len++] = *Str++;
	}
	Result[Len]=0;
	return Len!=0;
	unguard;
}


/*-----------------------------------------------------------------------------
	Setters.
	These don't validate lengths so you need to call them with a big buffer.
-----------------------------------------------------------------------------*/

//
// Output a vector.
//
UNENGINE_API char *SetFVECTOR(char *Dest,const FVector *FVector)
{
	guard(SetFVECTOR);
	sprintf (Dest,"%+013.6f,%+013.6f,%+013.6f",FVector->X,FVector->Y,FVector->Z);
	return Dest;
	unguard;
}

//
// Output a fixed point vector.
//
UNENGINE_API char *SetFIXFVECTOR(char *Dest,const FVector *FVector)
{
	guard(SetFIXFVECTOR);
	sprintf (Dest,"%+013.6f,%+013.6f,%+013.6f",FVector->X/65536.0,FVector->Y/65536.0,FVector->Z/65536.0);
	return Dest;
	unguard;
}

//
// Output a rotation.
//
UNENGINE_API char *SetROTATION(char *Dest,const FRotation *Rotation)
{
	guard(SetROTATION);
	sprintf (Dest,"%i,%i,%i",Rotation->Pitch,Rotation->Yaw,Rotation->Roll);
	return Dest;
	unguard;
}

//
// Output a scale.
//
UNENGINE_API char *SetFSCALE(char *Dest,const FScale *FScale)
{
	guard(SetFSCALE);
	sprintf (Dest,"X=%+013.6f Y=%+013.6f Z=%+013.6f S=%+013.6f AXIS=%i",FScale->Scale.X,FScale->Scale.Y,FScale->Scale.Z,FScale->SheerRate,FScale->SheerAxis);
	return Dest;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
