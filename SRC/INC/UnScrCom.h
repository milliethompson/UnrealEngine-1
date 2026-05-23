/*=============================================================================
	UnScrCom.h: UnrealScript compiler.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNSCRCOM
#define _INC_UNSCRCOM

/*-----------------------------------------------------------------------------
	Constants & types.
-----------------------------------------------------------------------------*/

//
// Token types.
//
enum ETokenType
{
	TOKEN_None				= 0x00,		// No token.
	TOKEN_Identifier		= 0x01,		// Alphanumeric identifier.
	TOKEN_Symbol			= 0x02,		// Symbol.
	TOKEN_Const				= 0x03,		// A constant.
	TOKEN_Max				= 0x0D
};

//
// Types of statements to allow within a particular nesting block.
//
enum ENestAllowFlags
{
	ALLOW_StateCmd			= 0x0001,	// Allow commands that reside in states only.
	ALLOW_Cmd				= 0x0002,	// Allow commands that take 0 cycle time.
	ALLOW_Function			= 0x0004,	// Allow Event declarations at this level.
	ALLOW_State				= 0x0008,	// Allow State declarations at this level.
	ALLOW_ElseIf			= 0x0010,	// Allow ElseIf commands at this level.
	ALLOW_VarDecl			= 0x0040,	// Allow variable declarations at this level.
	ALLOW_Class				= 0x0080,	// Allow class definition heading.
	ALLOW_Case				= 0x0100,	// Allow 'case' statement.
	ALLOW_Default			= 0x0200,	// Allow 'default' case statement.
	ALLOW_Return			= 0x0400,	// Allow 'return' within a function.
	ALLOW_Break				= 0x0800,	// Allow 'break' from loop.
	ALLOW_Label				= 0x2000,	// Allow any label.
	ALLOW_Ignores			= 0x4000,	// Allow function masks like 'Ignores'.
};

/*-----------------------------------------------------------------------------
	FToken.
-----------------------------------------------------------------------------*/

//
// Information about a token that was just parsed.
//
class FToken : public FProperty
{
public:
	// Variables.
	ETokenType	TokenType;		// Type of token.
	FName		TokenName;		// Name of token.
	int			StartPos;		// Starting position in script where this token came from.
	int			StartLine;		// Starting line in script.
	char		Identifier[MAX_IDENTIFIER_SIZE]; // Always valid.
	union
	{
		// TOKEN_Const values.
		BYTE	Byte;								// If CPT_Byte.
		int		Int;								// If CPT_Int.
		BOOL	Bool;								// If CPT_Bool.
		FLOAT	Float;								// If CPT_Float.
		UObject *Object;							// If CPT_Object.
		char	NameBytes[sizeof(FName)];			// If CPT_Name.
		char	String[MAX_STRING_CONST_SIZE];		// If CPT_String.
		char	VectorBytes[sizeof(FVector)];		// If CPT_Vector.
		char	RotationBytes[sizeof(FRotation)];	// If CPT_Rotation.
	};

	// Constructors.
	FToken()
	{
		InitToken( PROPBIN_MAX, CPT_None );
	}
	FToken( const FProperty &InType )
	{
		InitToken( PROPBIN_MAX, CPT_None );
		*(FProperty*)this = InType;
	}

	// Functions.
	static void InitPropertyData( FToken *ConstValue, FProperty &Property, BYTE *FrameDataStart, int i );

	// Inlines.
	void InitToken( EPropertyBin InBin, EPropertyType InType )
	{
		FProperty::Init(InBin,InType);
		TokenType		= TOKEN_None;
		TokenName		= NAME_None;
		StartPos		= 0;
		StartLine		= 0;
	}
	int Matches( const char *Str )
	{
		return (TokenType==TOKEN_Identifier || TokenType==TOKEN_Symbol) && stricmp(Identifier,Str)==0;
	}
	int Matches( FName Name )
	{
		return TokenType==TOKEN_Identifier && TokenName==Name;
	}
	void AttemptToConvertConstant( const FProperty &NewType )
	{
		checkState(TokenType==TOKEN_Const);
		switch( NewType.Type )
		{
			case CPT_Int:		{INT        V(0);           if( GetConstInt     (V) ) SetConstInt     (V); break;}
			case CPT_Bool:		{BOOL       V(0);           if( GetConstBool    (V) ) SetConstBool    (V); break;}
			case CPT_Float:		{FLOAT      V(0.f);         if( GetConstFloat   (V) ) SetConstFloat   (V); break;}
			case CPT_Name:		{FName      V(NAME_None);   if( GetConstName    (V) ) SetConstName    (V); break;}
			case CPT_Vector:	{FVector    V(0.f,0.f,0.f); if( GetConstVector  (V) ) SetConstVector  (V); break;}
			case CPT_Rotation:	{FRotation  V(0,0,0);       if( GetConstRotation(V) ) SetConstRotation(V); break;}
			case CPT_String:
			{
				if( NewType.Type == CPT_String )
				{
					checkState(NewType.ElementSize>0);
					if( NewType.ElementSize < ElementSize )
						String[NewType.ElementSize-1]=0;
					ElementSize = NewType.ElementSize;
				}
				break;
			}
			case CPT_Byte:
			{
				BYTE V; 
				if( NewType.Enum==NULL && GetConstByte(V) )
					SetConstByte(NULL,V); 
				break;
			}
			case CPT_Object:
			{
				UObject *Ob; 
				if( GetConstObject( NewType.Class, Ob ) )
					SetConstObject( NewType.Class, Ob ); 
				break;
			}
		}
	}

	// Setters.
	void SetConstByte( UEnumDef *InEnum, BYTE InByte )
	{
		Init(PROPBIN_MAX, CPT_Byte);
		Enum			= InEnum;
		Byte			= InByte;
		TokenType		= TOKEN_Const;
	}
	void SetConstInt( INT InInt )
	{
		Init(PROPBIN_MAX, CPT_Int);
		Int				= InInt;
		TokenType		= TOKEN_Const;
	}
	void SetConstBool( BOOL InBool )
	{
		Init(PROPBIN_MAX, CPT_Bool);
		Bool 			= InBool;
		TokenType		= TOKEN_Const;
	}
	void SetConstFloat( FLOAT InFloat )
	{
		Init(PROPBIN_MAX, CPT_Float);
		Float			= InFloat;
		TokenType		= TOKEN_Const;
	}
	void SetConstObject( UClass *InClass, UObject *InObject )
	{
		Init(PROPBIN_MAX, CPT_Object);
		Class	 		= InClass;
		Object			= InObject;
		TokenType		= TOKEN_Const;
	}
	void SetConstName( FName InName )
	{
		Init(PROPBIN_MAX, CPT_Name);
		*(FName *)NameBytes = InName;
		TokenType		= TOKEN_Const;
	}
	void SetConstString( char *InString, int MaxLength = MAX_STRING_CONST_SIZE )
	{
		checkState(MaxLength>0);
		Init(PROPBIN_MAX, CPT_String);
		if( InString != String ) strncpy( String, InString, MaxLength );
		String[MaxLength-1]=0;
		ElementSize		= strlen(String)+1;
		TokenType		= TOKEN_Const;
	}
	void SetConstVector( FVector &InVector )
	{
		Init(PROPBIN_MAX, CPT_Vector);
		*(FVector *)VectorBytes = InVector;
		TokenType		= TOKEN_Const;
	}
	void SetConstRotation( FRotation &InRotation )
	{
		Init(PROPBIN_MAX, CPT_Rotation);
		*(FRotation *)RotationBytes	= InRotation;
		TokenType		= TOKEN_Const;
	}

