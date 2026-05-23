/*=============================================================================
	UnThings.cpp: Various actor routines

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "UnGame.h"

/*-----------------------------------------------------------------------------
	Light base class
-----------------------------------------------------------------------------*/

int ALight::Process(ILevel *Level, FName Message, void *Params)
	{
	GUARD;
	switch (Message.Index)
		{
		case ACTOR_Tick:
			return 0;
		};
	return 0;
	UNGUARD("ALight::Process");
	};

int ASatellite::Process(ILevel *Level, FName Message, void *Params)
	{
	GUARD;
	switch (Message.Index)
		{
		case ACTOR_Tick:
			{
			if (!Level->Descriptor) return 1;
			FLOAT Time = 2.0 * PI * (Phase + Level->Descriptor->Time * Period);
			//
			FCoords Coords = GMath.UnitCoords;
			Coords.TransformByRotation(DrawRot);
			//
			Location =
				sin(Time) * Coords.ZAxis +
				cos(Time) * Coords.XAxis;
			return 1;
			};
		};
	return 0;
	UNGUARD("ALight::Process");
	};

int ASpotlight::Process(ILevel *Level, FName Message, void *Params){return 0;}

/*-----------------------------------------------------------------------------
	Camera base class
-----------------------------------------------------------------------------*/

int ACamera::Process(ILevel *Level, FName Message, void *Params)
	{
	GUARD;
	switch (Message.Index)
		{
		case ACTOR_PlayerTick:
			return GGame.PlayerTick(iMe,Params);
		case ACTOR_PlayerCalcView:
			break;
			//return aPawn (Level,iActor,Msg,Params);
		};
	return 0;
	UNGUARD("ACamera::Process");
	};

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
