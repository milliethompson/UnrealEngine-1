/*=============================================================================
	UnRandom.cpp: Random number generation code.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	Maintains a set of random numbers from 0.0-1.0 updated per tick.  One 
	set of random numbers is discontinuous and is regenerated every tick,
	and the other set is continuous and is smoothly interpolated every tick.

Notes:
*	Subsystem is designed for single use.

Revision history:
    10-21-96, Tim: Rewritten from the ground up.
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*------------------------------------------------------------------------------------
	FGlobalRandoms definition
------------------------------------------------------------------------------------*/

class FGlobalRandoms : public FGlobalRandomsBase
{
private:

	// Functions from FGlobalRandomsBase.
	void Init();
	void Exit();
	void Tick(FLOAT TimeSeconds);

	// Variables.
	static FLOAT RandomDeltas[N_RANDS]; // Deltas used to update Randoms.
	static int LastTicks;
	static int RefCount;
};

//
// FGlobalRandomsBase statics.
//
FLOAT FGlobalRandomsBase::RandomBases[N_RANDS];
FLOAT FGlobalRandomsBase::Randoms[N_RANDS];

//
// FGlobalRandoms statics.
//
FLOAT FGlobalRandoms::RandomDeltas[N_RANDS];
INT   FGlobalRandoms::LastTicks=0;
INT   FGlobalRandoms::RefCount=0;

/*------------------------------------------------------------------------------------
	FGlobalRandoms Implementation
------------------------------------------------------------------------------------*/

//
// Initialize random number subsytem.
//
void FGlobalRandoms::Init()
{
	guard(FGlobalRandoms::Init);
	if( ++RefCount != 0 ) appError("Subsystem is single use only");
	unguard;
}

//
// Shut down random number subsytem.
//
void FGlobalRandoms::Exit()
{
	guard(FGlobalRandoms::Exit);
	if ( RefCount-- != 0 ) appError("Reference counting error");
	unguard;
}

//
// Update random number tables.
//
void FGlobalRandoms::Tick( FLOAT TimeSeconds )
{
	guard(FGlobalRandoms::Tick);
	int ServerTicks = TimeSeconds * 35.0;

	// optimize: This code would benefit greatly from a fast random number generator
	// that generates floating point randoms from 0.0 to 1.0. This could be done with
	// an integer random generator and ieee-specific code like:
	//
	// float temp=1.0, *result;
	// int randombits = (some completely random bit pattern);
	// *(int*)&result = (*(int*)&temp & 0xff800000) | (randombits & 0x007fffff);

	// Regenerate all random bases for temporally discontinuous random numbers.
	for( int i=0; i<N_RANDS; i++ )
		RandomBases[i] = frand();
	
	// Update range of random numbers.
	if( ((ServerTicks - LastTicks) * RAND_CYCLE < N_RANDS) && LastTicks )
	{
		// No wraparound.
		int r = (ServerTicks * RAND_CYCLE) & RAND_MASK;
		int m = (LastTicks   * RAND_CYCLE) & RAND_MASK;
		for( int n=m; n!=r; n = (n+1) & RAND_MASK )
		{
			RandomDeltas[n] = (RandomBases[n] - Randoms[n]) / (N_RANDS/RAND_CYCLE);
		}
		
		// Update all other randoms.
		FLOAT f = (ServerTicks - LastTicks) & RAND_MASK;
		do
		{
			Randoms[r] += RandomDeltas[r] * f;
			r = (r+1) & RAND_MASK;
		}
		while( r != m );
	}
	else
	{
		// Wraparound or first time.
		for( int i=0; i<N_RANDS; i++)
		{
			Randoms     [i] = frand();
			RandomDeltas[i] = (RandomBases[i] - Randoms[i]) / (N_RANDS/RAND_CYCLE);
		}
	}

	// Remember tick count.
	LastTicks = ServerTicks;

	unguard;
}

/*------------------------------------------------------------------------------------
	Random subsystem instantiation
------------------------------------------------------------------------------------*/

FGlobalRandoms			GRandomsInstance;
FGlobalRandomsBase		*GRandoms = &GRandomsInstance;

/*------------------------------------------------------------------------------------
	The End
------------------------------------------------------------------------------------*/
