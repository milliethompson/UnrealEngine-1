/*=============================================================================
	UnLevTic.cpp: Level timer tick function

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * July 21, 1996: Mark added GLevel
        * Aug  31, 1996: Mark added GRestartLevelAfterTick
        * Aug  31, 1996: Mark added GJumpToLevelAfterTick
		* Dec  13, 1996: Tim removed GLevel.
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	Main level timer tick handler.
-----------------------------------------------------------------------------*/

//
// Update the level after a variable amount of time, DeltaSeconds, has passed.
// All child actors are ticked after their owners have been ticked.
//
void ULevel::Tick( int CamerasOnly, AActor *ActiveLocalPlayer, FLOAT DeltaSeconds )
{
	guard(ULevel::Tick);
	FMemMark Mark(GMem);
	clock(GServer.LevelTickTime);

	// Lock everything we need.
	Lock(LOCK_ReadWrite);

	// Here we alternate between bTicked and !bTicked to avoid a cache-costly clear.
	DWORD NotTicked = Element(0)->bTicked, Ticked=!NotTicked;
	INT NumUpdated,NumSkipped,NumIter=0,UpdatePlayers=0;

	// Update timing.
	INT UnusedDayOfWeek;
	GApp->SystemTime(&Info->Year,&Info->Month,&UnusedDayOfWeek,
		&Info->Day,&Info->Hour,&Info->Minute,&Info->Second,
		&Info->Millisecond);
	Info->TimeSeconds = GServer.TimeSeconds;
	Info->TimeDays =
	(
		Info->Hour/24.0 + 
		Info->Minute/60.0/24.0 + 
		Info->Second/60.0/60.0/24.0 + 
		Info->Millisecond/1000.0/60.0/60.0/24.0
	) * Info->DayFactor + Info->DayBase;
	Info->TimeDays -= (int)Info->TimeDays;

	// Compute DayFraction and NightFraction.
	int Base				= Info->Minute + Info->Hour*60;
	int NoonDistance		= Abs(Base - 12*60);
	int MidnightDistance	= (Base < 12*60) ? Base : (24*60 - Base);
	Info->NightFraction		= (MidnightDistance > 9*60) ? 0.0 : Square(MidnightDistance / (9.0 * 60.0));
	Info->DayFraction		= (NoonDistance     > 9*60) ? 0.0 : Square(NoonDistance     / (9.0 * 60.0));

	// Make time delta reasonable, to prevent absurd physics. This slows the game down if the frame
	// rate drops below 2.5 fps.
	DeltaSeconds = Min(DeltaSeconds,0.40f);

	// If caller wants time update only, skip the rest.
	if( CamerasOnly == 2 )
		goto SkipUpdate;
	
	// Update collision.
	Hash.Tick();

	// Go through actor list, updating actors who either have no parent, or whose
	// parent has been updated.  The result is that parent actors are always updated
	// before their children.
	clock(GServer.ActorTickTime);
	for( UpdatePlayers=0; UpdatePlayers<2; UpdatePlayers++ ) do
	{
		NumUpdated = 0;
		NumSkipped = 0;

		for( INDEX iActor=0; iActor<Num; iActor++ )
		{
			AActor *Actor = Element(iActor);
			if
			(	Actor
			&& !Actor->bStatic
			&& !Actor->bDeleteMe
			&& Actor->bTicked==NotTicked )
			{
				// See if this is a pawn.
				APawn *Pawn = Actor->IsA("Pawn") ? (APawn*)Actor : NULL;

				// Skip this actor if updating is inappropriate.
				if( Actor->Owner )
				{
					// Skip if owned by an actor which hasn't been updated yet.
					if( Actor->Owner->bTicked==NotTicked )
						continue;
				}
				else
				{
					// Update all players after all other actors.
					if( UpdatePlayers ^ (Pawn && Pawn->Camera) )
						continue;
				}

				// Update all animation, including multiple passes if necessary.
				INT Iterations = 0;
				FLOAT Seconds  = DeltaSeconds;
				while( Actor->AnimSequence!=NAME_None && Actor->AnimRate!=0.0 && Seconds!=0.0 && ++Iterations < 32 )
				{
					// Remember the old frame.
					FLOAT OldAnimFrame = Actor->AnimFrame;

					// Update animation, and possibly overflow it.
					if( Actor->AnimRate >= 0.0 )
					{
						// Update regular animation.
						Actor->AnimFrame += Actor->AnimRate * Seconds;
					}
					else
					{
						// Update velocity-scaled animation.
						Actor->AnimFrame += ::Max( Actor->AnimMinRate, Actor->Velocity.Size() * -Actor->AnimRate ) * Seconds;
					}

					// Handle all animation sequence notifys.
					if( Actor->Mesh && Actor->bAnimNotify )
					{
						FMeshAnimSeq* Seq = Actor->Mesh->GetAnimSeq( Actor->AnimSequence );
						if( Seq )
						{
							UMeshAnimNotifys::Ptr Notifys = Actor->Mesh->Notifys;
							for( int i=Seq->StartNotify; i<Seq->StartNotify+Seq->NumNotifys; i++ )
							{
								FMeshAnimNotify &Notify = Notifys(i);
								if( OldAnimFrame<Notify.Time && Actor->AnimFrame>=Notify.Time )
								{
									Seconds          = Seconds * (Actor->AnimFrame - Notify.Time) / (Actor->AnimFrame - OldAnimFrame);
									Actor->AnimFrame = Notify.Time;
									Actor->Process( Notifys(i).Function, NULL );
									continue;
								}
							}
						}
					}

					// Handle end of animation sequence.
					if( Actor->AnimFrame<Actor->AnimEnd || (Actor->bAnimLoop && OldAnimFrame>=Actor->AnimEnd && Actor->AnimFrame<1.0) )
					{
						// We have finished the animation updating for this tick.
						Seconds = 0.0;
					}
					else if( Actor->bAnimLoop && OldAnimFrame<1.0 && Actor->AnimFrame>1.0 )
					{
						// Just past loop end, so loop it.
						Seconds          = Seconds * (Actor->AnimFrame - 1.0) / (Actor->AnimFrame - OldAnimFrame);
						Actor->AnimFrame = 0.0;
					}
					else if( OldAnimFrame<Actor->AnimEnd )
					{
						// Just passed end-minus-one frame.
						Seconds = Seconds * (Actor->AnimFrame - Actor->AnimEnd) / (Actor->AnimFrame - OldAnimFrame);
						if( !Actor->bAnimLoop )
						{
							// End the one-shot animation.
							Actor->AnimFrame	 = Actor->AnimEnd;
							Actor->bAnimFinished = 1;
							Actor->AnimRate      = 0.0;
						}
						Actor->Process( NAME_AnimEnd, NULL );
					}
				}

				// This actor is tickable.
				FLOAT ThisDeltaSeconds = DeltaSeconds;
				if( Actor->TickRate>=0.0 && (Actor->TickCounter+=DeltaSeconds)>=Actor->TickRate )
				{
					if( Actor->TickRate != 0.0 )
					{
						// Adjust the tick count.
						ThisDeltaSeconds   = Actor->TickCounter;
						Actor->TickCounter = 0;
					}

					// Tick the actor.
					if( Pawn && Pawn->Camera )
					{
						// This is a player.
						if( !(Pawn->ShowFlags & SHOW_PlayerCtrl) )
							goto Skip;

							// Camera: Add keystrokes/mouse from local input.
						if( Actor==ActiveLocalPlayer && Pawn->Camera->Current )
						{
							// This player is the active local player, so we update him based
							// on his input.
							PPlayerTick PlayerTick( ThisDeltaSeconds );
							Pawn->Camera->ReadInput( PlayerTick, DeltaSeconds, Pawn->Camera );
							Pawn->inputCopyFrom( PlayerTick );
							Actor->Process( NAME_PlayerTick, &PlayerTick );
							GAudio.SetOrigin( &Actor->Location, &Pawn->ViewRotation );

							//if (!Actor->IsProbing(NAME_PlayerTick) //then do player control here
							//	Pawn.PlayerControl();
						}
						else
						{
							// This player is an inactive local player, so we update him
							// with an empty movement packet.
							PPlayerTick PlayerTick( ThisDeltaSeconds );
							Pawn->inputCopyFrom( PlayerTick );
							Actor->Process( NAME_PlayerTick, &PlayerTick );

						}
					}
					else if( Pawn && 0 /* ActorIsARemoteNetworkPlayer() */ )
					{
						// This is a remote network player.  He may have one or more movement
						// packets coming in from the network.  Here we should merge thim into
						// one input packet and tick him:
						//
						//FetchTheRemotePlayersInputPacketsFromTheIncomingStream();
						//RemoteMovementPacket.ThisDeltaTime = Info->GameTickRate;
						//SendMessage(iActor,NAME_PlayerTick,RemoteMovementPacket);
						//
						// Sending the network player response packets containing what he sees
						// happens elsewhere, just as rendering the local players' camera views
						// happens elsewhere.
					}
					else
					{
						// If only updating cameras, skip nonplayers.
						if( CamerasOnly )
							goto Skip;

						// Tick the nonplayer.
						PTick Tick( ThisDeltaSeconds );
						Actor->Process( NAME_Tick, &Tick );
					}

					// If actor destroyed itself, stop processing it.
					if( Actor->bDeleteMe ) continue;

					// Update the actor's script state code.
					guard(StateExec);
					debugState(Actor->MainStack.Object == Actor);

					// Create a work area for UnrealScript.
					BYTE Buffer[MAX_CONST_SIZE], *Addr;
					*(FLOAT*)Buffer = ThisDeltaSeconds;

					// If a latent action is in progress, update it.
					if( Actor->MainStack.Code && Actor->LatentAction )
					{
						(*GIntrinsics[Actor->LatentAction])( Actor->MainStack, NULL, Addr=Buffer );
						if( Actor->bDeleteMe )
							continue;
					}

					// Execute code.
					while( Actor->MainStack.Code && !Actor->LatentAction )
						(*GIntrinsics[*Actor->MainStack.Code++])( Actor->MainStack, Actor, Addr=Buffer );
					if( Actor->bDeleteMe )
						continue;
					unguard;

					// Update the actor's audio.
					Actor->UpdateSound();
				}

				// Update timers.
				if( Actor->TimerRate>0.0 && (Actor->TimerCounter+=DeltaSeconds)>=Actor->TimerRate )
				{
					// Normalize the timer count.
					int TimerTicksPassed = 1;
					if( Actor->TimerRate > 0.0 )
					{
						TimerTicksPassed     = (int)(Actor->TimerCounter/Actor->TimerRate);
						Actor->TimerCounter -= Actor->TimerRate * TimerTicksPassed;
						if( TimerTicksPassed && !Actor->bTimerLoop )
						{
							// Only want a one-shot timer message.
							TimerTicksPassed = 1;
							Actor->TimerRate = 0.0;
						}
					}

					// Call timer routine with count of timer events that have passed.
					Actor->Process( NAME_Timer, &PInt(TimerTicksPassed) );
					if( Actor->bDeleteMe )
						continue;
				}

				// Update LifeSpan.
				if( Actor->GetClass() && Actor->LifeSpan!=0.0 )
				{
					if( (Actor->LifeSpan -= DeltaSeconds) <= 0.0 )
					{
						// Actor's LifeSpan expired.
						Actor->Process( NAME_Expired, NULL );
						if( !Actor->bDeleteMe )
							DestroyActor( Actor );
						continue;
					}
				}
	
				// Perform physics.
				// Save old location for AI purposes.
				Actor->OldLocation = Actor->Location;

				if ( (Actor->Physics!=PHYS_None) && Actor->bEnginePhysics)
					Actor->performPhysics(DeltaSeconds);

				// update eyeheight and send visibility updates
				if (Pawn)
				{
					if (Pawn->SightCounter < 0.0)
						Pawn->SightCounter = 0.1; //FIXME - make as big as possible 
					Pawn->SightCounter = Pawn->SightCounter - DeltaSeconds; 
					if (Pawn->bIsPlayer)
					{
						Actor->Process( NAME_UpdateEyeHeight, &PFloat(DeltaSeconds) );
						Pawn->ShowSelf();
					}
					if ((Pawn->SightCounter < 0.0) && Pawn->IsProbing(NAME_EnemyNotVisible))
						Pawn->CheckEnemyVisible();
				}

                Actor->bTicked = Ticked;
				NumUpdated++;
			}
			else
			{
				// Skip this actor.
				Skip:
				NumSkipped++;
			}
		}
		NumIter++;
	} while( NumUpdated && NumSkipped );

	unclock(GServer.ActorTickTime);

	// Unlock everything.
	SkipUpdate:
	Unlock(LOCK_ReadWrite);

	Mark.Pop();
	unclock(GServer.LevelTickTime);
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
