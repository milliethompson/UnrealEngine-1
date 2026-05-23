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
	class APawn *GetPlayer() const;
	class ULevel *GetLevel() const;
	BOOL IsPlayer() const;
	BOOL IsOwnedBy( const AActor *TestOwner ) const;
	FLOAT WorldLightRadius() const {return 25.0 * ((int)LightRadius+1);}
	FLOAT WorldSoundRadius() const {return 25.0 * ((int)SoundRadius+1);}
	FLOAT WorldVolumetricRadius() const {return 25.0 * ((int)VolumeRadius+1);}
	BOOL IsBlockedBy( const AActor *Other ) const;
	BOOL IsIn( const AZoneInfo *Other ) const;
	BOOL IsBasedOn( const AActor *Other ) const;
	FCoords ToLocal() const;
	FCoords ToWorld() const;
	FVector GetCollisionExtent() const {return FVector(CollisionRadius,CollisionRadius,CollisionHeight);}

	// AActor collision functions.
	UPrimitive* GetPrimitive() const;
	BOOL IsOverlapping ( const AActor *Other ) const;


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
	void SetBase(AActor *NewBase);

	// AActor audio.
	void UpdateSound();
	void MakeSound( USound *Sound, FLOAT Radius=0.f, FLOAT Volume=1.f, FLOAT Pitch=1.f );
	void PrimitiveSound( USound *Sound, FLOAT Volume=1.f, FLOAT Pitch=1.f );
	void SetAmbientSound( USound *NewAmbient );

		//physics functions
	void setPhysics(BYTE NewPhysics, AActor *NewFloor = NULL);
	void performPhysics(FLOAT DeltaSeconds);
	void physProjectile(FLOAT deltaTime);
	void physFalling(FLOAT deltaTime);
	void physRolling(FLOAT deltaTime);
	void physicsRotation(FLOAT deltaTime, FVector OldVelocity);
	int fixedTurn(int current, int desired, int deltaRate, int fixed, int clockwise); 
	void Bump(AActor *Bumped);
	FLOAT FloorZ(FVector Location);  
	void TwoWallAdjust(FVector &DesiredDir, FVector &Delta, FVector &HitNormal, FVector &OldHitNormal, FLOAT HitTime);
	void physPathing(float DeltaTime);
	void physMovingBrush(float DeltaTime);

	//AI functions
	void CheckNoiseHearing(FLOAT Loudness);
	int CanBeHeardBy(AActor *Other);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
