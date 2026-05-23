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
	guard(inputCopyFrom);

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
	aTurn		= Tick.Axis[AXIS_Turn   ];
	aStrafe		= Tick.Axis[AXIS_Strafe ];
	aUp			= Tick.Axis[AXIS_Up     ];
	aLookUp		= Tick.Axis[AXIS_LookUp ];
	aExtra4		= Tick.Axis[AXIS_Extra4 ];
	aExtra3		= Tick.Axis[AXIS_Extra3 ];
	aExtra2		= Tick.Axis[AXIS_Extra2 ];
	aExtra1		= Tick.Axis[AXIS_Extra1 ];
	aExtra0		= Tick.Axis[AXIS_Extra0 ];

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
	AI_JumpLanding = 523,
	AI_FindStairRotation = 524, //FIXME - implement
	AI_FindRandomDest = 525
};

static void execFindStairRotation( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFindStairRotation);
	debugState(Context!=NULL);
	APawn *PawnContext = (APawn *)Context;

	P_GET_VECTOR(rotdir); //usually, rotdir = vector(ViewRotation)
	P_FINISH;

	/*
	//FIXME - Do this right
	// find start of next step, and use that as start
	
	FRotation stairRot;
	rotdir.Z = 0;
	rotdir.Normalize();
	FVector start = PawnContext->Location;
	FVector end = rotdir * 2 * PawnContext->CollisionRadius + start;
	if (PawnContext->GetLevel()->testMoveActor(PawnContext,start,end))
	{
		//using subdivision, figure out angle of steps
		
	}
	else
		stairRot = PawnContext->ViewRotation;

		rotdir = PawnContext->Location - realLocation;

		PawnContext->Velocity = rotdir * 16.0 - PawnContext->Zone->ZoneVelocity;
		PawnContext->GetLevel()->testWalking();
		if (Physics == PHYS_Falling)
		{
			rotdir.Normalize();
			stairRot = rotdir.Rotation();
		}
		else
		{
			rotdir = rotdir + PawnContext->Location - realLocation;
			rotdir.Normalize();
			stairRot = rotdir.Rotation();
		}
	}

	PawnContext->GetLevel()->FarMoveActor(PawnContext, &realLocation, 1);
	PawnContext->Velocity = realVelocity;
	*(FRotation*)Result = stairRot; */
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_FindStairRotation, execFindStairRotation);

static void execJumpLanding( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execJumpLanding);
	debugState(Context!=NULL);
	APawn *PawnContext = (APawn *)Context;
	FVector Landing;
	FVector vel = PawnContext->Velocity;
	PawnContext->jumpLanding(vel, Landing, 0);

	P_FINISH;

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
	P_GET_INT_OPT(maxpaths, 50); 
	P_GET_FLOAT_OPT(maxweight, 10000.0);
	P_GET_BOOL_OPT(bClearPaths, 1);
	P_FINISH;

	if (bClearPaths)
		PawnContext->clearPaths();
	ACreaturePoint * bestPath = NULL;
	AActor * newPath;
	if (PawnContext->findPathTo(point, maxpaths, maxweight, newPath))
		bestPath = (ACreaturePoint *)newPath;

	*(ACreaturePoint**)Result = bestPath; 
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_FindPathTo, execFindPathTo);

static void execFindPathToward( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFindPathToward);
	debugState(Context!=NULL);
	APawn *PawnContext = (APawn *)Context;

	P_GET_ACTOR(goal);
	P_GET_INT_OPT(maxpaths, 50);
	P_GET_FLOAT_OPT(maxweight, 10000.0);
	P_GET_BOOL_OPT(bClearPaths, 1);
	P_FINISH;

	if (bClearPaths)
		PawnContext->clearPaths();
	ACreaturePoint * bestPath = NULL;
	AActor * newPath;
	if (PawnContext->findPathToward(goal, maxpaths, maxweight, newPath))
		bestPath = (ACreaturePoint *)newPath;

	*(ACreaturePoint**)Result = bestPath; 
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_FindPathToward, execFindPathToward);

/* FindRandomDest()
returns a random pathnode which is reachable from the creature's location
*/
static void execFindRandomDest( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFindPathTo);
	debugState(Context!=NULL);
	APawn *PawnContext = (APawn *)Context;

	P_GET_INT_OPT(maxpaths, 100);
	P_GET_FLOAT_OPT(maxweight, 10000.0);
	P_GET_BOOL_OPT(bClearPaths, 1);
	P_FINISH;

	if (bClearPaths)
		PawnContext->clearPaths();
	ACreaturePoint * bestPath = NULL;
	AActor * newPath;
	if (PawnContext->findRandomDest(maxpaths, maxweight, newPath))
	{
		//debugf("Successfully found random destination");
		bestPath = (ACreaturePoint *)newPath;
	}

	*(ACreaturePoint**)Result = bestPath; 
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_FindRandomDest, execFindRandomDest);

static void execClearPaths( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFindPathTo);
	debugState(Context!=NULL);
	APawn *PawnContext = (APawn *)Context;

	P_FINISH;

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
	
	//debugf("Make Noise");
	ActorContext->CheckNoiseHearing(Loudness);
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_MakeNoise, execMakeNoise);

//FIXME - fix floorz or remove
static void execFloorZ( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloorZ);

	P_GET_VECTOR(point);
	P_FINISH;
	
	*(FLOAT*)Result = point.Z;//((AActor *)Context)->FloorZ(point);
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
//FIXME - don't use ground speed for flyers (or set theirs = flyspeed)
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

	FVector Move = dest - PawnContext->Location;
	PawnContext->MoveTarget = NULL;
	PawnContext->bReducedSpeed = 0;
	PawnContext->DesiredSpeed = Max(0.f, Min(1.f, speed));
	PawnContext->setMoveTimer(Move.Size()); 
	PawnContext->LatentAction = AI_PollMoveTo;
	PawnContext->Destination = dest;
	PawnContext->Focus = dest;
	unguardSlow;
}
AUTOREGISTER_INTRINSIC( AI_MoveTo, execMoveTo);

static void execPollMoveTo( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPollMoveTo);
	debugState(Stack.Object->IsA("Pawn"));
	APawn *Pawn = (APawn*)Stack.Object;

	Pawn->rotateToward(Pawn->Focus);
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

	if (!goal)
	{
		debugf("MoveToward with no goal");
		return;
	}

	FVector Move = goal->Location - PawnContext->Location;	
	PawnContext->bReducedSpeed = 0;
	PawnContext->DesiredSpeed = Max(0.f, Min(1.f, speed));
	if (goal->IsA("Pawn"))
		PawnContext->MoveTimer = 2.0; //max before re-assess movetoward
	else
		PawnContext->setMoveTimer(Move.Size()); 
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

	if (!Pawn->MoveTarget)
	{
		debugf("MoveTarget cleared during movetoward");
		Pawn->LatentAction = 0;
		return;
	}

	Pawn->Destination = Pawn->MoveTarget->Location;
	Pawn->Focus = Pawn->Destination;
	Pawn->rotateToward(Pawn->Focus);
	FLOAT oldDesiredSpeed = Pawn->DesiredSpeed;
	if (Pawn->moveToward(Pawn->Destination))
		Pawn->LatentAction = 0;
	Pawn->DesiredSpeed = oldDesiredSpeed; //don't slow down when moving toward an actor
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

	FVector Move = Dest - PawnContext->Location;
	PawnContext->MoveTarget = NULL;
	PawnContext->bReducedSpeed = 0;
	PawnContext->DesiredSpeed = 1.0;
	PawnContext->setMoveTimer(Move.Size()); 
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

	if (!goal)
	{
		debugf("StrafeFacing without goal");
		return;
	}
	FVector Move = Dest - PawnContext->Location;	
	PawnContext->bReducedSpeed = 0;
	PawnContext->DesiredSpeed = 1.0;
	PawnContext->setMoveTimer(Move.Size()); 
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

	if (!Pawn->MoveTarget)
	{
		debugf("MoveTarget cleared during strafefacing");
		Pawn->LatentAction = 0;
		return;
	}

	Pawn->Focus = Pawn->MoveTarget->Location;
	Pawn->rotateToward(Pawn->Focus);
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
	
	if (!goal)
		return;

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

	if (!Pawn->MoveTarget)
	{
		debugf("MoveTarget cleared during turntoward");
		Pawn->LatentAction = 0;
		return;
	}

	Pawn->Focus = Pawn->MoveTarget->Location;
	if (Pawn->rotateToward(Pawn->Focus))
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

	PawnContext->MoveTarget = NULL;
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
void APawn::setMoveTimer(FLOAT MoveSize)
{
	guard(APawn::setMoveTimer);

	FLOAT MaxSpeed = 100.0; //safety in case called with Physics = PHYS_None (shouldn't)

	if (Physics == PHYS_Walking)
		MaxSpeed = GroundSpeed;
	else if (Physics == PHYS_Falling)
		MaxSpeed = GroundSpeed;
	else if (Physics == PHYS_Flying)
		MaxSpeed = AirSpeed;
	else if (Physics == PHYS_Swimming)
		MaxSpeed = WaterSpeed;

	MoveTimer = 1.0 + 1.3 * MoveSize/(DesiredSpeed * MaxSpeed); 

	unguard;
}

/* moveToward()
move Actor toward a point.  Returns 1 if Actor reached point
(Set Acceleration, let physics do actual move)
*/
int APawn::moveToward(const FVector &Dest)
{
	guard(APawn::moveToward);
	FVector Direction = Dest - Location;
	if (Physics == PHYS_Walking) Direction.Z = 0.0; 
	FLOAT Distance = Direction.Size();
	Direction = Direction.Normal();
	int success = 0;
	FLOAT speed = Velocity.Size(); 
	FVector Veldir = Velocity.Normal();
	Acceleration = Direction * AccelRate;
	Acceleration -= 0.3 * (1 - (Direction | Veldir)) * Velocity.Size() * (Veldir - Direction); 

	if (MoveTarget && MoveTarget->IsA("Pawn"))
	{
		if (Distance < CollisionRadius + MoveTarget->CollisionRadius + 0.5 * MeleeRange)
			success = 1;
	}
	else if (Distance < Max((0.15 * speed), 30.0))
	{
		if (!bReducedSpeed) //haven't reduced speed yet
		{
			DesiredSpeed = 0.5 * DesiredSpeed;
			bReducedSpeed = 1;
		}
		DesiredSpeed = Min(DesiredSpeed, 200.f/speed);
		if (Distance < 15.0)
			success = 1;
	}

	if (MoveTimer < 0.0)
		success = 1; //give up

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
	//flying - in which case pitch is very important, as is roll (FIXME)
	int success = (Abs(DesiredRotation.Yaw - (Rotation.Yaw & 65535)) < 2000);
	if (!success) //check if on opposite sides of zero
		success = (Abs(DesiredRotation.Yaw - (Rotation.Yaw & 65535)) > 63535);	

	return success;
	unguard;
}

DWORD APawn::LineOfSightTo(AActor *Other)
{
	guard(APawn::LineOfSightTo);
	FVector ViewPoint, OtherBody;

	if (!Other)
		return 0;

	FCheckResult Hit(1.0);
	int result = 0;
	
	ViewPoint = Location;
	ViewPoint.Z += EyeHeight; //look from eyes

	//FIXME - when PVS, only do this test if in same PVS
	
	GetLevel()->Trace(Hit, this, Other->Location, ViewPoint, TRACE_Level); //VisBlocking);
	if (Hit.Time == 1.0)
		result = 1;

	//try viewpoint to head
	if (!result)
	{
		Hit = FCheckResult(1.0);
		OtherBody = Other->Location;
		OtherBody.Z += Other->CollisionHeight * 0.8;
		GetLevel()->Trace(Hit, this, OtherBody, ViewPoint, TRACE_Level); //VisBlocking); 
		if ( Hit.Time == 1.0)
			result = 1;
	}

	//try viewpoint to feet
	if (!result)
	{
		Hit = FCheckResult(1.0);
		OtherBody = Other->Location;
		OtherBody.Z = OtherBody.Z - Other->CollisionHeight * 0.8;
		GetLevel()->Trace(Hit, this, OtherBody, ViewPoint, TRACE_Level); //VisBlocking); 
		if ( Hit.Time == 1.0)
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

	GetLevel()->Trace(Hit, this, Other->Location, Location, TRACE_Level); //VisBlocking);
	if (Hit.Time == 1.0) 
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
		if (Actor && Actor->IsA("Pawn") && (Actor != this) && Actor->IsProbing(NAME_HearNoise)) 
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
			if (Pawn && (Pawn != this))
				if ((Pawn->SightCounter < 0.0) && Pawn->IsProbing(NAME_SeePlayer)) 
				{
					if (bIsPlayer)
					{
						if (Pawn->LineOfSightTo(this))
							Pawn->Process(NAME_SeePlayer, &PActor(this));
					}
					else if (Pawn->GetClass() != GetClass()) //FIXME || (Pawn->Team != Team))
					{
						if (Pawn->IsProbing(NAME_SeeMonster))
						{
							FVector LoS = Pawn->Location - Location;
							LoS.Normalize();
							if ((LoS | Rotation.Vector()) > 0.5)
								if (Pawn->LineOfSightTo(this))
									Pawn->Process(NAME_SeeMonster, &PActor(this));
						}
					}
		/*			else if (Pawn->IsProbing(NAME_SeeFriend)) FIXME - use teamleader to coordinate instead?
					{
						FVector LoS = Pawn->Location - Location;
						LoS.Normalize();
						if ((LoS | Rotation.Vector()) > 0.5)
							if (Pawn->LineOfSightTo(this))
								Pawn->Process(NAME_SeeFriend, &PActor(this));
					} */
				}
		}
	}
	unguard;
}

//FloorZ()
//returns Z height of the actor if placed on the floor below a Point
//(returns CollisionHeight + 1 below actor location, if floor below that)
//(for a given actor, with a given CollisionRadius and CollisionHeight)
//NOT USED NOW - FIX UP OR REMOVE
inline FLOAT AActor::FloorZ(FVector Point)
{
	guard(AActor::FloorZ);
	FLOAT FloorZ = Location.Z - 1000.0;
	FVector Down = FVector(0,0, -1 * (CollisionHeight + 1));
	FVector currentLocation = Location;
	FVector dest = Point;
	FCheckResult Hit(1.0);
	if (GetLevel()->FarMoveActor(this, dest, 1)) 
	{
		GetLevel()->MoveActor(this, Down, Rotation, Hit, 1);
		FloorZ = Location.Z;
		GetLevel()->FarMoveActor(this, currentLocation, 1, 1);
	}

	return FloorZ; 

	unguard;
}

int APawn::actorReachable(AActor *Other)
{
	guard(AActor::actorReachable);

	if (!Other)
		return 0;

	if (!Other->IsA("Pawn")) //only look past 800 for pawns
		if (GetLevel()->GetState() == LEVEL_UpPlay)
		{
			FVector Dir2D = Other->Location - Location;
			Dir2D.Z = 0.0;
			if (Dir2D.Size() > 800.0) //non-pawns must be within 800.0
				return 0;
		}

	//check other visible
	int clearpath = LineOfSightTo(Other); 

	if (clearpath)
		clearpath = Reachable(Other->Location);

	return clearpath;
	unguard;
}

int APawn::pointReachable(FVector aPoint)
{
	guard(AActor::pointReachable);

	if (GetLevel()->GetState() == LEVEL_UpPlay)
	{
		FVector Dir2D = aPoint - Location;
		Dir2D.Z = 0.0;
		if (Dir2D.Size() > 800.0) //points must be within 800.0
			return 0;
	}

	FCheckResult Hit(1.0);
	//check aPoint visible
	FVector	ViewPoint = Location;
	ViewPoint.Z += BaseEyeHeight; //look from eyes

	GetLevel()->Trace(Hit, this, aPoint, ViewPoint, TRACE_Level); //VisBlocking); 
	int clearpath = (Hit.Time == 1.0);

	if (clearpath) 
		clearpath = Reachable(aPoint);

	return clearpath;
	unguard;
}

int APawn::Reachable(FVector aPoint)
{
	guard(AActor::Reachable);

	FVector realLoc = Location;
	int	clearpath = GetLevel()->FarMoveActor(this, aPoint, 1);
	if (!clearpath)
		return 0;

	aPoint = Location; //adjust destination
	GetLevel()->FarMoveActor(this, realLoc,1,1);

	if (Physics == PHYS_Walking)
		clearpath = walkReachable(aPoint);
	else if (Physics == PHYS_Flying)
		clearpath = flyReachable(aPoint);
	else if (Physics == PHYS_Falling)
		clearpath = jumpReachable(aPoint);

	return clearpath;
	unguard;
}

int APawn::flyReachable(FVector Dest)
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

	int clearpath = GetLevel()->TestMoveActor(this, start, end, 1);

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
int APawn::walkReachable(FVector Dest)
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
		FLOAT closeSquared = 225.0; //should it be less for path building? its 15 * 15
		FLOAT MaxZDiff = CollisionHeight; //FIXME + MaxStepHeight? - YES, do it (after E3)
		FLOAT Movesize = 16.0; 
		FVector Direction = Dest - Location;
		Direction.Z = 0.0; //get 2D size
		FLOAT Dist2D = Direction.Size();
		if (GetLevel()->GetState() == LEVEL_UpPlay)
		{
			MaxZDiff = CollisionHeight + MaxStepHeight;
			FLOAT bigThresh = Dist2D - 800.0;
			if (bigThresh > 0.0)
			{
				FLOAT bigThreshSq = bigThresh * bigThresh;
				if (bigThreshSq > closeSquared)
				{
					MaxZDiff += bigThresh; //FIXME? * MaxStepHeight * 1/16; 
					closeSquared = bigThreshSq;
				}
			}
			if (JumpZ > 0)
				Movesize = Max(128.f, CollisionRadius);
			else
				Movesize = CollisionRadius;
		}

		float ticks = Dist2D * 0.125; //increase to 0.125 or 0.2

		while (stillmoving == 1) 
		{
			Direction = Dest - Location;

			FLOAT Zdiff = Direction.Z;
			Direction.Z = 0; //this is a 2D move
			FLOAT testZ = Zdiff - CollisionHeight;
			FLOAT DistanceSquared = Direction.SizeSquared(); //2D size
			FLOAT MoveSizeSquared = Movesize * Movesize;
			if ((testZ > 0) && (DistanceSquared < 0.8 * testZ * testZ))
				stillmoving = 0; //too steep to get there
			else
			{
				if (DistanceSquared > closeSquared) //move not too small to do
				{
					if (DistanceSquared < MoveSizeSquared) 
						stillmoving = walkMove(Direction, 8.0, 0);
					else
						stillmoving = walkMove(Direction.Normal() * Movesize, 4.1, 0);
					 
					if ((stillmoving != 1) && (JumpZ > 0.0)) //FIXME - add a bCanJump
					{
						if (stillmoving == -1) 
						{
							FVector Landing;
							stillmoving = FindBestJump(Dest, GroundSpeed * Direction.Normal(), Landing, 1);
						}
						else if (stillmoving == 0)
						{
							FVector Landing;
							stillmoving = FindJumpUp(Dest, GroundSpeed * Direction.Normal(), Landing, 1);
						}
					}
				}
				else
				{
					stillmoving = 0;
					if (abs(Zdiff) < MaxZDiff) 
						success = 1;
				}
				ticks = ticks - 1.0;
				if (ticks < 0)
					stillmoving = 0;
			}
		}
		GetLevel()->FarMoveActor(this, OriginalPos, 1, 1); //move actor back to starting point
		Velocity = realVel;
	}
	
	

	return success;
	unguard;
}

int APawn::jumpReachable(FVector Dest)
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
		GetLevel()->FarMoveActor(this, OriginalPos, 1, 1); //move actor back to starting point
	}
	
	return success;
	unguard;
}

/* jumpLanding()
determine landing position of current fall, given testVel as initial velocity.
Assumes near-zero acceleration by pawn during jump (make sure creatures do this FIXME)
*/
void APawn::jumpLanding(FVector testVel, FVector &Landing, int movePawn)
{
	guard(APawn::jumpLanding);


	FVector OriginalPos = Location;
	int landed = 0;
	int ticks = 0;
	FLOAT tickTime = 0.05;
	if (GetLevel()->GetState() == LEVEL_UpPlay)
		tickTime = 0.1;

	while (!landed)
	{
		testVel = testVel * (1 - Zone->ZoneFluidFriction * tickTime) 
					+ Zone->ZoneGravity * tickTime; 
		FVector Adjusted = (testVel + Zone->ZoneVelocity) * tickTime;
		FCheckResult Hit(1.0);
		GetLevel()->MoveActor(this, Adjusted, Rotation, Hit, 1);
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
					GetLevel()->MoveActor(this, Delta, Rotation, Hit, 1);
					if (Hit.Time < 1.0) //hit second wall
					{
						if (Hit.Normal.Z > 0.7)
							landed = 1;	
						FVector DesiredDir = Adjusted.Normal();
						TwoWallAdjust(DesiredDir, Delta, Hit.Normal, OldHitNormal, Hit.Time);
						GetLevel()->MoveActor(this, Delta, Rotation, Hit, 1);
						if (Hit.Normal.Z > 0.7)
							landed = 1;
					}
				}
			}
		}
		ticks++;
		if (ticks > 150)
		{
			GetLevel()->FarMoveActor(this, OriginalPos, 1, 1); //move actor back to starting point
			landed = 1;
		}
	}

	Landing = Location;
	if (!movePawn)
		GetLevel()->FarMoveActor(this, OriginalPos, 1, 1); //move actor back to starting point

	unguard;
}

int APawn::FindJumpUp(FVector Dest, FVector vel, FVector &Landing, int moveActor)
{
	guard(APawn::FindJumpUp);

	//FIXME - try to jump up 
	return 0;

	unguard;
}

/* Find best jump from current position toward destination.  Assumes that there is no immediate 
barrier.  Sets vel to the suggested initial velocity, Landing to the expected Landing, 
and moves actor if moveActor is set */
int APawn::FindBestJump(FVector Dest, FVector vel, FVector &Landing, int movePawn)
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
		success = (dist.Size() < olddist.Size() - 8.0);
		// FIXME - if failed, imagine with no jumpZ (step out first)
		if (!movePawn)
			GetLevel()->FarMoveActor(this, realLocation, 1, 1); //move actor back to starting point
	}
	return success;

	unguard;
}


/* walkMove() 
- returns 1 if move happened, zero if it didn't because of barrier, and -1
if it didn't because of ledge
Move direction must not be adjusted.
*/
int APawn::walkMove(const FVector &Delta, FLOAT threshold, int bAdjust)
{
	guard(APawn::walkMove);
	int result = 1;
	FVector StartLocation = Location;
	//acceleration shouldn't matter

	testWalking(Delta);
	if (Physics == PHYS_Falling) //fell off ledge
	{
		if (bAdjust) 
			GetLevel()->FarMoveActor(this, StartLocation, 1, 1);
		result = -1;
		Physics = PHYS_Walking;
	}
	else //check if move successful
	{
		FVector RealMove = Location - StartLocation;
		if (RealMove.Size() < threshold) //FIXME - bigger/smaller threshold?
		{
			if (bAdjust)
				GetLevel()->FarMoveActor(this, StartLocation, 1, 1);
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

	ULevel *MyLevel = GetLevel();
	for (INDEX i=0; i<MyLevel->Num; i++)
	{
		AActor *Actor = MyLevel->Element(i); 
		if (Actor && Actor->IsA("CreaturePoint"))
		{
			ACreaturePoint *node = (ACreaturePoint *)Actor;
			node->visitedWeight = 10000000.0;
			node->bEndPoint = 0;
			node->bSpecialBlock = 0;
			node->cost = 0.0;
		}
	}

	unguard;
}

int APawn::findPathToward(AActor *goal, INT maxpaths, FLOAT maxweight, AActor *&bestPath)
{
	guard(APawn::findPathToward);

	bestPath = NULL;
	if (!goal)
		return 0;

	FVector Dest = goal->Location;

	if (Physics != PHYS_Flying)
		if (goal->IsA("Pawn"))
		{
			APawn *goalpawn = (APawn *)goal;
			if (goalpawn->Physics == PHYS_Falling)
				goalpawn->jumpLanding(goalpawn->Velocity, Dest);
		//	else if (goalpawn->Physics == PHYS_Flying)
		//		Dest.Z = GetLevel()->FloorZ(Dest);
		}
	//debugf(" %s FindPathToward %s", GetClass()->GetName(), goal->GetClass()->GetName());
	
	AActor *EndAnchor;
	if (goal->IsA("CreaturePoint"))
	{
		EndAnchor = goal;
		//debugf("Goal is a creature point");
	}
	else
		EndAnchor = NULL;

	int	clearpath = findPathTo(Dest, maxpaths, maxweight, bestPath, EndAnchor);

	return clearpath;

	unguard;
}

#define MAXP 8

int APawn::findPathTo(FVector Dest, INT maxpaths, FLOAT maxweight, AActor *&bestPath, AActor *EndAnchorPath)
{
	guard(APawn::findPathTo);
	clock(GServer.AudioTickTime)
	bestPath = NULL;
	ULevel *MyLevel = GetLevel();
	FVector targdist;

	//if (maxpaths == 1)
	//	return(findOnePathTo(Dest, bestPath));
	//if (maxpaths == 2) //FIXME - support anchors in this, or merge it in
	//	return findTwoPathTo(Dest, bestPath, maxweight);
	int success = 0;
	int result = 0;
	AActor *EndPoint[MAXP];
	AActor *DestPoint[MAXP];
	FLOAT Dist[MAXP];
	FLOAT DestDist[MAXP];
	//INT Check[MAXP];

	int numEndPoints = 0;
	int numDestPoints = 0;
	FCheckResult Hit(1.0);

	//adjust destination using farmoveactor
	FVector RealLocation = Location;
	success = MyLevel->FarMoveActor(this, Dest, 1);
	Dest = Location;
	if (success)
		MyLevel->FarMoveActor(this, RealLocation, 1, 1);
	else
	{
		//debugf("destination in unreachable place");
		unclock(GServer.AudioTickTime);
		//debugf("Find path time was %f", GApp->CpuToMilliseconds(GServer.AudioTickTime));
		return 0;	
	}

	//FIXME - add ACreaturePoint linked list
	//FIXME - extend anchor distance beyond collision radius
	// find paths visible from this pawn
	FVector	ViewPoint = Location;
	ViewPoint.Z += BaseEyeHeight; //look from eyes
	int startanchor = 0;
	int endanchor = 0;

	if (MoveTarget && MoveTarget->IsA("CreaturePoint") 
		&& ((MoveTarget->Location - Location).Size() < CollisionRadius))
	{
		//debugf("On a start anchor");
		startanchor = 1;
		EndPoint[0] = MoveTarget;
		Dist[0] = 0;
		numEndPoints = 1;
	}

	if (EndAnchorPath != NULL)
	{
		//debugf("found an end anchor");
		endanchor = 1;
		DestPoint[0] = EndAnchorPath;
		DestDist[0] = 0;
		numDestPoints = 1;
	}

	INDEX i=0;
	if ( startanchor && endanchor )
		i = MyLevel->Num; //don't need to go through actors

	while ( i<MyLevel->Num )
	{
		AActor *Actor = MyLevel->Element(i); 
		if ( Actor && Actor->IsA("CreaturePoint") )
		{
			if (!startanchor && ((Location - Actor->Location).SizeSquared() < 640000) )
			{
				MyLevel->Trace(Hit, this, Actor->Location, ViewPoint, TRACE_Level); //VisBlocking))
				if (Hit.Time == 1.0) 
				{ 	
					if (maxpaths == 1) //then path must also be visible to target
						MyLevel->Trace(Hit, this, Dest, Actor->Location, TRACE_Level);
					if (Hit.Time == 1.0)
					{
						FLOAT dist2D = (Actor->Location - Location).Size2D() 
										+ 2 * Max(0.f, Actor->Location.Z - Location.Z); //penalize paths above
						if (numEndPoints < MAXP)
						{
							EndPoint[numEndPoints] = Actor;
							Dist[numEndPoints] = dist2D;
							numEndPoints++;
						}
						else
						{
							INDEX j = 0;
							while (j<MAXP)
							{
								if (Dist[j] > dist2D)
								{
									Dist[j] = dist2D;
									EndPoint[j] = Actor;
									j = 16;
								}
								j++;
							}
						}
						if (!endanchor && (maxpaths == 1)) //don't look for endanchor ??
						{
							if (numDestPoints < MAXP)
							{
								DestPoint[numDestPoints] = Actor;
								DestDist[numDestPoints] = dist2D;
								numDestPoints++;
							}
							else
							{
								INDEX j = 0;
								while (j<MAXP)
								{
									if (DestDist[j] > dist2D)
									{
										DestDist[j] = dist2D;
										DestPoint[j] = Actor;
										j = MAXP;
									}
									j++;
								}
							}
						}
					}
				}
			}
				
			if (!endanchor && (maxpaths > 1) && ((Dest - Actor->Location).SizeSquared() < 640000)) //fixme - smaller size when path building supports it
			{
				MyLevel->Trace(Hit, this, Dest, Actor->Location, TRACE_Level);
				if (Hit.Time == 1.0) //VisBlocking))
				{
					FLOAT dist2D = targdist.Size2D()
								 + 2 * Max(0.f, Actor->Location.Z - Location.Z);
					if ((dist2D > 4 * CollisionRadius) //FIXME > COLLISIONRADIUS not optimal
						|| (Abs(Actor->Location.Z - Dest.Z) > CollisionHeight))
					{
						if (numDestPoints < MAXP)
						{
							DestPoint[numDestPoints] = Actor;
							DestDist[numDestPoints] = dist2D;
							numDestPoints++;
						}
						else
						{
							INDEX j = 0;
							while (j<MAXP)
							{
								if (DestDist[j] > dist2D)
								{
									DestDist[j] = dist2D;
									DestPoint[j] = Actor;
									j = MAXP;
								}
								j++;
							}
						}
					}
					else if (MyLevel->FarMoveActor(this, Actor->Location, 1))
					{
						if (Reachable(Dest))
						{
							DestPoint[0] = Actor;
							numDestPoints = 1;
							endanchor = 1;
						}
						MyLevel->FarMoveActor(this, RealLocation, 1, 1);
					}
				}
			}
		}
		i++;
	}

	if ((numEndPoints == 0) || (numDestPoints == 0))
	{
		unclock(GServer.AudioTickTime);
		//debugf("Find path time was %f", GApp->CpuToMilliseconds(GServer.AudioTickTime));
		return 0;
	}

	//debugf("Visible endpoints = %d", numEndPoints);
	//debugf("visible destpoints = %d", numDestPoints);
	// order based on distance
	if (numEndPoints > 1)
	{
		for (INDEX j=0; j<numEndPoints; j++)
		{
			for (i=1; i<numEndPoints; i++)
			{
				if (Dist[i] < Dist[i-1])
				{
					FLOAT tempDist = Dist[i];
					AActor *tempActor = EndPoint[i];
					Dist[i] = Dist[i-1];
					EndPoint[i] = EndPoint[i-1];
					Dist[i-1] = tempDist;
					EndPoint[i-1] = tempActor;
				}
			}
		}
	}

	//mark all paths reachable by this pawn
	//and remove further visibles that are close enough
	//FIXME - finish merging findonepathto in - special case(check combined dist instead of dist for reaches)

	if (!startanchor) //look for a start anchor
	{
		INDEX j = 0;
		while (j<numEndPoints)
		{
			if (Dist[j] < 4 * CollisionRadius) //FIXME - greater than collision radius not optimal
			{
				if (Abs(EndPoint[j]->Location.Z - Location.Z) < CollisionHeight)
				{
					if (pointReachable(EndPoint[j]->Location))
					{
						startanchor = 1;
						EndPoint[0] = EndPoint[j];
						Dist[0] = Dist[j];
						j = numEndPoints;
					}
				}
			}
			else
				j = numEndPoints;

			j++;
		}
	}

	//if (startanchor && !(ACreaturePoint *)EndPoint[0])
	//	debugf("Illegal start anchor!");

	if (startanchor)
	{
		//debugf("Found start anchor");
		if (MyLevel->FarMoveActor(this, EndPoint[0]->Location, 1))
		{
			if (pointReachable(Dest))
			{
				bestPath = EndPoint[0];
				MyLevel->FarMoveActor(this, RealLocation, 1);
				unclock(GServer.AudioTickTime);
				//debugf("Find path time was %f", GApp->CpuToMilliseconds(GServer.AudioTickTime));
				return 1;
			}
			MyLevel->FarMoveActor(this, RealLocation, 1, 1);
		}

		numEndPoints = 1;
		ACreaturePoint *anchor = (ACreaturePoint *)EndPoint[0];
		if (anchor->bPathsDefined == 0)
		{
			unclock(GServer.AudioTickTime);
			//debugf("Find path time was %f", GApp->CpuToMilliseconds(GServer.AudioTickTime));
			return 0;
		}

		INDEX j = 0;
		FReachSpec *spec;
		while (j<16)
		{
			if (anchor->Paths[j] == -1)
				j = 16;
			else
			{
				//if (anchor->Paths[j] > GetLevel()->ReachSpecs->Num - 1)
				//	debugf("illegal reachspec offset");
				//else
				//{
					spec = &MyLevel->ReachSpecs->Element(anchor->Paths[j]);
					//if (spec->Start != anchor)
					//	debugf("Wrong reachspec called!");
					//else if (!spec)
					//	debugf("Spec was NULL");
					//else 
					if (spec->supports(this))
					{
						success = 1;
						((ACreaturePoint *)spec->End)->bEndPoint = 1;
						numEndPoints ++;
					}
				//}
				j++;
			}
		}
	}
	else
	{
		//debugf("NO start anchor");
		int num = 0;
		i = 0;
		while (i<numEndPoints)
		{
			if (pointReachable(EndPoint[i]->Location))
			{
				num++;
				((ACreaturePoint *)EndPoint[i])->bEndPoint = 1;
				success = 1;
				if (num == 3)
					i = numEndPoints;
				/*
				INDEX j = 0;
				while (j<16)
				{
					if (((ACreaturePoint *)EndPoint[i])->Paths[j] == -1)
						j = 16;
					else
					check if reachspec to any on this list which supports
					pawn, and the added distance isn't significant
				*/
			}
			i++;
		}
	}

	//debugf("Num reachable ends %d", numEndPoints);
	//now explore paths from destination
	if (success)
	{
		AActor *newPath = NULL;
		FCheckResult Result;
		FLOAT bestweight = maxweight;
		FLOAT currentweight;
		FLOAT newbest;
		FVector dir;

		// order based on distance
		if (numDestPoints > 1)
		{
			for (INDEX j=0; j<numDestPoints; j++)
			{
				for (i=1; i<numDestPoints; i++)
				{
					if (DestDist[i] < DestDist[i-1])
					{
						FLOAT tempDist = DestDist[i];
						AActor *tempActor = DestPoint[i];
						DestDist[i] = DestDist[i-1];
						DestPoint[i] = DestPoint[i-1];
						DestDist[i-1] = tempDist;
						DestPoint[i-1] = tempActor;
					}
				}
			}
		}

		int num = 0;
		i = 0;
		FLOAT minDist = (Location - Dest).Size();

		while ( i<numDestPoints )
		{
			AActor *Path = DestPoint[i];
			dir = Dest - Path->Location;
			currentweight = dir.Size();
			dir = Path->Location - RealLocation;
			if (((currentweight + dir.Size()) < bestweight)
				&& MyLevel->FarMoveActor(this, Path->Location, 1)
				&& pointReachable(Dest))
				{	
					newbest = bestweight;
					if (bestPathFrom(Path, newbest, currentweight, newPath, maxpaths))
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
					num++;
					if (num == 3)
						i = numDestPoints;
				}
			i++;
			if (bestweight < minDist * 1.5)
				i = numDestPoints;
		}
	}
	MyLevel->FarMoveActor(this, RealLocation, 1, 1);
	//if (result) debugf("Found a Path");
	//else debugf("NO Path found");
	unclock(GServer.AudioTickTime);
	//debugf("Find path time was %f", GApp->CpuToMilliseconds(GServer.AudioTickTime));
	return result;
	unguard;
}

/* FindOnePathTo() FIXME - NOT USED, REMOVE
fast support for common special case of findPathTo()
where only one intermediate ACreaturePoint is allowed
*/
/*
int APawn::findOnePathTo(FVector Dest, AActor *&bestPath)
{
	guard(APawn::findOnePathTo);
	int result = 0;
	ULevel *MyLevel = GetLevel();
	AActor *EndPoint[16];
	FLOAT Dist[16];
	int numEndPoints = 0;
	FCheckResult Hit(0.0);

	//assume I am adjusting from corner - so just try nearest path and see if it works


	
	//debugf("Find One Path");
	//adjust destination using farmoveactor
	FVector RealLocation = Location;
	CollisionRadius += 6.0; //FIXME - remove this adjust!!!
	if (MyLevel->FarMoveActor(this, Dest, 1))
	{
		Dest = Location;
		CollisionRadius -= 6.0;
		MyLevel->FarMoveActor(this, RealLocation, 1);
	}
	else
	{
		//debugf("destination in unreachable place");
		CollisionRadius -= 6.0;
		unclock(GServer.AudioTickTime);
		debugf("Find path time was %f", GApp->CpuToMilliseconds(GServer.AudioTickTime));

		return 0;	
	}

	//FIXME - add ACreaturePoint linked list
	//FIXME - extend anchor distance beyond collision radius
	// find paths visible from this pawn
	FVector	ViewPoint = Location;
	ViewPoint.Z += BaseEyeHeight; //look from eyes
	for (INDEX i=0; i<MyLevel->Num; i++)
	{
		AActor *Actor = MyLevel->Element(i); 
		if (Actor && Actor->IsA("CreaturePoint"))
		{
			MyLevel->Trace(Hit, this, Actor->Location, ViewPoint, TRACE_Level);
			if (Hit.Time == 1.0) //VisBlocking))
			{
				MyLevel->Trace(Hit, this, Actor->Location, Dest, TRACE_Level);
				if (Hit.Time == 1.0) //VisBlocking)) 
				{ 	
					FVector distance = Actor->Location - Location;
					FLOAT dist = distance.Size();
					distance = Actor->Location - Dest;
					dist += distance.Size();
					if (numEndPoints < 16)
					{
						EndPoint[numEndPoints] = Actor;
						Dist[numEndPoints] = dist;
						numEndPoints++;
					}
					else
					{
						INDEX j = 0;
						while (j<numEndPoints)
						{
							if (Dist[j] > dist)
							{
								Dist[j] = dist;
								EndPoint[j] = Actor;
								j = numEndPoints;
							}
							j++;
						}
					}
				}
			}
		}
	}

	//debugf("Visible endpoints = %d", numEndPoints);

	// order based on distance
	if (numEndPoints > 1)
	{
		for (INDEX j=0; j<numEndPoints; j++)
		{
			for (i=1; i<numEndPoints; i++)
			{
				if (Dist[i] < Dist[i-1])
				{
					FLOAT tempDist = Dist[i];
					AActor *tempActor = EndPoint[i];
					Dist[i] = Dist[i-1];
					EndPoint[i] = EndPoint[i-1];
					Dist[i-1] = tempDist;
					EndPoint[i-1] = tempActor;
				}
			}
		}
	}

	//find shortest path w/ one ACreaturePoint
	while(i<numEndPoints)
	{
		if (Reachable(EndPoint[i]->Location))
			if (MyLevel->FarMoveActor(this, EndPoint[i]->Location, 1))
			{
				if (pointReachable(Dest))
				{
					i = numEndPoints;
					result = 1;
					bestPath = EndPoint[i];
				}
				MyLevel->FarMoveActor(this, RealLocation, 1);
			}
		i++;
	}
	unclock(GServer.AudioTickTime);
	debugf("Find path time was %f", GApp->CpuToMilliseconds(GServer.AudioTickTime));
	return result;
	unguard;
}
*/

/* FindTwoPathTo()
fast support for common special case of findPathTo()
where only two intermediate ACreaturePoint are allowed
*/
//FIXME NOT CURRENTLY USED - fix or remove
int APawn::findTwoPathTo(FVector Dest, AActor *&bestPath, FLOAT maxweight)
{
	guard(APawn::findTwoPathTo);
	int result = 0;
	ULevel *MyLevel = GetLevel();
	AActor *EndPoint[16];
	AActor *DestPoint[16];
	FLOAT Dist[16];
	FLOAT DestDist[16];
	int numEndPoints = 0;
	int numDestPoints = 0;
	FCheckResult Hit(0.0);

	//debugf("Find Two Path");
	//adjust destination using farmoveactor
	FVector RealLocation = Location;
	CollisionRadius += 6.0; //FIXME - remove this adjust!!!
	if (MyLevel->FarMoveActor(this, Dest, 1))
	{
		Dest = Location;
		CollisionRadius -= 6.0;
		MyLevel->FarMoveActor(this, RealLocation, 1, 1);
	}
	else
	{
		//debugf("destination in unreachable place");
		CollisionRadius -= 6.0;
		unclock(GServer.AudioTickTime);
		//debugf("Find path time was %f", GApp->CpuToMilliseconds(GServer.AudioTickTime));

		return 0;	
	}

	// find paths visible from this pawn and from dest
	FVector	ViewPoint = Location;
	ViewPoint.Z += BaseEyeHeight; //look from eyes
	for (INDEX i=0; i<MyLevel->Num; i++)
	{
		AActor *Actor = MyLevel->Element(i); 
		if (Actor && Actor->IsA("CreaturePoint"))
		{
			MyLevel->Trace(Hit, this, Actor->Location, ViewPoint, TRACE_Level);
			if (Hit.Time == 1.0) //VisBlocking))
			{ 	
				FVector distance = Actor->Location - Location;
				FLOAT dist = distance.Size();
				if (numEndPoints < 16)
				{
					EndPoint[numEndPoints] = Actor;
					Dist[numEndPoints] = dist;
					numEndPoints++;
				}
				else
				{
					INDEX j = 0;
					while (j<numEndPoints)
					{
						if (Dist[j] > dist)
						{
							Dist[j] = dist;
							EndPoint[j] = Actor;
							j = numEndPoints;
						}
						j++;
					}
				}
			}
			MyLevel->Trace(Hit, this, Actor->Location, Dest, TRACE_Level);
			if (Hit.Time == 1.0) //VisBlocking)) 
			{ 	
				FVector distance = Actor->Location - Dest;
				FLOAT dist = distance.Size();
				if (numDestPoints < 16)
				{
					DestPoint[numDestPoints] = Actor;
					DestDist[numDestPoints] = dist;
					numDestPoints++;
				}
				else
				{
					INDEX j = 0;
					while (j<numDestPoints)
					{
						if (DestDist[j] > dist)
						{
							DestDist[j] = dist;
							DestPoint[j] = Actor;
							j = numDestPoints;
						}
						j++;
					}
				}
			}
		}
	}

	//debugf("Visible endpoints = %d", numEndPoints);

	// order endpoints based on distance
	if (numEndPoints > 1)
	{
		for (INDEX j=0; j<numEndPoints; j++)
		{
			for (i=1; i<numEndPoints; i++)
			{
				if (Dist[i] < Dist[i-1])
				{
					FLOAT tempDist = Dist[i];
					AActor *tempActor = EndPoint[i];
					Dist[i] = Dist[i-1];
					EndPoint[i] = EndPoint[i-1];
					Dist[i-1] = tempDist;
					EndPoint[i-1] = tempActor;
				}
			}
		}
	}

	// order destpoints based on distance
	if (numDestPoints > 1)
	{
		for (INDEX j=0; j<numDestPoints; j++)
		{
			for (i=1; i<numDestPoints; i++)
			{
				if (DestDist[i] < DestDist[i-1])
				{
					FLOAT tempDist = DestDist[i];
					AActor *tempActor = DestPoint[i];
					DestDist[i] = DestDist[i-1];
					DestPoint[i] = DestPoint[i-1];
					DestDist[i-1] = tempDist;
					DestPoint[i-1] = tempActor;
				}
			}
		}
	}

	//FIXME - create sorted list of all potential paths (with valid midpath) (up to 256)
	//find shortest path w/ two ACreaturePoints
	//starting from nearest endpoint - find what DestPoints can be reached, and in order
	//of potential dist try them (if any are better than BestDist
	FLOAT BestDist = maxweight;
	FLOAT PotentialDist;
	INT EndPointReachable;
	FReachSpec *spec;
	INDEX iSpec;

	while(i<numEndPoints)
	{
		EndPointReachable = 0;
		INDEX j = 0;
		if (((ACreaturePoint *)EndPoint[i])->bPathsDefined == 0)
			return 0;

		while (j<16)
		{
			iSpec = ((ACreaturePoint *)EndPoint[i])->Paths[j];
			if (iSpec == -1)
				j = 16;
			else
			{
				spec = &MyLevel->ReachSpecs->Element(iSpec);
				if (spec->supports(this))
					((ACreaturePoint *)spec->End)->bEndPoint = 1;
				j++;
			}
		}
		j = 0;
		while (j < numDestPoints)
		{
			if (((ACreaturePoint *)DestPoint[j])->bEndPoint)
			{
				((ACreaturePoint *)DestPoint[j])->bEndPoint = 0; //clear for next pass
				FVector MidPath = DestPoint[j]->Location - EndPoint[i]->Location;
				PotentialDist = Dist[i] + DestDist[j] + MidPath.Size();
				if (PotentialDist < BestDist)
				{
					if (EndPointReachable || Reachable(EndPoint[i]->Location))
					{
						EndPointReachable = 1;
						if (MyLevel->FarMoveActor(this, DestPoint[j]->Location, 1))
						{
							if (pointReachable(Dest))
							{
								result = 1;
								bestPath = EndPoint[i];
								BestDist = PotentialDist;
							}
							MyLevel->FarMoveActor(this, RealLocation, 1, 1);
						}
					}
					else
						j = numDestPoints;
				}
				j++;
			}
		}
		i++;
	}
	unclock(GServer.AudioTickTime);
	//debugf("Find path time was %f", GApp->CpuToMilliseconds(GServer.AudioTickTime));
	return result;
	unguard;
}

/* FindRandomDest()
returns a random pathnode reachable from creature's current location
*/
int APawn::findRandomDest(INT maxpaths, FLOAT maxweight, AActor *&bestPath)
{
	guard(APawn::findRandomDest);
	int result = 0;
	ULevel *MyLevel = GetLevel();
	AActor *EndPoint[16];
	FLOAT Dist[16];
	int numEndPoints = 0;
	FCheckResult Hit(0.0);

	// find paths visible from this pawn
	FVector	ViewPoint = Location;
	ViewPoint.Z += BaseEyeHeight; //look from eyes
	INDEX i=0;
	while (i<MyLevel->Num)
	{
		AActor *Actor = MyLevel->Element(i); 
		if (Actor && Actor->IsA("CreaturePoint"))
		{
			MyLevel->Trace(Hit, this, Actor->Location, ViewPoint, TRACE_Level); //VisBlocking))
			if (Hit.Time == 1.0)
			{ 	
				if (numEndPoints < 8)
				{
					FVector dir = Actor->Location - Location;
					EndPoint[numEndPoints] = Actor;
					Dist[numEndPoints] = dir.Size();
					numEndPoints++;
				}
				else
					i = MyLevel->Num;
			}
		}
		i++;
	}

	// order endpoints based on distance
	if (numEndPoints > 1)
	{
		for (INDEX j=0; j<numEndPoints; j++)
		{
			for (i=1; i<numEndPoints; i++)
			{
				if (Dist[i] < Dist[i-1])
				{
					FLOAT tempDist = Dist[i];
					AActor *tempActor = EndPoint[i];
					Dist[i] = Dist[i-1];
					EndPoint[i] = EndPoint[i-1];
					Dist[i-1] = tempDist;
					EndPoint[i-1] = tempActor;
				}
			}
		}
	}

	//mark reachable path nodes
	int numReached = 0;
	for(i=0; i<numEndPoints; i++)
	{
		if (!((ACreaturePoint *)EndPoint[i])->bEndPoint)
			if (Reachable(EndPoint[i]->Location))
				numReached = numReached + TraverseFrom(EndPoint[i]);
	}

	//pick a random path node from among those reachable
	i=0;
	bestPath = NULL;
	while (i<MyLevel->Num)
	{
		if (numReached == 0)
			i = MyLevel->Num;
		else
		{
			AActor *Actor = MyLevel->Element(i); 
			if (Actor && Actor->IsA("CreaturePoint"))
			{
				if (((ACreaturePoint *)Actor)->bEndPoint)
				{
					result = 1;
					bestPath = Actor;
					if (frand() * (float)numReached <= 1.0)
						i = MyLevel->Num; //quit with current path
					numReached -= 1;
				}
			}
			i++;
		}
	}
	return result;
	unguard;
}

/* TraverseFrom()
traverse the graph, marking all reached paths.  Return number of new paths marked
*/

int APawn::TraverseFrom(AActor *start)
{
	guard(APawn::TraverseFrom);
	int numMarked = 1; //mark the one we're at
	ACreaturePoint *node = (ACreaturePoint *)start;
	if (node->bPathsDefined == 0)
		return 0;

	node->bEndPoint = 1;
	FReachSpec *spec;
	ULevel *MyLevel = GetLevel();
	int i = 0;
	while (i<16)
	{
		if (node->Paths[i] == -1)
			i = 16;
		else
		{
			spec = &MyLevel->ReachSpecs->Element(node->Paths[i]);
			ACreaturePoint *nextNode = spec->End->IsA("CreaturePoint") ? (ACreaturePoint *)spec->End : NULL;

			if (nextNode && !nextNode->bEndPoint)
				if (spec->supports(this))
					numMarked = numMarked + TraverseFrom(nextNode);
			i++;
		}
	}
	return numMarked;
	unguard;
}

/* bestPathFrom()
parameters - bestweight is the current best weight, and currentweight is the current weight along this route
(currentweight should be less than best weight, or don't call)
(best weight may be updated)
startnode is the starting path
if a better path is found, it is returned
FIXME - use maxdepth
*/
int APawn::bestPathFrom(AActor *start, float &bestweight, float currentweight, AActor *&bestPath, int maxdepth)
{
	guard(APawn::bestPathFrom);
	int result = 0;
	ACreaturePoint *startnode = (ACreaturePoint *)start;
	if ((startnode->bPathsDefined == 0) || (startnode->visitedWeight <= currentweight))
		//paths not defined, or already came through here, faster
	{
		bestPath = NULL;
		result = 0;
	}
	else
	{
		startnode->visitedWeight = currentweight;
		//debugf("Weight for node at (%f, %f, %f) is %f",
		//	startnode->Location.X,startnode->Location.Y,startnode->Location.Z,currentweight);
		if (startnode->bEndPoint) //success
		{
			//debugf("Success!");
			bestweight = currentweight;
			bestPath = startnode;
			result = 1;
		}
		else if (maxdepth == 1)
		{
			bestPath = NULL;
			result = 0;
		}
		else
		{
			ULevel *MyLevel = GetLevel();
			AActor *nextPath;
			AActor *newPath = NULL;
			FReachSpec *spec;
			maxdepth = maxdepth - 1;;
			int i = 0;
			while (i<16)
			{
				if (startnode->upstreamPaths[i] == -1)
					i = 16;
				else
				{
					spec = &MyLevel->ReachSpecs->Element(startnode->upstreamPaths[i]);
					if (spec->supports(this))
					{ 
						nextPath = spec->Start;
						FLOAT newweight = currentweight + spec->distance + ((ACreaturePoint *)nextPath)->cost;
						if (newweight < bestweight)
							if (bestPathFrom(nextPath, bestweight, newweight, newPath, maxdepth))
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