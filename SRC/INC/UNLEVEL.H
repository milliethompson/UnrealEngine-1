/*=============================================================================
	UnLevel.h: ULevel definition.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * July 21, 1996: Mark added GLevel
        * Aug  31, 1996: Mark added GRestartLevelAfterTick
        * Aug  31, 1996: Mark added GJumpToLevelAfterTick
=============================================================================*/

#ifndef _INC_UNLEVEL
#define _INC_UNLEVEL

/*-----------------------------------------------------------------------------
	ULevel object.
-----------------------------------------------------------------------------*/

//
// The level object.  Contains the level's actor list, Bsp information, and brush list.
//
class UNENGINE_API ULevel : public UDatabase, public FOutputDevice
{
	DECLARE_DB_CLASS(ULevel,UDatabase,AActor*,NAME_Level,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Number of blocks of descriptive text to allocate with levels.
	enum {NUM_LEVEL_TEXT_BLOCKS=16};

	// Main variables, always valid.
	UModel*					Model;
	TArray<UModel*>::Ptr	BrushArray;
	TArray<UObject*>::Ptr	Misc;
	UReachSpecs::Ptr		ReachSpecs;
	UTextBuffer::Ptr		TextBlocks[NUM_LEVEL_TEXT_BLOCKS];

	// Only valid in memory.
	FCollisionHash			Hash;
	AActor					*FirstDeleted;
	ALevelInfo				*Info;

	// UObject interface.
	const char *Import      (const char *Buffer, const char *BufferEnd,const char *FileType);
	const char *ImportActors(const char *Buffer, const char *BufferEnd,const char *FileType);
	void Export      (FOutputDevice &Out,const char *FileType,int Indent);
	void ExportActors(FOutputDevice &Out,const char *FileType,int Indent);
	void PreKill();
	void PostLoadHeader(DWORD PostFlags)
	{
		guard(ULevel::PostLoadHeader);

		// Postload parent.
		UDatabase::PostLoadHeader( PostFlags );

		// Init level state if loading.
		if( PostFlags & POSTLOAD_File )
			State = LEVEL_Down;

		// Init collision.
		Hash.CollisionInitialized = 0;

		unguard;
	}
	void InitHeader()
	{
		guard(ULevel::InitHeader);

		// Init parent.
		UDatabase::InitHeader();

		// Init object header to defaults.
		State			= LEVEL_Down;
		BrushArray		= (TArray<UModel*>   *)NULL;
		Model           = (UModel            *)NULL;
		Misc			= (TArray<UObject*>  *)NULL;
		ReachSpecs		= (UReachSpecs       *)NULL;

		// Clear all text blocks.
		for( int i=0; i<NUM_LEVEL_TEXT_BLOCKS; i++ )
			TextBlocks[i]=(UTextBuffer *)NULL;

		// Init in-memory info.
		Hash.CollisionInitialized = 0;
		FirstDeleted              = NULL;

		unguard;
	}
	void SerializeHeader(FArchive &Ar)
	{
		guard(ULevel::SerializeHeader);

		// UDatabase.
		UDatabase::SerializeHeader(Ar);

		// ULevel.
		Ar << AR_OBJECT(Model) << AR_OBJECT(BrushArray) << AR_OBJECT(Misc);
		Ar << ReachSpecs;

		for( int i=0; i<NUM_LEVEL_TEXT_BLOCKS; i++ )
			Ar << TextBlocks[i];

		unguard;
	}
	void PostLoadData(DWORD PostFlags)
	{
		guard(ULevel::PostLoadData);
		UDatabase::PostLoadData(PostFlags);
		for( int i=Num; i<Max; i++ )
			Element(i)=NULL;
		unguard;
	}
	void SerializeData( FArchive &Ar )
	{
		int i=-1;
		guard(ULevel::SerializeData);
		UDatabase::Lock(LOCK_ReadWrite);
		for( i=0; i<Num; i++ )
			Ar << Element(i);
		UDatabase::Unlock(LOCK_ReadWrite);
		unguardf(("(%s %s: %i/%i)",this?GetClassName():"NULL",this?GetName():"NULL",i,Num));
	}

	// FOutputDevice interface.
	void Write(const void *Data, int Length, ELogType MsgType=LOG_None);

	// ULevel interface, always valid.
	ULevel						(int InMax, int Editable, int RootOutside);
	int     Lock 				(DWORD LockType);
	void	Unlock 				(DWORD OldLockType);
	void	EmptyLevel			();
	void	SetState			(ELevelState State);
	void	Tick				(int CamerasOnly, AActor *ActiveActor,FLOAT DeltaSeconds);
	ELevelState GetState		() {return State;}
	void	ReconcileActors		(int Remembered);
	void	RememberActors		();
	void	DissociateActors	();
	int		Exec				(const char *Cmd,FOutputDevice *Out=GApp);
	void	PlayerExec			(AActor *Actor,const char *Cmd,FOutputDevice *Out=GApp);
	void	ShrinkLevel			();
	void	ModifyAllItems		();

	// Actor-related functions, valid only when locked.
	void	SendEx				(FName Message, PMessageParms *Parms, AActor *Actor=NULL, FName TagName=NAME_None, UClass *Class=NULL);
	int		TestMoveActor		(AActor *Actor, FVector &Start, FVector &End);
	int		RotateActor			(AActor *Actor, FRotation &DestRotation, int Test=0);
	int		MoveActor			(AActor *Actor, FVector *Delta, FCheckResult &Hit, int Test=0);
	int		FarMoveActor		(AActor *Actor, FVector *DestLocation, int Test=0);
	int		DropToFloor			(AActor *Actor);
	int     DestroyActor		(AActor *Actor);
	void    CleanupDestroyed    ();
	int		PossessActor		(APawn *Actor, UCamera *Camera);
	UCamera*UnpossessActor		(APawn *Actor);
	AActor* SpawnActor			(UClass *Class, AActor *Owner=NULL, FName ActorName=NAME_None, const FVector *Location=NULL, const FRotation *Rotation=NULL, AActor *Template=NULL);
	AView*  SpawnViewActor      (UCamera *Camera,FName MatchName,AActor *Template);
	int		SpawnPlayActor		(UCamera *Camera);
	void    SetActorZone        (AActor *Actor, BOOL bForceRefresh=0);
	void	Trace               (FCheckResult &Hit, AActor *SourceActor, const FVector &End, const FVector &Start, BOOL TraceActors);

	// Collision.
	void InitCollision()
	{
		guard(ULevel::InitCollision);
		checkState(!IsLocked());

		// Init the hash table.
		Hash.Init();

		// Init all actor collision pointers.
		for( int i=0; i<Num; i++ )
			if( Element(i) )
				Element(i)->Hash = NULL;

		// Add all actors.
		for( i=0; i<Num; i++ )
			if( Element(i) && Element(i)->bCollideActors )
				Hash.AddActor( Element(i) );
		
		unguard;
	}

	// Accessors.
	UModel *Brush()
	{
		guardSlow(ULevel::Brush);
		return BrushArray->Element(0);
		unguardSlow;
	}

	INDEX GetActorIndex( AActor *Actor )
	{
		guard(ULevel::GetActorIndex);
		for( int i=0; i<Num; i++ )
			if( Element(i) == Actor )
				return i;
		appErrorf( "Actor not found: %s %s", Actor->GetClassName(), Actor->GetName() );
		return INDEX_NONE;
		unguard;
	}
	ALevelInfo *GetLevelInfo()
	{
		guardSlow(ULevel::GetLevelInfo);
		checkState(Element(0)!=NULL);
		checkState(Element(0)->IsA("LevelInfo"));
		return (ALevelInfo*)Element(0);
		unguardSlow;
	}
	AZoneInfo *GetZoneActor( INT iZone )
	{
		guardSlow(ULevel::GetZoneActor);
		return Model->Nodes->Zones[iZone].ZoneActor ? Model->Nodes->Zones[iZone].ZoneActor : GetLevelInfo();
		unguardSlow;
	}

private:
	// Private.
	ELevelState				State;
	int PlaceActor(AActor *Actor, FVector *DestLocation);
	int		MyBoxPointCheck
								(
								FCheckResult	&Result,
								AActor			*Owner,
								const FVector   &Point,
								FLOAT           Radius,
								FLOAT           Height,
								DWORD           ExtraNodeFlags
								); //FIXME - temporary, remove
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNLEVEL
