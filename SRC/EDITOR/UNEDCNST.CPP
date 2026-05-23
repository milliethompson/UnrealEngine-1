/*=============================================================================
	UnEdCnst.cpp: Functions related to movement constraints

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	What's happening: When the Visual Basic level editor is being used,
	this code exchanges messages with Visual Basic.  This lets Visual Basic
	affect the world, and it gives us a way of sending world information back
	to Visual Basic.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/
	 
#include "Unreal.h"

//
// To do: Convert to a C++ class.
// Genericise to allow for other types of constraints later on.
//

/*------------------------------------------------------------------------------
	Movement constraints
------------------------------------------------------------------------------*/

//
// Force a location and rotation corresponding to a brush to be constrained within
// a level.  Affects *Location and *Rotation, not actual brush properties.
// Handles grid alignment and vertex snapping.
//
// Returns 1 if snapped to a vertex.
// Returns -1 if brush location is invalid, i.e. partially off map. Not implemented.
//
int FGlobalEditor::constraintApply
(
	UModel			*LevelModel, 
	UModel			*Brush,
	FVector			*Location, 
	FRotation		*Rotation,
	FConstraints	*Constraints
)
{
	guard(FGlobalEditor::constraintApply);

	FVector  SourcePoint;
	FVector	 DestPoint;
	INDEX	 Temp;
	int		 Snapped = 0;

	if( Constraints->RotGridEnabled && Rotation )
	{
		*Rotation = Rotation->GridSnap(Constraints->RotGrid);
	}
	if( LevelModel && Brush && Constraints->SnapVertex )
	{
		SourcePoint = *Location + Brush->PostPivot;
		if( LevelModel->FindNearestVertex
			(SourcePoint,DestPoint,Constraints->SnapDist,Temp) >= 0.0)
		{
			*Location = DestPoint - Brush->PostPivot;
			Snapped   = 1;
		}
	}
	if( Constraints->GridEnabled && !Snapped )
	{
		*Location = Location->GridSnap(Constraints->Grid);
	}
	return Snapped;
	unguard;
}

//
// Constrain to grid only.
//
void constrainSimplePoint
(
	FVector			*Location,
	FConstraints	*Constraints 
)
{
	guard(constrainSimplePoint);

	if( Constraints->GridEnabled )
		*Location = Location->GridSnap(Constraints->Grid);

	unguard;
}

//
// Finish snapping a brush.
//
void FGlobalEditor::constraintFinishSnap(ULevel *Level,UModel *Brush)
{
	guard(FGlobalEditor::constraintFinishSnap);
	if( Brush->ModelFlags & MF_ShouldSnap )
	{
		Brush->Lock(LOCK_Trans);
		constraintApply(Level->Model,Brush,&Brush->Location,&Brush->Rotation,&Constraints);
		Brush->Unlock(LOCK_Trans);
		Brush->ModelFlags &= ~MF_ShouldSnap;
	}
	unguard;
}

//
// Finish snapping all brushes in a level
//
void FGlobalEditor::constraintFinishAllSnaps( ULevel *Level )
{
	guard(FGlobalEditor::constraintFinishAllSnaps);

	Level->Lock(LOCK_Trans);

	//	Constrain active brush.
	UModel *Brush = Level->BrushArray->Element(0);

	Brush->Lock   		(LOCK_Trans);
	constraintApply 	(Level->Model,Brush,&Brush->Location,&Brush->Rotation,&Constraints);
	Brush->Unlock 		(LOCK_Trans);

	// Constrain level brushes.
	if( MapEdit )
	{
		int n = Level->BrushArray->Num;
		for( int i=1; i<n; i++ )
		{
			constraintFinishSnap(Level,Level->BrushArray->Element(i));
		}
	}
	Level->Unlock(LOCK_Trans);
	unguard;
}

void FGlobalEditor::constraintInit(FConstraints *Const)
{
	guard(FGlobalEditor::constraintInit);
	memset (Const,0,sizeof (FConstraints));

	Const->Grid					= FVector(0,0,0);
	Const->GridBase				= FVector(0,0,0);
	Const->RotGrid				= FRotation(0,0,0);
	Const->SnapDist				= 0.0;

	Const->Flags				= 0;
	Const->GridEnabled			= 0;
	Const->RotGridEnabled		= 0;
	Const->SnapVertex			= 1;
	unguard;
}

/*------------------------------------------------------------------------------
	The end
------------------------------------------------------------------------------*/
