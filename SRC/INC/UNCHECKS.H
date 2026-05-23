/*==============================================================================
UnChecks.h: Unified debugging and assertion code.

Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Summary:

	Macros surrounding code in order to generate call stack upon crash:

		guard(FunctionName);
		unguard;
		unguardf((PrintfStyleExpression)); // Note requirement of double parenthesis.

		guardSlow(FunctionName);
		unguardSlow;
		unguardfSlow((PrintfStyleExpression));

	Compile-time options:
		DO_GUARD		(On by default ) Enables try/catch exception handling in guard.
		DP_SLOW_GUARD	(Off by default) Enables try/catch exception handling in guardSlow.
		CHECK_ALL		(Off by default) Perform all checks in release versions.

    Checking functions:

        void checkInput  (BOOL Condition);
        void checkLogic  (BOOL Condition);
        void checkOutput (BOOL Condition);
        void checkState  (BOOL Condition);
        void checkVital  (BOOL Condition, const char * Message);
		void checkFailed (const char * Message);

    The following debug checks are similar to the corresponding
    simple checks, but they are active only if _DEBUG is defined.
    Use the debug checks for checks you refuse to have in the
    release code:

        void debugInput  (BOOL Condition);
        void debugLogic  (BOOL Condition);
        void debugOutput (BOOL Condition);
        void debugState  (BOOL Condition);

	Other useful functions and macros:

		LOGIC(expr;)
			If CHECK_LOGIC (or CHECK_ALL) is defined, expr is emitted into
			the code stream. Otherwise, expr is ignored.  This is useful for
			performing minor work required for logic check which doesn't fit
			cleanly into a checkLogic(Condition) expression.
		BOOL IS_NAN(float)
			Returns whether the parameter is not a number, 0 otherwise.

Description:

    This header declares and defines objects to ease the addition of
    checking code to your programs. assert() is a standard way of 
    doing checks. The disadvantages of assert() are:
       - Assertions are usually turned off for release builds, but you
         might want to keep some important assertions. Other assertions
         are too expensive to keep. There is no standard way to 
         distinguish between these two and handle them differently.
       - You usually don't have much control over how a failed assertion
         is handled. You might want a project-specific behaviour for all
         failed assertions. You might want a different behaviour in a debug 
         build than the behaviour in a release build.
       - Assertions can be costly when left on (in particular, a lot of
         strings are created - the larger the assertion expressions, the
         larger the string space needed).
    The intent of this package is to provide a simple checking interface 
    whose implementation details can be tailored to a specific project.

    Some things are just too costly to check in released code.
    We introduce the notions of a simple check and a debug check, 
    and we make an arbitrary distinction between the two.

Definitions:
    debug check:
        A check which does not belong in release code (for whatever reason
        you decide, such as prohibitive cost).
    input check:
        A check for valid argments passed to a called function. Sometimes
        called the pre-conditions for a function. Examples:
          - A list index function checks that the index is not too large.
	guard:
		A mechanism based on structured exception handling that enables a
		program to display a human-readable call history when it fails.
    logic check:
         A localized check of the logic within a function or the correctness 
         of a function's local state. The expectation is that such checks 
         are normally removed from the released code.
    output check:
        A check for correct output from a called function. Sometimes
        called the post-conditions for a function. Examples:
          - A sort() function checks that it does indeed sort an input array.
    simple check:
        A check which would be reasonable to leave in the release code.
    state check:
        A check of the persistent state of an object or of the relationships
        between a persistent set of objects.
    vital check:
        An important check for critical, unrecoverable errors that could
        happen at run-time. Such checks are expected to be left in the 
        release code, although their behaviour in that code might differ
        from their behaviour in the debug code.
        Examples:
          - A function malloc's a vital object, and checks to make sure
            there was enough free store to hold the object.

Notes:
  1. A vital check is the only check for which text may be given.
     The other kinds of checks, when they fail, provide implicit information 
     such as the text of the failed condition, the source file name, and
     the source file line number.
  2. Because the checkXYZ and debugXYZ macros may compile to nothing, it is vital
     that you don't rely on side-effects of their expressions.
	 Example to avoid:
	      - debugInput(X-- > 0); // Logic error: X won't be incremented in the release version!

Requirements for this package:
  1. There should only be a few kinds of assertions - 5 or less. Any more
     would be too complicated and hard to remember.
  2. The interface to the user of this package should be the same regardless
     of the actual implementation.
  3. When turned off, checks should cause no overhead.
  4. Functions, variables, and macros must not conflict with existing C/C++
     library, Windows, MFC, or GCC names (such as assert, ASSERT, etc)..

Revision history:
    * 03/22/96, Created by Mark.
	* 10/30/96, Extended and integrated with guard mechanism by Tim.
==============================================================================*/

#ifndef _INC_CHECKS
#define _INC_CHECKS

#ifndef  _INC_UNPLATFM
#include "UnPlatfm.h"
#endif

/*-----------------------------------------------------------------------------
	Guarding
-----------------------------------------------------------------------------*/

//
// guard/unguardf/unguard macros:
// For showing calling stack when errors occur in major functions.
// Meant to be enabled in release builds.
//
#if defined(_DEBUG) || !DO_GUARD /* No guard mechanism */
	#define guard(func)			{static const char __FUNC_NAME__[]=#func;
	#define unguard				}
	#define unguardf(msg)		}
#else /* Enable guard mechansim */
	#define guard(func)			{static const char __FUNC_NAME__[]=#func; try{
	#define unguard				}catch(char *Err){throw Err;}catch(...){if(GApp->Debugging) GApp->DebugBreak(); GApp->GuardMessagef("%s",__FUNC_NAME__); throw;}}
	#define unguardf(msg)		}catch(char *Err){throw Err;}catch(...){if(GApp->Debugging) GApp->DebugBreak(); GApp->GuardMessagef("%s",__FUNC_NAME__); GApp->GuardMessagef msg; throw;}} /* Must enclose msg in double parenthesis */
#endif

//
// guardSlow/unguardfSlow/unguardSlow macros:
// For showing calling stack when errors occur in performance-critical functions.
// Meant to be disabled in release builds.
//
#if defined(_DEBUG) || !DO_GUARD || !DO_SLOW_GUARD /* No slow guard mechanism */
	#define guardSlow(func)		static const char __FUNC_NAME__[]=#func;{
	#define unguardfSlow(msg)	}
	#define unguardSlow			}
	#define unguardfSlow(msg)	}
#else /* Enable slow guard mechansim */
	#define guardSlow(func)		guard(func)
	#define unguardSlow			unguard
	#define unguardfSlow(msg)	unguardf(msg)
#endif

/*-----------------------------------------------------------------------------
	Check options.
-----------------------------------------------------------------------------*/

//
// Which types of checks to perform.
// checkXYZ is performed in all versions in which the corresponding CHECK_XYZ is defined to 1.
// debugXYZ is performed only in _DEBUG builds and only when the above criteria are met.
//
#if defined(_DEBUG) || CHECK_ALL
    // Debug code: Use 1 to enable a check, or 0 to disable it.
    // You may use these macros in your own code: if(CHECK_INPUT) ...
    #define CHECK_INPUT  1
    #define CHECK_LOGIC  1
    #define CHECK_OUTPUT 1
    #define CHECK_STATE  1
#else
    // Release code: Use 1 to enable a check, or 0 to disable it.
    // You may use these macros in your own code: if(CHECK_INPUT) ...
    #define CHECK_INPUT  1
    #define CHECK_LOGIC  1
    #define CHECK_OUTPUT 1
    #define CHECK_STATE  1
#endif

//
// Define the LOGIC macro.
//
#if CHECK_LOGIC
	#define LOGIC(expr) expr
#else
	#define LOGIC(expr)
#endif

//
// Pick one of these - whichever is best for you.
// The more information provided to checkFailed, the more useful
// the information, but the larger the code.
//
#define CHECK_FAILED(Condition,SourceFileName,LineNumber) checkFailed( #Condition, SourceFileName, LineNumber )
//#define CHECK_FAILED(Condition,SourceFileName,LineNumber) checkFailed( __FILE__, __LINE__ )
//#define CHECK_FAILED(Condition,SourceFileName,LineNumber) checkFailed( #Condition )
//#define CHECK_FAILED(Condition,SourceFileName,LineNumber) checkFailed( __LINE__ )
//#define CHECK_FAILED(Condition,SourceFileName,LineNumber) checkFailed()

/*-----------------------------------------------------------------------------
	checkXYZ and debugXYZ macros.
-----------------------------------------------------------------------------*/

//
// Define all checkXYZ macros.
//
#if CHECK_INPUT
    #define checkInput(Condition) (void) ( (Condition) || ( CHECK_FAILED( (Condition), __FILE__, __LINE__ ), 0 ) )
#else
    #define checkInput(Condition) 
#endif

#if CHECK_LOGIC
    #define checkLogic(Condition) (void) ( (Condition) || ( CHECK_FAILED( (Condition), __FILE__, __LINE__ ), 0 ) )
#else
    #define checkLogic(Condition) 
#endif

#if CHECK_OUTPUT
    #define checkOutput(Condition) (void) ( (Condition) || ( CHECK_FAILED( (Condition), __FILE__, __LINE__ ), 0 ) )
#else
    #define checkOutput(Condition) 
#endif

#if CHECK_STATE
    #define checkState(Condition) (void) ( (Condition) || ( CHECK_FAILED( (Condition), __FILE__, __LINE__ ), 0 ) )
#else
    #define checkState(Condition) 
#endif

//
// Define all debugXYZ macros.
//
#if defined(_DEBUG) || CHECK_ALL
    // Debug checks are the same as simple checks when debugging:
    #define debugInput(Condition)  checkInput(Condition)  
    #define debugLogic(Condition)  checkLogic(Condition)  
    #define debugOutput(Condition) checkOutput(Condition)  
    #define debugState(Condition)  checkState(Condition)  
#else
    // Debug checks are always turned off for release code:
    #define debugInput(Condition)
    #define debugLogic(Condition)
    #define debugOutput(Condition)
    #define debugState(Condition)
#endif

/*-----------------------------------------------------------------------------
	checkVital and checkFailed
-----------------------------------------------------------------------------*/

//
// Define the checkVital macro.
//

// The vital check is special - it might have a message constructed by the caller.
#define checkVital(Condition,Message) (void) ( (Condition) || ( checkFailed( Message ), 0 ) )

//
// checkFailed functions.  These are referenced by the checkXYZ and debugXYZ macros, but
// you may call them directly when you want to cause a critical error.
//
void UNENGINE_API checkFailed( const char * Message, const char * SourceFileName, int LineNumber );
void UNENGINE_API checkFailed( const char * SourceFileName, int LineNumber );
void UNENGINE_API checkFailed( const char * Message );
void UNENGINE_API checkFailed( int LineNumber );
void UNENGINE_API checkFailed( );

/*-----------------------------------------------------------------------------
	Static constants.
-----------------------------------------------------------------------------*/

//
// The checkXYZ and debugXYZ macros make use of __FUNC_NAME__ to be the name of the current 
// function that is compiling; the specific function name is defined as a static const
// char[] when you use the guard() macro.  Here we define a static global __FUNC_NAME__
// variable which is used when checkXYZ or debugXYZ are called outside the scope of
// a guard() block.
//
static const char __FUNC_NAME__[] = "Unknown";

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_CHECKS
