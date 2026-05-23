/*===========================================================================
	C++ "Actor" actor class definitions exported from UnrealEd
===========================================================================*/
#pragma pack (push,4) /* 4-byte alignment */

///////////////////////////////////////////////////////
// Actor class AActor
///////////////////////////////////////////////////////

enum EDrawType {
    DT_None                 =0,
    DT_Sprite               =1,
    DT_MeshMap              =2,
    DT_Brush                =3,
    DT_MAX                  =4,
};

enum EBlitType {
    BT_None                 =0,
    BT_Normal               =1,
    BT_Transparent          =2,
    BT_MAX                  =3,
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

enum EAI_Task {
    EAI_TaskNone            =0,
    EAI_TaskMove            =1,
    EAI_TaskSearch          =2,
    EAI_TaskWait            =3,
    EAI_TaskAttack          =4,
    EAI_MAX                 =5,
};

class UNREAL_API AActorBase {
public:
    UClass     *Class;
    FVector    Location;
    FVector    Velocity;
    FRotation  DrawRot;
    FRotation  ViewRot;
    INDEX      iParent;
    INDEX      iWeapon;
    BYTE       DrawType                 /* enum EDrawType */;
    BYTE       BlitType                 /* enum EBlitType */;
    UTexture   *Texture;
    UMeshMap   *MeshMap;
    UModel     *Brush;
    FLOAT      DrawScale;
    BYTE       InherentGlow;
    USound     *AmbientSound;
    BYTE       SoundRadius;
    BYTE       SoundVolume;
    BYTE       AnimSeq;
    FLOAT      AnimRate;
    FLOAT      AnimBase;
    BYTE       AnimCount;
    BYTE       AnimFirst;
    BYTE       AnimLast;
    INT        AnimMessage;
    FLOAT      CollisionRadius;
    FLOAT      CollisionHeight;
    BYTE       LightType                /* enum ELightType */;
    BYTE       LightEffect              /* enum ELightEffect */;
    BYTE       LightBrightness;
    BYTE       LightHue;
    BYTE       LightSaturation;
    BYTE       LightRadius;
    BYTE       LightPeriod;
    BYTE       LightPhase;
    BYTE       LightCone;
    BYTE       VolumeBrightness;
    BYTE       VolumeRadius;
    DWORD      bStatic:1;
    DWORD      bHidden:1;
    DWORD      bHiddenEd:1;
    DWORD      bDirectional:1;
    DWORD      bTemplateClass:1;
    DWORD      bSelected:1;
    DWORD      bNoDelete:1;
    DWORD      bTempLightChanged:1;
    DWORD      bTempDynamicLight:1;
    DWORD      bUser0:1;
    DWORD      bUser1:1;
    DWORD      bUser2:1;
    DWORD      bUser3:1;
    DWORD      bUser4:1;
    DWORD      bUser5:1;
    DWORD      bUser6:1;
    DWORD      bUser7:1;
    DWORD      bSpecialLit:1;
    DWORD      bOnHorizon:1;
    DWORD      bActorShadows:1;
    DWORD      bAnimate:1;
    DWORD      bMeshWet:1;
    DWORD      bMeshShadowCast:1;
    DWORD      bMeshEnviroMap:1;
    DWORD      bUnlit:1;
    DWORD      bNoSmooth:1;
    DWORD      bCollideActors:1;
    DWORD      bCollideWorld:1;
    DWORD      bBlocksActors:1;
    DWORD      bBlocksPlayers:1;
    DWORD      bBehindView:1;
    DWORD      bPad0:1;
    DWORD      bPad1:1;
    DWORD      bPad2:1;
    DWORD      bPad3:1;
    DWORD      bPad4:1;
    DWORD      bPad5:1;
    DWORD      bPad6:1;
    DWORD      bPad7:1;
    FName      Name;
    FName      Event;
    BYTE       Zone;
    INDEX      iMe;
    INDEX      iTarget;
    INDEX      iInventory;
    INDEX      iFloor;
    INDEX      iInstigator;
    FLOAT      Mass;
    FLOAT      WaterSinkRate;
    DWORD      bGravity:1;
    DWORD      bMomentum:1;
    DWORD      bDifficulty1:1;
    DWORD      bDifficulty2:1;
    DWORD      bDifficulty3:1;
    DWORD      bDifficulty4:1;
    DWORD      bNetCooperative:1;
    DWORD      bNetDeathMatch:1;
    DWORD      bNetPersistent:1;
    DWORD      bNetNoMonsters:1;
    DWORD      bProjTarget:1;
    DWORD      bCanBeTeleported:1;
    DWORD      bIsSecretGoal:1;
    DWORD      bIsKillGoal:1;
    DWORD      bIsItemGoal:1;
    INDEX      iTouchingActors[4];
    DWORD      bWasTouchedByPlayer:1;
    DWORD      bSleeping:1;
    DWORD      bTempEditor:1;
    DWORD      bJustDeleted:1;
    BYTE       ScriptCountdown;
    BYTE       ScriptTickRate;
    INT        TimerCountdown;
    FName      TimerMessage;
    FName      DefaultEdCategory;
    UClass     *BlockedClasses[3];
    UClass     *PassedClasses[3];
    BYTE       ChanceOfExistence;
    BYTE       RandomizerDivider;
    BYTE       RandomizerRemainder;
    INT        LifeSpan;
    INT        Age;
    INT        Era;
    BYTE       TriggerSequences[10];
    BYTE       TriggerFrames[10];
    BYTE       TriggerValues[10];
    BYTE       WhichTriggers;
    INT        TextureList;
    BYTE       TextureCount;
    BYTE       AITask                   /* enum EAI_Task */;
    INDEX      iPendingTeleporter;
    BYTE       TeleportDelay;
    #include "UnRoot.h"
};
class IMPLEMENTATION_API AActor : public AActorBase {
public:
    BYTE PropertyPad[643];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AActor);

///////////////////////////////////////////////////////
// Actor class AProjectile:AActor
///////////////////////////////////////////////////////

enum EDamageType {
    DMT_Basic               =0,
    DMT_Water               =1,
    DMT_Fire                =2,
    DMT_Electric            =3,
    DMT_Count               =4,
    DMT_None                =5,
    DMT_MAX                 =6,
};

class IMPLEMENTATION_API AProjectileBase : public AActorBase {
public:
    DWORD      bVerticalSeek:1;
    DWORD      bFollowFloor:1;
    DWORD      bBounce:1;
    DWORD      bIsInstantHit:1;
    UClass     *EffectAtLifeSpan;
    UClass     *EffectOnWallImpact;
    UClass     *EffectOnPawnImpact;
    UClass     *EffectOnImpact;
    FLOAT      Speed;
    FLOAT      Acceleration;
    FLOAT      MaxSpeed;
    BYTE       MaxBounceCount;
    BYTE       BounceCount;
    FLOAT      BounceIncidence;
    FLOAT      FollowFloorHeight;
    UTexture   *Textures[20];
    FLOAT      Damage[4];
    FLOAT      DamageDecay[4];
    BYTE       ExplosiveTransfer;
    BYTE       Hack                     /* enum EDamageType */;
};
class IMPLEMENTATION_API AProjectile : public AProjectileBase {
public:
    BYTE PropertyPad[482];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AProjectile);

///////////////////////////////////////////////////////
// Actor class AFireball:AProjectile:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AFireballBase : public AProjectileBase {
public:
};
class IMPLEMENTATION_API AFireball : public AFireballBase {
public:
    BYTE PropertyPad[482];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AFireball2:AProjectile:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AFireball2Base : public AProjectileBase {
public:
};
class IMPLEMENTATION_API AFireball2 : public AFireball2Base {
public:
    BYTE PropertyPad[482];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ABulletProjectile:AProjectile:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ABulletProjectileBase : public AProjectileBase {
public:
};
class IMPLEMENTATION_API ABulletProjectile : public ABulletProjectileBase {
public:
    BYTE PropertyPad[482];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AShellProjectile:AProjectile:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AShellProjectileBase : public AProjectileBase {
public:
};
class IMPLEMENTATION_API AShellProjectile : public AShellProjectileBase {
public:
    BYTE PropertyPad[482];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AStingerProjectile:AProjectile:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AStingerProjectileBase : public AProjectileBase {
public:
};
class IMPLEMENTATION_API AStingerProjectile : public AStingerProjectileBase {
public:
    BYTE PropertyPad[482];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ATentacleProjectile:AProjectile:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ATentacleProjectileBase : public AProjectileBase {
public:
};
class IMPLEMENTATION_API ATentacleProjectile : public ATentacleProjectileBase {
public:
    BYTE PropertyPad[482];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class APawn:AActor
///////////////////////////////////////////////////////

enum EAI_Move {
    EAIM_MoveNone           =0,
    EAIM_MoveApproach       =1,
    EAIM_MoveBackOff        =2,
    EAIM_MoveRunAway        =3,
    EAIM_MAX                =4,
};

enum ELifeState {
    LS_None                 =0,
    LS_Alive                =1,
    LS_Dying                =2,
    LS_Dead                 =3,
    LS_MAX                  =4,
};

class IMPLEMENTATION_API APawnBase : public AActorBase {
public:
    UCamera    *Camera;
    UPlayer    *Player;
    CHAR       PlayerName[32];
    CHAR       TeamName[32];
    FRotation  RotSpeed;
    FRotation  TargetRot;
    FLOAT      BaseEyeHeight;
    FLOAT      EyeHeight;
    FLOAT      Bob;
    FLOAT      OrthoZoom;
    FLOAT      FovAngle;
    INT        ShowFlags;
    INT        RendMap;
    INT        Misc1;
    INT        Misc2;
    DWORD      bNone:1;
    DWORD      bJump:1;
    DWORD      bFire:1;
    DWORD      bAltFire:1;
    DWORD      bZoom:1;
    DWORD      bRun:1;
    DWORD      bLookAround:1;
    DWORD      bDuck:1;
    DWORD      bNextWeapon:1;
    DWORD      bPrevWeapon:1;
    DWORD      bWeapon1:1;
    DWORD      bWeapon2:1;
    DWORD      bWeapon3:1;
    DWORD      bWeapon4:1;
    DWORD      bWeapon5:1;
    DWORD      bWeapon6:1;
    DWORD      bWeapon7:1;
    DWORD      bWeapon8:1;
    DWORD      bWeapon9:1;
    DWORD      bSlide:1;
    DWORD      bConsole:1;
    DWORD      bHalfConsole:1;
    DWORD      bExtra9:1;
    DWORD      bExtra8:1;
    DWORD      bExtra7:1;
    DWORD      bExtra6:1;
    DWORD      bExtra5:1;
    DWORD      bExtra4:1;
    DWORD      bExtra3:1;
    DWORD      bExtra2:1;
    DWORD      bExtra1:1;
    DWORD      bExtra0:1;
    FLOAT      aNone;
    FLOAT      aForward;
    FLOAT      aTurn;
    FLOAT      aSlide;
    FLOAT      aUp;
    FLOAT      aLookUp;
    FLOAT      aLookSide;
    FLOAT      aBaseX;
    FLOAT      aBaseY;
    FLOAT      aExtra6;
    FLOAT      aExtra5;
    FLOAT      aExtra4;
    FLOAT      aExtra3;
    FLOAT      aExtra2;
    FLOAT      aExtra1;
    FLOAT      aExtra0;
    BYTE       DeathCount;
    INT        ItemsFound;
    INT        KillCount;
    INT        SecretsFound;
    BYTE       ApproachPeriod;
    BYTE       ApproachRandomization;
    BYTE       AttackPeriod;
    BYTE       AttackRandomization;
    BYTE       BackOffPeriod;
    BYTE       BackOffRandomization;
    BYTE       BackOffThreshold;
    BYTE       TargetCloseness;
    BYTE       TargetZCloseness;
    BYTE       ApproachDirectness;
    BYTE       LurkingDistance;
    FLOAT      AuralAcuity;
    BYTE       RunAwayHealthThreshold;
    BYTE       AIMove                   /* enum EAI_Move */;
    BYTE       AIPreviousTask           /* enum EAI_Task */;
    BYTE       AIPreviousMove           /* enum EAI_Move */;
    INT        AmmoCount[10];
    USound     *MinorInjurySounds[3];
    USound     *MajorInjurySounds[3];
    USound     *DeathSounds[2];
    USound     *QuestingSounds[2];
    USound     *StillSounds[2];
    USound     *VictorySounds[2];
    USound     *AttackSounds[4];
    BYTE       SoundTimer;
    UClass     *AttackEffects[2];
    UClass     *ExplosionEffect;
    FLOAT      NaturalArmor[4];
    FLOAT      HealRate;
    FLOAT      Armor[4];
    FLOAT      AirDamping;
    FLOAT      JumpDamping;
    FLOAT      WaterDamping;
    FLOAT      GroundDamping;
    FLOAT      SightRadius;
    FLOAT      PeripheralVision;
    BYTE       HearingAccuracy;
    BYTE       Visibility;
    FLOAT      Noise;
    FLOAT      LungeSpeed;
    FLOAT      GroundSpeed;
    FLOAT      WaterSpeed;
    FLOAT      AirSpeed;
    FLOAT      MaxStrength;
    FLOAT      Strength;
    FLOAT      Stamina;
    FLOAT      Health;
    FLOAT      DesiredSpeed;
    BYTE       LifeState                /* enum ELifeState */;
    INDEX      iKiller;
    UClass     *DeathSpawn;
    BYTE       HitDisplayTimer;
    BYTE       ExplorationTimer;
    INT        TargetLostTime;
    FVector    TargetLastLocation;
    FVector    TargetLastVelocity;
    FLOAT      LastNonFallZ;
    INT        InvisibilityTimeLeft;
    INT        SilenceTimeLeft;
    INT        InvincibilityTimeLeft;
    INT        SuperStaminaTimeLeft;
    INT        SuperStrengthTimeLeft;
    FLOAT      ExplosiveCharge;
    FLOAT      DelayedDamage;
    BYTE       DamageDelay;
    BYTE       MaxStepUpHeight;
    BYTE       MaxStepDownHeight;
    BYTE       LookTimer;
    INT        AmmoCapacity[10];
    INDEX      iRecentWeapons[2];
    DWORD      bCanPossess:1;
    DWORD      bHasAI:1;
    DWORD      bSensesTargets:1;
    DWORD      bRespondsToNoise:1;
    DWORD      bRespondsToSights:1;
    DWORD      bHarmsMonsters:1;
    DWORD      bNeverStill:1;
    DWORD      bIsXenophobic:1;
    DWORD      bHasDistantMovingAttack:1;
    DWORD      bHasDistantStillAttack:1;
    DWORD      bHasCloseUpAttack:1;
    DWORD      bHasInvisibility:1;
    DWORD      bHasSilence:1;
    DWORD      bHasInvincibility:1;
    DWORD      bHasSuperStrength:1;
    DWORD      bHasSuperStamina:1;
    DWORD      bIsQuiescent:1;
    DWORD      bTargetIsNear:1;
    DWORD      bTargetWasHere:1;
    DWORD      bTargetWasNear:1;
    DWORD      bTargetWasLost:1;
    DWORD      bLimitRotation:1;
    DWORD      bLookingAlongStair:1;
    DWORD      bIsOnSurface:1;
    DWORD      bCheated:1;
    DWORD      bStatusChanged:1;
    DWORD      bIsAlarmed:1;
    DWORD      bIsSwimming:1;
    DWORD      bCannotTurn:1;
    DWORD      bCannotMove:1;
};
class IMPLEMENTATION_API APawn : public APawnBase {
public:
    BYTE PropertyPad[32];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(APawn);

///////////////////////////////////////////////////////
// Actor class ACamera:APawn:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ACameraBase : public APawnBase {
public:
};
class IMPLEMENTATION_API ACamera : public ACameraBase {
public:
    BYTE PropertyPad[32];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ACamera);

///////////////////////////////////////////////////////
// Actor class AHuman:APawn:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AHumanBase : public APawnBase {
public:
};
class IMPLEMENTATION_API AHuman : public AHumanBase {
public:
    BYTE PropertyPad[32];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AHuman);

///////////////////////////////////////////////////////
// Actor class ADragon:APawn:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ADragonBase : public APawnBase {
public:
};
class IMPLEMENTATION_API ADragon : public ADragonBase {
public:
    BYTE PropertyPad[32];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ADragon);

///////////////////////////////////////////////////////
// Actor class ASkaarj:APawn:AActor
///////////////////////////////////////////////////////

enum ESkaarjAnimationTriggers {
    SkaarjAT_None           =0,
    SkaarjAT_Lunge          =1,
    SkaarjAT_Spin           =2,
    SkaarjAT_Fire           =3,
    SkaarjAT_ClawLeft       =4,
    SkaarjAT_ClawRight      =5,
    SkaarjAT_MAX            =6,
};

enum ESkaarjAnimations {
    SkaarjA_None            =0,
    SkaarjA_Squat           =1,
    SkaarjA_Blade           =2,
    SkaarjA_TwoClaw         =3,
    SkaarjA_Death           =4,
    SkaarjA_Fighter         =5,
    SkaarjA_HeadUp          =6,
    SkaarjA_Firing          =7,
    SkaarjA_Looking         =8,
    SkaarjA_Jog             =9,
    SkaarjA_Lunge           =10,
    SkaarjA_Spin            =11,
    SkaarjA_T1              =12,
    SkaarjA_T2              =13,
    SkaarjA_T3              =14,
    SkaarjA_T4              =15,
    SkaarjA_T5              =16,
    SkaarjA_T6              =17,
    SkaarjA_TakeHit         =18,
    SkaarjA_MAX             =19,
};

class IMPLEMENTATION_API ASkaarjBase : public APawnBase {
public:
    BYTE       Hack1                    /* enum ESkaarjAnimationTriggers */;
    BYTE       Hack2                    /* enum ESkaarjAnimations */;
    USound     *LungeSound;
    USound     *SpinSound;
    USound     *ClawSound;
    USound     *ShootSound;
    BYTE       LungeDamage;
    BYTE       SpinDamage;
    BYTE       ClawDamage;
    BYTE       ShootDamage;
};
class IMPLEMENTATION_API ASkaarj : public ASkaarjBase {
public:
    BYTE PropertyPad[8];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ASkaarj);

///////////////////////////////////////////////////////
// Actor class ABigMan:APawn:AActor
///////////////////////////////////////////////////////

enum EBigManAnimationTriggers {
    BigManAT_None           =0,
    BigManAT_StillFireLeft  =1,
    BigManAT_StillFireRight =2,
    BigManAT_PistolWhip     =3,
    BigManAT_GutFire        =4,
    BigManAT_WalkFireLeft   =5,
    BigManAT_WalkFireRight  =6,
    BigManAT_MAX            =7,
};

enum EBigManAnimations {
    BigManA_None            =0,
    BigManA_StillLook       =1,
    BigManA_StillFire       =2,
    BigManA_PistolWhip      =3,
    BigManA_Sleep           =4,
    BigManA_GutShot         =5,
    BigManA_DieForward      =6,
    BigManA_ShootLeft       =7,
    BigManA_Walk            =8,
    BigManA_WalkLeft        =9,
    BigManA_WalkRight       =10,
    BigManA_ShootRight      =11,
    BigManA_T1              =12,
    BigManA_T2              =13,
    BigManA_T3              =14,
    BigManA_T4              =15,
    BigManA_T5              =16,
    BigManA_TakeHit         =17,
    BigManA_DieBackward     =18,
    BigManA_MAX             =19,
};

class IMPLEMENTATION_API ABigManBase : public APawnBase {
public:
    BYTE       Hack1                    /* enum EBigManAnimationTriggers */;
    BYTE       Hack2                    /* enum EBigManAnimations */;
    USound     *ShootSound;
    USound     *WhipSound;
    BYTE       ShootDamage;
    BYTE       WhipDamage;
};
class IMPLEMENTATION_API ABigMan : public ABigManBase {
public:
    BYTE PropertyPad[18];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ABigMan);

///////////////////////////////////////////////////////
// Actor class AArchAngel:ABigMan:APawn:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AArchAngelBase : public ABigManBase {
public:
};
class IMPLEMENTATION_API AArchAngel : public AArchAngelBase {
public:
    BYTE PropertyPad[18];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AArchAngel);

///////////////////////////////////////////////////////
// Actor class AGasbag:APawn:AActor
///////////////////////////////////////////////////////

enum EGasBagAnimationTriggers {
    GasBagAT_None           =0,
    GasBagAT_PunchLeft      =1,
    GasBagAT_PunchRight     =2,
    GasBagAT_Pound          =3,
    GasBagAT_Belch          =4,
    GasBagAT_MAX            =5,
};

enum EGasBagAnimations {
    GasBagA_None            =0,
    GasBagA_TwoPunch        =1,
    GasBagA_Belch           =2,
    GasBagA_Deflate         =3,
    GasBagA_Fiddle          =4,
    GasBagA_Fighter         =5,
    GasBagA_Float           =6,
    GasBagA_Grab            =7,
    GasBagA_Pound           =8,
    GasBagA_T1              =9,
    GasBagA_T2              =10,
    GasBagA_T3              =11,
    GasBagA_T4              =12,
    GasBagA_TakeHit         =13,
    GasBagA_MAX             =14,
};

class IMPLEMENTATION_API AGasbagBase : public APawnBase {
public:
    BYTE       Hack1                    /* enum EGasBagAnimationTriggers */;
    BYTE       Hack2                    /* enum EGasBagAnimations */;
    USound     *BelchSound;
    USound     *PunchSound;
    USound     *PoundSound;
    BYTE       BelchDamage;
    BYTE       PunchDamage;
    BYTE       PoundDamage;
};
class IMPLEMENTATION_API AGasbag : public AGasbagBase {
public:
    BYTE PropertyPad[13];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AGasbag);

///////////////////////////////////////////////////////
// Actor class AManta:APawn:AActor
///////////////////////////////////////////////////////

enum EMantaAnimationTriggers {
    MantaAT_None            =0,
    MantaAT_Sting           =1,
    MantaAT_Whip            =2,
    MantaAT_MAX             =3,
};

enum EMantaAnimations {
    MantaA_None             =0,
    MantaA_Fly              =1,
    MantaA_Sting            =2,
    MantaA_Whip             =3,
    MantaA_Die              =4,
    MantaA_Land             =5,
    MantaA_Launch           =6,
    MantaA_MAX              =7,
};

class IMPLEMENTATION_API AMantaBase : public APawnBase {
public:
    BYTE       Hack1                    /* enum EMantaAnimationTriggers */;
    BYTE       Hack2                    /* enum EMantaAnimations */;
    USound     *StingSound;
    USound     *WhipSound;
    BYTE       StingDamage;
    BYTE       WhipDamage;
};
class IMPLEMENTATION_API AManta : public AMantaBase {
public:
    BYTE PropertyPad[18];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AManta);

///////////////////////////////////////////////////////
// Actor class ATentacle:APawn:AActor
///////////////////////////////////////////////////////

enum ETentacleAnimationTriggers {
    TentacleAT_None         =0,
    TentacleAT_Shoot        =1,
    TentacleAT_MAX          =2,
};

enum ETentacleAnimations {
    TentacleA_None          =0,
    TentacleA_Waver         =1,
    TentacleA_Shoot         =2,
    TentacleA_Mebax         =3,
    TentacleA_Death         =4,
    TentacleA_MAX           =5,
};

class IMPLEMENTATION_API ATentacleBase : public APawnBase {
public:
    BYTE       Hack1                    /* enum ETentacleAnimationTriggers */;
    BYTE       Hack2                    /* enum ETentacleAnimations */;
    USound     *ShootSound;
    USound     *MebaxSound;
    UClass     *Projectile;
    BYTE       WhipDamage;
};
class IMPLEMENTATION_API ATentacle : public ATentacleBase {
public:
    BYTE PropertyPad[15];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ATentacle);

///////////////////////////////////////////////////////
// Actor class ALight:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ALightBase : public AActorBase {
public:
};
class IMPLEMENTATION_API ALight : public ALightBase {
public:
    BYTE PropertyPad[643];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ALight);

///////////////////////////////////////////////////////
// Actor class ASatellite:ALight:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ASatelliteBase : public ALightBase {
public:
    FLOAT      Period;
    FLOAT      Phase;
};
class IMPLEMENTATION_API ASatellite : public ASatelliteBase {
public:
    BYTE PropertyPad[632];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ASatellite);

///////////////////////////////////////////////////////
// Actor class ATorchFlame:ALight:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ATorchFlameBase : public ALightBase {
public:
};
class IMPLEMENTATION_API ATorchFlame : public ATorchFlameBase {
public:
    BYTE PropertyPad[643];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ATorchFlame);

