/*=============================================================================
	UnEdAct.cpp: Unreal editor actor-related functions

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

#pragma DISABLE_OPTIMIZATION /* Not performance-critical */

/*-----------------------------------------------------------------------------
   Editor actor movement functions
-----------------------------------------------------------------------------*/

//
// Move all selected actors *except* cameras.  No transaction tracking.
//
void FGlobalEditor::edactMoveSelected (ILevel *Level, FVector *Delta, FFloatRotation *Rotation)
	{
	GUARD;
	AActor *Actor = &Level->Actors->Element(0);
	for (INDEX i=0; i<Level->Actors->Max; i++)
		{
		if (Actor->bSelected)
			{
			if( *Delta != GMath.ZeroVector ) Actor->bTempDynamicLight = 1;
			else Actor->bTempLightChanged = 1;
			//
			Actor->Location.MoveBounded(*Delta);
			Actor->DrawRot.Add(Rotation->Pitch,Rotation->Yaw,Rotation->Roll);
			//
			if (Actor->IsMovingBrush())	
				{
				//
				// Update the moving brush.  Handles grid and rotgrid snapping of
				// the brush as well as updating keyframe position info.
				//
				Actor->UpdateBrushPosition(Level,i,1);
				};
			};
		Actor++;
		};
	UNGUARD("edactMoveSelected");
	};

/*-----------------------------------------------------------------------------
   Actor adding/deleting functions
-----------------------------------------------------------------------------*/

//
// Delete all selected actors.  Transaction-tracked.
//
void FGlobalEditor::edactDeleteSelected (ILevel *Level)
	{
	GUARD;
	AActor *Actor = &Level->Actors->Element(0);
	for (INDEX i=0; i<Level->Actors->Max; i++)
		{
		if (Actor->Class && Actor->bSelected)
			{
			Level->DestroyActor(i);
			};
		Actor++;
		};
	UNGUARD("edactDeleteSelected");
	};

//
// Duplicate all selected actors and select just the duplicated set.
// Transaction-tracked.
//
void FGlobalEditor::edactDuplicateSelected (ILevel *Level)
	{
	GUARD;
	AActor		*Actor,*NewActor;
	FVector		Delta = {32.0,32.0,0.0};
	INDEX		i,n;
	//
	// Tag all actors as non-current, no need to transaction track:
	//
	Actor = &Level->Actors->Element(0);
	for (i=0; i<Level->Actors->Max; i++) (Actor++)->bTempEditor=0;
	//
	// Duplicate and deselect all actors:
	//
	Actor = &Level->Actors->Element(0);
	for (i=0; i<Level->Actors->Max; i++)
		{
		if (Actor->bSelected)
			{
			GTrans->NoteActor (Level->Actors,i);
			Actor->bSelected = 0;
			//
			FVector NewLocation = Actor->Location + Delta;
			n 					= Level->SpawnActor(Actor->Class,Actor->Name,&NewLocation);
			NewActor			= &Level->Actors->Element(n);
			//
			*NewActor = *Actor;
			NewActor->Location    = NewLocation;
			NewActor->bTempEditor = 1;
			NewActor->iMe		  = n;
			//
			Level->UnlinkActor(n);
			//
			if (Actor->Brush)
				{
				NewActor->Brush = csgDuplicateBrush(Level->Level,Actor->Brush,0,0);
				NewActor->UpdateBrushPosition(Level,i,1);
				};
			};
		Actor++;
		};
	//
	// Select all duplicated actors:
	//
	Actor = &Level->Actors->Element(0);
	for (i=0; i<Level->Actors->Max; i++)
		{
		if (Actor->bTempEditor)
			{
			GTrans->NoteActor (Level->Actors,i);
			Actor->bTempEditor = 0;
			Actor->bSelected   = 1;
			};
		Actor++;
		};
	UNGUARD("edactDuplicateSelected");
	};

/*-----------------------------------------------------------------------------
   Actor selection functions
-----------------------------------------------------------------------------*/

//
// Select all actors except cameras. Transaction-tracked.
//
void FGlobalEditor::edactSelectAll (ILevel *Level)
	{
	GUARD;
	AActor *Actor = &Level->Actors->Element(0);
	for (INDEX i=0; i<Level->Actors->Max; i++)
		{
		if (!Actor->IsKindOf("Camera") && !Actor->bSelected && !Actor->bHiddenEd)
			{
			GTrans->NoteActor (Level->Actors,i);
			Actor->bSelected=1;
			};
		Actor++;
		};
	UNGUARD("edactSelectAll");
	};

//
// Select all actors in a particular class. Transaction-tracked.
//
void FGlobalEditor::edactSelectOfClass (ILevel *Level,UClass *Class)
	{
	GUARD;
	AActor *Actor = &Level->Actors->Element(0);
	for (INDEX i=0; i<Level->Actors->Max; i++)
		{
		if (Actor->Class==Class && !Actor->IsKindOf("Camera") && !Actor->bSelected)
			{
			GTrans->NoteActor (Level->Actors,i);
			Actor->bSelected=1;
			};
		Actor++;
		};
	UNGUARD("edactSelectAll");
	};

//
// Select no actors. Transaction-tracked.
//
void FGlobalEditor::edactSelectNone (ILevel *Level)
	{
	AActor		*Actor;
	//
	GUARD;
	Actor = &Level->Actors->Element(0);
	for (INDEX i=0; i<Level->Actors->Max; i++)
		{
		if (Actor->bSelected)
			{
			GTrans->NoteActor (Level->Actors,i);
			Actor->bSelected = 0;
			};
		Actor++;
		};
	UNGUARD("edactSelectNone");
	};

//
// Delete all actors that are descendents of a class.
//
void FGlobalEditor::edactDeleteDependentsOf (ILevel *Level,UClass *Class)
	{
	AActor *Actor;
	UClass *TestClass;
	//
	Actor = &Level->Actors->Element(0);
	for (INDEX i=0; i<Level->Actors->Max; i++)
		{
		TestClass = Actor->Class;
		while (TestClass)
			{
			if (Class==TestClass)
				{
				Level->DestroyActor(i);
				break;
				};
			TestClass = TestClass->ParentClass;
			};
		Actor++;
		};
	};

/*-----------------------------------------------------------------------------
   Actor selection functions
-----------------------------------------------------------------------------*/
