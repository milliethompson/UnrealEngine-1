/*==========================================================================
FILENAME:     UnFGAud.h
DESCRIPTION:  Declarations of the "FGlobalAudio" class and related
              routines.
NOTICE:       Copyright 1996 Epic MegaGames, Inc. This software is a
              trade secret.
TOOLS:        Compiled with Visual C++ 4.0
FORMAT:       8 characters per tabstop, 100 characters per line.
HISTORY:
  When      Who                 What
  ----      ---                 ----
  ??/??/96  Tim Sweeney         Created stubs for this module.
  04/18/96  Ammon R. Campbell   Misc. hacks started.
==========================================================================*/

#ifndef _INC_UNFGAUD /* Prevent header from being included multiple times */
#define _INC_UNFGAUD

/******************************* CONSTANTS *****************************/

/*
** Highest possible volume level for music,
** for MusicVolumeSet/Get calls.
*/
#define MAX_MUSIC_VOLUME	127

/*
** Highest possible volume level for sound effects,
** for SfxVolumeSet/Get calls.
*/
#define MAX_SFX_VOLUME		127

/***************************** TYPES/CLASSES ***************************/

/*
** FGlobalAudio:
** The class that contains the functions called by
** the Unreal engine to initialize, drive, and shut
** down the sound module.
*/
class UNENGINE_API FGlobalAudio
{
public:
	/*
	** Member functions:
	*/

	/* Performs once-per-instance initialization of sound stuff. */
	int Init(int Active);

	/* Performs once-per-instace shutdown of sound stuff. */
	void Exit(void);

	/* Execute a command. */
	int Exec(const char *Stream,FOutputDevice *Out);

	/* Initialize prior to playing a map. */
	int InitLevel(int MaxIndices);

	/* Clean up after playing a map. */
	void ExitLevel(void);

	/* Reinitializes the audio system; use after changing
        ** settings that require a restart of the system, like
	** DirectSound flag, mixing rate, etc. */
	void Restart(void);

	/* Called about 35 times per second to update sound stuff. */
	void Tick(void);

	/* Called by ULevel::Tick() to specify current player location. */
	void SetOrigin(const FVector *Where, const FRotation *Angles);

	/* Called to play a sound effect (see "UnFGAud.cpp"). */
	/* Note: SoundRadius==0 means the radius parameter should be ignored */
	INT PlaySfxOrigined(const FVector *Source, USound *usnd,
				const FLOAT SoundRadius=0.f,
				const FLOAT fScale=1.0,
				const FLOAT fPitchmod=1.0);
	INT PlaySfxPrimitive(USound *usnd,
				const INT fVolume=1.0,
				const FLOAT fPitchmod=1.0);
	INT PlaySfxLocated(const FVector *Source, USound *usnd,
				const INT iActor,
				const FLOAT SoundRadius=0.f,
				const FLOAT fScale=1.0,
				const FLOAT fPitchmod=1.0);

	/* Called to modify or stop sound effects. */
	void SfxStop(INT iPlay);
	void SfxStopActor(const INT iActor);
	void SfxMoveActor(const INT iActor, const FVector *Where, const FLOAT fScale=1.0);

	/* Volume get/set routines. */
	INT MusicVolumeGet(void);
	INT MusicVolumeSet(INT NewVol);
	INT SfxVolumeGet(void);
	INT SfxVolumeSet(INT NewVol);

	/* Start fade-in or fade-out. */
	void MusicFade(INT fFadeOut);	/* 1 = fade in; 0 = fade out */
	void SfxFade(INT fFadeOut);	/* 1 = fade in; 0 = fade out */

	/* Specify reverb parameters. */
	/*   Space = 0 (none) to ~1000 (huge cave) */
	/*   Depth = 1 (no regen) to 99 (max regen); 25-50 most useful. */
	void ReverbParamsSet(INT Space, INT Depth);

	/* Enable/disable DirectSound in Galaxy. */
	INT DirectSoundFlagGet(void);
	INT DirectSoundFlagSet(INT val);

	/* Enable/disable Galaxy interpolation filter. */
	INT FilterFlagGet(void);
	INT FilterFlagSet(INT val);

	/* Enable/disable Galaxy 16-bit playback mode. */
	INT Use16BitFlagGet(void);
	INT Use16BitFlagSet(INT val);

	/* Enable/disable surround sound effects. */
	INT SurroundFlagGet(void);
	INT SurroundFlagSet(INT val);

	/* Set sound driver mixing rate. */
	INT MixingRateSet(INT val);
	INT MixingRateGet(void);

	/* Specify which song to play during game. */
	void SpecifySong(UMusic *pSong);

	/* Temporarily stop/start sound during playback. */
	void Pause(void);
	void UnPause(void);

/*
private:
See notes in "UnFGAud.cpp" for psuedo-private stuff
*/
};

/******************************* EDITOR HELPERS *****************************/

void UNEDITOR_API
AudioCmdLine(const char *Str,FOutputDevice *Out);

void UNEDITOR_API
MusicCmdLine(const char *Str, FOutputDevice *Out);

#endif // _INC_UNFGAUD

/*
==========================================================================
End UnFGAud.h
==========================================================================
*/
