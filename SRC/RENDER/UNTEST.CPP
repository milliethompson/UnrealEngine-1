/*=============================================================================
	UnTest.cpp: File for testing optimizations. Check out the VC++ 4.0 
				generated /Unreal/Src/Listing/UnTest.asm file.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*
class FX{};
extern void *myget();

void* operator new(size_t size, FX &X,int Count=1,int Align=8)
{
	return myget();
}

void* doit()
{
	FX X;
	return new(X,4)FVector;
}
*/

// (Wow!)
FMemStack Mem;
void *Temp()
{
	return new(Mem)FVector;
}

void Xyzzy(BYTE *Dest)
{
	static int c;
	*Dest *= 0.80 + 0.20 * GRandoms->RandomBase(c++);
}

//
// The End
//
