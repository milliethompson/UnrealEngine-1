/*=============================================================================
	UnLevTic.cpp: Level timer tick function

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * July 21, 1996: Mark added GLevel
        * Aug  31, 1996: Mark added GRestartLevelAfterTick
        * Aug  31, 1996: Mark added GJumpToLevelAfterTick
=============================================================================*/

#include "Unreal.h"
#include "UnDynBsp.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

// Global level pointer, available when level is locked.
ILevel * GLevel = 0;

// Set TRUE to cause level to be restarted at next end of "tick" processing.
BOOL GRestartLevelAfterTick = FALSE;

// Set to a level name to go to at next end of "tick" processing.
char GJumpToLevelAfterTick[64] = "";

/*-----------------------------------------------------------------------------
	Main level timer tick handler.
-----------------------------------------------------------------------------*/

//
// Update the level after timer ticks have passed.  TicksPassed is usually 1,
// and will only be higher if the server is too slow and starts getting behind
// the 35 fps base frame rate.
//
// All child actors are ticked after their parents have been ticked.
//
void ULevel::Tick(int CamerasOnly, INDEX iActiveLocalPlayer)
{
	guard(ULevel::Tick);

	ILevel		Level;
	PPlayerTick	MovementPacket,NoMovementPacket;
	BYTE		*Ticked = (BYTE *)GMem.GetZeroed(ActorList->Max * sizeof(BYTE));
	int			NumUpdated,NumSkipped,NumIter=0;

	clock(GServer.LevelTickTime);

	// Null movement packet.
	NoMovementPacket.BuildAllMovement(NULL);

	// Lock everything we need.
	Lock(&Level,LOCK_NoTrans);

	if (!CamerasOnly)
		Level.Dynamics.AddAllActors(&Level);

	// Go through actor list, updating actors who either have no parent, or whose
	// parent has been updated.  The result is that parent actors are always updated
	// before their children.

	clock(GServer.ActorTickTime);
    UActorList::CompactList & CompactList = *Level.Actors->DynamicActors;
	do
	{
		NumUpdated = 0;
		NumSkipped = 0;

    	for( int Which = 0; Which < CompactList.Count(); Which++)
		{
            AActor * const Actor = CompactList[Which];
			
			if( Actor != 0 && !Ticked[Actor->iMe] )
			{
				if( Actor->iParent==INDEX_NONE || Ticked[Actor->iParent] )
				{
					int IsPawn = Actor->IsKindOf("Pawn");

					// This actor should be ticked.
					if( IsPawn && ((APawn*)Actor)->Camera )
					{
						// This is a player.
						APawn *Pawn = (APawn*)Actor;
						if( !(Pawn->ShowFlags & SHOW_PlayerCtrl) )
							goto Skip;

						// Camera: Add keystrokes/mouse from local input.
						if( Actor->iMe==iActiveLocalPlayer && Pawn->Camera->Current )
						{
							// This player is the active local player, so we update him based
							// on his input.

							//todo: Delete (once pause is working in both game and editor.
                            if( FALSE && GEditor )
                            {
                                // Update status - done here only when running the editor
                                // since the editor does not call the server tick function.
                                Pawn->Camera->Console->UpdateActionStatus();
                            }
							GAudio.SetOrigin(&Actor->Location, &Actor->ViewRot);
							MovementPacket.BuildAllMovement(Pawn->Camera);
							Level.SendMessage(Actor->iMe,ACTOR_PlayerTick,&MovementPacket);
						}
						else
						{
							// This player is an inactive local player, so we update him
							// with an empty movement packet.
							Level.SendMessage(Actor->iMe,ACTOR_PlayerTick,&NoMovementPacket);
						}
					}
					else if( IsPawn && 0 /* ActorIsARemoteNetworkPlayer() */ )
					{
						// This is a remote network player.  He may have one or more movement
						// packets coming in from the network.  Here we should merge thim into
						// one input packet and tick him:
						//
						//FetchTheRemotePlayersInputPacketsFromTheIncomingStream();
						//Level.SendMessage(Actor->iMe,ACTOR_Tick,FetchedNetworkMovementPacket);
						//
						// Sending the network player response packets containing what he sees
						// happens elsewhere, just as rendering the local players' camera views
						// happens elsewhere.
					}
					else
					{
						if (CamerasOnly) 
							goto Skip;

						// This is a nonplayer.
						Level.SendMessage(Actor->iMe,ACTOR_Tick,NULL);
					}
                    Ticked[Actor->iMe] = TRUE;
					NumUpdated++;
				}
				else
				{
					// Skip this actor.
					Skip:
					NumSkipped++;
				}
			}
		}
		NumIter++;
	} while (NumUpdated && NumSkipped);
	
	unclock(GServer.ActorTickTime);

	// Unlock everything.
	Unlock(&Level);
	GMem.Release(Ticked);

	unclock(GServer.LevelTickTime);
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
