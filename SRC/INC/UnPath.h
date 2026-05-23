/*=============================================================================
	UnPath.h: Path node creation and ReachSpec creations and management specific classes

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.


	Revision history:
		* Created by Steven Polge 3/97
=============================================================================*/

#ifndef _INC_UNPATH
#define _INC_UNPATH

#define DEBUGGINGPATHS  1 //1 to put path info in log
#define MAXMARKERS 3000 //bound number of turn markers
#define MAXREACHSPECS 3000 //bound number of reachspecs 
#define MAXCOMMONRADIUS 56 //max radius to consider in building paths
#define MAXCOMMONHEIGHT 60
#define COS30 0.8660254 

//Reachability flags - using bits to save space
// note that R_WALK and R_FLY are not mutually exclusive,
//but R_WALK and R_WALKJUMP are
enum EReachSpecFlags
{
	R_WALK = 1,	//walking/jumping only required (or swimming if R_WATER)
	R_FLY = 2,   //flying only required (or swimming if R_WATER) 
	R_AIR = 32,  //part of traversal is walking or flying in air
	R_WATER = 64,  //part of traversal is in water (submerged swimming) - FIXME change to R_SUBMERGED
	R_LAVA = 128,  //must touch lava if walking or submerge if R_WATER 
	R_SLIME = 256
}; 

// path node placement parameters
#define MAXWAYPOINTDIST 2.0  // max distance to a usable waypoint to avoid placing a new one after left turn
							// (ratio to collisionradius)
 
#if DEBUGGINGPATHS
    static inline void CDECL DebugPrint(const char * Message, ...)
    {
        static int Count = 0;
        if( Count <= 300 ) // To reduce total messages.
        {
            char Text[300];
            va_list ArgumentList;
            va_start(ArgumentList,Message);
            vsprintf(Text,Message,ArgumentList);
            va_end(ArgumentList);
            debugf( LOG_Info, Text );
        }
    }
    static inline void CDECL DebugVector(const char *Text, const FVector & Vector )
    {
        char VectorText[100];
        char Message[100];
        sprintf( VectorText, "[%4.4f,%4.4f,%4.4f]", Vector.X, Vector.Y, Vector.Z );
        sprintf( Message, Text, VectorText );
        DebugPrint(Message);
		DebugPrint(VectorText);
    }
	static inline void CDECL DebugFloat(const char *Text, const float & val )
    {
        char VectorText[100];
        char Message[100];
        sprintf( VectorText, "[%4.4f]", val);
        sprintf( Message, Text, VectorText );
        DebugPrint(Message);
		DebugPrint(VectorText);
    }
	static inline void CDECL DebugInt(const char *Text, const int & val )
    {
        char VectorText[100];
        char Message[100];
        sprintf( VectorText, "[%6d]", val);
        sprintf( Message, Text, VectorText );
        DebugPrint(Message);
		DebugPrint(VectorText);
    }
#else
    static inline void CDECL DebugPrint(const char * Message, ...)
    {
    }
     static inline void CDECL DebugVector(const char *Text, const FVector & Vector )
    {
    }
	static inline void CDECL DebugFloat(const char *Text, const float & val )
    {
    }
	static inline void CDECL DebugInt(const char *Text, const int & val )
    {
    }
   
#endif

class FPathMarker
{
public:
	FVector Location;
	FVector Direction;
	DWORD visible:1;
	DWORD marked:1;
	DWORD bigvisible:1;
	DWORD beacon:1;
	DWORD leftTurn:1;
	DWORD permanent:1;
	DWORD stair:1;
	DWORD routable:1;
	FLOAT radius;
	FLOAT budget;

	inline void initialize(const FVector &spot, const FVector &dir, int mrk, const int beac, int left)
	{
		Location = spot;
		Direction = dir;
		marked = mrk;
		bigvisible = 0;
		beacon = beac;
		permanent = 0;
		stair = 0;
		leftTurn = left;
	}

	inline int removable()
	{
		int result =  !marked && !beacon && !leftTurn && !permanent;
		return result;
	}

private:

};

class UNENGINE_API FPathBuilder
{
public:
	
	int buildPaths (ULevel *ownerLevel, int optimization);
	int removePaths (ULevel *ownerLevel);
	int showPaths (ULevel *ownerLevel);
	int hidePaths (ULevel *ownerLevel);
	void definePaths (ULevel *ownerLevel);
	void undefinePaths (ULevel *ownerLevel);

private:
	FPathMarker *pathMarkers;
	ULevel * Level; //owning level - FIXME - can use GLevel
	APawn * Scout;
	INDEX	numMarkers;
	FLOAT humanRadius;
	int optlevel;

	void changeScoutExtent(FLOAT Radius, FLOAT Height);
	int createPaths (int optimization);
	void	newPath(FVector spot);
	void getScout();
	INDEX addMarker();
	void createPathsFrom(FVector start);
	void checkObstructionFrom(FPathMarker *marker);
	int checkmergeSpot(const FVector &spot, FPathMarker *path1, FPathMarker *path2);
	void premergePath(INDEX iMarker);
	void mergePath(INDEX iMarker);
	void adjustPath(INDEX iMarker);
	void exploreWall(const FVector &moveDirection);
	inline int walkToward(const FVector &Destination, FLOAT Movesize);
	void followWall(FVector currentDirection);
	int checkLeft(FVector &leftDirection, FVector &currentDirection);
	int checkLeftPassage(FVector &currentDirection);
	int outReachable(FVector start, FVector destination);
	int fullyReachable(FVector start, FVector destination);
	int needPath(const FVector &start);
	int sawNewLeft(const FVector &start);
	int oneWaypointTo(const FVector &upstreamSpot);
	void markLeftReachable(const FVector &start);
	void markReachable(const FVector &start);
	int markReachableFromTwo(FPathMarker *path1, FPathMarker *path2);
	int tryPathThrough(FPathMarker *Waypoint, const FVector &Destination, FLOAT budget);
	int findPathTo(const FVector &Destination);
	int angleNearThirty(FVector dir);
	void nearestThirtyAngle (FVector &currentDirection);
	void addReachSpecs(AActor * start);
};

class UNENGINE_API FReachSpec
{

public:
	FLOAT distance; 
	AActor *Goal; //ultimate goal (used for routes)
	AActor *Start;
	AActor *End; //actor at endpoint of this path (next waypoint or goal)

	int supports (APawn * pawn);
	FReachSpec operator+ (const FReachSpec &Spec) const;
	int defineFor (AActor * begin, AActor * dest, APawn * Scout);
	int operator<= (const FReachSpec &Spec);
	int operator== (const FReachSpec &Spec);

	void Init()
	{
		guard(FReachSpec::Init);
		// Init everything here.
		Goal = Start = End = NULL;
		distance = CollisionRadius = CollisionHeight = jumpZ = dropZ = 0.0;
		reachFlags = 0;
		unguard;
	};

	friend FArchive& operator<< (FArchive &Ar, FReachSpec &ReachSpec )
	{
		guard(FReachSpec<<);
		// Serialize everything here.
		Ar << ReachSpec.distance << ReachSpec.Goal << ReachSpec.Start << ReachSpec.End;
		Ar << ReachSpec.CollisionRadius << ReachSpec.CollisionHeight;
		Ar << ReachSpec.jumpZ << ReachSpec.dropZ << ReachSpec.reachFlags;
		return Ar;
		unguard;
	};

//private:
	FLOAT CollisionRadius; 
    FLOAT CollisionHeight; 
	FLOAT jumpZ;
	FLOAT dropZ;
	DWORD reachFlags; //see defined bits above

	void changeScoutExtent(APawn * Scout, FLOAT Radius, FLOAT Height);
	int findBestReachable(FVector &Start, FVector &Destination, APawn * Scout);
};

//
// A routing table object contains a list of all reachspecs in a level.
//
class UNENGINE_API UReachSpecs : public UDatabase
{
	DECLARE_DB_CLASS(UReachSpecs,UDatabase,FReachSpec,NAME_ReachSpecs,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Variables.
	//(Stick custom variables here, if any), i.e. int Whatever;

	// Constructors.
	UReachSpecs(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}

	// UObject interface.
	void SerializeHeader(FArchive& Ar)
	{
		guard(UReachSpecs::SerializeHeader);
		UDatabase::SerializeHeader(Ar);
		// Serialize any custom variables here, i.e. Ar << Whatever;
		unguard;
	}

	// UReachSpecs interface.
	//(Stick custom functions here, if any)
};

#endif // _INC_UNPATH

