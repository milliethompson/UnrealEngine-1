/*=============================================================================
	UnActor.cpp: AActor implementation

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	AActor object implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(AActor);

/*-----------------------------------------------------------------------------
	Actor touch minions.
-----------------------------------------------------------------------------*/

//
// Note that TouchActor has begin touching Actor.
//
// If an actor's touch list overflows, neither actor receives the
// touch messages, as if they are not touching.
//
// This routine is reflexive.
//
// Handles the case of the first-notified actor changing its touch status.
//
void AActor::BeginTouch( AActor *Other )
{
	guard(AActor::BeginTouch);

	// See if a touch slot is available in Actor.
	int Available=-1;
	for( int i=0; i<ARRAY_COUNT(Touching); i++ )
	{
		if( Touching[i] == NULL )
			Available = i;
		else if( Touching[i] == Other )
			goto SkipActor;
	}
	if( Available >= 0 )
	{
		// Make Actor touch TouchActor.
		Touching[Available] = Other;
		Process( NAME_Touch, &PActor(Other) );

		// See if first actor did something that caused an UnTouch.
		if( Touching[Available] != Other )
			return;
	}
	SkipActor:;

	// See if a touch slot is available in TouchActor.
	Available=-1;
	for( i=0; i<ARRAY_COUNT(Other->Touching); i++ )
	{
		if( Other->Touching[i] == NULL )
			Available=i;
		else if( Other->Touching[i] == this )
			goto SkipTouchActor;
	}
	if( Available >= 0 )
	{
		Other->Touching[Available] = this;
		Other->Process( NAME_Touch, &PActor(this) );
	}
	SkipTouchActor:;

	unguard;
}

//
// Note that TouchActor is no longer touching Actor.
//
// If NoNotifyActor is specified, Actor is not notified but
// TouchActor is (this happens during actor destruction).
//
void AActor::EndTouch( AActor *Other, BOOL NoNotifySelf )
{
	guard(AActor::EndTouch);

	// Notify Actor.
	for( int i=0; i<ARRAY_COUNT(Touching); i++ )
	{
		if( Touching[i] == Other )
		{
			Touching[i] = NULL;
			if( !NoNotifySelf )
				Process( NAME_UnTouch, &PActor(Other) );
			break;
		}
	}

	// Notify TouchActor.
	for( i=0; i<ARRAY_COUNT(Other->Touching); i++ )
	{
		if( Other->Touching[i] == this )
		{
			Other->Touching[i] = NULL;
			Other->Process( NAME_UnTouch, &PActor(this) );
			break;
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	AActor member functions.
-----------------------------------------------------------------------------*/

//
// Update a moving brush's position based on its controlling actor's
// position.  If Editor=1 and UnrealEd is active, snaps the brush to the
// grid as needed.
//
void AActor::UpdateBrushPosition( ULevel *Level,int Editor )
{
	guard(AActor::UpdateBrushPosition);

	if( IsBrush() )
	{
		Brush->Location = Location;
		Brush->Rotation = Rotation;

		if (Editor && GEditor)
		{
			// Snap brush rotation and location to grid.
			if( GEditor->Constraints.RotGridEnabled )
			{
				Brush->Rotation = Brush->Rotation.GridSnap(GEditor->Constraints.RotGrid);
			}
			if( GEditor->Constraints.GridEnabled )
			{
				Brush->Location = Brush->Location.GridSnap(GEditor->Constraints.Grid);
			}
			if( Level )
				Process( NAME_PostEditMove, NULL );
		}
		Brush->BuildBound(1);
	}
	unguard;
}

//
// Returns 1 if the actor is a brush, 0 if not.
//
int AActor::IsBrush()
{
	return DrawType==DT_Brush && Brush!=NULL;
}

//
// Returns 1 if the actor is a moving brush, 0 if not.
//
int AActor::IsMovingBrush()
{
	return DrawType==DT_Brush && Brush!=NULL;
}

/*-----------------------------------------------------------------------------
	Relations.
-----------------------------------------------------------------------------*/

//
// Change the actor's owner.
//
void AActor::SetOwner( AActor *NewOwner )
{
	guard(AActor::SetOwner);

	// Sets this actor's parent to the specified actor.
	if( Owner != NULL )
		Owner->Process( NAME_LostChild, &PActor(this) );

	Owner = NewOwner;

	if( Owner != NULL )
		Owner->Process( NAME_GainedChild, &PActor(this) );

	unguard;
}

//
// Change the actor's floor.
//
void AActor::SetFloor( AActor *NewFloor )
{
	guard(AActor::SetFloor);

	//todo!!
	Floor = NewFloor;

	unguard;
}

/*-----------------------------------------------------------------------------
	The end.
-----------------------------------------------------------------------------*/
