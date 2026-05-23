/*=============================================================================
	UnClass.cpp: Actor class functions

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	UClass implementation.
-----------------------------------------------------------------------------*/

//
// Set the vtable for this class.
// If you pass NULL, we search for the proper VTable.
// If you pass a valid pointer, your pointer is used.
//
BOOL UClass::SetClassVTable( void *NewVTable, BOOL Checked )
{
	guard(UClass::SetClassVTable);
	if( ResVTablePtr != NULL )
	{
		checkState(NewVTable==NULL || NewVTable==ResVTablePtr);
		return 1;
	}
	if( NewVTable==NULL )
	{
		// Find the intrinsic implementation.
		if( ClassFlags & CLASS_Intrinsic )
		{
			// Form the name of the procedure entry point we're looking for.
			char ProcName[256];
			sprintf( ProcName, "autoclass%s%s", IsChildOf("Actor") ? "A" : "U", GetName() );

			// Form the DLL name.
			char DLLName[256];
			sprintf( DLLName, "%s.DLL", PackageName!=NAME_None ? PackageName() : "UnEngine" );

			// Look in the DLL.
			UClass *ClassPtr = (UClass *)GApp->GetProcAddress( DLLName, ProcName, 0 );
			if( !ClassPtr )
				appErrorf( "Couldn't find '%s' in DLL '%s'", ProcName, DLLName );

			// Found it in the game DLL.
			debugf( "Loaded class '%s' from '%s'", GetName(), DLLName );
			ResVTablePtr = ClassPtr->ResVTablePtr;
		}
		else if( ParentClass )
		{
			// Chase down vtable in parent class.
			ParentClass->SetClassVTable( ParentClass->ResVTablePtr );
			ResVTablePtr = ParentClass->ResVTablePtr;
		}
		else
		{
			if( Checked )
				appErrorf( "Can't find vftable for Class %s", GetName() );
			return 0;
		}
	}
	else
	{
		// Use the pointer that was specified.
		ResVTablePtr = NewVTable;
	}
	return 1;
	unguardobj;
}

//
// Fill this class's properties with its parent properties.  This is performed
// when a new class is created or recompiled, so that inheretance works seamlessly.
//
void UClass::AddParentProperties()
{
	guard(UClass::AddParentProperties);
	checkState(Bins[PROPBIN_PerObject]!=NULL);
	checkState(Bins[PROPBIN_PerClass]!=NULL);
	checkState(Num==0);
	checkState(Max==0);

	// Copy the property data.
	for( int i=0; i<PROPBIN_MAX; i++ )
	{
		if( ParentClass->Bins[i] )
		{
			checkState(i!=PROPBIN_PerFunction);
			checkState(Bins[i]!=NULL);
			Bins[i]->CopyDataFrom( ParentClass->Bins[i] );
		}
	}
	unguardobj;
}

//
// Delete a class and all its child classes.
// This is dangerous because actors may be sitting around that rely on the class.
//
void UClass::DeleteClass()
{
	guard(UClass::Delete);
	UClass *Class;

	Kill();
	FOR_ALL_TYPED_OBJECTS(Class,UClass)
	{
		if( Class->ParentClass == this )
			Class->ParentClass->DeleteClass();
	}
	END_FOR_ALL_TYPED_OBJECTS;
	unguardobj;
}

/*-----------------------------------------------------------------------------
	UClass UObject implementation.
-----------------------------------------------------------------------------*/

void UClass::InitHeader()
{
	guard(UClass::InitHeader);

	// Init parent.
	UDatabase::InitHeader();

	// Class info.
	ParentClass			= NULL;
	ScriptText			= NULL;
	Script				= NULL;
	StackTree			= NULL;
	Dependencies		= (UDependencies*)NULL;

	// Init property bins.
	for( int i=0; i<PROPBIN_MAX; i++ )
		Bins[i] = NULL;

	// Type info.
	ResVTablePtr		= NULL;
	ResFullHeaderSize   = 0;
	ResThisHeaderSize   = 0;
	ResRecordSize		= 0;
	ClassFlags			= 0;
	ResGUID[0]			= ResGUID[1] = ResGUID[2] = ResGUID[3] = 0;
	ResNextAutoReg		= NULL;

	// Names.
	PackageName			= NAME_None;

	unguardobj;
}
const char *UClass::Import( const char *Buffer, const char *BufferEnd,const char *FileType )
{
	guard(UClass::Import);
	char StrLine[256],Temp[256],TempName[NAME_SIZE];

	// Perform the importing.
	while( GetLINE(&Buffer,StrLine,256)==0 )
	{
		const char *Str=&StrLine[0];
		if( GetCMD(&Str,"DECLARECLASS") && GetSTRING(Str,"NAME=",Temp,NAME_SIZE) )
		{
			// Forward-declare a class, necessary because actor properties may refer to
			// classes which haven't been declared yet.
			if( !new(Temp,FIND_Optional)UClass )
				UClass *TempClass = new(Temp,CREATE_Unique)UClass(UObject::GetBaseClass());
		}
		else if( GetBEGIN(&Str,"CLASS") && GetSTRING(Str,"NAME=",TempName,NAME_SIZE) )
		{
			UClass *TempClass = new(TempName,FIND_Optional)UClass;
			if( TempClass && (TempClass->GetFlags() & RF_HardcodedRes ) )
			{
				// Gracefully update an existing hardcoded class.
				debugf( "Updating hardcoded class %s",TempClass->GetName() );
			}
			else TempClass = new(TempName,CREATE_Replace)UClass((UClass*)NULL);

			Buffer = TempClass->Import( Buffer, BufferEnd, FileType );
			if( !TempClass->Bins[PROPBIN_PerObject] )
			{
				debugf( "Empty import, killing Class %s", TempClass->GetName() );
				TempClass->Kill();
			}
		}
		else if( GetBEGIN(&Str,"TEXT") )
		{
			ScriptText = new(GetName(),CREATE_Replace,RF_NotForClient|RF_NotForServer)UTextBuffer;
			Buffer     = ScriptText->Import(Buffer,BufferEnd,FileType);
			if( GEditor ) GEditor->CompileScript( this, 0, 1 );
			//bug ("Script <%s> parent <%s>",Name,ParentClass?ParentClass->Name:"NONE");
		}
		else if( GetBEGIN(&Str,"DEFAULTPROPERTIES") && Bins[PROPBIN_PerObject] )
		{
			//bug ("Getting defaults...");
			BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
			WhichBins[PROPBIN_PerObject] = WhichBins[PROPBIN_PerClass] = 1;
			BYTE *ClassBins[PROPBIN_MAX]; GetClassBins(ClassBins);
			Buffer = ImportActorProperties
			(
				(ULevel*)NULL,
				this,
				ClassBins,
				Buffer,
				WhichBins,
				0
			);
		}
		else if (GetEND(&Str,"CLASS")) 
		{
			break;
		}
	}
	if( !Bins[PROPBIN_PerObject] || !Bins[PROPBIN_PerObject]->Num )
	{
		// Import failed.
		debugf( "Empty import, killing Script %s", GetName() );

		// Kill stuff.
		if( ScriptText )
			ScriptText->Kill(); ScriptText = NULL;

		// Kill properties.
		for( int i=0; i<PROPBIN_MAX; i++ )
			if( Bins[i] )
				Bins[i]->Kill();
	}
	//bug ("Finish importing %s",Name);

	return Buffer;
	unguardobj;
}
void UClass::Export( FOutputDevice &Out, const char *FileType, int Indent )
{
	guard(UClass::Export);
	checkState(Bins[PROPBIN_PerObject]!=NULL);
	checkState(Bins[PROPBIN_PerClass]!=NULL);

	static int RecursionDepth=0,i;

	if( !stricmp(FileType,"H") )
	{
		// Export as C++ header.
		if( RecursionDepth==0 )
		{
			Out.Logf
			(
				"/*===========================================================================\r\n"
				"	C++ \"%s\" class definitions exported from UnrealEd\r\n"
				"===========================================================================*/\r\n"
				"#pragma pack (push,4) /* 4-byte alignment */\r\n"
				"\r\n",
				GetName()
			);
		}

		// Text description.
		Out.Logf("///////////////////////////////////////////////////////\r\n// Class ");
		UClass *TempClass = this;
		while( TempClass )
		{
			Out.Logf("%s%s",TempClass->IsChildOf("Actor") ? "A" : "U", TempClass->GetName());
			TempClass = TempClass->ParentClass;
			if (TempClass) Out.Logf(":");
		}
		Out.Logf("\r\n///////////////////////////////////////////////////////\r\n\r\n");

		// Enum definitions.
		for( i=0; i<Num; i++ )
			if( Element(i).Type==CPT_EnumDef )
				Element(i).Enum->Export(Out,FileType,Indent);

		// Class definition.
		Out.Logf("class ");
		if( ClassFlags & CLASS_Intrinsic )
		{
			char API[256];
			sprintf(API,"%s_API ",PackageName());
			strupr(API);
			Out.Log(API);
		}
		Out.Logf("%s%s", IsChildOf("Actor") ? "A" : "U", GetName());
		if( ParentClass )
		{
			Out.Logf(" : public %s%s", ParentClass->IsChildOf("Actor") ? "A" : "U", ParentClass->GetName() );
		}

		// Opening.
		Out.Logf(" {\r\npublic:\r\n" );

		// All per-object properties defined in this class.
		for( i=0; i<Num; i++ )
		{
			if
			(	(Element(i).Bin==PROPBIN_PerObject)
			&&	(Element(i).ElementSize>0) )
			{
				Out.Logf(spc(Indent+4));
				Element(i).ExportH(Out);
				Out.Logf("\r\n");
			}
		}

		// Code.
		if( ClassFlags & CLASS_Intrinsic )
		{
			Out.Logf( "    enum {BaseFlags = CLASS_Intrinsic " );
			if( ClassFlags & CLASS_ScriptWritable )
				Out.Log(" | CLASS_ScriptWritable" );
			if( ClassFlags & CLASS_NoEditParent   )
				Out.Log(" | CLASS_NoEditParent"   );
			if( ClassFlags & CLASS_Transient      )
				Out.Log(" | CLASS_Transient"      );
			Out.Log("};\r\n");
			Out.Logf
			(
				"    DECLARE_CLASS(%s%s,%s%s,NAME_%s,NAME_%s)\r\n",
				IsChildOf("Actor") ? "A" : "U", GetName(),
				ParentClass->IsChildOf("Actor") ? "A" : "U", ParentClass->GetName(),
				(PackageName.GetFlags() & RF_HardcodedName) ? GetName()     : "None",
				(PackageName.GetFlags() & RF_HardcodedName) ? PackageName() : "None"
			);
			Out.Logf( "    #include \"%s%s.h\"\r\n", IsChildOf("Actor") ? "A" : "U", GetName() );
		}
		Out.Logf("};\r\n\r\n");
	}
	else if( !stricmp(FileType,"U"))
	{
		// Export as actor class text.
		checkState(ScriptText!=NULL);
		if( RecursionDepth==0 )
		{
			// Class declarations.
			UClass *TempClass;
			FOR_ALL_TYPED_OBJECTS(TempClass,UClass)
			{
				if( TempClass->GetFlags() & (RF_TagImp|RF_TagExp) )
					Out.Logf( "DeclareClass Name=%s\r\n", TempClass->GetName() );
			}
			END_FOR_ALL_TYPED_OBJECTS;
			Out.Logf( "\r\n" );
		}
		const char TEXT_SEPARATOR[] =
			"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
			"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\r\n";

		// Class CLASSNAME Expands PARENTNAME [Intrinsic].
		Out.Logf("Begin Class Name=%s\r\n",GetName());
		Out.Logf("   Begin Text\r\n");
		Out.Logf(TEXT_SEPARATOR);

		ScriptText->Export(Out,FileType,Indent);

		Out.Logf("\r\n");
		Out.Logf(TEXT_SEPARATOR);
		Out.Logf("   End Text\r\n");

		// Default properties.
		Out.Logf("   Begin DefaultProperties\r\n");

		// Export only default properties that differ from parent's.
		BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
		WhichBins[PROPBIN_PerObject] = WhichBins[PROPBIN_PerClass] = 1;
		BYTE *ClassBins [PROPBIN_MAX]; GetClassBins(ClassBins);
		ExportActor( this, ClassBins, Out, NAME_None, Indent+5, 0, 0, ParentClass, 1, -1, NAME_None, WhichBins );

		// End:
		Out.Logf("   End DefaultProperties\r\n");
		Out.Logf("End Class\r\n\r\n");
	}

	// Export all child classes that are tagged for export.
	UClass *ChildClass;
	FOR_ALL_TYPED_OBJECTS(ChildClass,UClass)
	{
		if( (ChildClass->ParentClass==this) && (ChildClass->GetFlags() & RF_TagExp) )
		{
			RecursionDepth++;
			ChildClass->Export(Out,FileType,Indent);
			RecursionDepth--;
		}
	}
	END_FOR_ALL_TYPED_OBJECTS;

	if( !stricmp(FileType,"H") && RecursionDepth==0 )
	{
		// Finish C++ header.
		Out.Logf("#pragma pack (pop) /* Restore alignment to previous setting */\r\n");
	}
	unguardobj;
}
void UClass::PostLoadHeader( DWORD PostFlags )
{
	guard(UClass::PostLoadHeader);

	// Postload parent class.
	UDatabase::PostLoadHeader(PostFlags);

	// Init pointers.
	SetClassVTable();

	unguardobj;
};
IMPLEMENT_DB_CLASS(UClass);

/*-----------------------------------------------------------------------------
	UClass constructors.
-----------------------------------------------------------------------------*/

//
// Create a new UClass given its parent.
//
UNENGINE_API UClass::UClass( UClass *InParentClass )
{
	guard(UClass::UClass);

	// Subobjects.
	ParentClass		= InParentClass;
	ScriptText		= NULL;
	Script			= NULL;
	StackTree		= NULL;
	Dependencies	= (UDependencies*)NULL;

	// Set default flags.
	ClassFlags |= CLASS_RecompileSet;

	// Allocate bins.
	AllocBins();

	// Copy information structure from parent.
	if( ParentClass )
		AddParentProperties();

	// Create dependency list.
	Dependencies = new( GetName(), CREATE_Replace )UDependencies(0);

	// Fill dependency list.
	Dependencies->AddItem(FDependency(this,0));
	if( ParentClass )
		Dependencies->AddItem(FDependency(ParentClass,0));

	// Find the actor function associated with this new class.
	ResFullHeaderSize = ResThisHeaderSize = 0;
	if( ParentClass )
	{
		SetClassVTable();
		ResFullHeaderSize += ParentClass->ResFullHeaderSize;
		ResRecordSize	   = ParentClass->ResRecordSize;
	}

	// Done creating class.
	unguardobj;
}

//
// UClass autoregistry constructor.
// Classes compiled in the main engine DLL will have hardcoded names.
// Classes compiled outside the main engine DLL will not have hardcoded names.
//warning: Called at DLL init time.
//
UClass::UClass
(
	UObject	*Template,
	DWORD	InHeaderSize,
	DWORD	InRecordSize,
	DWORD	InClassFlags,
	UClass	*InParentClass,
	DWORD	A,
	DWORD	B,
	DWORD	C,
	DWORD	D,
	FName	InName,
	FName	InPackageName
)
{
	// Init the base object.
	InitObject( NULL, INDEX_NONE, InName, RF_HardcodedRes );
	PackageName = InPackageName;

	// Init UDatabase info.
	Num					= 0;
	Max					= 0;

	// Set info.
	ResVTablePtr		= *(void**)Template;
	ResFullHeaderSize	= InHeaderSize;
	ResRecordSize		= InRecordSize;
	ClassFlags			= InClassFlags;
	ParentClass			= InParentClass!=this ? InParentClass : NULL;

	// Init GUID.
	ResGUID[0]			= A;
	ResGUID[1]			= B;
	ResGUID[2]			= C;
	ResGUID[3]			= D;

	// Add to the autoregistry chain, if and only if a name was specified.
	if( InName != NAME_None )
	{
		ResNextAutoReg                     = FGlobalObjectManager::AutoRegister;
		FGlobalObjectManager::AutoRegister = this;
	}
}

/*-----------------------------------------------------------------------------
	Link topic function.
-----------------------------------------------------------------------------*/

// Quicksort callback for sorting classes by name.
int CDECL ClassSortCompare( const void *elem1, const void *elem2 )
{
	return stricmp((*(UClass**)elem1)->GetName(),(*(UClass**)elem2)->GetName());
}

AUTOREGISTER_TOPIC( "Class", ClassTopicHandler );
void ClassTopicHandler::Get( ULevel *Level, const char *Item, FOutputDevice &Out )
{
	guard(ClassTopicHandler::Get);
	UClass	*Class;
	enum	{MAX_RESULTS=1024};
	int		NumResults = 0;
	UClass	*Results[MAX_RESULTS];

	if( GetCMD(&Item,"QUERY") )
	{
		UClass *Parent = NULL, *Class;
		GetUClass(Item,"PARENT=",Parent);

		// Make a list of all child classes.
		FOR_ALL_TYPED_OBJECTS(Class,UClass)
		{
			if( Class->ParentClass==Parent && NumResults<MAX_RESULTS)
				Results[NumResults++] = Class;
		}
		END_FOR_ALL_TYPED_OBJECTS;

		// Sort them by name.
		qsort(Results,NumResults,sizeof(UClass*),ClassSortCompare);

		// Return the results.
		for( int i=0; i<NumResults; i++ )
		{
			// See if this item has children.
			int Children = 0;
			FOR_ALL_TYPED_OBJECTS(Class,UClass)
			{
				if( Class->ParentClass==Results[i] )
					Children++;
			}
			END_FOR_ALL_TYPED_OBJECTS;

			// Add to result string.
			if( i>0 ) Out.Log(",");
			Out.Logf
			(
				"%s%s|%s",
				(Results[i]->PackageName==NAME_UnEngine || Results[i]->PackageName==NAME_UnGame) ? "*" : "",
				Results[i]->GetName(),
				Children ? "C" : "X"
			);
		}
	}
	else if( GetCMD(&Item,"EXISTS") )
	{
		if (GetUClass(Item,"NAME=",Class)) Out.Log("1");
		else Out.Log("0");
	}
	else if( GetCMD(&Item,"STATES") )
	{
		UClass* Class;
		if( GetUClass( Item, "CLASS=", Class ) && Class->StackTree )
		{
			int n=0;
			for( FStackNodePtr NodePtr=Class->StackTree->Element(0).ChildStates; NodePtr.Class; NodePtr=NodePtr->Next )
			{
				if( NodePtr->StackNodeFlags & SNODE_EditableState )
				{
					if( n++ > 0 ) Out.Log( "," );
					Out.Log( NodePtr->Name() );
				}
			}
		}
	}
	else if( GetCMD(&Item,"PACKAGE") )
	{
		UClass *Class;
		if( GetUClass( Item, "CLASS=", Class ) )
			Out.Log( Class->PackageName() );
	}
	unguard;
}
void ClassTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(ClassTopicHandler::Set);
	unguard;
}

/*-----------------------------------------------------------------------------
	Property functions.
-----------------------------------------------------------------------------*/

//
// Add a property to the class.  Verifies that all required members are
// set to valid values, and adjusts any of this property's values that need to be adjusted
// based on the class, i.e. by merging adjacent bit flags into DWORD's.  Returns pointer
// to the property in the frame if success, NULL if property could not be added.  Any 
// modifications are applied to this class property before adding it to the frame, 
// so the value of this property upon return are sufficient for initializing the data by 
// calling InitPropertyData.
//
FProperty &UClass::AddProperty( FProperty &Property, BYTE *&Data )
{
	guard(UClass::AddProperty);
	checkInput(Property.Type!=0);
	checkState(Bins[Property.Bin]!=NULL);

	// Set any special items required.
	switch( Property.Type )
	{
		case CPT_Byte:
		case CPT_Int:
		case CPT_Float:
		case CPT_Object:
		case CPT_Name:
		case CPT_Vector:
		case CPT_Rotation:
		case CPT_EnumDef:
			if     ( Property.ElementSize==2 )	Property.Offset = Align(Bins[Property.Bin]->Num,2);
			else if( Property.ElementSize>=4 )	Property.Offset = Align(Bins[Property.Bin]->Num,4);
			else								Property.Offset = Bins[Property.Bin]->Num;
			break;

		case CPT_String:
			Property.Offset = Bins[Property.Bin]->Num;
			break;

		case CPT_Bool:
			if
			(	(Num>0)
			&&	(Property.Bin!=PROPBIN_PerFunction)
			&&	(Property.ArrayDim==1)
			&&	(Element(Num-1).ArrayDim==1)
			&&	(Element(Num-1).Type==CPT_Bool)
			&&	(Element(Num-1).BitMask<<1) )
			{
				// Continue bit flag from the previous DWORD.
				Property.BitMask = Element(Num-1).BitMask << 1;
				Property.Offset  = Element(Num-1).Offset;
			}
			else
			{
				// Create a new DWORD bit mask for this bit field.
				Property.Offset  = Align(Bins[Property.Bin]->Num,4);
				Property.BitMask = 1;
			}
			break;

		default:
			appErrorf("Unknown property type",Property.Name());
			break;
	}

	// Set the property.
	int iProperty = AddItem(Property);

	// Add zero-filled space in the bin so that booleans pad properly.
	int NewBytes = Property.Offset + Property.Size() - Bins[Property.Bin]->Num;
	Bins[Property.Bin]->Add(NewBytes);
	for( int i=Bins[Property.Bin]->Num - NewBytes; i<Bins[Property.Bin]->Num; i++ )
		Bins[Property.Bin]->Element(i)=0;

	// Get data pointer to return.
	Data = Bins[Property.Bin]->Get(Property);

	// Return the added property.
	return Element(iProperty);
	unguardobj;
}

//
// Shrink the class and its subobjects.
//
void UClass::Finish()
{
	guard(UClass::Finish);

	// Shrink the class.
	UDatabase::Shrink();

	// Shrink subobjects.
	if( ScriptText   ) ScriptText  ->Shrink();
	if( Script       ) Script      ->Shrink();
	if( StackTree    ) StackTree   ->Shrink();
	if( Dependencies ) Dependencies->Shrink();

	// Tail 4-align and shrink the property data.
	for( int i=0; i<PROPBIN_MAX; i++ )
	{
		if( Bins[i] )
		{
			Bins[i]->Add( ((Bins[i]->Num+3)&~3) - Bins[i]->Num );
			Bins[i]->Shrink();
		}
	}

	// Set resource properties.
	ResFullHeaderSize = ResThisHeaderSize = Bins[PROPBIN_PerObject] ? Bins[PROPBIN_PerObject]->Num : 0;
	if( ParentClass )
		ResThisHeaderSize -= ParentClass->ResFullHeaderSize;

	// Init record size.
	ResRecordSize = 0;

	unguardobj;
}

/*-----------------------------------------------------------------------------
	Bin serialization.
-----------------------------------------------------------------------------*/

//
// Serialize all of the class's data that belongs in a particular
// bin and resides in Data.
//
void UClass::SerializeBin( FArchive &Ar, EPropertyBin Bin, void *Data, void *Defaults )
{
	FName PropertyName = NAME_None;
	guard(UClass::SerializeBin);
	//debugf("Serializing class %s, bin %i (%i total):",GetName(),Bin,Num);

	// Serialize each property that belongs in the bin.
	for( FPropertyIterator It(this); It; ++It )
	{
		FProperty &Property = It();
		if( Property.Bin==Bin )
		{
			// Get data pointer.
			PropertyName  = Property.Name;
			BYTE *Value   = (BYTE *)Data + Property.Offset;
			BYTE *Default = Defaults ? ((BYTE*)Defaults + Property.Offset) : NULL;
			Property.SerializeProperty( Ar, Value, Default );
			if( (Property.Flags & CPF_Transient) && Ar.IsLoading() )
			{
				// Zero-fill the non-intrinsic transient data.
				if( Property.Type!=CPT_Bool || Property.BitMask == 1 )
					memset( Value, 0, Property.ArrayDim * Property.ElementSize );
				else
					*(DWORD*)Value &= ~Property.BitMask;
			}
		}
	}
	unguardf(( "(class %s, property %s, bin %i)", GetName(), PropertyName(), Bin ));
}

/*-----------------------------------------------------------------------------
	Pushing/popping.
-----------------------------------------------------------------------------*/

//
// Push the class's contents.
//
void UClass::Push( FSavedClass &Saved, FMemStack &Mem )
{
	guard(UClass::Push);

	for( int i=PROPBIN_MAX-1; i>=0; i-- )
		if( Bins[i] )
			Bins[i]->Push(Saved.SavedBins[i],Mem);

	if( StackTree    ) StackTree->Push   (Saved.SavedStackTree,   Mem);
	if( Script       ) Script->Push      (Saved.SavedScript,      Mem);
	if( Dependencies ) Dependencies->Push(Saved.SavedDependencies,Mem);
	UObject::Push                      (Saved.SavedClass,       Mem);

	unguardobj;
}

//
// Pop the class's contents.
//
void UClass::Pop( FSavedClass &Saved )
{
	guard(UClass::Pop);
	UObject::Pop                        (Saved.SavedClass       );
	if( Dependencies ) Dependencies->Pop(Saved.SavedDependencies);
	if( Script       ) Script->Pop      (Saved.SavedScript      );
	if( StackTree    ) StackTree->Pop   (Saved.SavedStackTree   );

	for( int i=0; i<PROPBIN_MAX; i++ )
		if( Bins[i] )
			Bins[i]->Pop(Saved.SavedBins[i]);

	unguardobj;
}

/*-----------------------------------------------------------------------------
	UProperties implementation.
-----------------------------------------------------------------------------*/

//
// UObject interface.
//
void UProperties::InitHeader()
{
	guard(UProperties::InitHeader);

	// Init parent.
	UDatabase::InitHeader();

	// Init UProperties info.
	Class = NULL;
	Bin   = PROPBIN_PerObject;

	unguardobj;
}
void UProperties::SerializeData( FArchive& Ar )
{
	guard(UProperties::SerializeData);
	//debugf("Serializing bin %s (%i): %i bytes",GetName(),Bin,Num);

	// Make sure the class's data is loaded and linked, because
	// we'll need it while we're serializing.
	for( UClass *LoadClass=Class; LoadClass; LoadClass=LoadClass->ParentClass )
		Ar.Preload( LoadClass );

	// Serialize all properties.
	Class->SerializeBin( Ar, Bin, &Element(0), 0 );

	unguardobj;
}
IMPLEMENT_DB_CLASS(UProperties);

/*-----------------------------------------------------------------------------
	UDependencies.
-----------------------------------------------------------------------------*/

//
// FDependency constructors.
//
FDependency::FDependency()
{}
FDependency::FDependency( UClass* InClass )
:	Class			( InClass )
,	ScriptTextCRC	( (Class && Class->ScriptText) ? Class->ScriptText->DataCRC() : 0 )
{}
FDependency::FDependency( UClass* InClass, DWORD InScriptTextCRC )
:	Class(InClass)
,	ScriptTextCRC(InScriptTextCRC)
{}

//
// FDepdendency inlines.
//
BOOL FDependency::IsUpToDate()
{
	guard(FDependency::IsUpToDate);
	checkState(Class!=NULL);
	checkState(Class->Script!=NULL);
	return Class->ScriptText->DataCRC()==ScriptTextCRC;
	unguard;
}

IMPLEMENT_DB_CLASS(UDependencies);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
