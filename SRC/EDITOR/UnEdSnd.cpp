/*=============================================================================
FILENAME:     UnEdSnd.cpp
DESCRIPTION:  Sound-related functions that are specific to UnrealEd and
              hence don't belong in UnEngine.dll
NOTICE:       Copyright 1996-1997 Epic MegaGames, Inc.  This software is a
              trade secret.
TOOLS:        Compiled with Visual C++ 4.0, Calling method=__fastcall
FORMAT:       8 characters per tabstop, 100 characters per line.
HISTORY:
  When      Who                 What
  ----      ---                 ----
  12/09/96  Ammon Campbell      Pulled this code out of UnSound.cpp and
                                related sources.
=============================================================================*/

/********************************* INCLUDES ********************************/

#include "Unreal.h"	// Unreal engine includes
#include "UnAudio.h"	// Headers for using UnAudio.lib
#include "UfxEdit.h"	// Headers for using UfxEdit.dll
#include "UfxTest.h"	// Headers for using UfxTest.dll

#include <process.h>

/********************************* HEADERS *********************************/

static void audioQueryFamilyForLink(void);
static void audioQuerySoundForLink(char *FamilyName);

/********************************* CONSTANTS *******************************/

/* Maximum number of results during query. */
#define MAX_RESULTS	1024
#define MAX_FAM_RESULTS	256

/********************************* VARIABLES *******************************/

/* Used during sound queries. */
static DWORD GCurResult = 0;
static DWORD GNumResults = 0;
static USound **GTempList = NULL;

/* Used during family queries. */
static DWORD GNumFamResults = 0;
static DWORD GCurFamResult = 0;
static FName *GTempFamList = NULL;

/* Handle of sound being tested in UnrealEd, or -1 if not testing a sound. */
static int iTestPID = -1;

/********************************* FUNCTIONS *******************************/

/*---------------------------------------------------------------------------
	Audio command link
---------------------------------------------------------------------------*/

/*
** AudioCmdLine:
** Receives audio-related commands from the Unreal editor.
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
AudioCmdLine(const char *Str,FOutputDevice *Out)
{
	guard(AudioCmdLine);

	char TempStr[256];

	debugf(LOG_Info, "AudioCmdLine(\"%s\")\n", Str);

	/*
	** Do the right thing, depending on what editor
	** command was just given to us.
	*/
	if (GetCMD(&Str, "LOADFAMILY")) // AUDIO LOAD FILE=...
	{
		char Fname[80];

		/*
		** Load the specified audio Unrealfile
		** into memory.
		*/
		if (GetSTRING(Str,"FILE=",Fname,80))
		{
			/* Add the specified Unrealfile. */
			GObj.AddFile(Fname,NULL);
		}
	}
	else if (GetCMD(&Str, "SAVEFAMILY"))
	{
		FName	Name;			/* Family name. */
		char	TempFname[80];		/* Filename. */

		/*
		** Save the specified audio objects
		** to a file.
		*/
		if (GetCMD(&Str,"(All)") && GetSTRING(Str,"FILE=",TempFname,79))
		{
			/*
			** Save all sound objects to specified file.
			*/

			USound * Sound;		/* Temporary sound object. */

			GObj.UntagAll();
			FOR_ALL_TYPED_OBJECTS(Sound,USound)
			{
				if( Sound->FamilyName != NAME_None )
				{
					Sound->SetFlags(RF_TagExp);
				}
			}
			END_FOR_ALL_TYPED_OBJECTS;
			GObj.SaveDependentTagged( TempFname );
		}
		else if ((GetSTRING(Str,"FILE=",TempFname,79)) &&
			GetNAME(Str,"FAMILY=",&Name))
		{
			/*
			** Save sound objects from specified family
			** to specified file.
			*/

			GObj.UntagAll();
			if( GObj.TagAllReferencingName(Name,USound::GetBaseClass() ) == 0)
			{
				debug(LOG_Info,
					"AUDIO SAVEFAMILY:  Unknown family specified");
			}
			else
			{
				GObj.SaveDependentTagged( TempFname );
			}
		}
	}
	else if (GetCMD(&Str, "IMPORT"))
	{
		char	TempFName[256];		/* Filename. */
		char	TempName[256];		/* Name of object. */

		/*
		** Import a WAV file as a new object.
		*/

		/* Get parameters from command line. */
		if (!GetSTRING(Str, "FILE=", TempFName, 256))
		{
			debugf(LOG_Info, "AUDIO IMPORT:  No FILE= specified!\n");
			return;
		}
		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Info, "AUDIO IMPORT:  No NAME= specified!\n");
			return;
		}

		/* Create the sound object. */
		USound *Sound = new(TempName, TempFName, IMPORT_Replace)USound;
		if (Sound == NULL)
		{
			debugf(LOG_Info, "AUDIO IMPORT:  Failed creating new object!\n");
			return;
		}

		GetNAME(Str, "FAMILY=", &Sound->FamilyName);
		debugf(LOG_Info, "AUDIO IMPORT:  Sound object.\n");
	}
#if 0
	else if (GetCMD(&Str, "EXPORT"))
	{
		/*
		** Export a sound object to a WAV file.
		*/
		/* Not coded yet */
	}
#endif
	else if (GetCMD(&Str, "TEST"))
	{
		/*
		** Plays a sound object.  This happens
		** when the user presses the "test sound"
		** button in the Unreal editor.
		*/
#if 1
		char		TempName[256];	/* Name of object. */
		USound *	Sound;		/* Ptr to object. */

		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Info, "AUDIO TEST:  No NAME= specified!\n");
			return;
		}

		Sound = NULL;
		if (!GetUSound(Str, "NAME=", Sound))
		{
			debugf(LOG_Info, "AUDIO TEST:  Specified sound not found in object list!\n");
			return;
		}

		UfxTest(Sound->GetData(), TempName, 0);
#else
/* OLD STUFF -- Don't delete, used for reference. */
		char		TempName[256];	/* Name of object. */
		USound *	Sound;		    /* Ptr to object. */

		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Info, "AUDIO TEST:  No NAME= specified!\n");
			return;
		}

		Sound = NULL;
		if (!GetUSound(Str, "NAME=", Sound))
		{
			debugf(LOG_Info, "AUDIO TEST:  Specified sound not found in object list!\n");
			return;
		}

		/* Play the specified sound effect. */
		if (iTestPID == -1)
		{
#if 1
			GAudio.SpecifySong(0);
			GAudio.Restart();
#else
			GAudio.ExitLevel();
			GAudio.Exit();
			GAudio.Init(1);
			GAudio.InitLevel(4096);
#endif
			iTestPID = GAudio.PlaySfxPrimitive(Sound);
			debugf(LOG_Info, "AUDIO TEST:  Name==\"%s\" PID==%d\n", TempName, iTestPID);
		}
#endif
	}
	else if (GetCMD(&Str, "TESTOFF"))
	{
		debugf(LOG_Info, "AUDIO TESTOFF\n");
#if 0
/* OLD STUFF - Don't delete; used for reference. */
		if (iTestPID != -1)
		{
			GAudio.SfxStop(iTestPID);
			GAudio.ExitLevel();
			GAudio.Exit();
			iTestPID = -1;
		}
#endif
	}
	else if (GetCMD(&Str, "EDIT"))
	{
		/*
		** Edit a sound object.  This happens
		** when the user presses the "edit sound"
		** button in the Unreal editor.
		*/
		char		TempName[256];	/* Name of object. */
		USound *	Sound;		/* Ptr to object. */
		void *		NewCSound;	/* Ptr to edited data. */
		long		NewSize;	/* Size of edited data. */

		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Info, "AUDIO EDIT:  No NAME= specified!\n");
			return;
		}

		Sound = NULL;
		if (!GetUSound(Str, "NAME=", Sound))
		{
			debugf(LOG_Info, "AUDIO EDIT:  Specified sound not found in object list!\n");
			return;
		}

		/* Let user edit the sound's data. */
		if (UfxEdit(Sound->GetData(), &NewCSound, &NewSize))
		{
			/* User has edited the sound. */
			/* Replace sound's data with edited data. */
			Sound->DataSize = NewSize;
			Sound->Realloc();
			memcpy(Sound->GetData(), NewCSound, NewSize);
			free(NewCSound);
		}
	}
	else if (GetCMD (&Str,"QUERY"))
	{
		/* Command:  AUDIO QUERY [FAMILY=xxx] */

		if (GetSTRING(Str, "FAMILY=", TempStr, NAME_SIZE))
		{
			/* Return list of sounds in family. */
			audioQuerySoundForLink(TempStr);
		}
		else
		{
			/* Return list of sound families. */
			audioQueryFamilyForLink();
		}
	}
	else if (GetCMD (&Str,"DEBUG"))
	{
		/*
		** Output list of audio objects to log window,
		** for debugging.  This is called by issuing the
		** "AUDIO DEBUG" command to the UnrealEd console.
		*/
		USound	*Sound;
		int	i;

		/* Loop through all the sound objects. */
		i = 0;	
		FOR_ALL_TYPED_OBJECTS(Sound,USound)
		{
			debugf("USound %d:\n", i);
			debugf("  Name == \"%s\"\n",
				Sound->GetName());
			if( Sound->FamilyName != NAME_None )
			{
				debugf("  FamilyName == \"%s\"\n",
					Sound->FamilyName());
			}
			else
			{
				debugf("  Sound has no family name!\n");
			}
			i++;
		}
		END_FOR_ALL_TYPED_OBJECTS;

		/* If there were no sound objects, say so. */
		if (i < 1)
		{
			debugf("No sound objects found.\n");
		}
	}
	else
	{
		/* Add more commands as necessary... */
	}

	unguard;
} /* End AudioCmdLine() */

/*-----------------------------------------------------------------------------
	Audio link topic functions
-----------------------------------------------------------------------------*/

//
// Query a list of sounds.  Call with sound family's name,
// or "(All)" for all families.
//
static void
audioQuerySoundForLink(char *FamilyName)
{
	guard(audioQuerySoundForLink);

	FName	Name;
	int	All = !stricmp(FamilyName,"(All)");
	USound	*Sound;

#ifdef ADBG
	debugf(LOG_Info, "audioQuerySoundForLink(\"%s\")\n", FamilyName);
#endif /* ADBG */

	Name = FName( FamilyName, FNAME_Add );
	if (GTempList==NULL)
		GTempList = (USound **)appMalloc(MAX_RESULTS * sizeof(USound *),"Ammon");
	GCurResult  = 0;
	GNumResults = 0;

	FOR_ALL_TYPED_OBJECTS(Sound,USound)
	{
		if ((GNumResults < MAX_RESULTS) &&
			((Sound->FamilyName == Name) ||
			(All && Sound->FamilyName!=NAME_None)))
		{
			GTempList[GNumResults++] = Sound;
		}
	}
	END_FOR_ALL_TYPED_OBJECTS;

	unguard;
} /* End audioQuerySoundForLink() */

//
// Query a list of sound families.
//
static void
audioQueryFamilyForLink(void)
{
	guard(audioQueryFamilyForLink);

	USound	*Sound;
	FName	ListName;

#ifdef ADBG
	debugf(LOG_Info, "audioQueryFamilyForLink()\n");
#endif /* ABG */

	if (GTempFamList == NULL)
	{
		GTempFamList = (FName *)appMalloc(MAX_FAM_RESULTS * sizeof (FName),"Ammon");
	}

	GCurFamResult  = 0;
	GNumFamResults = 0;

	GTempFamList [GNumFamResults++] = FName( "(All)", FNAME_Add );
	FOR_ALL_TYPED_OBJECTS(Sound,USound)
	{
		ListName = Sound->FamilyName;
		if( ListName != NAME_None )
		{
			for (DWORD j = 0; j < GNumFamResults; j++)
			{
				if (GTempFamList[j] == ListName)
					break;
			}
			if ((j >= GNumFamResults) && (GNumFamResults < MAX_FAM_RESULTS))
			{
				GTempFamList [GNumFamResults++] = ListName;
			}
		}
	}
	END_FOR_ALL_TYPED_OBJECTS;

	unguard;
} /* End audioQueryFamilyForLink() */

//
// Communicates information between Unreal and UnrealEd.
//
AUTOREGISTER_TOPIC("Audio",AudioTopicHandler);

/*
** AudioTopicHandler::Get
** Gets called when Ed.Server.GetProp("Audio", "{item}") is
** called from the unreal editor in Visual Basic.
*/
void
AudioTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(AudioTopicHandler::Get);
	USound	*Sound;
	FName	FamilyName;

#ifdef ADBG
	debugf(LOG_Info, "AudioTopicHandler::Get(Level, Topic==\"%s\", Item==\"%s\", Data)\n",
			Topic, Item);
#endif /* ADBG */

	if ((stricmp(Item,"QUERYAUDIO")==0) && (GCurResult < GNumResults))
	{
		Sound = GTempList[GCurResult];
		//
		Out.Logf("%s", Sound->GetName());
		GCurResult++;
	}
	else if ((stricmp(Item,"QUERYFAM")==0) && (GCurFamResult < GNumFamResults))
	{
		FamilyName = GTempFamList [GCurFamResult];
		Out.Logf("%s", FamilyName());
		GCurFamResult++;
	}
#if 0
	else if ((stricmp(Item, "VibratoDepth") == 0)
	{
	}
	else if ((stricmp(Item, "VibratoSpeed") == 0)
	{
	}
	else if ((stricmp(Item, "TremoloDepth") == 0)
	{
	}
	else if ((stricmp(Item, "TremoloSpeed") == 0)
	{
	}
#endif
	unguard;
} /* End AudioTopicHandler::Get() */

/*
** AudioTopicHandler::Set
** Gets called when Ed.Server.SetProp("Audio", "{item}", "{data}") is
** called from the unreal editor in Visual Basic.
*/
void
AudioTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(AudioTopicHandler::Set);

#ifdef ADBG
	debugf(LOG_Info, "AudioTopicHandler::Set(Level, Topic==\"%s\", Item==\"%s\", Data)\n",
			Topic, Item);
#endif /* ADBG */

	if (stricmp(Item,"YourParameter")==0)
	{
		// Set some parameter based on the contents of *Data
	};
	unguard;
} /* End AudioTopicHandler::Set() */

/*
=============================================================================
End UnEdSnd.cpp
=============================================================================
*/
