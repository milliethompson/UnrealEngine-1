/*=============================================================================
	UnLevCol.cpp: Level actor collision code.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Design goal:
	To be self-contained. This collision code maintains its own collision hash
	table and doesn't rely on any far-away data structures like the BSP.

Revision history:
	* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

// Statistics.
static int GActorsAdded=0,GFragsAdded=0,GUsed=0,GChecks=0;

/*-----------------------------------------------------------------------------
	Collision statics.
-----------------------------------------------------------------------------*/

// ULevel statics.
INT ULevel::InitializedBasis=0;
INT ULevel::HashX[NUM_BUCKETS];
INT ULevel::HashY[NUM_BUCKETS];
INT ULevel::HashZ[NUM_BUCKETS];	

// Global pool of available links.
static ULevel::FActorLink *GAvailable = NULL;

// Debugging.
#define HASH_ALL_TO_SAME_BUCKET 0 /* Should be 0 */

/*-----------------------------------------------------------------------------
	Init and exit.
-----------------------------------------------------------------------------*/

//
// Initialize the actor collision information.
//
void ULevel::collisionInit()
{
	guard(ULevel::collisionInit);
	checkState(!CollisionInitialized);
	checkState(!IsLocked());
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

	// Init all actor collision hashes.
	for( int i=0; i<Actors->Num; i++ )
		if( Actors(i) )
			Actors(i)->Hash = NULL;

	// Init hash table.
	for( i=0; i<NUM_BUCKETS; i++ )
		Hash[i] = NULL;

	// Note that we're initialized.
	CollisionInitialized = 1;

	// Add all actors.
	for( i=0; i<Actors->Num; i++ )
		if( Actors(i) && Actors(i)->bCollideActors )
			collisionAddActor( Actors(i) );

	unguard;
}

//
// Shut down the actor collision information.
//
void ULevel::collisionExit()
{
	guard(ULevel::collisionExit);
	checkState(CollisionInitialized);
	checkState(!IsLocked());

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
	Collision tick - clean up collision info.
-----------------------------------------------------------------------------*/

//
// Cleanup the collision info.
//
void ULevel::collisionTick()
{
	guard(ULevel::collisionTick);
	checkState(CollisionInitialized);
	checkState(IsLocked());

	// Nothing to do.
	//debugf("Used=%i Added=%i Frags=%i Checks=%i",GUsed,GActorsAdded,GFragsAdded,GChecks);
	GActorsAdded=GFragsAdded=GChecks=0;

	unguard;
}

/*-----------------------------------------------------------------------------
	Adding and removing actors.
-----------------------------------------------------------------------------*/

//
// Add an actor to the collision info.
//
void ULevel::collisionAddActor( AActor *Actor )
{
	guard(ULevel::collisionAddActor);
	checkState(CollisionInitialized);
	checkInput(Actor->bCollideActors);

	if( Actor->bDeleteMe )
		return;

	//debugf("Adding %s",Actor->Class->GetName());
	GActorsAdded++;

	// Get extent.
	INT X0,Y0,Z0,X1,Y1,Z1;
	FVector Extent( Actor->CollisionRadius, Actor->CollisionRadius, Actor->CollisionHeight );
	collisionGetHashIndices( Actor->Location - Extent, X0, Y0, Z0 );
	collisionGetHashIndices( Actor->Location + Extent, X1, Y1, Z1 );

	// Add actor in all the specified places.
	for( INT X=X0; X<=X1; X++ )
	{
		for( INT Y=Y0; Y<=Y1; Y++ )
		{
			for( INT Z=Z0; Z<=Z1; Z++ )
			{
				INT iLocation;
				FActorLink *&Link = collisionGetHashLink( X, Y, Z, iLocation );
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
					Link        = new FActorLink( Actor, Link, iLocation );
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
void ULevel::collisionRemoveActor( AActor *Actor )
{
	guard(ULevel::collisionRemoveActor);
	checkState(CollisionInitialized);
	checkInput(Actor->bCollideActors);
	//debugf("Removing %s",Actor->Class->GetName());

	// Get extent.
	INT X0,Y0,Z0,X1,Y1,Z1;
	FVector Extent( Actor->CollisionRadius, Actor->CollisionRadius, Actor->CollisionHeight );
	collisionGetHashIndices( Actor->ColLocation - Extent, X0, Y0, Z0 );
	collisionGetHashIndices( Actor->ColLocation + Extent, X1, Y1, Z1 );

	// Remove actor.
	for( INT X=X0; X<=X1; X++ )
	{
		for( INT Y=Y0; Y<=Y1; Y++ )
		{
			for( INT Z=Z0; Z<=Z1; Z++ )
			{
				INT iLocation;
				FActorLink **Link=&collisionGetHashLink( X, Y, Z, iLocation );
				while( *Link )
				{
					if( (*Link)->Actor!=Actor || (*Link)->iLocation!=iLocation )
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
	unguard;
}

/*-----------------------------------------------------------------------------
	Collision checking.
-----------------------------------------------------------------------------*/

//
// Make a list of all actors which overlap with a cyllinder at Location
// with the given collision size.
//
int ULevel::collisionPointCheck
(
	const FVector &Location,
	FLOAT	      Radius,
	FLOAT	      Height,
	AActor	      **List,
	INT		      ListMax
)
{
	guard(ULevel::collisionPointCheck);
	checkState(CollisionInitialized);
	checkState(IsLocked());

	// Get extent.
	INT X0,Y0,Z0,X1,Y1,Z1;
	FVector Extent( Radius, Radius, Height );
	collisionGetHashIndices( Location - Extent, X0, Y0, Z0 );
	collisionGetHashIndices( Location + Extent, X1, Y1, Z1 );
	//debugf("Size = %i",(X1+1-X0)*(Y1+1-Y0)*(Z1+1-Z0));

	// Check all potentially colliding actors.
	INT ListCount = 0;
	FLOAT RadiusSquared = Square(Radius);
	for( INT X=X0; X<=X1; X++ )
	{
		for( INT Y=Y0; Y<=Y1; Y++ )
		{
			for( INT Z=Z0; Z<=Z1; Z++ )
			{
				INT iLocation;
				for( FActorLink *Link = collisionGetHashLink( X, Y, Z, iLocation ); Link; Link=Link->Next )
				{
					FVector Delta = Location - Link->Actor->Location;
					if
					(	(Delta.Z > -Height)
					&&	(Delta.Z <  Height)
					&&	(Delta.SizeSquared2D() < RadiusSquared) )
					{
						// Add to list.
						for( int i=0; i<ListCount; i++ )
							if( List[i] == Link->Actor )
								break;
						if( i >= ListCount )
							List[ListCount++] = Link->Actor;

						if( ListCount >= ListMax )
							return ListCount;
					}
				}
			}
		}
	}
	return ListCount;
	unguard;
}

//
// Make a time-sorted list of all actors which overlap a cyllinder moving 
// along a line from Start to End.
//
//Note: This routine is very inefficient for large lines, because it checks
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
int ULevel::collisionLineCheck
(
	const FVector	&Start,
	const FVector	&End,
	FLOAT			Radius,
	FLOAT			Height,
	FActorHit		*List,
	INT				ListMax 
)
{
	guard(ULevel::collisionLineCheck);
	checkState(CollisionInitialized);
	checkState(IsLocked());

	// Get extent.
	INT X0,Y0,Z0,X1,Y1,Z1;
	FVector Min=Start; Min.UpdateMinWith(End);
	FVector Max=Start; Max.UpdateMaxWith(End);
	FVector Extent( Radius, Radius, Height );
	collisionGetHashIndices( Min - Extent, X0, Y0, Z0 );
	collisionGetHashIndices( Max + Extent, X1, Y1, Z1 );
	//debugf("Size = %i",(X1+1-X0)*(Y1+1-Y0)*(Z1+1-Z0));

	// Get direction.
	FVector Vector = End - Start;

	// Check all potentially colliding actors.
	INT ListCount = 0;
	FLOAT RadiusSquared = Square(Radius);
	for( INT X=X0; X<=X1; X++ )
	{
		for( INT Y=Y0; Y<=Y1; Y++ )
		{
			for( INT Z=Z0; Z<=Z1; Z++ )
			{
				INT iLocation;
				for( FActorLink *Link = collisionGetHashLink( X, Y, Z, iLocation ); Link; Link=Link->Next )
				{
					GChecks++;
					FLOAT   NetRadius = Radius + Link->Actor->CollisionRadius;
					FLOAT   NetHeight = Height + Link->Actor->CollisionHeight;
					FVector Location  = Link->Actor->Location;

					// Quick X reject.
					FLOAT MaxX = Location.X + NetRadius;
					if( Start.X>MaxX && End.X>MaxX )
						continue;
					FLOAT MinX = Location.X - NetRadius;
					if( Start.X<MinX && End.X<MinX )
						continue;

					// Quick Y reject.
					FLOAT MaxY = Location.Y + NetRadius;
					if( Start.Y>MaxY && End.Y>MaxY )
						continue;
					FLOAT MinY = Location.Y - NetRadius;
					if( Start.Y<MinY && End.Y<MinY )
						continue;

					// Quick Z reject.
					FLOAT TopZ = Location.Z + NetHeight;
					if( Start.Z>TopZ && End.Z>TopZ )
						continue;
					FLOAT BotZ = Location.Z - NetHeight;
					if( Start.Z<BotZ && End.Z<BotZ )
						continue;

					// Skip if already in list.
					for( int i=0; i<ListCount; i++ )
						if( List[i].Actor == Link->Actor )
							continue;

					// Clip to top of cyllinder.
					FLOAT T0=0.0, T1=1.0;
					FVector HitNormal;
					if( Start.Z>TopZ && End.Z<TopZ )
					{
						FLOAT T = (TopZ - Start.Z)/(End.Z - Start.Z);
						if( T > T0 )
						{
							T0 = T;
							HitNormal = FVector(0,0,1);
						}
					}
					else if( Start.Z<TopZ && End.Z>TopZ )
						T1 = ::Min( T1, (TopZ - Start.Z)/(End.Z - Start.Z) );

					// Clip to bottom of cyllinder.
					if( Start.Z<BotZ && End.Z>BotZ )
					{
						FLOAT T = (BotZ - Start.Z)/(End.Z - Start.Z);
						if( T > T0 )
						{
							T0 = T;
							HitNormal = FVector(0,0,-1);
						}
					}
					else if( Start.Z>BotZ && End.Z<BotZ )
						T1 = ::Min( T0, (BotZ - Start.Z)/(End.Z - Start.Z) );
					
					// Reject.
					if( T0 >= T1 )
						continue;

					// Test setup.
					FLOAT   Kx        = Start.X - Location.X;
					FLOAT   Ky        = Start.Y - Location.Y;

					// 2D circle clip about origin.
					FLOAT   Vx        = End.X   - Start.X;
					FLOAT   Vy        = End.Y   - Start.Y;
					FLOAT   A         = Vx*Vx + Vy*Vy;
					FLOAT   B         = 2.0 * (Kx*Vx + Ky*Vy);
					FLOAT   C         = Kx*Kx + Ky*Ky - NetRadius*NetRadius;
					FLOAT   Discrim   = B*B - 4.0*A*C;

					// No intersection if discriminant is negative.
					if( Discrim < 0 )
						continue;

					// Unstable intersection if velocity is tiny.
					if( A < Square(0.0001) )
					{
						// Outside.
						if( C > 0 )
							continue;
					}
					else
					{
						// Compute intersection times.
						Discrim           = sqrt(Discrim);
						FLOAT R2A         = 0.5/A;
						T1                = ::Min( T1, +(Discrim-B) * R2A );
						FLOAT T           = -(Discrim+B) * R2A;
						if( T > T0 )
						{
							T0 = T;
							HitNormal = (Start + (End-Start)*T0 - Location).Normal();
						}
						if( T0 >= T1 )
							continue;
					}

					if( i >= ListCount )
					{
						List[ListCount].Actor     = Link->Actor;
						List[ListCount].Time      = ::Max(T0-0.001,0.0);
						List[ListCount].HitNormal = HitNormal;
						ListCount++;
					}
					if( ListCount >= ListMax )
						goto Filled;
				}
			}
		}
	}
	Filled:;

	// Sort the list.
	if( ListCount > 1 )
		QSort( List, ListCount );

	return ListCount;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
