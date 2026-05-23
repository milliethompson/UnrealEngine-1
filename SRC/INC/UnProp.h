/*=============================================================================
	UnProp.h: Actor class property definition.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * Aug 30, 1996: Mark added PLevel
		* Oct 19, 1996: Tim redesigned to eliminate redundency
=============================================================================*/

#ifndef _INC_UNPROP
#define _INC_UNPROP

/*-----------------------------------------------------------------------------
	FProperty.
-----------------------------------------------------------------------------*/

//
// Flags associated with each property in a class, overriding the
// property's default behavior.
//
enum EPropertyFlags
{
	// Regular flags.
	CPF_Edit			= 0x0001,	// Property is user-settable in the editor.
	CPF_Const			= 0x0002,	// Actor's property always matches class's default actor property.
	CPF_Private			= 0x0004,	// Private, inaccessible to scripts.
	CPF_ExportObject    = 0x0008,	// Object can be exported with actor.
	CPF_OptionalParm	= 0x0010,	// Optional parameter (if CPF_Param is set).
	CPF_Net			    = 0x0020,	// Property should be replicated on client side.
	CPF_NetSelf		    = 0x0040,	// Property should be replicated on client side iff actor is owned by client.
	CPF_Parm			= 0x0080,	// Function/When call parameter.
	CPF_OutParm			= 0x0100,	// Value is copied out after function call.
	CPF_SkipParm		= 0x0200,	// Property is a short-circuitable evaluation function parm.
	CPF_ReturnParm		= 0x0400,	// Return value.
	CPF_CoerceParm		= 0x0800,	// Coerce a parameter.
	CPF_Transient       = 0x2000,   // Property is transient: shouldn't be saved, reinitialized to default at load time.

	// Combinations of flags.
	CPF_ParmFlags		= CPF_OptionalParm | CPF_Parm | CPF_OutParm | CPF_SkipParm | CPF_ReturnParm | CPF_CoerceParm, // All function parm flags.
};

//
// Class property data types.
//warning: Script compiler has a hardcoded table based on these enum indices.
//
enum EPropertyType
{
	CPT_None		= 0,	// No property					(0 bytes)
	CPT_Byte		= 1,	// Byte value, 0 to 256			(1 byte)
	CPT_Int			= 2,	// 32-bit signed integer		(4 bytes)
	CPT_Bool		= 3,	// Bit flag within 32-bit dword	(0 or 4 bytes)
	CPT_Float		= 4,	// Floating point number		(4 bytes)
	CPT_Object		= 5,	// Object pointer				(4 bytes)
	CPT_Name		= 6,	// FName, a global name			(2 bytes)
	CPT_String		= 7,	// Null terminated string		(0+ bytes)
	CPT_Vector		= 8,	// Floating point vector		(16 bytes)
	CPT_Rotation	= 9,	// Pitch-yaw-roll rotation		(12 bytes)
	CPT_EnumDef		= 10,	// Enumeration definition		(0 bytes)
	CPT_MAX			= 11,
};

//
// One class property that resides in a given class.  A property describes
// a variable name, an offset within the actor's data, a data type, property flags,
// and optional, type-dependent data.
//
class UNENGINE_API FProperty // 32 bytes.
{
public:
	// Variables.
	FName			Name;			// Name of property, i.e. "Strength", "Intelligence".
	FName			Category;		// Property category name.
	EPropertyType	Type;			// Type of property.
	INT				Offset;			// Offset into property data for fast access.
	INT				Flags;			// Property flags, from EClassPropertyFlags.
	INT				ArrayDim;		// Dimension of array, 1 = a single variable, not an array.
	INT				ElementSize;	// Length of one element of the array (=PropertyLength if ArraySize=0 or 1).
	EPropertyBin	Bin;			// Which bin the property falls into.
	union
	{
		UEnumDef	*Enum;			// CPT_Byte:   Optional enum definition (or NULL=none).
		UClass		*Class;			// CPT_Object: Required IsA class.
		DWORD		BitMask;		// CPT_Bool:   Required bit mask.
	};

	// Constructors.
	FProperty()
	{}
	FProperty( EPropertyType InType )
	{
		Init( PROPBIN_MAX, InType );
	}
	FProperty( EPropertyType InType, UClass *InClass )
	{
		checkState(InType==CPT_Object);
		Init( PROPBIN_MAX, InType );
		Class = InClass;
	}

	// Inlines.
	void Init( EPropertyBin InBin, EPropertyType InType )
	{
		// Set regular info.
		Name		= NAME_None;
		Category	= NAME_None;
		Type		= InType;
		Offset		= 0;
		Flags		= 0;
		ArrayDim    = 1;
		Bin			= InBin;
		BitMask		= 0;

		// Set element size.
		switch( Type )
		{
			case CPT_None:		ElementSize = 0;					break;
			case CPT_Byte:		ElementSize = sizeof(BYTE);			break;
			case CPT_Int:		ElementSize = sizeof(INT);			break;
			case CPT_Bool:		ElementSize = sizeof(DWORD);		break;
			case CPT_Float:		ElementSize = sizeof(FLOAT);		break;
			case CPT_Object:	ElementSize = sizeof(UObject*);		break;
			case CPT_Name:		ElementSize = sizeof(FName);		break;
			case CPT_String:	ElementSize = 0;					break;
			case CPT_Vector:	ElementSize = sizeof(FVector);		break;
			case CPT_Rotation:	ElementSize = sizeof(FRotation);	break;
			case CPT_EnumDef:	ElementSize = 0;					break;
			default: appErrorf("FProperty::Init: invalid type %i",Type);
		};
	}
	INT Size() const
	{
		return ElementSize * ArrayDim;
	}

	// Functions.
	int  Matches     (BYTE *const*Bin1, BYTE *const*Bin2, int Element) const;
	int  MatchesType (const FProperty &Other, BOOL Identity) const;
	void ExportU     (FOutputDevice &Out, BYTE *Data);
	void ExportH     (FOutputDevice &Out);
	void ExportText
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
	);

	// Serializer.
	friend FArchive& operator<< (FArchive &Ar, FProperty &Property)
	{
		guard(FProperty<<);
		Ar << Property.Name << Property.Category;
		Ar << Property.Type;
		Ar << Property.Offset << Property.Flags;
		Ar << Property.ArrayDim << Property.ElementSize << Property.Bin;

		if( Property.Type == CPT_Byte )
			Ar << Property.Enum;
		else if( Property.Type == CPT_Bool )
			Ar << Property.BitMask;
		else if( Property.Type == CPT_Object )
			Ar << Property.Class;
		else if( Property.Type == CPT_EnumDef )
			Ar << Property.Enum;

		return Ar;
		unguard;
	}

	// Data serialization.
	void SerializeProperty( FArchive& Ar, BYTE *Value, BYTE *Default );
};

/*-----------------------------------------------------------------------------
	UProperties.
-----------------------------------------------------------------------------*/

//
// A buffer containing class property data.
//
class UNENGINE_API UProperties : public UBuffer
{
	DECLARE_DB_CLASS(UProperties,UBuffer,BYTE,NAME_Properties,NAME_UnEngine);

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// General variables.
	UClass			*Class;			// Class containing the property definitions.
	EPropertyBin	Bin;			// Property bin of the class for these properties.

	// Constructor.
	UProperties(UClass* InClass, EPropertyBin InBin)
	:	UBuffer(0,0), Class(InClass), Bin(InBin) {}

	// UObject interface.
	void InitHeader();
	void SerializeHeader(FArchive& Ar)
	{
		guard(UProperties::SerializeHeader);
		UBuffer::SerializeHeader(Ar);
		Ar << Class << Bin;
		unguardobj;
	}
	void SerializeData( FArchive& Ar );

	// UProperties interface.
	BYTE *Get( FProperty &Property, int iElement=0 )
	{
		return &Element(0) + Property.Offset + iElement * Property.ElementSize;
	}
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNPROP
