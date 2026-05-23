/*=============================================================================
	UnScript.h: Script resource class

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNSCRIPT
#define _INC_UNSCRIPT

/*-----------------------------------------------------------------------------
	UScript.
-----------------------------------------------------------------------------*/

//
// A tokenized, interpretable script resource referenced by a class.
// First element of the script's data is the root of the script's stack tree.
// Everything else in the script's data can be determined from the stack tree.
//
class UNREAL_API UScript : public UResource
{
	RESOURCE_CLASS(UScript,BYTE,RES_Script)

	// Variables.
	UClass *Class;		// Class the script applies to.
	INT Length;			// Total number of bytes in script.
	INT NumStackTree;	// Number of entries in script's stack tree.
	INT CodeOffset;		// Offset of start-of-code into script's data.
	INT	Pad[5];

	// Resource functions.
	void Register				(FResourceType *Type);
	void InitHeader				();
	void InitData				();
	int  QuerySize				();
	int  QueryMinSize			();
	const char *Import			(const char *Buffer, const char *BufferEnd,const char *FileType);
	char *Export				(char *Buffer,const char *FileType,int Indent);
	void QueryHeaderReferences	(FResourceCallback &Callback);
	void QueryDataReferences	(FResourceCallback &Callback);
	void Flip					();

	// Custom functions.
	char *Decompile				(char *Buffer,int ParentLinks);
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNSCRIPT
