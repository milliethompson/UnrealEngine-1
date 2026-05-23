/*=============================================================================
	UnPawn.h: Class functions residing in the APawn class.

	Note: This file is included during the APawnBase class
	definition when Root.h is compiled so that the builtin class functions
	of the pawn classes can be customized here.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Steven Polge
=============================================================================*/

#ifdef _INC_UNPAWN
#error UnPawn must only be included once!
#endif

/*-----------------------------------------------------------------------------
	Class functions
-----------------------------------------------------------------------------*/

//class UNREAL_API APawnBase {
public:

	// Functions.
	inline int walkToward(const FVector &Destination, FLOAT Movesize);
	int moveToward(const FVector &Dest);
	int rotateToward(const FVector &FocalPoint);
	int walkReachable(FVector Destination);
	inline int flyToward(const FVector &Destination, FLOAT Movesize);
	int flyReachable(FVector Destination);
	int walkMove(FVector Delta, int bugprint);
	int flyMove(FVector Delta);
	void physWalking(FLOAT deltaTime);
	void physFlying(FLOAT deltaTime);
	void LookForPlayer();
	DWORD LineOfSightTo(AActor *Other);
	void CheckEnemyVisible();


//};

/*-----------------------------------------------------------------------------
	The End
-----------------------------------------------------------------------------*/
