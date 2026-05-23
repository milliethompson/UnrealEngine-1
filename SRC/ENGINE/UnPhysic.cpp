/*=============================================================================
	UnPhysic.cpp: Actor physics implementation

  Physics related intrinsics

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Steven Polge 3/97
=============================================================================*/

#include "Unreal.h"

/* execEnginePhysics() is an intrinsic function which may be called by a script if 
script wants to explicitly cause the execution of the normal
engine physics
*/
static void execEnginePhysics( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execEnginePhysics);
	debugState(Context!=NULL);
	AActor *ActorContext = (AActor *)Context;

	P_GET_FLOAT(deltaTime);
	P_FINISH;

		ActorContext->performPhysics(deltaTime);

	unguardSlow;
}
AUTOREGISTER_INTRINSIC(EX_FirstPhysics, execEnginePhysics);

static void execMoveSmooth( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execMoveSmooth);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_VECTOR(Delta);
	P_FINISH;

	FCheckResult Hit(1.0);
	int didHit = ActorContext->GetLevel()->MoveActor( ActorContext, Delta, ActorContext->Rotation, Hit );
	if (Hit.Time < 1.0)
	{
		FVector Adjusted = (Delta - Hit.Normal * (Delta | Hit.Normal)) * (1.0 - Hit.Time);
		if( (Delta | Adjusted) >= 0 )
		{
			FVector OldHitNormal = Hit.Normal;
			FVector DesiredDir = Delta.Normal();
			ActorContext ->GetLevel()->MoveActor(ActorContext, Adjusted, ActorContext->Rotation, Hit);
			if (Hit.Time < 1.0)
			{
				ActorContext->Process(NAME_HitWall, &PVector(Hit.Normal));
				ActorContext->TwoWallAdjust(DesiredDir, Adjusted, Hit.Normal, OldHitNormal, Hit.Time);
				ActorContext->GetLevel()->MoveActor(ActorContext, Adjusted, ActorContext->Rotation, Hit);
			}
		}
	}

	*(DWORD*)Result = didHit;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC((EX_FirstPhysics + 1), execMoveSmooth );

static void execSetPhysics( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSetPhysics);
	debugState(Context!=NULL);
	AActor *ActorContext = (AActor *)Context;

	P_GET_BYTE(NewPhysics);
	P_FINISH;

		ActorContext->setPhysics(NewPhysics);

	unguardSlow;
}
AUTOREGISTER_INTRINSIC(EX_FirstPhysics+2, execSetPhysics);

//======================================================================================

void AActor::setPhysics(BYTE NewPhysics, AActor *NewFloor)
{
	guard(AActor::setPhysics);
	Physics = NewPhysics;
	//SetBase(NULL);
	/*
	if ((Physics == PHYS_Walking) || (Physics == PHYS_Rolling))
	{
		if (NewFloor)
			Floor = NewFloor;
		else
		{
		//determine floor
		}
	}
	*/
	unguard;
}

void AActor::performPhysics(FLOAT DeltaSeconds)
{
	guard(AActor::performPhysics);
	APawn *ThisPawn = this->IsA("Pawn") ? (APawn*)this : NULL;
	FVector OldVelocity = Velocity;

	// change position
	switch (Physics)
	{
		case PHYS_Walking: if (ThisPawn) ThisPawn->physWalking(DeltaSeconds); break;
		case PHYS_Falling: physFalling(DeltaSeconds); break;
		case PHYS_Flying: if (ThisPawn) ThisPawn->physFlying(DeltaSeconds); break;
		case PHYS_Projectile: physProjectile(DeltaSeconds); break;
		case PHYS_Rolling: physRolling(DeltaSeconds); break;
		// case PHYS_Swimming: if (ThisPawn) ThisPawn->physSwimming(DeltaSeconds); break;
		case PHYS_Interpolating: physPathing(DeltaSeconds); break;
		case PHYS_MovingBrush: physMovingBrush(DeltaSeconds); break;

	}

	// rotate
	if ((Physics != PHYS_None) && (RotationSpeed > 0) && (bPitch || bYaw || bRoll)) 
	{
		// note: You can make this happen automatically by declaring the C++ physicsRotation function as
		// virtual. That will cause the appropriate version to be called based on the actor's class. The new actors-as-objects code
		// works with C++ virtual functions. -Tim
		if (ThisPawn)
			ThisPawn->physicsRotation(DeltaSeconds, OldVelocity);
		else
			physicsRotation(DeltaSeconds, OldVelocity);
	}

	if (ThisPawn)
		ThisPawn->MoveTimer -= DeltaSeconds;

	unguard;
}


int AActor::fixedTurn(int current, int desired, int deltaRate, int fixed, int clockwise)
{
	guard(AActor::fixedTurn);
	int result = current & 65535;
	if (fixed)
	{
		if (clockwise)
		{
			if (result < desired)
				result += Min((result - desired), deltaRate);
			else
				result += Min((65536 + desired - result), deltaRate);
		}
		else
		{
			if (result > desired)
				result -= Min((result - desired), deltaRate);
			else
				result -= Min((65536 + result - desired), deltaRate);
		}
	}
	else
	{
		if (result > desired)
		{
			if (result - desired < 32768)
				result -= Min((result - desired), deltaRate);
			else
				result += Min((65536 + desired - result), deltaRate);
		}
		else 
		{
			if (desired - result < 32768)
				result += Min((desired - result), deltaRate);  
			else  	
				result -= Min((65536 - desired + result), deltaRate);
		}
	}

	return result;
	unguard;
}

void APawn::physicsRotation(FLOAT deltaTime, FVector OldVelocity)
{
	guard(APawn::physicsRotation);

	// Accumulate a desired new rotation.
	FRotation NewRotation = Rotation;	

	if (!bIsPlayer) //don't pitch or yaw player
	{
		int deltaRotation = (int)((float)(RotationSpeed) * deltaTime);
	
		//YAW 
		//if (bYaw) //don't need - pawns always yaw
		NewRotation.Yaw = fixedTurn(NewRotation.Yaw, DesiredRotation.Yaw, 
							deltaRotation, bFixedRotationDir, bYawClockwise);
		//}

		//PITCH
		if (bPitch)
		{
			//pawns pitch instantly
			NewRotation.Pitch = DesiredRotation.Pitch & 65535;
			//debugf("desired pitch %f actual pitch %f",DesiredRot.Pitch, NewRotation.Pitch);
			if (NewRotation.Pitch > MaxPitch) //bound pitch
			{
				if (NewRotation.Pitch < 32768)
					NewRotation.Pitch = MaxPitch;
				else if (NewRotation.Pitch < 65536 - MaxPitch)
					NewRotation.Pitch = 65536 - MaxPitch;
			}
		}
	}

	//ROLL
	if (bRoll && (MaxPitch > 0)) 
	{
		//pawns roll based on physics
		FVector RealAcceleration = Velocity - OldVelocity;
		RealAcceleration.TransformVectorBy(GMath.UnitCoords * NewRotation); //y component will affect roll
		if (RealAcceleration.Y < 0) 
			NewRotation.Roll = Min(MaxPitch, (int)(-1 * RealAcceleration.Y * 16384.0/AccelRate)); 
		else
			NewRotation.Roll = Max(65536 - MaxPitch, (int)(65536.0 - RealAcceleration.Y * 16384.0/AccelRate));
	}
	else
		NewRotation.Roll = 0;

	// Set the new rotation.
	if( NewRotation != Rotation )
	{
		FCheckResult Hit(1.0);
		GetLevel()->MoveActor( this, FVector(0,0,0), NewRotation, Hit );
	}

	unguard;
}


//FIXME - remove old velocity from actor physics rotation?
void AActor::physicsRotation(FLOAT deltaTime, FVector OldVelocity)
{
	guard(AActor::physicsRotation);
	
	// Accumulate a desired new rotation.
	FRotation NewRotation = Rotation;	

	int deltaRotation = (int)((float)(RotationSpeed) * deltaTime);

	if (bYaw) //YAW
		NewRotation.Yaw = fixedTurn(NewRotation.Yaw, DesiredRotation.Yaw, 
								deltaRotation, bFixedRotationDir, bYawClockwise);
	if (bPitch) //PITCH
		NewRotation.Pitch = fixedTurn(NewRotation.Pitch, DesiredRotation.Pitch, 
								deltaRotation, bFixedRotationDir, bPitchClockwise);
	if (bRoll) //ROLL
		NewRotation.Roll = fixedTurn(NewRotation.Roll, DesiredRotation.Roll, 
								deltaRotation, bFixedRotationDir, bRollClockwise);	

	// Set the new rotation.
	if( NewRotation != Rotation )
	{
		FCheckResult Hit(1.0);
		GetLevel()->MoveActor( this, FVector(0,0,0), NewRotation, Hit );
	}

	if (Rotation == DesiredRotation)
	{
		//by default, restart rotation (set desiredRotation to 359.9 degrees away)
		if (IsProbing(NAME_EndedRotation))
			Process(NAME_EndedRotation, NULL); //tell thing rotation ended
		else
		{
			if (bYaw)
			{
				if (bYawClockwise)
					DesiredRotation.Yaw -= 1;
				else
					DesiredRotation.Yaw += 1;
			}
			if (bPitch)
			{
				if (bPitchClockwise)
					DesiredRotation.Pitch -= 1;
				else
					DesiredRotation.Pitch += 1;
			}
			if (bRoll)
			{
				if (bRollClockwise)
					DesiredRotation.Roll -= 1;
				else
					DesiredRotation.Roll += 1;
			}
		}
	}
	unguard;
}


/* Bump() - FIXME REMOVE
called by Level::MoveActor(), so that physics can affect the bumped actor
*/

void AActor::Bump(AActor *Bumped)
{
	guard(AActor::Bump);
	APawn *Pawn = Bumped->IsA("Pawn") ? (APawn*)Bumped : NULL; 
/*	if (Pawn) 
	{
		if (Physics == PHYS_Walking)  
			Physics = PHYS_Falling;
		if (Physics == PHYS_Falling)
		{
			Pawn->Velocity += Mass/Pawn->Mass * 0.1 * Velocity;
			Pawn->Velocity.Z += 30;
		}
		//fixme support swimming and flying
	} */

	unguard;
}

inline void AActor::TwoWallAdjust(FVector &DesiredDir, FVector &Delta, FVector &HitNormal, FVector &OldHitNormal, FLOAT HitTime)
{
	guard(AActor::TwoWallAdjust);

	if ((OldHitNormal | HitNormal) <= 0) //90 or less corner, so use cross product for dir
	{
		FVector NewDir = (HitNormal ^ OldHitNormal);
		NewDir.Normalize();
		Delta = (Delta | NewDir) * (1.0 - HitTime) * NewDir;
		if ((DesiredDir | Delta) < 0)
			Delta = -1 * Delta;
	}
	else //adjust to new wall
	{
		Delta = (Delta - HitNormal * (Delta | HitNormal)) * (1.0 - HitTime); 
		if ((Delta | DesiredDir) <= 0)
			Delta = FVector(0,0,0);
	}
	unguard;
}

/*
testWalking()
like physwalking, except - no physics, no move adjustment, and no notifies
deltaTime always = 0.05 (the default timeTick rate)
*/

void APawn::testWalking(const FVector &DesiredMove)
{
	guard(APawn::testWalking);

	FVector Delta = DesiredMove;
	Delta.Z = 0.0;
	//-------------------------------------------------------------------------------------------
	//Perform the move
	FVector DesiredDir = DesiredMove.Normal();
	FVector GravDir = Zone->ZoneGravity.Normal(); 
	FVector TestDrop = GravDir * 4.0;
	FVector Down = GravDir * MaxStepHeight;
	FVector Up = -1 * Down;
	FCheckResult Hit(1.0);
	int stillgoing = 1;
	FVector StartLoc = Location;


	if (!Delta.IsNearlyZero())
	{
		GetLevel()->MoveActor(this, Delta, Rotation, Hit, 1, 1);
		if (Hit.Time < 1.0) //try to step up
		{
			Delta = Delta * (1.0 - Hit.Time);
			GetLevel()->MoveActor(this, Up, Rotation, Hit, 1, 1); //FIXME - step up only as far as needed
			GetLevel()->MoveActor(this, Delta, Rotation, Hit, 1, 1);
			GetLevel()->MoveActor(this, Down, Rotation, Hit, 1, 1);
			//Scouts want only good floors, else undo move
			if ((Hit.Time < 1.0) && (Hit.Normal.Z < 0.7))
			{
				GetLevel()->FarMoveActor(this, StartLoc, 1);
				stillgoing = 0;
			}
		}

		//FIXME instead of Hit.Normal.Z, use dotp with gravity everywhere
		//FIXME - handle sliding off ramps and falling better
		//drop to floor
		if (stillgoing)
		{
			GetLevel()->MoveActor(this, Down, Rotation, Hit, 1, 1);
			GetLevel()->MoveActor(this, TestDrop, Rotation, Hit, 1, 1); 
			if (Hit.Time > 0.526) //then falling
				Physics = PHYS_Falling; //default
			else if (Hit.Normal.Z < 0.7)
				GetLevel()->FarMoveActor(this, StartLoc, 1);
		}
	}

 	unguard;
}

/*
physWalking()

*/

void APawn::physWalking(FLOAT deltaTime)
{
	guard(APawn::physWalking);
	//bound acceleration
	//goal - support +-Z gravity, but not other vectors
	Velocity.Z = 0;
	Acceleration.Z = 0;

	FVector VelDir = Velocity.Normal();
	if (Acceleration.Size() == 0.0) 
	{
		FVector OldVel = Velocity;
		Velocity = Velocity - (2 * VelDir) * Velocity.Size()  
			* deltaTime * Zone->ZoneGroundFriction; //don't drift to a stop, brake
		if ((OldVel | Velocity) < 0.0) //brake to a stop, not backwards
			Velocity = FVector(0,0,0);
	}
	else
	{
		FVector AccelDir = Acceleration.Normal();
		Acceleration = AccelDir * Min(Acceleration.Size(), AccelRate);
		Velocity = Velocity - (VelDir - AccelDir) * Velocity.Size()  
			* deltaTime * Zone->ZoneGroundFriction; //don't drift to a stop, brake 
	}

	Velocity = Velocity + Acceleration * deltaTime;
	FLOAT maxVelocity = GroundSpeed;
	if (!bIsPlayer)
		maxVelocity *= DesiredSpeed;
	if (Velocity.Size() > maxVelocity)
		Velocity = Velocity.Normal() * maxVelocity;
	
	//debugf("Velocity = %f %f %f", Velocity.X, Velocity.Y, Velocity.Z);
	FVector DesiredMove = Velocity + Zone->ZoneVelocity;
	DesiredMove.Z = 0.0;
	//-------------------------------------------------------------------------------------------
	//Perform the move
	FLOAT remainingTime = deltaTime;
	FLOAT realVelZ = 0.0;
	FLOAT timeTick = 0.05;
	FVector DesiredDir = DesiredMove.Normal();
	FVector GravDir = Zone->ZoneGravity.Normal(); 
	FVector TestDrop = GravDir * 4.0;
	FVector Down = GravDir * MaxStepHeight;
	FVector Up = -1 * Down;
	FCheckResult Hit(1.0);
	while (remainingTime > 0.0)
	{
		if (remainingTime > timeTick)
			timeTick = Min(timeTick, remainingTime * 0.5f);
		else timeTick = remainingTime;

		remainingTime -= timeTick;

		//FIXME? - move calculation of velocity here?

		FVector Delta = timeTick * DesiredMove;
		if (!Delta.IsNearlyZero())
		{
			GetLevel()->MoveActor(this, Delta, Rotation, Hit);
			if (Hit.Time < 1.0) //try to step up
			{
				//fixme - handle short stairs (try stepping up again, if I moved some
				// and the hitnormal is vertical (don't go up ramps this way!!!)
				//right now, just handling message (don't send hitwalls on stairs & ramps!)
				//note that some too steep ramps may cause AI problems (no hitwall)
				Delta = Delta * (1.0 - Hit.Time);
				GetLevel()->MoveActor(this, Up, Rotation, Hit); //FIXME - step up only as far as needed
				GetLevel()->MoveActor(this, Delta, Rotation, Hit);
				if (Hit.Time < 1.0) 
				{
						if (!Hit.Actor->IsA("Pawn"))
							Process(NAME_HitWall, &PVector(Hit.Normal));
						//adjust and try again
						FVector OriginalDelta = Delta;
			
						// Try again.
						FVector OldHitNormal = Hit.Normal;
						Delta = (Delta - Hit.Normal * (Delta | Hit.Normal)) * (1.0 - Hit.Time);
						if( (Delta | OriginalDelta) >= 0 )
						{
							GetLevel()->MoveActor(this, Delta, Rotation, Hit);
							if (Hit.Time < 1.0)
							{
								if (!Hit.Actor->IsA("Pawn"))
									Process(NAME_HitWall, &PVector(Hit.Normal));
								TwoWallAdjust(DesiredDir, Delta, Hit.Normal, OldHitNormal, Hit.Time);
								GetLevel()->MoveActor(this, Delta, Rotation, Hit);
							}
						}
				}
				GetLevel()->MoveActor(this, Down, Rotation, Hit);
			
				if ((Hit.Time < 1.0) && (Hit.Normal.Z < 0.5))
					{
						Delta = (Down - Hit.Normal * (Down | Hit.Normal))  * (1.0 - Hit.Time);
						if( (Delta | Down) >= 0 )
							GetLevel()->MoveActor(this, Delta, Rotation, Hit);
					} 
			}
		}

			//FIXME instead of Hit.Normal.Z, use dotp with gravity everywhere
			//FIXME - handle sliding off ramps and falling better
			//drop to floor
			GetLevel()->MoveActor(this, Down, Rotation, Hit);
			FLOAT DropTime = Hit.Time;
			if (DropTime < 1.0) //slide down slope, depending on friction and gravity 
			{
				if ((Hit.Normal.Z < 1.0) && ((Hit.Normal.Z * Zone->ZoneGroundFriction) < 3.3))
				{
					FVector Slide = (timeTick * Zone->ZoneGravity/(2 * Max(0.5f, Zone->ZoneGroundFriction))) * timeTick;
					Delta = Slide - Hit.Normal * (Slide | Hit.Normal);
					if( (Delta | Slide) >= 0 )
						GetLevel()->MoveActor(this, Delta, Rotation, Hit);
				}				
			}

			GetLevel()->MoveActor(this, TestDrop, Rotation, Hit); 
			if ((Hit.Time > 0.526) || (Hit.Normal.Z < 0.7)) //then falling
			{
				/*if (Hit.Time < 1.0) //FIXME remove
				{
					debugf("Fall Test Time = %f",Hit.Time);
					debugf("Fall location = %f, %f, %f", Location.X, Location.Y, Location.Z);
				} */
				setPhysics(PHYS_Falling); //default
				FVector AdjustUp = -1 * (TestDrop * Hit.Time + Down * DropTime); 
				GetLevel()->MoveActor(this, AdjustUp, Rotation, Hit);
				Process(NAME_Falling, NULL);
				if (Physics == PHYS_Falling)
				{
					realVelZ = Location.Z;
					physFalling(remainingTime);
					realVelZ = Location.Z - realVelZ;
				}
				else 
				{
					Delta = remainingTime * DesiredDir;
					GetLevel()->MoveActor(this, Delta, Rotation, Hit); 
				}
			
				remainingTime = 0.0;
			}
			else if( Hit.Actor != Base)
			{
				// Handle floor notifications (standing on other actors).
				//debugf("%s is now on floor %s",GetClassName(),Hit.Actor ? Hit.Actor->GetClassName() : "None");
				SetBase( Hit.Actor );
			}

			//debugf("Z is %f",Location.Z);
	}
	// make velocity reflect actual move
	Velocity = (Location - OldLocation) / deltaTime;
	Velocity.Z = realVelZ;
 	unguard;
}

void AActor::physFalling(FLOAT deltaTime)
{
	guard(AActor::physFalling);

	// This is a hack so that falling actors get their base set to None.
	// Steve, you can remove this once we have an AActor::SetPhysics function
	// which performs the proper floor notifications when actors go into physics
	// modes which don't support floors.
	if( Base != NULL )
	{
		// Handle base notifications (standing on other actors).
		//debugf("%s is off floor",GetClassName());
		SetBase( NULL );
	}

	//bound acceleration, falling object has minimal ability to impact acceleration
	APawn *ThisPawn = this->IsA("Pawn") ? (APawn*)this : NULL;
	if (ThisPawn)
	{
 		FLOAT maxAccel = ThisPawn->AccelRate * 0.05;
		FLOAT speed2d = Velocity.Size2D(); 
 		if (speed2d < 40.0) //allow initial burst
			maxAccel = maxAccel + (40 - speed2d)/deltaTime;
	
		Acceleration.Z = 0;
		if (Acceleration.Size() > maxAccel)
			Acceleration = Acceleration.Normal() * maxAccel;
	}
	FLOAT remainingTime = deltaTime;
	FLOAT timeTick = 0.05;
	int numBounces = 0;
	while (remainingTime > 0.0)
	{
		if (remainingTime > timeTick)
			timeTick = Min(timeTick, remainingTime * 0.5f);
		else timeTick = remainingTime;

		remainingTime -= timeTick;
		Velocity = Velocity * (1 - Zone->ZoneFluidFriction * timeTick) 
					+ (Acceleration + Zone->ZoneGravity) * timeTick; 
		FVector Adjusted = (Velocity + Zone->ZoneVelocity) * timeTick;
		FCheckResult Hit(1.0);
		GetLevel()->MoveActor(this, Adjusted, Rotation, Hit);
		if (Hit.Time < 1.0)
		{
			if (bBounce)
			{
				Process(NAME_HitWall, &PVector(Hit.Normal));
				if (numBounces < 2)
					remainingTime += timeTick * (1.0 - Hit.Time);
				numBounces++;
			}
			else
			{
				if (Hit.Normal.Z > 0.7)
				{
					setPhysics(PHYS_Walking);

					// This should be moved to the physics-mode setting code,
					// perhaps SetPhysics( EPhysics NewPhysics, AActor *NewBase ).
					if( Hit.Actor != Base )
					{
						// Handle floor notifications (standing on other actors).
						//debugf("%s landed on floor %s",GetClassName(),Hit.Actor ? Hit.Actor->GetClassName() : "None");
						SetBase( Hit.Actor );
					}

					Process(NAME_Landed, NULL);
					if ((Physics == PHYS_Walking) && ThisPawn)
					{
						Acceleration = Acceleration.Normal();
						ThisPawn->physWalking(remainingTime + timeTick * (1.0 - Hit.Time));
						remainingTime = 0.0;
					}
				}
				else
				{
					if (!Hit.Actor->IsA("Pawn"))
							Process(NAME_HitWall, &PVector(Hit.Normal));
					FVector OldHitNormal = Hit.Normal;
					FVector Delta = (Adjusted - Hit.Normal * (Adjusted | Hit.Normal)) * (1.0 - Hit.Time);
					if( (Delta | Adjusted) >= 0 )
					{
						GetLevel()->MoveActor(this, Delta, Rotation, Hit);
						if (Hit.Time < 1.0) //hit second wall
						{
							if (Hit.Normal.Z > 0.7)
							{
								setPhysics(PHYS_Walking);
								Process(NAME_Landed, NULL);
							}
							else if (!Hit.Actor->IsA("Pawn"))
								Process(NAME_HitWall, &PVector(Hit.Normal));
		
							FVector DesiredDir = Adjusted.Normal();
							TwoWallAdjust(DesiredDir, Delta, Hit.Normal, OldHitNormal, Hit.Time);
							GetLevel()->MoveActor(this, Delta, Rotation, Hit);
							if (Hit.Normal.Z > 0.7)
							{
								setPhysics(PHYS_Walking);
								Process(NAME_Landed, NULL);
							}
						}
					}
				}
			}
		}

	}

	if (!bBounce)
		Velocity = (Location - OldLocation) / deltaTime;

	unguard;
}

/* Flying has no gravity
This is used only by pawns 
*/
void APawn::physFlying(FLOAT deltaTime)
{
	guard(APawn::physFlying);

	//bound acceleration, calculate velocity, add effects of friction and momentum
	if (Acceleration.Size() > ((APawn *)this)->AccelRate)
		Acceleration = Acceleration.Normal() * ((APawn *)this)->AccelRate;

	Velocity = Velocity * (1 - Zone->ZoneFluidFriction * deltaTime)
				+ Acceleration * deltaTime;
	FLOAT maxVelocity = AirSpeed;
	if (!bIsPlayer)
		maxVelocity *= DesiredSpeed;
	if (Velocity.Size() > maxVelocity)
		Velocity = Velocity.Normal() * maxVelocity;

	FVector Adjusted = (Velocity + Zone->ZoneVelocity) * deltaTime; 
	FCheckResult Hit;
	GetLevel()->MoveActor(this, Adjusted, Rotation, Hit);
	if (Hit.Time < 1.0) 
	{
		FVector DesiredDir = Adjusted.Normal();
		if (!Hit.Actor->IsA("Pawn"))
			Process(NAME_HitWall, &PVector(Hit.Normal));
		//adjust and try again
		FVector OldHitNormal = Hit.Normal;
		FVector Delta = (Adjusted - Hit.Normal * (Adjusted | Hit.Normal)) * (1.0 - Hit.Time);
		if( (Delta | Adjusted) >= 0 )
		{
			GetLevel()->MoveActor(this, Delta, Rotation, Hit);
			if (Hit.Time < 1.0) //hit second wall
			{
				if (!Hit.Actor->IsA("Pawn"))
					Process(NAME_HitWall, &PVector(Hit.Normal));
				TwoWallAdjust(DesiredDir, Delta, Hit.Normal, OldHitNormal, Hit.Time);
				GetLevel()->MoveActor(this, Delta, Rotation, Hit);
			}
		}
	}
	Velocity = (Location - OldLocation) / deltaTime;
	unguard;
}

/* PhysProjectile is tailored for projectiles 
*/
void AActor::physProjectile(FLOAT deltaTime)
{
	guard(AActor::physProjectile);

	//bound acceleration, calculate velocity, add effects of friction and momentum
	//friction affects projectiles less (more aerodynamic)
	float remainingTime = deltaTime;
	int numBounces = 0;

	while (remainingTime > 0.0)
	{
		Velocity = (Velocity * (1 - 0.2 * Zone->ZoneFluidFriction * remainingTime))
					+ Acceleration * remainingTime;
		float timeTick = remainingTime;
		remainingTime = 0.0;

		if (this->IsA("Projectile"))
		{
 			if (Velocity.Size() > ((AProjectile *)this)->MaxSpeed)
				Velocity = Velocity.Normal() * ((AProjectile *)this)->MaxSpeed;
		}

		FVector Adjusted = (Velocity + Zone->ZoneVelocity) * deltaTime; 
		FCheckResult Hit(1.0);
		GetLevel()->MoveActor(this, Adjusted, Rotation, Hit);
		
		if (Hit.Time < 1.0)
		{
			FVector DesiredDir = Adjusted.Normal();
			Process(NAME_HitWall, &PVector(Hit.Normal));
			if (bBounce)
			{
				if (numBounces < 2)
					remainingTime = timeTick * (1.0 - Hit.Time);
				numBounces++;
			}
		}
	}

	if (!bBounce)
		Velocity = (Location - OldLocation) / deltaTime;

	unguard;
}

/*
physRolling() - intended for non-pawns which are rolling or sliding along a floor

*/

void AActor::physRolling(FLOAT deltaTime)
{
	guard(APawn::physWalking);
	//bound acceleration
	//goal - support +-Z gravity, but not other vectors
	Velocity.Z = 0;
	Acceleration.Z = 0;

	Velocity = Velocity - (Velocity.Normal() - Acceleration.Normal()) * Velocity.Size()
		* (100.0/Max(1.f,Mass)) * deltaTime * Zone->ZoneGroundFriction; 

	Velocity = Velocity * (1 - Zone->ZoneFluidFriction * deltaTime) + Acceleration * deltaTime;
	FVector DesiredMove = Velocity + Zone->ZoneVelocity;
	DesiredMove.Z = 0.0;

	//-------------------------------------------------------------------------------------------
	//Perform the move
	FLOAT remainingTime = deltaTime;
	FLOAT realVelZ = 0.0;

	if (!DesiredMove.IsNearlyZero())
	{
	FLOAT timeTick = 0.05;
	FVector DesiredDir = DesiredMove.Normal();
	FVector GravDir = Zone->ZoneGravity.Normal(); 
	FVector TestDrop = GravDir * 4.0;
	FVector Down = GravDir * 24.0;
	FCheckResult Hit(1.0);
	int numBounces = 0;
	while (remainingTime > 0.0)
	{
		if (remainingTime > timeTick)
			timeTick = Min(timeTick, remainingTime * 0.5f);
		else timeTick = remainingTime;

		remainingTime -= timeTick;

		//FIXME? - move calculation of velocity here?

		FVector Delta = timeTick * DesiredMove;
		if (!Delta.IsNearlyZero())
		{
			GetLevel()->MoveActor(this, Delta, Rotation, Hit);
			if (Hit.Time < 1.0) 
			{
				Process(NAME_HitWall, &PVector(Hit.Normal));
				if (bBounce)
				{
					if (numBounces < 2)
						remainingTime = timeTick * (1.0 - Hit.Time);
					numBounces++;
				}
				else
				{
						//adjust and try again
						FVector OriginalDelta = Delta;
			
						// Try again.
						FVector OldHitNormal = Hit.Normal;
						Delta = (Delta - Hit.Normal * (Delta | Hit.Normal)) * (1.0 - Hit.Time);
						if( (Delta | OriginalDelta) >= 0 )
						{
							GetLevel()->MoveActor(this, Delta, Rotation, Hit);
							if (Hit.Time < 1.0)
							{
								Process(NAME_HitWall, &PVector(Hit.Normal));
								TwoWallAdjust(DesiredDir, Delta, Hit.Normal, OldHitNormal, Hit.Time);
								GetLevel()->MoveActor(this, Delta, Rotation, Hit);
							}
						}
				}
			}

			//FIXME instead of Hit.Normal.Z, use dotp with gravity everywhere
			//FIXME - handle sliding off ramps and falling better
			//drop to floor
			GetLevel()->MoveActor(this, Down, Rotation, Hit);
			FLOAT DropTime = Hit.Time;
			if (DropTime < 1.0) //slide down slope, depending on friction and gravity 
			{
				if ((Hit.Normal.Z < 1.0) && ((Hit.Normal.Z * Zone->ZoneGroundFriction) < 3.3))
				{
					FVector Slide = (timeTick * Zone->ZoneGravity/(2 * Max(0.5f, Zone->ZoneGroundFriction))) * timeTick;
					Delta = Slide - Hit.Normal * (Slide | Hit.Normal);
					if( (Delta | Slide) >= 0 )
						GetLevel()->MoveActor(this, Delta, Rotation, Hit);
				}				
			}

			GetLevel()->MoveActor(this, TestDrop, Rotation, Hit); 
			if ((Hit.Time > 0.526) || (Hit.Normal.Z < 0.7)) //then falling
			{
				setPhysics(PHYS_Falling); //default
				FVector AdjustUp = -1 * (TestDrop * Hit.Time + Down * DropTime); 
				GetLevel()->MoveActor(this, AdjustUp, Rotation, Hit);
				Process(NAME_Falling, NULL);
				if (Physics == PHYS_Falling)
				{
					realVelZ = Location.Z;
					physFalling(remainingTime);
					realVelZ = Location.Z - realVelZ;
				}
				else 
				{
					Delta = remainingTime * DesiredDir;
					GetLevel()->MoveActor(this, Delta, Rotation, Hit); 
				}
			
				remainingTime = 0.0;
			}
			else if( Hit.Actor )
			{
				// Code added by Tim to support floor notifications (standing on other actors).
			}

			//debugf("Z is %f",Location.Z);
		}
	}
	}
	// make velocity reflect actual move
	Velocity = (Location - OldLocation) / deltaTime;
	Velocity.Z = realVelZ;
 	unguard;
}