///////////////////////////////////////////////////////
// Actor class ASpotlight:ALight:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ASpotlightBase : public ALightBase {
public:
};
class IMPLEMENTATION_API ASpotlight : public ASpotlightBase {
public:
    BYTE PropertyPad[643];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ASpotlight);

///////////////////////////////////////////////////////
// Actor class AKeypoint:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AKeypointBase : public AActorBase {
public:
};
class IMPLEMENTATION_API AKeypoint : public AKeypointBase {
public:
    BYTE PropertyPad[643];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AKeypoint);

///////////////////////////////////////////////////////
// Actor class APlayerStart:AKeypoint:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API APlayerStartBase : public AKeypointBase {
public:
    UClass     *PlayerSpawnClass;
    BYTE       TeamNumber;
};
class IMPLEMENTATION_API APlayerStart : public APlayerStartBase {
public:
    BYTE PropertyPad[635];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ATeleporter:AKeypoint:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ATeleporterBase : public AKeypointBase {
public:
    CHAR       TeleportURL[64];
    FName      ProductRequired;
    DWORD      bChangesVelocity:1;
    DWORD      bChangesYaw:1;
    DWORD      bReversesX:1;
    DWORD      bReversesY:1;
    DWORD      bReversesZ:1;
    INT        TargetYaw;
    FVector    TargetVelocity;
};
class IMPLEMENTATION_API ATeleporter : public ATeleporterBase {
public:
    BYTE PropertyPad[548];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ATeleporter);

///////////////////////////////////////////////////////
// Actor class AZoneDescriptor:AKeypoint:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AZoneDescriptorBase : public AKeypointBase {
public:
    CHAR       ZoneName[32];
    DWORD      bWaterZone:1;
    DWORD      bFogZone:1;
    DWORD      bKillZone:1;
    DWORD      bEchoZone:1;
    DWORD      bNeutralZone:1;
    DWORD      bGravityZone:1;
    DWORD      bTeamOnly:1;
    FVector    ZoneGravity;
    FVector    ZoneVelocity;
    USound     *ZoneAmbientSound;
    FLOAT      ZoneBreadth;
    FLOAT      ZoneReflectivity;
    BYTE       AmbientBrightness;
    BYTE       AmbientHue;
    BYTE       AmbientSaturation;
    BYTE       RampHue;
    BYTE       RampSaturation;
    BYTE       FogThickness;
    BYTE       FogHue;
    BYTE       FogSaturation;
};
class IMPLEMENTATION_API AZoneDescriptor : public AZoneDescriptorBase {
public:
    BYTE PropertyPad[552];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ABlockMonsters:AKeypoint:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ABlockMonstersBase : public AKeypointBase {
public:
};
class IMPLEMENTATION_API ABlockMonsters : public ABlockMonstersBase {
public:
    BYTE PropertyPad[643];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ABlockAll:AKeypoint:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ABlockAllBase : public AKeypointBase {
public:
};
class IMPLEMENTATION_API ABlockAll : public ABlockAllBase {
public:
    BYTE PropertyPad[643];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ALevelDescriptor:AKeypoint:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ALevelDescriptorBase : public AKeypointBase {
public:
    FVector    LevelGravity;
    FLOAT      DayFactor;
    FLOAT      DayBase;
    FLOAT      Time;
    FLOAT      DayFraction;
    FLOAT      NightFraction;
    INT        Ticks;
    INT        Year;
    INT        Month;
    INT        Day;
    INT        Hour;
    INT        Minute;
    INT        Second;
    INT        Millisecond;
    CHAR       LevelTitle[64];
    CHAR       LevelAuthor[64];
    CHAR       LevelMusic[64];
    CHAR       LevelEnterText[64];
    DWORD      bLonePlayer:1;
    DWORD      bMirrorSky:1;
    FLOAT      TexUPanSpeed;
    FLOAT      TexVPanSpeed;
    FLOAT      SkyScale;
    FLOAT      SkyDayBright;
    FLOAT      SkyNightBright;
    FLOAT      SkyUPanSpeed;
    FLOAT      SkyVPanSpeed;
    FLOAT      SkyWavyness;
    FLOAT      SkyWavySpeed;
    FLOAT      SkyFlicker;
    UTexture   *SkyTexture;
    UTexture   *SkyPalette;
    BYTE       SkyFogBrightness;
    BYTE       SkyFogHue;
    BYTE       SkyFogSaturation;
    INT        RandomSeed;
    INT        RandomValue;
    INT        ItemGoals;
    INT        KillGoals;
    INT        SecretGoals;
};
class IMPLEMENTATION_API ALevelDescriptor : public ALevelDescriptorBase {
public:
    BYTE PropertyPad[240];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AAmbientSound:AKeypoint:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AAmbientSoundBase : public AKeypointBase {
public:
};
class IMPLEMENTATION_API AAmbientSound : public AAmbientSoundBase {
public:
    BYTE PropertyPad[643];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ATextMessage:AKeypoint:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ATextMessageBase : public AKeypointBase {
public:
};
class IMPLEMENTATION_API ATextMessage : public ATextMessageBase {
public:
    BYTE PropertyPad[643];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AMover:AActor
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

class IMPLEMENTATION_API AMoverBase : public AActorBase {
public:
    BYTE       MoverBumpType            /* enum EMoverBumpType */;
    BYTE       MoverTriggerType         /* enum EMoverTriggerType */;
    BYTE       MoverGlideType           /* enum EMoverGlideType */;
    BYTE       KeyNum;
    BYTE       PrevKeyNum;
    BYTE       WorldRaytraceKey;
    BYTE       BrushRaytraceKey;
    INT        MoverTime;
    INT        RemainOpenTime;
    FLOAT      BumpPlayerDamage;
    FRotation  FreeRotation;
    DWORD      bCanInterruptMove:1;
    DWORD      bSlave:1;
    DWORD      bTrigger:1;
    DWORD      bMoving:1;
    DWORD      bReverseWhenDone:1;
    DWORD      bTriggerOnceOnly:1;
    USound     *OpenSound;
    USound     *ClosedSound;
    USound     *MoveAmbientSound;
    FVector    KeyPos[4];
    FRotation  KeyRot[4];
    INDEX      iSlaves[16];
    FVector    BasePos;
    FVector    OldPos;
    FRotation  BaseRot;
    FRotation  OldRot;
    INT        CurTime;
    INT        HoldTime;
    DWORD      bAdded:1;
};
class IMPLEMENTATION_API AMover : public AMoverBase {
public:
    BYTE PropertyPad[348];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AMover);

///////////////////////////////////////////////////////
// Actor class AInventory:AActor
///////////////////////////////////////////////////////

enum EInvState {
    INV_None                =0,
    INV_Active              =1,
    INV_Activating          =2,
    INV_DeActivating        =3,
    INV_Using1              =4,
    INV_Using2              =5,
    INV_UsingCloseUp        =6,
    INV_Reloading           =7,
    INV_Playing             =8,
    INV_MAX                 =9,
};

enum EInventorySet {
    INVS_NoSet              =0,
    INVS_WeaponSet1         =1,
    INVS_WeaponSet2         =2,
    INVS_WeaponSet3         =3,
    INVS_WeaponSet4         =4,
    INVS_WeaponSet5         =5,
    INVS_WeaponSet6         =6,
    INVS_WeaponSet7         =7,
    INVS_WeaponSet8         =8,
    INVS_WeaponSet9         =9,
    INVS_WeaponSet10        =10,
    INVS_MAX                =11,
};

class IMPLEMENTATION_API AInventoryBase : public AActorBase {
public:
    BYTE       InvState                 /* enum EInvState */;
    INDEX      iNextActive;
    DWORD      bInPickupState:1;
    DWORD      bActiveInSet:1;
    DWORD      bNeedsReloading:1;
    USound     *PickupSound;
    USound     *RespawnSound;
    DWORD      bRespawnNetOnly:1;
    INT        RespawnTime;
    CHAR       PickupMessage[64];
    DWORD      bTakesDamage:1;
    UClass     *EffectWhenDestroyed;
    BYTE       OwningSet                /* enum EInventorySet */;
    BYTE       AutoSwitchPriority;
    FLOAT      DrawForward;
    FLOAT      DrawDown;
    INT        DrawPitch;
    INT        DrawRoll;
    INT        DrawYaw;
    UMeshMap   *PickupMesh;
    FLOAT      PickupScale;
    UMeshMap   *PlayerViewMesh;
    FLOAT      PlayerViewScale;
    UTexture   *AmmoStatusIcon;
    INT        YawSpeed;
    INT        PitchSpeed;
    INT        RollSpeed;
};
class IMPLEMENTATION_API AInventory : public AInventoryBase {
public:
    BYTE PropertyPad[484];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AInventory);

///////////////////////////////////////////////////////
// Actor class AWeapon:AInventory:AActor
///////////////////////////////////////////////////////

enum EAmmoType {
    AmmoType_Bullets        =0,
    AmmoType_Shells         =1,
    AmmoType_StingerAmmo    =2,
    AmmoType_FlameGunAmmo   =3,
    AmmoType_Reserved2      =4,
    AmmoType_Reserved3      =5,
    AmmoType_Reserved4      =6,
    AmmoType_Reserved5      =7,
    AmmoType_Reserved6      =8,
    AmmoType_Reserved7      =9,
    AmmoType_Count          =10,
    AmmoType_None           =11,
    AmmoType_MAX            =12,
};

class IMPLEMENTATION_API AWeaponBase : public AInventoryBase {
public:
    DWORD      bAutoVTarget:1;
    DWORD      bAutoHTarget:1;
    DWORD      bWasReleased:1;
    FLOAT      MaxTargetRange;
    FLOAT      SeekDamping;
    BYTE       AmmoType                 /* enum EAmmoType */;
    INT        AmmoUsed[2];
    BYTE       Discharges[2];
    BYTE       ReloadCount;
    FLOAT      Noise[2];
    BYTE       ReusePeriod[2];
    FLOAT      RecoilForce[2];
    BYTE       RecoilPitch[2];
    UClass     *MuzzleEffectClass[2];
    USound     *DischargeSounds[2];
    BYTE       bRepeatSounds[2];
    USound     *ReloadSound;
    USound     *CloseUpSound;
    INT        PickupAmmoCount;
    BYTE       CloseUpDamage;
    FLOAT      CloseUpStrengthFactor;
    INT        UseTime;
    INT        LastUseTime;
    UClass     *ProjectileClass[2];
    FLOAT      ProjStartDist;
    FLOAT      Dispersion[2];
};
class IMPLEMENTATION_API AWeapon : public AWeaponBase {
public:
    BYTE PropertyPad[364];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AWeapon);

///////////////////////////////////////////////////////
// Actor class AAutoMag:AWeapon:AInventory:AActor
///////////////////////////////////////////////////////

enum EAutoMagAnimations {
    EAMA_None               =0,
    EAMA_Still              =1,
    EAMA_Shoot              =2,
    EAMA_Shoot2             =3,
    EAMA_Twirl              =4,
    EAMA_Whip               =5,
    EAMA_T1                 =6,
    EAMA_T2                 =7,
    EAMA_MAX                =8,
};

enum EAutoMagAnimationTriggers {
    AutoMagAT_None          =0,
    AutoMagAT_Fire1         =1,
    AutoMagAT_Fire2         =2,
    AutoMagAT_MAX           =3,
};

class IMPLEMENTATION_API AAutoMagBase : public AWeaponBase {
public:
    BYTE       Hack1                    /* enum EAutoMagAnimations */;
    BYTE       Hack2                    /* enum EAutoMagAnimationTriggers */;
};
class IMPLEMENTATION_API AAutoMag : public AAutoMagBase {
public:
    BYTE PropertyPad[362];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AAutoMag);

///////////////////////////////////////////////////////
// Actor class AQuadShot:AWeapon:AInventory:AActor
///////////////////////////////////////////////////////

enum EQuadShotAnimations {
    EQSA_None               =0,
    EQSA_Fire               =1,
    EQSA_Reload             =2,
    EQSA_MAX                =3,
};

enum EQuadShotAnimationTriggers {
    QuadShotAT_None         =0,
    QuadShotAT_Fire         =1,
    QuadShotAT_MAX          =2,
};

class IMPLEMENTATION_API AQuadShotBase : public AWeaponBase {
public:
    BYTE       Hack1                    /* enum EQuadShotAnimations */;
    BYTE       Hack2                    /* enum EQuadShotAnimationTriggers */;
};
class IMPLEMENTATION_API AQuadShot : public AQuadShotBase {
public:
    BYTE PropertyPad[362];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AQuadShot);

///////////////////////////////////////////////////////
// Actor class AFlameGun:AWeapon:AInventory:AActor
///////////////////////////////////////////////////////

enum EFlameGunAnimations {
    EFGA_None               =0,
    EFGA_Still              =1,
    EFGA_Drop               =2,
    EFGA_Fire               =3,
    EFGA_MAX                =4,
};

class IMPLEMENTATION_API AFlameGunBase : public AWeaponBase {
public:
    BYTE       Hack1                    /* enum EFlameGunAnimations */;
};
class IMPLEMENTATION_API AFlameGun : public AFlameGunBase {
public:
    BYTE PropertyPad[363];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AFlameGun);

///////////////////////////////////////////////////////
// Actor class AStinger:AWeapon:AInventory:AActor
///////////////////////////////////////////////////////

enum EStingerAnimations {
    EStingA_None            =0,
    EStingA_Still           =1,
    EStingA_Fire1           =2,
    EStingA_Fire3           =3,
    EStingA_MAX             =4,
};

enum EStingerAnimationTriggers {
    StingerAT_None          =0,
    StingerAT_Fire1         =1,
    StingerAT_Fire2         =2,
    StingerAT_Fire3         =3,
    StingerAT_MAX           =4,
};

class IMPLEMENTATION_API AStingerBase : public AWeaponBase {
public:
    BYTE       Hack1                    /* enum EStingerAnimations */;
    BYTE       Hack2                    /* enum EStingerAnimationTriggers */;
    BYTE       PendingShots[5];
    BYTE       PendingShotCount;
};
class IMPLEMENTATION_API AStinger : public AStingerBase {
public:
    BYTE PropertyPad[356];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AStinger);

///////////////////////////////////////////////////////
// Actor class APickup:AInventory:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API APickupBase : public AInventoryBase {
public:
    BYTE       Unused;
};
class IMPLEMENTATION_API APickup : public APickupBase {
public:
    BYTE PropertyPad[483];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(APickup);

///////////////////////////////////////////////////////
// Actor class APowerUp:APickup:AInventory:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API APowerUpBase : public APickupBase {
public:
    FLOAT      Strength;
    FLOAT      Stamina;
    FLOAT      Health;
    FLOAT      Armor[4];
    INT        TimeLimit;
    DWORD      bInvisibility:1;
    DWORD      bSilence:1;
    DWORD      bInvincibility:1;
    DWORD      bSuperStrength:1;
    DWORD      bSuperStamina:1;
};
class IMPLEMENTATION_API APowerUp : public APowerUpBase {
public:
    BYTE PropertyPad[444];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(APowerUp);

///////////////////////////////////////////////////////
// Actor class AArmor:APowerUp:APickup:AInventory:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AArmorBase : public APowerUpBase {
public:
};
class IMPLEMENTATION_API AArmor : public AArmorBase {
public:
    BYTE PropertyPad[444];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AHealth:APowerUp:APickup:AInventory:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AHealthBase : public APowerUpBase {
public:
};
class IMPLEMENTATION_API AHealth : public AHealthBase {
public:
    BYTE PropertyPad[444];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AAmmo:APickup:AInventory:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AAmmoBase : public APickupBase {
public:
    INT        AmmoCount[10];
};
class IMPLEMENTATION_API AAmmo : public AAmmoBase {
public:
    BYTE PropertyPad[440];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AClip:AAmmo:APickup:AInventory:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AClipBase : public AAmmoBase {
public:
};
class IMPLEMENTATION_API AClip : public AClipBase {
public:
    BYTE PropertyPad[440];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AShells:AAmmo:APickup:AInventory:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AShellsBase : public AAmmoBase {
public:
};
class IMPLEMENTATION_API AShells : public AShellsBase {
public:
    BYTE PropertyPad[440];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AStingerAmmo:AAmmo:APickup:AInventory:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AStingerAmmoBase : public AAmmoBase {
public:
};
class IMPLEMENTATION_API AStingerAmmo : public AStingerAmmoBase {
public:
    BYTE PropertyPad[440];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ATriggers:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ATriggersBase : public AActorBase {
public:
};
class IMPLEMENTATION_API ATriggers : public ATriggersBase {
public:
    BYTE PropertyPad[643];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ATriggers);

///////////////////////////////////////////////////////
// Actor class ATrigger:ATriggers:AActor
///////////////////////////////////////////////////////

enum ETriggerType {
    TT_Proximity            =0,
    TT_Shoot                =1,
    TT_MAX                  =2,
};

class IMPLEMENTATION_API ATriggerBase : public ATriggersBase {
public:
    BYTE       TriggerType              /* enum ETriggerType */;
    CHAR       Message[80];
    DWORD      bTriggerOnceOnly:1;
};
class IMPLEMENTATION_API ATrigger : public ATriggerBase {
public:
    BYTE PropertyPad[552];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ATrigger);

///////////////////////////////////////////////////////
// Actor class ACounter:ATriggers:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ACounterBase : public ATriggersBase {
public:
    BYTE       NumToCount;
    CHAR       CountMessage[80];
    CHAR       CompleteMessage[80];
    DWORD      bShowMessage:1;
};
class IMPLEMENTATION_API ACounter : public ACounterBase {
public:
    BYTE PropertyPad[472];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ACounter);

///////////////////////////////////////////////////////
// Actor class ADispatcher:ATriggers:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ADispatcherBase : public ATriggersBase {
public:
    FName      OutEvents[8];
    INT        OutDelays[8];
    DWORD      bActive:1;
    INT        Count;
};
class IMPLEMENTATION_API ADispatcher : public ADispatcherBase {
public:
    BYTE PropertyPad[584];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ADispatcher);

///////////////////////////////////////////////////////
// Actor class ADecoration:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ADecorationBase : public AActorBase {
public:
    UClass     *EffectWhenDestroyed;
};
class IMPLEMENTATION_API ADecoration : public ADecorationBase {
public:
    BYTE PropertyPad[636];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ADecoration);

///////////////////////////////////////////////////////
// Actor class AVase:ADecoration:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AVaseBase : public ADecorationBase {
public:
};
class IMPLEMENTATION_API AVase : public AVaseBase {
public:
    BYTE PropertyPad[636];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AChandelier:ADecoration:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AChandelierBase : public ADecorationBase {
public:
};
class IMPLEMENTATION_API AChandelier : public AChandelierBase {
public:
    BYTE PropertyPad[636];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AHammok:ADecoration:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AHammokBase : public ADecorationBase {
public:
};
class IMPLEMENTATION_API AHammok : public AHammokBase {
public:
    BYTE PropertyPad[636];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API APyrotechnicsBase : public AActorBase {
public:
    FLOAT      GravityMult;
    FLOAT      AccelerationFactor;
    BYTE       Noise;
    USound     *InitialSound;
    UTexture   *Textures[20];
};
class IMPLEMENTATION_API APyrotechnics : public APyrotechnicsBase {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(APyrotechnics);

///////////////////////////////////////////////////////
// Actor class AWallHit:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AWallHitBase : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API AWallHit : public AWallHitBase {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class APawnHit:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API APawnHitBase : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API APawnHit : public APawnHitBase {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AExplode1:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AExplode1Base : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API AExplode1 : public AExplode1Base {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AExplode3:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AExplode3Base : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API AExplode3 : public AExplode3Base {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ABigManGunFlash:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ABigManGunFlashBase : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API ABigManGunFlash : public ABigManGunFlashBase {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ASkaarjGunFlash:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ASkaarjGunFlashBase : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API ASkaarjGunFlash : public ASkaarjGunFlashBase {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AGasBagBelchFlash:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AGasBagBelchFlashBase : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API AGasBagBelchFlash : public AGasBagBelchFlashBase {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class APlayerRespawn:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API APlayerRespawnBase : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API APlayerRespawn : public APlayerRespawnBase {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ATeleportIn:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ATeleportInBase : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API ATeleportIn : public ATeleportInBase {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class ATeleportOut:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ATeleportOutBase : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API ATeleportOut : public ATeleportOutBase {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AExplode2:APyrotechnics:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AExplode2Base : public APyrotechnicsBase {
public:
};
class IMPLEMENTATION_API AExplode2 : public AExplode2Base {
public:
    BYTE PropertyPad[544];
    FActorPrivate Private;
};
///////////////////////////////////////////////////////
// Actor class AExplosion:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AExplosionBase : public AActorBase {
public:
    UClass     *Effect;
    BYTE       Delay;
    USound     *InitialSound;
    BYTE       Noise;
    UClass     *Debris[10];
    INT        DebrisCount;
    FLOAT      Radius;
    FLOAT      RadiusIncrement;
    BYTE       Damage[4];
    FLOAT      DamageIncrement;
    FLOAT      Momentum;
    FLOAT      MomentumIncrement;
};
class IMPLEMENTATION_API AExplosion : public AExplosionBase {
public:
    BYTE PropertyPad[556];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AExplosion);

///////////////////////////////////////////////////////
// Actor class AClipExplosion:AExplosion:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AClipExplosionBase : public AExplosionBase {
public:
};
class IMPLEMENTATION_API AClipExplosion : public AClipExplosionBase {
public:
    BYTE PropertyPad[556];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AClipExplosion);

///////////////////////////////////////////////////////
// Actor class AShellExplosion:AExplosion:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API AShellExplosionBase : public AExplosionBase {
public:
};
class IMPLEMENTATION_API AShellExplosion : public AShellExplosionBase {
public:
    BYTE PropertyPad[556];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(AShellExplosion);

///////////////////////////////////////////////////////
// Actor class ATarydiumExplosion:AExplosion:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ATarydiumExplosionBase : public AExplosionBase {
public:
};
class IMPLEMENTATION_API ATarydiumExplosion : public ATarydiumExplosionBase {
public:
    BYTE PropertyPad[556];
    FActorPrivate Private;
    INT Process(ILevel *Level, FName Message, void *Params);
};
AUTOREGISTER_CLASS(ATarydiumExplosion);

#pragma pack (pop) /* Restore alignment to previous setting */
