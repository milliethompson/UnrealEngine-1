/*=============================================================================
	AActor.h: Class functions residing in the AActor class.

	Note: This file is included during the AActor class
	definition when Root.h is compiled so that the builtin class functions
	of all actor classes can be customized here.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifdef _INC_AACTOR
#error AActor.h must only be included once!
#endif
#define _INC_AACTOR

/*-----------------------------------------------------------------------------
	Class functions
-----------------------------------------------------------------------------*/

//class UNENGINE_API AActor {
public:

	// AActor inlines.
	inline class APawn *GetPlayer() const;
	inline class ULevel *GetLevel() const;
	inline BOOL IsPlayer() const;
	inline BOOL IsOwnedBy( const AActor *TestOwner ) const;
	inline BOOL IsOverlapping( const AActor *Other ) const;
	inline FLOAT WorldLightRadius() const {return 25.0 * ((int)LightRadius+1);}
	inline FLOAT WorldSoundRadius() const {return 25.0 * ((int)SoundRadius+1);}
	inline FLOAT WorldVolumetricRadius() const {return 25.0 * ((int)VolumeRadius+1);}
	inline BOOL IsBlockedBy( const AActor *Other ) const;

	// AActor collision functions.
	void GetCollisionExtent
	(
		FVector &Min,
		FVector &Max
	) const;
	void GetCoordinates
	(
		FCoords &Coords,
		FCoords &Uncoords
	) const;
	INT PointCheck
	(
		FCheckResult	&Result,
		const FVector	&Location,
		DWORD			ExtraNodeFlags
	);
	INT LineCheck
	(
		FCheckResult	&Result,
		const FVector	&V1,
		const FVector	&V2,
		DWORD ExtraNodeFlags
	);
	INT BoxPointCheck
	(
		FCheckResult	&Result,
		const FVector   &Point,
		FLOAT           Radius,
		FLOAT           Height,
		DWORD           ExtraNodeFlags
	);
	INT BoxLineCheck
	(
		FCheckResult	&Result,
		const FVector   &Start,
		const FVector   &End,
		FLOAT           Radius,
		FLOAT           Height,
		DWORD           ExtraNodeFlags
	);

	// AActor general functions.
	void BeginTouch(AActor *Other);
	void EndTouch(AActor *Other, BOOL NoNotifySelf);
	void SetOwner( AActor *Owner );
	void UpdateBrushPosition(ULevel *Level, int Editor);
	void BeginExecution();
	int  IsBrush();
	int  IsMovingBrush();
	int  GotoState(FName State);
	int  GotoLabel(FName Label);
	void SetCollision(BOOL NewCollideActors,BOOL NewBlockActors,BOOL NewBlockPlayers);
	void SetFloor(AActor *NewFloor);

	// AActor audio.
	void UpdateSound();
	void MakeSound( USound *Sound, FLOAT Radius=0.f, FLOAT Volume=1.f, FLOAT Pitch=1.f );
	void PrimitiveSound( USound *Sound, FLOAT Volume=1.f, FLOAT Pitch=1.f );
	void SetAmbientSound( USound *NewAmbient );

	// Physics functions.
	void performPhysics(FLOAT DeltaSeconds);
	void physProjectile(FLOAT deltaTime);
	void physFalling(FLOAT deltaTime);
	void physicsRotation(FLOAT deltaTime, FVector &OldVelocity);
	int fixedTurn(int current, int desired, int deltaRate, int fixed, int clockwise); 
	void Bump(AActor *Bumped);
	inline FLOAT FloorZ(FVector Location);  
	inline void TwoWallAdjust(FVector &DesiredDir, FVector &Delta, FVector &HitNormal, FVector &OldHitNormal, FLOAT HitTime);

	//AI functions
	void CheckNoiseHearing(FLOAT Loudness);
	int CanBeHeardBy(AActor *Other);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
