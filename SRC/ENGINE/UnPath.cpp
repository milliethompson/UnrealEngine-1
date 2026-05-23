/*=============================================================================
	UnPath.cpp: Unreal pathnode placement

  These methods are members of the FPathBuilder class, which adds pathnodes to a level.  
  Paths are placed when the level is built.  FPathBuilder is not used during game play
 
   General comments:
   Path building
   The FPathBuilder does a tour of the level (or a part of it) using the "Scout" actor, starting from
   every actor in the level.  
   This guarantees that correct reachable paths are generated to all actors.  It starts by going to the
   wall to the right of the actor, and following that wall, keeping it to the left.  NOTE: my definition
   of left and right is based on normal Cartesian coordinates, and not screen coordinates (e.g. Y increases
   upward, not downward).  This wall following is accomplished by moving along the wall, constantly looking
   for a leftward passage.  If the FPathBuilder is stopped, he rotates clockwise until he can move forward 
   again.  While performing this tour, it keeps track of the set of previously placed pathnodes which are
   visible and reachable.  If a pathnode is removed from this set, then the FPathBuilder searches for an 
   acceptable waypoint.  If none is found, a new pathnode is added to provide a waypoint back to the no longer
   reachable pathnode.  The tour ends when a full circumlocution has been made, or if the FPathBuilder 
   encounters a previously placed path node going in the same direction.  Pathnodes that were not placed as
   the result of a left turn are marked, since they are due to some possibly unmapped obstruction.  In the
   final phase, these obstructions are inspected.  Then, the pathnode due to the obstruction is removed (since
   the obstruction is now mapped, and any needed waypoints have been built).

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

  FIXME - ledge and landing marking
  FIXME - mark door centers with left turns (reduces total paths)
  FIXME - paths on top of all platforms
	Revision history:
		* Created by Steven Polge 3/97
=============================================================================*/
#include "Unreal.h"
#include "UnPath.h"

//FIXME: add hiding places, camping and sniping spots (not pathnodes, but other keypoints)
int UNENGINE_API FPathBuilder::buildPaths (ULevel *ownerLevel, int optimization)
{
	guard(FPathBuilder::buildPaths);

	int numpaths = 0;
	numMarkers = 0;
	Level = ownerLevel;
	pathMarkers = new FPathMarker[MAXMARKERS]; //FIXME - use DArray?
	getScout();

	//base path building on human sized scout
	FLOAT humanRadius = 18; //FIXME - get from class
	FLOAT humanHeight = 30;

	Scout->CollisionRadius = humanRadius;
	Scout->CollisionHeight = humanHeight;
	Scout->JumpZ = 0;
	Scout->SetCollision(1, 1, 1); 
	Scout->GroundSpeed = 320; //FIXME?
	Scout->MaxStepHeight = 24; //FIXME
	//DebugFloat("collision radius", Scout->CollisionRadius);
	//DebugFloat("collision height", Scout->CollisionHeight);
	numpaths = numpaths + createPaths(optimization);

	return numpaths;
	unguard;
}

int UNENGINE_API FPathBuilder::removePaths (ULevel *ownerLevel)
{
	guard(FPathBuilder::removePaths);
	Level = ownerLevel;
	int removed = 0;

	for (INDEX i=0; i<Level->Num; i++)
	{
		AActor *Actor = Level->Element(i); 
		if (Actor && Actor->IsA("PathNode"))
		{
			removed++;
			Level->DestroyActor( Actor ); 
		}
	}
	return removed;
	unguard;
}

void UNENGINE_API FPathBuilder::definePaths (ULevel *ownerLevel)
{
	guard(FPathBuilder::definePaths);
	Level = ownerLevel;
	getScout();

	//calculate and add reachspecs to pathnodes
	debugf("Add reachspecs");
	for (INDEX i=0; i<Level->Num; i++)
	{
		AActor *Actor = Level->Element(i); 
		if (Actor && Actor->IsA("PathNode"))
			addReachSpecs((APathNode *)Actor);
	}
	debugf("All done");
	unguard;
}

//------------------------------------------------------------------------------------------------
//Private methods

/* add reachspecs to path for every path reachable from it. Also add the reachspec to that
paths upstreamPath list
*/
void FPathBuilder::addReachSpecs(APathNode *node)
{
	guard(FPathBuilder::addReachspecs);

	int n = 0;
	INDEX j;
	FReachSpec newSpec;
	debugf("Add Reachspecs for node at (%f, %f, %f)", node->Location.X,node->Location.Y,node->Location.Z);
	for (INDEX i=0; i<Level->Num; i++)
	{
		AActor *Actor = Level->Element(i); 
		if (Actor && Actor->IsA("PathNode") && (Actor != node))
		{
			newSpec.Init();
			if (newSpec.defineFor(node, Actor, Scout))
			{
				int iSpec = Level->ReachSpecs->Add(1);
				debugf("Add reachspec %d to node at (%f, %f, %f)", iSpec, Actor->Location.X,Actor->Location.Y,Actor->Location.Z);
				Level->ReachSpecs->Element(iSpec) = newSpec;
				node->Paths[n] = iSpec;
				n++;
				j = 0;
				while (j < 16) //find a spot on Actor's upstream list
				{
					if (((APathNode *)Actor)->upstreamPaths[j] == -1)
					{
						((APathNode *)Actor)->upstreamPaths[j] = iSpec;
						j = 16;
					}
					j++;
				}
			}
		}
	}
	unguard;
}

/* createPaths()
build paths for a given pawn type (which Scout is emulating)
*/
int FPathBuilder::createPaths (int optimization)
{
	guard(FPathBuilder::removePaths);
	int numpaths = 0;
	numMarkers = 0;

	//Add a permanent+leftturn+beacon+marked path for every pathnode actor in the level
	int newMarker;
	for (INDEX i=0; i<Level->Num; i++) 
	{
		AActor *Actor = Level->Element(i);
		if (Actor)
		{
			if (Actor->IsA("PathNode"))
			{
				DebugPrint("Found a Pathnode");
				newMarker = addMarker();
				pathMarkers[newMarker].initialize(Actor->Location,GMath.ZeroVector,1,1,1);
				pathMarkers[newMarker].permanent = 1;
			}
			else if (Actor->IsA("Pawn"))
				Actor->SetCollision(0, 0, 0); //temporarily turn off Pawn collision while placing paths
		}
	}

	// build paths from every actor position in level
	for (i=0; i<Level->Num; i++) 
	{
		AActor *Actor = Level->Element(i);
		if ( Actor && (!Actor->Location.IsZero()) && (!Actor->IsA("PathNode")) && (!Actor->IsA("Scout")))
		{
			DebugInt("----------------------Starting From", i);
			DebugVector("Location ", Actor->Location); 
			createPathsFrom(Actor->Location); 
		}
	}
	
	//iteratively fill out paths by checking for obstructions and by checking for new connections
	// until no more markers are added - unless opt0
	
		DebugInt("Markers before obstruction check =", numMarkers);
		DebugPrint("Check obstructions--------------------------------------------------");

	if (optimization < 2)
	{
		int notDone = optimization + 1;
		while (notDone)
		{
			int oldmarkers = numMarkers;
			for (i=0; i<oldmarkers; i++)  
			{
				if (pathMarkers[i].marked)
				{
					DebugInt("Check obstruction at",i);
					DebugInt("Out of", numMarkers);
					if (!pathMarkers[i].leftTurn) //only check from left turns at opt2
						checkObstructionFrom(&pathMarkers[i]);	
					pathMarkers[i].marked = 0; //once the obstruction is mapped, remove the path
				}
			}
			notDone--;
		}
	}
	else //optimization == 2
	{
		for (i=0; i<numMarkers; i++)  
		{
			if (pathMarkers[i].marked)
			{
				DebugInt("Check obstruction at",i);
				DebugInt("Out of", numMarkers);
			 	checkObstructionFrom(&pathMarkers[i]); //only check from left turns at opt2	
				pathMarkers[i].marked = 0; //once the obstruction is mapped, remove the path
			}
		}
	}

	//try to move left turn markers out from walls
	for (i=0; i<numMarkers; i++)  
	{
		if (pathMarkers[i].leftTurn)
			adjustPath(i);
	}

	// merge and prune left turn markers (based on reachability to beacons)
	for (i=0; i<numMarkers; i++)  
	{
		if (pathMarkers[i].leftTurn)
			mergePath(i);
	}

	DebugPrint("Build Paths");
	//Now create an actor for all remaining left turn markers (except permanent paths which already have a node
	for (i=0; i<numMarkers; i++)  
	{
		if (pathMarkers[i].leftTurn && !pathMarkers[i].permanent)
		{
			newPath(pathMarkers[i].Location);
			numpaths++;
		}
	}
	
	for (i=0; i<Level->Num; i++) 
	{
		AActor *Actor = Level->Element(i);
		if (Actor && (Actor->IsA("Pawn")))
			Actor->SetCollision(1, 1, 1); //turn Pawn collision back on
	}

	DebugInt("Optimization Level = ", optimization);
	DebugInt("Number of Markers =",numMarkers);
	return numpaths;
	unguard;
}

//newPath() 
//- add new pathnode to level at position spot
void FPathBuilder::newPath(FVector spot)
{
	guard(FPathBuilder::newPath);
	
	UClass *pathClass = new("PathNode",FIND_Existing)UClass;
	APathNode *addedPath = (APathNode *)Level->SpawnActor(pathClass, NULL, NAME_None, &spot);
	//clear pathnode reachspec lists
	for (INDEX i=0; i<16; i++)
	{
		addedPath->Paths[i] = -1;
		addedPath->upstreamPaths[i] = -1;
	}
	return;
	unguard;
	};


/*getScout()
Find the scout actor in the level. If none exists, add one.
*/ 

void FPathBuilder::getScout()
{
	guard(FPathBuilder::findScout);
	Scout = NULL;
	for( INDEX i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level->Element(i); 
		if (Actor && Actor->IsA("Scout"))
			Scout = (APawn *)Actor;
	}
	if( !Scout )
	{
		UClass *scoutClass = new("Scout",FIND_Existing)UClass;
		Scout = (APawn *)Level->SpawnActor(scoutClass);
	}

	return;
	unguard;
}

//createPathsFrom() -
//create paths beginning from wall to the right of start location
void FPathBuilder::createPathsFrom(FVector start)
{
	guard(FPathBuilder::createPathsFrom);
	
	if (Level->FarMoveActor(Scout, &start)) //move Scout to starting point
	{
		debugf("Scout found valid start"); 
		if (Level->DropToFloor(Scout))
		{
			debugf("scout placed on valid floor");
			//Move in x direction to nearest barrier, then explore it
			FVector moveDirection = GMath.ZeroVector;
			moveDirection.X = 1.0;
			exploreWall(moveDirection);
		} 
 	}
	else 
		debugf("Scout didn't fit");

	return;
	unguard;
}

int FPathBuilder::checkmergeSpot(const FVector &spot, FPathMarker *path1, FPathMarker *path2)
{
	guard(FPathBuilder::checkmergeSpot);
	int acceptable = 1;
	for (INDEX i=0; i<numMarkers; i++) //check if reachable path list changed
	{
		if (acceptable && pathMarkers[i].visible && pathMarkers[i].beacon)
		{
			if (!fullyReachable(spot,pathMarkers[i].Location))
			{
				path1->leftTurn = 0;
				path2->leftTurn = 0; //don't use either of these as a waypoint
				acceptable = findPathTo(pathMarkers[i].Location);
				path1->leftTurn = 1;
				path2->leftTurn = 1;
			}
			//if (!acceptable) DebugVector("failed because of",pathMarkers[i].Location);
		}
	}
	return acceptable;
	unguard;
}

/* adjustPath()
adjust the left turn marker out from its corner
*/
void FPathBuilder::adjustPath(INDEX iMarker)
{
	guard(FPathBuilder::adjustPath);
	
	FPathMarker *marker = &pathMarkers[iMarker];
	Level->FarMoveActor(Scout, &marker->Location);
	Level->DropToFloor(Scout); //FIXME - why do I need this?
	FVector OutDir;
	OutDir.X = 0.707106781 * (marker->Direction.X + marker->Direction.Y);
	OutDir.Y = 0.707106781 * (marker->Direction.Y - marker->Direction.X);
	OutDir.Z = 0.0;
	int stillmoving = 1;
	markReachable(marker->Location);  //mark all markers reachable from marker
	marker->leftTurn = 0; //don't use as waypoint
	FVector Destination = Scout->Location + OutDir * 1.414 * (MAXCOMMONRADIUS - Scout->CollisionRadius);
	FVector GoodSpot = marker->Location;

	while (stillmoving) 
	{
		stillmoving = walkToward(Destination,Scout->CollisionRadius * 0.25);
		if (needPath(Scout->Location))
			stillmoving = 0;
		else
			GoodSpot = Scout->Location; 
		FVector Distance = Destination - Scout->Location;
		if (Distance.SizeSquared() < 1.0)
			stillmoving = 0;
	}

	FVector Adjustment = GoodSpot - marker->Location;
	DebugInt("Adjusted path ", iMarker);
	DebugFloat("By ", Adjustment.Size()); 
	marker->Location = GoodSpot;
	marker->leftTurn = 1;
	unguard;
}

