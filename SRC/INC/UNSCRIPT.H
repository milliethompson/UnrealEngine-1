/*=============================================================================
	UnScript.h: UnrealScript execution engine.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNSCRIPT
#define _INC_UNSCRIPT

/*-----------------------------------------------------------------------------
	Constants & types.
-----------------------------------------------------------------------------*/

// Sizes.
enum {MAX_NEST_LEVELS		= 16};
enum {MAX_IDENTIFIER_SIZE	= NAME_SIZE};
enum {MAX_STRING_CONST_SIZE	= 255};
enum {MAX_CONST_SIZE        = 16};
enum {MAX_FUNC_PARMS        = 16};

//
// guardexec mechanism for script debugging.
//
#define unguardexecSlow  unguardfSlow(("Script=%s Node=%i Code=%04x Context=%s",Stack.Link.Class->GetName(),Stack.Link.iNode,Stack.Code - &Stack.Object->GetClass()->Script->Element(0),Context?Context->GetClassName():"None"))
#define unguardexec      unguardf    (("Script=%s Node=%i Code=%04x Context=%s",Stack.Link.Class->GetName(),Stack.Link.iNode,Stack.Code - &Stack.Object->GetClass()->Script->Element(0),Context?Context->GetClassName():"None"))

//
// Information about a stack node.
//warning: Stored in a byte, limited to 8 flags.
//
enum EStackNodeFlags
{
	// Function flags.
	SNODE_IntrinsicFunc		= 0x01,		// Function is intrinsic (mapped to internal C++ function).
	SNODE_FinalFunc			= 0x02,		// Function is final (prebindable, non-overridable function).
	SNODE_PrivateFunc		= 0x04,		// Function is private (may not be called by scripts, only by C++ engine code).
	SNODE_DefinedFunc		= 0x08,		// Function has been defined (not just declared).
	SNODE_EngineFunc		= 0x10,		// Function is engine-generated and can be called without necessarily being defined.
	SNODE_LatentFunc		= 0x20,		// Function is a latent state function.
	SNODE_PreOperatorFunc	= 0x40,		// Unary operator is a prefix operator.

	// State flags.
	SNODE_EditableState		= 0x01,		// State should be user-selectable in UnrealEd.
	SNODE_AutoState			= 0x02,		// State is automatic (the default state).

	// Combinations of flags.
	SNODE_FuncOverrideMatch	= SNODE_IntrinsicFunc | SNODE_FinalFunc | SNODE_PreOperatorFunc, // Flags which must match in overridden functions.
};

//
// Code nesting types.
//
enum ENestType
{
	NEST_None				=0x0000,	//  No nesting.
	NEST_Class				=0x0001,	//  Class/EndClass.
	NEST_State				=0x0002,	//	State/EndState.
	NEST_Function			=0x0003,	//	Function/EndFunction.
	NEST_Operator			=0x0004,	//	Operator/EndOperator.
	NEST_If					=0x0005,	//  If/ElseIf/EndIf.
	NEST_Loop				=0x0006,	//  While/Do/Loop/Until.
	NEST_Switch				=0x0007,	//  Switch.
	NEST_For				=0x0008,	//  For.
	NEST_Max				=0x0009
};

//
// Evaluatable expression item types.
//
enum EExprToken
{
	// Variable references which map onto propery bin numbers.
	EX_MinVariable			= 0x00,		// Minimum variable token, must be 0.
	EX_LocalVariable		= 0x00,		// A local variable.
	EX_ObjectVariable		= 0x01,		// An object variable.
	EX_StaticVariable		= 0x02,		// A static variable.
	EX_UnusedVariable		= 0x03,		// Unused, reserved for state-locals.
	EX_MaxVariable			= 0x04,		// Maximum variable token.

	// Commands.
	EX_Return				= 0x04,		// Return from function.
	EX_Switch				= 0x05,		// Switch.
	EX_Jump					= 0x06,		// Goto a local address in code.
	EX_JumpIfNot			= 0x07,		// Goto if not expression.
	EX_Stop					= 0x08,		// Stop executing state code.
	EX_Assert				= 0x09,		// Assertion.
	EX_Case					= 0x0A,		// Case.
	EX_Nothing				= 0x0B,		// No operation.
	EX_LabelTable			= 0x0C,		// Table of labels.
	EX_GotoLabel			= 0x0D,		// Goto a label.
	EX_Broadcast			= 0x0E,		// Broadcast a function call.
	EX_Let					= 0x0F,		// Assign an arbitrary size value to a variable.
	EX_Let1					= 0x10,		// Assign to 1-byte variable.
	EX_Let4					= 0x11,		// Assign to 4-byte variable.
	EX_LetBool				= 0x12,		// Assign to bool varible.
	EX_LetString			= 0x13,		// Assign to string variable.
	EX_BeginFunction		= 0x14,		// Beginning of function in code.
	EX_EndCode				= 0x15,		// End of code block.
	EX_EndFunctionParms		= 0x16,		// End of function call parameters.
	EX_Self					= 0x17,		// Self actor.
	EX_Skip					= 0x18,		// Skippable expression.

	// Normal expression tokens.
	EX_Context				= 0x19,		// Call a function through an actor context.
	EX_ArrayElement			= 0x1A,		// Precedes EX_xxxVariable for array elements.
	EX_VirtualFunction		= 0x1B,		// A function call with parameters.
	EX_FinalFunction		= 0x1C,		// A prebound function call with parameters.
	EX_IntConst				= 0x1D,		// Int constant.
	EX_FloatConst			= 0x1E,		// Floating point constant.
	EX_StringConst			= 0x1F,		// String constant.
	EX_ObjectConst		    = 0x20,		// An object constant.
	EX_NameConst			= 0x21,		// A name constant.
	EX_RotationConst		= 0x22,		// A rotation constant.
	EX_VectorConst			= 0x23,		// A vector constant.
	EX_ByteConst			= 0x24,		// A byte constant.
	EX_IntZero				= 0x25,		// Zero.
	EX_IntOne				= 0x26,		// One.
	EX_True					= 0x27,		// Bool True.
	EX_False				= 0x28,		// Bool False.
	EX_DefaultVariable      = 0x29,     // Default value of a class's per-actor variable.
	EX_NoObject				= 0x2A,		// NoObject.
	EX_ResizeString			= 0x2B,		// Resize a string's length.
	EX_IntConstByte			= 0x2C,		// Int constant that requires 1 byte.
	EX_BoolVariable			= 0x2D,		// A bool variable which requires a bitmask.
	EX_ActorCast			= 0x2E,		// Safe actor class casting.

	// Intrinsic conversions.
	EX_MinConversion		= 0x30,		// Minimum conversion token.
	EX_RotationToVector		= 0x31,
	EX_ByteToInt			= 0x32,
	EX_ByteToBool			= 0x33,
	EX_ByteToFloat			= 0x34,
	EX_IntToByte			= 0x35,
	EX_IntToBool			= 0x36,
	EX_IntToFloat			= 0x37,
	EX_BoolToByte			= 0x38,
	EX_BoolToInt			= 0x39,
	EX_BoolToFloat			= 0x3A,
	EX_FloatToByte			= 0x3B,
	EX_FloatToInt			= 0x3C,
	EX_FloatToBool			= 0x3D,
	EX_ObjectToInt          = 0x3E,
	EX_ObjectToBool			= 0x3F,
	EX_NameToBool			= 0x40,
	EX_StringToByte			= 0x41,
	EX_StringToInt			= 0x42,
	EX_StringToBool			= 0x43,
	EX_StringToFloat		= 0x44,
	EX_StringToVector		= 0x45,
	EX_StringToRotation		= 0x46,
	EX_VectorToBool			= 0x47,
	EX_VectorToRotation		= 0x48,
	EX_RotationToBool		= 0x49,
	EX_ByteToString			= 0x4A,
	EX_IntToString			= 0x4B,
	EX_BoolToString			= 0x4C,
	EX_FloatToString		= 0x4D,
	EX_ObjectToString		= 0x4E,
	EX_NameToString			= 0x4F,
	EX_VectorToString		= 0x50,
	EX_RotationToString		= 0x51,
	EX_MaxConversion		= 0x53,		// Maximum conversion token.

	// Intrinsics.
	EX_ExtendedIntrinsic	= 0x60,
	EX_FirstIntrinsic		= 0x70,
	EX_FirstPhysics			= 0xF80,
	EX_Max					= 0x1000,
};

