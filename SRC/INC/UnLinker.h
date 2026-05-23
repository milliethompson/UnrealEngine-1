/*=============================================================================
	UnLinker.h: Unreal object linker.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNLINK
#define _INC_UNLINK

/*----------------------------------------------------------------------------
	Linker archive types.
----------------------------------------------------------------------------*/

#if 0 /* Ansi file i/o version */

// Binary file archive virtual base.
class FArchiveFile : public FArchive
{
public:
	// Seek to a position in the file.
	virtual void Seek( INT InPos )
	{
		Pos = InPos;
		if( fseek(File,Pos,SEEK_SET) != 0 )
			throw("Error seeking file");
	}
	// Return current position.
	virtual INT Tell()
	{
		return Pos;
	}
	// Close the file.
	virtual void Close()
	{
		if( File != NULL )
			fclose(File);
	}
	// Destructor.
	~FArchiveFile()
	{
		guard(FArchiveFile::~FArchiveFile);
		Close();
		unguard;
	}
	// Name reference serializer.
	FArchive& operator<<( class FName &N )
	{
		appError("FArchiveFile<<FName");
		return *this;
	}
	// Object reference serializer.
	FArchive& operator<<( class UObject *&Res )
	{
		appError("FArchiveFile<<UObject*");
		return *this;
	}
protected:
	// Constructors.
	FArchiveFile( const char *Filename, const char *Mode )
	:	FArchive()
	,	File(NULL)
	,	Pos(0)
	{
		if( stricmp(Mode,"rb") == 0 )
		{
			char NewFilename[256];
			if( GApp->FindFile( Filename, NewFilename ) )
				File = fopen( NewFilename, Mode );	
		}
		else File = fopen( Filename, Mode );
	}
	FArchiveFile()
	{}
	// Variables.
	INT Pos;
	FILE *File;
};

// Archive for saving binary files.
class FArchiveFileSave : public FArchiveFile
{
public:
	// Constructors.
	FArchiveFileSave( const char* Filename )
	:	FArchiveFile( Filename, "wb" )
	{
		guard(FArchiveFileSave::FArchiveFileSave);
		if( File == NULL )
			throwf( "Error opening file %s", Filename );
		unguard;
	}
	FArchiveFileSave()
	{}

	// Byte stream serializers.
	virtual FArchive& Serialize( void *V, int Length )
	{
		Pos += Length;
		if( fwrite( V, 1, Length, File ) != (size_t)Length )
			throw( "Error writing to file" );
		return *this;
	}
};

// Archive for loading binary files.
class FArchiveFileLoad : public FArchiveFile
{
public:
	// Constructors.
	FArchiveFileLoad( const char* Filename )
	:	FArchiveFile( Filename, "rb" )
	{
		guard(FArchiveFileLoad::FArchiveFileLoad);
		if( File == NULL )
			throwf( "Error opening file %s", Filename );
		unguard;
	}
	FArchiveFileLoad()
	{}

	// Byte stream serializer.
	virtual FArchive& Serialize( void *V, int Length )
	{
		Pos += Length;
		if( fread(V,1,Length,File) != (size_t)Length )
			throw( "Error reading from file" );
		return *this;
	}
};

#else /* File mapping version */

// Binary file archive virtual base.
class FArchiveFile : public FArchive
{
public:
	// Seek to a position in the file.
	virtual void Seek( INT InPos )
	{
		Pos = Start + InPos;
	}
	// Return current position.
	virtual INT Tell()
	{
		return Pos - Start;
	}
	// Close the file.
	virtual void Close()
	{
		guard(FArchiveFile::Close);
		GApp->CloseFileMapping( File, MaxSize ? End - Start : -1);
		unguard;
	}
	// Destructor.
	~FArchiveFile()
	{
		guard(FArchiveFile::~FArchiveFile);
		Close();
		unguard;
	}
	// Name reference serializer.
	FArchive& operator<<( class FName &N )
	{
		appError( "FArchiveFile<<FName" );
		return *this;
	}
	// Object reference serializer.
	virtual FArchive& operator<<( class UObject *&Res )
	{
		appError( "FArchiveFile<<UObject*" );
		return *this;
	}
	BOOL Failed()
	{
		return Start == NULL;
	}
protected:
	// Constructors.
	FArchiveFile( const char* Filename, INT InMaxSize )
	{
		MaxSize = InMaxSize;
		Start = Pos = End = NULL;

		char NewFilename[256];
		strcpy(NewFilename,Filename);
		if( InMaxSize!=0 || GApp->FindFile( Filename, NewFilename ) )
			Start = Pos = End = GApp->CreateFileMapping( File, NewFilename, MaxSize );
	}
	FArchiveFile()
	{}
	// Variables.
	BYTE *Start, *Pos, *End;
	FFileMapping File;
	INT MaxSize;
};

// Archive for saving binary files.
class FArchiveFileSave : public FArchiveFile
{
public:
	// Constructors.
	FArchiveFileSave( const char* Filename )
	:	FArchiveFile( Filename, 64 * 1000 * 1000 )
	{
		guard(FArchiveFileSave::FArchiveFileSave);
		if( Failed() )
			throwf( "Error opening file %s", Filename );
		unguard;
	}
	FArchiveFileSave()
	{}

	// Byte stream serializers.
	virtual FArchive& Serialize( void *V, int Length )
	{
		memcpy( Pos, V, Length );
		if( (Pos += Length) > End )
			End = Pos;
		return *this;
	}
};

// Archive for loading binary files.
class FArchiveFileLoad : public FArchiveFile
{
public:
	// Constructors.
	FArchiveFileLoad( const char* Filename )
	:	FArchiveFile( Filename, 0 )
	{
		guard(FArchiveFileLoad::FArchiveFileLoad);
		if( Failed() )
			throwf( "Error opening file %s", Filename );
		unguard;
	}
	FArchiveFileLoad()
	{}

	// Byte stream serializer.
	virtual FArchive& Serialize( void *V, int Length )
	{
		memcpy( V, Pos, Length );
		Pos += Length;
		return *this;
	}
};
#endif

/*----------------------------------------------------------------------------
	Items stored in Unrealfiles.
----------------------------------------------------------------------------*/

// Unrealfile summary, stored at top of file.
struct FUnrealfileSummary
{
	// Constants.
	enum {MAX_DEPENDENCIES = 16};

	// Variables.
	CHAR	Tag[32];			// Should be UNREALFILE_TAG.

	INT		UnrealFileVersion;	// Unreal file format version.
	INT		NumObjects;			// Number of objects.
	INT		NumNames;			// Number of names.

	DWORD	NamesOffset;		// Offset to name symbol table.
	DWORD	ObjectsOffset;		// Offset to object bases.
	DWORD	NumDependencies;	// Number of other Unrealfiles this Unrealfile is dependent on.

	CHAR	Dependencies[MAX_DEPENDENCIES][NAME_SIZE]; // Dependency filenames, no paths.

	// Constructor.
	FUnrealfileSummary()
	{
		memset( this, 0, sizeof(*this) );
	}

	// Add a dependency.
	void AddDependency( char *Name )
	{
		guard(FUnrealfileSummary::AddDependency);
		checkState(Name);
		checkState(Name[0]);
		checkState(strlen(Name)<NAME_SIZE);
		checkState(NumDependencies < MAX_DEPENDENCIES);
		strcpy(Dependencies[NumDependencies++], Name);
		unguard;
	}

	// Serializer.
	friend FArchive& operator<< (FArchive& Ar, FUnrealfileSummary &Sum)
	{
		guard(FUnrealfileSummary<<);

		// Note: Tag is stored as a fixed-length struct unlike most other strings.
		Ar.Serialize( Sum.Tag, sizeof(Sum.Tag) );
		Ar << Sum.UnrealFileVersion << Sum.NumObjects << Sum.NumNames;
		Ar << Sum.NamesOffset << Sum.ObjectsOffset << Sum.NumDependencies;

		for( DWORD i=0; i<Sum.NumDependencies; i++ )
			Ar.String( Sum.Dependencies[i], NAME_SIZE );

		return Ar;
		unguard;
	}
};

/*----------------------------------------------------------------------------
	ULinker.
----------------------------------------------------------------------------*/

//
// Maps object indices and name indices from a remote source (such as a 
// file or a remote server) to local objects and names. Linker maps
// are defined as objects so that we can keep block all of their referenced
// objects and names from garbage collection, even though we may not have
// active references to them elsewhere. This is the case in a network game
// where the player only has active references to objects he sees, and
// there are more objects in the level than he can see.
//
class UNENGINE_API ULinker : public UObject
{
	DECLARE_CLASS(ULinker,UObject,NAME_Linker,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Transient};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constants.
	enum {FNAME_SIZE=128};				// Maximum size of a filename.

	// Variables.
	UArray::Ptr	ResMap;					// Maps file object indices to UObject pointers.
	UEnumDef::Ptr NameMap;				// Maps file name indices to name table indices.
	TArray<ULinker*>::Ptr Dependencies;	// File dependencies.
	INT			Success;				// Whether the object was constructed successfully.
	INT			NumDependencies;		// Number of dependent files.

	// Constructors.
	ULinker( const char *InFilename )
	:	ResMap			(NULL)
	,	NameMap			(NULL)
	,	Success			(123456)
	,	Dependencies	(NULL)
	,	NumDependencies	(0)
	{}

	// Destructor, required since constructors may throw exceptions.
	~ULinker()
	{
		guard(ULinker::~ULinker);
		if( Success == 123456 )
			Kill();
		unguard;
	}

	// UObject interface.
	void InitHeader()
	{
		guard(ULinker::InitHeader);

		// Init UObject info.
		UObject::InitHeader();

		// Init ULinker info.
		ResMap  = (UArray   *)NULL;
		NameMap = (UEnumDef *)NULL;

		unguardobj;
	}
	void PreKill()
	{
		guard(ULinker::PreKill);

		// Kill the object and name maps.
		if( ResMap  ) ResMap->Kill();
		if( NameMap ) NameMap->Kill();

		unguardobj;
	}
	void SerializeHeader(FArchive& Ar)
	{
		guard(ULinker::SerializeHeader);
		//Note: Since this object is not saveable, we only need to
		// serialize the objects and names it references.
		UObject::SerializeHeader(Ar);
		Ar << ResMap << NameMap;
		unguardobj;
	}
};

/*----------------------------------------------------------------------------
	ULinkerLoad.
----------------------------------------------------------------------------*/

class ULinkerLoad : public ULinker, public FArchiveFileLoad
{
	DECLARE_CLASS(ULinkerLoad,ULinker,NAME_LinkerLoad,NAME_UnEngine);

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Transient};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Variables.
	FUnrealfileSummary Summary;
	CHAR Status[256];

	// Constructor.
	ULinkerLoad( const char *Filename )
	:	ULinker				(Filename)
	,	FArchiveFileLoad	(Filename)
	{
		guard(ULinkerLoad::ULinkerLoad);
		sprintf( Status, "Adding file %s...", Filename );

		GApp->StatusUpdate( Status, 0, 0 );
		debug( LOG_Info, Status );

		// Read summary from file.
		guard(LoadSummary);
		*this << Summary;
		unguard;

		// Check tag.
		guard( CheckTag );
		if( strcmp( Summary.Tag, UNREALFILE_TAG ) != 0 )
		{
			char Temp[256];
			sprintf( Temp,
				"The file %s is not an Unreal file.",
				Filename);
			appMessageBox( Temp, "Outdated file version", 0 );
			throw( "Aborted" );
		}
		unguard;

		// Validate the summary.
		guard(CheckVersion);
		if( Summary.UnrealFileVersion < UNREAL_MIN_VERSION )
		{
			char Temp[256];
			sprintf( Temp,
				"The file %s was saved by a previous version which is not "
				"backwards compatible with this one.  Reading it will likely fail, and "
				"may cause Unreal to crash.  Do you want to try anyway?",
				Filename );
			if( !appMessageBox(Temp,"Outdated file version",1) )
				throw( "Aborted" );
		}
		unguard;

		// Set status info.
		ArVer       = Summary.UnrealFileVersion;
		ArIsLoading = 1;
		ArForEdit   = GEditor ? 1 : 0;
		ArForClient = 1;
		ArForServer = 1;

		// Allocate linker tables.
		guard(AllocateTables);
		ResMap	= new(GetName(),CREATE_MakeUnique)UArray  (Summary.NumObjects,1);
		NameMap	= new(GetName(),CREATE_MakeUnique)UEnumDef(Summary.NumNames,  1);
		unguard;

		// Add names from name map (either creates new name, or notes reference to existing one).
		guard(LoadNames);
		if( Summary.NumNames > 0 )
		{
			debugf(LOG_Info,"Reading Names: %i names",Summary.NumNames);
			
			Seek(Summary.NamesOffset);
			for( int i=0; i<Summary.NumNames; i++ )
			{
				// Read the name entry from the file.
				FNameEntry NameEntry;
				*this << NameEntry;

				// Add it to the name table if it's needed in this context.				
				if(	(GUnreal.IsEditor() && (NameEntry.Flags & RF_LoadForEdit  ))
				||	(GUnreal.IsClient() && (NameEntry.Flags & RF_LoadForClient))
				||	(GUnreal.IsServer() && (NameEntry.Flags & RF_LoadForServer)) )
				{
					// Add this name to the table.
					NameMap(i) = FName( NameEntry.Name, FNAME_Add );
				}
				else
				{
					// Not needed in this context, so don't keep it.
					NameMap(i) = NAME_None;
				}
			}
		}
		unguard;
		unguard;
	}

	// Load all objects.
	void LoadAllObjects()
	{
		guard(ULinkerLoad::LoadEverything);

		// Load object bases.
		guard(LoadObjectBases);
		if( Summary.NumObjects > 0 )
		{
			debugf( LOG_Info, "Reading object bases: %i objects", Summary.NumObjects );
			Seek( Summary.ObjectsOffset );

			// Load the exports.
			for( int i=0; i<Summary.NumObjects; i++ )
			{
				if( !(i&7) ) GApp->StatusUpdate( Status, i, Summary.NumObjects );

				// Load from disk.
				UObjectBase Object;
				*this << Object;

				if( Object.FileHeaderOffset == 0 )
				{
					// Importing: look it up and stick it in the object map.
					if( Object.Class!=NULL && Object.Name!=NAME_None )
					{
						ResMap(i) = GObj.FindObject( Object.Name(), Object.Class, FIND_Optional );
						if( !ResMap(i) )
							appErrorf( "Object %s %s doesn't exist", Object.Class->GetName(), Object.Name() );
					}
					else ResMap(i) = NULL;
				}
				else
				{
					// Handle class info preloading.
					UClass *PreloadParentClass      = NULL;
					FName  PreloadPackageName       = NAME_None;
					DWORD  PreloadClassFlags        = 0;
					DWORD  PreloadResThisHeaderSize = 0;
					if( Object.Class == UClass::GetBaseClass() )
					{
						// Preload the class info.
						*this << AR_OBJECT(PreloadParentClass);
						*this << PreloadPackageName << PreloadClassFlags << PreloadResThisHeaderSize;
					}

					// Map the object into our table.
					UObject *Res = NULL;
					if( Object.Class != NULL )
					{
						if
						(	(GUnreal.IsEditor() && (Object.Flags&RF_LoadForEdit  ))
						||	(GUnreal.IsClient() && (Object.Flags&RF_LoadForClient))
						||	(GUnreal.IsServer() && (Object.Flags&RF_LoadForServer)) )
						{
							// We will load the export.
							//debugf("Export %s %s: %08X",Export.GetClassName(),Export.Name(),Export.Flags);
							Res = GObj.FindObject( Object.Name(), Object.Class, FIND_Optional );
							if( !Res )
							{
								// Create new object.
								Res	= (UObject *)appMalloc( Object.Class->ResFullHeaderSize, "Res(%s)", Object.Name() );
								Res->InitObject
								(
									Object.Class,
									INDEX_NONE,
									Object.Name,
									0
								);
								GObj.AddObject( Res );
							}
							else
							{
								// Replace existing object.
								Res->UnloadData();
								Res->PreKill();
								if( !(Res->Flags & RF_HardcodedRes) )
									Res->InitHeader();
							}

							// Copy UObjectBase information from file's object.
							Res->Name               = Object.Name;
							Res->Flags              = (Res->Flags & RF_Keep) | (Object.Flags & RF_Load) | RF_UnlinkedHeader | RF_UnlinkedData;
							Res->Class              = Object.Class;
							Res->FileHeaderOffset   = Object.FileHeaderOffset;
							Res->FileHeaderSize     = Object.FileHeaderSize;
							Res->FileCRC		    = Object.FileCRC;
							Res->FileDataSize       = Object.FileDataSize;
							Res->FileDataOffset     = Object.FileDataOffset;

							// If it's a class, set the class's vtable pointer, and preload the class.
							if( Object.Class == UClass::GetBaseClass() )
							{
								guard(PreloadClass);
								UClass *Class = (UClass*)Res;

								// Set vtable for class resource.
								*(void**)Class = UClass::GetBaseClass()->ResVTablePtr;

								// Set vital properties.
								Class->ParentClass       = PreloadParentClass;
								Class->PackageName       = PreloadPackageName;
								Class->ClassFlags        = PreloadClassFlags;
								Class->ResThisHeaderSize = PreloadResThisHeaderSize;
								Class->ResFullHeaderSize = PreloadResThisHeaderSize;
								if( PreloadParentClass )
									Class->ResFullHeaderSize += PreloadParentClass->ResFullHeaderSize;

								// Find class's vtable pointer.
								Class->SetClassVTable();

								unguard;
							}
						}
					}

					// Add to object map.
					ResMap(i) = Res;
				}
			}
		}
		unguard;

#if LINKER_LOAD_LOG
		// Display resource map for debugging.
		guard(DebugResMap);
		debugf("ULinkerLoad:");
		for( int i=0; i<ResMap->Num; i++ )
			debugf("   %i. %s %s", i, ResMap(i)->GetClassName(), ResMap(i)->GetName() );
		unguard;
#endif

		// Load all headers.
		guard(LoadHeaders);
		for( int i=0; i<Summary.NumObjects; i++ )
		{
			if( (i&15) == 0 )
				GApp->StatusUpdate( "Fabricating", i, Summary.NumObjects );

			UObject *Res = ResMap( i );
			if( Res && (Res->Flags & RF_UnlinkedHeader) && Res->FileHeaderOffset!=0 )
				LoadHeader( Res );
		}
		unguard;

		// Load all data.
		guard(LoadData);
		for( int i=0; i<Summary.NumObjects; i++ )
		{
			if( (i&15) == 0 )
				GApp->StatusUpdate( "Assimilating", i, Summary.NumObjects );

			UObject *Res = ResMap( i );
			if( Res && (Res->Flags & RF_UnlinkedData) && Res->FileHeaderOffset!=0 )
				LoadData( Res, 0 );
		}
		unguard;

		// Call header postloaders.
		guard(PostloadHeaders);
		for( int i=0; i<Summary.NumObjects; i++ )
		{
			UObject *Res = ResMap( i );
			if( Res && Res->FileHeaderOffset!=0 )
				Res->PostLoadHeader( POSTLOAD_File );
		}
		unguard;

		// Call data postloaders.
		guard(PostLoadData);
		for( int i=0; i<Summary.NumObjects; i++ )
		{
			UObject *Res = ResMap( i );
			if( Res && Res->FileHeaderOffset!=0 )
				Res->PostLoadData( POSTLOAD_File );
		}
		unguard;

		// Success.
		Success=1;
		unguardobj;
	}

	// UObject interface.
	void PreKill()
	{
		guard(ULinkerLoad::PreKill);
		ULinker::PreKill();
		Close();
		unguardobj;
	}

	// FArchive interface.
	FArchive& operator<< ( UObject *&Res )
	{
		guard(FArchiveResLoad<<UObject);

		ByteOrderSerialize(&Res,sizeof(Res));
		if( Res != NULL )
		{
			int Index = ((int)Res) - 1;	
			if( !ResMap->IsValidIndex( Index ) )
				appErrorf("Bad object index %i",Index);
			
			Res = ResMap( Index );
		}
		return *this;
		unguardobj;
	}
	FArchive& operator<< ( FName &Name )
	{
		guard(FArchiveResLoad<<Name);

		NAME_INDEX NameIndex;
		ByteOrderSerialize( &NameIndex, sizeof(NAME_INDEX) );
		if( NameIndex == NAME_None )
		{
			Name = NAME_None;
		}
		else
		{
			if( !NameMap->IsValidIndex(NameIndex-1) )
				appErrorf( "Bad name index %i/%i", NameIndex, NameMap->Num );
			
			Name = NameMap( NameIndex-1 );
		}
		return *this;
		unguardobj;
	}
	void PreloadHeader( UObject *Res )
	{
		guard(FArchiveResLoad::PreloadHeader);
		
		if( Res && (Res->GetFlags() & RF_UnlinkedHeader) )
		{
			//debugf("Preloading header: %s %s",Res->GetClassName(),Res->GetName());
			INT SavedPos = Tell();
			LoadHeader(Res);
			Seek(SavedPos);
		}
		unguardobj;
	}
	void Preload( UObject *Res )
	{
		guard(FArchiveResLoad::Preload);
		PreloadHeader(Res);

		if( Res && (Res->GetFlags() & RF_UnlinkedData) )
		{
			//debugf("Preloading data: %s %s",Res->GetClassName(),Res->GetName());
			INT SavedPos = Tell();
			LoadData(Res);
			Seek(SavedPos);
		}
		unguardobj;
	}

	// ULinkerLoad interface.
	void LoadHeader( UObject *Res )
	{
		guard(ULinkerLoad::LoadHeader);
		//debugf( "Loading %s %s header...", Res->GetClassName(), Res->GetName() );

		// Mark it unlinked before doing anything, so that 
		// PreloadHeader/Preload calls don't recurse forever.
		Res->ClearFlags(RF_UnlinkedHeader);

		// Link it.
		Seek( Res->FileHeaderOffset );
		Res->SerializeHeader( *this );

		// Make sure we serialized the right amount of stuff.
		DWORD Got = Tell() - Res->FileHeaderOffset;
		if( Got != Res->FileHeaderSize )
			appErrorf
			(
				"%s %s: Header serialization size mismatch: Got %i, Expected %i",
				Res->GetClassName(),
				Res->GetName(),
				Got,
				Res->FileHeaderSize
			);
		unguardobj;
	}
	void LoadData( UObject *Res, int DoPostLoad=1 )
	{
		guard(ULinkerLoad::LoadData);

		// Load data from the file.
		Res->Realloc();
		Seek(Res->FileDataOffset);
		Res->SerializeData(*this);

		// Make sure we serialized the right amount of stuff.
		DWORD Got = Tell() - Res->FileDataOffset;
		if( Got != Res->FileDataSize )
			appErrorf
			(
				"%s %s: Data serialization size mismatch: Got %i, Expected %i",
				Res->GetClassName(),
				Res->GetName(),
				Got,
				Res->FileDataSize
			);

		// Mark data as linked.
		Res->ClearFlags(RF_UnlinkedData);

		if( DoPostLoad ) Res->PostLoadData(POSTLOAD_File);

		unguardobj;
	}
};

/*----------------------------------------------------------------------------
	ULinkerSave.
----------------------------------------------------------------------------*/

class ULinkerSave : public ULinker, public FArchiveFileSave
{
	DECLARE_CLASS(ULinkerSave,ULinker,NAME_LinkerSave,NAME_UnEngine);

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Transient};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructor.
	ULinkerSave( const char *Filename, FUnrealfileSummary &Summary )
	:	ULinker(Filename)
	,	FArchiveFileSave(Filename)
	{
		ResMap	= new(GetName(),CREATE_Unique)UArray  ( Summary.NumObjects, 1 );
		NameMap	= new(GetName(),CREATE_Unique)UEnumDef( Summary.NumNames,   1 );

		// Set status info.
		ArIsSaving  = 1;
		ArForEdit   = GEditor ? 1 : 0;
		ArForClient = 1;
		ArForServer = 1;

		// Success.
		Success=1;
	}

	// UObject interface.
	void PreKill()
	{
		guard(ULinkerSave::PreKill);
		ULinker::PreKill();
		Close();
		unguardobj;
	}

	// FArchive interface.
	FArchive& operator<<( UObject *&Res )
	{
		guard(FArchiveSaveDelink<<Obj);
		if( Res && !(Res->GetClassFlags() & CLASS_Transient) )
		{
			for( INDEX i=0; i<ResMap->Num; i++ )
			{
				if( ResMap(i) == Res )
				{
					DWORD Save = i+1;
					return *this << Save;
				}
			}
			appErrorf("%s %s not found",Res->GetClassName(),Res->GetName());
		}
		DWORD Save=0;
		return *this << Save;
		unguardobj;
	}
	FArchive& operator<<( FName &Name )
	{
		guard(FArchiveSaveDelink<<Name);
		if( Name != NAME_None )
		{
			for( INDEX i=0; i < NameMap->Num; i++ )
			{
				if( NameMap(i) == Name )
				{
					NAME_INDEX Save = i+1;
					return *this << Save;
				}
			}
			appErrorf( "Name %i (%s) not found (of %i)",Name.GetIndex(),Name(),NameMap->Num);
		}
		NAME_INDEX Save=0;
		return *this << Save;
		unguardobj;
	}
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNLINK
