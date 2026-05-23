/*=============================================================================
	UnMem.cpp: Unreal memory grabbing functions

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	Code for managing a memory stack.

Motivation:
	The FMemStack class fills the need for fast, stack-oriented allocation
	of small, medium, and large chunks of memory.  The primary design goal is
	very fast average performance.  The secondary goal is to interoperate with
	the memory caching subsystem, so that FMemStack never consumes much more scarce
	memory resources than it acutally needs at any time.

Subsystem relationships:
	Each FMemStack object relies on one FMemCache object for allocating large chunks of
	memory.  The memory stack allocates large FMemCache items infrequently, and
	parcels them out to callers very frequently.

	In Unreal there is only one FMemCache object, and it divys up a huge pool of
	memory into large, LRU-purged chunks.  This enables almost all memory allocated in
	Unreal to be allocated from one place, and enables everything to be cached together.

	Callers use FMemMark objects to mark the current position of their FMemStack, and
	use its Pop() function to restore the state of the FMemStack.

Definitions:
	cache item
		One chunk of memory that has been allocated from a memory cache object.
		A cache item can either be locked, in which case the memory cache will
		not move or LRU-purge it, or it can be unlocked, in which case the
		memory cache can move or purge it at any time.
	memory cache
		An object which manages a large chunk of global memory which can be
		random-access allocated.
	memory marker
		An object which marks the current top-of-stack of a memory stack. The
		memory on the stack can later be popped by calling the memory marker's
		Pop function.
	memory stack
		An object which enables the caller to allocate pieces of memory
		in a stack-oriented fashion: Memory is allocated (pushed) via PushBytes or
		the specialized operator new, and memory is deallocated (popped) via an 
		FMemMark object.
	pop
		To return back to the memory stack all memory past a certain location.
	push
		To grab a bunch of new memory off the top of the memory stack.

Revision history:
	* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	FMemStack statics.
-----------------------------------------------------------------------------*/

BYTE FMemStack::InstanceCount = 0;

/*-----------------------------------------------------------------------------
	FMemStack implementation.
-----------------------------------------------------------------------------*/

//
// Initialize this memory stack.
//
void FMemStack::Init(FMemCache &InCache, int InMinChunkSize, int InMaxChunkSize)
{
	guard(FMemStack::AllocateStack);

	// Create a unique instance number for cache identity.
	Instance = InstanceCount++;

	// Init variables.
	GCache			= &InCache;
	MinChunkSize	= InMinChunkSize - DEFAULT_ALIGNMENT;
	MaxChunkSize	= InMaxChunkSize - DEFAULT_ALIGNMENT;

	// Make zero chunks available; the first chunk will be allocated the
	// first time memory is pushed.
	ActiveChunks	= 0;
	End				= NULL;
	Top				= NULL;

	unguard;
}

//
// Timer tick. Makes sure the memory stack is empty.
//
void FMemStack::Tick()
{
	guard(FMemStack::InitStack);

	// Make sure the stack is empty.
	checkState(ActiveChunks==0);

	unguard;
}

//
// Free this memory stack.
//
void FMemStack::Exit()
{
	guard(FMemStack::FreeStack);
	Tick();
	unguard;
}

//
// Return the amount of bytes that have been allocated from the
// cache by this memory stack.
//
int FMemStack::GetByteCount()
{
	guard(FMemStack::GetByteCount);
	int Count = 0;

	// Count all bytes in all fully exausted chunks.
	for( int i=0; i<ActiveChunks-1; i++ )
		Count += Chunks[i]->GetSize();

	// Count the used bytes in the last, partially-used chunk.
	if( ActiveChunks > 0 )
		Count += Top - Chunks[ActiveChunks-1]->GetData();

	return Count;
	unguard;
}

/*-----------------------------------------------------------------------------
	Chunk functions.
-----------------------------------------------------------------------------*/

//
// Allocate a new chunk of memory of at least MinSize size,
// and return it aligned to Align. Updates the memory stack's
// Chunks table and ActiveChunks counter.
//
BYTE *FMemStack::AllocateNewChunk( int MinSize, int Align )
{
	// Input checks.
	guard(FMemStack::AllocateNewChunk);
	checkInput( MinSize >= 0 );
	checkInput( (Align & (Align-1)) == 0 );
	checkState( ActiveChunks < MAX_CHUNKS );

	// Make cache id which is guaranteed unique for this chunk.
	DWORD CacheID     = MakeCacheID(CID_MemStackChunk,ActiveChunks,Instance);
	FCacheItem *&Item = Chunks[ActiveChunks++];

	// See if this chunk is already in the cache and it's large enough.
	Top = GCache->Get( CacheID, Item, Align );
	if( !Top || (Item->GetSize() - Align < MinSize) )
	{
		if( Top )
		{
			// The item is cached but isn't large enough to hold this, so flush it.
			Item->Unlock();
			GCache->Flush( CacheID );
		}

		// Create a new cache item for this chunk. If the cache is filled to
		// capacity with locked items, the cache handles the critical error.
		Top = GCache->Create
		(
			CacheID, Item,
			Max(MinSize, MinChunkSize) + Align,
			Align, 
			Max(MinSize, MaxChunkSize) + Align
		);
	}

	// Compute chunk end, accounting for worst-case alignment padding.
	End = Top + Item->GetSize() - Align;

	// Output checks.
	debugOutput( ((int)Top & (Align-1))==0 );
	debugOutput( Top+MinSize<=End );

	return Top;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
