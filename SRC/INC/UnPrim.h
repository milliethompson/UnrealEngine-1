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
// Results from a collision check.
//
struct FCheckResult
{
	// Variables.
	FVector		Location;   // Location of the hit in coordinate system of the returner.
	FVector		Normal;     // Normal vector in coordinate system of the returner. Zero=none.
	AActor*		Actor;		// Actor which was hit, or NULL=none.
	UPrimitive*	Primitive;  // Actor primitive which was hit, or NULL=none.
	INDEX		Item;       // Primitive data item which was hit, INDEX_NONE=none.
	FLOAT       Time;       // Time until hit, if line check.

	// Constructors.
	FCheckResult()
	{}
	FCheckResult( FLOAT InTime )
	:	Location	(0,0,0)
	,	Normal		(0,0,0)
	,	Actor		(NULL)
	,	Primitive	(NULL)
	,	Item		(INDEX_NONE)
	,	Time		(InTime)
	{}

	// QSort helper function.
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
	BOOL bCheckCollision;

	// UObject interface.
	void InitHeader()
	{
		guard(UPrimitive::InitHeader);

		// Call parent.
		UObject::InitHeader();

		// Init variables.
		bCheckCollision=0;
		LocalBound.Init();

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
		const FVector	&Location,
		DWORD			ExtraNodeFlags
	) {return 1;}
	virtual INT LineCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		const FVector	&Start,
		const FVector	&End,
		DWORD ExtraNodeFlags
	) {return 1;}
	virtual INT BoxPointCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		const FVector   &Point,
		FLOAT           Radius,
		FLOAT           Height,
		DWORD           ExtraNodeFlags
	) {return 1.0;}
	virtual INT BoxLineCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		const FVector   &Start,
		const FVector   &End,
		FLOAT           Radius,
		FLOAT           Height,
		DWORD           ExtraNodeFlags
	) {return 1.0;}
	virtual FBoundingRect GetBoundingRect( AActor *Owner )
	{ FBoundingRect R; R.Init(); return R; }
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNPRIM
