/*=============================================================================
	UnMem.cpp: Unreal memory grabbing functions

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	FMemPool implementation (not including inlines).
-----------------------------------------------------------------------------*/

//
// Allocate this memory pool's buffer.
//
void FMemPool::AllocatePool(int NewSize, const char *Name)
{
	guard(FMemPool::AllocatePool);

	char Descr[80]; sprintf(Descr,"MemPool(%s)",Descr);
	Start = appMallocArray(NewSize,BYTE,Descr);
	Size  = NewSize;
	InitPool();
	
	unguard;
}

//
// Initialize this memory pool to its empty state.
//
void FMemPool::InitPool()
{
	guard(FMemPool::InitPool);
	Top = Start;
	End = Start + Size;
	unguard;
}

//
// Free this memory pool's buffer.
//
void FMemPool::FreePool()
{
	guard(FMemPool::FreePool);

	appFree(Start);
	Start = NULL;
	Top   = NULL;
	End   = NULL;
	Size  = 0;

	unguard;
}

/*-----------------------------------------------------------------------------
	Memory pool errors.
-----------------------------------------------------------------------------*/

void FMemPool::OverflowError()
{
	appError("FMemPool::OverflowError");
}

void FMemPool::AccessError()
{
	appError("FMemPool::AccessError");
}

void FMemPool::SizeError()
{
	appError("FMemPool::SizeError");
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
