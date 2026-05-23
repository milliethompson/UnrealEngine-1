/*=============================================================================
	UnLevel.cpp: Level-related functions

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * July 21, 1996: Mark added global GLevel
=============================================================================*/

#include "Unreal.h"
#include "UnDynBsp.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

//
// Level state description text
//
UNREAL_API const char *GLevelStateDescr[LEVEL_MAX]=
{
	"Down","Up for play","Up for editing","Up for demo"
};

/*-----------------------------------------------------------------------------
	Level creation & emptying.
-----------------------------------------------------------------------------*/

//
//	Create a new level and allocate all resources needed for it.
//	Call with Editor=1 to allocate editor structures for it, also.
//
ULevel::ULevel( int Editable )
{
	guard(ULevel::ULevel);
	char TempName[256];

	State=LEVEL_Down;

	// Figure out how much stuff we need.
	INDEX MaxActors		= 1200;
	INDEX MaxTextSize	= 20000;
	INDEX MaxBrushes	= 5000;
	INDEX MaxMisc		= 64;

	// Allocate all resources (indentation shows hierarchy).
	Model			= new(Name,CREATE_Replace) UModel(Editable);

	// Allocate actor list.
	ActorList		= new(Name,CREATE_Replace) UActorList(MaxActors);
	ActorList->RelistActors();

	// Allocate brush array.
	sprintf(TempName,"%s_Brushes",Name);
	BrushArray		= new(Name,CREATE_Replace) TArray<UModel>(MaxBrushes);
	BrushArray->Add  (new("Brush",CREATE_Replace) UModel(Editable));

	// Allocate misc resources for level.
	sprintf(TempName,"%s_Misc",Name);
	Misc = new(TempName,CREATE_Replace) TArray<UResource>(MaxMisc);
	Misc->Add(GClasses.Actor);
	Misc->Add(GClasses.Camera);
	Misc->Add(GClasses.Light);
	Misc->Add(GClasses.LevelDescriptor);

	// Stick level in world.
	GServer.Levels->Add(this);

	// Bring level up.
	if (Editable) SetState (LEVEL_UpEdit,NULL);

	unguard;
}

//
// Empty the contents of a level.
//
void ULevel::Empty()
{
	guard(ULevel::Empty);

	// Clear the brush array except for brush 0.
	for (int i=BrushArray->Num-1; i>0; i--) 
	{
		BrushArray->Element(i)->Kill();
		BrushArray->Delete(BrushArray->Element(i));
	}

	// Clear the model.
	ILevel LevelInfo;
	Lock(&LevelInfo,LOCK_NoTrans);

	LevelInfo.ModelInfo.NumVectors  = 0;
	LevelInfo.ModelInfo.NumPoints   = 0;
	LevelInfo.ModelInfo.NumBspNodes = 0;
	LevelInfo.ModelInfo.NumBspSurfs = 0;
	LevelInfo.ModelInfo.NumVertPool = 0;
	LevelInfo.ModelInfo.ModelFlags  = 0;

	// Kill actors.
	AActor *Actor = &LevelInfo.Actors->Element(0);
	for ( i=0; i<LevelInfo.Actors->Max; i++ )
	{
		if( Actor->Class && !Actor->GetPlayer() )
		{
			LevelInfo.DestroyActor(i);
		}
		Actor->bJustDeleted = 0;
		Actor++;
	}
	LevelInfo.Actors->RelistActors();

	// Spawn the level descriptor.
	INDEX iTemp = LevelInfo.SpawnActor
	(
		GClasses.LevelDescriptor,
		NAME_NONE,
		&GMath.ZeroVector
	);

	if (iTemp!=0)
		appErrorf("Spawned bad leveldescriptor: %i",iTemp);
	
	Unlock(&LevelInfo);

	unguard;
}

//
// Delete all stuff related to a level.
//
void ULevel::KillAll()
{
	guard(ULevel::KillAll);

	SetState(LEVEL_Down,NULL);

	for (int i=1; i<BrushArray->Num; i++) 
		BrushArray->Element(i)->Kill();

	Kill();
	
	unguard;
}

/*-----------------------------------------------------------------------------
	Level locking and unlocking.
-----------------------------------------------------------------------------*/

void ULevel::Lock(ILevel *LevelInfo, int NewLockType)
{
	guard(ULevel::Lock);

	checkVital( GLevel==0, "Level: Multiple locks" );
	GLevel = LevelInfo;

	LevelInfo->State		= State;
	LevelInfo->Level		= this;
	LevelInfo->Model		= Model;
	LevelInfo->PlayerList	= PlayerList;
	LevelInfo->BrushArray	= BrushArray;
	LevelInfo->Brush        = BrushArray->Element(0);
	LevelInfo->Model->Lock	(&LevelInfo->ModelInfo,NewLockType);
	LevelInfo->Actors		= ActorList;
	LevelInfo->Actors->Lock(NewLockType);
	LevelInfo->Dynamics.Locked = 0;

    if( ActorList->ActiveActors != 0 )
    {
		// Safety check in case we haven't set up the lists yet.
        ActorList->StaticActors        ->Compress();
        ActorList->DynamicActors       ->Compress();
        ActorList->CollidingActors     ->Compress();
        ActorList->ActiveActors        ->Compress();
        ActorList->UnusedActors        ->Compress();
        ActorList->JustDeletedActors   ->Compress();
    }

	// Go through all actors and clear 'bJustDeleted' so that actors destroyed
	// in the previous tick can be recycled during this tick.
    if( ActorList->JustDeletedActors != 0 )
    {
        while( ActorList->JustDeletedActors->Count() > 0 )
        {
            AActor * Actor = (*ActorList->JustDeletedActors)[0];
            Actor->bJustDeleted = FALSE;
            Actor->Class = 0;
            ActorList->JustDeletedActors->RemoveIndex(0);
            ActorList->UnusedActors->Add(Actor);
        }
    }
	sporeLock(LevelInfo);
	LevelInfo->Dynamics.Lock(&GMem,LevelInfo,Max(1000,LevelInfo->ModelInfo.NumBspNodes*5));

	// Get the LevelDescriptor, if any.
	if ( ActorList->Element(0) && ActorList->Element(0).IsKindOf("LevelDescriptor") ) 
	{
		LevelInfo->Descriptor = (ALevelDescriptor*)&ActorList->Element(0);
		ALevelDescriptor* Descriptor = LevelInfo->Descriptor;

		// Set time info.
		INT UnusedDayOfWeek;
		GApp->SystemTime(&Descriptor->Year,&Descriptor->Month,&UnusedDayOfWeek,
			&Descriptor->Day,&Descriptor->Hour,&Descriptor->Minute,&Descriptor->Second,
			&Descriptor->Millisecond);
		Descriptor->Ticks	= GServer.Ticks;
		Descriptor->Time	=
		(
			Descriptor->Hour/24.0 + 
			Descriptor->Minute/60.0/24.0 + 
			Descriptor->Second/60.0/60.0/24.0 + 
			Descriptor->Millisecond/1000.0/60.0/60.0/24.0
		) * Descriptor->DayFactor + Descriptor->DayBase;
		Descriptor->Time -= (int)Descriptor->Time;

		// Compute DayFraction and NightFraction.
		int Base					= Descriptor->Minute + Descriptor->Hour*60;
		int NoonDistance			= Abs(Base - 12*60);
		int MidnightDistance		= (Base < 12*60) ? Base : (24*60 - Base);
		Descriptor->NightFraction	= (MidnightDistance > 9*60) ? 0.0 : Square(MidnightDistance / (9.0 * 60.0));
		Descriptor->DayFraction		= (NoonDistance     > 9*60) ? 0.0 : Square(NoonDistance     / (9.0 * 60.0));
	}
	else LevelInfo->Descriptor = NULL;

	unguard;
}

void ULevel::Unlock(ILevel *LevelInfo)
{
	guard(ULevel::Unlock);

	if( !GLevel )
		appError("Level: Unlock without lock");
	
	GLevel = NULL;

	LevelInfo->Dynamics.Unlock();
	sporeUnlock();

	State  = LevelInfo->State;
	LevelInfo->Actors->Unlock();
	LevelInfo->Model->Unlock (&LevelInfo->ModelInfo);

	unguard;
}

/*-----------------------------------------------------------------------------
	Level state transitions.
-----------------------------------------------------------------------------*/

//
// Return the level's current state (LEVEL_UpPlay, etc).
//
ELevelState ULevel::GetState()
{
	guard(ULevel::GetState);
	return State;
	unguard;
}

//
// Set the level's state.  Notifies all actors of the state change.
// If you're setting the state to LEVEL_UP_PLAY, you must specify
// the network mode and difficulty level.
//
void ULevel::SetState(ELevelState NewState,PBeginPlay *PlayInfo)
{
	guard(ULevel::SetState);

	ILevel		Level;
	ELevelState	OldState;

	OldState	= State;
	State		= NewState;

	if (NewState == OldState) 
		return;

	// Send messages to all level actors notifying state we're exiting.
	Lock (&Level,LOCK_NoTrans);

	if (OldState==LEVEL_UpPlay)	Level.BroadcastMessage (ACTOR_EndPlay,NULL);
	if (OldState==LEVEL_UpEdit)	Level.BroadcastMessage (ACTOR_EndEdit,NULL);

	// Send message to actors notifying them of new level state.
	if( NewState == LEVEL_UpPlay )
	{
		if (!PlayInfo)
			appError ("Missing PlayInfo");
		
		this->PlayInfo = *PlayInfo;
		debugf (LOG_Server,"Level %s is now up for play",Name);

		Level.BroadcastMessage(ACTOR_PreBeginPlay,PlayInfo);
		Level.BroadcastMessage(ACTOR_BeginPlay,PlayInfo);

		for( INDEX i=0; i<Level.Actors->Max; i++ )
		{
			AActor *Actor = &Level.Actors->Element(i);
			if( Actor->Class )
			{
				Actor->Zone = 0;
				Level.SetActorZone(i);
			}
		}
		Level.BroadcastMessage(ACTOR_PostBeginPlay,PlayInfo);
	}
	else if( NewState == LEVEL_UpEdit )
	{
		debugf (LOG_Server,"Level %s is now up for play",Name);
		Level.BroadcastMessage(ACTOR_BeginEdit,NULL);
	}
	else if( NewState == LEVEL_Down )
	{
		debugf (LOG_Server,"Level %s is now down",Name);
	}
	else 
	{
		appErrorf ("SetLevelState: Bad state %i",NewState);
	}
	Unlock (&Level);
	unguard;
}

/*-----------------------------------------------------------------------------
	Level resource implementation.
-----------------------------------------------------------------------------*/

void ULevel::Register(FResourceType *Type)
{
	guard(ULevel::Register);

	Type->HeaderSize = sizeof (ULevel);
	Type->RecordSize = 0;
	Type->Version    = 1;
	strcpy (Type->Descr,"Level");

	unguard;
}
void ULevel::InitHeader()
{
	guard(ULevel::InitHeader);

	// Init resource header to defaults.
	State			= LEVEL_Down;
	BrushArray		= NULL;
	Model		    = NULL;
	ActorList		= NULL;
	Misc			= NULL;

	// Clear all text blocks.
	for (int i=0; i<NUM_LEVEL_TEXT_BLOCKS; i++)
		TextBlocks[i]=NULL;

	unguard;
}
const char *ULevel::Import(const char *Buffer, const char *BufferEnd,const char *FileType)
{
	guard(ULevel::Import);

	int ImportedActiveBrush=0,NumBrushes=0;
	char StrLine[256],BrushName[NAME_SIZE];
	const char *StrPtr;

	if (BrushArray->Num==0)
		appError ("No active brush in level");

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
					Buffer    = TempModel->Import(Buffer,BufferEnd,FileType);
					if (!Buffer) return NULL;
					ImportedActiveBrush = 1;
				}
				else
				{
					// Parse a new brush, which has not yet been allocated.
					GRes.MakeUniqueName(BrushName,Name,"_S",RES_Model);
					TempModel = new(BrushName,CREATE_Unique)UModel;
					Buffer = TempModel->Import(Buffer,BufferEnd,FileType);
					if (!Buffer) return NULL;
					BrushArray->Add(TempModel);						
				}
				TempModel->ModelFlags |= MF_Selected;
			}
		}
		else if( GetBEGIN(&StrPtr,"ACTORLIST") )
		{
			Buffer = ActorList->Import(Buffer,BufferEnd,FileType);
			if (!Buffer)
				return NULL;
		}
	}
	return Buffer;
	unguard;
}
char *ULevel::Export(char *Buffer,const char *FileType,int Indent)
{
	guard(ULevel::Export);

	Buffer += sprintf (Buffer,"%s;\r\n",spc(Indent));
	Buffer += sprintf (Buffer,"%s; Unreal World Editor\r\n",spc(Indent));
	Buffer += sprintf (Buffer,"%s; Version: %s\r\n",spc(Indent),ENGINE_VERSION);
	Buffer += sprintf (Buffer,"%s; Exported map \r\n",spc(Indent));
	Buffer += sprintf (Buffer,"%s;\r\n",spc(Indent));

	Buffer += sprintf (Buffer,"%sBegin Map Name=%s Brushes=%i\r\n",spc(Indent),Name,BrushArray->Num);
	Buffer += sprintf (Buffer,"%s   ;\r\n",spc(Indent));

	// Export brushes.
	for (int i=0; i<BrushArray->Num; i++)
	{
		Buffer = BrushArray->Element(i)->Export(Buffer,FileType,Indent+3);
		if (!Buffer) return NULL;
	}

	// Export actors.
	Buffer  = ActorList->Export(Buffer,FileType,Indent+3);
	if (!Buffer)
		return NULL;

	Buffer += sprintf (Buffer,"%sEnd Map\r\n",spc(Indent));

	return Buffer;
	unguard;
}
void ULevel::QueryHeaderReferences(FResourceCallback &Callback)
{
	guard(ULevel::QueryHeaderReferences);

	Callback.Resource(this,(UResource**)&Model      ,0);
	Callback.Resource(this,(UResource**)&ActorList  ,0);
	Callback.Resource(this,(UResource**)&BrushArray ,0);
	Callback.Resource(this,(UResource**)&Misc       ,0);

	for (int i=0; i<NUM_LEVEL_TEXT_BLOCKS; i++)
		Callback.Resource(this,(UResource**)&TextBlocks[i],0);

	unguard;
}
void ULevel::QueryDataReferences(FResourceCallback &Callback)
{
	guard(ULevel::QueryDataReferences);
	unguard;
}
void ULevel::Flip()
{	
	guard(ULevel::Flip);

	UResource::Flip();
	//todo:

	unguard;
}
AUTOREGISTER_RESOURCE(RES_Level,ULevel,0xB2D90857,0xCCD211cf,0x91360000,0xC028B992);

/*-----------------------------------------------------------------------------
	Level link topic.
-----------------------------------------------------------------------------*/

AUTOREGISTER_TOPIC("Lev",LevTopicHandler);
void LevTopicHandler::Get(ULevel *Level, const char *Topic, const char *Item, char *Data)
{
	guard(LevTopicHandler::Get);

	UTextBuffer	*Text;
	int			ItemNum;

	if (!isdigit (Item[0]))
		return; // Item isn't a number.

	ItemNum = atoi (Item);
	if ((ItemNum < 0) || (ItemNum >= ULevel::NUM_LEVEL_TEXT_BLOCKS)) 
		return; // Invalid text block number.

	Text = Level->TextBlocks[ItemNum];

	if (Text) 
		strcpy (Data,Text->GetData());

	unguard;
}
void LevTopicHandler::Set(ULevel *Level, const char *Topic, const char *Item, const char *Data)
{
	guard(LevTopicHandler::Set);

	UTextBuffer	*Text;
	char		Name[NAME_SIZE];
	int			ItemNum;

	if (!isdigit (Item[0]))
		return; // Item isn't a number.

	ItemNum = atoi (Item);
	if ((ItemNum < 0) || (ItemNum >= ULevel::NUM_LEVEL_TEXT_BLOCKS)) return; // Invalid text block number

	GRes.CombineName (Name,Level->Name,"T",ItemNum);

	Text = new(Name,CREATE_Replace)UTextBuffer(strlen(Data)+1,1);
	strcpy (Text->GetData(),Data);

	Level->TextBlocks[ItemNum] = Text;

	unguard;
}

/*-----------------------------------------------------------------------------
	Reconcile actors and cameras after loading or creating a new level.
-----------------------------------------------------------------------------*/

//
// These functions provide the basic mechanism by which UnrealEd associates
// cameras and actors together, even when new maps are loaded which contain
// an entirely different set of actors which must be mapped onto the existing cameras.
//

//
// Step 1: Remember actors.  This is called prior to loading a level, and it
// places temporary camera actor location/status information into each camera,
// which can be used to reconcile the actors once the new level is loaded and the
// current actor list is replaced with an entirely new one.
//
void ULevel::RememberActors()
{
	guard(ULevel::RememberActors);
	UCamera	*Camera;

	for( int i=0; i<GCameraManager->CameraArray->Num; i++ )
	{
		Camera = GCameraManager->CameraArray->Element(i);
		Camera->RememberedActor = Camera->GetActor();
	}
	unguard;
}

//
// Remove all camera references from all actors in this level.
//
void ULevel::DissociateActors()
{
	guard(ULevel::DissociateActors);

	AActor *Actor = &ActorList->Element(0);
	
	for( INDEX i=0; i<ActorList->Max; i++ )
	{
		APawn *Player = Actor->GetPlayer();
		if( Player )
		{
			Player->Camera = NULL;
		}
		Actor++;
	}
	unguard;
}

//
// Step 2: Reconcile actors.  This is called after loading a level.
// It attempts to match each existing camera to an actor in the newly-loaded
// level.  If no decent match can be found, creates a new actor for the camera.
//
void ULevel::ReconcileActors( int Remembered )
{
	guard(ULevel::ReconcileActors);

	ILevel		LevelInfo;
	UCamera		*Camera;
	AActor		*Actor;
	FName		Name;

	Lock( &LevelInfo, LOCK_NoTrans );

	// Dissociate all actor Cameras.
	DissociateActors();

	if( LevelInfo.State == LEVEL_UpEdit )
	{
		// Match cameras and camera-actors with identical names.  These cameras
		// will obtain all of their desired display properties from the actors.
		for( int i=0; i<GCameraManager->CameraArray->Num; i++ )
		{
			Camera			= GCameraManager->CameraArray->Element(i);
			Camera->Level   = this;
			Camera->iActor	= INDEX_NONE;
			Name.Add(Camera->Name);

			for( INDEX j=0; j<LevelInfo.Actors->Max; j++ )
			{
				Actor = &LevelInfo.Actors->Element(j);
				if( Actor->Name==Name && Actor->IsKindOf("Camera") )
				{
					debugf (LOG_Info,"Matched camera %s",Camera->Name);
					Camera->iActor = j;

					goto NextCam;
				}
			}
			NextCam:;
		}

		// Match up all remaining cameras to actors.  These cameras will get default
		// display properties.
		for( i=0; i<GCameraManager->CameraArray->Num; i++ )
		{
			Camera = GCameraManager->CameraArray->Element(i);

			// Hook camera up to an existing (though unpossessed) camera actor, or create
			// a new camera actor for it.  Sends ACTOR_SPAWN and ACTOR_POSSESS.  Returns
			// actor index or INDEX_NONE if failed.
			if( Camera->iActor == INDEX_NONE )
			{
				Name.Add(Camera->Name);
				Camera->iActor = LevelInfo.SpawnCameraActor(Camera,Name);
				if( Camera->iActor == INDEX_NONE )
				{
					debug(LOG_Ed,"Camera spawn failed");
					Camera->Kill();
					i--;
				}
				else
				{
					debugf (LOG_Info,"Spawned camera %s",Camera->Name);
					if( Remembered )
					{
						AActor &Actor	= Camera->GetActor();
						Actor			= Camera->RememberedActor;
						Actor.iMe		= Camera->iActor;
						Actor.SetBinPointers(Actor.Class);
					}
				}
			}
		}
	}
	else
	{
		// State==LEVEL_UpPlay
		for( int i=0; i<GCameraManager->CameraArray->Num; i++ )
		{
			Camera          = GCameraManager->CameraArray->Element(i);
			Camera->iActor  = LevelInfo.SpawnPlayActor(Camera);

			if (Camera->iActor==INDEX_NONE) appError (
				"Can't play this level: No 'PlayerStart' actor was found to "
				"specify the player's starting position.");
		}
	}
	Unlock (&LevelInfo);

	// Associate cameras and actors.
	GCameraManager->UpdateActorUsers();

	// Kill any remaining camera actors.
	Lock (&LevelInfo,LOCK_NoTrans);
	for( INDEX i=0; i<LevelInfo.Actors->Max; i++ )
	{
		AActor *Actor = &LevelInfo.Actors->Element(i);

		if( Actor && Actor->IsKindOf("Camera") && !((ACamera*)Actor)->Camera )
		{
			LevelInfo.DestroyActor(i);
		}
	}
	Unlock (&LevelInfo);
	unguard;
}

/*-----------------------------------------------------------------------------
	ULevel command-line.
-----------------------------------------------------------------------------*/

int ULevel::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(ULevel::Exec);

	const char *Str = Cmd;
	if( GetCMD(&Str,"STATUS" ) && ( GetCMD(&Str,"LEVEL" ) || !Str[0]))
	{
		int n=0;
		for (int i=0; i<ActorList->Max; i++) if (ActorList->Element(i).Class) n++;

		Out->Logf("   LEVEL - %s, %i/%i actors",GLevelStateDescr[State],n,ActorList->Max);
		return Str[0]!=0;
	}
	else if( GetCMD(&Str,"KILLACTORS") )
	{
		ILevel LevelInfo;
		Lock(&LevelInfo,LOCK_NoTrans);
		AActor *Actor = &LevelInfo.Actors->Element(0);
		for( int i=0; i<LevelInfo.Actors->Max; i++ )
		{
			if( Actor->Class && !Actor->IsKindOf("Light") && 
				!Actor->GetPlayer() )
			{
				LevelInfo.DestroyActor(i);
			}
			Actor++;
		}
		Unlock(&LevelInfo);
		Out->Log("Killed all actors");
		return 1;
	}
	else if( GetCMD(&Str,"KILLPAWNS") )
	{
		ILevel LevelInfo;
		Lock(&LevelInfo,LOCK_NoTrans);
		AActor *Actor = &LevelInfo.Actors->Element(0);
		for (int i=0; i<LevelInfo.Actors->Max; i++)
		{
			if( Actor->Class && Actor->IsKindOf("Pawn") && !Actor->GetPlayer() )
				LevelInfo.DestroyActor(i);

			Actor++;
		}
		Out->Log("Killed all NPC's");
		Unlock(&LevelInfo);
		return 1;
	}
	else if( GetCMD(&Str,"LINKS") )
	{
		Out->Log("Level links:");
		AActor *Actor = &ActorList->Element(0);
		for (int i=0; i<ActorList->Max; i++)
		{
			Actor++;
		}
		return 1;
	}
	else if( GetCMD(&Str,"ACTORS") )
	{
		int TotalCount=0,TotalStatic=0,TotalCollision=0;

		int *ActorCount		= (int *)GMem.GetZeroed(GRes.MaxRes * sizeof(int));
		int *StaticCount	= (int *)GMem.GetZeroed(GRes.MaxRes * sizeof(int));
		int *CollisionCount	= (int *)GMem.GetZeroed(GRes.MaxRes * sizeof(int));

		UClass *Class=NULL;
		GetUClass(Str,"CLASS=",&Class);

		AActor *Actor = &ActorList->Element(0);
		for( int i=0; i<ActorList->Max; i++ )
		{
			if( Actor->Class )
			{
				ActorCount[Actor->Class->Index]++;
				TotalCount++;

				if (Actor->bStatic)
				{
					StaticCount[Actor->Class->Index]++;
					TotalStatic++;
				}
				if (Actor->bCollideActors)
				{
					CollisionCount[Actor->Class->Index]++;
					TotalCollision++;
				}
			}
			Actor++;
		}
		if (TotalCount>0)
			Out->Logf("Actors:");

		UClass *TestClass;
		FOR_ALL_TYPED_RES(TestClass,RES_Class,UClass)
		{
			if( ActorCount[TestClass->Index] && ((Class==NULL) || (Class==TestClass)) )
			{
				Out->Logf
				(
					" %s...%i (%i, %i)",TestClass->Name,
					ActorCount		[TestClass->Index],
					StaticCount		[TestClass->Index],
					CollisionCount	[TestClass->Index]
				);
			}
		}
		END_FOR_ALL_TYPED_RES;

		Out->Logf("%i Actors (%i static, %i collision)",TotalCount,TotalStatic,TotalCollision);
		GMem.Release(ActorCount);

		return 1;
	}
	else return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
