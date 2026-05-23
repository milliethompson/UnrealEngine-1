/*=============================================================================
	UnParams.h: Parameter-parsing routines

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNPARAMS
#define _INC_UNPARAMS

/*-----------------------------------------------------------------------------
	Parameter parsing functions (UnParams.cpp).
-----------------------------------------------------------------------------*/

//
// Bogus FStreamParser class, simply contains all stream parsing routines.
//
class UNENGINE_API FStreamParser
{
	friend UNENGINE_API int GetBYTE 		(const char *Stream, const char *Match,	BYTE *Value);
	friend UNENGINE_API int GetCHAR 		(const char *Stream, const char *Match,	CHAR *Value);
	friend UNENGINE_API int GetWORD 		(const char *Stream, const char *Match,	WORD *Value);
	friend UNENGINE_API int GetINDEX		(const char *Stream, const char *Match,	INDEX *Value);
	friend UNENGINE_API int GetSWORD 		(const char *Stream, const char *Match,	SWORD *Value);
	friend UNENGINE_API int GetFLOAT 		(const char *Stream, const char *Match,	FLOAT *Value);
	friend UNENGINE_API int GetFVECTOR 		(const char *Stream, const char *Match,	FVector *Value);
	friend UNENGINE_API int GetFVECTOR 		(const char *Stream,					FVector *Value);
	friend UNENGINE_API int GetFIXFVECTOR	(const char *Stream,					FVector *Value);
	friend UNENGINE_API int GetFSCALE 		(const char *Stream,					FScale *Scale);
	friend UNENGINE_API int GetFROTATION	(const char *Stream, const char *Match,	FRotation *Rotation,int ScaleFactor);
	friend UNENGINE_API int GetFROTATION	(const char *Stream,					FRotation *Rotation,int ScaleFactor);
	friend UNENGINE_API int GetDWORD 		(const char *Stream, const char *Match,	DWORD *Value);
	friend UNENGINE_API int GetINT 			(const char *Stream, const char *Match,	INT *Value);
	friend UNENGINE_API int GetSTRING 		(const char *Stream, const char *Match,	char *Value,int MaxLen);
	friend UNENGINE_API int GetONOFF 		(const char *Stream, const char *Match,	int *OnOff);
	friend UNENGINE_API int GetOBJ 			(const char *Stream, const char *Match,	UClass *Type, class UObject **Res);
	friend UNENGINE_API int GetNAME			(const char *Stream, const char *Match,	class FName *Name);
	friend UNENGINE_API int GetParam		(const char *Stream, const char *Param);
	friend UNENGINE_API int GetCMD 			(const char **Stream, const char *Match);
	friend UNENGINE_API int GetLINE 		(const char **Stream, char *Result,int MaxLen,int Exact=0);
	friend UNENGINE_API int GetBEGIN 		(const char **Stream, const char *Match);
	friend UNENGINE_API int GetEND			(const char **Stream, const char *Match);
	friend UNENGINE_API void GetNEXT		(const char **Stream);
	friend UNENGINE_API int  GrabSTRING		(const char *&Str,char *Result, int MaxLen);
	friend UNENGINE_API int  PeekCMD 		(const char *Stream, const char *Match);
	friend UNENGINE_API void SkipLINE		(const char **Stream);
	friend UNENGINE_API char *SetFVECTOR	(char *Dest,const FVector   *Value);
	friend UNENGINE_API char *SetFIXFVECTOR(char *Dest,const FVector   *Value);
	friend UNENGINE_API char *SetROTATION	(char *Dest,const FRotation *Rotation);
	friend UNENGINE_API char *SetFSCALE		(char *Dest,const FScale	*Scale);
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNPARAMS
