/*===========================================================================
	C++ "Actor" class definitions exported from UnrealEd
===========================================================================*/
#pragma pack (push,4) /* 4-byte alignment */

///////////////////////////////////////////////////////
// Class AActor:UObject
///////////////////////////////////////////////////////

enum EButton {
    BUT_None                =0,
    BUT_Zoom                =1,
    BUT_Run                 =2,
    BUT_Look                =3,
    BUT_Duck                =4,
    BUT_Strafe              =5,
    BUT_Fire                =6,
    BUT_AltFire             =7,
    BUT_Jump                =8,
    BUT_Extra7              =9,
    BUT_Extra6              =10,
    BUT_Extra5              =11,
    BUT_Extra4              =12,
    BUT_Extra3              =13,
    BUT_Extra2              =14,
    BUT_Extra1              =15,
    BUT_Extra0              =16,
    BUT_MAX                 =17,
};

enum EAxis {
    AXIS_None               =0,
    AXIS_BaseX              =1,
    AXIS_BaseY              =2,
    AXIS_BaseZ              =3,
    AXIS_Forward            =4,
    AXIS_Turn               =5,
    AXIS_Strafe             =6,
    AXIS_Up                 =7,
    AXIS_LookUp             =8,
    AXIS_Extra6             =9,
    AXIS_Extra5             =10,
    AXIS_Extra4             =11,
    AXIS_Extra3             =12,
    AXIS_Extra2             =13,
    AXIS_Extra1             =14,
    AXIS_Extra0             =15,
    AXIS_MAX                =16,
};

enum EDrawType {
    DT_None                 =0,
    DT_Sprite               =1,
    DT_Mesh                 =2,
    DT_Brush                =3,
    DT_MAX                  =4,
};

enum EBlitType {
    BT_None                 =0,
    BT_Normal               =1,
    BT_Transparent          =2,
    BT_MAX                  =3,
};

enum EAmbientForceType {
    AFT_None                =0,
    AFT_Machinery           =1,
    AFT_Rumble              =2,
    AFT_Shake               =3,
    AFT_MAX                 =4,
};

enum ELightType {
    LT_None                 =0,
    LT_Steady               =1,
    LT_Pulse                =2,
    LT_Blink                =3,
    LT_Flicker              =4,
    LT_Strobe               =5,
    LT_BackdropLight        =6,
    LT_MAX                  =7,
};

enum ELightEffect {
    LE_None                 =0,
    LE_TorchWaver           =1,
    LE_FireWaver            =2,
    LE_WateryShimmer        =3,
    LE_Searchlight          =4,
    LE_SlowWave             =5,
    LE_FastWave             =6,
    LE_CloudCast            =7,
    LE_StaticSpot           =8,
    LE_Shock                =9,
    LE_Disco                =10,
    LE_Warp                 =11,
    LE_Spotlight            =12,
    LE_StolenQuakeWater     =13,
    LE_ChurningWater        =14,
    LE_Satellite            =15,
    LE_Interference         =16,
    LE_Cylinder             =17,
    LE_Rotor                =18,
    LE_Unused               =19,
    LE_MAX                  =20,
};

enum EPhysics {
    PHYS_None               =0,
    PHYS_Walking            =1,
    PHYS_Falling            =2,
    PHYS_Swimming           =3,
    PHYS_Flying             =4,
    PHYS_Rotating           =5,
    PHYS_Projectile         =6,
    PHYS_Pathing            =7,
    PHYS_Splining           =8,
    PHYS_MAX                =9,
};

class UNENGINE_API AActor : public UObject {
public:
    class AActor *Owner;
    class ALevelInfo *Level;
    class ULevel *XLevel;
    FName Tag;
    FName State;
    FName Event;
    FName Group;
    class AActor *Target;
    class APawn *Instigator;
    class AInventory *Inventory;
    class AActor *Floor;
    class AZoneInfo *Zone;
    INT LatentAction;
    BYTE StandingCount;
    BYTE ZoneNumber;
    BYTE MiscNumber;
    BYTE LatentByte;
    INT LatentInt;
    FLOAT LatentFloat;
    class AActor *LatentActor;
    class AActor *Touching[4];
    class AActor *Hash;
    class AActor *Deleted;
    INT CollisionTag;
    DWORD bStatic:1;
    DWORD bHidden:1;
    DWORD bHiddenEd:1;
    DWORD bDirectional:1;
    DWORD bSelected:1;
    DWORD bNoDelete:1;
    DWORD bAnimFinished:1;
    DWORD bAnimLoop:1;
    DWORD bAnimNotify:1;
    DWORD bTempEditor:1;
    DWORD bDeleteMe:1;
    DWORD bAssimilated:1;
    DWORD bTicked:1;
    DWORD bLightChanged:1;
    DWORD bDynamicLight:1;
    DWORD bTimerLoop:1;
    DWORD bCollideActors:1;
    DWORD bCollideWorld:1;
    DWORD bBlockActors:1;
    DWORD bBlockPlayers:1;
    DWORD bProjTarget:1;
    FVector Location;
    FVector OldLocation;
    FVector ColLocation;
    FVector Velocity;
    FRotation Rotation;
    FVector Acceleration;
    BYTE DrawType /* enum EDrawType */;
    BYTE BlitType /* enum EBlitType */;
    class UTexture *Texture;
    class UMesh *Mesh;
    class UModel *Brush;
    FLOAT DrawScale;
    BYTE AmbientGlow;
    BYTE Fatness;
    BYTE SoundRadius;
    BYTE SoundVolume;
    BYTE SoundPitch;
    class USound *AmbientSound;
    BYTE AmbientForceType /* enum EAmbientForceType */;
    FLOAT CollisionRadius;
    FLOAT CollisionHeight;
    BYTE LightType /* enum ELightType */;
    BYTE LightEffect /* enum ELightEffect */;
    BYTE LightBrightness;
    BYTE LightHue;
    BYTE LightSaturation;
    BYTE LightRadius;
    BYTE LightPeriod;
    BYTE LightPhase;
    BYTE LightCone;
    BYTE VolumeBrightness;
    BYTE VolumeRadius;
    DWORD bSpecialLit:1;
    DWORD bOnHorizon:1;
    DWORD bActorShadows:1;
    DWORD bUnlit:1;
    DWORD bNoSmooth:1;
    DWORD bOnlyOwnerSee:1;
    DWORD bMeshEnviroMap:1;
    DWORD bMeshCurvy:1;
    DWORD bShadowCast:1;
    DWORD bUser3:1;
    DWORD bUser2:1;
    DWORD bUser1:1;
    DWORD bUser0:1;
    BYTE Physics /* enum EPhysics */;
    FLOAT Mass;
    FLOAT Buoyancy;
    DWORD bMomentum:1;
    DWORD bBounce:1;
    DWORD bPitch:1;
    DWORD bYaw:1;
    DWORD bRoll:1;
    DWORD bEnginePhysics:1;
    DWORD bFixedRotationDir:1;
    DWORD bYawClockwise:1;
    DWORD bPitchClockwise:1;
    DWORD bRollClockwise:1;
    INT RotationSpeed;
    FRotation DesiredRotation;
    FLOAT LifeSpan;
    FName AnimSequence;
    FLOAT AnimFrame;
    FLOAT AnimRate;
    FLOAT AnimEnd;
    FLOAT AnimMinRate;
    DWORD bDifficulty1:1;
    DWORD bDifficulty2:1;
    DWORD bDifficulty3:1;
    DWORD bSinglePlayer:1;
    DWORD bNet:1;
    DWORD bNetSpecial:1;
    DWORD bCanTeleport:1;
    DWORD bIsSecretGoal:1;
    DWORD bIsKillGoal:1;
    DWORD bIsItemGoal:1;
    FLOAT TickRate;
    FLOAT TimerRate;
    FLOAT TickCounter;
    FLOAT TimerCounter;
    FName DefaultEdCategory;
    enum {BaseFlags = CLASS_Intrinsic  | CLASS_ScriptWritable};
    DECLARE_CLASS(AActor,UObject,NAME_Actor,NAME_UnEngine)
    #include "AActor.h"
};

///////////////////////////////////////////////////////
// Class APawn:AActor:UObject
///////////////////////////////////////////////////////

enum EAttitude {
    ATTITUDE_Fear           =0,
    ATTITUDE_Hate           =1,
    ATTITUDE_Frenzy         =2,
    ATTITUDE_Threaten       =3,
    ATTITUDE_Ignore         =4,
    ATTITUDE_Friendly       =5,
    ATTITUDE_Follow         =6,
    ATTITUDE_MAX            =7,
};

enum EIntelligence {
    BRAINS_NONE             =0,
    BRAINS_REPTILE          =1,
    BRAINS_MAMMAL           =2,
    BRAINS_HUMAN            =3,
    BRAINS_MAX              =4,
};

enum ESurfaceForceType {
    SFT_None                =0,
    SFT_CalmWater           =1,
    SFT_TurbulentWater      =2,
    SFT_MAX                 =3,
};

enum EPlayForceType {
    PFT_Full                =0,
    PFT_RampUp              =1,
    PFT_RampDown            =2,
    PFT_Smooth              =3,
    PFT_Decay               =4,
    PFT_Instant             =5,
    PFT_MAX                 =6,
};

class UNENGINE_API APawn : public AActor {
public:
    class UCamera *Camera;
    class UPlayer *Player;
    CHAR PlayerName[32];
    CHAR Team[32];
    class AStatusInfo *StatusBar;
    class AWeapon *Weapon;
    FRotation ViewRotation;
    INT MaxPitch;
    FLOAT BaseEyeHeight;
    FLOAT EyeHeight;
    FLOAT Bob;
    FLOAT OrthoZoom;
    FLOAT FovAngle;
    INT ShowFlags;
    INT RendMap;
    INT Misc1;
    INT Misc2;
    BYTE DieCount;
    BYTE ItemCount;
    BYTE KillCount;
    BYTE SecretCount;
    INT AmmoCount[10];
    INT AmmoCapacity[10];
    FLOAT NaturalArmor[4];
    FLOAT Armor[4];
    FLOAT HealRate;
    DWORD bCanPossess:1;
    DWORD bLocalMovement:1;
    DWORD bLocalRotation:1;
    DWORD bBehindView:1;
    DWORD bIsPlayer:1;
    DWORD bReadyToAttack:1;
    DWORD bHasRangedAttack:1;
    DWORD bMovingRangedAttack:1;
    DWORD bCanStrafe:1;
    FLOAT SightRadius;
    FLOAT PeripheralVision;
    FLOAT Noise;
    FLOAT HearingThreshold;
    BYTE Visibility;
    FName Orders;
    FName OrderTag;
    class AActor *OrderObject;
    FVector LastSeenPos;
    FLOAT TimeBetweenAttacks;
    FLOAT Alertness;
    FName NextAnim;
    FLOAT SightCounter;
    FLOAT Stimulus;
    class APawn *Enemy;
    FLOAT Aggressiveness;
    BYTE AttitudeToPlayer /* enum EAttitude */;
    class APawn *OldEnemy;
    class AActor *MoveTarget;
    FName NextState;
    FVector Destination;
    FVector Focus;
    FLOAT DesiredSpeed;
    FLOAT GroundSpeed;
    FLOAT WaterSpeed;
    FLOAT AirSpeed;
    FLOAT Health;
    FLOAT AccelRate;
    FLOAT JumpZ;
    FLOAT MaxStepHeight;
    INT InvisibilityTime;
    INT SuperShieldTime;
    INT SuperStrengthTime;
    DWORD bZoom:1;
    DWORD bRun:1;
    DWORD bLook:1;
    DWORD bDuck:1;
    DWORD bStrafe:1;
    DWORD bFire:1;
    DWORD bAltFire:1;
    DWORD bJump:1;
    DWORD bExtra3:1;
    DWORD bExtra2:1;
    DWORD bExtra1:1;
    DWORD bExtra0:1;
    FLOAT aForward;
    FLOAT aTurn;
    FLOAT aStrafe;
    FLOAT aUp;
    FLOAT aLookUp;
    FLOAT aExtra4;
    FLOAT aExtra3;
    FLOAT aExtra2;
    FLOAT aExtra1;
    FLOAT aExtra0;
    BYTE SurfaceForceType /* enum ESurfaceForceType */;
    BYTE SurfaceForcePeriod;
    BYTE SurfaceForceMagnitude;
    enum {BaseFlags = CLASS_Intrinsic  | CLASS_ScriptWritable};
    DECLARE_CLASS(APawn,AActor,NAME_Pawn,NAME_UnEngine)
    #include "APawn.h"
};

///////////////////////////////////////////////////////
// Class AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AScriptedPawn : public APawn {
public:
};

///////////////////////////////////////////////////////
// Class AHuman:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AHuman : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class ADragon:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ADragon : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class ASkaarj:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ASkaarj : public AScriptedPawn {
public:
    class USound *LungeSound;
    class USound *SpinSound;
    class USound *ClawSound;
    class USound *ShootSound;
    BYTE LungeDamage;
    BYTE SpinDamage;
    BYTE ClawDamage;
    BYTE ShootDamage;
};

///////////////////////////////////////////////////////
// Class ABrute:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ABrute : public AScriptedPawn {
public:
    class USound *ShootSound;
    class USound *WhipSound;
    BYTE ShootDamage;
    BYTE WhipDamage;
};

///////////////////////////////////////////////////////
// Class AArchAngel:ABrute:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AArchAngel : public ABrute {
public:
};

///////////////////////////////////////////////////////
// Class AGasbag:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AGasbag : public AScriptedPawn {
public:
    class USound *BelchSound;
    class USound *PunchSound;
    class USound *PoundSound;
    BYTE BelchDamage;
    BYTE PunchDamage;
    BYTE PoundDamage;
};

///////////////////////////////////////////////////////
// Class AManta:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AManta : public AScriptedPawn {
public:
    class USound *StingSound;
    class USound *WhipSound;
    BYTE StingDamage;
    BYTE WhipDamage;
};

///////////////////////////////////////////////////////
// Class ATentacle:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ATentacle : public AScriptedPawn {
public:
    class USound *ShootSound;
    class USound *MebaxSound;
    BYTE WhipDamange;
    class AProjectile *Projectile;
};

///////////////////////////////////////////////////////
// Class AScout:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AScout : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AView:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AView : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class ARazorFish:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ARazorFish : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AKrall:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AKrall : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class ATitan:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ATitan : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AMale:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AMale : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AFemale:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AFemale : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class ACow:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ACow : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class APupae:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class APupae : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AMercenary:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AMercenary : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AQueen:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AQueen : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class ASquid:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ASquid : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class ASlith:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ASlith : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AWarlord:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AWarlord : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class ANali:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ANali : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class ATurret:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ATurret : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AHawk:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AHawk : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AFly:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AFly : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class ABlob:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class ABlob : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AFirefly:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AFirefly : public AScriptedPawn {
public:
};

///////////////////////////////////////////////////////
// Class AFireflySwarm:AScriptedPawn:APawn:AActor:UObject
///////////////////////////////////////////////////////

class AFireflySwarm : public AScriptedPawn {
public:
    BYTE swarmsize;
};

///////////////////////////////////////////////////////
// Class ABrush:AActor:UObject
///////////////////////////////////////////////////////

class UNENGINE_API ABrush : public AActor {
public:
    enum {BaseFlags = CLASS_Intrinsic  | CLASS_ScriptWritable};
    DECLARE_CLASS(ABrush,AActor,NAME_Brush,NAME_UnEngine)
    #include "ABrush.h"
};

///////////////////////////////////////////////////////
// Class AMover:ABrush:AActor:UObject
///////////////////////////////////////////////////////

enum EMoverBumpType {
    MB_StopWhenBump         =0,
    MB_ReturnWhenBump       =1,
    MB_CrushWhenBump        =2,
    MB_MAX                  =3,
};

enum EMoverTriggerType {
    MT_None                 =0,
    MT_TriggerOpenTimed     =1,
    MT_TriggerToggle        =2,
    MT_TriggerControl       =3,
    MT_TriggerCycleOn       =4,
    MT_TriggerCycleOff      =5,
    MT_TriggerInstant       =6,
    MT_ProximityOpenTimed   =7,
    MT_ProximityControl     =8,
    MT_StandOpenTimed       =9,
    MT_MAX                  =10,
};

enum EMoverGlideType {
    MV_MoveByTime           =0,
    MV_GlideByTime          =1,
    MV_Sinusoid             =2,
    MV_MAX                  =3,
};

class UNGAME_API AMover : public ABrush {
public:
    BYTE MoverBumpType /* enum EMoverBumpType */;
    BYTE MoverTriggerType /* enum EMoverTriggerType */;
    BYTE MoverGlideType /* enum EMoverGlideType */;
    BYTE PrevKeyNum;
    BYTE KeyNum;
    BYTE WorldRaytraceKey;
    BYTE BrushRaytraceKey;
    FLOAT MoveTime;
    FLOAT StayOpenTime;
    FLOAT BumpPlayerDamage;
    FRotation FreeRotation;
    DWORD bInterruptable:1;
    DWORD bSlave:1;
    DWORD bTrigger:1;
    DWORD bMoving:1;
    DWORD bReverseWhenDone:1;
    DWORD bTriggerOnceOnly:1;
    class USound *OpenSound;
    class USound *ClosedSound;
    class USound *MoveAmbientSound;
    FVector KeyPos[4];
    FRotation KeyRot[4];
    class AActor *Slaves[16];
    FVector BasePos;
    FVector OldPos;
    FRotation BaseRot;
    FRotation OldRot;
    FLOAT CurTime;
    FLOAT HoldTime;
    enum {BaseFlags = CLASS_Intrinsic  | CLASS_ScriptWritable};
    DECLARE_CLASS(AMover,ABrush,NAME_Mover,NAME_UnGame)
    #include "AMover.h"
};

///////////////////////////////////////////////////////
// Class AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AInventory : public AActor {
public:
    BYTE AutoSwitchPriority;
    BYTE InventoryGroup;
    DWORD bActivatable:1;
    DWORD bRepeats:1;
    class USound *PickupSound;
    class USound *RespawnSound;
    CHAR PickupMessage[64];
    FLOAT RespawnTime;
    DWORD bRespawnAlone:1;
    DWORD bRespawnNet:1;
    FVector PlayerViewOffset;
    class UMesh *PlayerViewMesh;
    FLOAT PlayerViewScale;
    class UMesh *PickupViewMesh;
    FLOAT PickupViewScale;
    class UTexture *StatusIcon;
    INT StatusCount;
    FLOAT StatusValue;
};

///////////////////////////////////////////////////////
// Class AWeapon:AInventory:AActor:UObject
///////////////////////////////////////////////////////

enum EWeaponType {
    WEAP_Projectile         =0,
    WEAP_InstantHit         =1,
    WEAP_Custom             =2,
    WEAP_MAX                =3,
};

class AWeapon : public AInventory {
public:
    DWORD bAutoVTarget:1;
    DWORD bAutoHTarget:1;
    FLOAT MaxTargetRange;
    FLOAT SeekDamping;
    BYTE AmmoType;
    INT AmmoUsed[2];
    BYTE Discharges[2];
    BYTE ReloadCount;
    FLOAT Noise[2];
    FLOAT RecoilForce[2];
    class UClass *MuzzleFxClass[2];
    INT PickupAmmoCount;
    class USound *DischargeSounds[2];
    class USound *ReloadSound;
    class USound *CloseUpSound;
    BYTE CloseUpDamage;
    FLOAT CloseUpStrength;
    FLOAT UseTime;
    FLOAT LastUseTime;
    class UClass *ProjectileClass[2];
    FLOAT ProjStartDist;
    FLOAT Dispersion[2];
};

///////////////////////////////////////////////////////
// Class AAutoMag:AWeapon:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AAutoMag : public AWeapon {
public:
};

///////////////////////////////////////////////////////
// Class AQuadShot:AWeapon:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AQuadShot : public AWeapon {
public:
};

///////////////////////////////////////////////////////
// Class AFlameGun:AWeapon:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AFlameGun : public AWeapon {
public:
};

///////////////////////////////////////////////////////
// Class AStinger:AWeapon:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AStinger : public AWeapon {
public:
};

///////////////////////////////////////////////////////
// Class Arazor:AWeapon:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class Arazor : public AWeapon {
public:
};

///////////////////////////////////////////////////////
// Class Aeightbal:AWeapon:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class Aeightbal : public AWeapon {
public:
};

///////////////////////////////////////////////////////
// Class Aflamet:AWeapon:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class Aflamet : public AWeapon {
public:
};

///////////////////////////////////////////////////////
// Class Arifle:AWeapon:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class Arifle : public AWeapon {
public:
};

///////////////////////////////////////////////////////
// Class APickup:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class APickup : public AInventory {
public:
};

///////////////////////////////////////////////////////
// Class APowerUp:APickup:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class APowerUp : public APickup {
public:
    FLOAT Strength;
    FLOAT Stamina;
    FLOAT Health;
    FLOAT Armor[4];
    INT TimeLimit;
    DWORD bInvisibility:1;
    DWORD bSilence:1;
    DWORD bInvincibility:1;
    DWORD bSuperStrength:1;
    DWORD bSuperStamina:1;
};

///////////////////////////////////////////////////////
// Class AHealth:APowerUp:APickup:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AHealth : public APowerUp {
public:
};

///////////////////////////////////////////////////////
// Class AArmor:APowerUp:APickup:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AArmor : public APowerUp {
public:
};

///////////////////////////////////////////////////////
// Class Aheart:APowerUp:APickup:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class Aheart : public APowerUp {
public:
};

///////////////////////////////////////////////////////
// Class AAmmo:APickup:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AAmmo : public APickup {
public:
};

///////////////////////////////////////////////////////
// Class AClip:AAmmo:APickup:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AClip : public AAmmo {
public:
};

///////////////////////////////////////////////////////
// Class AShells:AAmmo:APickup:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AShells : public AAmmo {
public:
};

///////////////////////////////////////////////////////
// Class AStingerAmmo:AAmmo:APickup:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AStingerAmmo : public AAmmo {
public:
};

///////////////////////////////////////////////////////
// Class ADetector:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class ADetector : public AInventory {
public:
};

///////////////////////////////////////////////////////
// Class AFlashlight:AInventory:AActor:UObject
///////////////////////////////////////////////////////

class AFlashlight : public AInventory {
public:
};

///////////////////////////////////////////////////////
// Class AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class AProjectile : public AActor {
public:
    DWORD bVerticalSeek:1;
    DWORD bFollowFloor:1;
    DWORD bIsInstantHit:1;
    class UClass *EffectAtLifeSpan;
    class UClass *EffectOnWallImpact;
    class UClass *EffectOnPawnImpact;
    class UClass *EffectOnImpact;
    FLOAT Speed;
    FLOAT MaxSpeed;
    FLOAT BounceIncidence;
    FLOAT FollowFloorHeight;
    BYTE MaxBounceCount;
    BYTE BounceCount;
    FLOAT Damage[4];
    FLOAT DamageDecay[4];
    BYTE ExplosiveTransfer;
};

///////////////////////////////////////////////////////
// Class ATentacleProjectile:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class ATentacleProjectile : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class AEightBall:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class AEightBall : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class AFireball:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class AFireball : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class AFireball2:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class AFireball2 : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class ABulletProjectile:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class ABulletProjectile : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class AShellProjectile:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class AShellProjectile : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class AStingerProjectile:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class AStingerProjectile : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class ABruteProjectile:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class ABruteProjectile : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class ASkaarjProjectile:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class ASkaarjProjectile : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class Abeam:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class Abeam : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class Arocket:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class Arocket : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class ABigRock:AProjectile:AActor:UObject
///////////////////////////////////////////////////////

class ABigRock : public AProjectile {
public:
};

///////////////////////////////////////////////////////
// Class AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class AKeypoint : public AActor {
public:
};

///////////////////////////////////////////////////////
// Class APlayerStart:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class APlayerStart : public AKeypoint {
public:
    class UClass *PlayerSpawnClass;
    BYTE TeamNumber;
};

///////////////////////////////////////////////////////
// Class ACreaturepoint:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class ACreaturepoint : public AKeypoint {
public:
    FName ownerClan;
};

///////////////////////////////////////////////////////
// Class APatrolpoint:ACreaturepoint:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class APatrolpoint : public ACreaturepoint {
public:
    FName Nextpatrol;
    FLOAT pausetime;
    FVector lookdir;
};

///////////////////////////////////////////////////////
// Class AAmbushpoint:ACreaturepoint:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class AAmbushpoint : public ACreaturepoint {
public:
    FVector lookdir;
    DWORD taken:1;
    BYTE survivecount;
};

///////////////////////////////////////////////////////
// Class AHomeBase:ACreaturepoint:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class AHomeBase : public ACreaturepoint {
public:
};

///////////////////////////////////////////////////////
// Class ASpawnpoint:ACreaturepoint:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class ASpawnpoint : public ACreaturepoint {
public:
};

///////////////////////////////////////////////////////
// Class ABlockMonsters:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class ABlockMonsters : public AKeypoint {
public:
};

///////////////////////////////////////////////////////
// Class ABlockAll:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class ABlockAll : public AKeypoint {
public:
};

///////////////////////////////////////////////////////
// Class AAmbientSound:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class AAmbientSound : public AKeypoint {
public:
};

///////////////////////////////////////////////////////
// Class APathNode:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class APathNode : public AKeypoint {
public:
    INT upstreamPaths[16];
    INT Paths[16];
    FVector turnDirection;
    FLOAT visitedWeight;
    FLOAT cost;
    DWORD bEndPoint:1;
    DWORD bSpecialBlock:1;
};

///////////////////////////////////////////////////////
// Class ATextMessage:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class ATextMessage : public AKeypoint {
public:
};

///////////////////////////////////////////////////////
// Class ACreatureFactory:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class ACreatureFactory : public AKeypoint {
public:
    class UClass *prototype;
    BYTE maxpawns;
    BYTE backlog;
    BYTE numspots;
    INT capacity;
    FLOAT interval;
    DWORD bShowSpawns:1;
    class ASpawnpoint *spawnspot[5];
};

///////////////////////////////////////////////////////
// Class ATeleporter:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class ATeleporter : public AKeypoint {
public:
    CHAR TeleportURL[64];
    FName ProductRequired;
    DWORD bChangesVelocity:1;
    DWORD bChangesYaw:1;
    DWORD bReversesX:1;
    DWORD bReversesY:1;
    DWORD bReversesZ:1;
    INT TargetYaw;
    FVector TargetVelocity;
};

///////////////////////////////////////////////////////
// Class AWayBeacon:AKeypoint:AActor:UObject
///////////////////////////////////////////////////////

class AWayBeacon : public AKeypoint {
public:
};

///////////////////////////////////////////////////////
// Class AInfo:AActor:UObject
///////////////////////////////////////////////////////

class AInfo : public AActor {
public:
};

///////////////////////////////////////////////////////
// Class AStatusInfo:AInfo:AActor:UObject
///////////////////////////////////////////////////////

class AStatusInfo : public AInfo {
public:
    INT ScaleHeight;
    INT ScaleWidth;
    class UTexture *Textures[16];
    FLOAT TexStartX[16];
    FLOAT TexEndX[16];
    FLOAT TexStartY[16];
    FLOAT TexEndY[16];
    class UFont *Fonts[12];
    CHAR FontText[24];
    FLOAT FontStartX[12];
    FLOAT FontEndX[12];
    FLOAT FontStartY[12];
    FLOAT FontEndY[12];
    DWORD bDirty:1;
    class UTexture *Surface;
};

///////////////////////////////////////////////////////
// Class AUnrealStatusInfo:AStatusInfo:AInfo:AActor:UObject
///////////////////////////////////////////////////////

class AUnrealStatusInfo : public AStatusInfo {
public:
};

///////////////////////////////////////////////////////
// Class AZoneInfo:AInfo:AActor:UObject
///////////////////////////////////////////////////////

class AZoneInfo : public AInfo {
public:
    CHAR Title[64];
    DWORD bWaterZone:1;
    DWORD bFogZone:1;
    DWORD bKillZone:1;
    DWORD bEchoZone:1;
    DWORD bNeutralZone:1;
    DWORD bGravityZone:1;
    DWORD bTeamOnly:1;
    FVector ZoneGravity;
    FVector ZoneVelocity;
    FLOAT ZoneGroundFriction;
    FLOAT ZoneFluidFriction;
    FLOAT AuralSpace;
    FLOAT AuralDepth;
    BYTE AmbientBrightness;
    BYTE AmbientHue;
    BYTE AmbientSaturation;
    BYTE RampHue;
    BYTE RampSaturation;
    BYTE FogThickness;
    BYTE FogHue;
    BYTE FogSaturation;
    class UMusic *Song;
};

///////////////////////////////////////////////////////
// Class ALevelInfo:AZoneInfo:AInfo:AActor:UObject
///////////////////////////////////////////////////////

enum ELevelState {
    LEVEL_Down              =0,
    LEVEL_UpPlay            =1,
    LEVEL_UpEdit            =2,
    LEVEL_UpDemo            =3,
    LEVEL_MAX               =4,
};

enum ESkyTime {
    SKY_Day                 =0,
    SKY_Dusk                =1,
    SKY_Night               =2,
    SKY_Dawn                =3,
    SKY_MAX                 =4,
};

class ALevelInfo : public AZoneInfo {
public:
    BYTE LevelState /* enum ELevelState */;
    FLOAT DayFactor;
    FLOAT DayBase;
    FLOAT TimeDays;
    FLOAT TimeSeconds;
    FLOAT DayFraction;
    FLOAT NightFraction;
    INT Year;
    INT Month;
    INT Day;
    INT Hour;
    INT Minute;
    INT Second;
    INT Millisecond;
    CHAR LevelAuthor[64];
    CHAR LevelEnterText[64];
    DWORD bLonePlayer:1;
    DWORD bMirrorSky:1;
    FLOAT TexUPanSpeed;
    FLOAT TexVPanSpeed;
    FLOAT SkyScale;
    FLOAT SkyFlicker;
    FLOAT SkyDayBright;
    FLOAT SkyNightBright;
    FLOAT SkyUPanSpeed;
    FLOAT SkyVPanSpeed;
    FLOAT SkyWavyness;
    FLOAT SkyWavySpeed;
    DWORD bNoSmoothSky:1;
    class UTexture *SkyTexture;
    class UTexture *CloudcastTexture;
    BYTE SkyFogBrightness;
    BYTE SkyFogHue;
    BYTE SkyFogSaturation;
    INT RandomSeed;
    INT RandomValue;
    INT ItemGoals;
    INT KillGoals;
    INT SecretGoals;
    BYTE Difficulty;
    BYTE NetMode;
    DWORD bNoMonsters:1;
    DWORD bInternet:1;
    DWORD bRestartLevel:1;
    CHAR JumpToLevel[80];
    FName Dependencies[16];
};

///////////////////////////////////////////////////////
// Class APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class APyrotechnics : public AActor {
public:
    FLOAT GravityMult;
    FLOAT AccelerationFactor;
    class USound *InitialSound;
};

///////////////////////////////////////////////////////
// Class APawnHit:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class APawnHit : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class AExplode1:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class AExplode1 : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class APlayerRespawn:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class APlayerRespawn : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class ATeleportIn:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class ATeleportIn : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class ATeleportOut:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class ATeleportOut : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class AWallHit:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class AWallHit : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class ABruteGunFlash:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class ABruteGunFlash : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class ASkaarjGunFlash:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class ASkaarjGunFlash : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class AGasBagBelchFlash:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class AGasBagBelchFlash : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class AExplode2:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class AExplode2 : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class AExplode3:APyrotechnics:AActor:UObject
///////////////////////////////////////////////////////

class AExplode3 : public APyrotechnics {
public:
};

///////////////////////////////////////////////////////
// Class ALight:AActor:UObject
///////////////////////////////////////////////////////

class ALight : public AActor {
public:
};

///////////////////////////////////////////////////////
// Class ASatellite:ALight:AActor:UObject
///////////////////////////////////////////////////////

class ASatellite : public ALight {
public:
    FLOAT Period;
    FLOAT Phase;
    FVector SatX;
    FVector SatY;
    FVector SatZ;
};

///////////////////////////////////////////////////////
// Class ATorchFlame:ALight:AActor:UObject
///////////////////////////////////////////////////////

class ATorchFlame : public ALight {
public:
};

///////////////////////////////////////////////////////
// Class ASpotlight:ALight:AActor:UObject
///////////////////////////////////////////////////////

class ASpotlight : public ALight {
public:
};

///////////////////////////////////////////////////////
// Class ATriggers:AActor:UObject
///////////////////////////////////////////////////////

class ATriggers : public AActor {
public:
};

///////////////////////////////////////////////////////
// Class ATrigger:ATriggers:AActor:UObject
///////////////////////////////////////////////////////

enum ETriggerType {
    TT_PlayerProximity      =0,
    TT_AnyProximity         =1,
    TT_Shoot                =2,
    TT_MAX                  =3,
};

class ATrigger : public ATriggers {
public:
    BYTE TriggerType /* enum ETriggerType */;
    CHAR Message[80];
    DWORD bTriggerOnceOnly:1;
};

///////////////////////////////////////////////////////
// Class ACounter:ATriggers:AActor:UObject
///////////////////////////////////////////////////////

class ACounter : public ATriggers {
public:
    BYTE NumToCount;
    DWORD bShowMessage:1;
    CHAR CountMessage[80];
    CHAR CompleteMessage[80];
    BYTE OriginalNum;
};

///////////////////////////////////////////////////////
// Class ADispatcher:ATriggers:AActor:UObject
///////////////////////////////////////////////////////

class ADispatcher : public ATriggers {
public:
    FName OutEvents[8];
    FLOAT OutDelays[8];
    INT i;
    class AActor *Other;
};

///////////////////////////////////////////////////////
// Class ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class ADecoration : public AActor {
public:
    class UClass *EffectWhenDestroyed;
};

///////////////////////////////////////////////////////
// Class AVase:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class AVase : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class AChandelier:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class AChandelier : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class AHammok:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class AHammok : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Aurn:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Aurn : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Alamp1:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Alamp1 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Alamp4:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Alamp4 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Aplant1:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Aplant1 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Aplant2:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Aplant2 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Aplant3:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Aplant3 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Asign1:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Asign1 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Aflag1:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Aflag1 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Aflag2:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Aflag2 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Aflag3:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Aflag3 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Acandle:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Acandle : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Acandle2:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Acandle2 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Afan1:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Afan1 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class Afan2:ADecoration:AActor:UObject
///////////////////////////////////////////////////////

class Afan2 : public ADecoration {
public:
};

///////////////////////////////////////////////////////
// Class AExplosion:AActor:UObject
///////////////////////////////////////////////////////

class AExplosion : public AActor {
public:
};

///////////////////////////////////////////////////////
// Class AClipExplosion:AExplosion:AActor:UObject
///////////////////////////////////////////////////////

class AClipExplosion : public AExplosion {
public:
};

///////////////////////////////////////////////////////
// Class AShellExplosion:AExplosion:AActor:UObject
///////////////////////////////////////////////////////

class AShellExplosion : public AExplosion {
public:
};

///////////////////////////////////////////////////////
// Class ATarydiumExplosion:AExplosion:AActor:UObject
///////////////////////////////////////////////////////

class ATarydiumExplosion : public AExplosion {
public:
};

#pragma pack (pop) /* Restore alignment to previous setting */
