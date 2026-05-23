/*=============================================================================
	UnClass.cpp: Actor class functions

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

//
// Temporary.
//
class AScript : public AActor
{
	public:
	INT Process(ILevel *Level, FName Message, void *Params)
	{
		//appError("AScript::Process");
		return 0;
	}
};

//
// Globals for linktopic function.
//
extern int 	GNumResults;
extern int 	GCurResult;
UClass  	**GTempClassList       		= NULL;
WORD  		*GTempClassChildCount 		= NULL;

#define MAX_RES_RESULTS 4096

/*-----------------------------------------------------------------------------
	UClass implementation.
-----------------------------------------------------------------------------*/

//
// Find the actor function for this class.  The actor function either resides
// in the actor DLL, or it's assumed to be the generic script-executor actor function.
//
void UClass::FindActorFunc()
{
	void **Temp = (void **)GApp->GetProcAddress(GAME_DLL,Name,0);

	if (Temp!=NULL) 
		*(void **)&ActorFunc = *Temp;
	else
		ActorFunc = (AActorBase::ACTOR_PROCESS_FUNC)AScript::Process;
}

//
// Fill this class's properties with its parent properties.  This is performed
// when a new class is created or recompiled, so that inheretance works seamlessly.
//
void UClass::AddParentProperties()
{
	guard(UClass::AddParentProperties);

	if( ParentClass )
	{
		Num	= ParentClass->Num;

		if( Num >= Max )
			appErrorf ("AddParentProperties: %s full",Name);

		for( int i=0; i<Num; i++ )
		{
			// Get property pointers.
			FClassProperty	*Property 		 = &Element(i);
			FClassProperty	*ParentProperty	 = &ParentClass->Element(i);

			// Copy the property.
			*Property	 	 = *ParentProperty;

			// Note that it came from the parent.
			Property->Flags |= CPF_FromParent;

			// Copy default per-actor properties.
			if( Property->IsPerActor() )
			{
				void	*Default  		= DefaultActor.GetPropertyPtrFromClass(this,i,0);
				void	*ParentDefault	= ParentClass->DefaultActor.GetPropertyPtr(i,0);
				memcpy (Default,ParentDefault,ParentProperty->PropertySize());
			}
		}
		DefaultActor.bTemplateClass=0;
	}
	unguard;
}

//
// Delete a class and all its child classes.
// This is dangerous because actors may be sitting around that rely on the class.
//
void UClass::Delete()
{
	guard(UClass::Delete);
	UClass *Class;

	Kill();
	FOR_ALL_TYPED_RES(Class,RES_Class,UClass)
	{
		if (Class->ParentClass==this)
			Class->ParentClass->Delete();
	}
	END_FOR_ALL_TYPED_RES;
	unguard;
}

//
// Resource functions.
//
void UClass::Register( FResourceType *Type )
{
	guard(UClass::Register);

	Type->HeaderSize = sizeof (UClass);
	Type->RecordSize = sizeof (FClassProperty);
	Type->Version    = 1;
	Type->TypeFlags  = RTF_ScriptReferencable;

	strcpy (Type->Descr,"Class");
	
	unguard;
}
void UClass::InitHeader()
{
	guard(UClass::InitHeader);

	// Init resource header to defaults.
	ParentClass			= NULL;
	ScriptText			= NULL;
	Script				= NULL;

	// Property information.
	Num					= 0;
	Max					= MAX_CLASS_PROPERTIES;

	// Init the default actor.
	DefaultActor.Init(this);

	unguard;
}
void UClass::InitData()
{
	guard(UClass::InitData);
	Num = 0;
	unguard;
}
int UClass::QuerySize()
{
	guard(UClass::QuerySize);
	return Max * sizeof (FClassProperty);
	unguard;
}
int UClass::QueryMinSize()
{
	guard(UClass::QueryMinSize);
	return Num * sizeof (FClassProperty);
	unguard;
}
const char *UClass::Import( const char *Buffer, const char *BufferEnd,const char *FileType )
{
	guard(UClass::Import);
	char StrLine[256],Temp[256],TempName[NAME_SIZE];

	// Required by ImportActorProperties:
	DefaultActor.Class = this;

	while( GetLINE(&Buffer,StrLine,256)==0 )
	{
		const char *Str=&StrLine[0];
		if( GetCMD(&Str,"DECLARECLASS") && GetSTRING(Str,"NAME=",Temp,NAME_SIZE) )
		{
			// Forward-declare a class, necessary because actor properties may refer to
			// classes which haven't been declared yet.
			if (!new(Temp,FIND_Optional)UClass)
			{
				UClass *TempClass = new(Temp,CREATE_Replace)UClass;
			}
		}
		else if( GetBEGIN(&Str,"CLASS") && GetSTRING(Str,"NAME=",TempName,NAME_SIZE) )
		{
			UClass *TempClass = new(TempName,CREATE_Replace)UClass((UClass*)-1);
			TempClass->AllocData();

			Buffer = TempClass->Import(Buffer,BufferEnd,FileType);

			if (!TempClass->DefaultActor.Class)
			{
				debugf("Failed import, killing %s",Name);
				TempClass->Kill(); // Import failed
			}
		}
		else if( GetBEGIN(&Str,"TEXT") )
		{
			ScriptText = new(Name,CREATE_Replace)UTextBuffer;
			Buffer     = ScriptText->Import(Buffer,BufferEnd,FileType);
			CompileScript(this,0);
			//bug ("Script <%s> parent <%s>",Name,ParentClass?ParentClass->Name:"NONE");
		}
		else if( GetBEGIN(&Str,"DEFAULTPROPERTIES") && DefaultActor.Class )
		{
			//bug ("Getting defaults...");
			Buffer = ImportActorProperties(&DefaultActor,Buffer);
		}
		else if (GetEND(&Str,"CLASS")) 
		{
			break;
		}
	}
	if( Num==0 )
	{
		// Import failed.
		debugf("Empty import, killing script %s",Name);
		if (ScriptText) ScriptText->Kill();
		ScriptText = NULL;
	}
	//bug ("Finish importing %s",Name);
	return Buffer;
	unguard;
}
char *UClass::Export(char *Buffer,const char *FileType,int Indent)
{
	guard(UClass::Export);
	static int RecursionDepth=0,i;

	int FirstProp=0; if (ParentClass) FirstProp=ParentClass->Num;
	if( !stricmp(FileType,"H") )
	{
		// Export as C++ header.
		if( RecursionDepth==0 )
		{
			Buffer += sprintf(Buffer,
				"/*===========================================================================\r\n"
				"	C++ \"%s\" actor class definitions exported from UnrealEd\r\n"
				"===========================================================================*/\r\n"
				"#pragma pack (push,4) /* 4-byte alignment */\r\n"
				"\r\n",
				Name);
		}

		// Text description.
		Buffer += sprintf(Buffer,"///////////////////////////////////////////////////////\r\n// Actor class ");
		UClass *TempClass = this;
		while( TempClass )
		{
			Buffer += sprintf(Buffer,"A%s",TempClass->Name);
			TempClass = TempClass->ParentClass;
			if (TempClass) Buffer += sprintf(Buffer,":");
		}
		Buffer += sprintf(Buffer,"\r\n///////////////////////////////////////////////////////\r\n\r\n");

		// Enum definitions.
		for (i=FirstProp; i<Num; i++)
		{
			if( (Element(i).Type==CPT_Byte) && Element(i).Enum 
				&& !(Element(i).Enum->Flags & RF_TagImp) )
			{
				Buffer = Element(i).Enum->Export(Buffer,FileType,Indent);

				// Don't export more than once.
				Element(i).Enum->Flags |= RF_TagImp;
			}
		}

		// class CLASSNAMEBase [: public PARENTCLASSBase]
		if (!ParentClass)	Buffer += sprintf(Buffer,"class UNREAL_API A%sBase",Name);
		else				Buffer += sprintf(Buffer,"class IMPLEMENTATION_API A%sBase : public A%sBase",Name,ParentClass->Name);
		Buffer += sprintf(Buffer," {\r\npublic:\r\n");

		// All properties.
		for( i=FirstProp; i<Num; i++ )
		{
			Buffer += sprintf(Buffer,spc(Indent+4));
			Buffer  = Element(i).ExportH(Buffer);
			Buffer += sprintf(Buffer,"\r\n");
		}
		if (!ParentClass) 
		{
			Buffer += sprintf(Buffer,"    #include \"UnRoot.h\"\r\n");
		}
		Buffer += sprintf(Buffer,"};\r\n");

		// class CLASSNAMEBase [: public PARENTCLASSBase]
		Buffer += sprintf(Buffer,"class IMPLEMENTATION_API A%s : public A%sBase {\r\npublic:\r\n",Name,Name);
		Buffer += sprintf(Buffer,"    BYTE PropertyPad[%i];\r\n",
			sizeof(AActorBase) + AActor::CLASS_PROP_EXTRA - PropertyFrameEndOffset(Num,&Element(0),0));
		Buffer += sprintf(Buffer,"    FActorPrivate Private;\r\n");

		if (Intrinsic)
		{
			Buffer += sprintf(Buffer,"    INT Process(ILevel *Level, FName Message, void *Params);\r\n");
		}

		Buffer += sprintf(Buffer,"};\r\n");

		// End.
		if (Intrinsic)
		{
			Buffer += sprintf(Buffer,"AUTOREGISTER_CLASS(A%s);\r\n\r\n",Name);
		}
	}
	else if( !stricmp(FileType,"TCX") )
	{
		// Export as actor class text.
		if (!ScriptText)
			appError ("Null ScriptText");
		
		if( RecursionDepth==0 )
		{
			Buffer += sprintf(Buffer,
				"'\r\n"
				"' Actor classes exported from UnrealEd\r\n"
				"'\r\n"
				);
			
			// Class declarations.
			UClass *TempClass;
			FOR_ALL_TYPED_RES(TempClass,RES_Class,UClass)
			{
				if ((TempClass->Flags & RF_TagExp) && TempClass->IsKindOf(this))
					Buffer += sprintf(Buffer,"DeclareClass Name=%s\r\n",TempClass->Name);
			}
			END_FOR_ALL_TYPED_RES;
			Buffer += sprintf(Buffer,"\r\n");
		}
		const char TEXT_SEPARATOR[] =
			"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
			"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\r\n";
		
		// Class CLASSNAME Expands PARENTNAME [Intrinsic].
		Buffer += sprintf(Buffer,"Begin Class Name=%s\r\n",Name);
		Buffer += sprintf(Buffer,"   Begin Text\r\n");
		Buffer += sprintf(Buffer,TEXT_SEPARATOR);
		Buffer  = ScriptText->Export(Buffer,FileType,Indent);
		Buffer += sprintf(Buffer,"\r\n");
		Buffer += sprintf(Buffer,TEXT_SEPARATOR);
		Buffer += sprintf(Buffer,"   End Text\r\n");

		// Default properties.
		Buffer += sprintf(Buffer,"   Begin DefaultProperties\r\n");
		if ((RecursionDepth==0) || (ParentClass==NULL))
		{
			// Export all default properties.
			Buffer += ExportActor (&DefaultActor,Buffer,NAME_NONE,Indent+5,0,0,NULL,1,-1,NAME_NONE);
		}
		else
		{
			// Export only default properties that differ from parent's.
			Buffer += ExportActor (&DefaultActor,Buffer,NAME_NONE,Indent+5,0,0,&ParentClass->DefaultActor,1,-1,NAME_NONE);
		}
		Buffer += sprintf(Buffer,"   End DefaultProperties\r\n");
		// EndClass:
		Buffer += sprintf(Buffer,"End Class\r\n\r\n");
	}

	// Export all child classes that are tagged for export.
	UClass *ChildClass;
	FOR_ALL_TYPED_RES(ChildClass,RES_Class,UClass)
	{
		if ((ChildClass->ParentClass==this) && (ChildClass->Flags & RF_TagExp))
		{
			RecursionDepth++;
			Buffer = ChildClass->Export(Buffer,FileType,Indent);
			RecursionDepth--;
		}
	}
	END_FOR_ALL_TYPED_RES;

	if ((!stricmp(FileType,"H")) && (RecursionDepth==0))
	{
		// Finish C++ header.
		Buffer += sprintf(Buffer,"#pragma pack (pop) /* Restore alignment to previous setting */\r\n");
	}
	return Buffer;

	unguard;
}
void UClass::QueryHeaderReferences(FResourceCallback &Callback)
{
	guard(UClass::QueryHeaderReferences);

	// Main references.
	Callback.Resource (this,(UResource **)&ParentClass,0);
	Callback.Resource (this,(UResource **)&ScriptText ,0);
	Callback.Resource (this,(UResource **)&Script     ,0);

	// References in the default actor.
	DefaultActor.QueryReferences(this,Callback,0,1);

	unguardf(("(%s,%i)",Name,(int)this));
}
void UClass::QueryDataReferences(FResourceCallback &Callback)
{
	guard(UClass::QueryDataReferences);

	// References in the class property list.
	for( int i=0; i<Num; i++ )
	{
		FClassProperty *Property = &Element(i);

		Callback.Name( this,&Property->Name,0 );
		Callback.Name( this,&Property->Category,0 );

		if( Property->Type == CPT_Byte )
		{
			// Byte properties may reference an enum resource.
			Callback.Resource(this,(UResource **)&Property->Enum,0);
		}
	}
	unguard;
}
void UClass::PostLoad()
{
	guard(UClass::PostLoad);
	Flip();
	FindActorFunc();
	DefaultActor.PostLoad(this,1);
	unguard;
};
void UClass::Flip()
{
	guard(UClass::Flip);
	//todo:
	UDatabase::Flip();
	unguard;
}
AUTOREGISTER_RESOURCE(RES_Class,UClass,0xB2D90853,0xCCD211cf,0x91360000,0xC028B992);

/*-----------------------------------------------------------------------------
	Global functions.
-----------------------------------------------------------------------------*/

//
// Create a new UClass given its parent.
//
UNREAL_API UClass::UClass(UClass *NewParentClass)
{
	guard(UClass::UClass);

	if (NewParentClass == (UClass*)-1) 
		NewParentClass=NULL;

	// Allocate the resource and its data.
	Max = MAX_CLASS_PROPERTIES;
	AllocData(1);

	// Resources.
	ParentClass		= NewParentClass;
	ScriptText		= NULL;
	Script			= NULL;

	// Copy information structure from parent.
	if (ParentClass)	AddParentProperties();
	else				DefaultActor.Init(this);

	DefaultActor.Class = this;
	DefaultActor.bTemplateClass = 0;

	// Find the actor function associated with this new class.
	FindActorFunc();

	//	Done creating class.
	unguard;
}

//
// Init actor classes and set up intrinsic classes (camera, light, etc).
//
void FGlobalClasses::Init()
{
	guard(FGlobalClasses::Init);

	// Make hardcoded names for all property types.
	FName Name;
	Name.AddHardcoded(CPT_Byte,		"Byte");
	Name.AddHardcoded(CPT_Integer,	"Integer");
	Name.AddHardcoded(CPT_Boolean,	"Boolean");
	Name.AddHardcoded(CPT_Real,		"Real");
	Name.AddHardcoded(CPT_Actor,	"Actor");
	Name.AddHardcoded(CPT_Resource,	"Resource");
	Name.AddHardcoded(CPT_Name,		"Name");
	Name.AddHardcoded(CPT_String,	"String");
	Name.AddHardcoded(CPT_Vector,	"Vector");
	Name.AddHardcoded(CPT_Rotation,	"Rotation");

	// Create the class list.
	mymemset (this,0,sizeof(FGlobalClasses));

	IntrinsicClasses = new("IntrinsicClasses",CREATE_Unique)TArray<UClass>(1024);
	GRes.AddToRoot(IntrinsicClasses);

	// Set all base classes to NULL.
	Actor = NULL;

	debugf(LOG_Info,"Actor classes initialized, size=%i",sizeof(AActor));
	unguard;
}

//
// Associate classes with related resources.
//
void FGlobalClasses::Associate()
{
	guard(FGlobalClasses::Associate);

	Actor			= new ("Actor",			FIND_Existing)UClass; IntrinsicClasses->Add(Actor);
	Camera			= new ("Camera",		FIND_Existing)UClass; IntrinsicClasses->Add(Camera);
	Light			= new ("Light",			FIND_Existing)UClass; IntrinsicClasses->Add(Light);
	LevelDescriptor	= new ("LevelDescriptor",FIND_Existing)UClass;IntrinsicClasses->Add(LevelDescriptor);

	unguard;
}

//
// Shut down all actor classes.
//
void FGlobalClasses::Exit()
{
	guard(FGlobalClasses::Exit);

	GRes.RemoveFromRoot(IntrinsicClasses);
	IntrinsicClasses->Kill();

	unguard;
}

/*-----------------------------------------------------------------------------
	Link topic function.
-----------------------------------------------------------------------------*/

//
// Quicksort callback for sorting classes by name.
//
int CDECL ClassSortCompare( const void *elem1, const void *elem2 )
{
	return stricmp((*(UClass**)elem1)->Name,(*(UClass**)elem2)->Name);
}

//
// Query a list of classes.  Call with resource or NULL=All.
//
void UNREAL_API classQueryForLink( UResource *Res )
{
	guard(classQueryForLink);
	UClass *Class;

	if (GTempClassList==NULL)
	{
		GTempClassList       = (UClass **)appMalloc(MAX_RES_RESULTS * sizeof (UResource *),"ClassQuery1");
		GTempClassChildCount = (WORD    *)appMalloc(MAX_RES_RESULTS * sizeof (WORD),"ClassQuery2");
	}
	GCurResult  = 0;
	GNumResults = 0;

	FOR_ALL_TYPED_RES(Class,RES_Class,UClass)
	{
		if ((Class->ParentClass==Res) || !Res)
		{
			if (GNumResults < MAX_RES_RESULTS)
			{
				GTempClassList[GNumResults++] = Class;
			}
		}
	}
	END_FOR_ALL_TYPED_RES;

	qsort(GTempClassList,GNumResults,sizeof(UClass*),ClassSortCompare);

	if (Res)
	{
		for (int i=0; i<GNumResults; i++)
		{
			GTempClassChildCount[i] = 0;

			FOR_ALL_TYPED_RES(Class,RES_Class,UClass)
			{
				if (Class->ParentClass==GTempClassList[i]) GTempClassChildCount [i]++;
			}
			END_FOR_ALL_TYPED_RES;
		}
	}
	unguard;
}

AUTOREGISTER_TOPIC("Class",ClassTopicHandler);
void ClassTopicHandler::Get(ULevel *Level, const char *Topic, const char *Item, char *Data)
{
	guard(ClassTopicHandler::Get);
	UClass	*Class;
	int		Children;

	if (!stricmp(Item,"QueryRes"))
	{
		// Query results.
		if (GCurResult < GNumResults)
		{
			Class    = GTempClassList       [GCurResult];
			Children = GTempClassChildCount [GCurResult];
			sprintf(Data,"%s%s|%s",Class->Intrinsic?"":"*", Class->Name, (Children==0)?"X":"C");
			GCurResult++;
		}
	}
	else if (!_strnicmp(Item,"EXISTS",6))
	{
		if (GetUClass(Item,"NAME=",&Class)) strcpy (Data,"1");
		else strcpy (Data,"0");
	}
	unguard;
}
void ClassTopicHandler::Set(ULevel *Level, const char *Topic, const char *Item, const char *Data)
{
	guard(ClassTopicHandler::Set);
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