	// Getters.
	int GetConstByte( BYTE &B )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Byte )
		{
			B = Byte;
			return 1;
		}
		else if( TokenType==TOKEN_Const && Type==CPT_Int && Int>=0 && Int<255 )
		{
			B = Int;
			return 1;
		}
		else if( TokenType==TOKEN_Const && Type==CPT_Float && Float>=0 && Float<255 && Float==(INT)Float)
		{
			B = Float;
			return 1;
		}
		else return 0;
	}
	int GetConstInt( INT &I )
	{
		if     ( TokenType==TOKEN_Const && Type==CPT_Int )
		{
			I = Int;
			return 1;
		}
		else if( TokenType==TOKEN_Const && Type==CPT_Byte )
		{
			I = Byte;
			return 1;
		}
		else if( TokenType==TOKEN_Const && Type==CPT_Float && Float==(INT)Float)
		{
			I=Float;
			return 1;
		}
		else return 0;
	}
	int GetConstBool( BOOL &B )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Bool )
		{
			B = Bool;
			return 1;
		}
		else return 0;
	}
	int GetConstFloat( FLOAT &R )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Float )
		{
			R = Float;
			return 1;
		}
		else if( TokenType==TOKEN_Const && Type==CPT_Int )
		{
			R = Int;
			return 1;
		}
		else if( TokenType==TOKEN_Const && Type==CPT_Byte )
		{
			R = Byte;
			return 1;
		}
		else return 0;
	}
	int GetConstObject( UClass *DesiredClass, UObject *&Ob )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Object && (DesiredClass==NULL || Class->IsChildOf(DesiredClass)) )
		{
			Ob = Object;
			return 1;
		}
		return 0;
	}
	int GetConstName( FName &n )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Name )
		{
			n = *(FName *)NameBytes;
			return 1;
		}
		return 0;
	}
	int GetConstString( char *&s )
	{
		if( TokenType==TOKEN_Const && Type==CPT_String )
		{
			s = String;
			return 1;
		}
		return 0;
	}
	int GetConstVector( FVector &v )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Vector )
		{
			v = *(FVector *)VectorBytes;
			return 1;
		}
		return 0;
	}
	int GetConstRotation( FRotation &r )
	{
		if( TokenType==TOKEN_Const && Type==CPT_Rotation )
		{
			r = *(FRotation *)RotationBytes;
			return 1;
		}
		return 0;
	}
};

/*-----------------------------------------------------------------------------
	Retry points.
-----------------------------------------------------------------------------*/

//
// A point in the script compilation state that can be set and returned to
// using InitRetry() and PerformRetry().  This is used in cases such as testing
// to see which overridden operator should be used, where code must be compiled
// and then "undone" if it was found not to match.
//
// Retries are not allowed to cross command boundaries (and thus nesting 
// boundaries).  Retries can occur across a single command or expressions and
// subexpressions within a command.
//
struct FRetryPoint
{
	int	InputPos;
	int	InputLine;
	int	CodeTop;
};

/*-----------------------------------------------------------------------------
	FNestInfo.
-----------------------------------------------------------------------------*/

//
// Types of code offset fixups we can perform.
//
enum EFixupType
{
	FIXUP_SwitchEnd		= 0, // Address past end of Switch construct.
	FIXUP_IfEnd			= 1, // Address past end of If construct.
	FIXUP_LoopStart		= 2, // Address of loop start.
	FIXUP_LoopEnd		= 3, // Address past end of Loop construct.
	FIXUP_ForStart		= 4, // Address of for start.
	FIXUP_ForEnd		= 5, // Address past end of For construct.
	FIXUP_Label			= 6, // Address of a label.
	FIXUP_IteratorEnd   = 7, // Address of end of iterator.
	FIXUP_MAX			= 8, // Maximum value.
};

//
// A fixup request.
//
struct FNestFixupRequest
{
	// Variables.
	EFixupType			Type;			// Type of fixup request.
	INT					iCode;			// Address in script code to apply the fixup.
	FName				Name;			// Label name, if FIXUP_Label.
	FNestFixupRequest	*Next;			// Next fixup request in nest info's linked list.

	// Constructor.
	FNestFixupRequest( EFixupType InType, INT iInCode, FName InName, FNestFixupRequest *InNext )
	:	Type	(InType)
	,	iCode	(iInCode)
	,	Name	(InName)
	,	Next	(InNext)
	{}
};

//
// Temporary compiler information about a label, stored at a nest level.
//
struct FLabelRecord : public FLabelEntry
{
	// Variables.
	FLabelRecord *Next; // Next label in the nest info's linked list of labels.

	// Constructor.
	FLabelRecord( FName InName, INT iInCode, FLabelRecord *InNext )
	:	FLabelEntry		( InName, iInCode )
	,	Next			( InNext )
	{}
};

//
// Information about a function we're compiling.
//
struct FFuncInfo
{
	// Variables;
	FToken		Function;		// Name of the function or operator.
	ENestType	NestType;		// NEST_Function or NEST_Operator.
	INT			NoDefaults;		// Whether default parms are allowed.
	INT			Precedence;		// Binary operator precedence.
	INT			StackNodeFlags;	// Stack node flags.
	INT			iIntrinsic;		// Index of intrinsic function.
	INT			ExpectParms;	// Number of parameters expected for operator.

	// Constructor.
	FFuncInfo()
	:	Function		()
	,	NestType		(NEST_None)
	,	NoDefaults		(0)
	,	Precedence		(0)
	,	StackNodeFlags	(0)
	,	iIntrinsic		(0)
	,	ExpectParms		(0)
	{}
};

//
// Information for a particular nesting level.
//
struct FNestInfo
{
	// Information for all nesting levels.
	FStackNodePtr   Link;               // Link to the stack node.
	ENestType		NestType;			// Statement that caused the nesting.
	INT				Allow;				// Types of statements to allow at this nesting level.

	// Information for cost nesting levels.
	INT				Fixups[FIXUP_MAX];	// Fixup addresses for PopNest to use.
	FNestFixupRequest *FixupList;		// Pending fixup requests.
	FLabelRecord	*LabelList;			// Linked list of labels.
	INT				iCodeChain;			// Code index for next command, i.e. in strings of if/elseif/elseif...

	// Command specific info.
	FToken			SwitchType;			// Type of Switch statement.
	FRetryPoint		ForRetry;			// Retry point (used in For command).

	// Return the stack node.
	FStackNode *Node()
	{
		return Link.iNode==INDEX_NONE ? NULL : &Link.Class->StackTree->Element(Link.iNode);
	}

	// Set a fixup address.
	void SetFixup( EFixupType Type, INT iCode )
	{
		checkState(Fixups[Type]==MAXWORD);
		Fixups[Type] = iCode;
	}
};

/*-----------------------------------------------------------------------------
	FScriptCompiler.
-----------------------------------------------------------------------------*/

//
// Script compiler class.
//
class FScriptCompiler
{
public:
	// Objects.
	UClass::Ptr		Class;					// Actor class info while compiling is happening.
	UTextBuffer		*ErrorText;				// Error text buffer.
	UScript			*Script;				// Script we're generating.
	FMemStack		*Mem;					// Pool for temporary allocations.

	// Variables.
	char			*Input;					// Input text.
	int				InputSize;				// Total length of input buffer.
	int				InputPos;				// Current position in text.
	int				InputLine;				// Current line in text.
	int				PrevPos;				// Position previous to last GetChar() call.
	int				PrevLine;				// Line previous to last GetChar() call.
	int				StatementsCompiled;		// Number of statements compiled.
	int				LinesCompiled;			// Total number of lines compiled.
	int				GotAffector;			// Got an expression that has a side effect?
	int				GotIterator;			// Got an iterator.
	int				AllowIterator;			// Allow iterators.
	int				Booting;				// Bootstrap compiling classes.
	int				Pass;					// Compilation pass.

	// Nesting.
	int				NestLevel;				// Current nest level, starts at 0.
	FNestInfo		*TopNest;				// Top nesting level.
	FStackNode		*TopNode;				// Top stack node.
	FNestInfo		Nest[MAX_NEST_LEVELS];	// Information about all nesting levels.

	// Subsystem functions.
	void			InitMake();
	void			ExitMake(int Success);

	// Precomputation.
	void			PrecomputeProbeMasks(FStackNodePtr Link);

	// High-level compiling functions.
	int				CompileScript(UClass *Class,FMemStack *Mem,BOOL ObjectPropertiesAreValid,BOOL Booting,INT Pass);
	int				CompileDeclaration(FToken &Token,BOOL &NeedSemicolon);
	void			CompileCommand(FToken &Token,BOOL &NeedSemicolon);
	int				CompileStatement();
	int				CompileExpr(const FProperty RequiredType, const char *ErrorTag=NULL, FToken *ResultType=NULL, INT MaxPrecedence=MAXINT, FProperty *HintType=NULL);
	int				CompileFunctionExpr(FToken Token, UClass::Ptr ClassContext, FToken &ResultType);
	int				CompileVariableExpr(FToken Token, UClass::Ptr ClassContext, FToken &ResultType);
	int				CompileDynamicCast(FToken Token, FToken &ResultType);
	void			CompileAssignment(const char *Tag);
	void			CompileAffector();
	void			CompileDirective();
	void			CompileSecondPass(INDEX iStackNode);
	UEnumDef*		CompileEnum();

	// High-level parsing functions.
	int				GetToken(FToken &Token, const FProperty *Hint=NULL, INT NoConsts=0 );
	int				GetRawToken(FToken &Token);
	void			UngetToken(FToken &Token);
	int				GetIdentifier(FToken &Token, INT NoConsts=0);
	int				GetSymbol(FToken &Token);
	void			CheckAllow(const char *Thing,int AllowFlags);
	int				FindVariable(FName Name, UClass::Ptr ClassContext=(UClass*)NULL, FProperty *FoundType=NULL);
	int				ConversionCost(const FProperty &Dest, const FProperty &Source);
	void			SkipStatements(int SubCount, const char *ErrorTag);

	// Properties.
	int				GetVarType(FProperty &VarProperty,int NoOverrides,EPropertyBin Bin,const char *Thing);
	FProperty&      GetVarNameAndDim(FProperty &VarProperty,int NoArrays,int NoDefault,int IsFunction,const char *HardcodedName, const char *Thing);
	int				SetPropertyFlags(FProperty &Property, FToken &Token);
	int				SetPropertyType(FProperty &Property, FToken &Token, int ArraySize, int Extra );

	// Low-level parsing functions.
	const char		*NestTypeName(ENestType NestType);
	char			GetChar(BOOL Literal=0);
	char			PeekChar();
	char			GetLeadingChar();
	void			UngetChar();
	int				IsEOL(char c);
	void VARARGS	AddResultText(char *Fmt,...);
	int				GetConstInt(int &Result, const char* Tag=NULL);
	int				GetConstFloat(FLOAT &Result, const char* Tag=NULL);

	// Nest management functions.
	void			PushNest(ENestType NestType,const char *Name,INT iNode=INDEX_NONE);
	void			PopNest(ENestType NestType,const char *Descr);
	int				FindNest(ENestType NestType);

	// Matching predefined text.
	int				MatchIdentifier(const char *Match);
	int				MatchSymbol(const char *Match);
	int				PeekSymbol(const char *Match);

	// Requiring predefined text.
	void			RequireIdentifier(const char *Match, const char *Tag);
	void			RequireSymbol(const char *Match, const char *Tag);
	void			RequireSizeOfParm(FToken &TypeToken,const char *Tag);

	// Retry functions.
	void			InitRetry(FRetryPoint &Retry);
	void			PerformRetry(FRetryPoint &Retry, BOOL Binary=1, BOOL Text=1);
	void			CodeSwitcheroo(FRetryPoint &LowRetry, FRetryPoint &HighRetry);

	// Emitters.
	void			EmitConstant(FToken &ConstToken);
	void			EmitStackNodeLinkFunction(FStackNodePtr Link, BOOL ForceFinal);
	void			EmitAddressToFixupLater(FNestInfo *Nest, EFixupType Type, FName Name);
	void			EmitAddressToChainLater(FNestInfo *Nest);
	void			EmitChainUpdate(FNestInfo *Nest);
	void			EmitSimpleVariable(const FProperty &Type, WORD wOffset, BOOL DefaultSpecifier);
	void			EmitSize(int Size,const char*Tag);
	void			EmitLet(FProperty &Type,const char *Tag);

	// Decompiler
	void			FindOriginalProperty(UClass::Ptr ContextClass, EPropertyBin Bin, WORD Base, DWORD BitMask, FProperty *&ResultProperty, WORD &ResultOffset, INT iFirstProperty, INT NumProperties);
	void			DecompileFunctionCallParms(UClass::Ptr ScriptClass, INT iScriptNode, UClass::Ptr ContextClass, INT iContextNode, FOutputDevice &Out, BYTE *&Code);
	FProperty*      DecompileExpr(UClass::Ptr ScriptClass, INT iScriptNode, UClass::Ptr ContextClass, INT iContextNode, FOutputDevice &Out, BYTE *&Code, INT Length);
	void			DecompileStackNode(UClass::Ptr Class, INT iNode, FOutputDevice &Out, int Indent, int ParentLinks);
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNSCRCOM
