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

	SetActorZone( Actor, 1 );
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
	SetActorZone( Actor, 1 );

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
	UClass			*Class,
	AActor			*Owner,
	FName			ActorName,
	const FVector	*Location,
	const FRotation *Rotation,
	AActor			*Template
)
{
	guard(ULevel::SpawnActor);
	checkState(IsLocked());
	INDEX iActor;
	if( ActorName==NAME_None )
		ActorName = Class->GetFName();

	// Make sure there's a free actor.
	guard(1);
	for( iActor=0; iActor<Num; iActor++ )
		if( Element(iActor)==NULL )
			break;
	if( iActor==Num && iActor<Max )
		iActor = Num++;
	else if( iActor >= Max )
		appError( "Actor list exhausted" ); // Expand the actor list!!
	unguard;

	// If no template was specified, use the class's default actor.
	if( Template == NULL ) 
		Template = &Class->GetDefaultActor();

	// Make sure this class isn't an abstract class, unless that's explicitly allowed.
	if( Class->ClassFlags&CLASS_Abstract || !Class->IsChildOf("Actor") )
		return NULL;

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

	// Set the actor's location, if one was specified.
	if( Location )
	{
		FVector spot = *Location;
		PlaceActor(Actor, &spot); //Actor->Location = *Location;
	}
	if( Rotation )
		Actor->Rotation = *Rotation;

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

	// Set collision.
	if( Actor->bCollideActors && Hash.CollisionInitialized  )
		Hash.AddActor( Actor );

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

	// Remove from world collision list.
	guard(4);
	if( ThisActor->bCollideActors && Hash.CollisionInitialized  )
		Hash.RemoveActor( ThisActor );
	unguard;

	// Remove the actor from the actor list, without killing it here.
	// The actors are actually killed in ULevel::Unlock; until then the actor
	// is still valid and in a state of limbo. It needs to be done this way in
	// case an actor is destroyed but is still referenced somewhere on the
	// execution stack.
	guard(2);
	Element(iActor)       = NULL;
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

	// If editing, cleanup the destroyed actors immediately.
	if( GetState() == LEVEL_UpEdit )
		CleanupDestroyed();

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
							if( *LinkedActor && (*LinkedActor)->bDeleteMe )
							{
								// Remove this reference.
								Actor->ModifyHeader();
								*LinkedActor = NULL;

								// Add to list of lost references.
								Ref = new(GMem)PLostReference( Actor, Property.Name, k, Ref );
							}
						}
					}
				}
			}
		}
		unguard;

		// Physically destroy the actors-to-delete.
		guard(KillEm);
		while( FirstDeleted != NULL )
		{
			checkState(FirstDeleted->bDeleteMe);
			AActor *ActorToKill = FirstDeleted;
			FirstDeleted        = FirstDeleted->Deleted;
			ActorToKill->Kill();
		}
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
AView *ULevel::SpawnViewActor( UCamera *Camera, FName MatchName, AActor *Template )
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
			new( "View", FIND_Existing )UClass,
			NULL,
			Camera->GetFName(),
			Template ? &Template->Location : NULL,
			Template ? &Template->Rotation : NULL,
			Template
		);
		if( !Actor )
			return NULL;

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
				&PlayerStart->Location
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
// Returns 1 if the actor has been successfully moved, or 0 if it couldn't fit.
//
// PlaceActor is private. 
// (PlaceActor is called by FarMoveActor and SpawnActor).
//

//MyBoxPointCheck is temporary hack, till Tim's works
int ULevel::MyBoxPointCheck(FCheckResult &Result,AActor *Owner, const FVector &Point, FLOAT Radius,
								FLOAT Height, DWORD ExtraNodeFlags)
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
				Start.X += 2 * (x - 0.5) * Radius;
				Start.Y += 2 * (y - 0.5) * Radius;
				Start.Z += 2 * (z - 0.5) * Height; 
				FVector End = Point;
				End.X += 2 * (i - 0.5) * Radius;
				End.Y += 2 * (j - 0.5) * Radius;
				End.Z += 2 * (k - 0.5) * Height;
				result = result && Model->LineCheck(Result,Owner,Start,End,0);
			}
	
	return result;
	unguard;
}

int ULevel::PlaceActor( AActor *Actor, FVector *Location )
{
	guard(ULevel::PlaceActor);
	//Temporary hack for FarMoveActor
	// FIXME- use Tim's (corrected) BoxPointCheck
	// Temporary use MyBoxPointCheck
	Actor->Location = *Location;
	int result = 1;
	FCheckResult Hit(1.0);
	if (Actor->bCollideWorld 
		&& !MyBoxPointCheck(Hit, Actor, *Location, Actor->CollisionRadius, Actor->CollisionHeight, 0)) 
	{
		//subdivide bounding box into eight zones. for each zone that fails, add to adjustment vector
		//FIXME - if Tim doesn't add suggested move to BoxPointCheck, then do this recursively?
		// Instead of recursively, for now make adjustments in small steps
		debugf("Adjusting PlaceActor");
		FVector Adjust = GMath.ZeroVector;
		FVector start;
		for (int x=0; x<2; x++)
			for (int y=0; y<2; y++)
				for (int z=0; z<2; z++)
				{
					start = *Location;
					start.X += (x - 0.5) * Actor->CollisionRadius;
					start.Y += (y - 0.5) * Actor->CollisionRadius;
					start.Z += (z - 0.5) * Actor->CollisionHeight;
					if (!MyBoxPointCheck(Hit, Actor, start, 0.5 * Actor->CollisionRadius, 0.5 * Actor->CollisionHeight, 0))
					{
						Adjust.X += 0.5 * ((0.5 - x) * Actor->CollisionRadius);
						Adjust.Y += 0.5 * ((0.5 - y) * Actor->CollisionRadius);
						Adjust.Z += 0.5 * ((0.5 - z) * Actor->CollisionHeight);
					}
				}
		Adjust.Z *= 1.1;
		int nudges = 1;
		result = 0;
		while (nudges <9)
		{
			start = *Location + nudges * 0.125 * Adjust;
			if (MyBoxPointCheck(Hit, Actor, start, Actor->CollisionRadius, Actor->CollisionHeight, 0))
			{
				nudges = 9;
				result = 1;
				Actor->Location = start;
			}
			nudges++;
		}
	}

	return result;
	unguard;
}

// Try to place an actor that has moved a long way.  This is for
// moving actors through teleporters, adding them to levels, and
// starting them out in levels.  The results of this function is independent
// of the actor's current location and rotation.
//
// If the actor doesn't fit exactly in the location specified, tries
// to slightly move it out of walls and such.
//
//
// Returns 1 if the actor has been successfully moved, or 0 if it couldn't fit.
//
// Updates the actor's Zone and sends ZoneChange if it changes.
//

int ULevel::FarMoveActor( AActor *Actor, FVector *Location, int test)
{
	guard(ULevel::FarMoveActor);
	checkState(IsLocked());
	int result = PlaceActor(Actor, Location);
	if (!test)
		SetActorZone( Actor );
	else //if test, figure out right zone without sending message
	{
		int NewZoneNumber = Model->PointZone(Actor->Location);
		if( NewZoneNumber != Actor->ZoneNumber )
		{
			AZoneInfo *Zone = GetZoneActor(NewZoneNumber);
			Actor->ZoneNumber = NewZoneNumber;
			Actor->Zone       = Zone;
		}
	}
	
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

	FVector FloorDelta = GMath.ZeroVector;
	FloorDelta.Z = -1000.0;
	FCheckResult Hit(1.0);
	MoveActor(Actor, &FloorDelta, Hit);
	int result = (Hit.Time < 1.0);
	SetActorZone( Actor );
	
	return result;
	unguard;
}

/* TestMoveActor()
Returns 1 if Actor can move from Start to End (completely).  Does not actually move actor.
Start must be a valid location for the actor (test using BoxPointCheck).
*/
int ULevel::TestMoveActor( AActor *Actor, FVector &Start, FVector &End)
{
	guard(ULevel::TestMoveActor);
	checkState(IsLocked());

	// Skip if no vector.
	FCheckResult Hit(1.0);
	FVector Delta = End - Start;
	if (Delta.IsNearlyZero()) 
		return 1;

	// Call collision trace function.
	FVector DeltaDir = Delta.Normal();
	FVector TestDelta = Delta + 2.0 * DeltaDir;

	// Get collision time with world.
	if (Actor->bCollideWorld)
		Model->BoxLineCheck
		(
			Hit,
			NULL,
			Start,
			Start + TestDelta,
			Actor->CollisionRadius,
			Actor->CollisionHeight,
			0
		);

	INT Moved = (Hit.Time == 1.0);
	if (Moved && Actor->bCollideActors && ( Actor->bBlockActors || Actor->bBlockPlayers ))
	{
		// Remove this actor from collision list.
		Hash.RemoveActor( Actor );

		// Now check actor collision along this line.
		INT MaybeTouched=0, NumHits=0;
		FCheckResult Hits[64];

		//FIXME - do my own optimized (stop at first hit) collision check
		//ignore pawns in that check (here I throw them away)
		FLOAT Dist = TestDelta.Size();
		FLOAT MoveDone = 0.0;
		FVector NextStart = Start;
		while (MoveDone < Dist)
		{
			NumHits = Hash.LineCheck
			(
				Hits,
				ARRAY_COUNT(Hits),
				NextStart,
				NextStart + Min(250.f,Dist - MoveDone) * DeltaDir,
				Actor->CollisionRadius,
				Actor->CollisionHeight
			);

			// Handle first blocking actor (but no pawns).
			int j = 0;
			while (j<NumHits)
			{
				if (!Hits[j].Actor->IsA("Pawn"))
					if( Actor->IsBlockedBy(Hits[j].Actor) && !Actor->IsOverlapping(Hits[j].Actor) )
					{
						Moved = 0;
						j = NumHits;
					}
				j++;
			}
		
			if (Moved) //still going
			{
				MoveDone += 250.0;
				NextStart += 250.0 * DeltaDir;
			}
			else
				MoveDone = Dist;
		}

		// Add actor back to collision list.
		Hash.AddActor( Actor );
	}
	// Success.
	return Moved;
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
int ULevel::MoveActor( AActor *Actor, FVector *Delta, FCheckResult &Hit, int test )
{
	guard(ULevel::MoveActor);
	checkState(IsLocked());

	// Skip if no vector.
	Hit = FCheckResult(1.0);
	if( (*Delta).IsNearlyZero() ) //( *Delta == GMath.ZeroVector )
		return 1;

	// Call collision trace function.
	FVector DeltaDir = (*Delta).Normal();
	FVector TestDelta = *Delta + 2.0 * DeltaDir;

	// Get collision time with world.
	if( Actor->bCollideWorld )
	{
		if( Model->BoxLineCheck
		(
			Hit,
			NULL,
			Actor->Location,
			Actor->Location + TestDelta,
			Actor->CollisionRadius,
			Actor->CollisionHeight,
			0
		) == 0 )
		{
			Hit.Actor = GetLevelInfo();
		}
	}

	INT Moved = (Hit.Time > 0.0);
	if( Moved )
	{
		// Remove this actor from collision list.
		if( Actor->bCollideActors )
			Hash.RemoveActor( Actor );

		// Now check actor collision along this line.
		INT MaybeTouched=0, NumHits=0;
		FCheckResult Hits[64];

		if( Actor->bCollideActors )
		{
			NumHits = Hash.LineCheck
			(
				Hits,
				ARRAY_COUNT(Hits),
				Actor->Location,
				Actor->Location + TestDelta,
				Actor->CollisionRadius,
				Actor->CollisionHeight
			);

			// Handle first blocking actor.
			if( Actor->bBlockActors || Actor->bBlockPlayers )
			{
				for( int j=0; j<NumHits && Hits[j].Time<Hit.Time; j++ )
				{
					if (!test || !Actor->IsA("Pawn"))
					{
						if( Actor->IsBlockedBy(Hits[j].Actor) && !Actor->IsOverlapping(Hits[j].Actor) )
							Hit = Hits[j];
						else MaybeTouched = 1;
					}
				}
			}
		}
		// Update actor's location.
		//keep away from obstacle
		FVector FinalDelta = *Delta;
		if( Hit.Time < 1.0 )
		{
			//fix up delta - given that testdelta = delta + 2
			FinalDelta = (TestDelta * Hit.Time);
			FinalDelta = FinalDelta - Min(2.f, FinalDelta.Size()) * DeltaDir;
			Hit.Time = FinalDelta.Size() / (*Delta).Size();
		}
		
		Actor->Location += FinalDelta;

		// Add actor back to collision list.
		if( Actor->bCollideActors )
			Hash.AddActor( Actor );

		// Handle bump and touch notifications.
		if (!test)
		{
			// Notify first bumped actor.
			if( Hit.Actor && Hit.Actor!=GetLevelInfo() )
			{
				// Tell physics code.
				Actor->Bump( Hit.Actor );

				// Notify actors.
				Hit.Actor->Process( NAME_Bump, &PActor(Actor ) );
				Actor->Process ( NAME_Bump, &PActor(Hit.Actor) );
			}

			// Handle touch notifications.
			if( MaybeTouched || !Actor->bBlockActors || !Actor->bBlockPlayers )
				for( int j=0; j<NumHits && Hits[j].Time<Hit.Time; j++ )
					if( !Actor->IsBlockedBy(Hits[j].Actor) && Actor->IsOverlapping(Hits[j].Actor))
						Actor->BeginTouch( Hits[j].Actor );

			// UnTouch notification.
			for( int i=0; i<ARRAY_COUNT(Actor->Touching); i++ )
				if( Actor->Touching[i] && !Actor->IsOverlapping(Actor->Touching[i]) )
					Actor->EndTouch( Actor->Touching[i], 0 );

			// Set actor zone.
			SetActorZone( Actor );
		}
		else
		{
			// If test, figure out right zone without sending message.
			int NewZoneNumber = Model->PointZone(Actor->Location);
			if( NewZoneNumber != Actor->ZoneNumber )
			{
				AZoneInfo *Zone = GetZoneActor(NewZoneNumber);
				Actor->ZoneNumber = NewZoneNumber;
				Actor->Zone       = Zone;
			}
		}
	}

	return Moved;
	unguard;
}

/*-----------------------------------------------------------------------------
	RotateActor.
-----------------------------------------------------------------------------*/

//
// Try to set the actor's rotation to the specified rotation, performing
// collision checking with the world (if the actor's collision shape is
// rotation variant), and attempting to move and rotate any actors standing
// on this actor.
//
int ULevel::RotateActor
(
	AActor*		Actor,
	FRotation&	DestRotation,
	int			Test
)
{
	guard(ULevel::RotateActor);

	//todo!! floor handling
	Actor->Rotation = DestRotation;
	return 1;

	unguard;
}

/*-----------------------------------------------------------------------------
	Trace.
-----------------------------------------------------------------------------*/

//
// Trace a line and return the first hit actor (LevelInfo means hit the world geomtry).
//
void ULevel::Trace
(
	FCheckResult	&Hit,			// Item hit.
	AActor			*SourceActor,	// Source actor, this or its parents is never hit.
	const FVector	&End,			// End location.
	const FVector	&Start,			// Start location.
	BOOL			TraceActors		// Whether to trace for actors.
)
{
	guard(ULevel::Trace);
	checkState(IsLocked());

	// Init hit info.
	Hit.Actor = NULL;

	// Perform raytrace with world's primitive.
	if( Model->LineCheck( Hit, NULL, Start, End, 0 ) == 0 )
	{
		// Hit a surface.
		Hit.Actor     = GetLevelInfo();
		Hit.Primitive = Model;
	}

	// Check collision with actors.
	if( TraceActors )
	{
		// Get list of hit actors.
		FCheckResult Hits[16];
		INT NumHits = Hash.LineCheck( Hits, ARRAY_COUNT(Hits), Start, End, 0, 0 );

		// Skip owned actors and get the nearest actor.
		INDEX iCheck = 0;
		if( SourceActor )
			while( iCheck<NumHits && SourceActor->IsOwnedBy(Hits[iCheck].Actor) )
				iCheck++;
		if( iCheck < NumHits && (Hit.Actor==NULL || Hits[0].Time < Hit.Time) )
		{
			Hit = Hits[iCheck];
			Hit.Location = Start + (End-Start)*Hit.Time;
		}
	}

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
void ULevel::SetActorZone( AActor *Actor, BOOL bForceRefresh )
{
	guard(ULevel::SetActorZone);
	checkState(IsLocked());
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
		if( Actor->IsPlayer() )
			Actor->Zone->Process( NAME_PlayerLeaving, &PActor(Actor) );

		AZoneInfo *Zone = GetZoneActor(NewZoneNumber);

		Actor->Process( NAME_ZoneChange, &PActor(Zone) );

		Actor->ZoneNumber = NewZoneNumber;
		Actor->Zone       = Zone;

		if( Actor->IsPlayer() )
			// Notify new zone info of player entering.
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
