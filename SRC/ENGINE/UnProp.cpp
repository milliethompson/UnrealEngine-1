/*=============================================================================
	UnClsPrp.cpp: FProperty implementation

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Architectural note:
		* This matrix code organization sucks!

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	Data serialization.
-----------------------------------------------------------------------------*/

//
// Serialize this property's data.
//
void FProperty::SerializeProperty( FArchive& Ar, BYTE *Value, BYTE *Default )
{
	guard(FProperty::SerializeProperty);
	if( !(Flags & CPF_Transient) )
	{
		// Serialize this.
		for( int j=0; j<ArrayDim; j++ ) switch( Type )
		{
			case CPT_Object:
				Ar << ((UObject **)Value)[j];
				break;
			case CPT_Name:
				Ar << ((FName *)Value)[j];
				break;
			case CPT_Byte:
				Ar << ((BYTE *)Value)[j];
				break;
			case CPT_Int:
				Ar << ((INT *)Value)[j];
				break;
			case CPT_Bool:
				if( BitMask == 1 )
					Ar << ((DWORD *)Value)[j];
				break;
			case CPT_Float:
				Ar << ((FLOAT *)Value)[j];
				break;
			case CPT_String:
				Ar.String( (char *)(Value + j*ElementSize), ElementSize );
				break;
			case CPT_Vector:
				Ar << ((FVector *)Value)[j];
				break;
			case CPT_Rotation:
				Ar << ((FRotation *)Value)[j];
				break;
			case CPT_EnumDef:
				break;
			default:
				appErrorf( "Bad property type %i",Type );
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Data management.
-----------------------------------------------------------------------------*/

//
// Compare a common property that exists in two compatible actors.
// Returns 1 if equal, 0 if not equal.
//
int FProperty::Matches( BYTE *const*Bin1, BYTE *const*Bin2, int iElement ) const
{
	guard(FProperty::Compare);

	int			ThisOffset	= Offset + iElement * ElementSize;
	const void *P1			= &Bin1[Bin][ThisOffset];
	const void *P2			= &Bin2[Bin][ThisOffset];

	switch( Type )
	{
		case CPT_None:
			return 0;

		case CPT_Byte:
			return *(BYTE *)P1 == *(BYTE *)P2;

		case CPT_Int:
			return *(INT *)P1 == *(INT *)P2;

		case CPT_Bool:
			return ((*(INT *)P1 ^ *(INT *)P2) & BitMask) == 0;

		case CPT_Float:
			return *(FLOAT *)P1 == *(FLOAT *)P2;

		case CPT_Object:
			return *(UObject **)P1 == *(UObject **)P2;

		case CPT_Name:
			return *(FName *)P1 == *(FName *)P2;

		case CPT_String:
			return strcmp((char *)P1,(char *)P2)==0;

		case CPT_Vector:
			return *(FVector *)P1 == *(FVector *)P2;

		case CPT_Rotation:
			return *(FRotation *)P1 == *(FRotation *)P2;

		case CPT_EnumDef:
			return 1;

		default:
			appErrorf("Bad property type %i",Type);
			return 0;
	}
	unguard;
}

//
// Return wehter we can safely convert Other type to this type.
//
int FProperty::MatchesType( const FProperty &Other, BOOL Identity ) const
{
	guard(FProperty::CompareTypes);
	checkInput(Type!=CPT_None || !Identity);

	// If converting to an l-value, we require an exact match with an l-value.
	if( Flags & CPF_OutParm )
	{
		if( (Other.Flags & CPF_Const) || !(Other.Flags & CPF_OutParm) )
			return 0;
		Identity = 1;
	}

	if( Type==CPT_None && (Other.Type==CPT_None || !Identity) )
	{
		// If Other has no type, accept anything.
		return 1;
	}
	else if( Type != Other.Type )
	{
		// Mismatched base types.
		return 0;
	}
	else if( ArrayDim != Other.ArrayDim )
	{
		// Mismatched array dimensions.
		return 0;
	}
	else if( Type==CPT_Byte )
	{
		// Make sure enums match, or we're generalizing.
		return Enum==Other.Enum || (Enum==NULL && !Identity);
	}
	else if( Type==CPT_Object )
	{
		checkState(Class!=NULL);

		// Make sure object types match, or we're generalizing.
		if     ( Identity          ) return Class==Other.Class;            // Exact match required.
		else if( Other.Class==NULL ) return 1;                             // Cannonical matches all types.
		else                         return Other.Class->IsChildOf(Class); // Generalization is ok.
	}
	else if( Type==CPT_String )
	{
		// Make sure lengths match, or we're generalizing (widening).
		if( Identity )  return ElementSize == Other.ElementSize;
		else            return ElementSize >= Other.ElementSize;
	}
	else
	{
		// General match.
		return 1;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Exporting.
-----------------------------------------------------------------------------*/

//
// Export this class property to a buffer, using the native text .u format.
// Returns the new end-of-buffer pointer.
//
void FProperty::ExportU( FOutputDevice &Out, BYTE *Data )
{
	guard(FProperty::ExportU);

	// Variable bin and modifiers.
	if( !(Flags & (CPF_Parm | CPF_ReturnParm)) )
	{
		// Static or Dim.
		if     ( Bin == PROPBIN_PerClass    ) Out.Logf("Static ");
		else if( Bin == PROPBIN_PerFunction ) Out.Logf("Local ");
		else							      Out.Logf("Var ");

		// Get overrides.
		if( Flags & CPF_Edit                ) Out.Logf("Editable ");
		if( Flags & CPF_Const               ) Out.Logf("Const ");
		if( Flags & CPF_Private             ) Out.Logf("Private ");
		if( Flags & CPF_ExportObject        ) Out.Logf("ExportObject ");
		if( Flags & CPF_Net                 ) Out.Logf("Net ");
		if( Flags & CPF_NetSelf             ) Out.Logf("NetSelf ");
	}
	else if( (Flags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm )
	{
		if( Flags & CPF_OutParm             ) Out.Logf("Out ");
	}

	// Type.
	switch( Type )
	{
		case CPT_Int:
			Out.Logf( "Int" );
			break;

		case CPT_Bool:
			Out.Logf( "Bool" );
			break;
		case CPT_Float:
			Out.Logf( "Float" );
			break;
		case CPT_Name:
			Out.Logf( "Name" );
			break;
		case CPT_Vector:
			Out.Logf( "Vector" );
			break;
		case CPT_Rotation:
			Out.Logf( "Rotation" );
			break;
		case CPT_EnumDef:
			Out.Logf( "EnumDef" );
			break;
		case CPT_String:
			Out.Logf( "String[%i]", ElementSize );
			break;
		case CPT_Object:
			Out.Logf( "%s", Class->GetName() );
			break;
		case CPT_Byte:
			if( Enum )
				Out.Logf( "%s", Enum->GetName() );
			else
				Out.Logf( "Byte" );
			break;
		default:
			appErrorf("Unknown type %i",Type);
			break;
	}

	// Name.
	if( !(Flags & CPF_ReturnParm) )
		Out.Logf(" %s",Name());

	// Array dimension.
	if( ArrayDim > 1 )
		Out.Logf(" [%i]",ArrayDim);


	// If the property is not editable, and its initializer value is non-default, should
	// append "=Value" to it.
	if( Data && !(Flags & CPF_Edit) )
	{
		switch( Type )
		{
			case CPT_Byte:
			{
				BYTE Temp = *(BYTE *)Data;
				if( Temp ) Out.Logf( "=%i", Temp );
				break;
			}
			case CPT_Int:
			{
				INT Temp = *(INT *)Data;
				if( Temp ) Out.Logf( "=%i", Temp );
				break;
			}
			case CPT_Bool:
			{
				INT Temp = *(DWORD *)Data & BitMask;
				if( Temp ) Out.Logf( "=True" );
				break;
			}
			case CPT_Float:
			{
				FLOAT Temp = *(FLOAT *)Data;
				if( Temp != 0.0 ) Out.Logf( "=%+013.6f", Temp );
				break;
			}
			case CPT_Object:
			{
				UObject *Temp = *(UObject **)Data;
				if( Temp ) Out.Logf( "=%s", Temp->GetName() );
				break;
			}
			case CPT_Name:
			{
				FName Temp = *(FName *)Data;
				if( Temp != NAME_None ) Out.Logf( "=%s", Temp() );
				break;
			}
			case CPT_String:
			{
				char *Temp = (char *)Data;
				if( *Temp ) Out.Logf( "=\"%s\"", Temp );
				break;
			}
			case CPT_Vector:
			{
				FVector *Temp = (FVector *)Data;
				if( *Temp != GMath.ZeroVector ) 
					Out.Logf( "(%+013.6f,%+013.6f,%+013.6f)",Temp->X,Temp->Y,Temp->Z);
				break;
			}
			case CPT_Rotation:
			{
				FRotation *Temp = (FRotation *)Data;
				if ( *Temp != GMath.ZeroRotation )
					Out.Logf( "(%i,%i,%i)",Temp->Pitch,Temp->Yaw,Temp->Roll);
				break;
			}
			case CPT_EnumDef:
			{
				break;
			}
			default:
			{
				Out.Logf( "#Error"); 
				break;
			}
		}
	}
	unguard;
}

//
// Export this class property to an output
// device as a C++ header file.
//
void FProperty::ExportH( FOutputDevice &Out )
{
	guard(FProperty::ExportH)
	char ArrayStr[80]="";

	if ( ArrayDim != 1 )
		sprintf(ArrayStr,"[%i]",ArrayDim);

	switch(Type)
	{
		case CPT_Byte:
			if (Enum)		Out.Logf("BYTE %s%s /* enum %s */;",Name(),ArrayStr,Enum->GetName());
			else			Out.Logf("BYTE %s%s;",Name(),ArrayStr);
			break;
		case CPT_Bool:
			if( ArrayDim==1 ) Out.Logf("DWORD %s%s:1;",Name(),ArrayStr);
			else			  Out.Logf("DWORD %s%s;",Name(),ArrayStr);
			break;
		case CPT_Int:
			Out.Logf("INT %s%s;",  Name(),ArrayStr);
			break;
		case CPT_Float:
			Out.Logf("FLOAT %s%s;",  Name(),ArrayStr);
			break;
		case CPT_Name:
			Out.Logf("FName %s%s;",  Name(),ArrayStr);
			break;
		case CPT_String:
			Out.Logf("CHAR %s[%i]%s;",Name(),ElementSize,ArrayStr);
			break;
		case CPT_Vector:
			Out.Logf("FVector %s%s;",  Name(),ArrayStr);
			break;
		case CPT_Rotation:
			Out.Logf("FRotation %s%s;",  Name(),ArrayStr);
			break;
		case CPT_EnumDef:
			break;
		case CPT_Object:
			Out.Logf
			(
				"class %s%s *%s%s;",
				Class->IsChildOf("Actor") ? "A" : "U",
				Class->GetName(),
				Name(),
				ArrayStr
			);
			break;
	}
	unguard;
}

//
// Export the contents of a property.
//
void FProperty::ExportText
(
	char*		TypeStr,
	char*		NameStr,
	char*		ValueStr,
	BYTE*const*	ActorBins,
	int			InFlags,
	int			Descriptive, 
	int			ArrayElement,
	BYTE*const*	DeltaBins,
	FName		InCategory
)
{
	guard(FProperty::ExportText);
	checkInput(Name!=NAME_None);

	// Init caller values.
	TypeStr [0]=0; 
	NameStr [0]=0; 
	ValueStr[0]=0;

	// Get property pointer.
	BYTE *PropertyValue = &ActorBins[Bin][Offset + ArrayElement * ElementSize];
	if 
	(
		((Flags & InFlags) || !InFlags) 
	&&	(!DeltaBins || !Matches(ActorBins,DeltaBins,ArrayElement))
	&&	( InCategory==NAME_None || InCategory==Category )
	)
	{
		strcpy( NameStr, Name() );
		switch( Type )
		{
			case CPT_Byte:
			{
				guardSlow(Byte);
				BYTE Temp = *(BYTE *)PropertyValue;

				if( Enum ) sprintf( TypeStr, "BYTE.%s", Enum->GetName() );
				else       strcpy ( TypeStr, "BYTE" );

				if( Enum && Descriptive==1 )
					sprintf( ValueStr, "%i - %s", Temp, Enum->Element(Temp)() );
				else if( Enum && Descriptive==0 )
					sprintf( ValueStr, "%s", Enum->Element(Temp)() );
				else
					sprintf( ValueStr, "%i", Temp );
				break;
				unguardSlow;
			}
			case CPT_Int:
			{
				guardSlow(Int);
				strcpy(TypeStr,"INTEGER");
				sprintf( ValueStr, "%i", *(INT *)PropertyValue );
				break;
				unguardSlow;
			}
			case CPT_Float:
			{
				guardSlow(Float);
				strcpy(TypeStr,"REAL");
				sprintf(ValueStr,"%+013.6f",*(FLOAT *)PropertyValue);
				break;
				unguardSlow;
			}
			case CPT_String:
			{
				guardSlow(String);
				strcpy(TypeStr,"STRING");
				if (Descriptive==1)
					sprintf(ValueStr,"%s",(char *)PropertyValue);
				else
					sprintf(ValueStr,"\"%s\"",(char *)PropertyValue);
				break;
				unguardSlow;
			}
			case CPT_Bool:
			{
				guardSlow(Bool);
				strcpy(TypeStr,"BOOLEAN");
				char *Temp = ((*(DWORD *)PropertyValue) & BitMask) ? "True" : "False";
				sprintf(ValueStr,"%s",Temp);
				break;
				unguardSlow;
			}
			case CPT_Object:
			{
				guardSlow(Object);
				strcpy( TypeStr, Class->GetName() );
				UObject *Temp = *(UObject **)PropertyValue;
				if( Temp != NULL ) strcpy( ValueStr, Temp->GetName() );
				else               strcpy( ValueStr, "None"          );
				break;
				unguardSlow;
			}
			case CPT_Name:
			{
				guardSlow(Name);
				strcpy(TypeStr,"NAME");
				FName Temp = *(FName *)PropertyValue;
				strcpy(ValueStr,Temp());
				break;
				unguardSlow;
			}
			case CPT_Vector:
			{
				guardSlow(Vector);
				strcpy(TypeStr,"VECTOR");
				FVector *Temp = (FVector *)PropertyValue;
				sprintf(ValueStr,"(%+013.6f,%+013.6f,%+013.6f)",Temp->X,Temp->Y,Temp->Z);
				break;
				unguardSlow;
			}
			case CPT_Rotation:
			{
				guardSlow(Rotation);
				strcpy(TypeStr,"ROTATION");
				FRotation *TempRot = (FRotation *)PropertyValue;
				sprintf(ValueStr,"(%i,%i,%i)",TempRot->Pitch,TempRot->Yaw,TempRot->Roll);
				break;
				unguardSlow;
			}
			case CPT_EnumDef:
			{
				guardSlow(EnumDef);
				break;
				unguardSlow;
			}
			default:
			{
				appErrorf("Unknown property type");
				break;
			}
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
