/*=============================================================================
	UnEdAct.cpp: Unreal editor actor-related functions

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

#pragma DISABLE_OPTIMIZATION /* Not performance-critical */

/*-----------------------------------------------------------------------------
   Editor actor movement functions.
-----------------------------------------------------------------------------*/

//
// Move all selected actors except cameras.  
// No transaction tracking.
//
void FGlobalEditor::edactMoveSelected
(
	ULevel		*Level, 
	FVector		&Delta, 
	FRotation	&Rotation
)
{
	guard(FGlobalEditor::edactMoveSelected);
	for( INDEX i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level->Element(i);
		if( Actor && Actor->bSelected )
		{
			// Update actor's temporary dynamic lighting flags.
			if( Delta != GMath.ZeroVector )
				Actor->bDynamicLight = 1;
			else
				Actor->bLightChanged = 1;

			// Update actor's location and rotation.
			Actor->Location.AddBounded( Delta );
			Actor->Rotation += Rotation;

			if( Actor->IsMovingBrush() )
			{
				// Update the moving brush.  Handles grid and rotgrid snapping of
				// the brush as well as updating keyframe position info.
				Actor->UpdateBrushPosition(Level,1);
			}
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
   Actor adding/deleting functions.
-----------------------------------------------------------------------------*/

//
// Delete all selected actors.
//
void FGlobalEditor::edactDeleteSelected( ULevel *Level )
{
	guard(FGlobalEditor::edactDeleteSelected);

	for( INDEX i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level->Element(i);
		if( Actor && Actor->bSelected )
			Level->DestroyActor( Actor );
	}
	unguard;
}

//
// Duplicate all selected actors and select just the duplicated set.
//
void FGlobalEditor::edactDuplicateSelected( ULevel *Level )
{
	guard(FGlobalEditor::edactDuplicateSelected);
	FVector Delta(32.0, 32.0, 0.0);

	// Untag all actors.
	for( int i=0; i<Level->Num; i++ )
		if( Level->Element(i) )
			Level->Element(i)->bTempEditor = 0;

	// Duplicate and deselect all actors.
	for( i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level->Element(i);
		if( Actor && Actor->bSelected && !Actor->bTempEditor )
		{
			FVector NewLocation = Actor->Location + Delta;
			AActor *NewActor = Level->SpawnActor
			(
				Actor->GetClass(),
				NULL,
				Actor->GetFName(),
				&NewLocation,
				&Actor->Rotation,
				Actor
			);
			if( NewActor )
			{
				NewActor->Lock(LOCK_Trans);
				if( NewActor->Brush )
				{
					NewActor->Brush = csgDuplicateBrush(Level,Actor->Brush,0,0,0,Actor->IsMovingBrush());
					NewActor->UpdateBrushPosition(Level,1);
				}
				NewActor->bTempEditor = 1;
				NewActor->Unlock(LOCK_Trans);
			}
			Actor->Lock(LOCK_Trans);
			Actor->bSelected = 0;
			Actor->Unlock(LOCK_Trans);
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
   Actor selection functions.
-----------------------------------------------------------------------------*/

//
// Select all actors except cameras and hidden actors.
//
void FGlobalEditor::edactSelectAll( ULevel *Level )
{
	guard(FGlobalEditor::edactSelectAll);
	for( INDEX i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level->Element(i);
		if( Actor && !Actor->IsA("View") && !Actor->bSelected && !Actor->bHiddenEd )
		{
			Actor->Lock(LOCK_Trans);
			Actor->bSelected=1;
			Actor->Unlock(LOCK_Trans);
		}
	}
	unguard;
}

//
// Select all actors in a particular class.
//
void FGlobalEditor::edactSelectOfClass( ULevel *Level, UClass *Class )
{
	guard(FGlobalEditor::edactSelectOfClass);
	for( INDEX i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level->Element(i);
		if( Actor && Actor->GetClass()==Class && !Actor->IsA("View") && !Actor->bSelected )
		{
			Actor->Lock(LOCK_Trans);
			Actor->bSelected=1;
			Actor->Unlock(LOCK_Trans);
		}
	}
	unguard;
}

//
// Select no actors.
//
void FGlobalEditor::edactSelectNone( ULevel *Level )
{
	guard(FGlobalEditor::edactSelectNone);
	for( INDEX i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level->Element(i);
		if( Actor && Actor->bSelected )
		{
			Actor->Lock(LOCK_Trans);
			Actor->bSelected = 0;
			Actor->Unlock(LOCK_Trans);
		}
	}
	unguard;
}

//
// Delete all actors that are descendents of a class.
//
void FGlobalEditor::edactDeleteDependentsOf( ULevel *Level, UClass *Class )
{
	guard(FGlobalEditor::edactDeleteDependentsOf);

	for( INDEX i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level->Element(i);
		if( Actor && Actor->IsA(Class) )
			Level->DestroyActor( Actor );
	}
	unguard;
}

/*-----------------------------------------------------------------------------
   The End.
-----------------------------------------------------------------------------*/
