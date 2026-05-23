/*=============================================================================
	UnPrim.h: Unreal UPrimitive definition.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNPRIM
#define _INC_UNPRIM

/*-----------------------------------------------------------------------------
	FCheckResult.
-----------------------------------------------------------------------------*/

//
// Results of an actor check.
//
struct FIteratorActorList : public FIteratorList
{
	// Variables.
	AActor*		Actor;		// Actor which was hit, or NULL=none.

	// Functions.
	FIteratorActorList()
	{}
	FIteratorActorList( FIteratorActorList* InNext, AActor* InActor )
	:	FIteratorList	(InNext)
	,	Actor			(InActor)
	{}
	FIteratorActorList* GetNext()
	{ return (FIteratorActorList*) Next; }
};

//
// Results from a collision check.
//
struct FCheckResult : public FIteratorActorList
{
	// Variables.
	FVector		Location;   // Location of the hit in coordinate system of the returner.
	FVector		Normal;     // Normal vector in coordinate system of the returner. Zero=none.
	UPrimitive*	Primitive;  // Actor primitive which was hit, or NULL=none.
	FLOAT       Time;       // Time until hit, if line check.
	INDEX		Item;       // Primitive data item which was hit, INDEX_NONE=none.

	// Functions.
	FCheckResult()
	{}
	FCheckResult( FLOAT InTime, FCheckResult* InNext=NULL )
	:	FIteratorActorList( InNext, NULL )
	,	Location	(0,0,0)
	,	Normal		(0,0,0)
	,	Primitive	(NULL)
	,	Time		(InTime)
	,	Item		(INDEX_NONE)
	{}
	FCheckResult*& GetNext()
	{ return *(FCheckResult**)&Next; }
	friend INT Compare( const FCheckResult &A, const FCheckResult &B )
	{ return A.Time - B.Time; }
};

/*-----------------------------------------------------------------------------
	UPrimitive.
-----------------------------------------------------------------------------*/

//
// UPrimitive, the base class of geometric entities capable of being
// rendered and collided with.
//
class UNENGINE_API UPrimitive : public UObject
{
	DECLARE_CLASS(UPrimitive,UObject,NAME_Primitive,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Variables.
	FBoundingVolume LocalBound;

	// UObject interface.
	void InitHeader()
	{
		guard(UPrimitive::InitHeader);

		// Call parent.
		UObject::InitHeader();

		// Init variables.
		LocalBound = FBoundingVolume(0);

		unguard;	
	}
	void SerializeHeader( FArchive &Ar )
	{
		guard(UPrimitive::SerializeHeader);

		// UObject references.
		UObject::SerializeHeader(Ar);

		// UPrimitive references.
		Ar << LocalBound;

		unguard;
	}

	// UPrimitive collision interface.
	virtual INT PointCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		FVector			Location,
		FVector			Extent,
		DWORD           ExtraNodeFlags
	);
	virtual INT LineCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		FVector			Start,
		FVector			End,
		FVector			Extent,
		DWORD           ExtraNodeFlags
	);
	virtual FBoundingBox GetRenderBoundingBox( const AActor *Owner ) const;
	virtual FBoundingBox GetCollisionBoundingBox( const AActor *Owner ) const;
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNPRIM
