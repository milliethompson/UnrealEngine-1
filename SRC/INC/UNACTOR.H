/*=============================================================================
	UnActor.h: AActor class inlines.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * Aug 30, 1996: Mark added PLevel
		* Oct 19, 1996: Tim redesigned to eliminate redundency
=============================================================================*/

#ifndef _INC_UNACTOR
#define _INC_UNACTOR

/*-----------------------------------------------------------------------------
	AActor inlines.
-----------------------------------------------------------------------------*/

//
// Return whether this actor's collision cyllinder overlaps
// with the other actor's.
//
inline BOOL AActor::IsOverlapping( const AActor *Other ) const
{
	guardSlow(AActor::IsOverlapping);
	debugInput(Other!=NULL);

	return
		Square(Location.X      - Other->Location.X)
	+	Square(Location.Y      - Other->Location.Y)
	<	Square(CollisionRadius + Other->CollisionRadius) 
	&&	Square(Location.Z      - Other->Location.Z)
	<	Square(CollisionHeight + Other->CollisionHeight);

	unguardSlow;
}

//
// See if this is the owner of the specified actor.
//
inline BOOL AActor::IsOwnedBy( const AActor *TestActor ) const
{
	guardSlow(AActor::IsOwnedBy);
	for( const AActor *Arg=this; Arg; Arg=Arg->Owner )
	{
		if( TestActor == Arg )
			return 1;
	}
	return 0;
	unguardSlow;
}

//
// If this actor is a player, return it as a Pawn.
// Otherwise return NULL.
//
inline APawn *AActor::GetPlayer() const
{
	guardSlow(AActor::GetPlayer);

	// Only descendents of the pawn class may be players.
	if( !IsA("Pawn") )
		return NULL;

	// Players must have cameras.
	if( ((APawn*)this)->Camera == NULL )
		return NULL;

	// This is a player.
	return (APawn*)this;

	unguardSlow;
}

//
// Return whether this actor looks like a player.
//
inline BOOL AActor::IsPlayer() const
{
	guardSlow(AActor::IsPlayer);

	// Only descendents of the pawn class may be players.
	if( !IsA("Pawn") )
		return 0;

	// Players must have cameras.
	return ((APawn*)this)->bIsPlayer;

	unguardSlow;
}

//
// Determine if BlockingActor should block actors of the given class.
// This routine needs to be reflexive or else it will create funky
// results, i.e. A->IsBlockedBy(B) <-> B->IsBlockedBy(A).
//
inline BOOL AActor::IsBlockedBy( const AActor *Other ) const
{
	guardSlow(AActor::IsBlockedBy);
	return
		(       GetPlayer() ? Other->bBlockPlayers : Other->bBlockActors)
	&&	(Other->GetPlayer() ?        bBlockPlayers :        bBlockActors);
	unguardSlow;
}

//
// Return the level of an actor.
//
inline class ULevel* AActor::GetLevel() const
{
	return XLevel;
}

inline void AActor::SetCollision
(
	BOOL NewCollideActors,
	BOOL NewBlockActors,
	BOOL NewBlockPlayers
)
{
	guard(AActor::SetCollision);

	// Untouch this actor.
	if( bCollideActors )
		GetLevel()->Hash.RemoveActor( this );

	// Set properties.
	bCollideActors = NewCollideActors;
	bBlockActors   = NewBlockActors;
	bBlockPlayers  = NewBlockPlayers;

	// Touch this actor.
	if( bCollideActors )
		GetLevel()->Hash.AddActor( this );

	unguard;
}

/*-----------------------------------------------------------------------------
	Collision.
-----------------------------------------------------------------------------*/

inline void AActor::GetCoordinates
(
	FCoords &Coords,
	FCoords &Uncoords
) const
{
	guardSlow(AActor::GetCoordinates);
	FCalcWorldCoords( Location, Rotation, Coords, Uncoords );
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	AActor audio.
-----------------------------------------------------------------------------*/

//
// Play a sound located at the actor.
//
inline void AActor::MakeSound( USound *Sound, FLOAT Radius, FLOAT Volume, FLOAT Pitch )
{
	guardSlow(AActor::MakeSound);
	if( Sound != NULL )
		GAudio.PlaySfxOrigined( &Location, Sound, Radius, Volume, Pitch );
	unguardSlow;
}

//
// Play a disembodied sound audible only to the owning player.
//
inline void AActor::PrimitiveSound( USound *Sound, FLOAT Volume, FLOAT Pitch )
{
	guardSlow(AActor::PrimitiveSound);
	if( Sound != NULL )
		GAudio.PlaySfxPrimitive( Sound, Volume, Pitch );
	unguardSlow;
}

//
// Set this actor's ambient sound.
//
inline void AActor::SetAmbientSound( USound *NewAmbient )
{
	guardSlow(AActor::SetAmbientSound);

	// Stop current ambient sound if one is playing.
	if( AmbientSound != NULL )
		GAudio.SfxStopActor( GetIndex() );

	// Set new ambient sound.
	AmbientSound = NewAmbient;
	if( AmbientSound != NULL )
	{
		GAudio.PlaySfxLocated
		(
			&Location,
			AmbientSound,
			GetIndex(),
			4.0 * WorldSoundRadius(),
			SoundVolume * 16.0 / 256.0,
			SoundPitch / 64.0
		);
	}
	unguardSlow;
}

//
// Update the actor's audio.
//
inline void AActor::UpdateSound()
{
	guardSlow(AActor::UpdateSound);
	GAudio.SfxMoveActor
	(
		GetIndex(),
		&Location
		//SoundVolume * 16.0 / 256.0
	);
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNACTOR
