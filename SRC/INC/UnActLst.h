/*=============================================================================
	UnActLst.h: Actor list object class definition.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * Aug 30, 1996: Mark added PLevel
		* Oct 19, 1996: Tim redesigned to eliminate redundency
=============================================================================*/

#ifndef _INC_UNACTLST
#define _INC_UNACTLST

/*-----------------------------------------------------------------------------
	FCollisionHash.
-----------------------------------------------------------------------------*/

//
// A collision hash table.
//
class FCollisionHash
{
public:
	// Constants.
	enum { NUM_BUCKETS = 16384             };
	enum { BASIS_BITS  = 8                 };
	enum { BASIS_MASK  = (1<<BASIS_BITS)-1 };
	enum { GRAN_XY     = 256               };
	enum { GRAN_Z      = 256               };
	enum { XY_OFS      = 65536             };
	enum { Z_OFS       = 65536             };

	// Linked list item.
	struct FActorLink
	{
		// Varibles.
		AActor          *Actor;     // The actor.
		FActorLink      *Next;      // Next link belonging to this collision bucket.
		INT				iLocation;  // Based hash location.

		// Functions.
		FActorLink( AActor *InActor, FActorLink *InNext, INT iInLocation )
		:	Actor		(InActor)
		,	Next        (InNext)
		,	iLocation	(iInLocation)
		{}
	} *Hash[NUM_BUCKETS];

	// Statics.
	static INT InitializedBasis;
	static INT CollisionTag;
	static INT HashX[NUM_BUCKETS];
	static INT HashY[NUM_BUCKETS];
	static INT HashZ[NUM_BUCKETS];

	// Variables.
	BOOL CollisionInitialized;

	// Low-level actor-actor collision checking functions.
	void  Init();
	void  Exit();
	void  Tick();
	void  AddActor( AActor *Actor );
	void  RemoveActor( AActor *Actor );
	int   LineCheck( FCheckResult *Hits, INT ListMax, const FVector &Start, const FVector &End, FLOAT Radius, FLOAT Height );
	int   PointCheck( const FVector &Location, FLOAT Radius, FLOAT Height, AActor **List, INT ListMax );
	void  GetActorExtent( AActor *Actor, INT &iX0, INT &iX1, INT &iY0, INT &iY1, INT &iZ0, INT &iZ1 );
	void  GetHashIndices( FVector Location, INT &iX, INT &iY, INT &iZ )
	{
		iX = Clamp(ftoi( (Location.X + XY_OFS) * (1.0/GRAN_XY) ), 0, (int)NUM_BUCKETS);
		iY = Clamp(ftoi( (Location.Y + XY_OFS) * (1.0/GRAN_XY) ), 0, (int)NUM_BUCKETS);
		iZ = Clamp(ftoi( (Location.Z + Z_OFS ) * (1.0/GRAN_Z ) ), 0, (int)NUM_BUCKETS);
	}
	FActorLink *&GetHashLink( INT iX, INT iY, INT iZ, INDEX &iLocation )
	{
		iLocation = iX + (iY << BASIS_BITS) + (iZ << (BASIS_BITS*2));
		return Hash[ HashX[iX] ^ HashY[iY] ^ HashZ[iZ] ];
	}
};

/*-----------------------------------------------------------------------------
	Global actor and class functions
-----------------------------------------------------------------------------*/

//
// Actor import/export functions (UnActLst.cpp).
//
UNENGINE_API void ExportActor
(
	UClass*			Class,
	BYTE*const*		ActorBin,
	FOutputDevice&	Out,
	FName			PropertyName,
	int				Indent,
	int				Descriptive,
	int				Flags, 
	UClass			*Diff,
	int				Objects, 
	int				ArrayElement,
	FName			Name,
	BYTE			WhichBins[PROPBIN_MAX]
);
UNENGINE_API void ExportMultipleActors
(
	ULevel*			Level,
	FOutputDevice&	Out,
	FName			Name,
	int				Indent,
	int				Descriptive,
	FName			Category,
	BYTE			WhichBins[PROPBIN_MAX]
);
UNENGINE_API const char *ImportActorProperties
(
	ULevel*			Level,
	UClass*			Class,
	BYTE**			ActorBins,
	const char*		Data,
	BYTE			WhichBins[PROPBIN_MAX],
	INT				ImportingFromFile
);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNACTLST
