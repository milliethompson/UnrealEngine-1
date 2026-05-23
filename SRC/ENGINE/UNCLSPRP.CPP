/*=============================================================================
	UnClsPrp.cpp: FClassProperty implementation

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnScrEng.h"

/*-----------------------------------------------------------------------------
	Main FClassProperty functions.
-----------------------------------------------------------------------------*/

//
// Initialize all FClassProperty members.
//
void FClassProperty::Init()
{
	guard(FClassProperty::Init);

	Name		= NAME_NONE;
	Category	= NAME_NONE;
	Type		= CPT_None;
	Offset		= 0;
	ArrayDim    = 1;
	ElementSize = 0;
	Bin			= 0;
	Flags		= 0;

	unguard;
}

//
// See if Override is a valid flag name and, if so, set the associated
// property flags and return 1.  Returns 0 if override is not a valid flag name.
//
int FClassProperty::SetFlags(const char *Override)
{
	guard(FClassProperty::SetFlags);

	if (!stricmp(Override,"Private"))
	{
		Flags |= CPF_Private;
		return 1;
	}
	else if (!stricmp(Override,"NoSave"))
	{
		Flags |= CPF_NoSaveResource;
		return 1;
	}
	else if (!stricmp(Override,"Const"))
	{
		Flags |= CPF_Const;
		return 1;
	}
	else if (!stricmp(Override,"Editable"))
	{
		Flags |= CPF_Edit;
		return 1;
	}
	else if (!stricmp(Override,"ExportResource"))
	{
		Flags |= CPF_ExportResource;
		return 1;
	}
	else return 0;

	unguard;
}

//
// Set the class property's type based on the value of Type.  Returns 1 if success,
// or 0 if Type is unrecognized.
//
int FClassProperty::SetType(const char *TypeStr,int ThisArrayDim,int Extra,char *Error)
{
	guard(FClassProperty::SetType);

	// Set up array info.
	if( ThisArrayDim<=0 )
	{
		sprintf(Error,"Array dimension must be positive");
		return 0;
	}
	ArrayDim = ThisArrayDim;

	// Check all base types.
	if( !stricmp(TypeStr,"BYTE") )
	{
		// Intrinsic BYTE type.
		Type		= CPT_Byte;
		ElementSize = sizeof(BYTE);
		Enum		= NULL;
		return 1;
	}
	if( !stricmp(TypeStr,"INTEGER") )
	{
		// Intrinsic INTEGER type.
		Type		= CPT_Integer;
		ElementSize = sizeof(INT);
		return 1;
	}
	if( !stricmp(TypeStr,"BOOLEAN") )
	{
		// Intrinsic BOOLEAN type.
		if (ArrayDim!=1)
		{
			sprintf(Error,"Boolean arrays aren't allowed");
			return 0;
		}
		Type		= CPT_Boolean;
		ElementSize = sizeof(DWORD);
		return 1;
	}
	if( !stricmp(TypeStr,"REAL") )
	{
		// Intrinsic REAL type
		Type		= CPT_Real;
		ElementSize = sizeof(FLOAT);
		return 1;
	}
	if( !stricmp(TypeStr,"ACTOR") )
	{
		// Intrinsic ACTOR type.
		Type		= CPT_Actor;
		ElementSize = sizeof(INDEX);
		Class		= GClasses.Actor;
		return 1;
	}
	if( !stricmp(TypeStr,"NAME") )
	{
		// Intrinsic NAME type.
		Type		= CPT_Name;
		ElementSize = sizeof(FName);
		return 1;
	}
	if( !stricmp(TypeStr,"STRING") )
	{
		// Intrinsic STRING type.
		if( Extra <= 0 )
		{
			sprintf(Error,"Must specify string size like: Dim S as String(20)");
			return 0;
		}
		ElementSize = Extra;
		Type        = CPT_String;
		return 1;
	}
	if( !stricmp(TypeStr,"VECTOR") )
	{
		// Intrinsic VECTOR type.
		Type		= CPT_Vector;
		ElementSize = sizeof(FVector);
		return 1;
	}
	if( !stricmp(TypeStr,"ROTATION") )
	{
		// Intrinsic ROTATION type.
		Type		= CPT_Rotation;
		ElementSize = sizeof(FRotation);
		return 1;
	}

	// See if it's a resource type.
	EResourceType TempResType = GRes.LookupType(TypeStr);
	if( (TempResType != RES_None)
		&&	(GRes.GetType(TempResType).TypeFlags & RTF_ScriptReferencable) )
	{
		Type		= CPT_Resource;
		ElementSize = sizeof(UResource *);
		ResType		= TempResType;
		return 1;
	}

	// See if it's an actor class type.
	UClass *TempClass = new(TypeStr,FIND_Optional)UClass;
	if( TempClass )
	{
		// May only hold a reference to an actor of this kind or child class.
		Type		= CPT_Actor;
		ElementSize = sizeof(INDEX);
		Class		= TempClass;
		return 1;
	}

	// See if it's an enumerated type.
	UEnum *TempEnum = new(TypeStr,FIND_Optional)UEnum;
	if( TempEnum )
	{
		Type		= CPT_Byte;
		ElementSize = sizeof(BYTE);
		Enum		= TempEnum;
		return 1;
	}

	// No matching type was found.
	sprintf(Error,"Unrecognized variable type");
 	return 0;

	unguard;
}

//
// Return the offset into the properties frame data of the first available byte past
// the end.
//
int PropertyFrameEndOffset
(
	int						NumFrameProperties,
	const FClassProperty	*FrameProperties,
	int						EffectiveElementSize
)
{
	if( NumFrameProperties > 0 )
	{
		const FClassProperty &Property = FrameProperties[NumFrameProperties-1];
		int	Result = Property.Offset + Property.PropertySize();

		if		(EffectiveElementSize==2)	Result = (Result+1) & ~1;
		else if (EffectiveElementSize>=4)	Result = (Result+3) & ~3;

		return Result;
	}
	else return 0;
}

//
// Add this class property to a property frame.  Verifies that all required members are
// set to valid values, and adjusts any of this property's values that need to be adjusted
// based on the frame, i.e. by merging adjacent bit flags into DWORD's.  Returns pointer
// to the property in the frame if success, NULL if property could not be added.  Any 
// modifications are applied to this class property before adding it to the frame, 
// so the value of this property upon return are sufficient for initializing the data by 
// calling InitPropertyData.
//
// Assumes that there is enough room in the FrameProperties array to add the specified property.
// Doesn't check to see that there are enough data bytes available to hold it.
//
FClassProperty *FClassProperty::AddToFrame
(
	int				*NumFrameProperties,
	FClassProperty	*FrameProperties
)
{
	guard(FClassProperty::AddToFrame);

	// Initialize it.
	FClassProperty *Property = &FrameProperties[*NumFrameProperties];
	*Property = *this;
	int EffectiveSize;

	// Set length and any special items required.
	switch( Type )
	{
		case CPT_Byte:
			EffectiveSize = sizeof(BYTE);
			break;

		case CPT_Integer:
			EffectiveSize = sizeof(INT);
			break;

		case CPT_Boolean:
			if( *NumFrameProperties && (Property[-1].Type==CPT_Boolean) &&
				(Property[-1].BitMask<<1) )
			{
				// Continue bit flag from the previous DWORD.
				BitMask = Property[-1].BitMask << 1;
				Property[-1].ElementSize = 0;
			}
			else // Create a new DWORD bit mask for this bit field.
			{
				BitMask = 1;
			}
			EffectiveSize = sizeof(DWORD);
			break;

		case CPT_Real:
			EffectiveSize = sizeof(FLOAT);
			break;

		case CPT_Actor:
			EffectiveSize = sizeof(INDEX);
			break;

		case CPT_Resource:
			EffectiveSize = sizeof(UResource *);
			break;

		case CPT_Name:
			EffectiveSize = sizeof(FName);
			break;

		case CPT_String:
			EffectiveSize = sizeof(char);
			break;

		case CPT_Vector:
			EffectiveSize = sizeof(FVector);
			break;

		case CPT_Rotation:
			EffectiveSize = sizeof(FRotation);
			break;

		default:
			appErrorf("Unknown property type",Name.Name());
			break;
	}

	// If this is the first property of a child class, it must be aligned
	// because of the way Visual C++ force-aligns the first new member of
	// a derived class.
	if( *NumFrameProperties && (Property[-1].Flags & CPF_FromParent) )
		EffectiveSize = 4;

	// Compute this property's offset.
	Offset = PropertyFrameEndOffset
	(
		*NumFrameProperties,FrameProperties,EffectiveSize
	);

	// Add to frame.
	*Property		= *this;
	(*NumFrameProperties)++;

	// Return it.
	return Property;
	unguard;
}

/*-----------------------------------------------------------------------------
	Data management.
-----------------------------------------------------------------------------*/

//
// Initialize a class property value.  FrameDataStart is a pointer to the beginning
// of the property frame where this property resides.
//
int FClassProperty::InitPropertyData(BYTE *FrameDataStart,FToken *InitToken)
{
	guard(FClassProperty::InitPropertyData);
	BYTE *Data = &FrameDataStart[Offset];

	// Init each element of array (or the single element, if not an array).
	switch( Type )
	{
		case CPT_Byte:
		{
			int v=0;
			if (InitToken && !InitToken->GetInteger(&v)) 
				return 0;

			for (INT i=0; i<ArrayDim; i++)
				{
				*(BYTE *)Data = v;
				Data += sizeof(BYTE);
				};
			break;
		}
		case CPT_Integer:
		{
			int v=0;
			if (InitToken && !InitToken->GetInteger(&v)) 
				return 0;

			for (INT i=0; i<ArrayDim; i++)
			{
				*(INT *)Data = v;
				Data += sizeof(INT);
			}
			break;
		}
		case CPT_Boolean:	
		{
			// Note that boolean arrays aren't allowed.
			int v=0;
			if (InitToken && !InitToken->GetInteger(&v))
				return 0;

			if (v)
				*(DWORD *)Data |=  BitMask;
			else
				*(DWORD *)Data &= ~BitMask;
			break;
		}
		case CPT_Real:
		{
			FLOAT v=0;
			if (InitToken && !InitToken->GetFloat(&v)) 
				return 0;

			for( INT i=0; i<ArrayDim; i++ )
			{
				*(FLOAT *)Data = v;
				Data += sizeof(FLOAT);
			}
			break;
		}
		case CPT_Actor:
		{
			// Actor initializers aren't allowed.
			if( InitToken ) 
				return 0;

			for( INT i=0; i<ArrayDim; i++ )
			{
				*(INDEX *)Data = INDEX_NONE;
				Data += sizeof(INDEX);
			}
			break;
		}
		case CPT_Resource:
		{
			UResource *R = NULL;
			if (InitToken && !InitToken->GetResource(&R,ResType))
				return 0;

			for( INT i=0; i<ArrayDim; i++ )
			{
				*(UResource **)Data = R;
				Data += sizeof(UResource *);
			}
			break;
		}
		case CPT_Name:
		{
			FName Name=NAME_NONE;
			if (InitToken && !InitToken->GetName(&Name))
				return 0;

			for( INT i=0; i<ArrayDim; i++ )
			{
				*(FName *)Data = Name;
				Data += sizeof(FName);
			}
			break;
		}
		case CPT_String:
		{
			mymemset(Data,0,ArrayDim * ElementSize * sizeof(char));

			if (InitToken)
			{
				if (InitToken->Type!=TOKEN_StringConst) 
					return 0;

				for( INT i=0; i<ArrayDim; i++ )
				{
					mystrncpy((char *)Data,InitToken->String,ArrayDim);
					Data += ElementSize;
				}
			}
			break;
		}
		case CPT_Vector:
			{
			FVector V = GMath.ZeroVector;
			if (InitToken && !InitToken->GetVector(&V))
				return 0;

			for( INT i=0; i<ArrayDim; i++ )
			{
				*(FVector *)Data = V;
				Data += sizeof(FVector);
			}
			break;
		}
		case CPT_Rotation:
		{
			FRotation R = GMath.ZeroRotation;
			if (InitToken && !InitToken->GetRotation(&R)) 
				return 0;

			for ( INT i=0; i<ArrayDim; i++ )
			{
				*(FRotation *)Data = R;
				Data += sizeof(FRotation);
			}
			break;
		}
		default:
		{
			appErrorf("Unknown property type",Name.Name());
			break;
		}
	}
	return 1;
	unguard;
}

//
// Compare a common property that exists in two compatible actors.
// Returns 1 if equal, 0 if not equal.
//
int FClassProperty::Compare(const AActor *Actor1, const AActor *Actor2, int iElement)
{
	guard(FClassProperty::Compare);

	int			ThisOffset	= Offset + iElement * ElementSize;
	const void *P1			= &((BYTE *)Actor1)[ThisOffset];
	const void *P2			= &((BYTE *)Actor2)[ThisOffset];

	switch( Type )
	{
		case CPT_None:
			return 0;

		case CPT_Byte:
			return *(BYTE *)P1 == *(BYTE *)P2;

		case CPT_Integer:
			return *(INT *)P1 == *(INT *)P2;

		case CPT_Boolean:
			return ((*(INT *)P1 ^ *(INT *)P2) & BitMask) == 0;

		case CPT_Real:
			return *(FLOAT *)P1 == *(FLOAT *)P2;

		case CPT_Actor:
			return *(INDEX *)P1 == *(INDEX *)P2;

		case CPT_Resource:
			return *(UResource **)P1 == *(UResource **)P2;

		case CPT_Name:
			return *(FName *)P1 == *(FName *)P2;

		case CPT_String:
			return strcmp((char *)P1,(char *)P2)==0;

		case CPT_Vector:
			return *(FVector *)P1 == *(FVector *)P2;

		case CPT_Rotation:
			return *(FRotation *)P1 == *(FRotation *)P2;

		default:
			appErrorf("Bad property type %i",Type);
			return 0;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Exporting.
-----------------------------------------------------------------------------*/

//
// Export this class property to a buffer, using the native text .TCX format.
// Returns the new end-of-buffer pointer.
//
char *FClassProperty::ExportTCX( char *Buffer, BYTE *Data )
{
	guard(FClassProperty::ExportTCX);
	char Extra[80]=" ";

	if( !(Flags & (CPF_Param | CPF_ReturnValue)) )
	{
		Buffer += sprintf(Buffer,"Dim ");
		if (Flags & CPF_Private)		strcat(Extra,"Private ");
		if (Flags & CPF_Const)			strcat(Extra,"Const ");
		if (Flags & CPF_Edit)			strcat(Extra,"Editable ");
		if (Flags & CPF_NoSaveResource)	strcat(Extra,"NoSave ");
		if (Flags & CPF_ExportResource)	strcat(Extra,"ExportResource ");
	}

	if( !(Flags & CPF_ReturnValue) )
	{
		Buffer += sprintf(Buffer,"%s ",Name.Name());
	}

	switch( Type )
	{
		case CPT_Byte:		
			if (Enum)	Buffer+=sprintf(Buffer,"as%s%s",          Extra,Enum->Name);
			else		Buffer+=sprintf(Buffer,"as%sByte",        Extra); 
			break;
		case CPT_Integer:	Buffer+=sprintf(Buffer,"as%sInteger", Extra); break;
		case CPT_Boolean:	Buffer+=sprintf(Buffer,"as%sBoolean", Extra); break;
		case CPT_Real:		Buffer+=sprintf(Buffer,"as%sReal",    Extra); break;
		case CPT_Actor:		Buffer+=sprintf(Buffer,"as%sActor",   Extra); break;
		case CPT_Resource:	Buffer+=sprintf(Buffer,"as%s%s",      Extra,GRes.GetTypeName(ResType)); break;
		case CPT_Name:		Buffer+=sprintf(Buffer,"as%sName",    Extra); break;
		case CPT_String:	Buffer+=sprintf(Buffer,"as%sString",  Extra); break;
		case CPT_Vector:	Buffer+=sprintf(Buffer,"as%sVector",  Extra); break;
		case CPT_Rotation:	Buffer+=sprintf(Buffer,"as%sRotation",Extra); break;
		default:			Buffer+=sprintf(Buffer,"#Error"); break;
	}
	
	if( ArrayDim )
	{
		Buffer += sprintf(Buffer,"(%i)",ArrayDim);
	}

	// If the property is not editable, and its initializer value is non-default, should
	// append "=Value" to it.
	if( Data && !(Flags & CPF_Edit) )
	{
		Data = &Data[Offset];

		switch( Type )
		{
			case CPT_Byte:
			{
				BYTE Temp = *(BYTE *)Data;
				if( Temp ) Buffer += sprintf( Buffer, "=%i", Temp );
				break;
			}
			case CPT_Integer:
			{
				INT Temp = *(INT *)Data;
				if( Temp ) Buffer += sprintf( Buffer, "=%i", Temp );
				break;
			}
			case CPT_Boolean:
			{
				INT Temp = *(DWORD *)Data & BitMask;
				if( Temp ) Buffer += sprintf( Buffer, "=1" );
				break;
			}
			case CPT_Real:
			{
				FLOAT Temp = *(FLOAT *)Data;
				if( Temp != 0.0 ) Buffer += sprintf( Buffer, "=%%+013.6f", Temp );
				break;
			}
			case CPT_Actor:
			{
				INDEX Temp = *(INDEX *)Data;
				if( Temp != INDEX_NONE ) Buffer += sprintf( Buffer, "=%i", Temp );
				break;
			}
			case CPT_Resource:
			{
				UResource *Temp = *(UResource **)Data;
				if( Temp ) Buffer += sprintf( Buffer, "=%s", Temp->Name );
				break;
			}
			case CPT_Name:
			{
				FName Temp = *(FName *)Data;
				if( Temp != NAME_NONE ) Buffer += sprintf( Buffer, "=%s", Temp.Name() );
				break;
			}
			case CPT_String:
			{
				char *Temp = (char *)Data;
				if( *Temp ) Buffer += sprintf( Buffer, "=\"%s\"", Temp );
				break;
			}
			case CPT_Vector:
			{
				FVector *Temp = (FVector *)Data;
				if( *Temp != GMath.ZeroVector ) 
					Buffer += sprintf(Buffer,"(%+013.6f,%+013.6f,%+013.6f)",Temp->X,Temp->Y,Temp->Z);
				break;
			}
			case CPT_Rotation:
			{
				FRotation *Temp = (FRotation *)Data;
				if ( *Temp != GMath.ZeroRotation )
					Buffer += sprintf(Buffer,"(%i,%i,%i)",Temp->Pitch,Temp->Yaw,Temp->Roll);
				break;
			}
			default:
			{
				Buffer += sprintf(Buffer,"#Error"); 
				break;
			}
		}
	}
	return Buffer;
	unguard;
}

//
// Export this class property to a buffer as a C++ header file.
// Returns the new end-of-buffer pointer.
//
char *FClassProperty::ExportH( char *Buffer )
{
	guard(FClassProperty::ExportH)
	char ArrayStr[80]="";

	if ( Flags & CPF_Array )
		sprintf(ArrayStr,"[%i]",ArrayDim);

	switch(Type)
	{
		case CPT_Byte:
			if (Enum)	Buffer+=sprintf(Buffer,"BYTE       %-24s%s /* enum %s */;",Name.Name(),ArrayStr,Enum->Name);
			else		Buffer+=sprintf(Buffer,"BYTE       %s%s;",Name.Name(),ArrayStr);
			break;
		case CPT_Integer:	Buffer+=sprintf(Buffer,"INT        %s%s;",  Name.Name(),ArrayStr); break;
		case CPT_Boolean:	Buffer+=sprintf(Buffer,"DWORD      %s%s:1;",Name.Name(),ArrayStr); break;
		case CPT_Real:		Buffer+=sprintf(Buffer,"FLOAT      %s%s;",  Name.Name(),ArrayStr); break;
		case CPT_Actor:		Buffer+=sprintf(Buffer,"INDEX      i%s%s;", Name.Name(),ArrayStr); break;
		case CPT_Resource:	Buffer+=sprintf(Buffer,"U%-9s *%s%s;",GRes.GetTypeName(ResType),Name.Name(),ArrayStr); break;
		case CPT_Name:		Buffer+=sprintf(Buffer,"FName      %s%s;",  Name.Name(),ArrayStr); break;
		case CPT_String:	Buffer+=sprintf(Buffer,"CHAR       %s[%i]%s;",Name.Name(),ElementSize,ArrayStr); break;
		case CPT_Vector:	Buffer+=sprintf(Buffer,"FVector    %s%s;",  Name.Name(),ArrayStr); break;
		case CPT_Rotation:	Buffer+=sprintf(Buffer,"FRotation  %s%s;",  Name.Name(),ArrayStr); break;
	}
	return Buffer;
	unguard;
}

/*-----------------------------------------------------------------------------
	Utility functions.
-----------------------------------------------------------------------------*/

char *GetPropertyTypeName( EClassPropertyType Type )
{
	guard(GetPropertyTypeName);

	static char *PropertyTypeNames[CPT_MAX] =
	{
		"None","Byte","Integer","Boolean",
		"Real","Actor","Resource","Name","String","Vector","Rotation"
	};

	if( Type<0 || Type>=CPT_MAX )
		appError("Invalid property type");

	return PropertyTypeNames[Type];

	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
