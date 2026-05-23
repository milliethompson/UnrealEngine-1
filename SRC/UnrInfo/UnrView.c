/*=============================================================================
	UnrView.c: Unreal file info code sample.

	Copyright 1997 Epic MegaGames, Inc.
	Freely distributable & modifiable.
	standard ANSI C (32-bit), Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Description:
		Public utility library for dealing with Unrealfiles.

	Revision history:
		* Created by Tim Sweeney.
=============================================================================*/

#include <stdio.h>
#include <ctype.h>
#include "UnrInfo.h"

/*-----------------------------------------------------------------------------
	Parameter helpers.
-----------------------------------------------------------------------------*/

//
// See if a parameter resides in the command line.
// Returns 1 if it does, 0 if not.
//
int findparm( char parm, int argc, char *argv[] )
{
	int i,j;
	for( i=1; i<argc; i++ )
		if( argv[i][0]=='-' || argv[i][0]=='/' )
			for( j=0; argv[i][j]!=0; j++ )
			if( toupper(argv[i][j])==toupper(parm) )
				return 1;
	return 0;
}

/*-----------------------------------------------------------------------------
	Main.
-----------------------------------------------------------------------------*/

//
// Display information about the specified Unrealfile.
//
int main( int argc, char *argv[] )
{
	unrealheader_t Header;
	unrealname_t   Name;
	unrealimport_t Import;
	unrealexport_t Export;
	FILE           *F;
	char           Filename[UNREAL_NAME_SIZE];
	int            i;

	// Print banner.
	printf("\r\n");
	printf("Unreal file information extractor, v" UNRINFO_VERSION ": " __DATE__ ".\r\n");
	printf("Copyright 1997 Epic MegaGames, Inc.\r\n");
	printf("\r\n");

	// Check parms.
	if( argc<2 || findparm('?',argc,argv) || findparm('h',argc,argv) )
	{
		printf("Usage:   UnrInfo <filename.unr> <options>\r\n");
		printf("Options: -d Display dependencies\r\n");
		printf("         -e Display exports (objects residing in the Unrealfile)\r\n");
		printf("         -i Display imports (objects required outside of the Unrealfile)\r\n");
		printf("         -n Display names\r\n");
		return -1;
	}

	// Open the file.
	F = fopen(argv[1],"rb");
	if( !F )
	{
		printf("Error opening: %s\r\n",argv[1]);
		return -1;
	}

	// See if it's an Unreal file.
	if( unrReadHeader( F, &Header ) < 0 )
	{
		printf("Not an Unrealfile: %s\r\n",argv[1]);
		fclose(F);
		return -1;
	}

	// Display info.
	printf("  Unrealfile: %s\r\n",argv[1]);
	printf("     Version: %i\r\n",Header.UnrealFileVersion);
	printf("Dependencies: %i\r\n",Header.NumDependencies);
	printf("     Exports: %i\r\n",Header.NumExports);
	printf("     Imports: %i\r\n",Header.NumImports);
	printf("       Names: %i\r\n",Header.NumNames);

	// Show dependencies.
	if( findparm('d',argc,argv) )
	{
		printf("\r\nIndex Dependency");
		printf("\r\n----- ----------\r\n");
		for( i=0; i<Header.NumDependencies; i++ )
		{
			if( unrReadString( F, Filename, sizeof( Filename ) ) < 0 ) goto Error;
			printf("% 5i %s\r\n",Filename);
		}
	}

	// Show exports.
	if( findparm('e',argc,argv) )
	{
		printf("\r\nIndex Tp Flags    Header          Data            Name");
		printf("\r\n----- -- -------- --------------- --------------- -----------\r\n");
		fseek( F, Header.ExportsOffset, SEEK_SET );
		for( i=0; i<Header.NumExports; i++ )
		{
			if( unrReadExport( F, &Export ) < 0 ) goto Error;
			printf( "% 5i %02i %08x %06x [%06x] %06x [%06x] %s\r\n",
				i, Export.Type, Export.Flags, Export.FileHeaderOffset, Export.FileHeaderSize,
				Export.FileDataOffset, Export.FileDataSize, Export.Name );
		}
	}

	// Show imports.
	if( findparm('i',argc,argv) )
	{
		printf("\r\nIndex Tp Name");
		printf("\r\n----- -- -----------\r\n");
		fseek( F, Header.ImportsOffset, SEEK_SET );
		for( i=0; i<Header.NumImports; i++ )
		{
			if( unrReadImport( F, &Import ) < 0 ) goto Error;
			printf("% 5i %02x %s\r\n", i, Import.Type, Import.Name );
		}
	}

	// Show names.
	if( findparm('n',argc,argv) )
	{
		printf("\r\nIndex Flags    Name");
		printf("\r\n----- -------- -----------\r\n");
		fseek( F, Header.NamesOffset, SEEK_SET );
		for( i=0; i<Header.NumNames; i++ )
		{
			if( unrReadName( F, &Name ) < 0 ) goto Error;
			printf("% 5i %08x %s\r\n", i, Name.Flags, Name.Name );
		}
	}

	// Success.
	return 0;

	// Error.
Error:
	printf("Error reading file");
	return -1;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
