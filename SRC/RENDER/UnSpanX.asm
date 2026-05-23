;==============================================================================
;
; UnSpanX.asm: Unreal span buffering assembly code.
;
; Copyright 1996  Epic MegaGames, Inc. and Evolution Software.
; This software is a trade secret.
;
; Assembled with MASM 6.11d using Tabs=4  Calling method=__cdecl
;
; Revision history:
;       *  November 1996    created by Erik de Neve
;
;==============================================================================

.586P
.Model FLAT, C


;
; Build syntax in MS Developer Studio:
;
;   Build->Settings-> [select UnSpanX.asm] ->Custom build:
;
;   ml /Fo$(IntDir)\UnSpanX.obj /c /coff /Cp /Cx $(InputPath)
;



;
; **  VC++ definitions of the functions implemented below:  **
;
;
; CopyFromRasterUpdateMASM (FSpanBuffer &AsmThis,
;                           FSpanBuffer &AsmScreen,
;                     const FRasterPoly &AsmRaster);
;
; MergeWithMASM (const FSpanBuffer &Other);
;
;
; CalcRectFromMASM (FSpanBuffer *AsmThis,
;             const FSpanBuffer &Source,
;                          BYTE GridXBits,
;                          BYTE GridYBits,
;                      FMemPool *Mem);
;
;
; CalcLatticeFromMASM (FSpanBuffer *AsmThis,
;                const FSpanBuffer &Source,
;                         FMemPool *Mem);
;



;
; Switches
;

 P5cache = 1   ; = 0 for no p5 cache priming
               ; -> does dummy reads before writing to ensure loading of
               ; cache lines.
               ; ( Switch only implemented in CopyFromRasterUpdate )


;---------------------------
; Definitions
;---------------------------
;
; These structures are needed to get the relative addresses
; of the data structures from their C++ namesake classes
; from Unraster.h / UnSpan.h / UnRender.h.
;


FSpan       STRUCT
    StartX  DWORD ? ;  int     Start;       // Starting X value
    EndX    DWORD ? ;  int     End;         // Ending X value
    Next    DWORD ? ;  FSpan   *Next;       // NULL = no more
FSpan       ENDS

SizeofFSpan = 12

;
;

FMemPool    STRUCT  ; // Simple linear-allocation memory pool
  MemStart  DWORD ? ;  BYTE        *Start;  // Start of memory area
    MemEnd  DWORD ? ;  BYTE        *End;    // End of memory data
    MemTop  DWORD ? ;  BYTE        *Top;    // Top of memory pool
   MemSize  DWORD ? ;  int         Size;
FMemPool    ENDS


;
;

FSpanBuf STRUCT     ; note: C++ name is FSpanBuffer
    StartY  DWORD ? ;  int     StartY;      // Starting Y value
      EndY  DWORD ? ;  int     EndY;        // Last Y value + 1
ValidLines  DWORD ? ;  int     ValidLines;  // Number of lines at beginning (for screen)
     Churn  DWORD ? ;  int     Churn;       // Number of lines added since start
     Index  DWORD ? ;  FSpan   **Index;     // Contains (EndY-StartY) units pointing to first span or NULL.
      List  DWORD ? ;  FSpan   *List;       // Entries
       Mem  DWORD ? ;  FMemPool *Mem;       // Memory pool everything is stored in
FSpanBuf ENDS

;
;

FRasterPoly STRUCT
    StartY  DWORD ? ;  int             StartY;
      EndY  DWORD ? ;  int             EndY;
     Lines  DWORD ? ;  TRasterLineT    Lines[1]; // NOT ptr but ACTUAL data !!!
FRasterPoly ENDS


;
; Note: unlike regular span buffers, the 'Lines[]' in FRasterPoly structures
; are not accessed through an index of pointers, but directly ; they form
; a single object, allocated in one piece with the StartY and EndY variables
; at the beginning.
;

TrasterLine STRUCT
    StartX  DWORD ? ;  int   Start.X  //  start of line
      EndX  DWORD ? ;  int     End.X  //    end of line
TrasterLine ENDS


;
; Macros with no effect
;

u           TEXTEQU <>  ; Denotes u-pipe instruction
v           TEXTEQU <>  ; Denotes v-pipe instruction
um          TEXTEQU <>  ; Denotes mandatory u-pipe instruction
vm          TEXTEQU <>  ; Denotes mandatory v-pipe instruction
xxx         TEXTEQU <>  ; Implied nop


;
; Structures
;

DD_GLOBAL MACRO Var:REQ
    PUBLIC Var
    Var DD ?
ENDM

DB_GLOBAL MACRO Var:REQ
    PUBLIC Var
    Var DB ?
ENDM

;
; Stack variable allocation macros - using ESP as base pointer
; since EBP is in use as a general register.
;
; The orthodox MASM method is to do it with "LOCAL varname:DWORD"
; but then you are limited to using EBP as base pointer.
;

DEFINEFRAMEVAR MACRO  varname, disp
    varname TEXTEQU <dword ptr [ESP + &disp + StackDisp]>
ENDM

ALLOCDW MACRO Varname
    DEFINEFRAMEVAR  Varname, %StackFrameCnt
    StackFrameCnt = StackFrameCnt + 4
ENDM






 .CODE


align 16
;--------------------------------------------------------------------------------
; SpanBuffer MERGE (MergeWith)
;--------------------------------------------------------------------------------
;
;void FSpanBuffer::MergeWith(const FSpanBuffer &Other)
;
;
;extern "C" int _cdecl MergeWithMASM (FSpanBuf *AsmThis,
;                                     FSpanBuf &AsmOther);
;
;
; Fast merge, by resizing and allocating new 'This' spans,
; but not changing the 'Other' buffer.
; Allocate a new index if needed, and copy the old one into that,
; zeroing any extra new space before and after in the index.
;
; * Uses the memory pool of 'This' spanbuffer.
;
; * Fully maintained ValidLines ( though INPUTS can have
;   boolean 'ValidLines' )
;

MergeWithMASM PROC    Thiz:DWORD, \
                     Other:DWORD

;ssssssssssssssssssssssssssssssssssssssssssssssssssss

    StackFrameCnt = 0
    StackDisp     = 0

    AllocDW  LastOtherIndex

    ; Direct access to caller-pushed vars:
    ; Starting at 4 * 5 = 1 return address + 4 pushed registers.

    DWThizPtr    TEXTEQU  < dword ptr [ESP+StackFrameCnt + 4*5 +StackDisp] >
    DWOtherPtr   TEXTEQU  < dword ptr [ESP+StackFrameCnt + 4*6 +StackDisp] >

;ssssssssssssssssssssssssssssssssssssssssssssssssssss

    ;; MASM generates: push ebp ; mov ebp,esp
    push    ebx
    push    esi
    push    edi

    sub     esp,StackFrameCnt
    ;-----------

    mov     edi,Thiz     ;
    mov     esi,Other    ;  both loaded using EBP

    mov     edx,[edi].FSpanBuf.Mem


    mov     eax,[edi].FSpanBuf.StartY ;  This.StartY
    mov     ebx,[esi].FSpanBuf.StartY ; Other.StartY

    mov     ebp,[edx].FMemPool.MemTop
    add     ebp, 3
    mov     ecx,[edi].FSpanBuf.EndY ;  This.EndY
    and     ebp,(not 3)             ; align on 32-bit DWORD boundary.
    mov     edx,[esi].FSpanBuf.EndY ; Other.EndY

    ;if ((Other.StartY < StartY) || (Other.EndY > EndY))
    ; // Must reallocate and copy index

    cmp     ebx,eax
    jb      SetNewStartY
    mov     ebx,eax       ; ebx = ourmin(ebx,eax)  = newstartY

    cmp     edx,ecx
    ja      SetNewEndY    ;

    ;-------------------
    ; //
    ; // Now merge other span into this one:
    ; //
   ;================
   PrepareMerging:
   ;================

    ; FSpan **ThisIndex  = &Index       [Other.StartY - StartY];
    ; FSpan **OtherIndex = &Other.Index [0];
    ; FSpan *TempSpan,**PrevLink;

    mov     ecx,[edi].FSpanBuf.ValidLines

    mov     eax,[esi].FSpanBuf.StartY
    mov     edx,[esi].FSpanBuf.EndY

    mov     ebx,[edi].FSpanBuf.StartY
    mov     edi,[edi].FSpanBuf.Index

    sub     edx,eax      ; Other.EndY - Other.StartY = nr of lines to merge
    sub     eax,ebx      ; Other.StartY - StartY ;= first line to do

    lea     edx,[4*edx]  ; Physical size of index to traverse

    mov     esi,[esi].FSpanBuf.Index  ; FSpan  **OtherIndex = &Other.Index [0];
    lea     edi,[edi+4*eax]           ; FSpan  **ThisIndex  = &Index [Other.StartY - StartY];

    ;; merge edx lines,  'Other' source = esi,  destination = edi

    add     edx,esi
    mov     LastOtherIndex,edx

    ;; last Other line to process =  esi+4*(edx-1)

   ;==================================
   LoopMergeWith:
   ;==================================
    ;     PrevLink    = ThisIndex;
    ;     ThisSpan    = *(ThisIndex++);
    ;     OtherSpan   = *(OtherIndex++);

    mov     edx,edi   ; EDX = PrevLink
    mov     eax,[edi] ; ThisSpan
    mov     ebx,[esi] ; OtherSpan
    add     esi,4
    add     edi,4

    ; while (ThisSpan && OtherSpan)
    test    ebx,ebx
    jz      ZeroOthers

    push    esi
    push    edi
   StackDisp = 8
    test    eax,eax
    jz      WhileOtherSpan

    ;==========================================================
    ;     while (ThisSpan && OtherSpan)
    ;-------------------------------------
    ; if (OtherSpan->End < ThisSpan->Start) // Link OtherSpan in completely before ThisSpan:
    ;             {
    ;             *PrevLink = TempSpan= (FSpan *)Mem->GetFast4(sizeof(FSpan));
    ;             TempSpan->Start     = OtherSpan->Start;
    ;             TempSpan->End       = OtherSpan->End;
    ;             TempSpan->Next      = ThisSpan;
    ;             PrevLink            = &TempSpan->Next;
    ;             //
    ;             OtherSpan           = OtherSpan->Next;
    ;             //
    ;             ValidLines++;
    ;             }
   ;------------------
   WhileBothLinks:   ; ThisSpan = eax, OtherSpan = ebx

    mov     esi,[eax].FSpan.StartX ;;ThisSpan->Start
    mov     edi,[ebx].FSpan.EndX   ;; OtherSpan->End
    cmp     edi,esi
    jae     MergeInto

   ;----------------
   OthersBefore:
   ;----------------
    mov     esi,[ebx].FSpan.StartX     ;;OtherSpan->Start

    mov     [edx],ebp                  ;;*PrevLink = GetFast()
    mov     [ebp].FSpan.StartX, esi

    lea     edx,[ebp].FSpan.Next ;PrevLink            = &TempSpan->Next;

    mov     [ebp].FSpan.EndX,   edi ;= OtherSpan.End
    mov     [ebp].FSpan.Next,   eax ;= ThisSpan

    mov     ebx,[ebx].FSpan.Next ;OtherSpan = OtherSpan ->Next

    add     ebp,SizeofFSpan
    inc     ecx    ;This.ValidLines++
    test    ebx,ebx
    jz      EndMainWhile  ;Zero: just close the span now since Other = null

    mov     esi,[eax].FSpan.StartX ;; ThisSpan->Start
    mov     edi,[ebx].FSpan.EndX   ;; OtherSpan->End
    cmp     edi,esi
    jb      OthersBefore
    ;-------------------


    ;-------------------
    ;         else
    ;
    ;             if (OtherSpan->Start <= ThisSpan->End) // Merge OtherSpan into ThisSpan:
    ;             {
    ;             *PrevLink           = ThisSpan;
    ;             ThisSpan->Start     = OurMin(ThisSpan->Start,OtherSpan->Start);
    ;             ThisSpan->End       = OurMax(ThisSpan->End,  OtherSpan->End);
    ;             TempSpan            = ThisSpan; // For maintaining End and Next
    ;             //
    ;             PrevLink            = &ThisSpan->Next;
    ;             ThisSpan            = ThisSpan->Next;
    ;             OtherSpan           = OtherSpan->Next;
    ;             //
    ;             while (1)
    ;                 {
    ;                 if (ThisSpan&&(ThisSpan->Start <= TempSpan->End))
    ;                     // Other span encompasses thisspans
    ;                     {
    ;                     TempSpan->End = OurMax(TempSpan->End,ThisSpan->End);
    ;                     ThisSpan      = ThisSpan->Next;
    ;                     ValidLines--;
    ;                     }
    ;                 else if (OtherSpan&&(OtherSpan->Start <= TempSpan->End))
    ;                     // Other spans append to temp span
    ;                     {
    ;                     TempSpan->End = OurMax(TempSpan->End,OtherSpan->End);
    ;                     OtherSpan     = OtherSpan->Next;
    ;                     }
    ;                 else break;
    ;                 };
    ;             }
    ;
   ;------------------
   MergeInto:
   ;------------------
    ;   checked:     (OtherSpan->End >= ThisSpan->Start)
    ;             if (OtherSpan->Start <= ThisSpan->End) // Merge OtherSpan into ThisSpan:
    mov     esi,[ebx].FSpan.StartX
    mov     edi,[eax].FSpan.EndX
    cmp     esi,edi
    ja      BeforeOtherSpan

    mov     edi,[eax].FSpan.StartX
    mov     [edx],eax
    cmp     edi,esi
    jb      Min_EDI
    nop
    mov     [eax].FSpan.StartX,esi
    Min_EDI:

    lea     edx,[eax].FSpan.Next   ; PrevLink  = &ThisSpan->Next;
                                   ; also doubles as 'tempspan'
    mov     edi,[eax].FSpan.EndX   ;
    mov     eax,[eax].FSpan.Next   ; ThisSpan  =  ThisSpan->Next;

   PreStartMerge:
    mov     esi,[ebx].FSpan.EndX   ;
    mov     ebx,[ebx].FSpan.Next   ; OtherSpan = OtherSpan->Next;
    cmp     esi,edi                ; max (this->end, other->end)
    jbe     MaxThis                ;

    ; and - equal means exit too actually ?
   MaxOther:
    nop
    mov     [edx-offset(FSpan.Next)].FSpan.EndX,esi  ; TempSpan->End
    test    eax,eax
    jz      OutThis0
    dec     ecx        ;This.ValidLines--   -> but undo if no linkup
    mov     edi,[eax].FSpan.StartX
    cmp     edi,esi
    ja      OutThis1

    mov     edi,[eax].FSpan.EndX
    mov     eax,[eax].FSpan.Next
    cmp     edi,esi
    jbe     MaxOther ;if too small, maybe next of ours ?

   MaxThis:
    nop
    mov     [edx-offset(FSpan.Next)].FSpan.EndX,edi  ; TempSpan->End
    test    ebx,ebx
    jz      EndMainWhile      ;total end (only eax may have a 'tail')
    nop
    mov     esi,[ebx].FSpan.StartX
    cmp     esi,edi
    jbe     PreStartMerge

    test    eax,eax         ; ebx != 0
    jz      WhileOtherSpan
    jmp     WhileBothLinks

   align 16
   OutThis0:                ; eax == 0
    test    ebx,ebx
    jz      EndMainWhile
    jmp     WhileOtherSpan

   OutThis1:                ; eax != 0
    inc     ecx
    test    ebx,ebx
    jnz     WhileBothLinks
    jmp     EndMainWhile


    ;
    ;         else // This span is entirely before the other span; keep it.
    ;             {
    ;             *PrevLink           = ThisSpan;
    ;             PrevLink            = &ThisSpan->Next;
    ;             ThisSpan            = ThisSpan->Next;
    ;             };
    ;keep traversing 'this' spans until one Other links up
    ;

   ;---------------
   BeforeOtherSpan:
   ;---------------
    ; this span's end < other span's start so keep it..

    mov     [edx], eax
    lea     edx,  [eax].FSpan.Next
    mov     eax,  [eax].FSpan.Next
    mov     esi,  [ebx].FSpan.StartX  ;needed below...
    test    eax,eax
    jz      WhileOtherSpan ; ebx != 0 so digest those

   IntoBefore:    ; keep linking This spans as long as Others don't link
    nop
    mov     edi,[eax].FSpan.EndX
    cmp     edi,esi
    jae     WhileBothLinks
    lea     edx,  [eax].FSpan.Next
    mov     eax,  [eax].FSpan.Next
    test    eax,eax
    jnz     IntoBefore
    ;-------------------------------------

    ;==========================================================
    ;     while (OtherSpan) // Just append spans from OtherSpan:
    ;-------------------------------------
    ;     while (OtherSpan) // Just append spans from OtherSpan:
    ;         {
    ;         *PrevLink = TempSpan    = (FSpan *)Mem->GetFast4(sizeof(FSpan));
    ;         TempSpan->Start         = OtherSpan->Start;
    ;         TempSpan->End           = OtherSpan->End;
    ;         PrevLink                = &TempSpan->Next;
    ;         //
    ;         OtherSpan               = OtherSpan->Next;
    ;         //
    ;         ValidLines++;
    ;         };
   ;-------------------
   WhileOtherSpan:     ;// ebx nonzero - keep copying OtherSpans
   ;-------------------
    mov     esi,[ebx].FSpan.StartX ;;OtherSpan->Start
    mov     edi,[ebx].FSpan.EndX   ;;OtherSpan->End

    inc     ecx                    ;This.ValidLines++
    mov     [edx],ebp                  ;;*PrevLink = GetFast()

    mov     edx,[ebp+20]         ;P5 cache priming for writes
    lea     edx,[ebp].FSpan.Next ;PrevLink            = &TempSpan->Next;

    mov     [ebp].FSpan.StartX, esi
    mov     [ebp].FSpan.EndX,   edi   ;= OtherSpan.End

    mov     ebx,[ebx].FSpan.Next ;OtherSpan = OtherSpan ->Next
    add     ebp,SizeofFSpan

    test    ebx,ebx
    jnz     WhileOtherSpan
    ;------------------------


   ;==============================
   EndMainWhile:
   ;==============================
    ;       *PrevLink = ThisSpan;
    pop     edi
    pop     esi
    StackDisp = 0

   ZeroOthers:          ;skips to here if EBX==0
    mov     [edx],eax   ;can be NULL, or appends start of a 'tail'.
    mov     eax,LastOtherIndex
    cmp     esi,eax
    jb      LoopMergeWith
    ;==============================

    ;=== epilog code ===
    mov     eax,DWThizPtr

    add     esp,StackFrameCnt ;; restore stack pointer

    mov     ebx,[eax].FSpanBuf.Mem
    mov     [eax].FSpanBuf.ValidLines, ecx
    pop     edi
    pop     esi
    mov     [ebx].FMemPool.MemTop,     ebp ;; put back memtop of this' mempool
    pop     ebx
    pop     ebp
    retn
    ;; 'ret'   would make MASM insert "LEAVE" =  mov  esp,ebp / pop ebp
    ;==================================================================




    ;--------------------------------------------------------
    ;// assert: ((Other.StartY < StartY) || (Other.EndY > EndY))
    ;// Must reallocate and copy index
    ;    {
    ;    int NewStartY = OurMin(StartY,Other.StartY);
    ;    int NewEndY   = OurMax(EndY,  Other.EndY);
    ;    int NewNum    = NewEndY - NewStartY;
    ;    FSpan **NewIndex = (FSpan **)Mem->Get(NewNum*sizeof(FSpan *));
    ;    //
    ;    mymemset(&NewIndex[0                    ],0,    (StartY-NewStartY)*sizeof(FSpan *));
    ;    memcpy(&NewIndex[StartY-NewStartY     ],Index,(EndY     -StartY)*sizeof(FSpan *));
    ;    mymemset(&NewIndex[NewNum-(NewEndY-EndY)],0,    (NewEndY  -EndY  )*sizeof(FSpan *));
    ;    //
    ;    StartY = NewStartY;
    ;    EndY   = NewEndY;
    ;    Index  = NewIndex;
    ;    //
    ;    };

    ReallocateIndex:
    ;==================
    SetNewStartY:       ;; eax = StartY, ebx= NewStartY (min)

    cmp     edx,ecx
    ja      SetNewEndY
    mov     edx,ecx     ;; edx = NewEndY

    SetNewEndY:         ;; ecx = EndY  , edx= NewEndY (max)
    ;...................

    mov     [edi].FSpanBuf.StartY ,ebx
    mov     [edi].FSpanBuf.EndY   ,edx

    push    edx
    sub     edx,ebx ;NewEndY - NewStartY

    ;    Index  = NewIndex = Mem->Get(NewNum*sizeof(FSpan *));
    mov     esi,[edi].FSpanBuf.Index     ; ESI= this.index
    mov     [edi].FSpanBuf.Index,ebp     ; new index start
    mov     edi,ebp                      ; edi = &NewIndex[0]

    lea     ebp,[ebp+4*edx]  ; Mem->Get(NewNum*sizeof(FSpan *));
    pop     edx

    ;; StartY  = eax
    ;; EndY    = ecx
    ;; NewStartY = ebx
    ;; NewEndY   = edx

    ;; ! need more dummy reads from [edi+n*32] for p5 cache priming

    ;    mymemset(&NewIndex[0],0,(StartY-NewStartY)*sizeof(FSpan *));
    push    eax
    push    ecx
    mov     ecx,[edi] ;P5 cache priming
    mov     ecx,eax
    sub     ecx,ebx   ;StartY-NewStartY
    xor     eax,eax   ;0
    rep     stosd
    pop     ecx
    pop     eax

    ;    memcpy(&NewIndex[StartY-NewStartY],Index,(EndY-StartY)*sizeof(FSpan *));

    sub     edx,ecx     ;NewEndY-EndY
    sub     ecx,eax     ;endY-StartY
    rep     movsd       ;ESI source = This.Index

    ;    mymemset(&NewIndex[NewNum-(NewEndY-EndY)],0,(NewEndY -EndY )*sizeof(FSpan *));

    mov     ecx,edx     ;= NewEndY-EndY
    xor     eax,eax     ;0
    rep     stosd
    ;==================
    ; Ready reallocating index

    mov     edi,DWThizPtr
    mov     esi,DWOtherPtr

    jmp     PrepareMerging
    ;---------------------------------------------------------------

MergeWithMASM ENDP

;-------------------------------------------------------------------------------













;--------------------------------------------------------------------------------
; Poly grinder
;--------------------------------------------------------------------------------
;
;int FSpanBuf::CopyFromRasterUpdate (FSpanBuf &Screen, FRasterPoly &Raster)
;
;
;extern "C" int _cdecl CopyFromRasterUpdateMASM (FSpanBuf *AsmThis,
;                                                FSpanBuf &AsmScreen,
;                                             FRasterPoly &AsmRaster);
;
    StackFrameCnt = 0
    StackDisp = 0

    .DATA
    align 16
    PrimeCache1             label DWORD
    ScreenMemTop            DD ?
    PrevScreenLink          DD ?
    OurStart                DD ?
    OurEnd                  DD ?
    StartMemTop             DD ?
    RelativeOutputIndex     DD ?
    RelativeRasterPolyIndex DD ?
    LastValueESI            DD ?
    OurThisPtr              DD ?
    OurScreenPtr            DD ?
    CFRU_ESPstore           DD ?

    .CODE


CFRU_EXPAND MACRO

;; CopyFromRasterUpdateMASM PROC     Thiz:DWORD, \
;;                                 Screen:DWORD, \
;;                                 Raster:DWORD

    ;=============================================
    ; * Compare both mem pools, (Output vs Screen)
    ;  if not same jump to the alternative
    ;=============================================

 IF TwoPools EQ 0

    ;; MASM generates: push ebp ; mov ebp,esp
    push    ebx
    mov     eax,PrimeCache1
    push    esi
    push    edi

    mov     CFRU_ESPstore,esp

    mov     edi,Thiz
    mov     esi,Screen
    mov     edx,Raster

    mov     OurThisPtr,edi
    mov     OurScreenPtr,esi

    mov     eax,[edi].FSpanBuf.Mem ; Thiz->Mem
    mov     ebx,[esi].FSpanBuf.Mem ; Screen->Mem

    cmp     eax,ebx
    jne     CopyFromRasterUpdate_TwoPools ; separate pools: different routine

    mov     ebp,[eax].FMemPool.MemTop
    mov     esp,[esi].FSpanBuf.ValidLines
    add     ebp, 3
    and     ebp,(not 3)     ; align on 32-bit DWORD boundary.
    mov     StartMemTop,ebp ; to determine 'Accept' at the end.

  ELSE

    mov     ebp,[eax].FMemPool.MemTop
    mov     ebx,[ebx].FMemPool.MemTop ; Screen-mem alignment is assumed.
    mov     esp,[esi].FSpanBuf.ValidLines ; main screen.ValidLines
    add     ebp, 3
    and     ebp,(not 3)      ; align on 32-bit DWORD boundary.
    mov     ScreenMemTop,ebx ; used when splitting screen spans
    mov     StartMemTop, ebp ; to determine 'Accept' at the end.

  ENDIF

    ;;
    ;; EBP will be the maintained memtop for output *and* screen spanbuffers
    ;;

    ;=============================================
    ;* calculate  max(Raster.StartY,Screen.StartY)
    ;* calculate  min(Raster.End,Screen.EndY)
    ;
    ; between these two are the lines we'll process
    ;=============================================

    mov     eax,[edx].FRasterPoly.StartY   ;Raster.StartY
    mov     ecx,[esi].FSpanBuf.StartY      ;Screen.StartY
    cmp     ecx,eax
   ja       Start_ECX
    mov     ecx,eax    ; fallthrough for  Raster.StartY > Screen.StartY
   Start_ECX:
    mov     OurStart,ecx ; = max

    mov     eax,[edx].FRasterPoly.EndY   ;Raster.EndY
    mov     ebx,[esi].FSpanBuf.EndY      ;Screen.EndY
    cmp     ebx,eax
   jb       End_EBX
    mov     ebx,eax    ; fallthrough for  Raster.EndY  < Screen.EndY
   End_EBX:
    mov     OurEnd,  ebx ; = min

    sub     ebx,ecx                ; OurEnd - OurStart
                                   ; = total nr of lines to process

    jbe     BlankOutPut            ; 0 or neg lines to process, all-nullls!

    ;=================================================
    ;* Clean up starting spans in dest. index that aren't in RasterPoly;
    ;   from  This.StartY  to   max( Starts of screen  & raster )
    ;=================================================

    mov     eax,[edi].FSpanBuf.StartY   ;;    This.StartY
    mov     edi,[edi].FSpanBuf.Index    ;;  & This.index[0]

    sub     ecx,eax   ;;  max(Raster.StartY,screen.startY) - This.StartY
    jbe     SkipZeroesStart
    ;
    ; Zero the index from StartY to max (Starts of screen/rasterpoly)
    ;
    xor     eax,eax   ;; direction flag=0 assumed (win32 convention)
    rep     stosd     ;; 10 cycle setup + 1 x N iterations, but loop would
                      ;; be slower for both large and small N.
   SkipZeroesStart:
    ;
    ; EDI now has right This.index[ (OurStart-StartY) ]  value.
    ;



    ;=========================================================================
    ; * Main Loop
    ;
    ;=========================================================================
    ;
    ; Registers assignments:
    ;
    ; EBP  - MEMORY top (32-bit aligned)
    ; EDI  - index for THIS-DESTINATION spans
    ; ESI  - index for SCREEN spans
    ; EDX  - index for FRASTERPOLY spans  --> use Line == [EDX] instead ?!
    ;        (then update FRasterPoly index walker in a mem var..)
    ; ECX  - needed for walking the SCREEN spans..
    ; EBX/EAX  -  general use.
    ;
    ; End of loop: when ESI comes across a special value..
    ;
    ; ---------------------------------------------------------------------
    ; (Temp)ValidLines: essential for our SCREEN buffer, needs to be maintained !
    ; ---------------------------------------------------------------------
    ;
    ;Line        = &Raster.Lines [OurStart-Raster.StartY]; //[0]= 'StartY', usually 0...
    ;ScreenIndex = &Screen.Index [OurStart-Screen.StartY];
    ;
    ; for (i=OurStart,i<OurEnd; i++)
    ;    {
    ; *  PrevScreenLink  = ScreenIndex;
    ;    ScreenSpan      = *(ScreenIndex++);
    ;    PrevLink        = TempIndex++;
    ;    ...
    ;    ...
    ;    Line ++;
    ;    }
    ;========================================
    ; edi already loaded and pointing to destination index

    lea     eax,[ebx*4]     ;;   ebx = number of lines to do - scaled by 4

    mov     ebx,OurStart    ;;   =OurMax(Raster.StartY,Screen.StartY)

    mov     ecx,[esi].FSpanBuf.StartY    ;; Screen.StartY
    mov     esi,[esi].FSpanBuf.Index     ;; & Screen.index[0] ;index=pointers..

    sub     ebx,ecx

    mov     ecx,[edx].FRasterPoly.StartY ;; ecx = Raster.StartY
    ;;
    ;; lea     edx,[edx].FRasterPoly.Lines  ;; Line = &Raster.Lines[0] (=+8!)
    ;; note FrasterPoly's lines are DIRECT members, not pointers

    ;; Now use a relative-addressing trick:
    ;;  RasterPoly index:   [edx + esi*2]
    ;;  Screen Index:       [edi + esi]  -> may use [edi] for subseq. links
    ;;  Output Index:       [esi] -> must not change ! use edx for sub.links?

    lea     esi,[esi+ebx*4] ;; &Screen.Index [OurStart-Screen.StartY];

    mov     ebx,OurStart
    add     eax,esi        ; Index range to do + esi = last loop ESI value
    sub     ebx,ecx        ; OurStart - Raster.StartY
    mov     LastValueESI,eax

    lea     edx,[edx+ebx*8].FRasterPoly.Lines ;;Line= &Raster.Lines [OurStart-Raster.StartY];

    sub     edi,esi ;; make dest index-pointer relative to esi
    sub     edx,esi ;; make poly index-pointer relative to esi*2
    sub     edx,esi ;;

    mov     RelativeOutputIndex,edi
    mov     RelativeRasterPolyIndex,edx

    lea     ecx,[esi-offset(FSpan.Next)]  ;;[ecx].FSpan.Next = *PrevScreenLink!

   IF P5cache EQ 1
    mov     eax,[edi+esi]       ;; P5
    mov     eax,[ebp]           ;; P5 prime cache   span outputs
    mov     eax,[edi+esi+32]    ;; P5 prime cache   index-writing
   ENDIF

    jmp     EnterCFRUFirst


    ;================
    ; Updated at start of every new line: (skip for first:)
    ; (ScreenSpan = ) *(ScreenIndex++);     ;; = our ESI /screen index
    ; (PrevLink   = ) TempIndex++;          ;; = our EDI /destination index
    ;                 Line++                ;; = our EDX /FSpanpoly index
    ;================


   WholeChunk:
    ; screen span overlaps poly line.
    ;           //
    ;           // Add complete chunk to output span buffer
    ;           //
    ;           UPDATE_PREVLINK_ALLOC(Line->Start.X, Line->End.X );
    ;    * SPLIT the screen span:
    ;    *      //
    ;    *      // Get memory for the new span.  Note that this may be drawing from
    ;    *      // the same memory pool as the destination.
    ;    *      //
    ;    *      NewScreenSpan        = (FSpan *)Screen.Mem->GetFast4(sizeof(FSpan));
    ;    *      NewScreenSpan->Start = Line->End.X;
    ;    *      NewScreenSpan->End   = ScreenSpan->End;
    ;    *      NewScreenSpan->Next  = ScreenSpan->Next;
    ;    *      //
    ;    *      ScreenSpan->Next     = NewScreenSpan;
    ;    *      ScreenSpan->End      = Line->Start.X;
    ;    *      //
    ;    *      Screen.ValidLines++;
    ;    *      //

   IF TwoPools EQ 0


    mov     [ebp].FSpan.StartX,ebx    ; Line->Start.X
    mov     [ebp].FSpan.EndX,  edx    ; Line->End.X
    mov     [edi],ebp                 ;*prevlink = newspan
    lea     edi,[ebp].FSpan.Next
    add     ebp,SizeofFSpan

    ; common mem pool used :

    mov     [ebp].FSpan.StartX,edx  ;   NewScreenSpan->Start = Line->End.X;

    mov     edx,[eax].FSpan.Next    ;

    mov     [eax].FSpan.Next,EBP    ;  ScreenSpan->Next     = NewScreenSpan;
    mov     [eax].FSpan.EndX,ebx    ;  ScreenSpan->End      = Line->Start.X;

    mov     [ebp].FSpan.EndX,ecx    ;  NewScreenSpan->End   = ScreenSpan->End;
    inc     esp                     ;  ScreenValidLines++
    mov     [ebp].FSpan.Next,edx    ;  NewScreenSpan->Next  = ScreenSpan->Next;
   IF P5cache EQ 1
    mov     ebx,[ebp+20]            ;  cache priming for next span  P5
   ENDIF
    add     ebp,SizeofFSpan         ;  Get()


    ;;jmp     MainCFRUReloop

   ELSE

    mov     [ebp].FSpan.StartX,ebx    ; Line->Start.X
    mov     [ebp].FSpan.EndX,  edx    ; Line->End.X
    mov     [edi],ebp                 ;*prevlink = newspan

    ; separate mem pools used - load screen mem top

    mov     [eax].FSpan.EndX,ebx    ;  ScreenSpan->End      = Line->Start.X;
    mov     ebx, ScreenMemTop

    lea     edi,[ebp].FSpan.Next    ; .. moved here to avoid ebx-AGI
    add     ebp,SizeofFSpan         ; ..

    mov     [ebx].FSpan.StartX,edx  ;   NewScreenSpan->Start = Line->End.X;

    mov     edx,[eax].FSpan.Next    ;
    mov     [eax].FSpan.Next,ebx    ;  ScreenSpan->Next     = NewScreenSpan;

    mov     [ebx].FSpan.EndX,ecx    ;  NewScreenSpan->End   = ScreenSpan->End;
    mov     [ebx].FSpan.Next,edx    ;  NewScreenSpan->Next  = ScreenSpan->Next;

    add     ebx,SizeofFSpan         ;  Get()
    inc     esp                     ;ScreenValidLines++
    mov     ScreenMemTop, ebx

   IF P5cache EQ 1
    mov     ecx,[ebx+8]             ; prime cache for screen output  P5
    mov     eax,[ebp+8]             ; prime cache for raster span output  P5
   ENDIF

    ;;jmp     MainCFRUReloop

   ENDIF

    ;=====================================
    ;* Actual line Y - loop restart
    ;
    ;=====================================

   MainCFRUReloop: ;; on entry: esi edi ebp used
    mov     eax,LastValueESI
    add     esi,4                ; move this further on ?!?!
    mov     dword ptr [edi],0    ; *PrevLink = NULL
    cmp     esi,eax
    lea     ecx,[esi-offset(FSpan.Next)]  ;;[ecx].FSpan.Next = *PrevScreenLink!
   jae      SkipMainCFRULoop

    ;=====================================
    ; * Skip if this screen span is already full, or if the raster is empty:
    ;   if ((!ScreenSpan) || (Line->End.X <= Line->Start.X)) goto NextLine;
    ;=====================================

   EnterCFRUFirst:

    mov     edi,RelativeOutputIndex
    mov     eax,[esi]       ;; esi = ptr into current screen span index element
    mov     edx,RelativeRasterPolyIndex ; EDX ready again
    add     edi,esi   ;; restored Prevlink for Output
    test    eax,eax
   jz       MainCFRUReloop ;; no screen spans this line, so reloop

    mov     ebx,[edx+esi*2].TRasterLine.StartX  ;; Poly line StartX
    mov     edx,[edx+esi*2].TRasterLine.EndX    ;; Poly line EndX
    cmp     edx,ebx ;; Line->End.X > Line->Start.X, else reloop !
   jna      MainCFRUReloop

    ;=====================================
    ; # Skip past all spans that occur before the raster:
    ;   while (ScreenSpan->End <= Line->Start.X)
    ;        {
    ;        PrevScreenLink  = &(ScreenSpan->Next);
    ;        ScreenSpan = ScreenSpan->Next;
    ;        if (!ScreenSpan) goto NextLine; // This line is full
    ;        };
    ;=====================================
    ;; esi, ebx, edx, ebp, ecx, edi used..

    mov     PrevScreenLink,ecx      ;
    mov     ecx,[eax].FSpan.EndX    ;
    cmp     ecx,ebx                 ; cmp screenspan->end , Line->Start
    ja      FoundScreenSpan         ;

   TraverseScreenSpans:             ;
    mov     PrevScreenLink,eax      ; new 'Prevlink..' = &(ScreenSpan) !!
    mov     eax,[eax].FSpan.Next    ;
    test    eax,eax                 ;
    jz      MainCFRUReloop          ; Next=nulll: goto NextLine
    nop                             ;
    mov     ecx,[eax].FSpan.EndX    ;
    cmp     ecx,ebx                 ; cmp  screenspan->end , Line->Start
    jbe     TraverseScreenSpans     ;

   FoundScreenSpan:    ; found one with Span->End.X > Line->Start.X

    ;=====================================
    ;   //
    ;   // ASSERT: ScreenSpan->End.X > Line->Start.X
    ;   //
    ;   // See if this span straddles the raster's starting point:
    ;   //
    ;   if (ScreenSpan->Start < Line->Start.X)
    ;       {
    ;       if (ScreenSpan->End > Line->End.X)
    ;           {
    ;           //
    ;           // Add complete chunk to output span buffer
    ;           //
    ;           UPDATE_PREVLINK_ALLOC(Line->Start.X, Line->End.X );
    ;           goto NextLine; // Done (everything is clean)
    ;       else
    ;           {
    ;           //
    ;           // Add partial chunk to output span buffer
    ;           //
    ;           UPDATE_PREVLINK_ALLOC(Line->Start.X, ScreenSpan->End);
    ;           ScreenSpan      = ScreenSpan->Next;
    ;           if (ScreenSpan == NULL) goto NextLine; // Done (everything is clean)
    ;           };
    ;       };
    ;=====================================

   StartStraddleHead:

    ;   eax= current screen span ; ebx/edx = polyLine start/end;
    ;
    ;   if (ScreenSpan->Start < Line->Start.X)
    ;       {

    mov     ecx,[eax].FSpan.StartX    ;ScreenSpan->Start
    cmp     ecx,ebx
    jae     EndStraddleHead

    ;  if (ScreenSpan->End > Line->End.X)
    mov     ecx,[eax].FSpan.EndX
    cmp     ecx,edx
    ja      WholeChunk        ;;jbe     PartialChunk

   PartialChunk:
    ;       //
    ;       // Add partial chunk to span buffer:
    ;       //
    ;       UPDATE_PREVLINK_ALLOC(Line->Start.X, ScreenSpan->End);
    ;
    mov     [ebp].FSpan.StartX,ebx  ; = Line->Start.X
    mov     [ebp].FSpan.EndX,  ecx
    mov     [edi],ebp                 ;*prevlink = newspan
    lea     edi,[ebp].FSpan.Next
   IF P5cache EQ 1
    mov     ecx,[ebp+20]              ; P5 cache priming
   ENDIF
    lea     ebp,[ebp+SizeofFSpan]     ;adding without affecting the flags...

    ;    *      //
    ;    *      // adjust a chunk, "Remove partial chunk" from the span buffer:
    ;    *      //
    ;    *      ScreenSpan->End = Line->Start.X;
    ;    *      //
    ;    *      PrevScreenLink  = &(ScreenSpan->Next);

    mov     [eax].FSpan.EndX,ebx
   je      MainCFRUReloop  ;&&&&& {e} EXTRA: if EQ condition, then both ends are equal, so ready...

    mov     PrevScreenLink,eax
    ;
    ;       ScreenSpan      = ScreenSpan->Next;
    ;       if (!ScreenSpan) goto NextLine; // Done (everything is clean)
    ;
    mov     eax,[eax].FSpan.Next
    test    eax,eax
   jz MainCFRUReloop ;; no more screen spans this line, so reloop


   EndStraddleHead:

    ;=====================================
    ;   //
    ;   // ASSERT: Span->Start >= Line->Start.X
    ;           // if (ScreenSpan->Start < Line->Start.X) appError ("Span2");
    ;   //
    ;   // Process all screen spans that are entirely within the raster:
    ;   //
    ;   while (ScreenSpan->End <= Line->End.X)
    ;       {
    ;       //
    ;       // Add entire chunk to temporary span buffer:
    ;       //
    ;       Accept = 1;
    ;       UPDATE_PREVLINK_ALLOC(ScreenSpan->Start,ScreenSpan->End);
    ;   *   //
    ;   *   // Delete this span from the span buffer:
    ;   *   //
    ;   *   *PrevScreenLink = ScreenSpan->Next;
    ;   *   Screen.ValidLines--;
    ;       ScreenSpan      = ScreenSpan->Next;
    ;       if (!ScreenSpan) goto NextLine; // Done (everything is clean)
    ;       };
    ;
    ;=====================================
   StartWithin:
    ;; edx = Line->EndX,       eax = ScreenSpan    esi = screen index
    ;; edi = prevlink(output)  ebp = memory   ; ebx/ecx free

    ;   while (ScreenSpan->End <= Line->End.X)
    mov ecx,[eax].FSpan.EndX
    mov ebx,[eax].FSpan.StartX
    cmp ecx,edx
    ja  EndStartWithin


   WhileWithin: ;;
    ;; Looping here is for polys seen through grids,bars, behind pillars etc.
    ;; screen span is entirely within the rasterpoly - output it
    ;; UPDATE_PREVLINK_ALLOC(ScreenSpan->Start,ScreenSpan->End);

    mov     [ebp].FSpan.StartX,ebx
    mov     [ebp].FSpan.EndX  ,ecx
    mov     [edi],ebp              ; *prevlink = newspan
    lea     edi,[ebp].FSpan.Next
    mov     ecx,PrevScreenLink     ;

   IF P5cache EQ 1
    mov     ebx,[ebp+20]           ; P5 cache priming
   ENDIF


    ;*   //
    ;*   // Delete this span from the span buffer:
    ;*   //
    ;*   *PrevScreenLink.Next  = ScreenSpan->Next;
    ;*   Screen.ValidLines--;

    mov     ebx,[eax].FSpan.Next   ;; PrevScreenSpan-Next = ScreenSpan->Next
    add     ebp,SizeofFSpan
    mov     [ecx].FSpan.Next,ebx   ;;

    ;       ScreenSpan      = ScreenSpan->Next;
    ;       if (!ScreenSpan) goto NextLine; // Done (everything is clean)

    mov     eax,[eax].FSpan.Next
    dec     esp  ;ScreenValidLines--
    test    eax,eax
   jz MainCFRUReloop ;; no screen spans this line, so reloop
    ;
    ;   while (ScreenSpan->End <= Line->End.X)
    mov ecx,[eax].FSpan.EndX
    mov ebx,[eax].FSpan.StartX
    cmp ecx,edx
   jbe  WhileWithin


   ;; EBX = FSpan.StartX = Span->Start

    EndStartWithin:
    ;=====================================
    ;
    ;   //
    ;   // ASSERT: Span->End > Line->End.X
    ;   // if (ScreenSpan->End <= Line->End.X) appError ("Span3");
    ;   //
    ;   // If span overlaps raster's end point, process the partial chunk:
    ;   //
    ;   if (ScreenSpan->Start < Line->End.X)
    ;       {
    ;       //
    ;       // Add chunk from Span->Start to Line->End.X to temp span buffer
    ;       //
    ;       Accept = 1;
    ;       UPDATE_PREVLINK_ALLOC(ScreenSpan->Start,Line->End.X);
    ;   *   //
    ;   *   // Shorten this span line by removing the raster:
    ;   *   //
    ;   *   ScreenSpan->Start = Line->End.X;
    ;       };
    ;=====================================
    StartStraddleTail:

    ;; edx = Line->EndX,       eax = ScreenSpan
    ;; edi = prevlink(output)  ebp = memory      ;; esi=Screen-prevlink?
    ;;
    ;; ebx = ScreenSpan->Start
    ;; ecx = ScreenSpan->End

    ;;   if (ScreenSpan->Start < Line->End.X)
    cmp     ebx,edx
    jae     EndStartStraddleTail


    ;; STRADDLETAIL:
    ;; UPDATE_PREVLINK_ALLOC(ScreenSpan->Start,Line->End.X);

    mov     [ebp].FSpan.StartX,ebx
    mov     [ebp].FSpan.EndX  ,edx
    mov     [edi],ebp     ;*prevlink = newspan
    lea     edi,[ebp].FSpan.Next
   IF P5cache EQ 1
    mov     ecx,[ebp+20]             ;P5 cache priming
   ENDIF
    add     ebp,SizeofFSpan

    ;   *   //
    ;   *   // Shorten this span line by removing the raster:
    ;   *   //
    ;   *   ScreenSpan->Start = Line->End.X;

    mov     [eax].FSpan.StartX,edx

    EndStartStraddleTail:
    ;=================

    ;-----------------
   GoForNextLine:
    mov     eax,LastValueESI
    add     esi,4                ; move this further on ?!?!
    mov     dword ptr [edi],0    ; *PrevLink = NULL
    cmp     esi,eax
    lea     ecx,[esi-offset(FSpan.Next)]  ;;[ecx].FSpan.Next = *PrevScreenLink!
   jb       EnterCFRUFirst
    ;-----------------
    ;=========================================================================
    ; END of main loop
    ;=========================================================================


   SkipMainCFRULoop:




    ;=============================================
    ;* Clean up ending spans in dest. index that aren't in RasterPoly;
    ;   from min(Raster.End,Screen.EndY) to This.EndY
    ;=============================================
    ;
    ;
    ; EDI + ESI needed to get actual destination index.
    ;

    mov     edi,RelativeOutputIndex    ;;
    add     edi,esi                    ;;
    ;
    mov     esi,OurThisPtr ;DWThizPtr              ;;
    mov     eax,OurEnd          ;;
    mov     ecx,[esi].FSpanBuf.EndY    ;; This.EndY
    sub     ecx,eax                    ;;- min(Raster.End,Screen.EndY)
   jbe      SkipZeroesEnd
    ;;
    ;; Zero the remainder of the destination index
    ;;
    xor     eax,eax   ;; direction flag=0 assumed (win32 convention)
    rep     stosd     ;; 10 cycle setup + 1 per iter, but still fastest
   SkipZeroesEnd:


   CFRU_End: ;; END for CFRU with one single mem pool


  IF TwoPools EQ 0
    ;===================================================================
    ;  1-mempool situation
    ;===================================================================
    ;---------Epilog code: update the mempool tops for
    ;         and ValidLines for both the output and screen span buffer
    ;
    ;mov     edi,DWThizPtr          ;;&&&&&& already loaded..
    ;mov     esi,DWScreenPtr

    mov     edi,OurThisPtr
    mov     esi,OurScreenPtr

    mov     eax,[edi].FSpanBuf.Mem  ;;   This.Mem Destination spanbuffer

    mov     edx,StartMemTop         ;; initial ebp-MemTop

    mov     [eax].FMemPool.MemTop,ebp ; update output memtop

    sub     ebp,edx                 ;; ebp == (new memtop - original memtop)
    ;;mov     eax,ScreenValidLines  ;;

    mov     [esi].FSpanBuf.ValidLines,esp ; ScreenValidLines (exact)
    mov     [edi].FSpanBuf.ValidLines,ebp ; Output 'ValidLines' (boolean)

    ;;add     esp,StackFrameCnt ;; restore stack frame
    mov     esp,CFRU_ESPstore

    ; eax-output is 'Accept' -> nonzero if more than 0 spans in output.
    mov     eax,ebp

    ;-----------
    pop     edi
    pop     esi
    pop     ebx
    pop     ebp
    retn
    ;; 'ret'   would make MASM insert "LEAVE" =  mov  esp,ebp / pop ebp
    ;============================

  ELSE

    ;===================================================================
    ; 2-mempool situation
    ;===================================================================
    ;---------Epilog code: update the mempool tops for
    ;         and ValidLines for both the output and screen span buffer
    ;
    ;mov     edi,DWThizPtr            ;; already loaded?
    ;mov     esi,DWScreenPtr

    mov     edi,OurThisPtr
    mov     esi,OurScreenPtr

    mov     edx,StartMemTop          ;; initial ebp-MemTop

    mov     eax,[edi].FSpanBuf.Mem   ;;   This.Mem Destination spanbuffer
    mov     ecx,[esi].FSpanBuf.Mem   ;; Screen.Mem Screen spanbuffer

    mov     ebx,ScreenMemTop

    mov     [eax].FMemPool.MemTop,ebp ; update output memtop
    mov     [ecx].FMemPool.MemTop,ebx ; update screen memtop

    sub     ebp,edx                  ;; ebp == (new memtop - original memtop)

    ;;;mov     eax,ScreenValidLines     ;;

    mov     [esi].FSpanBuf.ValidLines,esp ; ScreenValidLines (exact)
    mov     [edi].FSpanBuf.ValidLines,ebp ; Output 'ValidLines' (boolean)

    ;;add     esp,StackFrameCnt ;; restore stack frame

    mov     esp,CFRU_ESPstore

    ; eax-output is 'Accept' -> nonzero if more than 0 spans in output.
    mov     eax,ebp

    ;-----------
    pop     edi
    pop     esi
    pop     ebx
    pop     ebp
    retn
    ;; 'ret'   would make MASM insert "LEAVE" =  mov  esp,ebp / pop ebp
    ;============================

  ENDIF

   ;----------------------
   BlankOutput: ;; fill destin. index, amount: (This.EndY- This.StartY)
    mov     ecx,[edi].FSpanBuf.EndY     ;;    This.EndY
    sub     ecx,[edi].FSpanBuf.StartY   ;;    This.StartY
    mov     edi,[edi].FSpanBuf.Index    ;;  & This.index[0]
    jbe     CFRU_End
    ;
    ; Zero the output index
    ;
    xor     eax,eax
    rep     stosd
    jmp     CFRU_End
   ;----------------------

ENDM


;
; Expand to 2 versions, one where this.MemPool and Screen.MemPool are
; the same, and one where they are different.
;



align 16
CopyFromRasterUpdateMASM PROC     Thiz:DWORD, \
                                Screen:DWORD, \
                                Raster:DWORD
 TwoPools = 0
 CFRU_expand

CopyFromRasterUpdateMASM ENDP


align 16
CopyFromRasterUpdate_TwoPools PROC

 TwoPools = 1
 CFRU_expand

CopyFromRasterUpdate_TwoPools ENDP

;-----------------------------------------------------------------------------













;========================================================================
align 16
DB 08 dup (090h)    ;#natalign 2_

CalcRectFromMASM PROC      Thiz:DWORD, \
                         Source:DWORD, \
                          GridX:DWORD, \
                          GridY:DWORD, \
                            Mem:DWORD


 ArbitMaxX     = 9999 ;  Arbitrary maximum X start
 StackFrameCnt = 0    ;

 StackDisp = 0        ; set to +4 or higher when inside calling routines

 ;;
 ;; Stack frame vars below are equated to  " dword ptr [ESP + ..]  "
 ;;

    AllocDW NewStartX
    AllocDW NewEndX
    AllocDW LastYLoopIncrement
    AllocDW IndexPointer
    AllocDW TempIndex
    AllocDW PrevStartX   ; chunk not yet allocated
    AllocDW PrevEndX     ;
    AllocDW NoNullFlag   ; temp usage only...
    AllocDW StartCounter
    AllocDW StartFulcrum
    AllocDW DestIndex
    AllocDW PrevLink
    AllocDW IndexCopied
    AllocDW TopSpan
    AllocDW RectYCounter
    AllocDW NonConnect
    AllocDW FulcrumMask  ; = (1 << GridX) - 1
    AllocDW GridYSize    ; = (1 << GridY)

    ;; temp vars from our source spanbuffer

    AllocDW SourceStartY
    AllocDW SourceEndY
    AllocDW SourceSizeY
    AllocDW SourceIndex0

   ;; varname TEXTEQU <dword ptr [ESP + &disp + StackDisp]>

   ;; direct access to caller-pushed vars:
   ;; starting at 4 * 5 = 1 return address + 4 pushes.

    DWThisPtr   TEXTEQU <dword ptr [ESP+StackFrameCnt + 4*5 +StackDisp]>
    SourcePtr   TEXTEQU <dword ptr [ESP+StackFrameCnt + 4*6 +StackDisp]>
    DWGridXBits TEXTEQU <dword ptr [ESP+StackFrameCnt + 4*7 +StackDisp]>
    DWGridYBits TEXTEQU <dword ptr [ESP+StackFrameCnt + 4*8 +StackDisp]>
    DWGmem      TEXTEQU <dword ptr [ESP+StackFrameCnt + 4*9 +StackDisp]>

;================
   .DATA

   ;; Temp vars used when ESP needed as general register

   align 16
   DS_EndIndex        label DWORD
   DS_Line4Start4EndX label DWORD
                      DD 0

   DS_End4StartX      DD 0
   DS_ESP             DD 0

;================

   .CODE

;;
;; Stack frame preparation, C++ parameter loading
;; Note: Here Masm inserts "ENTER" sequence == push ebp / mov  ebp, esp
;;
    push ebx
    push esi
    push edi

    sub     esp, StackFrameCnt  ;; reserve temp vars

    mov     edi, Source     ; FSpanBuf ptr -> Source span buffer
    mov     esi, Mem        ; pointer to FMemPool struct

    mov     eax, [edi].FSpanBuf.StartY
    mov     ebx, [edi].FSpanBuf.EndY
    mov     SourceStartY,eax
    mov     SourceEndY  ,ebx

    sub     ebx, eax
    mov     SourceSizeY ,ebx

    mov     ecx, [edi].FSpanBuf.Index
    mov     esi, [esi].FMemPool.MemTop  ; mem top

    mov     SourceIndex0,ecx
    add     esi, 3                      ; mem top align
    mov     edi, Thiz                   ; used below..
    and     esi, (not 3) ;;== ~3        ; new 32bit aligned memtop = ESI


;; FulcrumMask = ( (1<<GridXBits) -1);
;; GridYSize   =    1<<GridYBits

;; jump to general case if  (X != 4) || (Y != 3)

    mov     ecx, GridX
    mov     eax, GridY
    cmp     ecx, 4
    jne     CalcRectFromGeneral
    cmp     eax, 3
    jne     CalcRectFromGeneral

;; EBP available as a general register from here.

;--------------------------------------------------------------------------------
;; now do the fixed  GridXbits=4 GridYbits=3
;;
;; FulcrumMask = ( (1<<GridXBits) -1); = 15
;;
;; fixed all GridYsize at 8 , FulcrumMask at 15, GridXsize at 16
;;
;; TempIndex = (FSpan **)Mem->Get(sizeof(FSpan *)<<GridYBits);
;; = 'SourceIndex' in C++

    mov     TempIndex, esi   ;; esi==Mem Top
    add     esi, 8*4         ;; GridYSize = 8   *  Sizeof(FSpan *) = 4
                             ;; to allocate the temp. index.

;; AllocIndex (Source.StartY>>GridYBits,1+((Source.EndY-1)>>GridYBits), Mem);

    mov     ebx, SourceEndY
    mov     eax, SourceStartY

    dec     ebx
    sar     eax, 3 ;Grid Ybits
    sar     ebx, 3
    inc     ebx

;; "AllocIndex" : init stack vars, only update real 'This' data at the end.

    ;;  eax==start  ebx==end

    ;; EDI = Thiz  pointer
    mov     [edi].FSpanbuf.StartY ,eax  ;; OurStartY,eax    ;==AllocStartY
    mov     [edi].FSpanbuf.EndY   ,ebx  ;; OurEndY,  ebx    ;==AllocEndY

    ;; AllocEndY - AllocStarY

    sub     ebx, eax       ;; if (dest) StartY>EndY then exit with NULLS..
    jl      RectNullOutput ;; but if equal just allocate 0 bytes ->Like C++ !

    ;; esi == Mem Top

    ;; &Index[0] = Index = Get  ((AllocEndY-AllocStartY) * sizeof(FSpan *));
    ;; 'TopSpan' = &List[0];    ==  TopSpan

    mov     [edi].FSpanbuf.Index,esi ; 'OurIndex0',esi  Index = get..()
    lea     edx,[ebx*4]              ;  * sizeof (FSpan *)
    mov     DestIndex, esi           ; 1st index entry to link Fspans to
    add     esi, edx                 ; + size of our dest. Index
    mov     [edi].FSpanbuf.List ,esi ; 'OurList' ,esi ; List = GetFast4(0)
    mov     TopSpan  ,esi            ; TopSpan = &List[0]


;; ! From here on TopSpan takes on task of Memory Top (as in C++ version)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    dec     ebx
    js      CalcRectFinish
    mov     RectYCounter,ebx ;; must be minimal 0, means 1 loop...

;;  GY          =   (StartY >>,<< GridYBits) - Source.StartY;
;;              =   0  - ( Source.StartY & ( 8-1) )

;;
;; EDX(0)  = SpanYL(Start) = min ( (GY0 + 8) , SourceSizeY )
;;

    mov     eax, SourceStartY
    mov     edx, 8            ;; GridYsize
    mov     esi, SourceSizeY
    and     eax, 7            ;;Grid Y-mask

    mov     ecx, SourceEndY

    sub     edx, eax    ;; 8 + (GY0) = 8 - (YStart & GridYMask)
    mov     edi, SourceIndex0       ;; always our starting value
    cmp     edx, esi    ;;
    jb      Minedx
    mov     edx, esi    ;;
    Minedx:

;; LastYLoopIncrement: = SpanYL size of last one..

    and     ecx, 7     ;; (EndY-1) && GridYMask... =.. if 0 then ==8!
    jnz     No8
    mov     ecx, 8     ;; fill with 8 - lastloopIncrement
    No8:

    sub     edi,4             ;; point at one BEFORE first....

    mov     LastYLoopIncrement,ecx ;; only really needed if RectYCounter >0

    mov     IndexPointer,edi      ;; too many, but better than extra jump


;======================================================================
; expect: EDX = 'SpanYL'   IndexPointer = point to last index we have processed

RectYLoop:

;; edi  initially pointing the one BEFORE the very 1st...
;; during processing, EDI may ALSO be pointed into TempIndex, but it gets
;; set right again HERE....

    mov     ecx, 1
    mov     edi, IndexPointer ;;
    mov     eax, DestIndex    ;; see below...
    sub     ecx, edx          ;; ecx = - ( SpanYL - 1 )
    lea     edi, [edi+edx*4]  ;; add amount to upgrade to next 'last'location..
    mov     StartCounter,ecx  ;; save
    mov     IndexPointer,edi  ;; save

; Clear flags;
; PrevLink = DestIndex++;     ;; point to next (dest.buf's) line index entry

    xor     ebx, ebx          ;;
    mov     PrevLink,   eax   ;;
    mov     IndexCopied,ebx   ;; Indicates we copied a SourceIndex to TempIndex
    add     eax,4             ;; Point to next pointer of destination index
    mov     PrevEndX,   ebx   ;; Indicates no previous span found
    mov     DestIndex,  eax
    nop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LookForMinMax:

;; 'HackoffChunk' : find FSpans with left and right extremes, and if
;;  these FSpans overlap, they span an entire chunk, so we don't have
;;  to check any further - unless there are non-null NEXT links.
;;
;; Assume ECX and EDI prepared.
;;  Load LAST line, may be ONLY line:
;;            normal loop: ecx= 0 is last inc/jng (exit)
;;     lastdonefirst loop: ecx=-1 is last inc/ jz (exit)
;;

    mov     DS_ESP,ESP

    xor     ebp, ebp        ; == 0 needed as first 'next' in core..
    mov     eax,[edi]       ; 'last' or only source line pointer
    test    eax, eax        ; not null please
    jz      LastLineEmpty

    mov     esi, [eax].FSpan.StartX
    mov     ebx, [eax].FSpan.EndX
    mov     esp, [eax].Fspan.Next       ; null-tag indicator

   FromLLE:   ;note Single line, ecx=0, will be resolved correctly in loop

    mov     DS_End4StartX,ebx        ;
    mov     DS_Line4Start4EndX,ebp   ; was line [edi+0*4].... and ebp==0...

    mov     eax, [edi+ ecx*4]   ; load first line from index, in advance...
    dec     ecx                 ; if ecx= -1, now -2 , does 2 incs = 1 loop..

    ;;jmp   EnterLoop
    ;;--|------
FromNullSkip:
    cmp     eax,1         ; CF=1 if eax=0
    inc     ecx           ; JA: CF and ZF both 0
    jna     NoLoopChunk   ; if < 0 loop; 0 = [0+edi] = last, already done..

;##################  6 * 6 cycles/link for typical,ideal core loop cases

LoopChunk:          ;#natalign 2_
    mov     ebp, [eax].FSpan.StartX
    mov     edx, [eax].FSpan.EndX

    cmp     esi,ebp                ; new min start
   jna      NoNewStartX            ; jump = keep old
    mov     esi,ebp                ; new min start
    mov     DS_End4StartX,edx      ; store associated maximum
   NoNewStartX:

    mov     ebp, [eax].FSpan.Next  ;
    mov     eax, [edi+ ecx*4 + 4]  ; load next line from index in advance
                                   ; erm- avoid '+4' by using different edi?!!
                                   ;      faster that way ??
    cmp     ebx,edx                ; new max end
   jnb      NoNewEndX
    mov     ebx,edx
    mov     DS_Line4Start4EndX,ecx ; later: only 3 instr./1.5 cyc needed to get it back
   NoNewEndX:

   EnterLoop:
    or      esp,ebp       ; inconsequential if from NullPtr..
    cmp     eax,1         ; CF=1 if eax=0
    inc     ecx           ; JA: CF and ZF both 0
    ja      LoopChunk     ; if < 0 loop; 0 = [0+edi] = last, already done..
NoLoopChunk:              ; just ONE fall-thru for null-stretches !

;##################

    mov     ebp, DS_Line4Start4EndX
    cmp     ecx,-1            ; if still <-1  then jump back with 'skip',
                              ; -1  = ready because eax==null AND last loop!!!
                              ;  0+ = ready, the INC made us exit.
    mov     eax, [edi+ ecx*4 + 4] ; load next line from index in advance
   jl       FromNullSkip      ; only happens if an original link was NULL..
                              ; usually falls through
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; now:
;; ESI= new min. Start
;; ebx = new max. End
;; Line4EndX  = [ebp] =  line indicator for rightmost span
;; End4StartX         =  endx for leftmost span
;; EDX = 0 means ALL-null-nexts....
;; keep EDI intact for span offset...
;;
;==========================================================

    mov     ecx,esp
    mov     ESP,DS_ESP       ;

    cmp     ebx,esi          ; end not bigger than start: NO span, exit.
    mov     eax,[edi+ebp*4]  ; the link to get Start4EndX
    mov     NoNullFlag,ecx   ;
    jbe     InvalidMaxes     ; this WILL happen for all-null spans ..!

    mov     edx,[eax].FSpan.StartX  ;;;=='Start4EndX'

    mov     NewEndX  , ebx   ; store
    mov     eax,PrevEndX
    mov     NewStartX, esi   ; store
    test    eax,eax
    jnz     HandlePrevSpan   ;; jz      TestDoubleSpan

TestDoubleSpan:     ;D   do the min/max spans connect ?
    mov     ecx,DS_End4StartX  ;
    mov     eax,edx            ; the saved Start4EndX...
    add     ecx,15             ; fulcrummask
    mov     esi, NewStartX  ; load for DoLastAlloc
    or      ecx,15          ; fulcrummask
    cmp     ecx,eax         ; ?  (lef span's end) >= (right span's start)
    jb      ComplexSpan     ; if not, resolve using classical method

SeeNonNulls:
    mov     edx, NoNullFlag ;
    mov     ebx, NewEndX    ; load for DoLastAlloc
    test    edx,edx
    jnz     MoreToDigest    ;


DoLastAlloc:                ; start,end in ESI,EBX

;;  allocate last (usually only) span for this (dest) line
;;   - puts zero in 'next' link.
;;
;;  UPDATE_PREVLINK( CUTSTART(NewStartX), CUTEND(NewEndX) );
;;
;;  {\
;;  *PrevLink       = TopSpan;\
;;  TopSpan->Start  = START;\
;;  TopSpan->End    = END;\
;;  PrevLink        = &((TopSpan++)->Next);\
;;  STAT(GStat.SpanTotalChurn++;)\
;;  ValidLines++;\
;;  };
;;

u   mov     eax, PrevLink   ;;
v   add     ebx, 15         ;; (NewEndX +FulcrumMask) >> GridXBits  == CUTEND
um  shr     ebx, 4          ;; shift right by GridXBits..
v   mov     edx, TopSpan    ;;

u   mov     [eax],edx              ;; point the index/prevlink to new span
v   add     edx, SizeofFSpan       ;; New mem top, next span
u   xor     eax,eax                ;;
v   mov     TopSpan, edx           ;; Store mem top
u   mov     ecx, dword ptr [edx+12]             ;; cache-priming dummy load
v   mov     [edx-SizeofFSpan].FSpan.EndX  ,ebx  ;;
um  shr     esi, 4                              ;; shift right by GridXBits..
v   mov     [edx-SizeofFSpan].FSpan.Next  ,eax  ;; next = zero, last link.
u   mov     [edx-SizeofFSpan].FSpan.StartX,esi  ;;

;;
;; Skipped processing line in 2 cases:
;;  - SpanYL <= 0              ; no lines to process
;;  - Max EndX <= Min StartX   ; no valid spans encountered on first chunking
;;

SkipThisLine: ;; make sure *Prevlink = 0 already done when here.

;;
;; GY        += 8 already done at start of loop :
;; *PrevLink  = NULL;     ( type:  FSpan **prevlink )      - already done..
;;

    mov     eax, RectYCounter
    dec     eax
    mov     edx, 8       ; default index stride
    mov     RectYCounter,eax
    jg      RectYLoop            ; jump back only for positive EAX/RectYCounter
    ;;
    ;; now if ZERO after DEC EAX, this is the last loop -
    ;; load the special last EDX increment-value
    ;;
    mov     edx, LastYLoopIncrement
    jz      RectYLoop     ; mess up previous' jump BTB, but just once


;;  Loops to new Y line using RectYCounter


CalcRectFinish:  ;; Epilog code

    ;; - C++:  Mem->GetFast4((int)TopSpan - (int)List);
    ;;   report back alloc'd mem to actual FMemPool structure
    ;;
    ;; - fill in ValidLines to dest spanbuffer
    ;;

    mov     edi, DWThisPtr ;;
    mov     eax, TopSpan                   ; (*)
    mov     ecx, DWGmem    ;; original pushed mem pool ptr

    add     esp, StackFrameCnt ;; unallocate stack vars...

    mov     ebx, [edi].FSpanbuf.List       ; (*) for comparing...
    mov     [ecx].FmemPool.MemTop    ,eax  ; update memtop
    sub     eax,ebx                        ; now eax = total space allocated
    mov     [edi].FSpanbuf.Mem       ,ecx

    mov     [edi].FSpanbuf.ValidLines,eax  ;; ValidLines: 'boolean', 0=empty

    ;; eax=0 means no output,  >0 for any output.

    ;; if accurate ValidLines are needed again, we cound use ESP as a counter
    ;; or just divide the allocated space by SizeofFSpan (12) at the end.

    pop     edi
    pop     esi
    pop     ebx
    pop     ebp
    ;; if RET instead of RETN, MASM inserts "LEAVE" = mov  esp,ebp / pop ebp
    retn
;;====================================================================

align 16
LastLineEmpty: ; initialize min/max with constants
    mov     esi,ArbitMaxX   ; smallest start of span
    xor     ebx,ebx         ; largest End of span - use previous max !...
    xor     esp,esp  ; Nextptr = guaranteed null
    jmp     FromLLE  ; jump back

;;==================

;;
;; all multi-chunk logic
;;
align 16
HandlePrevSpan:
    add     eax,15           ;fulcrummask
    xor     ecx,ecx
    or      eax,15           ;fulcrummask
    cmp     eax,esi          ; if (fulcr(PrevEnd) >=NewStart) link, else alloc

    mov     esi,PrevStartX   ;
    mov     ebx,PrevEndX

    mov     PrevEndX,ecx
    jb      AllocatePrev     ; don't connect so allocate previous span

    mov     NewStartX,esi    ; connect so replace new start in any case..
    mov     ecx,DS_End4StartX
    cmp     eax,edx          ; fulcr(PrevEnd) >=  Start4EndX ? if so == 1 span
    jae     FormOneSpan      ;

    ;; 2 stretches overlap: then sure of span (New=PrevStartX,NewEndX)
    ;; else go to 'complex' with New=PrevStartX,max(End4Start,PrevEndX)

    add     ecx,15  ;fulcrummask     ;end4startx..
    or      ecx,15  ;
    cmp     ecx,edx          ;fulcr(end4startx) >= start4endx ?
    jae     SeeNonNulls ;NewStart&End are right....

    ;; not touching - go to 'complex' with !End4Start = max (End4Start,PrevEndX)
    sub     eax,15           ; use 'un'fulcrummed PrevEndX
    mov     ebx,DS_End4StartX
    cmp     eax,ebx
    jbe     ComplexSpan
    nop
    mov     DS_End4StartX,eax
    nop
    jmp     ComplexSpan

align 16
 FormOneSpan: ;which is New=PrevStartX==esi, max (PrevEndX,NewEndX)
    sub     eax,15         ; 'unfulcrummed' PrevEndX
    mov     ebx,NewEndX
    cmp     eax,ebx
    jbe     SeeNonNulls
    nop
    mov     NewEndX,eax    ; replace it
    nop
    jmp     SeeNonNulls

AllocatePrev:
    call    Allocate16x8FSpan ; esi == PrevStartX
    ;;Allocate16x8FSpanMacro
    nop
    nop
    jmp     TestDoubleSpan

;;====================================================================

align 16
MoreToDigest:

 ;;
 ;; Consistent chunk(s) detected but there's more, maybe even attached.
 ;;

 ;; Change from read-only source index to temp index, if not already done

    mov     eax,IndexCopied
    mov     esi,edi           ;;
    mov     ecx,StartCounter; ;; StartCntr*4 + EDI = first line in source

    test    eax,eax
    jnz     AlreadyCopied1

    mov     edi,TempIndex     ;; TempIndex already allocated
    lea     eax,[ecx*4]       ;;
    sub     edi,eax           ;; actually ADDS (n-1)*4, so EDI will point
                              ;; at last one just like ESI
    Mov     IndexCopied,edi   ;'true'  mark that we DID copy the index now.

AlreadyCopied1:

    ;; looping:  [ecx-1 ... -1] ;; must cover  [ecx... 0].[edi]

    mov     eax,[esi+ecx*4]
    sub     ecx,2             ;if ecx was 0 (1 line:) now -2...
    jmp     EnterNexts


LoopNexts:
    nop
    mov     ebx, [eax].FSpan.Next
SkipNullNext:
    mov     [edi+ecx*4+4],ebx
    mov     eax,[esi+ecx*4+8]  ;next index...
EnterNexts:
    nop
    cmp     eax,1
    inc     ecx
   ja      LoopNexts ;jump if both eax and ecx are nonzero
    nop
    mov     ebx,eax    ;
    nop                ; ecx not 0: then eax has got to be 0 !
   js      SkipNullNext ; still signed: then NOT zero, so go to SkipNullNext..

 ;;
 ;; save prev. span found for later appending/allocation
 ;;

    mov     eax, NewStartX
    mov     ebx, NewEndX
    mov     PrevStartX,eax
    mov     PrevEndX,  ebx

    mov     ecx,Startcounter

    jmp     LookForMinMax
;----------------------------

align 16
Allocate16x8Fspan  label NEAR
;; ebx, esi = NewEndX and NewStartX must contain the start/end.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;          UPDATE_PREVLINK( CUTSTART(NewStartX), CUTEND(NewEndX) );
;;  {\
;;  *PrevLink       = TopSpan;\
;;  TopSpan->Start  = START;\
;;  TopSpan->End    = END;\
;;  PrevLink        = &((TopSpan++)->Next);\
;;  STAT(GStat.SpanTotalChurn++;)\
;;  ValidLines++;\
;;  };
;;
;; Prevlink points into the proper DEST LINE INDEX to link into
;;
;; ;#define CUTSTART(x) (int)((x)>>GridXBits)
;; ;#define CUTEND(x)   (int)(1+(((x)-1)>>GridXBits))
;;
;;      or: CUTEND(x)  (int)( ((x)+FulcrumMask) >> GridXBits )
;;
;; want esi and ebx already filled with start and end !
;;        esi = Prev/NewStartX
;;        ebx = Prev/NewEndX

    StackDisp = 4 ;; ESP becomes -=4 so adjust stack equates accordingly

u   mov     edx, TopSpan
v   mov     eax, PrevLink
um  shr     esi,4
v   add     ebx,15        ;; (NewEndX +FulcrumMask) >> GridXBits  == CUTEND
um  shr     ebx,4          ;; shift right by GridXBits..

v   mov     [eax],edx                  ;; point the index/prevlink to new span
u   mov     ecx,dword ptr [edx+24]     ;; cache-priming dummy load
v   mov     [edx].FSpan.StartX,esi
u   mov     [edx].FSpan.EndX  ,ebx

;;  PrevLink = &((TopSpan++)->Next);\
;;  ValidLines++

v   lea     eax, [edx].FSpan.Next ;; next pointer address for future links
u   add     edx, SizeofFSpan      ;; New mem top, next span
v   mov     PrevLink,eax
u   mov     TopSpan,edx           ;; Store mem top

    StackDisp = 0

v   retn
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;==========================================
    align 16
    DB 3 dup (090h) ;#natalign 3_

 ComplexSpan:

 ;; DO it the old way entirely for this line (still fast)
 ;; use the single loop, going back to 'chunker' if we need
 ;; new minimum-StartX detection..

 ;; FIRST copy the index 'blindly' (not Next links) _if_ needed
 ;; (3 cyc/dword)

    mov     eax,IndexCopied   ;;
    mov     esi,edi           ;;
    mov     ecx,StartCounter; ;; StartCntr*4 + EDI = first line in source
    test    eax,eax
    jnz     AlreadyCopied2


    mov     edi,TempIndex       ;; TempIndex already allocated
    lea     eax,[ecx*4]         ;;
    sub     edi,eax             ;; actually ADDS (n-1)*4, so EDI will point
                                ;; at last one just like ESI
    Mov     IndexCopied,edi     ;; 'true' mark that we DID copy the index now.


;; 3 cyc/dword but 1st-time pairing.

   LoopInCopy:
    nop
    mov     ebx,[esi+ecx*4]  ;next index...
    nop
    mov     [edi+ecx*4],ebx
    inc     ecx
    jng     LoopInCopy

AlreadyCopied2:

    ;; set up vars for 'conventional' FSpan-connecting loop
    ;; NEED minimum to start with, and associated maximum.
    ;; START still in NewStartX....

    mov     edx,DS_End4StartX    ; new end associated with min. start
    mov     ebx,edx              ;
    add     edx,15               ; fulcrummask
    or      edx,15               ; fulcrummed end

ReLoopSpan2:

    xor     eax,eax
    mov     ecx,StartCounter

    mov     NonConnect,eax       ; flags
    mov     NoNullFlag,    eax

    mov     eax, [ecx*4+edi]     ; get first link from index
    mov     StartFulcrum,edx     ; save fulcrum for comparing afterwards

;   'Conventional' loop, to start with the latest MinX and
;   maximum set to the line that produced the next MinX (or linked
;   to it..)

LoopSpan2:          ;#natalign 3_
    test    eax, eax
   jz      Skip_Comparing                ;;forced fall-through prediction=OK
u   cmp     edx, [eax].FSpan.StartX
v   xxx
u   xxx
v  jae      Do_Connect
    nop
    mov     NonConnect,edi     ;flag
    nop
    jmp     Skip_Comparing
   Do_Connect:
    mov     esi, [eax].FSpan.EndX
    mov     eax, [eax].FSpan.Next
    cmp     esi, ebx
   jbe     NoNewFulCrum2
    mov     ebx, esi    ; new end..
    add     esi, 15     ;fulcrummask
    mov     edx, esi
   NoNewFulCrum2:
    mov     [ecx*4+edi],eax ; next link update
    test    eax, eax
   jz DidFindNullNext
    mov     NoNullFlag,edi          ; set flag

   DidFindNullNext:
   Skip_Comparing:
    or      edx, 15             ; final piece of fulcrumming
    mov     eax, [ecx*4+4+edi]  ; load in advance
    inc     ecx
    jng     LoopSpan2   ; jng loops for negative AND zero values.
 EndSpan2:


 ;; ready if:
 ;; Nextsfound==all 0 AND ( FulcrumChange=0  or  NonConnects=0)
 ;; end of line if ALSO nonconnect=0...
 ;;
 ;; but if Fulcrum changed, and still ALL nexts were 0 and no
 ;; nonconnects: then also READY...


    mov     eax,NoNullFlag  ;
    mov     ecx,NonConnect  ;
    test    eax,eax         ; eax != 0 if any new links just uncovered
    jne     ReloopSpan2     ;  changed - then reloop with it

    ;; if ALL connected (+all nulls) we're out too, no need for fulc. check
    nop
    mov     eax,StartFulcrum
    test    ecx,ecx         ; no non-connecting spans ?
    je      ReadyWithLine

    cmp     eax,edx         ;  Fulcrum changed, but still some unconnected:
    jne     ReloopSpan2     ;  -> potential new links so reloop


ReadyWithLoopSpan:
;; allocate a span, but there are non-connects found so reloop to
;; lookforminmax....
    ;; EBX==the new NewEndX we just found
    mov     esi, NewStartX
    call    Allocate16x8FSpan
    mov     ecx,Startcounter
    jmp     LookForMinMax


;;
ReadyWithLine:
;; allocate the very last span of this line
    ;; EBX==the new NewEndX we just found
    mov     esi, NewStartX
    jmp     DoLastAlloc


;==================================================

 InvalidMaxes:
   ;;; This span is invalid but maybe there's still
   ;;; previous stuff to digest ...
   ;;; -> Allocate the PrevStart/End if present, but
   ;;;   then end this line...

    mov     edx, PrevLink
    xor     eax, eax

    mov     ebx, PrevEndX
    mov     esi, PrevStartX

    mov     dword ptr [edx],eax ; *PrevLink = NULL; (can be undone by
                                ; Allocate16x8FSpan  below if needed)
    test    ebx,ebx
    jz      SkipThisLine
    Call    Allocate16x8FSpan       ; flush any old spans..
    jmp     SkipThisLine
   ;--------------------


;;
;; There was a 0-line dest buffer when Allocating the destination index
;;

 RectNullOutPut: ;; edi still on 'this' ptr
    xor     ecx, ecx
    mov     [edi].FSpanbuf.Index,ecx ; 'OurIndex0',esi  Index = get..()
    mov     [edi].FSpanbuf.List ,ecx ; 'OurList' ,esi ; List = GetFast4(0)
    jmp     CalcRectFinish

CalcRectFromMASM ENDP
;------------------------------------------------------------------------
;========================================================================








;--------------------------------------------------------------------------------
; Calculate a Rectangle from Spanbuffer
;--------------------------------------------------------------------------------
;
;int FSpanBuf::CalcRectFrom(const FSpanBuf &Source,
;                                           BYTE GridXBits,
;                                           BYTE GridYBits,
;                                       FMemPool *Mem)
;
;
;C++:
;extern "C" int _cdecl CalcRectFromMASM (FSpanBuf *AsmThis,
;                                        FSpanBuf &Source,
;                                               BYTE GridXBits,
;                                               BYTE GridYBits,
;                                           FMemPool *Mem)
;

align 16
DB 02 dup (090h)    ;#natalign 2

CalcRectFromGeneral PROC  ;put NO params here, else MASM will insert push ebp !

 ArbitMaxX     = 9999 ;  Arbitrary maximum X start
 StackFrameCnt = 0    ;
 StackDisp = 0        ; set to +Nx4 or higher when inside calling routines

 ;;
 ;; Stack frame vars below are equated to  " dword ptr [ESP + ..]  "
 ;;

    AllocDW NewStartX
    AllocDW NewEndX
    AllocDW LastYLoopIncrement
    AllocDW IndexPointer
    AllocDW TempIndex
    AllocDW PrevStartX   ; chunk not yet allocated
    AllocDW PrevEndX     ;
    AllocDW NoNullFlag   ; temp usage only...
    AllocDW FulcrumMask  ; = (1 << GridX) - 1
    AllocDW StartCounter
    AllocDW StartFulcrum
    AllocDW GridYSize    ; = (1 << GridY)
    AllocDW DestIndex
    AllocDW PrevLink
    AllocDW IndexCopied
    AllocDW TopSpan
    AllocDW RectYCounter
    AllocDW NonConnect

    ;; temp vars from our source spanbuffer

    AllocDW SourceStartY
    AllocDW SourceEndY
    AllocDW SourceSizeY
    AllocDW SourceIndex0

    ;; varname TEXTEQU <dword ptr [ESP + &disp + StackDisp]>

    ;; direct access to caller-pushed vars:
    ;; starting at 4 * 5 = 1 return address + 4 pushes.

    DWThisPtr   TEXTEQU <dword ptr [ESP+StackFrameCnt + 4*5 +StackDisp]>
    SourcePtr   TEXTEQU <dword ptr [ESP+StackFrameCnt + 4*6 +StackDisp]>
    DWGridXBits TEXTEQU <dword ptr [ESP+StackFrameCnt + 4*7 +StackDisp]>
    DWGridYBits TEXTEQU <dword ptr [ESP+StackFrameCnt + 4*8 +StackDisp]>
    DWGmem      TEXTEQU <dword ptr [ESP+StackFrameCnt + 4*9 +StackDisp]>


;; FulcrumMask = ( (1<<GridXBits) -1);
;; GridYSize   =    1<<GridYBits

    mov     ebp, 1
    mov     ecx, DWGridXBits
    mov     eax, ebp
    shl     ebp, cl
    mov     ecx, DWGridYBits
    dec     ebp
    shl     eax, cl
    mov     FulcrumMask,ebp
    mov     GridYSize,eax

;; TempIndex = (FSpan **)Mem->Get(sizeof(FSpan *)<<GridYBits);
;; = 'SourceIndex' in C++

    lea     eax, [eax*4]     ;; GridYSize *   (Sizeof(FSpan *) = 4)
    mov     TempIndex, esi   ;; esi==Mem Top
    add     esi, eax

;; AllocIndex (Source.StartY>>GridYBits,1+((Source.EndY-1)>>GridYBits), Mem);

    mov     ecx, DWGridYBits
    mov     ebx, SourceEndY
    mov     eax, SourceStartY

    dec     ebx
    sar     eax, cl
    sar     ebx, cl
    inc     ebx

;; "AllocIndex" : init stack vars, only update real 'This' data at the end.

    ;;  eax==start  ebx==end

    ;; EDI = Thiz  pointer
    mov     [edi].FSpanbuf.StartY ,eax  ;; OurStartY,eax    ;==AllocStartY
    mov     [edi].FSpanbuf.EndY   ,ebx  ;; OurEndY,  ebx    ;==AllocEndY

    ;; AllocEndY - AllocStarY

    sub     ebx, eax       ;; if (dest) StartY>EndY then exit with NULLS..
    jl      RectNullOutput ;; but if equal just allocate 0 bytes ->Like C++ !

    ;; esi == Mem Top

    ;; &Index[0] = Index = Get  ((AllocEndY-AllocStartY) * sizeof(FSpan *));
    ;; 'TopSpan' = &List[0];    ==  TopSpan

    mov     [edi].FSpanbuf.Index,esi ; 'OurIndex0',esi  Index = get..()
    lea     edx,[ebx*4]              ;  * sizeof (FSpan *)
    mov     DestIndex, esi           ; 1st index entry to link Fspans to
    add     esi, edx                 ; + size of our dest. Index
    mov     [edi].FSpanbuf.List ,esi ; 'OurList' ,esi ; List = GetFast4(0)
    mov     TopSpan  ,esi            ; TopSpan = &List[0]


;; ! From here on TopSpan takes on task of Memory Top (as in C++ version)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    dec     ebx
    js      CalcRectFinish
    mov     RectYCounter,ebx ;; must be minimal 0, means 1 loop...

;;  GY          =   (StartY >>,<< GridYBits) - Source.StartY;
;;              =   0  - ( Source.StartY & ( GridYSize-1) )

;;
;; EDX(0)  = SpanYL(Start) = min ( (GY0 + GridYSize) , SourceSizeY )
;;

    mov     ebx, GridYSize
    mov     eax, SourceStartY
    mov     edx, ebx          ;; save
    dec     ebx               ;; gets 'Y-mask'
    mov     esi, SourceSizeY
    and     eax, ebx

    mov     ecx, SourceEndY

    sub     edx, eax    ;; GridYSize + (GY0) = GridYSize - (YStart & GridYMask)
    mov     edi, SourceIndex0       ;; always our starting value
    cmp     edx, esi    ;;
    jb      Minedx
    mov     edx, esi    ;;
    Minedx:

;; LastYLoopIncrement: = SpanYL size of last one..

    and     ecx, ebx           ;; (EndY-1) && GridYMask... =.. if 0 then ==GridYSize!
    jnz     NoGridYSize
    mov     ecx, GridYSize     ;; fill with GridYSize
    NoGridYSize:

    sub     edi,4             ;; point at one BEFORE first....

    mov     LastYLoopIncrement,ecx ;; only really needed if RectYCounter >0

    mov  IndexPointer,edi      ;; too many, but better than extra jump


;======================================================================
; expect: EDX = 'SpanYL'   IndexPointer = point to last index we have processed

RectYLoop:

;; edi  initially pointing the one BEFORE the very 1st...
;; during processing, EDI may ALSO be pointed into TempIndex, but it gets
;; set right again HERE....

    mov     ecx, 1
    mov     edi, IndexPointer ;;
    mov     eax, DestIndex    ;; see below...
    sub     ecx, edx          ;; ecx = - ( SpanYL - 1 )
    lea     edi, [edi+edx*4]  ;; add amount to upgrade to next 'last'location..
    mov     StartCounter,ecx  ;; save
    mov     IndexPointer,edi  ;; save

; Clear flags;
; PrevLink = DestIndex++;     ;; point to next (dest.buf's) line index entry

    xor     ebx, ebx          ;;
    mov     PrevLink,   eax   ;;
    mov     IndexCopied,ebx   ;; Indicates we copied a SourceIndex to TempIndex
    add     eax,4            ;; Point to next pointer of destination index
    mov     PrevEndX,   ebx   ;; Indicates no previous span found
    mov     DestIndex,  eax
    nop

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LookForMinMax:


;; 'HackoffChunk' : find FSpans with left and right extremes, and if
;; these FSpans overlap, they span an entire chunk, so we don't have
;; to check any further - unless there are non-null NEXT links.
;;
;; Assume ECX and EDI prepared.
;;  Load LAST line, may be ONLY line:
;;            normal loop: ecx= 0 is last inc/jng (exit)
;;     lastdonefirst loop: ecx=-1 is last inc/ jz (exit)
;;


    mov     DS_ESP,ESP

    xor     ebp, ebp        ; == 0 needed as first 'next' in core..
    mov     eax,[edi]       ; 'last' or only source line pointer
    test    eax, eax        ; not null please
    jz      LastLineEmpty

    mov     esi, [eax].FSpan.StartX
    mov     ebx, [eax].FSpan.EndX
    mov     esp, [eax].Fspan.Next       ; null-tag indicator

   FromLLE:   ;note Single line, ecx=0, will be resolved correctly in loop

    mov     DS_End4StartX,ebx        ;
    mov     DS_Line4Start4EndX,ebp   ; was line [edi+0*4].... and ebp==0...

    mov     eax, [edi+ ecx*4]   ; load first line from index, in advance...
    dec     ecx                 ; if ecx= -1, now -2 , does 2 incs = 1 loop..

    ;;jmp   EnterLoop
    ;;--|------
FromNullSkip:
    cmp     eax,1         ; CF=1 if eax=0
    inc     ecx           ; JA: CF and ZF both 0
    jna     NoLoopChunk   ; if < 0 loop; 0 = [0+edi] = last, already done..

;##################  6 * 6 cycles/link for typical,ideal core loop cases

LoopChunk:          ;#natalign 2
    mov     ebp, [eax].FSpan.StartX
    mov     edx, [eax].FSpan.EndX

    cmp     esi,ebp                ; new min start
   jna      NoNewStartX            ; jump = keep old
    mov     esi,ebp                ; new min start
    mov     DS_End4StartX,edx      ; store associated maximum
   NoNewStartX:

    mov     ebp, [eax].FSpan.Next  ;
    mov     eax, [edi+ ecx*4 + 4]  ; load next line from index in advance
    cmp     ebx,edx                ; new max end
   jnb      NoNewEndX
    mov     ebx,edx
    mov     DS_Line4Start4EndX,ecx ; later: only 3 instr./1.5 cyc needed to get it back
   NoNewEndX:

   EnterLoop:
    or      esp,ebp       ; inconsequential if from NullPtr..
    cmp     eax,1         ; CF=1 if eax=0
    inc     ecx           ; JA: CF and ZF both 0
    ja      LoopChunk     ; if < 0 loop; 0 = [0+edi] = last, already done..
NoLoopChunk:              ; just ONE fall-thru for null-stretches !

;##################

    mov     ebp, DS_Line4Start4EndX
    cmp     ecx,-1            ; if still <-1  then jump back with 'skip',
                              ; -1  = ready because eax==null AND last loop!!!
                              ;  0+ = ready, the INC made us exit.
    mov     eax, [edi+ ecx*4 + 4] ; load next line from index in advance
   jl       FromNullSkip      ; only happens if an original link was NULL..
                              ; usually falls through
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; now:
;; ESI= new min. Start
;; ebx = new max. End
;; Line4EndX  = [ebp] =  line indicator for rightmost span
;; End4StartX         =  endx for leftmost span
;; EDX = 0 means ALL-null-nexts....
;; keep EDI intact for span offset...
;;
;==========================================================

    mov     ecx,esp
    mov     ESP,DS_ESP       ; this could well be postponed to end of function!

    cmp     ebx,esi          ; end not bigger than start: NO span, exit.
    mov     eax,[edi+ebp*4]  ; the link to get Start4EndX
    mov     NoNullFlag,ecx   ;
    jbe     InvalidMaxes     ; this WILL happen for all-null spans ..!

    mov     edx,[eax].FSpan.StartX  ;;;=='Start4EndX'

    mov     NewEndX  , ebx   ; store
    mov     NewStartX, esi   ; store
    mov     eax,PrevEndX
    mov     ebp,FulcrumMask  ; from here (til LookForMinMax) EBP==fulcrummask
    test    eax,eax
    jnz     HandlePrevSpan   ;; jz      TestDoubleSpan

TestDoubleSpan:     ;D   do the min/max spans connect ?
    mov     ecx,DS_End4StartX  ;
    mov     eax,edx            ; the saved Start4EndX...
    add     ecx,ebp
    mov     esi, NewStartX  ; load for DoLastAlloc
    or      ecx,ebp
    cmp     ecx,eax         ; ?  (lef span's end) >= (right span's start)
    jb      ComplexSpan     ; if not, resolve using classical method

SeeNonNulls:
    mov     edx, NoNullFlag
    mov     ebx, NewEndX    ; load for DoLastAlloc
    test    edx,edx
    jnz     MoreToDigest    ;

DoLastAlloc:                ; start,end in ESI,EBX

;;  allocate last (usually only) span for this (dest) line
;;  last span this line - so puts zero in next link.
;;
;;  UPDATE_PREVLINK( CUTSTART(NewStartX), CUTEND(NewEndX) );
;;
;;  {\
;;  *PrevLink       = TopSpan;\
;;  TopSpan->Start  = START;\
;;  TopSpan->End    = END;\
;;  PrevLink        = &((TopSpan++)->Next);\
;;  STAT(GStat.SpanTotalChurn++;)\
;;  ValidLines++;\
;;  };
;; on entry: ebp = FulcrumMask
;; ebx, esi = NewEndX and NewStartX must contain the start/end.
;;

u   add     ebx, ebp        ;; (NewEndX +FulcrumMask) >> GridXBits  == CUTEND
v   mov     ecx, DWGridXBits
u   mov     edx, TopSpan    ;;
v   mov     eax, PrevLink
    shr     esi, cl         ;; 4 np cycles     shift right by GridXBits..
    shr     ebx, cl         ;; 4 np cycles     shift right by GridXBits..

u   mov     [eax],edx                  ;; point the index/prevlink to new span
v   add     edx, SizeofFSpan       ;; New mem top, next span
u   xor     eax,eax                    ;;
v   mov     TopSpan, edx           ;; Store mem top
u   mov     ecx, dword ptr [edx+12]    ;; cache-priming dummy load
v   mov     [edx-SizeofFSpan].FSpan.StartX,esi  ;;
u   mov     [edx-SizeofFSpan].FSpan.EndX  ,ebx  ;;
v   mov     [edx-SizeofFSpan].FSpan.Next  ,eax  ;; next = zero, last link.


;;
;; Skipped processing line in 2 cases:
;;  - SpanYL <= 0              ; no lines to process
;;  - Max EndX <= Min StartX   ; no valid spans encountered on first chunking
;;

SkipThisLine: ;; make sure *Prevlink = 0 already done when here.

;;
;; GY        += GridYSize already done at start of loop :
;; *PrevLink  = NULL;     ( type:  FSpan **prevlink )      - already done..
;;

    mov     eax, RectYCounter
    dec     eax
    mov     edx, GridYSize       ; default index stride
    mov     RectYCounter,eax
    jg      RectYLoop            ; jump back only for positive EAX/RectYCounter
    ;;
    ;; now if ZERO after DEC EAX, this is the last loop -
    ;; load the special last EDX increment-value
    ;;
    mov     edx, LastYLoopIncrement
    jz      RectYLoop     ; mess up previous' jump BTB, but just once

;;  Loops to new Y line using RectYCounter

CalcRectFinish:

;; C++:  Mem->GetFast4((int)TopSpan - (int)List);
;; HERE we've been using and maintaining our TopSpan as the mem top.

    ;; fill in ValidLines to dest spanbuffer
    ;; report back alloc'd mem to actual FMemPool structure

    mov     edi, DWThisPtr ;;-ThisPtr
    mov     eax, TopSpan                   ; (*)
    mov     ecx, DWGmem    ;;-GMemPoolPtr  ; original pushed mem pool ptr

    add     esp, StackFrameCnt ;; unallocate stack vars...

    mov     ebx, [edi].FSpanbuf.List       ; (*) for comparing...
    mov     [ecx].FmemPool.MemTop    ,eax  ; update memtop
    sub     eax,ebx                        ; now eax = total space allocated
    mov     [edi].FSpanbuf.Mem       ,ecx

    mov     [edi].FSpanbuf.ValidLines,eax  ;; ValidLines: 'boolean', 0=empty

    ;; eax=0 means no output,  >0 for any output.
    ;; Epilog code

    ;; if accurate ValidLines are needed, just divide the
    ;; space allocated by 12 !

    pop     edi
    pop     esi
    pop     ebx
    pop     ebp
    ;;! if RET instead of RETN, MASM inserts "LEAVE" = mov  esp,ebp / pop ebp
    retn
;;====================================================================


LastLineEmpty: ; initialize min/max with constants
    mov     esi,ArbitMaxX   ; smallest start of span
    xor     ebx,ebx         ; largest End of span - use previous max !...
    xor     esp,esp  ; Nextptr = guaranteed null
    jmp     FromLLE  ; jump back

;;==================

;;
;; all multi-chunk logic
;;

HandlePrevSpan:
    xor     ecx,ecx
    add     eax,ebp
    or      eax,ebp
    cmp     eax,esi          ; if (fulcr(PrevEnd) >=NewStart) link, else alloc

    mov     esi,PrevStartX   ;
    mov     ebx,PrevEndX

    mov     PrevEndX,ecx
    jb      AllocatePrev     ; don't connect so allocate previous span

    mov     NewStartX,esi    ; connect so replace new start in any case..
    mov     ecx,DS_End4StartX
    cmp     eax,edx          ; fulcr(PrevEnd) >=  Start4EndX ? if so == 1 span
    jae     FormOneSpan      ;

    ;; 2 stretches overlap: then sure of span (New=PrevStartX,NewEndX)
    ;; else go to 'complex' with New=PrevStartX,max(End4Start,PrevEndX)

    add     ecx,ebp ;end4startx..
    or      ecx,ebp
    cmp     ecx,edx          ;fulcr(end4startx) >= start4endx ?
    jae     SeeNonNulls ;NewStart&End are right....

    ;; not touching - go to 'complex' with !End4Start = max (End4Start,PrevEndX)
    sub     eax,ebp           ; use 'un'fulcrummed PrevEndX
    mov     ebx,DS_End4StartX
    cmp     eax,ebx
    jbe     ComplexSpan
    nop
    mov     DS_End4StartX,eax
    nop
    jmp     ComplexSpan


 FormOneSpan: ;which is New=PrevStartX==esi, max (PrevEndX,NewEndX)
    sub     eax,ebp        ; 'unfulcrummed' PrevEndX
    mov     ebx,NewEndX
    cmp     eax,ebx
    jbe     SeeNonNulls
    nop
    mov     NewEndX,eax    ; replace it
    nop
    jmp     SeeNonNulls

AllocatePrev:
    call    AllocateFSpan ; esi == PrevStartX
    ;;AllocateFSpanMacro
    nop
    nop
    jmp     TestDoubleSpan

;;====================================================================

MoreToDigest:

 ;;
 ;; Consistent chunk(s) detected but there's more, maybe even attached.
 ;;

 ;; Change from read-only source index to temp index, if not already done

    mov     eax,IndexCopied
    mov     esi,edi           ;;
    mov     ecx,StartCounter; ;; StartCntr*4 + EDI = first line in source

    test    eax,eax
    jnz     AlreadyCopied1

    mov     edi,TempIndex     ;; TempIndex already allocated
    lea     eax,[ecx*4]       ;;
    sub     edi,eax           ;; actually ADDS (n-1)*4, so EDI will point
                              ;; at last one just like ESI
    Mov     IndexCopied,edi   ;'true'  mark that we DID copy the index now.

AlreadyCopied1:

    ;; looping:  [ecx-1 ... -1] ;; must cover  [ecx... 0].[edi]

    mov     eax,[esi+ecx*4]
    sub     ecx,2             ;if ecx was 0 (1 line:) now -2...
    jmp     EnterNexts

LoopNexts:
    nop
    mov     ebx, [eax].FSpan.Next
SkipNullNext:
    mov     [edi+ecx*4+4],ebx
    mov     eax,[esi+ecx*4+8]  ;next index...
EnterNexts:
    nop
    cmp     eax,1
    inc     ecx
   ja      LoopNexts ;jump if both eax and ecx are nonzero
    nop
    mov     ebx,eax    ;
    nop                ; ecx not 0: then eax has got to be 0 !
   js      SkipNullNext ; still signed: then NOT zero, so go to SkipNullNext..

 ;;
 ;; save prev. span found for later appending/allocation
 ;;

    mov     eax, NewStartX
    mov     ebx, NewEndX
    mov     PrevStartX,eax
    mov     PrevEndX,  ebx

    mov     ecx,Startcounter

    jmp     LookForMinMax
;----------------------------




align 16
AllocateFspan  label NEAR
;; on entry: ebp = FulcrumMask
;; ebx, esi = NewEndX and NewStartX must contain the start/end.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;          UPDATE_PREVLINK( CUTSTART(NewStartX), CUTEND(NewEndX) );
;;  {\
;;  *PrevLink       = TopSpan;\
;;  TopSpan->Start  = START;\
;;  TopSpan->End    = END;\
;;  PrevLink        = &((TopSpan++)->Next);\
;;  STAT(GStat.SpanTotalChurn++;)\
;;  ValidLines++;\
;;  };
;;
;; Prevlink points into the proper DEST LINE INDEX to link into
;;
;; ;#define CUTSTART(x) (int)((x)>>GridXBits)
;; ;#define CUTEND(x)   (int)(1+(((x)-1)>>GridXBits))
;;
;;      or: CUTEND(x)  (int)( ((x)+FulcrumMask) >> GridXBits )
;;
;; want esi and ebx already filled with start and end !
;;        esi = Prev/NewStartX
;;        ebx = Prev/NewEndX

    StackDisp = 4 ;; ESP becomes -=4 so adjust stack equates accordingly

u   add     ebx, ebp        ;; (NewEndX +FulcrumMask) >> GridXBits  == CUTEND
v   mov     ecx, DWGridXBits
u   mov     eax, PrevLink
v   mov     edx, TopSpan
    shr     ebx,cl          ;; 4 np cycles   shift right by GridXBits..
    shr     esi,cl          ;; 4 np cycles   shift right by GridXBits..

u   mov     [eax],edx                  ;; point the index/prevlink to new span
v   mov     ecx,dword ptr [edx+24]     ;; cache-priming dummy load
u   mov     [edx].FSpan.StartX,esi
v   mov     [edx].FSpan.EndX  ,ebx

;;  PrevLink = &((TopSpan++)->Next);\
;;  ValidLines++

u   lea     eax, [edx].FSpan.Next ;; next pointer address for future links
v   add     edx, SizeofFSpan      ;; New mem top, next span
v   mov     PrevLink,eax
v   mov     TopSpan,edx           ;; Store mem top

    StackDisp = 0

v   retn


;==========================================
    align 16
    DB 5 dup (090h) ;#natalign 3

 ComplexSpan:

 ;; DO it the old way entirely for this line (still fast)
 ;; use the single loop, going back to 'chunker' if we need
 ;; new minimum-StartX detection..

 ;; FIRST copy the index 'blindly' (not Next links) _if_ needed
 ;; (3 cyc/dword)

    mov     eax,IndexCopied   ;;
    mov     esi,edi           ;;
    mov     ecx,StartCounter; ;; StartCntr*4 + EDI = first line in source
    test    eax,eax
    jnz     AlreadyCopied2


    mov     edi,TempIndex       ;; TempIndex already allocated
    lea     eax,[ecx*4]         ;;
    sub     edi,eax             ;; actually ADDS (n-1)*4, so EDI will point
                                ;; at last one just like ESI
    Mov     IndexCopied,edi     ;; 'true' mark that we DID copy the index now.


;; 3 cyc/dword but 1st-time pairing.

   LoopInCopy:
    nop
    mov     ebx,[esi+ecx*4]  ;next index...
    nop
    mov     [edi+ecx*4],ebx
    inc     ecx
    jng     LoopInCopy

AlreadyCopied2:

    ;; set up vars for 'conventional' FSpan-connecting loop
    ;; NEED minimum to start with, and associated maximum.
    ;; START still in NewStartX....

    mov     edx,DS_End4StartX    ; new end associated with min. start
    mov     ebx,edx              ;
    add     edx,ebp              ;
    or      edx,ebp              ; fulcrummed end

ReLoopSpan2:

    xor     eax,eax
    mov     ecx,StartCounter

    mov     NonConnect,eax       ; flags
    mov     NoNullFlag,    eax

    mov     eax, [ecx*4+edi]     ; get first link from index
    mov     StartFulcrum,edx     ; save fulcrum for comparing afterwards

;   'Conventional' loop, to start with the latest MinX and
;   maximum set to the line that produced the next MinX (or linked
;   to it..)

LoopSpan2:          ;#natalign 3
    test    eax, eax
   jz      Skip_Comparing                ;;forced fall-through prediction=OK
u   cmp     edx, [eax].FSpan.StartX
u   xxx
u   xxx
v  jae      Do_Connect
    nop
    mov     NonConnect,edi     ;flag
    nop
    jmp     Skip_Comparing
   Do_Connect:
    mov     esi, [eax].FSpan.EndX
    mov     eax, [eax].FSpan.Next
    cmp     esi, ebx
   jbe     NoNewFulCrum2
    mov     ebx, esi    ; new end..
    add     esi, ebp
    mov     edx, esi
   NoNewFulCrum2:
    mov     [ecx*4+edi],eax ; next link update
    test    eax, eax
   jz DidFindNullNext
    mov     NoNullFlag,edi          ; set flag

   DidFindNullNext:
   Skip_Comparing:
    or      edx, ebp            ; final piece of fulcrumming
    mov     eax, [ecx*4+4+edi]  ; load in advance
    inc     ecx
    jng     LoopSpan2   ; jng loops for negative AND zero values.
 EndSpan2:


 ;; ready if:
 ;; Nextsfound==all 0 AND ( FulcrumChange=0  or  NonConnects=0)
 ;; end of line if ALSO nonconnect=0...
 ;;
 ;; but if Fulcrum changed, and still ALL nexts were 0 and no
 ;; nonconnects: then also READY...


    mov     eax,NoNullFlag      ;
    mov     ecx,NonConnect  ;
    test    eax,eax         ; eax != 0 if any new links just uncovered
    jne     ReloopSpan2     ;  changed - then reloop with it

    ;; if ALL connected (+all nulls) we're out too, no need for fulc. check
    nop
    mov     eax,StartFulcrum
    test    ecx,ecx         ; no non-connecting spans ?
    je      ReadyWithLine

    cmp     eax,edx         ;  Fulcrum changed, but still some unconnected:
    jne     ReloopSpan2     ;  -> potential new links so reloop


ReadyWithLoopSpan:
;; allocate a span, but there are non-connects found so reloop to
;; lookforminmax....
    ;; EBX==the new NewEndX we just found
    mov     esi, NewStartX
    call    AllocateFSpan
    mov     ecx,Startcounter
    jmp     LookForMinMax


;;
ReadyWithLine:
;; allocate the very last span of this line
    ;; EBX==the new NewEndX we just found
    mov     esi, NewStartX
    jmp     DoLastAlloc




;==================================================

 InvalidMaxes:
   ;;; This span is invalid but maybe there's still
   ;;; previous stuff to digest ...
   ;;; -> Allocate the PrevStart/End if present, but
   ;;;   then end this line...

    mov     edx, PrevLink
    xor     eax, eax

    mov     ebx, PrevEndX
    mov     esi, PrevStartX

    mov     dword ptr [edx],eax ; *PrevLink = NULL; (can be undone by
                                ; AllocateFSpan  below if needed)
    test    ebx,ebx
    jz      SkipThisLine
    mov     ebp,FulcrumMask
    Call    AllocateFSpan       ; flush any old spans..
    jmp     SkipThisLine
   ;--------------------

;;
;; There was a 0-line dest buffer when Allocating the destination index
;;

 RectNullOutPut: ;; edi still on 'this' ptr
    xor     ecx, ecx
    mov     [edi].FSpanbuf.Index,ecx ; 'OurIndex0',esi  Index = get..()
    mov     [edi].FSpanbuf.List ,ecx ; 'OurList' ,esi ; List = GetFast4(0)
    jmp     CalcRectFinish

CalcRectFromGeneral ENDP
;------------------------------------------------------------------------
;========================================================================



















align 16
;--------------------------------------------------------------------------------
; Calculate a Lattice from CalcRectFrom output..
;--------------------------------------------------------------------------------
;
;int FSpanBuf::CalcLatticeFrom(const FSpanBuf &Source,
;                                       FMemPool *Mem)
;
;
;C++:
;extern "C" int _cdecl CalcLatticeFromMASM (FSpanBuf *AsmThis,
;                                           FSpanBuf &Source,
;                                           FMemPool *Mem)
;

DB 7 dup (090h)     ;#natalign 4


CalcLatticeFromMASM PROC             Thiz:DWORD, \
                                   Source:DWORD, \
                                      Mem:DWORD  \

 StackDisp = 0        ; set to +4 or higher when inside called routines..

 ;; These equates are for [ESP] access to the pushed parameters.
 ;; Parameters start at 5*4  = 4 pushed regs + 1 return address

 DWThisPtr TEXTEQU <dword ptr [ESP + 5*4 + StackDisp]>
 SourcePtr TEXTEQU <dword ptr [ESP + 6*4 + StackDisp]>
    DWGmem TEXTEQU <dword ptr [ESP + 7*4 + StackDisp]>

;;
;; C++ parameter loading
;;
;;  !implied: push ebp/ mov ebp,esp
;;
    push    ebx
    push    esi
    mov     edx, Source     ; FSpanBuf ptr -> Source span buffer
    push    edi

    mov     edi, Thiz                   ;; This-pointer = destination spanbuf

    mov     eax, [edx].FSpanBuf.StartY
    mov     ebx, [edx].FSpanBuf.EndY
    mov     ecx, [edx].FSpanBuf.Index   ;; ecx= SourceIndex0

    mov     edx, Mem                    ;; pointer to FMemPool struct

    mov     esi, ebx                    ;; eax/ebx = SourceStartY/EndY
    sub     esi, eax                    ;; SourceSizeY

    mov     ebp, [edx].FMemPool.MemTop  ;; ebp becomes 'MemTop'
    add     ebp, 3                      ;;
    inc     ebx                         ;; ebx = EndY+1
    and     ebp, (not 3)                ;; align on 32-bit bound

;;
;; AllocIndex (Source.StartY,Source.EndY+1,Mem);
;;
;; n            = Source.EndY - Source.StartY;
;; TopSpan      = &List[0];
;;
;; ebx == SourceEndY+1
;; eax == SourceStartY
;; esi == SourceEndY - SourceStartY

;;
;; "AllocIndex" : init stack vars, only update real Dest.'This' data at end.
;;
    ;; EDI = Thiz  pointer
    mov     [edi].FSpanbuf.StartY ,eax  ;; OurStartY,eax    ;==AllocStartY
    mov     [edi].FSpanbuf.EndY   ,ebx  ;; OurEndY,  ebx    ;==AllocEndY

  ; esi = 'SourceSizeY'
  ; esi < 1:  invalid input; exit with NULL pointers instead of allocation
  ; esi = 1:  single line input -> 2 line output, no looping
  ; esi > 1:  default case , >2 line output.

    cmp     esi,1
    jb      LatticeNullOutPut

;; &Index[0] = Index = Get  ((AllocEndY-AllocStartY) * sizeof(FSpan *));
;; 'TopSpan' = &List[0];    ==  TopSpan
;;  ebp == MemTop / TopSpan

    mov     [edi].FSpanbuf.Index,ebp ; allocated dest. Index
    mov     edx,ebp                  ; '&Index0' / PrevLink (Dest Index walker)

    lea     ebp,[ebp+esi*4+4]        ; + size of our dest. Index
    mov     [edi].FSpanbuf.List ,ebp ; allocated List = get(0)

    je      TwinLineOutPut  ; esi was 1, -> 2 line output

;;
;; Calculate loop vars / initialize registers
;;
;; n = Source.EndY - Source.StartY;   for ( i=1; i<n; i++ )
;; n-1 ones will be processed
;;
;; EDX and ECX kept on the indexes, EBP = MemTop
;;   Span1       = Source.Index[0];
;;   PrevLink    = &Index[0];

    ; esi == SourceSizeY
    ; ecx == SourceIndex0

    lea     eax, [ecx+esi*4-4]   ;; end of the (n-1) we'll process
    mov     DS_EndIndex, eax



;;
;; do first single line
;;
    mov     dword ptr [edx],0  ;; first line dest assumed null at first
    add     edx,4
    mov     edi,[ecx]      ;; edi == nonzero
    test    edi,edi        ;;
    jz      ZeroFirst

    mov     [edx-4],ebp
    call    SingleStretch

;;
;; Multi-chunk universal 'CalcLatticeFrom' core
;;
;; must be *sure* edi is nonzero when entering here...

   MultiplexReLoop: ;#natalign 4

    mov     edi,[ecx]           ;; new first one
    mov     esi,[ecx+4]         ;; starts at source line 1
    test    esi,esi             ;; esi is new, but edi always nonzero
    jz      ZeroSecond          ;; jz: only EDI=[ecx] is nonzero

   MultiplexLoop:
    nop
    mov     [edx],ebp ;; index ptr to the span we're doing now...

    mov     eax,[edi].FSpan.StartX ;; line A start
    mov     ebx,[esi].FSpan.StartX ;; line B start
    cmp     eax,ebx
    ja      MpLeftBRightLook

   MpLeftARightLook:      ;; line A has left extreme
    mov     [ebp].FSpan.StartX,eax
   IntoMpLeftA:
    mov     eax,[edi].FSpan.EndX ;; line A end
    mov     ebx,[esi].FSpan.EndX ;; line B end
    inc     eax
    inc     ebx
    mov     [ebp].FSpan.EndX  ,eax  ;;   [A  ,A]
    cmp     eax,ebx
    ja      MpReadySpan

   MpLeftARightB:
    mov     [ebp].FSpan.EndX  ,ebx  ;;   [A  ,B]
    mov     ebx, [esi].FSpan.StartX ;;line B start
    cmp     eax,ebx        ;; if  LineBStart > LineAEnd (load > eax), NoConnect-Exit !
    jb      MpNonConnectAB ;;
    ;-|--
   MpReadySpan:
    ;------------------------------
    mov     edi,[edi].FSpan.Next
    add     ebp,SizeofFSpan
    test    edi,edi
    jnz     NonZeroEDImulti

    mov     esi,[esi].FSpan.Next ;; edi zero, try [esi].next..
    xor     eax,eax
    test    esi,esi
    jnz     ZeroEDImulti   ; jump: esi nonzero, edi zero

   PrepNextMultiLine:
    add     ecx,4                ;; ecx = Source index
    mov     [ebp-SizeofFSpan].FSpan.Next,eax ;; set it to zero, end of line
    mov     eax, DS_EndIndex     ;;
    add     edx,4                ;; edx = Destin index
    cmp     ecx,eax              ;; end of source reached ?
    jb      MultiplexReloop      ;; next line....  for ( i=1; i<n; i++ )

    mov     edi,[ecx]
    jmp     LastLatticeLine      ;; start last (single) line
   ;------------------------------
   align 16
   ;-------------------------------
   MpLeftBRightLook: ;; line B has left extreme
    mov     [ebp].FSpan.StartX,ebx
   IntoMpLeftB:
    mov     ebx,[esi].FSpan.EndX    ;; line B end
    mov     eax,[edi].FSpan.EndX    ;; line A end
    inc     ebx
    inc     eax
    mov     [ebp].FSpan.EndX  ,ebx  ;;  [B  ,B]
    cmp     ebx,eax
    ja      MpReadySpan

   MpLeftBRightA:
    mov     [ebp].FSpan.EndX  ,eax  ;;  [B  ,A]
    mov     eax, [edi].FSpan.StartX ;;  line A start
    cmp     ebx,eax               ;; if  LineAStart > LineBEnd (load > ebx), NoConnect-Exit !
    jnb     MpReadySpan
    jmp     MpNonConnectBA
   ;--------------------------------
   ;================================


   align 16
   ZeroEDImulti: ; ESI nonzero, EDI zero
    mov     [ebp-SizeofFSpan].FSpan.Next,ebp ;;
    mov     edi,esi                          ;;
    jmp     IntoSingleSpanThing              ;;

   NonZeroEDImulti: ; EDI nonzero, ESI.next ??
    mov     esi,[esi].FSpan.Next
    mov     [ebp-SizeofFSpan].FSpan.Next,ebp ;; preemptive next-link
    test    esi,esi                          ;;
    jz      IntoSingleSpanThing

   ;; esi and edi are nonzero here so process the next links.
   ;; Start by checking if we connect with a previously allocated chunk.

   MulNextChunk:
    mov     eax,[edi].FSpan.StartX ;; line A start
    mov     ebx,[esi].FSpan.StartX ;; line B start
    cmp     eax,ebx
    ja      Mp2LeftBRightLook

   Mp2LeftARightLook: ;[A = eax
    mov     ebx,[ebp-SizeofFSpan].FSpan.EndX
    cmp     ebx,eax
    jb      MpLeftARightLook ; below: no connect.

    mov     eax,esi
    mov     esi,edi
    mov     edi,eax
    jmp     IntoLeftB

   Mp2LeftBRightLook: ;[B = ebx
    mov     eax,[ebp-SizeofFSpan].FSpan.EndX
    cmp     eax,ebx
    jb      MpLeftBRightLook ; below: no connect.

    mov     ebx,eax ;; have DestEnd in ebx
   IntoLeftB:
   ; DO connect: seldom happens; kludgy but small
    cmp     ebx,[edi].FSpan.EndX    ;; line A end
    ja      NextEDI         ;; if that end smaller, follow EDI link further
    cmp     ebx,[esi].FSpan.EndX    ;; line B end
    ja      NextESI         ;; if that end smaller, follow EDI link further
    sub     ebp,SizeofFSpan ;; WILL link up with last span, so merge
    jmp     IntoMPLeftB
    ;------------------------
   NextEDI:
    mov     edi,[edi].FSpan.Next
    test    edi,edi
    jnz     MulNextChunk
    mov     edi,esi
    jmp     IntoSingleSpanThing ;esi nonzero...
   NextESI:
    mov     esi,[esi].FSpan.Next
    test    esi,esi
    jnz     MulNextChunk
    jmp     IntoSingleSpanThing ;edi nonzero...


   ;allocate just the leftmost thing and update only ITS next,
   ;then loop back appropriately .....
   MpNonConnectBA: ;; end= line B end = ebx
    lea     eax, [ebp+SizeofFSpan]
    mov     [ebp].FSpan.EndX, ebx
    mov     [ebp].FSpan.Next,    eax
    mov     esi,[esi].FSpan.Next  ;; EDI already nonzero, checked..
    mov     ebp, eax
    test    esi, esi
    jnz     MulNextChunk
    jmp     IntoSingleSpanThing   ;; continue single spanning with EDI


   MpNonConnectAB: ;; end= line A end = eax
    lea     ebx, [ebp+SizeofFSpan]
    mov     [ebp].FSpan.EndX, eax
    mov     [ebp].FSpan.Next,    ebx
    mov     edi,[edi].FSpan.Next  ;; ESI already nonzero, checked..
    mov     ebp, ebx
    test    edi, edi
    jnz     MulNextChunk

    mov     edi,esi
    ;--|--
   IntoSingleSpanThing:            ;; line's remainder done with single link EDI
    mov     esi,[ebp-SizeofFSpan].FSpan.EndX ;; prev end to check linking to
    sub     ebp,SizeofFSpan        ;; may fit all into current output span...

    call    FinishSingleEnd        ;;

    add     ecx,4                  ;; ecx = Source index
    add     edx,4                  ;; edx = Destin index
    mov     eax, DS_EndIndex       ;;
    cmp     ecx,eax                ;; end of source reached ?
    jb      MultiplexReloop        ;; next line....  for ( i=1; i<n; i++ )
                                   ;;
    mov     edi,[ecx]              ;; edi == nonzero
    ;--------------------

   LastLatticeLine:                ;; assert edi nonzero !
    mov     [edx],ebp
    call    SingleStretch

   SkipLastLatticeLine:
   ;--------------------------
   ; EPILOG code
   ;==========================

   CalcLatticeFinish:

    ;;
    ;; Rounding up:
    ;;  - report back alloc'd mem to actual FMemPool structure
    ;;  - fill in the dest spanbuffer: Memory Pool ptr
    ;;  -                              ValidLines
    ;;

    mov     esi, DWThisPtr  ;;-ThisPtr (destination spanbuffer)
    mov     eax, DWGmem     ;;-GMemPoolPtr

    mov     ebx,[esi].FSpanbuf.List     ;; contains 'initial' MemTop
    mov     [esi].FSpanbuf.Mem    ,eax  ;; MemPool address into destbuf
    mov     [eax].FmemPool.MemTop ,ebp  ;; EBP==TopSpan

    sub     ebp,ebx    ;; calculate 0/nonzero 'boolean' ValidLines
    mov     [esi].FSpanbuf.ValidLines ,ebp  ;; = ValidLines

    pop     edi
    pop     esi
    pop     ebx
    pop     ebp
    ;;! if RET instead of RETN, MASM inserts "LEAVE" = mov  esp,ebp / pop ebp
    retn
;=============================


   align 16
   DB 15 dup (090h) ;#natalign 5
;=======================; called at least twice every time..
;  SingleStretchEDI:   ;; ebp == Topspan, (output destination)
;                      ;; edi == 'SPAN1'  (single input pointer) >>destroyed
;                      ;; edx == PrevLink
;=======================;

;; ESI must contain  (previous) 'DestEnd'

   SkipSpanFSE:
    nop
    mov     edi,[edi].FSpan.Next
    test    edi, edi
    jz      CloseSingle
   FinishSingleEnd: ;;(mind if following links have smaller DestEnds..)
    nop
    mov     ebx,[edi].FSpan.EndX
    cmp     ebx, esi  ;; smaller end: just skip this whole thing...
    jb      SkipSpanFSE

   TryConnectNextSingleLink:
    mov     eax,[edi].FSpan.StartX
    lea     ebx,[ebp+SizeofFSpan]
    cmp     eax,esi                 ;; Span->Start <= PREVIOUS DestEnd+1 :link!
    jbe     LoadNewEnd              ;; connect: so load new end,next link and try again
   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; UPDATE_PREVLINK (DestStart,DestEnd);
    mov     [ebp].FSpan.EndX,esi ;; nonconnecting, so independent one
    mov     [ebp].FSpan.Next,ebx    ;; NEXT: point to next *expected* span
    mov     ebp, ebx                ;; update MEMTOP
   ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; edi != null
  SingleStretch:    ;#natalign 5
   LoadNewStart:    ; Default Entry point: make sure [edx] set,and edi !=null
   ;; ASSUME: start already loaded ?
    mov     eax,[edi].FSpan.StartX
    mov     [ebp].FSpan.StartX,eax  ;; immediate save
   LoadNewEnd:
    mov     esi,[edi].FSpan.EndX
    mov     edi,[edi].FSpan.Next
    inc     esi
    test    edi,edi
    jnz     TryConnectNextSingleLink
   ;; Default case : LAST link for this line.
   ;;;;;;;;;;;;;;;;;;;;; UPDATE_PREVLINK (DestStart,DestEnd);
   CloseSingle:
    xor     ebx,ebx
    mov     [ebp].FSpan.EndX,esi
    mov     [ebp].FSpan.Next   ,ebx   ;; *PrevLink = NULL
    add     ebp,SizeofFSpan           ;; update MEMTOP
   ;;;;;;;;;;;;;;;;;;;;;
    RETN                              ;; SingleStretchEDI line completed
;==================================== ;;



   align 16
   ;-----------------------------------
   ;;
   ;; FAST null-input handler ( 0 or 1 input lines instead of 2 )
   ;;
   ;-----------------------------------
   ZeroSecond:               ;; [ecx+4] contains zero ptr
    mov     [edx],ebp        ;; point dest index to expected span
    call    SingleStretch    ;; EDI= nonzero
   BothZero:
    add     edx,4                 ;; edx = Destin index
    add     ecx,4                 ;; ecx = Source index
    mov     eax, DS_EndIndex      ;;
    cmp     ecx, eax              ;; end of source reached ?
    mov     dword ptr [edx],0     ;; in case of last line...
    jnb     SkipLastLatticeLine   ;; out if last line *which is zero !
   ;------------------------;;
   ZeroFirst:               ;; enter here only after very first line is zero
    mov     edi,[ecx+4]     ;; second nonzero?
    mov     dword ptr [edx],0  ;; dest. index to null in advance..
    test    edi,edi         ;;
    jz      BothZero        ;; makes fast loop for all-zero spans..

    mov     [edx],ebp       ;; point dest index to expected span
    call    SingleStretch   ;; digest span in EDI. Expect next

    add     ecx,4           ;; ecx = Source index
    add     edx,4           ;; edx = Destin index
    mov     eax, DS_EndIndex
    cmp     ecx, eax        ;; end of source reached ?
    mov     edi, [ecx]
    jnb     LastLatticeLine ;; out if last line *which is nonzero

    mov     esi, [ecx+4]    ;; edi now nonzero first, esi nonzero too?
    test    esi, esi
    jnz     MultiPlexLoop   ;; ESI and EDI both nonzero - proceed normally
    jmp     ZeroSecond
    ;-----------------------;;


;=====================================
 TwinLineOutput:
    xor     eax,eax
    mov     edi,[ecx]              ;; edi = single input line..
    mov     dword ptr [edx  ],eax
    test    edi,edi
    mov     dword ptr [edx+4],eax
    jz      SkipLastLatticeLine
    ;--------------------
    mov     [edx],ebp      ;; output index[0]
    call    SingleStretch

    mov     edi,[ecx]      ;; same input again
    mov     [edx+4],ebp    ;; output index[1]
    call    SingleStretch
    ;--------------------
    jmp     CalcLatticeFinish


;=====================================
 LatticeNullOutPut:
    xor     ecx, ecx
    mov     [edi].FSpanbuf.Index,ecx ; 'OurIndex0',esi  Index = get..()
    mov     [edi].FSpanbuf.List ,ecx ; 'OurList' ,esi ; List = GetFast4(0)
    jmp     CalcLatticeFinish
;=====================================


CalcLatticeFromMASM ENDP
;--------------------------------------------------------------------------------



END


