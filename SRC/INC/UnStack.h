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

struct FStackNodePtr;
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
struct UNENGINE_API FStackNodePtr
{
	UClass	*Class;	// Class the stack node resides in, NULL=empty entry.
	INT		iNode;	// Node index into class's script's stack tree.

	// Constructors.
	FStackNodePtr( UClass* InClass, INT iInNode )
	:	Class	(InClass)
	,	iNode	(iInNode)
	{}
	FStackNodePtr()
	{}

	// Serializer.
	inline friend FArchive& operator<<( FArchive &Ar, FStackNodePtr &Link );

	// Accessors.
	inline class FStackNode* operator->() const;
	inline class FStackNode& operator*() const;
};

/*-----------------------------------------------------------------------------
	IteratorList base.
-----------------------------------------------------------------------------*/

//
// Base class for UnrealScript iterator lists.
//
struct FIteratorList
{
	// Variables.
	FIteratorList* Next;

	// Functions.
	FIteratorList()
	{}
	FIteratorList( FIteratorList* InNext )
	:	Next( InNext )
	{}
	FIteratorList* GetNext()
	{ return (FIteratorList*)Next; }
};

/*-----------------------------------------------------------------------------
	Execution stack level.
-----------------------------------------------------------------------------*/

//
// Information about script execution at one stack level.
//
struct UNENGINE_API FExecStack
{	
	FStackNodePtr  Link;   // Points to stack node of the class, state, or function we're in.
	UObject*       Object; // Pointer to the owning object.
	UScript*       Script; // The script we're executing code in. NULL if not executing code.
	BYTE*          Code;   // Instruction pointer within script's data.
	BYTE*          Locals; // Base address of local variables.

	// Constructors.
	inline FExecStack() {};
	inline FExecStack( int );
	inline FExecStack( UObject* InObject );
	inline FExecStack( UObject* InObject, const FStackNodePtr InLink, BYTE* InLocals );
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
	// Variables;
	QWORD ProbeMask; // Enabled probe messages.

	// Serializer. Must properly serialize all permanent runtime contents to disk.
	friend FArchive& operator<<( FArchive& Ar, FExecStackMain &Exec );

	// Constructor.
	FExecStackMain() {};
	FExecStackMain( UObject *InObject );

	// Init for a new state.
	void InitForState();
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNSTACK
