/*ƒ- Internal revision no. 5.00b -ƒƒƒƒ Last revision at 15:52 on 25-04-1998 -ƒƒ

                        The 32 bit definition headerfile

                €€€ﬂﬂ€€€ €€€ﬂ€€€ €€€    €€€ﬂ€€€ €€€  €€€ €€€ €€€
                €€€  ﬂﬂﬂ €€€ €€€ €€€    €€€ €€€  ﬂ€€€€ﬂ  €€€ €€€
                €€€ ‹‹‹‹ €€€‹€€€ €€€    €€€‹€€€    €€     ﬂ€€€ﬂ
                €€€  €€€ €€€ €€€ €€€    €€€ €€€  ‹€€€€‹    €€€
                €€€‹‹€€€ €€€ €€€ €€€‹‹‹ €€€ €€€ €€€  €€€   €€€

                                MUSIC SYSTEM 
                This document contains confidential information
                     Copyright (c) 1993-98 Carlo Vogelsang

  ⁄ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒø
  ≥€≤± COPYRIGHT NOTICE ±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±≤€≥
  √ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ¥
  ≥ This headerfile, GALAXY.H, is Copyright (c) 1993-98  by  Carlo Vogelsang. ≥
  ≥ You may not copy, distribute, duplicate or clone this file  in  any form, ≥
  ≥ modified or non-modified without written permission of the author. By un- ≥
  ≥ authorized copying this file you are violating laws and will be punished. ≥
  ≥ So don't do that and let us all live in peace..                           ≥
  ≥                                                                     Carlo ≥
  ¿ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒŸ
*/
#ifndef _GALAXY_H
#define _GALAXY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Define constants used for soundcard type and base */

#define GLX_AUTODETECT			0

#define GLX_GRAVISULTRASOUND	1
#define GLX_SOUNDBLASTERAWE32	2
#define GLX_ADLIBGOLD			3
#define GLX_PROAUDIOSPECTRUM	4
#define GLX_SOUNDBLASTER16		5
#define GLX_SOUNDBLASTERPRO		6
#define GLX_SOUNDBLASTER		7
#define GLX_WINDOWSSOUNDSYSTEM	8
#define GLX_ADLIB				9
#define GLX_PCSPEAKER			10
#define GLX_COVOXDAC			11

#define GLX_DIRECTSOUND         12
#define GLX_WAVEDRIVER          13

#define GLX_NOSOUND				14

/* Define constants used for channel control */

#define GLX_OFF					0
#define GLX_ON					1
#define GLX_TOGGLE				2
#define GLX_STATUS              3

/* Define constants used for music control */

#define	GLX_VOLSET          	0
#define	GLX_VOLFADE				1
#define GLX_VOLFADE2            2

#define GLX_SETPOSITION         1
#define GLX_SETLOOPMODE         2
#define GLX_PREV               -3
#define GLX_CURR               -2
#define GLX_NEXT               -1
#define GLX_NOLOOP              0
#define GLX_LOOP                1

/* Define constants used for sample control */

#define GLX_MINSMPPANNING		0
#define GLX_MIDSMPPANNING	    16384
#define GLX_MAXSMPPANNING		32767
#define GLX_SURSMPPANNING       32768

#define GLX_MINSMPVOLUME		0
#define GLX_MAXSMPVOLUME	    32767

#define GLX_DEFSMPOFFSET		0
#define GLX_DEFSMPFREQUENCY	   -1
#define GLX_DEFSMPVOLUME       -1
#define GLX_DEFSMPPANNING      -1

/* Define constants used for instrument control */

#define GLX_MININSPANNING	    0
#define GLX_MIDINSPANNING		64
#define GLX_MAXINSPANNING		127

#define GLX_MININSVELOCITY		0
#define GLX_MAXINSVELOCITY	    127  

#define GLX_DEFINSPITCH			0
#define GLX_DEFINSVELOCITY	   -1
#define GLX_DEFINSPANNING	   -1
#define GLX_DEFINSMODULATION   -1

/* Define constants used for sample and instrument control */

#define GLX_AUTO                0
#define GLX_NORMAL				0
#define GLX_LOCKED				1

/* Define constants used for output mode control */

#define GLX_MONO 	   			0
#define GLX_STEREO              1

#define GLX_8BIT  				0
#define GLX_16BIT    	        2

#define GLX_SHOLD               0
#define GLX_COSINE              4

/* Define constants used for loaders */

#define GLX_LOADFROMSTREAM		1
#define GLX_LOADFROMMEMORY		2
#define GLX_LOADFROMARCHIVE     4
#define GLX_LOADASSTREAMING     8

#define GLX_SAVETOSTREAM		1
#define GLX_SAVETOMEMORY		2
#define GLX_SAVETOARCHIVE       4

#define GLX_FOURCC_AI			'  IA'
#define GLX_FOURCC_AM			'  MA'
#define GLX_FOURCC_AS			'  SA'

#define GLX_FOURCC_INIT			'TINI'
#define GLX_FOURCC_ORDR			'RDRO'
#define GLX_FOURCC_PATT			'TTAP'
#define GLX_FOURCC_STRM			'MRTS'
#define GLX_FOURCC_INST			'TSNI'
#define GLX_FOURCC_SAMP			'PMAS'

#define GLX_FOURCC_RIFF			'FFIR'
#define GLX_FOURCC_LIST			'TSIL'

#define GLX_FOURCC_MSTR			'RTSM'

/* Define maximum number of channels (do NOT modify !) */

#define GLX_TOTALBANKS          2
#define GLX_TOTALCHANNELS       64
#define GLX_TOTALINSTR          128

/* Define constants used for error messages */

#define GLX_NULL                0
#define GLXERR_NOERROR          0
#define GLXERR_MUSICLOADED      1
#define GLXERR_MUSICPLAYING     2
#define GLXERR_NOMUSICLOADED    3
#define GLXERR_NOMUSICPLAYING   4
#define GLXERR_OUTPUTACTIVE     5
#define GLXERR_OUTPUTNOTACTIVE  6
#define GLXERR_OUTOFCHANNELS    7
#define GLXERR_OUTOFINSTRUMENTS 8
#define GLXERR_OUTOFMEMORY      9
#define GLXERR_OUTOFPATTERNMEM  10
#define GLXERR_OUTOFSAMPLEMEM   11
#define GLXERR_DEVICEBUSY       12
#define GLXERR_DEVICENOWRITE    13
#define GLXERR_UNSUPPORTEDDEVICE 14
#define GLXERR_UNSUPPORTEDFORMAT 15
#define GLXERR_DAMAGEDFILE		16
#define GLXERR_BADPARAMETER     17

/* Type definitions used internally */

typedef unsigned long udword;                   /* Unsigned long (32 bit) */
typedef signed long sdword;                     /* Signed long (32 bit) */
typedef unsigned short uword;                   /* Unsigned short (16 bit) */
typedef signed short sword;                     /* Signed short (16 bit) */
typedef unsigned char ubyte;                    /* Unsigned byte (8 bit) */
typedef signed char sbyte;                      /* Signed byte (8 bit) */

/* Define structures used internally */

#pragma pack (push,1) 							/* Turn off alignment */

typedef struct
{
  ubyte      VibType;                           /* Vibrato waveform */
  uword      VibDelay;                          /* Vibrato start delay */
  sword      VibDepth;                          /* Vibrato depth (1/256 semi) */
  sword      VibSpeed;                          /* Vibrato speed (1/64 hz) */
  ubyte      TremType;                          /* Tremolo waveform */
  uword      TremDelay;                         /* Tremolo start delay */
  sword      TremDepth;                         /* Tremolo depth (1/32768 vol) */
  sword      TremSpeed;                         /* Tremolo speed (1/64 hz) */
  ubyte      VolFlag;    						/* Volume flag */
  ubyte		 VolReserved;						/* Volume reserved */	  
  ubyte      VolSize;                           /* Volume points */
  ubyte      VolSustain;   						/* Volume sustain */
  ubyte      VolLS;       						/* Volume loop start */
  ubyte      VolLE;      						/* Volume loop end */
  struct
  {
    uword Time;
    sword Value;
  }          Volume[10];                        /* Volume envelope */
  uword      VolFadeOut;						/* Volume fade-out */
  ubyte      PitFlag;    						/* Pitch flag */
  ubyte		 PitReserved;						/* Pitch reserved */	  
  ubyte      PitSize;                           /* Pitch points */
  ubyte      PitSustain;   						/* Pitch sustain */
  ubyte      PitLS;       						/* Pitch loop start */
  ubyte      PitLE;      						/* Pitch loop end */
  struct
  {
    uword Time;
    sword Value;
  }          Pitch[10];							/* Pitch envelope */
  uword      PitFadeOut;						/* Pitch fade-out */
  ubyte      PanFlag;    						/* Panning flag */
  ubyte		 PanReserved;						/* Panning reserved */	  
  ubyte      PanSize;                           /* Panning points */
  ubyte      PanSustain;   						/* Panning sustain */
  ubyte      PanLS;       						/* Panning loop start */
  ubyte      PanLE;      						/* Panning loop end */
  struct
  {
    uword Time;
    sword Value;
  }          Panning[10];						/* Panning envelope */
  uword      PanFadeOut;						/* Panning fade-out */
} glxArti;	    								/* Articulation structure */

typedef struct
{
  udword     Size;                              /* Buffer size */
  ubyte *    LinearOfs;							/* Buffer linear address */
  uword      LinearSel;							/* Buffer selector */
  uword      Handle;                            /* Buffer handle */
  udword     Physical;							/* Buffer physical address */
  udword     PlayOfs;                           /* Buffer data play ofs */
  uword      PlaySel;                           /* Buffer data play sel */
  udword     WriteOfs;                          /* Buffer data write ofs */
  uword      WriteSel;                          /* Buffer data write sel */
  ubyte      Part;                              /* Active buffer part */
  ubyte      Reserved[3];                       /* Reserved (alignment) */
} glxBuffer;	                                /* Buffer structure */

typedef struct
{
  udword	 FourCC;							/* FourCC value */
  udword	 Size;								/* Actual data size */
} glxChunk;										/* Chunk structure */

typedef struct
{
  udword	 FourCC;							/* FourCC value */
  udword	 Size;								/* Actual data size */
  udword	 Type;								/* Type value */
} glxForm; 										/* Chunk structure */

typedef struct
{
  ubyte      Type;								/* See defines above */
  ubyte      Output;							/* Output type needed */
  uword      Base;								/* Base address */
  ubyte      DMA;								/* DMA Channel */
  ubyte      IRQ;								/* IRQ Number */
  uword      Revision;               			/* Revision number */
  udword     RAM;								/* Bytes of on-board ram */
} glxSoundCard;                                 /* Soundcard structure */

typedef struct
{
  float 	 X;      							/* X Coordinate */
  float 	 Y;   								/* Y Coordinate */
  float 	 Z;   								/* Z Coordinate */
  float		 W;									/* W Coordinate */
} glxVector; 									/* Vector structure */

typedef struct
{
  udword     FourCC;                            /* FourCC value */
  udword     Size;                              /* Size of rest of structure */
  ubyte      Message[32];                       /* Name of sample */
  uword      Panning;							/* Panning position */
  uword      Volume;							/* Volume level */
  uword      Type;								/* Looping type/sample type */
  uword      Reserved;							/* Reserved for future use */
  udword     Length;							/* Length in samples */
  udword     LoopStart;							/* Loop starting point */
  udword     LoopEnd;							/* Loop ending point */
  udword     C4Speed;							/* Just like a piano */
  glxArti *  Articulation;                      /* Articulation structure */
  void *     Data;                              /* Actual sample data */
} glxSample;									/* Sample structure */

typedef struct
{
  udword     FourCC;                            /* FourCC value */
  udword     Size;                              /* Size of rest of structure */
  ubyte      Bank;                              /* Bank number */
  ubyte      Program;                           /* Program number */
  ubyte      Message[32];						/* Name of instrument */
  ubyte      Split[128];             			/* All keys, split points */
  glxArti    Articulation;						/* Articulation structure */
  uword      Samples;		 					/* Sample structures */
  glxSample  Sample[];                          /* Actual sample structures */
} glxInstrument;								/* Instrument structure */

typedef struct
{
  udword     FourCC;							/* FourCC value */
  udword     Size;                              /* Size of rest of structure */
  udword     StartTime;        					/* 0=Ins/-1=Stream/-2=Eff */
  ubyte      NoteNo;							/* Note number (&128=Key off) */
  ubyte      InstNo;							/* Instrument number */
  ubyte      SmpNo;								/* Sample number */
  ubyte      Active;           					/* 0=Stopped/1=Running */
  ubyte      Enabled;                			/* 0=Disabled/1=Enabled */
  ubyte      NewNote;                			/* 1=TRUE (Key on issued) */
  uword      Volume;   							/* 0=Silent..32767=Max. */
  uword      Panning;                			/* 0=Left..32767=Right */
  uword      BasePanning;						/* 0=Left..32767=Rigjt */
  uword      Velocity;                          /* 0=Silent..32767=Max. */
  uword		 Reserved;							/* Reserved for future use */
  uword		 Chorus;							/* Chorus send 0..127 */
  uword		 Reverb;							/* Reverb send 0..127 */
  glxArti *	 InsArt;                            /* Pointer to articulation data */
  sword      InsVol;                            /* Volume envelope value */
  sword      InsVolStep;                        /* Volume envelope step */
  uword      InsVolTime;                        /* Volume envelope time */
  uword      InsVolFade;                        /* Volume envelope fade-out */
  ubyte      InsVolPoint;                       /* Volume envelope point */
  ubyte      InsPitPoint;                       /* Pitch envelope point */
  uword      InsPitFade;                        /* Pitch envelope fade-out */
  uword      InsPitTime;                        /* Pitch envelope time */
  sword      InsPitStep;                        /* Pitch envelope step */
  sword      InsPit;                            /* Pitch envelope value */
  sword      InsPan;                            /* Panning envelope value */
  sword      InsPanStep;                        /* Panning envelope step */
  uword      InsPanTime;                        /* Panning envelope time */
  uword      InsPanFade;                        /* Panning envelope fade-out */
  ubyte      InsPanPoint;                       /* Panning envelope point */
  ubyte		 InsReserved[3];					/* Alignment stuff */
  glxSample *SmpHdr;                            /* Pointer to sample header */
  udword     SmpPtr;							/* Linear address play int. */
  uword      SmpFrac;							/* Linear address play frac. */
  uword      SmpType;	       					/* Looping/sample type */
  udword     SmpStart;							/* Linear address start */
  udword     SmpLoopStart; 						/* Linear address lpstart */
  udword     SmpLoopEnd;                        /* Linear address lpend */
  udword     SmpEnd;							/* Linear address end */
  udword     SmpC4Speed;						/* Just like a piano */
  sdword     SmpPitch;							/* C4Speed relative */
  uword      SmpVol;                 			/* Current sample volume */
  uword      SmpBaseVol;             			/* Current sample base volume */
  uword      SmpPeriod;							/* Period value */
  uword      SmpBasePeriod;						/* Period base value */
  sword      VibDepth;               			/* Vibrato depth */
  sword      VibSpeed;                          /* Vibrato speed */
  uword      VibIndex;               			/* Vibrato time index */
  ubyte      VibWaveType;            			/* Vibrato waveform */
  ubyte      TremWaveType;           			/* Tremolo waveform */
  uword      TremIndex;              			/* Tremolo time index */
  sword      TremSpeed;                         /* Tremolo speed */
  sword      TremDepth;              			/* Tremolo depth */
  uword      PortaDest;            				/* Portamento dest. */
  uword      PortaSpeed;    					/* Portamento speed */
  sword      BenderValue;                       /* Pitch bend value */
  ubyte      Vibrato;       					/* Vibrato settings */
  ubyte      Tremolo;             				/* Tremolo settings */
  ubyte      Portamento;                        /* Portamento settings */
  ubyte      SampleOffset;             			/* Sample offset */
  ubyte      Command;							/* Command number */
  ubyte      CommandValue;						/* Command data value */
  void *	 Custom1;							/* Custom data */
  void *	 Custom2;							/* Custom data */
} glxChannel;                                   /* Channel structure */

typedef struct
{
  ubyte      Identifier[4];                     /* Name of structure */
  udword     Size;                              /* Size of rest of structure */
  float		 Time;								/* Decay time -60dB */
  float		 Volume;							/* Overall volume */
  float      HFDamp;                            /* HFCutoff in Hz */
  struct
  {
    float Time;                                 /* Time 0 to 0.340Sec */
    float Gain;                                 /* Gain 0 to 1.0 */
  }          Direct[4];                         /* Direct layout */
  struct
  {
    float Time;                                 /* Time 0 to 0.340Sec */
    float Gain;                                 /* Gain 0 to 1.0 */
  }          Delay[6];                          /* Delay layout */
} glxReverb;									/* Reverb structure */

#pragma pack (pop)								/* Default alignment */

typedef void *(__cdecl *glxCallback)(glxChannel *Channel,void *Buffer,int Size);

/* Public external functions/procedures */

extern int             __cdecl glxAllocateSampleChannels(int Count);
extern int             __cdecl glxControlChannel(glxChannel *Channel,int Mode);
extern int             __cdecl glxControlMusic(int Command,int Parameter);
extern int             __cdecl glxControlSample(glxChannel *Channel,int Offset,int Frequency,int Volume,int Panning);
extern int             __cdecl glxControlSample3D(glxChannel *Channel,int Frequency,int Volume,glxVector *Position, glxVector *Velocity);
extern int             __cdecl glxControlInstrument(glxChannel *Channel,int Pitch,int Velocity,int Panning,int Modulation);
extern int             __cdecl glxDeinit(void);
extern int             __cdecl glxDetectOutput(int Card,uword Base);
extern int			   __cdecl glxFlushOutput(void);
extern int             __cdecl glxInfo(char **Version,char **Driver);
extern int             __cdecl glxInit(void);
extern int			   __cdecl glxLock(void);
extern int             __cdecl glxLoadMusic(void *Stream,int mode);
extern glxSample *     __cdecl glxLoadSample(void *Stream,int mode);
extern int             __cdecl glxLoadInstrument(int Instrument,void *Stream,int mode);
extern glxSample *	   __cdecl glxLoadInstrumentSample(int Instrument,int Sample,void *Stream,int Mode);
extern int             __cdecl glxRenderMusic(void *Stream,int Mode);
extern int             __cdecl glxResetMusic(void);
extern int             __cdecl glxSaveMusic(void *Stream,int Mode);
extern int             __cdecl glxSaveSample(glxSample *Sample,void *Stream,int Mode);
extern int             __cdecl glxSaveInstrument(int Instrument,void *Stream,int Mode);
extern int			   __cdecl glxSaveInstrumentSample(int Instrument,int Sample,void *Stream,int Mode);
extern int             __cdecl glxSetCallback(glxCallback Callback);
extern int             __cdecl glxSetCDVolume(int Volume,int Speed);
extern int             __cdecl glxSetMusicVolume(int Volume,int Speed);
extern int             __cdecl glxSetSampleVolume(int Volume,int Speed);
extern int             __cdecl glxSetIOInterface(void *newread,void *newseek,void *newtell,void *newwrite);
extern int             __cdecl glxSetMemInterface(void *newmalloc,void *newrealloc,void *newfree);
extern int			   __cdecl glxSetMusicReverb(glxReverb*Reverb);
extern int			   __cdecl glxSetSampleReverb(glxReverb *Reverb);
extern int             __cdecl glxStartCDTrack(int Track);
extern int             __cdecl glxStartMusic(void);
extern int             __cdecl glxStartOutput(void *Owner,int Rate,int Format,int Mixahead);
extern glxChannel *    __cdecl glxStartSample(int Channel,glxSample *Sample,int Offset,int Frequency,int Volume,int Panning,int Mode);
extern glxChannel *    __cdecl glxStartSample3D(int Channel,glxSample *Sample,int Frequency,int Volume,glxVector *Position, glxVector *Velocity,int Mode);
extern glxChannel *    __cdecl glxStartInstrument(int Channel,int Instrument,int Key,int Pitch,int Velocity,int Panning,int Modulation,int Mode);
extern int             __cdecl glxStopCDTrack(void);
extern int             __cdecl glxStopMusic(void);
extern int             __cdecl glxStopOutput(void);
extern int             __cdecl glxStopSample(glxChannel *Channel);
extern int			   __cdecl glxStopSample3D(glxChannel *Channel);
extern int             __cdecl glxStopInstrument(glxChannel *Channel);
extern int			   __cdecl glxUnlock(void);
extern int             __cdecl glxUnloadMusic(void);
extern int             __cdecl glxUnloadSample(glxSample *Sample);
extern int             __cdecl glxUnloadInstrument(int Instrument);
extern int			   __cdecl glxUnloadInstrumentSample(int Instrument,int Sample);

/* Public external uninitialised variables */

extern glxSoundCard    SoundCard;				/* Soundcard structure */
extern glxBuffer       SoundBuffer;				/* Soundbuffer structure */
extern glxChannel      Channels[64];            /* 64 Channel structures */
extern ubyte           SongName[32];            /* ASCIIZ Name of song */
extern ubyte           AuthorName[32];	        /* ASCIIZ Name of author */
extern ubyte           PlayerMode;				/* Period system etc. */
extern ubyte           MusicChannels;	        /* Music channels to be used */
extern ubyte           InitialSpeed;	        /* Default 48 (Ticks per beat) */
extern ubyte           InitialTempo;	        /* Default 120 (Beats per min) */
extern uword           ProSlideMin;				/* Minimum period allowed */
extern uword           ProSlideMax;				/* Maximum period allowed */
extern ubyte           InitialPanning[64];      /* Initial panning data */
extern ubyte           EffectChannels;          /* Effect channels to be used */
extern ubyte           SongLength;				/* Length of song base zero */
extern ubyte           Orders[256];				/* 256 Orders max. */
extern ubyte *         Patterns[256];	        /* 256 Patterns max. */
extern glxInstrument * Instruments[2][128];     /* 128 Instruments/bank */
extern volatile ubyte *CurrentPtr;              /* Current data */
extern volatile uword  CurrentTime;             /* Current time */
extern volatile ubyte  PatternRow;              /* Pattern row */
extern volatile ubyte  PatternBreak;	        /* Pattern break flag */
extern volatile ubyte  CurrentTempo;	        /* Current beats per min */
extern volatile ubyte  CurrentSpeed;	        /* Current ticks per row */
extern volatile ubyte  CurrentOrder;	        /* Current order */
extern volatile udword CurrentTick;				/* Current tick */
extern volatile udword CurrentSample;			/* Current sample */
extern volatile sbyte  MasterVolume;            /* 128 Levels */
extern volatile sbyte  MusicVolume;				/* 128 Levels */
extern volatile sbyte  EffectVolume;	        /* 128 Levels */
extern volatile sbyte  CDVolume;                /* 128 Levels */
extern volatile ubyte  MusicEnabled;	        /* Music enabled flag */
extern volatile ubyte  OutputActive;            /* Output active flag */
extern volatile ubyte  MusicLooping;            /* Music looping flag */
extern volatile udword Samplingrate;	        /* Sampling/mixing rate */
extern glxCallback     GlxCallback;             /* Callback function */
extern ubyte		   MMXFound;                /* MMX Found flag */

#ifdef __cplusplus
};
#endif

#endif
