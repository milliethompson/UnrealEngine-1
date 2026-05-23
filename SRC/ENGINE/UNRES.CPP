/*=============================================================================
	UnRes.cpp: Unreal object manager.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnLinker.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

// Print debugging info.
#define GARBAGE_LOG      0 /* Log garbage collection         */
#define KILL_LOG         0 /* Log object kills               */
#define REPLACE_LOG      0 /* Log replacing existing objects */
#define REALLOC_LOG      0 /* Log object reallocs            */
#define SAVE_EXPORT_LOG  0 /* Log exports when saving        */
#define LINKER_LOAD_LOG  0 /* Log loaded resources           */

/*-----------------------------------------------------------------------------
	Statics.
-----------------------------------------------------------------------------*/

UClass *FGlobalObjectManager::AutoRegister=NULL;

/*-----------------------------------------------------------------------------
	UObjectBase implementation.
-----------------------------------------------------------------------------*/

//
// UObjectBase serializer.
//
inline FArchive& operator<<( FArchive& Ar, UObjectBase &U )
{
	guard(UObjectBase<<);
	Ar << U.Name;

	// Serialize class by its name.
	FName ClassName;
	if( !Ar.IsLoading() )
		ClassName = U.Class->GetFName();
	Ar << ClassName;
	if( Ar.IsLoading() )
		U.Class = ClassName!=NAME_None ? new(ClassName(),FIND_Existing)UClass : NULL;

	// Serialize flags.
	Ar << U.Flags;
	Ar << U.FileCRC;
	Ar << U.FileHeaderOffset << U.FileDataOffset;
	Ar << U.FileHeaderSize   << U.FileDataSize;
	return Ar;
	unguard;
}

/*-----------------------------------------------------------------------------
	UObject implementation.
-----------------------------------------------------------------------------*/

//
// All base functionality provided for UObject-derived classes.
//

//
// Initialize the generic information in an object.  Internal only.
//
void UObject::InitObject( UClass *InClass, INDEX InIndex, FName InName, DWORD InFlags )
{
	guard(UObject::InitObject);
	checkState(this!=NULL);

	// Init the virtual function table pointer.
	guard(InitVTable);
	if( InClass )
		*(void **)this = InClass->ResVTablePtr;
	checkState(*(INT*)this!=0);
	unguard;

	// Set the standard properties.
	guard(InitProperties);
	Index           = InIndex;
	Class			= InClass ? InClass : UClass::GetBaseClass();
	Linker			= NULL;
	Data			= NULL;
	Flags			= InFlags;
	FileDataSize	= 0;
	Name            = InName;
	unguard;

	// Init locks.
	guard(CallInitLocks);
	InitLocks();
	unguard;

	// Call the standard header initialization.
	guard(CallInitHeader);
	InitHeader();
	unguard;

	unguardf(( "(%s %s)", InClass ? InClass->GetName() : "(null)", InName!=NAME_None ? InName() : "(none)" ));
}

//
// Set the resource's private pointers.
//
void UObject::SetBinPointers()
{
	guard(UObject::SetBinPointers);

	// Per-object bin.
	MainStack.Bins[PROPBIN_PerObject]    = (BYTE *)this;

	// Per-function bin.
	MainStack.Bins[PROPBIN_PerFunction] = NULL;

	// Per-class bin.
	UBuffer *ClassBin = GetClass()->Bins[PROPBIN_PerClass];
	MainStack.Bins[PROPBIN_PerClass] = ClassBin ? &ClassBin->Element(0) : NULL;

	unguard;
}

//
// Initialize all important parameters in the object's header.  All
// child classes that have meaningful information in their headers must override
// this.  When this function is called, you cannot assume that the object's
// data is valid or has been allocated.
//
// This specifically does not affect:
//	Type
//	Index
//	Flags
//	Name
//	FileDataOffset, FileDataSize, FileCRC
//
void UObject::InitHeader()
{
	guard(UObject::InitHeader);

	// Init basic info.
	Linker		= NULL; // Linker is not persistent.
	Data		= NULL;

	// Init file info.
	Linker		= NULL;

	// Init locks.
	InitLocks();

	// Init execution stack.
	MainStack = FExecStackMain();

	unguardobj;
}

//
// Import this object from a buffer.  This must be overridden by
// all objects which are capable of imporing.  The type of data being
// imported may be determined from *FileType, which represents the import
// file's extension (such as "PCX").
//
// Buffer is a pointer to the data being imported, and BufferEnd is a pointer
// to the end of the buffer.  The import function must return a pointer to
// the end of the data it parsed.  This is meaningless when importing a simple
// file like a PCX, but is important when importing multiple objects from a 
// complex Unreal text file.
//
// If importing from a text file, you are guaranteed that the buffer's last valid
// character will be a zero, so that you may disregard BufferEnd.
//
const char *UObject::Import(const char *Buffer, const char *BufferEnd,const char *FileType)
{
	// Default implementation should never be called.
	appError("UObject::Import called");
	return NULL;
}

//
// Export an object to a buffer.  This must be overridden by
// all objects which are capable of exporting.  The type
// of data to be exported (text, binary, file format, etc) may
// be determined from the FileType string, which represents the
// file's extension, such as "PCX" for a PCX file.
//
// Buffer is a pointer to the data buffer to begin storing the
// data; the Export function must return a pointer to the end of
// the data it has written; if it exports 10 bytes, for example,
// it must return (Buffer+10).  If the export function fails,
// it should return NULL, indicating that the data should not be
// saved.
//
// Indent is an optional parameter that objects may use in order
// to properly format data exported to Unreal text files.
//
void UObject::Export( FOutputDevice &Out, const char *FileType, int Indent )
{
	// Default implementation should never be called.
	appError("UObject::Export called");
}

//
// Return the full size of this object's data.  All resources
// which contain data must override this function.
//
// An object must be capable of computing its size
// using only the information in its header, as this function
// is called during load/save before the object's data has
// been loaded.  This is also called during allocation of
// the object, where the caller (or the object) must set
// size information in its header prior to allocating data.
//
int UObject::QuerySize()
{
	// Default implementation contains no data.
	return 0;
}

//
// Return the minimal size of this object's data.  This represents
// the number of contiguous bytes in the object's data which
// are valid.  This may be smaller than the value returned by
// QuerySize() if the object contains a variable number of
// records, and not all of the records are full.
//
// An object must be capable of computing its minimal size
// using only the information in its header, as this function
// is called during load/save before the object's data has
// been loaded.
//
int UObject::QueryMinSize()
{
	guard(UObject::QueryMinSize);

	// Default implementation assumes no slack.
	return QuerySize();

	unguardobj;
}

//
// Do any object-specific cleanup required in the header 
// immediately after loading an object, and immediately
// after any undo/redo.
//
void UObject::PostLoadHeader( DWORD PostFlags )
{
	guard(UObject::PostLoadHeader);

	// Init non-persistent information.
	InitLocks();

	// Set up property bins.
	SetBinPointers();

	// Init execution caches.
	MainStack.InitVfCache();

	// Clear all non-persistent flags.
	ClearFlags( RF_TransHeader );

	unguard;
}

//
// Do any object-specific cleanup required immediately
// after loading an object and immediately after any 
// undo/redo.
//
void UObject::PostLoadData( DWORD PostFlags )
{
	// Default implementation does nothing.
}

//
// Do any object-specific cleanup required immediately
// before an object is killed.  Child classes may override
// this if they have to do anything here.
//
void UObject::PreKill()
{
	// Default implementation does nothing.
}

//
// Export this object to a file.  Child classes do not
// override this, but they do provide an Export() function
// to do the resoource-specific export work.  Returns 1
// if success, 0 if failed.
//
int	UObject::ExportToFile( const char *Filename )
{
	guard(UObject::ExportToFile);

	FILE *File = fopen(Filename,"wb");
	if( !File )
	{
		debugf( LOG_Info,"Couldn't open %s",Filename );
		return 0;
	}
	GApp->StatusUpdatef( "Exporting %s", 0, 0, GetName() );

	// Allocate and export the data.
	UBuffer::Ptr Buffer = new("Export",CREATE_Unique)UBuffer(0);
	Export( *Buffer, fext(Filename), 0 );

	// Write all stuff.
	Buffer->Lock(LOCK_Read);
	int Success = Buffer->Num && fwrite(&Buffer(0),Buffer->Num,1,File)==1;
	Buffer->Unlock(LOCK_Read);
	Buffer->Kill();
	fclose(File);

	// Kill file if error.
	if( !Success )
		_unlink(Filename);

	return Success;
	unguardobj;
}

//
// Import this object from a file.  Child classes do
// not override this function, but they do provide an
// Import() function to handle object-specific data
// importing if they are importable.  Returns 1 if success,
// 0 if the import failed.
//
int	UObject::ImportFromFile(const char *Filename)
{
	guard(UObject::ImportFromFile);

	// Get file size and allocate import buffer memory.
	GApp->StatusUpdate ("Reading file",0,0);
	int BufferLength = fsize(Filename);
	if( BufferLength <= 0 )
	{
		debugf(LOG_Info,"Couldn't get size of %s",Filename);
		return 0; // Bad file.
	}
	char Descr[80]; sprintf( Descr, "Import(%s)", GetName() );
	char *Buffer = appMallocArray(BufferLength+1,char,Descr);

	// Open for reading.
	FILE *File=fopen(Filename,"rb");
	if( File==NULL )
	{
		appFree (Buffer);
		debugf( LOG_Info, "Couldn't open %s",Filename );
		return 0;
	}

	// Read all file data stuff.
	if( fread (Buffer,BufferLength,1,File)!=1 )
	{
		fclose  (File);
		appFree (Buffer);
		debugf  ( LOG_Info, "Couldn't read %s",Filename );
		return 0;
	}
	fclose (File);

	// In case importing text, make sure it's null-terminated.
	Buffer[BufferLength]=0;

	// Handle object specific import.
	if (!Import(Buffer,Buffer+BufferLength,fext(Filename)))
	{
		// Failed.
		appFree (Buffer);
		return 0;
	}

	// Success.
	appFree (Buffer);
	return 1; 

	unguardobj;
}

//
// Unload this object's data.  Not overridden by child classes.
//
void UObject::UnloadData()
{
	guard(UObject::UnloadData);
	if( Data && !(Flags & RF_NoFreeData) )
   	{
	   	appFree(Data);
		Data = NULL;
	}
	unguardobj;
}

//
// Kill this object by freeing its data, freeing
// its header, and removing its entry from the global
// object table.  Child classes do not override this function,
// but they can have a PreKill() routine to do any object-specific
// cleanup required before killing.
//
void UObject::Kill()
{
	guard(UObject::Kill);
	checkState(!IsLocked());
#if KILL_LOG
	debugf( "Killing %s %s", GetClassName(), GetName() );
#endif

	if( !(Flags & RF_HardcodedRes) )
	{
		INDEX ThisIndex = Index;
		UnloadData();
		PreKill();
		appFree(this);
		GObj.ResArray[ThisIndex] = NULL;
	}
	unguard;
}

//
// Reallocate this object's data to a presumably
// new size, keeping the existing contents intact.
// Child classes do not override this.
//
void *UObject::Realloc()
{
	guard(UObject::Realloc);
#if REALLOC_LOG
	debugf("Realloc %s %s to %i",GetClassName(),GetName(),QuerySize());
#endif

	// Mark header as modified.
	ModifyHeader();

	// Reallocate it.
	Data = appRealloc(Data,QuerySize(),"AllocData(%s)",GetName());
	return Data;

	unguardobj;
}

//
// Archive for computing CRC's. Two objects are considered identical
// on the client and server sides if their name, type, full CRC, and 
// lengths are all identical.
//
// The CRC value is invariant of local machine pointer addresses,
// byte order, and linker mappings as long as object implementors
// follow the rules and don't try to serialize inappropriate stuff.
//
class FArchiveCRC : public FArchive
{
public:
	// Constructor.
	FArchiveCRC()
	{
		CRC = 0xFFFFFFFF;
	}
	// Accessors.
	DWORD GetCRC()
	{
		return ~CRC;
	}
	// Serializer.
	FArchive& Serialize( void *V, int Length )
	{
		BYTE *Data = (BYTE *)V;
		for( int i=0; i<Length; i++ )
			CRC = ((CRC >> 8) & 0x00FFFFFF) ^ GCRCTable[(CRC ^ Data[i]) & 0xFF];
		return *this;
	}
	FArchive& operator<< (class FName &N)
	{
		if( N != NAME_None )
		{
			char Name[NAME_SIZE];
			strcpy(Name,N());
			_strupr(Name);
			Serialize(Name,strlen(Name));
		}
		return *this;
	}
	FArchive& operator<< (class UObject *&Res)
	{
		if( Res )
		{
			char Name[NAME_SIZE];
			strcpy(Name,Res->GetName());
			_strupr(Name);
			Serialize(Name,strlen(Name));
		}
		return *this;
	}
protected:
	DWORD CRC;
};

//
// Compute the pseudo-CRC32 of the object's header and
// data combined. Child classes do not override this function.
//
DWORD UObject::FullCRC()
{
	guard(UObject::FullCRC);

	FArchiveCRC Ar;
	SerializeHeader(Ar);
	SerializeData(Ar);

	return Ar.GetCRC();
	unguardobj;
}

//
// Compute the pseudo-CRC32 of the object's data only.
//
DWORD UObject::DataCRC()
{
	guard(UObject::DataCRC);

	FArchiveCRC Ar;
	SerializeData(Ar);

	return Ar.GetCRC();
	unguardobj;
}

//
// Note that the object header has been modified.
//
void UObject::ModifyHeader()
{
	guard(UObject::ModifyHeader);

	// Perform transaction tracking.
	if( IsTransLocked() )
		GTrans->NoteResHeader(this);

	unguardobj;
}

//
// Note that the object data has been modified.
//
void UObject::ModifyAllItems()
{
	guard(UObject::ModifyAllItems);
	unguardobj;
}

//
// Lock the object.
//
INT UObject::Lock( DWORD NewLockType )
{
	guard(UObject::Lock);
	checkState(NewLockType & LOCK_Read);

	if( (NewLockType & LOCK_Read)==LOCK_Read )
	{
		// Lock for reading.
		ReadLocks++;
	}

	if( (NewLockType & LOCK_Trans)==LOCK_Trans )
	{
		// Lock transactionally.
		checkState(GTrans != NULL);
		TransLocks++;
	}

	if( (NewLockType & LOCK_ReadWrite)==LOCK_ReadWrite )
	{
		// Lock for writing and save header if transactional.
		WriteLocks++;
		ModifyHeader();
	}

	return 1;
	unguardobj;
}

//
// Unlock the object.
//
void UObject::Unlock( DWORD OldLockType )
{
	guard(UObject::Unlock);
	checkState(OldLockType & LOCK_Read);

	if( (OldLockType & LOCK_Trans)==LOCK_Trans )
	{
		TransLocks--;
		checkState(TransLocks>=0);
	}

	if( (OldLockType & LOCK_ReadWrite)==LOCK_ReadWrite )
	{
		WriteLocks--;
		checkState(WriteLocks>=0);
	}

	if( (OldLockType & LOCK_Read)==LOCK_Read )
	{
		ReadLocks--;
		checkState(ReadLocks>=0);
	}
	unguardobj;
}

//
// Save an entire object for later restoration.
//
void UObject::Push( FSavedObject &Saved, FMemStack &Mem )
{
	guard(UObject::Push);
	Saved.MemMark.Push(Mem);

	// Remember sizes.
	Saved.SavedHeaderSize = GetClass()->ResHeaderSize;
	Saved.SavedDataSize   = QueryMinSize();

	// Allocate stack memory.
	Saved.SavedHeader     = new(Mem,Saved.SavedHeaderSize)BYTE;
	Saved.SavedData		  = new(Mem,Saved.SavedDataSize  )BYTE;

	// Copy memory.
	memcpy(Saved.SavedHeader, this,      Saved.SavedHeaderSize);
	memcpy(Saved.SavedData,   GetData(), Saved.SavedDataSize);

	unguardobj;
}

//
// Restore an entire object.
//
void UObject::Pop( FSavedObject &Saved )
{
	guard(UObject::Pop);

	// Remember the data pointer.
	void *RememberedData = GetData();

	// Restore the header.
	memcpy(this, Saved.SavedHeader, Saved.SavedHeaderSize);

	// Restore the old data pointer and reallocate.
	SetData(RememberedData);
	Realloc();

	// Restore the data.
	memcpy(GetData(), Saved.SavedData, Saved.SavedDataSize);

	Saved.MemMark.Pop();
	unguardobj;
}

//
// UObject header serializer.
//
void UObject::SerializeHeader( FArchive &Ar )
{
	guard(UObject::SerializeHeader);

	// Make sure this object's class's data is loaded and linked, because
	// we may need it while we're serializing.
	if( Class != UClass::GetBaseClass() )
		for( UClass *LoadClass=Class; LoadClass; LoadClass=LoadClass->ParentClass )
			Ar.Preload( LoadClass );

	// If not loading or saving, we might be tagging garbage, so serialize our name and class.
	if( !Ar.IsLoading() && !Ar.IsSaving() )
		Ar << Name << Class;

	// Serialize the execution stack.
	Ar << MainStack;

	// Serialize object properties which are defined in the class.
	if( Class != UClass::GetBaseClass() )
		GetClass()->SerializeBin( Ar, PROPBIN_PerObject, this, 1 );

	unguardobj;
}

//
// UObject data serializer.
//
void UObject::SerializeData( FArchive &Ar )
{
	guard(UObject::SerializeData);
	// Nothing to do.
	unguardobj;
}

IMPLEMENT_CLASS(UObject);

/*-----------------------------------------------------------------------------
	UObject IUnknown implementation.
-----------------------------------------------------------------------------*/

// Note: This are included as part of UObject in case we ever want
// to make Unreal objects into Component Object Model objects.
// The Unreal framework is set up so that it would be easy to componentize
// everything, but there is not yet any benefit to doing so, thus these
// functions aren't implemented.

//
// Query the object on behalf of an Ole client.
//
DWORD STDCALL UObject::QueryInterface( const FGUID &RefIID, void **InterfacePtr )
{
	guard(UObject::QueryInterface);
	// This is not implemented and might not ever be.
	*InterfacePtr = NULL;
	return 0;
	unguardobj;
}

//
// Add a reference to the object on behalf of an Ole client.
//
DWORD STDCALL UObject::AddRef()
{
	guard(UObject::AddRef);
	// This is not implemented and might not ever be.
	return 0;
	unguardobj;
}

//
// Release the object on behalf of an Ole client.
//
DWORD STDCALL UObject::Release()
{
	guard(UObject::Release);
	// This is not implemented and might not ever be.
	return 0;
	unguardobj;
}

/*-----------------------------------------------------------------------------
	UDatabase implementation.
-----------------------------------------------------------------------------*/

//
// UObject interface.
//
int UDatabase::QuerySize()
{
	guard(UDatabase::QuerySize);
	return Max * GetClass()->ResRecordSize;
	unguardobj;
}
int UDatabase::QueryMinSize()
{
	guard(UDatabase::QueryMinSize);
	return Num * GetClass()->ResRecordSize;
	unguardobj;
}
void UDatabase::InitHeader()
{
	guard(UDatabase::InitHeader);

	// Call parent class.
	UObject::InitHeader();

	// Init database info.
	Num = 0;
	Max = 0;

	unguardobj;
}

// UDatabase interface.
void UDatabase::PostLoadData( DWORD PostFlags )
{
	guard(UDatabase::PostLoadData);
	for( int i=0; i<Num; i++ )
		PostLoadItem( i, PostFlags );
	unguardobj;
}
void UDatabase::PostLoadItem( int Index, DWORD PostFlags )
{
	// Default implementation does nothing.
}
void UDatabase::Empty()
{
	guard(UDatabase::Empty);
	Num = Max = 0;
	Realloc();
	unguardobj;
}
void UDatabase::Shrink()
{
	guard(UDatabase::Shrink);
	Max = Num;
	Realloc();
	unguardobj;
}
int UDatabase::Add(int NumToAdd)
{
	guard(UDatabase::Add);

	// Mark the database modified to transactionally save Num.
	ModifyHeader();

	int Result = Num;
	if( Num + NumToAdd > Max )
	{
		// Reallocate.
		Max += NumToAdd + 256 + (Num/4);
		Realloc();
	}
	Num += NumToAdd;
	return Result;

	unguardobj;
}
void UDatabase::Remove( int Index, int Count )
{
	guard(UDatabase::Remove);
	checkInput(Index>=0 && Index<=Max); \
	
	// Note that Num is being modified.
	ModifyHeader();

	// Note that all items were modified.
	for( int i=Index+Count; i<Max; i++ )
		ModifyItem(i);

	// Remove item from list.
	memmove
	(
		(BYTE*)GetData() + (Index      ) * GetClass()->ResRecordSize,
		(BYTE*)GetData() + (Index+Count) * GetClass()->ResRecordSize,
		(Num - Index - Count           ) * GetClass()->ResRecordSize
	);
	Num--;
	unguardobj;
}
void UDatabase::ModifyAllItems()
{
	guard(UDatabase::ModifyAllItems);

	// Modify each element.
	for( int i=0; i<Num; i++ )
		ModifyItem(i);
	
	unguardobj;
}
void UDatabase::StandardSerializeData( FArchive &Ar )
{
	guard(UDatabase::StandardSerializeData);
	unguardobj;
}
IMPLEMENT_CLASS(UDatabase);

/*-----------------------------------------------------------------------------
	FGlobalObjectManager privates.
-----------------------------------------------------------------------------*/

//
// Find an object, no fail.
//
UObject *FGlobalObjectManager::Find( const char *Name, UClass *Type, EFindObject FindType )
{
	guard(FGlobalObjectManager::Find);

	// Try to find it.
	for( int i=0; i<MaxRes; i++ )
	{
		UObject *Res = ResArray[i];
		if( Res && Res->GetClass()==Type && stricmp( Name, Res->GetName() )==0 )
			return Res;
	}

	// Error if required but not found.
	if( FindType==FIND_Existing )
		appErrorf( "Can't find %s %s", Type->GetName(), Name );

	// Return not found.
	return NULL;
	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalObjectManager Init & Exit.
-----------------------------------------------------------------------------*/

//
// Init the object manager and allocate tables.
//
void FGlobalObjectManager::Init()
{
	guard(FGlobalObjectManager::Init);

	// Init names.
	FName::InitSubsystem();

	// Allocate all tables.
	MaxRes      = 1024;
	ResArray	= appMallocArray( MaxRes, UObject*, "ResArray" );

	// Init objects.
	for( INDEX i=0; i<MaxRes; i++ )
		ResArray[i] = NULL;

	// Add all autoregister classes.
	INT NumClasses=0;
	for( UClass *Class=AutoRegister; Class!=NULL; Class=Class->ResNextAutoReg )
	{
		if( !(Class->ClassFlags & CLASS_Intrinsic) )
			appErrorf( "Class %s needs CLASS_Intrinsic", Class->GetName() );
		if( Class->GetFName() == NAME_None )
			appErrorf( "An autoregistered class is unnamed" );
		AddObject(Class);
	}

	// Allocate hardcoded objects.
	Root = new("Root",CREATE_Unique)UArray(0);

	debugf( LOG_Init, "Object subsystem initialized" );
	unguard;
}

//
// Shut down the object manager.
//
void FGlobalObjectManager::Exit()
{
	guard(FGlobalObjectManager::Exit);

	// Kill all unclaimed objects.
	CollectGarbage(GApp);

	// Kill the root object.
	Root->Kill();

	// Kill all objects.
	debug (LOG_Exit,"FGlobalObjectManager::Exit");
	UObject *Res;
	FOR_ALL_OBJECTS(Res)
	{
#if GARBAGE_LOG
		debugf( LOG_Exit, "Unkilled %s %s", Res->GetClassName(), Res->GetName() );
#endif
		Res->Kill();
	}
	END_FOR_ALL_OBJECTS;

	// Shit down names.
	FName::ExitSubsystem();

	appFree( ResArray );
	
	debug (LOG_Exit,"Object subsystem successfully closed.");
	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalObjectManager Tick.
-----------------------------------------------------------------------------*/

//
// Mark one unit of passing time. This is used to update the object
// caching status. This must be called when the object manager is
// in a clean state (outside of all code which retains pointers to
// object data that was gotten).
//
void FGlobalObjectManager::Tick()
{
	guard(FGlobalObjectManager::Tick);

#if CHECK_ALL
	// Make sure all objects are unlocked.
	for( int i=0; i<MaxRes; i++ )
	{
		UObject *Res = ResArray[i];
		if( Res && (Res->ReadLocks>0 || Res->WriteLocks>0 || Res->TransLocks>0) )
		{
			appErrorf
			(
				"%s %s is locked: %i %i %i",
				Res->GetClassName(),
				Res->GetName(),
				Res->ReadLocks,
				Res->WriteLocks,
				Res->TransLocks
			);
		}
	}
#endif
	unguard;
}

/*-----------------------------------------------------------------------------
   FGlobalObjectManager command line.
-----------------------------------------------------------------------------*/

int FGlobalObjectManager::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(FGlobalObjectManager::Exec);
	const char *Str = Cmd;

	if( GetCMD(&Str,"OBJ") )
	{
		if( GetCMD(&Str,"GARBAGE") )
		{
			// Purge unclaimed objects.
			CollectGarbage(Out);
			return 1;
		}
		else if( GetCMD(&Str,"HASH") )
		{
			// Hash info.
			FName::DisplayHash(Out);
			return 1;
		}
		else if( GetCMD(&Str,"LIST") )
		{
			UClass *CheckType = NULL;
			GetUClass( Str, "CLASS=", CheckType );

			enum{MAX=256};
			int Num=0, TotalCount=0, TotalSize=0, TotalMinSize=0, Count[MAX], Size[MAX], MinSize[MAX];
			UClass *TypeList[MAX];

			Out->Log("Objects:");

			UObject *Res;
			FOR_ALL_OBJECTS(Res)
			{
				int ThisSize	= Res->QuerySize();
				int ThisMinSize = Res->QueryMinSize();

				TotalCount++;

				for( int i=0; i<Num; i++ )
					if( TypeList[i] == Res->GetClass() )
						break;
				if( i == Num )
				{
					Num++;
					Count[i] = Size[i] = MinSize[i] = 0;
					TypeList[i] = Res->Class;
				}

				Count   [i]++;
				Size	[i] += ThisSize;	 TotalSize		+= ThisSize;
				MinSize	[i] += ThisMinSize; TotalMinSize	+= ThisMinSize;

				if( CheckType && Res->IsA(CheckType) )
					Out->Logf("%s %s - %i/%i",Res->GetClassName(),Res->GetName(),Res->QueryMinSize(),Res->QuerySize());
			}
			END_FOR_ALL_OBJECTS;

			for( int i=0; i<Num; i++ )
			{
				UClass *Type = TypeList[i];
				if( Type && Size[i] && (CheckType==NULL || CheckType==Type) )
					Out->Logf(" %s...%i (%iK/%iK)",Type->GetName(),Count[i],MinSize[i]/1000,Size[i]/1000);
			}
			Out->Logf("%i Objects (%.3fM/%.3fM)",TotalCount,(FLOAT)TotalMinSize/1000000.0,(FLOAT)TotalSize/1000000.0);
			return 1;
		}
		else return 0;
	}
	else return 0; // Not executed

	unguard;
}

/*-----------------------------------------------------------------------------
   FGlobalObjectManager file loading.
-----------------------------------------------------------------------------*/

//
// Add all objects from an Unrealfile into memory and dynamically link
// their pointers.  Returns 1 if success, 0 if failed. All objects you obtain this 
// way have the RF_Modified bit clear.
//
// Once you add a file, its objects become intermixed with the other objects 
// the system tracks.  You can only kill objects individually, not on a per-file basis
// thus there is no way to "un-add" a file.
//
// If you pass a non-null Linker parameter, the Unrealfile is cached and
// the linker pointer is set to this Unrealfile's linker object.
//
int FGlobalObjectManager::AddFile( const char *Filename, ULinkerLoad **LinkerParm )
{
	guard(FGlobalObjectManager::AddFile);
	ULinkerLoad *Linker = NULL;

	try
	{
		// Create a new linker object which goes off and tries load the file.
		Linker = new("LoadLinker",CREATE_MakeUnique)ULinkerLoad(Filename);
	}
	catch( char *Error )
	{
		// Failed loading.
		debugf("Error adding file: %s",Error);
		return 0;
	}

	// Succeeded loading.
	if( LinkerParm )	*LinkerParm = Linker;
	else				Linker->Kill();
	return 1;

	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalObjectManager file saving.
-----------------------------------------------------------------------------*/

//
// Archive for tagging objects and names that must be exported
// to the file.  It tags the objects passed to it, and recursively
// tags all of the objects this object references.
//
class FArchiveSaveTagExports : public FArchive
{
public:
	FArchiveSaveTagExports(UObject *InParentRes)
	:	OurFlags(InParentRes->GetFlags() & RF_LoadContextFlags) {}
private:
	FArchive& operator<< ( UObject *&Res )
	{
		guard(FArchiveSaveTagExports<<Obj);
		if( Res && !(Res->GetClassFlags() & CLASS_Transient) )
		{
			DWORD FlagMask = OurFlags;
			if( Res->GetFlags() & RF_NotForEdit   ) FlagMask &= ~RF_LoadForEdit;
			if( Res->GetFlags() & RF_NotForClient ) FlagMask &= ~RF_LoadForClient;
			if( Res->GetFlags() & RF_NotForServer ) FlagMask &= ~RF_LoadForServer;
			if(
				(Res->GetFlags() & RF_TagExp) != RF_TagExp
			||	(Res->GetFlags() & FlagMask ) != FlagMask)
			{
				// Tag for export and set the appropriate context flags.
				Res->SetFlags(RF_TagExp | FlagMask);

				// Tag this object's children.
				FArchiveSaveTagExports Ar(Res);
				Res->SerializeHeader(Ar);
				Res->SerializeData(Ar);
			}
		}
		return *this;
		unguard;
	}
	FArchive& operator<< ( FName &Name )
	{
		guard(FArchiveSaveTagExports::Name);

		if( Name != NAME_None )
			Name.SetFlags( RF_TagExp | OurFlags );

		return *this;
		unguard;
	}
	DWORD OurFlags;
};

//
// Archive for tagging objects and names that must be listed in the
// file's imports table.
//
class FArchiveSaveTagImports : public FArchive
{
	FArchive& operator<< ( UObject *&Res )
	{
		guard(FArchiveSaveTagImports<<Obj);
		if
		(	Res
		&&	!(Res->GetFlags() & RF_TagExp) 
		&&	!(Res->GetClassFlags() & CLASS_Transient) )
		{
			Res->SetFlags(RF_TagImp);
		}
		return *this;
		unguard;
	}
};

//
// Archive to tag all names referenced by the object.
//
class FArchiveSaveTagNames : public FArchive
{
public:
	FArchiveSaveTagNames(UObject *InParentRes)
	:	LoadContextFlags(InParentRes->GetFlags() & RF_LoadContextFlags) {}
	FArchive& operator<< ( FName &Name )
	{
		if( Name != NAME_None )
			Name.SetFlags( RF_TagExp | LoadContextFlags );
		return *this;
	}
private:
	DWORD LoadContextFlags;
};

//
// Save all tagged objects into a file.
//
// If you specify an already-added delta file, this will only save the
// changes that occured between the in-memory objects and the original in-file
// objects, based on a CRC comparison.
//
void FGlobalObjectManager::SaveTagged( const char *Filename )
{
	guard(FGlobalObjectManager::SaveTagged);

	FNameEntry		*ResNames=NULL;		// File's name symbol table.
	UObjectBase		*ResImports=NULL;	// File's object import symbol table.
	UObjectBase		*ResExports=NULL;	// File's export object table.
	ULinkerSave		*Linker = NULL;		// Linker.

	char Status[80];
	sprintf(Status,"Saving file %s...",Filename);
	GApp->StatusUpdate (Status,0,0);
	debug(LOG_Info,Status);

	try
	{
		// Tag all objects for import that are (1) children of
		// exported objects, and (2) not being exported.
		UObject *Res;
		FOR_ALL_OBJECTS(Res)
		{
			if( Res->Flags & RF_TagExp )
			{
				// Build list.
				FArchiveSaveTagImports Ar;
				Res->SerializeHeader(Ar);
				Res->SerializeData(Ar);
			}
		}
		END_FOR_ALL_OBJECTS;

		// Initialize file summary.
		FUnrealfileSummary Summary;
		Summary.UnrealFileVersion   = UNREAL_FILE_VERSION;
		Summary.NumExports          = 0;
		Summary.NumImports          = 0;
		Summary.NumNames            = 0;
		strncpy(Summary.Tag,UNREALFILE_TAG,NAME_SIZE);

		FOR_ALL_OBJECTS(Res)
		{
			// Make sure we export resource type names.
			if( Res->Flags & (RF_TagExp|RF_TagImp) )
				Res->Class->GetFName().SetFlags
				(
					RF_TagExp | (Res->Class->GetFlags() & RF_LoadContextFlags)
				);
		}
		END_FOR_ALL_OBJECTS;

		// Compute object stats.
		FOR_ALL_OBJECTS(Res)
		{
			if( Res->Flags & RF_TagExp )
				Summary.NumExports++;
			else if( Res->Flags & RF_TagImp )
				Summary.NumImports++;
		}
		END_FOR_ALL_OBJECTS;

		// Compute name stats.
		for( int i=0; i<FName::GetMaxNames(); i++ )
			if( FName::GetEntry(i) && (FName::GetEntry(i)->Flags & RF_TagExp) )
				Summary.NumNames++;

		Linker = new("SaveLinker",CREATE_MakeUnique)ULinkerSave(Filename,Summary);

		// Allocate all memory we need.
		ResNames		= appMallocArray(Summary.NumNames,  FNameEntry,"SaveResNames");
		ResImports		= appMallocArray(Summary.NumImports,UObjectBase,"SaveResImports");
		ResExports		= appMallocArray(Summary.NumExports,UObjectBase,"SaveResExports");

		// Build NameSymbols and NameMap, which link the indices of
		// NAME_LENGTH-character names in the
		// global name table to their indices in the file.
		int c=0;
		for( i=0; i<FName::GetMaxNames(); i++ )
		{
			FNameEntry *Entry = FName::GetEntry(i);
			if( Entry && (Entry->Flags & RF_TagExp) )
			{
				mystrncpy( ResNames[c].Name, Entry->Name, NAME_SIZE );
				ResNames[c].Flags = (Entry->Flags & RF_LoadContextFlags);
				Linker->NameMap(c++) = FName::MakeNameFromIndex(i);
			}
		}
		checkState(c==Summary.NumNames);

		// Build object map which maps global objects to file objects, while
		// also building import symbol table.

		// First process all imports.
		c=0;
		FOR_ALL_OBJECTS(Res)
		{
			if( Res->Flags & RF_TagImp )
			{
				ResImports[c].Name  = Res->Name;
				ResImports[c].Class = Res->Class;
				Linker->ResMap(c++) = Res;
			}
		}
		END_FOR_ALL_OBJECTS;
		checkState(c==Summary.NumImports);

		// Now process all exports.
		FOR_ALL_OBJECTS(Res)
		{
			if( Res->Flags & RF_TagExp )
			{
				Linker->ResMap(c++) = Res;
#if SAVE_EXPORT_LOG
				debugf("Export %s %s: %08X",Res->GetClassName(),Res->GetName(),Res->GetFlags());
#endif
			}
		}
		END_FOR_ALL_OBJECTS;
		checkState(c==(Summary.NumImports + Summary.NumExports));

		// Write file summary; will be overwritten when we're done, which
		// is safe since the summary is fixed-length.
		*Linker << Summary;

		// Save names.
		Summary.NamesOffset = Linker->Tell();
		for( int k=0; k<Summary.NumNames; k++ )
			*Linker << ResNames[k];

		// Save import table.
		Summary.ImportsOffset = Linker->Tell();
		for( k=0; k<Summary.NumImports; k++ )
			*Linker << ResImports[k];

		// Sort classes to top of resource map.
		int m=0;
		for( k=0; k<Summary.NumExports; k++ )
			if( Linker->ResMap(k+Summary.NumImports)->Class->IsChildOf(UClass::GetBaseClass()) )
				Exchange( Linker->ResMap(Summary.NumImports + k), Linker->ResMap(Summary.NumImports + m++) );

		// Sort classes by hierarchy order.
		FMemMark Mark(GMem);
		BYTE *Order = new(GMem,m)BYTE;
		for( i=0; i<m; i++ )
		{
			Order[i]=0;
			for( UClass *Class = (UClass*)Linker->ResMap(i+Summary.NumImports); Class; Class=Class->ParentClass )
				Order[i]++;
			for( int j=0; j<i; j++ )
			{
				if( Order[j] > Order[i] )
				{
					Exchange( Linker->ResMap(i+Summary.NumImports), Linker->ResMap(j+Summary.NumImports) );
					Exchange( Order[i],                             Order[j]                             );
				}
			}
		}
		Mark.Pop();

		// Debug code to display sorted classes being exported.
		//for( i=0; i<m; i++ )
		//	debugf("%i %s",Order[i],Linker->ResMap(i+Summary.NumImports)->GetName());

		// Save headers and set export info.
		for( i=0; i < Summary.NumExports; i++ )
		{
			if( !(i&7) ) GApp->StatusUpdate (Status,i,Summary.NumExports);

			UObject     *Res    = Linker->ResMap(i + Summary.NumImports);
			UObjectBase &Export = ResExports[i];

			// Save data and note data size.
			Export.FileDataOffset = Linker->Tell();
			Res->SerializeData(*Linker);
			Export.FileDataSize = Linker->Tell() - Export.FileDataOffset;

			// Save header and note header size.
			Export.FileHeaderOffset = Linker->Tell();
			Res->SerializeHeader(*Linker);
			Export.FileHeaderSize = Linker->Tell() - Export.FileHeaderOffset;

			// Set other export info.
			Export.Class              = Res->Class;
			Export.Flags              = Res->Flags;
			Export.Name               = Res->Name;
		}

		// Save the exports.
		GApp->StatusUpdate( "Finalizing", 0, 0 );
		Summary.ExportsOffset = Linker->Tell();
		for( i=0; i < Summary.NumExports; i++ )
		{
			*Linker << ResExports[i];
			if( ResExports[i].Class == UClass::GetBaseClass() ) //!!
				*Linker << AR_OBJECT(((UClass*)Linker->ResMap(i + Summary.NumImports))->ParentClass);
		}

		// Rewrite updated file summary.
		GApp->StatusUpdate( "Closing", 0, 0 );
		Linker->Seek(0);
		*Linker << Summary;
	}
	catch(char *Err)
	{
		unlink(Filename);
		debugf( LOG_Info, "Error %s", Err );
	}
	if( ResNames    ) appFree(ResNames);
	if( ResImports  ) appFree(ResImports);
	if( ResExports  ) appFree(ResExports);
	if( Linker		) Linker->Kill();
	unguard;
}

//
// Save one specific object into an Unrealfile.
//
void FGlobalObjectManager::Save( UObject *Res, const char *Fname )
{
	guard(FGlobalObjectManager::Save);

	UntagAll();
	Res->Flags |= RF_TagExp;
	SetTaggedContextFlags();

	FArchiveSaveTagNames Ar(Res);
	Res->SerializeHeader(Ar);
	Res->SerializeData(Ar);

	SaveTagged( Fname );

	unguard;
}


//
// Recursively tag all objects that are referenced by tagged objects.
//
void FGlobalObjectManager::SaveTagAllDependents()
{
	guard(FGlobalObjectManager::SaveTagAllDependents);
	SetTaggedContextFlags();

	UObject *Res;
	FOR_ALL_OBJECTS(Res)
	{
		if( Res->Flags & RF_TagExp )
		{
			FArchiveSaveTagExports Ar(Res);
			Res->SerializeHeader(Ar);
			Res->SerializeData(Ar);
		}
	}
	END_FOR_ALL_OBJECTS;
	unguard;
}

//
// Save the specified object and all objects it references.
//
void FGlobalObjectManager::SaveDependent( UObject *Res, const char *Fname )
{
	guard(FGlobalObjectManager::SaveDependent);

	// Tag only this object for export.
	UntagAll();
	Res->Flags |= RF_TagExp;
	SetTaggedContextFlags();

	// Tag all objects which this one references.
	SaveTagAllDependents();

	// Save this object and its dependents.
	SaveTagged( Fname );

	unguard;
}

//
// Save all tagged objects and all objects that the tagged
// objects reference.
//
void FGlobalObjectManager::SaveDependentTagged( const char *Fname )
{
	guard(FGlobalObjectManager::SaveDependentTagged);

	// Tag all objects that are dependents of the tagged object.
	SaveTagAllDependents();

	// Save them.
	SaveTagged( Fname );

	unguard;
}

/*-----------------------------------------------------------------------------
	FGlobalObjectManager misc.
-----------------------------------------------------------------------------*/

//
// Add an object to the root array. This prevents the object and all
// its descendents from being deleted during garbage collection.
//
void FGlobalObjectManager::AddToRoot( UObject *Res )
{
	guard(FGlobalObjectManager::AddToRoot);
	Root->AddItem( Res );
	unguard;
}

//
// Remove an object from the root array.
//
void FGlobalObjectManager::RemoveFromRoot(UObject *Res)
{
	guard(FGlobalObjectManager::RemoveFromRoot);
	Root->RemoveItem(Res);
	unguard;
}

/*-----------------------------------------------------------------------------
	Object name functions.
-----------------------------------------------------------------------------*/

//
// Create a unique name by combining a base name and an arbitrary number string.  
// The object name returned is guaranteed not to exist.
//
char *FGlobalObjectManager::MakeUniqueName
(
	char		*Result,
	const char	*BaseName,
	UClass      *Type
)
{
	guard(FGlobalObjectManager::MakeUniqueName);
	checkInput(Type!=NULL);

	int TempInt = 0;
	char NewBase[NAME_SIZE], *End;
	char TempIntStr[NAME_SIZE];

	// Make base name sans appended numbers.
	strcpy( NewBase, BaseName );
	End = &NewBase[strlen(NewBase)];
	while( End>NewBase && isdigit(End[-1]) )
		End--;
	*End = 0;

	// Append numbers to base name.
	do
	{
		itoa    ( TempInt++, TempIntStr, 10 );
		strncpy ( Result, NewBase, NAME_SIZE-strlen(TempIntStr)-1 );
		Result  [ NAME_SIZE - strlen(TempIntStr) - 1 ] = 0;
		strcat  ( Result, TempIntStr );
	} while( Find( Result, Type, FIND_Optional ) );

	return Result;
	unguard;
}

/*-----------------------------------------------------------------------------
	Object tagging-for-export.
-----------------------------------------------------------------------------*/

//
// Untag all objects.  Call before tagging objects for saving.
//
void FGlobalObjectManager::UntagAll()
{
	guard(FGlobalObjectManager::UntagAll);
	UObject *Res;

	FOR_ALL_OBJECTS(Res)
	{
		Res->Flags &= ~(RF_TagImp | RF_TagExp);
	}
	END_FOR_ALL_OBJECTS;

	for( INDEX i=0; i<FName::GetMaxNames(); i++ )
		if( FName::GetEntry(i) )
			FName::GetEntry(i)->Flags &= ~(RF_TagImp | RF_TagExp);

	unguard;
}

//
// Set the load-context flags of all tagged objects
// to their standard context flags, and init the load-context
// flags of untagged objects.
//
void FGlobalObjectManager::SetTaggedContextFlags()
{
	UObject *Res;
	FOR_ALL_OBJECTS(Res)
	{
		Res->ClearFlags(RF_LoadForEdit | RF_LoadForClient | RF_LoadForServer );
		if( Res->Flags & RF_TagExp )
		{
			if( !(Res->GetFlags() & RF_NotForEdit  ) )	Res->SetFlags(RF_LoadForEdit);
			if( !(Res->GetFlags() & RF_NotForClient) )	Res->SetFlags(RF_LoadForClient);
			if( !(Res->GetFlags() & RF_NotForServer) )	Res->SetFlags(RF_LoadForServer);
		}
	}

	for( INDEX i=0; i<FName::GetMaxNames(); i++ )
		if( FName::GetEntry(i) )
			FName::GetEntry(i)->Flags &= ~(RF_LoadForEdit | RF_LoadForClient | RF_LoadForServer);

	END_FOR_ALL_OBJECTS;
}

//
// Tag all objects of type ResType that reference an object.
//
class FArchiveTagRefRes : public FArchive
{
public:
	int TagCount;
	FArchiveTagRefRes( UObject *InParentRes, UObject *InTagRes )
	:	FArchive(),
		TagRes(InTagRes),
		ParentRes(InParentRes),
		TagCount(0)
	{}
private:
	UObject *TagRes;
	UObject *ParentRes;
	FArchive& operator<< ( UObject *&Res )
	{
		guard(FArchiveTagRefRes<<Obj);
		if( (Res==TagRes) && !(ParentRes->GetFlags() & RF_TagExp) )
		{
			ParentRes->SetFlags( RF_TagExp );
			TagCount++;
		}
		return *this;
		unguard;
	}
};
int FGlobalObjectManager::TagAllReferencingObject( UObject *TagRes, UClass *Class )
{
	guard(FGlobalObjectManager::TagAllReferencingObject);
	
	int Count=0;
	UObject *Res;
	FOR_ALL_OBJECTS(Res)
	{
		if( Res->Class == Class )
		{
			FArchiveTagRefRes Ar(Res,TagRes);
			Res->SerializeHeader(Ar);
			Res->SerializeData(Ar);
			Count += Ar.TagCount;
		}
	}
	END_FOR_ALL_OBJECTS;

	return Count;
	unguard;
};

//
// Tag all objects that reference a particular name.
//
class FArchiveTagRefName : public FArchive
{
public:
	int TagCount;
	FArchiveTagRefName( UObject *InParentRes, FName InTagName )
	:	FArchive(),
		TagName(InTagName),
		ParentRes(InParentRes),
		TagCount(0)
	{}
private:
	FName TagName;
	UObject *ParentRes;
	FArchive& operator<< ( FName &Name )
	{
		guard(FArchiveTagRef::Name);

		if( Name==TagName )
		{
			ParentRes->SetFlags(RF_TagExp);
			TagCount++;
		}
		return *this;
		unguard;
	}
};
int FGlobalObjectManager::TagAllReferencingName( FName Name, UClass *Class )
{
	guard(FGlobalObjectManager::TagAllReferencingName);

	int Count=0;
	UObject *Res;
	FOR_ALL_OBJECTS(Res)
	{
		if( Res->Class == Class )
		{
			FArchiveTagRefName Ar(Res,Name);
			Res->SerializeHeader(Ar);
			Res->SerializeData(Ar);
			Count += Ar.TagCount;
		}
	}
	END_FOR_ALL_OBJECTS;

	return Count;
	unguard;
}

//
// Tag all objects that reference tagged resources.
// Recurses until all objects referencing tagged objects are tagged.
//
class FArchiveTagRefTagged : public FArchive
{
public:
	int NumNewlyTagged;
	UObject *ParentRes;
	FArchiveTagRefTagged( UObject *InParentRes )
	:	FArchive(),
		ParentRes(InParentRes),
		NumNewlyTagged(0)
	{};
private:
	FArchive& operator<< ( UObject *&Res )
	{
		guard(FArchiveTagRefTagged<<Obj);
		if( Res && !(ParentRes->GetFlags() & RF_TagExp) )
		{
			NumNewlyTagged++;
			ParentRes->SetFlags(RF_TagExp);
		}
		return *this;
		unguard;
	}
};
void FGlobalObjectManager::TagReferencingTagged( UClass *Class )
{
	guard(FGlobalObjectManager::TagReferencingTagged);
	int Num;
	do
	{
		Num=0;
		UObject *Res;
		FOR_ALL_OBJECTS(Res)
		{
			if( Res->Class == Class )
			{
				FArchiveTagRefTagged Ar(Res);
				Res->SerializeHeader(Ar);
				Res->SerializeData(Ar);
				Num += Ar.NumNewlyTagged;
			}
		}
		END_FOR_ALL_OBJECTS;
	} while( Num > 0 );
	unguard;
}

/*-----------------------------------------------------------------------------
	Creating and allocating data for new objects.
-----------------------------------------------------------------------------*/

//
// Add an object to the table.
//
void FGlobalObjectManager::AddObject( UObject *Res )
{
	guard(FGlobalObjectManager::AddObject);
	
	// Find an available index.
	for( INDEX Index=0; Index<MaxRes; Index++ )
		if( !ResArray[Index] )
			break;

	// Create a new index if needed.
	if( Index >= MaxRes )
	{
		Index     = MaxRes;
		MaxRes   += 256 + (MaxRes/4);
		ResArray  = (UObject **)appRealloc(ResArray,MaxRes * sizeof(UObject **),"ResArray");
		for( int i=Index; i<MaxRes; i++ )
			ResArray[i] = NULL;
	}

	// Add to global table here.
	ResArray[Index]	= Res;
	Res->Index      = Index;

	unguard;
}

//
// Create a new object of a certain type, and allocate its header but not
// its data.  Sets the RF_Modified flag. Retuns object if ok, NULL if error.
//
// If Name is NULL, tries to create an object with an arbitrary unique name.
//
UObject *FGlobalObjectManager::PrivateAlloc
(
	const char		*Name,
	UClass			*Type,
	ECreateObject   Create,
	DWORD			SetFlags
)
{
	guard(FGlobalObjectManager::PrivateAlloc);
	checkInput(Type!=NULL);
	char TempName[NAME_SIZE];

	// Validation check.
	if( Name && strlen(Name)>=NAME_SIZE )
		appErrorf( "Name %s is too big", Name );
	if( Type->ClassFlags & CLASS_Abstract )
		appErrorf( "Can't allocate %s: class %s is abstract", Name ? Name : "(null)", Type->GetName() );

	// Compose name.
	if( !Name || (Create==CREATE_MakeUnique && Find(Name,Type,FIND_Optional)) )
	{
		// Must create a unique name.
		if( !Name )
			Name = Type->GetName();

		GObj.MakeUniqueName( TempName, Name, Type );
		Create = CREATE_Unique;
		Name   = TempName;
	}

	// See if object already exists.
	INDEX Index;
	UObject *Res = Find( Name, Type, FIND_Optional );
	if( !Res )
	{
		// Object doesn't already exist.
		Index = INDEX_NONE;
		Res   = (UObject *)appMalloc(Type->ResHeaderSize,"Res(%s)",Name);
	}
	else if( Create == CREATE_Unique )
	{
		// Object already exists.
		Index = INDEX_NONE;
		appErrorf( "%s %s already exists", Type->GetName(), Name );
	}
	else
	{	
		// Replace an existing object.
#if REPLACE_LOG
		debugf(LOG_Info,"Replacing %s",Name);
#endif
		Index = Res->Index;
		Res->UnloadData();
	}

	// Init and set flags to prevent swapping as with loaded objects.
	Res->InitObject( Type, Index, FName(Name,FNAME_Add), SetFlags | RF_Modified );
	if( Index == INDEX_NONE )
		AddObject( Res );

	// Success.
	return Res;
	unguardf(("(%s %s)",Name ? Name : "(null)",Type->GetName()));
}

/*-----------------------------------------------------------------------------
   Garbage collection.
-----------------------------------------------------------------------------*/

//
// Archive for finding unused objects.
//
class FArchiveTagUsed : public FArchive
{
public:
	FArchive& operator<< ( UObject *&Res )
	{
		guard(FArchiveTagUsed<<Obj);
		if( Res )
		{
			if( Res->GetFlags() & RF_Unused )
			{
				// Only recurse the first time object is claimed.
				Res->ClearFlags(RF_Unused);

				// Recurse.
				Res->SerializeHeader(*this);
				Res->SerializeData(*this);
			}
		}
		return *this;
		unguard;
	}
	FArchive& operator<< ( FName &Name )
	{
		guard(FArchiveTagUsed::Name);
		Name.ClearFlags( RF_Unused );

		return *this;
		unguard;
	}
};

//
// Tag all unreferenced objects.
//
void FGlobalObjectManager::TagGarbage()
{
	guard(FGlobalObjectManager::TagGarbage);
	UObject *Res;

	// Tag all objects as unused.
	FOR_ALL_OBJECTS(Res)
	{
		Res->Flags |= RF_Unused;
	}
	END_FOR_ALL_OBJECTS;

	// Tag all names as unused.
	for( INDEX i=0; i<FName::GetMaxNames(); i++ )
		if( FName::GetEntry(i) )
			FName::GetEntry(i)->Flags |= RF_Unused;

	// Recursively tag all used objects and names, starting at the root.
	FArchiveTagUsed TagUsedAr;
	TagUsedAr << AR_OBJECT(Root);

	unguard;
}

//
// Purge all tagged objects.
//
void FGlobalObjectManager::PurgeGarbage( FOutputDevice *Out )
{
	guard(FGlobalObjectManager::PurgeGarbage);
	debugf( LOG_Info, "Purging garbage" );
	UObject *Res;

	// Purge all unused, unempty PreKill objects.
	guard(PreKillRes);
	FOR_ALL_OBJECTS(Res)
	{
		if( (Res->Flags & RF_Unused) && (Res->GetClassFlags() & CLASS_PreKill) && !(Res->GetFlags() & RF_HardcodedRes) )
		{
#if GARBAGE_LOG
			if( Out )
				Out->Logf( "Garbage collected object: %s %s", Res->GetClassName(), Res->GetName() );
#endif
			Res->Kill();
		}
	}
	END_FOR_ALL_OBJECTS;
	unguard;

	// Purge all unused, unempty normal objects.
	guard(NormalRes);
	FOR_ALL_OBJECTS(Res)
	{
		if( (Res->Flags & RF_Unused) && !(Res->GetFlags() & RF_HardcodedRes))
		{
#if GARBAGE_LOG
			if( Out )
				Out->Logf( "Garbage collected object: %s %s", Res->GetClassName(), Res->GetName() );
#endif
			Res->Kill();
		}
	}
	END_FOR_ALL_OBJECTS;
	unguard;

	// Purge all unused, unempty names.
	guard(Names);
	for( INDEX i=0; i<FName::GetMaxNames(); i++ )
	{
		FNameEntry *Name = FName::GetEntry(i);
		if
		(	(Name)
		&&	(Name->Flags & RF_Unused)
		&& !(Name->Flags & RF_HardcodedName) )
		{
#if GARBAGE_LOG
			if( Out )
				Out->Logf("Garbage collected name: %s",Name->Name);
#endif

			// Delete it.
			FName::DeleteEntry(i);
		}
	}
	unguard;
	unguard;
}

//
// Delete all unreferenced objects.
//
void FGlobalObjectManager::CollectGarbage( FOutputDevice *Out )
{
	guard(FGlobalObjectManager::CollectGarbage);
	debugf( LOG_Info, "Collecting garbage" );

	// Tag all garbage.
	TagGarbage();

	// Purge all garbage.
	PurgeGarbage(Out);

	unguard;
}

//
// Returns whether an object is referenced, not counting the
// one reference at Res. No side effects.
//
int FGlobalObjectManager::IsReferenced( UObject *&Res )
{
	guard(FGlobalObjectManager::RefCount);

	// Remember it.
	UObject *OriginalRes = Res;
	Res = NULL;

	// Tag all garbage.
	TagGarbage();

	// Stick the reference back.
	Res = OriginalRes;

	// Return whether this is tagged.
	return (Res->Flags & RF_Unused)==0;
	unguard;
}

//
// Purge this object if and only if it's unreferenced in the
// global object tree, not counting the one reference at *Res.
// Returns 1 if purged, 0 if it couldn't be purged because it's referenced.
//
// If purged, the passed referenced is set to NULL.
// Otherwise, the passed reference is left unchanged.
//
// This function has the side-effect of entirely garbage collecting the
// object tree.
//
int FGlobalObjectManager::AttemptPurge(UObject *&Res)
{
	guard(FGlobalObjectManager::AttempPurge);

	// Remember the original object.
	UObject *OriginalRes = Res;
	Res = NULL;

	// Tag all garbage.
	TagGarbage();

	if( OriginalRes->Flags & RF_Unused )
	{
		// This is garbage.
		PurgeGarbage(GApp);
		return 1;
	}
	else
	{
		// This is referenced, so we can't delete it.
		Res = OriginalRes;
		PurgeGarbage(GApp);
		return 0;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	UTextBuffer implementation.
-----------------------------------------------------------------------------*/

//
// UObject interface.
//
void UTextBuffer::InitHeader()
{
	guard(UTextBuffer::InitHeader);

	// Call parent.
	UDatabase::InitHeader();

	// Init UTextBuffer info.
	Pos = Top = 0;
	unguardobj;
}
const char *UTextBuffer::Import(const char *Buffer, const char *BufferEnd,const char *FileType)
{
	guard(UTextBuffer::Import);

	const char *RealBufferEnd = mystrstr(Buffer,"End Text");
	if (!RealBufferEnd) RealBufferEnd = BufferEnd;

	// Skip junk at end.
	while ((Buffer<RealBufferEnd) && ((*Buffer=='%')||(*Buffer=='\r')||(*Buffer=='\n'))) Buffer++;
	while ((RealBufferEnd>Buffer) && ((RealBufferEnd[-1]==' ')||(RealBufferEnd[-1]=='%')||(RealBufferEnd[-1]=='\r')||(RealBufferEnd[-1]=='\n'))) RealBufferEnd--;

	// Keep last cr/lf.
	if ((RealBufferEnd<BufferEnd) && ((RealBufferEnd[0]=='\r')||(RealBufferEnd[0]=='\n'))) RealBufferEnd++;
	if ((RealBufferEnd<BufferEnd) && ((RealBufferEnd[0]=='\r')||(RealBufferEnd[0]=='\n'))) RealBufferEnd++;

	Num = Max = (int)(RealBufferEnd-Buffer+1);
	Realloc();

	Lock(LOCK_ReadWrite);
	memcpy(&Element(0),Buffer,Num-1);
	Element(Num-1) = 0;
	Unlock(LOCK_ReadWrite);

	return RealBufferEnd;

	unguardobj;
}
void UTextBuffer::Export( FOutputDevice &Out, const char *FileType, int Indent )
{
	guard(UTextBuffer::Export);
	Lock(LOCK_Read);

	char *Start = (char*)GetData();
	char *End   = Start + Num - 1;

	while ((Start<End) && ((Start[0]=='\r')||(Start[0]=='\n')||(Start[0]==' '))) Start++;
	while ((End>Start) && ((End [-1]=='\r')||(End [-1]=='\n')||(End [-1]==' '))) End--;

	Out.Write( Start, End-Start );
	
	Unlock(LOCK_Read);
	unguardobj;
}
IMPLEMENT_DB_CLASS(UTextBuffer);

//
// FOutputDevice interface.
//
// Output a message to the text buffer.
// If there's no room, expands the text buffer to hold it.
void UTextBuffer::Write( const void *Data, int Length, ELogType MsgType )
{
	guard(UTextBuffer::Write);
	checkState(Num>0);
	checkState(Element(Num-1)==0);

	// See if we need to expand the text buffer.
	if( Num + Length > Max )
	{
		// Expand the text buffer.
		Max = Num + 512 + Length + (Length/2);
		Realloc();
	}
	Lock(LOCK_ReadWrite);

	// Output the message and a terminating zero.
	memcpy( &Element(Num-1), Data, Length );
	Num += Length;
	Element(Num-1)=0;

	Unlock(LOCK_ReadWrite);
	unguardobj;
}

//
// UTextBuffer interface.
//

// Clear the text buffer out.
void UTextBuffer::Empty()
{
	guard(UTextBuffer::Empty);

	Lock(LOCK_ReadWrite);
	Num = 0;
	Pos = 0;
	if( Max > 0 ) Element(0)=0;

	Unlock(LOCK_ReadWrite);
	unguardobj;
}

// Shrink the text buffer so that it's not wasting any space
// with padding.
void UTextBuffer::Shrink()
{
	guard(UTextBuffer::Shrink)
	Max = Num>0 ? Num : 1;
	Realloc();
	unguardobj;
}

/*-----------------------------------------------------------------------------
	UArray implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_DB_CLASS(UArray);

// Array link topic function.
AUTOREGISTER_TOPIC("Array",ArrayTopicHandler);
void ArrayTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(ArrayTopicHandler::Get);

	UArray *Array = new(Item,FIND_Optional)UArray;
	if( Array )
	{
		for( int i=0; i<Array->Num; i++ )
		{
			if (i>0) Out.Logf(",");
			Out.Logf("%s",Array->Element(i)->GetName());
		}
	}
	unguard;
}
void ArrayTopicHandler::Set(ULevel *Level, const char *Item, const char *Value)
{
	guard(EnumTopicHandler::Set);
	unguard;
}

/*-----------------------------------------------------------------------------
	UEnumDef implementation.
-----------------------------------------------------------------------------*/

// Add a name element to an array and return its index.
int UEnumDef::AddName( const char *Name )
{
	guard(UEnumDef::AddTag);

	int i = Add();
	Element(i) = FName( Name, FNAME_Add );
	return i;

	unguardobj;
}

// Find the index of a name, and set Result to it; returns success flag.
int UEnumDef::FindNameIndex( const char *NameStr, INDEX &Result ) const
{
	guard(UEnumDef::FindNameIndex);
	FName Name( NameStr, FNAME_Find );
	if( Name != NAME_None ) return FindItem( Name, Result );
	else return 0;
	unguardobj;
}

// UObject interface.
const char *UEnumDef::Import( const char *Buffer, const char *BufferEnd,const char *FileType )
{
	guard(UEnumDef::Import);
	return Buffer;
	unguardobj;
}
void UEnumDef::Export( FOutputDevice &Out, const char *FileType, int Indent )
{
	guard(UEnumDef::Export);
	
	if( !stricmp(FileType,"H") )
	{
		// C++ header.
		Out.Logf("%senum %s {\r\n",spc(Indent),GetName());
		for( int i=0; i<Num; i++ )
			Out.Logf("%s    %-24s=%i,\r\n",spc(Indent),Element(i)(),i);

		if( strchr(Element(0)(),'_') )
		{
			// Include tag_MAX enumeration.
			char Temp[256];
			strcpy(Temp,Element(0)());
			strcpy(strchr(Temp,'_'),"_MAX");
			Out.Logf("%s    %-24s=%i,\r\n",spc(Indent),Temp,i);
		}
		Out.Logf("};\r\n\r\n");
	}
	else if (!stricmp(FileType,"U"))
	{
		// Actor class text file.
		if( Num>0 )
		{
			int Count=0;
			Count += Out.Logf("%sEnum %s = ",spc(Indent),GetName());
			for( int i=0; i<Num; i++ )
			{
				Count += Out.Logf("%s",Element(i)()); 
				if( i<(Num-1) )
				{
					Count += Out.Logf(", ");
					
					if( Count > 60 )
					{
						// Line continuation.
						Out.Logf("_\r\n");
						Out.Logf("   %s",spc(Indent));
						Count = 0;
					}
				}
			}
			Out.Logf("\r\n");
		}
	}
	unguardobj;
}
IMPLEMENT_DB_CLASS(UEnumDef);

// Enumeration link topic function.
AUTOREGISTER_TOPIC("Enum",EnumTopicHandler);
void EnumTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(EnumTopicHandler::Get);

	UEnumDef *Enum = new(Item,FIND_Optional)UEnumDef;
	if( Enum )
	{
		for( int i=0; i<Enum->Num; i++ )
		{
			if (i>0) Out.Logf(",");
			Out.Logf("%i - %s",i,Enum->Element(i)());
		}
	}
	unguard;
}
void EnumTopicHandler::Set(ULevel *Level, const char *Item, const char *Value)
{
	guard(EnumTopicHandler::Set);
	unguard;
}

/*-----------------------------------------------------------------------------
	UBuffer implementation.
-----------------------------------------------------------------------------*/

//
// UObject interface.
//
const char *UBuffer::Import(const char *Buffer, const char *BufferEnd, const char *FileType)
{
	guard(UBuffer::Import);

	Num = (int)(BufferEnd-Buffer);
	memcpy(GetData(),Buffer,Num);
	return Buffer + Num;

	unguardobj;
}
IMPLEMENT_DB_CLASS(UBuffer);

//
// FOutputDevice interface.
// Output a message to the text buffer.
// If there's no room, expands the text buffer to hold it.
//
void UBuffer::Write( const void *Data, int Length, ELogType MsgType )
{
	guard(UBuffer::Write);
	Lock(LOCK_ReadWrite);

	// See if we need to expand the text buffer.
	if( Num + Length + 1 > Max )
	{
		// Expand the text buffer.
		Max = Num + 512 + Length + (Length/2);
		Realloc();
	}

	// Output the message.
	memcpy( &Element(Num), Data, Length);
	Num += Length;

	Unlock(LOCK_ReadWrite);
	unguardobj;
}

/*-----------------------------------------------------------------------------
	Res Link topic handler function.
-----------------------------------------------------------------------------*/

//
// Archive for counting children.
//
class FArchiveCountChildren : public FArchive
{
public:
	// Variables.
	UObject *Parent;
	int &Count;

	// Constructor.
	FArchiveCountChildren(int &ThisCount)
	:	FArchive(),
		Count(ThisCount)
	{
		Count = 0;
	}

	// FArchive interface.
	FArchive& operator<< ( UObject *&Res )
	{
		guard(FArchiveCountChildren<<Obj);	
		Count += (Res != NULL);
		return *this;
		unguard;
	}
};

// Link query archive.
class FArchiveLinkQuery : public FArchive
{
public:
	// Variables.
	enum		{MAX_RESULTS=8192};
	INT			NumResults;
	UObject	*Results [MAX_RESULTS];
	INT			Children [MAX_RESULTS];

	// Constructor.
	FArchiveLinkQuery()
	:	FArchive(),
		NumResults(0)
	{}

	// FArchive interface.
	FArchive& operator<< ( UObject *&Res )
	{
		guard(FArchiveLinkQuery<<Obj);
		
		// Add to list.
		if( Res && NumResults<MAX_RESULTS)
		{
			// Don't add duplicates.
			for( int i=0; i<NumResults; i++ )
				if( Results[i] == Res )
					return *this;

			Results  [NumResults] = Res;
			Children [NumResults] = 0;

			// Count children.
			FArchiveCountChildren Query(Children[NumResults]);
			Res->SerializeHeader(Query);
			Res->SerializeData(Query);

			NumResults++;
		}
		return *this;
		unguard;
	}
};

AUTOREGISTER_TOPIC("Res",ResTopicHandler);
void ResTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(ResTopicHandler::Get);

	if( GetCMD(&Item,"QUERY") )
	{
		UClass *Type;
		if( !GetUClass(Item,"TYPE=",Type) )
			return;

		UObject *Res = NULL;
		GetOBJ(Item,"NAME=",Type,&Res);

		FArchiveLinkQuery Query;
		if( !Res )
		{
			// Query all objects of the specified type.
			FOR_ALL_OBJECTS( Res )
			{
				if( Res->GetClass() == Type )
					Query << Res;
			}
			END_FOR_ALL_OBJECTS;
		}
		else 
		{
			// Query children of a particular object.
			Res->SerializeHeader(Query);
			Res->SerializeData(Query);
		}

		// Return the results string.
		for( int i=0; i<Query.NumResults; i++ )
		{
			if( i>0 ) Out.Log(",");
			Out.Logf
			(
				"%s %s|%s",
				Query.Results [i]->GetClassName(),
				Query.Results [i]->GetName(),
				Query.Children[i] ? "C" : "X"
			);
		}
	}
	unguard;
}
void ResTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(ResTopicHandler::Set);
	unguard;
}

/*-----------------------------------------------------------------------------
	Text Link topic handler function.
-----------------------------------------------------------------------------*/

AUTOREGISTER_TOPIC("Text",TextTopicHandler);
void TextTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(TextTopicHandler::Get);
	UTextBuffer	*Text = new(Item,FIND_Optional)UTextBuffer;
	if( Text && Text->Num>0 )
	{
		Text->Lock(LOCK_Read);
		Out.Log(&Text->Element(0));
		Text->Unlock(LOCK_Read);
	}
	unguard;
}
void TextTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(TextTopicHandler::Set);
	UTextBuffer	*Text = new(Item,CREATE_Replace,RF_NotForClient|RF_NotForServer)UTextBuffer(1);
	Text->Log(Data);
	unguard;
}

AUTOREGISTER_TOPIC("TextPos",TextPosTopicHandler);
void TextPosTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(TextPosTopicHandler::Get);
	UTextBuffer *Text = new(Item,FIND_Optional)UTextBuffer;
	if( Text )
		Out.Logf("%i",Text->Pos);
	unguard;
}
void TextPosTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(TextPosTopicHandler::Set);
	UTextBuffer *Text = new(Item,FIND_Optional)UTextBuffer;
	if( Text )
		Text->Pos = atoi(Data);
	unguard;
}

/*-----------------------------------------------------------------------------
	ULinker implementations.
-----------------------------------------------------------------------------*/

IMPLEMENT_CLASS(ULinker);
IMPLEMENT_CLASS(ULinkerLoad);
IMPLEMENT_CLASS(ULinkerSave);

/*-----------------------------------------------------------------------------
	Other general objects.
-----------------------------------------------------------------------------*/

IMPLEMENT_DB_CLASS(UWords);
IMPLEMENT_DB_CLASS(UInts);

/*-----------------------------------------------------------------------------
	Bit arrays.
-----------------------------------------------------------------------------*/

IMPLEMENT_DB_CLASS(UBitArray);
IMPLEMENT_DB_CLASS(UBitMatrix);

//
// Serialize helper.
//
static void EmitRunlength( FArchive &Ar, BYTE Code, int RunLength )
{
	BYTE A,B;
	if( RunLength <= 63 )	Ar << (A=Code + 0x00 + RunLength);
	else					Ar << (A=Code + 0x40 + (RunLength>>8)) << (B=RunLength);
}

//
// UBitArray serializer.
//
void UBitArray::SerializeData( FArchive &Ar )
{
	guard(UBitArray::SerializeData);
	if( Ar.IsSaving )
	{
		// Save the bit array compressed.
		BOOL Value     = 0;
		int  RunLength = 0;
		for( DWORD i=0; i<NumBits; i++ )
		{
			if( Get(i)==Value )
			{
				// No change in value.
				if( ++RunLength == 16383 )
				{
					// Overflow, so emit 01 + RunLength[0-16383] + 00+RunLength[0]
					EmitRunlength(Ar, 0, RunLength);
					Value     = !Value;
					RunLength = 0;
				}
			}
			else
			{
				// Change in value.
				if( i+1==NumBits || Get(i+1)!=Value )
				{
					// Permanent change in value.					
					EmitRunlength(Ar, 0, RunLength);
					Value     = !Value;
					RunLength = 0;
				}
				else
				{
					// Temporary 1-bit change in value.
					EmitRunlength(Ar, 0x80, RunLength);
					RunLength = 0;
				}
			}
		}
	}
	else if( Ar.IsLoading )
	{
		// Load the bit array.
		DWORD Count = 0, RunLength;
		BOOL  Value = 0;
		BYTE  A, B;
		while( Count<NumBits )
		{
			Ar << A;
			RunLength = A & 0x3f;
			if( A & 0x40 )
			{
				// Two byte runlength.
				Ar << B;
				RunLength = (RunLength << 8) + B;
			}
			while( RunLength-->0 && Count<NumBits )
			{
				// Fill RunLength.
				Set(Count++,Value);
			}
			if( A & 0x80 && Count<NumBits)
			{
				// Write one opposite bit.
				Set(Count++,!Value);
			}
			else
			{
				// Flip value.
				Value = !Value;
			}
		}
	}
	else StandardSerializeData(Ar);
	unguardobj;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
