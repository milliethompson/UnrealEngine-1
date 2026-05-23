/*=============================================================================
	UnMover.cpp: Keyframe mover actor code

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnGame.h"

/*-----------------------------------------------------------------------------
	Mover base class.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(AMover);
void AMover::Process( FName Message, PMessageParms *Parms )
{
	guard(AMover::Process);
	switch( Message.GetIndex() )
	{
		case NAME_BeginPlay:
		{
			PrevKeyNum = KeyNum = Clamp(KeyNum,(BYTE)0,(BYTE)3);

			bMoving    = 0;
			CurTime    = 0;
			HoldTime   = 0;

			Location = BasePos + KeyPos[KeyNum];
			Rotation = BaseRot + KeyRot[KeyNum];

			// Turn off actor collision checking if this mover doesn't need it.
			if
			(	(MoverTriggerType!=MT_ProximityOpenTimed)
			&&	(MoverTriggerType!=MT_ProximityControl)
			&&	(!bTrigger) )
			{
				SetCollision( 1, bBlockActors, bBlockPlayers );
			}
			break;
		}
		case NAME_KeyMoveTo:
		{
			if( !bMoving )
			{
				// Start moving.
				PrevKeyNum		 = KeyNum;
				KeyNum			 = PKeyMove::Get(Parms)->KeyNum;
				bMoving			 = 1;
				CurTime			 = 0;
				HoldTime		 = 0;
				bReverseWhenDone = 0;
				MakeSound( OpenSound );
                MakeSound( MoveAmbientSound );
			}
			else if( bInterruptable )
			{
				// Reverse course smoothly.
				Process( NAME_KeyReverse, NULL );
				bReverseWhenDone = 0;
			}
			else bReverseWhenDone ^= 1;
			break;
		}
		case NAME_KeyStop:
		{
			bMoving	= 0;
			break;
		}
		case NAME_KeyReverse:
		{
			if( bMoving )
			{
				BYTE Temp	= KeyNum;
				KeyNum		= PrevKeyNum;
				PrevKeyNum	= Temp;
				CurTime		= MoveTime - CurTime;
			}
			break;
		}
		case NAME_SetKeyPoint:
		{
			//todo: Implement.
			break;
		}
		case NAME_Tick:
		{
			PTick *Tick = PTick::Get(Parms);
			Rotation += FreeRotation;
			if( bMoving )
			{
				// We are moving.
				if( (CurTime+=Tick->DeltaTime) <= MoveTime )
				{
					// Continue moving.
					FLOAT Alpha = CurTime / MoveTime;
					if( MoverGlideType == MV_GlideByTime )
					{
						// Make alpha time-smooth and time-continuous
						Alpha = 3.0*Alpha*Alpha - 2.0*Alpha*Alpha*Alpha;
					}
					Location = BasePos + KeyPos[PrevKeyNum] + (KeyPos[KeyNum]-KeyPos[PrevKeyNum]) * Alpha;
					Rotation = BaseRot + KeyRot[PrevKeyNum] + (KeyRot[KeyNum]-KeyRot[PrevKeyNum]) * Alpha;
				}
				else
				{
					// Just finished moving.
					bMoving  = 0;
					CurTime  = 0;

					Location = BasePos + KeyPos[KeyNum];
					Rotation = BaseRot + KeyRot[KeyNum];

					if( bReverseWhenDone )
					{
						Process( NAME_KeyMoveTo, &PKeyMove(KeyNum ? 0 : 1) );
					}
                    else
                    {
						// Stop any current sound.
                        MakeSound(ClosedSound);
                    }
				}
			}
			else
			{
				// Not moving.
				switch( MoverTriggerType )
				{
					case MT_TriggerOpenTimed:
					case MT_ProximityOpenTimed:
					case MT_StandOpenTimed:
					{
						// If open and timer is active, wait till timer expires then close.
						if( (KeyNum==1) && (HoldTime!=0) )
						{
							if( (HoldTime+=Tick->DeltaTime) >= StayOpenTime )
							{
								// Time expired, now close.
								Process( NAME_KeyMoveTo, &PKeyMove(0) );
							}
						}
						break;
					}
				}
			}
			if( Brush && (Brush->Location!=Location || Brush->Rotation!=Rotation) )
			{
				for( int i=0; i<ARRAY_COUNT(Slaves); i++ )
				{
					if( Slaves[i] )
					{
						AActor &Slave = *Slaves[i];

						// Adjust for center of rotation.
						FRotation AdjustRot;
						FVector Out;
						FCoords Coords;

						AdjustRot = FRotation(0,MaskAngle(Rotation.Yaw)-MaskAngle(Brush->Rotation.Yaw),0);
						Coords = GMath.UnitCoords * AdjustRot;
						Slave.Location = Location + (Slave.Location - Brush->Location).TransformVectorBy(Coords);

						// Adjust for simple rotation.
						Slave.Rotation.Yaw += Rotation.Yaw - Brush->Rotation.Yaw;
						Slave.Rotation.Yaw += Rotation.Yaw - Brush->Rotation.Yaw;

						// Update pawn.
						if( Slave.IsA("Pawn"))
						{
							((APawn &)Slave).ViewRotation.Yaw += Rotation.Yaw - Brush->Rotation.Yaw;
						}
					}
				}
				GBrushTracker.Update( this );
			}
			break;
		}
		case NAME_Trigger:
		{
			switch (MoverTriggerType)
			{
				case MT_TriggerOpenTimed:
				case MT_TriggerControl:
				{
					// Open it.
					Process( NAME_KeyMoveTo, &PKeyMove(1) );
					break;
				}
				case MT_TriggerToggle:
				{
					// Toggle it.
					Process( NAME_KeyMoveTo, &PKeyMove(KeyNum ? 0 : 1) );
					break;
				}
			}
			break;
		}
		case NAME_UnTrigger:
		{
			switch( MoverTriggerType )
			{
				case MT_TriggerOpenTimed:
				{
					// Start timer ticking down.
					if( !HoldTime ) HoldTime=1;
					break;
				}
				case MT_TriggerToggle:
				{
					// Ignore untrigger.
					break;
				}
				case MT_TriggerControl:
				{
					// Close it.
					Process( NAME_KeyMoveTo, &PKeyMove(0) );
					break;
				}
			}
			break;
		}
		case NAME_Touch:
		{
			if( BumpPlayerDamage != 0.0 )
			{
				//todo: Damange the toucher.
			}
			if( bTrigger && !bMoving && KeyNum==0 && Event!=NAME_None )
			{
				// Trigger other events.
				GetLevel()->SendEx( NAME_Trigger, Parms, NULL, Event, NULL );
				if( bTriggerOnceOnly )
				{
					MoverTriggerType = MT_None;
					bTrigger = 0;
				}
			}
			switch( MoverTriggerType )
			{
				case MT_ProximityOpenTimed:
				case MT_ProximityControl:
				{
					// Open it.
					Process( NAME_KeyMoveTo, &PKeyMove(1) );
					break;
				}
			}
			break;
		}
		case NAME_UnTouch:
		{
			if( bTrigger && Event!=NAME_None )
			{
				// Trigger other events.
				GetLevel()->SendEx( NAME_UnTrigger, &PActor(this), NULL, Event, NULL );
			}
			switch( MoverTriggerType )
			{
				case MT_ProximityOpenTimed:
				{
					// Start timer ticking down.
					if (!HoldTime) HoldTime=1;
					break;
				}
				case MT_ProximityControl:
				{
					// Close it.
					Process( NAME_KeyMoveTo, &PKeyMove(0) );
					break;
				}
			}
			break;
		}
		case NAME_Spawned:
		{
			BasePos		= Location;
			BaseRot		= Rotation;
			PrevKeyNum	= KeyNum;
			break;
		}
		case NAME_PostEditChange:
		{
			// Validate KeyNum.
			if( KeyNum>=4 ) KeyNum=3;

			// Update BasePos.
			if( IsMovingBrush() )
			{
				BasePos  = Brush->Location - OldPos;
				BaseRot  = Brush->Rotation - OldRot;
			}
			else
			{
				BasePos  = Location - OldPos;
				BaseRot  = Rotation - OldRot;
			}
			
			// Update Old.
			OldPos = KeyPos[KeyNum];
			OldRot = KeyRot[KeyNum];
			
			// Update Location.
			Location = BasePos + OldPos;
			Rotation = BaseRot + OldRot;
			
			// Update PrevKeyNum.
			PrevKeyNum = KeyNum;

			UpdateBrushPosition(GetLevel(),0);
			break;
		}
		case NAME_PostEditMove:
		{
			if( KeyNum == 0 )
			{
				// Changing location.
				if( IsMovingBrush() )
				{
					BasePos  = Brush->Location - OldPos;
					BaseRot  = Brush->Rotation - OldRot;
				}
				else
				{
					BasePos  = Location - OldPos;
					BaseRot  = Rotation - OldRot;
				}
			}
			else
			{
				// Changing displacement of KeyPos[KeyNum] relative to KeyPos[0].
				if( IsMovingBrush() )
				{
					// Update Key:
					KeyPos[KeyNum] = Brush->Location - (BasePos + KeyPos[0]);
					KeyRot[KeyNum] = Brush->Rotation - (BaseRot + KeyRot[0]);
				}
				else
				{
					// Update Key:
					KeyPos[KeyNum] = Location - (BasePos + KeyPos[0]);
					KeyRot[KeyNum] = Rotation - (BaseRot + KeyRot[0]);
				}
				// Update Old:
				OldPos = KeyPos[KeyNum];
				OldRot = KeyRot[KeyNum];
			}
			Brush->Location = BasePos + KeyPos[KeyNum];
			break;
		}
		case NAME_SteppedOnBy:
		{
			AActor *Actor = PActor::Get(Parms)->Actor;
			if( Actor )
			{
				for( int i=0; i<ARRAY_COUNT(Slaves); i++ )
				{
					if( Slaves[i] == Actor )
						break;
				}
				if( i>=ARRAY_COUNT(Slaves) )
				{
					for( i=0; i<ARRAY_COUNT(Slaves); i++ )
					{
						if( Slaves[i]==NULL )
						{
							Slaves[i] = Actor;
							break;
						}
					}
				}
				switch( MoverTriggerType )
				{
					case MT_StandOpenTimed:
					{
						// Open it.
						Process( NAME_KeyMoveTo, &PKeyMove(1) );
						break;
					}
				}
			}
			break;
		}
		case NAME_SteppedOffBy:
		{
			AActor *Actor = PActor::Get(Parms)->Actor;
			if( Actor )
			{
				for( int i=0; i<16; i++ )
				{
					if( Slaves[i] == Actor )
						Slaves[i] = NULL;
				}
				switch( MoverTriggerType )
				{
					case MT_StandOpenTimed:
					{
						// Start timer ticking down
						if( !HoldTime )
							HoldTime=1;
						break;
					}
				}
			}
			break;
		}
		case NAME_PreRaytrace:  // Called before raytracing session beings.
		case NAME_PostRaytrace: // Called after raytracing session ends.
		{
			Location = BasePos + KeyPos[KeyNum];
			Rotation = BaseRot + KeyRot[KeyNum];
			GBrushTracker.Update( this );
			break;
		}
		case NAME_RaytraceWorld: // Place this brush in position to raytrace the world.
		{
			if( WorldRaytraceKey!=255 )
			{
				Location = BasePos + KeyPos[WorldRaytraceKey];
				Rotation = BaseRot + KeyRot[WorldRaytraceKey];
				GBrushTracker.Update( this );
			}
			else GBrushTracker.Flush( this );
			break;
		}
		case NAME_RaytraceBrush:
		{
			// Place this brush in position to raytrace the brush.
			if( BrushRaytraceKey != 255 )
			{
				Location = BasePos + KeyPos[BrushRaytraceKey];
				Rotation = BaseRot + KeyRot[BrushRaytraceKey];
				GBrushTracker.Update( this );
				PBoolean::Get(Parms)->bBoolean = 1;
			}
			else PBoolean::Get(Parms)->bBoolean = 0;
			break;
		}
		default:
		{
			ABrush::Process( Message, Parms );
			break;
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
