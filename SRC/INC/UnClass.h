/*=============================================================================
	UnClass.h: UClass definition.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney.
=============================================================================*/

#ifndef _INC_UNCLASS
#define _INC_UNCLASS

/*-----------------------------------------------------------------------------
	UDependencies.
-----------------------------------------------------------------------------*/

//
// One dependency record.
//
class UNENGINE_API FDependency
{
public:
	// Variables.
	UClass*		Class;			// Dependency class.
	DWORD		ScriptTextCRC;	// Dependency script text CRC.

	// Constructors.
	FDependency();
	FDependency( UClass* InClass );
	FDependency( UClass* InClass, DWORD InScriptTextCRC );

	// Inlines.
	BOOL IsUpToDate();

	// Serialization.
	friend FArchive& operator<< (FArchive &Ar, FDependency &Dep )
	{
		return Ar << Dep.Class << Dep.ScriptTextCRC;
	}
};

//
// A list of class dependencies.
//
class UNENGINE_API UDependencies : public UDatabase
{
	DECLARE_DB_CLASS(UDependencies,UDatabase,FDependency,NAME_Dependencies,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UDependencies(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UClass.
-----------------------------------------------------------------------------*/

// Information about a saved class.
struct FSavedClass
{
	// Variables.
	FSavedObject SavedClass;
	FSavedObject SavedScript;
	FSavedObject SavedStackTree;
	FSavedObject SavedDependencies;
	FSavedObject SavedBins[PROPBIN_MAX];
};

//
// An actor class.  Describes all of the attributes of a kind of actor, but
// does not refer to an actual instance of an actor.
//
class UNENGINE_API UClass : public UDatabase
{
	DECLARE_DB_CLASS(UClass,UDatabase,FProperty,NAME_Class,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_ScriptWritable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Vital class info.
	UClass		*ParentClass;			// Optional parent class of the actor, NULL if this class is a base class.
	FName		PackageName;			// Package the class belongs to.
	DWORD		ClassFlags;				// Object class flags (CLASS_).
	DWORD		ResThisHeaderSize;		// Size of this type's object header minus its parent's header's size.

	// Regular class info.
	UTextBuffer	*ScriptText;			// Optional script source code.
	UScript		*Script;				// Optional compiled script code.
	UStackTree	*StackTree;				// Optional script stack tree.
	UDependencies::Ptr Dependencies;	// This class's list of dependencies.
	UProperties	*Bins[PROPBIN_MAX];		// All property bins' initialized data.
	DWORD		AllPropertyFlags;		// Result of or'ing all property flags together.

	// Object type variables.
	DWORD		ResFullHeaderSize; 		// Size of this type's full object header.
	DWORD		ResRecordSize;			// If a database, size of each individual record; 0 if not applicable.
	INT			ResGUID[4];				// Globally unique identifier.

	// Object type variables, in memory only.
	void		*ResVTablePtr;			// Pointer to virtual function table (memory only).
	UClass      *ResNextAutoReg;		// Next type in autoregistry chain.

	// Constructors.
	UClass(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
	UClass(UObject *Template, DWORD InHeaderSize, DWORD InRecordSize, DWORD InClassFlags, UClass *InParentClass, DWORD A, DWORD B, DWORD C, DWORD D, FName InName,FName InPackageName);

	// UObject interface.
	void InitHeader();
	const char *Import(const char *Buffer, const char *BufferEnd,const char *FileType);
	void Export(FOutputDevice &Out,const char *FileType,int Indent);
	void PostLoadHeader(DWORD PostFlags);
	void SerializeHeader(FArchive &Ar)
	{
		guard(UClass::SerializeHeader);

		// Call parent.
		UDatabase::SerializeHeader(Ar);

		// Serialize regular properties.
		Ar << ParentClass;
		Ar << ScriptText << Script << StackTree << *(UArray**)&Dependencies;
		Ar << AllPropertyFlags << PackageName;

		// Serialize bins.
		for( int i=0; i<PROPBIN_MAX; i++ )
			Ar << Bins[i];

		// Object info.
		Ar << ResFullHeaderSize << ResThisHeaderSize << ResRecordSize << ClassFlags;
		Ar << ResGUID[0] << ResGUID[1] << ResGUID[2] << ResGUID[3];

		unguardobj;
	}
	void Push( FSavedClass &Saved, FMemStack &Mem );
	void Pop( FSavedClass &Saved );

	// UClass interface.
	BOOL IsChildOf( const UClass *SomeParent ) const
	{
		guardSlow(UClass::IsChildOf);
		for( const UClass *Class=this; Class; Class=Class->ParentClass )
			if( Class == SomeParent ) 
				return 1;
		return 0;
		unguardobjSlow;
	}
	BOOL IsChildOf( const char *ClassName ) const
	{
		guardSlow(UClass::IsChildOf);
		for( const UClass *Class=this; Class; Class=Class->ParentClass )
			if( stricmp(Class->GetName(),ClassName)==0 ) 
				return 1;
		return 0;
		unguardobjSlow;
	}
	AActor &GetDefaultActor()
	{
		guardSlow(UClass::GetDefaultActor);
		debugState(IsChildOf("Actor"));
		return *(AActor *)Bins[PROPBIN_PerObject]->GetData();
		unguardobjSlow;
	}
	void GetClassBins( BYTE **OutBins )
	{
		guardSlow(UClass::GetClassBins);
		for( int i=0; i<PROPBIN_MAX; i++ )
			OutBins[i] = Bins[i] ? (BYTE*)Bins[i]->GetData() : NULL;
		unguardobjSlow;
	}
	void AllocBins()
	{
		guard(UClass::AllocBins);
		Bins[PROPBIN_PerFunction] = NULL;
		Bins[PROPBIN_PerObject  ] = new( GetName(), CREATE_MakeUnique )UProperties( this, PROPBIN_PerObject   );
		Bins[PROPBIN_PerClass   ] = new( GetName(), CREATE_MakeUnique )UProperties( this, PROPBIN_PerClass    );
		unguard;
	}
	FProperty *FindProperty( FName Name, EPropertyBin Bin )
	{
		guard(UClass::FindProperty);
		for( int i=0; i<Num; i++ )
			if( Element(i).Name==Name && Element(i).Bin==Bin )
				return &Element(i);
		return &Element(0);
		unguard;
	}
	UClass						(UClass *ParentClass);
	void SerializeBin			(FArchive &Ar,EPropertyBin Bin, void *Data, void *Defaults);
	void AddParentProperties	();
	void DeleteClass			();
	BOOL SetClassVTable			(void *NewVTable=NULL, BOOL Checked=1);
	void Finish					();
	FProperty &AddProperty	    (FProperty &Property, BYTE *&Data);
};

/*-----------------------------------------------------------------------------
	FPropertyIterator.
-----------------------------------------------------------------------------*/

//
// Class for iterating through all class properties.
//
class FPropertyIterator
{
private:
	// Variables.
	UClass::Ptr	Class;    // Current iterator class.
	INDEX       Index;    // Property index into current class.
	DWORD		FlagMask; // Flags to mask out.

public:
	// Constructor.
	FPropertyIterator( UClass *InClass )
	:	Class   ( InClass   )
	,	Index   ( -1        )
	,   FlagMask( ~(DWORD)0 )
	{
		debugState(Class!=NULL);
		++*this;
	}

	// Go to next element.
	void operator++()
	{
		debugState(Class!=NULL);
		while( Class && ++Index >= Class->Num )
		{
			if( Class->ClassFlags & CLASS_NoEditParent )
				FlagMask &= ~CPF_Edit;
			Class = Class->ParentClass;
			Index = -1;
		}
	}

	// Get property pointer.
	FProperty& operator() ()
	{
		debugState(Index>=0);
		debugState(Class!=NULL);
		return Class->Element(Index);
	}

	// Convert to boolean.
	operator BOOL()
	{
		return Class!=NULL;
	}

	// Accessors.
	UClass *GetClass()   {return Class;   }
	INDEX  GetIndex()    {return Index;   }
	DWORD  GetFlagMask() {return FlagMask;}
};

/*-----------------------------------------------------------------------------
	UObject accessors that depend on UClass.
-----------------------------------------------------------------------------*/

//
// UObject inlines.
//
inline DWORD UObject::GetClassFlags() const
{
	return Class->ClassFlags;
}

//
// See if this objects belongs to the specified class.
//
inline BOOL UObject::IsA( const class UClass *SomeParent ) const
{
	guardSlow(UObject::IsA);
	for( UClass *TempClass=GetClass(); TempClass; TempClass=TempClass->ParentClass )
		if( TempClass==SomeParent )
			return 1;
	return 0;
	unguardSlow;
}

//
// See if this object belongs to the specified class.
//
inline BOOL UObject::IsA( const char *ClassName ) const
{
	guardSlow(UObject::IsA);
	for( UClass *TempClass=GetClass(); TempClass; TempClass=TempClass->ParentClass )
		if( stricmp( TempClass->GetName(), ClassName ) == 0 )
			return 1;
	return 0;
	unguardSlow;
}

//
// Get the pointer to a particular object property.
//
inline void *UObject::ObjectPropertyPtr( FProperty &Property, int iElement )
{
	guardSlow(UObject::ObjectPropertyPtr);
	switch( Property.Bin )
	{
		case PROPBIN_PerObject: return (BYTE*)this + Property.Offset + iElement * Property.ElementSize;
		case PROPBIN_PerClass:  return (BYTE*)&GetClass()->Bins[PROPBIN_PerClass]->Element(0) + Property.Offset + iElement * Property.ElementSize;
		default:                appErrorf( "Invalid bin %i", Property.Bin ); return NULL;
	}
	unguardSlow;
}

//
// Return the actor's property bin list.
//
inline void UObject::GetObjectBins( BYTE **OutBins )
{
	guardSlow(UObject::GetBins);

	// Get bins.
	OutBins[PROPBIN_PerFunction] = (BYTE*)NULL;
	OutBins[PROPBIN_PerObject  ] = (BYTE*)this;
	OutBins[PROPBIN_PerClass   ] = (BYTE*)&GetClass()->Bins[PROPBIN_PerClass]->Element(0);

	unguardSlow;
}

//
// Return whether an object wants to receive a named probe message.
//
inline BOOL UObject::IsProbing( FName ProbeName )
{
	guardSlow(UObject::IsProbing);
	return	(ProbeName.GetIndex() <  PROBE_MIN)
	||		(ProbeName.GetIndex() >= PROBE_MAX)
	||		(MainStack.ProbeMask & ((QWORD)1 << (ProbeName.GetIndex() - PROBE_MIN)));
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNCLASS
