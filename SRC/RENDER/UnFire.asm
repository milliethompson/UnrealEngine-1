
;/////////////////////////////////////////////////////////////////////////
;//
;//  Fire32.asm  -  486 / P5 / PPro  self-modifying optimized code
;//
;//  version 0.5   19-12-96
;//
;//  This is a part of the Unreal Dynamic Bitmap C++/asm library.
;//  Copyright (C) 1996 by  Epic Megagames & Evolution Software
;//
;//
;/////////////////////////////////////////////////////////////////////////
;
;----------------------------------------------------------
; MASM 6.11d code   for Win95 VC++ 4.1 flat model
;
; Filename: FIRE32.ASM
;
; default options : ML /c /Cx /coff  fire32.asm;
;
; Assemble/build syntax  from within Microsoft Developer Studio
;
;   Build->Settings->[select Fire32] ->Custom build:
;
;   ML /c /Cx /coff  /Fo$(OutDir)\fire32.obj fire32.asm
;
;
; add  /Zd /Zi /Zf  switches to generate debug/browser info.
;
; add  /W0 to ignore the 'line number info for non-CODE segment' warning.
;
;
;                       Speed (measured in CPU cycles per pixel)
;
; Classic fire algorithm:    Pentium120 =  5.0
;                            486DX66    = 11.0  ( about 33 for optimized C++ )
;
; Interpolated Water    :
;                            Pentium120 =  6.0     -> using cache-priming
;                            486DX66    = 14.0
;
;
;
; routine contents:
;
;in .DATA :
;
; SelfModFire        fire update code
; SelfModFireWrap    fire update code with wraparound
;
;in .CODE :
;
; CalculateFire      entry for fire algorithm, calls SelfModFire
; CalcWrapFire       entry for fire algorithm, calls SelfModFireWrap
; MasmRedrawSparks   fire spark drawing routine (no SM code needed)
;
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



.586                 ; 486 or 586 don't differ in MASM output here.
                     ; .MODEL ... C tells the assembler that parameters
.MODEL flat , C      ;            are pushed from right to left.


.DATA

assume CS: _DATA    ; needed for assembling code in the data segment

EXTERNDEF SpeedRindex:DWORD;

 MAXSPARKS = 1024   ; must be same as in FIRENGIN.H !

;==================================

 Advb EQU byte ptr [011111111h]   ;; dummy offsets to patch later
 Advw EQU word ptr [011111111h]

;==================================









;============================================================================

SelfModFire PROC

   ;;;;;;;;;;;;;;;
   ;;
   ;;             d1 d2       = BP    0 1
   ;;  cells    0  1  2  3    = SI -1 0 1 2         [+line]
   ;;              a  b       = DI    0 1           [+line+line]
   ;;
   ;; Bitmap:
   ;;  0 ....1n-1         xn = 'LastPixel'
   ;;  1n....2n-1
   ;;  2n....3n-1
   ;;  ..
   ;;  xn....
   ;;
   ;;;;;;;;;;;;;;;

   ;.................... Patch all the indexes / line-increments

   MOV EAX,011111111h
           org $-4
           LocalLineXSize DD 0

   MOV Cell1_Patch0,EAX

   INC EAX
   MOV Cell2_Patch0,EAX
   MOV Cell2_Patch2,EAX

   MOV EBX,EAX
   ADD EBX,2
   MOV Cell2_Patch1_p2,EBX



   INC EAX
   MOV Cell3_Patch1,EAX

   SUB EAX,2
   MOV Cell3_Patch2_n2,EAX



   MOV EAX,LocalLineXSize
   ADD EAX,EAX

   MOV CellA_Patch0,EAX
   MOV CellA_Patch1,EAX

   MOV EBX,EAX
   ADD EBX,2
   MOV CellA_Patch2_p2,EBX


   INC EAX
   MOV CellB_Patch1,EAX

   SUB EAX,2
   MOV CellB_Patch2_n2,EAX
   MOV CellB_Patch3_n2,EAX



   MOV EAX,011111111h
           org $-4
           P_RenderTable   DD 0

   MOV Table_Patch1,EAX
   MOV Table_Patch2,EAX
   MOV Table_Patch3,EAX
   MOV Table_Patch4,EAX
   MOV Table_Patch5,EAX

   ;.......................

   MOV  EBP,011111111h
             org $-4
             P_FireBitmap    DD 0


   MOV  EBX,011111111h
            org $-4
            LocalLineYSize   DD 0


   SUB  EBX,2 ;; last 2 lines never calculated

   MOV  EAX,[LocalLineXSize]

   MUL  EBX   ;; result in (EDX):EAX

   ADD  EAX,EBP ;; plus start of bitmap..

   MOV  [EndBitmapCheck], EAX


 ;;========= Y-loop \\
 align 16
 IntoCoreY:

 ;; EndLineCheck must be the loc. of next line..
 ;; ZERO the relevant registers

 MOV EAX,[LocalLineXSize]

   XOR EBX,EBX

 ADD EAX,EBP

   XOR ECX,ECX

 MOV [EndLineCheck],EAX

   XOR EAX,EAX

 ;;;;; EBP points to first destination-pixel in the line.

  MOV AL,Advb[EBP]
         org $-4
         Cell1_Patch0 DD 0

  XOR EDI,EDI ;;Cell0 == 0

  MOV ESI,EAX ;;Cell1

  MOV AL,Advb[EBP]
         org $-4
         Cell2_Patch0 DD 0

  MOV BL,Advb[EBP]          ;; CellA
         org $-4
         CellA_Patch0 DD 0

  ADD ESI,EAX               ;; ESI has Cell1
  ADD EDI,EBX               ;; EDI has Cell0

  jmp short EnterCoreX




 align 16 ;=====; 486 and Pentium Pro care about alignment

 IntoCoreX:


  MOV BL,Advb[EBP]   ;; UV
         org $-4
         CellB_Patch2_n2 DD 0

  ADD ECX,EAX        ;; UV


  MOV ESI,EAX        ;; UV

  MOV AL,Advb[EBP]   ;;pre-load: Cell2  used twice - next 'Cell0'
         org $-4
         Cell2_Patch2 DD 0


  MOV DH,Advb[EBX+ECX]   ;;no AGI if pairing as expected
         org $-4
         Table_Patch4 DD 0
  ;---------------------------- making pixel 0,1
  MOV BL,Advb[EBP]          ;; CellA
         org $-4
         CellA_Patch1 DD 0

  ADD ESI,EAX               ;; ESI has Cell1
  ADD EDI,EBX               ;; EDI has Cell0


  MOV [EBP-2],DX            ;; U pipe only (16-bit prefix)

 EnterCoreX:


  MOV DL,Advb[EDI+ESI] ;; ESI=Cell1+Cell2 , used in both lookups
         org $-4
         Table_Patch1 DD 0     ;; AGI


  MOV EDI,EAX

  MOV AL,Advb[EBP]   ;;Cell3 used twice - next 'Cell1'
         org $-4
         Cell3_Patch1 DD 0


  MOV BL,Advb[EBP]   ;;CellB
         org $-4
         CellB_Patch1 DD 0

  ADD ESI,EAX


  MOV ECX,EAX

  MOV AL,Advb[EBP]   ;;Cell2  used twice - next 'Cell0'
         org $-4
         Cell2_Patch1_p2 DD 0


  MOV DH,Advb[EBX+ESI] ;; no AGI if pairing as expected
         org $-4
         Table_Patch2   DD 0
  ;---------------------------- making pixel 2,3
  MOV BL,Advb[EBP]
         org $-4
         CellA_Patch2_p2 DD 0

  ADD ECX,EAX     ;; UV - U
  ADD EDI,EBX     ;; UV - V

  MOV [EBP+0],DX  ;; U pipe only ..

  ADD EBP,4       ;;

  MOV DL,Advb[EDI+ECX] ;; ECX=Cell1+Cell2 , used in both lookups
         org $-4
         Table_Patch3 DD 0

  MOV EDI,EAX

CMP  EBP, 011111111h
     org  $-4
     EndLineCheck DD 0

  MOV AL,Advb[EBP]   ;;Cell3 used twice - next 'Cell1'
         org $-4
         Cell3_Patch2_n2 DD 0


JB   short IntoCoreX   ;; bailout so last pixel sampled (cell3) == 0...

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


OutCoreX:

;;========= finish the last 2  pixels in this line / ignore AL/Cell3

  MOV BL,Advb[EBP]   ;;CellB
         org $-4
         CellB_Patch3_n2 DD 0

  ;;;;;;;;; EBP got advanced +LineLen ; check for end of whole bitmap now.
  CMP EBP,01111111h
      org $-4
      EndBitmapCheck DD 0

   MOV DH,Advb[EBX+ECX] ;; no AGI if pairing as expected
          org $-4
          Table_Patch5 DD 0

   MOV [EBP+2-4],DX

  JB   IntoCoreY

;========= Y-loop //

 RET


SelfModFire ENDP
;================================================================












;================================================================


align 16

SelfModFireWrap PROC

   ;;;;;;;;;;;;;;;
   ;;
   ;;             d1 d2       = BP    0 1
   ;;  cells    0  1  2  3    = SI -1 0 1 2         [+line]
   ;;              a  b       = DI    0 1           [+line+line]
   ;;
   ;; Bitmap:
   ;;  0 ....1n-1         xn = 'LastPixel'
   ;;  1n....2n-1
   ;;  2n....3n-1
   ;;  ..
   ;;  xn....
   ;;
   ;;;;;;;;;;;;;;;

   MOV EAX,011111111h
           org $-4
           WP_RenderTable   DD 0

   MOV WTable_Patch1,EAX
   MOV WTable_Patch2,EAX
   MOV WTable_Patch3,EAX
   MOV WTable_Patch4,EAX
   MOV WTable_Patch5,EAX

   ;.................... Patch all the indexes / line-increments


   ;.DO first Y-2 lines -.. ; init registers to jump into inner loop

   ;; Patch cell line 1
   MOV EAX,011111111h
           org $-4
           WLocalLineXSize DD 0
   call PatchCellLine1

   ;;; Patch Cell Line 2
   MOV EAX,WLocalLineXSize
   ADD EAX,EAX
   call PatchCellLine2


   MOV  EBP,011111111h
             org $-4
             WP_FireBitmap    DD 0


   MOV  EBX,011111111h
            org $-4
            WLocalLineYSize   DD 0

   SUB  EBX,2 ;; last 2 lines not calculated

   MOV  EAX,[WLocalLineXSize]

   MUL  EBX   ;; result in (EDX):EAX

   MOV  WBitmapSize,EAX

   ADD  EAX,EBP ;; plus start of bitmap..

   MOV  [WEndBitmapCheck], EAX

   Call WstartYLoop  ; first Y-2 lines

   ;.......................
   ;.DO line Y-2 line

   ;; Patch cell line 1: normal, so leave unpatched..
   ;;MOV EAX,011111111h
   ;;        org $-4
   ;;        WLocalLineXSize DD 0
   ;;call PatchCellLine1

   ;;; Patch Cell Line 2 ;; instead of + 2*Xsize, now "- (Ysize-2)*Xsize"

   MOV EAX,WBitmapSize ;;== (Ysize-2)*Xsize
   NEG EAX ;; use this negative to patch cell line 2

   call PatchCellLine2

   ;;;MOV  EBP,WP_FireBitmap ;use EBP as it was returned...

   MOV  EAX,WLocalLineXSize
   ADD  WEndBitmapCheck, EAX ;; new limit one line further...

   Call WstartYLoop  ; do line Y-2

   ;.......................
   ;.DO line Y-1 line

   ;; Patch cell line 1: instead of 1*Xsize, now - (Ysize-1)*Xsize

   MOV  EAX,WBitmapSize      ;; has  (Ysize-2)*Xsize now..
   ADD  EAX,WLocalLineXSize ;; now: (Ysize-1)*Xsize now..
   NEG  EAX

   call PatchCellLine1

   ; cell line 2 already patched to wrap around..

   ; keep EBP as it was returned ...

   MOV  EAX,WLocalLineXSize
   ADD  [WEndBitmapCheck], EAX ;; new limit one line further...

   Call WstartYLoop  ; do line Y-1
   ;.......................


   RET

   WBitmapSize DD 0


;============================
 PatchCellLine1 label near

   MOV ECX,EAX ;save

   MOV WCell1_Patch0,EAX

   INC EAX
   MOV WCell2_Patch0,EAX
   MOV WCell2_Patch2,EAX

   MOV EBX,EAX
   ADD EBX,2
   MOV WCell2_Patch1_p2,EBX

   INC EAX
   MOV WCell3_Patch1,EAX

   SUB EAX,2
   MOV WCell3_Patch2_n2,EAX

   ;;;;;;;;;;;

   MOV EAX,ECX    ;; WLocalLineXSize=EAX? -> not always!
   ADD EAX,WLocalLineXSize ;; go to end of line, whatever ECX was...
   DEC EAX
   MOV WCell0_WrapPatch0,EAX ;; put in (+X-1) := as for Cell2 -2...
   ;;;like WCell1_Patch0 but +( Xsize -1)

   MOV EAX,ECX
   SUB EAX,WLocalLineXSize ;; to beginning of line whatever ECX was

   ADD EAX,2 -2 ;; end of row, - Xsize (+2 for addressing cell3)
                ;; -2 for EBP bias.. ==0 !
   MOV WCell3_WrapPatch0_n2,EAX ;; put in (-X-?) := as for cell2 +1
   ;;;;;;;;;

 RETN

;============================
 PatchCellLine2 label near

   MOV WCellA_Patch0,EAX
   MOV WCellA_Patch1,EAX

   MOV EBX,EAX
   ADD EBX,2
   MOV WCellA_Patch2_p2,EBX

   INC EAX
   MOV WCellB_Patch1,EAX

   SUB EAX,2
   MOV WCellB_Patch2_n2,EAX
   MOV WCellB_Patch3_n2,EAX

 RETN
;============================



 ;;========= Y-loop \\
 align 16

 WstartYLoop label near
 WIntoCoreY:


 ;; EndLineCheck must be the loc. of next line..
 ;; ZERO the relevant registers

 MOV EAX,[WLocalLineXSize]

   XOR EBX,EBX

 ADD EAX,EBP

   XOR ECX,ECX

 MOV [WEndLineCheck],EAX

   XOR EAX,EAX

 ;;;;; EBP points to first destination-pixel in the line.

  MOV BL,Advb[EBP]
         org $-4
         WCell0_WrapPatch0 DD 0  ;; get WRAP-around left value = end of line..

  MOV AL,Advb[EBP]
         org $-4
         WCell1_Patch0 DD 0

  MOV EDI,EBX ;; XOR EDI,EDI ;; Cell0 == 0, or wrap-around value

  MOV ESI,EAX ;;Cell1

  MOV AL,Advb[EBP]
         org $-4
         WCell2_Patch0 DD 0

  MOV BL,Advb[EBP]          ;; CellA
         org $-4
         WCellA_Patch0 DD 0

  ADD ESI,EAX               ;; ESI has Cell1
  ADD EDI,EBX               ;; EDI has Cell0

  jmp short WEnterCoreX


 align 16 ;=====; useful for 486, Pentium don't care, PPro *does* care

 WIntoCoreX:

  MOV BL,Advb[EBP]   ;; UV
         org $-4
         WCellB_Patch2_n2 DD 0

  ADD ECX,EAX        ;; UV


  MOV ESI,EAX        ;; UV

  MOV AL,Advb[EBP]   ;;pre-load: Cell2  used twice - next 'Cell0'
         org $-4
         WCell2_Patch2 DD 0


  MOV DH,Advb[EBX+ECX]   ;;no AGI if pairing as expected
         org $-4
         WTable_Patch4 DD 0
  ;---------------------------- making pixel 0,1
  MOV BL,Advb[EBP]          ;; CellA
         org $-4
         WCellA_Patch1 DD 0

  ADD ESI,EAX               ;; ESI has Cell1
  ADD EDI,EBX               ;; EDI has Cell0


  MOV [EBP-2],DX            ;; U pipe only (16-bit prefix)


 WEnterCoreX:


  MOV DL,Advb[EDI+ESI] ;; ESI=Cell1+Cell2 , used in both lookups
         org $-4
         WTable_Patch1 DD 0     ;; AGI


  MOV EDI,EAX

  MOV AL,Advb[EBP]   ;;Cell3 used twice - next 'Cell1'
         org $-4
         WCell3_Patch1 DD 0



  MOV BL,Advb[EBP]   ;;CellB
         org $-4
         WCellB_Patch1 DD 0

  ADD ESI,EAX


  MOV ECX,EAX

  MOV AL,Advb[EBP]   ;;Cell2  used twice - next 'Cell0'
         org $-4
         WCell2_Patch1_p2 DD 0


  MOV DH,Advb[EBX+ESI] ;; no AGI if pairing as expected
         org $-4
         WTable_Patch2   DD 0
  ;---------------------------- making pixel 2,3
  MOV BL,Advb[EBP]
         org $-4
         WCellA_Patch2_p2 DD 0

  ADD ECX,EAX     ;; UV - U
  ADD EDI,EBX     ;; UV - V

  MOV [EBP+0],DX  ;; U pipe only ..

  ADD EBP,4       ;;

  MOV DL,Advb[EDI+ECX] ;; ECX=Cell1+Cell2 , used in both lookups
         org $-4
         WTable_Patch3 DD 0

  MOV EDI,EAX


  MOV AL,Advb[EBP]   ;;Cell3 used twice - next 'Cell1'
         org $-4
         WCell3_Patch2_n2 DD 0

 CMP  EBP, 011111111h
      org  $-4
      WEndLineCheck DD 0

                       ;;
JB   short WIntoCoreX   ;; bailout so last pixel sampled (cell3) == 0...

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

WOutCoreX:

;;========= finish the last 2  pixels in this line /
;  Need wrap-aroundignore AL/ CELL3...; AL has the wrong one !

  MOV AL,Advb[EBP]   ;;Cell3 used twice - next 'Cell1'
         org $-4
         WCell3_WrapPatch0_n2 DD 0

  MOV BL,Advb[EBP]   ;;CellB
         org $-4
         WCellB_Patch3_n2 DD 0

  ADD ECX,EAX ; ECX should become cell 1,2,3 with cell3 a wraparound..


  ;;;;;;;;; EBP got advanced +LineLen ; check for end of whole bitmap now.
  CMP EBP,01111111h
      org $-4
      WEndBitmapCheck DD 0


   MOV DH,Advb[EBX+ECX] ;; no AGI if pairing as expected
          org $-4
          WTable_Patch5 DD 0

   MOV [EBP+2-4],DX


  JB   WIntoCoreY


;========= Y-loop //

 RETN

;======================//


SelfModFireWrap ENDP
;================================================================



;================================================================
.CODE


CalculateFire PROC PBitmap:DWORD, \
                    Ptable:DWORD, \
                      XVar:DWORD, \
                      YVar:DWORD


 ;; VARIABLES: Bitmap pointer, Table pointer, Xsize, Ysize

 MOV EAX,PBitmap
 MOV P_FireBitmap  ,EAX

 MOV EAX,PTable
 MOV P_RenderTable ,EAX

 MOV EAX,XVar
 MOV LocalLineXSize,EAX

 MOV EAX,YVar
 MOV LocalLineYSize,EAX

  PUSH EBX
  PUSH EBP
  PUSH ESI
  PUSH EDI

 call SelfModFire

  POP EDI
  POP ESI
  POP EBP
  POP EBX

 RET  ; MASM makes this into Leave //  MOV sp,bp : pop BP : ret

CalculateFire ENDP

;================================================================


CalcWrapFire PROC PBitmap:DWORD, \
                    Ptable:DWORD, \
                      XVar:DWORD, \
                      YVar:DWORD

 ;; VARIABLES: Bitmap pointer, Table pointer, Xsize, Ysize

 MOV EAX,PBitmap
 MOV WP_FireBitmap  ,EAX

 MOV EAX,PTable
 MOV WP_RenderTable ,EAX

 MOV EAX,XVar
 MOV WLocalLineXSize,EAX

 MOV EAX,YVar
 MOV WLocalLineYSize,EAX

  PUSH EBX
  PUSH EBP
  PUSH ESI
  PUSH EDI

 call SelfModFireWrap

  POP EDI
  POP ESI
  POP EBP
  POP EBX

 RET        ;!!! _cdecl: no cleanup needed ?!!

CalcWrapFire ENDP








;================================================================

;  MasmRedrawSparks ( Params->BitmapAddr,
;                     Params->Xdimension,
;                     Params->Ydimension,
;                     (BYTE*) &(Params->Spark[0]),
;                     &(Params->SparkNum),
;                   );
;
;struct SparkParams {
;
;      BYTE Type;
;      BYTE Heat;
;      BYTE X;
;      BYTE Y;
;
;      BYTE ParamA;   // alt. usage: X-speed
;      BYTE ParamB;   //             Y-speed
;      BYTE ParamC;   //             Age            Emitter freq.
;      BYTE ParamD;   //             Exp.Time
;       }
;
;Random:
;      MOV ECX,SpeedRindex  // Rindex already set to RandArr offset
;      MOV EDX,SpeedRindex
;      ADD CL,4*53   ;;  53 works better than 63 in BSD/Glibc generator
;      ADD DL,4      ;;
;      MOV EAX,[ECX]
;      MOV SpeedRindex,EDX
;      XOR [EDX],EAX  ;; XOR for shift-register, or ADD as in BSD generator


    SPX equ      byte ptr [ESI+ECX*8+2]
    SPY equ      byte ptr [ESI+ECX*8+3]
 SPType equ      byte ptr [ESI+ECX*8+0]
 SPHeat equ      byte ptr [ESI+ECX*8+1]

 SParamA equ     byte ptr [ESI+ECX*8+4]
 SParamB equ     byte ptr [ESI+ECX*8+5]
 SParamC equ     byte ptr [ESI+ECX*8+6]
 SParamD equ     byte ptr [ESI+ECX*8+7]


 SPXNew     equ  byte ptr [ESI+EDX*8+2]
 SPYNew     equ  byte ptr [ESI+EDX*8+3]
 SPTypeNew  equ  byte ptr [ESI+EDX*8+0]
 SPHeatNew  equ  byte ptr [ESI+EDX*8+1]

 SParamANew equ  byte ptr [ESI+EDX*8+4]
 SParamBNew equ  byte ptr [ESI+EDX*8+5]
 SParamCNew equ  byte ptr [ESI+EDX*8+6]
 SParamDNew equ  byte ptr [ESI+EDX*8+7]




.DATA

 TempSparknum      DD 0
 SP_ImageLimit     DD 0

 SP_BitmapAddr     DD 0
 SP_Xsize          DD 0
 SP_Ysize          DD 0
 SP_SparkArrayPtr  DD 0
 SP_SparkNumPtr    DD 0
 SP_JitterTablePtr DD 0
 SP_HeatPhase      DB 0
 SP_HeatPulse      DB 0


.CODE


MasmRedrawSparks PROC     BitmapAddr:DWORD, \
                                Xdim:DWORD, \
                                Ydim:DWORD, \
                       SparkArrayPtr:DWORD, \
                         SparkNumPtr:DWORD, \
                      JitterTablePtr:DWORD, \
                         HeatPhaseIn:BYTE,  \
                         HeatPulseIn:BYTE

 MOV EAX,BitmapAddr
 MOV SP_BitmapAddr,EAX

 MOV EAX,Xdim
 MOV SP_Xsize,EAX

 MOV EAX,Ydim
 MOV SP_Ysize,EAX

 MOV EAX,SparkArrayPtr
 MOV SP_SparkArrayPtr,EAX

 MOV EAX,SparkNumPtr
 MOV SP_SparkNumPtr,EAX

 MOV EAX,JitterTablePtr
 MOV SP_JitterTablePtr,EAX

 MOV AL,HeatPhaseIn
 MOV SP_HeatPhase,AL

 MOV AL,HeatPulseIn
 MOV SP_HeatPulse,AL

 PUSH EBP
 PUSH EBX
 PUSH ESI
 PUSH EDI
 ;;;;;;;;

 MOV EBX,SP_SparkNumPtr
 MOV EAX,[EBX]
 MOV TempSparknum,EAX

 MOV  EAX,[SP_Ysize]
 SUB EAX,2           ;; only needed for NON-wrapping fire really..
 IMUL EAX,[SP_Xsize]

 MOV  [SP_ImageLimit],EAX

 XOR ECX,ECX ;index for sparks..

 MOV  EBP,[SpeedRindex]

 MOV  EDI,SP_BitmapAddr

 MOV  ESI,SP_SparkArrayPtr

 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 ;;
 ;; keep ESI on Spark array START,
 ;; and ECX as 'counter' (*8) for the individual spark structures.
 ;; TempSparkNum (has _nr_ of last spark +1;
 ;;    ie = 1 means spark[0] is the only one.

 MOV  EAX,TempSparknum ;; any sparks at all ?
 TEST EAX,EAX
 JZ MasmRedrawEnd

 jmp  TrySpark0Again

 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 align 16
 TrySpark0Again:
  MOV AL,SPType
 TrySpark0:
  test AL,AL
  JNZ TrySpark1
  ;;;;;;;;;;;;;;
 SparkType0:
  XOR EBX,EBX
  XOR EAX,EAX
  MOV EDX,SP_Xsize
  MOV BL,SPX
  MOV AL,SPY
  CMP EBX,EDX ;X coord too big ? (or wrap!)
  JAE SkipSpark0

  MUL EDX   ;; 10 cycles..
  ADD EBX,EAX ;; EBX = total dest,  EAX = Y * Xlinesize
  ;;;;;;;;;;;;;;; SET spark: RANDOM byte to [EBX+]
      MOV EAX,EBP   ;set both EDX and ECX to SpeedRindex
      MOV EDX,EBP   ;
      ADD AL,4*53   ;53 works better than 63 in BSD/Glibc generator
      ADD DL,4      ;
      ;;;
  CMP EBX,SP_ImageLimit
  JAE SkipSpark0
      ;;
      MOV EAX,[EAX]
      MOV EBP,EDX
      XOR [EDX],EAX  ;; XOR for shift-register, or ADD as in BSD generator
  ;;;;;;;;;;;;;;;;;;;;;
  MOV byte ptr [EDI+EBX],AL
  ;;;;;;;;;;;;;;;;;;
  SkipSpark0:
  EndSpark0:
  INC ECX
  CMP ECX,TempSparknum
  JB TrySpark0Again
  jmp MasmRedrawEnd
 ;;;;;;;;;;;;;;;;;;;;;



 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 align 16
 TrySpark1Again:
  MOV AL,SPType
 TrySpark1:
  CMP AL,1
  JNE TrySpark2
  ;;;;;;;;;;;;;;
 SparkType1:

      MOV EAX,EBP   ;set both EDX and ECX to SpeedRindex
      MOV EDX,EBP   ;
      ADD AL,4*53   ;53 works better than 63 in BSD/Glibc generator
      ADD DL,4      ;
      MOV EAX,[EAX]
      MOV EBP,EDX
      XOR [EDX],EAX  ;; XOR for shift-register, or ADD as in BSD generator

  MOV EDX,SP_JitterTablePtr

  MOV EBX,EAX ;
  AND EAX,255
  SHR EBX,24  ; 8 bits left in both -  0-255...

  MOV AL,[EDX+EAX] ; x & y jitter adding...
  MOV BL,[EDX+EBX] ;

  MOV EDX,SP_Xsize

  ADD BL,SPX
  ADD AL,SPY
  CMP EBX,EDX ;X coord too big ? (or wrap!)
  JAE SkipSpark1

  IMUL EAX,EDX   ;; 10 cycles..

  ADD  EBX,EAX   ;; EBX = total dest,  EAX = Y * Xlinesize

  ;;;;;;;;;;;;;;; SET spark: RANDOM byte to [EBX+]
      MOV EAX,EBP   ;set both EDX and ECX to SpeedRindex
      MOV EDX,EBP   ;
      ADD AL,4*53   ;53 works better than 63 in BSD/Glibc generator
      ADD DL,4      ;
      ;;;
  CMP EBX,SP_ImageLimit
  JAE SkipSpark1
      ;;
      MOV EAX,[EAX]
      MOV EBP,EDX
      XOR [EDX],EAX  ;; XOR for shift-register, or ADD as in BSD generator
  ;;;;;;;;;;;;;;;;;;
  MOV byte ptr [EDI+EBX],AL
  ;;;;;;;;;;;;;;;;;;
  SkipSpark1:
  EndSpark1:
  INC ECX
  CMP ECX,TempSparknum
  JB TrySpark1Again
  jmp MasmRedrawEnd
 ;;;;;;;;;;;;;;;;;;


 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; type 2: its own HEAT+ heatphase
 align 16
 TrySpark2Again:
  MOV AL,SPType
 TrySpark2:
  CMP AL,2
  JNE Tryspark3
  ;;;;;;;;;;;;;;
 SparkType2:

  XOR EAX,EAX
  XOR EBX,EBX
  MOV EDX,SP_Xsize

  MOV BL,SPX
  MOV AL,SPY

  CMP EBX,EDX ;X coord too big ? (or wrap!)
  JAE SkipSpark2

  IMUL EAX,EDX   ;; 10 cycles..

  ADD  EBX,EAX   ;; EBX = total dest,  EAX = Y * Xlinesize

  ;;;CMP EBX,SP_ImageLimit ;;static one, no check needed...
  ;;;JAE SkipSpark2

  ;;;;;;;;;;;;;;;;;;
  MOV AL,SPHeat
  ADD AL,SP_HeatPhase
  MOV byte ptr [EDI+EBX],AL
  ;;;;;;;;;;;;;;;;;;
  SkipSpark2:
  EndSpark2:
  INC ECX
  CMP ECX,TempSparknum
  JB TrySpark2Again
  jmp MasmRedrawEnd
 ;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;     Type 3: only heatPULSE
 align 16

 TrySpark3Again:
  MOV AL,SPType
 TrySpark3:
  CMP AL,3
  JNE TrySpark4
;;;;;;;;;;;;;;;;;;
 SparkType3:

  XOR EBX,EBX
  XOR EAX,EAX

  MOV EDX,SP_Xsize

  MOV BL,SPX
  MOV AL,SPY

  CMP EBX,EDX ;X coord too big ? (or wrap!)
  JAE SkipSpark3

  IMUL EAX,EDX   ;; 10 cycles..

  ADD  EBX,EAX   ;; EBX = total dest,  EAX = Y * Xlinesize

   MOV AL,SP_HeatPulse

  CMP EBX,SP_ImageLimit
  JAE SkipSpark3
  ;;;;;;;;;;;;;;;;;;
   MOV byte ptr [EDI+EBX],AL
  ;;;;;;;;;;;;;;;;;;
  SkipSpark3:
  EndSpark3:
  INC ECX
  CMP ECX,TempSparknum
  JB TrySpark3Again
  jmp MasmRedrawEnd

;;;;;;;;;;;;;;;;;;;;








;   case  4: // emit sparks of type 128 with random directions
;            if ( (Params->SparkNum < (MaxSparks))
;                  && (Params->EmitFreq > SpeedRand() ) )
;               {     // create it..
;                int NS = Params->SparkNum++;
;                 Params->Spark[NS].X = Params->Spark[S].X;
;                 Params->Spark[NS].Y = Params->Spark[S].Y;
;                 Params->Spark[NS].ParamA = SpeedRand();
;                 Params->Spark[NS].ParamB = SpeedRand();
;                 Params->Spark[NS].Heat = 240; // start heat
;                 Params->Spark[NS].Type = 128; // fixed 1st dynamic type
;               }
;

;;;;;;;;;;;;;;;;     Type 4: invisible, emits sparks of type 128, random dirs.
 align 16

 TrySpark4Again:
  MOV AL,SPType
 TrySpark4:
  CMP AL,4
  JNE TrySpark5

 ;;;;;;;;;;;;;;;;
 SparkType4:

  MOV EDX,TempSparkNum ;; new loaction

  MOV BL,SPX      ;;;;;; location can be depended on
  MOV AL,SPY

  CMP EDX,MaxSparks
  JAE EndSpark4

  MOV SPXNew,BL
  MOV SPYNew,AL

  ;;;;;;;;;;;;;;;;
  MOV EAX,EBP    ; Set both EBX and ECX to SpeedRindex
  MOV EBX,EBP    ;
  ADD AL,4*53    ; 53 works better than 63 in BSD/Glibc generator
  ADD BL,4       ;
  MOV EAX,[EAX]  ;
  MOV EBP,EBX    ;
  XOR [EBX],EAX  ; XOR for shift-register
  ;;;;;;;;;;;;;;;;

  MOV SParamAnew,AL        ;random speed

  MOV SPTypeNew, 128
  MOV SPHeatNew, 240

  MOV SParamBnew,AH        ;random speed

  INC EDX
  MOV TempSparkNum,EDX

  ;;;;;;;;;;;;;;;;;;

  SkipSpark4:
  EndSpark4:
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark4Again
  jmp MasmRedrawEnd
;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;     Type 5: the simple fountain... (emits type 129)
 align 16

 TrySpark5Again:
  MOV AL,SPType
 TrySpark5:
  CMP AL,5
  JNE TrySpark6

 ;;;;;;;;;;;;;;;;
 SparkType5:

  MOV EDX,TempSparkNum ;; new loaction

  MOV BL,SPX      ;;;;;; location can be depended on
  MOV AL,SPY

  CMP EDX,MaxSparks
  JAE EndSpark5

  MOV SPXNew,BL
  MOV SPYNew,AL

  ;;;;;;;;;;;;;;;;
  MOV EAX,EBP    ; Set both EBX and ECX to SpeedRindex
  MOV EBX,EBP    ;
  ADD AL,4*53    ; 53 works better than 63 in BSD/Glibc generator
  ADD BL,4       ;
  MOV EAX,[EAX]  ;
  MOV EBP,EBX    ;
  XOR [EBX],EAX  ; XOR for shift-register
  ;;;;;;;;;;;;;;;;

  AND AH,127
  SUB AH,63
  MOV SParamAnew,AH        ;random Xspeed: symmetric, [-63..64]...
  MOV SParamBnew,129 ;;(256-127)   ;fixed 'fountain' speed; max upwards.

  MOV SPTypeNew, 129   ;emit type 129
  MOV SPHeatNew, 190   ;low intensity heat

  INC EDX
  MOV TempSparkNum,EDX

  ;   Params->Spark[NS].ParamA = (SpeedRand()&127) - 63; //X speed arbit.
  ;   Params->Spark[NS].ParamB =(BYTE) -127; // Y speed UP
  ;   Params->Spark[NS].Heat = 186; // start heat
  ;   Params->Spark[NS].Type = 130; // fixed 3rd dynamic type

  ;;;;;;;;;;;;;;;;;;

  SkipSpark5:
  EndSpark5:
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark5Again
  jmp MasmRedrawEnd
;;;;;;;;;;;;;;;;





;;;;;;;;;;;;;;;;     Type 6: 'triangular' campfire
 align 16

 TrySpark6Again:
  MOV AL,SPType
 TrySpark6:
  CMP AL,6
  JNE TrySpark7

;;;;;;;;;;;;;;;;
 SparkType6:

  MOV EDX,TempSparkNum ;; new loaction

  MOV BL,SPX      ;;;;;; location can be depended on
  MOV AL,SPY

  CMP EDX,MaxSparks
  JAE EndSpark6

  MOV SPXNew,BL
  MOV SPYNew,AL

  ;;;;;;;;;;;;;;;;
  MOV EAX,EBP    ; Set both EBX and ECX to SpeedRindex
  MOV EBX,EBP    ;
  ADD AL,4*53    ; 63 works better than 63 in BSD/Glibc generator
  ADD BL,4       ;
  MOV EAX,[EAX]  ;
  MOV EBP,EBX    ;
  XOR [EBX],EAX  ; XOR for shift-register
  ;;;;;;;;;;;;;;;;

  AND AH,127
  SUB AH,63
  MOV SParamAnew,AH        ;random Xspeed: symmetric, [-63..64]...
  MOV SParamBnew,0 ;;(266-127)   ;fixed 'fountain' speed; max upwards.

  MOV SParamCnew,50  ;; TIME out value.. (lifetime)

  MOV SPTypeNew,130   ;emit type 130 (falling sparks)

  INC EDX
  MOV TempSparkNum,EDX

  ;
  ;   Params->Spark[NS].ParamA = (SpeedRand()&127) - 63; //X speed arbit.
  ;   Params->Spark[NS].ParamB =(BYTE) -127; // Y speed UP
  ;   Params->Spark[NS].Heat = 186; // start heat
  ;   Params->Spark[NS].Type = 130; // fixed 3rd dynamic type
  ;
  ;;;;;;;;;;;;;;;;;;

  SkipSpark6:
  EndSpark6:
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark6Again
  jmp MasmRedrawEnd

;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;     Type 7: 'triangular' campfire
 align 16

 TrySpark7Again:
  MOV AL,SPType
 TrySpark7:
  CMP AL,7
  JNE TrySpark8

;;;;;;;;;;;;;;;;
 SparkType7:

  MOV EDX,TempSparkNum ;; new loaction

  MOV BL,SPX      ;;;;;; location can be depended on
  MOV AL,SPY

  CMP EDX,MaxSparks
  JAE EndSpark7

  MOV SPXNew,BL
  MOV SPYNew,AL

  ;;;;;;;;;;;;;;;;
  MOV EAX,EBP    ; Set both EBX and ECX to SpeedRindex
  MOV EBX,EBP    ;
  ADD AL,4*53    ; 73 works better than 73 in BSD/Glibc generator
  ADD BL,4       ;
  MOV EAX,[EAX]  ;
  MOV EBP,EBX    ;
  XOR [EBX],EAX  ; XOR for shift-register
  ;;;;;;;;;;;;;;;;

  AND AH,63
  add AH,64 ;;64...127
  MOV SParamBnew,-29 ;;(277-127)   ;fixed 'fountain' speed; max upwards.
  MOV SParamAnew,AH        ;random Xspeed:

  MOV SParamCnew,83   ;lifetime (=glowing time)

  MOV SPTypeNew, 130   ;emit type 130 (falling sparks)

  INC EDX
  MOV TempSparkNum,EDX

  ;
  ;   Params->Spark[NS].ParamA = (SpeedRand()&127) - 73; //X speed arbit.
  ;   Params->Spark[NS].ParamB =(BYTE) -127; // Y speed UP
  ;   Params->Spark[NS].Heat = 187; // start heat
  ;   Params->Spark[NS].Type = 130; // fixed 3rd dynamic type
  ;
  ;;;;;;;;;;;;;;;;;;

  SkipSpark7:
  EndSpark7:
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark7Again
  jmp MasmRedrawEnd

;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;     Type 8: 'triangular' campfire
 align 16

 TrySpark8Again:
  MOV AL,SPType
 TrySpark8:
  CMP AL,8
  JNE TrySpark9

;;;;;;;;;;;;;;;;
 SparkType8:

  MOV EDX,TempSparkNum ;; new loaction

  MOV BL,SPX      ;;;;;; location can be depended on
  MOV AL,SPY

  CMP EDX,MaxSparks
  JAE EndSpark8

  MOV SPXNew,BL
  MOV SPYNew,AL

  ;;;;;;;;;;;;;;;;
  MOV EAX,EBP    ; Set both EBX and ECX to SpeedRindex
  MOV EBX,EBP    ;
  ADD AL,4*53    ; 83 works better than 83 in BSD/Glibc generator
  ADD BL,4       ;
  MOV EAX,[EAX]  ;
  MOV EBP,EBX    ;
  XOR [EBX],EAX  ; XOR for shift-register
  ;;;;;;;;;;;;;;;;

  AND AH,63
  SUB AH,128 ;;-64...-128
  MOV SParamBnew,-29 ;;(288-128)   ;fixed 'fountain' speed; max upwards.
  MOV SParamAnew,AH        ;random Xspeed:

  MOV SParamCnew,83   ;lifetime (=glowing time)

  MOV SPTypeNew, 130   ;emit type 130 (falling sparks)

  INC EDX
  MOV TempSparkNum,EDX

  ;
  ;   Params->Spark[NS].ParamA = (SpeedRand()&128) - 83; //X speed arbit.
  ;   Params->Spark[NS].ParamB =(BYTE) -128; // Y speed UP
  ;   Params->Spark[NS].Heat = 188; // start heat
  ;   Params->Spark[NS].Type = 130; // fixed 3rd dynamic type
  ;
  ;;;;;;;;;;;;;;;;;;

  SkipSpark8:
  EndSpark8:
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark8Again
  jmp MasmRedrawEnd

;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;     Type 9: emit up-speeders
 align 16

 TrySpark9Again:
  MOV AL,SPType
 TrySpark9:
  CMP AL,9
  JNE TrySparkHigher

;;;;;;;;;;;;;;;;
 SparkType9:

  MOV EDX,TempSparkNum ;; new loaction

  CMP EDX,MaxSparks
  JAE EndSpark9

  ;;;;;;;;;;;;;;;;
  MOV EAX,EBP    ; Set both EBX and ECX to SpeedRindex
  MOV EBX,EBP    ;
  ADD AL,4*53    ; 83 works better than 83 in BSD/Glibc generator
  ADD BL,4       ;
  MOV EAX,[EAX]  ;
  MOV EBP,EBX    ;
  XOR [EBX],EAX  ; XOR for shift-register
  ;;;;;;;;;;;;;;;;

  AND EAX,0FFFF1F1Fh ; low: 31+256*31

  MOV BL,SPX      ;;;;;; location, will get random translation
  ADD BL,AL
  MOV AL,SPY
  ADD AL,AH
  MOV SPXNew,BL
  MOV SPYNew,AL

  SHR EAX,16 ;; get fresh random..
  ;;;MOV SParamBnew,-29 ;;(288-128)   ;fixed 'fountain' speed; max upwards.

  ;;AND AL,127
  ;;SUB AL,127
  MOV SParamAnew,AL        ;random Xspeed: full range.

  MOV SParamCnew,255   ;lifetime (=glowing time)
  MOV SPTypeNew, 131   ;emit type 131 (up-floating sparks)
  INC EDX
  MOV TempSparkNum,EDX

  ;
  ;   Params->Spark[NS].ParamA = (SpeedRand()&128) - 83; //X speed arbit.
  ;   Params->Spark[NS].ParamB =(BYTE) -128; // Y speed UP
  ;   Params->Spark[NS].Heat = 188; // start heat
  ;   Params->Spark[NS].Type = 130; // fixed 3rd dynamic type
  ;
  ;;;;;;;;;;;;;;;;;;

  SkipSpark9:
  EndSpark9:
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark9Again
  jmp MasmRedrawEnd

;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;     Type 8: 'triangular' campfire
 align 16
 TrySparkHigher:
  CMP AL,10
  JB TrySpark0
  CMP AL,128
  JAE TrySpark128
  ;;; HERE: unknown spark encountered, just skip it//delete it..
  ;;CALL DeleteSpark
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark0Again
  jmp MasmRedrawEnd
;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;TRANSIENT TYPES::::::>


;;;;;;;;;;;;;;;;;;;;     Type 128: only heatPULSE
 align 16

 TrySpark128Again:
  MOV AL,SPType
 TrySpark128:
  CMP AL,128
  JNE TrySpark129
;;;;;;;;;;;;;;;;;;
 SparkType128:

  XOR EBX,EBX
  XOR EAX,EAX

  MOV EDX,SP_Xsize

  MOV BL,SPX
  MOV AL,SPY

  CMP EBX,EDX ;X coord too big ? (or wrap!)
  JAE DelSpark128

  IMUL EAX,EDX   ;; 10 cycles..

  ADD  EBX,EAX   ;; EBX = total dest,  EAX = Y * Xlinesize

  MOV AL,SPHeat
  SUB AL,5
  JBE DelSpark128 ;;glow-out !
  MOV SPHeat,AL  ;; re-store new heat...

  CMP EBX,SP_ImageLimit
  JAE DelSpark128

  MOV byte ptr [EDI+EBX],AL

  ;;;;;;;;;;;;;;;;;; EBX legit goal, now move for next time
  ;;
  ;; MOVE spark:
  ;;

  ;;;;;;;;;;;;;;;;
  MOV EAX,EBP    ; Set both EBX and ECX to SpeedRindex
  MOV EBX,EBP    ;
  ADD AL,4*53    ; 53 works better than 63 in BSD/Glibc generator
  ADD BL,4       ;
 MOV DL,SParamA
  MOV EAX,[EAX]  ;
  MOV EBP,EBX    ;
  XOR [EBX],EAX  ; XOR for shift-register
  ;;;;;;;;;;;;;;;;

 MOV BL,SParamB

  AND EAX,000007F7Fh ;; 127:127 max

  ;;;;;;;;;;;;;;;;;;

  TEST DL,DL
  JS NegX128
  PosX128:
    CMP DL,AL
    JB  ReadyX128
    INC SPX
    jnz ReadyX128
    jmp DelSpark128

  NegX128:
    ADD DL,AL
    JNS ReadyX128
    DEC SPX
    jz  DelSpark128
  ReadyX128:

  ;;;;;;;;;;;;;;;;;;

  TEST BL,BL
  JS NegY128
  PosY128:
     CMP BL,AH
     JB  ReadyY128
     INC SPY
     jnz ReadyY128
     jmp DelSpark128

  NegY128:
     ADD BL,AH
     JNS ReadyY128
     DEC SPY
     jz  DelSpark128
  ReadyY128:

  ;;;;;;;;;;;;;;;;;;

  EndSpark128:
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark128Again
  jmp MasmRedrawEnd


  DelSpark128:
  Call DeleteSpark
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark128Again
  jmp MasmRedrawEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;





;;;;;;;;;;;;;;;;;;;;     Type 129: only FOUNTAIN sparks
 align 16

 TrySpark129Again:
  MOV AL,SPType
 TrySpark129:
  CMP AL,129
  JNE TrySpark130
;;;;;;;;;;;;;;;;;;
 SparkType129:

  XOR EBX,EBX
  XOR EAX,EAX

  MOV EDX,SP_Xsize

  MOV BL,SPX
  MOV AL,SPY

  CMP EBX,EDX ;X coord too big ? (or wrap!)
  JAE DelSpark129

  IMUL EAX,EDX   ;; 10 cycles..

  ADD  EBX,EAX   ;; EBX = total dest,  EAX = Y * Xlinesize

  MOV AL,SPHeat
  SUB AL,2
  JBE DelSpark129 ;; glow-out !
  MOV SPHeat,AL   ;; re-store new heat...

  CMP EBX,SP_ImageLimit
  JAE DelSpark129

  MOV byte ptr [EDI+EBX],AL

  ;;;;;;;;;;;;;;;;;; EBX legit goal, now move for next time
  ;;
  ;; MOVE spark:
  ;;

  ;;;;;;;;;;;;;;;;
  MOV EAX,EBP    ; Set both EBX and ECX to SpeedRindex
  MOV EBX,EBP    ;
  ADD AL,4*53    ; 53 works better than 63 in BSD/Glibc generator
  ADD BL,4       ;
  MOV EAX,[EAX]  ;
  MOV EBP,EBX    ;
  XOR [EBX],EAX  ; XOR for shift-register
  ;;;;;;;;;;;;;;;;

  MOV DL,SParamA
  MOV BL,SParamB

  AND EAX,000007F7Fh ;; 127:127 max

  ;;;;;;;;;;;;;;;;;;

  TEST DL,DL
  JS NegX129
  PosX129:
    CMP DL,AL
    JB  ReadyX129
    INC SPX
    jnz ReadyX129
    jmp DelSpark129

  NegX129:
    ADD DL,AL ;; -127..0 compare to -(0..127) ..by adding;
    JNS  ReadyX129
    DEC SPX
    jz  DelSpark129
  ReadyX129:

  ;;;;;;;;;;;;;;;;;;

  TEST BL,BL
  JS NegY129
  PosY129:
     CMP BL,AH
     JB  ReadyY129
     INC SPY
     jnz ReadyY129
     jmp DelSpark129

  NegY129:
     ADD BL,AH
     JNS ReadyY129 ;;SIGN: neg won out-
     DEC SPY
     jz  DelSpark129
  ReadyY129:

  ;;;;;;;;;;;;;;;;;;
  EndSpark129:
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark129Again
  jmp MasmRedrawEnd

  DelSpark129:
  Call DeleteSpark
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark129Again
  jmp MasmRedrawEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;     Type 130: sparks with GRAVITY acting on 'em
 align 16

 TrySpark130Again:
  MOV AL,SPType
 TrySpark130:
  CMP AL,130
  JNE TrySpark131
;;;;;;;;;;;;;;;;;;
 SparkType130:

  XOR EBX,EBX
  XOR EAX,EAX

  MOV EDX,SP_Xsize

  MOV BL,SPX
  MOV AL,SPY

  CMP EBX,EDX ;X coord too big ? (or wrap!)
  JAE DelSpark130

  IMUL EAX,EDX   ;; 10 cycles..

  ADD  EBX,EAX   ;; EBX = total dest,  EAX = Y * Xlinesize

  MOV AL,SParamC
  SUB AL,1        ;;
  JBE DelSpark130 ;; TIME-out (but constant TEMP)
  MOV SParamC,AL   ;; re-store new heat...

  CMP EBX,SP_ImageLimit
  JAE DelSpark130

  MOV byte ptr [EDI+EBX],160 ;;  constant glow...

  ;;;;;;;;;;;;;;;;;; EBX legit goal, now move for next time
  ;;
  ;; MOVE spark:
  ;;

  ;;;;;;;;;;;;;;;;
  MOV EAX,EBP    ; Set both EDX and ECX to SpeedRindex
  MOV EDX,EBP    ;
  ADD AL,4*53    ; 53 works better than 63 in BSD/Glibc generator
  ADD DL,4       ;
 MOV BL,SParamB
  MOV EAX,[EAX]  ;
  MOV EBP,EDX    ;
  XOR [EDX],EAX  ; XOR for shift-register
  ;;;;;;;;;;;;;;;;
 MOV DL,SParamA


  AND EAX,000007F7Fh ;; 127:127 max

  CMP BL,122
  JG SpeedLimit130 ;127->128 ? don't save then
     ADD EBX,3          ;;STRONG gravity...
     MOV SParamB,BL
  SpeedLimit130:

  ;;;;;;;;;;;;;;;;;;

  TEST DL,DL
  JS NegX130
  PosX130:
    CMP DL,AL
    JB  ReadyX130
    INC SPX
    jnz ReadyX130
    jmp DelSpark130

  NegX130:
    ADD DL,AL ;; -127..0 compare to -(0..127) ..by adding;
    JNS  ReadyX130
    DEC SPX
    jz  DelSpark130
  ReadyX130:

  ;;;;;;;;;;;;;;;;;;

  TEST BL,BL
  JS NegY130
  PosY130:
     CMP BL,AH
     JB  ReadyY130
     INC SPY
     jnz ReadyY130
     jmp DelSpark130

  NegY130:
     ADD BL,AH
     JNS ReadyY130 ;;SIGN: neg won out-
     DEC SPY
     jz  DelSpark130
  ReadyY130:

  ;;;;;;;;;;;;;;;;;;
  EndSpark130:
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark130Again
  jmp MasmRedrawEnd

  DelSpark130:
  Call DeleteSpark
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark130Again
  jmp MasmRedrawEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;     Type 131: sparks with GRAVITY acting on 'em
 align 16

 TrySpark131Again:
  MOV AL,SPType
 TrySpark131:
  CMP AL,131
  JNE TrySpark0
;;;;;;;;;;;;;;;;;;
 SparkType131:

  XOR EBX,EBX
  XOR EAX,EAX

  MOV EDX,SP_Xsize

  MOV BL,SPX
  MOV AL,SPY

  CMP EBX,EDX ;X coord too big ? (or wrap!)
  JAE DelSpark131

  IMUL EAX,EDX   ;; 10 cycles..

  ADD  EBX,EAX   ;; EBX = total dest,  EAX = Y * Xlinesize

  MOV AL,SParamC
  SUB AL,3        ;;
  CMP AL,190
  JBE DelSpark131 ;; TIME-out (but constant TEMP)
  MOV SParamC,AL   ;; re-store new heat...

  CMP EBX,SP_ImageLimit
  JAE DelSpark131

  MOV byte ptr [EDI+EBX],AL ;; TIMEOUT== glow...

  ;;;;;;;;;;;;;;;;;; EBX legit goal, now move for next time
  ;;
  ;; MOVE spark:
  ;;

  ;;;;;;;;;;;;;;;;
  MOV EAX,EBP    ; Set both EDX and ECX to SpeedRindex
  MOV EDX,EBP    ;
  ADD AL,4*53    ; 53 works better than 63 in BSD/Glibc generator
  ADD DL,4       ;
 MOV BL,SParamB
  MOV EAX,[EAX]  ;
  MOV EBP,EDX    ;
  XOR [EDX],EAX  ; XOR for shift-register
  ;;;;;;;;;;;;;;;;
 MOV DL,SParamA


  AND EAX,000007F7Fh ;; 127:127 max

  ;;CMP BL,122
  ;;JG SpeedLimit131 ;127->128 ? don't save then
  ;;   ADD EBX,3          ;;STRONG gravity...
  ;;   MOV SParamB,BL
  ;;SpeedLimit131:

  ;;;;;;;;;;;;;;;;;;

  TEST DL,DL
  JS NegX131
  PosX131:
    CMP DL,AL
    JB  ReadyX131
    INC SPX
    jnz ReadyX131
    jmp DelSpark131

  NegX131:
    ADD DL,AL ;; -127..0 compare to -(0..127) ..by adding;
    JNS  ReadyX131
    DEC SPX
    jz  DelSpark131
  ReadyX131:

  ;;;;;;;;;;;;;;;;;;

  NegY131: ;; By definition...Y upwards fast
     DEC SPY
     jz  DelSpark131
     DEC SPY
     jz  DelSpark131
  ReadyY131:

  ;;;;;;;;;;;;;;;;;;
  EndSpark131:
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark131Again
  jmp MasmRedrawEnd

  DelSpark131:
  Call DeleteSpark
  INC ECX
  CMP ECX,TempSparknum
  JB  TrySpark131Again
  jmp MasmRedrawEnd

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;








align 16

 DeleteSpark label near ;; delete CURRENT (ECX) spark only;

 MOV  EAX,TempSparknum
 TEST EAX,EAX ;; if already zero, exit now
 JZ EndDelete

 MOV EDX,[ESI+EAX*8-8 + 0]     ;; first 4 bytes of LAST spark
 MOV EBX,[ESI+EAX*8-8 + 4]     ;; second 4 bytes of LAST spark

 DEC EAX

 MOV     [ESI+ECX*8   + 0],EDX ;;  overwrites current spark
 MOV TempSparkNum,EAX
 MOV     [ESI+ECX*8   + 4],EBX

 EndDelete:
 RETN

;;;;;;;;;;;;;;;;;;;;;;;;;;

 MasmRedrawEND:

 MOV EBX,SP_SparkNumPtr
 MOV EAX,TempSparkNum
 MOV [EBX],EAX

 MOV  [SpeedRindex],EBP

 POP  EDI
 POP  ESI
 POP  EBX
 POP  EBP

 RET

MasmRedrawSparks ENDP

END







