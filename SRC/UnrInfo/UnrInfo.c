/*=============================================================================
	UnrInfo.c: Unreal file info code.

	Copyright 1997 Epic MegaGames, Inc.
	Freely distributable & modifiable.
	standard ANSI C (32-bit), compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Description:
		Public utility library for dealing with Unreal files.

	Revision history:
		* Created by Tim Sweeney.
=============================================================================*/

#include <stdio.h>
#include "UnrInfo.h"

/*-----------------------------------------------------------------------------
	Unrealfile thing readers.
	All functions return 0 if success, <0 if error.
	All functions assume File is open for reading in binary mode ("rb").
-----------------------------------------------------------------------------*/

//
// Read a byte.
//
int unrReadByte( FILE *File, unsigned char *b )
{
	return fread( b, 1, 1, File )==1 ? 0 : -1;
}

//
// Read an integer.
//
int unrReadInt( FILE *File, int *i )
{
	union int_u
	{
		unsigned char c[4];
		__int32  i;
	} temp;

	if( unrReadByte( File, &temp.c[0] ) < 0 ) return -1;
	if( unrReadByte( File, &temp.c[1] ) < 0 ) return -1;
	if( unrReadByte( File, &temp.c[2] ) < 0 ) return -1;
	if( unrReadByte( File, &temp.c[3] ) < 0 ) return -1;

	*i = temp.i;
	return 0;
}

//
// Read a fixed length string.
//
int unrReadFixedLengthString( FILE *File, char *c, int Length )
{
	return fread( c, Length, 1, File )==1 ? 0 : -1;
}

//
// Read a zero-terminated string.
//
int unrReadString( FILE *File, char *c, int MaxLength )
{
	int Count = 0;
	while( Count < MaxLength )
	{
		if( fread( &c[Count], 1, 1, File )!=1 )
			return -1;
		else if( c[Count] == 0 )
			return 0;
		else
			Count++;
	}

	// Overflowed.
	return -1;
}

//
// Read an Unrealfile header.
//
int unrReadHeader( FILE *File, unrealheader_t *Header )
{
	// Read the header info.
	fseek( File, 0, SEEK_SET );
	if( unrReadFixedLengthString( File, Header->Tag, sizeof(Header->Tag) ) < 0 ) return -1;
	if( unrReadInt( File, &Header->UnrealFileVersion ) < 0 ) return -1;
	if( unrReadInt( File, &Header->NumExports        ) < 0 ) return -1;
	if( unrReadInt( File, &Header->NumImports        ) < 0 ) return -1;
	if( unrReadInt( File, &Header->NumNames          ) < 0 ) return -1;
	if( unrReadInt( File, &Header->NamesOffset       ) < 0 ) return -1;
	if( unrReadInt( File, &Header->ImportsOffset     ) < 0 ) return -1;
	if( unrReadInt( File, &Header->ExportsOffset     ) < 0 ) return -1;
	if( unrReadInt( File, &Header->NumDependencies   ) < 0 ) return -1;

	// Success.
	return 0;
}

//
// Read an Unreal name.
//
int unrReadName( FILE *File, unrealname_t *Name )
{
	if( unrReadString( File, Name->Name, UNREAL_NAME_SIZE ) < 0 ) return -1;
	if( unrReadInt( File, &Name->Flags ) < 0 ) return -1;
	return 0;
}

//
// Read an Unreal object import record.
//
int unrReadImport( FILE *File, unrealimport_t *Import )
{
	if( unrReadString( File, Import->Name, UNREAL_NAME_SIZE ) < 0 ) return -1;
	if( unrReadByte( File, &Import->Type ) < 0 ) return -1;
	return 0;
}

//
// Read an Unreal object export record.
//
int unrReadExport( FILE *File, unrealexport_t *Export )
{
	if( unrReadString( File, Export->Name, UNREAL_NAME_SIZE ) < 0 ) return -1;
	if( unrReadByte(   File, &Export->Type             ) < 0 ) return -1;
	if( unrReadInt(    File, &Export->Flags            ) < 0 ) return -1;
	if( unrReadInt(    File, &Export->FileCRC          ) < 0 ) return -1;
	if( unrReadInt(    File, &Export->FileHeaderOffset ) < 0 ) return -1;
	if( unrReadInt(    File, &Export->FileDataOffset   ) < 0 ) return -1;
	if( unrReadInt(    File, &Export->FileHeaderSize   ) < 0 ) return -1;
	if( unrReadInt(    File, &Export->FileDataSize     ) < 0 ) return -1;
	return 0;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
