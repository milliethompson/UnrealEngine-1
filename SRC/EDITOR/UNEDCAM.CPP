/*=============================================================================
	UnEdCam.cpp: Unreal editor camera movement/selection functions

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

//
// Flags for cameraClick, stored in Camera header's ClickFlags:
//
enum ECameraClick
{
	CF_MOVE_BRUSH	= 1,	// Set if the brush has been moved since first click
	CF_MOVE_ACTOR	= 2,	// Set if the actors have been moved since first click
	CF_MOVE_TEXTURE = 4,	// Set if textures have been adjusted since first click
	CF_MOVE_ALL     = (CF_MOVE_BRUSH | CF_MOVE_ACTOR | CF_MOVE_TEXTURE),
};

//
// Internal declarations:
//
void NoteBrushMovement 		(UCamera *Camera);
void NoteTextureMovement	(UCamera *Camera);
void NoteActorMovement 		(UCamera *Camera);
void constrainSimplePoint	(FVector *Location,FConstraints *Constraints);
int  GetMeshMapFrame		(UObject *MeshMap, int AnimSeq, int AnimOfs);

//
// Global variables (not beautiful, but they do the job)
//
int		GLastScroll		= 0;
int		GFixPanU=0,GFixPanV=0;
int		GFixScale=0;

/*-----------------------------------------------------------------------------
   Selection callbacks.
-----------------------------------------------------------------------------*/

//
// Callback for selecting a polygon, transaction-tracked.
//
void SelectPolyFunc( UModel *Model, INDEX iSurf )
	{
	FBspSurf *Poly = &Model->Surfs(iSurf);
	//
	if (!(Poly->PolyFlags & PF_Selected))
		{
		Model->Surfs->ModifyItem(iSurf,0);
		Poly->PolyFlags |= (PF_Selected);
		};
	};

//
// Callback for deselecting a polygon, transaction-tracked.
//
void DeselectPolyFunc (UModel *Model, INDEX iSurf)
	{
	FBspSurf *Poly = &Model->Surfs(iSurf);
	//
	if (Poly->PolyFlags & PF_Selected)
		{
		Model->Surfs->ModifyItem(iSurf,0);
		Poly->PolyFlags &= (~PF_Selected);
		};
	};

/*-----------------------------------------------------------------------------
   Routines to calculate various types of movement & rotation.
-----------------------------------------------------------------------------*/

//
// Freeform orthogonal movement and rotation.
//
// Limitation: Doesn't handle rotation properly in the XZ and YZ cameras.  Rotation
// there should rotate about the respective axis, but this isn't feasible since brushes
// just have P-Y-R rotations.  Can live with this.
//
void CalcFreeOrthoMoveRot
(
	UCamera		*Camera,
	FLOAT		MouseX,
	FLOAT		MouseY,
	DWORD		Buttons,
	FVector		&Delta,
	FRotation	&DeltaRot
)
{
	guard(CalcFreeOrthoMoveRot);
	FLOAT *OrthoAxis1,*OrthoAxis2,Axis2Sign,Axis1Sign,*OrthoAngle,AngleSign;

	FLOAT DeltaPitch = DeltaRot.Pitch;
	FLOAT DeltaYaw   = DeltaRot.Yaw;
	FLOAT DeltaRoll  = DeltaRot.Roll;

	if( Camera->Actor->RendMap == REN_OrthXY )
	{
		OrthoAxis1 = &Delta.X;  	Axis1Sign = +1;
		OrthoAxis2 = &Delta.Y;  	Axis2Sign = +1;
		OrthoAngle = &DeltaYaw;		AngleSign = +1;
	}
	else if( Camera->Actor->RendMap==REN_OrthXZ )
	{
		OrthoAxis1 = &Delta.X; 		Axis1Sign = +1;
		OrthoAxis2 = &Delta.Z; 		Axis2Sign = -1;
		OrthoAngle = &DeltaPitch; 	AngleSign = +1;
	}
	else if( Camera->Actor->RendMap==REN_OrthYZ )
	{
		OrthoAxis1 = &Delta.Y; 		Axis1Sign = +1;
		OrthoAxis2 = &Delta.Z; 		Axis2Sign = -1;
		OrthoAngle = &DeltaRoll; 	AngleSign = +1;
	}
	else
	{
		appError("Invalid rendering mode");
		return;
	}

	// Special movement controls.
	if( Buttons == BUT_LEFT )
	{
		// Left button: Move up/down/left/right.
		*OrthoAxis1 = Camera->Actor->OrthoZoom/30000.0*(FLOAT)MouseX;
		if      ((MouseX<0)&&(*OrthoAxis1==0)) *OrthoAxis1=-Axis1Sign;
		else if ((MouseX>0)&&(*OrthoAxis1==0)) *OrthoAxis1=Axis1Sign;

		*OrthoAxis2 = Axis2Sign*Camera->Actor->OrthoZoom/30000.0*(FLOAT)MouseY;
		if      ((MouseY<0)&&(*OrthoAxis2==0)) *OrthoAxis2=-Axis2Sign;
		else if ((MouseY>0)&&(*OrthoAxis2==0)) *OrthoAxis2=Axis2Sign;
	}
	else if( Buttons == (BUT_LEFT | BUT_RIGHT) )
	{
		// Both buttons: Zoom in/out.
		Camera->Actor->OrthoZoom -= Camera->Actor->OrthoZoom/200.0 * (FLOAT)MouseY;

		if (Camera->Actor->OrthoZoom<500.0)    		Camera->Actor->OrthoZoom = 500.0;
		if (Camera->Actor->OrthoZoom>2000000.0) 	Camera->Actor->OrthoZoom = 2000000.0;
	}
	else if( Buttons == BUT_RIGHT )
	{
		// Right button: Rotate.
		if (OrthoAngle!=NULL) *OrthoAngle = -AngleSign*8.0*(FLOAT)MouseX;
	}
	DeltaRot.Pitch	= DeltaPitch;
	DeltaRot.Yaw	= DeltaYaw;
	DeltaRot.Roll	= DeltaRoll;

	unguard;
}

//
// Axial orthogonal movement.
//
// Limitation: Doesn't handle rotation properly in the XZ and YZ cameras.  Rotation
// there should rotate about the respective axis, but this isn't feasible since brushes
// just have P-Y-R rotations.  Can live with this.
//
void CalcAxialOrthoMove
(
	UCamera		*Camera, 
	FLOAT		MouseX,
	FLOAT		MouseY,
	DWORD		Buttons,
	FVector		&Delta,
	FRotation	&DeltaRot
)
{
	guard(CalcAxialOrthoMove);
	FLOAT *OrthoAxis1,*OrthoAxis2,Axis2Sign,Axis1Sign,*OrthoAngle,AngleSign;

	FLOAT DeltaPitch = DeltaRot.Pitch;
	FLOAT DeltaYaw   = DeltaRot.Yaw;
	FLOAT DeltaRoll  = DeltaRot.Roll;

	if( Camera->Actor->RendMap == REN_OrthXY )
	{
		OrthoAxis1 = &Delta.X;  	Axis1Sign = +1;
		OrthoAxis2 = &Delta.Y;  	Axis2Sign = +1;
		OrthoAngle = &DeltaYaw;		AngleSign = +1;
	}
	else if( Camera->Actor->RendMap == REN_OrthXZ )
	{
		OrthoAxis1 = &Delta.X; 		Axis1Sign = +1;
		OrthoAxis2 = &Delta.Z;		Axis2Sign = -1;
		OrthoAngle = &DeltaPitch; 	AngleSign = +1;
	}
	else if( Camera->Actor->RendMap == REN_OrthYZ )
	{
		OrthoAxis1 = &Delta.Y; 		Axis1Sign = +1;
		OrthoAxis2 = &Delta.Z; 		Axis2Sign = -1;
		OrthoAngle = &DeltaRoll; 	AngleSign = +1;
	}
	else
	{
		appError("Invalid rendering mode");
		return;
	}

	// Special movement controls.
	if( Buttons & (BUT_LEFT | BUT_RIGHT) )
	{
		// Left, right, or both are pressed.
		if( Buttons & BUT_LEFT )
		{
			// Left button: Screen's X-Axis.
      		*OrthoAxis1 = Camera->Actor->OrthoZoom/30000.0*(FLOAT)MouseX;
      		if      ((MouseX<0)&&(*OrthoAxis1==0)) *OrthoAxis1=-Axis1Sign;
      		else if ((MouseX>0)&&(*OrthoAxis1==0)) *OrthoAxis1=Axis1Sign;
		}
		if( Buttons & BUT_RIGHT )
		{
			// Right button: Screen's Y-Axis.
      		*OrthoAxis2 = Axis2Sign*Camera->Actor->OrthoZoom/30000.0*(FLOAT)MouseY;
      		if      ((MouseY<0)&&(*OrthoAxis2==0)) *OrthoAxis2=-Axis2Sign;
      		else if ((MouseY>0)&&(*OrthoAxis2==0)) *OrthoAxis2=Axis2Sign;
		}
	}
	else if( Buttons == (BUT_MIDDLE) )
	{
		// Middle button: Zoom in/out.
		Camera->Actor->OrthoZoom -= Camera->Actor->OrthoZoom/200.0 * (FLOAT)MouseY;

		if		(Camera->Actor->OrthoZoom<500.0)    	Camera->Actor->OrthoZoom = 500.0;
		else if (Camera->Actor->OrthoZoom>2000000.0) 	Camera->Actor->OrthoZoom = 2000000.0;
	}
	DeltaRot.Pitch	= DeltaPitch;
	DeltaRot.Yaw	= DeltaYaw;
	DeltaRot.Roll	= DeltaRoll;

	unguard;
}

//
// Freeform perspective movement and rotation.
//
void CalcFreePerspMoveRot
(
	UCamera		*Camera, 
	FLOAT		MouseX,
	FLOAT		MouseY,
	DWORD		Buttons,
	FVector		&Delta,
	FRotation	&DeltaRot
)
{
	guard(CalcFreePerspMoveRot);
	APawn *Actor = Camera->Actor;

	if( Buttons == BUT_LEFT )
	{
		// Left button: move ahead and yaw.
		Delta.X      = -MouseY * GMath.CosTab(Actor->ViewRotation.Yaw);
		Delta.Y      = -MouseY * GMath.SinTab(Actor->ViewRotation.Yaw);
		DeltaRot.Yaw = MouseX * 64.0 / 20.0;
	}
	else if( Buttons == (BUT_LEFT | BUT_RIGHT) )
	{
		// Both buttons: Move up and left/right.
		Delta.X      = MouseX * -GMath.SinTab(Actor->ViewRotation.Yaw);
		Delta.Y      = MouseX *  GMath.CosTab(Actor->ViewRotation.Yaw);
		Delta.Z      = -MouseY;
		}
	else if( Buttons == BUT_RIGHT )
	{
		// Right button: Pitch and yaw.
		DeltaRot.Pitch = (64.0/12.0) * -MouseY;
		DeltaRot.Yaw   = (64.0/20.0) * MouseX;
	}
	unguard;
}

//
// Axial perspective movement.
//
void CalcAxialPerspMove
( 
	UCamera		*Camera, 
	SWORD		MouseX,
	SWORD		MouseY,
	DWORD		Buttons,
	FVector		&Delta,
	FRotation	&DeltaRot
)
{
	guard(CalcAxialPerspMove);

	// Do single-axis movement.
	if		(Buttons == BUT_LEFT)				Delta.X = +MouseX;
	else if (Buttons == BUT_RIGHT)				Delta.Y = +MouseX;
	else if (Buttons == (BUT_LEFT | BUT_RIGHT)) Delta.Z = -MouseY;

	unguard;
}

//
// See if a scale is within acceptable bounds:
//
int ScaleIsWithinBounds (FVector *V, FLOAT Min, FLOAT Max)
	{
	FLOAT Temp;
	//
	guard(ScaleIsWithinBounds);
	//
	Temp = Abs (V->X);
	if ((Temp<Min) || (Temp>Max)) return 0;
	//
	Temp = Abs (V->Y);
	if ((Temp<Min) || (Temp>Max)) return 0;
	//
	Temp = Abs (V->Z);
	if ((Temp<Min) || (Temp>Max)) return 0;
	//
	return 1;
	unguard;
	};

/*-----------------------------------------------------------------------------
   Camera movement computation.
-----------------------------------------------------------------------------*/

//
// Move and rotate camera freely.
//
void CameraMoveRot
(
	UCamera		*Camera,
	FVector		&Delta,
	FRotation	&DeltaRot
)
{
	guard(CameraMoveRot);

	Camera->Actor->ViewRotation.AddBounded(DeltaRot.Pitch,DeltaRot.Yaw,DeltaRot.Roll);
	Camera->Actor->Location.AddBounded(Delta);

	unguard;
}

//
// Move and rotate camera using gravity and collision where appropriate.
//
void CameraMoveRotWithPhysics
(
	UCamera		*Camera,
	FVector		&Delta,
	FRotation	&DeltaRot
)
{
	guard(CameraMoveRotWithPhysics);

	// Update rotation.
	Camera->Actor->ViewRotation.AddBounded
	(
		4.0*DeltaRot.Pitch,
		4.0*DeltaRot.Yaw,
		4.0*DeltaRot.Roll
	);

	//	Bound velocity and add it to delta.
	Camera->Actor->Velocity = Camera->Actor->Velocity.BoundToCube(40.0);
	Delta += Camera->Actor->Velocity;

	if( (Camera->Actor->ShowFlags & SHOW_PlayerCtrl) &&
		(!Camera->IsOrtho()) && 
		(!(Camera->Level->Model->ModelFlags & MF_InvalidBsp)) )
	{
		// Move with collision; player is outside and collision is enabled.
		FCheckResult Hit(1.0);
		Camera->Level->Model->LineCheck 
		(
			Hit,
			NULL,
			Camera->Actor->Location,
			Camera->Actor->Location + Delta,
			FVector(20.0,20.0,24.0),
			0
		);
		Camera->Actor->Location += Hit.Time * Delta;
		if( Hit.Time == 0.0 )
		{
			// Stuck, so allow free movement.
			Camera->Actor->Location.AddBounded(Delta);
		}
		else if( Hit.Time<1.0 && !(Camera->Actor->ShowFlags&SHOW_PlayerCtrl))
		{
			// Stop him.
			Camera->Actor->Velocity = FVector(0,0,0);
		}
	}
	else
	{
		// Move without collision.
		Camera->Actor->Location.AddBounded(Delta);
	}
	unguard;
}

//
// Move the camera so that it's facing the rotated/moved brush in exactly the
// same way as before.  Takes grid effects into account:
//
void CameraTrackBrush
(
	UCamera		*Camera,
	FVector		&Delta,
	FRotation	&DeltaRot
)
{
	guard(CameraTrackBrush);
	FVector		StartLocation,EndLocation;
	FRotation	StartRotation,EndRotation;

	UModel *Model = Camera->Level->Model;
	UModel *Brush = Camera->Level->Brush();

	Brush->Lock(LOCK_Read);

	StartLocation = Brush->Location; EndLocation = StartLocation;
	StartRotation = Brush->Rotation; EndRotation = StartRotation;

	EndLocation.AddBounded(Delta);
	EndRotation.AddBounded(DeltaRot.Pitch,DeltaRot.Yaw,DeltaRot.Roll);

	GUnrealEditor.constraintApply( Model, Brush, &StartLocation, &StartRotation, &GUnrealEditor.Constraints );
	GUnrealEditor.constraintApply( Model, Brush, &EndLocation  , &EndRotation,   &GUnrealEditor.Constraints );

	// Now move camera accordinly.
	Camera->Actor->Location += EndLocation;
	Camera->Actor->Location -= StartLocation;

	Brush->Unlock(LOCK_Read);
	unguard;
}

//
// If this is the first time the brush has moved since the user first
// pressed a mouse button, save the brush position transactionally so it can
// be undone/redone:
//
// Implicityly assumes that the selection set can't be changed between
// NoteBrushMovement/FinishBrushMovement pairs.
//
void NoteBrushMovement (UCamera *Camera)
	{
	FVector			WorldPivotLocation;
	UModel			*Brush;
	int 			i,n;
	//
	guard(NoteBrushMovement);
	if( !GTrans->IsLocked() && (!(Camera->ClickFlags & CF_MOVE_BRUSH)))
		{
		GTrans->Begin (Camera->Level,"Brush movement");
		//
		if (GUnrealEditor.MapEdit)
			{
			n = Camera->Level->BrushArray->Num;
			for (i=0; i<n; i++)
				{
				Brush = Camera->Level->BrushArray->Element(i);
				if ((Brush->ModelFlags & MF_Selected) || (i==0))
					{
					GTrans->NoteResHeader (Brush);
					Brush->ModelFlags |= MF_ShouldSnap;
					if (i!=0)
						{
						Brush->SetPivotPoint(&WorldPivotLocation,0);
						}
					else // i==0, first iteration
						{
						GUnrealEditor.constraintApply (NULL,NULL,&Brush->Location,&Brush->Rotation,&GUnrealEditor.Constraints);
						WorldPivotLocation = Brush->Location + Brush->PostPivot;
						};
					Brush->TransformedBound.IsValid=0;
					};
				};
			}
		else
			{
			GTrans->NoteResHeader (Camera->Level->Brush());
			Camera->Level->Brush()->TransformedBound.IsValid=0;
			};
		GTrans->End();
		//
		Camera->ClickFlags |= CF_MOVE_BRUSH;
		};
	unguard;
	};

//
// Finish any snaps that were applied after NoteBrushMovement:
//
void FinishBrushMovement(UCamera *Camera)
	{
	guard(FinishBrushMovement);
	//
	if (GUnrealEditor.MapEdit)
		{
		int n = Camera->Level->BrushArray->Num;
		for (int i=0; i<n; i++)
			{
			UModel *Brush = Camera->Level->BrushArray->Element(i);
			if ((i==0) || (Brush->ModelFlags & MF_ShouldSnap))
				{
				Brush->Lock(LOCK_Trans);
				GUnrealEditor.constraintApply(Camera->Level->Model,Brush,&Brush->Location,&Brush->Rotation,&GUnrealEditor.Constraints);
				Brush->ModelFlags &= ~MF_ShouldSnap;
				Brush->Unlock(LOCK_Trans);
				};
			if (Brush->ModelFlags & MF_Selected) Brush->BuildBound(1);
			};
		}
	else
		{
		UModel *Brush = Camera->Level->Brush();
		Brush->Lock(LOCK_Trans);
		GUnrealEditor.constraintApply (Camera->Level->Model,Brush,&Brush->Location,&Brush->Rotation,&GUnrealEditor.Constraints);
		Brush->Unlock(LOCK_Trans);
		Brush->BuildBound(1);
		};
	unguard;
	};

//
// If this is the first time called since first click, note all selected
// actors:
//
void NoteActorMovement( UCamera *Camera )
{
	guard(NoteActorMovement);

	int Found = 0;
	if( !GTrans->IsLocked() && !(Camera->ClickFlags & CF_MOVE_ACTOR) )
	{
		for( int i=0; i<Camera->Level->Num; i++ )
		{			
			AActor *Actor = Camera->Level->Element(i);
			if( Actor && Actor->bSelected )
			{
				if( !Found )
					GTrans->Begin( Camera->Level, "Actor movement" );
				Actor->Lock(LOCK_Trans);
				Actor->Unlock(LOCK_Trans);
				if( Actor->Brush )
				{
					Actor->Brush->Lock(LOCK_Trans);
					Actor->Brush->Unlock(LOCK_Trans);
				}
				Found++;
			}
		}
		if( Found )
			GTrans->End();

		Camera->ClickFlags |= CF_MOVE_ACTOR;
	}
	unguard;
}

//
// If this is the first time textures have been adjusted since the user first
// pressed a mouse button, save selected polygons transactionally so this can
// be undone/redone:
//
void NoteTextureMovement (UCamera *Camera)
	{
	guard(NoteTextureMovement);
	if ( !GTrans->IsLocked() && 
		!(Camera->ClickFlags & CF_MOVE_TEXTURE))
		{
		GTrans->Begin (Camera->Level,"Texture movement");
		Camera->Level->Model->Surfs->ModifySelected(1);
		GTrans->End ();
		//
		Camera->ClickFlags |= CF_MOVE_TEXTURE;
		};
	unguard;
	};

//
// Move and rotate brush.
//
void BrushMoveRot
(
	UCamera		*Camera,
	FVector		&Delta,
	FRotation	&DeltaRot 
)
{
	guard(BrushMoveRot);
	NoteBrushMovement( Camera );

	int Moved = 0;
	if( GUnrealEditor.MapEdit )
	{
		int n = Camera->Level->BrushArray->Num;
		for( int i=0; i<n; i++ )
		{
			UModel *Brush = Camera->Level->BrushArray->Element(i);
			if( Brush->ModelFlags & MF_Selected )
			{
		   		Brush->Lock(LOCK_Trans);
		   		Brush->Location.AddBounded(Delta);
				Brush->Rotation += DeltaRot;
		   		Brush->Unlock(LOCK_Trans);
				Moved = 1;
			}
		}
	}
	if( !Moved )
	{
		UModel *Brush = Camera->Level->Brush();
   		Brush->Lock(LOCK_Trans);
   		Brush->Location.AddBounded(Delta);
		Brush->Rotation += DeltaRot;
   		Brush->Unlock(LOCK_Trans);
	}
	unguard;
}

/*-----------------------------------------------------------------------------
   Editor camera movement.
-----------------------------------------------------------------------------*/

//
// Move the edit-camera.
//
void FGlobalEditor::edcamMove (UCamera *Camera, BYTE Buttons, FLOAT MouseX, FLOAT MouseY, int Shift, int Ctrl)
	{
	guard(FGlobalEditor::edcamMove);
	FVector     	Delta,Vector,SnapMin,SnapMax,DeltaMin,DeltaMax,DeltaFree;
	FRotation		DeltaRot;
	FLOAT			TempFloat,TempU,TempV,Speed;
	int				Temp;
	static int		ForceXSnap=0,ForceYSnap=0,ForceZSnap=0;
	static FLOAT	TextureAngle=0.0;
	static INDEX	*OriginalUVectors = NULL,*OriginalVVectors = NULL,OrigNumVectors=0;
	//
	if (Camera->IsBrowser()) return;
	if (!Camera->Lock(LOCK_ReadWrite|LOCK_CanFail)) return;
	UModel *Brush = Camera->Level->Brush();
	//
	Delta.X    		= 0.0;  Delta.Y  		= 0.0;  Delta.Z   		= 0.0;
	DeltaRot.Pitch	= 0.0;  DeltaRot.Yaw	= 0.0;  DeltaRot.Roll	= 0.0;
	//
	if (Buttons & BUT_FIRSTHIT)
		{
		//
		// Reset flags that last for the duration of the click:
		//
		Camera->ClickFlags &= ~(CF_MOVE_ALL);
		//
		Brush->Lock(LOCK_Trans);
		if (GUnrealEditor.Mode==EM_BrushSnap)
			{
			Brush->TempScale = Brush->PostScale;
			ForceXSnap = 0;
			ForceYSnap = 0;
			ForceZSnap = 0;
			}
		else if (GUnrealEditor.Mode==EM_TextureRotate)
			{
			//
			// Guarantee that each texture u and v vector on each selected polygon
			// is unique in the world.
			//
			if (OriginalUVectors) appFree(OriginalUVectors);
			if (OriginalVVectors) appFree(OriginalVVectors);
			//
			int Size = Camera->Level->Model->Surfs->Num * sizeof(INDEX);
			OriginalUVectors = (INDEX *)appMalloc(Size,"OriginalUVectors");
			OriginalVVectors = (INDEX *)appMalloc(Size,"OriginalVVectors");
			//
			OrigNumVectors = Camera->Level->Model->Vectors->Num;
			//
			for (int i=0; i<Camera->Level->Model->Surfs->Num; i++)
				{
				FBspSurf *Surf = &Camera->Level->Model->Surfs(i);
				OriginalUVectors[i] = Surf->vTextureU;
				OriginalVVectors[i] = Surf->vTextureV;
				//
				if (Surf->PolyFlags & PF_Selected)
					{
					int n			= Camera->Level->Model->Vectors->Add();
					FVector *V		= &Camera->Level->Model->Vectors(n);
					*V				= Camera->Level->Model->Vectors(Surf->vTextureU);
					Surf->vTextureU = n;
					//
					n				= Camera->Level->Model->Vectors->Add();
					V				= &Camera->Level->Model->Vectors(n);
					*V				= Camera->Level->Model->Vectors(Surf->vTextureV);
					Surf->vTextureV = n;
					//
					Surf->iLightMesh = INDEX_NONE; // Invalidate lighting mesh
					};
				};
			TextureAngle = 0.0;
			};
		Brush->Unlock(LOCK_Trans);
		};
	if (Buttons & BUT_LASTRELEASE)
		{
		FinishBrushMovement(Camera);
		//
		if (OriginalUVectors)
			{
			//
			// Finishing up texture rotate mode.  Go through and minimize the set of
			// vectors we've been adjusting by merging the new vectors in and eliminating
			// duplicates:
			//
			FMemMark Mark(GMem);
			int Count = Camera->Level->Model->Vectors->Num;
			FVector *AllVectors = new(GMem,Count)FVector;
			memcpy(AllVectors,&Camera->Level->Model->Vectors(0),Count*sizeof(FVector));
			//
			Camera->Level->Model->Vectors->Num = OrigNumVectors;
			//
			for (int i=0; i<Camera->Level->Model->Surfs->Num; i++)
				{
				FBspSurf *Surf = &Camera->Level->Model->Surfs(i);
				if (Surf->PolyFlags & PF_Selected)
					{
					// Update master texture coordinates but not base:
					polyUpdateMaster (Camera->Level->Model,i,1,0);
					// Add this poly's vectors, merging with the level's existing vectors:
					Surf->vTextureU = bspAddVector(Camera->Level->Model,&AllVectors[Surf->vTextureU],0);
					Surf->vTextureV = bspAddVector(Camera->Level->Model,&AllVectors[Surf->vTextureV],0);
					};
				};
			Mark.Pop();
			appFree(OriginalUVectors); OriginalUVectors=NULL;
			appFree(OriginalVVectors); OriginalVVectors=NULL;
			};
		};
	switch (GUnrealEditor.Mode)
		{
		case EM_None: /* Editor disabled */
			//
			debug (LOG_Problem,"Editor is disabled");
			//
			break;
		case EM_CameraMove: /* Move camera normally */
			//
			CameraMove:
			//
			if (Buttons & (BUT_FIRSTHIT | BUT_LASTRELEASE | BUT_SETMODE | BUT_EXITMODE))
				{
				Camera->Actor->Velocity = FVector(0,0,0);
				}
			else
				{
   				if (!(Ctrl||Shift))
					{
					Speed = 0.30*GUnrealEditor.MovementSpeed;
					if (Camera->IsOrtho())
						{
						if (Buttons == BUT_RIGHT)
							{
							Buttons = BUT_LEFT;
							Speed   = 0.60*GUnrealEditor.MovementSpeed;
							};
						CalcFreeOrthoMoveRot(Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
						}
					else
						{
						CalcFreePerspMoveRot(Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
						};
					Delta *= Speed;
					CameraMoveRotWithPhysics(Camera,Delta,DeltaRot); // Move camera
					}
				else
					{
					if (Camera->IsOrtho())	CalcFreeOrthoMoveRot  (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
					else                	CalcAxialPerspMove    (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
					//
					Delta *= GUnrealEditor.MovementSpeed*0.25;
					//
					if (Shift) CameraTrackBrush (Camera,Delta,DeltaRot);
   					BrushMoveRot				(Camera,Delta,DeltaRot); // Move brush
					};
				};
			break;
		case EM_CameraZoom:		/* Move camera with acceleration */
			//
			if (Buttons&(BUT_FIRSTHIT | BUT_LASTRELEASE | BUT_SETMODE | BUT_EXITMODE))
				{
				Camera->Actor->Velocity = FVector(0,0,0);
				}
			else
				{
				if (!(Ctrl||Shift))
					{
					if (Camera->IsOrtho())	CalcFreeOrthoMoveRot (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
					else                	CalcFreePerspMoveRot (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
					}
				else
					{
					if (Camera->IsOrtho())	CalcFreeOrthoMoveRot  (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
					else                	CalcAxialPerspMove    (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
					};
				Delta *= GUnrealEditor.MovementSpeed * 0.025;
				//
				Camera->Actor->Velocity += Delta;
				Delta = Camera->Actor->Velocity;
				//
   				if (!(Ctrl||Shift))
	   				{
					CameraMoveRotWithPhysics	(Camera,Delta,DeltaRot); // Move camera
					}
				else
					{
					if (Shift) CameraTrackBrush (Camera,Delta,DeltaRot);
					BrushMoveRot				(Camera,Delta,DeltaRot); // Move brush
					};
				};
			break;
		case EM_BrushFree:		/* Move brush free-form */
			//
			if (Buttons & (BUT_FIRSTHIT | BUT_LASTRELEASE | BUT_SETMODE | BUT_EXITMODE))
				{
				Camera->Actor->Velocity = FVector(0,0,0);
				}
			else
				{
				if (Ctrl && !Shift)
					{
					Temp 	= Shift;
					Shift = !Ctrl;
					Ctrl	= Temp;
					goto CameraMove; // Just want to move camera and not brush!
					};
				if (Camera->IsOrtho())	CalcFreeOrthoMoveRot (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
				else                	CalcFreePerspMoveRot (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
				//
				Delta *= GUnrealEditor.MovementSpeed * 0.25;
				//
				if (Shift || Ctrl) 		CameraMoveRotWithPhysics(Camera,Delta,DeltaRot); // Move camera
   				if (1)				 	BrushMoveRot			(Camera,Delta,DeltaRot); // Move brush
				};
			break;
		case EM_BrushMove:		/* Move brush along one axis at a time */
			//
			if (Buttons & (BUT_FIRSTHIT | BUT_LASTRELEASE | BUT_SETMODE | BUT_EXITMODE))
				{
				Camera->Actor->Velocity = FVector(0,0,0);
				}
			else if( Camera->Input->KeyDown(IK_Alt) )
				{
				goto TextureSet;
				}
			else
				{
				if (Ctrl && !Shift)
					{
					Temp 	= Shift;
					Shift = !Ctrl;
					Ctrl	= Temp;
					goto CameraMove; // Just want to move camera and not brush!
					};
				if (Camera->IsOrtho())	CalcAxialOrthoMove  (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
				else                	CalcAxialPerspMove	(Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
				//
				Delta *= GUnrealEditor.MovementSpeed*0.25;
				//
				if (Shift || Ctrl)	CameraMoveRotWithPhysics(Camera,Delta,DeltaRot); // Move camera
   				if (1)				BrushMoveRot			(Camera,Delta,DeltaRot); // Move brush
				};
			break;
		case EM_BrushRotate:		/* Rotate brush */
			//
			if (Camera->Input->KeyDown(IK_Alt)) goto TextureSet;
			else if (!Ctrl) goto CameraMove;
			//
			NoteBrushMovement(Camera);
   			Brush->Lock(LOCK_Trans);
			//
			CalcAxialPerspMove(Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
			//
			Brush->Rotation.Pitch += Delta.X * 4.0;
			Brush->Rotation.Yaw   += Delta.Y * 4.0;
			Brush->Rotation.Roll  += Delta.Z * 4.0;
			//
			Brush->Unlock(LOCK_Trans);
			break;
		case EM_BrushSheer: /* Sheer brush */
			//
			if (Camera->Input->KeyDown(IK_Alt)) goto TextureSet;
			else if (!Ctrl) goto CameraMove;
			//
			NoteBrushMovement(Camera);
   			Brush->Lock(LOCK_Trans);
			Brush->Scale.SheerRate = Clamp(Brush->Scale.SheerRate + (FLOAT)(-MouseY) / 240.0,-4.0,4.0);
			Brush->Unlock(LOCK_Trans);
			break;
		case EM_BrushScale:	/* Scale brush (proportionally along all axes) */
			//			  
			if (Camera->Input->KeyDown(IK_Alt)) goto TextureSet;
			else if (!Ctrl) goto CameraMove;
			//
			NoteBrushMovement(Camera);
   			Brush->Lock(LOCK_Trans);
			Vector  = Brush->Scale.Scale * (1 + (FLOAT)(-MouseY) / 256.0);
			//
			if (ScaleIsWithinBounds(&Vector,0.05,400.0)) Brush->Scale.Scale = Vector;
			Brush->Unlock(LOCK_Trans);
			break;
		case EM_BrushStretch: /* Stretch brush axially */
			//
			if (Camera->Input->KeyDown(IK_Alt)) goto TextureSet;
			else if (!Ctrl) goto CameraMove;
			//
			NoteBrushMovement(Camera);
   			Brush->Lock(LOCK_Trans);
			//
			if (Camera->IsOrtho())	CalcAxialOrthoMove (Camera,MouseX,-MouseY,Buttons,Delta,DeltaRot);
			else					CalcAxialPerspMove (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
			//
			Vector = Brush->Scale.Scale;
			Vector.X *= (1 + Delta.X / 256.0);
			Vector.Y *= (1 + Delta.Y / 256.0);
			Vector.Z *= (1 + Delta.Z / 256.0);
			//
			if (ScaleIsWithinBounds(&Vector,0.05,400.0)) Brush->Scale.Scale = Vector;
			Brush->Unlock(LOCK_Trans);
			break;
		case EM_BrushSnap: /* Scale brush snapped to grid */
			if (Camera->Input->KeyDown(IK_Alt)) goto TextureSet;
			else if (!Ctrl) goto CameraMove;
			//
			NoteBrushMovement(Camera);
   			Brush->Lock(LOCK_Trans);
			//
			if (Camera->IsOrtho())	CalcAxialOrthoMove (Camera,MouseX,-MouseY,Buttons,Delta,DeltaRot);
			else					CalcAxialPerspMove (Camera,MouseX,MouseY,Buttons,Delta,DeltaRot);
			//
			Vector = Brush->TempScale.Scale;
			Vector.X *= (1 + Delta.X / 400.0);
			Vector.Y *= (1 + Delta.Y / 400.0);
			Vector.Z *= (1 + Delta.Z / 400.0);
			//
			if (ScaleIsWithinBounds(&Vector,0.05,400.0))
				{
				Brush->TempScale.Scale = Vector;
				Brush->PostScale.Scale = Vector;
				//
				if (Camera->Level->Brush()->Polys->Num==0) break;
				//
   				Brush->Unlock(LOCK_Trans);
				Brush->BuildBound(1);
				FVector BoxMin=Camera->Level->Brush()->TransformedBound.Min;
				FVector BoxMax=Camera->Level->Brush()->TransformedBound.Max;
   				Brush->Lock(LOCK_Trans);
				//
				SnapMin   = BoxMin; constrainSimplePoint (&SnapMin,&GUnrealEditor.Constraints);
				DeltaMin  = Brush->Location + Brush->PostPivot - SnapMin;
				DeltaFree = Brush->Location + Brush->PostPivot - BoxMin;
				SnapMin.X = Brush->PostScale.Scale.X * DeltaMin.X/DeltaFree.X;
				SnapMin.Y = Brush->PostScale.Scale.Y * DeltaMin.Y/DeltaFree.Y;
				SnapMin.Z = Brush->PostScale.Scale.Z * DeltaMin.Z/DeltaFree.Z;
				//
				SnapMax   = BoxMax; constrainSimplePoint (&SnapMax,&GUnrealEditor.Constraints);
				DeltaMax  = Brush->Location + Brush->PostPivot - SnapMax;
				DeltaFree = Brush->Location + Brush->PostPivot - BoxMax;
				SnapMax.X = Brush->PostScale.Scale.X * DeltaMax.X/DeltaFree.X;
				SnapMax.Y = Brush->PostScale.Scale.Y * DeltaMax.Y/DeltaFree.Y;
				SnapMax.Z = Brush->PostScale.Scale.Z * DeltaMax.Z/DeltaFree.Z;
				//
				// Set PostScale so brush extents are grid snapped
				// in all directions of movement:
				//
				if (ForceXSnap || (Delta.X!=0))
					{
					ForceXSnap = 1;
					if ((SnapMin.X>0.05) &&
						((SnapMax.X<=0.05) ||
						(Abs(SnapMin.X-Brush->PostScale.Scale.X) < Abs(SnapMax.X-Brush->PostScale.Scale.X))))
						Brush->PostScale.Scale.X = SnapMin.X;
					else if (SnapMax.X>0.05)
						Brush->PostScale.Scale.X = SnapMax.X;
					};
				if (ForceYSnap || (Delta.Y!=0))
					{
					ForceYSnap = 1;
					if ((SnapMin.Y>0.05) &&
						((SnapMax.Y<=0.05) ||
						(Abs(SnapMin.Y-Brush->PostScale.Scale.Y) < Abs(SnapMax.Y-Brush->PostScale.Scale.Y))))
						Brush->PostScale.Scale.Y = SnapMin.Y;
					else if (SnapMax.Y>0.05)
						Brush->PostScale.Scale.Y = SnapMax.Y;
					};
				if (ForceZSnap || (Delta.Z!=0))
					{
					ForceZSnap = 1;
					if ((SnapMin.Z>0.05) &&
						((SnapMax.Z<=0.05) ||
						(Abs(SnapMin.Z-Brush->PostScale.Scale.Z) < Abs(SnapMax.Z-Brush->PostScale.Scale.Z))))
						Brush->PostScale.Scale.Z = SnapMin.Z;
					else if (SnapMax.Z>0.05)
						Brush->PostScale.Scale.Z = SnapMax.Z;
					};
				};
			Brush->Unlock(LOCK_Trans);
			break;
		case EM_MoveActor:		/* Move actor/light */
		case EM_AddActor:		/* Add actor/light */
			if( Buttons & (BUT_FIRSTHIT | BUT_LASTRELEASE | BUT_SETMODE | BUT_EXITMODE) )
			{
				Camera->Actor->Velocity = FVector(0,0,0);
			}
			else
			{
   				if( !Ctrl )
				{
					// Move camera.
					if( Camera->IsOrtho() )	CalcFreeOrthoMoveRot( Camera, MouseX, MouseY, Buttons, Delta, DeltaRot );
					else                	CalcFreePerspMoveRot( Camera, MouseX, MouseY, Buttons, Delta, DeltaRot );

					Delta *= GUnrealEditor.MovementSpeed * 0.25;

					CameraMoveRotWithPhysics( Camera, Delta, DeltaRot );
				}
				else if( MouseX || MouseY )
				{
					// Move actor.
					NoteActorMovement( Camera );

					if( Camera->IsOrtho() )	CalcFreeOrthoMoveRot( Camera, MouseX, MouseY, Buttons, Delta, DeltaRot );
					else                	CalcAxialPerspMove  ( Camera, MouseX, MouseY, Buttons, Delta, DeltaRot );

					Delta *= GUnrealEditor.MovementSpeed * 0.25;

					DeltaRot.Pitch	*= 2;
					DeltaRot.Yaw	*= 2;
					DeltaRot.Roll	*= 2;

					if( Shift )
						CameraMoveRot( Camera, Delta, DeltaRot );
   					edactMoveSelected(Camera->Level,Delta,DeltaRot);
				}
			}
			break;
		case EM_TexturePan:		/* Pan/scale textures */
			//
			if (!Ctrl) goto CameraMove;
			//
			NoteTextureMovement (Camera);
			//
			if (Buttons == (BUT_LEFT | BUT_RIGHT)) // Scale
				{
				GFixScale += Fix(MouseY)/64;
				//
				TempFloat = 1.0;
				Temp = Unfix(GFixScale); 
				while (Temp>0) {TempFloat *= 0.5; Temp--;};
				while (Temp<0) {TempFloat *= 2.0; Temp++;};
				//
				if (Buttons & BUT_LEFT)		TempU = TempFloat; else TempU = 1.0;
				if (Buttons & BUT_RIGHT)	TempV = TempFloat; else TempV = 1.0;
				//
				if ((TempU != 1.0) || (TempV != 1.0))
					{
					GUnrealEditor.polyTexScale(Camera->Level->Model,TempU,0.0,0.0,TempV,0);
					};
				GFixScale &= 0xffff;
				}
			else if (Buttons == BUT_LEFT)
				{
				GFixPanU += Fix(MouseX)/16;  GFixPanV += Fix(MouseY)/16;
				GUnrealEditor.polyTexPan(Camera->Level->Model,Unfix(GFixPanU),Unfix(0),0);
				GFixPanU &= 0xffff; GFixPanV &= 0xffff;
				}
			else // Right
				{
				GFixPanU += Fix(MouseX)/16;  GFixPanV += Fix(MouseY)/16;
				GUnrealEditor.polyTexPan(Camera->Level->Model,Unfix(0),Unfix(GFixPanV),0);
				GFixPanU &= 0xffff; GFixPanV &= 0xffff;
				}
			break;
		case EM_TextureSet:		/* Set textures */
			//
			TextureSet:
			goto CameraMove;
			//
			break;
		case EM_TextureRotate:	/* Rotate textures */
			{
			if (!Ctrl) goto CameraMove;
			NoteTextureMovement (Camera);
			//
			TextureAngle += (FLOAT)MouseX / 256.0;
			//
			for (int i=0; i<Camera->Level->Model->Surfs->Num; i++)
				{
				FBspSurf *Surf = &Camera->Level->Model->Surfs(i);
				if (Surf->PolyFlags & PF_Selected)
					{
					FVector U		= Camera->Level->Model->Vectors(OriginalUVectors[i]);
					FVector V		= Camera->Level->Model->Vectors(OriginalVVectors[i]);
					//
					FVector *NewU	= &Camera->Level->Model->Vectors(Surf->vTextureU);
					FVector *NewV	= &Camera->Level->Model->Vectors(Surf->vTextureV);
					//
					*NewU			= U * cos(TextureAngle) + V * sin(TextureAngle);
					*NewV			= V * cos(TextureAngle) - U * sin(TextureAngle);
					};
				};
			};
			break;
		case EM_BrushWarp:		/* Move brush verts */
			//
			goto CameraMove;
			//
			break;
		case EM_Terraform:		/* Terrain editing */
			//
			goto CameraMove;
			//
			break;
		default:
			//
			debugf(LOG_Problem,"Unknown editor mode %i",GUnrealEditor.Mode);
			goto CameraMove;
			break;
		};
	if (Camera->Actor->RendMap != REN_MeshView)
		{
		Camera->Actor->Rotation = Camera->Actor->ViewRotation;
		};
	Camera->Unlock(LOCK_ReadWrite,0);
	//
	unguardf(("(Mode=%i)",GUnrealEditor.Mode));
	};

/*-----------------------------------------------------------------------------
   Keypress handling.
-----------------------------------------------------------------------------*/

//
// Handle a regular ASCII key that was pressed in UnrealEd.
// Returns 1 if proceesed, 0 if not.
//
int FGlobalEditor::edcamKey( UCamera *Camera, int Key )
{
	guard(FGlobalEditor::edcamKey);
	ULevel		*Level;
	int			ModeClass;
	int 		Processed=0;

	if( Camera->IsBrowser() )
		return 0;

	ModeClass = GUnrealEditor.edcamModeClass(GUnrealEditor.edcamMode(Camera));
	Level     = Camera->Level;

	if( Camera->Input->KeyDown(IK_Shift) || (Key==IK_Delete) )
	{
		// Selection functions.
		if( ModeClass == EMC_Actor ) switch( toupper(Key) )
		{
			case 'A': GUnrealEditor.Exec  ("ACTOR SELECT ALL");		return 1;
			case 'N': GUnrealEditor.Exec  ("ACTOR SELECT NONE");	return 1;
			case 'Z': GUnrealEditor.Exec  ("ACTOR SELECT NONE");	return 1;
			case 'Y': GUnrealEditor.Exec  ("ACTOR DELETE");			return 1;
			case 'D': GUnrealEditor.Exec  ("ACTOR DUPLICATE");		return 1;
			case IK_Delete: GUnrealEditor.Exec("ACTOR DELETE");		return 1;			
		}
		else if( GUnrealEditor.MapEdit ) switch( toupper(Key) )
		{
			case 'A': GUnrealEditor.Exec  ("MAP SELECT ALL");		return 1;
			case 'N': GUnrealEditor.Exec  ("MAP SELECT NONE");		return 1;
			case 'Z': GUnrealEditor.Exec  ("MAP SELECT NONE");		return 1;
			case 'Y': GUnrealEditor.Exec  ("MAP DELETE");			return 1;
			case 'D': GUnrealEditor.Exec  ("MAP DUPLICATE");		return 1;
			case 'F': GUnrealEditor.Exec  ("MAP SELECT FIRST");		return 1;
			case 'L': GUnrealEditor.Exec  ("MAP SELECT LAST");		return 1;
			case 'G': GUnrealEditor.Exec  ("MAP BRUSH GET");		return 1;
			case 'P': GUnrealEditor.Exec  ("MAP BRUSH PUT");		return 1;
			case 'S': GUnrealEditor.Exec  ("MAP SELECT NEXT");		return 1;
			case 'X': GUnrealEditor.Exec  ("MAP SELECT PREVIOUS"); 	return 1;
			case IK_Delete: GUnrealEditor.Exec  ("MAP DELETE");		return 1;
		}
		else switch( toupper(Key) )
		{
			case 'A': GUnrealEditor.Exec  ("POLY SELECT ALL"); 				return 1;
			case 'N': GUnrealEditor.Exec  ("POLY SELECT NONE"); 			return 1;
			case 'Z': GUnrealEditor.Exec  ("POLY SELECT NONE"); 			return 1;
			case 'G': GUnrealEditor.Exec  ("POLY SELECT MATCHING GROUPS"); 	return 1;
			case 'I': GUnrealEditor.Exec  ("POLY SELECT MATCHING ITEMS"); 	return 1;
			case 'C': GUnrealEditor.Exec  ("POLY SELECT ADJACENT COPLANARS"); return 1;
			case 'J': GUnrealEditor.Exec  ("POLY SELECT ADJACENT ALL"); 	return 1;
			case 'W': GUnrealEditor.Exec  ("POLY SELECT ADJACENT WALLS"); 	return 1;
			case 'F': GUnrealEditor.Exec  ("POLY SELECT ADJACENT FLOORS"); 	return 1;
			case 'S': GUnrealEditor.Exec  ("POLY SELECT ADJACENT SLANTS"); 	return 1;
			case 'B': GUnrealEditor.Exec  ("POLY SELECT MATCHING BRUSH"); 	return 1;
			case 'T': GUnrealEditor.Exec  ("POLY SELECT MATCHING TEXTURE"); return 1;
			case 'Q': GUnrealEditor.Exec  ("POLY SELECT REVERSE");			return 1;
			case 'M': GUnrealEditor.Exec  ("POLY SELECT MEMORY SET"); 		return 1;
			case 'R': GUnrealEditor.Exec  ("POLY SELECT MEMORY RECALL"); 	return 1;
			case 'X': GUnrealEditor.Exec  ("POLY SELECT MEMORY XOR"); 		return 1;
			case 'U': GUnrealEditor.Exec  ("POLY SELECT MEMORY UNION"); 	return 1;
			case 'O': GUnrealEditor.Exec  ("POLY SELECT MEMORY INTERSECT"); return 1;
		}
	}
	else if( !Camera->Input->KeyDown(IK_Alt) )
	{
		if( Camera->Lock(LOCK_ReadWrite|LOCK_CanFail) )
		{
			switch( toupper(Key) )
			{
				case 'B': 
					// Toggle brush visibility.
					Camera->Actor->ShowFlags ^= SHOW_Brush;
					Processed=1;
					break;
				case 'K':
					// Toggle backdrop.
					Camera->Actor->ShowFlags ^= SHOW_Backdrop;
					Processed=1;
					break;
				case 'P':
					// Toggle player controls.
					Camera->Actor->ShowFlags ^= SHOW_PlayerCtrl;
					Processed=1;
					break;
				case 'H':
					// Hide actors.
					Camera->Actor->ShowFlags ^= SHOW_Actors;
					break;
				case 'U':
					// Undo.
					Camera->Unlock(LOCK_ReadWrite,0);
					if (GTrans->Undo(Camera->Level)) GCameraManager->RedrawLevel (Level);
					Camera->Lock(LOCK_ReadWrite);
					Processed=1;
					break;
				case 'R':
					// Redo.
					Camera->Unlock(LOCK_ReadWrite,0);
					if (GTrans->Redo(Camera->Level)) GCameraManager->RedrawLevel (Level);
					Camera->Lock(LOCK_ReadWrite);
					Processed=1;
					break;
				case 'L':
					// Look ahead.
		        	Camera->Actor->ViewRotation.Pitch = 0;
		        	Camera->Actor->ViewRotation.Roll  = 0;
					Processed=1;
					break;
				case '1':
					// Movement speed.
					GUnrealEditor.MovementSpeed = 1;
					Processed=1;
					break;
				case '2':
					// Movement speed.
					GUnrealEditor.MovementSpeed = 4;
					Processed=1;
					break;
				case '3':
					// Movement speed.
					GUnrealEditor.MovementSpeed = 16;
					Processed=1;
					break;
				case IK_Escape:
					// Escape, update screen.
					Processed=1;
					break;
				case IK_Delete:
					Camera->Unlock(LOCK_ReadWrite,0);
					if (ModeClass==EMC_Actor) GUnrealEditor.Exec  ("ACTOR DELETE");
					Camera->Lock(LOCK_ReadWrite);
					Processed=1;
					break;
			}
			Camera->Unlock(LOCK_ReadWrite,0);
		}
	}
	return Processed;
	unguard;
}

/*-----------------------------------------------------------------------------
   Coordinates.
-----------------------------------------------------------------------------*/

void cameraShowCoords(UCamera *Camera)
	{
	FVector			Location;
	FRotation		Rotation;
	UModel			*Brush;
	int				ModeClass,X,Y,State,i,n;
	char			XStr[40],YStr[40],ZStr[40],Descr[40],Fail[40];
	//
	guard(cameraShowCoords);
	ModeClass 	= GUnrealEditor.edcamModeClass (GUnrealEditor.Mode);
	State       = Camera->Level->GetState();
	Fail[0]     = 0;
	//
	if ((Camera->Actor->ShowFlags&SHOW_PlayerCtrl) || (State==LEVEL_UpPlay))
		{
		sprintf (Descr,"%s",Camera->Actor->GetClassName());
		if( Camera->Actor->GetFName() != NAME_None )
			sprintf (Descr+strlen(Descr)," %s",Camera->Actor->GetName());

		Location = Camera->Actor->Location;
		}
	else if (ModeClass==EMC_Actor) // Location of first selected actor
		{
		for (i=0; i<Camera->Level->Num; i++)
			{
			AActor *Actor = Camera->Level->Element(i);
			if (Actor && Actor->bSelected)
				{
				sprintf (Descr,"%s",Actor->GetClassName());
				if( Actor->GetFName() != NAME_None )
					sprintf (Descr+strlen(Descr)," %s",Actor->GetName());

				Location = Actor->Location;
				Rotation = Actor->Rotation;
				goto Found;
				};
			};
		strcpy (Descr,"Actor");
		strcpy (Fail, "None selected");
		}
	else  
		{ 
		if (GUnrealEditor.MapEdit) // Location of first selected brush
			{
			n = Camera->Level->BrushArray->Num;
			//
			for (i=0; i<n; i++)
				{
				Brush = Camera->Level->BrushArray->Element(i);
				if (Brush->ModelFlags & MF_Selected)
					{
					Location = Brush->Location;
					Rotation = Brush->Rotation;
					strcpy (Descr,GUnrealEditor.csgGetName((ECsgOper)Brush->CsgOper));
					goto Found;
					};
				};
			};
		//
		// Location of default brush:
		//
		Brush = Camera->Level->Brush();
		Brush->Lock(LOCK_Read);
		strcpy (Descr,GUnrealEditor.csgGetName((ECsgOper)Brush->CsgOper));
		//
		Location = Brush->Location;
		Rotation = Brush->Rotation;
		GUnrealEditor.constraintApply(Camera->Level->Model,Brush,&Location,&Rotation,&GUnrealEditor.Constraints);
		//
		Brush->Unlock(LOCK_Read);
		};
	Found:
	//
	sprintf (XStr,"X=%05i",(int)Location.X);
	sprintf (YStr,"Y=%05i",(int)Location.Y);
	sprintf (ZStr,"Z=%05i",(int)Location.Z);
	//
	X = 12;
	Y = Camera->Y - 24;
	//
	GGfx.MedFont->Printf(Camera->Texture,X+0,Y,0,Descr);
	//
	if (Fail[0]!=0)
		{
		GGfx.MedFont->Printf(Camera->Texture,X+100,Y,0,Fail);
		}
	else
		{
		GGfx.MedFont->Printf (Camera->Texture,X+105,Y,0,XStr);
		GGfx.MedFont->Printf (Camera->Texture,X+170,Y,0,YStr);
		GGfx.MedFont->Printf (Camera->Texture,X+235,Y,0,ZStr);
		};
	unguard;
	};

/*-----------------------------------------------------------------------------
   Texture browser routines.
-----------------------------------------------------------------------------*/

void DrawViewerBackground( UCamera *Camera )
{
	guard(DrawViewerBackground);
	GRend->DrawTiledTextureBlock(Camera,GGfx.BkgndTexture,0,Camera->X,0,Camera->Y,0,0);
	//Alternatively: GGfx.Clearscreen(&Camera,0);
	unguard;
}

void DrawTextureRect
(
	UCamera		*Camera,
	UTexture	*Texture,
	int			X,
	int			Y,
	int			Size,
	int			DoText,
	int			Highlight
)
{
	guard(DrawTextureRect);
	checkState(Texture->Palette!=NULL);

	FTextureInfo TextureInfo;
	Texture->Lock(TextureInfo,Camera->Texture,TL_RenderPalette);
	RAINBOW_PTR Colors = TextureInfo.Colors;

	BYTE	*TextureBits	= TextureInfo.Mips[0]->Data;
	INT		USize			= TextureInfo.Mips[0]->USize;
	INT		VSize			= TextureInfo.Mips[0]->VSize;

	BYTE			VShift,B;
	int				OrigY=Y,U,U1=1,V=0,XL=Size,YL=Size,i,j,VOfs,UMask,VMask,D;
	char			Temp[80];

	VMask		= VSize-1;
	UMask		= USize-1;
	VShift		= FLogTwo(USize);
	D			= Fix(Max(USize,VSize))/Size;

	if (Y<0) {V -=Y*D; YL+=Y; Y=0;};
	if (X<0) {U1-=X*D; XL+=X; X=0;};
	if ((Y+YL)>Camera->Y) YL = Camera->Y-Y;
	if ((X+XL)>Camera->X) XL = Camera->X-X;

	RAINBOW_PTR Dest,Dest1;
	Dest1.PtrBYTE = &Camera->Screen[(X + Y*Camera->Stride)*Camera->ColorBytes];

	for( i=0; i<YL; i++ )
	{
		Dest = Dest1;
		U    = U1;
		VOfs = (Unfix(V) & VMask) << VShift;
		if( Camera->ColorBytes==1 )
		{
			if( !(Texture->PolyFlags & PF_Masked)) for (j=0; j<XL; j++ )
			{
				*Dest.PtrBYTE++ = Colors.PtrBYTE[TextureBits [VOfs + (Unfix(U)&UMask)]];
				U += D;
			}
			else for( j=0; j<XL; j++ )
			{
				B = TextureBits [VOfs + (Unfix(U)&UMask)];
				if (B) *Dest.PtrBYTE = Colors.PtrBYTE[B];
				Dest.PtrBYTE++; U += D;
			}
		}
		else if( Camera->ColorBytes==2 )
		{
			if( !(Texture->PolyFlags & PF_Masked) ) for( j=0; j<XL; j++ )
			{
				*Dest.PtrWORD++ = Colors.PtrWORD[TextureBits [VOfs + (Unfix(U)&UMask)]];
				U += D;
			}
			else for( j=0; j<XL; j++ )
			{
				B = TextureBits [VOfs + (Unfix(U)&UMask)];
				if (B) *Dest.PtrWORD = Colors.PtrWORD[B];
				Dest.PtrWORD++; U += D;
			}
		}
		else if( Camera->ColorBytes==4 )
		{
			if( !(Texture->PolyFlags & PF_Masked) ) for( j=0; j<XL; j++ )
			{
				*Dest.PtrDWORD++ = Colors.PtrDWORD[TextureBits [VOfs + (Unfix(U)&UMask)]];
				U += D;
			}
			else for( j=0; j<XL; j++ )
			{
				B = TextureBits [VOfs + (Unfix(U)&UMask)];
				if (B) *Dest.PtrDWORD = Colors.PtrDWORD[B];
				Dest.PtrDWORD++; U += D;
			}
		}
		V += D;
		Dest1.PtrBYTE += Camera->ByteStride;
	}
	if( DoText )
	{
		strcpy(Temp,Texture->GetName());
		if (Size>=128) sprintf(Temp+strlen(Temp)," (%ix%i)",Texture->USize,Texture->VSize);
		GGfx.MedFont->StrLen (XL,YL,(Size>=128)?-1:0,0,Temp);
		X = X+(Size-XL)/2;
		GGfx.MedFont->Printf(Camera->Texture,X,OrigY+Size+1,-1,Temp);
	}
	Texture->Unlock(TextureInfo);
	unguardf(("(%s)",Texture->GetName()));
}

int CDECL ResNameCompare(const void *A, const void *B)
{
	return stricmp((*(UObject **)A)->GetName(),(*(UObject **)B)->GetName());
}

void DrawTextureBrowser( UCamera *Camera )
{
	guard(DrawTextureBrowser);

	TArray<UTexture*>* Array = (TArray<UTexture*>*)Camera->MiscRes;

	FMemMark Mark(GMem);
	enum {MAX=16384};
	UTexture	**List  = new(GMem,MAX)UTexture*;
	INT			Size	= Camera->Actor->Misc1;
	INT			PerRow	= Camera->X/Size;
	INT			Space	= (Camera->X - Size*PerRow)/(PerRow+1);
	INT			VSkip	= (Size>=64) ? 10 : 0;

	// Make the list.
	int n = 0;
	if( !Array )
	{
		// All textures.
		UTexture *Texture;
		FOR_ALL_TYPED_OBJECTS(Texture,UTexture)
		{
			if( n<MAX && !(Texture->TextureFlags & TF_NoTile) )
				List[n++] = Texture;
		}
		END_FOR_ALL_TYPED_OBJECTS;
	}
	else
	{
		// Just textures in this texture set.
		for(int i=0; i<Array->Num; i++)
		{
			if( n<MAX && !(Array->Element(i)->TextureFlags & TF_NoTile) )
				List[n++] = Array->Element(i);
		}
	}

	// Sort textures by name.
	qsort(&List[0],n,sizeof(UTexture *),ResNameCompare);

	// Draw them.
	int YL = Space+(Size+Space+VSkip)*((n+PerRow-1)/PerRow);
	if( YL > 0 )
	{
		int YOfs = -((Camera->Actor->Misc2*Camera->Y)/512);
		for( int i=0; i<n; i++ )
		{
			int X = (Size+Space)*(i%PerRow);
			int Y = (Size+Space+VSkip)*(i/PerRow)+YOfs;

			if( ((Y+Size+Space+VSkip)>0) && (Y<Camera->Y) )
			{
				if( GUnrealEditor.Scan.Active )
					GUnrealEditor.Scan.PreScan();

				if( List[i]==GUnrealEditor.CurrentTexture )
					Camera->Texture->BurnRect(X+1,X+Size+Space*2-2,Y+1,Y+Size+Space*2+VSkip-2,1);

				DrawTextureRect( Camera, List[i], X+Space, Y+Space, Size, (Size>=64), 0 );

				if( GUnrealEditor.Scan.Active )
					GUnrealEditor.Scan.PostScan(EDSCAN_BrowserTex,(int)List[i],0,0,&FVector(0,0,0));
			}
		}
	}
	Mark.Pop();
	GLastScroll = Max(0,(512*(YL-Camera->Y))/Camera->Y);
	unguardf(("(%s)",Camera->MiscRes->GetName()));
}

/*-----------------------------------------------------------------------------
   Camera frame drawing.
-----------------------------------------------------------------------------*/

//
// Draw an onscreen mouseable button.
//
int DrawButton( UCamera *Camera, UTexture *Texture, int X, int Y, int ScanCode )
{
	if (GUnrealEditor.Scan.Active)
		GUnrealEditor.Scan.PreScan();
	GRend->DrawScaledSprite
	(
		Camera,
		Texture,
		X+0.5,
		Y+0.5,
		Texture->USize,
		Texture->VSize,
		BT_Normal,
		NULL,
		0,
		0,
		0.0
	);
	if (GUnrealEditor.Scan.Active)
		GUnrealEditor.Scan.PostScan(EDSCAN_UIElement,ScanCode,0,0,&FVector(0,0,0));
	return Texture->USize+2;
	};

//
// Draw the camera view:
//
void FGlobalEditor::edcamDraw( UCamera *Camera, int DoScan )
{
	FVector				OriginalLocation;
	FRotation			OriginalRotation;
	DWORD				ShowFlags=0;
	char				Temp[80];
	int					InvalidBsp,ButtonX=2,Mode=0,ModeClass,RealClass;
	static FLOAT		LastTimeSeconds=0.0;
	guard(FGlobalEditor::edcamDraw);

	// Lock the camera.
	if( !Camera->Lock(LOCK_ReadWrite|LOCK_CanFail) )
	{
		debug(LOG_Problem,"Couldn't lock camera for drawing");
		return;
	}

	// Init scanner if desired.
	if( DoScan )
		Scan.Init(Camera); // Caller must set Scan.X, Scan.Y.
	else
		Scan.Active = 0;

	GRend->PreRender(Camera);
	Camera->Console->PreRender(Camera);
	GGfx.PreRender(Camera);

	APawn *Actor			= Camera->Actor;
	ShowFlags				= Actor->ShowFlags;

	switch( Actor->RendMap )
	{
		case REN_TexView:
		{
			Actor->bHiddenEd = 1;
			Actor->bHidden   = 1;
			DrawViewerBackground(Camera);
			DrawTextureRect(Camera,(UTexture *)Camera->MiscRes,4,4,128,128,1);
			goto Out;
		}
		case REN_TexBrowser:
		{
			Actor->bHiddenEd = 1;
			Actor->bHidden   = 1;
			DrawViewerBackground(Camera);
			DrawTextureBrowser(Camera);
			goto Out;
		}
		case REN_MeshView:
		{
			Camera->Texture->Clearscreen(0xe8);

			FVector NewLocation = Camera->Coords.ZAxis * (-Actor->Location.Size());
			if( FDist(Actor->Location, NewLocation) > 0.05 )
				Actor->Location = NewLocation;

			UMesh *Mesh = (UMesh*)Camera->MiscRes;
			if( Actor->Misc1 < Mesh->AnimSeqs->Num )
				Actor->AnimSequence = Mesh->AnimSeqs( Actor->Misc1 ).Name;
			else
				Actor->AnimSequence = NAME_None;

			const FMeshAnimSeq *Seq = Mesh->GetAnimSeq( Actor->AnimSequence );
			Camera->BuildCoords();

			// Auto rotate if wanted.
			if( (Actor->ShowFlags & SHOW_Brush) && (GServer.TimeSeconds - LastTimeSeconds < 0.25) )
				Actor->ViewRotation.Yaw += (GServer.TimeSeconds - LastTimeSeconds) * 8192.0;

			// Remember.
			OriginalLocation		= Actor->Location;
			OriginalRotation		= Actor->ViewRotation;
			Actor->Location			= FVector(0,0,0);
			Actor->bHiddenEd		= 1;
			Actor->bHidden			= 1;
			Actor->DrawType			= DT_Mesh;
			Actor->Mesh				= Mesh;
			Actor->ZoneNumber		= 0;
			Actor->Zone				= NULL;
			Actor->bCollideWorld	= 0;
			Actor->bCollideActors	= 0;

			// Update mesh.
			Mesh->Lock(LOCK_Read);
			FLOAT NumFrames = Seq ? Seq->NumFrames : 1.0;
			if( ShowFlags & SHOW_Backdrop )
			{
				Actor->AnimFrame += Min( 1.0, (GServer.TimeSeconds-LastTimeSeconds)*30.0 / NumFrames);
				Actor->AnimFrame -= floor(Actor->AnimFrame);
			}
			else Actor->AnimFrame = Actor->Misc2 / NumFrames;

			LastTimeSeconds = GServer.TimeSeconds;

			if     ( ShowFlags & SHOW_Frame  )	Camera->Actor->RendMap = REN_Wire;
			else if( ShowFlags & SHOW_Coords )	Camera->Actor->RendMap = REN_Polys;
			else								Camera->Actor->RendMap = REN_PlainTex;

			Mesh->Unlock(LOCK_Read);

			// Draw it.
			GRend->DrawActor( Camera, (AActor*)Camera->Actor );
			sprintf
			(
				Temp,"%s, Seq %i, Frame %i",
				Camera->MiscRes->GetName(),
				Camera->Actor->Misc1,
				(int)(Actor->AnimFrame / NumFrames)
			);
			GGfx.MedFont->Printf( Camera->Texture, 4, Camera->Y-12, -1, Temp );

			Camera->Actor->RendMap	= REN_MeshView;
			Actor->Location			= OriginalLocation;
			Actor->DrawType			= DT_None;
			goto Out;
		}
		case REN_MeshBrowser:
		{
			DrawViewerBackground(Camera);
			goto Out;
		}
	}
	Mode 		= edcamMode(Camera);
	ModeClass	= edcamModeClass(Mode);
	RealClass   = edcamModeClass(Mode);

	if( Camera->IsOrtho() || (Actor->ShowFlags & SHOW_NoCapture) )
		Actor->bHiddenEd=1;
	else
		Actor->bHiddenEd=0;

	if( Camera->Actor->ShowFlags & SHOW_PlayerCtrl )
	{
		OriginalLocation = Actor->Location;
		OriginalRotation = Actor->ViewRotation;

		PCalcView ViewInfo( Actor->Location, Actor->ViewRotation );
		Camera->Actor->Process( NAME_PlayerCalcView, &ViewInfo );
		Actor->Location			= ViewInfo.CameraLocation;
		Actor->ViewRotation		= ViewInfo.CameraRotation;

		// Rebuild coordinate system.
		Camera->BuildCoords();
		Actor->Location         = OriginalLocation;
		Actor->ViewRotation     = OriginalRotation;
	}

	// Handle case if Bsp is invalid.
	InvalidBsp = (Camera->Level->Model->ModelFlags & MF_InvalidBsp) &&
		!(Camera->IsOrtho() || Camera->IsRealWire());

	// Draw background.
	if (Camera->IsOrtho() || Camera->IsWire() || 
		Camera->IsInvalidBsp() || !(Camera->Actor->ShowFlags & SHOW_Backdrop))
		{
		if (Camera->IsOrtho())	Camera->Texture->Clearscreen(OrthoBackground);
		else					Camera->Texture->Clearscreen(WireBackground);
		GEdRend->DrawWireBackground(Camera);
		};

	// Draw the level.
	if( Camera->IsOrtho() || Camera->IsWire() || InvalidBsp )
	{
		GEdRend->DrawLevelBrushes(Camera);
		if (Camera->Actor->ShowFlags & SHOW_Actors)
			GEdRend->DrawLevelActors( Camera, (AActor*)Camera->Actor );
		if (Camera->Actor->ShowFlags & SHOW_MovingBrushes)
			GEdRend->DrawMovingBrushWires(Camera);
	}
	else GRend->DrawWorld(Camera);

	if( Camera->Actor->ShowFlags & SHOW_Brush )
	{
		if( ModeClass==EMC_Camera || ModeClass==EMC_Brush )
		{
			GEdRend->DrawActiveBrush(Camera);
			if( GUnrealEditor.Mode==EM_BrushSnap )
			{
				if( !Camera->Level->Brush()->TransformedBound.IsValid )
					Camera->Level->Brush()->BuildBound(1);

				constrainSimplePoint(&Camera->Level->Brush()->TransformedBound.Min,&Constraints);
				constrainSimplePoint(&Camera->Level->Brush()->TransformedBound.Max,&Constraints);

				GEdRend->DrawBoundingBox( Camera, &Camera->Level->Brush()->TransformedBound );
			}
		}
	}

	// Draw status.
	if( Camera->Actor->ShowFlags & SHOW_Coords )
		cameraShowCoords(Camera);
	
	if( InvalidBsp )
		GGfx.MedFont->Printf(Camera->Texture,8,20,0,"Map view - Geometry must be rebuilt");

	if
	(	(Camera->Level->GetState()==LEVEL_UpEdit)
	&&	(!GCameraManager->FullscreenCamera)
	&&	(!(Camera->Actor->ShowFlags & SHOW_NoButtons)) )
	{
		ButtonX +=
			DrawButton(Camera,(Camera->Actor->ShowFlags&SHOW_Menu)?GGfx.MenuUp:GGfx.MenuDn,ButtonX,2,1);
		if (!Camera->IsOrtho()) ButtonX +=
			DrawButton (Camera, (Camera->Actor->ShowFlags&SHOW_PlayerCtrl)?GGfx.PlyrOn:GGfx.PlyrOff,
			ButtonX,2,2);
	}

	Out:
	GGfx.PostRender(Camera);
	Camera->Console->PostRender(Camera,ButtonX);
	GRend->PostRender(Camera);

	Scan.Exit();
	Camera->Unlock(LOCK_ReadWrite,!DoScan);

	unguardf(("(Cam=%s,Mode=%i,Flags=%i",Camera->GetName(),Mode,ShowFlags));
}

/*-----------------------------------------------------------------------------
   Brush pivot setting.
-----------------------------------------------------------------------------*/

//
// Set the brush's pivot point to a location specified in world coordinates.
// This transforms the point into the brush's coordinate system and takes
// care of all the thorny details.
//
void SetWorldPivotPoint( ULevel *Level, UModel *Brush, FVector *PivotLocation, int SnapPivotToGrid )
{
	guard(SetWorldPivotPoint);
	if( GUnrealEditor.MapEdit )
	{
		int n = Level->BrushArray->Num;
		for( int i=0; i<n; i++ )
		{
			Brush = Level->BrushArray->Element(i);
			if( (Brush->ModelFlags&MF_Selected) || (i==0) )
				Brush->SetPivotPoint( PivotLocation, SnapPivotToGrid );
		}
	}
	else if( Brush )
	{
		Brush->SetPivotPoint( PivotLocation, SnapPivotToGrid );
	}
	else Level->Brush()->SetPivotPoint(PivotLocation,SnapPivotToGrid);
	
	unguard;
}

/*-----------------------------------------------------------------------------
   Camera mouse click handling.
-----------------------------------------------------------------------------*/

//
// Handle a mouse click in the camera window.
//
void FGlobalEditor::edcamClick
(
	UCamera *Camera, 
	BYTE Buttons, 
	SWORD MouseX, 
	SWORD MouseY,
	int Shift, 
	int Ctrl
)
{
	guard(FGlobalEditor::edcamClick);
	POLY_CALLBACK	Callback;
	UModel			*Brush;
	FBspNode	  	*Node;
	FBspSurf		*Poly;
	FVector			V;
	AActor			*Actor;
	int				i,UpdateWindow=0;
	int				Mode,ModeClass;
	char			Temp[80];

	GUnrealEditor.Scan.X = MouseX;
	GUnrealEditor.Scan.Y = MouseY;

	Camera->Draw( 1 );

	Mode          = GUnrealEditor.edcamMode(Camera);
	ModeClass     = GUnrealEditor.edcamModeClass(Mode);
	UModel *Model = Camera->Level->Model;

	if( !Camera->Lock(LOCK_ReadWrite|LOCK_CanFail) )
		return;

	// Check scan results.
	if( (Mode == EM_AddActor) && (Buttons == BUT_LEFT)
	&&	(Scan.Type != EDSCAN_Actor)
	&&	(Scan.Type != EDSCAN_UIElement)
	&&	(CurrentClass != NULL))
	{
		guard(AddActor);
		if( CurrentClass->ClassFlags & CLASS_Abstract )
		{
			char Temp[256];
			sprintf(Temp,"Class %s is abstract.  You can't add actors of this type to the world.",CurrentClass->GetName());
			GApp->MessageBox(Temp,"Can't add actor",0);
		}
		else if( GEdRend->Deproject(Camera,MouseX,MouseY,&V,1,CurrentClass->GetDefaultActor().CollisionRadius) )
		{
			Camera->Level->Unlock(LOCK_ReadWrite);
			GTrans->Begin         (Camera->Level,"Add Actor");
			GTrans->NoteResHeader (Camera->Level);
			Camera->Level->Lock(LOCK_Trans);

			if( !CurrentClass )
				CurrentClass = new("Light",FIND_Existing)UClass;
			Actor = Camera->Level->SpawnActor(CurrentClass,NULL,NAME_None,V);

			if( Actor )
			{
				Actor->bDynamicLight = 1;
				if( Camera->Level->FarMoveActor( Actor, V ) )
				{
					debug(LOG_Info,"Added actor successfully");
				}
				else
				{
					debug(LOG_Info,"Actor doesn't fit there");
					Camera->Level->DestroyActor( Actor );
				}
				if( CurrentClass->GetDefaultActor().Brush )
				{
					Actor->Brush = csgDuplicateBrush( Camera->Level, CurrentClass->GetDefaultActor().Brush, 0, 0, 0, 1 );
					Actor->UpdateBrushPosition(Camera->Level,1);
				}
			}
			Camera->Level->Unlock(LOCK_Trans);
			GTrans->End();
			Camera->Level->Lock(LOCK_ReadWrite);
		}
		unguard;
	}
	else switch( GUnrealEditor.Scan.Type )
	{
		case EDSCAN_BspNodePoly:
		{
			guard(EDSCAN_BspNodePoly);
			GTrans->Begin (Camera->Level,"Poly Click");

			Node = &Model->Nodes (GUnrealEditor.Scan.Index);
			Poly = &Model->Surfs (Node->iSurf);

			if( Mode == EM_TextureSet )
			{
				if( Buttons == BUT_RIGHT )
				{
					GUnrealEditor.CurrentTexture = Poly->Texture;
					GApp->EdCallback(EDC_CurTexChange,0);
				}
				else
				{
					if( Camera->Input->KeyDown(IK_Shift) )
					{
						// Set to all selected.
						Model->Surfs->Lock(LOCK_Trans);
						Model->Surfs->ModifySelected(1);
						for( INDEX i=0; i<Model->Surfs->Num; i++ )
						{
							if( Model->Surfs(i).PolyFlags & PF_Selected )
							{
								Model->Surfs(i).Texture = GUnrealEditor.CurrentTexture;
								GUnrealEditor.polyUpdateMaster (Model,i,0,0);
							}
						}
						Model->Surfs->Unlock(LOCK_Trans);
					}
					else
					{
						// Set to the one polygon clicked on.
						Model->Surfs->Lock(LOCK_Trans);
						Model->Surfs->ModifyItem(Node->iSurf,0);
						Model->Surfs->Unlock(LOCK_Trans);
						Poly->Texture = GUnrealEditor.CurrentTexture;
						GUnrealEditor.polyUpdateMaster(Model,Node->iSurf,0,0);
					}
				}
			}
			else
			{
				if( Buttons == BUT_RIGHT ) 
				{
					// If nothing is selected, select just this.
					Model->Surfs->Lock(LOCK_Trans);
					if (!(Poly->PolyFlags & PF_Selected)) GUnrealEditor.polySetAndClearPolyFlags(Model, 0, PF_Selected,0,0);
					GUnrealEditor.polyFindByBrush(Model,Poly->Brush,Poly->iBrushPoly,SelectPolyFunc);
					Model->Surfs->Unlock(LOCK_Trans);

					// Tell editor to bring up the polygon properties dialog.
					GApp->EdCallback(EDC_SelPolyChange,0);
					GApp->EdCallback(EDC_RtClickPoly,0);
				}
				else
				{
					if( Poly->PolyFlags & PF_Selected ) Callback = DeselectPolyFunc;
					else								Callback = SelectPolyFunc;

					Model->Surfs->Lock(LOCK_Trans);
					if( !Camera->Input->KeyDown(IK_Ctrl) )
						GUnrealEditor.polySetAndClearPolyFlags(Model, 0, PF_Selected,0,0);
					GUnrealEditor.polyFindByBrush(Model,Poly->Brush,Poly->iBrushPoly,Callback);
					Model->Surfs->Unlock(LOCK_Trans);

					GApp->EdCallback(EDC_SelPolyChange,0);
				}
			}
			GTrans->End ();
			break;
			unguard;
		}
		case EDSCAN_BspNodeSide:
		{
			guard(EDSCAN_BspNodeSide);
			break;
			unguard;
		}
		case EDSCAN_BspNodeVertex:
		{
			guard(EDSCAN_BspNodeVertex);
			break;
			unguard;
		}
		case EDSCAN_BrushPoly:
		case EDSCAN_BrushSide:
		{
			guard(EDSCAN_BrushPoly);
			if( GUnrealEditor.MapEdit )
			{
				GTrans->Begin( Camera->Level, "Select brush" );
				if( Buttons==BUT_RIGHT )
				{
					for( i=0; i<Camera->Level->BrushArray->Num; i++ )
					{
						Brush = Camera->Level->BrushArray->Element(i);
						if( Brush->ModelFlags & MF_Selected )
						{
							GTrans->NoteResHeader(Brush);
							Brush->ModelFlags &= ~MF_Selected;
						}
					}
				}
				Brush = (UModel *)GUnrealEditor.Scan.Index;
				GTrans->NoteResHeader (Brush);
				Brush->ModelFlags ^= MF_Selected;
				GTrans->End ();
			}
			break;
			unguard;
		}
		case EDSCAN_BrushVertex:
		{
			guard(EDSCAN_BrushVertex);

			UModel *Brush = (UModel *)GUnrealEditor.Scan.Index;
			GTrans->Begin( Camera->Level, "Brush Vertex Selection" );
			SetWorldPivotPoint( Camera->Level, Brush, &GUnrealEditor.Scan.V, (Buttons==BUT_RIGHT) );
			GTrans->End();

			// If this was a moving brush, update its actor accordingly.
			for( int i=0; i<Camera->Level->Num; i++ )
			{
				AActor *Actor = Camera->Level->Element(i);
				if( Actor && Actor->Brush==Brush )
					Actor->UpdateBrushPosition(Camera->Level,1);
			}
			break;
			unguard;
		}
		case EDSCAN_Actor:
		{
			guard(EDSCAN_Actor);
			Actor = (AActor*)GUnrealEditor.Scan.Index;
			if( !Actor->IsA("View") )
			{
				GTrans->Begin(Camera->Level,"clicking on actors");
				if( Buttons == BUT_RIGHT )
				{
					if( !Actor->bSelected )
						GUnrealEditor.edactSelectNone(Camera->Level);

					Actor->Lock(LOCK_Trans);
					Actor->bSelected=1;
					Actor->Unlock(LOCK_Trans);

					GApp->EdCallback(EDC_SelActorChange,0);
					GApp->EdCallback(EDC_RtClickActor,0);
				}
				else
				{
					if( Camera->Input->KeyDown(IK_Alt) )
					{
						// Set default add-class.
						CurrentClass = Actor->GetClass();
						GApp->EdCallback(EDC_CurClassChange,0);
					}
					else
					{
						// Select/deselect actor.
						if( !Camera->Input->KeyDown(IK_Ctrl) )
							edactSelectNone(Camera->Level);

						Actor->Lock(LOCK_Trans);
						Actor->bSelected ^= 1;
						Actor->Unlock(LOCK_Trans);

						GApp->EdCallback(EDC_SelActorChange,0);
					}
				}
				GTrans->End();
			}
			break;
			unguard;
		}
		case EDSCAN_UIElement:
		{
			guard(EDSCAN_UIElement);
			switch(GUnrealEditor.Scan.Index)
			{
				case 1:
					// Menu on/off button.
					Camera->Actor->ShowFlags ^= SHOW_Menu;
					UpdateWindow=1;
					break;
				case 2:
					// Player controls.
					Camera->Actor->ShowFlags ^= SHOW_PlayerCtrl;
					Camera->Logf(LOG_Info,"Player controls are %s",
						Camera->Actor->ShowFlags&SHOW_PlayerCtrl ? "On" : "Off");
					break;
			}
			break;
			unguard;
		}
		case EDSCAN_BrowserTex:
		{
			guard(EDSCAN_BrowserTex);
			Camera->Unlock(LOCK_ReadWrite,0);
			if( Buttons == BUT_LEFT )
			{
				strcpy(Temp,"POLY DEFAULT TEXTURE="); strcat(Temp,((UObject *)GUnrealEditor.Scan.Index)->GetName());
				GUnrealEditor.Exec (Temp);
				strcpy(Temp,"POLY SET TEXTURE="); strcat(Temp,((UObject *)GUnrealEditor.Scan.Index)->GetName());
				GUnrealEditor.Exec (Temp);
				GApp->EdCallback(EDC_CurTexChange,0);
			}
			else if( Buttons == BUT_RIGHT )
			{
				GUnrealEditor.CurrentTexture=(UTexture *)GUnrealEditor.Scan.Index;
				GApp->EdCallback(EDC_RtClickTexture,0);
			}
			if( !Camera->Lock(LOCK_ReadWrite|LOCK_CanFail) )
				return;
			break;
			unguard;
		}
		case EDSCAN_None:
		{
			guard(EDSCAN_None);
			if( Buttons == BUT_RIGHT )
				GApp->EdCallback( EDC_RtClickWindow, 0 );
			break;
			unguard;
		}
	}
	Camera->Unlock(LOCK_ReadWrite,0);

	if( UpdateWindow )
		Camera->UpdateWindow();

	unguardf(("(Mode=%i)",GUnrealEditor.Mode));
}

/*-----------------------------------------------------------------------------
   Editor camera mode.
-----------------------------------------------------------------------------*/

//
// Set the editor mode.
//
void FGlobalEditor::edcamSetMode( int Mode )
{
	int i;

	// Clear old mode.
	guard(FGlobalEditor::edcamSetMode);
	if( GUnrealEditor.Mode != EM_None )
		for( i=0; i<GCameraManager->CameraArray->Num; i++ )
			edcamMove(GCameraManager->CameraArray->Element(i),BUT_EXITMODE,0,0,0,0);

	// Set new mode.
	GUnrealEditor.Mode = Mode;
	if( GUnrealEditor.Mode != EM_None )
		for( i=0; i<GCameraManager->CameraArray->Num; i++ )
			edcamMove (GCameraManager->CameraArray->Element(i),BUT_SETMODE,0,0,0,0);

	unguard;
}

//
// Return editor camera mode given GUnrealEditor.Mode and state of keys.
// This handlers special keyboard mode overrides which should
// affect the appearance of the mouse cursor, etc.
//
int FGlobalEditor::edcamMode( UCamera *Camera )
{
	guard(FGlobalEditor::edcamMode);
	checkInput(Camera!=NULL);

	switch( Camera->Actor->RendMap )
	{
		case REN_TexView:		return EM_TexView;
		case REN_TexBrowser:	return EM_TexBrowser;
		case REN_MeshView:		return EM_MeshView;
		case REN_MeshBrowser:	return EM_MeshBrowser;
	}
	switch( GUnrealEditor.Mode )
	{
		case EM_None:
			return GUnrealEditor.Mode;
		case EM_CameraMove:
		case EM_CameraZoom:
			if( Camera->Input->KeyDown(IK_Alt) ) return EM_TextureSet;
			else if( Camera->Input->KeyDown(IK_Ctrl) ) return EM_BrushMove;
			else if( Camera->Input->KeyDown(IK_Shift) ) return EM_BrushMove;
			else return GUnrealEditor.Mode;
			break;
		case EM_BrushFree:
		case EM_BrushMove:
		case EM_BrushRotate:
		case EM_BrushSheer:
		case EM_BrushScale:
		case EM_BrushStretch:
		case EM_BrushWarp:
		case EM_BrushSnap:
			if( Camera->Input->KeyDown(IK_Alt) ) return EM_TextureSet;
			else if( Camera->Input->KeyDown(IK_Ctrl) ) return GUnrealEditor.Mode;
			else if( Camera->Input->KeyDown(IK_Shift) ) return EM_BrushMove;
			else return EM_CameraMove;
			break;
		case EM_AddActor:
		case EM_MoveActor:
			return GUnrealEditor.Mode;
			break;
		case EM_TextureSet:
			if( Camera->Input->KeyDown(IK_Ctrl) ) return EM_CameraMove;
			else return GUnrealEditor.Mode;
		case EM_TexturePan:
		case EM_TextureRotate:
		case EM_TextureScale:
			if( Camera->Input->KeyDown(IK_Alt) ) return EM_TextureSet;
			else return GUnrealEditor.Mode;
			break;
		case EM_Terraform:
			return GUnrealEditor.Mode;
			break;
		default:
			return GUnrealEditor.Mode;
	}
	unguard;
}

//
// Return classification of current mode:
//
int FGlobalEditor::edcamModeClass (int Mode)
	{
	switch (Mode)
		{
		case EM_CameraMove:
		case EM_CameraZoom:
			return EMC_Camera;
		case EM_BrushFree:
		case EM_BrushMove:
		case EM_BrushRotate:
		case EM_BrushSheer:
		case EM_BrushScale:
		case EM_BrushStretch:
		case EM_BrushSnap:
		case EM_BrushWarp:
			return EMC_Brush;
		case EM_AddActor:
		case EM_MoveActor:
			return EMC_Actor;
		case EM_TextureSet:
		case EM_TexturePan:
		case EM_TextureRotate:
		case EM_TextureScale:
			return EMC_Texture;
		case EM_Terraform:
			return EMC_Terrain;
		case EM_None:
			default:
			return EMC_None;
		};
	};

/*-----------------------------------------------------------------------------
	Ed link topic function.
-----------------------------------------------------------------------------*/

AUTOREGISTER_TOPIC("Ed",EdTopicHandler);
void EdTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
	{
	guard(EdTopicHandler::Get);
	//
	if		(!stricmp(Item,"LASTSCROLL"))	Out.Logf("%i",GLastScroll);
	else if (!stricmp(Item,"CURTEX"))		Out.Log(GUnrealEditor.CurrentTexture ? GUnrealEditor.CurrentTexture->GetName() : "None");
	else if (!stricmp(Item,"CURCLASS"))		Out.Log(GEditor->CurrentClass ? GEditor->CurrentClass->GetName() : "None");
	//
	unguard;
	};
void EdTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
	{
	guard(EdTopicHandler::Set);
	unguard;
	};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
