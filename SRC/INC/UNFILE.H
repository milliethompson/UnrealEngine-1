/*=============================================================================
	UnFile.h: General-purpose file utilities.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.
=============================================================================*/

#ifndef _INC_UNFILE // Prevent header from being included multiple times
#define _INC_UNFILE

/*-----------------------------------------------------------------------------
	Platform independent convenience functions.
-----------------------------------------------------------------------------*/

//
// File functions.
//
UNREAL_API long	fsize 				(const char *fname);
UNREAL_API char *fgetdir			(char *dir);
UNREAL_API int 	fsetdir				(const char *dir);
UNREAL_API int 	fmkdir				(const char *dir);
UNREAL_API int 	fdelete 			(const char *fname);
UNREAL_API int 	frename 			(const char *oldname, const char *newname);
UNREAL_API const char *fext			(const char *fname);
UNREAL_API const char *spc			(int num);

//
// Other functions.
//
UNREAL_API char *mystrncpy			(char *dest, const char *src, int maxlen); // Safe strncpy
UNREAL_API char *mystrncat			(char *dest, const char *src, int maxlen); // Safe strcat
UNREAL_API const char *mystrstr		(const char *str, const char *find); // Find string in string, case insensitive, non-alnum lead-in
UNREAL_API unsigned long strhash	(const char *Data, int NumKeys); // Hash value computer
UNREAL_API unsigned long strcrc		(const unsigned char *Data, int Length); // CRC32 computer
UNREAL_API void mymemset			(void *Dest,char c,int Count);
UNREAL_API int mymemeq				(void *P1, void *P2, int n);

/*-----------------------------------------------------------------------------
	General templates.
-----------------------------------------------------------------------------*/

//
// Swap two variables.
//
template<class T> inline void Swap(T& A, T& B)
{
	const T Temp = A;
	A = B;
	B = Temp;
}

/*-----------------------------------------------------------------------------
	Sorting templates.
-----------------------------------------------------------------------------*/

//
// Quicksort an array of items.  Implemented as a template so that the compare
// function may be inlined.
//
// Based on code in the Microsoft Visual C++ 4.0 runtime library source.
//
// Array points to an array of Num elements of class T.
// Compare points to a comparison function that returns <0, 0, or >0 based on the
//    relative ordering of elements A and B (for example, like strcmp).
// SortCutoff is an optional cutoff value below which elements are insertion-sorted.
//
// Requires:
//    Class T must have a valid assignment operator.
//    Class T must have a valid comparison friend function Compare(const T&A, const T&B)
//
// Example:
//	struct MyLong {
//		long Value;
//		friend inline int Compare(const CLong &A, const CLong &B)
//			{return (A.Value>B.Value)-(B.Value<A.Value);};
//	};
//	void f()
//	{
//		MyLong ArrayToSort[16]; // Initialize this somewhere...
//		myqsort(ArrayToSort,16); // Now the array is sorted
//	}
//
template<class T> inline void QSort(T *Array, int Num, unsigned SortCutoff=8)
{
	guard("QSort");

    T *Lo, *Hi;					// Ends of sub-array currently sorting.
    T *Mid;						// Points to middle of subarray.
    T *LoGuy, *HiGuy;			// Traveling pointers for partition step.
    T *LoStk[30], *HiStk[30];	// Stack for saving sub array.
    int StkPtr;					// Index into stack.
    unsigned Size;				// Size of the sub-array.

    // The number of stack entries required is no more than
    // 1 + log2(size), so 30 is sufficient for any array.
    if (Num < 2)
		// Nothing to do.
        return;

	// Initialize stack.
    StkPtr = 0;

	// Initialize limits.
    Lo = Array;
    Hi = Array + Num - 1;

    // this entry point is for pseudo-recursion calling: setting
    // lo and hi and jumping to here is like recursion, but stkptr is
    // preserved, locals aren't, so we preserve stuff on the stack
Recurse:

    Size = Hi - Lo + 1; // Elements to sort

    // below a certain size, it is faster to use an O(n^2) sorting method.
	if (Size <= SortCutoff)
	{
		// Note: in assertions below, i and j are alway inside original bound of
		// array to sort.
		T* NewHi = Hi;
		while (NewHi > Lo)
		{
			// A[i] <= A[j] for i <= j, j > hi.
			T* Max = Lo;
			for (T* P=Lo+1; P<=NewHi; P++)
			{
				// A[i] <= A[max] for lo <= i < p.
				if (Compare(*P, *Max) > 0)
				{
					Max = P;
				}
				// A[i] <= A[max] for lo <= i <= p.
			}
			// A[i] <= A[max] for lo <= i <= hi.

			Swap(*Max, *NewHi);
			// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi.

			NewHi--;
			// A[i] <= A[j] for i <= j, j > hi, loop top condition established.
		}
		// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
		// so array is sorted.
	}
    else
	{
		// First we pick a partititioning element.  The efficiency of the
        // algorithm demands that we find one that is approximately the
        // median of the values, but also that we select one fast.  Using
        // the first one produces bad performace if the array is already
        // sorted, so we use the middle one, which would require a very
        // wierdly arranged array for worst case performance.  Testing shows
        // that a median-of-three algorithm does not, in general, increase
        // performance.

        Mid = Lo + (Size / 2); // Find middle element.
        Swap(*Mid, *Lo);     // Swap it to beginning of array.

        // We now wish to partition the array into three pieces, one
        // consisiting of elements <= partition element, one of elements
        // equal to the parition element, and one of element >= to it.  This
        // is done below; comments indicate conditions established at every
        // step.

        LoGuy = Lo;
        HiGuy = Hi + 1;

        // Note that higuy decreases and loguy increases on every iteration,
        // so loop must terminate.

        for (;;)
		{
            // lo <= loguy < hi, lo < higuy <= hi + 1.
            // A[i] <= A[lo] for lo <= i <= loguy.
            // A[i] >= A[lo] for higuy <= i <= hi.

            do
			{
                LoGuy++;
            }
			while (LoGuy <= Hi && Compare(*LoGuy, *Lo) <= 0);

            // lo < loguy <= hi+1, A[i] <= A[lo] for lo <= i < loguy,
            // either loguy > hi or A[loguy] > A[lo].

            do
			{
				HiGuy--;
            } 
			while (HiGuy > Lo && Compare(*HiGuy, *Lo) >= 0);

            // lo-1 <= higuy <= hi, A[i] >= A[lo] for higuy < i <= hi,
            // either higuy <= lo or A[higuy] < A[lo].

            if (HiGuy < LoGuy)
                break;

            // if loguy > hi or higuy <= lo, then we would have exited, so
            // A[loguy] > A[lo], A[higuy] < A[lo],
            // loguy < hi, highy > lo.

            Swap(*LoGuy, *HiGuy);

            // A[loguy] < A[lo], A[higuy] > A[lo]; so condition at top
            // of loop is re-established.
        }

        //     A[i] >= A[lo] for higuy < i <= hi,
        //     A[i] <= A[lo] for lo <= i < loguy,
        //     higuy < loguy, lo <= higuy <= hi.

        //     A[i] >= A[lo] for loguy <= i <= hi,
        //     A[i] <= A[lo] for lo <= i <= higuy,
        //     A[i] = A[lo] for higuy < i < loguy.

        Swap(*Lo, *HiGuy); // put partition element in place

        // A[i] >= A[higuy] for loguy <= i <= hi,
        // A[i] <= A[higuy] for lo <= i < higuy
        // A[i] = A[lo] for higuy <= i < loguy    */

        // We've finished the partition, now we want to sort the subarrays
        // [lo, higuy-1] and [loguy, hi].
        // We do the smaller one first to minimize stack usage.
        // We only sort arrays of length 2 or more.

        if ( HiGuy - 1 - Lo >= Hi - LoGuy )
		{
            if (Lo + 1 < HiGuy) 
			{
				// Save big recursion for later.
                LoStk[StkPtr] = Lo;
                HiStk[StkPtr] = HiGuy - 1;
                StkPtr++;
            }
            if (LoGuy < Hi)
			{
				// Do small recursion.
                Lo = LoGuy;
                goto Recurse;
            }
        }
        else
		{
            if (LoGuy < Hi)
			{
				// Save big recursion for later.
                LoStk[StkPtr] = LoGuy;
                HiStk[StkPtr] = Hi;
                StkPtr++;
            }
            if (Lo + 1 < HiGuy)
			{
				// Do small recursion.
                Hi = HiGuy - 1;
                goto Recurse;
            }
        }
    }

    // We have sorted the array, except for any pending sorts on the stack.
    // Check if there are any, and do them.
    if (--StkPtr >= 0)
	{
		// Pop subarray from stack.
        Lo = LoStk[StkPtr];
        Hi = HiStk[StkPtr];
        goto Recurse;
    }

	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNFILE
