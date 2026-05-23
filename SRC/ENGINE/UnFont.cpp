/*=============================================================================
	UnFont.cpp: Unreal font code.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*------------------------------------------------------------------------------
	UFont character drawing.
------------------------------------------------------------------------------*/

//
// Draw a character into a texture.
//
void UFont::Cout
(
	UTexture	*DestTexture, 
	int			X, 
	int			Y, 
	int			XSpace, 
	char		C, 
	RAINBOW_PTR Colors
)
{
	guard(UFont::Cout);

	RAINBOW_PTR Table = Colors;
	FCacheItem *Item;
	if( !Colors.PtrVOID )
	{
		Texture->Lock(LOCK_Read);
		Table = Texture->Palette->GetColorDepthPalette(Item, DestTexture);
	}

	FFontCharacter  *FontIndex  	= &Element(C);
	BYTE			*Source			= &Texture->Element(0);
	RAINBOW_PTR		Dest			= DestTexture->GetData();
	
	BYTE			*SourcePtr1,*SourcePtr,*DestPtr1,*DestPtr,B;
	int				U,V,EndX,EndY,XC;

	U=0; if (X<0) {U+=(-X); X=0;}; EndX = X+FontIndex->USize-U; U += FontIndex->StartU;
	V=0; if (Y<0) {V+=(-Y); Y=0;}; EndY = Y+FontIndex->VSize-V; V += FontIndex->StartV;

	if (EndX > DestTexture->USize) EndX = DestTexture->USize;
	if (EndY > DestTexture->VSize) EndY = DestTexture->VSize;

	SourcePtr1 = &Source [U + V*Texture->USize];
	if( DestTexture->ColorBytes == 1 )
	{
		DestPtr1		= &Dest.PtrBYTE[X + Y*DestTexture->USize];
		while( Y++ < EndY )
		{
			SourcePtr = SourcePtr1;
			DestPtr   = DestPtr1;

			for( XC=X; XC<EndX; XC++ )
			{
				B = *SourcePtr++;
				if (B) *DestPtr = Table.PtrBYTE[B];
				DestPtr++;
			}
			SourcePtr1 += Texture->USize;
			DestPtr1   += DestTexture->USize;
		}
	}
	else if( DestTexture->ColorBytes == 2 )
	{
		WORD *DestPtr1 = &Dest.PtrWORD[X + Y*DestTexture->USize],*DestPtr;
		while( Y++ < EndY )
		{
			SourcePtr = SourcePtr1;
			DestPtr   = DestPtr1;

			for( XC=X; XC<EndX; XC++ )
			{
				B = *SourcePtr++;
				if (B) *DestPtr = Table.PtrWORD[B];
				DestPtr++;
			}
			SourcePtr1 += Texture->USize;
			DestPtr1   += DestTexture->USize;
		}
	}
	else if( DestTexture->ColorBytes == 4 )
	{
		DWORD *DestPtr1 = &Dest.PtrDWORD[X + Y*DestTexture->USize],*DestPtr;
		while( Y++ < EndY )
		{
			SourcePtr = SourcePtr1;
			DestPtr   = DestPtr1;

			for( XC=X; XC<EndX; XC++ )
			{
				B = *SourcePtr++;
				if (B) *DestPtr = Table.PtrDWORD[B];
				DestPtr++;
			}
			SourcePtr1 += Texture->USize;
			DestPtr1   += DestTexture->USize;
		}
	}
	if( !Colors.PtrVOID )
	{	
		Item->Unlock();
		Texture->Unlock(LOCK_Read);
	}
	unguard;
}

/*------------------------------------------------------------------------------
	UFont string length functions.
------------------------------------------------------------------------------*/

//
// Calculate the length of a string built from a font, starting at a specified
// position and counting up to the specified number of characters (-1 = infinite).
//
void UFont::StrLen
(
	int			&XL, 
	int			&YL, 
	int			XSpace, 
	int			YSpace, 
	const char	*Text,
	int			iStart,
	int			NumChars
)
{
	guard(UFont::PartialStrLen);
	Lock(LOCK_Read);

	XL = YL = 0;

	const char *c = &Text[iStart];
	while( *c && NumChars>0 )
	{
		XL += Element(*c).USize + XSpace;
		YL  = ::Max(YL,Element(*c).VSize);
		c++;
		NumChars--;
	}
	YL += YSpace;

	Unlock(LOCK_Read);
	unguard;
}

//
// Calculate the size of a string built from a font, word wrapped
// to a specified region.
//
void UFont::WrappedStrLen
(
	int			&XL, 
	int			&YL, 
	int			XSpace, 
	int			YSpace, 
	int			MaxWidth, 
	const char	*Text
)
{
	guard(UFont::WrappedStrLen);

	int iLine=0;
	int TestXL,TestYL;
	XL = YL = 0;

	// Process each output line.
	while( Text[iLine] )
	{
		// Process each word until the current line overflows.
		int iWord, iTestWord=iLine;
		do
		{
			iWord = iTestWord;
			if( !Text[iTestWord] )
				break;

			while( Text[iTestWord] && Text[iTestWord]!=' ' )
				iTestWord++;
			
			while( Text[iTestWord]==' ' )
				iTestWord++;

			StrLen( TestXL, TestYL, XSpace, YSpace, Text, iLine, iTestWord-iLine );
		} while( TestXL <= MaxWidth );
		
		if( iWord == iLine )
		{
			// The text didn't fit word-wrapped onto this line, so chop it.
			int iTestWord = iLine;
			do
			{
				iWord = iTestWord;
				if( !Text[iTestWord] )
					break;

				iTestWord++;

				StrLen( TestXL, TestYL, XSpace, YSpace, Text, iLine, iTestWord-iLine );
			} while( TestXL <= MaxWidth );
			
			// Word wrap failed because window is too small to hold a single character.
			if( iWord == iLine )
				return;
		}

		// Sucessfully split this line.
		StrLen( TestXL, TestYL, XSpace, YSpace, Text, iLine, iWord-iLine );
		checkLogic(TestXL<=MaxWidth);
		YL += TestYL;
		if( TestXL > XL )
			XL = TestXL;

		// Go to the next line.
		while( Text[iWord]==' ' )
			iWord++;
		
		iLine = iWord;
	}
	unguard;
}

/*------------------------------------------------------------------------------
	UFont string printing functions.
------------------------------------------------------------------------------*/

//
// Draw a font onto a texture.  Normally called with a camera's
// screen texture.
//
void VARARGS UFont::Printf
(
	UTexture	*DestTexture, 
	int			X, 
	int			Y, 
	int			XSpace, 
	const char	*Fmt,
	...
)
{
	va_list  ArgPtr;
	char	 Text[256];

	va_start (ArgPtr,Fmt);
	vsprintf (Text,Fmt,ArgPtr);
	va_end   (ArgPtr);

	guard(UFont::Printf);

	FCacheItem *Item;
	RAINBOW_PTR Ptr = Texture->Palette->GetColorDepthPalette(Item, DestTexture);
	Texture->Lock(LOCK_Read);

	char *c = &Text[0];
	while( *c != 0 )
	{
		Cout( DestTexture, X, Y, XSpace, *c, Ptr );
		X += Element(*c).USize + XSpace;
		c ++;
	}

	Texture->Unlock(LOCK_Read);
	Item->Unlock();
	unguard;
}

//
// Wrapped printf.
//
void VARARGS UFont::WrappedPrintf
(
	UTexture	*DestTexture,
	int			X, 
	int			Y, 
	int			XSpace, 
	int			YSpace,
	int			Width, 
	int			Center, 
	const char	*Fmt,...
)
{
	va_list  ArgPtr;
	char	 Text[256];

	va_start (ArgPtr,Fmt);
	vsprintf (Text,Fmt,ArgPtr);
	va_end   (ArgPtr);

	guard(UFont::WrappedPrintf);

	int iLine=0;
	int TestXL,TestYL;

	// Process each output line.
	while( Text[iLine] )
	{
		// Process each word until the current line overflows.
		int iWord, iTestWord=iLine;
		do
		{
			iWord = iTestWord;
			if (!Text[iTestWord]) 
				break;

			while (Text[iTestWord] && (Text[iTestWord]!=' ')) 
				iTestWord++;
			
			while (Text[iTestWord]==' ') 
				iTestWord++;

			StrLen(TestXL,TestYL,XSpace,YSpace,Text,iLine,iTestWord-iLine);

		} while (TestXL <= Width);

		if( iWord==iLine )
		{
			// The text didn't fit word-wrapped onto this line, so chop it.
			int iTestWord = iLine;
			do
			{
				iWord = iTestWord;
				if (!Text[iTestWord]) 
					break;

				iTestWord++;

				StrLen(TestXL,TestYL,XSpace,YSpace,Text,iLine,iTestWord-iLine);
			
			} while (TestXL <= Width);
			
			if (iWord==iLine) 
				return; // Word wrap failed.
		}

		// Sucessfully split this line, now draw it.
		char Temp[256];
		strcpy(Temp,&Text[iLine]);
		Temp[iWord-iLine]=0;

		StrLen(TestXL,TestYL,XSpace,YSpace,Text,iLine,iWord-iLine);
		
		checkLogic(TestXL<=Width);

		Printf
		(
			DestTexture,
			Center ? (X + ((Width-TestXL)>>1)) : X,
			Y,
			XSpace,
			"%s",
			Temp
		);
		Y += TestYL;

		// Go to the next line.
		while (Text[iWord]==' ') 
			iWord++;
		
		iLine = iWord;
	}
	unguard;
}

/*------------------------------------------------------------------------------
	Font importing/processing.
------------------------------------------------------------------------------*/

//
//	Fast pixel-lookup macro
//
//	a=screen buffer (byte pointer)
//	b=screen length (such as 320)
//	x=X coordinate
//	y=Y coordinate
//
inline BYTE AT(BYTE *Screen,int SXL,int X,int Y) {return Screen[X+Y*SXL];};

//
//	Find the border around a font character that starts at x,y (it's upper
//	left hand corner).  If it finds a character box, it returns 0 and the
//	character's length (xl,yl).  Otherwise returns -1.
//
// Doesn't check x or y for overflowing.
//
int UFont_ScanFontBox( UTexture *Texture, int X, int Y, int &XL, int &YL )
{
	guard(UFont_ScanFontBox);

	BYTE	*TextureData = &Texture->Element(0);
	int 	FontXL = Texture->USize;
	int 	NewXL,NewYL;

	// Find x-length.
	NewXL = 1;
	while
	(
		(AT(TextureData,FontXL,X+NewXL,Y)==255) &&
		(AT(TextureData,FontXL,X+NewXL,Y+1)!=255)
	)	NewXL++;
	
	if( AT(TextureData,FontXL,X+NewXL,Y)!=255 )
		return -1;

	// Find y-length.
	NewYL = 1;
	while
	(
		(AT(TextureData,FontXL,X,Y+NewYL)==255)&&
		(AT(TextureData,FontXL,X+1,Y+NewYL)!=255)
	) NewYL++;

	if (AT(TextureData,FontXL,X,Y+NewYL)!=255)
		return -1;

	XL = NewXL - 1;
	YL = NewYL - 1;
	return 0;

	unguard;
}

//
// UFont constructor.
//
UFont::UFont(UTexture *ThisTexture)
{
	guard(UFont::UFont);

	// Init header properties.
	Texture = ThisTexture;

	Texture->Lock(LOCK_Read);
	BYTE *TextureData = &Texture->Element(0);

	// Allocate chars.
	guard(A);
	Max = NUM_FONT_CHARS;
	Realloc();
	Num = Max;
	unguard;

	// Init all characters to "unavailable".
	for( int i=0; i<Num; i++ )
	{
		Element(i).StartU = 0; Element(i).USize = 0;
		Element(i).StartV = 0; Element(i).VSize = 0;
	}

	// Scan in all fonts, starting at character 32.
	i = 32;
	int Y = 0;

	do
	{
		int X = 0;
		while( (AT(TextureData,Texture->USize,X,Y) != 255)&&(Y < Texture->VSize) )
		{
			X++;
			if( X >= Texture->USize )
			{
				X = 0;
				if( ++Y >= Texture->VSize )
					break;
			}
		}

		// Scan all characters in this row.
		if( Y < Texture->VSize )
		{
			int XL,YL,MaxYL = 0;
			while( (i<Num) && (UFont_ScanFontBox(Texture,X,Y,XL,YL)==0) )
			{
				//bug ("C%i %i,%i - %i,%i",i,X,Y,XL,YL);
				Element(i).StartU = X+1;
				Element(i).StartV = Y+1;
				Element(i).USize  = XL;
				Element(i).VSize  = YL;

				X += XL + 1;
				i ++;
				if (YL>MaxYL) MaxYL = YL;
			}

			// Proceed past end of this row.
			Y = Y + MaxYL + 1;
		}
	} while( (i < Num) && (Y < Texture->VSize) );

	Texture->Unlock(LOCK_Read);
	unguard;
}

/*------------------------------------------------------------------------------
	UFont's UObject interface.
------------------------------------------------------------------------------*/

void UFont::InitHeader()
{
	guard(UFont::InitHeader);

	// Call parent.
	UDatabase::InitHeader();

	// Init UFont info.
	Texture		= (UTexture *)NULL;
	Max			= 0;
	Num			= 0;
	unguard;
}
IMPLEMENT_DB_CLASS(UFont);

/*------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------*/
