/*=============================================================================
	UnActor.cpp: AActor implementation

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	AActorHelper member functions
-----------------------------------------------------------------------------*/

//
// Update a moving brush's position based on its controlling actor's
// position.  If Editor=1 and UnrealEd is active, snaps the brush to the
// grid as needed.
//
void AActorBase::UpdateBrushPosition( ILevel *Level,INDEX iActor,int Editor )
{
	guard(AActorBase::UpdateBrushPosition);

	if (IsBrush())
	{
		Brush->Location = Location;
		Brush->Rotation = DrawRot;

		if (Editor && GEditor)
		{
			// Snap brush rotation and location to grid.
			if (GEditor->Constraints.RotGridEnabled)
			{
				Brush->Rotation = Brush->Rotation.GridSnap(GEditor->Constraints.RotGrid);
			}
			if (GEditor->Constraints.GridEnabled)
			{
				Brush->Location = Brush->Location.GridSnap(GEditor->Constraints.Grid);
			}
			if (Level)
			{
				Level->SendMessage(iActor,ACTOR_PostEditMove,NULL);
			}
		}
		Brush->BuildBound(1);
	}
	unguard;
}

//
// Returns 1 if the actor is a brush, 0 if not.
//
int AActorBase::IsBrush()
{
	return (DrawType==DT_Brush) && (Brush!=NULL);
}

//
// Returns 1 if the actor is a moving brush, 0 if not.
//
int AActorBase::IsMovingBrush()
{
	return (DrawType==DT_Brush) && (Brush!=NULL);
}

//
// Compute the actor's view coordinate system.  Upon return:
//
// Coords->XAxis points in the direction that the actor perceives as 'right'.
// Coords->YAxis points in the direction that the actor perceives as 'down'.
// Coords->ZAxis points in the direction that the actor perceives as 'forward'.
//
void AActorBase::GetViewCoords( FCoords *Coords ) const
{
	guard(AActorBase::GetViewCoords);

	*Coords = GMath.CameraViewCoords;
	Coords->DeTransformByRotation (ViewRot);

	unguard;
}

//
// Compute the actor's rendering coordinate system.
//
void AActorBase::GetDrawCoords(FCoords *Coords) const
{
	guard(AActorBase::GetDrawCoords);

	*Coords = GMath.CameraViewCoords;
	Coords->DeTransformByRotation (DrawRot);

	unguard;
}

//
// Transform a world point into a point relative to the actor.  
// On return:
//
// LocalPoint.X indicates how far right the point is of the actor.
// LocalPoint.Y indicates how far the point is below the actor.
// LocalPoint.Z indicates how far ahead the point is of the actor.
//
void AActorBase::TransformPoint( FVector &LocalPoint, const FVector &WorldPoint )
{
	guard(AActorBase::TransformPoint);

	FCoords Coords;
	GetViewCoords(&Coords);

	LocalPoint = WorldPoint;
	LocalPoint.TransformVector(Coords);

	unguard;
}

/*-----------------------------------------------------------------------------
	Initialization.
-----------------------------------------------------------------------------*/

//
// Initialize all root properties of an actor.
//
void AActorBase::Init(UClass *ThisClass)
{
	guard(AActorBase::Init);

	// Zero fill the actor.
	mymemset(this,0,sizeof(AActor));

	// Set its basic properties.
	Class		= ThisClass;
	Name		= NAME_NONE;
	Event		= NAME_NONE;
	iParent		= INDEX_NONE;
	iTarget		= INDEX_NONE;
	iWeapon		= INDEX_NONE;
	iInventory	= INDEX_NONE;
	BlitType	= BT_Normal;
	DrawType	= DT_Sprite;
	DrawScale	= 1.0;
	Mass		= 200.0;
	DefaultEdCategory	= NAME_NONE;

	// Set the pointers to its local and static data.
	SetBinPointers(Class);

	// Init touching records.
	for (int i=0; i<MAX_TOUCHING_ACTORS; i++)
		iTouchingActors[i] = INDEX_NONE;

	unguard;
}

//
// Set the actor's private pointers.
//
void AActorBase::SetBinPointers(UClass *Class)
{
	guard(AActorBase::SetBinPointers);

	// Global data.
	((AActor*)this)->Private.PropertyBins[0] = (BYTE *)this;

	// Static data.
	((AActor*)this)->Private.PropertyBins[1] = (BYTE *)&Class->DefaultActor;

	// Local data.
	((AActor*)this)->Private.PropertyBins[2] = NULL;

	unguard;
}

/*-----------------------------------------------------------------------------
	Resource related functions.
-----------------------------------------------------------------------------*/

//
// Call the callback with all names and resources referenced by the actor.  This
// is used both by UClass's query functions and UActorList's query functions, since
// they both contain actors.
//
void AActorBase::QueryReferences
(
	UResource			*Owner,
	FResourceCallback	&Callback,
	DWORD				ContextFlags,
	INT					AllProperties
)
{
	// Remember possibly-delinked class.
	UClass *TrueClass;
	guard(AActorBase::QueryReferences Setup);
	{
		if (Owner->Type==RES_ActorList)
			TrueClass = (UClass *)Callback.GetActualResource(Owner,Class);
		else
			TrueClass = (UClass *)Owner;

		// Set up property bins.
		SetBinPointers(TrueClass);
	}
	unguard;

	// Return all references according to class property list.
	// Relies on the class's data being in place, but can not expect that the
	// class's resources and names are linked.
	guard(AActorBase::QueryReferences);

	for (int i=0; i<TrueClass->Num; i++)
	{
		FClassProperty	*Property = &TrueClass->Element(i);

		if( AllProperties || Property->IsPerActor() )
		{
			if ((Property->Type == CPT_Resource) && !(Property->Flags & CPF_NoSaveResource))
			{
				for (int j=0; j<Property->ArrayDim; j++)
				{
					Callback.Resource
					(
						Owner,
						(UResource **)GetPropertyPtrFromClass(TrueClass,i,j),
						ContextFlags
					);
				}
			}
			else if (Property->Type == CPT_Name)
			{
				for (int j=0; j<Property->ArrayDim; j++)
				{
					Callback.Name
					(
						Owner,
						(FName *)GetPropertyPtrFromClass(TrueClass,i,j),
						ContextFlags
					);
				}
			}
		}
	}
	unguardf(("(Ptr=%i, Name=%s)",(int)TrueClass,TrueClass->Name));
}

//
// Called after loading an actor from disk.
//
void AActorBase::PostLoad( UResource *Owner, INT AllProperties )
{
	guard(AActorBase::PostLoad);

	// Set up property bins.
	SetBinPointers(Class);

	// Fix up everything.
	FClassProperty *Property = &Class->Element(0);
	for (int j=0; j<Class->Num; j++)
	{
		if( (Property->Type == CPT_Resource)
			&& (Property->Flags & CPF_NoSaveResource)
			&& (AllProperties || Property->IsPerActor()) )
		{
			for( int k=0; k<Property->ArrayDim; k++ )
				*(UResource **)GetPropertyPtr(j,k) = NULL;
		}
		Property++;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The end.
-----------------------------------------------------------------------------*/