/*-----------------------------------------------------------------------------
	UScript.
-----------------------------------------------------------------------------*/

//
// A tokenized, interpretable script object referenced by a class.
// First element of the script's data is the root of the script's stack tree.
// Everything else in the script's data can be determined from the stack tree.
//
class UNENGINE_API UScript : public UBuffer, public FArchive
{
	DECLARE_DB_CLASS(UScript,UBuffer,BYTE,NAME_Script,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Friends.
	friend class FScriptCompiler;
	friend class UClass;

	// Variables.
	UClass *Class;

	// Constructor.
	UScript(UClass *InClass)
	:	Class(InClass) {}

	// UObject interface.
	void SerializeHeader(FArchive &Ar)
	{
		guard(UScript::SerializeHeader);
		UDatabase::SerializeHeader(Ar);
		Ar << Class;
		unguard;
	}
	void SerializeData(FArchive &Ar);

private:
	// FArchive interface, relevant only to script compiler.
	FArchive& Serialize(const void *V, int Length)
	{
		int iStart = Add(Length);
		memcpy(&Element(iStart),V,Length);
		return *this;
	}
	FArchive& Serialize(void *V, int Length)
	{
		int iStart = Add(Length);
		memcpy(&Element(iStart),V,Length);
		return *this;
	}
	FArchive& operator<<( class FName &N )
		{NAME_INDEX W=N.GetIndex(); return *this << W;}
	FArchive& operator<<( class UObject *&Res )
		{DWORD D = (DWORD)Res; return *this << D;}
	FArchive& operator<<( const char *S )
		{return Serialize(S,strlen(S)+1);}
	FArchive& operator<<( enum EExprToken E )
		{BYTE B=E; return *this<<B;}
	FArchive& operator<<( enum ECodeToken E )
		{BYTE B=E; return *this<<B;}
	FArchive& operator<<( enum EPropertyType E )
		{BYTE B=E; return *this<<B;}
};

/*-----------------------------------------------------------------------------
	Stack nodes.
-----------------------------------------------------------------------------*/

//
// A tree of all possible stack levels (class, functions, states) which are
// defined in the owning class.  Node 0 is always defined and it represents
// the root of that script's stack tree.  Except in the root script, stack nodes
// are always linked to stack nodes in other scripts' stack trees.
//
// Starting at the root of a script's stack tree, one can traverse the tree and
// find all functions and states that are accessible to actors of that class.  
// This will include functions and states defined in the script itself, as well 
// as functions and scripts defined in all parent scripts.
//
// As a script executes, execution can go back and forth between that script's code,
// and code defined in all of its parent classes.  However, only the primary script's
// stack tree is used as the sole index/reference to which overridden functions are
// where.
//
// ChildFunctions and ChildStates are indices to first item of each type (function, state)
// of each type.  This is used in traversing the linked list when a function is called,
// a state is set, or the script it decompiled/qureried.  0 indicates no child.
//
// iNext is an index to next peer scope link, or iNext==0 if either (1) this is the
// last item in the linked list, or (2) this is element 0 in the scope table,
// i.e. the global scope.
//
class UNENGINE_API FStackNode
{
public:
	// Links.
	FStackNodeLink  Parent;				// Link to parent class's version of this state.
	FStackNodeLink	ChildFunctions;		// Child functions at this scope.
	FStackNodeLink	ChildStates;		// Child states immediately below this scope.
	FStackNodeLink	Next;				// Next stack node at this scope.

	// General information.
	FName			Name;				// Name if function, NAME_None otherwise.
	WORD			iCode;				// Offset of start of code in script's code bin, or MAXWORD=no code.
	WORD			CodeLabelOffset;	// State: offset of label table in code or WORD_NONE; function: return value offset.
	WORD			Line;				// First-pass compile text line.
	WORD			Pos;				// First-pass compile text position.
	BYTE			NestType;			// Type of this scope block.
	BYTE			StackNodeFlags;		// EStackNodeFlags.
	BYTE			NumParms;			// Number of function, including return value.
	BYTE			OperPrecedence;		// Operator precedence.

	// Information specific to stack node types.
	union
	{
		// Information for states and classes.
		struct
		{
			QWORD	ProbeMask;			// Mask showing which probe messages are handled in this NEST_Class/NEST_State.
			QWORD	IgnoreMask;			// Probes to explicitly ignore.
		};

		// Information for functions.
		struct
		{
			WORD	LocalsSize;			// Size of locals used by the function.
			WORD	ParmsSize;			// Size of function parameters.
			WORD	iIntrinsic;			// Index of intrinsic function into C++'s intrinsic function list, 0=none.
			WORD	iFirstProperty;		// Index into class of first property.
			WORD	NumProperties;		// Total number of properties (parms + regular variables declared on this scope level).
		};
	};

	// Functions.
	friend FArchive& operator<<( FArchive& Ar, FStackNode &Node );
};

//
// Entry in a state's label table.
//
struct UNENGINE_API FLabelEntry
{
	// Variables.
	FName	Name;		// Name of the label.
	WORD	iCode;		// Code offset where the label begins.

	// Constructor.
	FLabelEntry( FName InName, WORD iInCode )
	:	Name	(InName)
	,	iCode	(iInCode)
	{}

	// Functions.
	friend FArchive& operator<< (FArchive& Ar, FLabelEntry &Label )
	{
		Ar << Label.Name;
		Ar << Label.iCode;
		return Ar;
	}
};

/*-----------------------------------------------------------------------------
	UStackTree.
-----------------------------------------------------------------------------*/

//
// A database of stack nodes.
//
class UNENGINE_API UStackTree : public UDatabase
{
	DECLARE_DB_CLASS(UStackTree,UDatabase,FStackNode,NAME_StackTree,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Variables.
	UClass *Class;

	// Constructors.
	UStackTree( UClass *InClass )
	:	UDatabase(0,0), Class(InClass) {}
	
	// UObject interface.
	void SerializeHeader( FArchive& Ar )
	{
		guard(UStackTree::SerializeHeader);
		UDatabase::SerializeHeader(Ar);
		Ar << Class;
		unguard;
	}
};

/*-----------------------------------------------------------------------------
	Intrinsic functions.
-----------------------------------------------------------------------------*/

//
// Intrinsic function table.
//
extern UNENGINE_API void (*GIntrinsics[])( FExecStack &Stack, UObject *Context, BYTE *&Result );
int UNENGINE_API RegisterIntrinsic(int iIntrinsic, void (*Func)( FExecStack &Stack, UObject *Context, BYTE *&Result ));
void UNENGINE_API execUndefined( FExecStack &Stack, UObject *Context, BYTE *&Result  );

//
// Registering an intrinsic function.
//
#define AUTOREGISTER_INTRINSIC(num,func) \
	static int func##Temp = RegisterIntrinsic(num,func);

/*-----------------------------------------------------------------------------
	Macros.
-----------------------------------------------------------------------------*/

//
// Macros for grabbing parameters for intrinsic functions.
//
#define P_GET_INT(var)              INT           var;   {INT *Ptr=&var;       (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_INT_OPT(var,def)      INT       var=def;   {INT *Ptr=&var;       (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_INT_REF(var)          INT   a##var=0,*var=&a##var;              {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&var );          }
#define P_GET_BOOL(var)             DWORD         var;   {DWORD *Ptr=&var;     (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_BOOL_OPT(var,def)     DWORD     var=def;   {DWORD *Ptr=&var;     (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_BOOL_REF(var)         DWORD a##var=0,*var=&a##var;              {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&var );          }
#define P_GET_FLOAT(var)            FLOAT         var;   {FLOAT *Ptr=&var;     (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_FLOAT_OPT(var,def)    FLOAT     var=def;   {FLOAT *Ptr=&var;     (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_FLOAT_REF(var)        FLOAT a##var=0.0,*var=&a##var;            {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&var );          }
#define P_GET_BYTE(var)             BYTE          var;   {BYTE *Ptr=&var;      (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_BYTE_OPT(var,def)     BYTE      var=def;   {BYTE *Ptr=&var;      (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_BYTE_REF(var)         BYTE  a##var=0,*var=&a##var;              {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&var );          }
#define P_GET_NAME(var)             FName         var;   {FName *Ptr=&var;     (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_NAME_OPT(var,def)     FName     var=def;   {FName *Ptr=&var;     (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_NAME_REF(var)         FName a##var=NAME_None,*var=&a##var;      {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&var );          }
#define P_GET_ACTOR(var)            AActor       *var;   {AActor **Ptr=&var;   (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_ACTOR_OPT(var,def)    AActor   *var=def;   {AActor **Ptr=&var;   (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_ACTOR_REF(var)        AActor *a##var=NULL,**var=&a##var;        {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&var );          }
#define P_GET_VECTOR(var)           FVector       var;   {FVector *Ptr=&var;   (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_VECTOR_OPT(var,def)   FVector   var=def;   {FVector *Ptr=&var;   (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_VECTOR_REF(var)       FVector a##var(0,0,0),*var=&a##var;       {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&var );          }
#define P_GET_ROTATION(var)         FRotation     var;   {FRotation *Ptr=&var; (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_ROTATION_OPT(var,def) FRotation var=def;   {FRotation *Ptr=&var; (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_ROTATION_REF(var)     FRotation a##var(0,0,0),*var=&a##var;     {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&var );          }
#define P_GET_OBJECT(cls,var)       cls          *var;   {cls**Ptr=&var;       (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_OBJECT_OPT(var,def)   UObject*var=def;     {UObject**Ptr=&var;   (*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Ptr ); var=*Ptr;}
#define P_GET_OBJECT_REF(var)       UObject*a##var=NULL,**var=&a##var;        {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&var );          }
#define P_GET_STRING(var)           CHAR var##T[MAX_STRING_CONST_SIZE], *var=var##T; {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object,*(BYTE**)&var);     }
#define P_GET_STRING_OPT(var,def)   CHAR var##T[MAX_STRING_CONST_SIZE]=def, *var=var##T; {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object,*(BYTE**)&var); }
#define P_GET_STRING_REF(var)       CHAR a##var[MAX_STRING_CONST_SIZE],*var=a##var; {(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&var );          }
#define P_GET_SKIP_OFFSET(var)      WORD          var;   {debugState(*Stack.Code==EX_Skip); Stack.Code++; var=*(WORD*)Stack.Code; Stack.Code+=2;        }
#define P_FINISH                                         {debugState(*Stack.Code==EX_EndFunctionParms); Stack.Code++;                                   }

/*-----------------------------------------------------------------------------
	Inlines.
-----------------------------------------------------------------------------*/

//
// FStackNodeLink serializer.
//
inline FArchive& operator<<( FArchive &Ar, FStackNodeLink &Link )
{
	guard(FStackNodeLink::<<);		
	Ar << Link.Class;
	if( Link.Class ) Ar << Link.iNode;
	return Ar;
	unguard;
}

//
// FStackNodeLink node getter.
//
inline FStackNode &FStackNodeLink::Node() const
{
	return Class->StackTree->Element(iNode);
}

//
// FExecStack constructors.
//
inline FExecStack::FExecStack( int )
:	Link	( 0 )
,	Script	(NULL)
,	Code	(NULL)
,	Object	(NULL)
,	Locals	(NULL)
{}
inline FExecStack::FExecStack( UObject *InObject )
:	Link	( 0 )
,	Script	(NULL)
,	Code	(NULL)
,	Object	(InObject)
,	Locals	(NULL)
{}
inline FExecStack::FExecStack( UObject *InObject, const FStackNodeLink &InLink, BYTE *InLocals )
:	Link	(InLink)
,	Script	(Link.Class->Script)
,	Code	(&Script->Element(Link.Node().iCode))
,	Object	(InObject)
,	Locals	(InLocals)
{}

//
// FExecStackMain constructors.
//
inline FExecStackMain::FExecStackMain( UObject *InObject )
:	FExecStack	( InObject )
,	ProbeMask	( ~(QWORD)0 )
{
	InitVfCache();
}

//
// FExecStackMain serializer.
//
inline FArchive& operator<<( FArchive& Ar, FExecStackMain &Exec )
{
	guard(FExecStackMain<<);

	// Serialize the stack node and exec script.
	Ar << Exec.Link << Exec.Script;

	// Serialize the code pointer.
	if( Exec.Script )
	{
		Ar.Preload(Exec.Script);
		WORD W      = Exec.Code==NULL ? MAXWORD : Exec.Code - &Exec.Script->Element(0); Ar << W;
		Exec.Code   = W==MAXWORD      ? NULL    : &Exec.Script->Element(W);
	}

	// Serialize custom info.
	Ar << Exec.ProbeMask;

	return Ar;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNSCRIPT
