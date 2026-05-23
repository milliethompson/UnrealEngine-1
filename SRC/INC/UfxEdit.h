/*
===========================================================================
FILENAME:     UfxEdit.h
DESCRIPTION:  Primary include file for UfxEdit.dll and calling applications
AUTHOR:       Ammon Campbell
COPYRIGHT:    (C) Copyright 1996 Epic MegaGames, Inc.
NOTICE:       This source code contains trade secrets and/or proprietary
              information of Epic MegaGames, Inc., and may not be disclosed
              without prior written consent.
TOOLS:        Microsoft Visual C++ version 4.0
FORMAT:       100 characters per line, 8 characters per tabstop
===========================================================================
*/

#ifndef _INC_UFXEDIT_H
#define _INC_UFXEDIT_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************** CONSTANTS *****************************/

	/* NONE */

/******************************** TYPES *********************************/

	/* NONE */

/******************************** FUNCTIONS *****************************/

int
//#ifdef COMPILE_UFXEDIT
__declspec(dllexport)
//#else
//__declspec(dllimport)
//#endif /* COMPILE_UFXEDIT */
UfxEdit(void *UfxIn, void **UfxOut, long *UfxSize);

#ifdef __cplusplus
};
#endif /* __cplusplus */
#endif /* _INC_UFXEDIT_H */

/*
===========================================================================
End UfxEdit.h
===========================================================================
*/
