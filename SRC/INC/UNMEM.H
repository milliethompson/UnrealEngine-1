/*=============================================================================
	UnMem.h: FMemStack class, ultra-fast temporary memory allocation

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNMEM
#define _INC_UNMEM

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

// Enums for specifying memory allocation type.
enum EMemZeroed {MEM_Zeroed=1};
enum EMemOned   {MEM_Oned  =1};

/*-----------------------------------------------------------------------------
	FMemStack.
-----------------------------------------------------------------------------*/

// Simple linear-allocation memory stack.
// Items are allocated via PushBytes() or the specialized operator new()s.
// Items are freed en masse by using FMemMark to Pop() them.
class UNENGINE_API FMemStack
{
public:
	// Get bytes.
	inline BYTE *PushBytes( int AllocSize, int Align )
	{
		// Debug checks.
		guardSlow(FMemStack::PushBytes);
		debugInput(AllocSize>=0);
		debugInput((Align&(Align-1))==0);
		debugInput(Top<=End);

		// Try to get memory from the current chunk.
		BYTE *Result = (BYTE *)(((int)Top+(Align-1))&~(Align-1));
		Top = Result + AllocSize;

		// Make sure we didn't overflow.
		if( Top > End )
		{
			// We'd pass the end of the current chunk, so allocate a new one.
			AllocateNewChunk( AllocSize, Align );
			Result = (BYTE *)(((int)Top+(Align-1))&~(Align-1));
			Top    = Result + AllocSize;
		}
		return Result;
		unguardSlow;
	}

	// Main functions.
	void Init(FMemCache &Cache, int MinChunkSize, int MaxChunkSize);
	void Exit();
	void Tick();
	int  GetByteCount();

	// Friends.
	friend class FMemMark;
	friend inline void *operator new( size_t Size, FMemStack &Mem, int Count=1, int Align=DEFAULT_ALIGNMENT );
	friend inline void *operator new( size_t Size, FMemStack &Mem, EMemZeroed Tag, int Count=1, int Align=DEFAULT_ALIGNMENT );
	friend inline void *operator new( size_t Size, FMemStack &Mem, EMemOned Tag, int Count=1, int Align=DEFAULT_ALIGNMENT );

private:
	// Constants.
	enum {MAX_CHUNKS=256};

	// Variables.
	FMemCache		*GCache;		// The memory cache we use for chunk allocation.
	BYTE			*Top;			// Top of current chunk (Top<=End).
	BYTE			*End;			// End of current chunk.
	INT				MinChunkSize;	// Minimum chunk size to allocate.
	INT				MaxChunkSize;	// Maximum chunk size to allocate.
	INT				ActiveChunks;	// Number of chunks in use.
	BYTE			Instance;		// Unique instance number of this memory cache.
	static BYTE		InstanceCount;	// Number of memory stacks allocated.

	// Chunk cache items.
	FCacheItem		*Chunks[MAX_CHUNKS]; // Only chunks 0..ActiveChunks-1 are valid.

	// Functions.
	BYTE *AllocateNewChunk(int MinSize, int Align);
};

/*-----------------------------------------------------------------------------
	FMemStack operator new's.
-----------------------------------------------------------------------------*/

// Operator new for typesafe memory stack allocation.
inline void *operator new( size_t Size, FMemStack &Mem, int Count, int Align )
{
	// Get uninitialized memory.
	guardSlow(FMemStack::New1);
	BYTE *Result = Mem.PushBytes( Size*Count, Align );
	return Result;
	unguardSlow;
}
inline void *operator new( size_t Size, FMemStack &Mem, EMemZeroed Tag, int Count, int Align )
{
	// Get zero-filled memory.
	guardSlow(FMemStack::New2);
	BYTE *Result = Mem.PushBytes( Size*Count, Align );
	memset( Result, 0, Size*Count );
	return Result;
	unguardSlow;
}
inline void *operator new( size_t Size, FMemStack &Mem, EMemOned Tag, int Count, int Align )
{
	// Get one-filled memory.
	guardSlow(FMemStack::New3);
	BYTE *Result = Mem.PushBytes( Size*Count, Align );
	memset( Result, 255, Size*Count );
	return Result;
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	FMemMark.
-----------------------------------------------------------------------------*/

// FMemMark marks a top-of-stack position in the memory stack.
// When the marker is constructed or initialized with a particular memory 
// stack, it saves the stack's current position. When marker is popped, it
// pops all items that were added to the stack subsequent to initialization.
class UNENGINE_API FMemMark
{
public:
	// Constructors.
	FMemMark() {}
	FMemMark(FMemStack &InMem)
	{
		guardSlow(FMemMark::FMemMark);
		Mem          = &InMem;
		Top          = Mem->Top;
		End          = Mem->End;
		ActiveChunks = Mem->ActiveChunks;
		unguardSlow;
	}
	// FMemMark interface.
	void Push(FMemStack &InMem)
	{
		// Save the state of the memory stack.
		guardSlow(FMemMark::Push);
		Mem          = &InMem;
		Top          = Mem->Top;
		End          = Mem->End;
		ActiveChunks = Mem->ActiveChunks;
		unguardSlow;
	}
	void Pop()
	{
		// Check state.
		guardSlow(FMemMark::Pop);
		checkState(ActiveChunks<=Mem->ActiveChunks);

		// Unlock any new chunks that were allocated.
		while( Mem->ActiveChunks > ActiveChunks )
			Mem->Chunks[--Mem->ActiveChunks]->Unlock();

		// Restore the memory stack's state.
		Mem->Top = Top;
		Mem->End = End;
		unguardSlow;
	}

private:
	// Implementation variables.
	FMemStack	*Mem;			// The memory stack this marker is saving.
	BYTE		*Top;			// The memory stack's saved top pointer.
	BYTE		*End;			// The memory stack's saved end pointer.
	INT			ActiveChunks;	// Number of active chunks at save position.
};

/*-----------------------------------------------------------------------------
	FMemAutoMark.
-----------------------------------------------------------------------------*/

// A memory stack marker which automatically pops itself when destroyed.
class UNENGINE_API FMemAutoMark : public FMemMark
{
public:
	// Constructor.
	FMemAutoMark( FMemStack &InMem )
	:	FMemMark( InMem )
	{}
	// Destructor.
	~FMemAutoMark()
	{Pop();}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNMEM
