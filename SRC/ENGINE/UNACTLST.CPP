/*=============================================================================
	UnActor.cpp: UActorList implementation

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	UActorList exportation helpers.
-----------------------------------------------------------------------------*/

//
// Set the text type, name, and value of a single actor property.  Used for
// exporting actor properties.
//
void ExportActorProperty
(
	char			*Type,
	char			*Name,
	char			*Value,
	int				iProperty,
	AActor			*Actor, 
	int				Flags, 
	int				Descriptive, 
	int				ArrayElement, 
	AActor			*Delta, 
	FName			Category
)
{
	guard(ExportActorProperty);

	// Init caller values.
	Type[0]=0; 
	Name[0]=0; 
	Value[0]=0;

	// Get property pointer.
	FClassProperty *Property = &Actor->Class->Element(iProperty);

	// Make sure property is valid.
	if (Property->Name.IsNone())
		appErrorf("Missing name in class %s %i",Actor->Class->Name,iProperty);

	BYTE *RawActor = (BYTE*) Actor;
	BYTE *PropertyValue = &RawActor
	[
		Property->Offset + ArrayElement * Property->ElementSize
	];
	if 
	(
		((Property->Flags & Flags) || !Flags) &&
		((!Delta) || !Property->Compare(Actor,Delta,ArrayElement)) &&
		((Category==NAME_NONE)||(Category==Property->Category))
	)
	{
		strcpy(Name,Property->Name.Name());

		switch( Property->Type )
		{
			case CPT_Byte:
			{
				BYTE Temp = *(BYTE *)PropertyValue;

				if (Property->Enum)
					sprintf(Type,"BYTE.%s",Property->Enum->Name);
				else
					strcpy(Type,"BYTE");

				if (Property->Enum && Descriptive)
					sprintf(Value,"%i - %s",Temp,Property->Enum->Element(Temp).Name());
				else
					sprintf(Value,"%i",Temp);
				break;
			}
			case CPT_Integer:
			{
				strcpy(Type,"INTEGER");
				sprintf(Value,"%i",*(INT *)PropertyValue);
				break;
			}
			case CPT_Real:
			{
				strcpy(Type,"REAL");
				sprintf(Value,"%+013.6f",*(FLOAT *)PropertyValue);
				break;
			}
			case CPT_String:
			{
				strcpy(Type,"STRING");
				if (Descriptive)
					sprintf(Value,"%s",(char *)PropertyValue);
				else
					sprintf(Value,"\"%s\"",(char *)PropertyValue);
				break;
			}
			case CPT_Boolean:
			{
				strcpy(Type,"BOOLEAN");
				char *Temp = ((*(DWORD *)PropertyValue) & Property->BitMask) ? "True" : "False";
				sprintf(Value,"%s",Temp);
				break;
			}
			case CPT_Actor:
			{
				strcpy(Type,"ACTOR");
				INDEX Temp = *(INDEX *)PropertyValue;
				if (Temp!=INDEX_NONE)
					sprintf(Value,"%i",Temp);
				else
					sprintf(Value,"None");
				break;
			}
			case CPT_Resource:
			{
				strcpy(Type,GRes.GetTypeName(Property->ResType));
				UResource *Temp = *(UResource **)PropertyValue;
				if (Temp)
					strcpy(Value,Temp->Name);
				else
					strcpy(Value,"None");
				break;
			}
			case CPT_Name:
			{
				strcpy(Type,"NAME");
				FName Temp = *(FName *)PropertyValue;
				strcpy(Value,Temp.Name());
				break;
			}
			case CPT_Vector:
			{
				strcpy(Type,"VECTOR");
				FVector *Temp = (FVector *)PropertyValue;
				sprintf(Value,"(%+013.6f,%+013.6f,%+013.6f)",Temp->X,Temp->Y,Temp->Z);
				break;
			}
			case CPT_Rotation:
			{
				strcpy(Type,"ROTATION");
				FRotation *TempRot = (FRotation *)PropertyValue;
				sprintf(Value,"(%i,%i,%i)",TempRot->Pitch,TempRot->Yaw,TempRot->Roll);
				break;
			}
			default:
			{
				appErrorf("ExportActor: Unknown type in class %s",Actor->Class->Name);
				break;
			}
		}
	}
	unguard;
}

//
// Export one entire actor, or the specified named property, to text.
//
int ExportActor
(
	AActor		*Actor,
	char		*Ptr,
	FName		PropertyName,
	int			Indent,
	int			Descriptive,
	int			Flags, 
	AActor		*Delta,
	int			Resources,
	int			ArrayElement,
	FName		Category
)
{
	guard(ExportActor);
	char			*NewPtr	= Ptr;
	char			Type[128],Name[128],Value[128];
	FClassProperty	*Property;

	// Export actor.

	Property = &Actor->Class->Element(0);
	for (int i=0; i<Actor->Class->Num; i++)
	{
		if( (PropertyName.IsNone()) || (PropertyName==Property->Name) )
		{
			if( !(Property->Flags & CPF_Array) )
			{
				// Export single element.
				ExportActorProperty(Type,Name,Value,i,Actor,Flags,Descriptive,0,Delta,Category);
				if (Type[0] && Name[0])
				{
					if( Resources && (Property->Type==CPT_Resource) &&
						(Property->Flags & CPF_ExportResource) )
					{
						UResource *Res = *(UResource **)Actor->GetPropertyPtr(i);
						if (Res && !(Res->Flags & RF_TagImp))
						{
							// Don't export more than once.
							NewPtr = Res->Export(NewPtr,"",Indent+1);
							Res->Flags |= RF_TagImp;
						}
					}
					if (Descriptive!=2) NewPtr += sprintf(NewPtr,"%s%s %s=%s\r\n",spc(Indent),Descriptive?Type:"",Name,Value);
					else                NewPtr += sprintf(NewPtr,"%s",Value);
					//bug("%s%s %s=%s\r\n",spc(Indent),Descriptive?Type:"",Name,Value);
				}
			}
			else
			{
				// Export array.
				for( INT j=0; j<Property->ArrayDim; j++ )
				{
					ExportActorProperty(Type,Name,Value,i,Actor,Flags,Descriptive,j,Delta,Category);
					if( Type[0] && Name[0] && ((ArrayElement==-1) || (ArrayElement==(int)j)) )
					{
						if( Resources && (Property->Type==CPT_Resource) &&
							(Property->Flags & CPF_ExportResource) )
						{
							UResource *Res = *(UResource **)Actor->GetPropertyPtr(i,j);
							if (Res && !(Res->Flags & RF_TagImp))
							{
								// Don't export more than once.
								NewPtr = Res->Export(NewPtr,"",Indent+1);
								Res->Flags |= RF_TagImp;
							}
						}
						if (Descriptive!=2) NewPtr += sprintf(NewPtr,"%s%s %s(%i)=%s\r\n",spc(Indent),Descriptive?Type:"",Name,j,Value);
						else                NewPtr += sprintf(NewPtr,"%s",Value);
					}
				}
			}
		}
		Property++;
	}
	return (int)(NewPtr-Ptr);
	unguard;
}

//
// Export multiple selected actors for editing.  This sends only the properties that are
// shared between all of the selected actors, and the text values is sent as a blank if
// the property values are not all identical.
//
// Only exports CPF_Edit (editable) properties.
// Does not export arrays properly, except for strings (only exports first element of array).
// Assumes that the script compiler prevents non-string arrays from being declared as editable.
//
int ExportMultipleActors
(
	UActorList	*Actors,
	char		*Ptr,
	FName		PropertyName,
	int			Indent,
	int			Descriptive,
	FName		Category
)
{
	guard(ExportMultipleActors);

	AActor			*FirstActor=NULL,*Actor;
	char			*NewPtr	= Ptr;
	int				iActor,i,j,iStart=-1;
	char			FirstType[128],FirstName[128],FirstValue[128];
	char			Type[128],Name[128],Value[128];

	// Find first actor.
	for (iActor=0; iActor<Actors->Max; iActor++)
	{
		FirstActor = &Actors->Element(iActor);
		if (FirstActor->Class && FirstActor->bSelected)
		{
			iStart = iActor+1;
			break;
		}
	}

	// At least one actor is selected.
	if (iStart>=0)
	{
		// Find property in first actor.
		FClassProperty *FirstProperty = &FirstActor->Class->Element(0);
		for (i=0; i<FirstActor->Class->Num; i++)
		{
			if ( ((PropertyName.IsNone()) || (PropertyName==FirstProperty->Name))
			&&	(!(FirstProperty->Flags & CPF_Array)) )
			{
				ExportActorProperty(FirstType,FirstName,FirstValue,i,FirstActor,CPF_Edit,Descriptive,0,NULL,Category);
				if( FirstType[0] && FirstName[0] )
				{
					// Now go through all other actors and see if this property is shared.
					for( iActor=iStart; iActor<Actors->Max; iActor++ )
					{
						Actor = &Actors->Element(iActor);
						if( Actor->Class && Actor->bSelected )
						{
							FClassProperty *Property = &Actor->Class->Element(0);
							for( j=0; j<Actor->Class->Num; j++ )
							{
								if( Property->Name == FirstProperty->Name )
								{
									if( Property->ArrayDim != 1 )
										goto NextProperty;
									
									ExportActorProperty
									(
										Type,
										Name,
										Value,
										j,
										Actor,
										CPF_Edit,
										Descriptive,
										0,
										NULL,
										Category
									);
									
									if( stricmp(Type,FirstType) || stricmp(Name,FirstName) )
									{					
										// Mismatch.					
										goto NextProperty;
									}
									if (stricmp(Value,FirstValue))
									{
										// Blank out value if not identical.
										FirstValue[0]=0;
									}
								}
								Property++;
							}
						}
					}
					if( strcmp(Value,FirstValue)==0 )
					{
						NewPtr += sprintf(NewPtr,"%s%s %s=%s\r\n",spc(Indent),Descriptive?Type:"",Name,Value);
					}
					else
					{
						NewPtr += sprintf(NewPtr,"%s%s %s=\r\n",spc(Indent),Descriptive?Type:"",Name);
					}
				}
			}
			NextProperty:;
			FirstProperty++;
			}
	}
	return (int)(NewPtr-Ptr);
	unguard;
}

/*-----------------------------------------------------------------------------
	UActorList importation helpers.
-----------------------------------------------------------------------------*/

//
// Import all of the properties in Data into the specified actor.
// Data may contain multiple properties, but it might not contain all of 
// the properties for the actor, so the actor should be initialized
// first.
//
// This requires that the actor's Class be set in advance.
//
const char *ImportActorProperties( AActor *Actor, const char *Data )
{
	guard(ImportActorProperties);

	UClass	*Class		= Actor->Class;
	BYTE	*RawActor	= (BYTE *)Actor;
	char	*PropText	= (char *)GMem.Get(65536);
	char	*Top		= &PropText[0];
	char	*MemTop		= &PropText[0];

	// Parse all resources stored in the actor.
	// Build list of all text properties.

	*PropText = 0;
	char StrLine[256];
	while( GetLINE (&Data,StrLine,256)==0 )
	{
		const char *Str = &StrLine[0];
		if( GetBEGIN(&Str,"BRUSH") )
		{
			// Parse brush on this line.
			char BrushName[NAME_SIZE];

			if (GetSTRING(Str,"NAME=",BrushName,NAME_SIZE))
			{
				// If a brush with this name already exists in the
				// level, rename the existing one.  This is necessary
				// because we can't rename the brush we're importing without
				// losing our ability to associate it with the actor properties
				// that reference it.

				UModel *ExistingBrush = new(BrushName,FIND_Optional)UModel;
				if( ExistingBrush )
				{
					char TempName[NAME_SIZE];
					GRes.MakeUniqueName(TempName,BrushName,"",RES_Model);

					debugf(LOG_Info,"Renaming %s to %s",ExistingBrush->Name,TempName);
					strcpy(ExistingBrush->Name,TempName);

					if (ExistingBrush->Polys)
						strcpy(ExistingBrush->Polys->Name,TempName);
				}
				UModel *Brush = new(BrushName,CREATE_Unique)UModel;
				Data = Brush->Import(Data,NULL,"");
			}
		}
		else if (GetEND(&Str,"ACTOR") || GetEND(&Str,"DEFAULTPROPERTIES"))
		{
			// End of actor properties.
			break;
		}
		else
		{
			// More actor properties.
			strcpy(Top,Str);
			Top += strlen(Top);
		}
	}

	// Parse all text properties.
	FClassProperty	*Property = Class->GetData();
	for (int i=0; i<Class->Num; i++)
	{
		char LookFor[80];

		for( int j=0; j<Property->ArrayDim; j++ )
		{
			BYTE *Value	= &RawActor
				[
				Property->Offset + j*Property->ElementSize
				];

			if( Property->Flags & CPF_Array )
				sprintf(LookFor,"%s(%i)=",Property->Name.Name(),j);
			else
				sprintf(LookFor,"%s=",Property->Name.Name());

			switch( Property->Type )
			{
				case CPT_Byte:
					GetBYTE(PropText,LookFor,(BYTE *)Value);
					break;
				case CPT_Integer:
					GetINT(PropText,LookFor,(INT *)Value);
					break;
				case CPT_Boolean:
					int Result;
					if (GetONOFF(PropText,LookFor,&Result))
					{
						if (Result) *(DWORD *)Value |=  Property->BitMask;
						else		*(DWORD *)Value &= ~Property->BitMask;
					}
					break;
				case CPT_Real:
					GetFLOAT(PropText,LookFor,(FLOAT *)Value);
					break;
				case CPT_Actor:
					GetINDEX(PropText,LookFor,(INDEX *)Value);
					break;
				case CPT_Resource:
					GetRES(PropText,LookFor,Property->ResType,(UResource **)Value);
					break;
				case CPT_Name:
					GetNAME(PropText,LookFor,(FName *)Value);
					break;
				case CPT_String:
					GetSTRING(PropText,LookFor,(char *)Value,Property->ElementSize);
					break;
				case CPT_Vector:
					GetFVECTOR(PropText,LookFor,(FVector *)Value);
					break;
				case CPT_Rotation:
					GetFROTATION(PropText,LookFor,(FRotation *)Value,1);
					break;
				default:
					appErrorf("Bad class property type %i in %s",Property->Type,Class->Name);
					break;
			}
		}
		Property++;
	}
	GMem.Release(MemTop);
	return Data;

	unguard;
}

/*-----------------------------------------------------------------------------
	UActorList resource implementation.
-----------------------------------------------------------------------------*/

//
// Standard resource functions.
//
void UActorList::Register(FResourceType *Type)
{
	guard(UActorList::Register);

	Type->HeaderSize = sizeof (UActorList);
	Type->RecordSize = sizeof (AActor);
	Type->Version    = 1;
	strcpy (Type->Descr,"ActorList");
	
	unguard;
}
void UActorList::InitHeader()
{
	guard(UActorList::InitHeader);

	Max					= 0;
	Num					= 0;
	LockType			= LOCK_None;
    StaticActors        = 0;
    DynamicActors       = 0;
    CollidingActors     = 0;
    ActiveActors        = 0;
    UnusedActors        = 0;
    JustDeletedActors   = 0;
	
	unguard;
}
void UActorList::InitData()
{
	guard(UActorList::InitData);
	Num = 0;
	unguard;
}
const char *UActorList::Import(const char *Buffer, const char *BufferEnd,const char *FileType)
{
	guard(UActorList::Import);	
	char StrLine[256];

	// Allocate working memory for remapping.
	INDEX *Remaps  = (INDEX *)GMem.Get(Max * 4 * sizeof(INDEX));
	INDEX *IsRemap = (INDEX *)GMem.Get(Max *     sizeof(INDEX));

	for( int i=0; i<Max; i++ )
		IsRemap[i] = 0;

	for( i=0; i<Max*4; i++ )
		Remaps[i] = INDEX_NONE;

	// Empty the lists.
    StaticActors        ->Empty(TRUE);
    DynamicActors       ->Empty(TRUE);
    CollidingActors     ->Empty(TRUE);
    ActiveActors        ->Empty(TRUE);
    UnusedActors        ->Empty(TRUE);
    JustDeletedActors   ->Empty(TRUE);

	// Import a sparse actor list.
	AActor *Actor = NULL;
	while( GetLINE (&Buffer,StrLine,256)==0 )
	{
		const char *Str = &StrLine[0];
		if( GetBEGIN(&Str,"ACTOR") )
		{
			INDEX  iNew;
			UClass *TempClass;

			if( GetUClass(Str,"CLASS=",&TempClass) )
			{
				// Find a new index for this actor.
				iNew = INDEX_NONE;
				for( i=0; i<Max; i++ )
				{
					if( Element(i).Class == NULL )
					{
						iNew = i;
						break;
					}
				}
				if( iNew != INDEX_NONE )
				{
					// Import it.
					Actor			 = &Element(iNew);
					Actor->Class	 = TempClass;
					memcpy(Actor,&Actor->Class->DefaultActor,sizeof(AActor));
					Actor->SetBinPointers(Actor->Class);
					Buffer			 = ImportActorProperties(Actor,Buffer);

					if( Actor->iMe>=0 && Actor->iMe<Max*4 )
					{
						if( Actor->iMe==0 && Actor->IsKindOf("LevelDescriptor") )
						{
							// Replace the existing level descriptor with this one.
							Element(0)   = *Actor;
							Actor->Class = NULL;
						}
						else
						{
							// Remember to remap it.
							IsRemap[iNew      ] = 1;
							Remaps [Actor->iMe] = iNew;
						}
					}
					else
					{
						// Kill the invalid actor.
						Actor->Class = NULL;
					}
				}
			}
		}
		else if (GetEND(&Str,"ACTORLIST")) 
		{
			break;
		}
	}

	// Remap the imported actors.
	// Necessary because we may be merging two maps by importing
	// a second map into the first, and the second map's actors will
	// need new indices.
	for( i=0; i<Max; i++ )
	{
		if( IsRemap[i] )
		{
			// Remap this actor.
			AActor         *Actor    = &Element(i);
			UClass		   *Class    = Actor->Class;
			FClassProperty *Property = &Class->Element(0);
			
			if( Actor->iMe == INDEX_NONE ) appErrorf("!%s!",Actor->Class->Name);

			// Check all properties.
			for( int j=0; j<Class->Num; j++ )
			{
				if( Property->Type == CPT_Actor && Property->IsPerActor() )
				{
					for( int k=0; k<Property->ArrayDim; k++ )
					{
						INDEX *iActor = (INDEX *)Actor->GetPropertyPtr(j,k);
						if( *iActor>=0 && *iActor<Max*4 )
						{
							// Remap this.
							*iActor = Remaps[*iActor];
						}
					}
				}
				Property++;
			}
			if( Actor->iMe != i ) appErrorf("Remap failed %i %i",Actor->iMe,i);
		}
	}

	Num=Max;
	Realloc();
    RelistActors();
	GMem.Release(Remaps);
	return Buffer;

	unguard;
}
char *UActorList::Export(char *Buffer,const char *FileType,int Indent)
{
	guard(UActorList::Export);

	// Export all active actors.
	Buffer += sprintf (Buffer,"%sBegin ActorList Max=%i\r\n",spc(Indent),Max);

	for( INDEX iActor=0; iActor<Num; iActor++ )
	{
		AActor *Actor = &Element(iActor);
		if( Actor->Class && !Actor->IsKindOf("Camera") )
		{
			Buffer += sprintf(Buffer,"%s   Begin Actor Class=%s\r\n",spc(Indent),Actor->Class->Name);
			Buffer += ExportActor (Actor,Buffer,NAME_NONE,Indent+6,0,0,&Actor->Class->DefaultActor,1,-1,NAME_NONE);
			Buffer += sprintf(Buffer,"%s   End Actor\r\n",spc(Indent));
		}
	}
	Buffer += sprintf (Buffer,"%sEnd ActorList\r\n",spc(Indent));
	return Buffer;
	
	unguard;
}
void UActorList::QueryHeaderReferences( FResourceCallback &Callback )
{
	guard(UActorList::QueryHeaderReferences);
	unguard;
}
void UActorList::QueryDataReferences( FResourceCallback &Callback )
{
	guard(UActorList::QueryDataReferences);

	AActor *Actor = &Element(0);
	for( INDEX iActor=0; iActor<Num; iActor++ )
	{
		if (Actor->Class)
		{
			Actor->QueryReferences( this,Callback,0,0 );
		}
		Actor++;
	}
	unguard;
}
void UActorList::PostLoad()
{
	guard(UActorList::PostLoad);

	Flip();
	for( int i=0; i<Num; i++ )
	{
		AActor *Actor = &Element(i);
		if( Actor->Class )
			Actor->PostLoad(this,0);
	}
    StaticActors		= 0;
	DynamicActors		= 0;
	CollidingActors		= 0;
	ActiveActors		= 0;
	UnusedActors		= 0;
	JustDeletedActors	= 0;
    RelistActors();

	unguard(UActorList::PostLoad);
}
void UActorList::PreKill()
{
	guard(UActorList::PreKill);

	// Free the actor lists.
    delete StaticActors;
    delete DynamicActors;
    delete CollidingActors;
    delete ActiveActors;
    delete UnusedActors;
    delete JustDeletedActors;

	unguard;
}
void UActorList::Flip()
{
	guard(UActorList::Flip);

	UDatabase::Flip();
	//todo:

	unguard;
}
AUTOREGISTER_RESOURCE(RES_ActorList,UActorList,0xB2D90850,0xCCD211cf,0x91360000,0xC028B992);

