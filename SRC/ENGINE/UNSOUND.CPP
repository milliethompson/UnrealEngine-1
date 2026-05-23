/*=============================================================================
FILENAME:     UnSound.cpp
DESCRIPTION:  Implementation of the "USound" class and related routines.
NOTICE:       Copyright 1996 Epic MegaGames, Inc.  This software is a trade
              secret.
TOOLS:        Compiled with Visual C++ 4.0, Calling method=__fastcall
FORMAT:       8 characters per tabstop, 100 characters per line.
HISTORY:
  When      Who                 What
  ----      ---                 ----
  ??/??/96  Tim Sweeney         Create stubs for this module.
  04/18/96  Ammon R. Campbell   Misc. hacks started.
=============================================================================*/

/*
----------------------------
ADBG:  Define this symbol
       for lots of debug
       output.
----------------------------
*/
// #define ADBG

/********************************* INCLUDES ********************************/

#pragma warning (disable : 4201) /* nonstandard extension used : nameless struct/union */
//#include <windows.h>
#include "Unreal.h"		// Unreal engine includes
#include "UnSound.h"
#include "UnAudio.h"		/* Declarations for using UnAudio.lib */
#include "UfxEdit.h"	// Headers for using UfxEdit.dll

/********************************* HEADERS *********************************/

#if 0
static void audioQueryFamilyForLink(void);
static void audioQuerySoundForLink(char *FamilyName);
#endif

/********************************* CONSTANTS *******************************/

#if 0
/* Maximum number of results during query. */
#define MAX_RESULTS	1024
#define MAX_FAM_RESULTS	256
#endif

/********************************* VARIABLES *******************************/

#if 0
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
#endif

/********************************* FUNCTIONS *******************************/

/*************************************************************************
                     Implementation of USound class
*************************************************************************/

/*
** USound::InitHeader:
** Initialize the header of the object.
*/
void
USound::InitHeader(void)
{
	guard(USound::InitHeader);

#ifdef ADBG
	debugf(LOG_Audio, "USound::InitHeader()\n");
#endif /* ADBG */

	/* Vital: Must call parent handler! */
	UObject::InitHeader();

	/* Zero all the variables to known states. */
	DataSize = 0;
	FamilyName = NAME_None;
	SoundID = -1;

	unguard;
};

/*
** USound::QuerySize:
** Determine size of object.
**
** Parameters:
**	NONE
**
** Returns:
**	Size of object in bytes.
*/
int
USound::QuerySize(void)
{
	guard(USound::QuerySize);
#ifdef ADBG
	debugf(LOG_Audio, "USound::QuerySize()\n");
#endif /* ADBG */

	return DataSize;

	unguard;
} /* End USound::QuerySize() */

/*
** USound::Import:
** Import a sound effect from a buffer in memory.
**
** Parameters:
**	Name		Description
**	----		-----------
**	Buffer		Pointer to data containing object to
**			be imported.
**	BufferEnd	Pointer to first byte following object
**			to be imported.
**	FileType	Extension of the file, i.e. "WAV".
**
** Returns:
**	Value	Meaning
**	-----	-------
**	NULL	Error occured.
**	other	Same as BufferEnd if successful.
*/
const char *
USound::Import(const char *Buffer, const char *BufferEnd,const char *FileType)
{
	guard(USound::Import);

#ifdef ADBG
	debugf(LOG_Audio, "USound::Import(Buffer, BufferEnd, FileType)\n");
#endif /* ADBG */

	/* If object is a WAV, turn it into a one-shot UFX */
	/* NOT CODED YET */

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
} /* End USound::Import() */

/*
** USound::Export:
** Export a sound effect to a buffer in memory.
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
USound::Export(char *Buffer, const char *FileType, int Indent)
{
	guard(USound::Export);

#ifdef ADBG
	debugf(LOG_Audio, "USound::Export(Buffer, BufferEnd, FileType)\n");
#endif /* ADBG */

	/* Copy from our data buffer to export buffer. */
	Lock(LOCK_ReadWrite);
	memcpy(Buffer, GetData(), DataSize);
	Unlock(LOCK_ReadWrite);

	return Buffer + DataSize;

	unguard;
} /* End USound::Export() */

/*
** USound::SerializeData:
**   Serialize the sound effect's data.
*/
void
USound::SerializeData( FArchive &Ar )
{
	guard(USound::SerializeData);
#ifdef ADBG
	debugf(LOG_Audio, "USound::SerializeData(&Callback)\n");
#endif /* ADBG */

	Lock(LOCK_ReadWrite);
	Ar.Serialize(GetData(), DataSize);
	Unlock(LOCK_ReadWrite);

	unguard;
} /* End USound::SerializeData() */

IMPLEMENT_CLASS(USound);

/*************************************************************************
                      Unreal editor hooks for audio
*************************************************************************/

#if 0
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
void
UNENGINE_API AudioCmdLine(const char *Str)
{
	guard(AudioCmdLine);

	char TempStr[256];

	debugf(LOG_Audio, "AudioCmdLine(\"%s\")\n", Str);

	/*
	** Do the right thing, depending on what editor
	** command was just given to us.
	*/
	if (GetCMD(&Str, "LOADFAMILY")) // AUDIO LOAD FILE=...
	{
		char Fname[80];

		/*
		** Load the specified audio Unrealfile file
		** into memory.
		*/
		if (GetSTRING(Str,"FILE=",Fname,80))
		{
			/* Add the specified Unrealfile. */
			GObj.AddFile(Fname);
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
				if (!Sound->FamilyName.IsNone())
				{
					Sound->Flags |= RF_TagExp;
				}
			}
			END_FOR_ALL_TYPED_OBJECTS;
			GObj.SaveDependentTagged(TempFname, 0);
		}
		else if ((GetSTRING(Str,"FILE=",TempFname,79)) &&
			GetNAME(Str,"FAMILY=",&Name))
		{
			/*
			** Save sound objects from specified family
			** to specified file.
			*/

			GObj.UntagAll();
			if (GObj.TagAllReferencingName(Name, RES_Sound) == 0)
			{
				debug(LOG_Audio,
					"AUDIO SAVEFAMILY:  Unknown family specified");
			}
			else
			{
				GObj.SaveDependentTagged(TempFname, 0);
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
			debugf(LOG_Audio, "AUDIO IMPORT:  No FILE= specified!\n");
			return;
		}
		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Audio, "AUDIO IMPORT:  No NAME= specified!\n");
			return;
		}

		/* Create the sound object. */
		USound *Sound = new(TempName, TempFName, IMPORT_Replace)USound;
		if (Sound == NULL)
		{
			debugf(LOG_Audio, "AUDIO IMPORT:  Failed creating new object!\n");
			return;
		}

		GetNAME(Str, "FAMILY=", &Sound->FamilyName);
		debugf(LOG_Audio, "AUDIO IMPORT:  Sound object created.\n");
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
		char		TempName[256];	/* Name of object. */
		USound *	Sound;		/* Ptr to object. */

		if (!GetSTRING(Str, "NAME=", TempName, 256))
		{
			debugf(LOG_Audio, "AUDIO TEST:  No NAME= specified!\n");
			return;
		}

		Sound = NULL;
		if (!GetUSound(Str, "NAME=", &Sound))
		{
			debugf(LOG_Audio, "AUDIO TEST:  Specified sound not found in object list!\n");
			return;
		}

		/* Play the specified sound effect. */
		if (iTestPID == -1)
		{
#if 1
			GAudio.SpecifySong(NULL);
			GAudio.Restart();
#else
			GAudio.ExitLevel();
			GAudio.Exit();
			GAudio.Init(1);
			GAudio.InitLevel(GRes.GetMaxRes());
#endif
			iTestPID = GAudio.PlaySfxPrimitive(Sound);
			debugf(LOG_Audio, "AUDIO TEST:  Name==\"%s\" PID==%d\n", TempName, iTestPID);
		}
	}
	else if (GetCMD(&Str, "TESTOFF"))
	{
		debugf(LOG_Audio, "AUDIO TESTOFF\n");
		if (iTestPID != -1)
		{
			GAudio.SfxStop(iTestPID);
			GAudio.ExitLevel();
			GAudio.Exit();
			iTestPID = -1;
		}
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
			debugf(LOG_Audio, "AUDIO EDIT:  No NAME= specified!\n");
			return;
		}

		Sound = NULL;
		if (!GetUSound(Str, "NAME=", &Sound))
		{
			debugf(LOG_Audio, "AUDIO EDIT:  Specified sound not found in object list!\n");
			return;
		}

		/* Let user edit the sound's data. */
		if (UfxEdit(Sound->Data, &NewCSound, &NewSize))
		{
			/* User has edited the sound. */
			/* Replace sound's data with edited data. */
			Sound->DataSize = NewSize;
			Sound->Realloc();
			memcpy(Sound->Data, NewCSound, NewSize);
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
				Sound->Name);
			if (!Sound->FamilyName.IsNone())
			{
				debugf("  FamilyName == \"%s\"\n",
					Sound->FamilyName.Name());
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
	debugf(LOG_Audio, "audioQuerySoundForLink(\"%s\")\n", FamilyName);
#endif /* ADBG */

	Name.Add(FamilyName);
	if (GTempList==NULL)
		GTempList = (USound **)appMalloc(MAX_RESULTS * sizeof(USound *),"Ammon");
	GCurResult  = 0;
	GNumResults = 0;

	FOR_ALL_TYPED_OBJECTS(Sound,USound)
	{
		if ((GNumResults < MAX_RESULTS) &&
			((Sound->FamilyName == Name) ||
			(All && (!Sound->FamilyName.IsNone()))))
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
	debugf(LOG_Audio, "audioQueryFamilyForLink()\n");
#endif /* ABG */

	if (GTempFamList == NULL)
	{
		GTempFamList = (FName *)appMalloc(MAX_FAM_RESULTS * sizeof (FName),"Ammon");
	}

	GCurFamResult  = 0;
	GNumFamResults = 0;

	GTempFamList [GNumFamResults++].Add("(All)");
	FOR_ALL_TYPED_OBJECTS(Sound,USound)
	{
		ListName = Sound->FamilyName;
		if (!ListName.IsNone())
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
AudioTopicHandler::Get(ULevel *Level, const char *Topic, const char *Item, char *Data)
{
	guard(AudioTopicHandler::Get);
	USound	*Sound;
	FName	FamilyName;

#ifdef ADBG
	debugf(LOG_Audio, "AudioTopicHandler::Get(Level, Topic==\"%s\", Item==\"%s\", Data)\n",
			Topic, Item);
#endif /* ADBG */

	if ((stricmp(Item,"QUERYAUDIO")==0) && (GCurResult < GNumResults))
	{
		Sound = GTempList[GCurResult];
		//
		sprintf(Data, "%s", Sound->Name);
		GCurResult++;
	}
	else if ((stricmp(Item,"QUERYFAM")==0) && (GCurFamResult < GNumFamResults))
	{
		FamilyName = GTempFamList [GCurFamResult];
		sprintf(Data, "%s", FamilyName.Name());
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
AudioTopicHandler::Set(ULevel *Level, const char *Topic, const char *Item, const char *Data)
{
	guard(AudioTopicHandler::Set);

#ifdef ADBG
	debugf(LOG_Audio, "AudioTopicHandler::Set(Level, Topic==\"%s\", Item==\"%s\", Data)\n",
			Topic, Item);
#endif /* ADBG */

	if (stricmp(Item,"YourParameter")==0)
	{
		// Set some parameter based on the contents of *Data
	};
	unguard;
} /* End AudioTopicHandler::Set() */
#endif

/*
=============================================================================
End UnSound.cpp
=============================================================================
*/
