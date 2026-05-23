/*=============================================================================
	UnPawn.cpp: APawn AI implementation

  This contains both C++ methods (movement and reachability), as well as some 
  AI related intrinsics

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Steven Polge 3/97
=============================================================================*/

#include "Unreal.h"
/*-----------------------------------------------------------------------------
	APawn object implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(APawn);

/*-----------------------------------------------------------------------------
	Pawn input.
-----------------------------------------------------------------------------*/

//
// Copy the input from a PPlayerTick structure to the member
// variables of a Pawn.
//
void APawn::inputCopyFrom( const PPlayerTick &Tick )
{
	guard(CopyInputTo);

	// Copy buttons.
	bZoom		= Tick.Buttons[BUT_Zoom];
	bRun		= Tick.Buttons[BUT_Run];
	bLook		= Tick.Buttons[BUT_Look];
	bDuck		= Tick.Buttons[BUT_Duck];
	bStrafe		= Tick.Buttons[BUT_Strafe];
	bFire		= Tick.Buttons[BUT_Fire];
	bAltFire	= Tick.Buttons[BUT_AltFire];
	bJump		= Tick.Buttons[BUT_Jump];
	bExtra3		= Tick.Buttons[BUT_Extra3];
	bExtra2		= Tick.Buttons[BUT_Extra2];
	bExtra1		= Tick.Buttons[BUT_Extra1];
	bExtra0		= Tick.Buttons[BUT_Extra0];

	// Copy axes.
	aForward	= Tick.Axis[AXIS_Forward];
	aTurn		= Tick.Axis[AXIS_Turn];
	aStrafe		= Tick.Axis[AXIS_Strafe];
	aUp			= Tick.Axis[AXIS_Up];
	aLookUp		= Tick.Axis[AXIS_LookUp];
	aExtra4		= Tick.Axis[AXIS_Extra4];
	aExtra3		= Tick.Axis[AXIS_Extra3];
	aExtra2		= Tick.Axis[AXIS_Extra2];
	aExtra1		= Tick.Axis[AXIS_Extra1];
	aExtra0		= Tick.Axis[AXIS_Extra0];

	unguard;
}


/*-----------------------------------------------------------------------------
	Pawn related functions.
-----------------------------------------------------------------------------*/


enum EAIFunctions
{
	AI_MoveTo = 500,
	AI_PollMoveTo = 501,
	AI_MoveToward = 502,
	AI_PollMoveToward = 503,
	AI_StrafeTo = 504,
	AI_PollStrafeTo = 505,
	AI_StrafeFacing = 506,
	AI_PollStrafeFacing = 507,
	AI_TurnTo = 508,
	AI_PollTurnTo = 509,
	AI_TurnToward = 510,
	AI_PollTurnToward = 511,
	AI_MakeNoise = 512,
	AI_LineOfSightTo = 514,
	AI_FloorZ = 516,
	AI_FindPathToward = 517,
	AI_FindPathTo = 518,
	AI_DescribeSpec = 519,
	AI_ActorReachable = 520,
	AI_PointReachable = 521,
	AI_ClearPaths = 522,
	AI_JumpLanding = 523
};

static void execJumpLanding( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execJumpLanding);
	debugState(Context!=NULL);
	APawn *PawnContext = (APawn *)Context;
	FVector Landing;
	FVector vel = PawnContext->Velocity;
	PawnContext->jumpLanding(vel, Landing, 0);

	*(FVector*)Result = Landing;
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_JumpLanding, execJumpLanding);

/* execDescribeSpec - temporary debug
*/
static void execDescribeSpec( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execDescribeSpec);
	debugState(Context!=NULL);
	AActor *ActorContext = (AActor *)Context;

	P_GET_INT(iSpec);
	P_FINISH;

	FReachSpec spec = ActorContext->GetLevel()->ReachSpecs->Element(iSpec);
	debugf("Reachspec from (%f, %f, %f) to (%f, %f, %f)", spec.Start->Location.X,
		spec.Start->Location.Y, spec.Start->Location.Z, spec.End->Location.X, 
		spec.End->Location.Y, spec.End->Location.Z);
	debugf("Height %f , Radius %f", spec.CollisionHeight, spec.CollisionRadius);
	if (spec.reachFlags & R_WALK)
		debugf("walkable");
	if (spec.reachFlags & R_FLY)
		debugf("flyable");

	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_DescribeSpec, execDescribeSpec);

static void execActorReachable( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execActorReachable);
	debugState(Context!=NULL);
	AActor *ActorContext = (AActor *)Context;

	P_GET_ACTOR(actor);
	P_FINISH;

	*(DWORD*)Result = ((APawn *)ActorContext)->actorReachable(actor);  
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_ActorReachable, execActorReachable);

static void execPointReachable( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPointReachable);
	debugState(Context!=NULL);
	AActor *ActorContext = (AActor *)Context;

	P_GET_VECTOR(point);
	P_FINISH;

	*(DWORD*)Result = ((APawn *)ActorContext)->pointReachable(point);  
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_PointReachable, execPointReachable);

/* FindPathTo()
returns the best pathnode toward a point - even if point is directly reachable
If there is no path, returns None
By default clears paths.  If script wants to preset some path weighting, etc., then
it can explicitly clear paths using execClearPaths before presetting the values and 
calling FindPathTo with clearpath = 0

  FIXME add optional bBlockDoors (no paths through doors), bBlockTeleporters, bBlockSwitches,
  maxNodes (max number of nodes), etc.
*/

static void execFindPathTo( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFindPathTo);
	debugState(Context!=NULL);
	APawn *PawnContext = (APawn *)Context;

	P_GET_VECTOR(point);
	P_GET_FLOAT_OPT(maxweight, 1000000.0);
	P_GET_BOOL_OPT(bClearPaths, 1);
	P_FINISH;

	if (bClearPaths)
		PawnContext->clearPaths();
	APathNode * bestPath = NULL;
	AActor * newPath;
	if (PawnContext->findPathTo(point, maxweight, newPath))
		bestPath = (APathNode *)newPath;

	*(APathNode**)Result = bestPath; 
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_FindPathTo, execFindPathTo);

static void execFindPathToward( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFindPathToward);
	debugState(Context!=NULL);
	APawn *PawnContext = (APawn *)Context;

	P_GET_ACTOR(goal);
	P_GET_FLOAT_OPT(maxweight, 1000000.0);
	P_GET_BOOL_OPT(bClearPaths, 1);
	P_FINISH;

	if (bClearPaths)
		PawnContext->clearPaths();
	APathNode * bestPath = NULL;
	AActor * newPath;
	if (PawnContext->findPathToward(goal, maxweight, newPath))
		bestPath = (APathNode *)newPath;

	*(APathNode**)Result = bestPath; 
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_FindPathToward, execFindPathToward);

static void execClearPaths( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFindPathTo);
	debugState(Context!=NULL);
	APawn *PawnContext = (APawn *)Context;

	PawnContext->clearPaths(); 
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_ClearPaths, execClearPaths);

/*MakeNoise
- check to see if other creatures can hear this noise
*/
static void execMakeNoise( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execMakeNoise);
	debugState(Context!=NULL);
	AActor *ActorContext = (AActor *)Context;

	P_GET_FLOAT(Loudness);
	P_FINISH;
	
	ActorContext->CheckNoiseHearing(Loudness);
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_MakeNoise, execMakeNoise);

static void execFloorZ( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloorZ);

	P_GET_VECTOR(point);
	P_FINISH;
	
	*(FLOAT*)Result = ((AActor *)Context)->FloorZ(point);
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_FloorZ, execFloorZ);

static void execLineOfSightTo( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execLineOfSightTo);
	debugState(Context!=NULL);
	debugState(Context->IsA("Pawn"));

	P_GET_ACTOR(Other);
	P_FINISH;

	*(DWORD*)Result = ((APawn *)Context)->LineOfSightTo(Other);
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_LineOfSightTo, execLineOfSightTo);

/* execMoveTo()
start moving to a point -does not use routing
Destination is set to a point
*/
static void execMoveTo( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execMoveTo);
	debugState(Context!=NULL);
	debugState(Context->IsA("Pawn"));
	APawn *PawnContext = (APawn*)Context;

	P_GET_VECTOR(dest);
	P_GET_FLOAT_OPT(speed, 1.0);
	P_FINISH;
	
	PawnContext->DesiredSpeed = Max(0.f, Min(1.f, speed));
	PawnContext->LatentAction = AI_PollMoveTo;
	PawnContext->Destination = dest;
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_MoveTo, execMoveTo);

static void execPollMoveTo( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPollMoveTo);
	debugState(Stack.Object->IsA("Pawn"));
	APawn *Pawn = (APawn*)Stack.Object;

	Pawn->rotateToward(Pawn->Destination);
	if (Pawn->moveToward(Pawn->Destination))
		Pawn->LatentAction = 0;

	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_PollMoveTo, execPollMoveTo);

/* execMoveToward()
start moving toward a goal actor -does not use routing
MoveTarget is set to goal
*/
static void execMoveToward( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execMoveToward);
	debugState(Context!=NULL);
	debugState(Context->IsA("Pawn"));
	APawn *PawnContext = (APawn*)Context;

	P_GET_ACTOR(goal);
	P_GET_FLOAT_OPT(speed, 1.0);
	P_FINISH;

	PawnContext->DesiredSpeed = Max(0.f, Min(1.f, speed));
	PawnContext->MoveTarget = goal;
	PawnContext->LatentAction = AI_PollMoveToward;
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_MoveToward, execMoveToward);

static void execPollMoveToward( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPollMoveToward);
	debugState(Stack.Object->IsA("Pawn"));
	APawn *Pawn = (APawn*)Stack.Object;

	Pawn->Destination = Pawn->MoveTarget->Location;
	Pawn->rotateToward(Pawn->MoveTarget->Location);
	if (Pawn->moveToward(Pawn->MoveTarget->Location))
		Pawn->LatentAction = 0;

	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_PollMoveToward, execPollMoveToward);

/* execStrafeTo()
Strafe to Destination, pointing at Focus
*/
static void execStrafeTo( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStrafeTo);
	debugState(Context!=NULL);
	debugState(Context->IsA("Pawn"));
	APawn *PawnContext = (APawn*)Context;

	P_GET_VECTOR(Dest);
	P_GET_VECTOR(FocalPoint);
	P_FINISH;

	PawnContext->DesiredSpeed = 1.0;
	PawnContext->LatentAction = AI_PollStrafeTo;
	PawnContext->Destination = Dest;
	PawnContext->Focus = FocalPoint;

	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_StrafeTo, execStrafeTo);

static void execPollStrafeTo( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPollStrafeTo);
	debugState(Stack.Object->IsA("Pawn"));
	APawn *Pawn = (APawn*)Stack.Object;

	Pawn->rotateToward(Pawn->Focus);
	if (Pawn->moveToward(Pawn->Destination))
		Pawn->LatentAction = 0;

	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_PollStrafeTo, execPollStrafeTo);

/* execStrafeFacing()
strafe to Destination, pointing at MoveTarget
*/
static void execStrafeFacing( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStrafeFacing);
	debugState(Context!=NULL);
	debugState(Context->IsA("Pawn"));
	APawn *PawnContext = (APawn*)Context;

	P_GET_VECTOR(Dest)
	P_GET_ACTOR(goal);
	P_FINISH;

	PawnContext->DesiredSpeed = 1.0;
	PawnContext->Destination = Dest;
	PawnContext->MoveTarget = goal;
	PawnContext->LatentAction = AI_PollStrafeFacing;
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_StrafeFacing, execStrafeFacing);

static void execPollStrafeFacing( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPollStrafeFacing);
	debugState(Stack.Object->IsA("Pawn"));
	APawn *Pawn = (APawn*)Stack.Object;

	Pawn->rotateToward(Pawn->MoveTarget->Location);
	if (Pawn->moveToward(Pawn->Destination))
		Pawn->LatentAction = 0;

	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_PollStrafeFacing, execPollStrafeFacing);

/* execTurnToward()
turn toward MoveTarget
*/
static void execTurnToward( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execTurnToward);
	debugState(Context!=NULL);
	debugState(Context->IsA("Pawn"));
	APawn *PawnContext = (APawn*)Context;

	P_GET_ACTOR(goal);
	P_FINISH;
	
	PawnContext->MoveTarget = goal;
	PawnContext->LatentAction = AI_PollTurnToward;
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_TurnToward, execTurnToward);

static void execPollTurnToward( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPollTurnToward);
	debugState(Stack.Object->IsA("Pawn"));
	APawn *Pawn = (APawn*)Stack.Object;

	if (Pawn->rotateToward(Pawn->MoveTarget->Location))
		Pawn->LatentAction = 0;  

	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_PollTurnToward, execPollTurnToward);

/* execTurnTo()
Turn to focus
*/
static void execTurnTo( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execTurnTo);
	APawn *PawnContext = (APawn*)Context;

	P_GET_VECTOR(FocalPoint);
	P_FINISH;
	
	PawnContext->LatentAction = AI_PollTurnTo;
	PawnContext->Focus = FocalPoint;
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_TurnTo, execTurnTo);

static void execPollTurnTo( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPollTurnTo);
	debugState(Stack.Object->IsA("Pawn"));
	APawn *Pawn = (APawn*)Stack.Object;

	if (Pawn->rotateToward(Pawn->Focus))
		Pawn->LatentAction = 0;

	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_PollTurnTo, execPollTurnTo);

//=================================================================================
/* moveToward()
move Actor toward a point.  Returns 1 if Actor reached point
(Set Acceleration, let physics do actual move)
*/
int APawn::moveToward(const FVector &Dest)
{
	guard(APawn::moveToward);
	FVector Direction = Dest - Location;
	if (Physics == PHYS_Walking) Direction.Z = 0.0; 
	FLOAT DistanceSquared = Direction.SizeSquared();
	int success = 1;
	Acceleration = GMath.ZeroVector;

	//FIXME - return success based on when I know I'll reach the point
	//FIXME - don't get stuck on walls (use normal)
	if (DistanceSquared > 0.1 * Velocity.SizeSquared()) //move not too small to do
	{
		Acceleration = Direction.Normal() * AccelRate;
		success = 0;
	}

	return success;
	unguard;
}

/* rotateToward()
rotate Actor toward a point.  Returns 1 if target rotation achieved.
(Set DesiredRotation, let physics do actual move)
*/
int APawn::rotateToward(const FVector &FocalPoint)
{
	guard(APawn::rotateToward);
	FVector Direction = FocalPoint - Location;

	// Rotate toward destination
	DesiredRotation = Direction.Rotation();
	DesiredRotation.Yaw = DesiredRotation.Yaw & 65535;

	//only base success on Yaw (assume can aim up and down, even if I pitch the pawn), unless
	//gliding - in which case pitch is very important, as is roll (FIXME)
	//FIXME - better judging of having reached correct rotation
	int success = (Abs(DesiredRotation.Yaw - Rotation.Yaw) < 2000);
	return success;
	unguard;
}

DWORD APawn::LineOfSightTo(AActor *Other)
{
	guard(APawn::LineOfSightTo);
	FVector ViewPoint, OtherBody;
	FCheckResult Hit(0.0);
	int result = 0;
	
	ViewPoint = Location;
	ViewPoint.Z += BaseEyeHeight; //look from eyes

	//FIXME - when PVS, only test if in same PVS
	
	//try viewpoint to center
	GetLevel()->Trace(Hit, this, OtherBody, ViewPoint, 1);
	if ( Hit.Actor == Other)
		result = 1;

	//try viewpoint to head
	if (!result)
	{
		OtherBody = Other->Location;
		OtherBody.Z += Other->CollisionHeight - 6;
		GetLevel()->Trace(Hit, this, OtherBody, ViewPoint, 1);
		if ( Hit.Actor == Other)
			result = 1;
	}

	//try viewpoint to feet
	if (!result)
	{
		OtherBody.Z = OtherBody.Z - Other->CollisionHeight - Other->CollisionHeight + 12;
		GetLevel()->Trace(Hit, this, OtherBody, ViewPoint, 1);
		if ( Hit.Actor == Other)
			result = 1;
	}

	//FIXME - try checking sides?
	return result;
	unguard;
}

int AActor::CanBeHeardBy(AActor *Other)
{
	guard(AActor::CanBeHeardBy);
	int result = 0;
	FCheckResult Hit(1.0);

	GetLevel()->Trace(Hit, this, Other->Location, Location, 0);
	if (Hit.Actor == Other)
		result = 1;
	return result;
	unguard;
}

/* Send a HearNoise() message to all Pawns which could possibly hear this noise
*/
void AActor::CheckNoiseHearing(FLOAT Loudness)
{
	guard(AActor::CheckNoiseHearing);

	for (INDEX i=0; i<GetLevel()->Num; i++) 
	{
		AActor *Actor = GetLevel()->Element(i);
		if (Actor)
		if (Actor->IsA("Pawn")) 
		{
			if (CanBeHeardBy(Actor)) 
				Actor->Process(NAME_HearNoise, &PNoise(Loudness, this) );
		}
	}
	unguard;
}

void APawn::CheckEnemyVisible()
{
	guard(APawn::CheckEnemyVisible);
	if (Enemy)
	{
		if (!LineOfSightTo(Enemy))
			Process(NAME_EnemyNotVisible, NULL );
		else 
			LastSeenPos = Enemy->Location;
	}

	unguard;
}

/* Player shows self to pawns that are ready
*/
void APawn::ShowSelf()
{
	guard(APawn::LookForPlayer);

	for (INDEX i=0; i<GetLevel()->Num; i++) 
	{
		AActor *Actor = GetLevel()->Element(i);
		if (Actor)
		{
			APawn *Pawn = Actor->IsA("Pawn") ? (APawn*)Actor : NULL;
			if (Pawn)
				if ((Pawn->SightCounter < 0.0) && Pawn->IsProbing(NAME_SeePlayer)) 
					if (Pawn->LineOfSightTo(this))
						Pawn->Process(NAME_SeePlayer, &PActor(this) );
		}
	}
	unguard;
}

//FloorZ()
//returns Z height of the actor if placed on the floor below a Point
//(returns CollisionHeight + 1 below actor location, if floor below that)
//(for a given actor, with a given CollisionRadius and CollisionHeight)
inline FLOAT AActor::FloorZ(FVector Point)
{
	//FIXME - this is very broken!!!!
	guard(AActor::FloorZ);
	FLOAT Floor;
	FVector Down = GMath.ZeroVector;
	Down.Z = -1 * (CollisionHeight + 1);
	FVector currentLocation = Location;
	FCheckResult Hit(1.0);
	GetLevel()->MoveActor(this, &Down, Hit);
	Floor = Location.Z + 2.0; //2.0 = slight float
	Down.Z = currentLocation.Z - Location.Z;
	GetLevel()->MoveActor(this, &Down, Hit);

	return Floor; 

	unguard;
}

int APawn::actorReachable(AActor *Other)
{
	guard(AActor::actorReachable);

	//check other visible
	int clearpath = LineOfSightTo(Other); 

	//FIXME? go back to fly reachable for all?	
	//Fixme - adjust destination using a FarMoveActor

	if (clearpath) 
		clearpath = Reachable(Other->Location);

	return clearpath;
	unguard;
}

int APawn::pointReachable(FVector &aPoint)
{
	guard(AActor::pointReachable);

	FCheckResult Hit(0.0);
	//check aPoint visible
	FVector	ViewPoint = Location;
	ViewPoint.Z += BaseEyeHeight; //look from eyes

	int clearpath = GetLevel()->Model->LineCheck(Hit,NULL,ViewPoint, aPoint,0); //FIXME - should be trace

	if (clearpath) 
		clearpath = Reachable(aPoint);

	return clearpath;
	unguard;
}

int APawn::Reachable(FVector &aPoint)
{
	guard(AActor::Reachable);
	//FIXME? go back to fly reachable for all?	
	//Fixme - adjust destination using a FarMoveActor

	int clearpath = 0;

	if (Physics == PHYS_Walking)
		clearpath = walkReachable(aPoint);
	else if (Physics == PHYS_Flying)
		clearpath = flyReachable(aPoint);
	else if (Physics == PHYS_Falling)
		clearpath = jumpReachable(aPoint);

	return clearpath;
	unguard;
}

int APawn::flyReachable(FVector &Dest)
{
	guard(APawn::flyReachable);
	
	//FIXME assess zones - if succeed, check points along the way
	//FIXME - only try maxstepheight up?  How will this work for pathbuilding?
	FVector path = Dest - Location;
	path = path - Min((CollisionRadius * 0.25f), path.Size()) * path.Normal();	
	if (path.IsNearlyZero())
		return 1;

	FVector start = Location;
	FVector end = start + path;
	start.Z += MaxStepHeight;
	end.Z += MaxStepHeight;

	int clearpath = GetLevel()->TestMoveActor(this, start, end);
	if (!clearpath)
	{
		start.Z -= MaxStepHeight;
		end.Z -= MaxStepHeight;
		clearpath = GetLevel()->TestMoveActor(this, start, end);
	}

	return clearpath;
	unguard;
}
/*walkReachable() -
//walkReachable returns 0 if iActor cannot reach dest, and 1 if it can reach dest by moving in
// straight line
FIXME - take into account zones (lava, water, etc.). - note that pathbuilder should
initialize Scout to be able to do all these things
// actor must remain on ground at all times
// Note that Actor is not moved (when all is said and done)
// FIXME - allow jumping up and down if bCanJump (false for Scout!)

*/
int APawn::walkReachable(FVector &Dest)
{
	guard(APawn::walkReachable);

	int success = 1;
	//FIXME - precheck destination Z-height?
	if (success)
	{
		FVector OriginalPos = Location;
		FVector realVel = Velocity;
		//FIXME - also save  oldlocation(?), acceleration(?)

		int stillmoving = 1;
		success = 0;
		FLOAT closeSquared = CollisionRadius * CollisionRadius * 0.25 * 0.25; //should it be less?
		FLOAT Movesize = GroundSpeed * 0.05;

		while (stillmoving == 1) 
		{
			FVector Direction = Dest - Location;
			FLOAT Zdiff = Direction.Z;
			Direction.Z = 0; //this is a 2D move
			FLOAT DistanceSquared = Direction.SizeSquared();
			if (DistanceSquared > closeSquared) //move not too small to do
			{
				if (DistanceSquared < Movesize * Movesize)
					stillmoving = walkMove(Direction);
				else
					stillmoving = walkMove(Direction.Normal() * Movesize);
				 
				if (JumpZ > 0.0) //FIXME - add a bCanJump
				{
					if (stillmoving == -1) 
					{
						FVector Landing;
						stillmoving = FindBestJump(Dest, Velocity, Landing, 1);
					}
					else if (stillmoving == 0)
					{
						FVector Landing;
						stillmoving = FindJumpUp(Dest, Velocity, Landing, 1);
					}
				}
			}
			else
			{
				stillmoving = 0;
				if (abs(Zdiff) < CollisionHeight) 
					success = 1;
			}
		}
		GetLevel()->FarMoveActor(this, &OriginalPos, 1); //move actor back to starting point
		Velocity = realVel;
	}
	
	return success;
	unguard;
}

int APawn::jumpReachable(FVector &Dest)
{
	guard(APawn::jumpReachable);

	int success = 1;
	//FIXME - precheck destination Z-height?
	if (success)
	{
		FVector OriginalPos = Location;
		//FIXME - also save  oldlocation(?), acceleration(?)

		FVector Landing;
		jumpLanding(Velocity, Landing, 1); 
		success = walkReachable(Dest);
		GetLevel()->FarMoveActor(this, &OriginalPos, 1); //move actor back to starting point
	}
	
	return success;
	unguard;
}

/* jumpLanding()
determine landing position of current fall, given testVel as initial velocity.
Assumes near-zero acceleration by pawn during jump (make sure creatures do this FIXME)
*/
void APawn::jumpLanding(FVector testVel, FVector &Landing, int moveActor)
{
	guard(APawn::jumpLanding);


	FVector OriginalPos = Location;
	int landed = 0;
	while (!landed)
	{
		testVel = testVel * (1 - Zone->ZoneFluidFriction * 0.05) 
					+ Zone->ZoneGravity * 0.05; 
		FVector Adjusted = (testVel + Zone->ZoneVelocity) * 0.05;
		FCheckResult Hit(1.0);
		GetLevel()->MoveActor(this, &Adjusted, Hit, 1);
		if (Hit.Time < 1.0)
		{
			if (Hit.Normal.Z > 0.7)
				landed = 1;
			else
			{
				FVector OldHitNormal = Hit.Normal;
				FVector Delta = (Adjusted - Hit.Normal * (Adjusted | Hit.Normal)) * (1.0 - Hit.Time);
				if( (Delta | Adjusted) >= 0 )
				{
					GetLevel()->MoveActor(this, &Delta, Hit, 1);
					if (Hit.Time < 1.0) //hit second wall
					{
						if (Hit.Normal.Z > 0.7)
							landed = 1;	
						FVector DesiredDir = Adjusted.Normal();
						TwoWallAdjust(DesiredDir, Delta, Hit.Normal, OldHitNormal, Hit.Time);
						GetLevel()->MoveActor(this, &Delta, Hit, 1);
						if (Hit.Normal.Z > 0.7)
							landed = 1;
					}
				}
			}
		}
	}

	Landing = Location;
	if (!moveActor)
		GetLevel()->FarMoveActor(this, &OriginalPos, 1); //move actor back to starting point

	unguard;
}

int APawn::FindJumpUp(FVector &Dest, FVector &vel, FVector &Landing, int moveActor)
{
	guard(APawn::FindJumpUp);

	//FIXME - try to jump up
	return 0;

	unguard;
}

/* Find best jump from current position toward destination.  Assumes that there is no immediate 
barrier.  Sets vel to the suggested initial velocity, Landing to the expected Landing, 
and moves actor if moveActor is set */

int APawn::FindBestJump(FVector &Dest, FVector &vel, FVector &Landing, int moveActor)
{
	guard(APawn::FindBestJump);

	FVector realLocation = Location;

	//FIXME - walk along ledge as far as possible to get closer to destination
	//determine how long I might be in the air (assume full jumpz velocity to start)
	//FIXME - make sure creatures set no acceleration (normal of direction) while jumping down
	vel.Z = JumpZ;
	FLOAT timeToFloor = 0.0;
	FLOAT floor = Dest.Z - Location.Z;
	FLOAT currentZ = 0.0;
	FLOAT gravZ = Zone->ZoneGravity.Z;
	FLOAT ticks = 0.0;
	while ((currentZ > floor) || (vel.Z > 0.0))
	{
		vel.Z = vel.Z + gravZ * 0.05;
		ticks += 0.05; 
		currentZ = currentZ + ticks * vel.Z * 0.05;
	}

	if (Abs(vel.Z) > 1.0) 
		ticks = ticks - 0.05 * (currentZ - floor)/(0.05 * vel.Z); //correct overshoot

	int success = 0;
	if (ticks > 0.0)
	{
		vel = (Dest - Location) / ticks;
		vel.Z = 0;
		float velsize = Min(1.f * GroundSpeed, vel.Size()); //FIXME - longwinded because of compiler bug
		vel = vel.Normal();
		vel *= velsize;
		vel.Z = JumpZ;
		
		// Now imagine jump
		//debugf("Jump from (%f, %f, %f)", Location.X, Location.Y, Location.Z);
		jumpLanding(vel, Landing, 1);
		//debugf("Landed at (%f, %f, %f)", Location.X, Location.Y, Location.Z);
		FVector olddist = Dest - realLocation;
		FVector dist = Dest - Location;
		success = (dist.Size() < olddist.Size());
		// FIXME - if failed, imagine with no jumpZ (step out first)
		if (!moveActor)
			GetLevel()->FarMoveActor(this, &realLocation, 1); //move actor back to starting point
	}
	return success;

	unguard;
}

/* walkMove() 
- returns 1 if move happened, zero if it didn't because of barrier, and -1
if it didn't because of ledge
Move direction must not be adjusted.
*/
int APawn::walkMove(FVector Delta)
{
	guard(APawn::walkMove);
	int result = 1;
	FVector StartLocation = Location;
	Velocity = 20 * Delta;// + (-1 * Actor->Zone->ZoneVelocity)); 	// counteract zonevelocity when setting velocity
	Velocity = Velocity.Normal() * Min(GroundSpeed, Velocity.Size());
	//acceleration shouldn't matter

	testWalking();
	if (Physics == PHYS_Falling) //fell off ledge
	{
		GetLevel()->FarMoveActor(this, &StartLocation, 1);
		result = -1;
		Physics = PHYS_Walking;
	}
	else //check if move successful
	{
		FVector RealMove = Location - StartLocation;
		if (RealMove.Size() < 4.0) //FIXME - bigger/smaller threshold?
		{
			//if (RealMove.Size() > 1.0)
			//	DebugPrint("Sorta real move not taken");
			GetLevel()->FarMoveActor(this, &StartLocation, 1);
			result = 0;
		}
	}
	return result;
	unguard;
}

/* clearPaths()
clear all temporary path variables used in routing
*/

void APawn::clearPaths()
{
	guard(APawn::clearPaths);

	ULevel *Level = GetLevel();
	debugf("clear paths");
	for (INDEX i=0; i<Level->Num; i++)
	{
		AActor *Actor = Level->Element(i); 
		if (Actor && Actor->IsA("PathNode"))
		{
			APathNode *node = (APathNode *)Actor;
			node->visitedWeight = 10000000.0;
			node->bEndPoint = 0;
			node->bSpecialBlock = 0;
			node->cost = 0.0;
		}
	}

	unguard;
}

int APawn::findPathToward(AActor *goal, FLOAT maxweight, AActor *&bestPath)
{
	guard(APawn::findPathToward);

	FVector Dest = goal->Location;
	if (Physics != PHYS_Flying)
		if (goal->IsA("Pawn"))
		{
			APawn *goalpawn = (APawn *)goal;
			if (goalpawn->Physics == PHYS_Falling)
				jumpLanding(goalpawn->Velocity, Dest);
		//	else if (goalpawn->Physics == PHYS_Flying)
		//		Dest.Z = GetLevel()->FloorZ(Dest);
		}
		
	return findPathTo(Dest, maxweight, bestPath);
	unguard;
}

int APawn::findPathTo(FVector Dest, FLOAT maxweight, AActor *&bestPath)
{
	guard(APawn::findPathTo);
	bestPath = NULL;
	int success = 0;
	int result = 0;
	ULevel *Level = GetLevel();

	//first mark all paths reachable by this pawn
	for (INDEX i=0; i<Level->Num; i++)
	{
		AActor *Actor = Level->Element(i); 
		if (Actor && Actor->IsA("PathNode"))
			if (pointReachable(Actor->Location))
			{
				((APathNode *)Actor)->bEndPoint = 1;
				success = 1;
			}
	}

	//adjust destination using farmoveactor
	FVector RealLocation = Location;
	success = Level->FarMoveActor(this, &Dest, 1);
	Dest = Location;
	
	//now explore paths from destination
	if (success)
	{
		AActor *newPath = NULL;
		FCheckResult Result;
		FLOAT bestweight = maxweight;
		FLOAT currentweight;
		FLOAT newbest;
		FVector dir;

		for (INDEX i=0; i<Level->Num; i++)
		{
			AActor *Actor = Level->Element(i); 
			if (Actor && Actor->IsA("PathNode"))
			{
				dir = Dest - Actor->Location;
				currentweight = dir.Size();
				dir = Actor->Location - RealLocation;
				if ((currentweight + dir.Size()) < bestweight)
					if (Level->Model->LineCheck(Result,Owner,Actor->Location,Dest,0))
						if (Level->FarMoveActor(this, &Actor->Location, 1))
							if (pointReachable(Dest))
							{
								newbest = bestweight;
								if (bestPathFrom(Actor, newbest, currentweight, newPath))
								{
									dir = RealLocation - newPath->Location;
									newbest += dir.Size();
									if (newbest < bestweight)
									{
										bestweight = newbest;
										bestPath = newPath;
										result = 1;
									}
								}
							}
			}
		}
	}
	Level->FarMoveActor(this, &RealLocation, 1);

	return result;
	unguard;
}

/* bestPathFrom()
parameters - bestweight is the current best weight, and currentweight is the current weight along this route
(currentweight should be less than best weight, or don't call)
(best weight may be updated)
startnode is the starting path
if a better path is found, it is returned
*/
int APawn::bestPathFrom(AActor *start, float &bestweight, float currentweight, AActor *&bestPath)
{
	guard(APawn::bestPathFrom);
	int result = 0;
	APathNode *startnode = (APathNode *)start;
	if (startnode->visitedWeight <= currentweight) //already came through here, faster
	{
		bestPath = NULL;
		result = 0;
	}
	else
	{
		startnode->visitedWeight = currentweight;
		debugf("Weight for node at (%f, %f, %f) is %f",
			startnode->Location.X,startnode->Location.Y,startnode->Location.Z,currentweight);
		if (startnode->bEndPoint) //success
		{
			debugf("Success!");
			bestweight = currentweight;
			bestPath = startnode;
			result = 1;
		}
		else
		{
			ULevel *Level = GetLevel();
			AActor *nextPath;
			AActor *newPath = NULL;
			FReachSpec *spec;
			int i = 0;
			while (i<16)
			{
				if (startnode->upstreamPaths[i] == -1)
					i = 16;
				else
				{
					spec = &Level->ReachSpecs->Element(startnode->upstreamPaths[i]);
					if (spec->supports(this))
					{ 
						nextPath = spec->Start;
						FLOAT newweight = currentweight + spec->distance + ((APathNode *)nextPath)->cost;
						if (newweight < bestweight)
							if (bestPathFrom(nextPath, bestweight, newweight, newPath))
							{ 
								result = 1;
								bestPath = newPath;
							}
					}
					i++;
				}
			}
		}
	}

	return result;
	unguard;
}