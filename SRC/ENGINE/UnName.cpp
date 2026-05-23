/*=============================================================================
	UnName.cpp: Unreal global name code.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	FName statics.
-----------------------------------------------------------------------------*/

INT          FName::InitedHash      = 0;
INDEX		 FName::MaxNames        = 0;
FNameEntry** FName::Names           = NULL;
FNameEntry*  FName::NameHash[1024];

/*-----------------------------------------------------------------------------
	FName implementation.
-----------------------------------------------------------------------------*/

//
// FName constructor.
//
FName::FName( const char *Name, EFindName FindType )
{
	guard(FName::FName);
	checkInput(Name!=NULL);

	// If empty name was specified, return NAME_None.
	if( Name[0]==0 || stricmp(Name,"NONE")==0 )
	{
		Index = NAME_None;
		return;
	}

	// Compute hash.
	INT iHash = strihash(Name) & (ARRAY_COUNT(NameHash)-1);

	// Try to find the name in the hash.
	for( FNameEntry *Hash=NameHash[iHash]; Hash; Hash=Hash->HashNext )
	{
		if( stricmp( Name, Hash->Name )==0 )
		{
			Index = Hash->Index;
			return;
		}
	}

	// Didn't find name.
	if( FindType == FNAME_Find )
	{
		// Not found.
		Index = NAME_None;
	}
	else if( FindType == FNAME_Add )
	{
		// Validate the name.
		if( strlen(Name) >= NAME_SIZE )
			appErrorf( "Name '%s' is too big", Name );

		// Find an available entry in the name table.
		for( Index=0; Index<MaxNames; Index++ )
			if( Names[Index]== NULL )
				break;

		// Expand the name table if necessary.
		if( Index >= MaxNames )
		{
			Index     = MaxNames;
			MaxNames += MaxNames/4;
	        Names     = (FNameEntry **)appRealloc( Names, MaxNames*sizeof(FNameEntry*), "Names" );
			for( INDEX i=Index; i<MaxNames; i++ )
				Names[i] = NULL;
		}

		// Allocate the name and set it.
		NameHash[iHash] = Names[Index] = AllocateNameEntry( Name, Index, 0, NameHash[iHash] );
	}
	else appError("FName: Bad find type");
	unguard;
}

/*-----------------------------------------------------------------------------
	FName subsystem.
-----------------------------------------------------------------------------*/

//
// Initialize the name subsystem.
//
void FName::InitSubsystem()
{
	guard(FName::InitSubsystem);

	// Check that hash was initialized by Autoregister.
	checkState(InitedHash);
	checkState((ARRAY_COUNT(NameHash)&(ARRAY_COUNT(NameHash)-1)) == 0);

	// Alloc table.
	MaxNames   = 1024;
	Names	   = appMallocArray( MaxNames, FNameEntry*, "Names" );

	// Init names.
	for( int i=0; i<MaxNames; i++ ) 
		Names[i] = NULL;

	// Add all hardcoded names to the list.
	int AddedNames=0;
	for( i=0; i<ARRAY_COUNT(NameHash); i++ )
	{
		for( FNameEntry *Hash=NameHash[i]; Hash; Hash=Hash->HashNext )
		{
			// Validate everything.
			checkState(Hash->Flags & RF_HardcodedName);
			checkState(Hash->Name[0]!=0);
			checkState(Hash->Index<(DWORD)MaxNames);
			if( Names[Hash->Index]!=NULL )
				appErrorf( "Hardcoded name %i duplicated", Hash->Index );
			for( FNameEntry *Other=NameHash[i]; Other!=Hash; Other=Other->HashNext )
				if( stricmp(Other->Name,Hash->Name)==0 )
					appErrorf( "Hardcoded name '%s' registered twice", Other->Name );

			// Add to name table.
			Names[Hash->Index] = Hash;
			AddedNames++;
		}
	}
	debugf( LOG_Init, "Name subsystem initialized: %i names", AddedNames );
	unguard;
}

//
// Shut down the name subsystem.
//
void FName::ExitSubsystem()
{
	guard(FName::ExitSubsystem);

	// Kill all names.
	for( int i=0; i<MaxNames; i++ )
		if( Names[i] && !(Names[i]->Flags & RF_HardcodedName) )
			delete Names[i];
	appFree( Names );

	debugf( LOG_Init, "Name subsystem shut down" );
	unguard;
}

//
// Display the contents of the global name hash.
//
void FName::DisplayHash( FOutputDevice *Out )
{
	guard(FName::DisplayHash);

	int UsedBins=0, NameCount=0;
	for( int i=0; i<ARRAY_COUNT(NameHash); i++ )
	{
		if( NameHash[i] != NULL ) UsedBins++;
		for( FNameEntry *Hash = NameHash[i]; Hash; Hash=Hash->HashNext )
			NameCount++;
	}
	Out->Logf( "Hash: %i names, %i/%i hash bins", NameCount, UsedBins, ARRAY_COUNT(NameHash) );

	unguard;
}

//
// Delete an name permanently; called by garbage collector.
//
void FName::DeleteEntry( int i )
{
	guard(FName::DeleteEntry);

	// Unhash it.
	FNameEntry *Name = Names[i];
	checkState(Name!=NULL);

	int iHash = strihash(Name->Name) & (ARRAY_COUNT(NameHash)-1);
	for( FNameEntry **HashLink=&NameHash[iHash]; *HashLink && *HashLink!=Name; HashLink=&(*HashLink)->HashNext );
	if( !*HashLink )
		appErrorf( "Unhashed name '%s'", Name->Name );
	*HashLink = (*HashLink)->HashNext;

	// Remove it from the global name table.
	Names[i] = NULL;

	// Delete it.
	delete Name;

	unguard;
}

/*-----------------------------------------------------------------------------
	Name autoregistry.
-----------------------------------------------------------------------------*/

//
// Automatically register a name.
//warning: Called at library initialization; can't guard. Be very careful.
//
UNENGINE_API BYTE GAutoregisterName( FNameEntryV &AutoName )
{
	// Initialize the name hash if needed.
	if( !FName::InitedHash )
	{
		FName::InitedHash = 1;
		for( int i=0; i<ARRAY_COUNT(FName::NameHash); i++ )
			FName::NameHash[i] = NULL;
	}

	// Add name to name hash.
	int iHash              = strihash(AutoName.Name) & (ARRAY_COUNT(FName::NameHash)-1);
	AutoName.HashNext      = FName::NameHash[iHash];
	FName::NameHash[iHash] = (FNameEntry*)&AutoName;

	// Return bogus value.
	return 0;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
