/*===============================================================================
UnCache.h: Unreal fast memory cache support.

Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:

	A memory based object cache. This manages a big chunk of memory which
	we use for storing temporary objects.

	Objects are identified by an identifier (DWORD Id). The caller is  responsible 
	for assigning Id's such that no two cache objects have the same Id pair.

	Call Create() to create a new object in the cache. Upon creation, the
	object is locked. You must later unlock it by calling the corresponding
	FCacheItem::Unlock.

	Call Get() to get an already-created object. If the object hasn't been
	created, or it's no longer in the cache, the result is NULL.  The object
	is locked and you must later unlock it. Locked items are never flushed.

	Call Tick() once per game tick to refresh the cache.

	Unlocked items are occasionally flushed to make room for new stuff.

Critical failure conditions:
	1.	Trying to create an item bigger than the cache can hold.
	2.	Trying to create an item when that item is too large to fit
		between the other items that are locked.
	3.	Flushing a locked element.

This is optimized for ultra-fast Get's, decently fast Create's.

Revision history:
	* Initially implementated by Mark Randell.
	* Rewritten by Tim Sweeney (speed, speed, speed!)
===============================================================================*/

#ifndef _INC_UNCACHE
#define _INC_UNCACHE

/*-----------------------------------------------------------------------------
	FMemoryCache.
-----------------------------------------------------------------------------*/

class UNREAL_API FMemoryCache
{
public:
	//////////////////////
	// Public interface //
	//////////////////////

	// Constants.
	enum {DEFAULT_ALIGNMENT=8};		// Default boundary to align stuff on.

	// Information about a cache item.
	class UNREAL_API FCacheItem
	{
	public:
		// Unlock the cache item.
		void Unlock()
		{
			if( this == NULL )
				appError( "Unlock: Null cache item" );

			if( Cost < COST_INFINITE )
				appErrorf( "Unlock: Item %08X is not locked",Id );
			
			Cost -= COST_INFINITE;
		}
		// Accessors.
		DWORD GetId()
		{
			return Id;
		}
		BYTE *GetData()
		{
			return Data;
		}
		BYTE GetSize()
		{
			return Size;
		}
	private:
		// Private variables: Note 32-byte alignment.
		BYTE		*Data;			// Pointer to the item's data.
		DWORD		Id;				// This item's cache id, 0=unused.
		INT			Time;			// Last Get() time.
		INT			Size;			// Data size.
		INT			Cost;			// Cost to flush this item.
		FCacheItem	*LinearNext;	// Next cache item in linear list, or NULL if last.
		FCacheItem	*LinearPrev;	// Previous cache item in linear list, or NULL if first.
		FCacheItem	*HashNext;		// Next cache item in hash table, or NULL if last.

		// Cost is defined as:
		// If the item is free(Id==0), 0;
		// If the item is locked, COST_INFINITE;
		// If the item is fresh (CacheTime-Time<=1), Size;
		// If the item is old, exponentially loses 1/32nd of its value per frame.
	friend class FMemoryCache;
	};

	// Constructor.
	FMemoryCache() {Initialized=0;}

	// Init and exit.
    void Init( int BytesToAllocate, int MaxItems );
	void Exit();

	// Flushing.
	void Flush( DWORD Id=0, DWORD Mask=~0 );

	// Creating elements.
	BYTE *Create( DWORD Id, FCacheItem *&Item, INT CreateSize, INT Alignment=DEFAULT_ALIGNMENT );

	// Time passing.
	void Tick();

	// Make sure the state is valid.
	void CheckState();

	// Get an element.
	BYTE *Get( DWORD Id, FCacheItem *&Item, INT Alignment=DEFAULT_ALIGNMENT )
	{	
		guardSlow(FMemoryCache::Get);
		NumGets++;
		for(FCacheItem *HashItem=HashItems[Id%HASH_COUNT]; HashItem; HashItem=HashItem->HashNext )
		{
			if( HashItem->Id == Id )
			{
				// Set the item, lock it, and return its data.
				Item            = HashItem;
				HashItem->Time  = Time;
				HashItem->Cost += COST_INFINITE;
				return Align( HashItem->Data, Alignment );
			}
		}
		return NULL;
		unguardSlow;
	}

	// Status.
	void Status( char *Msg );
	void DrawCache( BYTE *Dest, int XR, int YR, int ColorBytes );

private:
	////////////////////////////
	// Private implementation //
	////////////////////////////

	// Constants.
	enum {COST_INFINITE=0x1000000}; // An infinite removal cost.
	enum {HASH_COUNT=11731};		// Hash table size.
	enum {IGNORE_SIZE=128};			// Don't bother tracking items less than this size.

	// Whether we're initialized.
	INT Initialized;

	// Current time.
	INT Time;

	// Stats.
	INT NumGets,NumCreates,CreateTime;
	INT ItemsFresh,ItemsStale,ItemsTotal;
	INT MemFresh,MemStale,MemTotal;

	// Linked list of item associated with cache memory, linked via LinearNext and
	// LinearPrev order of memory.
	FCacheItem *CacheItems;
	void CreateNewFreeSpace( BYTE *Start,BYTE *End,FCacheItem *Prev,FCacheItem *Next );

	// First item in unused item list (these items are not associated with cache
	// memory). Linked via LinearNext in FIFO order.
	FCacheItem *UnusedItems;

	// The hash table.
	FCacheItem *HashItems[HASH_COUNT];
	void Unhash( DWORD Id )
	{
		for( FCacheItem **PrevLink=&HashItems[Id%HASH_COUNT]; *PrevLink; PrevLink=&(*PrevLink)->HashNext )
		{
			if( (*PrevLink)->Id == Id )
			{
				*PrevLink = (*PrevLink)->HashNext;
				return;
			}
		}
		appError( "Unhashed item" );
	}

	// Merging items.
	FCacheItem *MergeWithNext( FCacheItem *First );

	// Flushing individual items.
	FCacheItem *FlushItem( FCacheItem *Item );

	// Original memory allocations.
	FCacheItem *UnusedItemMemory;
	BYTE       *CacheMemory;

	// State checking.
	void ConditionalCheckState()
	{
#if defined(CHECK_ALL) || defined(_DEBUG)
		CheckState();
#endif
	}

friend class FCacheItem;
};

// Global scope:
typedef FMemoryCache::FCacheItem FCacheItem;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNCACHE
