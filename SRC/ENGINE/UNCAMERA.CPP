/*=============================================================================
	UnCamera.cpp: Generic Unreal camera code

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"
#include "UnIn.h"

/*-----------------------------------------------------------------------------
	UCamera object implementation.
-----------------------------------------------------------------------------*/

void UCamera::InitHeader()
{
	guard(UCamera::InitHeader);

	// Call parent class.
	UObject::InitHeader();

	// Start camera looking ahead, facing along the X axis, looking
	// towards the origin.
	OnHold		= 0;
	Texture     = NULL;
	Level       = GServer.GetLevel();
	Actor       = NULL;
	MiscRes		= NULL;
	DataSize    = 0;
	LastUpdateTime = 0;

	// Default sizes (can be changed before call to pOpenCameraWindow).
	X			= GDefaults.CameraSXR;
	Y			= GDefaults.CameraSYR;
	Caps		= 0;

	Current		= 0;
	ClickFlags  = 0;

	OpenX		  = -1;				// No default opening position.
	OpenY		  = 0;				// No default opening size.
	ParentWindow  = 0;

	GCameraManager->InitCameraWindow (this);
	unguard;
}
IMPLEMENT_CLASS(UCamera);

/*-----------------------------------------------------------------------------
	Custom camera creation and destruction.
-----------------------------------------------------------------------------*/

//
// UCamera constructor.  Creates the camera object but doesn't open its window.
//
UCamera::UCamera( ULevel *ThisLevel )
{
	guard(UCamera::UCamera);

	// Figure out which level to use.
	if( ThisLevel == NULL )
	{
		if( !GServer.GetLevel() )
			appError ("Can't create camera in empty world");
		
		Level = GServer.GetLevel();
	}
	else Level = ThisLevel;

	Level->Lock(LOCK_ReadWrite);
	GCameraManager->CameraArray->AddItem(this);

	// Find an available camera actor or spawn a new one.  Sends ACTOR_SPAWN message followed
	// by ACTOR_POSSESS message.
	if( Level->GetState()==LEVEL_UpEdit )
	{
		checkState(GEditor!=NULL);
		Actor = Level->SpawnViewActor( this, NAME_None, NULL );
		checkState(Actor!=NULL);
	}
	else
	{
		checkState(GEditor==NULL);
		if( !Level->SpawnPlayActor( this ) ) appError
		(
			"Can't play this level: No 'PlayerStart' actor was found to "
			"specify the player's starting position."
		);
		checkState(Actor!=NULL);
	}

	// Allocate texture for the camera.
	Texture = new(GetName(),CREATE_Unique)UTexture;
	Texture->TextureFlags |= TF_NoTile;

	// Init player console.
	Console = GGameBase->CreateCameraConsole(this);
	Console->Init(this);

	// Init input system.
	Input = NewInput();
	Input->Init(this,GApp);

	Level->Unlock(LOCK_ReadWrite);
	unguard;
}

//
// Close a camera.
//warning: Lots of interrelated stuff is happening here between the camera code,
// object code, platform-specific camera manager code, and Windows.
//
void UCamera::PreKill()
{
	guard(UCamera::PreKill);

	// Close the camera window.
	GCameraManager->CloseCameraWindow(this);

	// Unlink then delete actor unless camera is already locked (which happens
	// when camera is closed upon destroying a camera actor).
	if( GApp->ServerAlive )
	{
		Level->Lock(LOCK_ReadWrite);
		Actor->Camera = NULL;
		Level->DestroyActor( Actor );
		Actor = NULL;
		Level->Unlock(LOCK_ReadWrite);
		Console->Exit();
	}
	GGameBase->DestroyCameraConsole(Console);

	// Kill the input subsystem.
	Input->Exit();

	// Close the camera window.
	GCameraManager->CloseCameraWindow(this);

	// Shut down Windows objects.
	GCameraManager->CameraArray->RemoveItem(this);
	Texture->Kill();

	unguard;
}

/*-----------------------------------------------------------------------------
	Camera action handlers.
-----------------------------------------------------------------------------*/

//
// Handle all mouse movement in a camera.  Returns 1 if the camera movement 
// was processed by UnrealEd, or 0 if it should be queued up for later 
// gameplay use.
//
int UCamera::Move( BYTE Buttons, SWORD MouseX, SWORD MouseY, int Shift, int Ctrl )
{
	guard(UCamera::Move);
	if( GEditor )
	{
		GEditor->edcamMove (this,Buttons,MouseX,MouseY,Shift,Ctrl);
		return 1;
	}
	else return 0;
	unguard;
}

//
// General purpose mouse click handling. Returns 1 if processed, 0 if not.
//
int UCamera::Click( BYTE Buttons, SWORD MouseX, SWORD MouseY,int Shift, int Ctrl )
{
	guard(UCamera::Click);

	ELevelState State = Level->GetState();
	if( State==LEVEL_UpEdit )
	{
		if( GEditor )
			GEditor->edcamClick( this, Buttons, MouseX, MouseY, Shift, Ctrl );
		return 1;
	}
	else return 0;

	unguard;
}

//
// Render a frame for the camera.
//
void UCamera::Draw( int Scan )
{
	guard(UCamera::Draw);
	FMemMark MemMark(GMem);
	FMemMark DynMark(GDynMem);

	if( X>0 && Y>0 )
	{
		if( GEditor )	GEditor->edcamDraw( this, Scan );
		else			GUnreal.Draw( this, Scan );
	}

	MemMark.Pop();
	DynMark.Pop();

	unguard;
}

//
// Build camera's coordinate system.
//
void UCamera::BuildCoords()
{
	guard(UCamera::BuildCoords);
	FCalcViewCoords(Actor->Location,Actor->ViewRotation,Coords,Uncoords);
	unguard;
}

//
// If camera is in an orthogonal mode, return its viewing plane normal.
// Otherwise, return NULL.
//
const FVector *UCamera::GetOrthoNormal()
{
	switch( Actor->RendMap )
	{
		case REN_OrthXY:	return &GMath.ZAxisVector;
		case REN_OrthXZ:	return &GMath.YAxisVector;
		case REN_OrthYZ:	return &GMath.XAxisVector;
		default:			return NULL;
	}
}

//
// Precompute rendering information.
//
void UCamera::PrecomputeRenderInfo( int CamSXR, int CamSYR )
{
	guard(UCamera::PrecomputeRenderInfo);

	// Stride.
	Texture->USize = Stride;

	// Sizing.
	X 			= CamSXR;
	Y 			= CamSYR;
	ByteStride	= Stride * ColorBytes;

	// Integer precomputes.
	X2 			= X/2;
	Y2 			= Y/2;
	FixX 		= Fix(X);

	// Float precomputes.
	FX 			= (FLOAT)X;
	FY 			= (FLOAT)Y;
	FX1 		= 65536.0 * (FX + 1);
	FY1 		= FY + 1;
	FX2			= FX * 0.5;
	FY2			= FY * 0.5;	
	FX15		= (FX+1.0001) * 0.5;
	FY15		= (FY+1.0001) * 0.5;	
	FXM5		= FX15 - 0.5;
	FYM5		= FY15 - 0.5;

	INT Angle   = (0.5 * 65536.0 / 360.0) * Actor->FovAngle;
	ProjZ       = FX / (2.0*GMath.SinTab(Angle) / GMath.CosTab(Angle));

	Zoom 		= Actor->OrthoZoom / (X * 15.0);
	RZoom       = 1.0/Zoom;
	RProjZ		= 1.0/ProjZ;

	// The following have a fudge factor to adjust for floating point imprecision in
	// the screenspace clipper.
	ProjZRX2	= 1.00005 * ProjZ / FX2;
	ProjZRY2	= 1.00005 * ProjZ / FY2;

	// Calculate camera view coordinates.
	BuildCoords();

	// Precomputed camera view frustrum edges (worldspace).
	FLOAT TempSigns[2]={-1.0,+1.0};
	for( int i=0; i<2; i++ )
	{
		for( int j=0; j<2; j++ )
		{
			ViewSides[i*2+j] = FVector(TempSigns[i] * FX2, TempSigns[j] * FY2, ProjZ).Normal().TransformVectorBy(Uncoords);
		}
		ViewPlanes[i] = FPlane
		(
			Coords.Origin,
			FVector(0,TempSigns[i] / FY2,1.0/ProjZ).Normal().TransformVectorBy(Uncoords)
		);
		ViewPlanes[i+2] = FPlane
		(
			Coords.Origin,
			FVector(TempSigns[i] / FX2,0,1.0/ProjZ).Normal().TransformVectorBy(Uncoords)
		);
	}
	unguard;
}

/*---------------------------------------------------------------------------------------
	Camera information functions.
---------------------------------------------------------------------------------------*/

//
// Is this camrea a wireframe view?
//
int UCamera::IsWire()
{
	guard(UCamera::IsWire);
	int RendMap = Actor->RendMap;
	return ((GEditor&&GEditor->MapEdit) || (RendMap==REN_OrthXY)||(RendMap==REN_OrthXZ)||(RendMap==REN_OrthYZ)||(RendMap==REN_Wire));
	unguard;
}

/*-----------------------------------------------------------------------------
	Camera locking & unlocking.
-----------------------------------------------------------------------------*/

//
// Locks the camera.  When locked, a Camera's framebuffer will
// not be resized.  When a camera is unlocked, you can't
// count on the size remaining constant (the user may resize it or close it).
//
// Returns 0 if ok, nonzero if the user has closed the camera window.  Any code that
// locks the camera must check for a nonzero result, which can happen in the normal
// course of operation.
//
INT UCamera::Lock(DWORD ThisLockType)
{
	guard(UCamera::Lock);
	checkState(!IsLocked());
	
	if( OnHold || X==0 || Y==0 )
	{
		// Can't lock it.
		if( !(ThisLockType & LOCK_CanFail) )
			appErrorf("Failed locking Camera %s",GetName());

		return 0;
	}

	// Compute information.
	Stride = X;

	// Call the platform-specific camera manager to lock the camera's window
	// and frame buffer.  It sets Camera->SXStride and Camera->RealScreen.
	Texture->Lock(ThisLockType);
	if( !GCameraManager->LockCameraWindow(this) )
	{
		if( !(ThisLockType & LOCK_CanFail) )
			appErrorf("Failed locking Camera %s",GetName());

		Texture->Unlock(ThisLockType);
		return 0;
	}

	// Set the screen pointer.
	Screen = RealScreen;

	// Lock the level.
	Level->Lock( ThisLockType );

	// Get extra polygon flags.
	ExtraPolyFlags = 
		((Actor->RendMap==REN_PolyCuts) || (Actor->RendMap==REN_Zones)) ? PF_NoMerge : 0;

	// Set border.
	FXB = FYB = 0.0;

	// Precomputed info.
	PrecomputeRenderInfo(X,Y);

	// Successfully locked it and set Screen pointer.
	return UObject::Lock(ThisLockType);
	unguard;
}

//
// Unlock a camera.  When unlocked, you can't access the *Screen pointer because
// the corresponding window may have been resized or deleted.
//
void UCamera::Unlock(DWORD OldLockType, int Blit)
{
	guard(UCamera::Unlock);
	checkState(IsLocked());

	// Done unlocking.
	UObject::Unlock(OldLockType);

	// Unlock the level.
	Level->Unlock(OldLockType);

	// Unlock the platform specifics.
	GCameraManager->UnlockCameraWindow(this,Blit);

	// Unlock the texture.
	Texture->Unlock(OldLockType);

	unguard;
}

/*-----------------------------------------------------------------------------
	Window-related.
-----------------------------------------------------------------------------*/

//
// Open the window for the camera.
//
void UCamera::OpenWindow( DWORD ParentWindow, int Temporary )
{
	guard(UCamera::OpenWindow);
	GCameraManager->OpenCameraWindow( this, ParentWindow, Temporary );
	unguard;
}

//
// Update the camera's window.
//
void UCamera::UpdateWindow()
{
	guard(UCamera::UpdateWindow);
	GCameraManager->UpdateCameraWindow( this );
	unguard;
}

/*-----------------------------------------------------------------------------
	Holding and unholding.
-----------------------------------------------------------------------------*/

//
// Put the camera on hold, preventing it from being resized.
//
void UCamera::Hold()
{
	guard(UCamera::Hold);
	OnHold=1;
	unguard;
}

//
// Unhold the camera, and perform any latent resizing.
//
void UCamera::Unhold()
{
	guard(UCamera::Unhold);
	if (OnHold)
	{
		OnHold = 0;
		UpdateWindow();
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/

//
// UCamera command line.
//
int UCamera::Exec( const char *Cmd, FOutputDevice *Out )
{
	guard(UCamera::Exec);
	const char *Str = Cmd;

	if( GetCMD(&Str,"SHOWACTORS") )
	{
		Actor->ShowFlags |= SHOW_Actors;
		return 1;
	}
	else if( GetCMD(&Str,"HIDEACTORS") )
	{
		Actor->ShowFlags &= ~SHOW_Actors;
		return 1;
	}
	else if( GetCMD(&Str,"BEHINDVIEW") )
	{
		Actor->bBehindView = 1;
		return 1;
	}
	else if( GetCMD(&Str,"NORMALVIEW") )
	{
		Actor->bBehindView = 0;
		return 1;
	}
	else if( GetCMD(&Str,"RESPAWN") )
	{
		if( !GEditor )
		{
			Level->Lock(LOCK_ReadWrite);
			if( !Level->SpawnPlayActor(this) ) Out->Logf("Respawned failed");
			else Out->Logf("Respawned player");
			Level->Unlock(LOCK_ReadWrite);
		}
		else Out->Logf(LOG_ExecError,"Can't respawn player");
		return 1;
	}
	else if( GetCMD(&Str,"RMODE") )
	{
		int Mode = atoi(Str);
		if( (Mode>REN_None) && (Mode<REN_MAX) )
		{
			Actor->RendMap = Mode;
			Out->Logf("Rendering mode set to %i",Mode);
		}
		else Out->Logf(LOG_ExecError,"Invalid mode");
		return 1;
	}
	else if( GetCMD(&Str,"EXEC") )
	{
		char Filename[64];
		if( GrabSTRING(Str,Filename,ARRAY_COUNT(Filename)) )
			ExecMacro( Filename, Out );
		else 
			Out->Logf("Missing filename to exec");
		return 1;
	}
	else if( Console && Console->Exec(Cmd,Out) )
	{
		// Camera console handled it.
		return 1;
	}
	else if( Input && Input->Exec(Cmd,Out) )
	{
		// Input system handled it.
		return 1;
	}
	else if( Level && Actor && GetCMD(&Str,"SERVER") )
	{
		// Send to special player-exec handler, i.e. for firing.
		Level->Lock(LOCK_ReadWrite);
		Level->PlayerExec( Actor, Str, Out );
		Level->Unlock( LOCK_ReadWrite );
		return 1;
	}
	else
	{
		// Pass to engine for execing.
		return GUnreal.Exec( Cmd, Out );
	}
	unguard;
}

//
// Execute a macro on this camera.
//
void UCamera::ExecMacro( const char *Filename, FOutputDevice *Out )
{
	guard(UCamera::ExecMacro);

	UTextBuffer *Text = new("Macro",Filename,IMPORT_MakeUnique)UTextBuffer;
	if( Text )
	{
		Out->Logf("Execing %s",Filename);
		Text->Lock(LOCK_Read);
		char Temp[256];
		const char *Data = &Text->Element(0);
		while( GetLINE (&Data,Temp,256)==0 )
		{
			// Pass to game console handler for execing.
			Exec( Temp, Out );
		}
		Text->Unlock(LOCK_Read);
		Text->Kill();
	}
	else Out->Logf(LOG_ExecError,"Macro file %s not found",Filename);

	unguard;
}

/*-----------------------------------------------------------------------------
	Input processing.
-----------------------------------------------------------------------------*/

//
// Process low-level input.
//
int	UCamera::Process( EInputKey iKey, EInputState State, FLOAT Delta )
{
	guard(UCamera::Process);
	if( Console->Process( iKey, State, Delta ) )
	{
		// Player console handled it.
		return 1;
	}
	else if( Input->Process( *Console, iKey, State, Delta ) )
	{
		// Input system handled it.
		return 1;
	}
	else
	{
		// Nobody handled it.
		return 0;
	}
	unguard;
}

//
// Process a keystroke.
//
int UCamera::Key( INT Key )
{
	guard(UCamera::Key);

	if( Console->Key(Key) )
	{
		// Player console handled it.
		return 1;
	}
	else if( GEditor && GEditor->edcamKey( this, Key ) )
	{
		// Editor handled it.
		return 1;
	}
	else
	{
		// Nobody processed it.
		return 0;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	UCamera FOutputDevice interface.
-----------------------------------------------------------------------------*/

//
// Output a message on the camera's console.
//
void UCamera::Write(const void *Data, int Length, ELogType MsgType)
{
	guard(UCamera::Write);

	// Pass to console.
	if( Console ) Console->Write(Data,Length,MsgType);

	unguard;
}

/*-----------------------------------------------------------------------------
	Input related.
-----------------------------------------------------------------------------*/

//
// Read input from the camera.
//
void UCamera::ReadInput( PPlayerTick &Move, FLOAT DeltaSeconds, FOutputDevice *Out )
{
	guard(UCamera::ReadInput);
	checkState(Input!=NULL);
	checkState(Console!=NULL);

	// Get input from input system.
	Input->ReadInput( Move, DeltaSeconds, Out );

	// Allow game-specific camera console to postprocess the input.
	Console->PostReadInput( Move, DeltaSeconds, Out );
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
