/*=============================================================================
	FirEngin.H : Firengin header file - Unreal real-time animated texture
	synthesis code.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Erik de Neve 1996,97
=============================================================================*/

#ifndef _INC_UNFIREEN
#define _INC_UNFIREEN

/*----------------------------------------------------------------------------
	Fire parameter structures.
----------------------------------------------------------------------------*/

// Fire constants.
#define MaxSparks    1024 /* Maximum sparks per fire texture */
#define MaxSparkHeat 240  /* Maximum spark heat */

//
// Information abiut a single spark.
//
struct SparkParams
{
      BYTE Type;     // Spark type.
      BYTE Heat;     // Spark heat.
      BYTE X;        // Spark X location (0 - Xdimension-1).
      BYTE Y;        // Spark Y location (0 - Ydimension-1).

      BYTE ParamA;   // X-speed.
      BYTE ParamB;   // Y-speed.
      BYTE ParamC;   // Age, Emitter freq.
      BYTE ParamD;   // Exp.Time.
};

//
// Parameters controlling a fire texture.
//
struct FireEngineParams
{
      // Data about our associated bitmap.
      BYTE*  BitmapAddr;
      DWORD  ImageLimit;
      int    Xdimension;
      int    Ydimension;

      // Fire algorithm ('heat') parameters.
      BYTE  RenderTable[1028];
      int   RenderHeat;

      // Sparks.
      int    SparkNum;
      struct SparkParams Spark[MaxSparks];

      // Spark-type specific parameters.
      BYTE  HeatPhase1;
      BYTE  HeatPhaseDev1;
      BYTE  HeatPulse1;
      BYTE  HeatPulseDev1;

      char  JitterTable[256];

      BYTE  JitterSize;
      BYTE  EmitFreq;
      BYTE  EmitJitter;
      BYTE  EmitDirection;

      char  SineTable[256];

      // Future types or extensions, pointers to add-on structures or extra (workspace) bitmaps.
      DWORD AuxXsize;
      DWORD AuxYsize;

      DWORD AuxRandSeed;

      BYTE* AuxBitmap1;
      BYTE* AuxBitmap2;

      BYTE* AuxTable1;
      BYTE* AuxTable2;

      // Temp vars - required at edit time.
      BYTE   DrawSparkType;
      BYTE   DrawLineType;
      BYTE   DrawTorchType;
      BYTE   DrawEmitType;

      BYTE   DrawAuxType;
      BYTE   DrawWaveType;
      BYTE   DrawImageType;
      BYTE   DrawDepthType;

      DWORD  DrawSparkHeat;

      DWORD  DrawPosX;
      DWORD  DrawPosY;
};

/*----------------------------------------------------------------------------
	Water parameter structures.
----------------------------------------------------------------------------*/

// Water constants.
#define MaxDrops 256 /* Maximum number of drops in a water texture. */

//
// Information about a drop of water in a water texture.
//
struct DropParams
{
      BYTE Type;     // Water drop type.
      BYTE Depth;    // Depth.
      BYTE X;        // Spark X location (0 - Xdimension-1).
      BYTE Y;		 // Spark Y location (0 - Ydimension-1).

      BYTE ParamA;   // X-speed.
      BYTE ParamB;   // Y-speed.
      BYTE ParamC;   // Age, Emitter freq.
      BYTE ParamD;   // Exp.Time.
};

//
// Parameters controlling a water texture.
//
struct WaterParams
{
      // Data about our associated bitmap.
      BYTE*  BitmapAddr;
      BYTE*  SourceFields;
      DWORD  ImageLimit;
      int    Xdimension;
      int    Ydimension;

      // Fire algorithm ('heat') parameters.
      BYTE  RenderTable[1028];
      int   RenderShift;

      BYTE  WaveTable[1600];
      int   WaveDamping;

      BYTE WaveParity;  // Odd/even source field toggle.
      BYTE IDcode;      // To avoid superfluous self-mod setups in ASM.

      // Sparks.
      int DropNum;
      DropParams Drop[MaxDrops];

      // Drop-type specific parameters.
      BYTE  RainFreq;
      BYTE  RainDropSize;
      BYTE  Reserved0;
      BYTE  Reserved1;

      char  SineTable[256];

      // Future types or extensions,
      // pointers to add-on structures or extra (workspace) bitmaps.
      DWORD AuxXsize;
      DWORD AuxYsize;

      DWORD AuxRandSeed;

      BYTE* AuxBitmap1;
      BYTE* AuxBitmap2;

      BYTE* AuxTable1;
      BYTE* AuxTable2;

      // Temp vars, required at edit time.
      BYTE   DrawDropType;
      BYTE   DrawLineType;
      BYTE   DrawSpoonType;
      BYTE   DrawEmitType;

      BYTE   DrawAuxType;
      BYTE   DrawWaveType;
      BYTE   DrawImageType;
      BYTE   DrawDepthType;

      DWORD  DrawDropDepth;

      DWORD  DrawPosX;
      DWORD  DrawPosY;
};

/*----------------------------------------------------------------------------
	Random number functions.
----------------------------------------------------------------------------*/

void InitSpeedRand();
BYTE SpeedRand();

/*----------------------------------------------------------------------------
	Fire functions.
----------------------------------------------------------------------------*/

void EngineTileInit
(
	int Xdim,
    int Ydim,
    BYTE* FireBitmap,
    FireEngineParams* ThisTile
);

void TempSpark
(
	int SparkX,
    int SparkY,
    BYTE intensity,
    FireEngineParams* Params
);

void DrawSparkLine
(
	int StartX,
    int StartY,
    int DestX,
    int DestY,
    int Density,
    FireEngineParams* Params
);

void AddSpark
(
	int SparkX,
    int SparkY,
    FireEngineParams* Params
);

void DelSparks
(
	int SparkX,
    int SparkY,
    int AreaWidth,
    FireEngineParams* Params
);

void ClearSparks
(
	FireEngineParams* Params
);

void FirePaint
(
	int MouseX,
    int MouseY,
    int LeftButton,
    int RightButton,
    FireEngineParams* Params
);

void FireUpdate
(
	FireEngineParams *Params
);

void FireWrapUpdate
(
	FireEngineParams *Params
);

void Jazz2Update
(
	FireEngineParams *Params
);

void RedrawSparks
(
	FireEngineParams *Params
);

void FireTableFill
(
	FireEngineParams* ThisTile
);

void JitterTableFill
(
	FireEngineParams* ThisTile
);

/*----------------------------------------------------------------------------
	Fire functions.
----------------------------------------------------------------------------*/

void WaterInit
(
	int Xdim,
    int Ydim,
    BYTE* OutputBitmap,
    WaterParams* Pool
);

void WaterOutputLink
(
	BYTE* OutputBitmap,
    WaterParams* Pool
);

void SetWaveLight
(
	int ViewerAngle,
    int LightAngle,
    WaterParams* Pool
);

void WaterUpdate
(
	WaterParams* Pool
);

void AddDrop
(
	int DropX,
    int DropY,
    WaterParams* Pool
);

void WaterDrawDrops
(
	WaterParams* Pool
);

void WaterSplash
(
	int Xpos,
    int Ypos,
    int DropSize,
    WaterParams* Pool
);

void WaterPaint
(
	int MouseX,
    int MouseY,
    int LeftButton,
    int RightButton,
    WaterParams* Pool
);

void WaterReverse
(
	WaterParams* Pool
);
void WaterClean
(
	WaterParams* Pool
);

/*----------------------------------------------------------------------------
	Caustic functions.
----------------------------------------------------------------------------*/

void CausticsUpdate
(
	FireEngineParams* Plasm
);

void CausticsEngineInit
(
	int Xdim,
    int Ydim,
    BYTE* FireBitmap,
    FireEngineParams* CausticsParams
);

void BumpEngineInit
(
	int Xdim,
    int Ydim,
    BYTE* FireBitmap,
    FireEngineParams* BumpMapParams
);

/*----------------------------------------------------------------------------
	Functions for debugging only.
----------------------------------------------------------------------------*/

void WaterUpdateBound
(
	WaterParams* Pool
);

float BenchMarkFire
(
	FireEngineParams* Params
);

float BenchMarkWater
(
	WaterParams* Pool
);

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNFIREEN
