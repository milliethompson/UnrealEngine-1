/*=============================================================================
	UnCache.cpp: FMemoryCache implementation.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Notes:
	You can't lock an object more than 126 times. If you do, the lock count
	overflows and you'll get a "not locked" error when unlocking.

Revision history:
	* Initially implementated by Mark Randell.
	* Rewritten by Tim Sweeney (speed, speed, speed!)
=============================================================================*/

#define CHECK_ALL /* Turn on all checks for development */

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	Init & Exit.
-----------------------------------------------------------------------------*/

//
// Init the memory cache.
//
void FMemoryCache::Init( int BytesToAllocate, int MaxItems )
{
	guard(FMemoryCache::Init);
	checkState( Initialized==0 );

	// Remember totals.
	MemTotal   = BytesToAllocate;
	ItemsTotal = MaxItems;

	// Allocate cache memory.
	CacheMemory = (BYTE *)appMallocAligned( BytesToAllocate, DEFAULT_ALIGNMENT, "CacheMemory" );

	// Allocate item tracking memory.
	UnusedItemMemory = (FCacheItem *)appMallocAligned( MaxItems*sizeof(FCacheItem), 32, "CacheItems" );

	// Build linked list of items not associated with cache memory.
	FCacheItem **PrevLink = &UnusedItems;
	for( int i=0; i<MaxItems; i++ )
	{
		*PrevLink = &UnusedItemMemory[i];
		PrevLink  = &UnusedItemMemory[i].LinearNext;
	}
	*PrevLink = NULL;

	// Mark the first item unused and associate it with entire cache memory.
	CreateNewFreeSpace
	(
		CacheMemory,
		CacheMemory + BytesToAllocate,
		NULL,
		NULL
	);

	// Init the hash table to empty since no items are used.
	for( i=0; i<HASH_COUNT; i++ )
		HashItems[i] = NULL;

	// Success.
	Initialized=1;
	CheckState();
	unguard;
}

//
// Shut down the memory cache.
//
void FMemoryCache::Exit()
{
	guard(FMemoryCache::Exit);
	CheckState();

	// Release all memory.
	appFree( UnusedItemMemory );
	appFree( CacheMemory );

	// Success.
	Initialized=0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Internal functions.
-----------------------------------------------------------------------------*/

//
// Merge a cache item and its immediate successor into one
// item, and remove the second. Returns the new merged item.
//
FMemoryCache::FCacheItem *FMemoryCache::MergeWithNext( FCacheItem *First )
{
	guard(FMemoryCache::AttemptToMerge);

	// Sanity check.
	checkInput( First != NULL );

	// Get second item.
	FCacheItem *Second = First->LinearNext;

	// Validate everything.
	debugInput( Second != NULL );
	debugInput( First->LinearNext == Second );	
	debugInput( Second->LinearPrev == First );
	debugInput( First->Data + First->Size == Second->Data );

	// Absorb the second item into the first.
	First->Size       += Second->Size;
	First->LinearNext  = Second->LinearNext;

	if( First->LinearNext != NULL )
		First->LinearNext->LinearPrev = First;

	// Stick the second item at the head of the unused list.
	Second->LinearNext = UnusedItems;
	UnusedItems        = Second;

	// Success.
	return First;
	unguard;
}

//
// Flush a cache item and return the one immediately
// following it in memory, or NULL if at end.
//
FMemoryCache::FCacheItem *FMemoryCache::FlushItem( FCacheItem *Item )
{
	guard(FMemoryCache::FlushItem);
	debugInput( Item != NULL );
	debugInput( Item->Id != 0 );

	// Flushing a locked object is a critical error.
	if( Item->Cost >= COST_INFINITE ) 
		appError( "Flushed a locked cache object ");

	// Flush this one item.
	Item->Id	= 0;
	Item->Cost	= 0;

	// If previous item is free space, merge with it.
	if( Item->LinearPrev && Item->LinearPrev->Id==0 )
		Item = MergeWithNext( Item->LinearPrev );

	// If next item is free space, merge with it.
	if( Item->LinearNext && Item->LinearNext->Id==0 )
		Item = MergeWithNext( Item );

	return Item->LinearNext;
	unguard;
}

//
// Make sure the state is valid.
//
void FMemoryCache::CheckState()
{
	guard(FMemoryCache::CheckState);

	// Make sure we're initialized.
	checkState( Initialized == 1 );

	// Make sure there's an initial item.
	checkState( CacheItems != NULL );

	// Init stats.
	INT ItemCount=0, UsedItemCount=0, WasFree=0, HashCount=0, MemoryCount=0;
	BYTE *ExpectedPointer = CacheMemory;

	// Traverse all cache items.
	for( FCacheItem *Item=CacheItems; Item; Item=Item->LinearNext )
	{
		// Validate this item.
		checkState( Item->Size > 0 );

		// Make sure this memory is where we expect.
		checkState( Item->Data == ExpectedPointer );

		// Count memory.
		MemoryCount     += Item->Size;
		ExpectedPointer += Item->Size;

		// Count items.
		ItemCount++;

		// Make sure that free items aren't contiguous.
		if( Item->Id == 0 )
			checkState( !WasFree );

		WasFree = (Item->Id == 0);

		// Verify previous link.
		if( Item != CacheItems )
		{
			checkState( Item->LinearPrev->LinearNext == Item );
		}

		// Verify next link.
		if( Item->LinearNext != NULL )
			checkState( Item->LinearNext->LinearPrev == Item );

		// If used, make sure this item is hashed exactly once.
		if( Item->Id )
		{
			UsedItemCount++;
			INT HashedCount=0;

			for( FCacheItem *HashItem=HashItems[Item->Id%HASH_COUNT]; HashItem; HashItem=HashItem->HashNext )
				HashedCount += (HashItem == Item);

			if( HashedCount != 1 )
				appErrorf("HashedCount=%i",HashedCount);
		}
	}
	checkState( ExpectedPointer == CacheMemory + MemTotal );

	// Traverse all unused items.
	for( Item=UnusedItems; Item; Item=Item->LinearNext )
		ItemCount++;

	// Make sure all items are accounted for.
	checkState( ItemCount == ItemsTotal );

	// Make sure all hashed items are used, and there are no
	// duplicate Id's.
	for( DWORD i=0; i<HASH_COUNT; i++ )
	{
		for( Item=HashItems[i]; Item; Item=Item->HashNext )
		{
			// Count this hash item.
			HashCount++;

			// Make sure this Id belongs here.
			checkState( (Item->Id % HASH_COUNT) == i );

			// Make sure this Id is not duplicated.
			for( FCacheItem *Other=Item->HashNext; Other; Other=Other->HashNext )
				checkState( Other->Id != Item->Id );
		}
	}
	checkState( HashCount == UsedItemCount );

	// Success.
	unguard;
}

//
// Create a block of free space in the cache, and link it in.
// If either of Next or Prev are free space, simply merges
// and does not create a new item.
//
void FMemoryCache::CreateNewFreeSpace
(
	BYTE		*Start, 
	BYTE		*End, 
	FCacheItem	*Prev, 
	FCacheItem	*Next 
)
{
	guard(FMemoryCache::CreateNewFreeSpace);

	// Make sure parameters are valid.
	debugInput( Start >= CacheMemory );
	debugInput( End <= (CacheMemory + MemTotal) );
	debugInput( Start < End );

	if( Prev && Prev->Id==0 )
	{
		// The previous item is free space, so merge with it.
		Prev->Size += End-Start;
	}
	else if( Next && Next->Id==0 )
	{
		// The next item is free space, so merge with it.
		Next->Size += End-Start;
		Next->Data  = Start;
	}
	else
	{
		// Make sure we can grab a new item.
		checkState( UnusedItems != NULL );

		// Grab a free space item from the list.
		FCacheItem *Item = UnusedItems;
		UnusedItems = UnusedItems->LinearNext;

		// Create the free space item.
		Item->Data			= Start;
		Item->Time			= 0;
		Item->Id			= 0;
		Item->Size			= End - Start;
		Item->Cost			= 0;
		Item->LinearNext	= Next;
		Item->LinearPrev	= Prev;
		Item->HashNext		= NULL;

		// Link it in.
		if( Prev != NULL )
			Prev->LinearNext = Item;
		else
			CacheItems = Item;

		if( Next != NULL )
			Next->LinearPrev = Item;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Flushing.
-----------------------------------------------------------------------------*/

//
// Flush the memory cache.
// If Mask=~0, flush exactly Id.
// Otherwise flush all Ids for which (TestId&Mask)==Id.
//
void FMemoryCache::Flush( DWORD Id, DWORD Mask )
{
	guard(FMemoryCache::Flush);

	if( Id == 0 )
		// Flush all items.
		Mask = 0;

	if( Mask == ~0 )
	{
		// Quickly flush a single element.
		FCacheItem **PrevLink = &HashItems[Id % HASH_COUNT];
		while( *PrevLink != NULL )
		{
			FCacheItem *Item = *PrevLink;			
			if( Item->Id == Id )
			{
				// Remove item from hash.
				*PrevLink = Item->HashNext;

				// Flush the item.
				FlushItem( Item );
				
				// Successfully flushed.
				break;
			}
			PrevLink = &Item->HashNext;
		}
	}
	else
	{
		// Slow wildcard flush of all in-memory cache items.
		FCacheItem *Item=CacheItems;
		while( Item )
		{
			if( Item->Id!=0 && (Item->Id & Mask)==(Id & Mask) )
			{
				// Remove item from hash table.
				Unhash( Item->Id );

				// Flush the item and get the next item in linear sequence.
				Item = FlushItem( Item );
			}
			else Item = Item->LinearNext;
		}
		if( Mask==0 )
		{
			// Make sure we flushed the entire cache and there is only
			// one cache item remaining.
			checkState( CacheItems!=NULL );
			checkState( CacheItems->LinearNext==NULL );
			checkState( CacheItems->Id==0 );
		}
	}
	ConditionalCheckState();
	unguard;
}

/*-----------------------------------------------------------------------------
	Creating.
-----------------------------------------------------------------------------*/

//
// Create an element in the cache.
//
// This is O(num_items_in_cache), sacrificing some speed in the
// name of better cache efficiency. However there aren't any really
// good algorithms for this kind of routine where the priorities
// change so often.
//
BYTE *FMemoryCache::Create
(
	DWORD		Id, 
	FCacheItem	*&Item, 
	INT			CreateSize, 
	INT			Alignment
)
{
	guard(FMemoryCache::Create);
	clock(CreateTime);
	checkInput( CreateSize > 0 );
	checkInput( Id != 0 );
	NumCreates++;

	// Best cost and starting element found thus far.
	INT		   BestCost   = COST_INFINITE;
	FCacheItem *BestFirst = NULL;
	FCacheItem *BestLast  = NULL;

	// Iterate through items. Find shortest contiguous sets of items
	// which contain enough space for this entry. Evaluate the sum cost
	// for each potential set, remembering the best cost.
	FCacheItem *First = CacheItems;
	INT        Cost=0, Size=0;
	for( FCacheItem *Last = CacheItems; Last; Last = Last->LinearNext )
	{
		// Add the cost and size of new Last element to our accumulator:
		Cost += Last->Cost;
		Size += Last->Size;

		// While the interval from First to Last (inclusive) contains
		// enough space for the item we're creating, consider it as a
		// candidate, and go to the next First.
		while( First && (Size + First->Data - Align(First->Data,Alignment) >= CreateSize))
		{
			// Is this the best solution so far?
			if( Cost < BestCost )
			{
				BestCost  = Cost;
				BestFirst = First;
				BestLast  = Last;
			}

			// Subtract the cost and size from the element we're passing:
			Cost -= First->Cost;
			Size -= First->Size;

			// Go to next First.
			First = First->LinearNext;
		}
	}

	// See if we found a suitable place to put the item.
	if( BestFirst == NULL )
	{
		// Critical error: the item can't fit in the cache.
		appErrorf( "Create failed: %i, %i", Size, Alignment );
	}

	// Merge all items from Start to End into one bigger item,
	// while unhashing them all.
	while( BestLast != BestFirst )
	{
		if( BestLast->Id != 0 ) Unhash( BestLast->Id );
		BestLast = MergeWithNext( BestLast->LinearPrev );
	}
	if( BestFirst->Id != 0 ) Unhash( BestFirst->Id );

	// Now we have a big free memory block from BestFirst->Data to 
	// BestFirst->Data + BestFirst->Size.
	BYTE *Result = Align(BestFirst->Data,Alignment);
	debugLogic( Result + CreateSize <= BestFirst->Data + BestFirst->Size );
	debugLogic( ((int)Result & (Alignment-1)) == 0 );

	// Claim BestFirst for the block we're creating.
	BestFirst->Time = Time;
	BestFirst->Id   = Id;
	BestFirst->Cost = CreateSize + COST_INFINITE;

	// Hash it.
	FCacheItem **HashPtr	= &HashItems[Id % HASH_COUNT];
	BestFirst->HashNext		= *HashPtr;
	*HashPtr				= BestFirst;

	// Create free space past the end of the newly allocated block.
	if( UnusedItems && (Result + CreateSize < BestFirst->Data + BestFirst->Size))
	{
		CreateNewFreeSpace
		(
			Result + CreateSize, 
			BestFirst->Data + BestFirst->Size,
			BestFirst,
			BestFirst->LinearNext
		);

		// Chop the new free space off this item.
		BestFirst->Size = Result + CreateSize - BestFirst->Data;
	}

	// Create free space before the beginning of the newly allocated.
	if( UnusedItems && (Result - BestFirst->Data) >= IGNORE_SIZE )
	{
		CreateNewFreeSpace
		(
			BestFirst->Data, 
			Result,
			BestFirst->LinearPrev,
			BestFirst
		);

		// Chop the new free space off this item.
		BestFirst->Size -= Result - BestFirst->Data;
		BestFirst->Data  = Result;
	}

	// Set the resulting Item.
	Item = BestFirst;

	unclock(CreateTime);
	ConditionalCheckState();
	return Result;
	unguard;
}

/*-----------------------------------------------------------------------------
	Tick.
-----------------------------------------------------------------------------*/

//
// Handle time passing.
//
void FMemoryCache::Tick()
{
	guard(FMemoryCache::Tick);
	CheckState();

	// Init memory stats.
	MemFresh = MemStale = 0;
	ItemsFresh = ItemsStale = 0;

	// Check each item.
	for( FCacheItem *Item = CacheItems; Item; Item=Item->LinearNext )
	{
		if( Item->Id != 0 )
		{
			// Make sure no items are locked.
			if( Item->Cost >= COST_INFINITE )
			{
				appErrorf( "Cache item %08X still locked in call to Tick", Item->Id );
			}
			else if( Time - Item->Time > 1)
			{
				// Exponentially decrease Cost for stale items.
				Item->Cost -= Item->Cost >> 5;
				MemStale   += Item->Size;
				ItemsStale ++;
			}
			else if( Time - Item->Time == 1)
			{
				// Cut Cost by 1/4th as soon as it first becomes stale.
				Item->Cost = Item->Cost >> 2;
			}
			else
			{
				MemFresh   += Item->Size;
				ItemsFresh ++;
			}
		}
	}
	// Update the cache's time.
	Time++;
	unguard;
}

/*-----------------------------------------------------------------------------
	Status.
-----------------------------------------------------------------------------*/

//
// Write useful human readable performance stats into a string.
//
void FMemoryCache::Status( char *StatusText )
{
	// Display stats.
	sprintf( StatusText, "Gets=%04i Crts=%03i Msec=%05.1f Fresh=%03i%% Stale=%03i%% Items=%03i",
		NumGets,
		NumCreates,
		GApp->CpuToMilliseconds(CreateTime),
		(MemFresh*100)/MemTotal,
		(MemStale*100)/MemTotal,
		ItemsFresh
		);
	// Reinitialize time-variant stats.
	NumGets = NumCreates = CreateTime = 0;
}

//
// Draw the cache layout to a frame buffer.
//
void FMemoryCache::DrawCache( BYTE *Dest, int XR, int YR, int ColorBytes )
{
	if( !XR || YR<8)
		return;

	FLOAT Factor = (FLOAT)MemTotal / (XR * 8);
	FLOAT Current = 0.0;

	// Draw all cache items.
	for( FCacheItem *Item = CacheItems; Item; Item = Item->LinearNext )
	{
		FLOAT Next = (FLOAT) ColorBytes * (Item->Data + Item->Size - CacheMemory) / Factor;

		BYTE B;		
		if		( Item->Id == 0 )			B = 255;
		else if ((Time - Item->Time) <= 1 )	B = 32;
		else								B = 150;

		while( Current < Next )
		{
			*Dest++ = B;
			Current+=1.0;
		}
	}
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
