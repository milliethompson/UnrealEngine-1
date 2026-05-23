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
static int GActorsAdded=0,GFragsAdded=0,GUsed=0,GChecks=0;

/*-----------------------------------------------------------------------------
	AActor collision functions.
-----------------------------------------------------------------------------*/

//
// Compute this actor's transformed collision bounding box.
//
inline void AActor::GetCollisionExtent( FVector &Min, FVector &Max ) const
{
	guardSlow(AActor::GetCollisionExtent);
	if( !Brush || !Brush->bCheckCollision )
	{
		// Get simple extent of actor.
		Min = FVector( Location.X - CollisionRadius, Location.Y - CollisionRadius, Location.Z - CollisionHeight );
		Max = FVector( Location.X + CollisionRadius, Location.Y + CollisionRadius, Location.Z + CollisionHeight );
	}
	else
	{
		// Get extent of actor's moving brush.
		Min = Brush->TransformedBound.Min;
		Max = Brush->TransformedBound.Max;
	}
	unguardSlow;
}

//
// See if a point is touching this actor.
// Returns 0 if collided, 1 if outside.
//
INT AActor::PointCheck
(
	FCheckResult	&Result,
	const FVector	&Point,
	DWORD			ExtraNodeFlags
)
{
	guardSlow(AActor::PointCheck);
	if( !Brush || !Brush->bCheckCollision )
	{
		// Treat this actor as a cyllinder.
		return
		(		Square(Location.Z-Point.Z)                            > Square(CollisionHeight)
		||      Square(Location.X-Point.X)+Square(Location.Y-Point.Y) > Square(CollisionRadius) );
	}
	else
	{
		// Check collision with brush.
		//!! must transform then detransform brushspace.
		return Brush->PointCheck( Result, this, Point, ExtraNodeFlags );
	}
	unguardSlow;
}

//
// See if a line is touching this actor.
// Returns 0 if collided, 1 if outside.
//
INT AActor::LineCheck
(
	FCheckResult	&Result,
	const FVector	&V1,
	const FVector	&V2,
	DWORD ExtraNodeFlags
)
{
	guardSlow(AActor::LineCheck);
	if( !Brush || !Brush->bCheckCollision )
	{
		// Use box-cyllinder test.
		return BoxLineCheck( Result, V1, V2, 0.0, 0.0, 0 );
	}
	else
	{
		// Use this actor's primitive for collision checking.
		return 1;
	}
	unguardSlow;
}

//
// See if a collision box is touching this actor.
// Returns 0 if collided, 1 if outside.
//
INT AActor::BoxPointCheck
(
	FCheckResult	&Result,
	const FVector   &Point,
	FLOAT           Radius,
	FLOAT           Height,
	DWORD           ExtraNodeFlags
)
{
	guardSlow(AActor::BoxPointCheck);
	if( !Brush || !Brush->bCheckCollision )
	{
		// Treat this actor as a cyllinder.
		return
		(		Square(Location.Z-Point.Z)                            > Square(CollisionHeight+Height)
		||      Square(Location.X-Point.X)+Square(Location.Y-Point.Y) > Square(CollisionRadius+Radius) );
	}
	else
	{
		// Use this actor's primitive for collision checking.
		return 1;
	}
	unguardSlow;
}

//
// See if a collision box moving along a line is touching this actor.
// Returns 0 if collided, 1 if outside.
//
INT AActor::BoxLineCheck
(
	FCheckResult	&Result,
	const FVector   &Start,
	const FVector   &End,
	FLOAT           Radius,
	FLOAT           Height,
	DWORD           ExtraNodeFlags
)
{
	guardSlow(AActor::BoxLineCheck);
	if( !Brush || !Brush->bCheckCollision )
	{
		// Treat this actor as a cyllinder.
		FLOAT   NetRadius = Radius + CollisionRadius;
		FLOAT   NetHeight = Height + CollisionHeight;

		// Quick X reject.
		FLOAT MaxX = Location.X + NetRadius;
		if( Start.X>MaxX && End.X>MaxX )
			return 1;
		FLOAT MinX = Location.X - NetRadius;
		if( Start.X<MinX && End.X<MinX )
			return 1;

		// Quick Y reject.
		FLOAT MaxY = Location.Y + NetRadius;
		if( Start.Y>MaxY && End.Y>MaxY )
			return 1;
		FLOAT MinY = Location.Y - NetRadius;
		if( Start.Y<MinY && End.Y<MinY )
			return 1;

		// Quick Z reject.
		FLOAT TopZ = Location.Z + NetHeight;
		if( Start.Z>TopZ && End.Z>TopZ )
			return 1;
		FLOAT BotZ = Location.Z - NetHeight;
		if( Start.Z<BotZ && End.Z<BotZ )
			return 1;

		// Clip to top of cyllinder.
		FLOAT T0=0.0, T1=1.0;
		if( Start.Z>TopZ && End.Z<TopZ )
		{
			FLOAT T = (TopZ - Start.Z)/(End.Z - Start.Z);
			if( T > T0 )
			{
				T0 = T;
				Result.Normal = FVector(0,0,1);
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
				Result.Normal = FVector(0,0,-1);
			}
		}
		else if( Start.Z>BotZ && End.Z<BotZ )
			T1 = ::Min( T0, (BotZ - Start.Z)/(End.Z - Start.Z) );
		
		// Reject.
		if( T0 >= T1 )
			return 1;

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
			return 1;

		// Unstable intersection if velocity is tiny.
		if( A < Square(0.0001) )
		{
			// Outside.
			if( C > 0 )
				return 1;
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
				Result.Normal   = (Start + (End-Start)*T0 - Location);
				Result.Normal.Z = 0;
				Result.Normal.Normalize();
			}
			if( T0 >= T1 )
				return 1;
		}
		Result.Time     = ::Max(T0-0.001,0.0);
		Result.Location = Start + (End-Start) * Result.Time;
		return 0;
	}
	else
	{
		// Use this actor's primitive for collision checking.
		//!! must transform then detransform brushspace.
		return Brush->BoxLineCheck( Result, this, Start, End, Radius, Height, ExtraNodeFlags );
	}
	unguardSlow;
}

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
	FVector Min,Max;
	Actor->GetCollisionExtent( Min, Max );

	// Discretize to hash coordinates.
	GetHashIndices( Min, X0, Y0, Z0 );
	GetHashIndices( Max, X1, Y1, Z1 );

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

	//debugf("Adding %s",Actor->Class->GetName());
	GActorsAdded++;

	// Get extent.
	INT X0,Y0,Z0,X1,Y1,Z1;
	GetActorExtent( Actor, X0, X1, Y0, Y1, Z0, Z1 );

	// Add actor in all the specified places.
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
void FCollisionHash::RemoveActor( AActor *Actor )
{
	guard(FCollisionHash::RemoveActor);
	checkState(CollisionInitialized);
	checkInput(Actor->bCollideActors);
	//debugf("Removing %s",Actor->Class->GetName());

	// Get extent.
	INT X0,Y0,Z0,X1,Y1,Z1;
	GetActorExtent( Actor, X0, X1, Y0, Y1, Z0, Z1 );

	// Remove actor.
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
	FCollisionHash collision checking.
-----------------------------------------------------------------------------*/

//
// Make a list of all actors which overlap with a cyllinder at Location
// with the given collision size.
//
int FCollisionHash::PointCheck
(
	const FVector &Location,
	FLOAT	      Radius,
	FLOAT	      Height,
	AActor	      **List,
	INT		      ListMax
)
{
	guard(FCollisionHash::PointCheck);
	checkState(CollisionInitialized);

	// Update collision tag.
	CollisionTag++;

	// Get extent.
	INT X0,Y0,Z0,X1,Y1,Z1;
	FVector Extent( Radius, Radius, Height );
	GetHashIndices( Location - Extent, X0, Y0, Z0 );
	GetHashIndices( Location + Extent, X1, Y1, Z1 );
	//debugf("Size = %i",(X1+1-X0)*(Y1+1-Y0)*(Z1+1-Z0));

	// Check all actors in this neighborhood.
	INT ListCount = 0;
	FLOAT RadiusSquared = Square(Radius);
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

					// Collision test.
					FCheckResult Hit;
					if( Link->Actor->BoxPointCheck( Hit, Location, Radius, Height, 0 )==0 )
					{
						// Hit.
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
int FCollisionHash::LineCheck
(
	FCheckResult	*Hits,
	INT				ListMax,
	const FVector	&Start,
	const FVector	&End,
	FLOAT			Radius,
	FLOAT			Height
)
{
	guard(FCollisionHash::LineCheck);
	checkState(CollisionInitialized);

	// Update collision tag.
	CollisionTag++;

	// Get extent.
	INT X0,Y0,Z0,X1,Y1,Z1;
	FVector Min=Start; Min.UpdateMinWith(End);
	FVector Max=Start; Max.UpdateMaxWith(End);
	FVector Extent( Radius, Radius, Height );
	GetHashIndices( Min - Extent, X0, Y0, Z0 );
	GetHashIndices( Max + Extent, X1, Y1, Z1 );
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
				for( FActorLink *Link = GetHashLink( X, Y, Z, iLocation ); Link; Link=Link->Next )
				{
					// Skip if we've already checked this actor.
					if( Link->Actor->CollisionTag == CollisionTag )
						continue;
					Link->Actor->CollisionTag = CollisionTag;

					// Check collision.
					if( Link->Actor->BoxLineCheck( Hits[ListCount], Start, End, Radius, Height, 0 ) == 0 )
					{
						// Collided.
						Hits[ListCount].Actor = Link->Actor;
						if( ++ListCount >= ListMax )
							goto Filled;
					}
				}
			}
		}
	}
	Filled:;

	// Sort the list.
	if( ListCount > 1 )
		QSort( Hits, ListCount );

	return ListCount;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
