/*=============================================================================
	UnName.h: Unreal global name types.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNNAME // Prevent header from being included multiple times
#define _INC_UNNAME

/*----------------------------------------------------------------------------
	Definitions.
----------------------------------------------------------------------------*/

// Maximum size of name in characters.
enum {NAME_SIZE	= 32};

// Name index.
typedef WORD NAME_INDEX;

// Enumeration for finding name.
enum EFindName
{
	FNAME_Find,		// Find a name; return 0 if it doesn't exist.
	FNAME_Add,		// Find a name or add it if it doesn't exist.
};

/*----------------------------------------------------------------------------
	FNameEntry.
----------------------------------------------------------------------------*/

//
// A global name, as stored on-disk in an Unrealfile.
// Private name table entry, tracked by Unres.
//
struct FNameEntry
{
	// Variables.
	NAME_INDEX	Index;				// Index of name in hash.
	DWORD		Flags;				// RF_TagImport, RF_TagExport, RF_HardcodedName.
	FNameEntry	*HashNext;			// Pointer to the next entry in this hash bin's linked list.

	// The name string.
	char		Name[NAME_SIZE];	// Name, variable-sized.

	// Functions.
	friend FArchive& operator<< (FArchive &Ar, FNameEntry &E)
	{
		guard(FNameEntry<<);
		Ar.String( E.Name, NAME_SIZE );
		return Ar << E.Flags;
		unguard;
	}
	friend FNameEntry *AllocateNameEntry( const char *Name, DWORD Index, DWORD Flags, FNameEntry *HashNext )
	{
		guard(AllocateNameEntry);

		FNameEntry *NameEntry = (FNameEntry*)malloc( sizeof(FNameEntry) + strlen(Name) + 1 - NAME_SIZE );
		NameEntry->Index      = Index;
		NameEntry->Flags      = Flags;
		NameEntry->HashNext   = HashNext;
		strcpy( NameEntry->Name, Name );
		return NameEntry;

		unguard;
	}
};

//
// Variable length name entry; must mirror FNameEntry.
//
struct FNameEntryV
{
	DWORD		Index;
	DWORD		Flags;
	FNameEntry	*HashNext;
	CHAR		Name[];
};

/*----------------------------------------------------------------------------
	Name autoregistry.
----------------------------------------------------------------------------*/

// Automatically register a name.
#define AUTOREGISTER_NAME( num, namestr, flags ) \
	FNameEntryV namestr##NAME = { num, flags, NULL, #namestr }; \
	BYTE namestr##AUTONAME = GAutoregisterName( namestr##NAME );

/*----------------------------------------------------------------------------
	FName.
----------------------------------------------------------------------------*/

//
// Public name, available to the world.  Names are stored as WORD indices
// into the name table and every name in Unreal is stored once
// and only once in that table.  Names are case-insensitive.
//
class UNENGINE_API FName 
{
	// Friends.
	friend BYTE UNENGINE_API GAutoregisterName( FNameEntryV &AutoName );

public:
	// Accessors.
	const char *operator() () const
	{
		debugInput(Index < MaxNames);
		debugInput(Names[Index]!=NULL);
		return Names[Index]->Name;
	}
	NAME_INDEX GetIndex() const
	{
		debugInput(Index < MaxNames);
		debugInput(Names[Index]!=NULL);
		return Index;
	}
	DWORD GetFlags() const
	{
		debugInput(Index < MaxNames);
		debugInput(Names[Index]!=NULL);
		return Names[Index]->Flags;
	}
	void SetFlags( DWORD Set )
	{
		debugInput(Index < MaxNames);
		debugInput(Names[Index]!=NULL);
		Names[Index]->Flags |= Set;
	}
	void ClearFlags( DWORD Clear )
	{
		debugInput(Index < MaxNames);
		debugInput(Names[Index]!=NULL);
		Names[Index]->Flags &= ~Clear;
	}
	int operator==( const FName &Other ) const
	{
		return Index==Other.Index;
	}
	int operator!=( const FName &Other ) const
	{
		return Index!=Other.Index;
	}
	static FName MakeNameFromIndex( NAME_INDEX InIndex )
	{
		if( InIndex>=MaxNames || Names[InIndex]==NULL )
			return FName( NAME_None );
		else
			return FName( (EName)InIndex );
	}

	// Constructors.
	FName()
	{
	}
	FName( enum EName N )
	{
		Index = N;
	}
	FName( enum EPropertyType T )
	{
		Index = T;
	}
	FName( const char *Name, EFindName FindType );

	// Name subsystem.
	static void InitSubsystem();
	static void ExitSubsystem();

	// Name subsystem accessors.
	static int GetMaxNames()           {return MaxNames;}
	static FNameEntry *GetEntry(int i) {return Names[i];}
	static void DeleteEntry( int i );
	static void DisplayHash( FOutputDevice *Out );

private:
	// Instance variables.
	NAME_INDEX			Index;			 // Index of this name.

	// Static variables.
	static INDEX		MaxNames; 		 // Maximum names in global name table.
	static FNameEntry	**Names;   		 // Global table of names.
	static FNameEntry   *NameHash[1024]; // Hashed names.
	static INT          InitedHash;		 // Whether hash has been initialized.
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNNAME
