/*=============================================================================
	UnActor.cpp: Actor list functions.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	Actor exportation helpers.
-----------------------------------------------------------------------------*/

//
// Export one entire actor, or the specified named property, to text.
//
void ExportActor
(
	UClass*			XClass,
	BYTE*const*		ActorBins,
	FOutputDevice&	Out,
	FName			PropertyName,
	int				Indent,
	int				Descriptive,
	int				Flags, 
	UClass			*Diff,
	int				Objects,
	int				ArrayElement,
	FName			Category,
	BYTE			WhichBins[PROPBIN_MAX]
)
{
	guard(ExportActor);
	char Type[128],Name[128],Value[128];

	// Get difference bins.
	BYTE **DiffBins=NULL, *DeltaBins[PROPBIN_MAX];
	if( Diff )
	{
		Diff->GetClassBins(DeltaBins);
		DiffBins = DeltaBins;
	}

	// Export actor.
	for( FPropertyIterator It(XClass); It; ++It )
	{
		FProperty &Property = It();
		if
		(	(PropertyName==NAME_None || PropertyName==Property.Name)
		&&	(WhichBins[Property.Bin])
		&& !(Property.Flags & (CPF_Private | CPF_Transient | CPF_Intrinsic)) )
		{
			if( Property.ArrayDim == 1 )
			{
				// Export single element.
				Property.ExportText
				(
					Type,
					Name,
					Value,
					ActorBins,
					Flags,
					Descriptive,
					0,
					Diff && Diff->IsChildOf(It.GetClass()) ? DiffBins : NULL,
					Category
				);
				if( Type[0] && Name[0] )
				{
					if
					(	(Objects)
					&&	(Property.Type == CPT_Object)
					&&	(Property.Flags & CPF_ExportObject) )
					{
						UObject *Res = *(UObject **)&ActorBins[Property.Bin][Property.Offset];
						if (Res && !(Res->GetFlags() & RF_TagImp))
						{
							// Don't export more than once.
							Res->Export( Out, "", Indent+1 );
							Res->SetFlags( RF_TagImp );
						}
					}
					if (Descriptive!=2) Out.Logf("%s%s %s=%s\r\n",spc(Indent),Descriptive?Type:"",Name,Value);
					else                Out.Logf("%s",Value);
					//bug("%s%s %s=%s\r\n",spc(Indent),Descriptive?Type:"",Name,Value);
				}
			}
			else
			{
				// Export array.
				for( INT j=0; j<Property.ArrayDim; j++ )
				{
					Property.ExportText
					(
						Type,
						Name,
						Value,
						ActorBins,
						Flags,
						Descriptive,
						j,
						Diff && Diff->IsChildOf(It.GetClass()) ? DiffBins : NULL,
						Category
					);
					if( Type[0] && Name[0] && (ArrayElement==-1 || ArrayElement==(int)j) )
					{
						if
						(	(Objects)
						&&	(Property.Type==CPT_Object)
						&&	(Property.Flags & CPF_ExportObject) )
						{
							UObject *Res = *(UObject **)&ActorBins[Property.Bin][Property.Offset + j*Property.ElementSize];
							if (Res && !(Res->GetFlags() & RF_TagImp))
							{
								// Don't export more than once.
								Res->Export(Out,"",Indent+1);
								Res->SetFlags(RF_TagImp);
							}
						}
						if( Descriptive!=2 ) Out.Logf("%s%s %s(%i)=%s\r\n",spc(Indent),Descriptive?Type:"",Name,j,Value);
						else                 Out.Logf("%s",Value);
					}
				}
			}
		}
	}
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
void ExportMultipleActors
(
	ULevel*			Actors,
	FOutputDevice&	Out,
	FName			PropertyName,
	INT				Indent,
	INT				Descriptive,
	FName			Category,
	BYTE			WhichBins[PROPBIN_MAX]
)
{
	guard(ExportMultipleActors);

	AActor			*FirstActor=NULL,*Actor;
	char			FirstType[256],FirstName[256],FirstValue[256];
	char			Type[256],Name[256],Value[256];

	// Find first actor.
	INDEX iStart = -1;
	for( INDEX iActor=0; iActor<Actors->Num; iActor++ )
	{
		FirstActor = Actors->Element(iActor);
		if( FirstActor && FirstActor->bSelected )
		{
			iStart = iActor+1;
			break;
		}
	}

	// At least one actor is selected.
	if( iStart>=0 )
	{
		// Find property in first actor.
		for( FPropertyIterator It1(FirstActor->GetClass()); It1; ++It1 )
		{
			FProperty &FirstProperty = It1();
			if
			(	(PropertyName==NAME_None || PropertyName==FirstProperty.Name)
			&&	(FirstProperty.ArrayDim == 1) 
			&&	(WhichBins[FirstProperty.Bin])
			&&	(It1.GetFlagMask() & CPF_Edit)
			&& !(FirstProperty.Flags & (CPF_Private|CPF_Transient|CPF_Intrinsic))
			)
			{
				BYTE *ObjectBins[PROPBIN_MAX]; FirstActor->GetObjectBins(ObjectBins);
				FirstProperty.ExportText
				(
					FirstType,
					FirstName,
					FirstValue,
					ObjectBins,
					CPF_Edit,
					Descriptive,
					0,
					NULL,
					Category
				);
				if( FirstType[0] && FirstName[0] )
				{
					// Now go through all other actors and see if this property is shared.
					for( iActor=iStart; iActor<Actors->Num; iActor++ )
					{
						Actor = Actors->Element(iActor);
						if( Actor && Actor->bSelected )
						{
							for( FPropertyIterator It2(Actor->GetClass()); It2; ++It2 )
							{
								FProperty &Property = It2();
								if
								(	(Property.Name==FirstProperty.Name)
								&&  (Property.ArrayDim == 1)
								&&	(WhichBins[Property.Bin])
								&&	(It2.GetFlagMask() & CPF_Edit)
								&& !(Property.Flags & (CPF_Private|CPF_Transient|CPF_Intrinsic))
								)
								{
									BYTE *ObjectBins[PROPBIN_MAX]; Actor->GetObjectBins(ObjectBins);
									Property.ExportText
									(
										Type,
										Name,
										Value,
										ObjectBins,
										CPF_Edit,
										Descriptive,
										0,
										NULL,
										Category
									);
									
									if( stricmp(Type,FirstType)!=0 || stricmp(Name,FirstName)!=0 )
									{					
										// Mismatch.					
										goto NextProperty;
									}
									if( stricmp(Value,FirstValue)!=0 )
									{
										// Blank out value if not identical.
										FirstValue[0]=0;
									}
								}
							}
						}
					}
					if( strcmp(Value,FirstValue)==0 )
						Out.Logf("%s%s %s=%s\r\n",spc(Indent),Descriptive?Type:"",Name,Value);
					else
						Out.Logf("%s%s %s=\r\n",spc(Indent),Descriptive?Type:"",Name);
				}
			}
			NextProperty:;
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Actor importation helpers.
-----------------------------------------------------------------------------*/

//
// Import all of the properties in Data into the specified actor.
// Data may contain multiple properties, but it might not contain all of 
// the properties for the actor, so the actor should be initialized
// first.
//
// This requires that the actor's Class be set in advance.
//
UNENGINE_API const char *ImportActorProperties
(
	ULevel*			Actors,
	UClass*			Class,
	BYTE**			ObjectBins,
	const char*		Data,
	BYTE			WhichBins[PROPBIN_MAX],
	INT				ImportingFromFile
)
{
	guard(ImportActorProperties);
	FMemMark Mark(GMem);

	char	*PropText	= new(GMem,65536)char;
	char	*Top		= &PropText[0];

	// Parse all objects stored in the actor.
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

			if( GetSTRING(Str,"NAME=",BrushName,NAME_SIZE) )
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
					GObj.MakeUniqueObjectName(TempName,BrushName,UModel::GetBaseClass());

					debugf(LOG_Info,"Renaming %s to %s",ExistingBrush->GetName(),TempName);
					ExistingBrush->Rename(TempName);

					if( ExistingBrush->Polys )
						ExistingBrush->Polys->Rename(TempName);
				}
				UModel *Brush = new(BrushName,CREATE_Unique)UModel;
				Data = Brush->Import(Data,NULL,"");

				// Prep it for collision.
				checkState(GEditor!=NULL);
				GEditor->csgPrepMovingBrush(Brush);
			}
		}
		else if( GetEND(&Str,"ACTOR") || GetEND(&Str,"DEFAULTPROPERTIES") )
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
	for( FPropertyIterator It(Class); It; ++It )
	{
		FProperty &Property = It();
		if
		(	WhichBins[Property.Bin]
		&& !(Property.Flags & (CPF_Private|CPF_Transient|CPF_Intrinsic) )
		&&	stricmp(Property.Name(),"Zone") )
		{
			char LookFor[80];
			for( int j=0; j<Property.ArrayDim; j++ )
			{
				BYTE *Value	= &ObjectBins[Property.Bin][Property.Offset + j*Property.ElementSize];

				if( Property.ArrayDim == 1 )
					sprintf(LookFor,"%s=",Property.Name());
				else
					sprintf(LookFor,"%s(%i)=",Property.Name(),j);

				switch( Property.Type )
				{
					case CPT_Byte:
						if( !GetBYTE( PropText, LookFor, (BYTE *)Value ) && Property.Enum )
						{
							// Try to get enum tag.
							char TempTag[NAME_SIZE];
							if( GetSTRING( PropText, LookFor, TempTag, NAME_SIZE ) )
							{
								// Map tag to enum index.
								INDEX iEnum;
								if( Property.Enum->FindNameIndex( TempTag, iEnum ) )
									*(BYTE*)Value = iEnum;
							}
						}
						break;
					case CPT_Int:
						GetINT(PropText,LookFor,(INT *)Value);
						break;
					case CPT_Bool:
						int Result;
						if (GetONOFF(PropText,LookFor,&Result))
						{
							if (Result) *(DWORD *)Value |=  Property.BitMask;
							else		*(DWORD *)Value &= ~Property.BitMask;
						}
						break;
					case CPT_Float:
						GetFLOAT(PropText,LookFor,(FLOAT *)Value);
						break;
					case CPT_Object:
					{
						if( Property.Class->IsChildOf("Actor") )
						{
							// An actor reference.
							if( ImportingFromFile == 0 )
							{
								// Import regular object name.
								INDEX iTemp;
								if( GetINDEX( PropText, LookFor, &iTemp ) && iTemp!=INDEX_NONE )
									if( Actors!=NULL && iTemp>=0 && iTemp<Actors->Max )
										*(AActor**)Value = Actors->Element(iTemp);
							}
							else if( ImportingFromFile == 1 )
							{
								// Importing from old format .t3d.
								*(INDEX*)Value = INDEX_NONE;
								GetINDEX( PropText, LookFor, (INDEX*)Value );
							}
							else
							{
								// Importing from new format .t3d.
								*(FName*)Value = NAME_None;
								GetNAME( PropText, LookFor, (FName*)Value );
							}
						}
						else
						{
							// An object reference.
							GetOBJ(PropText,LookFor,Property.Class,(UObject **)Value);
						}
						break;
					}
					case CPT_Name:
						if( !GetNAME(PropText,LookFor,(FName *)Value) )
						{
							// Update Name to Tag from old version.
							if( Property.Name==NAME_Tag )
								GetNAME(PropText,"Name=",(FName *)Value);
						}
						break;
					case CPT_String:
						GetSTRING(PropText,LookFor,(char *)Value,Property.ElementSize);
						break;
					case CPT_Vector:
						GetFVECTOR(PropText,LookFor,(FVector *)Value);
						break;
					case CPT_Rotation:
						GetFROTATION(PropText,LookFor,(FRotation *)Value,1);
						break;
					case CPT_EnumDef:
						break;
					default:
						appErrorf("Bad property type %i (%i/%i) in %s of %s (%s)",Property.Type,It.GetIndex(),It.GetClass()->Num,Property.Name(),It.GetClass()->GetName(),Class->GetName());
						break;
				}
			}
		}
	}
	Mark.Pop();
	return Data;
	unguard;
}

/*-----------------------------------------------------------------------------
	Actor importing and exporting.
-----------------------------------------------------------------------------*/

const char *ULevel::ImportActors( const char *Buffer, const char *BufferEnd, const char *FileType )
{
	guard(ULevel::ImportActors);

	// Default to old-format.
	int ImportFormat = 1;

	// Allocate working memory for remapping.
	FMemMark Mark(GMem);
	INDEX *Remaps      = new(GMem,Max * 4)INDEX;
	INDEX *IsRemap     = new(GMem,Max    )INDEX;
	FName *ImportNames = new(GMem,Max    )FName;

	// Init items between Num and Max.
	for( int i=Num; i<Max; i++ )
		Element(i) = NULL;

	// Init remap tables.
	for( int j=0; j<Max; j++ )
	{
		IsRemap    [j] = 0;
		ImportNames[j] = NAME_None;
	}
	for( j=0; j<Max*4; j++ )
	{
		Remaps[j] = INDEX_NONE;
	}
	Remaps[0] = 0;

	// Import a sparse actor list.
	guard(Importing);
	char StrLine[256];
	while( GetLINE (&Buffer,StrLine,256)==0 )
	{
		const char *Str = &StrLine[0];
		if( GetBEGIN(&Str,"ACTOR") )
		{
			UClass *TempClass;
			FName ActorName(NAME_None);
			if( GetUClass(Str,"CLASS=",TempClass) )
			{
				if( GetNAME( Str, "NAME=", &ActorName ) )
				{
					// This indicates we are importing a new .t3d format file.
					ImportFormat = 2;
				}

				// Find a new index for this actor.
				INDEX iActor = INDEX_NONE;
				for( j=0; j<Max; j++ )
				{
					if( Element(j) == NULL )
					{
						iActor = j;
						break;
					}
				}
				if( iActor != INDEX_NONE )
				{
					// Import it.
					AActor *Actor = Element(iActor) = (AActor*)GObj.CreateObject
					(
						ActorName!=NAME_None ? ActorName() : TempClass->GetName(),
						TempClass,
						CREATE_MakeUnique
					);
					memcpy
					(
						(BYTE *)Actor                                           + sizeof(UObject),
						&Actor->GetClass()->Bins[PROPBIN_PerObject]->Element(0) + sizeof(UObject),
						Actor->GetClass()->Bins[PROPBIN_PerObject]->Num         - sizeof(UObject)
					);

					// Init properties.
					Actor->ZoneNumber = 0;
					Actor->XLevel     = new( "TestLev", FIND_Existing )ULevel;
					Actor->Hash       = NULL;

					BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
					WhichBins[PROPBIN_PerObject] = 1;
					BYTE *ObjectBins[PROPBIN_MAX]; Actor->GetObjectBins(ObjectBins);

					INT ImportIndex = INDEX_NONE;
					GetINT( Buffer, "SELFINDEX=", &ImportIndex );//!!
					GetINT( Buffer, "ME=", &ImportIndex );
					Buffer = ImportActorProperties
					(
						this,
						Actor->GetClass(),
						ObjectBins,
						Buffer,
						WhichBins,
						ImportFormat
					);

					if( (ImportIndex!=INDEX_NONE && ImportIndex<Max*4) || ActorName!=NAME_None )
					{
						if( Actor->IsA("LevelInfo") )
						{
							// Copy the one LevelInfo the position #0.
							if( Element(0) )
								Element(0)->Kill();
							Element(0)       = Actor;
							Element(iActor)  = NULL;
							iActor           = 0;
						}
						if( ImportIndex!=INDEX_NONE )
						{
							IsRemap[iActor          ] = 1;
							Remaps [ImportIndex     ] = iActor;
						}
						ImportNames[iActor] = ActorName;
					}
					else
					{
						// Kill the invalid actor.
						debugf( "Got an invalid actor: %s %s", Actor->GetClassName(), Actor->GetName() );
						Actor->Kill();
						Element(iActor) = NULL;
					}
				}
			}
		}
		else if( GetEND(&Str,"ACTORLIST") ) break;
	}
	unguard;

	// Remap the imported actors.
	// Necessary because we may be merging two maps by importing
	// a second map into the first, and the second map's actors will
	// need adjusting.
	guard(Remapping);
	Num = 0;
	for( int i=0; i<Max; i++ )
	{
		AActor *Actor = Element(i);
		if( Actor && IsRemap[i] )
		{
			// Check all properties.
			UClass *Class = Actor->GetClass();
			for( FPropertyIterator It(Actor->GetClass()); It; ++It )
			{
				FProperty &Property = It();
				if( Property.Type==CPT_Object && Property.Bin==PROPBIN_PerObject && Property.Class->IsChildOf("Actor") )
				{
					for( int k=0; k<Property.ArrayDim; k++ )
					{
						// Remap this.
						void *Test = Actor->ObjectPropertyPtr(Property,k);
						if( ImportFormat == 1 )
						{
							// Old format t3d.
							guard(RemapOldIndex);
							if( *(INDEX*)Test == INDEX_NONE )	*(AActor**)Test = NULL;
							else								*(AActor**)Test = Element(Remaps[*(INDEX*)Test]);
							unguard;
						}
						else
						{
							// New format t3d.
							guard(RemapNewIndex);
							if( *(FName*)Test == NAME_None )
							{
								*(AActor**)Test=NULL;
							}
							else
							{
								for( int i=0; i<Max; i++ )
								{
									if( ImportNames[i] == *(FName*)Test )
									{
										checkState(Element(i)!=NULL);
										*(AActor**)Test = Element(i);
									}
								}
							}
							unguard;
						}
					}
				}
			}
		}
		if( Actor )
		{
			guard(FixupActor);
			Actor->Level = (ALevelInfo*)Element(0);
			Actor->Zone  = Actor->Zone ? Actor->Zone : (ALevelInfo*)Element(0);
			Num          = i + 1;
			unguard;
		}
	}
	checkLogic(Element(0)!=NULL && Element(0)->IsA("LevelInfo"));
	unguard;

	Mark.Pop();
	return Buffer;

	unguard;
}
void ULevel::ExportActors
(
	FOutputDevice	&Out,
	const char		*FileType,
	int				Indent
)
{
	guard(ULevel::ExportActors);

	// Export all active actors.
	Out.Logf("%sBegin ActorList Max=%i\r\n",spc(Indent),Max);

	BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
	WhichBins[PROPBIN_PerObject] = 1;

	for( INDEX iActor=0; iActor<Num; iActor++ )
	{
		AActor *Actor = Element(iActor);
		if( Actor && !Actor->IsA("View") )
		{
			Out.Logf
			(
				"%s   Begin Actor Class=%s Name=%s\r\n",
				spc(Indent),
				Actor->GetClassName(),
				Actor->GetName()
			);
			BYTE *ObjectBins[PROPBIN_MAX]; Actor->GetObjectBins(ObjectBins);
			ExportActor
			(
				Actor->GetClass(),
				ObjectBins,
				Out,
				NAME_None,
				Indent+6,
				0,
				0,
				Actor->GetClass(),
				1,
				-1,
				NAME_None,
				WhichBins
			);
			Out.Logf("%s   End Actor\r\n",spc(Indent));
		}
	}
	Out.Logf("%sEnd ActorList\r\n",spc(Indent));
	unguard;
}

/*---------------------------------------------------------------------------------------
   Actor link topic handler.
---------------------------------------------------------------------------------------*/

enum {MAX_PROP_CATS=64};
FName GPropCats[MAX_PROP_CATS];
int GNumPropCats;

void CheckPropCats( UClass *Class )
{
	for( FPropertyIterator It(Class); It; ++It )
	{
		FProperty &Property = It();
		FName     CatName   = Property.Category;

		if
		(	(It.GetFlagMask() & Property.Flags & CPF_Edit)
		&&	(CatName != NAME_None)
		&&	(GNumPropCats < MAX_PROP_CATS) )
		{
			for( int j=0; j<GNumPropCats; j++ )
				if( GPropCats[j]==CatName )
					break;
			if( j>=GNumPropCats )
				GPropCats[GNumPropCats++] = CatName;
		}
	}
}

AUTOREGISTER_TOPIC("Actor",ActorTopicHandler);
void ActorTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(ActorTopicHandler::Get);

	int		 n			= 0;
	INDEX	 AnyClass	= 0;
	UClass	 *AllClass	= NULL;

	for( int i=0; i<Level->Num; i++ )
	{
		if( Level->Element(i) && Level->Element(i)->bSelected )
		{
			if( AnyClass && Level->Element(i)->GetClass()!=AllClass ) 
				AllClass = NULL;
			else 
				AllClass = Level->Element(i)->GetClass();

			AnyClass=1;
			n++;
		}
	}

	if
	(
		(!strnicmp(Item,"Properties",10)) ||
		(!strnicmp(Item,"DefaultProperties",17)) ||
		(!strnicmp(Item,"LevelProperties",15))
	)
	{
		// Parse name and optional array element.
		FName PropertyName = NAME_None;
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
			PropertyName = FName( Temp, FNAME_Find );
			if( PropertyName == NAME_None )
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

		FName CategoryName=NAME_None;
		GetNAME(Item,"CATEGORY=",&CategoryName);

		if( !strnicmp(Item,"Properties",10) )
		{
			BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
			WhichBins[PROPBIN_PerObject] = 1;
			if( n==1 )
			{
				// 1 actor is selected - just send it.
				for( i=0; i<Level->Num; i++ )
				{
					AActor *Actor = Level->Element(i);
					if( Actor && Actor->bSelected )
					{
						BYTE *ObjectBins[PROPBIN_MAX]; Actor->GetObjectBins(ObjectBins);
						ExportActor
						(
							Actor->GetClass(),
							ObjectBins,
							Out,
							PropertyName,
							0,
							Descriptive,
							PropertyMask,
							NULL,
							0,
							Element,
							CategoryName,
							WhichBins
						);
						break;
					}
				}
			}
			else if( n > 1 ) 
			{
				// Export multiple actors.
				ExportMultipleActors
				(
					Level,
					Out,
					PropertyName,
					0,
					1,
					CategoryName,
					WhichBins
				);
			}
		}
		else if( !strnicmp(Item,"DefaultProperties",17) )
		{
			BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
			WhichBins[PROPBIN_PerObject] = WhichBins[PROPBIN_PerClass] = 1;
			UClass *Class;
			if( GetUClass(Item,"CLASS=",Class) )
			{
				BYTE *ClassBins[PROPBIN_MAX];
				Class->GetClassBins(ClassBins);
				ExportActor
				(
					Class,
					ClassBins,
					Out,
					PropertyName,
					0,
					Descriptive,
					PropertyMask,
					NULL,
					0,
					Element,
					CategoryName,
					WhichBins
				);
			}
		}
		else if
		(	(!strnicmp(Item,"LevelProperties",15))
		&&	Level->Element(0)
		&&	Level->Element(0)->IsA("LevelInfo") )
		{
			BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
			WhichBins[PROPBIN_PerObject] = 1;
			BYTE *ObjectBins[PROPBIN_MAX]; Level->Element(0)->GetObjectBins(ObjectBins);
			ExportActor 
			(
				Level->Element(0)->GetClass(),
				ObjectBins,
				Out,
				PropertyName,
				0,
				Descriptive,
				PropertyMask,
				NULL,
				0,
				Element,
				CategoryName,
				WhichBins
			);
		}
	}
	if ( !strnicmp(Item,"PropCats",8) ||
		 !strnicmp(Item,"DefaultPropCats",15))
	{
		GNumPropCats=0;
		if( !strnicmp(Item,"PropCats",8) )
		{
			for( int i=0; i<Level->Num; i++ )
			{
				AActor *Actor = Level->Element(i);
				if( Actor && Actor->bSelected )
					CheckPropCats(Actor->GetClass());
			}
		}
		else if( !strnicmp(Item,"DefaultPropCats",15) )
		{
			UClass *Class;
			if (GetUClass(Item,"CLASS=",Class)) 
				CheckPropCats(Class);
		}
		for (int i=0; i<GNumPropCats; i++) 
			Out.Logf("%s ",GPropCats[i]());
	}
	else if( !stricmp(Item,"NumSelected") )
	{
		Out.Logf("%i",n);
	}
	else if( !stricmp(Item,"ClassSelected") )
	{
		if( AnyClass && AllClass )
			Out.Logf( "%s", AllClass->GetName() );
	}
	else if( !strnicmp(Item,"IsKindOf",8) )
	{
		// Sees if the one selected actor belongs to a class.
		UClass *Class;
		if( GetUClass(Item,"CLASS=",Class) && AllClass && AllClass->IsChildOf(Class) )
			Out.Logf("1");
		else
			Out.Logf("0");
	}
	unguard;
}
void ActorTopicHandler::Set( ULevel *Level, const char *Item, const char *Data )
{
	guard(ActorTopicHandler::Set);

	if( !stricmp(Item,"Properties") )
	{
		GTrans->Begin(Level,"Changing actor property");
		Level->Lock(LOCK_Trans);

		for( int i=0; i<Level->Num; i++ )
		{
			AActor *Actor = Level->Element(i);
			if( Actor && Actor->bSelected )
			{
				Actor->Process( NAME_PreEditChange, NULL );

				BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
				WhichBins[PROPBIN_PerObject] = 1;
				BYTE *ObjectBins[PROPBIN_MAX]; Actor->GetObjectBins(ObjectBins);

				Actor->Lock(LOCK_Trans);
				ImportActorProperties
				(
					Level,
					Level->Element(i)->GetClass(),
					ObjectBins,
					Data,
					WhichBins,
					0
				);
				Actor->Process( NAME_PostEditChange, NULL );
				Actor->bLightChanged = 1; // Force light meshes to be rebuilt
				Actor->Unlock(LOCK_Trans);
			}
		}
		Level->Unlock(LOCK_Trans);
		GTrans->End();
	}
	else if( !strnicmp(Item,"DefaultProperties",17) )
	{
		UClass *Class;
		if( GetUClass(Item,"CLASS=",Class) )
		{
			GTrans->Begin(Level,"Changing default actor property");

			BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
			WhichBins[PROPBIN_PerObject] = WhichBins[PROPBIN_PerClass] = 1;

			BYTE *ClassBins[PROPBIN_MAX]; Class->GetClassBins(ClassBins);
			ImportActorProperties
			(
				(ULevel*)NULL,
				Class,
				ClassBins,
				Data,
				WhichBins,
				0
			);
			GTrans->End();
		}
	}
	else if
	(	(!strnicmp(Item,"LevelProperties",15))
	&&	Level->Element(0)
	&&	Level->Element(0)->IsA("LevelInfo") )
	{
		GTrans->Begin(Level,"Changing level info");
		Level->Element(0)->Lock(LOCK_Trans);

		BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
		WhichBins[PROPBIN_PerObject] = 1;
		BYTE *ObjectBins[PROPBIN_MAX]; Level->Element(0)->GetObjectBins(ObjectBins);

		ImportActorProperties
		(
			Level,
			Level->Element(0)->GetClass(),
			ObjectBins,
			Data,
			WhichBins,
			0
		);
		Level->Element(0)->bLightChanged = 1;
		Level->Element(0)->Unlock(LOCK_Trans);
		GTrans->End();
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
