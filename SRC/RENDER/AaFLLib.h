/*=============================================================================
    aafllib.h: Unreal depth-shaded linedrawing  math support routines

    FLOAT-> fixed point conversion, rounding,
    and fast fractional extraction routines.

    Copyright 1997 Epic MegaGames, Inc. and Evolution Software
    Compiled with Visual C++ 4.2    Best viewed with Tabs=4.

    Revision history:
        * Created by Erik de Neve  March 1997

=============================================================================*/

#include <float.h>   // needed for _controlfp()

#ifndef _INC_AAFLLIB // Prevent header from being included multiple times
#define _INC_AAFLLIB

/*-----------------------------------------------------------------------------
    Declarations
-----------------------------------------------------------------------------*/

FLOAT  FloatOne = 1.0F;

DWORD  Scale24  = 0x3f800000; //  2^23 + 2^22
DWORD  Scale53  = 0x59c00000; //  2^52 + 2^51
DWORD  Scale64  = 0x5f400000; //  2^63 + 2^62
DWORD  ScaleXX;

DWORD  Round  = 0x3EFFFFFF;   // 1 lsbit closer to 0 than 0.5
DWORD  Round2 = 0x3F7FFFFF;   // doubled
FLOAT  Round05=-0.5;

// 2^( 52- 32(=fractionsize) ) = 2^20 (+ 2^ 19) for negs.
float Scale32 = 1572864.0F;

// 2^21+2^20 -> pushes out one more, making it 1:31 fixed point (or
// rather signed fixed point)
float Scale31 = (1572864.0 * 2.0);

inline void InitFlLib()
{
    if( (_controlfp (0,0) & _MCW_PC) == _PC_24 )
        appError("FP precision must be 53 or 64");
    else if( (_controlfp (0,0) & _MCW_PC) == _PC_53 )
        ScaleXX = Scale53;
    else
        ScaleXX = Scale64;
}

//
// Truncate 32bit floats to 32bit ints, with overflow checks (saturation)
//          . FPU rounding state = round to nearest or even (default)
// clip range: -2,147,483,648 ... 2,147,483,647
//
inline int TruncFL( FLOAT F )
#if ASM
#pragma warning (disable : 4035) // allow implied return value in EAX
{
    int I;
    __asm
	{
        mov     edx,dword ptr[F]    // load as int
        fld     [F]                 // load as float
        fsub    dword ptr[Round]    // optimized for positive case..
        add     edx,edx             // sign bit->carry
        jc      Sign
        fadd    dword ptr[ScaleXX]  // crowd out noninteger bits
        cmp     edx,((127+31)<<24)  // check exponent for overflow
        jbe     Store               // fadd/fstp latency masks jmp time
        mov     eax,0x7FFFFFFF      // positive saturation
        jmp     Exitt
    Sign:
        fadd    dword ptr [Round2]  // undo the fsub....
        mov     eax,0x80000000      // preemptive negative saturation
        fadd    dword ptr [ScaleXX] // crowd out noninteger bits
        cmp     edx,((127+31)<<24)  // check exponent for overflow
        ja      Exitt
    Store:
        fsub    dword ptr [ScaleXX] // remove again
        fistp   dword ptr [I]       // store in 32-bit format
        mov     eax,dword ptr [I]   // 32 bits signed integer
    Exitt:
    }
}
#pragma warning (default : 4035)
#else
{
	return (int)F;
}
#endif

//
// Assumed 0 <= F <1 float to 0:32 Fixedpoint.
//
inline DWORD Fix32( FLOAT F )
#if ASM
#pragma warning (disable : 4035) // allow implied return value in EAX.
{
    double D;               // temp 64-bit float
    __asm
	{
        fld   [F]           // Load as floating point number
        fadd  [Scale32]     // add to align the bits
        fstp  [D]           // Store as 64-bit fp double and pop
        mov   eax,dword ptr [D] // retrieve unsigned 32-bit value...
    }
}
#pragma warning (default : 4035)
#else
{
	return (DWORD)( F * (double)0xFFFFFFFF ) ;
}
#endif

inline int IntFix31(FLOAT F)   // assumed 0 <= F <1 float to 0:32 Fixedpoint
#if ASM
#pragma warning (disable : 4035) // allow implied return value in EAX
{
    double D;               // temp 64-bit float
    __asm
	{
        fld   [F]           // Load as floating point number
        fadd  [Scale31]     // add to align the bits
        fstp  [D]           // Store as 64-bit fp double and pop
        mov   eax,dword ptr [D] // retrieve SIGNED 32-bit value...
    }
}
#pragma warning (default : 4035)
#else
{
	return (INT) (F * (double) 0x7FFFFFFF ) ;
}
#endif

//
// Extract 1-fraction from a FLOAT.
//
inline FLOAT InvcFL( FLOAT F )
#if ASM
{
    FLOAT  M;
    __asm
	{
        mov     edx,dword ptr [F] //       load as int
        fld     [F]                 //       load as float
        fsub    dword ptr [Round] //       optimize esp. for positive case
        add     edx,edx          //
        jc      Sign             //
        fadd    dword ptr [ScaleXX]
        fsub    dword ptr [ScaleXX]
        fsub   [F]                 // was fsubr, fsub to get negative result..
        fadd    dword ptr [FloatOne]
        jmp     Store
    Sign:
        fadd    dword ptr [Round2];  // undo the fsub....
        fsub    dword ptr [ScaleXX];     //  scaling value  add/sub pushes out fractional bits +truncates
        fadd    dword ptr [ScaleXX];
        fsub   [F]
        fadd   dword ptr [FloatOne]
    Store:
        fstp    dword ptr [M]
    }
    return M;
}
#else
{
    return (1.0F - ( F - (float)((int)F)));
}
#endif

//
// Extract fraction from a FLOAT.
//
inline FLOAT FracFL(FLOAT F)
#if ASM
{
    FLOAT  M;
    __asm
	{
        mov     edx,dword ptr[F]  //       load as int
        fld     [F]                //       load as float
        fsub    dword ptr[Round]   //       optimize esp. for positive case
        add     edx,edx           //
        jc      Sign              //
        fadd    dword ptr[ScaleXX];
        fsub    dword ptr[ScaleXX];
        fsubr   [F]
        jmp     Store
    Sign:
        fadd    dword ptr [Round2];  // undo the fsub....
        fsub    dword ptr [ScaleXX];     //  scaling value  add/sub pushes out fractional bits +truncates
        fadd    dword ptr [ScaleXX];
        fsubr   [F]
    Store:
        fstp    dword ptr [M]
    }
    return M;
}
#else
{
    return F - (float)((int)F);
}
#endif

//
// simplest: ROUND a float to int using FIST.
//
inline int  RoundFL(FLOAT F)
#if ASM
{
    int K;
    __asm    fld     [F]     //  Load as float
    __asm    fistp   K       // Store as integer and pop
    return K;
}
#else
{
    return (int)(F+0.5);
}
#endif

/*-----------------------------------------------------------------------------
    The End
-----------------------------------------------------------------------------*/
#endif // _INC_AAFLLIB
