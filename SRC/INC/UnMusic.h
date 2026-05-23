/*==========================================================================
FILENAME:     UnMusic.h
DESCRIPTION:  Declarations of the "UMusic" class and related routines.
NOTICE:       Copyright 1996 Epic MegaGames, Inc. This software is a
              trade secret.
TOOLS:        Compiled with Visual C++ 4.0, Calling method=__fastcall
FORMAT:       8 characters per tabstop, 100 characters per line.
==========================================================================*/

#ifndef _INC_UNMUSIC /* Prevent header from being included multiple times */
#define _INC_UNMUSIC

/*
** UMusic:
** Class used to store information about each song object.
** This is derived from the UObject class.
*/
class UNENGINE_API UMusic : public UObject
{
	DECLARE_CLASS(UMusic,UObject,NAME_Music,NAME_UnEngine)

	/*
	** Identification.
	*/
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Swappable | CLASS_ScriptWritable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	/*
	** Variables.
	*/

	/* Size of WAV data in bytes. */
	INT	DataSize;

	/*
	** Object function overriden from UObject.
	*/
	void InitHeader();
	int QuerySize();
	const char *Import(const char *Buffer, const char *BufferEnd, const char *FileType);
	char *Export(char *Buffer,const char *FileType,int Indent);

	/*
	** Serialization.
	*/
	void SerializeData(FArchive &Ar);
	void SerializeHeader(FArchive &Ar)
	{
		guard(UMusic::SerializeHeader);
		UObject::SerializeHeader(Ar);
		Ar << DataSize;
		unguard;
	}
};

#endif // _INC_UNMUSIC

/*
==========================================================================
End UnMusic.h
==========================================================================
*/
