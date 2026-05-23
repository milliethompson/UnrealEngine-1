/*=============================================================================
FILENAME:     UnEdMus.cpp
DESCRIPTION:  Music-related functions that are specific to UnrealEd and
              hence don't belong in UnEngine.dll
NOTICE:       Copyright 1996 Epic MegaGames, Inc.  This software is a trade
              secret.
TOOLS:        Compiled with Visual C++ 4.0, Calling method=__fastcall
FORMAT:       8 characters per tabstop, 100 characters per line.
=============================================================================*/

/********************************* INCLUDES ********************************/

#pragma warning (disable : 4201) /* nonstandard extension used : nameless struct/union */
#include "Unreal.h"		// Unreal engine includes
#include "UnAudio.h"	// Headers for using UnAudio.lib

#include <process.h>
#include "UfxTest.h"	// Headers for using UfxTest.dll

/********************************* HEADERS *********************************/

static void musicQuerySongForLink(void);

/********************************* CONSTANTS *******************************/

/* Maximum number of results during query. */
#define MAX_RESULTS	1024

/********************************* VARIABLES *******************************/

/* Used during song queries. */
static DWORD GCurResult = 0;
static DWORD GNumResults = 0;
static UMusic **GTempList = NULL;

/********************************* FUNCTIONS *******************************/

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
void UNEDITOR_API
MusicCmdLine(const char *Str, FOutputDevice *Out)
{
	guard(MusicCmdLine);

	debugf(LOG_Info, "MusicCmdLine(\"%s\")\n", Str);

	/*
	** Do the right thing, depending on what editor
	** command was just given to us.
	*/
	if (GetCMD(&Str, "LOAD")) // MUSIC LOAD FILE=...
	{
		char Fname[80];

		/*
		** Load the specified music object file
		** into memory.
		*/
		if (GetSTRING(Str,"FILE=",Fname,80))
		{
			/* Add the specified Unrealfile. */
			GObj.AddFile(Fname,NULL);
		}
	}
	else if (GetCMD(&Str, "SAVE"))
	{
		char	TempFname[80];		/* Filename. */
		char	TempName[80];		/* Resname. */

		/*
		** Save the music object to a file.
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
				Music->SetFlags(RF_TagExp);
			}
			END_FOR_ALL_TYPED_OBJECTS;
			GObj.SaveDependentTagged( TempFname );
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
			if (!GetUMusic(Str, "NAME=", Music))
			{
				debugf(LOG_Info, "MUSIC TEST:  Specified song not found in object list!\n");
				return;
			}
			if (Music != NULL)
			{
				GObj.UntagAll();
				Music->SetFlags(RF_TagExp);
				GObj.SaveDependentTagged( TempFname );
			}
			else
			{
				debug(LOG_Info,
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
			debugf(LOG_Info, "MUSIC IMPORT:  No FILE= specified!\n");
			return;
		}
		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Info, "MUSIC IMPORT:  No NAME= specified!\n");
			return;
		}

		/* Create the music object. */
		UMusic *Music = new(TempName, TempFName, IMPORT_Replace)UMusic;
		if (Music == NULL)
		{
			debugf(LOG_Info, "MUSIC IMPORT:  Failed creating new object!\n");
			return;
		}

		debugf(LOG_Info, "MUSIC IMPORT:  Music object created.\n");
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
		UMusic *	Music;		    /* Ptr to object. */

		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Info, "MUSIC TEST:  No NAME= specified!\n");
			return;
		}

		Music = NULL;
		if (!GetUMusic(Str, "NAME=", Music))
		{
			debugf(LOG_Info, "MUSIC TEST:  Specified song not found in object list!\n");
			return;
		}

		UfxTest(Music->GetData(), TempName, 1);
#if 0
#if 1
		char		TempName[256];	/* Name of object. */
		char		CmdLine[256];	/* Command to execute. */
		UMusic *	Music;		    /* Ptr to object. */
		FILE *		hfile;

		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Info, "MUSIC TEST:  No NAME= specified!\n");
			return;
		}

		Music = NULL;
		if (!GetUMusic(Str, "NAME=", Music))
		{
			debugf(LOG_Info, "MUSIC TEST:  Specified song not found in object list!\n");
			return;
		}

		strcat(TempName, ".tmp");
		hfile = fopen(TempName, "wb");
		if (hfile != NULL)
		{
			fwrite(Music->GetData(), Music->DataSize, 1, hfile);
			fclose(hfile);
			strcpy(CmdLine, "UnAudTst.exe ");
			strcat(CmdLine, TempName);
			strcat(CmdLine, " /MUSIC /DELETE");
			spawnlp(_P_NOWAIT, "UnAudTst.exe", "UnAudTst.exe",
				TempName, "/DELETE", "/MUSIC", NULL);
		}
#else
/* OLD STUFF - Don't delete; used for reference. */
		char		TempName[256]; /* Name of object. */
		UMusic *	Music;		   /* Ptr to object. */

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
		GAudio.InitLevel(4096);
#endif
#endif
#endif
	}
	else if (GetCMD(&Str, "TESTOFF"))
	{
		debugf(LOG_Info, "MUSIC TESTOFF\n");

#if 0
/* OLD STUFF - Don't delete; used for reference. */
		/* Stop playing specified song. */
		GAudio.SpecifySong((UMusic *)NULL);
		GAudio.ExitLevel();
		GAudio.Exit();
#endif
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
			debugf("  Name == \"%s\"\n",Music->GetName());
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
AUTOREGISTER_TOPIC("Music",MusicTopicHandler);

/*
** MusicTopicHandler::Get
** Gets called when Ed.Server.GetProp("Music", "{item}") is
** called from the unreal editor in Visual Basic.
*/
void
MusicTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
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
		Out.Logf("%s", Music->GetName());
		GCurResult++;
	}
	unguard;
} /* End MusicTopicHandler::Get() */

/*
** MusicTopicHandler::Set
** Gets called when Ed.Server.SetProp("Music", "{item}", "{data}") is
** called from the unreal editor in Visual Basic.
*/
void
MusicTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
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

/*
=============================================================================
End UnEdMus.cpp
=============================================================================
*/
