/*=============================================================================
	UnRoot.cpp: Root actor class code

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "UnGame.h"
#include "UnFActor.h"
#include "UnRandom.h"

/*-----------------------------------------------------------------------------
	Root actor function
-----------------------------------------------------------------------------*/

int AActor::Process(ILevel *Level, FName Message, void *Params)
	{
	GUARD;
    FActor & Actor = FActor::Actor(*this);
	switch (Message.Index)
		{
		case ACTOR_Null: // Do nothing
			return 1;
		case ACTOR_Debug: // Debug inquiry
			debugf (LOG_Actor,"%s %i debug query",Level->Level->Name,iMe);
			return 1;
		case ACTOR_PlayerTick: // Player timer tick
		case ACTOR_Tick: // Nonplayer timer tick
			{
	        GAudio.SfxMoveActor(iMe,&Location);
            // Take care of life-span checking:
            if( LifeSpan > 0 && Age > LifeSpan )
            {
                return Actor.Send_Die();
            }
            Age++;
            Era++;
            if( TeleportDelay > 0 )
            {
                TeleportDelay--;
            }
            Actor.AnimationTick();
            // Take care of timer:
            if( TimerCountdown > 0 )
            {
                TimerCountdown--;
                if( TimerCountdown==0 )
                {
                    Level->SendMessage( iMe, (TimerMessage!=NAME_NONE)?TimerMessage:ACTOR_Timer, 0 );
                }
            }
            if( iPendingTeleporter != INDEX_NONE )
            {
                Actor.TeleportTo( iPendingTeleporter );
                iPendingTeleporter = INDEX_NONE;
            }
			return 1;
			};
		case ACTOR_Spawn: // Actor was just created
            {
            // Spawned actors don't count in level scoring...
            Actor.bIsSecretGoal = FALSE;
            Actor.bIsItemGoal   = FALSE;
            Actor.bIsKillGoal   = FALSE;
			return 1;
            }
		case ACTOR_Destroy: // Actor is about to be destroyed
			return 1;
		case ACTOR_TextMsg: // Chat message
			return 1;
		case ACTOR_PreBeginPlay:
			{
			Actor.bSelected = 0;
			//
			for( int i=0; i<AActor::MAX_TOUCHING_ACTORS; i++ )
			{
				Actor.iTouchingActors[i]=INDEX_NONE;
			}
            FActor::Actor(Actor).InitializeAI();
            Actor.Age = 1;
                // Notes about age:
                // We start with an Age of 1. Age is incremented in the ARoot processing for Tick,
                // which is expected to be called after class-specific processing.
                // If this convention is followed, then class-specific Tick handlers will 
                // behave as follows:
                //  - during the first Tick, the age will be 1
                //  - subsequent Tick's will have the age incremented by 1
                //  - when the Tick reaches the LifeSpan (if LifeSpan != 0),
                //    the root class processing will make the actor die, *but*
                //    the class-specific handler will first get a Tick message
                //    with the actor's age equal to LifeSpan.
            {
                //todo: Find a cleaner way (and better place) to initialize the level descriptor!
                static BOOL FirstTime = TRUE; 
                if( Level->Descriptor != 0 && FirstTime )
                {
                    FirstTime = FALSE;
                    ALevelDescriptor & Descriptor = *Level->Descriptor;
                    FRandom::Initialize( Descriptor.RandomSeed );
                    Descriptor.RandomValue = FRandom::Value(0,32767); 
                    debugf
                    ( 
                        LOG_Init
                    ,   "Level seed: %i  value: %i"
                    ,   int(Descriptor.RandomSeed)
                    ,   int(Descriptor.RandomValue) 
                    );
                }
            }
	
			//
			// Kill this actor if its flags aren't appropriate for
			// this mode of play:
			//
			PBeginPlay *PlayInfo = (PBeginPlay *)Params;
			//
			BOOL DestroyIt =
				((PlayInfo->Difficulty==0  ) && !bDifficulty1   ) ||
				((PlayInfo->Difficulty==1  ) && !bDifficulty2   ) ||
				((PlayInfo->Difficulty==2  ) && !bDifficulty3   ) ||
				((PlayInfo->Difficulty==3  ) && !bDifficulty4   ) ||
				((PlayInfo->bNetCooperative) && !bNetCooperative) ||
				((PlayInfo->bNetDeathMatch ) && !bNetDeathMatch ) ||
				((PlayInfo->bNetPersistent ) && !bNetPersistent ) ||
				((PlayInfo->bNetNoMonsters ) && !bNetNoMonsters ) 
				;
            if( ChanceOfExistence > 0 && ChanceOfExistence < 100 )
            {
                // Randomly destroy object based on ChanceOfExistence:
                DestroyIt = DestroyIt || !FRandom::Percent(ChanceOfExistence);
            } 
            if( !DestroyIt && RandomizerDivider != 0 )
            {
                if( Level->Descriptor != 0 )
                {
                    // Randomly destroy object based on RandomizerDivider and RandomizerRemainder:
                    ALevelDescriptor & Descriptor = *Level->Descriptor;
                    DestroyIt = (Descriptor.RandomValue % RandomizerDivider) != RandomizerRemainder;
                }
            }
            if( DestroyIt )
            {
				Level->DestroyActor(iMe);
            }
			return 1;
			};
		case ACTOR_BeginPlay:
			{
			PBeginPlay *PlayInfo = (PBeginPlay *)Params;
            {
                // Update the level info with secrets, items, and so on...
                if( Level->Descriptor != 0 )
                {
                    ALevelDescriptor & Descriptor = *Level->Descriptor;
                    if( bIsSecretGoal )
                    {
                        Descriptor.SecretGoals++;
                    }
                    if( bIsItemGoal )
                    {
                        Descriptor.ItemGoals++;
                    }
                    if( bIsKillGoal )
                    {
                        Descriptor.KillGoals++;
                    }
		}
            }
	
			return 1;
			};
		case ACTOR_PostBeginPlay:
			{
			PBeginPlay *PlayInfo = (PBeginPlay *)Params;
			if(AmbientSound)
				{
				GAudio.PlaySfxLocated(&Location, AmbientSound, iMe, Actor.WorldSoundRadius() );
				};
			return 1;
			};
		case ACTOR_EndPlay:
			return 1;
		case ACTOR_Target:
			return 1;
		case ACTOR_UnTarget:
			return 1;
		case ACTOR_Possess:
			return 1;
		case ACTOR_UnPossess:
			return 1;
		case ACTOR_KillCredit:
        {
            // Since the actor has declined to handle the credit, let's pass it
            // on to the parent.
			if( iParent != INDEX_NONE )
            {
                return Level->SendMessage( iParent, ACTOR_KillCredit, Params );
            }
			return 1;
        }
	
		case ACTOR_Die:
			Level->DestroyActor(iMe);
			return 1;
		case ACTOR_DieProjHit:
		case ACTOR_DieMeleeHit:
		case ACTOR_DieInstantHit:
		case ACTOR_DieFloorHit:
		case ACTOR_DieMoverHit:
			// Reroute message to original class's generic ACTOR_DIE handler
			return Level->SendMessage(iMe,ACTOR_Die,Params);;
		case ACTOR_Hit:
        {
            PHit & Info = PHit::Convert(Params);
            if( Info.Momentum != 0 && Info.iSourceActor != INDEX_NONE )
            {
                FActor & Source = FActor::Actor(Info.iSourceActor);
                Actor.GainMomentum( Info.Momentum, Actor.Location - Source.Location );
            }
			return 1;
        }
		case ACTOR_PlayerCalcView:
			((PCalcView *)Params)->ViewLocation = Location;
			((PCalcView *)Params)->ViewRotation = ViewRot;
			return 1;
		case ACTOR_PickupQuery:
			// A pickup is asking a pawn whether it wants to pick up a certain inventory class.
			// Default implementation is to return -1 to ignore the pickup.
			return -1;
		case ACTOR_PreemptPickupQuery:
			// A pawn is asking this inventory actor whether it wants to preempt a pickup.
			// Return 1 to preempt it, -1 to allow it.
			return -1;
		case ACTOR_PickupNotify:
			// We're being notified that we just picked up an actor
			return 1;
		case ACTOR_AddInventory:
			{
			// Add an inventory actor to this pawn's inventory. Returns 1 if added.
			// If the actor chooses not to add the thing, it must kill the actor passed to it
			// and return -1.
			PActor *InvInfo   = &PActor::Convert(Params);
			INDEX  iInvActor  = InvInfo->iActor;
			AActor *InvActor  = (AActor *)&Level->Actors->Element(iInvActor);
			//
			// Make sure we only use Inventory class descendents:
			//
			if (InvActor->IsKindOf("Inventory"))
				{
				//
				// Link the inventory actor into the top of our inventory chain:
				//
				InvActor->iParent    = iMe;
				InvActor->iInventory = iInventory;
				InvActor->iInstigator = iMe;
				iInventory           = iInvActor;
				};
			return 1;
			};
		case ACTOR_DeleteInventory:
			{
			//
			// Delete an actor from inventory.  Returns 1 if the actor existed and was
			// deleted, or -1 if the actor doesn't exist in our inventory.
			//
			PActor *InvInfo   = &PActor::Convert(Params);
			INDEX  iInvActor  = InvInfo->iActor;
			//
			INDEX *PrevLink  = &iInventory;
			INDEX iTempActor = iInventory;
			//
			while (iTempActor != INDEX_NONE)
				{
				AActor *TempActor = &Level->Actors->Element(iTempActor);
				if (iTempActor == iInvActor)
					{
					// Found this actor in the inventory chain; delink it:
					*PrevLink             = TempActor->iInventory;
					TempActor->iInventory = INDEX_NONE;
					Level->DestroyActor(iTempActor);
					return 1; // Found and deleted actor
					};
				PrevLink   = &TempActor->iInventory;
				iTempActor = TempActor->iInventory;
				};
			return -1; // Didn't find actor
			};
		case ACTOR_Activate:
			// Activate this inventory actor
			return 1;
		case ACTOR_DeActivate:
			// Deactivate this inventory actor
			return 1;
		case ACTOR_Use:
			// Use this inventory actor
			return 1;
		case ACTOR_UseExtra:
			// If UseExtra wasn't handled, route to regular Use handler
			return Level->SendMessage(iMe,ACTOR_Use,Params);
		case ACTOR_ZoneChange:
			//bug ("ZoneChange in %s",Class->Name);
			return 1;
		case ACTOR_Exec:
			{
			// Execute some text command.
			// Default action is to ignore it.
			PExec *ExecInfo = (PExec *)Params;
			return 1;
			};
		case ACTOR_Touch:
            {
            PTouch & Info = PTouch::Convert(Params);
            const INDEX iToucher = Info.iActor;
            #if 0 //todo: Add or delete touching momentum - whatever we decide
            if( bMomentum && Mass != 0 && iToucher != INDEX_NONE )
            {
                AActor & Toucher = FActor::Root(iToucher);
                if( Toucher.bMomentum && Toucher.Mass != 0 )
                {
                    // Let's heuristically alter the velocities of both the toucher
                    // and touched object. The touched object will have a fraction
                    // of the touching object's velocity added (scaled by mass).
                    AActor & Touched = *this;
                    const FVector Momentum = Touched.Mass*Touched.Velocity + Toucher.Mass*Toucher.Velocity;
                    FVector TouchedVelocityChange = Toucher.Velocity;
                    TouchedVelocityChange *= Toucher.Mass / Touched.Mass / 5.0;
                    Touched.Velocity += TouchedVelocityChange;
                    // Conservation of momentum will give us the toucher's new velocity:
                    Toucher.Velocity = Momentum - Touched.Mass*Touched.Velocity;
                    Toucher.Velocity /= Toucher.Mass;
                }
            }
            #endif
            return ProcessDone;
            break;
            }
		case ACTOR_SetParent:
			{
			// Sets this actors parent to the specified actor
			if (iParent!=INDEX_NONE) Level->SendMessage(iParent,ACTOR_LostChild,&iMe);
			iParent = *(INDEX *)Params;
			if (iParent!=INDEX_NONE) Level->SendMessage(iParent,ACTOR_GainedChild,&iMe);
			//
			return 1;
			};
		case ACTOR_GainedChild:
			{
			// Sent to a parent actor when another actor attaches to it
			return 1;
			};
		case ACTOR_LostChild:
			{
			// Sent to a parent actor when another actor detaches from it
			return 1;
			};
		case ACTOR_QueryChildActors:
			{
			// Causes kernel to call QueryActorCallback for all child actors
			PTouch *Touch = (PTouch*)Params;
			AActor *Actor = &Level->Actors->Element(0);
			for (INDEX iActor=0; iActor<Level->Actors->Max; iActor++)
				{
				if (Actor->Class && (Actor->iParent==iMe))
					{
					Level->SendMessage(Touch->iActor,ACTOR_QueryCallback,&iActor);
					};
				Actor++;
				};
			return 1;
			};
		case ACTOR_QueryEventActors:
			{
			// Causes kernel to call QueryActorCallback for all actors using an event
			AActor *Actor = &Level->Actors->Element(0);
			PName  *Info  = (PName*)Params;
			for (INDEX iActor=0; iActor<Level->Actors->Max; iActor++)
				{
				if (Actor->Class && (Actor->Event==Info->Name))
					{
					Level->SendMessage(Info->iActor,ACTOR_QueryCallback,&iActor);
					};
				Actor++;
				};
			return 1;
			};
		case ACTOR_QueryNameActors:
			{
			// Causes kernel to call QueryActorCallback for all actors using a name
			AActor *Actor = &Level->Actors->Element(0);
			PName  *Info  = (PName*)Params;
			for (INDEX iActor=0; iActor<Level->Actors->Max; iActor++)
				{
				if (Actor->Class && (Actor->Name==Info->Name))
					{
					Level->SendMessage(Info->iActor,ACTOR_QueryCallback,&iActor);
					};
				Actor++;
				};
			return 1;
			};
		case ACTOR_QueryCallback:
			{
			// Called by kernel in response to query messages
			return 1;
			};
		case ACTOR_RestartLevel:
			{
            debugf( LOG_Info, "Restart level is not yet working" );
            GRestartLevelAfterTick = TRUE;
            return 1;
            }
		case ACTOR_GoToLevel:
			{
            const PLevel & Info = PLevel::Convert(Params);
            debugf( LOG_Info, "Go to level [%s] is not yet working", Info.Name );
            strcpy( GJumpToLevelAfterTick, Info.Name );
            return 1;
            }
        //--------------------------------------------------------------------
        //                    Teleportation
        //--------------------------------------------------------------------
        case ACTOR_PreTeleport:
        {
            PTouch & Info = PTouch::Convert(Params);
            if( bCanBeTeleported && TeleportDelay == 0 && iPendingTeleporter == INDEX_NONE )
            {
                iPendingTeleporter = Info.iActor;
            }
            return 1; // This means to accept the teleportation. -1 means to reject it.
            break;
        }
        case ACTOR_PostTeleport:
        {
            PTouch & Info = PTouch::Convert(Params);
            TeleportDelay = 3; // Delay until the next allowed teleportation
            return ProcessParent; 
            break;
        }
		};
	return 0;
	UNGUARD("AActor::Process");
	};

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
