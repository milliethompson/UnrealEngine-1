/*=============================================================================
	UnStack.h: UnrealScript execution stack definition.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNSTACK
#define _INC_UNSTACK

/*-----------------------------------------------------------------------------
	Forward declarations.
-----------------------------------------------------------------------------*/

struct FStackNodeLink;
struct FExecStack;
struct FExecStackMain;
class UClass;
class UScript;

/*-----------------------------------------------------------------------------
	Enums.
-----------------------------------------------------------------------------*/

// Grouping of an actor property.
enum EPropertyBin
{
	PROPBIN_PerFunction	= 0,		// Per-actor local data at current nesting level.
	PROPBIN_PerObject	= 1,		// Per-actor data.
	PROPBIN_PerClass	= 2,		// Per-class static data.
	PROPBIN_MAX			= 3,		// Maximum.
};

/*-----------------------------------------------------------------------------
	Stack node link.
-----------------------------------------------------------------------------*/

//
// Specifies a class and a stack node index which completely determines a
// linkage to a particular stack node in a script.  This is used for linked
// lists of stack nodes, where the linked list can contain multiple nodes
// from multiple scripts.
//
struct UNENGINE_API FStackNodeLink
{
	UClass	*Class;	// Class the stack node resides in, NULL=empty entry.
	INT		iNode;	// Node index into class's script's stack tree.

	// Constructors.
	FStackNodeLink( UClass* InClass, INT iInNode )
	:	Class	(InClass)
	,	iNode	(iInNode)
	{}
	FStackNodeLink( int )
	:	Class	(NULL)
	,	iNode	(0)
	{}
	FStackNodeLink()
	{}

	// Serializer.
	inline friend FArchive& operator<<( FArchive &Ar, FStackNodeLink &Link );

	// Accessors.
	inline class FStackNode &Node() const;
};

/*-----------------------------------------------------------------------------
	Execution stack level.
-----------------------------------------------------------------------------*/

//
// Information about script execution at one stack level.
//
struct UNENGINE_API FExecStack
{
	// The stack node we're in. This is always valid.
	// In state:    The state's stack node.
	// In no state: Stack class's stack node.
	// In function: The stack node where the function's parms are defined.
	FStackNodeLink Link;

	// The script we're executing code in. If we're in a state or no state,
	// this may be NULL to indicate we're not currently executing code.
	UScript *Script;

	// The object pointer, for optimization.
	UObject *Object;

	// Pointer to code within the script's data. NULL means we're not executing
	// code now, otherwise this is a pointer into Script's data.
	BYTE *Code;

	// Local variables.
	BYTE *Locals;

	// Constructors.
	inline FExecStack() {};
	inline FExecStack( int );
	inline FExecStack( UObject *InObject );
	inline FExecStack( UObject *InObject, const FStackNodeLink &InLink, BYTE *InLocals );
};

/*-----------------------------------------------------------------------------
	Main execution stack level for an actor.
-----------------------------------------------------------------------------*/

//
// Information about script execution at the main stack level.
// This part of an actor's script state is saveable at any time.
//
struct UNENGINE_API FExecStackMain : public FExecStack
{
	// Probe messages which are currently enabled.
	QWORD ProbeMask;

	// Virtual function cache.
	enum {NUM_CACHE_ENTRIES=4};
	struct UNENGINE_API FVfCacheEntry
	{
		// Variables.
		FName			Name;		// Name of the virtual function, NAME_None=empty.
		WORD			Staleness;	// Lower means don't replace it.
		FStackNodeLink	Link;		// Stack node where the virtual function is defined.

		// Constructor.
		FVfCacheEntry() {};
	} VfCache[NUM_CACHE_ENTRIES];

	// Empty the cache; must be called during state change.
	void InitVfCache()
	{
		for( int i=0; i<NUM_CACHE_ENTRIES; i++ )
		{
			VfCache[i].Name      = NAME_None;
			VfCache[i].Staleness = MAXWORD;
		}
	}

	// Serializer. Must properly serialize all permanent runtime contents to disk.
	friend FArchive& operator<<( FArchive& Ar, FExecStackMain &Exec );

	// Constructor.
	FExecStackMain() {};
	FExecStackMain( UObject *InObject );
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNSTACK
