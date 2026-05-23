/*=============================================================================
	UnPort.h: All platform-specific macros and stuff to aid in porting
	between different systems.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNPORT // Prevent multiple includes
#define _INC_UNPORT

/*----------------------------------------------------------------------------
	Platforms:
		__WIN32__		Windows
		__MAC__			Macintosh
		__N64__			Nintendo 64

	Compilers:
		__MSVC__		Microsoft Visual C++ 4.0

	Byte order:
		__INTEL__		Intel byte order (otherwise non-Intel byte order)
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
	Ansi C libraries
----------------------------------------------------------------------------*/

#ifndef  _INC_STDIO
#include <stdio.h>
#endif

#ifndef  _INC_STDLIB
#include <stdlib.h>
#endif

#ifndef  _INC_STDLIB
#include <stdlib.h>
#endif

#ifndef  _INC_STRING
#include <string.h>
#endif

#ifndef  _INC_MATH
#include <math.h>
#endif

#ifndef  _INC_STDARG
#include <stdarg.h>
#endif

#ifndef  _INC_MALLOC
#include <malloc.h>
#endif

#ifndef  _INC_CTYPE
#include <ctype.h>
#endif

#ifndef _INC_TYPES
#include <sys\types.h>
#endif

/*----------------------------------------------------------------------------
	PC Windows 95/NT: Microsoft Visual C++ 4.0 or later
----------------------------------------------------------------------------*/
#if _MSC_VER

#define __MSVC__	1
#define __WIN32__	1
#define __INTEL__	1

//
// Undo any Windows defines
//
#ifdef _WINDOWS_
	#undef BYTE
	#undef CHAR
	#undef WORD
	#undef DWORD
	#undef INT
	#undef FLOAT
	#undef TRUE
	#undef FALSE
	#undef MAXBYTE
	#undef MAXWORD
	#undef MAXDWORD
	#undef MAXINT
	#undef VOID
	#undef CDECL
#endif

//
// Optimization macros (preceeded by #pragma):
//
#define DISABLE_OPTIMIZATION optimize("",off)
#define ENABLE_OPTIMIZATION  optimize("",on)

//
// Assembly code
//
#if ASM
	#define ASMVAR extern
#else
	#define ASMVAR
#endif

//
// Function type macros:
//
#define DLL_IMPORT	__declspec(dllimport)	/* Import function from DLL */
#define DLL_EXPORT  __declspec(dllexport)	/* Export function to DLL */
#define VARARGS     __cdecl					/* Functions with variable arguments */
#define CDECL	    __cdecl					/* Standard C function */

//
// API defines used so that Unreal headers work both when 
// compiling each of the particular DLL's:
//
#ifdef COMPILING_ENGINE
	#define UNREAL_API DLL_EXPORT /* Export when compiling the engine */
#else
	#define UNREAL_API DLL_IMPORT /* Import when compiling an add-on */
#endif

#ifdef COMPILING_GAME
	#define UNGAME_API DLL_EXPORT /* Export when compiling game */
#else
	#define UNGAME_API DLL_IMPORT /* Import when not compiling game */
#endif

#ifdef COMPILING_NETWORK
	#define UNNETWORK_API DLL_EXPORT /* Export when compiling network code */
#else
	#define UNNETWORK_API DLL_IMPORT /* Import when compiling network code */
#endif

#ifdef COMPILING_RENDER
	#define UNRENDER_API DLL_EXPORT /* Export when compiling rendering code */
#else
	#define UNRENDER_API DLL_IMPORT /* Import when compiling rendering code */
#endif

#ifdef COMPILING_EDITOR
	#define UNEDITOR_API DLL_EXPORT /* Export when compiling editor code */
#else
	#define UNEDITOR_API DLL_IMPORT /* Export when compiling editor code */
#endif

#ifdef _DEBUG
	#define COMPILER "Compiled with Visual C++ 4.0 Debug"
#else
	#define COMPILER "Compiled with Visual C++ 4.0"
#endif

//
// Precision timing (Thanks to Erik de Nieve)
//
#if ASM
	#pragma warning (disable : 4035) // legalize implied return value EAX
	inline unsigned int CPUTIME()
	{
		__asm
		{
			xor   eax,eax	// Required so that VC++ realizes EAX is modified
			_emit 0x0F		// RDTSC  -  Pentium+ time stamp register to EDX:EAX.
			_emit 0x31		// Use only 32 bits in EAX - even a Ghz cpu would have a 4+ sec period.
			xor   edx,edx	// Required so that VC++ realizes EDX is modified
		}
	}
	#pragma warning (default : 4035)
    enum{CPUTIME_OVERHEAD=14};  /* 14 cycles = RDTSC and SUB time */
#else
	inline unsigned int CPUTIME() {return 0;}
	enum{CPUTIME_OVERHEAD=0};
#endif

#if NO_CLOCK
	#define clock(Timer)
	#define unclock(Timer)
#else
	#define clock(Timer)   Timer-=CPUTIME();
	#define unclock(Timer) Timer+=CPUTIME()-CPUTIME_OVERHEAD;
#endif

#if NO_SLOW_CLOCK
	#define clockSlow(Timer)
	#define unclockSlow(Timer)
#else
	#define clockSlow(Timer)   Timer-=CPUTIME();
	#define unclockSlow(Timer) Timer+=CPUTIME()-CPUTIME_OVERHEAD;
#endif

//
//	Base data types
//
// Unsigned base types.
typedef unsigned	char	BYTE;			// 8-bit  unsigned
typedef unsigned	short	WORD;			// 16-bit unsigned
typedef unsigned	long	DWORD;			// 32-bit unsigned
typedef unsigned	__int64	QWORD;			// 64-bit unsigned
// Signed base types.
typedef				char	CHAR;			// 8-bit  signed
typedef signed		short	SWORD;			// 16-bit signed
typedef signed		int  	INT;			// 32-bit signed
typedef signed		int 	BOOL;			// 32-bit signed
typedef signed		__int64 SQWORD;			// 64-bit signed
// Special types.
typedef signed		int		INDEX;			// 32-bit signed
typedef	signed		int		ANGLE_TYPE;		// 32-bit signed
typedef unsigned	int		RESINDEX;		// 32-bit unsigned
// Other base types.
typedef void		VOID;					// Void
typedef float       FLOAT;					// 32-bit IEEE floating point
typedef double		DOUBLE;					// 64-bit IEEE double

//
// Unwanted VC++ level 4 warnings to disable:
//
#pragma warning(disable : 4244) /* conversion to float, possible loss of data							*/
#pragma warning(disable : 4761) /* integral size mismatch in argument; conversion supplied				*/
#pragma warning(disable : 4699) /* creating precompiled header											*/
#pragma warning(disable : 4055) /* void pointer casting													*/
#pragma warning(disable : 4152) /* function/data pointer conversion										*/
#pragma warning(disable : 4200) /* Zero-length array item at end of structure, a VC-specific extension	*/
#pragma warning(disable : 4100) /* unreferenced formal parameter										*/
#pragma warning(disable : 4514) /* unreferenced inline function has been removed						*/
#pragma warning(disable : 4201) /* nonstandard extension used : nameless struct/union					*/
#pragma warning(disable : 4710) /* inline function not expanded											*/
#pragma warning(disable : 4702) /* unreachable code in inline expanded function							*/
#pragma warning(disable : 4711) /* function selected for autmatic inlining								*/
#pragma warning(disable : 4725) /* Pentium fdiv bug														*/
#pragma warning(disable : 4127) /* Conditional expression is constant									*/
#pragma warning(disable : 4512) /* assignment operator could not be generated                           */

#ifndef _CPPUNWIND /* C++ exception handling is disabled; force guarding to be off */
	#undef NO_GUARD
	#undef NO_SLOW_GUARD
	#define NO_GUARD 1
	#define NO_SLOW_GUARD 1
#endif

#ifdef _CHAR_UNSIGNED
	#error Bad VC++ option: Characters must be signed.
#endif

#ifndef _M_IX86 /* Not compiling for x86 platform */
	#undef ASM
#endif

/*----------------------------------------------------------------------------
	Some specific mac compiler
----------------------------------------------------------------------------*/
#elif USING_SOME_SPECIFIC_MAC_COMPILER /* todo: [Lion] Stick Mac compiler options here */

#define __MAC__		1

/* 
todo: [Lion]: add in macros and typedefs similar to the above for Windows/Visual C++.
*/

/*----------------------------------------------------------------------------
	Unknown compiler
----------------------------------------------------------------------------*/
#else

#error Unknown compiler, not supported by Unreal!
#endif

/*----------------------------------------------------------------------------
	Global constants
----------------------------------------------------------------------------*/

enum {MAXBYTE		= 0xff};
enum {MAXWORD		= 0xffffU};
enum {MAXDWORD		= 0xffffffffU};
enum {MAXSBYTE		= 0x7f};
enum {MAXSWORD		= 0x7fff};
enum {MAXINT		= 0x7fffffff};
enum {INDEX_NONE	= -1};

enum EBool {FALSE=0, TRUE=1};

union RAINBOW_PTR // A union of pointers of all base types.
{
	// All pointers.
	VOID  *PtrVOID;
	BYTE  *PtrBYTE;
	WORD  *PtrWORD;
	DWORD *PtrDWORD;
	QWORD *PtrQWORD;
	FLOAT *PtrFLOAT;

	// Conversion constructors.
	RAINBOW_PTR() {}
	RAINBOW_PTR(VOID *Ptr) : PtrVOID(Ptr) {};
};

/*----------------------------------------------------------------------------
	Byte order conversion

Definitions:
	local byte order:
		The natural byte order of the machine we're compiling for. This is
		either Intel or non-Intel depending on whether __INTEL__ is defined.
	TCP/IP byte order:
		The byte order of TCP/IP network headers, always non-Intel.
	Unreal byte order:
		The byte order of stuff stored in Unreal resource files and network
		data packets, always Intel.

Notes:
	Host and ToHost toggle Unreal byte order <-> local byte order
	Net  and ToNet  toggle TCP/IP byte order <-> local byte order
	Host(x)   and Net(x)   return x converted to the proper format.
	ToHost(x) and ToNet(x) just convert x to the proper format.
----------------------------------------------------------------------------*/

// Unsigned swappers.
inline BYTE   Swap(BYTE   x) { return x;                                                              }
inline WORD   Swap(WORD   x) { return (x>>8) + ((x&0xff)<<8);                                         }
inline DWORD  Swap(DWORD  x) { return (x>>24) + ((x&0xff0000)>>8) + ((x&0xff00)<<8) + ((x&0xff)<<24); }
inline QWORD  Swap(QWORD  x) { return (QWORD)Swap((DWORD)x<<32) + (QWORD)Swap((DWORD)x>>32);          }

// Signed swappers.
inline CHAR   Swap(CHAR   x) { return x;             }
inline SWORD  Swap(SWORD  x) { return Swap((WORD)x); }
inline INT    Swap(INT    x) { return Swap((DWORD)x);}
inline SQWORD Swap(SQWORD x) { return Swap((QWORD)x);}

// Other swappers.
inline FLOAT  Swap(FLOAT   x) { *(DWORD*)&x = Swap(*(DWORD*)&x); return x;};
inline DOUBLE Swap(DOUBLE  x) { *(QWORD*)&x = Swap(*(QWORD*)&x); return x;};

// Local platform specific converters.
#ifdef	__INTEL__ /* Intel byte order */
	template<class T> T Host(T x) { return x;       }
	template<class T> T Net (T x) { return Swap(x); }
#else /* Non-Intel byte order */
	template<class T> Host(T x) { return Swap(x); }
	template<class T> Net (T x) { return x;       }
#endif

// In-place converters.
template<class T> void ToHost(T &x) { x = Host (x);    }
template<class T> void ToNet (T &x) { x = ToNet(x);    }

/*----------------------------------------------------------------------------
	The End
----------------------------------------------------------------------------*/
#endif // _INC_UNPORT
