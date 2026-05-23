/*=============================================================================
	FirEngin.cpp: Unreal real-time animated texture synthesis code

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Erik de Neve 1996,97
=============================================================================*/

#include "Unreal.h"
#include "UnFireEn.h"

/*----------------------------------------------------------------------------
	Macros.
----------------------------------------------------------------------------*/

#define WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)

/*----------------------------------------------------------------------------
	Globals.
----------------------------------------------------------------------------*/

//
//   Shift-register random generator
//   based on trinomial trinomial x**63 + x**n + 1
//   Period of about 2^63 (*if* trinomial is irreducible and primitive)
//

BYTE SpeedRandArr[512];

extern "C"
{
	DWORD SpeedRindex;
}

/*----------------------------------------------------------------------------
	Random number functions.
----------------------------------------------------------------------------*/

//
// Init random numbers.
//
void InitSpeedRand()
{
	for( int Tt = 0 ; Tt < 512 ; Tt++ )
		SpeedRandArr[Tt] = (BYTE)(rand() & 255) ;

#if ASM
    // Reset AND make it the address of the 256-byte-aligned random array.
	SpeedRindex = ((DWORD)(&SpeedRandArr) +0xFF ) & 0xFFFFFF00;
#else
	SpeedRindex = 0;
#endif
}

//
// Return a fast high quality 32-bit random number.
//
#if ASM
inline BYTE SpeedRand()
#pragma warning (disable : 4035) // legalize implied return value EAX/AX/AL
{
	__asm
	{
		MOV ECX,SpeedRindex  // Rindex already set to RandArr offset
		MOV EDX,SpeedRindex
		ADD CL,4*59   ;;
		ADD DL,4      ;; Don't work:
		MOV EAX,[ECX]
		MOV SpeedRindex,EDX
		XOR [EDX],EAX  ;; XOR for shift-register, or ADD as in BSD generator
        //  only AL used but EAX is full 32-bit random output
	}
}
#pragma warning (default : 4035)
#else
//
// 8-bits C version of random generator;
// Rindex *not* set to array offset.
//
inline BYTE SpeedRand()
{
    SpeedRindex = (++SpeedRindex) & 63;
    return(
        SpeedRandArr[ SpeedRindex ] ^=
        SpeedRandArr[ (SpeedRindex+59) & 63 ]
    );
    }
#endif

inline int SpeedRand31()  // max = INT_MAX	2147483647
#pragma warning (disable : 4035) // legalize implied return value EAX/AX/AL
{
__asm{  // A fast high quality 31-bit random INT
    MOV ECX,SpeedRindex  // Rindex already set to RandArr offset
    MOV EDX,SpeedRindex
    ADD CL,4*59   ;;  Cool: 59 ! 55 .. 43..
    ADD DL,4      ;;  Not: 47,63,61,53(reasonable),57,51,49,45
    MOV EAX,[ECX]
    MOV SpeedRindex,EDX
    XOR [EDX],EAX  ;; XOR for shift-register, or ADD as in BSD generator
     //  only AL used but EAX is full 32-bit random output
    SHR EAX,1
    }
}
#pragma warning (default : 4035)



// 0..number = random(number)
inline float FRandom()     // output float nr in range [0..1>
    {
    return ( (float) (  (double)SpeedRand31()/ (double)MAXINT)  ) ;
    };

/*----------------------------------------------------------------------------
	Initialization routines.
----------------------------------------------------------------------------*/

//
// Initialization routines
//
void CausticsEngineInit( int Xdim,
                     int Ydim,
                     BYTE* FireBitmap,
                     FireEngineParams* CausticsParams)
{
       CausticsParams->BitmapAddr = FireBitmap;
       CausticsParams->SparkNum = 0;
       CausticsParams->Xdimension = Xdim;
       CausticsParams->Ydimension = Ydim;
       CausticsParams->RenderHeat = 10;
       CausticsParams->DrawSparkType = 1;
       CausticsParams->DrawLineType  = 1;
       CausticsParams->DrawAuxType   = 0;
       CausticsParams->ImageLimit = Xdim * ( Ydim - 2 );

       FireTableFill(CausticsParams);
       JitterTableFill(CausticsParams);
}






void BumpEngineInit( int Xdim,
                     int Ydim,
                     BYTE* FireBitmap,
                     FireEngineParams* BumpMapParams)
{
       BumpMapParams->BitmapAddr = FireBitmap;

       BumpMapParams->SparkNum = 0;

       BumpMapParams->Xdimension = Xdim;
       BumpMapParams->Ydimension = Ydim;
       BumpMapParams->RenderHeat = 10;
       BumpMapParams->DrawSparkType = 1;
       BumpMapParams->DrawLineType  = 1;
       BumpMapParams->DrawAuxType   = 0;

       BumpMapParams->ImageLimit = Xdim * ( Ydim - 2 );

       FireTableFill(BumpMapParams);
       JitterTableFill(BumpMapParams);
}




/*----------------------------------------------------------------------------
  Here's some  explanation about the  sinus table generator in the
  ACE BBS advert.
  The method used is a recursive sinus synthesis.  It's possible to
  compute all sinus values with only the two previous ones and the
  value  of  cos(2ã/N) , where  n  is the number of values for one
  period.

  It's as follow:

  Sin(K)=2.Cos(2ã/N).Sin(K-1)-Sin(K-2)

  or

  Cos(K)=2.Cos(2ã/N).Cos(K-1)-Cos(K-2)

  The last one is easiest to use , because the two first values of
  the cos table are 1 & cos(2ã/n) and with this two values you are
  able to build all the following...

  Some simple code:

  the cos table has 1024 values & ranges from -2^24 to 2^24

  build_table:          lea    DI,cos_table
                        mov    CX,1022
                        mov    EBX,cos_table[4]
                        mov    EAX,EBX
  @@calc:
                        imul   EBX
                        shrd   EAX,EDX,23
                        sub    EAX,[DI-8]
                        stosd
                        loop   @@calc

  cos_table             dd     16777216     ; 2^24
                        dd     16776900     ; 2 ^24*cos(2ã/1024)
                        dd     1022 dup (?)

                               Enjoy  KarL/NoooN
----------------------------------------------------------------------------*/

float Cos_Table [1028];
void CreateCosTable()
{
	// remake to:
// VERY fast table generation, works with FPU, even in C++ should
// be faster than the above ASM integer version
// because value retained in FPU, always 53/60 bit accuracy internally...

	// 2 DO: make inline FPU asm + 4x speedup using the symmetry
    // + check if that still works in 24 bits etc...

	const double CosGen = 0.9999811752826011;

    Cos_Table[0] = (float)1.0;
    Cos_Table[1] = (float)CosGen;  //Cos(2*pi/1024);

    int T =2;
	do {
	Cos_Table[T]   = 2.0 * CosGen * Cos_Table[T-1]  - Cos_Table[T-2];
	Cos_Table[T+1] = 2.0 * CosGen * Cos_Table[T]    - Cos_Table[T-1];
	T +=2;
	} while (T<1028);
};

/*----------------------------------------------------------------------------
	Table filling.
----------------------------------------------------------------------------*/



//
//  set up lookup table for the fast flame algo
//

void FireTableFill(FireEngineParams* ThisTile)
{
      int tmp;
      for (int T = 0; T < 1024; T++)
          {
                  tmp = (int) ((T/4.0) - (ThisTile->RenderHeat/10) );
                  if (tmp<0) tmp = 0;
                  if (tmp>255) tmp = 255;

                  ThisTile->RenderTable[T] = tmp;
          }
}





//
//  set up lookup table for fast X/Y jitter
//  array gets filled with simple alternating 'sawtooth' function
//  so  JitterTable[SpeedRand()]   has an effect like
//      rand()*JitterSize - (JitterSize/2)
//


void JitterTableFill(FireEngineParams* ThisTile)
{
      char FillMax = (ThisTile->JitterSize+2) / 2 ;

      if (FillMax > 127) FillMax = 127;
      char Fill1   =  FillMax;
      char Fill2   = -FillMax;

      char Tmp;

      for (int T = 0; T < 256; T++)
       {
              if (T&1) { // ODD
                         Tmp = Fill1;
                         if ( --Fill1 < 0 ) Fill1 =  FillMax;
                       }
                 else  { // EVEN
                         Tmp = Fill2;
                         if ( ++Fill2 > 0 ) Fill2 = -FillMax;
                       }
          ThisTile->JitterTable[T] = Tmp;
       }

}

/*----------------------------------------------------------------------------
	Fire calculation.
----------------------------------------------------------------------------*/


//
// Fire calculation
//


#if ASM

extern "C" void  CalculateFire(  BYTE* BitmapAddr,
                              BYTE* RenderTable,
                                  DWORD Xdimension,
                                  DWORD Ydimension );

extern "C" void  __cdecl CalcWrapFire(  BYTE* BitmapAddr,
                              BYTE* RenderTable,
                                  DWORD Xdimension,
                                  DWORD Ydimension );

#else

void  CalculateFire(  BYTE* BitmapAddr,
                              BYTE* RenderTable,
                                  DWORD Xdimension,
                                  DWORD Ydimension )
{

  for  (DWORD Y = 0 ;Y < (Ydimension - 2) ; Y++ )
     {
         BYTE* ThisLine  = BitmapAddr + Y * Xdimension;
         BYTE* BelowLine =   ThisLine +   Xdimension;
         BYTE* LowerLine =  BelowLine +   Xdimension;

         // Special case: X=0
         *(ThisLine) = RenderTable[
             *(BelowLine    ) +
         //  *(BelowLine -1 ) +
             *(BelowLine +1 ) +
             *(LowerLine    )
                  ];

         for (DWORD X=1; X < (Xdimension-1); X++ )
       {
         *(ThisLine + X ) = RenderTable[
             *(BelowLine + X   ) +
             *(BelowLine + X-1 ) +
             *(BelowLine + X+1 ) +
             *(LowerLine + X   )
                  ];
       }

         //Special case: X=(Xdimension-1)
         *(ThisLine + Xdimension -1 ) = RenderTable[
             *(BelowLine + Xdimension-1   ) +
             *(BelowLine + Xdimension-1-1 ) +
         //  *(BelowLine + Xdimension-1+1 ) +
             *(LowerLine + Xdimension-1   )
                  ];
     } //Y
}

/*----------------------------------------------------------------------------
	Wrapping fire calcuation.
----------------------------------------------------------------------------*/



void  CalcWrapFire(  BYTE* BitmapAddr,
                              BYTE* RenderTable,
                                  DWORD Xdimension,
                                  DWORD Ydimension )
{

  for  (DWORD Y = 0 ;Y < (Ydimension - 2) ; Y++ )
     {
         BYTE* ThisLine  = BitmapAddr + Y * Xdimension;
         BYTE* BelowLine =   ThisLine +   Xdimension;
         BYTE* LowerLine =  BelowLine +   Xdimension;

         // Special case: X=0
         *(ThisLine) = RenderTable[
             *(BelowLine    ) +
             *(BelowLine +Xdimension-1 ) +     //wrapping around left edge
             *(BelowLine +1 ) +
             *(LowerLine    )
                  ];

         for (DWORD X=1; X < (Xdimension-1); X++ )
       {
         *(ThisLine + X ) = RenderTable[
             *(BelowLine + X   ) +
             *(BelowLine + X-1 ) +
             *(BelowLine + X+1 ) +
             *(LowerLine + X   )
                  ];
       }

         //Special case: X=(Xdimension-1)
         *(ThisLine + Xdimension -1 ) = RenderTable[
             *(BelowLine + Xdimension-1   ) +
             *(BelowLine + Xdimension-1-1 ) +
             *(BelowLine + 0 )              + //wrapping around right edge
             *(LowerLine + Xdimension-1   )
                  ];
     } //Y



    // special case: line-before-last -> lowest line wraps around

  Y = (Ydimension - 2);
     {
         BYTE* ThisLine  = BitmapAddr + Y * Xdimension;
         BYTE* BelowLine =   ThisLine +   Xdimension;
         BYTE* LowerLine =  BitmapAddr;

         // Special case: X=0
         *(ThisLine) = RenderTable[
             *(BelowLine    ) +
             *(BelowLine +Xdimension-1 ) +     //wrapping around left edge
             *(BelowLine +1 ) +
             *(LowerLine    )
                  ];

         for (DWORD X=1; X < (Xdimension-1); X++ )
       {
         *(ThisLine + X ) = RenderTable[
             *(BelowLine + X   ) +
             *(BelowLine + X-1 ) +
             *(BelowLine + X+1 ) +
             *(LowerLine + X   )
                  ];
       }

         //Special case: X=(Xdimension-1)
         *(ThisLine + Xdimension -1 ) = RenderTable[
             *(BelowLine + Xdimension-1   ) +
             *(BelowLine + Xdimension-1-1 ) +
             *(BelowLine + 0 )              + //wrapping around right edge
             *(LowerLine + Xdimension-1   )
                  ];
     } //Y




    // special case: last line -> both lower lines wrap around

  Y = (Ydimension - 1);
     {
         BYTE* ThisLine  = BitmapAddr + Y * Xdimension;
         BYTE* BelowLine =  BitmapAddr;
         BYTE* LowerLine =  BitmapAddr + Xdimension;

         // Special case: X=0
         *(ThisLine) = RenderTable[
             *(BelowLine    ) +
             *(BelowLine +Xdimension-1 ) +     //wrapping around left edge
             *(BelowLine +1 ) +
             *(LowerLine    )
                  ];

         for (DWORD X=1; X < (Xdimension-1); X++ )
       {
         *(ThisLine + X ) = RenderTable[
             *(BelowLine + X   ) +
             *(BelowLine + X-1 ) +
             *(BelowLine + X+1 ) +
             *(LowerLine + X   )
                  ];
       }

         //Special case: X=(Xdimension-1)
         *(ThisLine + Xdimension -1 ) = RenderTable[
             *(BelowLine + Xdimension-1   ) +
             *(BelowLine + Xdimension-1-1 ) +
             *(BelowLine + 0 )              + //wrapping around right edge
             *(LowerLine + Xdimension-1   )
                  ];
     } //Y

}



#endif



/*----------------------------------------------------------------------------
	Temporary spark setter, i.e. for mouse torch in editor.
----------------------------------------------------------------------------*/

//
// non-permanent spark setter - eg. mouse torch in editor.
//

void TempSpark(int SparkX,
               int SparkY,
               BYTE Intensity,
               FireEngineParams* Params)
{
          DWORD SparkDest = (DWORD)(
               SparkX +
               SparkY * Params->Xdimension
               );

          //check if destination still inside bitmap, then set..
          if (SparkDest < Params->ImageLimit)
          *(BYTE*)(Params->BitmapAddr+SparkDest) = Intensity;

}

/*----------------------------------------------------------------------------
	Spark line drawing.
----------------------------------------------------------------------------*/




//
// Unelegant but reasonably fast implementation of bresenham-line-drawing
//

void DrawSparkLine(int StartX,
                   int StartY,
                   int DestX,
                   int DestY,
                   int Density,
                   FireEngineParams* Params)
{

 int Xinc;
 int Yinc;

 int DivX = DestX - StartX;
 int DivY = DestY - StartY;

 if ((DivX == 0) && (DivY == 0)) return;

 if (DivX<0) Xinc = -1;
    else { if (DivX>0) Xinc = 1;
           else Xinc = 0;
         }

 if (DivY<0) Yinc = -1;
    else { if (DivY>0) Yinc = 1;
           else Yinc = 0;
         }

 DivX = abs(DivX);
 DivY = abs(DivY);

 int Xpoint = StartX;
 int Ypoint = StartY;

 if (DivX >= DivY)   // draw line based on X loop
      {
        int DivY2  = DivY + DivY;
        int Diff   = DivY2 - DivX;
        int DivXY2 = DivY2 - DivX - DivX;

      for (int LCount = 1; LCount <= DivX; LCount++)
        {
         AddSpark(Xpoint,Ypoint,Params);

         if (Diff<0)  Diff += DivY2;
           else {
                Diff   += DivXY2;
                Ypoint += Yinc;
                }
         Xpoint += Xinc;

        }
     }

 else                // draw line based on Y loop
      {
        int DivX2  = DivX  + DivX;
        int Diff   = DivX2 - DivY;
        int DivXY2 = DivX2 - DivY - DivY;

      for (int LCount = 1; LCount <= DivY; LCount++)
        {
         AddSpark(Xpoint,Ypoint,Params);

         if (Diff<0)  Diff += DivX2;
           else {
                Diff   += DivXY2;
                Xpoint += Xinc;
                }
         Ypoint += Yinc;
        }
     }

}


/*----------------------------------------------------------------------------
	Spark adding.
----------------------------------------------------------------------------*/


//
// Adding a spark to the FireEngineParams structure
//


void AddSpark(int SparkX,
              int SparkY,
              FireEngineParams* Params)
{


  // return if out of bounds  or  out of sparks
  if (!( ((SparkX < Params->Xdimension) && (SparkY < Params->Ydimension))
          && ((SparkX>0) && (SparkY>0))
          && (Params->SparkNum < (MaxSparks))  ))
  return;


  int S = Params->SparkNum;
  Params->Spark[S].X = SparkX;
  Params->Spark[S].Y = SparkY;
  Params->Spark[S].Heat = 240;
  Params->Spark[S].Type = Params->DrawSparkType;
  Params->SparkNum++;

  Params->DrawAuxType++;

  //// the rest are spark-type specific assignments:

  switch( Params->DrawSparkType )
       {

       // case 0: // still sparks
       // case 1: // jitter sparks
       // case 4: // fireball
       // case 5: // fountain
       // case 6: // local emitter
       // case 7: // falling sparks right
       // case 8: // falling sparks left
       // case 9: // 2xfast up-floaters

       case 2: // phased
         Params->Spark[S].Heat = Params->DrawAuxType;
       break;

       case 3: // phased
         Params->Spark[S].Heat = Params->DrawAuxType;
       break;

       }

}


/*----------------------------------------------------------------------------
	Spark deletion.
----------------------------------------------------------------------------*/


//
// Delete a spark within a certain area in the image
//

void DelSparks(int SparkX,
               int SparkY,
               int AreaWidth,
               FireEngineParams* Params)
{
  if (Params->SparkNum>0)
    for (int S = 0; S < Params->SparkNum; S++)
        {
           if (AreaWidth >= (
                   abs(SparkX - Params->Spark[S].X) +
                   abs(SparkY - Params->Spark[S].Y) )
                   )
           {   //delete spark by replacing it with last one (+ delete last one)
                   int LastSpark = --Params->SparkNum;
                Params->Spark[S] =   Params->Spark[LastSpark];
           }
        }
}




//
// Erase all sparks
//

void ClearSparks(FireEngineParams* Params)
{
  Params->SparkNum = 0;
}

/*----------------------------------------------------------------------------
	Fire painting.
----------------------------------------------------------------------------*/


//
// Paint routine to be called from the editor,
//   with a FireEngineParams variable that has been
//   initialized with EngineTileInit.
//


void FirePaint( int MouseX,
                int MouseY,
                int LeftButton,
                int RightButton,
                FireEngineParams* Params)
{
  static int LastMouseX = 0;
  static int LastMouseY = 0;
  static int LastLeftButton  = 0;
  static int LastRightButton = 0;

  BOOL PosChanged = ((LastMouseX != MouseX) || (LastMouseY != MouseY));

  if ( (LeftButton > 0) &&  PosChanged )
        // Draws the kind of spark/linetype currently selected
      {
        if ( ( Params->DrawLineType == 1 ) &&
             ( LastLeftButton == LeftButton ) &&
             ( Params->DrawSparkType != 9   )  )
          DrawSparkLine(LastMouseX,LastMouseY,MouseX,MouseY,1,Params);
           else
          AddSpark(MouseX,MouseY,Params);
      }

  if ( (RightButton > 0) && PosChanged )
            // Delete any sparks within certain range of mouse cursor
            DelSparks(MouseX,MouseY,12,Params);

  LastMouseX = MouseX;
  LastMouseY = MouseY;
  LastLeftButton = LeftButton;
  LastRightButton = RightButton;

}


/*----------------------------------------------------------------------------
	Fire update.
----------------------------------------------------------------------------*/

//
// General Fire update, called from the editor or the game itself.
//
void FireUpdate(FireEngineParams* Params)
{
	RedrawSparks(Params);
    CalculateFire
	(
		Params->BitmapAddr,
        (BYTE*) &(Params->RenderTable),
        Params->Xdimension,
        Params->Ydimension
	);
}

void FireWrapUpdate(FireEngineParams* Params)
{    
	RedrawSparks(Params);
    CalcWrapFire
	(
		Params->BitmapAddr,
        (BYTE*) &(Params->RenderTable),
        Params->Xdimension,
        Params->Ydimension
	);    
}

#if ASM
extern "C" void MasmRedrawSparks
(
	BYTE *BitmapAddr,
    DWORD Xdimension,
    DWORD Ydimension,
    BYTE  *PSpark,
    int   *PSparkNum,
    BYTE  *PJitterTable,
    BYTE  HeatPhase,
    BYTE  HeatPulse
);
#endif

/*----------------------------------------------------------------------------
	Spark updating.
----------------------------------------------------------------------------*/

/*
 Spark types:

     0 static w. random intensity
     1 static with x/y randomization
     2 phased sliding
     3 phased pulsating
     4 fireball
     5 fountain
     6 local fountain
     7 fountain right
     8 fountain left

     9..127    reserved

     128..255  reserved for sparks emitted by types < 128
*/

//
// Advance a spark stochastically using signed 8-bit speed.
//
inline void SparkMove(SparkParams* Spark)
{
    char DevX = Spark->ParamA;
    if( DevX<0 )
    {
		// Negative update.
		if( (SpeedRand()&127) < -DevX )
        Spark->X--;
	}
    else
    {
		// Positive update.
		if( (SpeedRand()&127) <  DevX )
			Spark->X++;
	}

    char DevY = Spark->ParamB;
    if( DevY<0 )
	{
		// Negative update.
		if( (SpeedRand()&127) < -DevY )
			Spark->Y--;
	}
    else
	{
		// Positive update.
		if( (SpeedRand()&127) <  DevY )
			Spark->Y++ ;
	}
}

//
// Advance a spark stochastically using signed 8-bit speed, up.
//
inline void SparkMoveTwo(SparkParams* Spark)
{
    char DevX = Spark->ParamA;
    if( DevX<0 )
    {
		// Negative update.
		if( (SpeedRand()&127) < -DevX )
			Spark->X--;
	}
    else
	{
		// Positive update.
		if( (SpeedRand()&127) <  DevX )
             Spark->X++;
	}
	*(BYTE*)&Spark->Y -= 2;
}

/*----------------------------------------------------------------------------
	Spark redrawing.
----------------------------------------------------------------------------*/

//
// Redraw all sparks.
//
void RedrawSparks(FireEngineParams* Params)
{
	Params->HeatPhase1 += Params->HeatPhaseDev1;
	Params->HeatPulse1 += Params->HeatPulseDev1;

#if ASM
	MasmRedrawSparks
	(
		Params->BitmapAddr,
        Params->Xdimension,
        Params->Ydimension,
        (BYTE*) &(Params->Spark[0]),
        &(Params->SparkNum),
        (BYTE*) &(Params->JitterTable[0]),
        Params->HeatPhase1,  //byte
        Params->HeatPulse1   //byte
	);
#else
	DWORD NewSparkX;
	DWORD SparkDest;

	for( int S = 0; S < Params->SparkNum; S++ )
	switch( Params->Spark[S].Type )
	{
		case 0: // Still sparks.
			NewSparkX = (DWORD) Params->Spark[S].X;

			SparkDest = (DWORD)
			(	NewSparkX
            +	Params->Spark[S].Y * Params->Xdimension );

			//Check if destination still inside bitmap, then set.
			if
			(	(SparkDest < Params->ImageLimit)
			&&	(NewSparkX < (DWORD)Params->Xdimension) )
                 *(BYTE*)(Params->BitmapAddr+SparkDest) = SpeedRand();
                 //Params->Spark[S].Heat;
                 //SpeedRand();
			break;

   case 1: // normal spark with positional jitter

         NewSparkX =
             (DWORD) ( Params->Spark[S].X +
                       Params->JitterTable[SpeedRand()] );

         SparkDest = (DWORD)(
                NewSparkX
                + ( Params->Spark[S].Y + Params->JitterTable[SpeedRand()] )
                  * Params->Xdimension
                );

         //check if destination still inside bitmap, then set..
         if    ((SparkDest < Params->ImageLimit)
             && (NewSparkX < (DWORD)Params->Xdimension))
                  *(BYTE*)(Params->BitmapAddr+SparkDest) =
                  Params->Spark[S].Heat;
         break;

   case  2: // phased sparks

           NewSparkX =
           (DWORD) (Params->Spark[S].X );

         SparkDest = (DWORD)(
               NewSparkX
             + ( Params->Spark[S].Y ) * Params->Xdimension
             );
         //check if destination still inside bitmap, then set..
         if    ((SparkDest < Params->ImageLimit)
             && (NewSparkX < (DWORD)Params->Xdimension))
                  *(BYTE*)(Params->BitmapAddr+SparkDest) =
                   (BYTE) (Params->Spark[S].Heat + Params->HeatPhase1);

           break;


   case  3: // pulse-phased sparks

           NewSparkX =
           (DWORD) (Params->Spark[S].X );

         SparkDest = (DWORD)(
               NewSparkX
             + ( Params->Spark[S].Y ) * Params->Xdimension
             );
         //check if destination still inside bitmap, then set..
         if    ((SparkDest < Params->ImageLimit)
             && (NewSparkX < (DWORD)Params->Xdimension))
                  *(BYTE*)(Params->BitmapAddr+SparkDest) =
                   (BYTE) (Params->HeatPulse1);

           break;

   case  4: // emit sparks of type 128 with random directions

            if ( (Params->SparkNum < (MaxSparks))
                  && (Params->EmitFreq > SpeedRand() ) )
               {     // create it..
                int NS = Params->SparkNum++;
                 Params->Spark[NS].X = Params->Spark[S].X;
                 Params->Spark[NS].Y = Params->Spark[S].Y;
                 Params->Spark[NS].ParamA = SpeedRand();
                 Params->Spark[NS].ParamB = SpeedRand();
                 Params->Spark[NS].Heat = 240; // start heat
                 Params->Spark[NS].Type = 128; // dynamic type
               }

           break;


   case  5: // FOUNTAIN - sparks of type 129 mostly upward

            if ( (Params->SparkNum < (MaxSparks))
                  && (Params->EmitFreq > SpeedRand() ) )
               {     // create it..
                int NS = Params->SparkNum++;
                 Params->Spark[NS].X = Params->Spark[S].X;
                 Params->Spark[NS].Y = Params->Spark[S].Y;
                 Params->Spark[NS].ParamA = (SpeedRand()&127) - 63; //X speed arbit.
                 Params->Spark[NS].ParamB =(BYTE) -127; // Y speed UP
                 Params->Spark[NS].Heat = 186; // start heat
                 Params->Spark[NS].Type = 129; //dynamic type
               }
           break;


   case  6: // symmetric gravity-emitting - sparks of type 130

            if ( (Params->SparkNum < (MaxSparks))
                  && (Params->EmitFreq > SpeedRand() ) )
               {     // create it..
                int NS = Params->SparkNum++;
                 Params->Spark[NS].X = Params->Spark[S].X;
                 Params->Spark[NS].Y = Params->Spark[S].Y;
                 Params->Spark[NS].ParamA = (SpeedRand()&127) - 63; //X speed arbit.
                 Params->Spark[NS].ParamB =0; // Y speed UP
                 Params->Spark[NS].ParamC =50; // timer
                 Params->Spark[NS].Type = 130; //
               }
           break;


   case  7: // Erupt to the right

            if ( (Params->SparkNum < (MaxSparks))
                  && (Params->EmitFreq > SpeedRand() ) )
               {     // create it..
                int NS = Params->SparkNum++;
                 Params->Spark[NS].X = Params->Spark[S].X;
                 Params->Spark[NS].Y = Params->Spark[S].Y;
                 Params->Spark[NS].ParamA = (SpeedRand()&63) + 63; //X speed arbit.
                 Params->Spark[NS].ParamB =(BYTE) -29; // Y speed UP
                 Params->Spark[NS].ParamC =83; // timer
                 Params->Spark[NS].Type = 130; // const. heat ?
               }
           break;

   case  8: // Erupt to the left

            if ( (Params->SparkNum < (MaxSparks))
                  && (Params->EmitFreq > SpeedRand() ) )
               {     // create it..
                int NS = Params->SparkNum++;
                 Params->Spark[NS].X = Params->Spark[S].X;
                 Params->Spark[NS].Y = Params->Spark[S].Y;
                 Params->Spark[NS].ParamA = (SpeedRand()&63) -128; //X speed arbit.
                 Params->Spark[NS].ParamB =(BYTE) -29; // Y speed UP
                 Params->Spark[NS].ParamC =83; // timer
                 Params->Spark[NS].Type = 130; //
               }
           break;



   case  9: // emitting - sparks of type 131
            //  Whirly-floaty fire sparks, go up & glow out.

            if ( (Params->SparkNum < (MaxSparks))
                  && (Params->EmitFreq > SpeedRand() ) )
               {     // create it..
                int NS = Params->SparkNum++;
                 Params->Spark[NS].X = Params->Spark[S].X + (SpeedRand()&31);
                 Params->Spark[NS].Y = Params->Spark[S].Y + (SpeedRand()&31);
                 Params->Spark[NS].ParamA = SpeedRand() - 127; //X speed arbit.
                 Params->Spark[NS].ParamB = 256-127; // Y speed UP
                 Params->Spark[NS].ParamC = 255; // timer - steps --2, to 128
                 Params->Spark[NS].Type = 131; //
               }
           break;



   ///
   /// spark numbers above 128 are all 'spawned' from lower spark types
   ///

           // Dynamic emitted sparks.
    case 128:
          NewSparkX = (DWORD) (Params->Spark[S].X );

          SparkDest = (DWORD)(
                NewSparkX
              + ( Params->Spark[S].Y ) * Params->Xdimension
              );

          //check if destination still inside bitmap, then set..
          if  (  (SparkDest < Params->ImageLimit)
              && (NewSparkX < (DWORD)Params->Xdimension)
              && (( Params->Spark[S].Heat -= 5 )< 250 )  )  // glowout-wraparound?
               { // set spark
                   *(BYTE*)(Params->BitmapAddr+SparkDest) =
                    (BYTE) (Params->Spark[S].Heat);
                 // advance position
                    SparkMove( &Params->Spark[S] );
                 // no gravity.

             }

           else  // out of bounds: delete the spark
              Params->Spark[S] = Params->Spark[--Params->SparkNum];

          break;


                     // Dynamic emitted sparks.
    case 129:
          NewSparkX = (DWORD) (Params->Spark[S].X );

          SparkDest = (DWORD)(
                NewSparkX
              + ( Params->Spark[S].Y ) * Params->Xdimension
              );

          //check if destination still inside bitmap, then set..
          if  (  (SparkDest < Params->ImageLimit)
              && (NewSparkX < (DWORD)Params->Xdimension)
              && (( Params->Spark[S].Heat -= 2 )< 250 )  )  // glowout-wraparound?
               { // set spark
                   *(BYTE*)(Params->BitmapAddr+SparkDest) =
                    (BYTE) (Params->Spark[S].Heat);

                 // advance position
                 SparkMove( &Params->Spark[S] );
                 // no gravity.

             }

           else  // out of bounds: delete the spark
              Params->Spark[S] = Params->Spark[--Params->SparkNum];

          break;



              // Dynamic falling ones: GRAVITY. if out of range, delete.
    case 130:
          NewSparkX = (DWORD) (Params->Spark[S].X );
            SparkDest = (DWORD)(
                NewSparkX
              + ( Params->Spark[S].Y ) * Params->Xdimension
              );

          //check if destination still inside bitmap, then set..
          if  (  (SparkDest < Params->ImageLimit)
              && (NewSparkX < (DWORD)Params->Xdimension)
              && (( Params->Spark[S].ParamC -= 1 )< 254 )  )  // timeout
               { // set spark
                   *(BYTE*)(Params->BitmapAddr+SparkDest) = 160;
                    //(BYTE) (Params->Spark[S].Heat);

                 // advance position
                    SparkMove( &Params->Spark[S] );

                 //gravity:
                 if ((char)Params->Spark[S].ParamB < 122)
                         Params->Spark[S].ParamB +=3;
               }

             else  // out of bounds: delete the spark
              Params->Spark[S] = Params->Spark[--Params->SparkNum];

          break;



              // Whirly-floaty fire sparks, go up & glow out.
    case 131:
          NewSparkX = (DWORD) (Params->Spark[S].X );
            SparkDest = (DWORD)(
                NewSparkX
              + ( Params->Spark[S].Y ) * Params->Xdimension
              );

          //check if destination still inside bitmap, then set..
          if  (  (SparkDest < Params->ImageLimit)
              && (NewSparkX < (DWORD)Params->Xdimension)
              && (( Params->Spark[S].ParamC -= 3 )> 190 )  )  // timeout
               { // set spark
                   *(BYTE*)(Params->BitmapAddr+SparkDest) =
                    (BYTE) (Params->Spark[S].ParamC);
                 // advance position
                    SparkMoveTwo( &Params->Spark[S] );
               }

             else  // out of bounds: delete the spark
              Params->Spark[S] = Params->Spark[--Params->SparkNum];

          break;


      } //switch
#endif


}

/*----------------------------------------------------------------------------
	Water initialization.
----------------------------------------------------------------------------*/






//
// the WAVE - water effect.
//

void WaterInit( int Xdim,
                int Ydim,
                BYTE* OutputBitmap,
                WaterParams* Pool)

{
      InitSpeedRand(); // ! needs called only ONCE externally,
                       //    because multiple tiles all share the
                       //    same random array.


      int WMapSize = Xdim * Ydim * 2;
      Pool->SourceFields = new BYTE[ WMapSize ];

  // ! This assumes the routine is never called twice for the same Pool !!
  // Memory could also be allocated externally as long as it's the right
  // size.


       // establish an ID code (avoids superfluous self-modification in ASM)
       static BYTE UniqueID = 1;
       Pool->IDcode = UniqueID++;

       Pool->DropNum = 0;

       Pool->BitmapAddr = OutputBitmap;

       Pool->Xdimension = Xdim;
       Pool->Ydimension = Ydim;
       Pool->RenderShift   = 10;
       Pool->DrawDropType  = 1;
       Pool->DrawLineType  = 1;
       Pool->DrawAuxType   = 0;

       Pool->RainFreq = 0;
       Pool->RainDropSize = 0;

       Pool->ImageLimit = Xdim*Ydim*2; //double field


       Pool->WaveParity = 0; // map toggle..

       for (int i=0; i< 2*(Pool->Xdimension)*(Pool->Ydimension);
                        i++)
              Pool->SourceFields[i] = SpeedRand(); // 128;


       // fill the color table

       for (  i=0; i < 1024 ; i++)
           {
              Pool->RenderTable[i] = (i/4);
           }


       // fill the algorithm lookup table


       for ( i=0; i<1600; i++)
        {
          int TempWav = ((i / 2) - 256) ;

          // totally reversible algo: ((i/2)-256)&255
          // eliminate bias...
          // normal: 0->0, 1->0, 2->1, 254->127, 255->127, 256->128 ;
          // now: 0->0, 1->1, 2->1, 254->127, 255->128

          if ((i-512)<256) TempWav++;

          if (TempWav < 0)  {
                       TempWav = 0;
                      }

          if (TempWav>255)  {
                       TempWav = 255;
                       }

          Pool->WaveTable[i] = (BYTE)TempWav;
        }

}



void WaterOutputLink( BYTE* OutputBitmap,
                      WaterParams* Pool)
{
       Pool->BitmapAddr = OutputBitmap;
}




double FakeAtan(double Tan)
{
    return ( (3.1415F * 0.5F) * (Tan /(fabs( (float)Tan )+1.0F) ));
}

//
//  Bump-mapped Waves    light look-up table initialize
//

void SetWaveLight( int ViewerAngle,     //
                   int LightAngle,      // 0..1023..
                   WaterParams* Pool)
{

      // fill the color table according to light direction;
      //      0 = "-512" facet facing extreme left
      // 1023   = "+511" facet facing extreme right
      //
      // actually used: [-510..0..+510], symmetrically.
      //
      // Palette itself [255..0] assumed to be linear.
      //
      //
      //   \    /------
      //    \  / |    |
      //      /  |    D
      //     /   |    |
      //    /----------
      //    |    |
      //    |    |
      //    ------
      //    X    X+1
      //
      //
      // Changing D into a Normal angle:
      //  D =  -510 .. +510
      //  NORMAL of the plane = ATAN( D*C ) + (pi/2)
      //  C = arbitrary 'wave magnification' constant,
      //
      // light = COS (  lightangle -  normal )
      //
      // light/view angles at input are: 0..1023  =  0..2pi
      //

       const double pi = 3.1415926535;


       double Lamp   = ( ((double)LightAngle) / 1023.0) * pi;
       double Viewer = ( ((double)ViewerAngle) / 1023.0) * pi;
       double Normal;
       double Reflected;

       int TempLight;

       for (int  i=0; i < 1024 ; i++)

         {

         //  calculate shade for all possible surface normals

         //  ATAN could be approximated by simple linear mapping
         // Default:  atan(  (512-(double)i) / 196.0 ) + (pi * 0.5);


          //Normal =  atan(  (512-(double)i) / 196.0 ) + (pi * 0.5) ;

         Normal = FakeAtan(  (512-(double)i) / 196.0 ) + (pi * 0.5) ;


         // max in this palette is about 235, diffuse light reaches to 150

          TempLight = (int) ( 150.0 * cos ( Normal- Lamp )  );


         // create a phong-ish highlight of size 0.11 * 2  radians.
         // based on angle between viewer direction and REFLECTED light:
         // reflected light angle : = Normal*2 - Lamp;

          Reflected = (Normal*2 - Lamp);
          if ( fabs(Reflected-Viewer) < 0.11 )
             {
              TempLight +=(int)
              ( 132.0 * ( 1.0 / 0.11 ) * ( 0.11 - fabs(Reflected-Viewer)) );
             }
         // good values: ( <0.16,  and  * 100.0)


         // clip color range to fit inside the windows non-system colors range
          if (TempLight>235)  TempLight = 235;
          if (TempLight<16 )  TempLight = 16;

          Pool->RenderTable[i] = TempLight;

          }

}

/*----------------------------------------------------------------------------
	Water interpolated calculation.
----------------------------------------------------------------------------*/





////
//// Note that this C++ water code is rather 'spun out', it was
//// designed as a test for the way the update is done in assembler.
//// Most of the code results from having to make the water wrap correctly.
////


#if ASM

extern "C" void  CalcWaterASM(  BYTE* PBitmap    ,
                                     BYTE* PWavMap    ,
                                     BYTE* PRendTable ,
                                     BYTE* PWaveTable ,
                                     DWORD XVar       ,
                                     DWORD YVar       ,
                                     DWORD Parity     ,
                                     DWORD WaterID
                                     );

void  CalculateWater( WaterParams* Pool )
{

  Pool->WaveParity++;   // odd/even counter

  CalcWaterASM(   Pool->BitmapAddr  ,    //  PBitmap
                      Pool->SourceFields,    //  PWavMap
                      Pool->RenderTable ,    //  PRendTable
                      Pool->WaveTable   ,    //  PWaveTable
                      Pool->Xdimension  ,    //  XVar
                      Pool->Ydimension  ,    //  YVar
                      Pool->WaveParity  ,    //  Parity
                      Pool->IDcode           //  WaveID
                       );
}


#else


void Output4Pix (
                  BYTE SourceA,
                  BYTE SourceC,
                  BYTE SourceE,
                  BYTE SourceG,
                  BYTE SourceB,
                  BYTE SourceD,
                  BYTE SourceF,
                  BYTE SourceH,
                  BYTE* DestCell,
                  BYTE* Dest1,
                  BYTE* Dest2,           //
                  BYTE* Dest3,           //  A C E.G    12
                  BYTE* Dest4,           //  B D F H    34
                  WaterParams* Pool      //  DestCell is in midst of EGFH
                 )
{
 *DestCell = *( Pool->WaveTable + 512 +
               ( (int)SourceE
               + (int)SourceG
               + (int)SourceF
               + (int)SourceH
               )
               - ( ((int)*DestCell)  << 1 )
               );


  *Dest1 = *( Pool->RenderTable + 512 + (
              ( (int)SourceE-(int)SourceA ) +
              ( (int)SourceF-(int)SourceB ) +
              ( (int)SourceG-(int)SourceC ) +
              ( (int)SourceH-(int)SourceD )
              ) / 2   // SAR for signed ints ...
            );

  *Dest2 = *( Pool->RenderTable + 512 +
              ( (int)SourceG-(int)SourceC ) +
              ( (int)SourceH-(int)SourceD )
            );

  *Dest3 = *( Pool->RenderTable + 512 +
              ( (int)SourceF-(int)SourceB ) +
              ( (int)SourceH-(int)SourceD )
            );

  *Dest4 = *( Pool->RenderTable + 512 +
              ( (int)SourceH-(int)SourceD )+
              ( (int)SourceH-(int)SourceD )
            );




  // 2,3 step instead of '2,4' - sharper image, but blockier too

  /*

   *Dest1 = *( Pool->RenderTable + 512 + (
              ( (int)SourceE-(int)SourceC ) +
              ( (int)SourceF-(int)SourceD ) +

              ( (int)SourceG-(int)SourceE ) +
              ( (int)SourceH-(int)SourceF )
              ) / 2   // SAR for signed ints ...
            );

  *Dest2 = *( Pool->RenderTable + 512 +
              ( (int)SourceG-(int)SourceE ) +
              ( (int)SourceH-(int)SourceF )
            );

  *Dest3 = *( Pool->RenderTable + 512 +
              ( (int)SourceF-(int)SourceD ) +
              ( (int)SourceH-(int)SourceF )
            );

  *Dest4 = *( Pool->RenderTable + 512 +
              ( (int)SourceH-(int)SourceF )+
              ( (int)SourceH-(int)SourceF )
            );

 */

}

/*----------------------------------------------------------------------------
	Interpolated water.
----------------------------------------------------------------------------*/



////
//// Interpolated water, CPP source version
////
////


void  CalculateWater( WaterParams* Pool )

                           //  BYTE* BitmapAddr,
                           //  BYTE* RenderTable,
                           //  BYTE* WaveTable,
                           //  BYTE* SourceFields
                           //
                           //  DWORD Xdimension,
                           //  DWORD Ydimension )
{
  BYTE* BitMapAddr  =  Pool->BitmapAddr; //pointer

  BYTE* RenderTable = (BYTE*) &Pool->RenderTable;  //actual table
  BYTE*   WaveTable = (BYTE*) &Pool->WaveTable;    //actual table

  DWORD Xdimension = Pool->Xdimension;
  DWORD Ydimension = Pool->Ydimension;

  BYTE* WaterMap; // map1 & map 2: interleaved...

  WaterMap = Pool->SourceFields; // ptr to interleaved simulated water field

  Pool->WaveParity++;   // odd/even counter

  int TotalSize = 2 * Xdimension * Ydimension;

  BYTE* DestCell;
  int   DestPixel;


  if ((Pool->WaveParity & 1) == 0)


{

 // EVEN water: source =  cells on ODD lines, destin = EVEN lines
 //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 // start with UPPER row of even, 2 wrap up+left at beginning, n-2 all wrap up

 DestCell  = (WaterMap+0);
 DestPixel = 0; // always lower right pixel dest

 Output4Pix (
               *(DestCell+TotalSize-3),
               *(DestCell+TotalSize-2),
               *(DestCell+TotalSize-1),
               *(DestCell+TotalSize-0-Xdimension),
               *(DestCell-3 + Xdimension*2),
               *(DestCell-2 + Xdimension*2),
               *(DestCell-1 + Xdimension*2),
               *(DestCell-0 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1               +TotalSize*2,  //
               BitMapAddr+DestPixel   - Xdimension*2+TotalSize*2,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 + Xdimension*2,  //  3 4
               BitMapAddr+DestPixel,                   //
               Pool);

 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell+TotalSize-3),
               *(DestCell+TotalSize-2),
               *(DestCell+TotalSize-1-Xdimension),
               *(DestCell+TotalSize-0-Xdimension),
               *(DestCell-3 + Xdimension*2),
               *(DestCell-2 + Xdimension*2),
               *(DestCell-1 + Xdimension  ),
               *(DestCell-0 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2+TotalSize*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2+TotalSize*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);

 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell+TotalSize-3),
               *(DestCell+TotalSize-2-Xdimension),
               *(DestCell+TotalSize-1-Xdimension),
               *(DestCell+TotalSize-0-Xdimension),
               *(DestCell-3 + Xdimension*2),
               *(DestCell-2 + Xdimension  ),
               *(DestCell-1 + Xdimension  ),
               *(DestCell-0 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2+TotalSize*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2+TotalSize*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);


 /// cuz of way ASM works (saved results) ASM needs only 2 wrappers

 for (int X = 3; X < (int)Xdimension  ; X++ ) // total Xdimension-1 cells
        {
 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell+TotalSize-3-Xdimension),
               *(DestCell+TotalSize-2-Xdimension),
               *(DestCell+TotalSize-1-Xdimension),
               *(DestCell+TotalSize-0-Xdimension),
               *(DestCell-3 + Xdimension  ),
               *(DestCell-2 + Xdimension  ),
               *(DestCell-1 + Xdimension  ),
               *(DestCell-0 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2+TotalSize*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2+TotalSize*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);

        } //X loop end


 //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

 for (int Y = 1; Y < (int)Ydimension  ; Y++ ) // total Xdimension-1 cells
 {
  // first 2 wrap, rest goes always same

 DestCell  = WaterMap+ Y*Xdimension*2 ;
 DestPixel = Y*Xdimension*2*2;          // always lower right pixel dest

 //DestCell++;
 //DestPixel+=2;

 Output4Pix (
               *(DestCell-3),
               *(DestCell-2),
               *(DestCell-1),
               *(DestCell-0 - Xdimension),
               *(DestCell-3 + Xdimension*2),
               *(DestCell-2 + Xdimension*2),
               *(DestCell-1 + Xdimension*2),
               *(DestCell-0 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1               ,  //
               BitMapAddr+DestPixel   - Xdimension*2,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 + Xdimension*2,  //  3 4
               BitMapAddr+DestPixel,                   //
               Pool);

 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell-3),
               *(DestCell-2),
               *(DestCell-1 - Xdimension),
               *(DestCell-0 - Xdimension),
               *(DestCell-3 + Xdimension*2),
               *(DestCell-2 + Xdimension*2),
               *(DestCell-1 + Xdimension  ),
               *(DestCell-0 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);

 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell-3),
               *(DestCell-2 - Xdimension),
               *(DestCell-1 - Xdimension),
               *(DestCell-0 - Xdimension),
               *(DestCell-3 + Xdimension*2),
               *(DestCell-2 + Xdimension  ),
               *(DestCell-1 + Xdimension  ),
               *(DestCell-0 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);


 /// cuz of way ASM works (saved results) ASM needs only 2 wrappers

 for (int X = 3; X < (int)Xdimension  ; X++ ) // total Xdimension-1 cells
        {
 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell-3 - Xdimension),
               *(DestCell-2 - Xdimension),
               *(DestCell-1 - Xdimension),
               *(DestCell-0 - Xdimension),
               *(DestCell-3 + Xdimension  ),
               *(DestCell-2 + Xdimension  ),
               *(DestCell-1 + Xdimension  ),
               *(DestCell-0 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);

        } //X loop end

 } //  Y loop end...

 } //  EVEN water end



 else



 {
 //  ODD water: source = EVEN lines, destin =  ODD lines

 // LAST line...



 DestCell  = (WaterMap+(Ydimension-1)*Xdimension*2 + Xdimension);
 DestPixel = (Ydimension-1)*Xdimension*4 + Xdimension*2  +1;
             // always lower right pixel dest

 Output4Pix (
               *(DestCell-2),
               *(DestCell-1),
               *(DestCell-0-Xdimension),
               *(DestCell+1-Xdimension),
               *(DestCell-TotalSize-2 + Xdimension*2),
               *(DestCell-TotalSize-1 + Xdimension*2),
               *(DestCell-TotalSize-0 + Xdimension  ),
               *(DestCell-TotalSize+1 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);

 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell-2),
               *(DestCell-1-Xdimension),
               *(DestCell-0-Xdimension),
               *(DestCell+1-Xdimension),
               *(DestCell-TotalSize-2 + Xdimension*2),
               *(DestCell-TotalSize-1 + Xdimension  ),
               *(DestCell-TotalSize-0 + Xdimension  ),
               *(DestCell-TotalSize+1 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);


 /// cuz of way ASM works (saved results) ASM needs only 2 wrappers

 for (int X = 2; X < (int)(Xdimension-1)  ; X++ ) // total Xdimension-1 cells
        {
 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell-2-Xdimension),
               *(DestCell-1-Xdimension),
               *(DestCell-0-Xdimension),
               *(DestCell+1-Xdimension),
               *(DestCell-TotalSize-2 + Xdimension  ),
               *(DestCell-TotalSize-1 + Xdimension  ),
               *(DestCell-TotalSize-0 + Xdimension  ),
               *(DestCell-TotalSize+1 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);

        } //X loop end


 // last one needs SOURCE wrap to right...

 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell-2-Xdimension),
               *(DestCell-1-Xdimension),
               *(DestCell-0-Xdimension),
               *(DestCell+1-Xdimension-Xdimension),
               *(DestCell-TotalSize-2 + Xdimension  ),
               *(DestCell-TotalSize-1 + Xdimension  ),
               *(DestCell-TotalSize-0 + Xdimension  ),
               *(DestCell-TotalSize+1 + 0 ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);



 //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

 for (int Y = 0; Y < (int)(Ydimension-1)  ; Y++ ) // total Xdimension-1 cells
 {
  // first 2 wrap, rest goes always same

 //DestCell++;
 //DestPixel+=2;

 DestCell  = WaterMap+ Y*Xdimension*2 +Xdimension;
 DestPixel = Y*Xdimension*2*2 + Xdimension*2 + 1;

    // always lower right pixel dest

 Output4Pix (
               *(DestCell-2),
               *(DestCell-1),
               *(DestCell-0-Xdimension),
               *(DestCell+1-Xdimension),
               *(DestCell-2 + Xdimension*2),
               *(DestCell-1 + Xdimension*2),
               *(DestCell-0 + Xdimension  ),
               *(DestCell+1 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);

 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell-2),
               *(DestCell-1-Xdimension),
               *(DestCell-0-Xdimension),
               *(DestCell+1-Xdimension),
               *(DestCell-2 + Xdimension*2),
               *(DestCell-1 + Xdimension  ),
               *(DestCell-0 + Xdimension  ),
               *(DestCell+1 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);


 /// cuz of way ASM works (saved results) ASM needs only 2 wrappers

 for (int X = 2; X < (int)(Xdimension-1)  ; X++ ) // total Xdimension-1 cells
        {
 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell-2-Xdimension),
               *(DestCell-1-Xdimension),
               *(DestCell-0-Xdimension),
               *(DestCell+1-Xdimension),
               *(DestCell-2 + Xdimension  ),
               *(DestCell-1 + Xdimension  ),
               *(DestCell-0 + Xdimension  ),
               *(DestCell+1 + Xdimension  ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);

        } //X loop end

 // last one needs SOURCE wrap to right...

 DestCell++;
 DestPixel+=2;

 Output4Pix (
               *(DestCell-2-Xdimension),
               *(DestCell-1-Xdimension),
               *(DestCell-0-Xdimension),
               *(DestCell+1-Xdimension-Xdimension),
               *(DestCell-2 + Xdimension  ),
               *(DestCell-1 + Xdimension  ),
               *(DestCell-0 + Xdimension  ),
               *(DestCell+1 + 0 ),
               DestCell,
               BitMapAddr+DestPixel-1 -Xdimension*2  ,  //
               BitMapAddr+DestPixel   -Xdimension*2  ,  //  1 2   4==BitMapAddr+DestPixel*2 ! (for black/EVEN!)
               BitMapAddr+DestPixel-1 ,  //  3 4
               BitMapAddr+DestPixel   ,  //
               Pool);


 } //  Y loop end...


 } // ODD water end


} // END of void CalculateWater / interpolating version


#endif




void WaterSplash( int Xpos,
                  int Ypos,
                  int DropSize,
                  WaterParams* Pool)
{

 for (int y = 0; y < DropSize; y++)
   {
   DWORD DropDestY =(DWORD)( (Ypos/2+y) * Pool->Xdimension * 2);

   for (int x = 0; x < DropSize; x++)
    {
    DWORD DropDest = DropDestY+(Xpos/2+x);

       // check if destination still inside source fields, then set..

       if ( DropDest < (DWORD) (Pool->ImageLimit) )
       {
        Pool->SourceFields[ DropDest ]                     =  255-4;
        Pool->SourceFields[ DropDest + Pool->Xdimension ]  =  4;
       }
    }

   }

}



void WaterClean(WaterParams* Pool)
{
 for (int i=0; i< 2*(Pool->Xdimension)*(Pool->Ydimension);i++)
         Pool->SourceFields[i] = 128; // 128;
 Pool->DropNum = 0;
}


void WaterReverse(WaterParams* Pool)
    {
    Pool->WaveParity++;
    }




void WaterDrawDrops(WaterParams* Pool)
{

  // very crude random RAINDROPS for now..

  if ( Pool->RainFreq > SpeedRand() ) //  max 255 = 1 drop every frame
   {
    int Xrandom = (SpeedRand() * Pool->Xdimension) >> 7;
    int Yrandom = (SpeedRand() * Pool->Ydimension) >> 7;
    WaterSplash( Xrandom,
                 Yrandom,
                 Pool->RainDropSize,
                 Pool);
   }


 // Waterdrops setting from the list...
 // types: 0=transient 1=fixed

 DWORD DropDest;
 DWORD NewDropX;

 BYTE BWave;

 BWave = (Pool->Reserved0++) &31; // drop.Depht as kind of 'phase'?
    if  (BWave > 15)  BWave = 31-BWave;
    BWave = BWave << 4;

 for (int S = 0; S < Pool->DropNum; S++)

 switch( Pool->Drop[S].Type )
 {

   case 1: // PULSING DROPS
         NewDropX =
             (DWORD) Pool->Drop[S].X / 2;

    //check if destination still inside bitmap, then set..
     if ( NewDropX < (DWORD)Pool->Xdimension )
      {
    //////// DROP setting
      DropDest = (DWORD)( (Pool->Drop[S].Y/2) * Pool->Xdimension * 2)
                       + NewDropX ;

    if ( DropDest < (DWORD) (Pool->ImageLimit) )
       {
       Pool->SourceFields[ DropDest ]                       =  BWave;
       Pool->SourceFields[ DropDest+1 ]                     =  BWave;
       Pool->SourceFields[ DropDest   + Pool->Xdimension ]  =  BWave;
       Pool->SourceFields[ DropDest+1 + Pool->Xdimension ]  =  BWave;
       }
    //////////
    }
    break; //type 1
  } //switch

}








void WaterDrawDropsBound(WaterParams* Pool)
{

  // chain to normal one
  WaterDrawDrops(Pool);

   // places a MENISCUS-like depression in the water, for testing
  // the new lighting routines.



   for (int y=0; y < (Pool->Ydimension );y++)
        {
        DWORD SparkDest = (DWORD) ( y * Pool->Xdimension * 2);
        // if ( SparkDest < (DWORD) (Pool->ImageLimit) )
                {
            Pool->SourceFields[ SparkDest ]                     =  0;
            Pool->SourceFields[ SparkDest + Pool->Xdimension ]  =  0;
            Pool->SourceFields[ SparkDest +70 ]                    =  0;
            Pool->SourceFields[ SparkDest + Pool->Xdimension +70]  =  0;
            Pool->SourceFields[ SparkDest +71 ]                    =  0;
            Pool->SourceFields[ SparkDest + Pool->Xdimension +71]  =  0;
                 }
        }

        /*
        for (int x=0; x < 70;x++)
        {
        DWORD SparkDest = (DWORD) ( x );
                {
            Pool->SourceFields[ SparkDest ]                     =  0;
            Pool->SourceFields[ SparkDest + Pool->Xdimension ]  =  0;
                 }
        }
        */
}






void WaterPaint( int MouseX,
                int MouseY,
                int LeftButton,
                int RightButton,
                WaterParams* Pool)
{
  static int LastMouseX = 0;
  static int LastMouseY = 0;
  static int LastLeftButton  = 0;
  static int LastRightButton = 0;

  BOOL PosChanged = ((LastMouseX != MouseX) || (LastMouseY != MouseY));

  if (LeftButton > 0) //&&  PosChanged )
        // Draws the kind of spark/linetype currently selected
      {
          //DrawSparkLine(LastMouseX,LastMouseY,MouseX,MouseY,1,Pool);
       WaterSplash(MouseX,MouseY,3,Pool);

      }

  if (RightButton > 0) //&&  PosChanged )
        // Draws the kind of spark/linetype currently selected
      {
       AddDrop(MouseX,MouseY,Pool);

      }

  LastMouseX = MouseX;
  LastMouseY = MouseY;
  LastLeftButton = LeftButton;
  LastRightButton = RightButton;

}



//
// Adding a semi-permanent drop to the Pool structure
//


void AddDrop(int DropX,
             int DropY,
             WaterParams* Pool)
{

  // return if out of bounds  or  out of Drops
  if (!( (((DropX/2) < Pool->Xdimension) && ((DropY/2) < Pool->Ydimension))
          && ((DropX>0) && (DropY>0))
          && (Pool->DropNum < (MaxDrops))  ))
  return;

  int S = Pool->DropNum;
  Pool->Drop[S].X = DropX;
  Pool->Drop[S].Y = DropY;
  Pool->Drop[S].Depth = 240;
  Pool->Drop[S].Type = Pool->DrawDropType;
  Pool->DropNum++;

  Pool->DrawAuxType++;

  //// the rest are Drop-type specific assignments:

  /*
  switch( Pool->DrawDropType )
       {
       // case 0: // still Drops
       // case 1: // jitter Drops
       }
  */

}


/*----------------------------------------------------------------------------
	Fire initialization.
----------------------------------------------------------------------------*/

//
// Init fire.
//
void EngineTileInit
(
	int Xdim,
    int Ydim,
    BYTE* FireBitmap,
    FireEngineParams* ThisTile
)
{
	// Init random numbers.
	InitSpeedRand();

	// Remember parameters.
    ThisTile->BitmapAddr = FireBitmap;
    ThisTile->Xdimension = Xdim;
    ThisTile->Ydimension = Ydim;

	// Set image write limit.
    ThisTile->ImageLimit    = Xdim*(Ydim-2);

	// Init sparks and info.
    ThisTile->SparkNum      = 0;
    ThisTile->RenderHeat    = 10;
    ThisTile->DrawSparkType = 1;
    ThisTile->DrawLineType  = 1;
    ThisTile->DrawAuxType   = 0;

	// Init the fire table.
    FireTableFill(ThisTile);

	// Init the jitter table.
    JitterTableFill(ThisTile);
}

/*----------------------------------------------------------------------------
	Updating.
----------------------------------------------------------------------------*/

void WaterUpdate(WaterParams* Pool)
{
    WaterDrawDrops(Pool);

    CalculateWater(Pool);
}


void WaterUpdateBound(WaterParams* Pool)
{
    WaterDrawDropsBound(Pool);
    CalculateWater(Pool);
}







void CausticsUpdate(FireEngineParams* Params)
{
  static BYTE CausticsInit = 0;
  static BYTE Phase1 = 0;
  static BYTE Phase2 = 128;

  Phase1++;

  BYTE* BitmapAddr = Params->BitmapAddr;
  BYTE* RenderTable = Params->RenderTable;
  DWORD Xdimension = Params->Xdimension;
  DWORD Ydimension = Params->Ydimension;

  BYTE DispY;

  static int Oldheat = 0;

  static BYTE PTable[256];
  static BYTE XTable[256];
  static BYTE YTable[256];
  static BYTE XPattern[256];
  static BYTE YPattern[256];

  if ((CausticsInit == 0) || (Oldheat != Params->RenderHeat))
  {
  for ( int tt=0; tt < 256 ; tt++ )
         {
	      PTable  [tt] = (BYTE) (sin( (double)tt *( 4*6.283/256.0 ) ) * 15 + 16); //2,16

          XTable  [tt] = (BYTE) (cos( (double)tt *( 4*6.283/256.0 ) ) * 15 + 16); //2,16
          YTable  [tt] = (BYTE) (sin( (double)tt *( 2*6.283/256.0 ) ) * 15 + 16); //4,16
          XPattern[tt] = (BYTE) (cos( (double)tt *( 3*6.283/256.0 ) ) * 14 + 15 );//3,80  16  +128

          YPattern[tt] = (BYTE) (  ( cos(   (double)tt *( 6*6.283/256.0 ) )  ) *18 + 18 ); //7,59  16 +59
         }
     CausticsInit=11;
	 Oldheat = Params->RenderHeat;
    }

  Phase2 = PTable[Phase1 >> 1 ];

  int YProc = 0;

 for  ( int Y = 0 ; Y < (int)Ydimension  ; Y++ )
      {
         BYTE* ThisLine  = BitmapAddr + Y * Xdimension;

		 YProc += 1;
		 int Xindisp = YTable [(YProc+Phase2) & 255 ];

		 for (int X=0; X < (int)Xdimension ; X +=32 ) //+4 ->+32 means unrolled 8x..
		 {
         __asm // clear the new line
		 {
			 mov eax,[ThisLine]
		     add eax,[X]
			 mov ecx,dword ptr [eax+32] //warm cache line
			 mov edx,0x10101010;
			 mov dword ptr [eax],edx
			 mov dword ptr [eax+4],edx
			 mov dword ptr [eax+8],edx
			 mov dword ptr [eax+12],edx
			 mov dword ptr [eax+16],edx
			 mov dword ptr [eax+20],edx
			 mov dword ptr [eax+24],edx
			 mov dword ptr [eax+28],edx
		 }
         //*(DWORD*)(ThisLine+X) = 0x10101010;
		 }

         for ( X=0; X < (int)Xdimension; X++ )
          {
         
          DispY = XTable[ (X+Phase1) & 255 ];

		  int NewDisp =  X + ( YPattern[ DispY + Y] + YPattern[Xindisp + X]  );

		  if ( (NewDisp>4) && (  NewDisp < (int)Xdimension) )
		    {
            *(DWORD*) (ThisLine + NewDisp-4) +=0x07080706;

		    }

         }

     } //Y
}

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
