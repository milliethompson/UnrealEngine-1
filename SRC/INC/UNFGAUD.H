/*==========================================================================
FILENAME:     UnFGAud.h
DESCRIPTION:  Declarations of the "FGlobalAudio" class and related
              routines.
NOTICE:       Copyright 1996 Epic MegaGames, Inc. This software is a
              trade secret.
TOOLS:        Compiled with Visual C++ 4.0, Calling method=__fastcall
FORMAT:       8 characters per tabstop, 100 characters per line.
HISTORY:
  When      Who                 What
  ----      ---                 ----
  ??/??/96  Tim Sweeney         Created stubs for this module.
  04/18/96  Ammon R. Campbell   Misc. hacks started.
  05/12/96  Ammon R. Campbell   Added volume get/set routines.
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
class UNREAL_API FGlobalAudio
{
public:
	/*
	** Member functions:
	*/

	/* Performs once-per-instance initialization of sound stuff. */
	int Init(int Active);

	/* Performs once-per-instace shutdown of sound stuff. */
	void Exit(void);

	/* Initialize prior to playing a map. */
	int InitLevel(int MaxIndices);

	/* Clean up after playing a map. */
	void ExitLevel(void);

	/* Called about 35 times per second to update sound stuff. */
	void Tick(void);

	/* Called by ULevel::Tick() to specify current player location. */
	void SetOrigin(const FVector *Where, const FRotation *Angles);

	/* Called to play a sound effect (see "UnFGAud.cpp"). */
	/* Note: SoundRadius==0 means the radius parameter should be ignored */
	INT PlaySfxOrigined(const FVector *Source, USound *usnd,
				const FLOAT SoundRadius=0.f,
				const FLOAT fScale=1.0);
	INT PlaySfxPrimitive(USound *usnd);
	INT PlaySfxLocated(const FVector *Source, USound *usnd,
				const INT iActor,
				const FLOAT SoundRadius=0.f,
				const FLOAT fScale=1.0);

	/* Called to modify or stop sound effects. */
	void SfxStop(INT iPlay);
	void SfxStopActor(const INT iActor);
	void SfxMoveActor(const INT iActor, const FVector *Where, const FLOAT fScale=1.0);

	/* Volume get/set routines. */
	INT MusicVolumeGet(void);
	INT MusicVolumeSet(INT NewVol);
	INT SfxVolumeGet(void);
	INT SfxVolumeSet(INT NewVol);

	/* Enable/disable DirectSound in Galaxy. */
	INT DirectSoundFlagGet(void);
	INT DirectSoundFlagSet(INT val);

	/* Enable/disable Galaxy interpolation filter. */
	INT FilterFlagGet(void);
	INT FilterFlagSet(INT val);

	/* Set sound driver mixing rate. */
	INT MixingRateSet(INT val);
	INT MixingRateGet(void);

	/* Specify which song to play during game. */
	void SpecifySong(int iSong);

	/* Temporarily stop/start sound during playback. */
	void Pause(void);
	void UnPause(void);

	/* Console exec function. */
	int Exec(const char *Cmd, FOutputDevice *Out);

/*
private:
See notes in "UnFGAud.cpp" for psuedo-private stuff
*/
};

#endif // _INC_UNFGAUD

/*
==========================================================================
End UnFGAud.h
==========================================================================
*/
