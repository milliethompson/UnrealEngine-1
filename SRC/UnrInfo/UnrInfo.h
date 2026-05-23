/*=============================================================================
	UnrInfo.h: Unreal file info code.

	Copyright 1997 Epic MegaGames, Inc.
	Freely distributable & modifiable.
	standard ANSI C (32-bit), Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Description:
		Public utility library for dealing with Unrealfiles.

	The Unrealfile format:

		Unreal files consist of a file header (shown in unrealheader_t), a list of 
		dependencies, and different categories of records containing information
		about the Unreal objects and names stored in the file.

		All data stored in Unrealfiles is in little-endian (Intel) byte order.

		Many datatypes, including strings, are stored with a variable length in
		Unrealfiles.  Therefore, it is easiest to read Unrealfiles byte by byte.

		Unrealfiles can store a wide variety of interrelated objects. 
		To accomodate this, Unreal's internal objects are dynamically "linked" when 
		loaded into the engine from Unrealfiles, and are dynamically "delinked" when 
		saved into Unrealfiles. This involves associating pointers with file indices, 
		and is similar to the dynalinking process that occurs when loading and unloading 
		DLL's.  Therefore, loading and saving Unrealfiles is not straightforward like 
		dealing with .bmp or .dxf files.  This means that you cannot arbitrarily add 
		resources to, or remove objects from, an Unrealfile unless you know the format
		of the objects you're dealing with.

		Like a DLL, an Unrealfile contains a table of imports (objects which the
		Unrealfile requires but does not contain), and a table of exports (objects which
		are stored within the Unrealfile).  Also like a DLL, whenever an Unrealfile contains 
		one or more imports, the Unrealfile also contains a dependency list, a list of 
		other Unrealfiles which the Unrealfile relies on.

		Each object in an Unrealfile has a type, which is identified by a byte value
		that is unique for that type.  To deal with an object in the Unrealfile, you
		need to know the format of that object type.  The object type formats are
		not documented here.

		The six types of records in the Unrealfile are:

		*	The file header, found at the beginning of the file.  This contains
		    a tag, version information, and counts and offsets of the other records
			in the file.

		*	The dependency list, a list of other Unrealfiles which must be loaded
		    before the Unrealfile itself is loaded.  Unrealfiles may not have
			circular dependencies.

        *   Export table, a list of generic information about each object in the file.
		    This format is shown in unrealexport_t.

		*	Export headers, a variable-length block of type-specific information about 
		    each object in the file. The information in the export table is generally 
			small.

		*	Export data, a large amount of bulk data for each kind of object.  By
		    separating objects into the table and data, we are able to keep the
			summary information about an object into memory at all times, while
			demand paging the bulk data.  For example, an Unreal texture map: the table
			contains the texture map's name, dimensions, and other basic info, and the data
			portion contains the texture's bitmaps.

		*	Import table, a list of objects which the Unrealfile requires to be loaded.
		    The objects listed in the import table can be found within the Unrealfiles 
			listed in the dependency list.

		*	Names, a list of case-insensitive name strings (up to 32 characters) referenced
		    by the Unrealfile.  Names are indexed into this table, rather than stored
			individually, to save space.

	Revision history:
		* Created by Tim Sweeney.
=============================================================================*/

#ifndef _INC_UNRINFO
#define _INC_UNRINFO

/*-----------------------------------------------------------------------------
	Unrealfile headers.
-----------------------------------------------------------------------------*/

// Version of this utility.
#define UNRINFO_VERSION "0.9"

// Tag found in all Unrealfiles.
#define RES_FILE_TAG "Unrealfile\x1A" /* 31 characters or less */

// Size of Unreal names.
#define UNREAL_NAME_SIZE 32

// Maximum dependencies per Unrealfile.
#define MAX_DEPENDENCIES 16

// Unrealfile header.
typedef struct unrealheader_s
{
	char Tag[UNREAL_NAME_SIZE];  // Should be RES_FILE_TAG.

	int  UnrealFileVersion;	     // Unreal file format version.
	int  NumExports;			 // Number of objects exported.
	int  NumImports;			 // Number of objects imported.
	int  NumNames;				 // Number of names referenced.

	int  NamesOffset;			 // Offset to name symbol table (FNameEntry array).
	int  ImportsOffset;		     // Offset to object imports    (UObjectBase array).
	int  ExportsOffset;		     // Offset to object exports    (UObjectBase array).
	int  NumDependencies;		 // Number of files this Unrealfile is dependent on.
} unrealheader_t;

// An Unreal name.
typedef struct unrealname_s
{
	char Name[UNREAL_NAME_SIZE]; // Name.
	int  Flags;                  // Flags.
} unrealname_t;

// An Unreal import.
typedef struct unrealimport_s
{
	char Name[UNREAL_NAME_SIZE]; // Name.
	unsigned char Type;          // Type.
} unrealimport_t;

// An Unreal export.
typedef struct unrealexport_s
{
	char Name[UNREAL_NAME_SIZE]; // Name of object.
	unsigned char Type;          // Type of object.
	int  Flags;                  // Object flags.
	int  FileCRC;                // CRC32 of object.
	int  FileHeaderOffset;       // Offset into file of object's header.
	int  FileDataOffset;         // Offset into file of object's data.
	int  FileHeaderSize;         // Size of object's header as stored in file.
	int  FileDataSize;           // Size of object's data as stored in file.
} unrealexport_t;

/*-----------------------------------------------------------------------------
	Unrealfile functions.
-----------------------------------------------------------------------------*/

// Unrealfile thing reading functions.
// All functions return 0 if success, <0 if error.
// All functions assume file is open for reading in binary mode ("rb").
int unrReadByte( FILE *File, unsigned char *b );
int unrReadInt( FILE *File, int *i );
int unrReadFixedLengthString( FILE *File, char *c, int Length );
int unrReadString( FILE *File, char *c, int MaxLength );
int unrReadHeader( FILE *File, unrealheader_t *Header );
int unrReadName( FILE *File, unrealname_t *Name );
int unrReadImport( FILE *File, unrealimport_t *Import );
int unrReadExport( FILE *File, unrealexport_t *Export );

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNRINFO
