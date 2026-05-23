/*=============================================================================
	UnObjBas.h: Unreal object base class.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNOBJBAS // Prevent header from being included multiple times
#define _INC_UNOBJBAS

/*----------------------------------------------------------------------------
	Definitions.
----------------------------------------------------------------------------*/

// Forward declarations.
class UObject;
class UDatabase;
class ULinker;
class UArray;
class UClass;
class FArchive;
struct PMessageParms;
int UNENGINE_API GetOBJ( const char *Stream, const char *Match, class UClass *Type, class UObject **DestRes );

// Cast for archiving objects.
#define AR_OBJECT(objptr) (*(UObject**)&(objptr))

/*-----------------------------------------------------------------------------
	Finding/creating/importing objects via operator new.
-----------------------------------------------------------------------------*/

// Enumerated types recognized by UObject-derived operator new() functions for 
// creating, finding, and importing objects easily.  See operator new().

enum EFindObject 
{
	FIND_Existing		= 0, // Find an existing, named object. No-fail; if it doesn't exist, calls GApp->Error.
	FIND_Optional		= 1	 // See if a named object exists; either returns its pointer, or NULL.
};

enum ECreateObject
{
	CREATE_Unique		= 0, // Create a new, unique object. Calls GApp->Error if the object already exists.
	CREATE_Replace		= 1, // Create a new object.  If exists, kills and replaces but retains the same index and pointer.
	CREATE_MakeUnique	= 2, // Create; if object exists, append number to make name unique.
};

enum EImportObject
{
	IMPORT_Unique		= 0, // Import an object from a file.  Calls GApp->Error if the object already exists.
	IMPORT_Replace		= 1, // Import a new object.  If exists, kills and replaces but retains the same index and pointer.
	IMPORT_MakeUnique	= 2, // Import with a forced unique name.
};

enum EDuplicateObject
{
	DUPLICATE_Unique    = 0, // Duplicate an object with a new object with a unique name.
	DUPLICATE_Replace   = 1, // Duplicate an object to a new object and replace it.
	DUPLICATE_MakeUnique= 2, // Duplicate and force the name to be unique.
};

/*-----------------------------------------------------------------------------
	Enums.
-----------------------------------------------------------------------------*/

// Lock types for locking objects.
enum ELockType
{
	LOCK_None			= 0, // Not locked.
	LOCK_Read			= 1, // Allow reading.
	LOCK_ReadWrite		= 3, // Allow reading and writing.
	LOCK_Trans			= 7, // Transactional reading and writing.
	LOCK_CanFail		= 8, // Bit flag indicates that failure is ok.
};

// Flags for postloading data.
enum EPostLoadFlags
{
	POSTLOAD_File		= 1, // Postloading from file.
	POSTLOAD_Trans		= 2, // Postloading after transactional undo/redo.
};

/*----------------------------------------------------------------------------
	guardobj mechanism.
----------------------------------------------------------------------------*/

#define unguardobjSlow		unguardfSlow(("(%s)",this?GetClassName():"NULL",this?GetName():"NULL"))
#define unguardobj			unguardf(("(%s %s)",this?GetClassName():"NULL",this?GetName():"NULL"))

/*----------------------------------------------------------------------------
	FTransactionTracker.
----------------------------------------------------------------------------*/

//
// Class for handling undo/redo among objects.
//
class UNENGINE_API FTransactionTracker
{
public:
	virtual void NoteResHeader(UObject *Res)=0;
	virtual void NoteSingleChange(UObject *Res, int Index)=0;
};

/*----------------------------------------------------------------------------
	Class flags.
----------------------------------------------------------------------------*/

//
// Flags describing an object type.
//
enum EClassFlags
{
	// Base flags.
	CLASS_Abstract          = 0x00001,  // Class is abstract and can't be instantiated directly.
	CLASS_ScriptWritable	= 0x00002,	// Scripts can write this object type.
	CLASS_PreKill           = 0x00004,	// This object should be deleted before non-PreKill ones.
	CLASS_Transient			= 0x00008,	// This object type can't be saved; null it out at save time.
	CLASS_Swappable			= 0x00010,	// Object's data is swappable.
	CLASS_Intrinsic			= 0x00020,	// Class is intrinsic: dependencies exist in the C++ code.
	CLASS_NoEditParent      = 0x00080,  // All parent properties are forced to be uneditable.
	CLASS_Decompile			= 0x10000,	// Decompile the class.

	// Combinations.
	CLASS_Inheret           = CLASS_ScriptWritable | CLASS_Transient, // Flags to inheret from parent class.
	CLASS_RecompileClear    = CLASS_Inheret | CLASS_Decompile | CLASS_NoEditParent, // Flags to clear when recompiling.
	CLASS_RecompileSet      = CLASS_ScriptWritable, // Flags to set when recompiling.
};

/*----------------------------------------------------------------------------
	FGlobalObjectManager.
----------------------------------------------------------------------------*/

//
// Object hash link.
//
struct FObjectHashLink
{
	// Variables.
	UObject*         Object;
	FObjectHashLink* HashNext;

	// Constructor.
	FObjectHashLink( UObject* InObject, FObjectHashLink* InHashNext )
	:	Object	(InObject)
	,	HashNext(InHashNext)
	{}
};

//
// The global object manager.  This tracks all information about all
// active objects, names, types, and files.
//
class UNENGINE_API FGlobalObjectManager
{
	// Friends.
	friend class FName;
	friend class UObject;
	friend class ULinker;
	friend class ULinkerLoad;
	friend class ULinkerSave;
	friend class UClass;

public:
	// Hash.
	enum {HASH_COUNT=1024};

	// Accessors.
	DWORD GetMaxRes() {return MaxRes;}
	UObject *GetResArray(DWORD i) {return ResArray[i];}

	// Init/Exit.
	void Init();
	void Exit();
	int	 Exec(const char *Cmd,FOutputDevice *Out=GApp);
	void Tick();

	// Root object.
	void AddToRoot(UObject *Res);
	void RemoveFromRoot(UObject *Res);

	// Unrealfiles.
	int AddFile(const char *Filename, ULinkerLoad **Linker,int NoWarn=0);
	int Save(UObject *Res, const char *Filename, int NoWarn=0);
	int SaveDependent(UObject *Res, const char *Filename, int NoWarn=0);
	int SaveTagged(const char *Filename,int NoWarn=0);
	int SaveDependentTagged(const char *Filename,int NoWarn=0);
	void SaveTagAllDependents();

	// Resource tagging.
	void UntagAll();
	void SetTaggedContextFlags();
	int TagAllReferencingObject(UObject *Res, UClass *Class);
	int TagAllReferencingName(FName Name, UClass *Class);
	void TagReferencingTagged(UClass *Class);
	void TagImports(DWORD ClassSkipFlags);

	// Garbage collection.
	void CollectGarbage(FOutputDevice *Out);
	int IsReferenced(UObject *&Res);
	int AttemptPurge(UObject *&Res);

	// Internal.
	UObject *CreateObject(const char *Name, UClass *Class, ECreateObject Create, DWORD SetFlags=0);
	UObject *FindObject(const char *Name,UClass *Class,enum EFindObject FindType);
	char *MakeUniqueObjectName(char *Result, const char *BaseName, UClass *Class);

private:
	// Variables.
	static UClass	*AutoRegister;  // Classes to automatically register.
	INDEX			MaxRes;	 		// Maximum objects in table.
	UArray			*Root;	 		// Root array, for tracking active objects.
	UObject 		**ResArray;	 	// Global table of all objects.
	FObjectHashLink	**ResHash;		// Object hash.

	// Internal functions.
	void AddObject(UObject *Res);
	void TagGarbage();
	void PurgeGarbage(FOutputDevice *Out);
};

/*-----------------------------------------------------------------------------
	Autoregistration macro.
-----------------------------------------------------------------------------*/

//
// Ole globally unique identifier.
//
struct UNENGINE_API FGUID
{
	DWORD A,B,C,D;
	friend BOOL operator==(const FGUID& X, const FGUID& Y)
		{return X.A==Y.A && X.B==Y.B && X.C==Y.C && X.D==Y.D;}
	friend BOOL operator!=(const FGUID& X, const FGUID& Y)
		{return X.A!=Y.A || X.B!=Y.B || X.C!=Y.C || X.D!=Y.D;}
};

//
// Ole IUnknown interface.
//
class UNENGINE_API FUnknown
{
	virtual DWORD STDCALL QueryInterface(const FGUID &RefIID, void **InterfacePtr) {return 0;}
	virtual DWORD STDCALL AddRef() {return 0;}
	virtual DWORD STDCALL Release() {return 0;}
};

/*-----------------------------------------------------------------------------
	Object class implementation macros.
-----------------------------------------------------------------------------*/

//
// Register a class with Unreal's global object manager.  This
// macro should be used exactly once at global scope for every unique
// class that the object manager must manage.
//
#define IMPLEMENT_CLASS(ClassName)                              \
	/* Instantiate an object of this class in global memory */  \
	static ClassName autoobject##ClassName;                     \
	/* Construct an instance of this class */                   \
	DLL_EXPORT UClass autoclass##ClassName                      \
	(                                                           \
		&autoobject##ClassName,                                 \
		sizeof(ClassName),                                      \
		ClassName::RecordSize,                                  \
		ClassName::BaseFlags,                                   \
		ClassName::BaseParentClass::BaseClass,                  \
		ClassName::GUID1,                                       \
		ClassName::GUID2,                                       \
		ClassName::GUID3,                                       \
		ClassName::GUID4,                                       \
		(EName)ClassName::BaseName,                             \
		(EName)ClassName::BasePackageName                       \
	);                                                          \
	/* Instantiate the ClassName::BaseClass static variable. */ \
	UClass *ClassName::BaseClass = &autoclass##ClassName;

#define IMPLEMENT_DB_CLASS(ClassName) \
	/* Implement the object class */ \
	IMPLEMENT_CLASS(ClassName) \
	/* Automatic data serializer. */ \
	void ClassName::StandardSerializeData(FArchive &Ar) \
	{ \
		int i=-1; \
		guard(ClassName::StandardSerializeData); \
		Lock(LOCK_ReadWrite); \
		if( sizeof(BaseDataType)==1 ) Ar.Serialize(&Element(0),Num); \
		else for( i=0; i<Num; i++ ) Ar << Element(i); \
		Unlock(LOCK_ReadWrite); \
		unguardf(("(%s %s: %i/%i)",this?GetClassName():"NULL",this?GetName():"NULL",i,Num)); \
	}

/*-----------------------------------------------------------------------------
	DECLARE_CLASS & DECLARE_DB_CLASS macros.
-----------------------------------------------------------------------------*/

//
// Macros for creating classes.  These would be implemented as C++ templates
// were it not for various VC++ 4.0 problems in dealing with templates imported from DLL's.
//

//
// Declare a base class.
//
#define DECLARE_BASE_CLASS(TClass,TParentClass,TResName,TPackageName)\
public:\
	/* Identification */ \
	static UClass *BaseClass; \
	typedef TParentClass BaseParentClass;\
	enum {BaseName = TResName}; \
	enum {BasePackageName = TPackageName}; \
	\
	/* Create a new object of this class; see ECreateObject */\
	void *operator new(size_t Size,const char *Name,ECreateObject Create,DWORD SetFlags=0)\
		{return GObj.CreateObject(Name,GetBaseClass(),Create,SetFlags);}\
	\
	/* Try to import an object from a file; returns pointer if success, NULL if failure */\
	void *operator new(size_t Size,const char *Name,const char *Filename,EImportObject ImportType,DWORD SetFlags=0)\
	{\
		TClass *Temp = new(Name,(ECreateObject)ImportType,SetFlags)TClass;\
		if (Temp->ImportFromFile(Filename)) return Temp;\
		Temp->Kill(); return NULL;\
	}\
	/* Try to find an existing object */\
	void *operator new(size_t Size,const char *Name,EFindObject FindType)\
		{return GObj.FindObject( Name, GetBaseClass(), FindType );}\
	\
	/* Duplicate an object. */\
	void *operator new(size_t Size,const char *Name,TClass *Source, EDuplicateObject DupeType, DWORD SetFlags=0)\
	{\
		TClass *New = (TClass *)GObj.CreateObject(Name,GetBaseClass(),(ECreateObject)DupeType,SetFlags);\
		New->UObject::Lock(LOCK_ReadWrite); Source->UObject::Lock(LOCK_Read); \
		memcpy((UObject *)New+1,(UObject *)Source+1,sizeof(TClass)-sizeof(UObject));\
		New->Realloc(); \
		if( New->QueryMinSize()>0 ) memcpy(New->GetData(),Source->GetData(),New->QueryMinSize()); \
		New->UObject::Unlock(LOCK_ReadWrite); Source->UObject::Unlock(LOCK_Read); \
		return New;\
	}\
	/* Try to parse the object's name */\
	friend int Get##TClass(const char *Stream, const char *Match, TClass *&Res)\
		{return GetOBJ(Stream,Match,GetBaseClass(),(UObject **)&Res);}\
	\
	/* Default constructor; must do nothing */\
	TClass() {};\
	\
	/* Get base type */ \
	static UClass *GetBaseClass() \
		{return BaseClass;}

//
// Macro to create a UObject-derived class.  Includes automatic support for
// many commonly-used member functions.
//
#define DECLARE_CLASS(TClass,TParentClass,TResName,TPackageName) \
	DECLARE_BASE_CLASS(TClass,TParentClass,TResName,TPackageName) \
	/* Typesafe pointer archiver */ \
	extern friend FArchive &operator<< (FArchive &Ar, TClass *&Res) \
		{return Ar << AR_OBJECT(Res);}

//
// Macro to create a UDatabase-derived class.  Includes automatic support for
// many commonly-used member functions.
//
#define DECLARE_DB_CLASS(TClass,TParentClass,TDataType,TResName,TPackageName)\
public:\
	\
	/* Use regular DECLARE_CLASS macro to declare standard UObject info */ \
	DECLARE_CLASS(TClass,TParentClass,TResName,TPackageName);\
	\
	/* Special UDatabase specific */\
	typedef TDataType BaseDataType;\
	enum {RecordSize = sizeof(BaseDataType)};\
	\
	/* Return the type-safe ith element of a database object */\
	BaseDataType &Element(int i)\
	{ \
		/* Element is only valid when an object is locked or is not swappable. */ \
		/*debugState( IsLocked() || !(GetClassFlags() & CLASS_Swappable) );*/ \
		return ((BaseDataType *)GetData())[i]; \
	} \
	const BaseDataType &Element(int i) const\
	{ \
		/* Element is only valid when an object is locked or is not swappable. */ \
		/*debugState( IsLocked() || !(GetClassFlags() & CLASS_Swappable) );*/ \
		return ((BaseDataType *)GetData())[i]; \
	} \
	/* An a new item */ \
	int AddItem(BaseDataType Item) \
	{ \
		int Index = Add(1); \
		Element(Index) = Item; \
		return Index; \
	} \
	/* Remove an item if it exists */ \
	void RemoveItem(BaseDataType Item) \
	{ \
		for( int i=Num-1; i>=0; i-- ) \
			if( memcmp(&Element(i),&Item,sizeof(BaseDataType))==0 ) \
				Remove(i); \
	} \
	/* Find an item and return 1 if found, 0 if not */ \
	int FindItem(BaseDataType Item, INDEX &Result) const \
	{ \
		for( int i=0; i<Num; i++ ) \
			if( memcmp(&Element(i),&Item,sizeof(BaseDataType))==0 ) \
				{Result = i; return 1;}; \
		return 0; \
	} \
	/* Add an item if it's not a duplicate of an existing item */ \
	int AddUniqueItem(BaseDataType Item) \
	{ \
		for( int i=0; i<Num; i++ ) \
			if( memcmp(&Element(i),&Item,sizeof(BaseDataType))==0 ) \
				return i; \
		return AddItem( Item ); \
	} \
	/* Copy data from another object */\
	void CopyDataFrom(TClass *Src) \
	{ \
		Num = Max = Src->Num; Realloc(); \
		for( int i=0; i<Num; i++ ) \
			Element(i) = Src->Element(i); \
	} \
	/* Pointer to an object of this class */\
	struct UNENGINE_API Ptr \
	{ \
		TClass *Res; \
		Ptr() {} \
		Ptr(TClass *Set) {Res = Set;} \
		      TClass& operator*()        {return *Res;} \
		const TClass& operator*()  const {return *Res;} \
		      TClass* operator->()       {return Res;} \
		const TClass* operator->() const {return Res;} \
		operator       TClass* ()        {return Res;} \
		operator const TClass* ()  const {return Res;} \
		      BaseDataType& operator() (int i)       {return ((BaseDataType *)Res->GetData())[i];} \
		const BaseDataType& operator() (int i) const {return ((BaseDataType *)Res->GetData())[i];} \
		extern friend FArchive &operator<< (FArchive &Ar, Ptr &P)\
			{return Ar << AR_OBJECT(P.Res);} \
	}; \
	/* Standard serialization routine, generated by IMPLEMENT_DB_CLASS macro. */\
	void StandardSerializeData(FArchive &Ar);

/*-----------------------------------------------------------------------------
	UObject.
-----------------------------------------------------------------------------*/

// Flags associated with each object instance.
enum EObjectFlags
{
	RF_Modified			= 0x000001,	// Was modified since last load.
	RF_Unused			= 0x000002,	// Unused, can be freed in next call to resCleanup.
	RF_TagChild			= 0x000004,	// Is child of unused object (used in resCleanup only).
	RF_TagImp			= 0x000008,	// Temporary import tag in load/save.
	RF_TagExp			= 0x000010,	// Temporary export tag in load/save.
	RF_NoFreeData		= 0x000020,	// Don't free this object's data (custom memory allocation).
	RF_HardcodedName	= 0x000040,	// Hardcoded name, don't free it.
	RF_TransData		= 0x000080,	// Used by transaction tracking system to track changed data.
	RF_TransHeader		= 0x000100,	// Used by transaction tracking system to track changed headers.
	RF_HardcodedRes     = 0x000200, // Hardcoded object; don't free or export it.
	RF_HighlightedName  = 0x000400, // A hardcoded name which should be syntax-highlighted.
	RF_InSingularFunc   = 0x000800, // In a singular function.
	RF_Unused1          = 0x001000, // Unused.
	RF_Temp1			= 0x002000,	// Temporary flag for user routines.
	RF_UnlinkedHeader	= 0x004000,	// During load/save, indicates that objects/names are unlinked.
	RF_UnlinkedData		= 0x008000,	// During load/save, indicates that objects/names are unlinked.
	RF_LoadForClient	= 0x010000,	// In-file load for client.
	RF_LoadForServer	= 0x020000,	// In-file load for client.
	RF_LoadForEdit		= 0x040000,	// In-file load for client.
	RF_Unused2          = 0x080000, // Unused;
	RF_NotForClient		= 0x100000,	// Don't load this object for the game client.
	RF_NotForServer		= 0x200000,	// Don't load this object for the game server.
	RF_NotForEdit		= 0x400000,	// Don't load this object for the editor.
	RF_Unused3          = 0x800000, // Unused.
	RF_ContextFlags		= RF_NotForClient | RF_NotForServer | RF_NotForEdit, // All context flags.
	RF_LoadContextFlags	= RF_LoadForClient | RF_LoadForServer | RF_LoadForEdit, // Flags affecting loading.
	RF_Load  			= RF_LoadContextFlags, // Flags to load from Unrealfiles.
	RF_Keep             = RF_NoFreeData | RF_HardcodedRes | RF_HardcodedName, // Flags to persist across loads.
};

//
// Information about a saved object.
//
struct FSavedObject
{
	FMemMark	MemMark;
	void		*SavedHeader;
	void		*SavedData;
	int			SavedHeaderSize;
	int			SavedDataSize;
};

//
// The base class of UObject.
//
class UNENGINE_API UObjectBase : public FUnknown
{
	// Friends.
	friend class FGlobalObjectManager;
	friend class ULinker;
	friend class ULinkerLoad;
	friend class ULinkerSave;

public:
	// General info.
	FName		Name;				// Name of the object.
	UClass*		Class;	  			// Class the object belongs to.
	DWORD		Flags;				// Private EObjectFlags used by object manager.

	// Info for objects stored in files only.
	DWORD		FileCRC;			// CRC32 value of header+data as stored in file.
	DWORD		FileHeaderOffset;	// Offset of header start into file.
	DWORD 		FileDataOffset;		// Offset of data in Unrealfiles.
	DWORD		FileHeaderSize;		// Size of header in file.
	DWORD		FileDataSize;		// Size of data in object data.

	// Serializer.
	friend FArchive& operator<<( FArchive& Ar, UObjectBase &U );
};

//
// The base class of all objects.
//
class UNENGINE_API UObject : private UObjectBase
{
	DECLARE_BASE_CLASS(UObject,UObject,NAME_Object,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};
	typedef UObject BaseParentClass;

	// Friends.
	friend class FGlobalObjectManager;
	friend class ULinker;

private:
	// Information relevent in memory only.
	ULinker*		XLinker;			// Linker it came from or NULL if none.
	void*			Data;				// Pointer to data in memory, not meaningful when stored on disk.
	mutable INT		ReadLocks;			// Number of read locks applied.
	INT				WriteLocks;			// Number of write locks applied.
	INT				TransLocks;			// Number of transactional locks applied.
	INDEX			Index;				// Index of object into FGlobalObjectManager's ResArray table.
	INDEX			FileIndex;			// File index it came from in the Linker, or INDEX_NONE.

public:
	// UnrealScript execution stack.
	FExecStackMain  MainStack;			// Main execution stack.

	// Constants.
	enum {RecordSize = 0};

	// FUnknown interface.
	virtual DWORD STDCALL QueryInterface(const FGUID &RefIID, void **InterfacePtr);
	virtual DWORD STDCALL AddRef();
	virtual DWORD STDCALL Release();

	// Standard object functions that are and not overriden by child classes.
    void            InitObject		(UClass *Class,INDEX Index,FName Name,DWORD Flags);
	virtual void    Process         (FName Message, PMessageParms *Parms);
	virtual void	Kill			();
	virtual void	*Realloc		();
	virtual int		ExportToFile	(const char *Filename);
	virtual int		ImportFromFile	(const char *Filename);
	virtual DWORD	FullCRC			();
	virtual DWORD	DataCRC			();
	virtual void	ModifyHeader	();
	virtual void	ModifyAllItems	();
	virtual void    Push			(FSavedObject &Saved, FMemStack &Mem);
	virtual void	Pop				(FSavedObject &Saved);

	// Standard functions which may be overridden by all child classes to
	// provide whatever functionality is needed by each class:
	virtual void    InitHeader		();
	virtual int     QuerySize		();
	virtual int     QueryMinSize	();
	virtual const char *Import		(const char *Buffer, const char *BufferEnd,const char *FileType);
	virtual void    Export			(FOutputDevice &Out,const char *FileType,int Indent);
	virtual void    PostLoadHeader	(DWORD PostFlags);
	virtual void    PostLoadData	(DWORD PostFlags);
	virtual void    PreKill			();
	virtual void    SerializeData	(FArchive &Ar);
	virtual void	SerializeHeader (FArchive &Ar);
	virtual INT     Lock			(DWORD LockType);
	virtual void    Unlock			(DWORD LockType);
	virtual INT     ReadLock		() const;
	virtual void    ReadUnlock		() const;
	virtual void    UnloadData		();

	// Accessors.
	UClass*     GetClass() const			{return Class;}
	void        SetClass(UClass *In)        {Class=In;}
	const char* GetClassName() const		{return ((UObject*)Class)->GetName();}
	DWORD		GetFlags() const			{return Flags;}
	void		SetFlags(DWORD NewFlags) 	{Flags|=NewFlags;}
	void		ClearFlags(DWORD NewFlags)	{Flags&=~NewFlags;}
	DWORD		GetContextFlags() const		{return Flags & RF_ContextFlags;}
	void		SetData(void *New)			{Data=New;}
	const char* GetName() const				{return Name();}
	const FName GetFName() const            {return Name;}
	void		Rename(const char *NewName)	{Name = FName(NewName,FNAME_Add);}
	DWORD		GetIndex() const			{return Index;}
	INT			IsLocked() const			{return ReadLocks>0;}
	INT			IsWriteLocked() const		{return WriteLocks>0;}
	INT			IsTransLocked() const		{return TransLocks>0;}
	void		InitLocks()					{ReadLocks=WriteLocks=TransLocks=0;}
	DWORD       GetClassFlags() const;
	void*       ObjectPropertyPtr(class FProperty &Property, int iElement=0);
	void        GetObjectBins(BYTE**);
	BOOL        IsProbing(FName ProbeName);
	BOOL        IsA(const class UClass *SomeParent) const;
	BOOL        IsA(const char *Name) const;
	void*       GetData()
	{
		// GetData is only valid when an object is locked or is not swappable.
		//debugState( IsLocked() || !(GetClassFlags() & CLASS_Swappable) );
		return Data;
	}
	const void*  GetData() const
	{
		// GetData is only valid when an object is locked or is not swappable.
		//debugState( IsLocked() || !(GetClassFlags() & CLASS_Swappable) );
		return Data;
	}
};

/*-----------------------------------------------------------------------------
	UDatabase.
-----------------------------------------------------------------------------*/

//
// The parent class of all database-oriented objects.
//
// This provides all of the basic functionality required for classes
// which are arrays of simple, constant-length records.  These classes
// have the capability of being transaction-tracked for Undo/Redo features.
// The Register() member function of each UDatabase-derived class must 
// set the class's RecordSize member to the size of one fixed-length record.
//
class UNENGINE_API UDatabase : public UObject
{
	DECLARE_CLASS(UDatabase,UObject,NAME_Database,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

public:
	// UDatabase variables.
	INT		Num; // Number of active records in the database.
	INT		Max; // Number of allocated records in the database.

	// Constructors.
	UDatabase( int MaxElements, int Occupy=0 )
	{
		guard(UDatabase::UDatabase);
		Num = 0;
		Max = MaxElements;
		Realloc();
		if( Occupy )
		{
			memset( GetData(), 0, QuerySize() );
			Num = Max;
		}
		unguard;
	}

	// UObject interface, not to be overridden.
	int  QuerySize();
	int  QueryMinSize();
	void PostLoadData(DWORD PostFlags);

	// UObject interface, overridable.
	void InitHeader();
	void SerializeHeader(FArchive &Ar)
	{
		guard(UDatabase::SerializeHeader);
		UObject::SerializeHeader(Ar);
		Ar << Num << Max;
		unguard;
	}
	void SerializeData(FArchive &Ar)
	{
		StandardSerializeData(Ar);
	}

	// UDatabase interface.
	virtual int	 IsValidIndex	(int i) {return i>=0 && i<Num;}
	virtual void Empty			();
	virtual void Shrink			();
	virtual int  Add			(int NumToAdd=1);
	virtual void Remove			(int Index, int Count=1);
	virtual void ModifyAllItems	();
	virtual void PostLoadItem	(int Index,DWORD PostFlags);
	virtual void StandardSerializeData(FArchive &Ar);

	// Inlines.
	void ModifyItem( int i )
	{
		checkInput(i>=0 && i<=Max);
		if( IsTransLocked() )
			GUndo->NoteSingleChange( this, i );
	}
};

/*----------------------------------------------------------------------------
	Macros.
----------------------------------------------------------------------------*/

// Verify the a class definition and C++ definition match up.
#define VERIFY_CLASS_OFFSET(Pre,ClassName,Member) \
	{for( FPropertyIterator It(new( #ClassName, FIND_Existing )UClass); It; ++It ) \
		if( stricmp(It().Name(),#Member)==0 ) \
			if( It().Offset != STRUCT_OFFSET(Pre##ClassName,Member) ) \
				appErrorf("Class %s Member %s problem: Script=%i C++=%i", #ClassName, #Member, It().Offset, STRUCT_OFFSET(Pre##ClassName,Member) );}

// Iterate for all active objects:
#define FOR_ALL_OBJECTS(RESVAR)\
	{\
	for( DWORD RESVAR##index=0; RESVAR##index<GObj.GetMaxRes(); RESVAR##index++ )\
		{\
		RESVAR = GObj.GetResArray(RESVAR##index);\
		if (RESVAR)\
			{

// Iterate for all active objects of a particular class:
#define FOR_ALL_TYPED_OBJECTS(RESVAR,RESCLASS)\
	{\
		for (DWORD RESVAR##index=0; RESVAR##index<GObj.GetMaxRes(); RESVAR##index++)\
		{\
			RESVAR = (RESCLASS *)GObj.GetResArray(RESVAR##index);\
			if( RESVAR && RESVAR->GetClass()==RESCLASS::GetBaseClass() )\
			{
#define END_FOR_ALL_TYPED_OBJECTS END_FOR_ALL_OBJECTS\

// Close the iteration, for FOR_ALL_RES and FOR_ALL_TYPED_RES.
#define END_FOR_ALL_OBJECTS\
			}\
		}\
	}

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNOBJBAS
