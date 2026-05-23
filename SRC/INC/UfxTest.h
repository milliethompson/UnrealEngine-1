/*
===========================================================================
FILENAME:     UfxTest.h
DESCRIPTION:  Primary include file for UfxTest.dll and calling applications
AUTHOR:       Ammon Campbell
COPYRIGHT:    (C) Copyright 1996-1997 Epic MegaGames, Inc.
NOTICE:       This source code contains trade secrets and/or proprietary
              information of Epic MegaGames, Inc., and may not be disclosed
              without prior written consent.
TOOLS:        Microsoft Visual C++ version 4.0
FORMAT:       100 characters per line, 8 characters per tabstop
===========================================================================
*/

#ifndef _INC_UFXTEST_H
#define _INC_UFXTEST_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************** CONSTANTS *****************************/

	/* NONE */

/******************************** TYPES *********************************/

	/* NONE */

/******************************** FUNCTIONS *****************************/

int
//#ifdef COMPILE_UFXTEST
__declspec(dllexport)
//#else
//__declspec(dllimport)
//#endif /* COMPILE_UFXTEST */
UfxTest(void *UfxIn, char *UfxName, int fMusic);

#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif /* _INC_UFXTEST_H */

/*
===========================================================================
End UfxTest.h
===========================================================================
*/
