/*=============================================================================
	UnLevAct.cpp: Level actor functions

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Revision history:
	* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	Level actor messaging.
-----------------------------------------------------------------------------*/

//
// Send a message to all actors which match ALL of the following tests:
//
// ThisActor = Actor to send to, or NULL = all actors.
// Name      = Name of actor, NAME_None = all names.
// Class     = Actor's class or its arbitrary-level superclass, or NULL = all classes.
//
void ULevel::SendEx( FName Msg, PMessageParms *Parms, AActor *ThisActor, FName Name, UClass *Class )
{
	guard(ULevel::SendEx);
	checkState(IsLocked());

	for( int iActor=0; iActor<Num; iActor++ )
	{
		AActor *Actor = Element(iActor);
		if
		(	( Actor )
		&&	( ThisActor==NULL    || Actor == ThisActor ) 
        &&  ( Name==NAME_None    || Name  == Actor->Tag )
        &&  ( Class==NULL        || Class == Actor->GetClass() ) )
        {
            Actor->Process( Msg, Parms );
        }
    }
	unguardf(("(%s)",Msg()));
}

/*-----------------------------------------------------------------------------
	Level actor possession.
-----------------------------------------------------------------------------*/

//
// Hook a camera or user up to an actor.  Updates the actor.  You must update
// the camera or user with the actor's index yourself.  This only fails if the 
// actor is already hooked up to a camera or user; actors can't refuse 
// possession.  Returns 1 if success, 0 if failure.
//
int ULevel::PossessActor( APawn *Actor, UCamera *Camera )
{
	guard(ULevel::PossessActor);
	checkState(IsLocked());
	checkInput(Actor!=NULL);

	if( !Actor->IsA("Pawn") )
	{
		debugf( "Possess non-pawn failed" );
		return 0;
	}

	if( Actor->Camera )
	{
		debugf("Possess occupied pawn failed");
		return 0;
	}

	if( Camera->Actor )
		UnpossessActor( Camera->Actor );

	Actor->Camera = Camera;
	Camera->Actor = Actor;
	Actor->Process( NAME_Possess, NULL );

	SetActorZone( Actor, 0, 1 );
	debugf("Possess '%s %s' succeeded", Actor->GetClassName(), Actor->GetName() );

	// Set the new actor's game rendering properties.
	Actor->ShowFlags	= GGfx.DefaultCameraFlags;
	Actor->RendMap		= GGfx.DefaultRendMap;
	Actor->OrthoZoom	= 40000.0;
	Actor->Misc1		= 0;
	Actor->Misc2		= 0;

	return 1;
	unguard;
}

//
// Unpossess an actor, unhooking the actor's camera or user.  You must update
// the possessor yourself.  Returns the old posessor, or NULL if none.
//
UCamera *ULevel::UnpossessActor( APawn *Actor )
{
	guard(ULevel::UnpossessActor);
	checkState(IsLocked());
	checkInput(Actor!=NULL);

	// Make sure it's a player.
	if( !Actor->Camera )
		return NULL;

	// Remember old possessing camera.
	UCamera	*OldPossessor = Actor->Camera;

	// Unpossess.
	Actor->Camera = NULL;

	// Inform actor of the unpossession.
	Actor->Process( NAME_UnPossess, NULL );

	// Reset its zone.
	SetActorZone( Actor, 0, 1 );

	return OldPossessor;
	unguard;
}

/*-----------------------------------------------------------------------------
	Level actor management.
-----------------------------------------------------------------------------*/

//
// Create a new actor and sends it the Spawned message. Returns the
// new actor, or NULL if the actor could not be spawned. See the code below
// for criteria which causes spawning to fail.
//
AActor *ULevel::SpawnActor
(
	UClass*			Class,
	AActor*			Owner,
	FName			ActorName,
	FVector			Location,
	FRotation		Rotation,
	AActor*			Template
)
{
	guard(ULevel::SpawnActor);
	checkState(IsLocked());

	// Make sure this class is spawnable.
	if( !Class )
	{
		debugf( LOG_Problem, "SpawnActor failed because no class was specified" );
		return NULL;
	}
	if( Class->ClassFlags & CLASS_Abstract )
	{
		debugf( LOG_Problem, "SpawnActor failed because class %s is abstract", Class->GetName() );
		return NULL;
	}
	else if( !Class->IsChildOf("Actor") )
	{
		debugf( LOG_Problem, "SpawnActor failed because %s is not an actor class", Class->GetName() );
		return NULL;
	}

	// Setup.
	INDEX iActor;
	if( ActorName==NAME_None )
		ActorName = Class->GetFName();

	// Find an available actor index.
	guard(1);
	for( iActor=0; iActor<Num; iActor++ )
		if( Element(iActor)==NULL )
			break;
	if( iActor==Num && iActor<Max )
		iActor = Num++;
	else if( iActor == Max )
		iActor = Add();
	unguard;

	// Use class's default actor as a template.
	if( !Template )
		Template = &Class->GetDefaultActor();
	checkState(Template!=NULL);

	// Make sure actor will fit at desired location, and adjust location if necessary.
	if( Template->bCollideWorld || Template->bCollideWhenPlacing )
	{
		if( !FindSpot( Template->GetCollisionExtent(), Location, 0 ) )
		{
			debugf( "SpawnActor %s failed because FindSpot didn't fit", Class->GetName() );
			return NULL;
		}
	}

	// Save previous empty entry, if transactional.
	ModifyItem( iActor );

	// Create an actor.
    AActor *Actor = Element(iActor) = (AActor*)GObj.CreateObject(ActorName(),Class,CREATE_MakeUnique);
	memcpy
	( 
		(BYTE*)Actor                        + sizeof(UObject),
		(BYTE*)Template                     + sizeof(UObject),
		Class->Bins[PROPBIN_PerObject]->Num - sizeof(UObject)
	);
	Num = ::Max( Num, iActor + 1 );

	// Set base actor properties.
	Actor->Tag					= ActorName;
	Actor->ZoneNumber			= 0;
	Actor->Zone	= Actor->Level  = GetLevelInfo();
	Actor->Hash					= NULL;
	Actor->XLevel				= this;

	// Actors spawned during gameplay must be nonstatic.
	if( GetState() == LEVEL_UpPlay )
		Actor->bStatic = 0;

	// Set the actor's location and rotation.
	Actor->Location = Location;
	Actor->Rotation = Rotation;
	if( Actor->bCollideActors && Hash.CollisionInitialized  )
		Hash.AddActor( Actor );

	// Remove the actor's brush, if it has one, because moving brushes
	// are not duplicatable.
	if( Actor->Brush )
		Actor->Brush = NULL;

	// Init scripting before sending any messages.
	if( GetState() == LEVEL_UpPlay )
		Actor->BeginExecution();

	// Set owner.
	Actor->SetOwner( Owner );

	// Send Spawned.
	Actor->Process( NAME_Spawned, NULL );

	// Send PreBeginPlay.
	if( GetState() == LEVEL_UpPlay )
		Actor->Process( NAME_PreBeginPlay, NULL );

	// Send BeginPlay.
	if( GetState() == LEVEL_UpPlay )
		Actor->Process( NAME_BeginPlay, NULL );

	// Set the actor's zone.
	SetActorZone( Actor );

	// Send PostBeginPlay.
	if( GetState() == LEVEL_UpPlay ) 
		Actor->Process( NAME_PostBeginPlay, NULL );

	// Success: Return the actor.
	return Actor;
	unguard;
}

//
// Destroy an actor.
// Returns 1 if destroyed, 0 if it couldn't be destroyed.
//
// What this routine does:
// * Remove the actor from the actor list.
// * Generally cleans up the engine's internal state.
//
// What this routine does not do, but is done in ULevel::Tick instead:
// * Removing references to this actor from all other actors.
// * Killing the actor resource.
//
// This routine is set up so that no problems occur even if the actor
// being destroyed inside its recursion stack.
//
int ULevel::DestroyActor( AActor *ThisActor )
{
	guard(ULevel::DestroyActor);
	checkState(IsLocked());
	checkInput(ThisActor!=NULL);

	// During gameplay, don't allow deletion of actors with
	// bStatic, bNoDelete, or bDeleteMe.
	if( GetState() == LEVEL_UpPlay )
	{
		// Can't kill bStatic and bNoDelete actors during play.
		if( ThisActor->bStatic || ThisActor->bNoDelete )
			return 0;

		// If already on list to be deleted, pretend the call was successful.
		if( ThisActor->bDeleteMe )
			return 1;
	}

	// Get index.
	INDEX iActor = GetActorIndex(ThisActor);
	ModifyItem( iActor );
	if( GTrans ) GTrans->NoteResHeader( ThisActor );

	// Stop any sounds this actor is playing.
	guard(1);
	GAudio.SfxStopActor( ThisActor->GetIndex() );
	unguard;

	// Remove from base.
	guard(2);
	if( ThisActor->Base )
		ThisActor->SetBase( NULL );
	unguard;

	// Remove from world collision hash.
	guard(3);
	if( ThisActor->bCollideActors && Hash.CollisionInitialized  )
		Hash.RemoveActor( ThisActor );
	Hash.CheckActorNotReferenced( ThisActor );
	unguard;

	// Remove the actor from the actor list.
	guard(4);
	Element(iActor) = NULL;
	ThisActor->bDeleteMe = 1;
	unguard;

	// Tell all touching actors they're no longer touching this.
	guard(5);
	for( int iActor=0; iActor<Num; iActor++ )
		for( int j=0; j<ARRAY_COUNT(ThisActor->Touching); j++ )
			if( Element(iActor) && Element(iActor)->Touching[j]==ThisActor )
				ThisActor->EndTouch( Element(iActor), 1 );
	unguard;

	// Tell this actor it's about to be destroyed.
	guard(6);
	ThisActor->Process( NAME_Destroyed, NULL );
	unguard;

	// Recursively destroy all child actors. We do this after sending the Destroyed
	// message so that, for example, a monster can unlink his inventory items and chuck
	// them on the floor.
	guard(7);
	for( int iActor = 0; iActor<Num; iActor++ )
		if( Element(iActor) && Element(iActor)->Owner==ThisActor )
			DestroyActor( Element(iActor) );
	unguard;

	// If this actor has an owner, notify it that is has lost a child.
	guard(8);
	if( ThisActor->Owner )
		ThisActor->Owner->Process( NAME_LostChild, &PActor(ThisActor) );
	unguard;

	// Scrap the actor's camera, if any:
	guard(9);
	APawn *Player = ThisActor->GetPlayer();
	if( Player )
		Player->Camera->Kill();
	unguard;

	// Cleanup.
	guard(10);
	if( GetState() == LEVEL_UpPlay )
	{
		// During play, just add to delete-list list and destroy when level is unlocked.
		ThisActor->Deleted = FirstDeleted;
		FirstDeleted       = ThisActor;
	}
	else
	{
		// Destroy them now.
		CleanupDestroyed();
	}
	unguard;

	// Return success.
	return 1;
	unguard;
}

//
// Cleanup destroyed actors.
// During gameplay, called in ULevel::Unlock.
// During editing, called after each actor is deleted.
//
void ULevel::CleanupDestroyed()
{
	guard(ULevel::CleanupDestroyed);

	// Note: You could trick this into looping infinitely by spawning and deleting an
	// actor inside a LostReference routine. Don't do that.
	while( FirstDeleted != NULL )
	{
		// Track list of references that were destroyed.
		FMemMark LostMark(GMem);
		PLostReference *Ref = NULL;

		// Remove all references to actors tagged for deletion.
		guard(CleanupRefs);
		for( INDEX iActor=0; iActor<Num; iActor++ )
		{
			AActor *Actor = Element(iActor);
			if( Actor )
			{
				checkState(!Actor->bDeleteMe);
				for( FPropertyIterator It(Actor->GetClass()); It; ++It )
				{
					FProperty &Property = It();
					if
					(	(Property.Bin  == PROPBIN_PerObject)
					&&	(Property.Type == CPT_Object) )
					{
						AActor **LinkedActor = (AActor **)Actor->ObjectPropertyPtr(Property,0);
						for( INT k=0; k<Property.ArrayDim; k++,LinkedActor++ )
						{
							if( *LinkedActor==FirstDeleted && (*LinkedActor)->bDeleteMe )
							{
								// Remove this reference.
								Actor->ModifyHeader();
								*LinkedActor = NULL;

								// Add to list of lost references.
								Ref = new( GMem )PLostReference( Actor, Property.Name, k, Ref );
							}
						}
					}
				}
			}
		}
		unguard;

		// Physically destroy the actor-to-delete.
		guard(FinishHim);
		checkState(FirstDeleted->bDeleteMe);
		AActor *ActorToKill = FirstDeleted;
		FirstDeleted        = FirstDeleted->Deleted;
		ActorToKill->Kill();
		unguard;

		// Notify everybody of the references which were lost.
		// In the notification message handlers, actors might delete each other.
		// If that happens, we keep looping until everything settles.
		guard(NotifyLostRefs);
		for( Ref; Ref!=NULL; Ref=Ref->Next )
			Ref->Actor->Process( NAME_LostReference, Ref );
		unguard;

		// Release memory.
		LostMark.Pop();
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Player spawning.
-----------------------------------------------------------------------------*/

//
// Find an available camera actor in the level and return it, or spawn a new
// one if none are available.  Returns actor number or NULL if none are
// available.
//
AView *ULevel::SpawnViewActor( UCamera *Camera, FName MatchName )
{
	guard(ULevel::SpawnViewActor);
	checkState(IsLocked());

	AView *Actor = NULL;

	// Find an existing camera actor.
	guard(1);
	for( int iActor=0; iActor<Num; iActor++ )
	{
		AActor *TestActor = Element(iActor);
		if
		(	TestActor
		&&	TestActor->IsA("View")
		&& !((AView*)TestActor)->Camera
		&&	(MatchName==NAME_None || MatchName==TestActor->GetFName())
		) 
		{
			Actor = (AView*)TestActor;
            break;
		}
    }
	unguard;

	guard(2);
    if( !Actor )
	{
		// None found, spawn a new one and set default position.
		Actor = (AView*)SpawnActor
		(
			new("View",FIND_Existing)UClass,
			NULL,
			Camera->GetFName(),
			FVector(-500,-300,+300),
			FRotation(0,0,0)
		);
		if( !Actor )
		{
			debugf( LOG_Problem, "SpawnViewActor failed because of SpawnActor" );
			return NULL;
		}

		// Set the new actor's game rendering properties.
		Actor->ShowFlags	= GGfx.DefaultCameraFlags;
		Actor->RendMap		= GGfx.DefaultRendMap;
		Actor->OrthoZoom	= 40000.0;
		Actor->Misc1		= 0;
		Actor->Misc2		= 0;
	}
	unguard;

	// Successfully spawned an actor.
	guard(3);
	if( !PossessActor( Actor, Camera ) )
	{
		// Posess failed.
		debugf( "SpawnViewActor failed to possess %s %s", Actor->GetClassName(), Actor->GetName() );
		DestroyActor( Actor );
		return NULL;
	}
	unguard;

	return Actor;
	unguard;
}

//
// Spawn a player actor for gameplay.
// Places at an appropriate PlayerStart point using the PlayerStart's yaw.
//
int ULevel::SpawnPlayActor( UCamera *Camera )
{
	guard(ULevel::SpawnPlayActor);
	checkState(IsLocked());

	APawn *Actor  = NULL;
	Camera->Level = this;

	for( INDEX i=0; i<Num; i++ )
	{
		AActor *TestActor = Element(i);
		if( TestActor && TestActor->IsA("PlayerStart") )
		{
			APlayerStart *PlayerStart = (APlayerStart *)TestActor;
			Actor = (APawn *)SpawnActor
			(
				PlayerStart->PlayerSpawnClass,
				NULL,
				NAME_None,
				PlayerStart->Location
			);
			if( Actor )
			{
				Actor->Rotation.Yaw	    = PlayerStart->Rotation.Yaw;
				Actor->ViewRotation.Yaw	= PlayerStart->Rotation.Yaw;

				if( PossessActor( Actor, Camera ) )
				{
					// Set the new actor's game rendering properties.
					Actor->ShowFlags	= GGfx.DefaultCameraFlags;
					Actor->RendMap		= GGfx.DefaultRendMap;
					Actor->OrthoZoom	= 40000.0;
					Actor->Misc1		= 0;
					Actor->Misc2		= 0;

					// The spawn has succeeded.
					return 1;
				}
				else
				{
					DestroyActor( Actor );
				}
				return 0;
			}
		}
	}
	return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Level actor moving/placing.
-----------------------------------------------------------------------------*/

//
// Try to place an actor that has moved a long way.  This is for
// moving actors through teleporters, adding them to levels, and
// starting them out in levels.  The results of this function is independent
// of the actor's current location and rotation.
//
// If the actor doesn't fit exactly in the location specified, tries
// to slightly move it out of walls and such.
// Will move actor by up to CollisionRadius in the X and Y directions, or CollisionHeight
// in the Z direction
//
// Returns 1 if the actor can been successfully moved, or 0 if it couldn't fit.
// The location passed is adjusted to the recommended value if the result is 1

// PlaceActor is private. 
// (PlaceActor is called by FarMoveActor and SpawnActor).
//

//MyBoxPointCheck is temporary hack, till Tim's works
int ULevel::MyBoxPointCheck
(
	FCheckResult&	Result,
	AActor*			Owner,
	FVector			Point,
	FVector			Extent,
	DWORD			ExtraNodeFlags
)
{
	guard(ULevel::MyBoxPointCheck);

	int result = 1;
	
	for (int x=0; x<2; x++)
	for (int y=0; y<2; y++)
	for (int z=0; z<2; z++)
	for (int i=x; i<2; i++)
	for (int j=y; j<2; j++)
	for (int k=z; k<2; k++)
			{
				FVector Start = Point;
				Start.X += 2 * (x - 0.5) * Extent.X;
				Start.Y += 2 * (y - 0.5) * Extent.Y;
				Start.Z += 2 * (z - 0.5) * Extent.Z; 
				FVector End = Point;
				End.X += 2 * (i - 0.5) * Extent.X;
				End.Y += 2 * (j - 0.5) * Extent.Y;
				End.Z += 2 * (k - 0.5) * Extent.Z;
				result = result && Model->LineCheck(Result,NULL,Start,End,FVector(0,0,0),0);
			}
	
	return result;
	unguard;
}

//
// Find a suitable nearby location to place a collision box.
//
int ULevel::FindSpot
(
	FVector Extent,
	FVector &Location,
	BOOL	bCheckActors
)
{
	guard(ULevel::FindSpot);

	// Don't allow adjusting out by much more than the collision size.
	FVector MaxAdjust = Extent * 1.15;

	// Try some successive adjustments.
	FVector Test = Location;
	FVector OldHitNormal(0,0,0);
	for( int Iter=0; Iter<6; Iter++ )
	{
		FMemMark Mark(GMem);
		FCheckResult Hit(1.0);
		if( Hash.SinglePointCheck( Hit, Test, Extent, 0, GetLevelInfo(), bCheckActors )==1 )
		{
			// Found a safe point, so return it if the adjustment is acceptable.
			if
			(	Abs( Test.X - Location.X ) < MaxAdjust.X
			&&	Abs( Test.Y - Location.Y ) < MaxAdjust.Y
			&&	Abs( Test.Z - Location.Z ) < MaxAdjust.Z )
			{
				Location = Test;
				return 1;
			}
			else return 0;
		}
		else if( Hit.Normal == FVector(0,0,0) )
		{
			// Center is inside a wall, so we have no hope of getting out.
			break;
		}
		else if( Iter>0 && (Hit.Normal | OldHitNormal)<0.0 )
		{
			// Adjust the test location with a backprojection if we've hit two facing normals.
			FVector Adjusted = Hit.Normal + OldHitNormal;
			if( Adjusted.IsNearlyZero() )
				break;
			Adjusted.Normalize();
			Test = Hit.Location + Adjusted * Hit.Location.Size() / (Adjusted | Hit.Normal);
		}
		else Test = Hit.Location;
		OldHitNormal = Hit.Normal;
	}

	// Subdivide into 8 sub-cubes and check collision of each one.
	FCheckResult Hit(1.0);
	FVector Adjust(0,0,0), Offset;
	for( Offset.X=-0.5; Offset.X<=0.5; Offset.X+=1.0 )
		for( Offset.Y=-0.5; Offset.Y<=0.5; Offset.Y+=1.0 )
			for( Offset.Z=-0.5; Offset.Z<=0.5; Offset.Z+=1.0 )
				if( Model->PointCheck( Hit, NULL, Location + Offset * Extent, 0.5 * Extent, 0 ) )
					Adjust += Offset;

	Adjust = 0.125 * FVector( Sgn(Adjust.X), Sgn(Adjust.Y), Sgn(Adjust.Z) ) * MaxAdjust;
	FVector Start = Location;
	//debugf("Adjust is %f %f %f", Adjust.X, Adjust.Y, Adjust.Z);

	for( int Nudges=0; Nudges<8; Nudges++ )
	{
		if( Model->PointCheck( Hit, NULL, Start+=Adjust, Extent, 0 ) )
		{
			Location = Start;
			return 1;
		}
	}

	// Failed.
	return 0;
	unguard;
}

//
// Try to place an actor that has moved a long way.  This is for
// moving actors through teleporters, adding them to levels, and
// starting them out in levels.  The results of this function is independent
// of the actor's current location and rotation.
//
// If the actor doesn't fit exactly in the location specified, tries
// to slightly move it out of walls and such.
//
// Returns 1 if the actor has been successfully moved, or 0 if it couldn't fit.
//
// Updates the actor's Zone and sends ZoneChange if it changes.
//
int ULevel::FarMoveActor( AActor *Actor, FVector DestLocation,  BOOL test, BOOL bNoCheck )
{
	guard(ULevel::FarMoveActor);
	checkState(IsLocked());
	checkInput(Actor!=NULL);

	if( Actor->bCollideActors ) //&& !test
		Hash.RemoveActor( Actor );

	FVector newLocation = DestLocation;
	int result = 1;
	if (!bNoCheck && Actor->bCollideWorld || Actor->bCollideWhenPlacing) 
		result = FindSpot( Actor->GetCollisionExtent(), newLocation, 0 );

	if (result)
	{
		Actor->Location = newLocation;
		Actor->OldLocation = newLocation; //to zero velocity

		SetActorZone( Actor, test );	
	}

	if( Actor->bCollideActors ) //&& !test
		Hash.AddActor( Actor );

	return result;
	unguard;
}

//
// Place the actor on the floor below.  May move the actor a long way down.
// Updates the actor's Zone and sends ZoneChange if it changes.
//
//

int ULevel::DropToFloor( AActor *Actor)
{
	guard(ULevel::DropToFloor);
	checkState(IsLocked());
	checkInput(Actor!=NULL);

	// Try moving down a long way and see if we hit the floor.

	FCheckResult Hit(1.0);
	MoveActor( Actor, FVector( 0, 0, -1000 ), Actor->Rotation, Hit );
	return (Hit.Time < 1.0);

	unguard;
}

/* TestMoveActor()
Returns 1 if Actor can move from Start to End (completely).  Does not actually move actor.
Start must be a valid location for the actor (test using PointCheck).
*/
int ULevel::TestMoveActor( AActor *Actor, FVector Start, FVector End, BOOL IgnorePawns )
{
	guard(ULevel::TestMoveActor);
	checkState(IsLocked());
	checkInput(Actor!=NULL);

	// Skip if no vector.
/*	FCheckResult Hit(1.0);
	FVector Delta = End - Start;
	if ( Delta.IsNearlyZero() ) 
		return 1;

	// Call collision trace function.
	FVector DeltaDir = Delta.Normal();
	FVector TestDelta = Delta + 2.0 * DeltaDir;

	// Get collision time with world.
	if( Actor->bCollideWorld )
		if (Model->LineCheck
		(
			Hit,
			NULL,
			Start,
			Start + TestDelta,
			Actor->GetCollisionExtent(),
			0
		) == 0)
			return 0;

	// Handle collision with actors.
	if( Actor->bCollideActors && (Actor->bBlockActors || Actor->bBlockPlayers) )
	{
		// Now check actor collision along this line.
		//FIXME - do my own optimized (stop at first hit) collision check
		//ignore pawns in that check (here I throw them away)
		FLOAT Subdivision = 250.f;
		FLOAT Dist        = TestDelta.Size();
		FLOAT MoveDone    = 0.0;
		FVector NextStart = Start;
		while( MoveDone < Dist )
		{
			FMemMark Mark(GMem);
			FCheckResult* FirstHit = Hash.LineCheck
			(
				GMem,
				NextStart,
				NextStart + Min(Subdivision,Dist - MoveDone) * DeltaDir,
				Actor->GetCollisionExtent(),
				Actor->bCollideActors && (Actor->bBlockActors || Actor->bBlockPlayers),
				NULL // Actor->bCollideWorld ? GetLevelInfo() : NULL
			);

			// Handle first blocking actor.
			for( FCheckResult* Hit=FirstHit; Hit; Hit=Hit->GetNext() )
				if( Hit->Actor!=Actor && (!IgnorePawns || !Hit->Actor->IsA("Pawn")) )
					if( Actor->IsBlockedBy(Hit->Actor) && !Actor->IsOverlapping(Hit->Actor) )
						break;
			Mark.Pop();
			if( Hit!=NULL )
				return 0;

			// Still going.
			MoveDone  += Subdivision;
			NextStart += Subdivision * DeltaDir;
		}
	}
	// Success, can move.
	return 1; */
	debugf("Called TestMoveActor");
	return 0;
	unguard;
}

//
// Tries to move the actor by a movement vector.  If no collision occurs, this function 
// just does a Location+=Move.
//
// Assumes that the actor's Location is valid and that the actor
// does fit in its current Location. Assumes that the level's 
// Dynamics member is locked, which will always be the case during
// a call to ULevel::Tick; if not locked, no actor-actor collision
// checking is performed.
//
// If bCollideWorld, checks collision with the world.
//
// For every actor-actor collision pair:
//
// If both have bCollideActors and bBlocksActors, performs collision
//    rebound, and dispatches Touch messages to touched-and-rebounded 
//    actors.  
//
// If both have bCollideActors but either one doesn't have bBlocksActors,
//    checks collision with other actors (but lets this actor 
//    interpenetrate), and dispatches Touch and UnTouch messages.
//
// Returns 1 if some movement occured, 0 if no movement occured.
//
// Updates actor's Zone and sends ZoneChange if it changes.
//
// If Test = 1 (default 0), do not send notifications.
//
int ULevel::MoveActor
(
	AActor*			Actor,
	FVector			Delta,
	FRotation		NewRotation,
	FCheckResult	&Hit,
	BOOL			bTest,
	BOOL			bIgnorePawns,
	BOOL			bIgnoreBases
)
{
	guard(ULevel::MoveActor);
	checkState(IsLocked());
	checkInput(Actor!=NULL);

	// Skip if no vector.
	Hit = FCheckResult(1.0);
	if( Delta.IsNearlyZero() && NewRotation==Actor->Rotation )
		return 1;

	// Set up.
	FMemMark Mark(GMem);
	FVector DeltaDir       = Delta.Normal();
	FVector TestDelta      = Delta + 2.0 * DeltaDir;
	INT     MaybeTouched   = 0;
	INT     NumHits        = 0;
	FCheckResult* FirstHit = NULL;

	// Perform movement collision checking if needed for this actor.
	if( (Actor->bCollideActors || Actor->bCollideWorld) && !Actor->Brush && !Delta.IsNearlyZero() )
	{
		FirstHit = Hash.LineCheck
		(
			GMem,
			Actor->Location,
			Actor->Location + TestDelta,
			Actor->GetCollisionExtent(),
			(Actor->bCollideActors && !Actor->Brush) ? 1              : 0,
			(Actor->bCollideWorld && !Actor->Brush)  ? GetLevelInfo() : NULL
		);

		// Handle first blocking actor.
		if( Actor->bCollideWorld || Actor->bBlockActors || Actor->bBlockPlayers )
		{
			for( FCheckResult* Test=FirstHit; Test; Test=Test->GetNext() )
			{
				if
				(	(!bIgnorePawns || !Test->Actor->IsA("Pawn")     )
				&&	(!bIgnoreBases || !Actor->IsBasedOn(Test->Actor))
				&&	(!Test->Actor->IsBasedOn(Actor)                 ) )
				{
					MaybeTouched = 1;
					if( Actor->IsBlockedBy(Test->Actor) )
					{
						Hit = *Test;
						break;
					}
				}
			}
		}
	}

	// Attenuate movement.
	FVector FinalDelta = Delta;
	if( Hit.Time < 1.0 )
	{
		// Fix up delta, given that TestDelta = Delta + 2.
		FinalDelta = TestDelta * Hit.Time;
		FinalDelta = FinalDelta - Min(2.f, FinalDelta.Size()) * DeltaDir;
		Hit.Time   = FinalDelta.Size() / Delta.Size();
	}

	// Move the based actors (BEFORE encroachment checking).
	if( Actor->StandingCount && !bTest )
	{
		for( int i=0; i<Num; i++ )
		{
			AActor *Other = Element(i);
			if( Other && Other->Base==Actor )
			{
				// Move base.
				FVector   RotMotion( 0, 0, 0 );
				FRotation DeltaRot ( 0, NewRotation.Yaw - Actor->Rotation.Yaw, 0 );
				if( NewRotation != Actor->Rotation )
				{
					// Handle rotation-induced motion.
					FRotation ReducedRotation = FRotation( 0, ReduceAngle(NewRotation.Yaw) - ReduceAngle(Actor->Rotation.Yaw), 0 );
					FVector   Pointer         = Actor->Location - Other->Location;
					RotMotion                 = Pointer - Pointer.TransformVectorBy( GMath.UnitCoords * ReducedRotation );
				}
				FCheckResult Hit(1.0);
				MoveActor( Other, FinalDelta + RotMotion, Other->Rotation + DeltaRot, Hit, 0, 0, 1 );
				if( Other->IsA("Pawn") )
					((APawn*)Other)->ViewRotation += DeltaRot;
			}
		}
	}

	// Abort if encroachment declined.
	if( CheckEncroachment( Actor, Actor->Location + FinalDelta, NewRotation ) )
		return 0;

	// Update the location.
	if( Actor->bCollideActors ) Hash.RemoveActor( Actor );
	Actor->Location += FinalDelta;
	Actor->Rotation  = NewRotation;
	if( Actor->bCollideActors ) Hash.AddActor( Actor );

	// Handle bump and touch notifications.
	if( !bTest )
	{
		// Notify first bumped actor unless it's the level or the actor's base.
		if( Hit.Actor && Hit.Actor!=GetLevelInfo() && !Actor->IsBasedOn(Hit.Actor) )
		{
			// Notify both actors of the bump.
			Hit.Actor->Process( NAME_Bump, &PActor(Actor    ) );
			Actor->Process    ( NAME_Bump, &PActor(Hit.Actor) );
		}

		// Handle Touch notifications.
		if( MaybeTouched || !Actor->bBlockActors || !Actor->bBlockPlayers )
			for( FCheckResult* Test=FirstHit; Test && Test->Time<Hit.Time; Test=Test->GetNext() )
				if
				(	(!Test->Actor->IsBasedOn(Actor))
				&&	(!bIgnoreBases || Actor->IsBasedOn(Test->Actor))
				&&	(!Actor->IsBlockedBy(Test->Actor))
				&&	(Actor->IsOverlapping(Test->Actor)) )
					Actor->BeginTouch( Test->Actor );

		// UnTouch notifications.
		for( int i=0; i<ARRAY_COUNT(Actor->Touching); i++ )
			if( Actor->Touching[i] && !Actor->IsOverlapping(Actor->Touching[i]) )
				Actor->EndTouch( Actor->Touching[i], 0 );

	}

	// Set actor zone.
	SetActorZone( Actor, bTest );
	Mark.Pop();

	// Update moving brush.
	if( !bTest && Hit.Time>0.0 && Actor->Brush && (Actor->Brush->Location!=Actor->Location || Actor->Brush->Rotation!=Actor->Rotation) )
		GBrushTracker.Update( Actor );

	// Return whether we moved at all.
	return Hit.Time>0.0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Encroachment.
-----------------------------------------------------------------------------*/

//
// Check whether Actor is encroaching other actors after a move, and return
// 0 to ok the move, or 1 to abort it.
//
int ULevel::CheckEncroachment
(
	AActor*		Actor,
	FVector		TestLocation,
	FRotation	TestRotation
)
{
	guard(ULevel::CheckEncroachment);
	checkState(IsLocked());
	checkInput(Actor!=NULL);

	// If this actor doesn't need encroachment checking, allow the move.
	if( !Actor->bCollideActors || (!Actor->bBlockActors && !Actor->bBlockPlayers) || !Actor->Brush )
		return 0;

	// Query the mover about what he wants to do with the actors he is encroaching.
	FMemMark Mark(GMem);
	FCheckResult* FirstHit = Hash.EncroachmentCheck( GMem, Actor, TestLocation, TestRotation, 0 );	
	for( FCheckResult* Test = FirstHit; Test!=NULL; Test=Test->GetNext() )
	{
		if( Test->Actor!=Actor && Test->Actor!=GetLevelInfo() && Actor->IsBlockedBy( Test->Actor ) )
		{
			PActorBool ActorBool( Test->Actor );
			Actor->Process( NAME_EncroachingOn, &ActorBool );
			if( ActorBool.Result != 0 )
				return 1;
		}
	}

	// Notify the encroached actors but not the level.
	for( Test = FirstHit; Test; Test=Test->GetNext() )
		if( Test->Actor!=Actor && Test->Actor!=GetLevelInfo() && Actor->IsBlockedBy( Test->Actor ) )
			Test->Actor->Process( NAME_EncroachedBy, &PActor(Actor) );
	Mark.Pop();

	// Ok the move.
	return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Trace.
-----------------------------------------------------------------------------*/

//
// Trace a line and return the first hit actor (LevelInfo means hit the world geomtry).
//
int ULevel::Trace
(
	FCheckResult	&Hit,			// Item hit.
	AActor			*SourceActor,	// Source actor, this or its parents is never hit.
	const FVector	&End,			// End location.
	const FVector	&Start,			// Start location.
	DWORD           TraceFlags,		// Trace flags.
	FVector			Extent			// Collision extent.
)
{
	guard(ULevel::Trace);
	checkState(IsLocked());

	// Get list of hit actors.
	FMemMark Mark(GMem);
	FCheckResult* FirstHit = Hash.LineCheck
	(
		GMem,
		Start,
		End,
		Extent,
		(TraceFlags & TRACE_AllColliding) ? 1 : 0,
		(TraceFlags & TRACE_Level       ) ? GetLevelInfo() : NULL
	);

	// Skip owned actors and return the one nearest actor.
	for( FCheckResult* Check = FirstHit; Check!=NULL; Check=Check->GetNext() )
	{
		if( !SourceActor || !SourceActor->IsOwnedBy( Check->Actor ) )
		{
			if( Check->Actor->IsA("LevelInfo") )
			{
				if( TraceFlags & TRACE_Level )
					break;
			}
			else if( Check->Actor->IsA("Pawn") )
			{
				if( TraceFlags & TRACE_Pawns )
					break;
			}
			else if( Check->Actor->IsA("Mover") )
			{
				if( TraceFlags & TRACE_Movers )
					break;
			}
			else if( Check->Actor->IsA("ZoneInfo") )
			{
				if( TraceFlags & TRACE_ZoneChanges )
					break;
			}
			else
			{
				if( TraceFlags & TRACE_Others )
					break;
			}
		}
	}
	if( Check )
		Hit = *Check;
	else
	{
		Hit.Time = 1.0;
		Hit.Actor = NULL;
	}

	Mark.Pop();
	return Check==NULL;
	unguard;
}

/*-----------------------------------------------------------------------------
	ULevel zone functions.
-----------------------------------------------------------------------------*/

//
// Figure out which zone an actor is in, update the actor's iZone,
// and notify the actor of the zone change.  Skips the zone notification
// if the zone hasn't changed.
//
void ULevel::SetActorZone( AActor *Actor, BOOL bTest, BOOL bForceRefresh )
{
	guard(ULevel::SetActorZone);
	checkState(IsLocked());
	checkInput(Actor!=NULL);

	if( Actor->bDeleteMe )
		return;

	if( bForceRefresh )
	{
		// Init the actor's zone.
		Actor->ZoneNumber = 0;
		Actor->Zone       = GetLevelInfo();
	}

	// Find zone based on actor's location and see if it has changed.
	int NewZoneNumber = Model->PointZone(Actor->Location);
	if( NewZoneNumber != Actor->ZoneNumber )
	{
		// Notify old zone info of player leaving.
		if( Actor->IsPlayer() && !bTest )
			Actor->Zone->Process( NAME_PlayerLeaving, &PActor(Actor) );

		AZoneInfo *Zone = GetZoneActor(NewZoneNumber);

		if( !bTest )
			Actor->Process( NAME_ZoneChange, &PActor(Zone) );

		Actor->ZoneNumber = NewZoneNumber;
		Actor->Zone       = Zone;

		if( Actor->IsPlayer() && !bTest )
			Actor->Zone->Process( NAME_PlayerEntered, &PActor(Actor) );
	} 
	unguard;
}

/*-----------------------------------------------------------------------------
	ULevel player command-line.
-----------------------------------------------------------------------------*/

//
// A special command line which is executed relative to a level and player.
//
void ULevel::PlayerExec( AActor *Actor, const char *Cmd, FOutputDevice *Out )
{
	guard(ULevel::PlayerExec);
	checkInput(Actor!=NULL);
	const char *Str = Cmd;

	// If a valid message was specified, send it to this actor.
	char MsgStr[NAME_SIZE]="Server";
	if( GrabSTRING(Str,MsgStr+strlen(MsgStr),NAME_SIZE-strlen(MsgStr)) )
	{
		FName Msg(MsgStr,FNAME_Find);
		if( Msg != NAME_None )
		{
			// Get optional parameters.
			PServer ServerInfo;
			char NS[NAME_SIZE];
			GetFLOAT(Str,"F=",&ServerInfo.F);
			GetSTRING(Str,"S=",ServerInfo.S,NAME_SIZE);
			GetSTRING(Str,"N=",NS,NAME_SIZE);
			ServerInfo.N = FName( NS, FNAME_Find );

			// Send message.
			for( int i=0; i<Num; i++ )
			{
				if( Element(i) && Element(i)->IsOwnedBy(Actor) )
					Element(i)->Process( Msg, &ServerInfo );
			}
		}
		else Out->Logf("Unknown message %s",MsgStr);
	}
	else Out->Logf("Missing message");
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
