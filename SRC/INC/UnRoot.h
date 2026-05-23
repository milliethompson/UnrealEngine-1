/*=============================================================================
	UnRoot.h: Class functions residing in the AActor class.

	Note: This file is included during the AActorBase class
	definition when Root.h is compiled so that the builtin class functions
	of all actor classes can be customized here.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifdef _INC_UNROOT
#error UnRoot must only be included once!
#endif

/*-----------------------------------------------------------------------------
	Class functions
-----------------------------------------------------------------------------*/

//class UNREAL_API AActorBase {
public:

	// Constants.
	enum {MAX_TOUCHING_ACTORS = 4};		// Maximum actors whose touch status is tracked.
	enum {AI_INFO_SIZE        = 188};	// Size of information structure needed for AI.
	enum {CLASS_PROP_EXTRA    = 640};	// Size of extra class property info usable by AActor-derived classes.

	// Types.
	enum EPropertyBin
	{
		PROPBIN_Global		= 0,		// Actor global data.
		PROPBIN_Static		= 1,		// Actor static data.
		PROPBIN_Local		= 2,		// Actor local data at current nesting level.
		PROPBIN_MAX			= 3,		// Maximum.
	};
	class FActorPrivate
	{
	public:
		BYTE *PropertyBins[PROPBIN_MAX];// Pointers to the actor's property bins.
		BYTE AIInfo[AI_INFO_SIZE];		// Info used for internal AI code.
	};
	typedef int (AActorBase::*ACTOR_PROCESS_FUNC)(ILevel *Level, FName Message, void *Params);

	// Conversions.
	// Here we cast via a void pointer because the cast (AActor*)this is a recursive cast.
	inline operator AActor &() {return*(AActor*)(void*)this;}
	inline operator AActor *() {return (AActor*)(void*)this;}

	// Inlines.
	inline class APawn *GetPlayer() const;
	inline BOOL IsKindOf( const class UClass *SomeParent ) const;
	inline BOOL IsKindOf( const char *Name ) const;
	inline const void *GetPropertyPtr( int iProperty,int iElement=0 ) const;
	inline void *GetPropertyPtr( int iProperty,int iElement=0 );
	inline void *GetPropertyPtrFromClass( UClass *Class, int iProperty,int iElement=0 );
	inline FLOAT WorldLightRadius() const {return 25.0 * ((int)LightRadius+1);}
	inline FLOAT WorldSoundRadius() const {return 25.0 * ((int)SoundRadius+1);}
	inline FLOAT WorldVolumetricRadius() const {return 25.0 * ((int)VolumeRadius+1);}
	inline INT MaxClassPropertySize() const {return sizeof(AActorBase) + CLASS_PROP_EXTRA;}
	inline BOOL IsBlockedBy( const AActor &Other ) const;

	// Functions.
	void Init(UClass *Class);
	void QueryReferences(UResource *ParentRes,FResourceCallback &Callback, DWORD ContextFlags, INT AllProperties);
	void PostLoad(UResource *ParentRes, INT AllProperties);
	void GetDrawCoords(FCoords *Coords) const;
	void GetViewCoords(FCoords *Coords) const;
	void TransformPoint(FVector &LocalPoint, const FVector &WorldPoint);
	void UpdateBrushPosition(ILevel *Level,INDEX iActor,int Editor);
	void SetBinPointers(UClass *Class);
	int  IsBrush();
	int  IsMovingBrush();
//};

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
