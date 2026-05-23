/*=============================================================================
	UnClsPrp.h: Actor class property class definition.

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * Aug 30, 1996: Mark added PLevel
		* Oct 19, 1996: Tim redesigned to eliminate redundency
=============================================================================*/

#ifndef _INC_UNCLSPRP
#define _INC_UNCLSPRP

/*-----------------------------------------------------------------------------
	FClassProperty.
-----------------------------------------------------------------------------*/

//
// Flags associated with each property in a class, overriding the
// property's default behavior.
//
enum EClassPropertyFlags
{
	CPF_Edit			= 0x0001,	// Property is user-settable in the editor.
	CPF_Const			= 0x0004,	// Actor's property always matches class's default actor property.
	CPF_Private			= 0x0008,	// Private, inaccessible to scripts.
	CPF_NoSaveResource  = 0x0010,	// Don't save this CPT_Resource on disk.
	CPF_ExportResource	= 0x0020,	// Resource can be exported with actor.
	CPF_Param			= 0x0040,	// Function/When call parameter.
	CPF_OptionalParam	= 0x0080,	// Optional parameter (if CPF_Param is set).
	CPF_ReturnValue		= 0x0100,	// Return value.
	CPF_Initialized		= 0x0200,	// UnrealScript has specified an initializer value.
	CPF_Array			= 0x0400,	// This is an array.
	CPF_FromParent		= 0x0800,	// Property was inhereted from parent class.
};

//
// Class property data types.
//
enum EClassPropertyType
{
	CPT_None			= 0x00,		// No property							(0 bytes)
	CPT_Byte			= 0x01,		// Byte value, 0 to 256					(1 byte)
	CPT_Integer			= 0x02,		// 32-bit signed integer				(4 bytes)
	CPT_Boolean			= 0x03,		// Bit flag within 32-bit dword			(0 or 4 bytes)
	CPT_Real			= 0x04,		// Floating point number				(4 bytes)
	CPT_Actor			= 0x05,		// Actor index							(2 bytes)
	CPT_Resource		= 0x06,		// Resource pointer						(4 bytes)
	CPT_Name			= 0x07,		// FName, a global name					(2 bytes)
	CPT_String			= 0x08,		// Null terminated string				(0+ bytes)
	CPT_Vector			= 0x09,		// Floating point vector				(16 bytes)
	CPT_Rotation		= 0x0A,		// Pitch-yaw-roll rotation				(6 bytes)
	CPT_MAX				= 0x0B,
};

//
// One class property that resides in a given class.  A property describes
// a variable name, an offset within the actor's data, a data type, property flags,
// and optional, type-dependent data.
//
class UNREAL_API FClassProperty // 32 bytes
{
public:
	// Variables.
	FName			Name;			// Name of property, i.e. "Strength", "Intelligence".
	FName			Category;		// Property category name.
	EClassPropertyType Type;		// Type of property.
	INT				Offset;			// Offset into property data for fast access.
	INT				Flags;			// Property flags, from EClassPropertyFlags.
	INT				ArrayDim;		// Dimension of array, 0 = a single variable, not an array.
	INT				ElementSize;	// Length of one element of the array (=PropertyLength if ArraySize=0 or 1).
	INT				Bin;			// Which bin the property falls into.
	union
	{
		UEnum			*Enum;		// Enum (or NULL=none) if BYTE or INT.
		UClass			*Class;		// Optional IsKindOf class for CPT_Actor's (NULL=any kind is ok).
		EResourceType	ResType;	// Property resource ID for AP_RESOURCE.
		DWORD			BitMask;	// Bit mask for bit if type is AP_BITFLAG.
	};

	// Inlines.
	inline INT PropertySize() const
	{
		return ElementSize * ArrayDim;
	};
	inline INT IsPerActor() const
	{
		return (Bin==AActor::PROPBIN_Global) || (Bin==AActor::PROPBIN_Local);
	};

	// Functions.
	void Init();
	int  SetFlags(const char *Override);
	int  SetType(const char *Type,int ArraySize,int Extra,char *ErrorMessage);
	FClassProperty *AddToFrame(int *NumFrameProperties,FClassProperty *FrameProperties);
	int  InitPropertyData(BYTE *FrameDataStart,class FToken *OptionalInitToken=NULL);
	void DebugDump(char *Str);
	char *ExportTCX(char *Buffer,BYTE *Data);
	char *ExportH(char *Buffer);
	int Compare(const AActor *RawActor1, const AActor *RawActor2,int Element);
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNCLSPRP
