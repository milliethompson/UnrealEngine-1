/*
==============================================================================
UnExplos.cpp: Actor explosion handling

Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
    Functions for handling actors of class Explosion.

Revision history:
    * 09/26/96: Created by Mark
==============================================================================
*/

#include "UnGame.h"
#include "UnFActor.h"

//---------------------------------------------------------------------------
//              AExplosion::Process
//---------------------------------------------------------------------------
int AExplosion::Process(ILevel *Level, FName Message, void *Params)
{
   GUARD;
   FActor & Actor = FActor::Actor(*this);
   switch (Message.Index)
    {
        //--------------------------------------------------------------------
        //                      Initialization
        //--------------------------------------------------------------------
        case ACTOR_BeginPlay:
        {
            break;
        }
        //--------------------------------------------------------------------
        //                      Time passes...
        //--------------------------------------------------------------------
        case ACTOR_Tick:
        {
            //todo: Do we really need EDamageCount kinds of damage for an explosion?
            // How about a single Damage value and an EDamageType value to indicate
            // the kind of damage. This simplifies things a little but limits an 
            // explosion to 1 kind of damage.

            // Each tick the explosion lives we will "explode", affecting the actors
            // within the explosion radius. However, an explosion will usually have
            // a LifeSpan of 1, which means it explodes just once. In addition, if
            // the radius becomes <= 0, the explosion is effectively ended.
            const BOOL Delayed = Age < Delay;
            if( Age == Delay+1 )
            {
                Actor.Instigate(Effect);
                Actor.MakeSound(InitialSound);
                Actor.MakeNoise(Noise);
            }
            if( !Delayed && Radius > 0 )
            {
                PHit HitInfo; 
                // Prepare a hit message to be sent to affected actors.
                {
                    HitInfo.Empty();
                    HitInfo.iInstigator  = iInstigator    ;
                    HitInfo.iSourceActor = iMe            ;
                    HitInfo.HitLocation  = Actor.Location ;
                }
                // Scan all appropriate actors to see if they are affected. 
                // Only check colliding actors.
                const FLOAT ExplosionRadiusSquared = Radius*Radius;
    	        for( int Which = 0; Which < Level->Actors->CollidingActors->Count(); Which++ )
                {
                    AActor * const CheckActor = (*Level->Actors->CollidingActors)[Which];
                    if( CheckActor != 0  )
                    {
                        const FLOAT DistanceSquared = (CheckActor->Location - Actor.Location).SizeSquared();
                        // See if the actor is within the explosion radius:
                        if( DistanceSquared <= ExplosionRadiusSquared )
                        {
                            // Determine a factor for damage and momentum calculations.
                            const FLOAT Factor = 1.0-DistanceSquared/ExplosionRadiusSquared;
                            if( Momentum > 0 )
                            {
                                HitInfo.Momentum = Momentum * Factor;
                            }
                            else
                            {
                                HitInfo.Momentum = 0;
                            }
                            for( int Which = 0; Which < DMT_Count; Which++ )
                            {
                                if( Damage[Which] > 0 )
                                {
                                    HitInfo.Damage[Which] = Damage[Which]  * Factor;
                                }
                            }
                            FActor::Actor(*CheckActor).Send_Hit( HitInfo );
                        }
                    }
                }
            }

            // Adjust properties for next Tick:
            if( !Delayed )
            {
                for( int Which = 0; Which < DMT_Count; Which++ )
                {
                    Damage[Which] -= DamageIncrement;
                }
                Momentum -= MomentumIncrement   ;
                Radius   -= RadiusIncrement     ;
            }
            break;
        }
    }
    return ProcessParent;
    UNGUARD( "AExplosion::Process" );
}

//---------------------------------------------------------------------------
//              ATarydiumExplosion::Process
//---------------------------------------------------------------------------
int ATarydiumExplosion::Process(ILevel *Level, FName Message, void *Params)
{
   GUARD;
    return ProcessParent;
    UNGUARD( "ATarydiumExplosion::Process" );
}

//---------------------------------------------------------------------------
//              AShellExplosion::Process
//---------------------------------------------------------------------------
int AShellExplosion::Process(ILevel *Level, FName Message, void *Params)
{
   GUARD;
    return ProcessParent;
    UNGUARD( "AShellExplosion::Process" );
}

//---------------------------------------------------------------------------
//              AClipExplosion::Process
//---------------------------------------------------------------------------
int AClipExplosion::Process(ILevel *Level, FName Message, void *Params)
{
   GUARD;
    return ProcessParent;
    UNGUARD( "AClipExplosion::Process" );
}
