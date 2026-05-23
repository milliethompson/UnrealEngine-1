/*=============================================================================
	UnActPro.cpp: Projectile actor code

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "UnGame.h"
#include "UnFActor.h"
#include "UnCheat.h"

/*=============================================================================
	Public projectile functions
=============================================================================*/

//
// Spawn a projectile and return its actor index or INDEX_NONE
//
// Velocity  = Velocity vector, usually the direction the parent is facing.
// Class     = Class of projectile
INDEX FGame::SpawnProjectile
(
    INDEX           iParentActor
,   UClass        * Class
,   const FVector & Velocity
,   FLOAT           StartDist
)
{
    GUARD;
    FActor & Parent = FActor::Actor(iParentActor);
    FVector Start = Parent.Location + Velocity * (StartDist/Velocity.Size());
    //
    FActor * ProjectileActor = Parent.Instigate(Class,&Start);
    if( ProjectileActor != 0 )
    {
        AProjectile & Projectile = ProjectileActor->Projectile();
        //
        Projectile.Velocity     = Velocity      ;
        Projectile.iParent      = iParentActor  ;
        //tbc: Conversion
        if( Parent.IsPlayer() )
        {
            Projectile.DrawRot = Parent.ViewRot;
        }
        else
        {
            Projectile.DrawRot = ((FVector&)Velocity).Rotation();
        }
        if( GCheat->SlowProjectiles )
        {
            Projectile.Velocity.Normalize();
            Projectile.Velocity *= 2.0;
            Projectile.LifeSpan = 700;
        }
    }
    return ProjectileActor != 0 ? ProjectileActor->iMe : INDEX_NONE;
    UNGUARD("FGame::SpawnProjectile");
}

//tbm:
// Yaw the vector by the specified Angle.
//tbi: This is not really what happens.
static inline void Yaw(FVector & This, WORD Angle) //tbi:word
{
    const FLOAT X1 = This.X;
    const FLOAT Y1 = This.Y;
    const FLOAT Cos = GMath.CosTab(Angle);
    const FLOAT Sin = GMath.SinTab(Angle);
    This.X = Y1*Sin + X1*Cos;
    This.Y = Y1*Cos - X1*Sin;
}

//
// Spawn a projectile based on an actor's rotation
//
INDEX FGame::SpawnForwardProjectile 
(
    INDEX       iParentActor
,   UClass    * Class
,   FLOAT       StartDist
,   int         YawDeviation
,   int         PitchDeviation
)
{
    GUARD;
    FActor & Parent  = FActor::Actor(iParentActor);
    FCoords Coords;
    FVector Velocity;
    //
    // Transform the actor's vector:
    //
    Parent.GetViewCoords(&Coords);
    AProjectile & DefaultProjectile = FActor::Actor( Class->DefaultActor ).Projectile();
    if( YawDeviation != 0 )
    {
        Yaw( Coords.ZAxis, YawDeviation );                                            
    }
    //todo: Use PitchDeviation
    Velocity = Coords.ZAxis * DefaultProjectile.Speed;
    //
    return SpawnProjectile(iParentActor,Class,Velocity,StartDist);
    UNGUARD("FGame::SpawnForwardProjectile");
}

//----------------------------------------------------------------------------
//                 AProjectile base processing
//----------------------------------------------------------------------------
int AProjectile::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    FActor & Actor = FActor::Actor(*this);
    switch (Message.Index)
    {
        //----------------------------------------------------------
        //                     Initialization
        //----------------------------------------------------------
        case ACTOR_BeginPlay:
        {
            // Establish any texture animation:
            if( TextureCount > 0 )
            {
                TextureList = int(&Textures[0]); //tbi: Conversion            
            }
            return ProcessParent;
            break;
        }
        //----------------------------------------------------------
        //                 Time passes
        //----------------------------------------------------------
        case ACTOR_Tick:
        case ACTOR_PlayerTick:
        {
            //todo: Delete:  ARoot::Process(Level,Message,Params);
            // Move the projectile and check for wall collision
            Level->MoveActor(iMe,&Velocity);
            if (Level->ModelInfo.FuzzyPointClass(Location,CollisionRadius*1.01,0)<0.9999)
            {
                // The projectile collided with a wall.
                Actor.Send_HitWall();
            }
            return ProcessParent;
            break;
        }
        //----------------------------------------------------------
        //                 The projectile hit a wall
        //----------------------------------------------------------
        case ACTOR_HitWall:
        {
            const BOOL ShouldBounce = bBounce && ( MaxBounceCount==0 || BounceCount<MaxBounceCount );
            BOOL ShouldDie = !ShouldBounce;
            Actor.Instigate(EffectOnWallImpact != 0 ? EffectOnWallImpact : EffectOnImpact);
            if (ShouldBounce)
            {
                FVector Reflection;
                BounceCount++;
                const BOOL ReflectsOkay = Level->ModelInfo.SphereReflect(Location,CollisionRadius*1.01,Reflection);
                if( ReflectsOkay )
                {
                    Reflection.Normalize();
                    if( BounceIncidence > 0 )
                    {
                        // Make sure the incident angle of the bounce is such that
                        // we want to do the bounce.
                        FVector Direction = Velocity.Normal();
                        const FLOAT Cosine = - (Direction | Reflection); // Cosine of incident angle (relative to normal)
                        if( Cosine > BounceIncidence )
                        {
                            ShouldDie = TRUE;
                        }
                    }
                    if( !ShouldDie )
                    {
                        Velocity = Velocity.Mirror( Reflection);
                        DrawRot = Velocity.Rotation();
                    }
                }
                else
                {
                    ShouldDie = TRUE;
                }
            }
            if( ShouldDie )
            {
                Level->DestroyActor(iMe);
            }
            return ProcessDone;
            break;
        }
        //----------------------------------------------------------
        //           The projectile touched something (someone?)
        //----------------------------------------------------------
        case ACTOR_Touch: 
        {
            PTouch & TouchInfo = *(PTouch*)Params;
            FActor & HitActor = FActor::Actor(TouchInfo.iActor );
            const BOOL HitAPawn = HitActor.IsKindOf("Pawn");
            // If we touched something which is not a projectile target,
            // then let's just pass through it. (If the HitActor did not
            // allow the projectile to pass through, we would get an ACTOR_Bump
            // instead of an ACTOR_Touch message.
            if( HitActor.bProjTarget )
            {
                UClass * Effect = (HitAPawn && EffectOnPawnImpact != 0) ? EffectOnPawnImpact : EffectOnImpact;
                Actor.Instigate(Effect);
                PHit HitInfo;
                HitInfo.Empty();
                for( int Which = 0; Which < DMT_Count; Which++ )
                {
                    FLOAT DamageAmount = Damage[Which] - (Age-1) * DamageDecay[Which];
                    if( DamageAmount < 0 ) { DamageAmount = 0.0; }
                    HitInfo.Damage[Which] = DamageAmount;
                }
                HitInfo.iInstigator     = iInstigator   ;
                HitInfo.iSourceActor    = iMe           ;
                HitInfo.iSourceWeapon   = INDEX_NONE    ;
                HitInfo.Momentum        = Mass * Velocity.Size();
                FActor & ParentActor = FActor::Actor(iParent);
                if( ParentActor.Class->IsKindOf("Weapon") )
                {
                    //todo:[Delete] HitInfo.iSourceActor  = ParentActor.iParent;
                    HitInfo.iSourceWeapon = iParent;
                }
                HitActor.Send_Hit( HitInfo );
                // Handle any explosive charge imparted to target:
                if( ExplosiveTransfer != 0 )
                {
                    HitActor.AddExplosiveCharge(ExplosiveTransfer);
                }
                Actor.Send_Die();
                //todo: [Delete] LifeSpan = 1; 
                //todo: [Delete] Velocity = GMath.ZeroVector;
            }
            return ProcessParent;
            break;
        }
        //----------------------------------------------------------
        //     The projectile bumped into something impenetrable
        //----------------------------------------------------------
        case ACTOR_Bump:
        {
            PTouch & TouchInfo = *(PTouch*)Params;
            FActor & HitActor = FActor::Actor(TouchInfo.iActor );
            const BOOL HitAPawn = HitActor.IsKindOf("Pawn");
            if( !HitActor.bProjTarget )
            {
                // Let's treat this like hitting a wall, but don't allow bouncing...            
                Actor.Instigate(EffectOnWallImpact != 0 ? EffectOnWallImpact : EffectOnImpact);
                Level->DestroyActor(iMe);
            }
            return ProcessDone;
        }
        //----------------------------------------------------------
        //           The projectile should cease to exist.
        //----------------------------------------------------------
        case ACTOR_Die:
        {
            Actor.Instigate( EffectAtLifeSpan );
            return ProcessParent;
            break;
        }
    }
    return ProcessParent;
    UNGUARD("AProjectile::Process");
}

#if 0 //todo: Delete
//----------------------------------------------------------------------------
//                 AFireball base processing
//----------------------------------------------------------------------------
int AFireball::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    return 0;
    UNGUARD("AFireball::Process");
}

//----------------------------------------------------------------------------
//                 AFireball2 base processing
//----------------------------------------------------------------------------
int AFireball2::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    return ProcessParent;
    UNGUARD("AFireball2::Process");
}


//----------------------------------------------------------------------------
//                 AShellProjectile base processing
//----------------------------------------------------------------------------
int AShellProjectile::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    return ProcessParent;
    UNGUARD("AShellProjectile::Process");
}

//----------------------------------------------------------------------------
//                 ABulletProjectile base processing
//----------------------------------------------------------------------------
int ABulletProjectile::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    return ProcessParent;
    UNGUARD("ABulletProjectile::Process");
}

//----------------------------------------------------------------------------
//                 AStingerProjectile base processing
//----------------------------------------------------------------------------
int AStingerProjectile::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    return ProcessParent;
    UNGUARD("AStingerProjectile::Process");
}

//----------------------------------------------------------------------------
//                 ATentacleProjectile base processing
//----------------------------------------------------------------------------
int ATentacleProjectile::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    return ProcessParent;
    UNGUARD("ATentacleProjectile::Process");
}
#endif

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
