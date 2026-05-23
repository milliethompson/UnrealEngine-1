/*=============================================================================
	UnNames.h: Header file registering global hardcoded Unreal names.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Notes:
		This file is included by the standard Unreal.h include, and the names
		defined here are made part of the EName enumeration.

		A special macro in UnEngine.cpp causes all names to be autoregistered
		with the engine using the AUTOREGISTER_NAME macro.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/*-----------------------------------------------------------------------------
	Macros.
-----------------------------------------------------------------------------*/

// Define a message as an enumeration.
#ifndef REGISTER_NAME
	#define REGISTER_NAME(num,name) NAME_##name = num,
	#define REG_NAME_HIGH(num,name) NAME_##name = num,
	#define REGISTERING_ENUM
	enum EName {
#endif

/*-----------------------------------------------------------------------------
	Hardcoded names which are not messages.
-----------------------------------------------------------------------------*/

// Special zero value, meaning no name.
REG_NAME_HIGH(   0, None             )

// Class property types; these map straight onto CPT_.
REG_NAME_HIGH(   1, Byte		     )
REG_NAME_HIGH(   2, Int			     )
REG_NAME_HIGH(   3, Bool		     )
REG_NAME_HIGH(   4, Float		     )
REGISTER_NAME(   5, Object		     )
REG_NAME_HIGH(   6, Name		     )
REG_NAME_HIGH(   7, String		     )
REG_NAME_HIGH(   8, Vector		     )
REG_NAME_HIGH(   9, Rotation	     )
REG_NAME_HIGH(  10, Enum		     )

// Packages.
REGISTER_NAME(  20, UnEngine	     )
REGISTER_NAME(  21, UnGame		     )
REGISTER_NAME(  22, UnEditor         )

// Keywords.
REGISTER_NAME( 100, Begin			 )
REGISTER_NAME( 101, Database         )
REG_NAME_HIGH( 102, State            )
REG_NAME_HIGH( 103, Function         )
REG_NAME_HIGH( 104, Self             )
REG_NAME_HIGH( 105, True             )
REG_NAME_HIGH( 106, False            )
REG_NAME_HIGH( 107, Transient        )
REGISTER_NAME( 110, X                )
REGISTER_NAME( 111, Y                )
REGISTER_NAME( 112, Z                )
REGISTER_NAME( 114, Pitch            )
REGISTER_NAME( 115, Yaw              )
REGISTER_NAME( 116, Roll             )

// Object class names.
REGISTER_NAME( 130, Buffer			 )
REGISTER_NAME( 131, Words            )
REGISTER_NAME( 132, Array			 )
REGISTER_NAME( 133, TextBuffer		 )
REGISTER_NAME( 134, Texture			 )
REGISTER_NAME( 135, Font			 )
REGISTER_NAME( 136, Palette			 )
REGISTER_NAME( 137, TextureSet		 )
REGISTER_NAME( 138, Script			 )
REG_NAME_HIGH( 139, Class			 )
REGISTER_NAME( 140, Actors			 )
REGISTER_NAME( 141, Sound			 )
REGISTER_NAME( 142, StackTree		 )
REGISTER_NAME( 143, Mesh			 )
REGISTER_NAME( 144, Dependencies	 )
REGISTER_NAME( 145, Vectors			 )
REGISTER_NAME( 146, BspNodes		 )
REGISTER_NAME( 147, BspSurfs		 )
REGISTER_NAME( 148, LightMesh		 )
REGISTER_NAME( 149, Polys			 )
REGISTER_NAME( 150, Model			 )
REGISTER_NAME( 151, Level			 )
REGISTER_NAME( 152, ReachSpecs  	 )
REGISTER_NAME( 153, Ints             )
REGISTER_NAME( 154, Camera			 )
REGISTER_NAME( 155, Properties       )
REGISTER_NAME( 156, Player			 )
REGISTER_NAME( 157, Verts			 )
REGISTER_NAME( 158, Music			 )
REGISTER_NAME( 159, TransBuffer		 )
REGISTER_NAME( 160, Bounds			 )
REGISTER_NAME( 161, EnumDef			 )
REGISTER_NAME( 162, Linker			 )
REGISTER_NAME( 163, LinkerLoad		 )
REGISTER_NAME( 164, LinkerSave		 )
REGISTER_NAME( 165, MeshVerts		 )
REGISTER_NAME( 166, MeshTris		 )
REGISTER_NAME( 167, MeshAnimSeqs	 )
REGISTER_NAME( 168, MeshVertConnects )
REGISTER_NAME( 169, Floats			 )
REGISTER_NAME( 170, BitArray		 )
REGISTER_NAME( 171, BitMatrix        )
REGISTER_NAME( 172, Type			 )
REGISTER_NAME( 173, Primitive		 )
REGISTER_NAME( 174, MeshAnimNotifys	 )
REGISTER_NAME( 175, Cylinder         )

// Intrinsic actor class names.
REGISTER_NAME( 250, Actor			 )
REGISTER_NAME( 251, Pawn			 )
REGISTER_NAME( 252, Mover   		 )
REGISTER_NAME( 253, Brush   		 )

/*-----------------------------------------------------------------------------
	Special engine-generated probe messages.
-----------------------------------------------------------------------------*/

// In the description for each message, the type of parameter associated with the
// message is shown in square brackets, such as [PActor], or [null] if there are no
// parameters. These parameter types are found in UnMsgPar.h.

#ifndef PROBE_MIN
#define PROBE_MIN 300 /* Index of first probe message */
#define PROBE_MAX 364 /* One past last probe message */
#endif

// Creation and destruction.
REGISTER_NAME( 300, Spawned			 ) // [null] Reverse-sent to actor immediately after spawning.
REGISTER_NAME( 301, Destroyed        ) // [null] Called immediately before actor is removed from actor list.

// Gaining/losing actors.
REGISTER_NAME( 302, GainedChild		 ) // [PActor] Sent to a parent actor when another actor attaches to it.
REGISTER_NAME( 303, LostChild		 ) // [PActor] Sent to a parent actor when another actor detaches from it.
REGISTER_NAME( 304, LostReference    ) // [PLostReference] Sent to an actor when an actor it references is destroyed.

// Triggers.
REGISTER_NAME( 306, Trigger			 ) // [PTrigger] Message sent by Trigger actors.
REGISTER_NAME( 307, UnTrigger		 ) // [PTrigger] Message sent by Trigger actors.

// Physics & world interaction.
REGISTER_NAME( 308, Timer			 ) // [null] The per-actor timer has fired.
REGISTER_NAME( 309, HitWall			 ) // [PVector] Ran into a wall.
REGISTER_NAME( 310, Falling			 ) // [null] Actor is falling.
REGISTER_NAME( 311, Landed			 ) // [null] Actor has landed.
REGISTER_NAME( 312, ZoneChange		 ) // [PActor] Actor has changed into a new zone.
REGISTER_NAME( 313, Touch			 ) // [PActor] Actor was just touched by another actor.
REGISTER_NAME( 314, UnTouch			 ) // [PActor] Actor touch just ended, always sent sometime after Touch.
REGISTER_NAME( 315, Bump			 ) // [PActor] Actor was just touched and blocked. No interpenetration. No UnBump.
REGISTER_NAME( 316, BeginState		 ) // [null] Just entered a new state.
REGISTER_NAME( 317, EndState		 ) // [null] About to leave the current state.
REGISTER_NAME( 318, BaseChange		 ) // [null] Sent to actor when its floor changes.
REGISTER_NAME( 319, Attach			 ) // [PActor] Sent to actor when it's stepped on by another actor.
REGISTER_NAME( 320, Detach			 ) // [PActor] Sent to actor when another actor steps off it.
REGISTER_NAME( 321, PlayerEntered	 ) // [PActor] Sent to a ZoneInfo actor when a player enters.
REGISTER_NAME( 322, PlayerLeaving	 ) // [PActor] Sent to a ZoneInfo actor when a player is leaving.
REGISTER_NAME( 323, KillCredit		 ) // [null] Actor has just received credit for a kill.
REGISTER_NAME( 324, AnimEnd			 ) // [null] Animation sequence has ended.
REGISTER_NAME( 325, EndedRotation	 ) // [null] Physics based rotation just ended.
REGISTER_NAME( 326, InterpolateEnd   ) // [null] Movement interpolation sequence finished.
REGISTER_NAME( 327, EncroachingOn    ) // [PActor] Encroaching on another actor.
REGISTER_NAME( 328, EncroachedBy     ) // [PActor] Being encroached by another actor.

// Kills.
REGISTER_NAME( 335, Die				 ) // [null] Actor died (sent if specific die message not processed).

// Updates.
REGISTER_NAME( 336, Tick			 ) // [PTick] Clock tick update for nonplayer.
REGISTER_NAME( 337, PlayerTick		 ) // [PTick] Clock tick update for player.
REGISTER_NAME( 338, Expired		     ) // [null] Actor's LifeSpan expired.

// AI.
REGISTER_NAME( 340,SeePlayer         ) // [PActor] Can see player.
REGISTER_NAME( 341,EnemyNotVisible   ) // [null] Current Enemy is not visible.
REGISTER_NAME( 342,HearNoise         ) // [PNoise] Noise nearby.
REGISTER_NAME( 343,UpdateEyeHeight   ) // [PFloat] update eye level (after physics).
REGISTER_NAME( 344,SeeMonster        ) // [PActor] Can see non-player.
REGISTER_NAME( 345,SeeFriend         ) // [PActor] Can see non-player friend.

// Special tag meaning 'All probes'.
REGISTER_NAME( 363, All				 ) // [null] Special meaning, not a message.

/*-----------------------------------------------------------------------------
	All standard actor messages (must be 1-31 characters).
-----------------------------------------------------------------------------*/

// Querying.
REGISTER_NAME( 350, Callback		 ) // [PActor] Called by kernel in response to query messages.

// Pawn messages.
REGISTER_NAME( 360, Possess			 ) // [UCamera] Actor was just possessed by a user.
REGISTER_NAME( 361, UnPossess		 ) // [null] Actor was just unpossessed.
REGISTER_NAME( 530, PreTeleport      ) // [PActor] Called in actor about to be teleported. Return -1 to abort teleportation, +1 to accept.
REGISTER_NAME( 531, PostTeleport     ) // [PActor] Called in actor after being teleported.

// Updates.
REGISTER_NAME( 370, PlayerCalcView	 ) // [PCalcView] Calculate player view.
REGISTER_NAME( 371, InvCalcView1     ) // [null] Inventory first-person view.
REGISTER_NAME( 372, InvCalcView3     ) // [null] Inventory third-person view.

// UnrealEd messages (not scriptable).
REGISTER_NAME( 400, PostEditChange	 ) // [null] Called after editing properties.
REGISTER_NAME( 401, PostEditMove	 ) // [null] Called after an actor is moved in the editor.
REGISTER_NAME( 402, PreEditChange	 ) // [null] Called before editing properties.
REGISTER_NAME( 404, PreRaytrace		 ) // [null] Called by editor before raytracing begins.
REGISTER_NAME( 405, PostRaytrace	 ) // [null] Called by editor after raytracing ends.
REGISTER_NAME( 406, RaytraceWorld	 ) // [null] Tell actor to position itself for world raytracing.
REGISTER_NAME( 407, RaytraceBrush	 ) // [PBoolean] Tell actor to position itself for brush raytracing.

// Engine level state.
REGISTER_NAME( 450, PreBeginPlay	 ) // [null] Sent right before BeginPlay, for root initialization.
REGISTER_NAME( 451, BeginPlay		 ) // [null] Play has just begin.
REGISTER_NAME( 452, PostBeginPlay	 ) // [null] Sent right after BeginPlay, for root processing.
REGISTER_NAME( 453, EndPlay			 ) // [null] Play has just ended.
REGISTER_NAME( 454, BeginEdit		 ) // [null] Editing has just begin.
REGISTER_NAME( 455, EndEdit			 ) // [null] Editing has just ended.

/*-----------------------------------------------------------------------------
	Hardcoded names used by the compiler.
-----------------------------------------------------------------------------*/

// Constants.
REG_NAME_HIGH( 600, Vect)
REG_NAME_HIGH( 601, Rot)
REG_NAME_HIGH( 602, MaxInt)
REG_NAME_HIGH( 603, Pi)
REG_NAME_HIGH( 604, SizeOf)
REG_NAME_HIGH( 605, ArrayCount)
REG_NAME_HIGH( 606, EnumCount)

// Flow control.
REG_NAME_HIGH( 620, Else)
REG_NAME_HIGH( 621, If)
REG_NAME_HIGH( 622, Goto)
REG_NAME_HIGH( 623, Stop)
REG_NAME_HIGH( 624, Broadcast)
REG_NAME_HIGH( 625, Until)
REG_NAME_HIGH( 626, While)
REG_NAME_HIGH( 627, Do)
REG_NAME_HIGH( 628, Break)
REG_NAME_HIGH( 629, For)
REG_NAME_HIGH( 630, ForEach)
REG_NAME_HIGH( 631, Assert)
REG_NAME_HIGH( 632, Switch)
REG_NAME_HIGH( 633, Case)
REG_NAME_HIGH( 634, Default)

// Variable overrides.
REG_NAME_HIGH( 640, Private)
REG_NAME_HIGH( 641, Const)
REG_NAME_HIGH( 642, Out)
REG_NAME_HIGH( 643, ExportObject)
REG_NAME_HIGH( 644, Net)
REG_NAME_HIGH( 645, NetSelf)
REG_NAME_HIGH( 646, Skip)
REG_NAME_HIGH( 647, Coerce)
REG_NAME_HIGH( 648, Optional)

// Class overrides.
REG_NAME_HIGH( 650, Expands)
REG_NAME_HIGH( 651, Intrinsic)
REG_NAME_HIGH( 652, HideParent)
REG_NAME_HIGH( 653, Abstract)
REG_NAME_HIGH( 654, Package)
REG_NAME_HIGH( 656, Guid)
REG_NAME_HIGH( 657, ScriptConst)

// State overrides.
REG_NAME_HIGH( 670, Auto)
REG_NAME_HIGH( 672, Ignores)

// Calling overrides.
REG_NAME_HIGH( 680, CallGlobal)
REG_NAME_HIGH( 681, CallParent)
REG_NAME_HIGH( 682, CallClass)

// Function overrides.
REG_NAME_HIGH( 690, Operator)
REG_NAME_HIGH( 691, PreOperator)
REG_NAME_HIGH( 692, PostOperator)
REG_NAME_HIGH( 693, Final)
REG_NAME_HIGH( 694, Iterator)
REG_NAME_HIGH( 695, Latent)
REG_NAME_HIGH( 696, Return)
REG_NAME_HIGH( 697, Singular)

// Variable declaration.
REG_NAME_HIGH( 710, Var)
REG_NAME_HIGH( 711, Static)
REG_NAME_HIGH( 712, Local)

// Special commands.
REG_NAME_HIGH( 720, Spawn)

// Misc.
REGISTER_NAME( 740, Tag)

/*-----------------------------------------------------------------------------
	Closing.
-----------------------------------------------------------------------------*/

#ifdef REGISTERING_ENUM
	};
	#undef REGISTER_NAME
	#undef REG_NAME_HIGH
	#undef REGISTERING_ENUM
#endif

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
