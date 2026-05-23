/*=============================================================================
	UnMsgPar.h: Actor message parameter struct definitions

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNMSGPAR
#define _INC_UNMSGPAR

/*-----------------------------------------------------------------------------
	Actor message parameter structs.

	Note: Any messages that contain booleans must contain one DWORD per
	boolean value.  A zero value indicates false, nonzero indicates true.
-----------------------------------------------------------------------------*/

// Child structs of FActorMessageParms are all sent to actors using the 
// "void *Parms" pointer in various actor messages.
// Refer to UnNames.h for the parameter types expected for particular messages.
struct PMessageParms
{};

// Actor identification.
struct PActor : public PMessageParms
{
	AActor *Actor;
	PActor( AActor *InActor )
	: Actor( InActor )
	{}
	static PActor* Get(void  *Parms)
	{
		return (PActor*)Parms;
	}
};

// Player tick.
struct PPlayerTick : public PMessageParms
{
	FLOAT			DeltaTime;
	DWORD			Buttons[BUT_MAX];
	FLOAT			Axis[AXIS_MAX];
	static PPlayerTick* Get( void *Parms )
	{
		return (PPlayerTick*)Parms;
	}
	PPlayerTick( FLOAT InDeltaTime )
	:	DeltaTime( InDeltaTime )
	{
		for( int i=0; i<BUT_MAX;  i++ ) Buttons[i]=0;
		for(     i=0; i<AXIS_MAX; i++ ) Axis   [i]=0.0;
	}
};

//Noise
struct PNoise : public PMessageParms
{
	FLOAT			Loudness;
	AActor		* Actor;
	static PNoise* Get( void *Parms )
	{
		return (PNoise*)Parms;
	}
	PNoise(FLOAT InLoudness, AActor *InActor) :
		Loudness(InLoudness), Actor(InActor)
	{}
};

// Server custom message.
struct PServer : public PMessageParms
{
	FLOAT F;
	FName N;
	CHAR  S[255];
	PServer()
	:	F(0.0), N(NAME_None)
	{
		S[0]=0;
	}
	static PServer* Get(void *Parms)
	{
		return (PServer*)Parms;
	}
};

// Timer tick.
struct PTick : public PMessageParms
{
	FLOAT DeltaTime;
	static PTick* Get( void *Parms )
	{
		return (PTick*)Parms;
	}
	PTick( FLOAT InDeltaTime )
	:	DeltaTime( InDeltaTime )
	{}
};

// View calculation information structure, sent to actors
// with active cameras immediately before rendering, to allow the actors
// to move the viewpoint for effects such as bouncing the player's view
// while she's walking, or implementing a behind-the-player view.
struct PCalcView : public PMessageParms
{
	FVector	  CameraLocation;
	FRotation CameraRotation;
	static PCalcView* Get( void *Parms )
	{
		return (PCalcView*)Parms;
	}
	PCalcView( FVector &InLocation, FRotation &InRotation )
	:	CameraLocation(InLocation), CameraRotation(InRotation)
	{}
};

// Boolean.
struct PBoolean : public PMessageParms
{
	DWORD bBoolean;
	static PBoolean* Get( void *Parms )
	{
		return (PBoolean*)Parms;
	}
	PBoolean( FLOAT bInBoolean )
	:	bBoolean( bInBoolean )
	{}
};

// Integer.
struct PInt : public PMessageParms
{
	INT Int;
	static PInt* Get( void *Parms )
	{
		return (PInt*)Parms;
	}
	PInt( INT InInt )
	:	Int( InInt )
	{}
};

// Float.
struct PFloat : public PMessageParms
{
	FLOAT fl;
	static PFloat* Get( void *Parms )
	{
		return (PFloat*)Parms;
	}
	PFloat( FLOAT Infl )
	:	fl( Infl )
	{}
};

//Vector
struct PVector : public PMessageParms
{
	FVector Vec;
	static PVector* Get( void *Parms )
	{
		return (PVector*)Parms;
	}
	PVector( FVector &InVec )
	:	Vec( InVec )
	{}
};

// KeyMoveTo.
struct PKeyMove : public PMessageParms
{
	BYTE KeyNum; // Keyframe number to move to.
	PKeyMove( BYTE InKeyNum )
	: KeyNum( InKeyNum )
	{}
	static PKeyMove* Get( void *Parms )
	{
		return (PKeyMove*)Parms;
	}
};

// A reference to a name.
struct PName : public PMessageParms
{
	FName Name;
	PName( FName InName )
	: Name( InName )
	{}
	static PName *Get( void *Parms )
	{
		return (PName*)Parms;
	}
};

//
// Information about a lost reference.
//
struct PLostReference : public PMessageParms
{
	AActor		   *Actor;
	FName		   VarName;
	INT			   ArrayIndex;
	PLostReference *Next;
	PLostReference( AActor *InActor, FName InVarName, INT InArrayIndex, PLostReference *InNext )
	:	Actor		( InActor )
	,	VarName		( InVarName )
	,	ArrayIndex	( InArrayIndex )
	,	Next		( InNext )
	{}
	static PLostReference* Get( void *Parms )
	{
		return (PLostReference*)Parms;
	}
};

//
// Inventory view calculation.
//
struct PInvCalcView : public PMessageParms
{
	FVector Location;
	FRotation Rotation;
	PInvCalcView( FVector &InLocation, FRotation &InRotation )
	:	Location	( InLocation )
	,	Rotation	( InRotation )
	{}
	static PInvCalcView* Get( void *Parms )
	{
		return (PInvCalcView*)Parms;
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNMSGPAR
