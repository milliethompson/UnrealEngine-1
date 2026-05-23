/*=============================================================================
	UnReach.cpp: Reachspec creation and management

  These methods are members of the FReachSpec class, 

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Steven Polge 3/97
=============================================================================*/
#include "Unreal.h"
#include "UnPath.h"

/*
supports() -
 returns true if it supports the requirements of aPawn.  Distance is not considered.
*/
int FReachSpec::supports (APawn * pawn)
{
	guard(FReachSpec::supports);
	int result = (CollisionRadius >= pawn->CollisionRadius) && 
		(CollisionHeight >= pawn->CollisionHeight);
	if (pawn->Physics == PHYS_Flying)
		result = result && (reachFlags & R_FLY);
	else
		result = result && (reachFlags & R_WALK);

	//FIXME - check the other flags too

	return result; 
	unguard;
}

/* 
+ adds two reachspecs - returning the combined reachability requirements and total distance 
Note that Start, End, and Goal are not set
*/
FReachSpec FReachSpec::operator+ (const FReachSpec &Spec) const
{
	guard(FReachSpec::operator+);
	FReachSpec Combined;
	
	Combined.CollisionRadius = Min(CollisionRadius, Spec.CollisionRadius);
	Combined.CollisionHeight = Min(CollisionHeight, Spec.CollisionHeight);
	Combined.jumpZ = Max(jumpZ, Spec.jumpZ);
	Combined.dropZ = Max(dropZ, Spec.dropZ);

	//combine all the flags
	Combined.reachFlags = (reachFlags & Spec.reachFlags & (R_WALK + R_FLY)); 
	Combined.reachFlags = Combined.reachFlags + 
						((reachFlags | Spec.reachFlags) & (R_AIR + R_WATER + R_LAVA + R_SLIME));

	Combined.distance = distance + Spec.distance;
	
	return Combined; 
	unguard;
}
/* operator <=
Used for comparing reachspecs for choosing the best one
less than means that this either has easier reach requirements (equal or easier in all categories,
or equal reach requirements and lower distance.
Note that for reachspecs < and > are not symmetric! (could have mixed ease of reach)
does not compare goal, start, and end
*/
int FReachSpec::operator<= (const FReachSpec &Spec)
{
	guard(FReachSpec::operator<=);
	int result = (distance <= Spec.distance) && 
		(CollisionRadius <= Spec.CollisionRadius) &&
		(CollisionHeight <= Spec.CollisionHeight) &&
		(jumpZ <= Spec.jumpZ) &&
		(dropZ <= Spec.dropZ) &&
		((reachFlags & Spec.reachFlags) == Spec.reachFlags);
	return result; 
	unguard;
}

/* operator ==
Used for comparing reachspecs for choosing the best one
does not compare goal, start, end, and next
*/
int FReachSpec::operator== (const FReachSpec &Spec)
{
	guard(FReachSpec::operator ==);
	int result = (distance == Spec.distance) && 
		(CollisionRadius == Spec.CollisionRadius) &&
		(CollisionHeight == Spec.CollisionHeight) &&
		(jumpZ == Spec.jumpZ) &&
		(dropZ == Spec.dropZ) &&
		(reachFlags == Spec.reachFlags);
	
	return result; 
	unguard;
}

void FReachSpec::changeScoutExtent(APawn * Scout, FLOAT Radius, FLOAT Height)
{
	guard(FReachSpec::changeScoutExtent);

	Scout->GetLevel()->Hash.RemoveActor( Scout );
	Scout->CollisionRadius = Radius;
	Scout->CollisionHeight = Height;
	Scout->GetLevel()->Hash.AddActor( Scout );

	unguard;
}

/* defineFor()
initialize the reachspec for a  traversal from start actor to end actor.
Note - this must be a direct traversal (no routing).
Returns 1 if the definition was successful (there is such a reachspec), and zero
if no definition was possible
*/

int FReachSpec::defineFor(AActor * begin, AActor * dest, APawn * Scout)
{
	guard(FReachSpec::defineFor);
	reachFlags = 0;
	jumpZ = 0.0;
	dropZ = 0.0;
	Start = begin;
	End = dest;
	Scout->Physics = PHYS_Walking;
	Scout->JumpZ = 280.0; //FIXME- test with range of JumpZ values - or let reachable code set max needed
	Scout->GroundSpeed = 320.0; //FIXME? -in best reachable, go through range of creatures?
	Scout->MaxStepHeight = 24; //FIXME - get this stuff from human class

	int result = 0;
	if (findBestReachable(Start->Location, End->Location,Scout))
	{
		jumpZ = 0.0; 	//FIXME = reachable code should set jumpZ and dropZ
		dropZ = 0.0;
		result = 1;
		reachFlags = reachFlags | R_WALK;
		CollisionRadius = Scout->CollisionRadius;
		CollisionHeight = Scout->CollisionHeight;
		FVector path = End->Location - Start->Location;
		distance = path.Size(); //fixme - reachable code should calculate
		//FIXME = reachable code should determine zones too (and send back info)
		//or just fail on unacceptable? (lava)
		//maybe reachable code builds ReachInfo as it goes along
	}

	/*
	FIXMEs - get rid of testmoveactor?

	Scout->Physics = PHYS_Flying;
	if (findBestReachable(Start->Location, End->Location,Scout))
	{
		reachFlags = reachFlags | R_FLY;
		if (!result)
		{
			result = 1;
			CollisionRadius = Scout->CollisionRadius;
			CollisionHeight = Scout->CollisionHeight;
			FVector path = End->Location - Start->Location;
			distance = path.Size(); //fixme - reachable code should calculate
		}
	}
	*/

	return result; 
	unguard;
}

int FReachSpec::findBestReachable(FVector &begin, FVector &Destination, APawn * Scout)
{
	guard(FReachSpec::findBestReachable);
	changeScoutExtent(Scout, 18.0, 30.0);

	int result = 0;
	FLOAT stepsize = MAXCOMMONRADIUS - Scout->CollisionRadius;
	int success;
	int stilltrying = 1;
	FLOAT bestRadius = 0;
	FLOAT bestHeight = 0;
	//debugf("Find reachspec from %f %f %f to %f %f %f", begin.X, begin.Y, begin.Z,
	//	Destination.X, Destination.Y, Destination.Z);
	while (stilltrying) //find out max radius
	{
		success = Scout->GetLevel()->FarMoveActor( Scout, begin);
		if (success)
			success = Scout->pointReachable(Destination);

		if (success)
		{
			result = 1;
			bestRadius = Scout->CollisionRadius;
			changeScoutExtent(Scout, Scout->CollisionRadius + stepsize, Scout->CollisionHeight);
			stepsize *= 0.5;
			if (stepsize < 2)
				stilltrying = 0;
			else if (Scout->CollisionRadius > MAXCOMMONRADIUS)
				stilltrying = 0;
		}
		else
		{
			changeScoutExtent(Scout, Scout->CollisionRadius - stepsize, Scout->CollisionHeight);
			stepsize *= 0.5;
			if (stepsize < 2)
				stilltrying = 0;
			else if (Scout->CollisionRadius < 18)
				stilltrying = 0;
		}
	}
	
	if (result)
	{
		changeScoutExtent(Scout, bestRadius, Scout->CollisionHeight + 4);
		bestHeight = Scout->CollisionHeight;
		stilltrying = 1;
		stepsize = MAXCOMMONHEIGHT - Scout->CollisionHeight; 
	}

	while (stilltrying) //find out max height
	{
		success = Scout->GetLevel()->FarMoveActor( Scout, begin);
		if (success)
			success = Scout->pointReachable(Destination);
		if (success)
		{
			bestHeight = Scout->CollisionHeight;
			changeScoutExtent(Scout, Scout->CollisionRadius, Scout->CollisionHeight + stepsize);
			stepsize *= 0.5;
			if (stepsize < 1.0)
				stilltrying = 0;
			else if (Scout->CollisionHeight > MAXCOMMONHEIGHT) 
				stilltrying = 0;
		}
		else
		{
			changeScoutExtent(Scout, Scout->CollisionRadius, Scout->CollisionHeight - stepsize);
			stepsize *= 0.5;
			if (stepsize < 1.0)
				stilltrying = 0;
			else if (Scout->CollisionHeight < 30.0)
				stilltrying = 0;
		}
	}

			changeScoutExtent(Scout, Scout->CollisionRadius, bestHeight);

	return result; 
	unguard;
}



