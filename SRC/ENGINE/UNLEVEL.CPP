/*=============================================================================
	UnLevel.cpp: Level-related functions

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * July 21, 1996: Mark added global GLevel.
		* Dec  13, 1996: Tim removed GLevel.
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	Level creation & emptying.
-----------------------------------------------------------------------------*/

//
//	Create a new level and allocate all objects needed for it.
//	Call with Editor=1 to allocate editor structures for it, also.
//
ULevel::ULevel( int InMax, int InEditable, int InRootOutside )
:	UDatabase( InMax, 0 )
{
	guard(ULevel::ULevel);

	// Init state.
	State = LEVEL_Down;

	// Allocate model.
	Model = new( GetName(), CREATE_Replace )UModel( InEditable, InRootOutside );

	// Allocate brush array.
	if( InEditable )
	{
		char TempName[256];
		sprintf(TempName,"%s_Brushes",GetName());
		BrushArray = new( GetName(),CREATE_Replace,RF_NotForClient|RF_NotForServer ) TArray<UModel*>( 0 );
		BrushArray->AddItem( new("Brush",CREATE_Replace,RF_NotForClient|RF_NotForServer)UModel(InEditable,1) );
	}

	// Allocate ReachSpecs.
	ReachSpecs = new( GetName(), CREATE_Replace ) UReachSpecs(0);

	// Bring level up.
	if( InEditable )
		SetState( LEVEL_UpEdit );

	// Empty the level.
	EmptyLevel();

	unguard;
}

//
// Empty the contents of a level.
//
void ULevel::EmptyLevel()
{
	guard(ULevel::EmptyLevel);

	// Clear the brush array except for brush 0.
	Lock(LOCK_ReadWrite);
	for( int i=BrushArray->Num-1; i>0; i-- )
	{
		BrushArray->Element(i)->Kill();
		BrushArray->Remove(i);
	}

	// Clear the model.
	Model->EmptyModel( 1, 1 );

	// Kill all nonplayer actors.
	for( i=0; i<Num; i++ )
		if( Element(i) && !Element(i)->GetPlayer() )
			DestroyActor( Element(i) );

	// Spawn the level info.
	AActor *Temp = SpawnActor( new("LevelInfo",FIND_Existing)UClass );
	checkState(Temp==Element(0));

	// Hook the level info up to all player actors.
	for( i=0; i<Num; i++ )
		if( Element(i) )
			Element(i)->Level = GetLevelInfo();

	// Allocate misc objects for level.
	char TempName[NAME_SIZE];
	sprintf(TempName,"%s_Misc",GetName());
	Unlock(LOCK_ReadWrite);

	unguard;
}

//
// Shrink everything in a level to its minimum size.
//
void ULevel::ShrinkLevel()
{
	guard(ULevel::Shrink);

	Model->ShrinkModel();
	if( BrushArray ) BrushArray->Shrink();
	if( ReachSpecs ) ReachSpecs->Shrink();

	unguard;
}

/*-----------------------------------------------------------------------------
	Level locking and unlocking.
-----------------------------------------------------------------------------*/

//
// Lock this level.
//
INT ULevel::Lock(DWORD NewLockType)
{
	guard(ULevel::Lock);
	checkState(!IsLocked());

	// Check intrinsics.
	extern int GIntrinsicDuplicate;
	if( GIntrinsicDuplicate )
		appErrorf("Duplicate intrinsic registered: %i", GIntrinsicDuplicate);

	// Init collision if first time through.
	guard(1);
	if( !Hash.CollisionInitialized )
		InitCollision();
	unguard;

	// Lock our parent class.
	INT Result=0;
	guard(2);
	Result = UDatabase::Lock(NewLockType);
	unguard;

	// Init deleted chain.
	guard(3);
	FirstDeleted = NULL;
	unguard;

	guard(4);
	Model->Lock(NewLockType);
	unguard;

	guard(5);
	if( BrushArray )
		BrushArray->Lock(NewLockType);
	unguard;

	guard(6);
	GBrushTracker.Lock();
	unguard;

	// Get the LevelInfo, if any.
	guard(7);
	Info = NULL;
	if( Num>0 && Element(0) && Element(0)->IsA("LevelInfo") ) 
		Info = (ALevelInfo*)Element(0);
	unguard;

	return Result;
	unguard;
}

//
// Unlock this level.
//
void ULevel::Unlock(DWORD OldLockType)
{
	guard(ULevel::Unlock);
	checkState(IsLocked());

	// Clean up deleted actors.
	guard(1);
	CleanupDestroyed();
	unguard;

	// Unlock stuff.
	guard(2);
	GBrushTracker.Unlock();
	unguard;

	guard(3);
	Model->Unlock(OldLockType);
	unguard;

	guard(4);
	if( BrushArray )
		BrushArray->Unlock(OldLockType);
	unguard;

	// Unlock our parent class.
	guard(5);
	UDatabase::Unlock(OldLockType);
	unguard;

	unguard;
}

/*-----------------------------------------------------------------------------
	Level state transitions.
-----------------------------------------------------------------------------*/

//
// Set the level's state.  Notifies all actors of the state change.
// If you're setting the state to LEVEL_UP_PLAY, you must specify
// the network mode and difficulty level.
//
void ULevel::SetState( ELevelState NewState )
{
	guard(ULevel::SetState);

	ELevelState	OldState;
	OldState	= State;
	State		= NewState;

	if( State == OldState )
		return;

	// Send messages to all level actors notifying state we're exiting.
	Lock(LOCK_ReadWrite);

	if( OldState==LEVEL_UpPlay ) SendEx( NAME_EndPlay, NULL );
	if( OldState==LEVEL_UpEdit ) SendEx( NAME_EndEdit, NULL );

	if( Info )
		Info->LevelState = State;

	// Send message to actors notifying them of new level state.
	if( NewState == LEVEL_UpPlay )
	{
		// Init touching actors.
		for( INDEX i=0; i<Num; i++ )
			if( Element(i) )
				for( int j=0; j<ARRAY_COUNT(Element(i)->Touching); j++ )
					Element(i)->Touching[j] = NULL;

		// Begin scripting.
		for( i=0; i<Num; i++ )
			if( Element(i) )
				Element(i)->BeginExecution();

		// Send begin play messages.
		SendEx( NAME_PreBeginPlay, NULL );
		SendEx( NAME_BeginPlay,    NULL );

		// Set zones.
		for( i=0; i<Num; i++ )
			if( Element(i) )
				SetActorZone( Element(i) );

		SendEx( NAME_PostBeginPlay, NULL );
		debugf( LOG_Info, "Level %s is now up for play", GetName() );
	}
	else if( NewState == LEVEL_UpEdit )
	{
		SendEx( NAME_BeginEdit, NULL );
		debugf( LOG_Info, "Level %s is now up for edit", GetName() );
	}
	else if( NewState == LEVEL_Down )
	{
		debugf( LOG_Info, "Level %s is now down", GetName() );
	}
	else 
	{
		appErrorf( "SetLevelState: Bad state %i",NewState );
	}
	Unlock( LOCK_ReadWrite );
	unguard;
}

/*-----------------------------------------------------------------------------
	Level object implementation.
-----------------------------------------------------------------------------*/

const char *ULevel::Import( const char *Buffer, const char *BufferEnd, const char *FileType )
{
	guard(ULevel::Import);
	checkState(BrushArray->Num>0);

	int ImportedActiveBrush=0,NumBrushes=0;
	char StrLine[256],BrushName[NAME_SIZE];
	const char *StrPtr;

	// Assumes data is being imported over top of a new, valid map.
	GetNEXT  (&Buffer);
	if       (!GetBEGIN (&Buffer,"MAP")) return NULL;
	GetINT(Buffer,"Brushes=",&NumBrushes);

	while( GetLINE (&Buffer,StrLine,256)==0 )
	{
		StrPtr = StrLine;
		if( GetEND(&StrPtr,"MAP") )
		{
			// End of brush polys.
			break;
		}
		else if( GetBEGIN(&StrPtr,"BRUSH") )
		{
			GApp->StatusUpdate("Importing Brushes",BrushArray->Num,NumBrushes);
			if( GetSTRING(StrPtr,"NAME=",BrushName,NAME_SIZE) )
			{
				UModel *TempModel;
				if( !ImportedActiveBrush )
				{
					// Parse the active brush, which has already been allocated.
					TempModel = BrushArray->Element(0);
					Buffer = TempModel->Import(Buffer,BufferEnd,FileType);
					if( !Buffer )
						return NULL;
					ImportedActiveBrush = 1;
				}
				else
				{
					// Parse a new brush which has not yet been allocated.
					TempModel = new(BrushName,CREATE_MakeUnique,RF_NotForClient|RF_NotForServer)UModel;
					Buffer = TempModel->Import(Buffer,BufferEnd,FileType);
					if( !Buffer )
						return NULL;
					BrushArray->AddItem(TempModel);						
				}
				TempModel->ModelFlags |= MF_Selected;
			}
		}
		else if( GetBEGIN(&StrPtr,"ACTORLIST") )
		{
			for( int i=0; i<Num; i++ )
			{
				if( Element(i) )
				{
					Element(i)->bTempEditor = 1;
					Element(i)->bSelected   = 0;
				}
			}
			Buffer = ImportActors( Buffer, BufferEnd, FileType );
			if( !Buffer )
				return NULL;
			for( i=0; i<Num; i++ )
				if( Element(i) )
					Element(i)->bSelected = !Element(i)->bTempEditor;
		}
	}
	return Buffer;
	unguard;
}
void ULevel::Export( FOutputDevice &Out, const char *FileType, int Indent )
{
	guard(ULevel::Export);

	Out.Logf( "%s;\r\n", spc(Indent) );
	Out.Logf( "%s; UnrealEd %s\r\n", spc(Indent), ENGINE_VERSION );
	Out.Logf( "%s; Exported map \r\n", spc(Indent) );
	Out.Logf( "%s;\r\n", spc(Indent) );

	Out.Logf( "%sBegin Map Name=%s Brushes=%i\r\n", spc(Indent), GetName(), BrushArray->Num );
	Out.Logf( "%s   ;\r\n", spc(Indent) );

	// Export brushes.
	for( int i=0; i<BrushArray->Num; i++ )
		BrushArray->Element(i)->Export( Out, FileType, Indent+3 );

	// Export actors.
	ExportActors( Out, FileType, Indent+3 );

	// End.
	Out.Logf( "%sEnd Map\r\n", spc(Indent) );
	unguard;
}
void ULevel::ModifyAllItems()
{
	guard(ULevel::ModifyAllItems);

	// Modify all actors that exist.
	for( int i=0; i<Max; i++ )
	{
		ModifyItem( i );
		if( Element(i) )
			Element(i)->ModifyHeader();
	}
	unguard;
}
void ULevel::PreKill()
{
	// Free the cached collision info.
	if( Hash.CollisionInitialized )
		Hash.Exit();
}
IMPLEMENT_DB_CLASS(ULevel);

/*-----------------------------------------------------------------------------
	Level link topic.
-----------------------------------------------------------------------------*/

AUTOREGISTER_TOPIC("Lev",LevTopicHandler);
void LevTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(LevTopicHandler::Get);

	UTextBuffer	*Text;
	int			ItemNum;

	if (!isdigit (Item[0]))
		return; // Item isn't a number.

	ItemNum = atoi (Item);
	if( (ItemNum < 0) || (ItemNum >= ULevel::NUM_LEVEL_TEXT_BLOCKS) )
		return; // Invalid text block number.

	Text = Level->TextBlocks[ItemNum];

	if( Text )
	{
		Text->Lock(LOCK_Read);
		Out.Log( &Text->Element(0) );
		Text->Unlock(LOCK_Read);
	}

	unguard;
}
void LevTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(LevTopicHandler::Set);

	if( !isdigit(Item[0]) )
		return; // Item isn't a number.

	int ItemNum = atoi (Item);
	if ((ItemNum < 0) || (ItemNum >= ULevel::NUM_LEVEL_TEXT_BLOCKS)) return; // Invalid text block number

	if( !Level->TextBlocks[ItemNum] )
		Level->TextBlocks[ItemNum] = new(Level->GetName(),CREATE_MakeUnique,RF_NotForClient|RF_NotForServer)UTextBuffer;
	
	Level->TextBlocks[ItemNum]->Max = strlen(Data)+1;
	Level->TextBlocks[ItemNum]->Realloc();
	
	Level->TextBlocks[ItemNum]->Lock(LOCK_ReadWrite);
	strcpy(&Level->TextBlocks[ItemNum]->Element(0),Data);
	Level->TextBlocks[ItemNum]->Unlock(LOCK_ReadWrite);

	unguard;
}

/*-----------------------------------------------------------------------------
	Reconcile actors and cameras after loading or creating a new level.

	These functions provide the basic mechanism by which UnrealEd associates
	cameras and actors together, even when new maps are loaded which contain
	an entirely different set of actors which must be mapped onto the existing 
	cameras.
-----------------------------------------------------------------------------*/

//
// Remember actors.  This is called prior to loading a level, and it
// places temporary camera actor location/status information into each camera,
// which can be used to reconcile the actors once the new level is loaded and the
// current actor list is replaced with an entirely new one.
//
void ULevel::RememberActors()
{
	guard(ULevel::RememberActors);
	for( int i=0; i<GCameraManager->CameraArray->Num; i++ )
	{
		UCamera *Camera = GCameraManager->CameraArray->Element(i);
		Camera->Actor   = NULL;
	}
	unguard;
}

//
// Remove all camera references from all actors in this level.
//
void ULevel::DissociateActors()
{
	guard(ULevel::DissociateActors);
	
	for( INDEX i=0; i<Num; i++ )
		if( Element(i) && Element(i)->GetPlayer() )
			Element(i)->GetPlayer()->Camera = NULL;

	unguard;
}

//
// Reconcile actors.  This is called after loading a level.
// It attempts to match each existing camera to an actor in the newly-loaded
// level.  If no decent match can be found, creates a new actor for the camera.
//
void ULevel::ReconcileActors()
{
	guard(ULevel::ReconcileActors);
	Lock(LOCK_ReadWrite);

	// Dissociate all actor Cameras.
	DissociateActors();

	if( GetState() == LEVEL_UpEdit )
	{
		// Match cameras and camera-actors with identical names.  These cameras
		// will obtain all of their desired display properties from the actors.
		for( int i=0; i<GCameraManager->CameraArray->Num; i++ )
		{
			UCamera *Camera	= GCameraManager->CameraArray->Element(i);
			Camera->Level   = this;
			Camera->Actor	= NULL;
			char NameToFind[NAME_SIZE+1];
			sprintf( NameToFind, "%s0", Camera->GetName() );

			for( INDEX j=0; j<Num; j++ )
			{
				AActor *Actor = Element(j);
				if( Actor && Actor->IsA("View") && stricmp(Actor->GetName(),NameToFind)==0 )
				{
					debugf( LOG_Info, "Matched camera %s", Camera->GetName() );
					Camera->Actor = (APawn *)Actor;
					break;
				}
			}
		}

		// Match up all remaining cameras to actors.  These cameras will get default
		// display properties.
		for( i=0; i<GCameraManager->CameraArray->Num; i++ )
		{
			UCamera *Camera = GCameraManager->CameraArray->Element(i);

			// Hook camera up to an existing (though unpossessed) camera actor, or create
			// a new camera actor for it.  Sends NAME_SPAWN and NAME_POSSESS.  Returns
			// actor index or INDEX_NONE if failed.
			if( !Camera->Actor )
			{
				FName Name(Camera->GetName(),FNAME_Add);
				Camera->Actor = SpawnViewActor( Camera, Name );
				if( !Camera->Actor )
				{
					debug( LOG_Problem, "Failed to spawn view actor for camera" );
					Camera->Kill();
					i--;
				}
				debugf( LOG_Info, "Spawned view actor %s for camera %s", Camera->Actor->GetName(), Camera->GetName() );
			}
		}
	}
	else
	{
		// Match existing cameras to actors.
		for( int i=0; i<GCameraManager->CameraArray->Num; i++ )
		{
			UCamera *Camera	= GCameraManager->CameraArray->Element(i);
			Camera->Level   = this;
			Camera->Actor	= NULL;

			for( INDEX j=0; j<Num; j++ )
			{
				AActor *Actor = Element(j);
				if( Actor && Actor->IsA("Pawn") && ((APawn*)Actor)->bIsPlayer && !((APawn*)Actor)->Camera )
				{
					debugf( LOG_Info, "Matched camera %s to %s %s", Camera->GetName(), Actor->GetClassName(), Actor->GetName() );
					Camera->Actor            = (APawn *)Actor;
					((APawn *)Actor)->Camera = Camera;
					break;
				}
			}
		}

		// Spawn actors for any unmatched cameras.
		for( i=0; i<GCameraManager->CameraArray->Num; i++ )
		{
			UCamera *Camera = GCameraManager->CameraArray->Element(i);
			if( !Camera->Actor )
				if( !SpawnPlayActor(Camera) ) appError 
					(
						"Can't play this level: No 'PlayerStart' actor was found to "
						"specify the player's starting position."
					);
		}
	}
	Unlock(LOCK_ReadWrite);

	// Associate cameras and actors.
	GCameraManager->UpdateActorUsers();

	// Kill any remaining camera actors.
	Lock(LOCK_ReadWrite);
	for( INDEX i=0; i<Num; i++ )
		if( Element(i) && Element(i)->IsA("View") && !((AView*)Element(i))->Camera )
			DestroyActor(Element(i));

	Unlock(LOCK_ReadWrite);
	unguard;
}

/*-----------------------------------------------------------------------------
	ULevel command-line.
-----------------------------------------------------------------------------*/

int ULevel::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(ULevel::Exec);

	const char *Str = Cmd;
	if( GetCMD(&Str,"KILLACTORS") )
	{
		Lock(LOCK_ReadWrite);
		for( int i=0; i<Num; i++ )
			if( Element(i) && !Element(i)->IsA("Light") && !Element(i)->GetPlayer() )
				DestroyActor( Element(i) );

		Unlock(LOCK_ReadWrite);
		Out->Log("Killed all actors");
		return 1;
	}
	else if( GetCMD(&Str,"KILLPAWNS") )
	{
		Lock(LOCK_ReadWrite);
		for( int i=0; i<Num; i++ )
			if( Element(i) && Element(i)->IsA("Pawn") && !Element(i)->GetPlayer() )
				DestroyActor( Element(i) );

		Out->Log("Killed all NPC's");
		Unlock(LOCK_ReadWrite);
		return 1;
	}
	else if( GetCMD(&Str,"LEVEL") )
	{
		if( GetCMD(&Str,"REDRAW") )
		{
			GCameraManager->RedrawLevel(this);
			return 1;
		}
		else if( GetCMD(&Str,"LINKS") )
		{
			UTextBuffer *Results = new("Results",CREATE_Replace)UTextBuffer(1);
			int Internal=0,External=0;
			Results->Logf("Level links:\r\n");

			Lock(LOCK_ReadWrite);
			for( int i=0; i<Num; i++ )
			{
				if( Element(i) && Element(i)->IsA("Teleporter") )
				{
					ATeleporter &Teleporter = *(ATeleporter *)Element(i);
					Results->Logf("   %s\r\n",Teleporter.URL);
					if(strchr(Teleporter.URL,'//')) External++;
					else Internal++;
				}
			}
			Unlock(LOCK_ReadWrite);
			Results->Logf("End, %i internal link(s), %i external.\r\n",Internal,External);
			return 1;
		}
		else if( GetCMD(&Str,"VALIDATE") )
		{
			// Validate the level.
			UTextBuffer *Results = new("Results",CREATE_Replace)UTextBuffer(1);
			int Errors=0, Warnings=0;
			Results->Log("Level validation:\r\n");

			// Make sure it's not empty.
			if( Model->Nodes->Num == 0 )
			{
				Results->Log("Error: Level is empty!\r\n");
				return 1;
			}

			// Make sure BSP is valid.
			if( Model->ModelFlags & MF_InvalidBsp )
			{
				Results->Logf("Error: Geometry must be rebuild!\r\n");
				return 1;
			}

			// Find playerstart.
			for( int i=0; i<Num; i++ )
				if( Element(i) && Element(i)->IsA("PlayerStart") )
					break;
			if( i == Num )
			{
				Results->Log( "Error: Missing PlayerStart actor!\r\n" );
				return 1;
			}

			// Make sure PlayerStarts are outside.
			for( i=0; i<Num; i++ )
			{
				if( Element(i) && Element(i)->IsA("PlayerStart") )
				{
					FCheckResult Hit(0.0);
					if( !Model->PointCheck( Hit, NULL, Element(i)->Location, FVector(0,0,0), 0 ) )
					{
						Results->Log( "Error: PlayerStart doesn't fit!\r\n" );
						return 1;
					}
				}
			}

			// Check scripts.
			if( GEditor && !GEditor->CheckScripts( new(TOP_CLASS_NAME,FIND_Existing)UClass, *Results ) )
			{
				Results->Logf( "\r\nError: Scripts need to be rebuilt!\r\n" );
				return 1;
			}

			// Check level title.
			if( GetLevelInfo()->Title[0]==0 )
			{
				Results->Logf( "Error: Level is missing a title!" );
				return 1;
			}
			else if( stricmp(GetLevelInfo()->Title,"Untitled")==0 )
				Results->Logf( "Warning: Level is untitled\r\n" );

			// Check actors.
			for( i=0; i<Num; i++ )
			{
				AActor *Actor = Element(i);
				if( Actor )
				{
					guard(CheckingActors);
					checkState(Actor->GetClass()!=NULL);
					checkState(Actor->GetClass()->Script!=NULL);
					checkState(Actor->GetClass()->StackTree!=NULL);
					checkState(Actor->GetClass()->StackTree->Num>0);
					checkState(Actor->MainStack.Object==Actor);
					checkState(Actor->Level!=NULL);
					checkState(Actor->XLevel!=NULL);
					checkState(Actor->XLevel==this);
					checkState(Actor->XLevel->Element(0)!=NULL);
					checkState(Actor->XLevel->Element(0)==Actor->Level);
					unguardf(("(%i %s)",i,Actor->GetClassName()));
				}
			}

			// Success.
			Results->Logf("Success: Level validation succeeded!\r\n");
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	FOutputDevice interface.
-----------------------------------------------------------------------------*/

//
// Output a message to all players in the level.
//
void ULevel::Write(const void *Data, int Length, ELogType MsgType)
{
	guard(ULevel::Write);
	for( int i=0; i<Num; i++ )
	{
		if( Element(i) && Element(i)->IsA("Pawn") )
		{
			APawn *Actor = (APawn *)Element(i);
			if( Actor->Camera )
			{
				//todo: Also send to network players!!
				Actor->Camera->Write(Data,Length,MsgType);
			}
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
