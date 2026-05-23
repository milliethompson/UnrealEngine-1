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
	Mover processing function.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(AMover);
void AMover::Process( FName Message, PMessageParms *Parms )
{
	guard(AMover::Process);
	switch( Message.GetIndex() )
	{
		case NAME_PostEditChange:
		{
			// Validate KeyNum.
			KeyNum = Clamp( (INT)KeyNum, (INT)0, (INT)ARRAY_COUNT(KeyPos) );

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

			UpdateBrushPosition( GetLevel(), 0 );
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
		case NAME_PreRaytrace:
		case NAME_PostRaytrace:
		{
			// Called before/after raytracing session beings.
			Location = BasePos + KeyPos[KeyNum];
			Rotation = BaseRot + KeyRot[KeyNum];
			GBrushTracker.Update( this );
			break;
		}
		case NAME_RaytraceWorld:
		{
			// Place this brush in position to raytrace the world.
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
		case NAME_Spawned:
		{
			BasePos		= Location;
			BaseRot		= Rotation;
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
