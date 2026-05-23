/*=============================================================================
	UnLevCol.cpp: Actor list collision code.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Design goal:
	To be self-contained. This collision code maintains its own collision hash
	table and doesn't know about any far-away data structures like the level BSP.

Revision history:
	* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	FCollisionHash statics.
-----------------------------------------------------------------------------*/

// FCollisionHash statics.
INT FCollisionHash::InitializedBasis=0;
INT FCollisionHash::CollisionTag=0;
INT FCollisionHash::HashX[NUM_BUCKETS];
INT FCollisionHash::HashY[NUM_BUCKETS];
INT FCollisionHash::HashZ[NUM_BUCKETS];	

// Global pool of available links.
static FCollisionHash::FActorLink *GAvailable = NULL;

// Debugging.
#define HASH_ALL_TO_SAME_BUCKET 0 /* Should be 0 */

// Global statistics.
static int GActorsAdded=0, GFragsAdded=0, GUsed=0, GChecks=0;

/*-----------------------------------------------------------------------------
	FCollisionHash init/exit.
-----------------------------------------------------------------------------*/

//
// Initialize the actor collision information.
//
void FCollisionHash::Init()
{
	guard(FCollisionHash::Init);
	checkState(!CollisionInitialized);
	GAvailable = NULL;

	// Initialize collision basis tables if necessary.
	if( !InitializedBasis )
	{
		for( int i=0; i<NUM_BUCKETS; i++ )
#if HASH_ALL_TO_SAME_BUCKET
			HashX[i] = HashY[i] = HashZ[i] = 0;
#else
			HashX[i] = HashY[i] = HashZ[i] = i;
#endif
		srand(0x1234567);
		for( i=0; i<NUM_BUCKETS; i++ )
		{
			Exchange( HashX[i], HashX[rand() % NUM_BUCKETS] );
			Exchange( HashY[i], HashY[rand() % NUM_BUCKETS] );
			Exchange( HashZ[i], HashZ[rand() % NUM_BUCKETS] );
		}
	}

	// Init hash table.
	for( int i=0; i<NUM_BUCKETS; i++ )
		Hash[i] = NULL;

	// Note that we're initialized.
	CollisionInitialized = 1;

	unguard;
}

//
// Shut down the actor collision information.
//
void FCollisionHash::Exit()
{
	guard(FCollisionHash::Exit);
	checkState(CollisionInitialized);

	// Free all stuff.
	for( int i=0; i<NUM_BUCKETS; i++ )
	{
		while( Hash[i] != NULL )
		{
			FActorLink *Link = Hash[i];
			Hash[i]          = Hash[i]->Next;
			delete Link;
		}
	}
	while( GAvailable != NULL )
	{
		FActorLink *Link = GAvailable;
		GAvailable       = GAvailable->Next;
		delete Link;
	}
	CollisionInitialized = 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	FCollisionHash tick - clean up collision info.
-----------------------------------------------------------------------------*/

//
// Cleanup the collision info.
//
void FCollisionHash::Tick()
{
	guard(FCollisionHash::Tick);
	checkState(CollisionInitialized);

	// All we do here is stats.
	//debugf("Used=%i Added=%i Frags=%i Checks=%i",GUsed,GActorsAdded,GFragsAdded,GChecks);
	GActorsAdded = GFragsAdded = GChecks = 0;

	unguard;
}

/*-----------------------------------------------------------------------------
	FCollisionHash extent.
-----------------------------------------------------------------------------*/

//
// Compute the extent of an actor in hash coordinates.
//
void FCollisionHash::GetActorExtent
(
	AActor *Actor,
	INT &X0, INT &X1, INT &Y0, INT &Y1, INT &Z0, INT &Z1
)
{
	guard(FCollisionHash::GetActorExtent);

	// Get actor's bounding box.	
	FBoundingBox Box = Actor->GetPrimitive()->GetCollisionBoundingBox( Actor );

	// Discretize to hash coordinates.
	GetHashIndices( Box.Min, X0, Y0, Z0 );
	GetHashIndices( Box.Max, X1, Y1, Z1 );

	unguard;
}

/*-----------------------------------------------------------------------------
	FCollisionHash adding/removing.
-----------------------------------------------------------------------------*/

//
// Add an actor to the collision info.
//
void FCollisionHash::AddActor( AActor *Actor )
{
	guard(FCollisionHash::AddActor);
	checkState(CollisionInitialized);
	checkInput(Actor->bCollideActors);
	if( Actor->bDeleteMe )
		return;
	CheckActorNotReferenced( Actor );
	GActorsAdded++;

	// Add actor in all the specified places.
	INT X0,Y0,Z0,X1,Y1,Z1;
	GetActorExtent( Actor, X0, X1, Y0, Y1, Z0, Z1 );
	for( INT X=X0; X<=X1; X++ )
	{
		for( INT Y=Y0; Y<=Y1; Y++ )
		{
			for( INT Z=Z0; Z<=Z1; Z++ )
			{
				INT iLocation;
				FActorLink *&Link = GetHashLink( X, Y, Z, iLocation );
				if( GAvailable )
				{
					// Get a link from the table.
					FActorLink *NewLink = GAvailable;
					GAvailable          = GAvailable->Next;
					*NewLink            = FActorLink( Actor, Link, iLocation );
					Link                = NewLink;
				}
				else
				{
					// Allocate a new link.
					Link = new FActorLink( Actor, Link, iLocation );
				}
				GUsed++;
				GFragsAdded++;
			}
		}
	}
	Actor->ColLocation = Actor->Location;
	unguard;
}

//
// Remove an actor from the collision info.
//
void FCollisionHash::RemoveActor( AActor *Actor )
{
	guard(FCollisionHash::RemoveActor);
	checkInput(Actor->bCollideActors);
	checkState(CollisionInitialized);
	if( Actor->bDeleteMe )
		return;
	if( !GEditor && Actor->Location!=Actor->ColLocation )
		appErrorf( "%s %s moved without proper hashing", Actor->GetClassName(), Actor->GetName() );

	// Remove actor.
	INT X0,Y0,Z0,X1,Y1,Z1;
	INT ExpectFrags=0, FoundFrags=0;
	GetActorExtent( Actor, X0, X1, Y0, Y1, Z0, Z1 );
	for( INT X=X0; X<=X1; X++ )
	{
		for( INT Y=Y0; Y<=Y1; Y++ )
		{
			for( INT Z=Z0; Z<=Z1; Z++ )
			{
				INT iLocation;
				FActorLink **Link = &GetHashLink( X, Y, Z, iLocation );
				while( *Link )
				{
					if( (*Link)->Actor != Actor )
					{
						Link = &(*Link)->Next;
					}
					else
					{
						FActorLink *Scrap = *Link;
						*Link             = (*Link)->Next;
						Scrap->Next       = GAvailable;
						GAvailable	      = Scrap;
						GUsed--;
					}
				}
			}
		}
	}
	CheckActorNotReferenced( Actor );
	unguard;
}

/*-----------------------------------------------------------------------------
	FCollisionHash collision checking.
-----------------------------------------------------------------------------*/

//
// Make a list of all actors which overlap with a cyllinder at Location
// with the given collision size.
//
FCheckResult* FCollisionHash::PointCheck
(
	FMemStack&		Mem,
	FVector			Location,
	FVector			Extent,
	DWORD			ExtraNodeFlags,
	ALevelInfo*		Level,
	BOOL			bActors
)
{
	guard(FCollisionHash::PointCheck);
	checkState(CollisionInitialized);

	// Get extent indices.
	INT X0,Y0,Z0,X1,Y1,Z1;
	GetHashIndices( Location - Extent, X0, Y0, Z0 );
	GetHashIndices( Location + Extent, X1, Y1, Z1 );
	FCheckResult *Result, **PrevLink = &Result;
	CollisionTag++;

	// Check with level.
	if( Level )
	{
		FCheckResult TestHit(1.0);
		if( Level->XLevel->Model->PointCheck( TestHit, NULL, Location, Extent, 0 )==0 )
		{
			// Hit.
			TestHit.Actor = Level;
			*PrevLink     = new(GMem)FCheckResult;
			**PrevLink    = TestHit;
			PrevLink      = &(*PrevLink)->GetNext();
		}
	}

	// Check all actors in this neighborhood.
	if( bActors )
	{
		for( INT X=X0; X<=X1; X++ ) for( INT Y=Y0; Y<=Y1; Y++ ) for( INT Z=Z0; Z<=Z1; Z++ )
		{
			INT iLocation;
			for( FActorLink *Link = GetHashLink( X, Y, Z, iLocation ); Link; Link=Link->Next )
			{
				// Skip if we've already checked this actor.
				if( Link->Actor->CollisionTag == CollisionTag )
					continue;
				Link->Actor->CollisionTag = CollisionTag;

				// Collision test.
				FCheckResult TestHit(1.0);
				if( Link->Actor->GetPrimitive()->PointCheck( TestHit, Link->Actor, Location, Extent, 0 )==0 )
				{
					checkState(TestHit.Actor==Link->Actor);
					*PrevLink  = new(GMem)FCheckResult;
					**PrevLink = TestHit;
					PrevLink   = &(*PrevLink)->GetNext();
				}
			}
		}
	}
	*PrevLink = NULL;
	return Result;
	unguard;
}

//
// Check for encroached actors.
//
FCheckResult* FCollisionHash::EncroachmentCheck
(
	FMemStack&		Mem,
	AActor*			Actor,
	FVector			Location,
	FRotation		Rotation,
	DWORD			ExtraNodeFlags
)
{
	guard(FCollisionHash::EncroachmentCheck);
	checkState(CollisionInitialized);
	checkInput(Actor!=NULL);

	// Save actor's location and rotation.
	Exchange( Location, Actor->Location );
	Exchange( Rotation, Actor->Rotation );

	// Get extent indices.
	INT X0,Y0,Z0,X1,Y1,Z1;
	GetActorExtent( Actor, X0, X1, Y0, Y1, Z0, Z1 );
	FCheckResult *Result, **PrevLink = &Result;
	CollisionTag++;

	// Check all actors in this neighborhood.
	for( INT X=X0; X<=X1; X++ ) for( INT Y=Y0; Y<=Y1; Y++ ) for( INT Z=Z0; Z<=Z1; Z++ )
	{
		INT iLocation;
		for( FActorLink *Link = GetHashLink( X, Y, Z, iLocation ); Link; Link=Link->Next )
		{
			// Skip if we've already checked this actor.
			if( Link->Actor->CollisionTag == CollisionTag )
				continue;
			Link->Actor->CollisionTag = CollisionTag;

			// Collision test.
			FCheckResult TestHit(1.0);
			if
			(	!Link->Actor->Brush
			&&	Link->Actor!=Actor
			&&	Actor->GetPrimitive()->PointCheck( TestHit, Actor, Link->Actor->Location, Link->Actor->GetCollisionExtent(), 0 )==0 )
			{
				TestHit.Actor     = Link->Actor;
				TestHit.Primitive = NULL;
				*PrevLink         = new(GMem)FCheckResult;
				**PrevLink        = TestHit;
				PrevLink          = &(*PrevLink)->GetNext();
			}
		}
	}

	// Restore actor's location and rotation.
	Exchange( Location, Actor->Location );
	Exchange( Rotation, Actor->Rotation );

	*PrevLink = NULL;
	return Result;
	unguard;
}

//
// Check for nearest hit.
// Return 1 if no hit, 0 if hit.
//
int FCollisionHash::SinglePointCheck
(
	FCheckResult&	Hit,
	FVector			Location,
	FVector			Extent,
	DWORD			ExtraNodeFlags,
	ALevelInfo*		Level,
	BOOL			bActors
)
{
	guard(FCollisionHash::SinglePointCheck);
	FMemMark Mark(GMem);
	FCheckResult* Hits = PointCheck( GMem, Location, Extent, ExtraNodeFlags, Level, bActors );
	if( !Hits )
		return 1;
	Hit = *Hits;
	for( Hits = Hits->GetNext(); Hits!=NULL; Hits = Hits->GetNext() )
		if( (Hits->Location-Location).SizeSquared() < (Hit.Location-Location).SizeSquared() )
			Hit = *Hits;
	Mark.Pop();
	return 0;
	unguard;
}

//
// Make a time-sorted list of all actors which overlap a cyllinder moving 
// along a line from Start to End. If LevelInfo is specified, also checks for
// collision with the level itself and terminates collision when the trace
// hits solid space.
//
// Note: This routine is very inefficient for large lines, because it checks
// collision with all actors inside a bounding box containing the line's start
// and end point. This is a reasonable approach for regular actor movement
// like player walking, but it becomes very inefficient for long line traces, such
// as checking the collision of a bullet. To handle these cases, it would be smart
// to do the following optimizations:
//
// * Only test hash cubes which the line actually falls into. This could be done using
//   a raycasting-style algorithm.
//
// * Stop tracing once we have hit an actor which we know is guaranteed to be the 
//   nearest possible actor along the line.
//
FCheckResult* FCollisionHash::LineCheck
(
	FMemStack		&Mem,
	FVector			Start,
	FVector			End,
	FVector			Size,
	BOOL			bCheckActors,
	ALevelInfo*		LevelInfo
)
{
	guard(FCollisionHash::LineCheck);
	checkState(CollisionInitialized);

	// Update collision tag.
	CollisionTag++;

	// Get extent.
	INT X0,Y0,Z0,X1,Y1,Z1;
	FBoundingBox Box( FBoundingBox(0) + Start + End );
	GetHashIndices( Box.Min - Size, X0, Y0, Z0 );
	GetHashIndices( Box.Max + Size, X1, Y1, Z1 );
	
	// Init hits.
	INT NumHits=0;
	FCheckResult Hits[64];

	// Check for collision with the level, and cull the end point.
	if( LevelInfo && LevelInfo->XLevel->Model->LineCheck( Hits[NumHits], NULL, Start, End, Size, 0 )==0 )
	{
		Hits[NumHits].Actor = LevelInfo;
		End = Start + (End-Start) * Hits[NumHits++].Time;
	}

	// Check all potentially colliding actors in the hash.
	for( INT X=X0; X<=X1; X++ )
	{
		for( INT Y=Y0; Y<=Y1; Y++ )
		{
			for( INT Z=Z0; Z<=Z1; Z++ )
			{
				INT iLocation;
				for( FActorLink *Link = GetHashLink( X, Y, Z, iLocation ); Link; Link=Link->Next )
				{
					// Skip if we've already checked this actor.
					if( Link->Actor->CollisionTag == CollisionTag )
						continue;
					Link->Actor->CollisionTag = CollisionTag;

					// Check collision.
					Hits[NumHits].Actor = NULL;
					if( Link->Actor->GetPrimitive()->LineCheck( Hits[NumHits], Link->Actor, Start, End, Size, 0 )==0 )
					{
						checkState(Hits[NumHits].Actor!=NULL);
						if( ++NumHits >= ARRAY_COUNT(Hits) )
							goto Finished;
					}
				}
			}
		}
	}

	// Sort the list.
	Finished:
	FCheckResult* Result = NULL;
	if( NumHits > 0 )
	{
		Result = new(Mem,NumHits)FCheckResult;
		QSort( Hits, NumHits );
		for( int i=0; i<NumHits; i++ )
		{
			Result[i]      = Hits[i];
			Result[i].Next = (i+1<NumHits) ? &Result[i+1] : NULL;
		}
	}
	return Result;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
