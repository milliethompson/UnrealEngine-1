/*=============================================================================
FILENAME:     UnMusic.cpp
DESCRIPTION:  Implementation of the "UMusic" class and related routines.
NOTICE:       Copyright 1996 Epic MegaGames, Inc.  This software is a trade
              secret.
TOOLS:        Compiled with Visual C++ 4.0, Calling method=__fastcall
FORMAT:       8 characters per tabstop, 100 characters per line.
=============================================================================*/

// ADBG:  Define this symbol for extra debug output.
// #define ADBG

/********************************* INCLUDES ********************************/

#pragma warning (disable : 4201) /* nonstandard extension used : nameless struct/union */
#include "Unreal.h"		// Unreal engine includes
#include "UnAudio.h"		/* Declarations for using UnAudio.lib */

/********************************* HEADERS *********************************/

#if 0
static void musicQuerySongForLink(void);
#endif

/********************************* CONSTANTS *******************************/

#if 0
/* Maximum number of results during query. */
#define MAX_RESULTS	1024
#endif

/********************************* VARIABLES *******************************/

#if 0
/* Used during song queries. */
static DWORD GCurResult = 0;
static DWORD GNumResults = 0;
static UMusic **GTempList = NULL;
#endif

/********************************* FUNCTIONS *******************************/

/*************************************************************************
                     Implementation of UMusic class
*************************************************************************/

/*
** UMusic::InitHeader:
** Initialize the header of the object.
*/
void
UMusic::InitHeader(void)
{
	guard(UMusic::InitHeader);
#ifdef ADBG
	debugf(LOG_Audio, "UMusic::InitHeader()\n");
#endif /* ADBG */

	/* Vital: Must call parent handler! */
	UObject::InitHeader();

	/* Zero all the variables to known states. */
	DataSize = 0;

	unguard;
} /* End InitHeader() */

/*
** UMusic::QuerySize:
** Determine size of object.
**
** Parameters:
**	NONE
**
** Returns:
**	Size of object in bytes.
*/
int
UMusic::QuerySize(void)
{
	guard(UMusic::QuerySize);
#ifdef ADBG
	debugf(LOG_Audio, "UMusic::QuerySize()\n");
#endif /* ADBG */

	return DataSize;

	unguard;
} /* End UMusic::QuerySize() */

/*
** UMusic::Import:
** Import a song from a buffer in memory.
**
** Parameters:
**	Name		Description
**	----		-----------
**	Buffer		Pointer to data containing object to
**			be imported.
**	BufferEnd	Pointer to first byte following object
**			to be imported.
**	FileType	Extension of file, i.e. "S3M".
**
** Returns:
**	Value	Meaning
**	-----	-------
**	NULL	Error occured.
**	other	Same as BufferEnd if successful.
*/
const char *
UMusic::Import(const char *Buffer, const char *BufferEnd,const char *FileType)
{
	guard(UMusic::Import);

#ifdef ADBG
	debugf(LOG_Audio, "UMusic::Import(Buffer, BufferEnd, FileType)\n");
#endif /* ADBG */

	/* Determine size of object in bytes. */
	DataSize = (int)(BufferEnd - Buffer);

	/* Make space for object. */
	Realloc();

	/* Copy import buffer to our data buffer. */
	Lock(LOCK_ReadWrite);
	memcpy(GetData(), Buffer, DataSize);
	Unlock(LOCK_ReadWrite);

	return BufferEnd;
	unguard;
} /* End UMusic::Import() */

/*
** UMusic::Export:
** Export a song to a buffer in memory.
**
** Parameters:
**	Name		Description
**	----		-----------
**	Buffer		Pointer to data containing object to
**			be imported.
**	FileType	Type of file, i.e. WAV
**	Ident		Not applicable (text files only).
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Same as BufferEnd if successful.
*/
char *
UMusic::Export(char *Buffer, const char *FileType, int Indent)
{
	guard(UMusic::Export);

#ifdef ADBG
	debugf(LOG_Audio, "UMusic::Export(Buffer, BufferEnd, FileType)\n");
#endif /* ADBG */

	/* Copy from our data buffer to export buffer. */
	Lock(LOCK_ReadWrite);
	memcpy(Buffer, GetData(), DataSize);
	Unlock(LOCK_ReadWrite);

	return Buffer + DataSize;

	unguard;
} /* End UMusic::Export() */


/*
** UMusic::SerializeData:
**   Serialize the music's data.
*/
void
UMusic::SerializeData( FArchive &Ar )
{
	guard(UMusic::SerializeData);
#ifdef ADBG
	debugf(LOG_Debug, "USound::SerializeData(&Callback)\n");
#endif /* ADBG */

	Lock(LOCK_ReadWrite);
	Ar.Serialize(GetData(), DataSize);
	Unlock(LOCK_ReadWrite);

	unguard;
} /* End UMusic::SerializeData() */


IMPLEMENT_CLASS(UMusic);

/*************************************************************************
                      Unreal editor hooks for music
*************************************************************************/

#if 0

/*---------------------------------------------------------------------------
	Music command link
---------------------------------------------------------------------------*/

/*
** MusicCmdLine:
** Receives music-related commands from the Unreal editor.
** Tim's comment:
**   See the huge function in UnEdSrv.cpp and the parsers in UnParams.cpp
**   for an example of how to do this.
**
** Parameters:
**	Name	Description
**	----	-----------
**	Str	Remainder of command line after first token is removed.
**
** Returns:
**	NONE
*/
void
UNENGINE_API MusicCmdLine(const char *Str)
{
	guard(MusicCmdLine);

	debugf(LOG_Audio, "MusicCmdLine(\"%s\")\n", Str);

	/*
	** Do the right thing, depending on what editor
	** command was just given to us.
	*/
	if (GetCMD(&Str, "LOAD")) // MUSIC LOAD FILE=...
	{
		char Fname[80];

		/*
		** Load the specified music objects file
		** into memory.
		*/
		if (GetSTRING(Str,"FILE=",Fname,80))
		{
			/* Add the specified object file. */
			GObj.AddFile(Fname);
		}
	}
	else if (GetCMD(&Str, "SAVE"))
	{
		char	TempFname[80];		/* Filename. */
		char	TempName[80];		/* Resname. */

		/*
		** Save the music objects to a file.
		*/
		if (GetCMD(&Str,"ALL") && GetSTRING(Str,"FILE=",TempFname,79))
		{
			/*
			** Save all song objects to specified file.
			*/

			UMusic * Music;		/* Temporary music object. */

			GObj.UntagAll();
			FOR_ALL_TYPED_OBJECTS(Music,UMusic)
			{
				Music->Flags |= RF_TagExp;
			}
			END_FOR_ALL_TYPED_OBJECTS;
			GObj.SaveDependentTagged(TempFname, 0);
		}
		else if ((GetSTRING(Str,"FILE=",TempFname,79)) &&
			GetSTRING(Str,"NAME=",TempName,79))
		{
			/*
			** Save a specific music object
			** to specified file.
			*/
			UMusic * Music;		/* Temporary music object. */

			Music = NULL;
			if (!GetUMusic(Str, "NAME=", &Music))
			{
				debugf(LOG_Audio, "MUSIC TEST:  Specified song not found in object list!\n");
				return;
			}
			if (Music != NULL)
			{
				GObj.UntagAll();
				Music->Flags |= RF_TagExp;
				GObj.SaveDependentTagged(TempFname, 0);
			}
			else
			{
				debug(LOG_Audio,
					"MUSIC SAVE:  Specified song not found");
			}
		}
	}
	else if (GetCMD(&Str, "IMPORT"))
	{
		char	TempFName[256];		/* Filename. */
		char	TempName[256];		/* Name of object. */

		/*
		** Import a S3M file as a new object.
		*/

		/* Get parameters from command line. */
		if (!GetSTRING(Str, "FILE=", TempFName, 256))
		{
			debugf(LOG_Audio, "MUSIC IMPORT:  No FILE= specified!\n");
			return;
		}
		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Audio, "MUSIC IMPORT:  No NAME= specified!\n");
			return;
		}

		/* Create the music object. */
		UMusic *Music = new(TempName, TempFName, IMPORT_Replace)UMusic;
		if (Music == NULL)
		{
			debugf(LOG_Audio, "MUSIC IMPORT:  Failed creating new object!\n");
			return;
		}

		debugf(LOG_Audio, "MUSIC IMPORT:  Music object created.\n");
	}
#if 0
	else if (GetCMD(&Str, "EXPORT"))
	{
		/*
		** Export a music object to a S3M file.
		*/
		/* Not coded yet */
	}
#endif
	else if (GetCMD(&Str, "TEST"))
	{
		/*
		** Plays a music object.  This happens
		** when the user presses the "test music"
		** button in the Unreal editor.
		*/
		char		TempName[256];	/* Name of object. */
		UMusic *	Music;		/* Ptr to object. */

		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Audio, "MUSIC TEST:  No NAME= specified!\n");
			return;
		}

		Music = NULL;
		if (!GetUMusic(Str, "NAME=", &Music))
		{
			debugf(LOG_Audio, "MUSIC TEST:  Specified song not found in object list!\n");
			return;
		}

		/* Play the specified song. */
#if 1
		GAudio.SpecifySong(Music);
		GAudio.Restart();
#else
		GAudio.ExitLevel();
		GAudio.Exit();
		GAudio.Init(1);
		GAudio.SpecifySong(Music);
		GAudio.InitLevel(GRes.GetMaxRes()); //!!AMMON: Note this change; 4096 may not have been enough
#endif
	}
	else if (GetCMD(&Str, "TESTOFF"))
	{
		debugf(LOG_Audio, "MUSIC TESTOFF\n");

		/* Stop playing specified song. */
		GAudio.SpecifySong((UMusic *)NULL);
		GAudio.ExitLevel();
		GAudio.Exit();
	}
	else if (GetCMD (&Str,"QUERY"))
	{
		/* Command:  MUSIC QUERY */

		/* Return list of songs. */
		musicQuerySongForLink();
	}
	else if (GetCMD (&Str,"DEBUG"))
	{
		/*
		** Output list of music objects to log window,
		** for debugging.  This is called by issuing the
		** "MUSIC DEBUG" command to the UnrealEd console.
		*/
		UMusic	*Music;
		int	i;

		/* Loop through all the song objects. */
		i = 0;	
		FOR_ALL_TYPED_OBJECTS(Music,UMusic)
		{
			debugf("UMusic %d:\n", i);
			debugf("  Name == \"%s\"\n",
				Music->Name);
			i++;
		}
		END_FOR_ALL_TYPED_OBJECTS;

		/* If there were no song objects, say so. */
		if (i < 1)
		{
			debugf("No song objects found.\n");
		}
	}
	else
	{
		/* Add more commands as necessary... */
	}

	unguard;
} /* End MusicCmdLine() */

/*-----------------------------------------------------------------------------
	Music link topic functions
-----------------------------------------------------------------------------*/

//
// Query a list of songs.
//
static void
musicQuerySongForLink(void)
{
	guard(musicQuerySongForLink);

	UMusic	*Music;

#ifdef ADBG
	debugf(LOG_Audio, "musicQuerySongForLink()");
#endif /* ADBG */

	if (GTempList==NULL)
		GTempList = (UMusic **)appMalloc(MAX_RESULTS * sizeof(UMusic *),"Ammon");
	GCurResult  = 0;
	GNumResults = 0;

	FOR_ALL_TYPED_OBJECTS(Music,UMusic)
	{
		if (GNumResults < MAX_RESULTS)
		{
			GTempList[GNumResults++] = Music;
		}
	}
	END_FOR_ALL_TYPED_OBJECTS;

	unguard;
} /* End musicQuerySongForLink() */

//
// Communicates information between Unreal and UnrealEd.
//
IMPLEMENT_CLASS(UMusic);

/*
** MusicTopicHandler::Get
** Gets called when Ed.Server.GetProp("Music", "{item}") is
** called from the unreal editor in Visual Basic.
*/
void
MusicTopicHandler::Get(ULevel *Level, const char *Topic, const char *Item, char *Data)
{
	guard(MusicTopicHandler::Get);
	UMusic	*Music;

#ifdef ADBG
	debugf(LOG_Audio, "MusicTopicHandler::Get(Level, Topic==\"%s\", Item==\"%s\", Data)\n",
			Topic, Item);
#endif /* ADBG */

	if ((stricmp(Item,"QUERYMUSIC")==0) && (GCurResult < GNumResults))
	{
		Music = GTempList[GCurResult];
		//
		sprintf(Data, "%s", Music->Name);
		GCurResult++;
	}
	else
	{
		*Data = '\0';
	}
	unguard;
} /* End MusicTopicHandler::Get() */

/*
** MusicTopicHandler::Set
** Gets called when Ed.Server.SetProp("Music", "{item}", "{data}") is
** called from the unreal editor in Visual Basic.
*/
void
MusicTopicHandler::Set(ULevel *Level, const char *Topic, const char *Item, const char *Data)
{
	guard(MusicTopicHandler::Set);

#ifdef ADBG
	debugf(LOG_Audio, "MusicTopicHandler::Set(Level, Topic==\"%s\", Item==\"%s\", Data)\n",
			Topic, Item);
#endif /* ADBG */

	if (stricmp(Item,"YourParameter")==0)
	{
		// Set some parameter based on the contents of *Data
	};
	unguard;
} /* End MusicTopicHandler::Set() */

#endif

/*
=============================================================================
End UnMusic.cpp
=============================================================================
*/
