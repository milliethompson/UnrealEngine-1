/*=============================================================================
	Galaxy.cpp: Unreal texture and illumination dithering setup.
	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.

Revision history:
	* Created by Tim Sweeney.
=============================================================================*/

/*------------------------------------------------------------------------------------
	Dependencies.
------------------------------------------------------------------------------------*/

#pragma warning (disable:4201)
#include <windows.h>
#include <mmsystem.h>
#include "Engine.h"
#include "UnRender.h"

/*------------------------------------------------------------------------------------
	Galaxy includes.
------------------------------------------------------------------------------------*/

#include "Galaxy.h"
extern "C"
{
	extern ubyte MMXFound;
	extern void* A3D;
}

/*------------------------------------------------------------------------------------
	Constants.
------------------------------------------------------------------------------------*/

//
// Constants.
//
#define MAX_EFFECTS_CHANNELS 32
#define MUSIC_CHANNELS       32
#define EFFECT_FACTOR        1.0

//
// Macros.
//
#define safecall(f) \
{ \
	guard(f); \
	INT Error=f; \
	if( Error ) \
		debugf( NAME_Warning, "%s failed: %i", #f, Error ); \
	unguard; \
}
#define silentcall(f) \
{ \
	guard(f); \
	f; \
	unguard; \
}

//
// Information about a playing sound.
//
class FPlayingSound
{
public:
	glxChannel*	Channel;
	AActor*		Actor;
	INT			Id;
	UBOOL		Is3D;
	USound*		Sound;
	FVector		Location;
	FLOAT		Volume;
	FLOAT		Radius;
	FLOAT		Pitch;
	FLOAT		Priority;
	FPlayingSound()
	:	Channel	(NULL)
	,	Actor	(NULL)
	,	Id		(0)
	,	Is3D	(0)
	,	Sound	(0)
	,	Priority(0)
	{}
	FPlayingSound( AActor* InActor, INT InId, USound* InSound, FVector InLocation, FLOAT InVolume, FLOAT InRadius, FLOAT InPitch, FLOAT InPriority )
	:	Channel	(NULL)
	,	Actor	(InActor)
	,	Id		(InId)
	,	Is3D    (0)
	,	Sound	(InSound)
	,	Location(InLocation)
	,	Volume	(InVolume)
	,	Radius	(InRadius)
	,	Pitch	(InPitch)
	,	Priority(InPriority)
	{}
};

//
// The Galaxy implementation of UAudioSubsystem.
//
class DLL_EXPORT UGalaxyAudioSubsystem : public UAudioSubsystem
{
	DECLARE_CLASS(UGalaxyAudioSubsystem,UAudioSubsystem,CLASS_Config)

	// Configuration.
	UBOOL			UseDirectSound;
	UBOOL			UseFilter;
	UBOOL			UseSurround;
	UBOOL			UseStereo;
	UBOOL			UseCDMusic;
	UBOOL			UseDigitalMusic;
	UBOOL			UseSpatial;
	UBOOL			UseReverb;
	UBOOL			Use3dHardware;
	UBOOL			ReverseStereo;
	UBOOL			LowSoundQuality;
	UBOOL			Initialized;
	INT				Latency;
	BYTE			OutputRate;
	INT				EffectsChannels;
	BYTE			MusicVolume;
	BYTE			SoundVolume;
	FLOAT			AmbientFactor;
	FLOAT			DopplerSpeed;
	DWORD			SavedWaveOutVolume;

	// Variables.
	UViewport*		Viewport;
	FPlayingSound	PlayingSounds[MAX_EFFECTS_CHANNELS];
	DOUBLE			LastTime;
	UMusic*			CurrentMusic;
	BYTE			CurrentCDTrack;
	BYTE			CurrentSection;
	glxReverb		CurrentReverb;
	INT				FreeSlot;
	FLOAT			MusicFade;

	// Constructor.
	UGalaxyAudioSubsystem();
	static void InternalClassInitializer( UClass* Class );

	// UObject interface.
	void Destroy();
	void PostEditChange();
	void ShutdownAfterError();

	// UAudioSubsystem interface.
	UBOOL Init();
	void SetViewport( UViewport* Viewport );
	UBOOL Exec( const char* Cmd, FOutputDevice* Out=GSystem );
	void Update( FPointRegion Region, FCoords& Coords );
	void UnregisterSound( USound* Sound );
	void UnregisterMusic( UMusic* Music );
	UBOOL PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch );
	void NoteDestroy( AActor* Actor );
	void RegisterSound( USound* Sound );
	void RegisterMusic( UMusic* Music ) {};
	UBOOL GetLowQualitySetting() {return LowSoundQuality;}

	// Internal functions.
	void SetVolumes();
	void StopSound( INT Index );
	FPlayingSound* FindActiveSound( INT Id, INT& Index );

	// Inlines.
	glxSample* GetSound( USound* Sound )
	{
		check(Sound);
		if( !Sound->Handle )
			RegisterSound( Sound );
		return (glxSample*)Sound->Handle;
	}
	FLOAT SoundPriority( UViewport* Viewport, FVector Location, FLOAT Volume, FLOAT Radius )
	{
		return Volume * (1.0 - (Location-Viewport->Actor->Location).Size()/Radius);
	}
};

/*-----------------------------------------------------------------------------
	Class and package registration.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UGalaxyAudioSubsystem);
IMPLEMENT_PACKAGE(Galaxy);

/*-----------------------------------------------------------------------------
	Sound control.
-----------------------------------------------------------------------------*/

UGalaxyAudioSubsystem::UGalaxyAudioSubsystem()
{
	guard(UGalaxyAudioSubsystem::UGalaxyAudioSubsystem);

	// Set defaults.
	MusicVolume			= 63;
	SoundVolume			= 127;
	AmbientFactor		= 1.0;
	UseDirectSound		= 1;
	UseFilter			= 1;
	UseSurround			= 1;
	UseStereo			= 1;
	UseCDMusic			= 1;
	UseDigitalMusic		= 1;
	UseSpatial			= 0;
	UseReverb			= 1;
	ReverseStereo		= 0;
	LowSoundQuality		= 0;
	Latency				= 40;
	OutputRate			= 1;
	EffectsChannels		= 16;
	DopplerSpeed		= 8000.0;

	// Set variables.
	MusicFade			= 1.0;
	CurrentCDTrack		= 255;
	LastTime			= appSeconds();

	unguard;
}

//
// Static class initializer.
//
void UGalaxyAudioSubsystem::InternalClassInitializer( UClass* Class )
{
	guard(UGalaxyAudioSubsystem::InternalClassInitializer);
	if( appStricmp( Class->GetName(), "GalaxyAudioSubsystem" )==0 )
	{
		UEnum* OutputRates = new( Class, "OutputRates" )UEnum( NULL );
		new( OutputRates->Names )FName( "11025Hz" );
		new( OutputRates->Names )FName( "22050Hz" );
		new( OutputRates->Names )FName( "44100Hz" );
		new(Class,"UseDirectSound",  RF_Public)UBoolProperty  (CPP_PROPERTY(UseDirectSound ), "Audio", CPF_Config );
		new(Class,"UseFilter",       RF_Public)UBoolProperty  (CPP_PROPERTY(UseFilter      ), "Audio", CPF_Config );
		new(Class,"UseSurround",     RF_Public)UBoolProperty  (CPP_PROPERTY(UseSurround    ), "Audio", CPF_Config );
		new(Class,"UseStereo",       RF_Public)UBoolProperty  (CPP_PROPERTY(UseStereo      ), "Audio", CPF_Config );
		new(Class,"UseCDMusic",      RF_Public)UBoolProperty  (CPP_PROPERTY(UseCDMusic     ), "Audio", CPF_Config );
		new(Class,"UseDigitalMusic", RF_Public)UBoolProperty  (CPP_PROPERTY(UseDigitalMusic), "Audio", CPF_Config );
		new(Class,"UseSpatial",      RF_Public)UBoolProperty  (CPP_PROPERTY(UseSpatial     ), "Audio", CPF_Config );
		new(Class,"UseReverb",       RF_Public)UBoolProperty  (CPP_PROPERTY(UseReverb      ), "Audio", CPF_Config );
		new(Class,"Use3dHardware",   RF_Public)UBoolProperty  (CPP_PROPERTY(Use3dHardware  ), "Audio", CPF_Config );
		new(Class,"ReverseStereo",   RF_Public)UBoolProperty  (CPP_PROPERTY(ReverseStereo  ), "Audio", CPF_Config );
		new(Class,"LowSoundQuality", RF_Public)UBoolProperty  (CPP_PROPERTY(LowSoundQuality), "Audio", CPF_Config );
		new(Class,"Latency",         RF_Public)UIntProperty   (CPP_PROPERTY(Latency        ), "Audio", CPF_Config );
		new(Class,"OutputRate",      RF_Public)UByteProperty  (CPP_PROPERTY(OutputRate     ), "Audio", CPF_Config, OutputRates );
		new(Class,"EffectsChannels", RF_Public)UIntProperty   (CPP_PROPERTY(EffectsChannels), "Audio", CPF_Config );
		new(Class,"MusicVolume",     RF_Public)UByteProperty  (CPP_PROPERTY(MusicVolume    ), "Audio", CPF_Config );
		new(Class,"SoundVolume",     RF_Public)UByteProperty  (CPP_PROPERTY(SoundVolume    ), "Audio", CPF_Config );
		new(Class,"AmbientFactor",   RF_Public)UFloatProperty (CPP_PROPERTY(AmbientFactor  ), "Audio", CPF_Config );
		new(Class,"DopplerSpeed",    RF_Public)UFloatProperty (CPP_PROPERTY(DopplerSpeed   ), "Audio", CPF_Config );
	}
	unguard;
}

UBOOL UGalaxyAudioSubsystem::Init()
{
	guard(UGalaxyAudioSubsystem::Init);

	// Preinit checks.
	check(GLX_TOTALCHANNELS==64);
	check(MAX_EFFECTS_CHANNELS+MUSIC_CHANNELS==GLX_TOTALCHANNELS);

	// Init wave audio.
	SavedWaveOutVolume=0;
	waveOutGetVolume(NULL,&SavedWaveOutVolume);
	waveOutSetVolume(NULL,65535+(65535<<16));

	// Initialize Galaxy.
	guard(glxInit);
	verify(glxInit()==0);
	unguard;

	// Handle MMX.
	if( !GIsMMX )
		MMXFound = 0;

	// Allocate channels for sound effects.
	guard(glxAllocateEffectChannels);
    verify(glxAllocateSampleChannels( EffectsChannels )!=0);
	unguard;

	// Detect hardware.
	guard(glxDetectOutput);
    if
	(	UseDirectSound
	&& !ParseParam(appCmdLine(), "nodsound")
	&&	glxDetectOutput(GLX_DIRECTSOUND,0)==GLXERR_NOERROR )
		debugf( NAME_Init, "Galaxy is using DirectSound" );
	else
		debugf( NAME_Init, "Galaxy is using WinMM" );
	unguard;

	// Handle 3d sound.
	if( A3D )
		debugf( NAME_Init, "Aureal A3D 3D sound hardware found!" );

	// Set initialized flag.
	USound::Audio = this;
	UMusic::Audio = this;
	Initialized=1;

	debugf( NAME_Init, "Galaxy initialized" );
	return 1;
	unguard;
}

void UGalaxyAudioSubsystem::PostEditChange()
{
	guard(UGalaxyAudioSubsystem::PostEditChange);

	// Validate configurable variables.
	OutputRate      = Clamp(OutputRate,(BYTE)0,(BYTE)2);
	Latency         = Clamp(Latency,10,250);
	EffectsChannels = Clamp(EffectsChannels,0,MAX_EFFECTS_CHANNELS);
	DopplerSpeed    = Clamp(DopplerSpeed,1.f,100000.f);
	AmbientFactor   = Clamp(AmbientFactor,0.f,10.f);
	SetVolumes();

	unguard;
}

void UGalaxyAudioSubsystem::SetViewport( UViewport* InViewport )
{
	guard(UGalaxyAudioSubsystem::SetViewport);
	debugf( NAME_DevAudio, "Galaxy SetViewport: %s", InViewport ? InViewport->GetName() : "NULL" );

	// Stop all playing sounds.
	memset( &CurrentReverb, 0, sizeof(CurrentReverb) );
	glxSetSampleReverb( &CurrentReverb );
	for( INT i=0; i<EffectsChannels; i++ )
		StopSound( i );

	// Remember the viewport.
	if( Viewport != InViewport )
	{
		if( Viewport )
		{
			// Unregister everything.
			debugf( "Galaxy unregister" );
			for( TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt )
				if( MusicIt->Handle )
					UnregisterMusic( *MusicIt );

			// Shut down Galaxy.
			safecall(glxStopOutput());
		}
		Viewport = InViewport;
		if( Viewport )
		{
			// Figure out startup parameters.
			debugf( "Galaxy reset viewport" );
			DWORD OutputMode = GLX_16BIT;
			if( UseFilter )
				OutputMode |= GLX_COSINE;
			if( UseStereo )
				OutputMode |= GLX_STEREO;
			if( Viewport->Actor->Song && Viewport->Actor->Transition==MTRAN_None )
				Viewport->Actor->Transition = MTRAN_Instant;

			// Start sound output.
			guard(glxStartOutput);
			check(Viewport->GetWindow());
			INT Rate = OutputRate==0 ? 11024 : OutputRate==1 ? 22050 : 44100;
			INT Result;
			try
			{
				Result = glxStartOutput( Viewport->GetWindow(), Rate, OutputMode, Latency );
			}
			catch( ... )
			{
				Result = -1;
			}
			if( Result!=0 )
			{
				// Initialization failed.
				debugf( NAME_Init, "Safely failed to initialize Galaxy: %i", Result );
				Viewport = NULL;
				return;
			}
			unguard;
			SetVolumes();
		}
	}
	unguard;
}

void UGalaxyAudioSubsystem::SetVolumes()
{
	guard(UGalaxyAudioSubsystem::SetVolumes);

	// Normalize the volumes.
	FLOAT NormSoundVolume = SoundVolume/255.0;
	FLOAT NormMusicVolume = Clamp(MusicVolume/255.0,0.0,1.0);

	// Set music and effects volumes.
	verify(glxSetSampleVolume(127*NormSoundVolume,GLX_VOLSET)==0);
	if( UseDigitalMusic )
		verify(glxSetMusicVolume(127*NormMusicVolume*Max(MusicFade,0.f),GLX_VOLSET)==0);
	if( UseCDMusic )
		glxSetCDVolume(127*NormMusicVolume*Max(MusicFade,0.f),GLX_VOLSET);

	unguard;
}

void UGalaxyAudioSubsystem::Destroy()
{
	guard(UGalaxyAudioSubsystem::Destroy);
	if( Initialized )
	{
		// Unhook.
		USound::Audio = NULL;
		UMusic::Audio = NULL;

		// Shut down viewport.
		SetViewport( NULL );

		// Stop CD.
		if( UseCDMusic && CurrentCDTrack!=255 )
			glxStopCDTrack();

		// Deinitialize sound.
		safecall(glxDeinit());

		// Set WaveOut volumes.
		waveOutSetVolume(NULL,SavedWaveOutVolume);

		debugf( NAME_Exit, "Galaxy shut down" );
	}
	UAudioSubsystem::Destroy();
	unguard;
}

void UGalaxyAudioSubsystem::ShutdownAfterError()
{
	guard(UGalaxyAudioSubsystem::ShutdownAfterError);

	// Unhook.
	USound::Audio = NULL;
	UMusic::Audio = NULL;

	// Safely shut down.
	debugf( NAME_Exit, "UGalaxyAudioSubsystem::ShutdownAfterError" );
	safecall(glxStopOutput());
	if( Viewport )
		safecall(glxDeinit());

	Super::ShutdownAfterError();
	unguard;
}

/*-----------------------------------------------------------------------------
	Sound and music registration.
-----------------------------------------------------------------------------*/

void UGalaxyAudioSubsystem::RegisterSound( USound* Sound )
{
	guard(UGalaxyAudioSubsystem::RegisterSound);
	debug(Sound);
	if( !Sound->Handle )
	{
		check(Sound->Data.Num()>0);
		debugf( NAME_DevSound, "Register sound: %s (%i)", Sound->GetName(), Sound->Data.Num() );

		// Register the sound.
		guard(glxLoadSample);
		Sound->Handle = glxLoadSample( &Sound->Data(0), GLX_LOADFROMMEMORY );
		if( !Sound->Handle )
			appErrorf( "Invalid sound format in %s", Sound->GetFullName() );
		unguard;

		// If in game, scrap the source data we no longer need.
		if( !GIsEditor )
			Sound->Data.Empty();
	}
	unguard;
}
void UGalaxyAudioSubsystem::UnregisterSound( USound* Sound )
{
	guard(UGalaxyAudioSubsystem::UnregisterSound);
	check(Sound);
	if( Sound->Handle )
	{
		debugf( NAME_DevSound, "Unregister sound: %s", Sound->GetFullName() );

		// Shut it up.
		for( INT i=0; i<EffectsChannels; i++ )
			if( PlayingSounds[i].Sound==Sound )
				StopSound( i );

		// Unload from Galaxy.
		safecall(glxUnloadSample( (glxSample*)Sound->Handle ));
	}
	unguard;
}
void UGalaxyAudioSubsystem::UnregisterMusic( UMusic* Music )
{
	guard(UGalaxyAudioSubsystem::UnregisterMusic);
	check(Music);
	if( Music->Handle )
	{
		check(Music==CurrentMusic);
		debugf( NAME_DevMusic, "Unregister music: %s", Music->GetFullName() );

		// Stop the current music, if it's playing (may return failure code).
		guard(glxStopMusic);
		glxStopMusic();
		unguard;

		// Unload the current music.
		safecall(glxUnloadMusic());

		// Disown.
		CurrentMusic = NULL;
		Music->Handle = NULL;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/

UBOOL UGalaxyAudioSubsystem::Exec( const char* Cmd, FOutputDevice* Out )
{
	guard(UGalaxyAudioSubsystem::Exec);
	if( Viewport && ParseCommand( &Cmd, "CDTRACK") )
	{
		INT i = atoi(Cmd);
		Out->Logf( "CD Track %i", i );
		Viewport->Actor->CdTrack = i;
		Viewport->Actor->Transition = MTRAN_Instant;
		return 1;
	}
	else if( CurrentMusic && ParseCommand( &Cmd, "MUSICORDER") )
	{
		INT i = atoi(Cmd);
		Out->Logf( "Galaxy order %i", i );
		glxControlMusic( GLX_SETPOSITION, i );
		return 1;
	}
	return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Internal functions.
-----------------------------------------------------------------------------*/

//
// Stop an active sound effect.
//
void UGalaxyAudioSubsystem::StopSound( INT Index )
{
	guard(UGalaxyAudioSubsystem::StopSound);
	FPlayingSound& Playing = PlayingSounds[Index];

	//debugf( "Stop %s", Actives(Index).Sound->GetName() );
	if( Playing.Channel )
	{
		guard(StopSample);
		if( Playing.Is3D ) 
			glxStopSample3D( Playing.Channel );
		else
			glxStopSample( Playing.Channel );
		unguard;
	}
	PlayingSounds[Index] = FPlayingSound();

	unguard;
}

/*-----------------------------------------------------------------------------
	Sound playing.
-----------------------------------------------------------------------------*/

UBOOL UGalaxyAudioSubsystem::PlaySound
(
	AActor*	Actor,
	INT		Id,
	USound*	Sound,
	FVector	Location,
	FLOAT	Volume,
	FLOAT	Radius,
	FLOAT	Pitch
)
{
	guard(UGalaxyAudioSubsystem::StartSound);
	check(Radius);
	if( !Viewport )
		return 0;

	// Allocate a new slot if requested.
	if( (Id&14)==2*SLOT_None )
		Id = 16 * --FreeSlot;

	// Compute priority.
	FLOAT Priority = SoundPriority( Viewport, Location, Volume, Radius );

	// If already playing, stop it.
	INT   Index        = -1;
	FLOAT BestPriority = Priority;
	for( INT i=0; i<EffectsChannels; i++ )
	{
		FPlayingSound& Playing = PlayingSounds[i];
		if( (Playing.Id&~1)==(Id&~1) )
		{
			// Skip if not interruptable.
			if( Id&1 )
				return 0;

			// Stop the sound.
			StopSound( i );
			Index = i;
			break;
		}
		else if( Playing.Priority<=BestPriority )
		{
			Index = i;
			BestPriority = Playing.Priority;
		}
	}

	// If no sound, or its priority is overruled, stop it.
	if( !Sound || Index==-1 )
		return 0;

	// Put the sound on the play-list.
	PlayingSounds[Index] = FPlayingSound( Actor, Id, Sound, Location, Volume, Radius, Pitch, Priority );
	return 1;

	unguard;
}

void UGalaxyAudioSubsystem::NoteDestroy( AActor* Actor )
{
	guard(UGalaxyAudioSubsystem::NoteDestroy);
	check(Actor);

	// Stop referencing actor.
	check(Actor->IsValid());
	for( int i=0; i<EffectsChannels; i++ )
	{
		if( PlayingSounds[i].Actor==Actor )
		{
			if( (PlayingSounds[i].Id&14)==SLOT_Ambient*2 )
			{
				// Stop ambient sound when actor dies.
				StopSound(i);
			}
			else
			{
				// Unbind regular sounds from actors.
				PlayingSounds[i].Actor=NULL;
			}
		}
	}

	unguard;
}

/*-----------------------------------------------------------------------------
	Timer update.
-----------------------------------------------------------------------------*/

//
// Update all active sound effects.
//
void UGalaxyAudioSubsystem::Update( FPointRegion Region, FCoords& Coords )
{
	guard(UGalaxyAudioSubsystem::Update);
	if( !Viewport )
		return;

	// Lock Galaxy so that all sound starting is synched.
	glxLock();

	// Get time passed.
	DOUBLE DeltaTime = appSeconds() - LastTime;
	LastTime += DeltaTime;
	DeltaTime = Clamp( DeltaTime, 0.0, 1.0 );

	// See if any new ambient sounds need to be started.
	UBOOL Realtime = Viewport->IsRealtime();
	if( Realtime )
	{
		guard(StartAmbience);
		for( INT i=0; i<Viewport->Actor->XLevel->Num(); i++ )
		{
			AActor* Actor = Viewport->Actor->XLevel->Actors(i);
			if
			(	Actor
			&&	Actor->AmbientSound
			&&	FDistSquared(Viewport->Actor->Location,Actor->Location)<=Square(Actor->WorldSoundRadius()) )
			{
				INT Id = Actor->GetIndex()*16+SLOT_Ambient*2;
				for( INT j=0; j<EffectsChannels; j++ )
					if( PlayingSounds[j].Id==Id )
						break;
				if( j==EffectsChannels )
				{
					//debugf( "Start ambient %s (%s)", Actor->AmbientSound->GetName(), Actor->GetFullName() );
					PlaySound( Actor, Id, Actor->AmbientSound, Actor->Location, AmbientFactor*Actor->SoundVolume/255.0, Actor->WorldSoundRadius(), Actor->SoundPitch/64.0 );
				}
			}
		}
		unguard;
	}

	// Update all playing ambient sounds.
	guard(UpdateAmbience);
	for( INT i=0; i<EffectsChannels; i++ )
	{
		FPlayingSound& Playing = PlayingSounds[i];
		if( (Playing.Id&14)==SLOT_Ambient*2 )
		{
			check(Playing.Actor);
			if
			(	FDistSquared(Viewport->Actor->Location,Playing.Actor->Location)>Square(Playing.Actor->WorldSoundRadius())
			||	Playing.Actor->AmbientSound!=Playing.Sound 
			|| !Realtime )
			{
				// Ambient sound went out of range.
				StopSound( i );
				//debugf( "Stop ambient out" );
			}
			else
			{
				// Update basic sound properties.
				FLOAT Brightness = 2.0 * (AmbientFactor*Playing.Actor->SoundVolume/255.0);
				if( Playing.Actor->LightType!=LT_None )
				{
					FPlane Color;
					Brightness *= Playing.Actor->LightBrightness/255.0;
					Viewport->Client->Engine->Render->GlobalLighting( (Viewport->Actor->ShowFlags & SHOW_PlayerCtrl)!=0, Playing.Actor, Brightness, Color );
				}
				Playing.Volume = Brightness;
				Playing.Radius = Playing.Actor->WorldSoundRadius();
				Playing.Pitch  = Playing.Actor->SoundPitch/64.0;
			}
		}
	}
	unguard;

	// Update all active sounds.
	guard(UpdateSounds);
	for( INT Index=0; Index<EffectsChannels; Index++ )
	{
		FPlayingSound& Playing = PlayingSounds[Index];
		if( Playing.Actor )
			check(Playing.Actor->IsValid());
		if( PlayingSounds[Index].Id==0 )
		{
			// Sound is not playing.
			continue;
		}
		else if( Playing.Channel && !glxControlChannel(Playing.Channel,GLX_STATUS) )
		{
			// Sound is finished.
			StopSound( Index );
		}
		else
		{
			// Update positioning from actor, if available.
			if( Playing.Actor )
				Playing.Location = Playing.Actor->Location;
			if( Playing.Actor )

			// Update the priority.
			Playing.Priority = SoundPriority( Viewport, Playing.Location, Playing.Volume, Playing.Radius );

			// Compute the spatialization.
			FVector Location = Playing.Location.TransformPointBy( Coords );
			FLOAT   PanAngle = appAtan2(Location.X, Abs(Location.Z));

			// Despatialize sounds when you get real close to them.
			FLOAT CenterDist  = 0.1*Playing.Radius;
			FLOAT Size        = Location.Size();
			if( Location.SizeSquared() < Square(CenterDist) )
				PanAngle *= Size / CenterDist;

			// Compute panning and volume.
			INT     GlxPan      = Clamp( (INT)(GLX_MAXSMPPANNING/2 + PanAngle*GLX_MAXSMPPANNING*7/8/PI), 0, GLX_MAXSMPPANNING );
			FLOAT   Attenuation = Clamp(1.0-Size/Playing.Radius,0.0,1.0);
			INT     GlxVolume   = Clamp( (INT)(GLX_MAXSMPVOLUME * Playing.Volume * Attenuation * EFFECT_FACTOR), 0, GLX_MAXSMPVOLUME );
			if( ReverseStereo )
				GlxPan = GLX_MAXSMPPANNING-GlxPan;
			if( Location.Z<0.0 && UseSurround )
				GlxPan |= GLX_SURSMPPANNING;

			// Compute doppler shifting (doesn't account for player's velocity).
			FLOAT Doppler=1.0;
			if( Playing.Actor )
			{
				FLOAT V = (Playing.Actor->Velocity/*-Viewport->Actor->Velocity*/) | (Playing.Actor->Location - Viewport->Actor->Location).SafeNormal();
				Doppler = Clamp( 1.0 - V/DopplerSpeed, 0.5, 2.0 );
			}

			// Update the sound.
			glxSample* Sample = GetSound(Playing.Sound);
			FVector Z(0,0,0);
			FVector L(Location.X/400.0,Location.Y/400.0,Location.Z/400.0);
			if( Playing.Channel )
			{
				// Update an existing sound.
				guard(ControlSample);
				if( Playing.Is3D ) glxControlSample3D
				(
					Playing.Channel,
					Sample->C4Speed * Playing.Pitch * Doppler,
					GlxVolume,
					(glxVector*)&L,
					(glxVector*)&Z
				);
				else glxControlSample
				(	
					Playing.Channel,
					0,
					Sample->C4Speed * Playing.Pitch * Doppler,
					GlxVolume,
					Playing.Channel->Panning
				);
				Playing.Channel->BasePanning = GlxPan;
				unguard;
			}
			else
			{
				// Start this new sound.
				guard(StartSample);
				if( A3D && Use3dHardware ) Playing.Channel = glxStartSample3D
				(
					Index+1, 
					Sample, 
					Sample->C4Speed * Playing.Pitch * Doppler, 
					GlxVolume, 
					(glxVector*)&L,
					(glxVector*)&Z,
					GLX_NORMAL
				);
				Playing.Is3D = Playing.Channel!=NULL;
				if( !Playing.Channel ) Playing.Channel = glxStartSample
				(
					Index+1, 
					Sample, 
					0, 
					Sample->C4Speed * Playing.Pitch * Doppler, 
					GlxVolume, 
					GlxPan, 
					GLX_NORMAL
				);
				check(Playing.Channel);
				check(Playing.Channel==&Channels[Index]);
				unguard;
			}
		}
	}
	unguard;

	// Handle music transitions.
	guard(UpdateMusic);
	if( Viewport->Actor->Transition!=MTRAN_None )
	{
		UBOOL ChangeMusic = CurrentMusic!=Viewport->Actor->Song;
		/*debugf
		(
			"MTRAN %s %i %s %i -- %i",
			CurrentMusic?CurrentMusic->GetName():"NONE",
			CurrentSection,
			Viewport->Actor->Song?Viewport->Actor->Song->GetName():"NONE",
			Viewport->Actor->SongSection,
			Viewport->Actor->Transition
		);*/
		if( CurrentMusic!=NULL || CurrentCDTrack!=255 )
		{
			// Do music transition.
			UBOOL Ready = 0;
			guard(UpdateStatus);
			if( CurrentSection==255 )
			{
				Ready = 1;
			}
			else if( Viewport->Actor->Transition == MTRAN_Fade )
			{
				MusicFade -= DeltaTime * 1.0;
				Ready = (MusicFade<-1.0*2.0*Latency/1000.f);
			}
			else if( Viewport->Actor->Transition == MTRAN_SlowFade )
			{
				MusicFade -= DeltaTime * 0.2;
				Ready = (MusicFade<-0.2*2.0*Latency/1000.f);
			}
			else if( Viewport->Actor->Transition == MTRAN_FastFade )
			{
				MusicFade -= DeltaTime * 3.0;
				Ready = (MusicFade<-3.0*2.0*Latency/1000.f);
			}
			else
			{
				Ready = 1;
			}
			unguard;
			//debugf("FADING %f ready %i",MusicFade,Ready);

			// Stop old music if done waiting for transition.
			guard(ShutUpMusic);
			if( Ready )
			{
				//debugf("READY");
				if( CurrentMusic && ChangeMusic )
				{
					UnregisterMusic( CurrentMusic );
				}
				if( UseCDMusic && CurrentCDTrack!=255 )
				{
					guard(glxStopCDTrack);
					glxStopCDTrack();
					unguard;
				}
				CurrentMusic   = NULL;
				CurrentCDTrack = 255;
			}
			else
			{
				SetVolumes();
			}
			unguard;
		}
		if( CurrentMusic==NULL && CurrentCDTrack==255 )
		{
			guard(NewMusic);
			MusicFade = 1.0;
			SetVolumes();
			CurrentMusic   = Viewport->Actor->Song;
			CurrentCDTrack = Viewport->Actor->CdTrack;
			CurrentSection = Viewport->Actor->SongSection;
			if( CurrentMusic && UseDigitalMusic )
			{
				// !!memory inefficient - duplicates in memory.
				guard(StartNewMusic);
				if( ChangeMusic )
				{
					debugf( NAME_DevMusic, "Load music: %s", CurrentMusic->GetFullName() );
					CurrentMusic->Data.Add(1024); /* Workaround to carlo reading outside of memory bug!! */
					safecall(glxLoadMusic(&CurrentMusic->Data(0),GLX_LOADFROMMEMORY));
					CurrentMusic->Handle = (void*)-1;
				}
				SetVolumes();
				if( CurrentSection!=255 )
				{
					silentcall(glxStartMusic());
					safecall(glxControlMusic( GLX_SETPOSITION, Viewport->Actor->SongSection ));
				}
				else
				{
					silentcall(glxStopMusic());
				}
				unguardf(( "(%s)", CurrentMusic->GetFullName() ));
			}
			if( CurrentCDTrack!=255 && UseCDMusic )
			{
				// Start CD.
				silentcall(glxStartCDTrack( Viewport->Actor->CdTrack ));
			}
			Viewport->Actor->Transition = MTRAN_None;
			unguard;
		}
		/*else
		{
			CurrentSection = 255;
		}*/
	}
	unguard;

	// Update reverb.
	guard(UpdateReverb);
	glxReverb Reverb;
	memset( &Reverb, 0, sizeof(Reverb) );
	if( UseReverb && Viewport->Actor->Region.Zone && Viewport->Actor->Region.Zone->bReverbZone )
	{
		AZoneInfo* ReverbZone = Viewport->Actor->Region.Zone;
		Reverb.Volume     = ReverbZone->MasterGain/255.0;
		Reverb.HFDamp     = Clamp(ReverbZone->CutoffHz,0,44100);//max samp rate
		for( INT i=0; i<ARRAY_COUNT(ReverbZone->Delay); i++ )
		{
			Reverb.Delay[i].Time = ReverbZone->Delay[i]/500.0;
			Reverb.Delay[i].Gain = ReverbZone->Gain[i]/255.0;
		}
	}
	if( memcmp(&CurrentReverb,&Reverb,sizeof(Reverb))!=0 )
	{
		memcpy(&CurrentReverb,&Reverb,sizeof(Reverb));
		safecall(glxSetSampleReverb(&Reverb));
	}
	unguard;

	// Unlock Galaxy.
	glxUnlock();
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
