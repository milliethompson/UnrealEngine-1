/*=============================================================================
	UnLine.cpp: INCLUDABLE C FILE for drawing lines.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	This is used to avoid the overhead of establishing a stack frame and calling
	a routine for every scanline to be drawn.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

//
//void DrawLine(const CAMERA_INFO *Camera, FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2,..)
//	{
	FLOAT	FloatFixDX,Temp;
	BYTE	*Screen;
	INT		FixDX,FixX,X,Y,DY,Count,DestX,ScreenInc,ColorBytes;
	//
#ifdef  DEPTHSHADE
		FLOAT	G1,G2;
		INT		FixG1,FixG2,FixDG,FixTemp;
#endif
	guard(UnLine);
	//
	// Bounds check
	//
	if ((X1 < 0.0       ) || (X2 < 0.0        ) ||
		(Y1 < 0.0       ) || (Y2 < 0.0        ) ||
		(X1 > Camera->FX) || (X2 > Camera->FX ) ||
		(Y1 > Camera->FY) || (Y2 > Camera->FY))
		{
		debugf(LOG_Problem,"Line: Bounds (%f,%f) (%f,%f)",X1,Y1,X2,Y2);
		goto LABEL2(Out);
		};
	ColorBytes=Camera->ColorBytes;
	//
	// Depth shading
	//
#ifdef DEPTHSHADE
		#define DEPTHSETUP(Arclen) FixDG = (FixG2-FixG1)/Arclen;
		G1 = 65536.0 * 12 - RZ1 * 65536.0 * 100000.0; if (G1<0.0) G1=0.0;
		G2 = 65536.0 * 12 - RZ2 * 65536.0 * 100000.0; if (G2<0.0) G2=0.0;
		FixG1 = ftoi(G1);
		FixG2 = ftoi(G2);
#else
		#define DEPTHSETUP(Arclen)
#endif
	//
	// Arrange so that Y2 >= Y1
	//
	if (Y2 < Y1)
		{
		Temp = Y1; Y1 = Y2; Y2 = Temp;
		Temp = X1; X1 = X2; X2 = Temp;
		//
#ifdef DEPTHSHADE
			FixTemp=FixG1; FixG1 = FixG2; FixG2 = FixTemp;
#endif
		};
	DestX	= ftoi(X2);
	Y		= ftoi(Y1-0.5);
	DY      = ftoi(Y2-0.5)-Y;
	//
	if (DY==0) // Horizontal line
		{
		if (X2>X1)
			{
			X = ftoi(X1-0.5); Count = ftoi(X2-0.5) - X;
			}
		else
			{
			X = ftoi(X2-0.5); Count = ftoi(X1-0.5) - X;
			//
#ifdef DEPTHSHADE
				Exchange(FixG1,FixG2);
#endif
			};
#if ISDOTTED
			LineToggle = X&1;
#endif
		if (Count>0)
			{
			guard(Case 1);
			DEPTHSETUP(Count);
			Screen = Camera->Screen + ((X + Y*Camera->Stride)<<SHIFT);
			//
			#if defined(ASM) && defined(ASMPIXEL)
			__asm
				{
				mov edi,[Screen]
				mov esi,[ColorBytes]
				mov ecx,[Count]
#ifdef DEPTHSHADE
					mov eax,[FixG1]
#else
					mov eax,[NewColor]
#endif
				;
				LABEL2(HInner):
				L_ASMPIXEL(HInner)
				add edi,esi
				dec ecx
				jg  LABEL2(HInner)
				};
			#else
			while (Count-- > 0)
				{
				L_DRAWPIXEL(Screen); 
				Screen += ColorBytes;
				};
			#endif
			unguard;
			};
		goto LABEL2(Out);
		};
	FloatFixDX = 65536.0 * (X2-X1) / (Y2-Y1);
	FixDX      = ftoi(FloatFixDX);
	FixX       = ftoi(65536.0 * X1 + FloatFixDX * ((FLOAT)(Y+1) - Y1));
	//
	if (FixDX < -Fix(1)) // From -infinity to -1 (Horizontal major)
		{		   
		guard(Case 3);
		X      = ftoi(X1-0.5);
		Screen = Camera->Screen + ((X + Y*Camera->Stride)<<SHIFT);
		//
		#if ISDOTTED
			LineToggle = X&1;
		#endif
		//
		DEPTHSETUP(Max(DY,X-ftoi(X2-0.5)));
		//
		while (--DY >= 0)
			{
			Count 	 = X;
			X 		 = Unfix(FixX);
			Count 	-= X;
			while (Count-- > 0)
				{
				Screen -= ColorBytes; 
				L_DRAWPIXEL(Screen);
				};
			Screen 	+= Camera->Stride << SHIFT;
			FixX   	+= FixDX;
			};
		while (X-- > DestX) {Screen -= ColorBytes; L_DRAWPIXEL(Screen);};
		unguard;
		}
	else if (FixDX > Fix(1)) // From 1 to +infinity (Horizontal major)
		{
		guard(Case 4);
		X 	   = ftoi(X1-0.5);
		Screen = Camera->Screen + ((X + Y*Camera->Stride)<<SHIFT);
		//
		#if ISDOTTED
			LineToggle = X&1;
		#endif
		//
		DEPTHSETUP(Max(DY,ftoi(X2-0.5)-X));
		//
		while (--DY >= 0)
			{
			Count 	 = X;
			X 		 = Unfix(FixX);
			Count 	-= X;
			while (Count++ < 0)
				{
				L_DRAWPIXEL(Screen);
				Screen += ColorBytes;
				};
			Screen 	+= Camera->Stride << SHIFT;
			FixX   	+= FixDX;
			};
		while (++X < DestX)
			{
			L_DRAWPIXEL(Screen);
			Screen += ColorBytes;
			};
		unguard;
		}
	else if (DY>0) // Vertical major
		{
		Screen = Camera->Screen + ((Y*Camera->Stride)<<SHIFT);
		ScreenInc = Camera->Stride << SHIFT;
		#if ISDOTTED
			LineToggle = Y & 1;
		#endif
		//
		DEPTHSETUP(DY);
		//
		int D=DY,XX=FixX;
		guard(Case 2);
		#if defined(ASM) && defined(ASMPIXEL)
		__asm
			{
#ifdef DEPTHSHADE
				mov eax,[FixG1]
#else
				mov eax,[NewColor]
#endif
			;
			LABEL2(VInner):
			mov edx,[FixX]					; u Fixed point X location
			mov esi,[FixDX]					; v Get X increment
			add esi,edx						; u Update X increment
			mov edi,[Screen]				; v Screen destination
			sar edx,16-SHIFT				; u Unfix the location it
			mov [FixX],esi					; v Saved next X value
			mov esi,[ScreenInc]				; u Get screen increment
			and edx,0xffffffff << SHIFT		; v Mask out 1/2/4-byte color
			add esi,edi						; u Get next screen destination
			add edi,edx						; v Get destination address forthis pixel
			mov [Screen],esi				; u Save next screen destination
			;
			L_ASMPIXEL(VInner)
			;
			mov edx,[DY]
			dec edx
			mov [DY],edx
			jg LABEL2(VInner)
			};
		#else
		do  {
			L_DRAWPIXEL(Screen + (Unfix(FixX)<<SHIFT));
			FixX 	+= FixDX;
			Screen 	+= ScreenInc;
			} while (--DY > 0);
		#endif
		unguard;
		};
	#undef DEPTHSETUP
	unguard;
	LABEL2(Out):;
//	};
