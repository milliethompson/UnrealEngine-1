/*=============================================================================
	UnMsgPar.h: Actor message parameter class definitions

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNMSGPAR
#define _INC_UNMSGPAR

/*-----------------------------------------------------------------------------
	Actor message parameter classes.
-----------------------------------------------------------------------------*/

//
// Child classes of FActorMessageParams are all sent to actors using the 
// "void *Params" pointer in various actor messages.
// Refer to UnMsgs.h for the parameter types expected for particular messages.
//
class PMessageParams
{
	// Contains nothing.
};

//!!
class PSense : public PMessageParams
{
public:
    INDEX       SensedActor     ; // Sensed actor, or INDEX_NONE.
    BOOL        LocationKnown   ; // TRUE if Location is known.
    union
    {
        FVector Location        ; // LocationKnown==TRUE: Location of sensed object
        FVector Direction       ; // LocationKnown==FALSE: Direction of sensed object
    };
    // Notes:
    //   1. The accuracy of this->Direction depends on the accuracy of the sensing actor.
    static PSense & Convert(void * Params) { return *(PSense*)Params; }
};

//!!
class PFrame : public PMessageParams
{
public:
    int     Sequence    ; // Animation sequence (1..)
    int     Frame       ; // Animation frame (0..)
    int     Trigger     ; // Trigger value associated with the frame.
    static PFrame & Convert(void * Params) { return *(PFrame*)Params; }
};

//
// Player movement packet information structure:
//
//!!
enum EMovementAxis // An axis of player movement:
	{
	PlayerAxis_None         , // No axis (always 0).
    // Movements:
	PlayerAxis_Forward      , // Forward (>0) or backward (<0)
	PlayerAxis_Rightward    , // Rightward (>0) or leftward (<0)
	PlayerAxis_Upward       , // Upward (>0) or downward (<0)
    // Rotations:
	PlayerAxis_Yaw          , // Yaw to the right (>0) or left (<0)
	PlayerAxis_Pitch        , // Pitch upward (>0) or downward (<0)
	PlayerAxis_Roll         , // Roll clockwise (>0) or counterclockwise (<0)
    // Administrative:
    PlayerAxis_Count        , // Number of axes, including PlayerAxis_None.

    // Keep the following synchronized with the above values:
    PlayerAxis_FirstMovement    = PlayerAxis_Forward    ,
    PlayerAxis_LastMovement     = PlayerAxis_Upward     ,
    PlayerAxis_MovementCount    = PlayerAxis_LastMovement - PlayerAxis_FirstMovement + 1,
    PlayerAxis_FirstRotation    = PlayerAxis_Yaw        ,
    PlayerAxis_LastRotation     = PlayerAxis_Roll       ,
    PlayerAxis_RotationCount    = PlayerAxis_LastRotation - PlayerAxis_FirstRotation + 1
	};

//!!
struct PPlayerMotion
{
    FLOAT   Analog          ; // Analog position of motion.
    FLOAT   Differential    ; // Differential change in motion.
    void Empty() { Analog = 0; Differential = 0; }
    static void Empty( PPlayerMotion * Motions, int Count ) // Empty an array of player motions.
    {
        memset( Motions, 0, Count * sizeof(Motions[0]) ); // Note: This assumes 0-ness of floating point 0.
    }
};

//!!
class PPlayerTick : public PMessageParams
{
public:
    PPlayerMotion Movements[PlayerAxis_Count]; 
    //todo: Make Movements an array of PlayerAxis_MovementCount elements once the
    // rotation handling has been moved to PCalcView.
    //todo: [add] FAction::TActionStatus Actions[FAction::ActionCount];
    // We use the declaration below to avoid many many many recompilations
    // after changes in FAction. Eventually, we should use the above declaration
    // instead of this:
    BYTE Actions[100];
	int	 BuildAllMovement(UCamera *Camera);
    static PPlayerTick & Convert(void * Params) { return *(PPlayerTick*)Params; }
};

//
// View calculation information structure, sent to actors
// with active cameras immediately before rendering, to allow the actors
// to move the viewpoint for effects such as bouncing the player's view
// while she's walking, or implementing a behind-the-player view.
//
//!!
class PCalcView : public PMessageParams
	{
	public:
	FVector	  ViewLocation;
	FRotation ViewRotation;
	FCoords	  *Coords;
	FCoords	  *Uncoords;
    PPlayerMotion Rotations[PlayerAxis_RotationCount];  // Indexed by (EMovementAxis-PlayerAxis_FirstRotation)
    const PPlayerMotion & Rotation(EMovementAxis RotationAxis) const // RotationAxis must be a rotation.
        {
        return Rotations[RotationAxis - PlayerAxis_FirstRotation];
        }
    PPlayerMotion & Rotation(EMovementAxis RotationAxis) // RotationAxis must be a rotation.
        {
        return Rotations[RotationAxis - PlayerAxis_FirstRotation];
        }
    static PCalcView & Convert(void * Params) { return *(PCalcView*)Params; }
	};

// Actor identification.
class PActor : public PMessageParams
{
	public:
	INDEX iActor;
    static PActor & Convert(void * Params) { return *(PActor*)Params; }
};

// Touch identification.
class PTouch : public PActor
{
	public:
    static PTouch & Convert(void * Params) { return *(PTouch*)Params; }
};

// Pickup identification.
class PPickupQuery : public PActor
{
	public:
    static PPickupQuery & Convert(void * Params) { return *(PPickupQuery*)Params; }
};

// Hit identification.
//!!
class PHit : public PMessageParams
{
public:
	INDEX iInstigator ;             // Ultimate instigator (player or pawn) of hit.
	INDEX iSourceActor;		// Actor which is the direct source of damage, or INDEX_NONE.
	INDEX iSourceWeapon;	// Weapon who originated the hit or INDEX_NONE
    //tbi: It would be nice to use DMT_Count below instead of "4".
	FLOAT Damage[4]; // Damage, indexed with EDamageType.
    FLOAT ActualDamage  ; // Output value: actual total damage inflicted on pawn.
	FVector HitLocation;	// Exact location of hit
    FLOAT Momentum;  // Momentum to impart (based on direction of iSourceActor), 0 for none.
    void Empty()
    {
        iInstigator     = INDEX_NONE    ;
        iSourceActor    = INDEX_NONE    ;
        iSourceWeapon   = INDEX_NONE    ;
        for( int Which = 0; Which < 4; ++Which )
        {
			Damage[Which] = 0;
        }
        ActualDamage = 0;
        Momentum = 0;
        HitLocation = GMath.ZeroVector;
    }
    static PHit & Convert(void * Params) { return *(PHit*)Params; }
};

//
// Parameters for HitNotify message:
//
//!!
class PHitNotify : public PMessageParams
{
public:
	FVector		HitLocation;
	INDEX		iHitActor;
    static PHitNotify & Convert(void * Params) { return *(PHitNotify*)Params; }
};

//
// Parameters for WallNotify message:
//!!
class PWallNotify : public PMessageParams
{
public:
	FVector		WallLocation;
	FVector		WallNormal;
	INDEX		iWallNode;
    static PWallNotify & Convert(void * Params) { return *(PWallNotify*)Params; }
};

// Using inventory items:
class PUse : public PMessageParams
{
public:
    BOOL    Count       ; // Output value - number of times item did something.
    // Notes:
    //   1. Count for a weapon is the number of discharges effected.
    static PUse & Convert(void * Params) { return *(PUse*)Params; }
};

//
// KeyMoveTo
//
class PKeyMove : public PMessageParams
{
	public:
	BYTE KeyNum; // Keyframe number to move to
    static PKeyMove & Convert(void * Params) { return *(PKeyMove*)Params; }
};

//
// A reference to a name
//
class PName : public PMessageParams
{
public:
	INDEX iActor;
	FName Name;
    static PName & Convert(void * Params) { return *(PName*)Params; }
};


//
// Level information:
//
class PLevel : public PMessageParams
{
public:
    enum { MAX_NAME_LENGTH = 63 };
    char Name[MAX_NAME_LENGTH+1]; // +1 for trailing null.
    static PLevel & Convert(void * Params) { return *(PLevel*)Params; }
};

//
// A text message structure used by TextMsg message
//
enum {TEXTMSG_LENGTH=120};
class PText
{
public:
	INDEX iSourceActor;
	BYTE  MsgType /* ETextMsgType */;
	char Message[TEXTMSG_LENGTH];
    static PText & Convert(void * Params) { return *(PText*)Params; }
};

//
// A string return value
//
class PStringReturn
{
public:
	char String[TEXTMSG_LENGTH];
};

//
// Exec structure for actor Exec, GetProp, SetProp messages
//
class PExec
{
public:
	INDEX iSourceActor;
	char Arg[TEXTMSG_LENGTH];
    static PExec & Convert(void * Params) { return *(PExec*)Params; }
};

//
// Gameplay mode information for BeginPlay
//
class PBeginPlay
{
public:
	DWORD	bNetCooperative		:1;
	DWORD	bNetDeathMatch		:1;
	DWORD	bNetPersistent		:1;
	DWORD	bNetNoMonsters		:1;

	BYTE	Difficulty;
    static PBeginPlay & Convert(void * Params) { return *(PBeginPlay*)Params; }
};


// Information about a requested animation:
//!!
class PAnimate : public PMessageParams
{
public:
    typedef enum // Kinds of animations
    {
        NoAnimation    = 0           // Always 0.
    ,   StillAnimation               // Animation to use when the actor is still.
    ,   IdleAnimation                // Animation to use when actor is doing nothing.
    ,   MoveAnimation                // Animation to use for normal movement.
    ,   RunAnimation                 // Animation to use for faster movement.
    ,   FlyAnimation                 // Animation to use when flying, if different from MoveAnimation.
    ,   HitAnimation                 // Animation to use when actor is hit.
    ,   PainAnimation                // Animation to use when actor is in pain.
    ,   SearchAnimation              // Animation to use when actor is searching.
    ,   ThreatenAnimation            // Animation to threaten (bellow, roar, menace).
    ,   DistantStillAttackAnimation  // Animation for long-distance non-moving attack.
    ,   DistantMovingAttackAnimation // Animation for long-distance moving attack.
    ,   CloseUpAttackAnimation       // Animation for nearby attack.
    ,   DeathAnimation               // Animation to use when actor dies.
        // Notes:
        //   1. It might seem silly to have a StillAnimation, since why would you
        //      animate an actor which is still. However, actors are almost always
        //      playing an animation, even if it is a single frame.
        //   2. StillAnimation and IdleAnimation are similar but their separation allows
        //      us to have monsters which, although doing nothing really important, 
        //      nonetheless have some movement (for realism or entertainment). A monster
        //      might treat StillAnimation and IdleAnimation as the same.
    }
    TKind;  
    TKind Kind;  
    static PAnimate & Convert(void * Params) { return *(PAnimate*)Params; }
};

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
#endif // _INC_UNMSGPAR