/*---------------------------------------------------------------------------------------
   Actor link topic handler.
---------------------------------------------------------------------------------------*/

enum {MAX_PROP_CATS=64};
FName GPropCats[MAX_PROP_CATS];
int GNumPropCats;

void CheckPropCats(UClass *Class)
{
	for( int i=0; i<Class->Num; i++ )
	{
		FClassProperty *Prop = &Class->Element(i);
		FName CatName = Prop->Category;

		if( (Prop->Flags & CPF_Edit) && 
			(CatName!=NAME_NONE) && 
			(GNumPropCats < MAX_PROP_CATS) )
		{
			for (int j=0; j<GNumPropCats; j++)
				if (GPropCats[j]==CatName) 
					break;
			if (j>=GNumPropCats) 
				GPropCats[GNumPropCats++] = CatName;
		}
	}
}

AUTOREGISTER_TOPIC("Actor",ActorTopicHandler);
void ActorTopicHandler::Get(ULevel *Level, const char *Topic, const char *Item, char *Data)
{
	guard(ActorTopicHandler::Get);

	UActorList	*Actors = Level->ActorList;
	UClass		*AllClass   = NULL;
	INDEX		i,n,AnyClass;

	Actors->Lock(LOCK_Read);
	n        = 0;
	AnyClass = 0;

	for( i=0; i<Actors->Max; i++ )
	{
		if( Actors->Element(i).Class && Actors->Element(i).bSelected )
		{
			if (AnyClass && (Actors->Element(i).Class != AllClass)) 
				AllClass = NULL;
			else 
				AllClass = Actors->Element(i).Class;

			AnyClass=1;
			n++;
		}
	}

	Actors->Unlock();

	if
	(
		(!strnicmp(Item,"Properties",10)) ||
		(!strnicmp(Item,"DefaultProperties",17)) ||
		(!strnicmp(Item,"LevelProperties",15))
	)
	{
		// Parse name and optional array element.
		FName PropertyName = NAME_NONE;
		int Element = -1;
		char Temp[80];

		if( GetSTRING(Item,"NAME=",Temp,80) )
		{
			char *c = strstr(Temp,"(");
			if( c )
			{
				Element = atoi(&c[1]);
				*c = 0;
			}
			if (!PropertyName.Find(Temp)) 
				return;
		}
		// Only return editable properties.
		int PropertyMask = CPF_Edit;
		int Descriptive  = 1;

		int Raw=0; GetONOFF(Item,"RAW=",&Raw);
		if( Raw )
		{
			PropertyMask = 0; // Return all requested properties.
			Descriptive  = 2; // Data only, not names or formatting.
		}

		FName CategoryName=NAME_NONE;
		GetNAME(Item,"CATEGORY=",&CategoryName);

		if( !strnicmp(Item,"Properties",10) )
		{
			if( n==1 )
			{
				// 1 actor is selected - just send it.
				for( i=0; i<Actors->Max; i++ )
				{
					AActor *Actor = &Actors->Element(i);
					if( Actor->Class && Actor->bSelected )
					{
						ExportActor (Actor,(char *)Data,PropertyName,0,Descriptive,PropertyMask,NULL,0,Element,CategoryName);
						break;
					}
				}
			}
			else if (n>1) 
			{
				// Export multiple actors.
				ExportMultipleActors(Actors,(char *)Data,PropertyName,0,1,CategoryName);
			}
		}
		else if( !strnicmp(Item,"DefaultProperties",17) )
		{
			UClass *Class;
			if (GetUClass(Item,"CLASS=",&Class))
			{
				if(Class!=Class->DefaultActor.Class) appError ("Actor class mismatch");
				ExportActor (&Class->DefaultActor,(char *)Data,PropertyName,0,1,PropertyMask,NULL,0,Element,CategoryName);
			}
		}
		else if( (!strnicmp(Item,"LevelProperties",15)) && 
			Actors->Element(0) &&
			Actors->Element(0).IsKindOf("LevelDescriptor") )
		{
			ExportActor (&Actors->Element(0),(char *)Data,PropertyName,0,1,PropertyMask,NULL,0,Element,CategoryName);
		}
	}
	if ( !strnicmp(Item,"PropCats",8) ||
		 !strnicmp(Item,"DefaultPropCats",15))
	{
		GNumPropCats=0;
		if( !strnicmp(Item,"PropCats",8) )
		{
			for( int i=0; i<Actors->Max; i++ )
			{
				AActor *Actor = &Actors->Element(i);
				if (Actor->Class && Actor->bSelected) 
					CheckPropCats(Actor->Class);
			}
		}
		else if( !strnicmp(Item,"DefaultPropCats",15) )
		{
			UClass *Class;
			if (GetUClass(Item,"CLASS=",&Class)) 
				CheckPropCats(Class);
		}
		*Data=0;
		for (int i=0; i<GNumPropCats; i++) 
			Data += sprintf(Data,"%s ",GPropCats[i].Name());
	}
	else if( !stricmp(Item,"NumSelected") )
	{
		sprintf(Data,"%i",n);
	}
	else if( !stricmp(Item,"ClassSelected") )
	{
		if (AnyClass && AllClass)
			sprintf(Data,"%s",AllClass->Name);
		else
			sprintf(Data,"");
	}
	else if( !strnicmp(Item,"IsKindOf",8) )
	{
		// Sees if the 1 selected actor belongs to a class.
		UClass *Class;
		if (GetUClass(Item,"CLASS=",&Class) && AllClass && AllClass->IsKindOf(Class))
			sprintf(Data,"1");
		else
			sprintf(Data,"0");
	}
	unguard;
}
void ActorTopicHandler::Set(ULevel *Level, const char *Topic, const char *Item, const char *Data)
{
	guard(ActorTopicHandler::Set);
	
	UActorList	*Actors = Level->ActorList;

	if( !stricmp(Item,"Properties") )
	{
		GTrans->Begin(Level,"Changing actor property");
		ILevel LevelInfo;
		Level->Lock(&LevelInfo,LOCK_Trans);

		for (int i=0; i<Actors->Max; i++)
		{
			AActor *Actor = &Actors->Element(i);
			if( Actor->Class && Actor->bSelected )
			{
				LevelInfo.SendMessageReverse(i,ACTOR_PreEditChange,NULL);

				GTrans->NoteActor (Actors,i);
				ImportActorProperties(&Actors->Element(i),Data);
				Actors->Element(i).iMe = i;

				LevelInfo.SendMessageReverse(i,ACTOR_PostEditChange,NULL);

				Actor->bTempLightChanged = 1; // Force light meshes to be rebuilt
			}
		}
		Level->Unlock(&LevelInfo);
		GTrans->End();
	}
	else if( !strnicmp(Item,"DefaultProperties",17) )
	{
		UClass *Class;
		if( GetUClass(Item,"CLASS=",&Class) )
		{
			GTrans->Begin			(Level,"Changing default actor property");
			GTrans->NoteResHeader	(Class);
			ImportActorProperties	(&Class->DefaultActor,Data);
			GTrans->End				();
		}
	}
	else if( (!strnicmp(Item,"LevelProperties",15)) && 
		Actors->Element(0) &&
		Actors->Element(0).IsKindOf("LevelDescriptor") )
	{
		GTrans->Begin			(Level,"Changing level properties");
		GTrans->NoteActor		(Actors,0);
		ImportActorProperties	(&Actors->Element(0),Data);
		Actors->Element(0).bTempLightChanged = 1;
		GTrans->End				();
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Actor list locking and unlocking.
-----------------------------------------------------------------------------*/

//
// Lock an actor list.
//
void UActorList::Lock(int NewLockType)
{
	guard(UActorList::Lock);

	// Check state.
	if (NewLockType == LOCK_None)
		appError ("LOCK_NONE");
	if (LockType != LOCK_None)
		appErrorf("%s is already locked",Name);

	if (GTrans && (NewLockType==LOCK_Trans))
	{
		GTrans->NoteResHeader(this);
		Trans=1;
	}
	else
	{
		Trans=0;
	}

	LockType = NewLockType;

	 // Mark as modified.
	Modify();

	unguard;
}

//
// Unlock an actor list.
//
void UActorList::Unlock()
{
	guard(UActorList::Unlock);
	LockType = LOCK_None;
	unguard;
}

/*-----------------------------------------------------------------------------
	Actor list manipulations.
-----------------------------------------------------------------------------*/

//
// Rebuild the above redundant lists from scratch by scanning the actor list.
//
void UActorList::RelistActors()
{
    guard(UActorList::RelistActors);

    if( UnusedActors == 0 )
    {
        StaticActors        = new CompactList;
        DynamicActors       = new CompactList;
        CollidingActors     = new CompactList;
        ActiveActors        = new CompactList;
        UnusedActors        = new CompactList;
        JustDeletedActors   = new CompactList;
    }
    else
    {
        ActiveActors->Empty();
        UnusedActors->Empty();
        JustDeletedActors->Empty();
        StaticActors->Empty();
        DynamicActors->Empty();
        CollidingActors->Empty();
    }

    {
        // We make enough room in each list for all the actors.
        // This is important because sometimes we iterate over the lists
        // using pointer iteration (instead of index iteration), and such 
        // an approach is doomed to disaster if new elements are added to
        // the array mid-iteration and the array is reallocated.
        StaticActors        ->MakeRoom( Max );
        DynamicActors       ->MakeRoom( Max );
        CollidingActors     ->MakeRoom( Max );
        ActiveActors        ->MakeRoom( Max );
        UnusedActors        ->MakeRoom( Max );
        JustDeletedActors   ->MakeRoom( Max );
    }

    // Traverse the list in reverse order so that unused actors are put
    // on the UnusedActors list in reverse order. This lets us take
    // actors off the end of the UnusedActors list and keep the actor indexes (iMe)
    // low.
    for( int Which = Max-1; Which >= 0; Which-- )
    {
        AActor * Actor = &Element(Which);
        if( Which >= Num )
        {
            // Actors in the latter part of the array (beyond Num) are unused.
            Actor->Class        = 0     ;
            Actor->bJustDeleted = FALSE ;
        }
        if( Actor->Class != 0 )
        {
            ActiveActors->Add(Actor);
            ListActor( Actor );
        }
        else if( Actor->bJustDeleted )
        {
            JustDeletedActors->Add(Actor);
        }
        else
        {
            UnusedActors->Add(Actor);
        }
    }
    unguard;
}
 
//
// Remove an actor from the appropriate main lists (based on its properties).
//
void UActorList::UnlistActor(AActor * Actor)
{
    if( Actor->bStatic )
    {
        StaticActors->RemoveActor(Actor);
    }
    else
    {
        DynamicActors->RemoveActor(Actor);
    }
    if( Actor->bCollideActors )
    {
        CollidingActors->RemoveActor(Actor);
    }
}

//
// Add an actor to appropriate main lists (based on its properties).
//
void UActorList::ListActor(AActor * Actor)
{
	guard(UActorList::ListActor);

    if( Actor->Class != 0 )
    {
        if( Actor->bStatic )
        {
            StaticActors->Add(Actor);
        }
        else
        {
            DynamicActors->Add(Actor);
        }
        if( Actor->bCollideActors )
        {
            CollidingActors->Add(Actor);
        }
    }
	unguard;
}

//
// Debug: Check the redundant lists for correctness and completeness.
//
void UActorList::CheckLists()
{
	guard(UActorList::CheckLists);

    // As a bonus, let's give some info about the lists.
    debugf( LOG_Info, "ActiveActors: %i", ActiveActors->Count() );
    debugf( LOG_Info, "UnusedActors: %i", UnusedActors->Count() );
    debugf( LOG_Info, "JustDeletedActors: %i", JustDeletedActors->Count() );
    debugf( LOG_Info, "StaticActors: %i", StaticActors->Count() );
    debugf( LOG_Info, "DynamicActors: %i", DynamicActors->Count() );
    debugf( LOG_Info, "CollidingActors: %i", CollidingActors->Count() );

    for( int Which = 0; Which < Num; Which++)
    {
        AActor * Actor = &Element(Which);
        const BOOL IsOnActiveList       = ActiveActors->Find(Actor)      >= 0;
        const BOOL IsOnUnusedList       = UnusedActors->Find(Actor)      >= 0;
        const BOOL IsOnJustDeletedList  = JustDeletedActors->Find(Actor) >= 0;
        const int Count = // Number of administrative lists this actor is on.
            ( IsOnActiveList       ? 1 : 0 )
        +   ( IsOnUnusedList       ? 1 : 0 )
        +   ( IsOnJustDeletedList  ? 1 : 0 )
        ;
        if( Count != 1 ) // Actor is not on exactly one administrative list?
        {
            debugf( LOG_Info, "Actor [%i] is on %i administrative lists!", Which, Count );
        }
        if( IsOnActiveList && Actor->Class == 0 )
        {
            debugf( LOG_Info, "Actor [%i] on active list with class==0", Which );
        }
        if( IsOnUnusedList && Actor->Class != 0 )
        {
            debugf( LOG_Info, "Actor [%i] on unused list with class!=0", Which );
        }
        if( IsOnUnusedList && Actor->bJustDeleted )
        {
            debugf( LOG_Info, "Actor [%i] on unused list with bJustDeleted", Which );
        }
        if( IsOnJustDeletedList && Actor->Class != 0 )
        {
            debugf( LOG_Info, "Actor [%i] on just deleted list with class!=0", Which );
        }
        if( IsOnJustDeletedList && !Actor->bJustDeleted )
        {
            debugf( LOG_Info, "Actor [%i] on just deleted list with !bJustDeleted", Which );
        }
        const BOOL IsOnStaticList    = StaticActors->Find(Actor)    >= 0;
        const BOOL IsOnDynamicList   = DynamicActors->Find(Actor)   >= 0;
        const BOOL IsOnCollidingList = CollidingActors->Find(Actor) >= 0;
        if( IsOnStaticList && Actor->Class == 0 )
        {
            debugf( LOG_Info, "Actor [%i] on static list with Class==0", Which );
        }
        if( IsOnDynamicList && Actor->Class == 0 )
        {
            debugf( LOG_Info, "Actor [%i] on dynamic list with Class==0", Which );
        }
        if( IsOnCollidingList && Actor->Class == 0 )
        {
            debugf( LOG_Info, "Actor [%i] on colliding list with Class==0", Which );
        }
        if( Actor->Class != 0 )
        {
            if( IsOnStaticList && !Actor->bStatic )
            {
                debugf( LOG_Info, "Actor [%i] on static list with !bStatic", Which );
            }
            else if( !IsOnStaticList && Actor->bStatic )
            {
                debugf( LOG_Info, "Actor [%i] not on static list but has bStatic", Which );
            }
            if( IsOnDynamicList && Actor->bStatic )
            {
                debugf( LOG_Info, "Actor [%i] on dynamic list with bStatic", Which );
            }
            else if( !IsOnDynamicList && !Actor->bStatic )
            {
                debugf( LOG_Info, "Actor [%i] not on dynamic list but has !bStatic", Which );
            }
            if( IsOnCollidingList && !Actor->bCollideActors )
            {
                debugf( LOG_Info, "Actor [%i] on colliding list with !bCollideActors", Which );
            }
            else if( !IsOnCollidingList && Actor->bCollideActors )
            {
                debugf( LOG_Info, "Actor [%i] not on colliding list but has bCollideActors", Which );
            }
        }
    }
    unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
