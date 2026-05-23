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
// Trace actor options.
//
enum ETraceActorFlags
{
	// Bitflags.
	TRACE_Pawns         = 0x01, // Check collision with pawns.
	TRACE_Movers        = 0x02, // Check collision with movers.
	TRACE_Level         = 0x04, // Check collision with level geometry.
	TRACE_ZoneChanges   = 0x08, // Check collision with soft zone boundaries.
	TRACE_Others        = 0x10, // Check collision with all other kinds of actors.

	// Combinations.
	TRACE_VisBlocking   = TRACE_Level | TRACE_Movers,
	TRACE_AllColliding  = TRACE_Pawns | TRACE_Movers | TRACE_Level | TRACE_Others,
	TRACE_AllEverything = TRACE_Pawns | TRACE_Movers | TRACE_Level | TRACE_ZoneChanges | TRACE_Others,
};

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
		Ar << AR_OBJECT(Model) << AR_OBJECT(BrushArray);
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
	void	ReconcileActors		();
	void	RememberActors		();
	void	DissociateActors	();
	int		Exec				(const char *Cmd,FOutputDevice *Out=GApp);
	void	PlayerExec			(AActor *Actor,const char *Cmd,FOutputDevice *Out=GApp);
	void	ShrinkLevel			();
	void	ModifyAllItems		();

	// Actor-related functions, valid only when locked.
	void	SendEx				(FName Message, PMessageParms *Parms, AActor *Actor=NULL, FName TagName=NAME_None, UClass *Class=NULL);
	int		TestMoveActor		(AActor *Actor, FVector Start, FVector End, BOOL IgnorePawns);
	int		MoveActor			(AActor *Actor, FVector Delta, FRotation NewRotation, FCheckResult &Hit, BOOL Test=0, BOOL IgnorePawns=0, BOOL bIgnoreBases=0);
	int		FarMoveActor		(AActor *Actor, FVector DestLocation, BOOL Test=0, BOOL bNoCheck=0);
	int		DropToFloor			(AActor *Actor);
	int     DestroyActor		(AActor *Actor);
	void    CleanupDestroyed    ();
	int		PossessActor		(APawn *Actor, UCamera *Camera);
	UCamera*UnpossessActor		(APawn *Actor);
	AActor* SpawnActor			(UClass *Class, AActor *Owner=NULL, FName ActorName=NAME_None, FVector Location=FVector(0,0,0), FRotation Rotation=FRotation(0,0,0), AActor* Template=NULL);
	AView*  SpawnViewActor      (UCamera *Camera,FName MatchName);
	int		SpawnPlayActor		(UCamera *Camera);
	void    SetActorZone        (AActor *Actor, BOOL bTest=0, BOOL bForceRefresh=0);
	int		Trace               (FCheckResult &Hit, AActor *SourceActor, const FVector &End, const FVector &Start, DWORD TraceFlags, FVector Extent=FVector(0,0,0) );
	int		FindSpot			(FVector Extent, FVector &Location, BOOL bCheckActors);
	int		CheckEncroachment	(AActor* Actor, FVector TestLocation, FRotation TestRotation);

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
	ELevelState				State;
	int MyBoxPointCheck( FCheckResult &Result, AActor* Owner, FVector Point, FVector Extent, DWORD ExtraNodeFlags ); //FIXME - temporary, remove
};

/*-----------------------------------------------------------------------------
	FTestMove.
-----------------------------------------------------------------------------*/

//
// Mechanism for remembering the location of actors so that
// they can be restored after testing movement with ULevel::MoveActor.
//
class FTestMove
{
private:
	// Information about a remembered actor.
	class FRememberedActor
	{
	public:
		// Variables.
		AActor*				Actor;
		FVector				Location;
		FRotation			Rotation;
		AZoneInfo*			Zone;
		INT					ZoneNumber;
		FRememberedActor*	Next;

		// Constructor.
		FRememberedActor( AActor* InActor, FRememberedActor* InNext )
		:	Actor			(InActor)
		,	Location		(Actor->Location)
		,	Rotation		(Actor->Rotation)
		,	Zone			(Actor->Zone)
		,	ZoneNumber		(Actor->ZoneNumber)
		,	Next			(InNext)
		{}

		// Undo.
		void Pop()
		{
			Actor->Location   = Location;
			Actor->Rotation   = Rotation;
			Actor->Zone       = Zone;
			Actor->ZoneNumber = ZoneNumber;
		}
	};

	// Variables.
	ULevel::Ptr				Level;
	AActor*					Actor;
	FMemStack&				Mem;
	FMemMark				Mark;
	FRememberedActor*		First;

	// Remember an actor.
	void RememberActor( AActor* Actor )
	{
		Level->Hash.RemoveActor( Actor );
		First = new( Mem )FRememberedActor( Actor, First );
		if( Actor->StandingCount )
			for( int i=0; i<Level->Num; i++ )
				if( Level(i) && Level(i)->Base==Actor )
					RememberActor( Level(i) );
	}

public:
	// Constructor.
	FTestMove( ULevel::Ptr InLevel, AActor* InActor, FMemStack& InMem )
	:	Level	(InLevel)
	,	Actor	(InActor)
	,	Mem		(InMem)
	,	Mark	(Mem)
	,	First	(NULL)
	{
		// Remember all actors.
		RememberActor( Actor );
	}

	// Clean up.
	void Pop()
	{
		for( FRememberedActor* Remembered=First; Remembered; Remembered=Remembered->Next )
		{
			Remembered->Pop();
			Level->Hash.AddActor( Remembered->Actor );
		}
		Mark.Pop();
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNLEVEL
