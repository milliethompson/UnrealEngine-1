/*=============================================================================
	APawn.h: Class functions residing in the APawn class.

	Note: This file is included during the APawn class
	definition when Root.h is compiled so that the builtin class functions
	of the pawn classes can be customized here.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Steven Polge
=============================================================================*/

#ifdef _INC_APAWN
#error APawn.h must only be included once!
#endif
#define _INC_APAWN

/*-----------------------------------------------------------------------------
	Class functions
-----------------------------------------------------------------------------*/

//class UNENGINE_API APawn {
public:

	// Functions.
	void inputCopyFrom( const struct PPlayerTick &Tick );

	inline int walkToward(const FVector &Destination, FLOAT Movesize);
	void setMoveTimer(FLOAT MoveSize);
	int moveToward(const FVector &Dest);
	int rotateToward(const FVector &FocalPoint);
	void testWalking(const FVector &DesiredMove);
	void physWalking(FLOAT deltaTime);
	void physFlying(FLOAT deltaTime);
	void ShowSelf();
	DWORD LineOfSightTo(AActor *Other);
	void CheckEnemyVisible();
	void physicsRotation(FLOAT deltaTime, FVector OldVelocity);
	int pointReachable(FVector aPoint);
	int actorReachable(AActor *Other);
	int Reachable(FVector aPoint);
	int walkReachable(FVector Dest);
	int jumpReachable(FVector Dest);
	int flyReachable(FVector Dest);
	void jumpLanding(FVector testvel, FVector &Landing, int moveActor = 0);
	int walkMove(const FVector &Delta, FLOAT threshold = 4.1, int bAdjust = 1);
	int FindBestJump(FVector Dest, FVector vel, FVector &Landing, int moveActor = 0); 
	int FindJumpUp(FVector Dest, FVector vel, FVector &Landing, int moveActor = 0); 
	int findPathTo(FVector Dest, INT maxpaths, FLOAT maxweight, 
					AActor *&bestPath, AActor *EndAnchorPath = NULL);
//	int findOnePathTo(FVector Dest, AActor *&bestPath); 
	int findTwoPathTo(FVector Dest, AActor *&bestPath, FLOAT maxweight);
	int findPathToward(AActor *goal, INT maxpaths, 
						FLOAT maxweight, AActor *&bestPath);
	int findRandomDest(INT maxpaths, FLOAT maxweight, AActor *&bestPath);
	int TraverseFrom(AActor *startnode);
	int bestPathFrom(AActor *startnode, float &bestweight, float currentweight, AActor *&bestPath, int maxdepth);
	void clearPaths();
//};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
