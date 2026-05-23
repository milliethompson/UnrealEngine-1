/*==========================================================================
FILENAME:     UnSound.h
DESCRIPTION:  Declarations of the "USound" class and related routines.
NOTICE:       Copyright 1996 Epic MegaGames, Inc. This software is a
              trade secret.
TOOLS:        Compiled with Visual C++ 4.0, Calling method=__fastcall
FORMAT:       8 characters per tabstop, 100 characters per line.
HISTORY:
  When      Who                 What
  ----      ---                 ----
  ??/??/96  Tim Sweeney         Created stubs for this module.
  04/18/96  Ammon R. Campbell   Misc. hacks started.
  05/03/96  Ammon R. Campbell   More or less working now.
==========================================================================*/

#ifndef _INC_UNSOUND /* Prevent header from being included multiple times */
#define _INC_UNSOUND

/*
** USound:
** Class used to store information about each sound effect object.
** This is derived from the UObject class.
*/
class UNENGINE_API USound : public UObject
{
	DECLARE_CLASS(USound,UObject,NAME_Sound,NAME_UnEngine)

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

	/* Name of the sound effect family to which this sound belongs. */
	FName FamilyName;

	/* Sound ID of this sound when it is registered with the sound drivers. */
	INT SoundID;

	/*
	** Object function overriden from UObject.
	*/
	void InitHeader();
	int  QuerySize();
	const char *Import(const char *Buffer, const char *BufferEnd, const char *FileType);
	char *Export(char *Buffer,const char *FileType,int Indent);

	/*
	** Serialization.
	*/
	void SerializeData(FArchive &Ar);
	void SerializeHeader(FArchive &Ar)
	{
		guard(USound::SerializeHeader);
		UObject::SerializeHeader(Ar);
		Ar << DataSize << FamilyName;
		unguard;
	}
};

#endif // _INC_UNSOUND

/*
==========================================================================
End UnSound.h
==========================================================================
*/
