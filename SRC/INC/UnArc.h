/*=============================================================================
	UnArc.h: Unreal archive class.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNARC
#define _INC_UNARC

/*-----------------------------------------------------------------------------
	Forward declarations.
-----------------------------------------------------------------------------*/

class FName;
class UObject;

/*-----------------------------------------------------------------------------
	The archive class.
-----------------------------------------------------------------------------*/

//
// Archive class. This is used for serializing (loading and saving)
// things in an endian-neutral way.
//
class UNENGINE_API FArchive
{
public:
	// Process a byte stream. Must be overridden by implementor.
	virtual FArchive& Serialize(void *V, int Length) {return *this;}

	// Routines that must be overridden.
	virtual FArchive& operator<< (class FName &N) {return *this;}
	virtual FArchive& operator<< (class UObject *&Res) {return *this;}
	virtual void PreloadHeader(UObject *Res) {}
	virtual void Preload(UObject *Res) {}

	// Hardcoded datatype routines that may not be overridden.
	FArchive& ByteOrderSerialize(void *V, int Length)
	{
#if __INTEL__
		// Serialize forward.
		Serialize(V,Length);
#else
		// Serialize, reversing byte order (must be 8 or fewer bytes).
		for( int i=Length-1; i>=0; i-- )
			Serialize((BYTE*)V + i,1);
#endif
		return *this;
	}

	// Serialize a string (couldn't be implemented well as an operator).
	FArchive& String(char *S, int MaxLength)
	{
		checkInput(MaxLength>0);
		for( int Count=0; Count<MaxLength-1; Count++ )
		{
			Serialize(&S[Count],1);
			if( S[Count] == 0 )
				break;
		}
		S[Count] = 0;
		return *this;
	}

	// Constructor.
	FArchive()
	:	ArVer		(UNREAL_FILE_VERSION),
		ArIsLoading	(0),
		ArIsSaving	(0),
		ArForEdit	(1),
		ArForClient	(1),
		ArForServer	(1)
	{}

	// Status accessors.
	int Ver()			{return ArVer;}
	int IsLoading()		{return ArIsLoading;}
	int IsSaving()		{return ArIsSaving;}
	int ForEdit()		{return ArForEdit;}
	int ForClient()		{return ArForClient;}
	int ForServer()		{return ArForServer;}

protected:
	// Status variables.
	int ArVer;
	int ArIsLoading;
	int ArIsSaving;
	int ArForEdit;
	int ArForClient;
	int ArForServer;
};
/*-----------------------------------------------------------------------------
	FArchive operators.
-----------------------------------------------------------------------------*/

// BYTE.
static inline FArchive& operator<< (FArchive &Ar, BYTE &B)
	{return Ar.Serialize(&B,1);}

// CHAR.
static inline FArchive& operator<< (FArchive &Ar, CHAR &C)
	{return Ar.Serialize(&C,1);}

// WORD.
static inline FArchive& operator<< (FArchive &Ar, WORD &W)
	{return Ar.ByteOrderSerialize(&W,sizeof(W));}

// SWORD.
static inline FArchive& operator<< (FArchive &Ar, SWORD &S)
	{return Ar.ByteOrderSerialize(&S,sizeof(S));}

// DWORD.
static inline FArchive& operator<< (FArchive &Ar, DWORD &D)
	{return Ar.ByteOrderSerialize(&D,sizeof(D));}

// INT.
static inline FArchive& operator<< (FArchive &Ar, INT &I)
	{return Ar.ByteOrderSerialize(&I,sizeof(I));}

// FLOAT.
static inline FArchive& operator<< (FArchive &Ar, FLOAT &F)
	{return Ar.ByteOrderSerialize(&F,sizeof(F));}

// QWORD.
static inline FArchive& operator<< (FArchive &Ar, QWORD &Q)
	{return Ar.ByteOrderSerialize(&Q,sizeof(Q));}

// SQWORD.
static inline FArchive& operator<< (FArchive &Ar, SQWORD &S)
	{return Ar.ByteOrderSerialize(&S,sizeof(S));}

// Macro to serialize an enumeration as a byte.
#define ARCHIVE_ENUM_AS_BYTE(EnumType) \
	static inline FArchive& operator<<( FArchive &Ar, enum EnumType &E ) \
		{BYTE B = (BYTE)E; Ar << B; E=(EnumType)B; return Ar;}

// Serialize these enums as bytes.
ARCHIVE_ENUM_AS_BYTE(EPropertyBin);
ARCHIVE_ENUM_AS_BYTE(EPropertyType);
ARCHIVE_ENUM_AS_BYTE(ESheerAxis);
ARCHIVE_ENUM_AS_BYTE(ECsgOper);
ARCHIVE_ENUM_AS_BYTE(ENestType);
ARCHIVE_ENUM_AS_BYTE(EExprToken);
ARCHIVE_ENUM_AS_BYTE(ECodeToken);

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNARC
