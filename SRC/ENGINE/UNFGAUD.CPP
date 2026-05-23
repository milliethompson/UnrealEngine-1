/*=============================================================================
FILENAME:     UnFGAud.cpp
DESCRIPTION:  Implementation of the "FGlobalAudio" class and related
              routines.
NOTICE:       Copyright 1996 Epic MegaGames, Inc.  This software is a trade
              secret.
TOOLS:        Compiled with Visual C++ 4.0.
FORMAT:       8 characters per tabstop, 100 characters per line.
HISTORY:
  When      Who                 What
  --------- ------------------- -------------------------------------------
  07/17/96  Ammon R. Campbell   This rewrite obsoletes the totally
                                different prior versions.
  07/30/96  Ammon R. Campbell   Added function to specify which song should
                                be played, based on an integer song number.
=============================================================================*/

/********************************* INCLUDES ********************************/

#pragma warning (disable : 4201) /* nonstandard extension used : nameless struct/union */

#include "Unreal.h"		/* Unreal headers (includes "UnFGAud.h") */
#include "UnSound.h"
#include "UnAudio.h"		/* Declarations for using UnAudio.lib */
#include "UnConfig.h"

// The symbol TEST enables a little bit of code that
// I use for debugging.  This should be commented out
// for normal use.
#define TEST

/********************************* HEADERS *********************************/

	/* See "UnFGAud.h" for public declarations and prototypes */

/********************************* CONSTANTS *******************************/

/******************************* LOCAL TYPES *******************************/

/******************************* LOCAL CLASSES *****************************/

/********************************* VARIABLES *******************************/

/*
** GAudio:
** The instantiation of the global audio class.
*/
FGlobalAudio GAudio;

/*
** num_ticks:
** The number of times FGlobalAudio::Tick() has been
** called.
*/
static unsigned long	num_ticks = 0L;

/********************************* FUNCTIONS *******************************/

/*************************************************************************
                      Functions local to this module
*************************************************************************/

/*
** plog:
** Function used for logging output from the sound engine
** library.
**
** Parameters:
**	Name	Description
**	----	-----------
**	msg	String to be output to log.
**
** Returns:
**	NONE
*/
void CDECL
plog(char *msg)
{
	if (msg[0] != '\0' && msg[strlen(msg) - 1] == '\n')
		msg[strlen(msg) - 1] = '\0';
	debugf(LOG_Info, "%s", msg);
} /* End plog() */

/*************************************************************************
             Implementation of FGlobalAudio member functions
*************************************************************************/

/*
** FGlobalAudio::Init:
** Performs tasks necessary to start up the audio system
** cleanly.  Called once when the application starts up.
**
** Parameters:
**	Name		Description
**	----		-----------
**	MakeActive	Flag; 1 if unreal game is starting,
**			or 0 if unreal editor is starting.
**
** Returns:
**	Value	Meaning
**	-----	-------
**	1	No error.
**	0	Fatal error occured.
*/
int
FGlobalAudio::Init(int MakeActive)
{
	guard(FGlobalAudio::Init);

	/* If UnrealEd is running, disable audio. */
	if (GEditor)
	{
		MakeActive = 0;
	}

	/* Connect sound engine log output to Unreal log. */
	SoundEngLogSetup(plog);
//	SoundEngLogDetail(1);
	plog("FGlobalAudio::Init()\n");

	/* Read audio settings from Unreal configuration file. */
	/* NOTE:  The strings that write these settings are in UnConfig.cpp */
	MusicVolumeSet( GConfiguration.GetInteger( FConfiguration::AudioSection, "MusicVolume", 42 ) );        
	SfxVolumeSet( GConfiguration.GetInteger( FConfiguration::AudioSection, "SoundVolume", 127 ) );        
	DirectSoundFlagSet( GConfiguration.GetBoolean( FConfiguration::AudioSection, "UseDirectSound", TRUE ) );
	FilterFlagSet( GConfiguration.GetBoolean( FConfiguration::AudioSection, "EnableFilter", TRUE ) );
	Use16BitFlagSet( GConfiguration.GetBoolean( FConfiguration::AudioSection, "Enable16Bit", TRUE ) );
	SurroundFlagSet( GConfiguration.GetBoolean( FConfiguration::AudioSection, "EnableSurround", TRUE ) );
	MixingRateSet( GConfiguration.GetInteger( FConfiguration::AudioSection, "MixingRate", 22050 ) );
	SfxMute( GConfiguration.GetInteger( FConfiguration::AudioSection, "SfxMute", 0));
	MusicMute( GConfiguration.GetInteger( FConfiguration::AudioSection, "MusicMute", 0));

	/* Initialize sound engine. */
	if (!SoundEngGlobalInit(MakeActive))
//	if (!SoundEngGlobalInit(1))
	{
		/* Init failed. */
		return 0;
	}

	return 1;
	unguard;
} /* End FGlobalAudio::Init() */

/*
** FGlobalAudio::Exit:
** Performs tasks necessary to shut down the audio system
** cleanly.  Called once just before Unreal terminates.
**
** Parameters:
**	NONE
**
** Returns:
**	NONE
*/
void
FGlobalAudio::Exit(void)
{
	guard(FGlobalAudio::Exit);

	plog("FGlobalAudio::Exit()\n");

	SoundEngGlobalDeinit();

	unguard;
} /* End FGlobalAudio::Exit() */

/*
** FGlobalAudio::Restart:
** Restarts the audio system, returning it to its present
** state, except changes to global settings such as
** mixing rate, direct sound flag, etc., will take effect.
**
** Parameters:
**	NONE
**
** Returns:
**	NONE
*/
void
FGlobalAudio::Restart(void)
{
	guard(FGlobalAudio::Restart);

	USound *Sound;		/* Used for object enumeration loop. */
	int	count;		/* Count of objects. */
	char	sSong[256];	/* Temporary name of song file. */
	char *	pSong;		/* Temporary song pointer. */

	plog("FGlobalAudio::Restart()\n");

	/* Shut off the sound system. */
	SoundEngMusicGetSongName(sSong, 255);
	pSong = SoundEngMusicGetSongPtr();
	SoundEngLocalDeinit();
	SoundEngGlobalDeinit();

	/* Restart sound system. */
	SoundEngGlobalInit(1);
	if (pSong != NULL)
		SoundEngMusicSpecifySong(pSong, 0);
	else
		SoundEngMusicSpecifySong(sSong, 1);

	/* Register all sound effects with sound engine. */
	/* Loop through all USound objects. */
	count = 0;
	FOR_ALL_TYPED_OBJECTS(Sound,USound)
	{
		/*
		** Load this sound into sound engine, saving
		** the sound ID returned by the sound engine.
		*/
//		bug("Registering %s",Sound->Name);
		Sound->SoundID = SoundEngSoundRegister(Sound->GetData());
		if (Sound->SoundID == -1)
		{
			appErrorf("Failed registering %s",Sound->GetName());
		}
//		bug("Registered %s",Sound->Name);
		count++;
	}
	END_FOR_ALL_TYPED_OBJECTS;

	SoundEngLocalInit();

	unguard;
} /* End FGlobalAudio::Restart() */

/*
** FGlobalAudio::InitLevel:
** Performs initializations specific to a map.
** Cannot be called until after FGlobalAudio::Init()
** is called.  FGlobalAudio::ExitLevel() should be
** called between subsequent calls to this function.
**
** Parameters:
**	Value		Meaning
**	-----		-------
**	MaxIndeces	Maximum # of actors in the level
**			that is beginning play.  This may
**			be used to determine the maximum
**			# of entities that may attempt to
**			play sounds.
**
** Returns:
**	Value	Meaning
**	-----	-------
**	1	No error.
**	0	Error occured.
*/
int
FGlobalAudio::InitLevel(int MaxIndices)
{
	guard(FGlobalAudio::InitLevel);

	USound *Sound;	/* Used for object enumeration loop. */
	int	count;	/* Count of objects. */
#ifdef TEST
	USound *TmpSound = NULL;
#endif /* TEST */

	plog("FGlobalAudio::InitLevel()\n");

	/* Register all sound effects with sound engine. */
	/* Loop through all USound objects. */
	count = 0;
	FOR_ALL_TYPED_OBJECTS(Sound,USound)
	{
		/*
		** Load this sound into sound engine, saving
		** the sound ID returned by the sound engine.
		*/
		Sound->SoundID = SoundEngSoundRegister(Sound->GetData());
		if (Sound->SoundID == -1)
		{
			appErrorf("Failed registering %s",Sound->GetName());
		}

#ifdef TEST
		debugf(LOG_Info, "Registered sound %s", Sound->GetName());
		// Look for the sound object TESTSND, which is
		// a continuously looping sound that I stick
		// into Unreal.ucx for debugging purposes.
		if (stricmp(Sound->GetName(), "TESTSND") == 0)
		{
			debugf(LOG_Info, "Found test sound");
			TmpSound = Sound;
		}
#endif /* TEST */

		count++;
	}
	END_FOR_ALL_TYPED_OBJECTS;

	/* Perform local init of sound engine. */
	if (!SoundEngLocalInit())
	{
		/* Failed init. */
		return 0;
	}

#ifdef TEST
	// Play the test sound, unless it wasn't found.
	if (TmpSound != NULL)
	{
//		FVector *Src = new FVector((FLOAT)17.0, (FLOAT)-5.0, (FLOAT)-95.0);
		FVector *Src = new FVector((FLOAT)60.0, (FLOAT)-108.0, (FLOAT)-113.0);
		debugf(LOG_Info, "Playing test sound");
		PlaySfxLocated(Src,	/* Location. */
			TmpSound,	/* Sound. */
			4095,		/* Actor. */
			0.0,		/* Radius. */
			1.0,		/* fScale. */
			1.0);		/* fPitchmod. */
	}
	else
	{
		debugf(LOG_Info, "Didn't find test sound");
	}
#endif /* TEST */

	return 1;
	unguard;
} /* End FGlobalAudio::InitLevel() */

/*
** FGlobalAudio::ExitLevel:
** Performs clean up tasks necessary after playing
** a map.  Should not be called unless a prior call
** was made to FGlobalAudio::InitLevel().
**
** Parameters:
**	NONE
**
** Returns:
**	NONE
*/
void
FGlobalAudio::ExitLevel(void)
{
	guard(FGlobalAudio::ExitLevel);
	plog("FGlobalAudio::ExitLevel()\n");

	USound *Sound;	/* Used for object enumeration loop. */
	int	count;	/* Count of objects. */

	/* Perform local shutdown of sound engine. */
	SoundEngLocalDeinit();

	/* Mark all sound objects as no longer registered. */
	count = 0;
	FOR_ALL_TYPED_OBJECTS(Sound,USound)
	{
		/* Mark this sound objects as no longer registered. */
		Sound->SoundID = -1;
		count++;
	}
	END_FOR_ALL_TYPED_OBJECTS;

	unguard;
} /* End FGlobalAudio::ExitLevel() */

/*
** FGlobalAudio::Tick:
** Called about 35 times per second to update the state
** of the sound system.
**
** Parameters:
**	NONE
**
** Returns:
**	NONE
*/
void
FGlobalAudio::Tick(void)
{
	guard(FGlobalAudio::Tick);

// Don't log here unless necessary for debugging,
// as this is called *very* often.
//	plog("FGlobalAudio::Tick()\n");

	/* Update count of times called. */
	num_ticks++;

#ifdef TEST
#if 0
	// Test SfxMoveActor
	{
//		FVector *Src = new FVector((FLOAT)17.0, (FLOAT)-5.0, (FLOAT)-95.0);
		FVector *Src = new FVector((FLOAT)60.0, (FLOAT)-108.0, (FLOAT)-113.0);
		SfxMoveActor(
			4095,		/* Actor. */
			Src, 		/* Where. */
//			0.25);		/* Fscale. */
			1.0);		/* Fscale. */
	}
#endif
#endif /* TEST */

	/* Force sound engine update. */
	SoundEngUpdate();

	unguard;
} /* End FGlobalAudio::Tick() */

/*
** FGlobalAudio::SetOrigin:
** Sets the current player position and rotation that is used for
** calculating volume levels and pan positions for game sound
** effects.
**
** Parameters:
**	Name	Description
**	----	-----------
**	Where	Location of player in map.
**	Angles	Which direction the player is facing.
**
** Returns:
**	NONE
*/
void
FGlobalAudio::SetOrigin(const FVector *Where, const FRotation *Angles)
{
	guard(FGlobalAudio::SetOrigin);

	SoundEngSetOrigin(Where->X, Where->Y, Where->Z,
			Angles->Yaw, Angles->Pitch, Angles->Roll);

	unguard;
} /* End FGlobalAudio::SetOrigin() */

/*
** FGlobalAudio::PlaySfxOrigined:
** Plays a sound effect, calculating the volume and pan
** position based on the sound source location provided
** as an argument, and the last known player location and
** angles that were set by the most recent call to
** FGlobalAudio::SetOrigin().
** NOTE that this function has been superseded by
** ::PlaySfxLocated(), but remains for compatibility
** with prior versions of source.
**
** Parameters:
**	Name		Description
**	----		-----------
**	Source		Position of sound source in map.
**	usnd		Pointer to USound object to be played.
**	SoundRadius	Radius in world units at which the
**			sound becomes inaudible.
**	fScale		Volume scalar.  The sound effect's
**			calculated volume will be multiplied
**			by this value (1.0 = normal,
**			0.0 = silent).
**	fPitchmod	Pitch multiplier.  The sound effect's
**			pitch will be multiplied by this
**			value (to raise or lower the pitch
**			accordingly; 1.0 = normal pitch).
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Playback ID number that can be used
**		in calls to SfxStop(), etc.
*/
INT
FGlobalAudio::PlaySfxOrigined(const FVector *Source, USound *usnd,
					const FLOAT SoundRadius,
					const FLOAT fScale,
					const FLOAT fPitchmod)
{
	guard(FGlobalAudio::PlaySfxOrigined);
	INT	pid;

	/* Check for bogus arguments. */
	if (usnd == NULL || Source == NULL)
	{
		return -1;
	}

	/* Play the sound. */
	pid = SoundEngSoundPlayLocatedEx(
				usnd->SoundID,	/* Registered ID of sound */
				1,		/* actor ID */
				Source->X,	/* World location */
				Source->Y,
				Source->Z,
				fPitchmod,
				SoundRadius,
				fScale);
//	SoundEngSoundSetRadius(pid, SoundRadius);
//	SoundEngSoundScaleVolume(pid, fScale);

	return pid;

	unguard;
} /* End FGlobalAudio::PlaySfxOrigined() */

/*
** FGlobalAudio::PlaySfxLocated:
** Plays a sound effect, calculating the volume and pan
** position based on the sound source location provided
** as an argument, and the last known player location and
** angles that were set by the most recent call to
** FGlobalAudio::SetOrigin().
**
** Parameters:
**	Name		Description
**	----		-----------
**	Source		Position of sound source in map.
**	usnd		Pointer to USound object to be played.
**	iActor		Unique number (usually actor index) that
**			identifies the actor/thing/object/etc
**			that made this sound.  May be -1 for
**			sounds that are not associated with
**			anything in particular.
**	SoundRadius	Radius in world units at which the
**			sound becomes inaudible.
**	fScale		Volume scalar.  The sound effect's
**			calculated volume will be multiplied
**			by this value (1.0 = normal,
**			0.0 = silent).
**	fPitchmod	Pitch multiplier.  The sound effect's
**			pitch will be multiplied by this
**			value (to raise or lower the pitch
**			accordingly; 1.0 = normal pitch).
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Playback ID number that can be used
**		in calls to SfxStop(), etc.
*/
INT
FGlobalAudio::PlaySfxLocated(const FVector *Source, USound *usnd,
				const INT iActor,
				const FLOAT SoundRadius,
				const FLOAT fScale,
				const FLOAT fPitchmod)
{
	guard(FGlobalAudio::PlaySfxLocated);
	INT	pid;

	/* Check for bogus arguments. */
	if (usnd == NULL || Source == NULL)
	{
		return -1;
	}

	/* Play the sound. */
	pid = SoundEngSoundPlayLocatedEx(
				usnd->SoundID,	/* Registered ID of sound */
				iActor,		/* actor ID */
				Source->X,	/* World location */
				Source->Y,
				Source->Z,
				fPitchmod,
				SoundRadius,
				fScale);
//	SoundEngSoundSetRadius(pid, SoundRadius);
//	SoundEngSoundScaleVolume(pid, fScale);

	return pid;

	unguard;
} /* End FGlobalAudio::PlaySfxLocated() */

/*
** FGlobalAudio::PlaySfxPrimitive:
** Plays a sound effect.  This is similar to PlaySfx(), but
** allows the caller to specify a fixed volume and pan
** position.  This would generally be used for menu or
** error message beeps, not in-game sound effects.
**
** Parameters:
**	Name		Description
**	----		-----------
**	usnd		Pointer to the USound to be played.
**	fVolume		Volume level (0.0 = silent, 1.0 =
**			maximum).
**	fPitchmod	Pitch multiplier.  The sound effect's
**			pitch will be multiplied by this
**			value (to raise or lower the pitch
**			accordingly; 1.0 = normal pitch).
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Playback ID number that can be used
**		in calls to SfxStop(), etc.
*/
INT
FGlobalAudio::PlaySfxPrimitive(USound *usnd,
			const INT fVolume,
			const FLOAT fPitchmod)
{
	guard(FGlobalAudio::PlaySfxPrimitive);
	INT	pid;

	/* Check for bogus arguments. */
	if (usnd == NULL)
	{
		return -1;
	}

	/* Play the sound. */
	pid = SoundEngSoundPlayEx(usnd->SoundID,
			(int)((float)CSOUND_MAX_VOLUME * fVolume),
			CSOUND_PAN_CENTER,
			fPitchmod);

	return pid;

	unguard;
} /* End FGlobalAudio::PlaySfxPrimitive() */

/*
** FGlobalAudio::SfxStop:
** Stops playing a sound effect that was started
** by a previous call to ::PlaySfxPrimitive,
** ::PlaySfxOrigined, or ::PlaySfxLocated.
**
** Parameters:
**	Name	Description
**	----	-----------
**	iPlay	Playback ID number from previous
**		::PlaySfx... call.
**
** Returns:
**	NONE
*/
void
FGlobalAudio::SfxStop(INT iPlay)
{
	SoundEngSoundStop(iPlay);
} /* End FGlobalAudio::SfxStop() */

/*
** FGlobalAudio::SfxStopActor:
** Stops playback of any sound effects that were
** started by a prior call to ::PlaySfxLocated()
** with a particular iActor argument.
**
** Parameters:
**	Name	Description
**	----	-----------
**	iActor	Actor index or other identifying
**		number as supplied to a prior call
**		to ::PlaySfxLocated().
**
** Returns:
**	NONE
*/
void
FGlobalAudio::SfxStopActor(const INT iActor)
{
	SoundEngActorStop(iActor);
} /* End FGlobalAudio::SfxStopActor() */

/*
** FGlobalAudio::SfxMoveActor:
** Moves any sound effects associated with a particular
** actor in the virtual listening space.  This should be
** called periodically for each actor to keep the sounds
** moving along with the actor's movement.
**
** Parameters:
**	Name	Description
**	----	-----------
**	iActor	Actor index or other identifying
**		number as supplied to a prior call
**		to ::PlaySfxLocated().
**	Where	Position of actor in map.
**
** Returns:
**	NONE
*/
void
FGlobalAudio::SfxMoveActor(const INT iActor, const FVector *Where,
			const FLOAT fScale)
{
	SoundEngActorMove(iActor,
			Where->X,
			Where->Y,
			Where->Z,
			fScale);
} /* End FGlobalAudio::SfxMoveActor() */

/*
** FGlobalAudio::MusicVolumeSet:
** Changes the volume level for music playback.
**
** Parameters:
**	Name	Description
**	----	-----------
**	NewVol	New volume level (0..MAX_MUSIC_VOLUME)
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Previous volume level for music playback.
*/
INT
FGlobalAudio::MusicVolumeSet(INT NewVol)
{
	guard(FGlobalAudio::MusicVolumeSet);

	if (NewVol < 0)
		NewVol = 0;
	if (NewVol > MAX_MUSIC_VOLUME)
		NewVol = MAX_MUSIC_VOLUME;
	return SoundEngMusicVolumeSet(NewVol);

	unguard;
} /* End FGlobalAudio::MusicVolumeSet() */

/*
** FGlobalAudio::MusicVolumeGet:
** Retrieves the current volume level for music playback.
**
** Parameters:
**	NONE
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Current volume level for music playback.
*/
INT
FGlobalAudio::MusicVolumeGet(void)
{
	guard(FGlobalAudio::MusicVolumeGet);

	return SoundEngMusicVolumeGet();

	unguard;
} /* End FGlobalAudio::MusicVolumeGet() */

/*
** FGlobalAudio::SfxVolumeSet:
** Changes the volume level for Sfx playback.
**
** Parameters:
**	Name	Description
**	----	-----------
**	NewVol	New volume level (0..MAX_Sfx_VOLUME)
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Previous volume level for Sfx playback.
*/
INT
FGlobalAudio::SfxVolumeSet(INT NewVol)
{
	guard(FGlobalAudio::SfxVolumeSet);

	if (NewVol < 0)
		NewVol = 0;
	if (NewVol > MAX_SFX_VOLUME)
		NewVol = MAX_SFX_VOLUME;
	return SoundEngSfxVolumeSet(NewVol);

	unguard;
} /* End FGlobalAudio::SfxVolumeSet() */

/*
** FGlobalAudio::SfxVolumeGet:
** Retrieves the current volume level for Sfx playback.
**
** Parameters:
**	NONE
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Current volume level for Sfx playback.
*/
INT
FGlobalAudio::SfxVolumeGet(void)
{
	guard(FGlobalAudio::SfxVolumeGet);

	return SoundEngSfxVolumeGet();

	unguard;
} /* End FGlobalAudio::SfxVolumeGet() */

/*
** MusicFade:
** Starts the music fading in or fading out.
** The fade takes about two seconds.
**
** Parameters:
**	Name		Description
**	----		-----------
**	fFadeOut	Flag; nonzero if fade-out is to be started.
**			Zero if fade-in is to be started.
**
** Returns:
**	NONE
*/
void
FGlobalAudio::MusicFade(INT fFadeOut)
{
	fFadeOut &= 1;
	SoundEngMusicFade(fFadeOut);
} /* End FGlobalAudio::MusicFade() */

/*
** MusicFade:
** Starts the sound effects fading in or fading out.
** The fade takes about two seconds.
**
** Parameters:
**	Name		Description
**	----		-----------
**	fFadeOut	Flag; nonzero if fade-out is to be started.
**			Zero if fade-in is to be started.
**
** Returns:
**	NONE
*/
void
FGlobalAudio::SfxFade(INT fFadeOut)
{
	fFadeOut &= 1;
	SoundEngSfxFade(fFadeOut);
} /* End FGlobalAudio::SfxFade() */

/*
** ReverbParamsSet:
** Specifies parameters for reverberation (sound reflection).
** The current reverberation parameters are applied to all
** sound effects except continuously looping samples.
**
** Parameters:
**	Name	Description
**	----	-----------
**	Space	Value ranging from zero to about 1000.
**		Zero means no reverb.  Larger numbers
**		represent increasing "room sizes" (i.e.
**		increasing time delays between audible
**		reflections).
**	Depth	Value ranging from 1 to 99, specifying
**		the amount of regeneration, where one
**		is minimum (not audible), and 99 is
**		maximum (ridiculously reflective).
**		Typical depth values are in the range
**		of about 25 to 50.  Higher values might
**		be used for wierd special effects.
**		Values less than 25 are almost inaudible,
**		thus not very useful.
**
** Returns:
**	NONE
*/
void
FGlobalAudio::ReverbParamsSet(INT Space, INT Depth)
{
	if (Space < 0)
		Space = 0;
	if (Space > 5000)
		Space = 5000;
	if (Depth < 1)
		Depth = 1;
	if (Depth > 99)
		Depth = 99;
	SoundEngReverbParamsSet(Space, Depth);
} /* End FGlobalAudio::ReverbParamsSet() */

/*
** FGlobalAudio::DirectSoundFlagGet:
** Retrieves the current setting of the DirectSound enable
** flag.
**
** Parameters:
**	NONE
**
** Returns:
**	Value	Meaning
**	-----	-------
**	1	Unreal will use DirectSound for playback.
**	0	Unreal will not use DirectSound.
*/
INT
FGlobalAudio::DirectSoundFlagGet()
{
	guard(FGlobalAudio::DirectSoundFlagGet);

	return SoundEngDirectSoundFlagGet();

	unguard;
} /* End FGlobalAudio::DirectSoundFlagGet() */

/*
** FGlobalAudio::DirectSoundFlagSet:
** Changes the DirectSound enable flag.  The change
** will not take effect until ::Init() is called.
**
** Parameters:
**	Name	Description
**	----	-----------
**	val	New enable setting.
**		0 means don't use DirectSound.
**		Other means use DirectSound.
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Previous setting of DirectSound flag.
*/
INT
FGlobalAudio::DirectSoundFlagSet(INT val)
{
	guard(FGlobalAudio::DirectSoundFlagSet);

	val &= 1;
	return SoundEngDirectSoundFlagSet(val);

	unguard;
} /* End FGlobalAudio::DirectSoundFlagSet() */

/*
** FGlobalAudio::DirectSoundOwnerWindowSet:
** Specifies to the audio system the handle of the
** window that "owns" the DirectSound subsystem.
** This is required in order for DirectSound to be
** utilized.
**
** Parameters:
**	Name	Description
**	----	-----------
**	hWnd	Window handle (cast to void *) of the
**		window that is to be the DirectSound
**		"owner" window (i.e. Unreal's main
**		display window).
**
** Returns:
**	NONE
*/
void
FGlobalAudio::DirectSoundOwnerWindowSet(void *hWnd)
{
	SoundEngSetDirectSoundOwnerWnd(hWnd);
} /* End FGlobalAudio::DirectSoundOwnerWindowSet() */

/*
** FGlobalAudio::FilterFlagGet:
** Retrieves the current setting of the sound system's
** filter enable flag.
**
** Parameters:
**	NONE
**
** Returns:
**	Value	Meaning
**	-----	-------
**	1	Filtering is enabled.
**	0	Filtering is not enabled.
*/
INT
FGlobalAudio::FilterFlagGet(void)
{
	guard(FGlobalAudio::FilterFlagGet);

	return SoundEngFilterFlagGet();

	unguard;
} /* End FGlobalAudio::FilterFlagGet() */

/*
** FGlobalAudio::FilterFlagSet:
** Changes the sound system's filter enable flag.  The change
** will not take effect until ::Init() is called.
**
** Parameters:
**	Name	Description
**	----	-----------
**	val	New enable setting.
**		0 means don't use filter.
**		Other means use filter.
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Previous setting of filter flag.
*/
INT
FGlobalAudio::FilterFlagSet(INT val)
{
	guard(FGlobalAudio::FilterFlagSet);

	val &= 1;
	return SoundEngFilterFlagSet(val);

	unguard;
} /* End FGlobalAudio::FilterFlagSet() */

/*
** FGlobalAudio::Use16BitFlagGet:
** Retrieves the current setting of the sound system's
** 16-bit playback enable flag.
**
** Parameters:
**	NONE
**
** Returns:
**	Value	Meaning
**	-----	-------
**	1	16-bit playback is enabled.
**	0	16-bit playback is not enabled.
*/
INT
FGlobalAudio::Use16BitFlagGet(void)
{
	guard(FGlobalAudio::Use16BitFlagGet);

	return SoundEng16BitFlagGet();

	unguard;
} /* End FGlobalAudio::Use16BitFlagGet() */

/*
** FGlobalAudio::Use16BitFlagSet:
** Changes the sound system's 16-bit playback enable flag.
** The change will not take effect until ::Init() is called.
**
** Parameters:
**	Name	Description
**	----	-----------
**	val	New enable setting.
**		0 means don't use 16-bit playback.
**		Other means use 16-bit playback.
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Previous setting of 16-bit playback flag.
*/
INT
FGlobalAudio::Use16BitFlagSet(INT val)
{
	guard(FGlobalAudio::Use16BitFlagSet);

	val &= 1;
	return SoundEng16BitFlagSet(val);

	unguard;
} /* End FGlobalAudio::Use16BitFlagSet() */

/*
** FGlobalAudio::SurroundFlagGet:
** Retrieves the current setting of the sound system's
** surround sound enable flag.
**
** Parameters:
**	NONE
**
** Returns:
**	Value	Meaning
**	-----	-------
**	1	Surround effects are enabled.
**	0	Surround effects are not enabled.
*/
INT
FGlobalAudio::SurroundFlagGet(void)
{
	guard(FGlobalAudio::SurroundFlagGet);

	return SoundEngSurroundFlagGet();

	unguard;
} /* End FGlobalAudio::SurroundFlagGet() */

/*
** FGlobalAudio::SurroundFlagSet:
** Changes the sound system's surround sound enable flag.
**
** Parameters:
**	Name	Description
**	----	-----------
**	val	New enable setting.
**		0 means don't enable surround effects.
**		Other means enable surround effects.
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Previous setting of flag.
*/
INT
FGlobalAudio::SurroundFlagSet(INT val)
{
	guard(FGlobalAudio::SurroundFlagSet);

	val &= 1;
	return SoundEngSurroundFlagSet(val);

	unguard;
} /* End FGlobalAudio::SurroundFlagSet() */

/*
** FGlobalAudio::MixingRateSet:
** Changes the current audio driver mixing rate in Hertz.
**
** Parameters:
**	Name	Description
**	----	-----------
**	val	New mixing rate in Hertz.
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Prior setting of audio driver mixing rate in Hertz.
*/
INT
FGlobalAudio::MixingRateSet(INT val)
{
	guard(FGlobalAudio::MixingRateSet);

	if (val < 11000)
		val = 11000;
	if (val > 44100)
		val = 44100;
	return SoundEngMixingRateSet(val);

	unguard;
} /* End FGlobalAudio::MixingRateSet() */

/*
** FGlobalAudio::MixingRateGet:
** Retrieves the current audio driver mixing rate in Hertz.
**
** Parameters:
**	NONE
**
** Returns:
**	Value	Meaning
**	-----	-------
**	any	Current audio driver mixing rate in Hertz.
*/
INT
FGlobalAudio::MixingRateGet()
{
	guard(FGlobalAudio::MixingRateGet);

	return SoundEngMixingRateGet();

	unguard;
} /* End FGlobalAudio::MixingRateGet() */

/*
** FGlobalAudio::Pause:
** Pauses the sound output (by calling Galaxy's StopOutput(), if
** Galaxy is running).
**
** Parameters:
**	NONE
**
** Returns:
**	NONE
*/
void
FGlobalAudio::Pause(void)
{
	guard(FGlobalAudio::Pause);

	SoundEngPause();

	unguard;
} /* End FGlobalAudio::Pause() */

/*
** FGlobalAudio::UnPause:
** Resumes sound output after a previous call to ::Pause().
**
** Parameters:
**	NONE
**
** Returns:
**	NONE
*/
void
FGlobalAudio::UnPause(void)
{
	guard(FGlobalAudio::UnPause);

	SoundEngUnPause();

	unguard;
} /* End FGlobalAudio::UnPause() */

/*
** FGlobalAudio::SpecifySong:
** Specifies which song will be played the next time ::InitLevel()
** is called.
**
** Parameters:
**	Name	Description
**	----	-----------
**	pSong	Pointer to UMusic object containing the song
**		to be played.
**
** Returns:
**	NONE
*/
void
FGlobalAudio::SpecifySong(UMusic *pSong)
{
	guard(FGlobalAudio::SpecifySong);

	if (pSong == NULL)
		SoundEngMusicSpecifySong((char *)NULL, 0);
	else
		SoundEngMusicSpecifySong((char *)pSong->GetData(), 0);

	unguard;
} /* End FGlobalAudio::SpecifySong() */

/*
** MusicMute:
** Turns muting of music on or off.
**
** Parameters:
**	Value	Meaning
**	-----	-------
**	bMute	Flag; 0 = unmute; 1 = mute.
**
** Returns:
**	NONE
*/
void
FGlobalAudio::MusicMute(INT bMute)
{
	bMute &= 1;
	SoundEngMusicMute(bMute);
} /* End FGlobalAudio::MusicMute() */

/*
** MusicIsMuted:
** Determines if music is currently muted.
**
** Parameters:
**	NONE
**
** Returns:
**	Value	Meaning
**	-----	-------
**	1	Muted.
**	0	Not muted.
*/
INT
FGlobalAudio::MusicIsMuted(void)
{
	return SoundEngMusicMute(-1);
} /* End FGlobalAudio::MusicIsMuted() */

/*
** SfxMute:
** Turns muting of sound effects on or off.
**
** Parameters:
**	Value	Meaning
**	-----	-------
**	bMute	Flag; 0 = unmute; 1 = mute.
**
** Returns:
**	NONE
*/
void
FGlobalAudio::SfxMute(INT bMute)
{
	bMute &= 1;
	SoundEngSfxMute(bMute);
} /* End FGlobalAudio::SfxMute() */

/*
** SfxIsMuted:
** Determines if sound effects are currently muted.
**
** Parameters:
**	NONE
**
** Returns:
**	Value	Meaning
**	-----	-------
**	1	Muted.
**	0	Not muted.
*/
INT
FGlobalAudio::SfxIsMuted(void)
{
	return SoundEngSfxMute(-1);
} /* End FGlobalAudio::SfxIsMuted() */

/*
** PanExaggerationSet:
** Specify exaggeration factor for panning calculations
** for located sounds.
**
** Parameters:
**	Name	Description
**	----	-----------
**	val	Exaggeration multiplier.
**		1.0 = normal
**		0.5 = less panning
**		2.0 = more panning
**
** Returns:
**	NONE
*/
void
FGlobalAudio::PanExaggerationSet(FLOAT val)
{
	SoundEngPanExaggerationSet((double)val);
} /* End FGlobalAudio::PanExaggerationSet() */

/*
** DopplerExaggerationSet:
** Specify exaggeration factor for Doppler shift effects
** for located sounds.
**
** Parameters:
**	Name	Description
**	----	-----------
**	val	Exaggeration multiplier.
**		1.0 = normal shifting
**		0.5 = less shifting
**		2.0 = more shifting
**
** Returns:
**	NONE
*/
void
FGlobalAudio::DopplerExaggerationSet(FLOAT val)
{
	SoundEngDopplerExaggerationSet((double)val);
} /* End FGlobalAudio::DopplerExaggerationSet() */

/*******************************************************************
                      Unreal console handler
*******************************************************************/

/*
** FGlobalAudio::Exec:
** Handles commands from the Unreal game console.
**
** Parameters:
**	Name	Description
**	----	-----------
**	Cmd	Command line typed by the user.
**	Out	Where the console output goes.
**
** Returns:
**	Value	Meaning
**	-----	-------
**	1	Command was parsed here.
**	0	Command was not parsed here.
*/
int
FGlobalAudio::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(FGlobalAudio::Exec);
	const char *Str = Cmd;
	char stmp[80];
	char stmp2[80];

	if (GetCMD(&Str, "AUDIOLOG"))
	{
		/*
		** Specify debug level for sound engine.
		*/
		if (GetSTRING(Str, "LEVEL=", stmp, 80))
		{
			SoundEngLogDetail(atoi(stmp));
			sprintf(stmp2, "Sound system debug level specified:  %d", atoi(stmp));
			Out->Log(stmp2);
		}
		else
		{
			SoundEngLogDetail(1);
			Out->Log("Sound system debug level reset");
		}
		return 1;
	}
	else
	{
		return 0;
	}
	return 0;

	unguard;
} /* End FGlobalAudio::Exec() */

/*
=============================================================================
End UnFGAud.cpp
=============================================================================
*/