/* mergePath()
examine reachable path nodes. Try to merge this pathnode with another pathnode.  
Between it and every nearby reachable permanent pathnode, look for a point at which
all beacon pathnodes reachable from either can be reached from this point 
*/
void FPathBuilder::mergePath(INDEX iMarker)
{
	guard(FPathBuilder::mergePath);
	
	FPathMarker *marker = &pathMarkers[iMarker];
	float maxmergesqr = 16 * MAXWAYPOINTDIST * MAXWAYPOINTDIST * Scout->CollisionRadius * Scout->CollisionRadius;
	for (INDEX i=0; i<numMarkers; i++) 
	{
		FPathMarker *candidate = &pathMarkers[i];
		if (candidate->leftTurn && !candidate->permanent && (i != iMarker))
		{
			FVector direction = marker->Location - candidate->Location;
			if (direction.SizeSquared() < maxmergesqr )  
				if (fullyReachable(marker->Location, candidate->Location))
				{
					DebugVector("Try to merge path at", marker->Location);
					DebugVector("And path at", candidate->Location);
					markReachable(marker->Location);  //mark all markers reachable from marker
					int addedmarkers = 0;
					for (INDEX j=0; j<numMarkers; j++) // add those reachable from candidate 
					{
						if (!pathMarkers[j].visible && pathMarkers[j].beacon)
						{
							pathMarkers[j].visible = fullyReachable(candidate->Location, pathMarkers[j].Location);
							if (pathMarkers[j].visible)
								addedmarkers = 1;
						}
					}
					int acceptable = 0;
					if (marker->permanent) //then can't move it
					{
						acceptable = !addedmarkers;
						if (acceptable)
						{
							Level->FarMoveActor(Scout, &marker->Location);
							Level->DropToFloor(Scout); //FIXME - why do I need this?
						}
					}
					else //try to find suitable in-between point
					{
						FVector span = candidate->Location - marker->Location;
						FVector start = 0.5 * (marker->Location + candidate->Location);
						//First try between the two
						FVector dir;
						dir.X = -1.0/span.Y;
						dir.Y = span.X;
						dir.Z = 0.0;
						dir.Normalize();

						FVector end = start + dir * 0.5 * span.Size();
						Level->FarMoveActor(Scout, &start);
						Level->DropToFloor(Scout); //FIXME - why do I need this?
						int stillmoving = 1;
						while (stillmoving) 
						{
							acceptable = checkmergeSpot(Scout->Location, marker, candidate);
							if (!acceptable) 
								stillmoving = walkToward(end,Scout->CollisionRadius * 0.25);
							else
								stillmoving = 0;
						}

						if (!acceptable)
						{
							end = start - dir * 0.5 * span.Size();
							Level->FarMoveActor(Scout, &start);
							Level->DropToFloor(Scout); //FIXME - why do I need this?
							int stillmoving = 1;
							while (stillmoving) 
							{
								acceptable = checkmergeSpot(Scout->Location, marker, candidate);
								if (!acceptable) 
									stillmoving = walkToward(end,Scout->CollisionRadius * 0.25);
								else
									stillmoving = 0;
							}
						}

						if (!acceptable)
						{
							end = marker->Location;
							Level->FarMoveActor(Scout, &start);
							Level->DropToFloor(Scout); //FIXME - why do I need this?
							int stillmoving = 1;
							while (stillmoving) 
							{
								acceptable = checkmergeSpot(Scout->Location, marker, candidate);
								if (!acceptable) 
									stillmoving = walkToward(end,Scout->CollisionRadius * 0.25);
								else
									stillmoving = 0;
							}
						}

						if (!acceptable)
						{
							end = candidate->Location;
							Level->FarMoveActor(Scout, &start);
							Level->DropToFloor(Scout); //FIXME - why do I need this?
							int stillmoving = 1;
							while (stillmoving) 
							{
								acceptable = checkmergeSpot(Scout->Location, marker, candidate);
								if (!acceptable) 
									stillmoving = walkToward(end,Scout->CollisionRadius * 0.25);
								else
									stillmoving = 0;
							}
						}
					}
				
					if (acceptable) //found an acceptable merge point
						{
							DebugVector("Successful merge at", Scout->Location);
							marker->Location = Scout->Location; //move to merge point
							candidate->leftTurn = 0; //mark other node for removal
							candidate->beacon = 0; //other node no longer beacon
						}
				}
		}
	}
	
	return;
	unguard;
}

//checkObstructionsFrom() -
//pathnode was marked as not being created as result of left turn - which means some other (possibly unmapped)
//obstruction created it.  Explore this obstruction
//FIXME - why does it ever fail to find a path to the obstructed marker if the obstruction has been mapped?
//(e.g. cases where findPathTo fails, but no leftturn markers added to obstruction)
void FPathBuilder::checkObstructionFrom(FPathMarker *marker)
{
	guard(FPathBuilder::checkObstructionFrom);

	Level->FarMoveActor(Scout, &marker->Location);
	Level->DropToFloor(Scout); //FIXME - why do I need this?
	if (marker->leftTurn) //if this is a left turn marker, then walk at current direction (look for outside wall)
	{
		DebugPrint("exploring out from left turn");
		exploreWall(marker->Direction);
	}
	else
	{
		markReachable(marker->Location);
		Scout->walkMove(marker->Direction * Scout->CollisionRadius); //move to spot where obstruction was observed	
	
		for (INDEX i=0; i<numMarkers; i++) //check if visible+reachable path list changed
		{
			FPathMarker *checkMarker = &pathMarkers[i];
			if (checkMarker->visible && checkMarker->beacon)
				if (!fullyReachable(Scout->Location,checkMarker->Location)) //vis+reach changed - examine obstruction
					if (!findPathTo(checkMarker->Location))
					{
						DebugPrint("found the obstruction");
						FVector moveDirection = checkMarker->Location - Scout->Location;
						moveDirection.Z = 0;
						moveDirection.Normalize();
						exploreWall(moveDirection);
					}				 
		}
	}
	return;
	unguard;
}

void FPathBuilder::exploreWall(const FVector &moveDirection)
{
	guard(FPathBuilder::exploreWall);
	int stillmoving = 1;
	DebugVector("Move dir",moveDirection);
	while (stillmoving == 1)
		stillmoving = Scout->walkMove(moveDirection * Scout->CollisionRadius);
		
	DebugVector("Start location", Scout->Location);

	//  follow wall
	FVector newDirection = GMath.ZeroVector;
	newDirection.X = moveDirection.Y; 
	newDirection.Y = -1.0 * moveDirection.X; //turn clockwise 90 degrees
	nearestThirtyAngle(newDirection); //FIXME don't do this if I use normal to wall
	int oldMarkers = numMarkers;
	followWall(newDirection); 
	DebugInt("New paths created", numMarkers - oldMarkers);

	return;
	unguard;
}

/*
needPath()
checks if any paths or markers marked reachable are no longer reachable from start
if so, returns true
*/

int FPathBuilder::needPath(const FVector &start)
{
	guard(FPathBuilder::needPath);

	int need = 0;
	for (INDEX i=0; i<numMarkers; i++) //check if visible+reachable path list changed
	{
		if (!need && pathMarkers[i].visible && pathMarkers[i].beacon)
			if (!fullyReachable(start,pathMarkers[i].Location))
				need = !findPathTo(pathMarkers[i].Location); //vis+reach changed - look for an acceptable waypoint
	}
	return need;
	unguard;
}

/*
markReachable()
marks all beacon paths and markers as reachable or not from start.
*/

void FPathBuilder::markReachable(const FVector &start)
{
	guard(FPathBuilder::markReachable);

	for (INDEX i=0; i<numMarkers; i++) 
	{
		if (pathMarkers[i].beacon) 
			pathMarkers[i].visible = fullyReachable(start,pathMarkers[i].Location);
	}

	return;
	unguard;
}

/* oneWaypointTo()
looks for a NEARBY permanent waypoint which will reach to upstream spot.  Since scout has just made a left turn,
we are looking for a closeby waypoint which was also dropped as a result of the left turn (perhaps with
a different collision radius).  
*/
int FPathBuilder::oneWaypointTo(const FVector &upstreamSpot)
{
	guard(FPathBuilder::oneWaypointTo);
	int success = 0;
	FLOAT maxdistSquared = MAXWAYPOINTDIST * MAXWAYPOINTDIST * Scout->CollisionRadius * Scout->CollisionRadius;
	for (INDEX i=0; i<numMarkers; i++) 
	{
		if (!success && pathMarkers[i].leftTurn)
		{
			FVector distance = pathMarkers[i].Location - Scout->Location;
			if (distance.SizeSquared() < maxdistSquared)
				success = (fullyReachable(pathMarkers[i].Location, upstreamSpot) && fullyReachable(Scout->Location,pathMarkers[i].Location));
		}			
	}
	if (success)
		DebugPrint("Found an acceptable alternate left turn marker");
	return success;
	unguard;
}

/* addMarker()
returns index to a new marker
*/
INDEX FPathBuilder::addMarker()
{
	guard(FPathBuilder::addMarker);
	if (numMarkers < MAXMARKERS - 1)
		numMarkers++;
	else  //try to remove an old obstruction marker
	{
		int compressed = 0;
		INDEX i = 0;
		while (!compressed)
		{
			if (pathMarkers[i].removable())
			{
				pathMarkers[i] = pathMarkers[numMarkers - 1];
				compressed = 1;
			}
			i++;
			if (i == MAXMARKERS)
				compressed = 1;
		}
		DebugPrint("RAN OUT OF MARKERS!");
	}
	if (numMarkers > MAXMARKERS - 100) DebugInt("ADDED MARKER #", numMarkers);
	return (numMarkers - 1);
	unguard;
}

//followWall() - 
//follow wall, always keeping wall to my left
//look for reachable paths, and drop paths when my reachability list changes by removal
//stop when either I touch a path which was placed while going in the same direction as my
//current direction, or if I pass the last path I dropped
//Compare current direction to heading for start location.  
//If angle >= 90 degrees then I've passed it , and if NetYaw > 360, I've gone all the way around
//FIXME - to improve speed, remove redundant reachability checks
void FPathBuilder::followWall(FVector currentDirection)
{
	guard(FPathBuilder::followWall);
	int stillmoving = 1;  //This isn't the case, but I need to find out what it is really
	FVector newDirection;
	FLOAT NetYaw = 0.0;
	INDEX LastDropped = 0;
	INDEX LastRightTurn = 0;
	int turnedLeft = 0;
	FVector tempV;
	int turning = 0;
	FVector upstreamSpot = Scout->Location;
	FVector startLocation = Scout->Location;
	int keepMapping = 1;
	DebugVector("Following wall", currentDirection);

	while (keepMapping)
	{
		FVector oldPosition = Scout->Location;
		FVector oldDirection = currentDirection;
		if (checkLeftPassage(currentDirection)) //made a left turn
		{
			NetYaw = NetYaw - 90.0;
			if (!fullyReachable(Scout->Location, upstreamSpot))  //can I still reach my anchor? 
				if (!oneWaypointTo(upstreamSpot)) //check if there is a nearby legal waypoint 
				{
					upstreamSpot = oldPosition;
					LastDropped = addMarker();
					turnedLeft = 1;
					pathMarkers[LastDropped].initialize(oldPosition,oldDirection,1,1,1);
					NetYaw = 0.0;
					startLocation = oldPosition;
					DebugPrint("made left turn marker");
				}
		}
		else
		{
			stillmoving = Scout->walkMove(currentDirection * Scout->CollisionRadius);
			if (stillmoving == 1) //check for interior obstructions
			{	
				markReachable(oldPosition); //mark all visible/reachable turn and permanent paths from oldPosition
				if (needPath(Scout->Location)) //check if I need an obstruction marker at oldPosition
				{
					LastDropped = addMarker();
					pathMarkers[LastDropped].initialize(oldPosition,oldDirection,1,0,0);
					NetYaw = 0.0;
					startLocation = oldPosition;
					DebugVector("marked obstruction at", oldPosition);
				}
			}
		}

		if (stillmoving == 1) //check if tour is complete
		{
			turning = 0; //not in a turn
			//Stop if I touch a marker with the same direction as my current direction
			FLOAT touchRangeSquared = Scout->CollisionRadius * Scout->CollisionRadius * 0.25 * 0.25;
			for (INDEX i=0; i<numMarkers; i++) 
			{
				if (i != LastDropped) //Don't touch last dropped
				{
					tempV = pathMarkers[i].Location - Scout->Location;
					if (tempV.SizeSquared() < touchRangeSquared) //touching path
					{
						DebugVector("Near path at", pathMarkers[i].Location);
						tempV = currentDirection - pathMarkers[i].Direction;
						tempV.Normalize();
						DebugVector("Relative direction is", tempV);
						keepMapping = !tempV.IsNearlyZero(); //check if direction same as when path was laid
						if (!keepMapping) DebugVector("Touched a compatible marker at", pathMarkers[i].Location);
					}
				}
			}
		}	
		else //adjust direction clockwise (I couldn't go forward or left)
		{
			//FIXME: implement using normal of forward barrier 
			// make direction 90 degrees clockwise of xy component of normal
			//temporarily - just rotate around slowly (30 degrees at a time)
			//if current direction isn't multiple of 30, first correct to nearest 30 (so compatible path checks will work)
			upstreamSpot = Scout->Location; //set anchor at turn point
			DebugVector("turn right at", Scout->Location);
			if ((!turning) && (turnedLeft)) //check if its a stairstep right turn (odd angled wall)
					if (fullyReachable(Scout->Location, pathMarkers[LastRightTurn].Location))
					{
						turnedLeft = 0;
						turning = 1; //don't mark this turn
						DebugPrint("Not marking stair step right"); //FIXME - is this not working?
					}
			if (!turning) //mark turn
			{
				turning = 1;
				turnedLeft = 0;
				LastDropped = addMarker();
				LastRightTurn = LastDropped;
				pathMarkers[LastDropped].initialize(oldPosition,oldDirection,0,1,0);
				startLocation = oldPosition;
				NetYaw = 0.0;
				DebugPrint("made right turn marker");
			}
			if (angleNearThirty(currentDirection))
			{
				newDirection.Y = COS30 * currentDirection.Y - 0.5 * currentDirection.X;
				newDirection.X = COS30 * currentDirection.X + 0.5 * currentDirection.Y;
				NetYaw = NetYaw + 30.0;
			}
			else //correct clockwise to nearest multiple of 30 
			{
				newDirection = currentDirection;
				nearestThirtyAngle(newDirection);
			}
			newDirection.Z = 0.0;
			currentDirection = newDirection.Normal();
			stillmoving = 1;
			DebugVector("new direction", currentDirection);
		}
				
		//Alternate test for stopping (in case pathnode touch fails because it was the lastDropped)
		if (keepMapping && (Abs(NetYaw) >= 360.0))  //have rotated all the way around from start
		{ 
			tempV = startLocation - Scout->Location;
			keepMapping = ((tempV | currentDirection) > 0.0); //keep mapping if angle < 90 degrees
			if (!keepMapping) DebugVector("All the way around at",Scout->Location);
		}
	} //while keepMapping

	return;
	unguard;
}

//nearestThirtyAngle()
// returns the nearest direction whose xy angle is a multiple of 30, in the clockwise direction
// assumes the FVector passed is normalized 
//FIXME move the code above to here
void FPathBuilder::nearestThirtyAngle(FVector &currentDirection)
{	
	FVector newDirection;
	newDirection.Z = 0.0;
	int signX = 2 * (currentDirection.X >= 0) - 1; //+1 if positive, -1 if negative
	int signY = 2 * (currentDirection.Y >= 0) - 1;
	FLOAT magX = Abs(currentDirection.X);
	FLOAT magY = Abs(currentDirection.Y);
	DebugVector ("correct current direction of", currentDirection);

	if (signX == signY) //quadrants I and III - increase X
	{
		if (magX > COS30)
		{
			newDirection.X = 1.0 * signX;
			newDirection.Y = 0.0;
		}
		else if (magX > 0.5)
		{
			newDirection.X = COS30 * signX;
			newDirection.Y = 0.5 * signY;
		}
		else
		{
			newDirection.X = 0.5 * signX;
			newDirection.Y = COS30 * signY;
		}
	}
	else //quadrants II and IV - decrease X
	{
		if (magX > COS30)
		{
			newDirection.X = COS30 * signX;
			newDirection.Y = 0.5 * signY;
		}
		else if (magX > 0.5)
		{
			newDirection.X = 0.5 * signX;
			newDirection.Y = COS30 * signY;
		}
		else 
		{
			newDirection.X = 0.0;
			newDirection.Y = 1.0 * signY;
		}
	}
	DebugVector("Corrected direction = ",newDirection);
	currentDirection = newDirection;
	return;
};

//angleNearThirty()
//FIXME - make this a member of FVector?
// returns true if the xy angle of the vector is near a multiple of 30 degrees
// and z direction is zero
// use FVector::IsNearlyZero(), since that's my standard measure

int FPathBuilder::angleNearThirty(FVector dir)
{
	guard(FPathBuilder::angleNearThirty);
	int result = 0;
	dir.Normalize(); 
	if (dir.Z == 0.0)
	{
		dir.X = Abs(dir.X);
		FVector test = dir;
		test.Y = 0;  //since normalized, and Z=0, know Y from X
		if (test.IsNearlyZero())
			result = 1;
		else
		{
			test.X = test.X - 1.0;
			if (test.IsNearlyZero())
				result = 1;
			else
			{
				test.X = test.X + 0.5;
				if (test.IsNearlyZero())
					result = 1;
				else
				{
					test.X = test.X + 0.5 - COS30;
					if (test.IsNearlyZero())
						result = 1;
				}
			}
		}
	}

	return result;
	unguard;
}

/* tryPathThrough()
recursively try to find a path to destination from Waypoint that fits into the distance budget
check budget first, since its cheaper than the reachability test
*/
int FPathBuilder::tryPathThrough(FPathMarker *Waypoint, const FVector &Destination, FLOAT budget)
{
	guard(FPathBuilder::tryPathThrough);

	int result = 0;
	if (fullyReachable(Waypoint->Location,Destination))
		result = 1; //I know it fits into my budget, since I wouldn't have tried this waypoint if it didn't
	else
	{
		Waypoint->budget = budget;
		FVector direction;
		FLOAT distance;
		FLOAT minTotal;
		FPathMarker *NextNode;

		for (INDEX iNext=0; iNext<numMarkers; iNext++)  //check all reachable pathnodes
		{
			NextNode = &pathMarkers[iNext];
			if (!result && NextNode->leftTurn)	
			{
				direction = Waypoint->Location - NextNode->Location;
				distance = direction.Size();
				direction = NextNode->Location - Destination;
				minTotal = distance + direction.Size();
				if ((NextNode->budget < (budget - distance)) && (minTotal < budget)) //check not visited with better budget, and current min distance fits budget
					if (fullyReachable(Waypoint->Location,NextNode->Location)) //if fits in budget, try this path
						result = tryPathThrough(NextNode,Destination, budget - distance);
			}
		}
	}
	return result;
	unguard;
}

/* findPathTo() -
// iDest is no longer reachable from Scout's position, but was from oldPosition
(because of some obstruction)  If the obstruction is marked, there is an almost straight path
to iDest - look for that
*/
int FPathBuilder::findPathTo(const FVector &Destination)
{
	guard(FPathBuilder::findPathTo);
	FVector direction = Destination - Scout->Location;
	FLOAT budget = direction.Size() + Scout->CollisionRadius * (1 + 2 * MAXWAYPOINTDIST); 
		
	//clear budgets (temp used for storing remaining budget through that pathnode)
	for (INDEX i=0; i<numMarkers; i++)  
	{
			pathMarkers[i].budget = 0.0;	
	}
	
	FPathMarker ScoutMarker;
	ScoutMarker.Location = Scout->Location;
	int acceptable = tryPathThrough(&ScoutMarker,Destination,budget);
	return acceptable;
	unguard;
}

/* checkLeftPassage()
Looks for a left turn.  Returns 1 if left turn was made, zero otherwise.
If possible, changes currentDirection, and moves Scout 
FIXME - should it be based on normal of barrier to the left?
*/
int FPathBuilder::checkLeftPassage(FVector &currentDirection)
{
	guard(FPathBuilder::checkLeftPassage);
	FVector oldLocation = Scout->Location;
	FVector leftDirection;
	leftDirection.X = -1.0 * currentDirection.Y; //turn left 90 degrees
	leftDirection.Y = currentDirection.X;
	leftDirection.Z = 0;
	int leftTurn = 0;

	int walkresult = Scout->walkMove(leftDirection * Scout->CollisionRadius); 
	if (walkresult == 1) //check if it was a full move
	{
		FVector move = Scout->Location - oldLocation;
		walkresult = (move.Size() > 0.7 * Scout->CollisionRadius);  //FIXME - problem with steep slopes?
	}
	if (walkresult == 1) //explore this path
	{
		DebugVector("Follow left passage", leftDirection);
		DebugVector("Turned left at", oldLocation);
		currentDirection.X = leftDirection.X;
		currentDirection.Y = leftDirection.Y;
		leftDirection.X = -1.0 * currentDirection.Y; //turn left 90 degrees
		leftDirection.Y = currentDirection.X;
		Scout->walkMove(leftDirection * Scout->CollisionRadius); //get all the way over to wall (max one collisionradius back)
		leftTurn = 1;
		DebugVector("New location",Scout->Location);
	}
	/* else if (walkresult == -1) //FIXME: mark ledge?
	{
	} */
	else
	{
		Level->FarMoveActor(Scout, &oldLocation); //no left passage, go back to exploration
		Level->DropToFloor(Scout); //FIXME - why do I need this?
	}

	return leftTurn;
	unguard;
}

//check if there is a line of sight from start to destination, and if Scout can walk between the two
//only used for path building
//then check that here
int FPathBuilder::fullyReachable(FVector start,FVector destination)
{
	guard(FPathBuilder::fullyReachable);

	FVector oldPosition = Scout->Location;
	Level->FarMoveActor(Scout,&start);
	Scout->Physics = PHYS_Flying;
	int result = Scout->pointReachable(destination); 

	Scout->Physics = PHYS_Walking;
	if (result)
		result = Scout->pointReachable(destination); 

	if (result) //FIXME - remove the symmetric check- shouldn't be needed
	{
		Level->FarMoveActor(Scout,&destination);
		Level->DropToFloor(Scout); //FIXME - why do I need this?
		result = result	&& (Scout->walkReachable(start));
		if (!result) DebugPrint("Movement not symmetric!");
	}
	Level->FarMoveActor(Scout,&oldPosition);

	return result; 
	unguard;
}

//FIXME - methods below here should be AActor members
/* walkToward()
walk iActor toward a point.  Returns 1 if iActor successfully moved
*/
inline int FPathBuilder::walkToward(const FVector &Destination, FLOAT Movesize)
{
	guard(FPathBuilder::walkToward);
	FVector Direction = Destination - Scout->Location;
	Direction.Z = 0; //this is a 2D move
	FLOAT DistanceSquared = Direction.SizeSquared();
	int success = 0;

	if (DistanceSquared > 1.0) //move not too small to do
	{
		if (DistanceSquared < Movesize * Movesize)
			success = (Scout->walkMove(Direction) == 1);
		else
		{
			Direction.Normalize();
			success = (Scout->walkMove(Direction * Movesize) == 1);
		}
	}
	return success;
	unguard;
}



