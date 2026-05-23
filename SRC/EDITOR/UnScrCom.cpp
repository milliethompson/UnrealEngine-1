/*=============================================================================
	UnScrCom.cpp: UnrealScript compiler.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	The long-awaited UnrealScript compiler.

Revision history:
	* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnScrCom.h"

/*-----------------------------------------------------------------------------
	Constants & declarations.
-----------------------------------------------------------------------------*/

enum {MAX_VARIABLE_DATA_SIZE=4096};
enum {MAX_ARRAY_SIZE=255};

/*-----------------------------------------------------------------------------
	Utility functions.
-----------------------------------------------------------------------------*/

//
// Table of conversion expression tokens between all built-in types.
// Converting a type to itself has no conversion function.
//
// Notes:
// * EX_Max indicates that a conversion isn't present.
// * Conversions to type CPT_String must not be automatic.
// * Access with GConversions[DestType][SourceType]
//
#define AUTOCONVERT 0x100 /* Compiler performs the conversion automatically */
#define TRUNCATE    0x200 /* Conversion requires truncation */
#define CONVERT_MASK ~(AUTOCONVERT | TRUNCATE)
#define AC  AUTOCONVERT
#define TAC TRUNCATE|AUTOCONVERT
static DWORD GConversions[CPT_MAX][CPT_MAX] =
{
			/*   None                  Byte                  Int                   Bool                  Float                 Object                Name                  String                Vector                Rotation
			/*   --------------------  --------------------  --------------------  --------------------  --------------------  --------------------  --------------------  --------------------  --------------------  -------------------- */
/* None     */ { EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max               },
/* Byte     */ { EX_Max,               EX_Max,               EX_IntToByte|TAC,     EX_BoolToByte,	     EX_FloatToByte|TAC,   EX_Max,               EX_Max,               EX_StringToByte,      EX_Max,               EX_Max               },
/* Int      */ { EX_Max,               EX_ByteToInt|AC,      EX_Max,               EX_BoolToInt,         EX_FloatToInt|TAC,    EX_ObjectToInt,       EX_Max,               EX_StringToInt,       EX_Max,               EX_Max               },
/* Bool     */ { EX_Max,               EX_ByteToBool,        EX_IntToBool,         EX_Max,               EX_FloatToBool,       EX_ObjectToBool,      EX_NameToBool,        EX_StringToBool,      EX_VectorToBool,      EX_RotationToBool    },
/* Float    */ { EX_Max,               EX_ByteToFloat|AC,    EX_IntToFloat|AC,     EX_BoolToFloat,       EX_Max,               EX_Max,               EX_Max,               EX_StringToFloat,     EX_Max,               EX_Max               },
/* Object   */ { EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max               },
/* Name     */ { EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max               },
/* String   */ { EX_Max,               EX_ByteToString,      EX_IntToString,       EX_BoolToString,      EX_FloatToString,     EX_ObjectToString,    EX_NameToString,      EX_Max,               EX_VectorToString,    EX_RotationToString  },
/* Vector   */ { EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_StringToVector,    EX_Max,               EX_RotationToVector  },
/* Rotation */ { EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_Max,               EX_StringToRotation,  EX_VectorToRotation,  EX_Max               },
};
#undef AC
#undef TAC

/*-----------------------------------------------------------------------------
	Single-character processing.
-----------------------------------------------------------------------------*/

//
// Get a single character from the input stream and return it, or 0=end.
//
char inline FScriptCompiler::GetChar( BOOL Literal )
{
	guardSlow(FScriptCompiler::GetChar);
	int CommentCount=0;

	PrevPos  = InputPos;
	PrevLine = InputLine;
	if( InputPos < InputSize )
	{
		Loop:
		char c = Input[InputPos++];
		if( c==0x0a )
		{
			InputLine++;
		}
		else if( !Literal && c=='/' && Input[InputPos]=='*' )
		{
			CommentCount++;
			InputPos++;
			goto Loop;
		}
		else if( !Literal && c=='*' && Input[InputPos]=='/' )
		{
			if( --CommentCount < 0 )
				throwf( "Unexpected '*/' outside of comment" );
			InputPos++;
			goto Loop;
		}
		if( CommentCount > 0 )
		{
			if( c==0 )
				throwf( "End of script encountered inside comment" );
			goto Loop;
		}
		return c;
	}
	else
	{
		// So that UngetChar() works properly.
		InputPos++; 
		return 0;
	}
	unguardSlow;
}

//
// Unget the previous character retrieved with GetChar().
//
void inline FScriptCompiler::UngetChar()
{
	guardSlow(FScriptCompiler::UngetChar);

	InputPos  = PrevPos;
	InputLine = PrevLine;

	unguardSlow;
}

//
// Look at a single character from the input stream and return it, or 0=end.
// Has no effect on the input stream.
//
char inline FScriptCompiler::PeekChar()
{
	guardSlow(FScriptCompiler::PeekChar);
	return (InputPos<InputSize) ? Input[InputPos] : 0;
	unguardSlow;
}

//
// Skip past all spaces and tabs in the input stream.
//
char inline FScriptCompiler::GetLeadingChar()
{
	guardSlow(FScriptCompiler::GetLeadingChar);

	// Skip blanks.
	char c;
	Skip1: do c=GetChar(); while( c==0x20 || c==0x09 || c==0x0d || c==0x0a );
	if( c=='/' && Input[InputPos]=='/' )
	{
		// Comment, so skip to start of next line.
		do c=GetChar(1); while( c!=0x0d && c!=0x0a && c!=0x00 );
		goto Skip1;
	}
	return c;
	unguardSlow;
}

//
// Return 1 if input as a valid end-of-line character, or 0 if not.
// EOL characters are: Comment, CR, linefeed, 0 (end-of-file mark)
//
int inline FScriptCompiler::IsEOL( char c )
{
	guardSlow(FScriptCompiler::IsEOL);
	return c==0x0d || c==0x0a || c==0;
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	Code emitting.
-----------------------------------------------------------------------------*/

//
// Emit a constant expression into an expression residing in code.
//
void FScriptCompiler::EmitConstant( FToken &Token )
{
	guard(FScriptCompiler::EmitConstant);
	checkState(Token.TokenType==TOKEN_Const);

	switch( Token.Type )
	{
		case CPT_Int:
		{
			if( Token.Int == 0 )
			{
				*Script << EX_IntZero;
			}
			else if( Token.Int == 1 )
			{
				*Script << EX_IntOne;
			}
			else if( Token.Int>=0 && Token.Int<=255 )
			{
				BYTE B = Token.Int;
				*Script << EX_IntConstByte;
				*Script << B;
			}
			else
			{
				*Script << EX_IntConst;
				*Script << Token.Int;
			}
			break;
		}
		case CPT_Byte:
		{
			*Script << EX_ByteConst;
			*Script << Token.Byte;
			break;
		}
		case CPT_Bool:
		{
			if( Token.Bool ) *Script << EX_True;
			else *Script << EX_False;
			break;
		}
		case CPT_Float:
		{
			*Script << EX_FloatConst;
			*Script << Token.Float;
			break;
		}
		case CPT_String:
		{
			*Script << EX_StringConst;
			Script->String(Token.String,MAX_STRING_CONST_SIZE);
			break;
		}
		case CPT_Object:
		{
			if( Token.Object==NULL )
			{
				*Script << EX_NoObject;
			}
			else
			{
				if( Token.Class->IsChildOf("Actor") )
					throwf( "Illegal actor constant" );
				*Script << EX_ObjectConst;
				*Script << Token.Object;
			}
			break;
		}
		case CPT_Name:
		{
			FName N;
			Token.GetConstName(N);
			*Script << EX_NameConst;
			*Script << N;
			break;
		}
		case CPT_Rotation:
		{
			FRotation R;
			Token.GetConstRotation(R);
			*Script << EX_RotationConst;
			*Script << R;
			break;
		}
		case CPT_Vector:
		{
			FVector V;
			Token.GetConstVector(V);
			*Script << EX_VectorConst;
			*Script << V;
			break;
		}
		default:
		{
			throwf( "Internal EmitConstant token type error" );
		}
	}
	unguard;
}

//
// Emit the function corresponding to a stack node link.
//
void FScriptCompiler::EmitStackNodeLinkFunction( FStackNodePtr Link, BOOL ForceFinal )
{
	guard(FScriptCompiler::EmitStackNodeFunction);
	FStackNode &Node = *Link;
	BOOL IsIntrinsic = (Node.StackNodeFlags & SNODE_IntrinsicFunc);
	BOOL IsFinal     = (Node.StackNodeFlags & SNODE_FinalFunc) || ForceFinal;

	// Emit it according to function type.
	if( IsIntrinsic && IsFinal )
	{
		// Intrinsic, final function.
		if( Node.iIntrinsic <= 255 )
		{
			// One-byte call.
			checkState( Node.iIntrinsic >= EX_FirstIntrinsic );
			BYTE B = Node.iIntrinsic;
			*Script << B;
		}
		else
		{
			// Two-byte call.
			BYTE B = EX_ExtendedIntrinsic + (Node.iIntrinsic/256);
			checkState( B < EX_FirstIntrinsic );
			BYTE C = (Node.iIntrinsic) % 256;
			*Script << B;
			*Script << C;
		}
	}
	else if( IsFinal )
	{
		// Prebound, non-overridable function.
		checkState(Link.Class->StackTree!=NULL);
		*Script << EX_FinalFunction;
		*Script << Link;
	}
	else
	{
		// Virtual function.
		*Script << EX_VirtualFunction;
		*Script << Node.Name;
	}
	unguard;
}

//
// Emit a code offset which the script compiler will fix up in the
// proper PopNest call.
//
void FScriptCompiler::EmitAddressToFixupLater( FNestInfo *Nest, EFixupType Type, FName Name )
{
	guard(FScriptCompiler::EmitAddressToFixupLater);

	// Add current code address to the nest level's fixup list.
	Nest->FixupList = new(GMem)FNestFixupRequest( Type, Script->Num, Name, Nest->FixupList );

	// Emit a dummy code offset as a placeholder.
	WORD Temp=0;
	*Script << Temp;

	unguard;
}

//
// Emit a code offset which should be chained to later.
//
void FScriptCompiler::EmitAddressToChainLater( FNestInfo *Nest )
{
	guard(FScriptCompiler::EmitAddressToChainLater);

	// Note chain address in nest info.
	Nest->iCodeChain = Script->Num;

	// Emit a dummy code offset as a placeholder.
	WORD Temp=0;
	*Script << Temp;

	unguard;
}

//
// Update and reset the nest info's chain address.
//
void FScriptCompiler::EmitChainUpdate( FNestInfo *Nest )
{
	guard(FScriptCompiler::EmitChainUpdate);

	// If there's a chain address, plug in the current script offset.
	if( Nest->iCodeChain != INDEX_NONE )
	{
		*(WORD*)&Script->Element( Nest->iCodeChain ) = Script->Num;
		Nest->iCodeChain = INDEX_NONE;
	}

	unguard;
}

//
// Emit a variable size, making sure it's within reason.
//
void FScriptCompiler::EmitSize( int Size, const char *Tag )
{
	guard(FScriptCompiler::EmitSize);
	BYTE B = Size;
	if( B != Size )
		throwf("%s: Variable is too large (%i bytes, 255 max)",Tag,Size);
	*Script << B;
	unguard;
}

//
// Emit an assignment.
//
void FScriptCompiler::EmitLet( FProperty &Type, const char *Tag )
{
	guard(FScriptCompiler::EmitLet);

	// Validate the required type.
	if( Type.Flags & CPF_Const )
		throwf( "Can't assign Const variables" );

	// Emit let token.
	if( Type.Type == CPT_Bool )
	{
		// Boolean assignment.
		*Script << EX_LetBool;
	}
	else if( Type.Type == CPT_String )
	{
		// String assignment.
		*Script << EX_LetString;
	}
	else if( Type.Size() == 1 )
	{
		// 1-byte assignment.
		*Script << EX_Let1;
	}
	else if( Type.Size() == 4 )
	{
		// 4-byte assignment.
		*Script << EX_Let4;
	}
	else
	{
		// Normal assignment.
		*Script << EX_Let;
		checkState(Type.Size()>0);
		EmitSize(Type.Size(),Tag);
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	FToken.
-----------------------------------------------------------------------------*/

//
// Initialize a class property value.  FrameDataStart is a pointer to the beginning
// of the property frame where this property resides.
//
void FToken::InitPropertyData
(
	FToken		*ConstValue,
	FProperty	&Property,
	BYTE		*Data,
	int			i 
)
{
	guard(FToken::InitPropertyData);
	checkInput(!ConstValue ||ConstValue->TokenType==TOKEN_Const);

	// Init each element of array (or the single element, if not an array).
	switch( Property.Type )
	{
		case CPT_Byte:
		{
			BYTE b=0;
			if( ConstValue && !ConstValue->GetConstByte(b) )
				throwf("Bad Byte initializer");
			((BYTE *)Data)[i] = b;
			break;
		}
		case CPT_Int:
		{
			int v=0;
			if( ConstValue && !ConstValue->GetConstInt(v) )
				throwf("Bad Int initializer");
			((INT *)Data)[i] = v;
			break;
		}
		case CPT_Bool:	
		{
			BOOL v=0;
			if( ConstValue && !ConstValue->GetConstBool(v) )
				throwf("Bad Bool initializer");

			if( v )	((DWORD *)Data)[i] |=  Property.BitMask;
			else	((DWORD *)Data)[i] &= ~Property.BitMask;
			break;
		}
		case CPT_Float:
		{
			FLOAT v=0.0;
			if( ConstValue && !ConstValue->GetConstFloat(v) )
				throwf("Bad Float initializer");
			((FLOAT *)Data)[i] = v;
			break;
		}
		case CPT_Object:
		{
			UObject *Ob = NULL;
			if( ConstValue && !ConstValue->GetConstObject( Property.Class, Ob ) ) 
				throwf( "Bad %s initializer", Property.Class->GetName() );
			if( Property.Class->IsChildOf("Actor") && Ob!=NULL )
				throwf( "Can only initialize Actors to None" );
			((UObject**)Data)[i] = Ob;
			break;
		}
		case CPT_Name:
		{
			FName Name=NAME_None;
			if( ConstValue && !ConstValue->GetConstName(Name))
				throwf("Bad Name initializer");
			((FName *)Data)[i] = Name;
			break;
		}
		case CPT_String:
		{
			memset( Data + i * Property.ElementSize, 0, Property.ElementSize );
			if( ConstValue )
			{
				if( ConstValue->Type != CPT_String )
					throwf("Bad String initializer");
				mystrncpy( (char *)Data, ConstValue->String, Property.ElementSize );
				Data += Property.ElementSize;
			}
			break;
		}
		case CPT_Vector:
		{
			FVector V = FVector(0,0,0);
			if( ConstValue && !ConstValue->GetConstVector(V) )
				throwf("Bad Vector initializer");
			((FVector *)Data)[i] = V;
			break;
		}
		case CPT_Rotation:
		{
			FRotation R = FRotation(0,0,0);
			if( ConstValue && !ConstValue->GetConstRotation(R) )
				throwf("Bad Rotation initializer");
			((FRotation *)Data)[i] = R;
			break;
		}
		case CPT_EnumDef:
		{
			break;
		}
		default:
		{
			appErrorf("Unknown property type %i",Property.Type);
			break;
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Tokenizing.
-----------------------------------------------------------------------------*/

//
// Get the next token from the input stream, set *Token to it.
// Returns 1 if processed a token, 0 if end of line.
//
// If you initialize the token's Type info prior to calling this
// function, we perform special logic that tries to evaluate Token in the 
// context of that type. This is how we distinguish enum tags.
//
int FScriptCompiler::GetToken( FToken &Token, const FProperty *Hint, INT NoConsts )
{
	guard(FScriptCompiler::GetToken);
	Token.TokenName	= NAME_None;

	char c=GetLeadingChar();
	char p=PeekChar();
	if( c == 0 )
	{
		UngetChar();
		return 0;
	}

	Token.StartPos		= PrevPos;
	Token.StartLine		= PrevLine;

	if( (c>='A' && c<='Z') || (c>='a' && c<='z') )
	{
		// Alphanumeric token.
		int Length=0;
		do	{
			Token.Identifier[Length++] = c;
			if( Length > MAX_IDENTIFIER_SIZE )
				throwf( "Identifer length exceeds maximum of %i", MAX_IDENTIFIER_SIZE );
			c = GetChar();
		} while( ((c>='A')&&(c<='Z')) || ((c>='a')&&(c<='z')) || ((c>='0')&&(c<='9')) || (c=='_') );
		UngetChar();
		Token.Identifier[Length]=0;

		// Assume this is an identifier unless we find otherwise.
		Token.TokenType = TOKEN_Identifier;

		// Lookup the token's global name.
		Token.TokenName = FName( Token.Identifier, FNAME_Find );

		// See if the idenfitifier is part of a vector, rotation, or object constant.
		if( Token.TokenName==NAME_Vect && !NoConsts && MatchSymbol("(") )
		{
			// This is a vector constant.
			FVector V;
			if(!GetConstFloat(V.X))  throwf( "Missing X component of vector" );
			if(!MatchSymbol(","))    throwf( "Missing ',' in vector"         );
			if(!GetConstFloat(V.Y))  throwf( "Missing Y component of vector" );
			if(!MatchSymbol(","))    throwf( "Missing ',' in vector"         );
			if(!GetConstFloat(V.Z))  throwf( "Missing Z component of vector" );
			if(!MatchSymbol(")"))    throwf( "Missing ')' in vector"         );

			Token.SetConstVector(V);
			return 1;
		}
		if( Token.TokenName==NAME_Rot && !NoConsts && MatchSymbol("(") )
		{
			// This is a rotation constant.
			FRotation R;
			if(!GetConstInt(R.Pitch))             throwf( "Missing Pitch component of rotation" );
			if(!MatchSymbol(","))                 throwf( "Missing ',' in rotation"             );
			if(!GetConstInt(R.Yaw))               throwf( "Missing Yaw component of rotation"   );
			if(!MatchSymbol(","))                 throwf( "Missing ',' in vector"               );
			if(!GetConstInt(R.Roll))              throwf( "Missing Roll component of rotation"  );
			if(!MatchSymbol(")"))                 throwf( "Missing ')' in vector"               );

			Token.SetConstRotation(R);
			return 1;
		}
		if( Token.TokenName==NAME_MaxInt && !NoConsts )
		{
			Token.SetConstInt(MAXINT);
			return 1;
		}
		if( Token.TokenName==NAME_Pi && !NoConsts )
		{
			Token.SetConstFloat(PI);
			return 1;
		}
		if( Token.TokenName==NAME_True && !NoConsts )
		{
			Token.SetConstBool(1);
			return 1;
		}
		if( Token.TokenName==NAME_False && !NoConsts )
		{
			Token.SetConstBool(0);
			return 1;
		}
		if( Token.TokenName==NAME_SizeOf && !NoConsts )
		{
			FToken TypeToken;
			RequireSizeOfParm(TypeToken,"'SizeOf'");
			Token.SetConstInt(TypeToken.Size());
			return 1;
		}
		if( Token.TokenName==NAME_ArrayCount && !NoConsts )
		{
			FToken TypeToken;
			RequireSizeOfParm(TypeToken,"'SizeOf'");
			if( TypeToken.ArrayDim==1 )
				throwf( "ArrayCount argument is not an array" );
			Token.SetConstInt(TypeToken.ArrayDim);
			return 1;
		}
		if( Token.TokenName==NAME_EnumCount && !NoConsts )
		{
			FToken TypeToken;
			RequireSizeOfParm(TypeToken,"'EnumCount'");
			if( TypeToken.Type != CPT_EnumDef )
				throwf( "EnumCount argument is not an enumeration name" );
			checkState(TypeToken.Enum!=NULL);
			Token.SetConstInt(TypeToken.Enum->Num);
			return 1;
		}
		if( Token.Matches("None") && !NoConsts )
		{
			Token.SetConstObject( NULL, NULL );
			return 1;
		}

		// See if this is an enum, which we can only evaluate with knowledge
		// about the specified type.
		if( Hint && Hint->Type==CPT_Byte && Hint->Enum && Token.TokenName!=NAME_None && !NoConsts )
		{
			// Find index into the enumeration.
			INDEX EnumIndex=INDEX_NONE;
			if( Hint->Enum->FindItem( Token.TokenName, EnumIndex ) )
			{
				Token.SetConstByte(Hint->Enum,EnumIndex);
				return 1;
			}
		}

		// See if this is a general object constant.
		if( !NoConsts && PeekSymbol("'") )
		{
			UClass *Type = new( Token.Identifier, FIND_Optional )UClass;
			if
			(	(Type)
			&&	(Type->ClassFlags & CLASS_ScriptWritable)
			&&	!Type->IsChildOf("Actor")
			&&	!NoConsts
			&&	MatchSymbol("'") )
			{
				// This is an object constant.
				FToken NameToken;
				if( !GetIdentifier(NameToken,1) )
					throwf( "Missing %s name", Type->GetName() );
				UObject *Ob = GObj.FindObject( NameToken.Identifier, Type, FIND_Optional );
				if( Ob == NULL )
					throwf( "Can't find %s '%s'", Type->GetName(), NameToken.Identifier );
				if( !MatchSymbol("'") )
					throwf( "Missing single quote after %s name", Type->GetName() );

				Token.SetConstObject( Type, Ob );
				return 1;
			}
		}
		return 1;
	}
	else if( (c>='0' && c<='9') || ((c=='+' || c=='-') && (p>='0' && p<='9')) && !NoConsts )
	{
		// Integer or floating point constant.
		int  IsFloat = 0;
		int  Length  = 0;
		int  IsHex   = 0;
		do 
		{
			if( c=='.' ) IsFloat = 1;
			if( c=='X' ) IsHex   = 1;

			Token.Identifier[Length++] = c;
			if( Length >= MAX_IDENTIFIER_SIZE )
				throwf( "Number length exceeds maximum of %i ", MAX_IDENTIFIER_SIZE );
			c = toupper(GetChar());
		} while( (c>='0' && c<='9') || c=='.' || c=='X' || (c>='A' && c<='F') );

		Token.Identifier[Length]=0;
		UngetChar();

		if( IsFloat )
		{
			Token.SetConstFloat( atof(Token.Identifier) );
		}
		else if( IsHex )
		{
			char *End = Token.Identifier+strlen(Token.Identifier);
			Token.SetConstInt( strtoul(Token.Identifier,&End,0) );
		}
		else
		{
			Token.SetConstInt( atoi(Token.Identifier) );
		}
		return 1;
	}
	else if( c=='\'' && !NoConsts)
	{
		// Name constant.
		int Length=0;
		c = GetChar();
		while( (c>='A' && c<='Z') || (c>='a' && c<='z') || (c>='0' && c<='9') || (c=='_') )
		{
			Token.Identifier[Length++] = c;
			if( Length > NAME_SIZE )
				throwf( "Name length exceeds maximum of %i", NAME_SIZE );
			c = GetChar();
		}
		if( c != '\'' )
			throwf( "Illegal character in name" );
		Token.Identifier[Length]=0;

		// Make constant name.
		Token.SetConstName( FName(Token.Identifier,FNAME_Add) );
		return 1;
	}
	else if( c=='"' )
	{
		// String constant.
		char Temp[MAX_STRING_CONST_SIZE];
		int Length=0;
		c = GetChar(1);
		while( (c!='"') && !IsEOL(c) )
		{
			if(c=='\\')
			{
				c = GetChar(1);
				if( IsEOL(c) )
					break;
			}
			Temp[Length++] = c;
			if( Length >= MAX_STRING_CONST_SIZE )
				throwf( "String constant exceeds maximum of %i characters", MAX_IDENTIFIER_SIZE );
			c = GetChar(1);
		}
		if( c!='"' ) 
			throwf( "Unterminated string constant" );

		Temp[Length]=0;

		Token.SetConstString(Temp);
		return 1;
	}
	else
	{
		// Symbol.
		int Length=0;
		Token.Identifier[Length++] = c;

		// Handle special 2-character symbols.
		#define PAIR(cc,dd) ((c==cc)&&(d==dd)) /* Comparison macro for convenience */
		char d=GetChar();
		if
		(	PAIR('<','<')
		||	PAIR('>','>')
		||	PAIR('!','=')
		||	PAIR('<','=')
		||	PAIR('>','=')
		||	PAIR('+','+')
		||	PAIR('-','-')
		||	PAIR('+','=')
		||	PAIR('-','=')
		||	PAIR('*','=')
		||	PAIR('/','=')
		||	PAIR('&','&')
		||	PAIR('|','|')
		||	PAIR('^','^')
		||	PAIR('=','=')
		||	PAIR('*','*')
		||	PAIR('~','=')
		)
		{
			Token.Identifier[Length++] = d;
		}
		else UngetChar();
		#undef PAIR

		Token.Identifier[Length]=0;
		Token.TokenType = TOKEN_Symbol;

		// Lookup the token's global name.
		Token.TokenName = FName( Token.Identifier, FNAME_Find );

		return 1;
	}
	return 0;
	unguard;
}

//
// Get a raw token until we reach end of line.
//
int FScriptCompiler::GetRawToken( FToken &Token )
{
	guard(FScriptCompiler::GetRawToken);

	// Get token after whitespace.
	char Temp[MAX_STRING_CONST_SIZE];
	int  Length=0;
	char c=GetLeadingChar();
	while( !IsEOL(c) )
	{
		if( (c=='/' && PeekChar()=='/') || (c=='/' && PeekChar()=='*') )
			break;
		Temp[Length++] = c;
		if( Length >= MAX_STRING_CONST_SIZE )
			throwf( "Identifier exceeds maximum of %i characters", MAX_IDENTIFIER_SIZE );
		c = GetChar(1);
	}
	UngetChar();

	// Get rid of trailing whitespace.
	while( Length>0 && (Temp[Length-1]==' ' || Temp[Length-1]==9 ) )
		Length--;
	Temp[Length]=0;

	Token.SetConstString(Temp);

	return Length>0;
	unguard;
}

//
// Get an identifier token, return 1 if gotten, 0 if not.
//
int	FScriptCompiler::GetIdentifier( FToken &Token, INT NoConsts )
{
	guard(FScriptCompiler::GetIdentifier);

	if( !GetToken( Token, NULL, NoConsts ) )
		return 0;

	if( Token.TokenType == TOKEN_Identifier )
		return 1;

	UngetToken(Token);
	return 0;
	unguard;
}

//
// Get a symbol token, return 1 if gotten, 0 if not.
//
int	FScriptCompiler::GetSymbol( FToken &Token )
{
	guard(FScriptCompiler::GetSymbol);

	if( !GetToken(Token) )
		return 0;

	if( Token.TokenType == TOKEN_Symbol ) 
		return 1;

	UngetToken(Token);
	return 0;
	unguard;
}

//
// Get an integer constant, return 1 if gotten, 0 if not.
//
int FScriptCompiler::GetConstInt( int &Result, const char *Tag )
{
	guard(FScriptCompiler::GetConstInt);

	FToken Token;
	if( GetToken(Token) ) 
	{
		if( Token.GetConstInt(Result) ) return 1;
		else                            UngetToken(Token);
	}

	if( Tag ) throwf( "%s: Missing constant integer" );
	return 0;

	unguard;
}

//
// Get a real number, return 1 if gotten, 0 if not.
//
int FScriptCompiler::GetConstFloat( FLOAT &Result, const char *Tag )
{
	guard(FScriptCompiler::GetConstFloat);

	FToken Token;
	if( GetToken(Token) ) 
	{
		if( Token.GetConstFloat(Result) ) return 1;
		else                              UngetToken(Token);
	}

	if( Tag ) throwf( "%s: Missing constant integer" );
	return 0;

	unguard;
}

//
// Get a specific identifier and return 1 if gotten, 0 if not.
// This is used primarily for checking for required symbols during compilation.
//
int	FScriptCompiler::MatchIdentifier( const char *Match )
{
	guard(FScriptCompiler::MatchIdentifier);
	FToken Token;

	if( !GetToken(Token) )
		return 0;

	if( (Token.TokenType==TOKEN_Identifier) && !stricmp(Token.Identifier,Match) )
		return 1;

	UngetToken(Token);
	return 0;
	unguard;
}

//
// Get a specific symbol and return 1 if gotten, 0 if not.
//
int	FScriptCompiler::MatchSymbol( const char *Match )
{
	guard(FScriptCompiler::MatchSymbol);
	FToken Token;

	if( !GetToken(Token,NULL,1) )
		return 0;

	if( Token.TokenType==TOKEN_Symbol && !stricmp(Token.Identifier,Match) )
		return 1;

	UngetToken(Token);
	return 0;
	unguard;
}

//
// Peek ahead and see if a symbol follows in the stream.
//
int FScriptCompiler::PeekSymbol( const char *Match )
{
	guard(FScriptCompiler::PeekSymbol);

	FToken Token;
	if( !GetToken(Token,NULL,1) )
		return 0;
	UngetToken(Token);

	return Token.TokenType==TOKEN_Symbol && stricmp(Token.Identifier,Match)==0;
	unguard;
}

//
// Unget the most recently gotten token.
//
void inline FScriptCompiler::UngetToken(FToken &Token)
{
	guardSlow(FScriptCompiler::UngetToken);
	InputPos	= Token.StartPos;
	InputLine	= Token.StartLine;
	unguardSlow;
}

//
// Require a symbol.
//
void FScriptCompiler::RequireSymbol( const char *Match, const char *Tag )
{
	guardSlow(FScriptCompiler::RequireSymbol);
	if( !MatchSymbol(Match) )
		throwf( "Missing '%s' in %s", Match, Tag );
	unguardSlow;
}

//
// Require an identifier.
//
void FScriptCompiler::RequireIdentifier( const char *Match, const char *Tag )
{
	guardSlow(FScriptCompiler::RequireSymbol);
	if( !MatchIdentifier(Match) )
		throwf( "Missing '%s' in %s", Match, Tag );
	unguardSlow;
}

//
// Require a SizeOf-style parenthesis-enclosed type.
//
void FScriptCompiler::RequireSizeOfParm( FToken &TypeToken, const char *Tag )
{
	guard(FScriptCompiler::RequireSizeOfParm);

	// Setup a retry point.
	FRetryPoint Retry;
	InitRetry(Retry);

	// Get leading paren.
	RequireSymbol("(","'ArrayCount'");

	// Get a variable expression.
	CompileExpr( FProperty(CPT_None), Tag, &TypeToken );
	if( TypeToken.Type == CPT_None )
		throwf( "Bad or missing type in '%s'", Tag );

	// Get trailing paren.
	RequireSymbol(")","'ArrayCount'");

	// Return binary code pointer (not script text) to where it was.
	PerformRetry(Retry,1,0);

	unguard;
}

/*-----------------------------------------------------------------------------
	Variables.
-----------------------------------------------------------------------------*/

//
// Find a variable in the specified (possibly null) class context.
// Returns 1 if found, 0 if not found.
//
int FScriptCompiler::FindVariable
(
	FName		Name,
	UClass::Ptr	ClassContext,
	FProperty*  FoundType
)
{
	guard(FScriptCompiler::FindVariable);
	checkInput(Name!=NAME_None);

	// Check locals.
	if( ClassContext == NULL )
	{
		ClassContext = Class;
		if( TopNode->NestType!=NEST_Class && TopNode->NestType!=NEST_State )
		{
			// We have a locals frame, so check it.
			for( int i=TopNode->iFirstProperty; i<TopNode->iFirstProperty+TopNode->NumProperties; i++ )
			{
				FProperty &Property = Class(i);
				if( Property.Name == Name )
				{
					if( FoundType  )
						*FoundType  = Property;
					return 1;
				}
			}
		}
	}

	// Check globals.
	for( FPropertyIterator It(ClassContext); It; ++It )
	{
		FProperty &Property = It();
		if( Property.Name==Name && ( Property.Bin==PROPBIN_PerObject || Property.Bin==PROPBIN_PerClass ) )
		{
			if( FoundType )
				*FoundType  = Property;
			return 1;
		}
	}
	return 0;
	unguard;
}

//
// Emit a variable reference.
//
void FScriptCompiler::EmitSimpleVariable
(
	const FProperty &Type,
	WORD			wOffset,
	BOOL			DefaultSpecifier
)
{
	guard(FScriptCompiler::EmitVariable);

	// Get the bin identifier.
	EExprToken ExprToken = EX_DefaultVariable;
	if( !DefaultSpecifier ) switch( Type.Bin )
	{
		case PROPBIN_PerObject:		ExprToken = EX_ObjectVariable;    break;
		case PROPBIN_PerClass:		ExprToken = EX_StaticVariable;    break;
		case PROPBIN_PerFunction:	ExprToken = EX_LocalVariable;     break;
		default:					throwf( "Unknown property bin" ); break;
	}

	// Emit the bin info.
	if( Type.Type == CPT_Bool )
	{
		// A boolean, so emit the bin identifier and the bitmask.
		BYTE  bOffset = 0;
		DWORD BitMask = Type.BitMask;
		checkState(BitMask!=0);
		while( BitMask != 1 )
		{
			bOffset++;
			BitMask = BitMask >> 1;
		}
		BYTE bCode = bOffset | (ExprToken << 5);
		*Script << EX_BoolVariable;
		*Script << bCode;
	}
	else
	{
		// Just emit the bin identifier.
		*Script << ExprToken;
	}

	// Emit the offset.
	*Script << wOffset;

	unguard;
}

//
// See if Token is a local or global variable.  Returns 1 if it is, or 0 if it's
// not a recognized variable.  If successful, sets Type to the variable's type.
//
int FScriptCompiler::CompileVariableExpr
(
	FToken		Token,
	UClass::Ptr	ClassContext,
	FToken		&ResultType
)
{
	guard(FScriptCompiler::CompileVariableExpr);

	// Get variable name.
	if( Token.TokenName == NAME_None ) 
	{
		// No global name means it's not a variable.
		return 0;
	}
	if( Token.TokenName == NAME_Self )
	{
		// Special Self context expression.
		*Script << EX_Self;
		ResultType = FToken(FProperty( CPT_Object, ClassContext ? ClassContext : Class ));
		return 1;
	}

	// Handle default specifier.
	BOOL DefaultSpecifier = 0;
	if( Token.TokenName==NAME_Default && MatchSymbol(".") )
	{
		GetToken( Token );
		if( Token.TokenType != TOKEN_Identifier )
			throwf( "Missing variable after 'Default.'" );
		DefaultSpecifier = 1;
	}

	if( !FindVariable( Token.TokenName, ClassContext, &ResultType ) )
	{
		// Not a variable in this class context.
		if( DefaultSpecifier )
			throwf( "Can't find default variable '%s'", Token.TokenName() );
		return 0;
	}
	else
	{
		// Process the variable we found.
		if( DefaultSpecifier && ResultType.Bin!=PROPBIN_PerObject )
			throwf( "You can't access the default value of static and local variables" );

		// Set class context.
		if( !ClassContext )
			ClassContext = Class;

		// Handle variable.
		int IsArrayElement = 0;
		if( ResultType.ArrayDim!=1 && MatchSymbol("[") )
		{
			// Handle array opening.
			IsArrayElement = 1;
			*Script << EX_ArrayElement;
		}
		else if( ResultType.ArrayDim!=1 && MatchSymbol("(") )
			throwf( "Use [] for arrays, not ()" );

		// Make sure the variable is accessible.
		if( ResultType.Flags & CPF_Private )
			throwf( "Variable '%s' is internal and can't be accessed", ResultType.Name() );

		FRetryPoint LowRetry; InitRetry(LowRetry);
		if( IsArrayElement )
		{
			checkLogic(MAX_ARRAY_SIZE<256);

			// Prepare to save element size and array dimension.
			BYTE ElementSize = ResultType.ElementSize;
			BYTE ArrayDim    = ResultType.ArrayDim;

			ResultType.ArrayDim = 1;
			CompileExpr( FProperty(CPT_Int), "array index" );
			if( !MatchSymbol("]") )
				throwf( "%s is an array; expecting ']'",Token.Identifier );
			*Script << ElementSize;
			*Script << ArrayDim;
		}

		// Intercept member selection operator and get member offset.
		WORD wOffset = ResultType.Offset;
		if( PeekSymbol(".") )
		{
			// Consider impact of member selection on offset.
			if( ResultType.Type==CPT_Vector )
			{
				// Handle vector members.
				MatchSymbol(".");
				FToken Tag; GetToken(Tag);
				if     ( Tag.Matches(NAME_X) ) wOffset += 0;
				else if( Tag.Matches(NAME_Y) ) wOffset += 4;
				else if( Tag.Matches(NAME_Z) ) wOffset += 8;
				else throwf( "Unrecognized vector component" );
				ResultType.Init( ResultType.Bin, CPT_Float, ResultType.Flags & CPF_PropagateFromStruct );
			}
			else if( ResultType.Type==CPT_Rotation )
			{
				// Handle rotation members.
				MatchSymbol(".");
				FToken Tag; GetToken(Tag);
				if     ( Tag.Matches(NAME_Pitch) ) wOffset += 0;
				else if( Tag.Matches(NAME_Yaw  ) ) wOffset += 4;
				else if( Tag.Matches(NAME_Roll ) ) wOffset += 8;
				else throwf( "Unrecognized rotation component" );
				ResultType.Init( ResultType.Bin, CPT_Int, ResultType.Flags & CPF_PropagateFromStruct );
			}
		}

		// Emit the variable info.
		FRetryPoint HighRetry; InitRetry(HighRetry);
		EmitSimpleVariable( ResultType, wOffset, DefaultSpecifier );

		// Switch array info and variable info.
		CodeSwitcheroo( LowRetry, HighRetry );

		// Note that the resulting type is an l-value.
		ResultType.Flags |= CPF_OutParm;
		return 1;
	}
	unguard;
}

//
// Compile an enumeration definition.
//
UEnumDef* FScriptCompiler::CompileEnum()
{
	guard(FScriptCompiler::CompileEnum);

	// Get enumeration name.
	FToken EnumToken;
	if( !GetIdentifier(EnumToken) )
		throwf( "Missing enumeration name" );

	// Verify that the enumeration definition is unique within this scope.
	if( EnumToken.TokenName!=NAME_None && FindVariable(EnumToken.TokenName,(UClass*)NULL) )
		throwf( "Enum: Enumeration '%s' already defined here", EnumToken.TokenName() );

	// Get opening brace.
	RequireSymbol("{","'enum'");

	// Parse all enums tags.
	INT NumEnums=0;
	FToken TagTokens[255]; // Maximum enumerations allowed.
	while( GetIdentifier(TagTokens[NumEnums]) )
	{
		for( int i=0; i<NumEnums; i++ )
		{
			if( !stricmp(TagTokens[i].Identifier,TagTokens[NumEnums].Identifier) )
				throwf( "Duplicate enumerator %s",TagTokens[NumEnums].Identifier );
		}

		if( ++NumEnums > 255 )
			throwf( "Exceeded maximum of 255 enumerators" );

		if( !MatchSymbol(",") )
			break;
	}

	// Make sure we got the tags safely.
	if( !NumEnums )
		throwf( "Enumeration must contain at least one enumerator" );

	// Trailing brace.
	RequireSymbol("}","'enum'");

	// Add enumeration as a class property.
	FProperty VarProperty;
	VarProperty.Init
	(
		TopNest->NestType==NEST_Class ? PROPBIN_PerObject : PROPBIN_PerFunction,
		CPT_EnumDef
	);
	VarProperty.Name = FName( EnumToken.Identifier, FNAME_Add );
	VarProperty.Enum = new(EnumToken.Identifier,CREATE_Replace)UEnumDef(NumEnums);//MakeUnique would be better for scoping, but some enums like EButton are hardcoded!!
	BYTE *Data; FProperty *Result = &Class->AddProperty( VarProperty, Data );
	if( TopNode->NestType==NEST_Function || TopNode->NestType==NEST_Operator )
		TopNode->NumProperties++;

	// Add all the enum tags.
	FName TempName;
	for( int i=0; i<NumEnums; i++ )
		VarProperty.Enum->AddItem( FName(TagTokens[i].Identifier,FNAME_Add) );

	return VarProperty.Enum;
	unguard;
}

/*-----------------------------------------------------------------------------
	Retry management.
-----------------------------------------------------------------------------*/

//
// Remember the current compilation points, both in the source being
// compiled and the object code being emitted.  Required because
// UnrealScript grammar isn't quite LALR-1.
//
void FScriptCompiler::InitRetry( FRetryPoint &Retry )
{
	guardSlow(FScriptCompiler::InitRetry);

	Retry.InputPos	= InputPos;
	Retry.InputLine	= InputLine;
	Retry.CodeTop	= Script ? Script->Num : 0;

	unguardSlow;
}

//
// Return to a previously-saved retry point.
//
void FScriptCompiler::PerformRetry( FRetryPoint &Retry, BOOL Binary, BOOL Text )
{
	guardSlow(FScriptCompiler::PerformRetry);

	if( Text	         ) InputPos	 = Retry.InputPos;
	if( Text	         ) InputLine = Retry.InputLine;
	if( Binary && Script ) checkState(Retry.CodeTop <= Script->Num);
	if( Binary && Script ) Script->Num	= Retry.CodeTop;

	unguardSlow;
}

//
// Insert the code in the interval from Retry2-End into the code stream
// beginning at Retry1.
//
void FScriptCompiler::CodeSwitcheroo( FRetryPoint &LowRetry, FRetryPoint &HighRetry )
{
	guardSlow(FScriptCompiler::CodeSwitcheroo);
	FMemMark Mark(GMem);
	INT HighSize = Script->Num       - HighRetry.CodeTop;
	INT LowSize  = HighRetry.CodeTop - LowRetry.CodeTop;

	BYTE *Temp = new(GMem,HighSize)BYTE;
	memcpy ( Temp,                                          &Script->Element(HighRetry.CodeTop),HighSize);
	memmove( &Script->Element(LowRetry.CodeTop + HighSize), &Script->Element(LowRetry.CodeTop), LowSize);
	memcpy ( &Script->Element(LowRetry.CodeTop           ), Temp,                               HighSize);

	Mark.Pop();
	unguardSlow;
}

/*-----------------------------------------------------------------------------
	Functions.
-----------------------------------------------------------------------------*/

//
// Try to compile a complete function call with a name matching Token.  Returns
// 1 if a function call was successfully parsed, or 0 if no matching function
// was found.  Handles the error condition where the function was called but the
// specified parameters didn't match, or there was an error in a parameter 
// expression.
//
// The function to call must be accessible within the current scope.
//
// This also handles unary operators identically to functions, but not binary
// operators.
//
// Sets ResultType to the function's return type.
//
int FScriptCompiler::CompileFunctionExpr
(
	FToken		Token,
	UClass::Ptr	ClassContext,
	FToken		&ResultType
)
{
	guard(FScriptCompiler::CompileFunctionExpr);
	FRetryPoint Retry; InitRetry(Retry);

	FStackNodePtr Link(NULL,0);
	UClass         *CallClass = NULL;
	BOOL           ForceFinal = 0;
	BOOL           CallGlobal = 0;
	BOOL           CallParent = 0;
	BOOL           IsFunction = 0;
	BOOL           Accept     = 1;

	// Handle function specifiers CallGlobal and CallClass.
	for( ; ; )
	{
		if( Token.Matches(NAME_CallGlobal) )
		{
			// Call the highest global, final (non-state) version of a function.
			ForceFinal = 1;
			CallGlobal = 1;
			IsFunction = 1;
			RequireSymbol( ".", "'CallGlobal'" );
			if( ClassContext )
				throwf( "Can only use 'CallGlobal' with self, not with other actors" );
		}
		else if( Token.Matches(NAME_CallParent) )
		{
			ForceFinal = 1;
			IsFunction = 1;
			Accept     = 0;
			CallParent = 1;
			CallClass  = Class->ParentClass;
			RequireSymbol( ".", "'CallParent'" );
			if( ClassContext )
				throwf( "Can only use 'CallParent' with self, not with other actors" );
		}
		else if( Token.Matches(NAME_CallClass) )
		{
			// Call the final version of a function residing at or below a certain class.
			ForceFinal = 1;
			IsFunction = 1;
			Accept     = 0;
			RequireSymbol("(","'CallClass'");
			FToken ClassToken;
			if( !GetIdentifier(ClassToken) )
				throwf( "Missing class name" );
			CallClass = new(ClassToken.Identifier,FIND_Optional)UClass;
			if( !CallClass )
				throwf( "Bad class name '%s'", ClassToken.Identifier );
			RequireSymbol(")","'CallClass'");
			RequireSymbol(".","'CallClass'");
			if( ClassContext )
				throwf( "Can only use 'CallClass' with self, not with other actors" );
		}
		else break;
		GetToken(Token);
	}

	// No name, or no opening paren means this is not a function.
	if( Token.TokenName==NAME_None || !MatchSymbol("(") )
	{
		PerformRetry(Retry);
		return 0;
	}

	// Note that this expression has side effects.
	GotAffector = 1;

	// Make sure the class context is ok.
	if( ClassContext && (ClassContext->StackTree==NULL || ClassContext->StackTree->Num==0) )
		throwf( "Class '%s' is not scripted",ClassContext->GetName() );

	// Find the stack node link for this function.
	for( int i = (ClassContext||CallGlobal) ? 1 : NestLevel-1; i>=1 && Link.Class==NULL; i-- )
	{
		FStackNodePtr TestLink = ClassContext
		?	ClassContext->StackTree->Element(0).ChildFunctions
		:	Nest[i].Node()->ChildFunctions;
		while( TestLink.Class!=NULL && Link.Class==NULL )
		{
			FStackNode &Node = *TestLink;
			Accept           = Accept || TestLink.Class==CallClass || (CallParent && TestLink.Class!=Class);
			if( Node.Name==Token.TokenName && Node.NestType==NEST_Function && Accept )
				Link = TestLink;
			TestLink = TestLink->Next;
		}
	}

	// Found a matching function?
	if( Link.Class == NULL )
	{
		if( IsFunction )
			throwf( "Unknown function '%s'", Token.Identifier );
		PerformRetry(Retry);
		return 0;
	}

	// Get node.
	FStackNode &Node = *Link;

	// Verify that the function is callable here.
	if( Node.StackNodeFlags & SNODE_PrivateFunc )
		throwf( "Function '%s' is Private", Node.Name() );
	if( Node.StackNodeFlags & SNODE_LatentFunc )
		CheckAllow( Node.Name(), ALLOW_StateCmd );
	if( (Node.StackNodeFlags & SNODE_IteratorFunc) && !AllowIterator )
		throwf( "Can't call iterator functions here" );
	if( Node.StackNodeFlags & SNODE_IteratorFunc )
		GotIterator = 1;
	
	// Emit the function call.
	EmitStackNodeLinkFunction( Link, ForceFinal );

	// See if this is an iterator with automatic casting of parm 2 object to the parm 1 class.
	BOOL IsIteratorCast =
	(	(Node.StackNodeFlags & SNODE_IteratorFunc)
	&&	(Node.NumParms>=2)
	&&	(Link.Class->Element(Node.iFirstProperty+0).Type==CPT_Object)
	&&	(Link.Class->Element(Node.iFirstProperty+0).Class->IsA("Class"))
	&&	(Link.Class->Element(Node.iFirstProperty+1).Type==CPT_Object ));
	UClass* IteratorClass = NULL;

	// Parse all parameters except for return type; the parameters are emitted to 
	// code as a series of non-empty expressions terminated by EX_EndFunctionParms.
	// If the parameters aren't in agreement, skip this function and it will either be 
	// picked up by an override somewhere along the way, or the "Mismatched parameter" 
	// message will be thrown after we've checked them all.  Handles the case of optional 
	// parameters not specified.
	FToken ParmToken[MAX_FUNC_PARMS];
	for( int j=0; j<Node.NumParms; j++ )
	{
		// Get parameter.
		FProperty Parm = Link.Class->Element( j + Node.iFirstProperty );
		if( Parm.Flags & CPF_ReturnParm )
			break;

		// If this is an iterator, automatically adjust the second parameter's type.
		if( j==1 && IteratorClass )
			Parm.Class = IteratorClass;

		// Get comma parameter delimiter.
		if( j!=0 && !MatchSymbol(",") )
		{
			// Failed to get a comma.
			if( !(Parm.Flags & CPF_OptionalParm) )
				throwf( "Call to '%s': missing or bad parameter %i", Node.Name(), j+1 );

			// Ok, it was optional.
			break;
		}

		int Result = CompileExpr( Parm, NULL, &ParmToken[j] );
		if( Result == -1 )
		{
			// Type mismatch.
			throwf( "Call to '%s': type mismatch in parameter %i", Token.Identifier, j+1 );
		}
		else if( Result == 0 )
		{
			// Failed to get an expression.
			if( !(Parm.Flags & CPF_OptionalParm) ) 
				throwf( "Call to '%s': bad or missing parameter %i", Token.Identifier, j+1 );
			if( PeekSymbol("(") )
				break;
			else
				*Script << EX_Nothing;
		}
		else if( IsIteratorCast && j==0 )
			ParmToken[j].GetConstObject( UClass::GetBaseClass(), *(UObject**)&IteratorClass );
	}
	for( ; j<Node.NumParms && !(Link.Class->Element(j + Node.iFirstProperty).Flags & CPF_ReturnParm); j++ )
	{
		checkState(Link.Class->Element(j + Node.iFirstProperty).Flags & CPF_OptionalParm);
		*Script << EX_Nothing;
	}

	// Get closing paren.
	FToken Temp;
	GetToken(Temp);
	if( !Temp.Matches(")") )
		throwf( "Call to '%s': Bad '%s' or missing ')'", Token.Identifier, Temp.Identifier );

	// Emit end-of-function-parms tag.
	*Script << EX_EndFunctionParms;
	FProperty &Return = Link.Class->Element( Node.iFirstProperty + Node.NumParms - 1 );

	// Spawn special case: Make return type the same as a constant class passed to it.
	if( Token.Matches(NAME_Spawn) )
		ParmToken[0].GetConstObject( UClass::GetBaseClass(), *(UObject**)&Return.Class);
	
	// Check return value.
	if( Node.NumParms==0 || !(Return.Flags & CPF_ReturnParm) )
	{
		// No return value.
		ResultType = FToken(CPT_None);
	}
	else
	{
		// Has a return value.
		ResultType        = Return;
		ResultType.Flags &= ~CPF_OutParm;
	}

	return 1;
	unguard;
}

/*-----------------------------------------------------------------------------
	Type conversion.
-----------------------------------------------------------------------------*/

//
// Return the cost of converting a type from Source to Dest:
//    0 if types are identical.
//    1 if dest is more precise than source.
//    2 if converting integral to float.
//    3 if dest is less precise than source, or a generalization of source.
//    MAXINT if the types are incompatible.
//
int FScriptCompiler::ConversionCost( const FProperty &Dest, const FProperty &Source )
{
	guard(FScriptCompiler::ConversionCost);
	DWORD Conversion = GConversions[Dest.Type][Source.Type];

	if( Dest.MatchesType(Source,1) )
	{
		// Identical match.
		//AddResultText("Identical\r\n");
		return 0;
	}
	else if( Dest.Flags & CPF_OutParm )
	{
		// If converting to l-value, conversions aren't allowed.
		//AddResultText("IllegalOut\r\n");
		return MAXINT;
	}
	else if( Dest.MatchesType(Source,0) )
	{
		// Generalization.
		//AddResultText("Generalization\r\n");
		int Result = 1;
		if( Source.Type==CPT_Object && Source.Class!=NULL )
		{
			// The fewer classes traversed in this conversion, the better the quality.
			checkState(Dest.Type==CPT_Object);
			checkState(Dest.Class!=NULL);
			for( UClass *Test=Source.Class; Test && Test!=Dest.Class; Test=Test->ParentClass )
				Result++;
			checkState(Test!=NULL);
		}
		return Result;
	}
	else if( Dest.ArrayDim!=1 || Source.ArrayDim!=1 )
	{
		// Can't cast arrays.
		//AddResultText("NoCastArrays\r\n");
		return MAXINT;
	}
	else if( Dest.Type==CPT_Byte && Dest.Enum!=NULL )
	{
		// Illegal enum cast.
		//AddResultText("IllegalEnumCast\r\n");
		return MAXINT;
	}
	else if( Dest.Type==CPT_Object && Dest.Class!=NULL )
	{
		// Illegal object cast.
		//AddResultText("IllegalObjectCast\r\n");
		return MAXINT;
	}
	else if( (Dest.Flags & CPF_CoerceParm) ? (Conversion==EX_Max) : !(Conversion & AUTOCONVERT) )
	{
		// No conversion at all.
		//AddResultText("NoConversion\r\n");
		return MAXINT;
	}
	else if( GConversions[Dest.Type][Source.Type] & TRUNCATE )
	{
		// Truncation.
		//AddResultText("Truncation\r\n");
		return 103;
	}
	else if( (Source.Type==CPT_Int || Source.Type==CPT_Byte) && Dest.Type==CPT_Float )
	{
		// Conversion to float.
		//AddResultText("ConvertToFloat\r\n");
		return 102;
	}
	else
	{
		// Expansion.
		//AddResultText("Expansion\r\n");
		return 101;
	}
	unguard;
}

//
// Compile an actor cast expression.
//
int FScriptCompiler::CompileDynamicCast( FToken Token, FToken &ResultType )
{
	guard(FScriptCompiler::CompileDynamicCast);
	FRetryPoint LowRetry; InitRetry(LowRetry);
	if( MatchSymbol("(") )
	{
		UClass *DestClass = new( Token.Identifier, FIND_Optional )UClass;
		if( DestClass )
		{
			// Compile the enclosed expression to cast.
			char Tag[80]; sprintf( Tag, "'%s' cast", DestClass->GetName() );
			FProperty RequiredType( CPT_Object, UObject::GetBaseClass() );
			CompileExpr( RequiredType, Tag, &ResultType );

			// Get ending paren.
			RequireSymbol(")",Tag);

			// See what kind of conversion this is.
			if( !ResultType.Class || ResultType.Class->IsChildOf(DestClass) )
			{
				// Redundent conversion.
				throwf
				(
					"Cast from '%s' to '%s' is unnecessary",
					ResultType.Class ? ResultType.Class->GetName() : "None",
					DestClass->GetName()
				);
			}
			else if( DestClass->IsChildOf(ResultType.Class) )
			{
				// Dynamic cast, must be handled at runtime.
				FRetryPoint HighRetry; InitRetry(HighRetry);
				*Script << EX_ActorCast;
				*Script << *(DWORD*)&DestClass;
				CodeSwitcheroo(LowRetry,HighRetry);
			}
			else
			{
				// The cast will always fail.
				throwf( "Cast from '%s' to '%s' will always fail", ResultType.Class->GetName(), DestClass->GetName() );
			}

			// A cast is no longer an l-value.
			ResultType = FToken( FProperty( CPT_Object, DestClass ) );
			return 1;
		}
		else
		{
			// Not an actor class.
			PerformRetry( LowRetry );
			return 0;
		}
	}
	else return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Expressions.
-----------------------------------------------------------------------------*/

//
// Compile an expression. Call with:
//
// RequiredType = the mandatory type of the expression, or CPT_None = any allowable type.
// Optional     = 1 if the expression is optional, 0 if required.
//
// Returns:
//		 0	if no expression was parsed.
//		 1	if an expression matching RequiredType (or any type if CPT_None) was parsed.
//		-1	if there was a type mismatch.
//
// The parsed expression has been emitted to the code stream and begins with one of the EExprToken 
// tokens which will be EX_None if no expression was parsed.
//
int FScriptCompiler::CompileExpr
(
	const FProperty	RequiredType,
	const char		*ErrorTag,
	FToken			*ResultToken,
	INT				MaxPrecedence,
	FProperty		*HintType
)
{
	guard(FScriptCompiler::CompileExpr);
	FRetryPoint LowRetry; InitRetry(LowRetry);

	FToken Token(CPT_None);
	if( !GetToken( Token, HintType ? HintType : &RequiredType ) )
	{
		// This is an empty expression.
		Token.Init(PROPBIN_MAX,CPT_None);
	}
	else if( Token.TokenType == TOKEN_Const )
	{
		// This is some kind of constant.
		Token.AttemptToConvertConstant(RequiredType);
		Token.Flags &= ~CPF_OutParm;
		EmitConstant(Token);
	}
	else if( Token.Matches("(") )
	{
		// Parenthesis. Recursion will handle all error checking.
		if( !CompileExpr( RequiredType, NULL, &Token ) )
			throwf( "Bad or missing expression in parenthesis" );
		RequireSymbol(")","expression");
		if( Token.Type == CPT_None )
			throwf( "Bad or missing expression in parenthesis" );
	}
	else if( Token.TokenName.GetIndex()>CPT_None && Token.TokenName.GetIndex()<CPT_MAX && MatchSymbol("(") )
	{
		// An explicit type conversion.
		EPropertyType ToType = (EPropertyType)Token.TokenName.GetIndex();
		FToken FromType;
		CompileExpr(FProperty(CPT_None),Token.TokenName(),&FromType);
		if( FromType.Type == CPT_None )
			throwf("'%s' conversion: Bad or missing expression",Token.TokenName());

		// Can we perform this explicit conversion?
		if( GConversions[ToType][FromType.Type] != EX_Max )
		{
			// Perform conversion.
			FRetryPoint HighRetry; InitRetry(HighRetry);
			*Script << (EExprToken)(GConversions[ToType][FromType.Type] & CONVERT_MASK);
			CodeSwitcheroo(LowRetry,HighRetry);
			Token = FToken(FProperty(ToType));
		}
		else if( ToType == FromType.Type )
			throwf( "No need to cast '%s' to itself", FName(ToType)() );
		else
			throwf( "Can't convert '%s' to '%s'", FName(FromType.Type)(), FName(ToType)() );

		// The cast is no longer an l-value.
		Token.Flags &= ~CPF_OutParm;
		if( !MatchSymbol(")") )
			throwf( "Missing ')' in type conversion" );
	}
	else if( PeekSymbol("(") && CompileDynamicCast( FToken(Token), Token) )
	{
		// We successfully compiled an actor class.
	}
	else if( CompileVariableExpr( FToken(Token), (UClass*)NULL, Token ) )
	{
		// We successfully parsed a variable expression, or a context function call.
	}
	else if( CompileFunctionExpr( FToken(Token), (UClass*)NULL, Token ) )
	{
		// We successfully parsed a function-call expression.
		Token.Flags &= ~CPF_OutParm;
	}
#if 0
	else if( RequiredType.Type==CPT_Name && Token.TokenName!=NAME_None )
	{
		// This is a name constant, recognized after variables and functions.
		Token.SetConstName( Token.TokenName );
		Token.Flags &= ~CPF_OutParm;
		EmitConstant(Token);
	}
	else if( RequiredType.Type==CPT_Object && RequiredType.Class!=NULL && GObj.FindObject( Token.Identifier, RequiredType.Class, FIND_Optional ) )
	{
		// This is an object constant, recognized after variables and functions.
		Token.SetConstObject( RequiredType.Class, GObj.FindObject( Token.Identifier, RequiredType.Class, FIND_Optional ) );
		Token.Flags &= ~CPF_OutParm;
		EmitConstant(Token);
	}
#endif
	else
	{
		// This doesn't match an expression, so put it back.  It might be some kind of
		// delimiter like a comma, but whatever routine called this will error out if it's
		// not valid in its particular context.
		UngetToken(Token);
		Token.Init(PROPBIN_MAX,CPT_None);
	}

	// Intercept member selection operator.
	FToken OriginalToken=Token;
	while( Token.Type==CPT_Object && MatchSymbol(".") )
	{
		// Compile an actor context expression.
		checkState(Token.Class!=NULL);

		// Emit actor context override token.
		FRetryPoint HighRetry; InitRetry(HighRetry);
		*Script << EX_Context;
		CodeSwitcheroo( LowRetry, HighRetry );

		// Get new class context.
		UClass *NewClassContext = Token.Class;

		// Make sure this class context is on our dependency list.
		Class->Dependencies->AddUniqueItem(FDependency(NewClassContext));

		// Get the context variable or expression.
		FToken TempToken;
		GetToken(TempToken);

		// Get ready to compile expr.
		FRetryPoint ContextStart; InitRetry(ContextStart);

		// Compile a variable or function expression.
		if(	!CompileVariableExpr( TempToken, NewClassContext, Token )
		&&	!CompileFunctionExpr( FToken(TempToken), NewClassContext, Token ) )
			throwf( "Unrecognized class member after '%s'", OriginalToken.TokenName() );

		// Insert skipover info for handling null contexts.
		FRetryPoint ContextEnd; InitRetry(ContextEnd);
		WORD wSkip = Script->Num - ContextStart.CodeTop; *Script << wSkip;
		EmitSize(Token.Size(),"Context expression");
		CodeSwitcheroo( ContextStart, ContextEnd );
	}

	// See if the following character is a binary or postfix operator.
	Test:
	FToken OperToken;
	INT NumOperLinks=0, Precedence=0, BestMatch=0, Matches=0, NumParms=3;
	FStackNodePtr OperLinks[16], BestOperLink;
	INT RequiredStackNodeFlags = Token.Type==CPT_None ? SNODE_PreOperatorFunc : 0;
	if( GetToken(OperToken,NULL,1) )
	{
		if( OperToken.TokenName != NAME_None )
		{
			// Build a list of matching operators.
			for( int i = NestLevel-1; i>=1; i-- )
			{
				FStackNodePtr TestLink = Nest[i].Node()->ChildFunctions;
				while( TestLink.Class != NULL )
				{
					FStackNode &Node = *TestLink;
					if
					(
						Node.Name              == OperToken.TokenName
					&&	Node.NestType          == NEST_Operator
					&&	RequiredStackNodeFlags == (Node.StackNodeFlags & SNODE_PreOperatorFunc)
					)
					{
						// Add this operator to the list.
						OperLinks[NumOperLinks++] = TestLink;
						Precedence                = Node.OperPrecedence;
						NumParms                  = Min(NumParms,(int)Node.NumParms);
						checkState(NumOperLinks < ARRAY_COUNT(OperLinks));
					}
					TestLink = Node.Next;
				}
			}

			// See if we got a valid operator, and if we want to handle it at the current precedence level.
			if( NumOperLinks>0 && Precedence<MaxPrecedence )
			{
				// Compile the second expression.
				FRetryPoint MidRetry; InitRetry(MidRetry);
				FProperty NewRequiredType(CPT_None);
				FToken NewResultType;
				if( NumParms==3 || RequiredStackNodeFlags==SNODE_PreOperatorFunc )
				{
					char NewErrorTag[80];
					sprintf( NewErrorTag, "Following '%s'", OperToken.TokenName() );
					CompileExpr( NewRequiredType, NewErrorTag, &NewResultType, Precedence, &Token );
					if( NewResultType.Type == CPT_None )
						throwf("Bad or missing expression after '%s'", OperToken.TokenName() );
				}

				// Figure out which operator overload is best.
				BestOperLink.Class = NULL;
				//AddResultText("Oper %s:\r\n",OperLinks[0]->Node().Name());
				BOOL AnyLeftValid=0, AnyRightValid=0;
				for( int i=0; i<NumOperLinks; i++ )
				{
					// See how good a match the first parm is.
					FStackNode  &Node     = *OperLinks[i];
					INT			ThisMatch = 0;
					INT			iParm     = 0;

					if( Node.NumParms==3 || RequiredStackNodeFlags!=SNODE_PreOperatorFunc )
					{
						// Check match of first parm.
						FProperty &Parm1 = OperLinks[i].Class->Element(Node.iFirstProperty + iParm++);
						//AddResultText("Left  (%s->%s): ",FName(Token.Type)(),FName(Parm1.Type)());
						INT Cost         = ConversionCost(Parm1,Token);
						ThisMatch        = Cost;
						AnyLeftValid     = AnyLeftValid || Cost!=MAXINT;
					}

					if( Node.NumParms == 3 || RequiredStackNodeFlags==SNODE_PreOperatorFunc )
					{
						// Check match of second parm.
						FProperty &Parm2 = OperLinks[i].Class->Element(Node.iFirstProperty + iParm++);
						//AddResultText("Right (%s->%s): ",FName(NewResultType.Type)(),FName(Parm2.Type)());
						INT Cost         = ConversionCost(Parm2,NewResultType);
						ThisMatch        = Max(ThisMatch,Cost);
						AnyRightValid    = AnyRightValid || Cost!=MAXINT;
					}

					if( (BestOperLink.Class==NULL || ThisMatch<BestMatch) && (Node.NumParms==NumParms) )
					{
						// This is the best match.
						BestOperLink = OperLinks[i];
						BestMatch    = ThisMatch;
						Matches      = 1;
					}
					else if( ThisMatch == BestMatch ) Matches++;
				}
				if( BestMatch == MAXINT )
				{
					if( AnyLeftValid && !AnyRightValid )
						throwf( "Right type is incompatible with '%s'", OperToken.TokenName() );
					else if( AnyRightValid && !AnyLeftValid )
						throwf( "Left type is incompatible with '%s'", OperToken.TokenName() );
					else
						throwf( "Types are incompatible with '%s'", OperToken.TokenName() );
				}
				else if( Matches > 1 )
					throwf( "Operator '%s': Can't resolve overload (%i matches of quality %i)", OperToken.TokenName(), Matches, BestMatch );

				//
				// Now BestOperLink points to the operator we want to use, and the code stream
				// looks like:
				//
				//       |LowRetry| Expr1 |MidRetry| Expr2
				//
				// Here we carefully stick any needed expression conversion operators into the
				// code stream, and swap everything until we end up with:
				//
				// |LowRetry| Oper Size1 [Conv1] Expr1 |MidRetry| Size2 [Conv2] Expr2 0
				//

				// Get operator parameter pointers.
				checkState(BestOperLink.Class!=NULL);
				FStackNode &Node = BestOperLink.Class->StackTree->Element(BestOperLink.iNode);
				FProperty &OperParm1  = BestOperLink.Class->Element(Node.iFirstProperty + 0);
				FProperty &OperParm2  = BestOperLink.Class->Element(Node.iFirstProperty + (RequiredStackNodeFlags!=SNODE_PreOperatorFunc));
				FProperty &OperReturn = BestOperLink.Class->Element(Node.iFirstProperty + Node.NumParms - 1);
				checkState(OperReturn.Flags & CPF_ReturnParm);

				// Convert Expr2 if necessary.
				if( Node.NumParms==3 || RequiredStackNodeFlags==SNODE_PreOperatorFunc )
				{
					if( OperParm2.Flags & CPF_OutParm )
					{
						// Note that this expression has a side-effect.
						GotAffector = 1;
					}
					if( NewResultType.Type != OperParm2.Type )
					{
						// Emit conversion.
						FRetryPoint HighRetry; InitRetry(HighRetry);
						*Script << (EExprToken)(GConversions[OperParm2.Type][NewResultType.Type] & CONVERT_MASK);
						CodeSwitcheroo(MidRetry,HighRetry);
					}
					if( OperParm2.Flags & CPF_SkipParm )
					{
						// Emit skip expression for short-circuit operators.
						FRetryPoint HighRetry; InitRetry(HighRetry);
						WORD wOffset = 1 + HighRetry.CodeTop - MidRetry.CodeTop;
						*Script << EX_Skip;
						*Script << wOffset;
						CodeSwitcheroo(MidRetry,HighRetry);
					}
				}

				// Convert Expr1 if necessary.
				if( RequiredStackNodeFlags != SNODE_PreOperatorFunc )
				{
					if( OperParm1.Flags & CPF_OutParm )
					{
						// Note that this expression has a side-effect.
						GotAffector = 1;
					}
					if( Token.Type != OperParm1.Type  )
					{
						// Emit conversion.
						FRetryPoint HighRetry; InitRetry(HighRetry);
						*Script << (EExprToken)(GConversions[OperParm1.Type][Token.Type] & CONVERT_MASK);
						CodeSwitcheroo(LowRetry,HighRetry);
					}
				}

				// Emit the operator function call.			
				FRetryPoint HighRetry; InitRetry(HighRetry);
				EmitStackNodeLinkFunction( BestOperLink, 1 );
				CodeSwitcheroo(LowRetry,HighRetry);

				// End of call.
				*Script << EX_EndFunctionParms;

				// Update the type with the operator's return type.
				Token        = OperReturn;
				Token.Flags &= ~CPF_OutParm;
				//AddResultText("Oper '%s' returned %s (%i)\r\n",Node.Name(),FName(OperReturn.Type)(),Node.NumParms);
				goto Test;
			}
		}
	}
	UngetToken(OperToken);

	// Verify that we got an expression.
	if( Token.Type==CPT_None && RequiredType.Type!=CPT_None )
	{
		// Got nothing.
		if( ErrorTag )
			throwf( "Bad or missing expression in %s", ErrorTag );
		if( ResultToken ) *ResultToken = Token;
		return 0;
	}

	// Make sure the type is correct.
	if( !RequiredType.MatchesType(Token,0) )
	{
		// Can we perform an automatic conversion?
		DWORD Conversion = GConversions[RequiredType.Type][Token.Type];
		if( RequiredType.Flags & CPF_OutParm )
		{
			// If the caller wants an l-value, we can't do any conversion.
			if( ErrorTag )
			{
				if( Token.TokenType == TOKEN_Const )
					throwf( "Expecting a variable, not a constant" );
				else if( Token.Flags & CPF_Const )
					throwf( "Const mismatch in Out variable %s", ErrorTag );
				else
					throwf( "Type mismatch in Out variable %s", ErrorTag );
			}
			if( ResultToken ) *ResultToken = Token;
			return -1;
		}
		else if( RequiredType.ArrayDim!=1 || Token.ArrayDim!=1 )
		{
			// Type mismatch, and we can't autoconvert arrays.
			if( ErrorTag )
				throwf( "Array mismatch in %s", ErrorTag );
			if( ResultToken ) *ResultToken = Token;
			return -1;
		}
		else if( RequiredType.Type==CPT_String && Token.Type==CPT_String )
		{
			// Perform an automatic string conversion.
			FRetryPoint HighRetry; InitRetry(HighRetry);
			*Script << EX_ResizeString;
			BYTE b = RequiredType.ElementSize; *Script << b;
			CodeSwitcheroo(LowRetry,HighRetry);
			Token.ElementSize = RequiredType.ElementSize;
		}
		else if
		(	( (RequiredType.Flags & CPF_CoerceParm) ? (Conversion!=EX_Max) : (Conversion & AUTOCONVERT) )
		&&	(RequiredType.Type!=CPT_Byte || RequiredType.Enum==NULL) )
		{
			// Perform automatic conversion or coercion.
			FRetryPoint HighRetry; InitRetry(HighRetry);
			*Script << (EExprToken)(GConversions[RequiredType.Type][Token.Type] & CONVERT_MASK);
			CodeSwitcheroo(LowRetry,HighRetry);
			Token.Flags &= ~CPF_OutParm;
			Token = FToken(FProperty(RequiredType.Type));
		}
		else
		{
			// Type mismatch.
			if( ErrorTag )
				throwf( "Type mismatch in %s", ErrorTag );
			if( ResultToken ) *ResultToken = Token;
			return -1;
		}
	}

	if( ResultToken ) *ResultToken = Token;
	return 1;
	unguard;
}

/*-----------------------------------------------------------------------------
	Make.
-----------------------------------------------------------------------------*/

//
// Begin the make.
//
void FScriptCompiler::InitMake()
{
	guard(FScriptCompiler::InitMake);

	StatementsCompiled	= 0;
	LinesCompiled		= 0;
	ErrorText			= new("Results",CREATE_Replace)UTextBuffer(1);

	unguard;
}

//
// End the make.
//
void FScriptCompiler::ExitMake(int Success)
{
	guard(FScriptCompiler::ExitMake);

	if( Success )
	{
		if( LinesCompiled ) AddResultText("Success: Compiled %i line(s), %i statement(s).\r\n",LinesCompiled,StatementsCompiled);
		else AddResultText( "Success: Everything is up to date" );
	}
	else
	{
		//!!AddResultText("Script compilation failed.\r\n");
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Nest information.
-----------------------------------------------------------------------------*/

//
// Return the name for a nest type.
//
const char *FScriptCompiler::NestTypeName(ENestType NestType)
{
	guard(FScriptCompiler::NestTypeName);
	switch( NestType )
	{
		case NEST_None:		return "Global Scope";
		case NEST_Class:	return "Class";
		case NEST_State:	return "State";
		case NEST_Function:	return "Function";
		case NEST_Operator:	return "Operator";
		case NEST_If:		return "If";
		case NEST_Loop:		return "Loop";
		case NEST_Switch:	return "Switch";
		case NEST_For:		return "For";
		case NEST_ForEach:	return "ForEach";
		default:			return "Unknown";
	}
	unguard;
}

//
// Make sure that a particular kind of command is allowed on this nesting level.
// If it's not, issues a compiler error referring to the token and the current
// nesting level.
//
void FScriptCompiler::CheckAllow( const char *Thing, int AllowFlags )
{
	guard(FScriptCompiler::CheckAllow);
	if( (TopNest->Allow & AllowFlags) != AllowFlags )
	{
		if( TopNest->NestType==NEST_None )
		{
			throwf( "%s is not allowed before the Class definition", Thing );
		}
		else
		{
			throwf( "%s is not allowed here", Thing );
		}
	}
	if( AllowFlags & ALLOW_Cmd )
	{
		// Don't allow variable declarations after commands.
		TopNest->Allow &= ~(ALLOW_VarDecl | ALLOW_Function | ALLOW_Ignores);
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Nest management.
-----------------------------------------------------------------------------*/

//
// Increase the nesting level, setting the new top nesting level to
// the one specified.  If pushing a function or state and it overrides a similar
// thing declared on a lower nesting level, verifies that the override is legal.
//
void FScriptCompiler::PushNest( ENestType NestType, const char *Name, INT iNode )
{
	guard(FScriptCompiler::PushNest);
	//debugf( "PushNest %i: %s", iNode, Name );

	// Defaults.
	FName		ThisName(Name,FNAME_Add);
	FStackNode	*PrevNode	= NULL;
	DWORD		PrevAllow	= 0;

	// Update pointer to top nesting level.
	TopNest					= &Nest[NestLevel++];
	TopNode					= NULL;
	TopNest->Link			= FStackNodePtr(Class,iNode);
	TopNest->NestType		= NestType;
	TopNest->iCodeChain		= INDEX_NONE;
	TopNest->SwitchType		= FProperty(CPT_None);
	TopNest->FixupList		= NULL;
	TopNest->LabelList		= NULL;

	// Init fixups.
	for( int i=0; i<FIXUP_MAX; i++ )
		TopNest->Fixups[i] = MAXWORD;

	// Prevent overnesting.
	if( NestLevel >= MAX_NEST_LEVELS )
		throwf( "Maximum nesting limit exceeded" );

	// Inheret info from stack node above us.
	INT IsNewNode = NestType==NEST_Class || NestType==NEST_State || NestType==NEST_Function || NestType==NEST_Operator;
	if( NestLevel > 1 )
	{
		if( Pass == 1 )
		{
			if( !IsNewNode )
				TopNest->Link = TopNest[-1].Link;
			TopNode = TopNest[0].Node();
		}
		else
		{
			if( IsNewNode )
			{	
				// Create a new stack node.
				TopNest->Link				= FStackNodePtr( Class, Class->StackTree->Add() );
				TopNode						= TopNest->Node();

				// Init links.
				TopNode->ParentItem         = FStackNodePtr(NULL,0);
				TopNode->ParentNest			= FStackNodePtr(NULL,0);
				TopNode->ChildFunctions		= FStackNodePtr(NULL,0);
				TopNode->ChildStates		= FStackNodePtr(NULL,0);
				TopNode->Next				= FStackNodePtr(NULL,0);

				// Init general info.
				TopNode->Name				= ThisName;
				TopNode->CodeLabelOffset	= MAXWORD;
				TopNode->iCode				= MAXWORD;
				TopNode->NestType			= NestType;
				TopNode->StackNodeFlags		= 0;
				TopNode->NumParms			= 0;
				TopNode->OperPrecedence		= 0;

				// Init type-specific info.
				if( NestType==NEST_Class || NestType == NEST_State )
				{
					// Init class/state info.
					TopNode->ProbeMask			= 0;
					TopNode->IgnoreMask			= ~(QWORD)0;
					TopNode->VfHash             = NULL;
				}
				else
				{
					// Init function info.
					TopNode->LocalsSize			= 0;
					TopNode->ParmsSize			= 0;
					TopNode->iIntrinsic			= 0;
					TopNode->iFirstProperty		= MAXWORD;
					TopNode->NumProperties		= 0;
					TopNode->HashNext           = FStackNodePtr(NULL,0);

					// Init the temporary locals bin.
					Class->Bins[PROPBIN_PerFunction]->Num = 0;
				}
			}
			else
			{
				// Use the existing stack node.
				TopNest->Link = TopNest[-1].Link;
				TopNode		  = &Class->StackTree->Element(TopNest->Link.iNode);
			}
		}
		checkState(TopNode!=NULL);
		PrevNode  = TopNest[-1].Node();
		PrevAllow = TopNest[-1].Allow;
	}

	// NestType specific logic.
	switch( NestType )
	{
		case NEST_None:
			checkState(PrevNode==NULL);
			TopNest->Allow				= ALLOW_Class;
			break;

		case NEST_Class:
			checkState(ThisName!=NAME_None);
			checkState(PrevNode==NULL);
			TopNest->Allow = ALLOW_VarDecl | ALLOW_Function | ALLOW_State | ALLOW_Ignores;
			break;

		case NEST_State:
			checkState(ThisName!=NAME_None);
			checkState(PrevNode!=NULL);
			TopNest->Allow = ALLOW_Function | ALLOW_Label | ALLOW_StateCmd | ALLOW_Ignores;
			if( Pass==0 )
			{
				TopNode->ParentNest      = TopNest[-1].Link;
				TopNode->Next			 = PrevNode->ChildStates;
				PrevNode->ChildStates    = TopNest->Link;
			}
			break;

		case NEST_Function:
			checkState(ThisName!=NAME_None);
			checkState(PrevNode!=NULL);
			TopNest->Allow				 = ALLOW_VarDecl | ALLOW_Return | ALLOW_Cmd | ALLOW_Label;
			if( Pass==0 )
			{
				TopNode->iFirstProperty  = Class->Num;
				TopNode->ParentNest      = TopNest[-1].Link;
				TopNode->Next			 = PrevNode->ChildFunctions;
				PrevNode->ChildFunctions = TopNest->Link;
			}
			break;

		case NEST_Operator:
			checkState(ThisName!=NAME_None);
			checkState(PrevNode!=NULL);
			TopNest->Allow				 = ALLOW_VarDecl | ALLOW_Return;
			if( Pass==0 )
			{
				TopNode->iFirstProperty  = Class->Num;
				TopNode->ParentNest      = TopNest[-1].Link;
				TopNode->Next			 = PrevNode->ChildFunctions;
				PrevNode->ChildFunctions = TopNest->Link;
			}
			break;

		case NEST_If:
			checkState(ThisName==NAME_None);
			checkState(PrevNode!=NULL);
			TopNest->Allow = ALLOW_ElseIf | (PrevAllow & (ALLOW_Cmd|ALLOW_Label|ALLOW_Break|ALLOW_StateCmd|ALLOW_Return));
			break;

		case NEST_Loop:
			checkState(ThisName==NAME_None);
			checkState(PrevNode!=NULL);
			TopNest->Allow = ALLOW_Break | (PrevAllow & (ALLOW_Cmd|ALLOW_Label|ALLOW_Break|ALLOW_StateCmd|ALLOW_Return));
			break;

		case NEST_Switch:
			checkState(ThisName==NAME_None);
			checkState(PrevNode!=NULL);
			TopNest->Allow = ALLOW_Case | ALLOW_Default | (PrevAllow & (ALLOW_StateCmd|ALLOW_Return));
			break;

		case NEST_For:
			checkState(ThisName==NAME_None);
			checkState(PrevNode!=NULL);
			TopNest->Allow = ALLOW_Break | (PrevAllow & (ALLOW_Cmd|ALLOW_Label|ALLOW_Break|ALLOW_StateCmd|ALLOW_Return));
			break;

		case NEST_ForEach:
			checkState(ThisName==NAME_None);
			checkState(PrevNode!=NULL);
			TopNest->Allow = ALLOW_Break | (PrevAllow & (ALLOW_Cmd|ALLOW_Label|ALLOW_Break|ALLOW_Return));
			break;

		default:
			throwf( "Internal error in PushNest, type %i", NestType );
			break;
	}
	unguard;
}

//
// Decrease the nesting level and handle any errors that result.
//
void FScriptCompiler::PopNest( ENestType NestType, const char *Descr )
{
	guard(FScriptCompiler::PopNest);

	// Validate the nesting state.
	if( NestLevel <= 0 )
		throwf( "Unexpected '%s' at global scope", Descr, NestTypeName(NestType) );
	else if( NestType==NEST_None )
		NestType = TopNest->NestType;
	else if( TopNest->NestType!=NestType )
		throwf( "Unexpected '%s' in '%s' block", Descr, NestTypeName(TopNest->NestType) );

	if( Pass == 0 )
	{
		// Remember code position.
		if( NestType==NEST_State )
		{
			TopNode->Pos        = InputPos;
			TopNode->Line       = InputLine;
		}
		else if( NestType==NEST_Function || NestType==NEST_Operator )
		{
			TopNode->Pos        = InputPos;
			TopNode->Line       = InputLine;
			TopNode->LocalsSize = Class->Bins[PROPBIN_PerFunction]->Num;
		}
		else if( NestType!=NEST_Class )
		{
			appErrorf("Bad first pass NestType %i",NestType);
		}
	}
	else
	{
		// If ending a state, process labels.
		if( NestType==NEST_State )
		{
			// Emit stop command.
			*Script << EX_EndCode;

			// Write all labels to code.
			if( TopNest->LabelList )
			{
				// Make sure the label table entries are aligned.
				while( (Script->Num & 3) != 3 )
					*Script << EX_Nothing;
				*Script << EX_LabelTable;

				// Remember code offset.
				TopNode->CodeLabelOffset = Script->Num;

				// Write out all label entries.
				for( FLabelRecord *LabelRecord = TopNest->LabelList; LabelRecord; LabelRecord=LabelRecord->Next )
					*Script << *LabelRecord;

				// Write out empty record.
				FLabelEntry Entry(NAME_None,MAXWORD);
				*Script << Entry;
			}
		}
		else if( NestType==NEST_Function || NestType==NEST_Operator )
		{
			// Emit return.
			if( TopNode->iCode!=MAXWORD )
			{
				*Script << EX_Return;
				*Script << EX_EndCode;
			}
		}
		else if( NestType==NEST_Switch )
		{
			if( TopNest->Allow & ALLOW_Case )
			{
				// No default was specified, so emit end-of-case marker.
				EmitChainUpdate(TopNest);
				*Script << EX_Case;
				WORD W=MAXWORD; *Script << W;
			}

			// Here's the big end.
			TopNest->SetFixup(FIXUP_SwitchEnd,Script->Num);
		}
		else if( NestType==NEST_If )
		{
			if( MatchIdentifier("Else") )
			{
				// Send current code to the end of the if block.
				*Script << EX_Jump;
				EmitAddressToFixupLater(TopNest,FIXUP_IfEnd,NAME_None);

				// Update previous If's next-address.
				EmitChainUpdate(TopNest);

				if( MatchIdentifier("If") )
				{
					// ElseIf.
					CheckAllow( "'Else If'", ALLOW_ElseIf );

					// Jump to next evaluator if expression is false.
					*Script << EX_JumpIfNot;
					EmitAddressToChainLater(TopNest);

					// Compile boolean expr.
					RequireSymbol("(","'Else If'");
					CompileExpr( FProperty(CPT_Bool), "'Else If'" );
					RequireSymbol(")","'Else If'");

					// Handle statements.
					if( !MatchSymbol("{") )
					{
						CompileStatement();
						PopNest( NEST_If, "'ElseIf'" );
					}
				}
				else
				{
					// Else.
					CheckAllow( "'Else'", ALLOW_ElseIf );

					// Prevent further ElseIfs.
					TopNest->Allow &= ~(ALLOW_ElseIf);

					// Handle statements.
					if( !MatchSymbol("{") )
					{
						CompileStatement();
						PopNest( NEST_If, "'Else'" );
					}
				}
				return;
			}
			else
			{
				// Update last link, if any:
				EmitChainUpdate(TopNest);

				// Here's the big end.
				TopNest->SetFixup( FIXUP_IfEnd, Script->Num );
			}
		}
		else if( NestType==NEST_For )
		{
			// Compile the incrementor expression here.
			FRetryPoint Retry; InitRetry(Retry);
			PerformRetry(TopNest->ForRetry,0,1);		
				CompileAffector();
			PerformRetry(Retry,0,1);

			// Jump back to start of loop.
			*Script << EX_Jump;
			EmitAddressToFixupLater(TopNest,FIXUP_ForStart,NAME_None);

			// Here's the end of the loop.
			TopNest->SetFixup(FIXUP_ForEnd,Script->Num);
		}
		else if( NestType==NEST_ForEach )
		{
			// Perform next iteration.
			*Script << EX_IteratorNext;

			// Here's the end of the loop.
			TopNest->SetFixup( FIXUP_IteratorEnd, Script->Num );
			*Script << EX_IteratorPop;
		}
		else if( NestType==NEST_Loop )
		{
			if( MatchIdentifier("Until") )
			{
				// Jump back to start of loop.
				*Script << EX_JumpIfNot;
				EmitAddressToFixupLater( TopNest, FIXUP_LoopStart, NAME_None );

				// Compile boolean expression.
				RequireSymbol( "(", "'Until'" );
				CompileExpr( FProperty(CPT_Bool), "'Until'" );
				RequireSymbol( ")", "'Until'" );

				// Here's the end of the loop.
				TopNest->SetFixup( FIXUP_LoopEnd, Script->Num );
			}
			else
			{
				// Jump back to start of loop.
				*Script << EX_Jump;
				EmitAddressToFixupLater( TopNest, FIXUP_LoopStart, NAME_None );

				// Here's the end of the loop.
				TopNest->SetFixup( FIXUP_LoopEnd, Script->Num );
			}
		}

		// Perform all code fixups.
		for( FNestFixupRequest *Fixup = TopNest->FixupList; Fixup!=NULL; Fixup=Fixup->Next )
		{
			if( Fixup->Type == FIXUP_Label )
			{
				// Fixup a local label.
				for( FLabelRecord *LabelRecord = TopNest->LabelList; LabelRecord; LabelRecord=LabelRecord->Next )
				{
					if( LabelRecord->Name == Fixup->Name )
					{
						*(WORD *)&Script->Element(Fixup->iCode) = LabelRecord->iCode;
						break;
					}
				}
				if( LabelRecord == NULL )
					throwf( "Label '%s' not found in this block of code", Fixup->Name() );
			}
			else
			{
				// Fixup a code structure address.
				if( TopNest->Fixups[Fixup->Type] == MAXWORD )
					throwf( "Internal fixup error %i", Fixup->Type );
				*(WORD *)&Script->Element(Fixup->iCode) = TopNest->Fixups[Fixup->Type];
			}
		}
	}

	// Make sure there's no dangling chain.
	checkState(TopNest->iCodeChain==INDEX_NONE);

	// Pop the nesting level.
	NestType = TopNest->NestType;
	NestLevel--;
	TopNest--;
	TopNode	= TopNest->Node();

	// Update allow-flags.
	if( NestType == NEST_Function )
	{
		// Don't allow variable declarations after functions.
		TopNest->Allow &= ~(ALLOW_VarDecl);
	}
	else if( NestType == NEST_State )
	{
		// Don't allow variable declarations after states.
		TopNest->Allow &= ~(ALLOW_VarDecl);
	}
	unguard;
}

//
// Find the highest-up nest info of a certain type.
// Used (for example) for associating Break statements with their Loops.
//
INT FScriptCompiler::FindNest( ENestType NestType )
{
	guard(FScriptCompiler::FindNest);
	for( int i=NestLevel-1; i>0; i-- )
		if( Nest[i].NestType == NestType )
			return i;
	return -1;
	unguard;
}

/*-----------------------------------------------------------------------------
	Compiler directives.
-----------------------------------------------------------------------------*/

//
// Process a compiler directive.
//
void FScriptCompiler::CompileDirective()
{
	guard(FScriptCompiler::ProcessCompilerDirective);
	FToken Directive;

	if( !GetIdentifier(Directive) )
	{
		throwf( "Missing compiler directive after '#'" );
	}
	else if( Directive.Matches("Error") )
	{
		throwf( "#Error directive encountered" );
	}
	else if( Directive.Matches("Decompile") )
	{
		Class->ClassFlags |= CLASS_Decompile;
	}
	else if( Directive.Matches("Call") || Directive.Matches("AlwaysCall") )
	{
		FToken Identifier;
		if( !GetRawToken(Identifier) )
			throwf("'#Call': missing filename");

		if( Booting || Directive.Matches("AlwaysCall") )
		{
			UTextBuffer *Text = new( "Macro", Identifier.String, IMPORT_MakeUnique )UTextBuffer;
			if( Text )
			{
				Text->Lock( LOCK_Read );
				char Temp[256];
				const char *Data = &Text->Element(0);
				while( GetLINE (&Data,Temp,256)==0 )
				{
					GEditor->Bootstrapping++;
					GEditor->Exec( Temp );
					GEditor->Bootstrapping--;
				}
				Text->Unlock( LOCK_Read );
				//old: Text->Kill();
			}
			else throwf("Error opening file '%s'",Identifier.String);
		}
	}
	else if( Directive.Matches("Exec") || Directive.Matches("AlwaysExec") )
	{
		FToken Identifier;
		if( !GetRawToken(Identifier) )
			throwf("'#Exec': missing command line");

		if( Booting || Directive.Matches("AlwaysExec") )
		{
			GEditor->Bootstrapping++;
			GEditor->Exec( Identifier.String );
			GEditor->Bootstrapping--;
		}
	}
	else if( Directive.Matches("Exec") )
	{
		throwf("Exec: Not yet supported");
	}
	else
	{
		throwf( "Unrecognized compiler directive %s", Directive.Identifier );
	}

	// Skip to end of line.
	char c;
	while( !IsEOL( c=GetChar() ) );
	if( c==0 ) UngetChar();
	unguard;
}

/*-----------------------------------------------------------------------------
	Variable declaration parser.
-----------------------------------------------------------------------------*/

//
// Parse one variable declaration and set up its properties in VarProperty.
// Returns pointer to the class property if success, or NULL if there was no variable 
// declaration to parse. Called during variable 'Dim' declaration and function 
// parameter declaration.
//
// If you specify a hard-coded name, that name will be used as the variable name (this
// is used for function return values, which are automatically called "ReturnValue"), and
// a default value is not allowed.
//
int FScriptCompiler::GetVarType
(
	FProperty		&VarProperty,
	int				NoOverrides,
	EPropertyBin	Bin,
	const char		*Thing
)
{
	guard(FScriptCompiler::GetVarType);
	FProperty	TypeDefType;
	UClass		*TempClass;
	BOOL		IsVariable = 0;

	// Get flags.
	DWORD Flags=0;
	for( ; ; )
	{
		FToken Specifier; GetToken(Specifier);
		if( Specifier.Matches(NAME_Private) )
		{
			Flags      |= CPF_Private;
			IsVariable  = 1;
		}
		else if( Specifier.Matches(NAME_Const) )
		{
			Flags      |= CPF_Const;
			IsVariable  = 1;
		}
		else if( Specifier.Matches(NAME_Transient) )
		{
			Flags      |= CPF_Transient;
			IsVariable  = 1;
		}
		else if( Specifier.Matches(NAME_Intrinsic) )
		{
			Flags      |= CPF_Intrinsic;
			IsVariable  = 1;
		}
		else if( Specifier.Matches(NAME_Out) )
		{
			Flags      |= CPF_OutParm;
			IsVariable  = 1;
		}
		else if( Specifier.Matches(NAME_ExportObject) )
		{
			Flags      |= CPF_ExportObject;
			IsVariable  = 1;
		}
		else if( Specifier.Matches(NAME_Net) )
		{
			Flags     |= CPF_Net;
			IsVariable = 1;
		}
		else if( Specifier.Matches(NAME_NetSelf) )
		{
			Flags     |= CPF_NetSelf;
			IsVariable = 1;
		}
		else if( Specifier.Matches(NAME_Skip) )
		{
			Flags     |= CPF_SkipParm;
			IsVariable = 1;
		}
		else if( Specifier.Matches(NAME_Coerce) )
		{
			Flags     |= CPF_CoerceParm;
			IsVariable = 1;
		}
		else if( Specifier.Matches(NAME_Optional) )
		{
			Flags     |= CPF_OptionalParm;
			IsVariable = 1;
		}
		else
		{
			UngetToken(Specifier);
			break;
		}
	}

	// Get variable type.
	FToken VarType;
	if( !GetIdentifier(VarType,1) )
	{
		if( !Thing )
			return 0;
		throwf( "%s: Missing variable type", Thing );
	}
	else if( VarType.Matches(NAME_Enum) )
	{
		// Compile an Enum definition here.
		VarProperty      = FProperty(CPT_Byte);
		VarProperty.Enum = CompileEnum();
	}
	else if( VarType.Matches(NAME_Byte) )
	{
		// Intrinsic Byte type.
		VarProperty = FProperty(CPT_Byte);
	}
	else if( VarType.Matches(NAME_Int) )
	{
		// Intrinsic Int type.
		VarProperty = FProperty(CPT_Int);
	}
	else if( VarType.Matches(NAME_Bool) )
	{
		// Intrinsic Bool type.
		VarProperty = FProperty(CPT_Bool);
	}
	else if( VarType.Matches(NAME_Float) )
	{
		// Intrinsic Real type
		VarProperty = FProperty(CPT_Float);
	}
	else if( VarType.Matches(NAME_Name) )
	{
		// Intrinsic Name type.
		VarProperty = FProperty(CPT_Name);
	}
	else if( VarType.Matches(NAME_String) )
	{
		// Intrinsic String type.
		VarProperty = FProperty(CPT_String);

		if( !MatchSymbol("[") )
			throwf( "Missing string dimension, i.e. String[16]" );

		if( !VarType.Matches(NAME_String) )
			throwf( "Arrays dimension belongs after name, not type" );
		
		if( !GetConstInt(VarProperty.ElementSize) )
			throwf( "%s: Missing string size",Thing?Thing:"Declaration" );
		
		if( VarProperty.ElementSize<=0 || VarProperty.ElementSize>MAX_STRING_CONST_SIZE )
			throwf( "%s: Illegal string size %i",Thing?Thing:"Declaration",VarProperty.ElementSize );
		
		if( !MatchSymbol("]") )
			throwf( "%s: Missing ']'", Thing?Thing:"Declaration" );
	}
	else if( VarType.Matches(NAME_Vector) )
	{
		// Intrinsic Vector type.
		VarProperty = FProperty(CPT_Vector);
	}
	else if( VarType.Matches(NAME_Rotation) )
	{
		// Intrinsic Rotation type.
		VarProperty = FProperty(CPT_Rotation);
	}
	else if( (TempClass = new(VarType.Identifier,FIND_Optional)UClass) != NULL )
	{
		// Either an object or actor.
		if
		(	(Flags & CPF_Const)
		||	(TempClass->ClassFlags & CLASS_ScriptWritable)
		||	(Bin == PROPBIN_PerFunction) )
		{
			VarProperty = FProperty( CPT_Object, TempClass );
		}
		else throwf( "Variables of type %s must be Const", TempClass->GetName() );
	}
	else if
	(	(VarType.TokenName!=NAME_None)
	&&	(FindVariable(VarType.TokenName,(UClass*)NULL,&TypeDefType))
	&&	(TypeDefType.Type==CPT_EnumDef) )
	{
		// Got a valid enumeration.
		VarProperty      = FProperty( CPT_Byte );
		VarProperty.Enum = TypeDefType.Enum;
	}
	else if( !Thing )
	{
		// Not recognized.
		UngetToken(VarType);
		return 0;
	}
	else throwf( "Unrecognized type '%s'", VarType.Identifier );

	// Set info.
	VarProperty.Bin    = Bin;
	VarProperty.Flags |= Flags;

	// If editable but no category was specified, the category name is our class name.
	if( (VarProperty.Flags & CPF_Edit) && (VarProperty.Category==NAME_None) )
		VarProperty.Category = Class->GetFName();

	// Make sure the overrides are allowed here.
	if( NoOverrides && (VarProperty.Flags & ~CPF_ParmFlags))
		throwf( "Variable type modifiers (Static, Editable, etc) are not allowed here" );

	// Validate combinations.
	if( VarProperty.Type==CPT_Object && VarProperty.Bin==PROPBIN_PerClass && VarProperty.Class->IsChildOf("Actor") )
		throwf("Static actor variables are not allowed");
	if( (VarProperty.Flags & (CPF_Transient|CPF_Intrinsic)) && VarProperty.Bin!=PROPBIN_PerObject )
		throwf("Static and local variables may not be transient or intrinsic");

	return 1;
	unguard;
}

FProperty &FScriptCompiler::GetVarNameAndDim
(
	FProperty	&VarProperty,
	int			NoArrays,
	int			NoDefault,
	int			IsFunction,
	const char	*HardcodedName,
	const char	*Thing
)
{
	guard(FScriptCompiler::GetVarNameAndDim);

	// Get varible name.
	FToken VarToken;
	if( HardcodedName )
	{
		// Hard-coded variable name, such as with return value.
		VarToken.TokenType = TOKEN_Identifier;
		strcpy(VarToken.Identifier,HardcodedName);
	}
	else if( !GetIdentifier(VarToken) ) 
		throwf( "Missing variable name" );

	VarProperty.Name = FName( VarToken.Identifier, FNAME_Add );
	if( VarProperty.Name == NAME_None ) 
		throwf( "%s: '%s' is illegal",Thing,VarToken.Identifier );

	// Verify that the variable is unique within this scope.
	if( FindVariable( VarProperty.Name ) )
		throwf( "%s: '%s' already defined", Thing, VarToken.Identifier );

	// Get optional dimension immediately after name.
	if( MatchSymbol("[") )
	{
		if( NoArrays ) 
			throwf( "Arrays aren't allowed in this context" );

		if( VarProperty.Type == CPT_Bool )
			throwf( "Bool arrays are not allowed" );
		
		if( !GetConstInt(VarProperty.ArrayDim) )
			throwf( "%s %s: Bad or missing array size", Thing, VarToken.Identifier );
		
		if( VarProperty.ArrayDim<=1 && VarProperty.ArrayDim>MAX_ARRAY_SIZE )
			throwf( "%s %s: Illegal array size %i", Thing, VarToken.Identifier, VarProperty.ArrayDim );
		
		if( !MatchSymbol("]") )
			throwf( "%s %s: Missing ']'", Thing, VarToken.Identifier );
	}
	else if( MatchSymbol("(") )
		throwf( "Use [] for arrays, not ()" );

	// See if a default value is specified.
	BOOL Initialize=0, Initializing=0;
	if( MatchSymbol("=") )
	{
		if( NoDefault ) 
			throwf( "You don't need an '=' here" );

		if( VarProperty.Flags & CPF_Parm )
			throwf( "Function parameters can't be initialized (use 'optional')" );

		if( VarProperty.Bin == PROPBIN_PerFunction )
			throwf( "Local variables can't be initialized" );

		if( VarProperty.Flags & CPF_Edit )
			throwf( "%s %s: Can't specify default value for editable properties", Thing, VarToken.Identifier );

		if( VarProperty.ArrayDim > 1 )
			RequireSymbol("{","array initializer list");

		Initialize = Initializing = 1;
	}

	// Add property to class and get the property and data pointers.
	BYTE *Data;
	FProperty &Result = Class->AddProperty( VarProperty, Data );
	if( TopNode->NestType==NEST_Function || TopNode->NestType==NEST_Operator )
		TopNode->NumProperties++;

	// Init the property data with the init token.
	if( VarProperty.Bin != PROPBIN_PerFunction )
	{
		for( int i=0; i<VarProperty.ArrayDim; i++ )
		{
			if( Initializing )
			{
				// Get initializer token.
				FToken ConstToken;
				if( !GetToken( ConstToken, &VarProperty ) || ConstToken.TokenType!=TOKEN_Const )
					throwf( "%s %s: Bad default value", Thing, VarToken.Identifier );

				// Make a temporary property so array size mismatches don't confuse us.
				FProperty TempProperty = VarProperty;
				TempProperty.ArrayDim  = 1;

				// Perform any constant conversion.
				ConstToken.AttemptToConvertConstant( TempProperty );

				// Make sure constant token is ok.
				if( !TempProperty.MatchesType(ConstToken,0) )
					throwf( "%s %s: Type mismatch in default value", Thing, VarToken.Identifier );

				// Init the property's data with the initializer.
				FToken::InitPropertyData( &ConstToken, VarProperty, Data, i );

				// End of initializers?
				if( VarProperty.ArrayDim>1 && !MatchSymbol(",") )
					Initializing = 0;
			}
			else FToken::InitPropertyData( NULL, VarProperty, Data, i );
		}
	}

	// Initializer closing.
	if( Initialize && VarProperty.ArrayDim > 1 )
		RequireSymbol("}","array initializer list");

	return Result;
	unguard;
}

//
// Compile a variable assignment statement.
//
void FScriptCompiler::CompileAssignment( const char *Tag )
{
	guard(FScriptCompiler::CompileAssignment);

	// Set up.
	FRetryPoint LowRetry; InitRetry(LowRetry);
	FToken RequiredType, VarToken;

	// Compile l-value expression.
	CompileExpr( FProperty(CPT_None), "Assignment", &RequiredType );
	if( RequiredType.Type == CPT_None )
		throwf( "%s assignment: Missing left value", Tag );
	else if( !(RequiredType.Flags & CPF_OutParm) )
		throwf( "%s assignment: Left value is not a variable", Tag );
	else if( !MatchSymbol("=") )
		throwf( "%s: Missing '=' after %s", Tag );

	// Emit let.
	FRetryPoint HighRetry; InitRetry(HighRetry);
	EmitLet(RequiredType,Tag);

	// Switch around.
	CodeSwitcheroo(LowRetry,HighRetry);

	// Compile right value.
	RequiredType.Flags &= ~CPF_OutParm;
	CompileExpr( RequiredType, Tag );

	unguard;
}

//
// Try to compile an affector expression or assignment.
//
void FScriptCompiler::CompileAffector()
{
	guard(FScriptCompiler::CompileAffector);

	// Try to compile an affector expression or assignment.
	FRetryPoint LowRetry; InitRetry(LowRetry);
	GotAffector=0;

	// Try to compile an expression here.
	FProperty RequiredType( CPT_None );
	FToken ResultType;
	if( CompileExpr( RequiredType, NULL, &ResultType ) != 1 )
	{
		FToken Token;
		GetToken(Token);
		throwf( "'%s': Bad command or expression", Token.Identifier );
	}

	// Did we get a function call or a varible assignment?
	if( MatchSymbol("=") )
	{
		// Variable assignment.
		if( !(ResultType.Flags & CPF_OutParm) )
			throwf( "'=': Left value is not a variable" );

		// Compile right value.
		RequiredType        = ResultType;
		RequiredType.Flags &= ~CPF_OutParm;
		CompileExpr( RequiredType, "'='" );

		// Emit the let.
		FRetryPoint HighRetry; InitRetry(HighRetry);
		EmitLet( ResultType, "'='" );
		CodeSwitcheroo(LowRetry,HighRetry);
	}
	else if( GotAffector )
	{
		// Function call or operators with outparms.
	}
	else if( ResultType.Type != CPT_None )
	{
		// Whatever expression we parsed had no effect.
		FToken Token;
		GetToken(Token);
		throwf( "'%s': Expression has no effect", Token.Identifier );
	}
	else
	{
		// Didn't get anything, so throw an error at the appropriate place.
		FToken Token;
		GetToken(Token);
		throwf( "'%s': Bad command or expression", Token.Identifier );
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Statement compiler.
-----------------------------------------------------------------------------*/

//
// Compile a declaration in Token. Returns 1 if compiled, 0 if not.
//
int FScriptCompiler::CompileDeclaration( FToken &Token, BOOL &NeedSemicolon )
{
	guard(FScriptCompiler::CompileDeclaration);

	if( Token.Matches(NAME_Class) && (TopNest->Allow & ALLOW_Class) )
	{
		// Start of a class block.
		guard(Class);
		CheckAllow("'Class'",ALLOW_Class);

		// Class name.
		if( !GetToken(Token) )
			throwf( "Missing class name" );

		if( !Token.Matches(Class->GetName()) )
			throwf( "Class must be named %s, not %s", Class->GetName(), Token.Identifier );

		// Get parent class.
		if( MatchIdentifier("Expands") )
		{
			if( !GetIdentifier(Token) )
				throwf( "Missing parent class name" );

			UClass *TempClass = new(Token.Identifier,FIND_Optional)UClass;

			if( !TempClass )
				throwf( "Parent class %s not found", Token.Identifier );

			if( Class->ParentClass == NULL )
				Class->ParentClass = TempClass;
			else if( Class->ParentClass != TempClass )
				throwf( "%s's parent class must be %s, not %s", Class->GetName(), Class->ParentClass->GetName(), TempClass->GetName() );

			// Copy properties from parent class.
			Class->AddParentProperties();
		}
		else if( Class->ParentClass )
			throwf( "Class: missing 'Expands %s'", Class->ParentClass->GetName() );

		// Push the class nesting.
		PushNest( NEST_Class, Class->GetName() );

		// Class attributes.
		FToken Token;
		for( ; ; )
		{
			GetToken(Token);
			if( Token.Matches(NAME_Intrinsic) )
			{
				// Note that this class has C++ code dependencies.
				Class->ClassFlags |= CLASS_Intrinsic;
			}
			else if( Token.Matches(NAME_HideParent) )
			{
				// Hide all editable properties.
				Class->ClassFlags |= CLASS_NoEditParent;
			}
			else if( Token.Matches(NAME_Abstract) )
			{
				// Hide all editable properties.
				Class->ClassFlags |= CLASS_Abstract;
			}
			else if( Token.Matches(NAME_Package) )
			{
				// This class belongs to a package.
				RequireSymbol("(","'Package'");
				FToken Token;
				if( !GetIdentifier(Token) )
					throwf("'Standard': Missing package name");
				Class->PackageName = FName( Token.Identifier, FNAME_Add );
				RequireSymbol(")","'Standard'");
			}
			else if( Token.Matches(NAME_Guid) )
			{
				// Get the class's GUID.
				RequireSymbol("(","'Guid'");
					GetConstInt(Class->ResGUID[0]);
				RequireSymbol(",","'Guid'");
					GetConstInt(Class->ResGUID[1]);
				RequireSymbol(",","'Guid'");
					GetConstInt(Class->ResGUID[2]);
				RequireSymbol(",","'Guid'");
					GetConstInt(Class->ResGUID[3]);
				RequireSymbol(")","'Guid'");
			}
			else if( Token.Matches(NAME_Transient) )
			{
				// Transient class.
				Class->ClassFlags |= CLASS_Transient;
			}
			else if( Token.Matches(NAME_ScriptConst) )
			{
				// Scripts can only have global variables of this class which are const.
				Class->ClassFlags &= ~CLASS_ScriptWritable;
			}
			else
			{
				UngetToken(Token);
				break;
			}
		}

		// Copy parent's stack node links if parent is scripted.
		if( Class->ParentClass && Class->ParentClass->StackTree )
		{
			checkState(Class->ParentClass->StackTree!=NULL);
			TopNode->ParentItem     = FStackNodePtr( Class->ParentClass, 0 );
			TopNode->ChildFunctions	= TopNode->ParentItem->ChildFunctions;
			TopNode->ChildStates	= TopNode->ParentItem->ChildStates;
		}
		unguard;
	}
	else if
	(	Token.Matches(NAME_Function)
	||	Token.Matches(NAME_Operator)
	||	Token.Matches(NAME_PreOperator) 
	||	Token.Matches(NAME_PostOperator) 
	||	Token.Matches(NAME_Intrinsic) 
	||	Token.Matches(NAME_Final) 
	||	Token.Matches(NAME_Private) 
	||	Token.Matches(NAME_Latent)
	||	Token.Matches(NAME_Iterator)
	||	Token.Matches(NAME_Singular)
	)
	{
		// Function or operator.
		guard(Function/Operator);
		FRetryPoint FuncNameRetry;
		FFuncInfo FuncInfo;

		// Process all specifiers.
		for( ;; )
		{
			InitRetry(FuncNameRetry);
			if( Token.Matches(NAME_Function) )
			{
				// Get function name.
				FuncInfo.NestType		= NEST_Function;
				FuncInfo.NoDefaults		= 0;
				CheckAllow( "'Function'",ALLOW_Function);
			}
			else if( Token.Matches(NAME_Operator) )
			{
				// Get operator name or symbol.
				CheckAllow("'Operator'",ALLOW_Function);
				FuncInfo.NestType		= NEST_Operator;
				FuncInfo.NoDefaults		= 1;
				FuncInfo.ExpectParms    = 3;

				if( !MatchSymbol("(") )
					throwf( "Missing '(' and precedence after 'Operator'" );
				else if( !GetConstInt(FuncInfo.Precedence) )
					throwf( "Missing precedence value" );
				else if( FuncInfo.Precedence<0 || FuncInfo.Precedence>255 )
					throwf( "Bad precedence value" );
				else if( !MatchSymbol(")") )
					throwf( "Missing ')' after operator precedence" );
			}
			else if( Token.Matches(NAME_PreOperator) )
			{
				// Get operator name or symbol.
				CheckAllow("'PreOperator'",ALLOW_Function);
				FuncInfo.NestType		 = NEST_Operator;
				FuncInfo.NoDefaults		 = 1;
				FuncInfo.ExpectParms     = 2;
				FuncInfo.StackNodeFlags |= SNODE_PreOperatorFunc;
			}
			else if( Token.Matches(NAME_PostOperator) )
			{
				// Get operator name or symbol.
				CheckAllow("'PostOperator'",ALLOW_Function);
				FuncInfo.NestType		= NEST_Operator;
				FuncInfo.NoDefaults		= 1;
				FuncInfo.ExpectParms    = 2;
			}
			else if( Token.Matches(NAME_Intrinsic) )
			{
				// Intrinsic override.
				FuncInfo.StackNodeFlags |= SNODE_IntrinsicFunc;

				// Get internal id.
				if( !MatchSymbol("(") )
					throwf( "Missing '(' and internal id after 'Intrinsic'" );
				else if( !GetConstInt(FuncInfo.iIntrinsic) )
					throwf( "Missing intrinsic id" );
				else if( !MatchSymbol(")") )
					throwf( "Missing ')' after internal id" );
			}
			else if( Token.Matches(NAME_Iterator) )
			{
				FuncInfo.StackNodeFlags |= SNODE_IteratorFunc;
			}
			else if( Token.Matches(NAME_Singular) )
			{
				FuncInfo.StackNodeFlags |= SNODE_SingularFunc;
			}
			else if( Token.Matches(NAME_Latent) )
			{
				// This is a slow intrinsic function.
				FuncInfo.StackNodeFlags |= SNODE_LatentFunc;
			}
			else if( Token.Matches(NAME_Final) )
			{
				// This is a final (prebinding, non-overridable) function or operator.
				FuncInfo.StackNodeFlags |= SNODE_FinalFunc;
			}
			else if( Token.Matches(NAME_Private) )
			{
				// This function is not script-callable.
				FuncInfo.StackNodeFlags |= SNODE_PrivateFunc;
			}
			else break;
			GetToken(Token);
		}
		UngetToken(Token);

		// Make sure we got a function.
		if( FuncInfo.NestType == NEST_None )
			throwf( "Missing 'Function' or 'Operator'" );

		// Warn if intrinsic doesn't actually exist.
		if( FuncInfo.iIntrinsic!=0 )
			if( FuncInfo.iIntrinsic<EX_FirstIntrinsic || FuncInfo.iIntrinsic>EX_Max || GIntrinsics[FuncInfo.iIntrinsic]==execUndefined )
				debugf( "Warning: Bad intrinsic id %i\r\n",FuncInfo.iIntrinsic);//!!

		// Get return type.
		FRetryPoint Start; InitRetry(Start);
		FProperty ReturnType;
		BOOL HasReturnValue = GetVarType( ReturnType, 1, PROPBIN_PerFunction, NULL );
		if( HasReturnValue && PeekSymbol("(") )
		{
			// We were fooled -- it's a function name not a return type.
			HasReturnValue = 0;
			PerformRetry(Start);
		}

		// Get function or operator name.
		if( !GetIdentifier(FuncInfo.Function) && (FuncInfo.NestType==NEST_Function || !GetSymbol(FuncInfo.Function)) )
			throwf("Missing %s name",NestTypeName(FuncInfo.NestType));

		// Validate flag combinations.
		if( !(FuncInfo.StackNodeFlags & SNODE_IntrinsicFunc) )
		{
			if( FuncInfo.StackNodeFlags & SNODE_LatentFunc  )
				throwf( "Only intrinsic functions may use 'Latent'" );
			if( FuncInfo.StackNodeFlags & SNODE_IteratorFunc )
				throwf( "Only intrinsic functions may use 'Iterator'" );
		}

		// Allocate local property frame, push nesting level and verify 
		// uniqueness at this scope level.
		PushNest( FuncInfo.NestType, FuncInfo.Function.Identifier );
		TopNode->StackNodeFlags |= FuncInfo.StackNodeFlags;
		TopNode->OperPrecedence  = FuncInfo.Precedence;
		TopNode->iIntrinsic      = FuncInfo.iIntrinsic;

		// Get parameter list.
		if( !MatchSymbol("(") )
		{
			throwf( "Bad %s definition", NestTypeName(FuncInfo.NestType) );
		}
		else if( !MatchSymbol(")") )
		{
			int Optional=0;
			do
			{
				// Get parameter type.
				FProperty Property;
				GetVarType( Property, 1, PROPBIN_PerFunction, "Function parameter" );

				// Note this is a function parameter.
				Property.Flags |= CPF_Parm;
				TopNode->NumParms++;

				// Get parameter info.
				GetVarNameAndDim( Property, 0, FuncInfo.NoDefaults, 1, NULL, "Function parameter" );

				// Check operator parameters.
				if( FuncInfo.NestType==NEST_Operator && (Property.Flags & ~CPF_ParmFlags) )
					throwf( "Operator parameters may not have modifiers" );
				else if( Property.Type==CPT_Bool && (Property.Flags & CPF_OutParm) )
					throwf( "Booleans may not be out parameters" );
				else if( (Property.Flags & CPF_SkipParm) && (TopNode->iIntrinsic==0 || TopNode->NestType!=NEST_Operator || TopNode->NumParms!=2) )
					throwf( "Only parameter 2 of intinsic operators may be 'Skip'" );

				// Handle optional parameters.
				if( Property.Flags & CPF_OptionalParm )
				{
					if( Property.Flags & CPF_OutParm )
						throwf( "Out parameters may not be optional" );
					Optional = 1;
				}
				else if( Optional ) throwf( "After an optional parameters, all other parmeters must be optional" );
			} while( MatchSymbol(",") );

			// Get closing paren.
			if( !MatchSymbol(")") )
				throwf( "Missing ')' after parameter list" );
		}

		// Get return type, if any.
		if( HasReturnValue )
		{
			ReturnType.Flags |= CPF_Parm | CPF_OutParm | CPF_ReturnParm;
			TopNode->NumParms++;
			FProperty &ReturnProperty = GetVarNameAndDim( ReturnType, 1, 0, 1, "ReturnValue", "Function return type" );
			TopNode->CodeLabelOffset = ReturnProperty.Offset;
		}

		// Set ParmsSize.
		TopNode->ParmsSize = Class->Bins[PROPBIN_PerFunction]->Num;

		// Check overflow.
		if( TopNode->NumProperties > MAX_FUNC_PARMS )
			throwf("'%s': too many parameters",TopNode->Name());

		// For operators, verify that: the operator is either binary or unary, there is
		// a return value, and all parameters have the same type as the return value.
		if( FuncInfo.NestType == NEST_Operator )
		{
			int i = TopNode->iFirstProperty;
			int n = TopNode->NumParms;
			if( n != FuncInfo.ExpectParms )
				throwf( "%s must have %i parameters", NestTypeName(FuncInfo.NestType), FuncInfo.ExpectParms-1 );

			if( !(Class->Element(i + n - 1).Flags & CPF_ReturnParm) )
				throwf( "Operator must have a return value" );

			if( !(FuncInfo.StackNodeFlags & SNODE_FinalFunc) )
				throwf( "Operators must be declared as 'Final'" );
		}

		// Detect whether the function is being defined or declared.
		if( PeekSymbol(";") )
		{
			// Function is just being declared, not defined.
			checkState( (TopNode->StackNodeFlags & SNODE_DefinedFunc)==0 );
		}
		else
		{
			// Function is being defined.
			TopNode->StackNodeFlags |= SNODE_DefinedFunc;
			if( TopNode->StackNodeFlags & SNODE_IntrinsicFunc )
				throwf( "Intrinsic functions may only be declared, not defined" );

			// Require bracket.
			RequireSymbol("{",NestTypeName(FuncInfo.NestType));
			NeedSemicolon=0;
		}

		// See if this function name begins with 'Server', which requires a special
		// parm format.
		BOOL IsServerVariation = strnicmp(TopNode->Name(),"SERVER",6)==0;

		// Verify parameter list and return type compatibility within the 
		// function, if any, that it overrides.
		for( int i=NestLevel-2; i>=1; i-- )
		{
			checkState(Nest[i].Link.iNode!=INDEX_NONE);//!!
			FStackNodePtr Link = Nest[i].Node()->ChildFunctions;
			while( Link.Class )
			{
				FStackNode *Node = &*Link;
				INT IsServer     = IsServerVariation && stricmp(Node->Name(),"SERVER")==0;

				// If the other function's name matches this one's, process it.
				if
				(
					(Node->Name==TopNode->Name || IsServer)
				&&	(Node!=TopNode)
				&&	(((Node->StackNodeFlags ^ TopNode->StackNodeFlags) & SNODE_PreOperatorFunc)==0)
				)
				{
					// Prevent mismatch of underlying type (Function or Operator).
					if( Node->NestType != TopNode->NestType )
						throwf( "Redefinition of '%s' mismatches '%s' with '%s'",
							FuncInfo.Function.Identifier, NestTypeName((ENestType)Node->NestType), NestTypeName((ENestType)TopNode->NestType) );

					// Check precedence.
					if( Node->OperPrecedence!=TopNode->OperPrecedence && Node->NumParms==TopNode->NumParms )
						throwf( "Overloaded operator differs in precedence" );

					// See if all parameters match.
					//AddResultText("%s::%s %i-%i\r\n",Link->Class->GetName(),Node->Name(),NumParms1,Node.NumProperties);
					char *Differ = IsServer ? "standard 'Server' function syntax" : "original";
					if
					(	(TopNode->NumParms != Node->NumParms)
					||	((TopNode->NumParms>0) && 
						((Class(TopNode->iFirstProperty+TopNode->NumParms-1).Flags ^ Link.Class->Element(Node->iFirstProperty+Node->NumParms-1).Flags) & CPF_ReturnParm)!=0) )
						throwf( "Redefinition of '%s %s' differs from %s", NestTypeName(FuncInfo.NestType), FuncInfo.Function.Identifier, Differ );

					// Check all individual parameters.
					for( int j=0; j<TopNode->NumParms; j++ )
					{
						if( !Class(TopNode->iFirstProperty + j).MatchesType(Link.Class->Element(Node->iFirstProperty + j), 1) )
						{
							if( Class(TopNode->iFirstProperty + j).Flags & CPF_ReturnParm )
								throwf( "Redefinition of %s %s differs only by return type", NestTypeName(FuncInfo.NestType), FuncInfo.Function.Identifier );
							else if( FuncInfo.NestType != NEST_Operator )
								throwf( "Redefinition of '%s %s' differs from %s", NestTypeName(FuncInfo.NestType), FuncInfo.Function.Identifier, Differ );
							goto Next;
						}
					}

					// Balk if required specifiers differ.
					if( (Node->StackNodeFlags&SNODE_FuncOverrideMatch) != (FuncInfo.StackNodeFlags&SNODE_FuncOverrideMatch) )
						throwf( "Function '%s' specifiers differ from original", Node->Name() );

					// Are we overriding a function?
					if( (i == NestLevel-2) && (Class == Link.Class) )
					{
						// Duplicate.
						if( !IsServer )
						{
							PerformRetry(FuncNameRetry);
							throwf( "Duplicate function '%s'", Node->Name() );
						}
					}
					else
					{
						// Overriding an existing function.
						if( Node->StackNodeFlags & SNODE_FinalFunc )
						{
							PerformRetry(FuncNameRetry);
							throwf( "%s: Can't override a 'Final' function", Node->Name() );
						}
					}

					// Here we have found the original.
					TopNode->ParentItem = Link;
					goto Found;
				}
				Next:
				Link = Link->Next;
			}
		}
		Found:

		// If declaring a function, end the nesting.
		if( !(TopNode->StackNodeFlags & SNODE_DefinedFunc) )
			PopNest(FuncInfo.NestType,NestTypeName(FuncInfo.NestType));

		unguard;
	}
	else if
	(	Token.Matches(NAME_Var)
	||	Token.Matches(NAME_Static) 
	||	Token.Matches(NAME_Local) 
	)
	{
		// Variable definition.
		guard(Dim);

		EPropertyBin	Bin;
		int				NoOverrides;

		if( Token.Matches(NAME_Static) )
		{
			CheckAllow("'Static'",ALLOW_VarDecl);
			if( TopNest->NestType != NEST_Class )
				throwf( "Static variables are only allowed at class scope" );

			// Declaring per-class variables.
			NoOverrides			= 0;
			Bin					= PROPBIN_PerClass;
		}
		else if( Token.Matches(NAME_Var) )
		{
			// Declaring per-actor variables.
			CheckAllow("'Var'",ALLOW_VarDecl);
			if( TopNest->NestType != NEST_Class )
				throwf( "Actor variables are only allowed at class scope" );
			NoOverrides			= 0;
			Bin					= PROPBIN_PerObject;
		}
		else
		{
			// Declaring local variables.
			CheckAllow("'Local'",ALLOW_VarDecl);
			if( TopNest->NestType == NEST_Class )
				throwf( "Local variables are only allowed in functions" );
			NoOverrides			= 1;
			Bin					= PROPBIN_PerFunction;
		}

		// Get category, if any.
		FName EdCategory = NAME_None;
		DWORD EdFlags    = 0;
		if( MatchSymbol("(") )
		{
			EdFlags |= CPF_Edit;

			// Get optional property editing category.
			FToken Category;
			if( GetIdentifier(Category,1) )	EdCategory = FName( Category.Identifier, FNAME_Add );
			else							EdCategory = Class->GetFName();
			
			if( !MatchSymbol(")") )
				throwf( "Missing ')' after editable category" );
		}
		
		FProperty OriginalProperty;
		GetVarType( OriginalProperty, NoOverrides, Bin, "Variable declaration" );
		OriginalProperty.Flags     |= EdFlags;
		OriginalProperty.Category   = EdCategory;

		// Validate.
		if( OriginalProperty.Flags & CPF_ParmFlags )
			throwf( "Illegal type modifiers in variable" );

		// Process all variables of this type.
		do
		{
			FProperty Property = OriginalProperty;
			GetVarNameAndDim( Property, 0, 0, 0, NULL, "Variable declaration" );
		} while( MatchSymbol(",") );
		unguard;
	}
	else if( Token.Matches(NAME_Enum) )
	{
		// Enumeration definition.
		guard(Enum);
		CheckAllow("'Enum'",ALLOW_VarDecl);

		// Compile enumeration.
		CompileEnum();

		unguard;
	}
	else if
	(	Token.Matches(NAME_State)
	||	Token.Matches(NAME_Auto) )
	{
		// State block.
		guard(State);
		checkState(TopNode!=NULL);
		CheckAllow("'State'",ALLOW_State);
		DWORD StackNodeFlags=0, GotState=0;

		// Process all specifiers.
		for( ;; )
		{
			if( Token.Matches(NAME_State) )
			{
				GotState=1;
				if( MatchSymbol("(") )
					RequireSymbol( ")", "'State'" );
				StackNodeFlags |= SNODE_EditableState;
			}
			else if( Token.Matches(NAME_Auto) )
			{
				StackNodeFlags |= SNODE_AutoState;
			}
			else
			{
				UngetToken(Token);
				break;
			}
			GetToken(Token);
		}
		if( !GotState )
			throwf( "Missing 'State'" );

		FToken NameToken;
		if( !GetIdentifier(NameToken) )
			throwf( "Missing state name" );

		// Find parent state and make sure this state doesn't duplicate an existing state on the same nest level.
		for( FStackNodePtr Link = TopNode->ChildStates; Link.Class; Link = Link->Next )
		{
			if( stricmp( Link->Name(), NameToken.Identifier ) == 0 )
			{
				if( Link.Class == Class )
				{
					// This is a duplicate in this class.
					throwf( "Duplicate state '%s'", NameToken.Identifier );
				}
				else
				{
					// This is an override of a state in a parent class.
					break;
				}
			}
		}

		// Check for 'expands' keyword.
		if( MatchIdentifier("Expands") )
		{
			FToken ParentToken;
			if( Link.Class != NULL )
				throwf( "'Expands' not allowed here: state '%s' overrides version in parent class", NameToken.Identifier );
			if( !GetIdentifier(ParentToken) )
				throwf( "Missing parent state name" );

			// Find the overridden state.
			for( Link = TopNode->ChildStates; Link.Class; Link = Link->Next )
				if( stricmp( Link->Name(), ParentToken.Identifier ) == 0 )
					break;

			if( !Link.Class )
				throwf( "'Expands': Parent state '%s' not found", ParentToken.Identifier );
		}

		// Begin the state block.
		PushNest( NEST_State, NameToken.Identifier );
		TopNode->StackNodeFlags |= StackNodeFlags;
		RequireSymbol("{","'State'");
		NeedSemicolon=0;

		// Copy overridden info over.
		if( Link.Class )
		{
			// Copy the overridden stack node's child, next, and state links to this node.
			TopNode->ChildFunctions = Link->ChildFunctions;
			TopNode->ChildStates    = Link->ChildStates;
			TopNode->ParentItem     = Link;
		}
		unguard;
	}
	else if( Token.Matches(NAME_Ignores) )
	{
		// Probes to ignore in this state.
		CheckAllow("'Ignores'",ALLOW_Ignores);
		for( ; ; )
		{
			FToken IgnoreFunction;
			if( !GetToken(IgnoreFunction) )
				throwf( "'Ignores': Missing probe function name" );
			else if( IgnoreFunction.TokenName == NAME_None
			||	IgnoreFunction.TokenName.GetIndex() < PROBE_MIN || IgnoreFunction.TokenName.GetIndex() >= PROBE_MAX )
				throwf( "'Ignores': '%s' is not a probe function", IgnoreFunction.TokenName() );

			// Unmask the probe bit.
			checkState(TopNest->NestType==NEST_State || TopNest->NestType==NEST_Class);
			TopNode->IgnoreMask &= ~((QWORD)1 << (IgnoreFunction.TokenName.GetIndex() - PROBE_MIN));

			// More?
			if( !MatchSymbol(",") )
				break;
		}
	}
	else if( Token.Matches("#") )
	{
		// Compiler directive.
		guard(Directive);
		CompileDirective();
		NeedSemicolon=0;
		unguard;
	}
	else
	{
		// Not a declaration.
		return 0;
	}
	return 1;
	unguard;
}

//
// Compile a command in Token. Handles any errors that may occur.
//
void FScriptCompiler::CompileCommand( FToken &Token, BOOL &NeedSemicolon )
{
	guard(FScriptCompiler::CompileCommand);
	checkState(Pass==1);
	
	if( Token.Matches(NAME_Switch) )
	{
		// Switch.
		guard(Switch);
		CheckAllow("'Switch'",ALLOW_Cmd);
		PushNest(NEST_Switch,"");

		// Compile the select-expression.
		*Script << EX_Switch;
		FRetryPoint LowRetry; InitRetry(LowRetry);
		CompileExpr( FProperty(CPT_None), "'Switch'", &TopNest->SwitchType );
		if( TopNest->SwitchType.ArrayDim != 1 )
			throwf( "Can't switch on arrays" );
		FRetryPoint HighRetry; InitRetry(HighRetry);
		if( TopNest->SwitchType.Type == CPT_String )
		{
			BYTE B=0;
			*Script << B;
		}
		else
		{
			EmitSize(TopNest->SwitchType.Size(),"'Switch'");
		}
		CodeSwitcheroo(LowRetry,HighRetry);
		TopNest->SwitchType.Flags &= ~(CPF_OutParm);

		// Get bracket.
		RequireSymbol("{","'Switch'");
		NeedSemicolon=0;

		unguard;
	}
	else if( Token.Matches(NAME_Case) )
	{
		guard(Case);
		CheckAllow("'Class'",ALLOW_Case);

		// Update previous Case's chain address.
		EmitChainUpdate(TopNest);

		// Insert this case statement and prepare to chain it to the next one.
		*Script << EX_Case;
		EmitAddressToChainLater(TopNest);
		CompileExpr( TopNest->SwitchType,"'Case'");
		RequireSymbol(":","'Case'");
		NeedSemicolon=0;

		TopNest->Allow |= ALLOW_Cmd | ALLOW_Label | ALLOW_Break;
		unguard;
	}
	else if( Token.Matches(NAME_Default) )
	{
		// Default case.
		guard(Default);
		CheckAllow("'Default'",ALLOW_Case);

		// Update previous Case's chain address.
		EmitChainUpdate(TopNest);

		// Emit end-of-case marker.
		*Script << EX_Case;
		WORD W=MAXWORD; *Script << W;
		RequireSymbol(":","'Default'");
		NeedSemicolon=0;

		// Don't allow additional Cases after Default.
		TopNest->Allow &= ~ALLOW_Case;
		TopNest->Allow |=  ALLOW_Cmd | ALLOW_Label | ALLOW_Break;
		unguard;
	}
	else if( Token.Matches(NAME_Return) )
	{
		// Only valid from within a function or operator.
		guard(Return);
		CheckAllow( "'Return'", ALLOW_Return );
		for( int i=NestLevel-1; i>0; i-- )
		{
			if( Nest[i].NestType==NEST_Function || Nest[i].NestType==NEST_Operator )
				break;
			else if( Nest[i].NestType==NEST_ForEach )
				*Script << EX_IteratorPop;
		}
		if( i <= 0 )
			throwf( "Internal consistency error on 'Return'" );
		for( int j=0; j<Nest[i].Node()->NumParms; j++ )
		{
			FProperty Property = Class( j + Nest[i].Node()->iFirstProperty );
			if( Property.Flags & CPF_ReturnParm )
			{
				// Emit an assignment to the variable.
				EmitLet(Property,"'Return'");

				// Emit variable info.
				EmitSimpleVariable( Property, Property.Offset, 0 );

				// Return expression.
				Property.Flags &= ~CPF_OutParm;
				CompileExpr( Property, "'Return'" );
				break;
			}
		}

		// Emit the return.
		*Script << EX_Return;
		unguard;
	}
	else if( Token.Matches(NAME_If) )
	{
		// If.
		guard(If);
		CheckAllow( "'If'", ALLOW_Cmd );
		PushNest( NEST_If, "" );

		// Jump to next evaluator if expression is false.
		*Script << EX_JumpIfNot;
		EmitAddressToChainLater(TopNest);

		// Compile boolean expr.
		RequireSymbol("(","'If'");
		CompileExpr( FProperty(CPT_Bool), "'If'" );
		RequireSymbol(")","'If'");

		// Handle statements.
		NeedSemicolon=0;
		if( !MatchSymbol("{") )
		{
			CompileStatement();
			PopNest( NEST_If, "'If'" );
		}

		unguard;
	}
	else if( Token.Matches(NAME_While) )
	{
		guard(While);
		CheckAllow( "'While'", ALLOW_Cmd );
		PushNest( NEST_Loop, "" );

		// Here is the start of the loop.
		TopNest->SetFixup(FIXUP_LoopStart,Script->Num);

		// Evaluate expr and jump to end of loop if false.
		*Script << EX_JumpIfNot; 
		EmitAddressToFixupLater(TopNest,FIXUP_LoopEnd,NAME_None);

		// Compile boolean expr.
		RequireSymbol("(","'While'");
		CompileExpr( FProperty(CPT_Bool), "'While'" );
		RequireSymbol(")","'While'");

		// Handle statements.
		NeedSemicolon=0;
		if( !MatchSymbol("{") )
		{
			CompileStatement();
			PopNest( NEST_Loop, "'While'" );
		}

		unguard;
	}
	else if(Token.Matches(NAME_Do))
	{
		guard(Do);
		CheckAllow( "'Do'", ALLOW_Cmd );
		PushNest(NEST_Loop,"");

		TopNest->SetFixup(FIXUP_LoopStart,Script->Num);

		// Handle statements.
		NeedSemicolon=0;
		if( !MatchSymbol("{") )
		{
			CompileStatement();
			PopNest( NEST_Loop, "'Do'" );
		}

		unguard;
	}
	else if( Token.Matches(NAME_Break) )
	{
		guard(Break);
		CheckAllow( "'Break'", ALLOW_Break );
		
		// Find the nearest For or Loop.
		INT iNest = FindNest(NEST_Loop);
		iNest     = Max(iNest,FindNest(NEST_For    ));
		iNest     = Max(iNest,FindNest(NEST_ForEach));
		iNest     = Max(iNest,FindNest(NEST_Switch ));
		checkState(iNest>0);

		// Jump to the loop's end.
		*Script << EX_Jump;
		if     ( Nest[iNest].NestType == NEST_Loop    ) EmitAddressToFixupLater( &Nest[iNest], FIXUP_LoopEnd,     NAME_None );
		else if( Nest[iNest].NestType == NEST_For     ) EmitAddressToFixupLater( &Nest[iNest], FIXUP_ForEnd,      NAME_None );
		else if( Nest[iNest].NestType == NEST_ForEach ) EmitAddressToFixupLater( &Nest[iNest], FIXUP_IteratorEnd, NAME_None );
		else                                            EmitAddressToFixupLater(TopNest,FIXUP_SwitchEnd,NAME_None);

		unguard;
	}
	else if(Token.Matches(NAME_For))
	{
		guard(For);
		CheckAllow( "'For'", ALLOW_Cmd );
		PushNest(NEST_For,"");

		// Compile for parms.
		RequireSymbol("(","'For'");
			CompileAssignment("'For'");
		RequireSymbol(";","'For'");
			TopNest->SetFixup(FIXUP_ForStart,Script->Num);
			*Script << EX_JumpIfNot; 
			EmitAddressToFixupLater(TopNest,FIXUP_ForEnd,NAME_None);
			CompileExpr( FProperty(CPT_Bool), "'For'" );
		RequireSymbol(";","'For'");
			// Skip the increment expression text but not code.
			InitRetry(TopNest->ForRetry);
			CompileAffector();
			PerformRetry(TopNest->ForRetry,1,0);
		RequireSymbol(")","'For'");

		// Handle statements.
		NeedSemicolon=0;
		if( !MatchSymbol("{") )
		{
			CompileStatement();
			PopNest( NEST_For, "'If'" );
		}
		unguard;
	}
	else if( Token.Matches(NAME_ForEach) )
	{
		guard(ForEach);
		CheckAllow( "'ForEach'", ALLOW_Cmd );
		PushNest(NEST_ForEach,"");

		// Compile iterator function call.
		AllowIterator = 1;
		GotIterator   = 0;

		// Emit iterator token.
		*Script << EX_Iterator;

		// Compile the iterator expression.
		FToken TypeToken;
		if( !CompileExpr( FProperty(CPT_None), "'ForEach'" ) )
			throwf( "'ForEach': Missing iterator expression" );
		if( !GotIterator )
			throwf( "'ForEach': An iterator expression is required" );
		AllowIterator = 0;

		// Emit end offset.
		EmitAddressToFixupLater( TopNest, FIXUP_IteratorEnd, NAME_None );

		// Handle statements.
		NeedSemicolon = 0;
		if( !MatchSymbol("{") )
		{
			CompileStatement();
			PopNest( NEST_ForEach, "'ForEach'" );
		}
		unguard;
	}
	else if( Token.Matches(NAME_Assert) )
	{
		guard(Assert);
		CheckAllow( "'Assert'", ALLOW_Cmd );
		WORD wLine = InputLine;
		*Script << EX_Assert;
		*Script << wLine;
		CompileExpr( FProperty(CPT_Bool), "'Assert'" );
		unguard;
	}
	else if( Token.Matches(NAME_Goto) )
	{
		guard(Goto);
		CheckAllow( "'Goto'", ALLOW_Label );

		// All state gotos are virtual; all function labels are final.
		if( TopNest->Allow & ALLOW_StateCmd )
		{
			// Emit virtual goto.
			*Script << EX_GotoLabel;
			CompileExpr( FProperty(CPT_Name), "'Goto'" );
		}
		else
		{
			// Get label list for this nest level.
			for( int iNest=NestLevel-1; iNest>=2; iNest-- )
				if( Nest[iNest].NestType==NEST_State || Nest[iNest].NestType==NEST_Function || Nest[iNest].NestType==NEST_ForEach )
					break;
			if( iNest < 2 )
				throwf( "Goto is not allowed here" );
			FNestInfo *LabelNest = &Nest[iNest];

			// Get label.
			FToken Label;
			if( !GetToken(Label) )
				throwf( "Goto: Missing label" );
			if( Label.TokenName == NAME_None )
				Label.TokenName = FName( Label.Identifier, FNAME_Add );
			if( Label.TokenName == NAME_None )
				throwf( "Invalid label '%s'", Label.Identifier );

			// Emit final goto.
			*Script << EX_Jump;
			EmitAddressToFixupLater( LabelNest, FIXUP_Label, Label.TokenName );
		}
		unguard;
	}
	else if( Token.Matches(NAME_Stop) )
	{
		guard(Stop);
		CheckAllow( "'Stop'", ALLOW_StateCmd );
		*Script << EX_Stop;
		unguard;
	}
	else if( Token.Matches(NAME_Broadcast) )
	{
		// Broadcast a function call.
		guard(Broadcast);
		CheckAllow( "'Broadcast'", ALLOW_Cmd );

		if( !Class->IsChildOf("Actor") )
			throwf( "Only Actor classes can broadcast" );

		*Script << EX_Broadcast;
		UClass *ClassContext = new("Actor",FIND_Existing)UClass;
		if( MatchSymbol("(") && !MatchSymbol(")") )
		{
			// Specify an optional name and class match.
			CompileExpr( FProperty(CPT_Name), "'Broadcast' parameter 1" );
			if( MatchSymbol(",") )
			{
				// Compile the class specifier.
				FToken ResultType;
				FProperty RequiredType( CPT_Object, UClass::GetBaseClass() );
				CompileExpr( RequiredType,"'Broadcast' parameter 2", &ResultType );

				// Get the constant class for later scope checking.
				ResultType.GetConstObject( UClass::GetBaseClass(), *(UObject **)&ClassContext );
			}
			else
			{
				*Script << EX_ObjectConst;
				*Script << AR_OBJECT(ClassContext);
			}
			RequireSymbol(")","'Broadcast'");
		}
		else
		{
			// Emit no name.
			*Script << EX_NameConst;
			FName NoName(NAME_None);
			*Script << NoName;

			// Emit no class.
			*Script << EX_ObjectConst;
			*Script << AR_OBJECT(ClassContext);
		}
		RequireSymbol(".","'Broadcast'");

		// Compile expression.
		FRetryPoint Start; InitRetry(Start);
		FToken Token,ResultType;
		if( !GetToken(Token) || !CompileFunctionExpr(Token,ClassContext,ResultType) )
			throwf( "'Broadcast' to class '%s': Unrecognized function call", ClassContext->GetName() );

		// Insert skipover info.
		FRetryPoint End; InitRetry(End);
		WORD wSkip = End.CodeTop - Start.CodeTop; 
		*Script << wSkip;
		CodeSwitcheroo(Start,End);

		unguard;
	}
	else if( Token.Matches("}") )
	{
		// End of block.
		guard(EndBracket);
		if( TopNest->NestType==NEST_Class )
			throwf("Unexpected '}' at class scope");
		else if( TopNest->NestType==NEST_None )
			throwf("Unexpected '}' at global scope");
		PopNest(NEST_None,"'}'");
		NeedSemicolon=0;
		unguard;
	}
	else if( Token.Matches(";") )
	{
		// Extra semicolon.
		guard(Semicolon);
		NeedSemicolon=0;
		unguard;
	}
	else if( MatchSymbol(":") )
	{
		// A label.
		guard(Label);
		CheckAllow( "Label", ALLOW_Label );

		// Validate label name.
		if( Token.TokenName == NAME_None )
			Token.TokenName = FName( Token.Identifier, FNAME_Add );
		if( Token.TokenName == NAME_None )
			throwf( "Invalid label name '%s'", Token.Identifier );

		// Handle first label in a state.
		if( !(TopNest->Allow & ALLOW_Cmd ) )
		{
			// This is the first label in a state, so set the code start and enable commands.
			checkState(TopNode->iCode==MAXWORD);
			checkState(TopNode->NestType==NEST_State);
			TopNest->Allow     |= ALLOW_Cmd;
			TopNest->Allow     &= ~(ALLOW_Function | ALLOW_VarDecl);
			TopNode->iCode      = Script->Num;
		}

		// Get label list for this nest level.
		for( int iNest=NestLevel-1; iNest>=2; iNest-- )
			if( Nest[iNest].NestType==NEST_State || Nest[iNest].NestType==NEST_Function || Nest[iNest].NestType==NEST_ForEach )
				break;
		if( iNest < 2 )
			throwf( "Labels are not allowed here" );
		FNestInfo *LabelNest = &Nest[iNest];

		// Make sure the label is unique here.
		for( FLabelRecord *LabelRec = LabelNest->LabelList; LabelRec; LabelRec=LabelRec->Next )
			if( LabelRec->Name == Token.TokenName )
				throwf( "Duplicate label '%s'", Token.TokenName() );

		// Add label.
		LabelNest->LabelList = new(GMem)FLabelRecord( Token.TokenName, Script->Num, LabelNest->LabelList );
		NeedSemicolon=0;

		unguard;
	}
	else
	{
		guard(Unknown);
		CheckAllow( "Expression", ALLOW_Cmd );
		UngetToken(Token);

		// Try to compile an affector expression or assignment.
		CompileAffector();

		unguard;
	}
	unguard;
}

//
// Compile a statement: Either a declaration or a command.
// Returns 1 if success, 0 if end of file.
//
int FScriptCompiler::CompileStatement()
{
	guard(FScriptCompiler::CompileStatement);
	BOOL NeedSemicolon = 1;

	// Get a token and compile it.
	FToken Token;
	if( !GetToken(Token,NULL,1) )
	{
		// End of file.
		return 0;
	}
	else if( !CompileDeclaration(Token, NeedSemicolon) )
	{
		if( Pass == 0 )
		{
			// Skip this and subsequent commands so we can hit them on next pass.
			if( NestLevel < 3 )
				throwf("Unexpected '%s'",Token.Identifier);
			UngetToken(Token);
			PopNest( TopNest->NestType, NestTypeName(TopNest->NestType) );
			SkipStatements( 1, NestTypeName(TopNest->NestType) );
			NeedSemicolon = 0;
		}
		else
		{
			// Compile the command.
			CompileCommand( Token, NeedSemicolon );
		}
	}

	// Make sure no junk is left over.
	if( NeedSemicolon )
	{
		if( !MatchSymbol(";") )
		{
			if( GetToken(Token) )	throwf( "Missing ';' before '%s'", Token.Identifier );
			else					throwf( "Missing ';'" );
		}
	}
	return 1;
	unguard;
}

/*-----------------------------------------------------------------------------
	Probe mask building.
-----------------------------------------------------------------------------*/

//
// Generate probe bitmask for a script.
//
void FScriptCompiler::PrecomputeProbeMasks( FStackNodePtr Link  )
{
	guard(FScriptCompiler::PrecomputeProbeMasks);

	// Accumulate probe masks based on all functions in this state.
	for( FStackNodePtr FuncLink = Link->ChildFunctions; FuncLink.Class; FuncLink=FuncLink->Next )
		if
		(	(FuncLink->Name.GetIndex() >= PROBE_MIN)
		&&	(FuncLink->Name.GetIndex() <  PROBE_MAX)
		&&  (FuncLink->StackNodeFlags & SNODE_DefinedFunc) )
			Link->ProbeMask |= (QWORD)1 << (FuncLink->Name.GetIndex() - PROBE_MIN);

	// Recurse with all child states in this class.
	for( FStackNodePtr StateLink = Link->ChildStates; StateLink.Class; StateLink=StateLink->Next )
		if( StateLink.Class == Class )
			PrecomputeProbeMasks( StateLink  );

	unguard;
}

/*-----------------------------------------------------------------------------
	Code skipping.
-----------------------------------------------------------------------------*/

//
// Skip over code, honoring { and } pairs.
//
void FScriptCompiler::SkipStatements( int SubCount, const char *ErrorTag  )
{
	guard(FScriptCompiler::SkipStatements);
	FToken Token;
	while( SubCount>0 && GetToken( Token ) )
	{
		if		( Token.Matches("{") ) SubCount++;
		else if	( Token.Matches("}") ) SubCount--;
	}
	if( SubCount > 0 )
		throwf("Unexpected end of file at end of %s",ErrorTag);
	unguard;
}

/*-----------------------------------------------------------------------------
	Main script compiling routine.
-----------------------------------------------------------------------------*/

//
// Recursively perform a second-pass compile.
//
void FScriptCompiler::CompileSecondPass( INDEX iStackNode )
{
	guard(FScriptCompiler::CompileSecondPass);

	// Restore code pointer to where it was saved in the parsing pass.
	FStackNode &Node          = Class->StackTree->Element(iStackNode);
	INT        StartNestLevel = NestLevel;

	// Push this new nesting level.
	PushNest( (ENestType)Node.NestType, Node.Name(), iStackNode );
	checkState(TopNode==&Node);

	// Compile all child functions in this class or state.
	for( FStackNodePtr Link = Node.ChildFunctions; Link.Class==Class; Link=Link->Next )
		CompileSecondPass( Link.iNode );
	checkState(TopNode==&Node);

	// Compile all child states in this class.
	for( Link = Node.ChildStates; Link.Class==Class; Link=Link->Next )
		CompileSecondPass( Link.iNode );
	checkState(TopNode==&Node);

	// Remember input positions.
	InputPos  = PrevPos  = Node.Pos;
	InputLine = PrevLine = Node.Line;

	// Init the node.
	TopNode->iCode = MAXWORD;

	// Compile the code.
	BOOL DoCompile=0;
	LinesCompiled -= InputLine;
	if( InputLine != MAXWORD )
	{
		// Emit function parms info into code stream.
		if( (TopNode->NestType==NEST_Function || TopNode->NestType==NEST_Operator) && !(TopNode->StackNodeFlags & SNODE_IntrinsicFunc) )
		{
			*Script << EX_BeginFunction;
			TopNode->iCode = Script->Num;
			for( int i=0; i<TopNode->NumParms; i++ )
			{
				FProperty &Property = Class( TopNode->iFirstProperty + i );
				if( !(Property.Flags & CPF_ReturnParm) )
				{
					// Emit parm size into code stream.
					INT Size = Property.Size();
					if( i+1 < TopNode->NumParms )
					{
						FProperty &Next = Class( TopNode->iFirstProperty + i + 1);
						if( !(Next.Flags & CPF_ReturnParm) )
							Size += Next.Offset - (Property.Offset + Property.Size());
					}
					BYTE bSize = Size;
					if( bSize != Size )
						throwf( "Parameter '%s' is too large",Property.Name() );
					checkState(bSize!=0);
					*Script << bSize;

					// Emit outparm flag into code stream.
					BYTE bOutParm = (Property.Flags & CPF_OutParm) ? 1 : 0;
					*Script << bOutParm;
				}
			}
			BYTE bEnd = 0;
			*Script << bEnd;

			// Should we compile any code?
			DoCompile = (TopNode->StackNodeFlags & SNODE_DefinedFunc);
		}
		else if( TopNode->NestType==NEST_State )
		{
			// Compile the code.
			DoCompile = 1;
		}
	}

	if( DoCompile )
	{
		// Compile statements until we get back to our original nest level.
		while( NestLevel > StartNestLevel )
			if( !CompileStatement() )
				throwf("Unexpected end of code in %s",NestTypeName((ENestType)Node.NestType));
	}
	else if( iStackNode != 0 )
	{
		// Pop the nesting.
		PopNest( (ENestType)Node.NestType, NestTypeName((ENestType)Node.NestType) );
	}
	LinesCompiled += InputLine;
	unguardf(("(%s)",Class->StackTree->Element(iStackNode).Name()));
}

//
// Compile the script associated with the specified class.
// Returns 1 if compilation was a success, 0 if any errors occured.
//
int FScriptCompiler::CompileScript
(
	UClass		*CompileClass,
	FMemStack	*MemStack,
	BOOL		ObjectPropertiesAreValid,
	BOOL		InBooting,
	INT			InPass
)
{
	guard(FScriptCompiler::CompileScript);
	Booting       = InBooting;
	Class         = CompileClass;
	Pass	      = InPass;
	Mem           = MemStack;
	BOOL Success  = 0;

	// Message.
	guard(InitMessage);
	//AddResultText( "%s %s...\r\n",Pass ? "Compiling" : "Parsing", Class->GetName() );
	GApp->StatusUpdatef( "%s %s...", 0, 0, Pass ? "Compiling" : "Parsing", Class->GetName() );
	unguard;

	// Make sure our parent classes is parsed.
	guard(CheckParsed);
	for( UClass *Temp = Class->ParentClass; Temp; Temp=Temp->ParentClass )
		if( Temp->StackTree==NULL && Temp->IsChildOf("Actor") )//!!
			throwf( "'%s' can't be compiled: Parent class '%s' has errors", Class->GetName(), Temp->GetName() );
	unguard;

	// If the actor has valid properties, save their contents.
	UTextBuffer *ActorPropertiesBuffer = NULL;
	BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins));
	WhichBins[PROPBIN_PerObject] = WhichBins[PROPBIN_PerClass] = 1;

	guard(SaveProperties);
	ObjectPropertiesAreValid = (ObjectPropertiesAreValid && Class->IsChildOf("Actor"));//!!
	if( ObjectPropertiesAreValid && Pass==0 )
	{
		ActorPropertiesBuffer = new("Properties",CREATE_Replace)UTextBuffer(1);
		BYTE *ClassBins[PROPBIN_MAX]; Class->GetClassBins(ClassBins);
		ExportActor
		(
			Class,
			ClassBins,
			*ActorPropertiesBuffer,
			NAME_None,
			0,
			0,
			CPF_Edit,
			NULL,
			0,
			-1,
			NAME_None,
			WhichBins
		);
	}
	unguard;

	// Save stuff for later recall.
	FSavedClass SavedClass;
	FMemMark Mark(*Mem);
	Class->Push(SavedClass,GMem);

	// Init class.
	if( Pass == 0 )
	{
		// First pass.
		guard(InitFirstPass);
		Class->Empty();
		Class->AllocBins();
		Class->Script          = new(Class->GetName(),CREATE_Replace)UScript(Class);
		Class->StackTree       = new(Class->GetName(),CREATE_Replace)UStackTree(Class);
		Class->PackageName     = NAME_None;

		// Allocate a temporary locals bin.
		Class->Bins[PROPBIN_PerFunction] = new(Class->GetName(),CREATE_MakeUnique)UProperties( Class, PROPBIN_PerFunction );

		// Set class flags.
		Class->ClassFlags     &= ~CLASS_RecompileClear;
		Class->ClassFlags     |= CLASS_RecompileSet;
		if( Class->ParentClass )
			Class->ClassFlags |= (Class->ParentClass->ClassFlags) & CLASS_Inheret;

		// Init dependencies.
		Class->Dependencies    = new(Class->GetName(),CREATE_Replace)UDependencies(0);
		Class->Dependencies->AddItem(FDependency(Class));

		unguard;
	}
	else
	{
		// Second pass.
		guard(InitSecondPass);
		Class->Script = new(Class->GetName(),CREATE_Replace)UScript(Class);
		unguard;
	}

	// Init compiler variables.
	guard(InitCompiler);
	Class->ScriptText->Lock(LOCK_Read);
	Script        = Class->Script;
	AllowIterator = 0;
	Input		  = &Class->ScriptText->Element(0);
	InputSize	  = Class->ScriptText->Num;
	InputPos	  = 0;
	InputLine	  = 1;
	PrevPos		  = 0;
	PrevLine	  = 1;
	unguard;

	// Init nesting.
	guard(InitNesting);
	NestLevel	= 0;
	TopNest		= &Nest[-1];
	PushNest(NEST_None,"");
	unguard;

	// Try to compile it, and catch any errors.
	guard(TryCompile);
	try
	{
		// Compile until we get an error.
		if( Pass == 0 )
		{
			// Parse entire program.
			guard(FirstPass);
			while( CompileStatement() )
				StatementsCompiled++;
			
			// Set AllPropertyFlags.
			Class->AllPropertyFlags=0;
			for( FPropertyIterator It(Class); It; ++It )
				Class->AllPropertyFlags |= It().Flags;

			// Precompute info for runtime optimization.
			PrecomputeProbeMasks( FStackNodePtr(Class,0) );
			LinesCompiled += InputLine;

			// Stub out the script.
			//old: Class->Script->Kill();
			Class->Script = NULL;
			unguard;
		}
		else
		{
			// Compile all uncompiled sections of parsed code.
			guard(SecondPass);
			CompileSecondPass(0);

			// Check for overflow.
			if( Script->Num > 65534 ) throwf( "Code space overflowed by %i bytes",Script->Num - 65534 );

			if( Class->ClassFlags & CLASS_Decompile )
			{
				AddResultText("--Begin decompilation--\r\n");
				GEditor->DecompileScript(Class,*ErrorText,0);
				AddResultText("--End decompilation--\r\n");
			}
			unguard;
		}

		// Make sure the compilation ended with valid nesting.
		if     ( NestLevel==0 )	throwf ( "Internal nest inconsistency" );
		else if( NestLevel==1 )	throwf ( "Missing 'Class' definition" );
		else if( NestLevel==2 ) PopNest( NEST_Class, "'Class'" );
		else if( NestLevel >2 ) throwf ( "Unexpected end of script in '%s' block", NestTypeName(TopNest->NestType) );

		// Update the default actor.
		guard(RestoreProperties);
		if( ObjectPropertiesAreValid && Pass==0 )
		{
			ActorPropertiesBuffer->Lock(LOCK_ReadWrite);
			BYTE *ClassBins[PROPBIN_MAX]; Class->GetClassBins(ClassBins);
			ImportActorProperties
			(
				(ULevel*)NULL,
				Class,
				ClassBins,
				&ActorPropertiesBuffer->Element(0),
				WhichBins,
				0
			);
			ActorPropertiesBuffer->Unlock(LOCK_ReadWrite);
		}
		if( Class->IsChildOf("Actor") )
			Class->GetDefaultActor().SetClass( Class );
		unguard;

		// Kill temporary locals bin.
		if( Pass == 0 )
		{
			guard(KillTempLocals);
			checkState(Class->Bins[PROPBIN_PerFunction]!=0);
			Class->Bins[PROPBIN_PerFunction]->Kill();
			Class->Bins[PROPBIN_PerFunction] = NULL;
			unguard;
		}

		// Shrink all subobjects to eliminate padding.
		Class->Finish();
		Success = 1;
	}
	catch( char *ErrorMsg )
	{
		// All errors are critical when booting.
		guard(CompileError);
		if( GEditor->Bootstrapping )
			appErrorf("Error in script %s, Line %i: %s",Class->GetName(),InputLine,ErrorMsg);

		// Handle compiler error.
		AddResultText( "Error in %s, Line %i: %s\r\n", Class->GetName(), InputLine, ErrorMsg );

		// Restore the class to its precompiled state.
		Class->Pop(SavedClass);

		// Invalidate this class and scrap its dependencies.
		if( Class->StackTree )
		{
			//old: Class->StackTree->Kill();
			Class->StackTree = NULL;
		}
		if( Class->Script )
		{
			//old: Class->Script->Kill();
			Class->Script = NULL;
		}
		Class->Dependencies->Empty();
		Class->Dependencies->AddItem(FDependency(Class,0));
		if( Class->ParentClass ) 
			Class->Dependencies->AddItem( FDependency(Class->ParentClass,0) );
		unguard;
	}
	unguard;

	// Clean up and exit.
	guard(Cleanup);
	Class->ScriptText->Unlock(LOCK_Read);
	Class->SetClassVTable( NULL, 0 );
	Mark.Pop();
	unguard;

	return Success;
	unguardf(("(Script %s, Pass %i, Line %i)",Class->GetName(),InPass,InputLine));
}

/*-----------------------------------------------------------------------------
	FScriptCompiler error handling.
-----------------------------------------------------------------------------*/

//
// Print a formatted debugging message (same format as printf).
//
void VARARGS FScriptCompiler::AddResultText(char *Fmt,...)
{
	char 	 TempStr[4096];
	va_list  ArgPtr;

	va_start (ArgPtr,Fmt);
	vsprintf (TempStr,Fmt,ArgPtr);
	va_end   (ArgPtr);

	debugf(LOG_Info,TempStr);
	ErrorText->Log(TempStr);
};

/*-----------------------------------------------------------------------------
	Global functions.
-----------------------------------------------------------------------------*/

//
// Object flags used for make.
//
enum
{
	RF_DidAnalyze = RF_Temp1, // Set for classes that were analyzed (and maybe recompiled).
};

//
// Hierarchically downgrade the status of all classes based on their dependencies.
//
void DowngradeClasses()
{
	guard(DowngradeClasses);
	debugf("DowngradeClasses:");

	// Find any classes whose scripts have changed and mark them uncompiled and unlinked.
	UClass *Class;
	FOR_ALL_TYPED_OBJECTS(Class,UClass)
	{
		if( Class->ScriptText!=NULL )
		{
			checkState(Class->Dependencies!=NULL);
			checkState(Class->Dependencies->Num>=1);
			if( Class->ScriptText->DataCRC() != Class->Dependencies(0).ScriptTextCRC )
			{
				debugf("   Uncompiled: %s",Class->GetName());
				if( Class->StackTree )
				{
					//old: Class->StackTree->Kill();
					Class->StackTree = NULL;
				}
				if( Class->Script )
				{
					//old: Class->Script->Kill();
					Class->Script = NULL;
				}
			}
		}
	}
	END_FOR_ALL_OBJECTS;

	// Find any classes with unparsed dependencies and mark them as uncompiled.
	FOR_ALL_TYPED_OBJECTS(Class,UClass)
	{
		// If this class has unparsed dependencies, mark it uncompiled.
		if( Class->ScriptText!=NULL )
		{
			for( int i=1; i<Class->Dependencies->Num; i++ )
			{
				if( Class->Dependencies(i).Class->StackTree==NULL )
				{
					debugf("   Unlinked due to dependency on %s: %s",Class->Dependencies(i).Class->GetName(),Class->GetName());
					if( Class->Script )
					{
						//old: Class->Script->Kill();
						Class->Script = NULL;
					}
				}
			}

			// If this class has unparsed parents, mark it unparsed.
			for( UClass *Parent = Class->ParentClass; Parent; Parent=Parent->ParentClass )
			{
				if( Parent->StackTree==NULL && Parent->ScriptText!=NULL )//!!
				{
					debugf("   Parent unparsed: %s",Class->GetName());

					if( Class->StackTree )
						Class->StackTree = NULL;

					if( Class->Script )
						Class->Script = NULL;
				}
			}
		}
	}
	END_FOR_ALL_OBJECTS;

	unguard;
}

//
// Guarantee that Class and all its child classes are CLASS_Compiled and return 1, 
// or 0 if error.
//
int ParseScripts( FScriptCompiler &Compiler, UClass *Class, int MakeAll )
{
	guard(MakeScript);
	checkInput(Class!=NULL);
	if( Class->ScriptText == NULL )
		return 1;

	// First-pass compile this class if needed.
	if( MakeAll && Class->StackTree!=NULL )
	{
		//old: Class->StackTree->Kill();
		Class->StackTree = NULL;
	}
	checkState(Class->Dependencies!=NULL);
	if( Class->StackTree==NULL )
		if( !Compiler.CompileScript( Class, &GMem, 1, 0, 0 ) )
			return 0;
	checkState(Class->StackTree!=NULL);

	// First-pass compile child classes.
	UClass *TestClass;
	FOR_ALL_TYPED_OBJECTS(TestClass,UClass)
	{
		if( TestClass->ParentClass==Class && !ParseScripts( Compiler, TestClass, MakeAll ) )
			return 0;
	}
	END_FOR_ALL_OBJECTS;

	// Success.
	return 1;
	unguard;
}

//
// Make all scripts.
// Returns 1 if success, 0 if errors.
// Not recursive.
//
int FGlobalEditor::MakeScripts( int MakeAll )
{
	guard(FGlobalEditor::MakeScripts);

	// Downgrade all classes.
	DowngradeClasses();

	// Do compiling.
	FScriptCompiler	Compiler;
	Compiler.InitMake();

	//!!if( MakeAll )	Compiler.AddResultText("Compiling all scripts.\r\n");	
	//!!else			Compiler.AddResultText("Compiling changed scripts.\r\n");

	// Hierarchically first-pass compile all classes.
	int Success = ParseScripts( Compiler, new(TOP_CLASS_NAME,FIND_Existing)UClass, MakeAll );

	// Recompile any classes that were parsed but not compiled.
	if( Success )
	{
		UClass *TestClass;
		FOR_ALL_TYPED_OBJECTS(TestClass,UClass)
		{
			if( TestClass->ScriptText != NULL )
			{
				checkState(TestClass->StackTree!=NULL);
				if( MakeAll && TestClass->Script )
				{
					//old: TestClass->Script->Kill();
					TestClass->Script = NULL;
				}
				if( TestClass->Script==NULL )
				{
					if( !Compiler.CompileScript( TestClass, &GMem, 1, 0, 1 ) )
					{
						Success = 0;
						goto Skip;
					}
					checkState(TestClass->Script!=NULL);
				}
			}
		}
		END_FOR_ALL_OBJECTS;
	}

	// Done with make.
	Skip:
	Compiler.ExitMake(Success);
	return Success;
	unguard;
}

//
// Compile the specified actor class's text script and output the
// results into its token script.
//
// Assumes:
// * Class->ScriptText contains the script text.
// * Class has been initialized as a valid object.
// * ObjectPropertiesAreValid indicates whether actor properties are set to meaningful values.
//
// Does not assume:
// * Class contains valid properties
//
int FGlobalEditor::CompileScript( UClass *Class, BOOL ObjectPropertiesAreValid, BOOL Booting )
{
	guard(FGlobalEditor::CompileScript);
	checkState(Class->ScriptText!=NULL);
	INT Success;

	// Initialize make.
	FScriptCompiler	Compiler;
	Compiler.InitMake();

	// Downgrade all classes.
	if( Booting )
	{
		// Parse this one script.
		Success = Compiler.CompileScript( Class, &GMem, ObjectPropertiesAreValid, Booting, 0 );
	}
	else
	{
		// Note changed classes.
		DowngradeClasses();

		// Mark this class unparsed.
		if( Class->StackTree )
		{
			//old: Class->StackTree->Kill();
			Class->StackTree = NULL;
		}
		if( Class->Script )
		{
			//old: Class->Script->Kill();
			Class->Script = NULL;
		}

		// Parse all unparsed classes.
		Success = ParseScripts(Compiler,new(TOP_CLASS_NAME,FIND_Existing)UClass, 0);
		if( Success )
		{
			// Compile thie class.
			Success = Compiler.CompileScript( Class, &GMem, ObjectPropertiesAreValid, Booting, 1 );
		}
	}
	Compiler.ExitMake(Success);
	return Success;
	unguard;
}

//
// Verify that all scripts are up to date.
// Returns 1 if so, 0 if not.
//
int FGlobalEditor::CheckScripts( UClass *Class, FOutputDevice &Out )
{
	guard(FGlobalEditor::CheckScripts);
	checkInput(Class!=NULL);

	// Skip if not a scripted class.
	if( Class->Dependencies == NULL )
		return 1;
	
	checkState(Class->Dependencies->Num>=1);
	checkState(Class->Dependencies(0).Class==Class);

	// Make sure this class is parsed.
	if( Class->StackTree==NULL )
	{
		Out.Logf( "Class %s is unparsed",Class->GetName() );
		return 0;
	}

	// Make sure this class is compiled.
	if( Class->Script==NULL )
	{
		Out.Logf( "Class %s is uncompiled", Class->GetName() );
		return 0;
	}

	// Check all dependencies.
	for( int i=0; i<Class->Dependencies->Num; i++ )
	{
		if( !Class->Dependencies(i).IsUpToDate() )
		{
			if( i==0 )
				Out.Logf( "Class %s is out of date", Class->GetName() );
			else if( i==1 && Class->ParentClass )
				Out.Logf( "Class %s's parent is out of date", Class->GetName() );
			else
				Out.Logf( "Class %s's dependency %s is out of date", Class->GetName(), Class->Dependencies(i).Class->GetName() );
			return 0;
		}
	}

	// Check all child class scripts.
	UClass *TestClass;
	FOR_ALL_TYPED_OBJECTS(TestClass,UClass)
	{
		if( TestClass->ParentClass==Class && !CheckScripts(TestClass,Out) )
			return 0;
	}
	END_FOR_ALL_OBJECTS;

	// Everything here is up to date.
	return 1;
	unguardf(("(%s)",Class->GetName()));
}

/*-----------------------------------------------------------------------------
	UScript decompiling.
-----------------------------------------------------------------------------*/

//
// Figure out which class property is responsible for a certain offset
// into the class's property bin.  
//
void FScriptCompiler::FindOriginalProperty
(
	UClass::Ptr		ContextClass,
	EPropertyBin	Bin,
	WORD			Base,
	DWORD			BitMask,
	FProperty		*&ResultProperty,
	WORD			&ResultOffset,
	INT				iFirstProperty,
	INT				NumProperties
)
{
	guard(FindOriginalProperty);
	Refind:
	for( int i=iFirstProperty; i<iFirstProperty+NumProperties; i++ )
	{
		if( ContextClass(i).Bin==Bin && Base>=ContextClass(i).Offset && Base<ContextClass(i).Offset+ContextClass(i).Size() )
		{
			if( ContextClass(i).Type!=CPT_Bool || ContextClass(i).BitMask==BitMask )
			{
				ResultProperty = &ContextClass(i);
				ResultOffset   = Base - ContextClass(i).Offset;
				return;
			}
		}
	}
	if( Bin!=PROPBIN_PerFunction && ContextClass->ParentClass )
	{
		ContextClass   = ContextClass->ParentClass;
		iFirstProperty = 0;
		NumProperties  = ContextClass->Num;
		goto Refind;
	}
	throwf( "Misplaced property in %s @ %i:%i ", ContextClass->GetName(), Bin, Base );
	unguard;
}

//
// Decompile function call parameters.
//
void FScriptCompiler::DecompileFunctionCallParms
(
	UClass::Ptr			ScriptClass,
	INT					iScriptNode,
	UClass::Ptr			ContextClass,
	INT					iContextNode,
	FOutputDevice		&Out,
	BYTE				*&Code
)
{
	guard(DecompileFunctionCallParms);
	checkState(ContextClass!=NULL);
	checkState(ContextClass->StackTree!=NULL);
	checkState(iContextNode<ContextClass->StackTree->Num);

	// Decompile all properties.
	FStackNode &Node = ContextClass->StackTree->Element(iContextNode);
	for( int i=Node.iFirstProperty; i<Node.iFirstProperty+Node.NumProperties; i++ )
	{
		if( (ContextClass(i).Flags & (CPF_Parm|CPF_ReturnParm)) != CPF_Parm )
			break;

		if( *Code == EX_EndFunctionParms )
			throwf( "Premature termination in %s.%s", ContextClass->GetName(), Node.Name() );

		// Comma.
		if( i != Node.iFirstProperty )
			Out.Log(",");

		// Decompile one expression.
		DecompileExpr
		(
			ScriptClass,
			iScriptNode,
			ScriptClass,
			iScriptNode,
			Out,
			Code,
			ContextClass(i).Size()
		);
	}
	if( *Code != EX_EndFunctionParms )
		throwf("Missing end: %s.%s (%i/%i)",ContextClass->GetName(),Node.Name(),i-Node.iFirstProperty,Node.NumProperties);
	Code++;
	unguard;
}

//
// Decompile an expression.
//
FProperty *FScriptCompiler::DecompileExpr
(
	UClass::Ptr			ScriptClass,
	INT					iScriptNode,
	UClass::Ptr			ContextClass,
	INT					iContextNode,
	FOutputDevice		&Out,
	BYTE				*&Code,
	INT					Length
)
{
	guard(DecompileExpr);

	Redo:
	EExprToken E = (EExprToken)*Code++;
	switch( E )
	{
		case EX_LocalVariable:
		case EX_ObjectVariable:
		case EX_StaticVariable:
		case EX_ArrayElement:
		case EX_DefaultVariable:
		case EX_BoolVariable:
		{
			FStackNode &Node = ContextClass->StackTree->Element(iContextNode);
			int IsArrayElement = 0;
			if( E==EX_ArrayElement )
			{	
				// Special array element override.
				IsArrayElement = 1;
				E = (EExprToken)*Code++;
				checkState( E==EX_LocalVariable || E==EX_ObjectVariable || E==EX_StaticVariable || E==EX_BoolVariable || E==EX_DefaultVariable );
			}

			DWORD BitMask=0;
			if( E == EX_BoolVariable )
			{
				// Get bitmask.
				BitMask = (DWORD)1 << (*Code & 31);
				E       = (EExprToken)(*Code >> 5);
				checkState( E==EX_LocalVariable || E==EX_ObjectVariable || E==EX_StaticVariable || E==EX_DefaultVariable );
				Out.Logf("%i#",*Code&31);
				Code++;
			}

			// Get offset of variable.
			EPropertyBin Bin=PROPBIN_PerObject;
			WORD Offset=0, Base = *(WORD *)Code; Code += 2;
			if( E==EX_LocalVariable )
			{
				checkState(iContextNode!=MAXWORD);
				checkState(ContextClass->StackTree->Element(iContextNode).iFirstProperty!=MAXWORD);
				Bin = PROPBIN_PerFunction;
			}
			else if( E==EX_ObjectVariable || E==EX_DefaultVariable )
			{
				Bin = PROPBIN_PerObject;
			}
			else if( E==EX_StaticVariable )
			{
				Bin = PROPBIN_PerClass;
			}
			else appErrorf("Bad bin %i",Bin);

			FProperty *VarProperty;
			FindOriginalProperty
			(
				ContextClass,
				Bin,
				Base,
				BitMask,
				VarProperty,
				Offset,
				Bin==PROPBIN_PerFunction ? Node.iFirstProperty : 0,
				Bin==PROPBIN_PerFunction ? Node.NumProperties  : ContextClass->Num
			);
			Out.Logf
			(
				"[%s %s",
				E == EX_ObjectVariable  ? "Global"
			:	E == EX_LocalVariable   ? "Local"
			:	E == EX_DefaultVariable ? "Default"
			:	                          "Static",
				VarProperty->Name()
			);
			if( VarProperty->Type==CPT_Vector && Length==sizeof(FLOAT) )
			{
				if     ( Offset == 0  ) Out.Logf(".X");
				else if( Offset == 4  ) Out.Logf(".Y");
				else if( Offset == 8  ) Out.Logf(".Z");
				else if( Offset == 12 ) Out.Logf(".D");
				else throwf( "Bad vector offset" );
			}
			else if( VarProperty->Type==CPT_Rotation && Length==sizeof(INT) )
			{
				if     ( Offset == 0  ) Out.Logf(".Pitch");
				else if( Offset == 4  ) Out.Logf(".Yaw"  );
				else if( Offset == 8  ) Out.Logf(".Roll" );
				else throwf( "Bad rotation offset" );
			}
			else
			{
				// We don't necessarily know Length, so this decompiler can only guess
				// that we're not referring to a member variable.
			}
			//Out.Logf("[%s +%i",E==EX_ObjectVariable?"Global":E==EX_LocalVariable?"Local":"Static",Base);

			if( IsArrayElement )
			{
				Out.Logf("[");
				DecompileExpr( ScriptClass, iScriptNode, ScriptClass, iScriptNode, Out, Code, sizeof(INT) );
				Out.Logf("*%i",*Code++);
				Out.Logf("/%i]",*Code++);
			}
			Out.Logf("]");
			return VarProperty;
		}
		case EX_Context:
		{
			FProperty *VarProperty = DecompileExpr( ScriptClass, iScriptNode, ContextClass, iContextNode, Out, Code, sizeof(INDEX) );
			checkState(VarProperty!=NULL);
			checkState(VarProperty->Type==CPT_Object);
			checkState(VarProperty->Class!=NULL);
			Out.Log(".");

			WORD wSkip = *(WORD*)Code; Code+=2;
			BYTE bSize = *Code++;

			return DecompileExpr( ScriptClass, iScriptNode, VarProperty->Class, 0, Out, Code, Length );
		}
		case EX_VirtualFunction:
		{
			FName FuncName = *(FName*)Code; Code+=sizeof(FName);
			Out.Logf("%s(",FuncName());
			checkState(ContextClass!=NULL);
			checkState(iContextNode!=MAXWORD);
			Recheck:
			FStackNodePtr Link = ContextClass->StackTree->Element(iContextNode).ChildFunctions;
			while( Link.Class != NULL )
			{
				if( Link->Name == FuncName )
				{
					DecompileFunctionCallParms( ScriptClass, iScriptNode, Link.Class, Link.iNode, Out, Code  );
					break;
				}
				Link = Link->Next;
			}
			if( Link.Class == NULL )
			{
				FStackNodePtr Link = FStackNodePtr(ContextClass,iContextNode)->ParentNest;
				if( Link.Class != NULL )
				{
					ContextClass = Link.Class;
					iContextNode = Link.iNode;
					goto Recheck;
				}
				throwf( "Failed to chase down function '%s' in class '%s', node %i", FuncName(), ContextClass->GetName(), iContextNode );
			}
			Out.Logf(")");
			return NULL;
		}
		case EX_FinalFunction:
		{
			FStackNodePtr DefiningLink = *(FStackNodePtr*)Code; Code += sizeof(FStackNodePtr);
			checkState(DefiningLink.Class!=NULL);
			checkState(DefiningLink.Class->StackTree!=NULL);
			Out.Logf("%s(",DefiningLink.Class->StackTree->Element(DefiningLink.iNode).Name());
			DecompileFunctionCallParms
			(
				ScriptClass,
				iScriptNode,
				DefiningLink.Class,
				DefiningLink.iNode,
				Out,
				Code
			);
			Out.Logf(")");
			return NULL;
		}
		case EX_IntConst:
		{
			Out.Logf("%i",*(INT *)Code);
			Code += sizeof(INT);
			return NULL;
		}
		case EX_ByteConst:
		{
			Out.Logf("%i",*(BYTE*)Code++);
			return NULL;
		}
		case EX_Nothing:
		{
			Out.Logf("Nothing");
			return NULL;
		}
		case EX_IntZero:
		{
			Out.Logf("0");
			return NULL;
		}
		case EX_IntOne:
		{
			Out.Logf("1");
			return NULL;
		}
		case EX_IntConstByte:
		{
			Out.Logf("%i",*(BYTE *)Code++);
			return NULL;
		}
		case EX_True:
		{
			Out.Logf("True");
			return NULL;
		}
		case EX_False:
		{
			Out.Logf("False");
			return NULL;
		}
		case EX_FloatConst:
		{
			Out.Logf("%f",*(FLOAT *)Code);
			Code += sizeof(FLOAT);
			return NULL;
		}
		case EX_StringConst:
		{
			Out.Logf("\"%s\"",(char *)Code);
			Code += strlen((char *)Code)+1;
			return NULL;
		}
		case EX_ObjectConst:
		{
			UObject *Res = *(UObject **)Code; Code += sizeof(UObject *);
			if( Res ) Out.Logf("%s(%s)", Res->GetClassName(), Res->GetName() );
			else      Out.Logf("None");
			return NULL;
		}
		case EX_NameConst:
		{
			Out.Logf("%s",(*(FName *)Code)());
			Code += sizeof(FName);
			return NULL;
		}
		case EX_RotationConst:
		{
			FRotation R;
			R.Pitch = *(WORD *)Code; Code+=2;
			R.Yaw   = *(WORD *)Code; Code+=2;
			R.Roll  = *(WORD *)Code; Code+=2;

			Out.Logf("Rot(%i,%i,%i)",R.Pitch,R.Yaw,R.Roll);
			return NULL;
		}
		case EX_VectorConst:
		{
			FVector *V = (FVector *)Code;
			Code += sizeof(FVector);

			Out.Logf("Vect(%f,%f,%f)",V->X,V->Y,V->Z);
			return NULL;
		}
		case EX_NoObject:
		{
			Out.Logf("None");
			return NULL;
		}
		case EX_Self:
		{
			Out.Logf("Self");
			FProperty *Property = new(GMem)FProperty(CPT_Object,ScriptClass);
			return Property;
		}
		case EX_ResizeString:
		{
			Length = *Code++;
			Out.Logf("ResizeString[%i] ",Length);
			goto Redo;
			return NULL;
		}
		case EX_ActorCast:
		{
			UClass *Class = *(UClass **)Code; Code+=sizeof(UClass*);
			Out.Logf( "%s(", Class->GetName() );
			FProperty *ReturnedProperty = DecompileExpr( ScriptClass, iScriptNode, ScriptClass, iScriptNode, Out, Code, sizeof(INDEX) );
			FProperty *Property = NULL;
			if( ReturnedProperty )
			{
				checkState(ReturnedProperty->Type==CPT_Object && ReturnedProperty->Class!=NULL);
				Property = new(GMem)FProperty;
				*Property = *ReturnedProperty;
				Property->Class = Class;
			}
			Out.Logf(")");
			return Property;
		}
		case EX_Skip:
		{
			WORD wOffset = *(WORD*)Code; Code+=2;
			goto Redo;
			break;
		}

		case EX_ByteToInt           :Out.Logf("ByteToInt "           );	Length=sizeof(BYTE);	   goto Redo;
		case EX_ByteToBool          :Out.Logf("ByteToBool "          );	Length=sizeof(BYTE);	   goto Redo;
		case EX_ByteToFloat			:Out.Logf("ByteToFloat "         );	Length=sizeof(BYTE);	   goto Redo;
		case EX_IntToByte			:Out.Logf("IntToByte "           );	Length=sizeof(INT);		   goto Redo;
		case EX_IntToBool			:Out.Logf("IntToBool "           );	Length=sizeof(INT);		   goto Redo;
		case EX_IntToFloat			:Out.Logf("IntToFloat "          );	Length=sizeof(INT);		   goto Redo;
		case EX_BoolToByte			:Out.Logf("BoolToByte "          );	Length=sizeof(DWORD);	   goto Redo;
		case EX_BoolToInt			:Out.Logf("BoolToInt "           );	Length=sizeof(DWORD);	   goto Redo;
		case EX_BoolToFloat			:Out.Logf("BoolToFloat "         );	Length=sizeof(DWORD);	   goto Redo;
		case EX_FloatToByte			:Out.Logf("FloatToByte "         );	Length=sizeof(FLOAT);	   goto Redo;
		case EX_FloatToInt			:Out.Logf("FloatToInt "          );	Length=sizeof(FLOAT);	   goto Redo;
		case EX_FloatToBool			:Out.Logf("FloatToBool "         );	Length=sizeof(FLOAT);	   goto Redo;
		case EX_ObjectToString		:Out.Logf("ObjectToString "      );	Length=sizeof(UObject*);   goto Redo;
		case EX_ObjectToBool		:Out.Logf("ObjectToBool "        );	Length=sizeof(UObject*);   goto Redo;
		case EX_ObjectToInt         :Out.Logf("ObjectToInt "         );	Length=sizeof(UObject*);   goto Redo;
		case EX_NameToBool			:Out.Logf("NameToBool "          );	Length=sizeof(FName);	   goto Redo;
		case EX_StringToByte		:Out.Logf("StringToByte "        );	Length=0;				   goto Redo;
		case EX_StringToInt			:Out.Logf("StringToInt "         );	Length=0;				   goto Redo;
		case EX_StringToBool		:Out.Logf("StringToBool "        );	Length=0;				   goto Redo;
		case EX_StringToFloat		:Out.Logf("StringToFloat "       );	Length=0;				   goto Redo;
		case EX_StringToVector		:Out.Logf("StringToVector "      );	Length=0;				   goto Redo;
		case EX_StringToRotation	:Out.Logf("StringToRotation "    );	Length=0;				   goto Redo;
		case EX_VectorToBool		:Out.Logf("VectorToBool "        ); Length=sizeof(FVector);	   goto Redo;
		case EX_VectorToRotation	:Out.Logf("VectorToRotation "    );	Length=sizeof(FVector);	   goto Redo;
		case EX_RotationToBool		:Out.Logf("RotationToBool "      );	Length=sizeof(FRotation);  goto Redo;
		case EX_RotationToVector	:Out.Logf("RotationToVector "    );	Length=sizeof(FRotation);  goto Redo;
		case EX_ByteToString		:Out.Logf("ByteToString "        ); Length=sizeof(BYTE);       goto Redo;
		case EX_IntToString			:Out.Logf("IntToString "         ); Length=sizeof(INT);		   goto Redo;
		case EX_BoolToString		:Out.Logf("BoolToString "        ); Length=sizeof(DWORD);	   goto Redo;
		case EX_FloatToString		:Out.Logf("FloatToString "       ); Length=sizeof(FLOAT);	   goto Redo;
		case EX_NameToString		:Out.Logf("NameToString "        ); Length=sizeof(FName);	   goto Redo;
		case EX_VectorToString		:Out.Logf("VectorToString "      ); Length=sizeof(FVector);	   goto Redo;
		case EX_RotationToString	:Out.Logf("RotationToString "    ); Length=sizeof(FRotation);  goto Redo;
		default:
		{
			INT iIntrinsic=0;
			if     ( E >= EX_FirstIntrinsic    ) iIntrinsic = E;
			else if( E >= EX_ExtendedIntrinsic ) iIntrinsic = 256*(INT)(E - EX_ExtendedIntrinsic) + *Code++;
			else                                 throwf("Bad token %02X",E);
			Recheck1:
			FStackNodePtr Link = ContextClass->StackTree->Element(iContextNode).ChildFunctions;
			while( Link.Class != NULL )
			{
				FStackNode &Node = *Link;
				if( Link->iIntrinsic == iIntrinsic )
				{
					Out.Logf( "%s(", Link->Name() );
					DecompileFunctionCallParms
					(
						ScriptClass,
						iScriptNode,
						Link.Class,
						Link.iNode,
						Out,
						Code
					);
					break;
				}
				Link = Link->Next;
			}
			if( Link.Class == NULL )
			{
				if( iContextNode != 0 )
				{
					// This isn't totally general - it doesn't handle functions that 
					// exist only in a state and are overridden there, etc!!
					iContextNode = 0;
					goto Recheck1;
				}
				throwf( "Failed to chase down intrinsic %i", iIntrinsic );
			}
			Out.Logf(")");
			return NULL;
		}
	}
	unguard;
}

//
// Decompile everything at a particular stack tree node, including
// variable declarations, functions, states, and code.
//
void FScriptCompiler::DecompileStackNode
(
	UClass::Ptr		Class,
	INT				iNode,
	FOutputDevice	&Out,
	int				Indent,
	int				ParentLinks
)
{
	guard(DecompileStackNode);
	UScript		*Script = Class->Script;
	FStackNode	&Node	= Class->StackTree->Element(iNode);

	// Decompile peer stack nodes first, since they exist in the linked list in
	// reverse order.
	if( ParentLinks ? Node.Next.Class!=NULL : Node.Next.Class==Class )
		DecompileStackNode
		(
			Node.Next.Class,
			Node.Next.iNode,
			Out,
			Indent,
			ParentLinks
		);

	// Figure out parameters/variables.
	int iStart	   = Node.iFirstProperty;
	int iEnd	   = iStart + Node.NumProperties;
	int ThisIndent = iNode==0 ? Indent : Indent+4;
	INT WhichBins[PROPBIN_MAX]={1,1,1};
	DWORD Exclude  = 0;

	// Command that caused the nesting.
	switch( Node.NestType )
	{
		case NEST_Class:
		{
			Out.Logf("    :%sClass %s",spc(Indent),Class->GetName());
			if( Class->ParentClass ) Out.Logf(" Expands %s",Class->ParentClass->GetName());
			if( Class->ClassFlags & CLASS_Intrinsic ) Out.Logf(" Intrinsic");
			if( Class->PackageName != NAME_None )
				Out.Logf(" Package(%s)", Class->PackageName() );
			Out.Logf("; {\r\n");

			Out.Logf("    :%sUses ",spc(Indent));
			for( int i=0; i<Class->Dependencies->Num; i++ )
			{
				if( i > 0 ) Out.Logf(",");
				Out.Logf(Class->Dependencies(i).Class->GetName());
			}
			Out.Logf(";\r\n");
			iStart   = 0;
			iEnd     = Class->Num;
			WhichBins[PROPBIN_PerFunction]=0;
			break;
		}
		case NEST_State:
		{
			// State <name> [,[Auto] [Editable]]
			Out.Logf("    :%sState %s {\r\n",spc(Indent),Node.Name());
			iStart = iEnd = 0;
			break;
		}
		case NEST_Function:
		case NEST_Operator:
		{
			int First=1;
			Out.Logf("    :%s",spc(Indent));

			if( Node.StackNodeFlags & SNODE_IntrinsicFunc )
				Out.Logf("Intrinsic(%i) ",Node.iIntrinsic);
			if( Node.StackNodeFlags & SNODE_FinalFunc )
				Out.Logf("Final ");
			if( Node.StackNodeFlags & SNODE_PrivateFunc )
				Out.Logf("Private ");
			if( Node.StackNodeFlags & SNODE_IteratorFunc )
				Out.Logf("Iterator ");
			if( Node.StackNodeFlags & SNODE_LatentFunc )
				Out.Logf("Latent ");

			if( Node.NestType==NEST_Function )
			{
				Out.Logf("Function ");
			}
			else if( Node.NumParms == 3 )
			{
				Out.Logf("Operator(%i) ",Node.OperPrecedence);
			}
			else if( Node.StackNodeFlags & SNODE_PreOperatorFunc )
			{
				Out.Logf("PreOperator ");
			}
			else
			{
				Out.Logf("PostOperator ");
			}
			Out.Logf( "%s(", Node.Name() );
			FProperty *Property = &Class->Element(iStart);
			while( iStart<iEnd && (Property->Flags & (CPF_Parm|CPF_ReturnParm))==CPF_Parm )
			{
				if( !First )
					Out.Logf(", ");
				Out.Logf("[%i]",Property->Offset);
				Property->ExportU
				(
					Out,
					Node.NestType==NEST_Function ? NULL : &Class->Bins[Property->Bin]->Element(Property->Offset)
				);
				First=0;
				Property++;
				iStart++;
			}
			Out.Logf( ")" );
			if( iStart<iEnd && (Property->Flags & CPF_ReturnParm) )
			{
				Out.Logf(" ");
				Property->ExportU
				(
					Out,
					Node.NestType==NEST_Function ? NULL : &Class->Bins[Property->Bin]->Element(Property->Offset)
				);
				iStart++;
			}
			Out.Logf( " {\r\n" );
			break;
		}
		default:
		{
			throwf( "Unknown nest type %i\r\n", spc(Indent), Node.NestType );
			break;
		}
	}

	// Variable declarations at this stack node.
	if( Node.NumProperties > 0 )
	{
		FProperty *Property = &Class->Element(iStart);
		while( iStart < iEnd )
		{
			if( WhichBins[Property->Bin] && !(Property->Flags & Exclude) )
			{
				Out.Logf("    :%s",spc(ThisIndent));
				Property->ExportU
				(
					Out,
					Node.NestType==NEST_Function ? NULL : &Class->Bins[Property->Bin]->Element(Property->Offset)
				);
				Out.Logf(";\r\n");
			}
			iStart++;
			Property++;
		}
	}

	// Child functions and states.
	if( ParentLinks ? (Node.ChildFunctions.Class!=NULL) : (Node.ChildFunctions.Class==Class) )
		DecompileStackNode
		(
			Node.ChildFunctions.Class,
			Node.ChildFunctions.iNode,
			Out,
			Indent+4,
			ParentLinks
		);

	// Child functions and states copied from parent.
	if( ParentLinks ? (Node.ChildStates.Class!=NULL) : (Node.ChildStates.Class==Class) )
		DecompileStackNode
		(
			Node.ChildStates.Class,
			Node.ChildStates.iNode,
			Out,
			Indent+4,
			ParentLinks
		);

	// Code at this stack node.
	if( Node.iCode!=MAXWORD && Class->Script!=NULL )
	{
		Indent += 4;
		INT SwitchLength=0;
		BYTE B, *Code = &Script->Element(Node.iCode), *CodeStart = &Script->Element(0);

		// Skip parm info in code stream.
		if( Node.NestType==NEST_Function || Node.NestType==NEST_Operator )
		{
			while( *Code++ != 0 )
			{
				//Out.Logf("Parm size=%i Out=%i\r\n",Code[-1],Code[0]);
				*Code++;
			}
		}

		// Decompile code.
		do
		{
			Out.Logf("%04x:",Code - CodeStart);

			B = *Code++;
			switch( B )
			{
				case EX_Stop:
				{
					Out.Logf("%sStop;\r\n",spc(Indent));
					break;
				}
				case EX_EndCode:
				{
					Out.Logf("%sEndCode;\r\n",spc(Indent));
					break;
				}
				case EX_IteratorPop:
				{
					Out.Logf("%sIteratorPop;\r\n",spc(Indent));
					break;
				}
				case EX_Switch:
				{
					SwitchLength = *Code++;
					Out.Logf("%sSwitch %i(",spc(Indent),SwitchLength);
					DecompileExpr(Class,iNode,Class,iNode,Out,Code,SwitchLength);
					Out.Logf(")\r\n");
					break;
				}
				case EX_Case:
				{
					WORD W = *(WORD*)Code; Code+=2;
					Out.Logf("%sCase.%04X(",spc(Indent),W);
					if( W == MAXWORD ) Out.Logf("Default");
					else DecompileExpr(Class,iNode,Class,iNode,Out,Code,SwitchLength);
					Out.Logf("):\r\n");
					break;
				}
				case EX_Jump:
				{
					WORD W = *(WORD*)Code; Code+=2;
					Out.Logf("%sJump.%04x;\r\n",spc(Indent),W);
					break;
				}
				case EX_IteratorNext:
				{
					Out.Logf("%sIteratorNext;\r\n",spc(Indent));
					break;
				}
				case EX_JumpIfNot:
				{
					WORD W = *(WORD*)Code; Code+=2;
					Out.Logf("%sJumpIfNot.%04x (",spc(Indent),W);
					DecompileExpr( Class, iNode, Class, iNode, Out, Code, sizeof(DWORD) );
					Out.Logf( ");\r\n" );
					break;
				}
				case EX_Let:
				{
					BYTE LetLength = *Code++;
					Out.Logf("%sLet %i ",spc(Indent),LetLength);
					DecompileExpr(Class,iNode,Class,iNode,Out,Code,LetLength);
					Out.Logf("=");
					DecompileExpr(Class,iNode,Class,iNode,Out,Code,LetLength);
					Out.Logf(";\r\n");
					break;
				}
				case EX_LetString:
				case EX_LetBool:
				case EX_Let1:
				case EX_Let4:
				{
					Out.Logf("%sLet ",spc(Indent));
					DecompileExpr(Class,iNode,Class,iNode,Out,Code,4);
					Out.Logf("=");
					DecompileExpr(Class,iNode,Class,iNode,Out,Code,4);
					Out.Logf(";\r\n");
					break;
				}
				case EX_Return:
				{
					Out.Logf("%sReturn;\r\n",spc(Indent));
					break;
				}
				case EX_Assert:
				{
					Out.Logf("%sAssert ",spc(Indent));
					WORD Line = *(WORD *)Code; Code+=sizeof(WORD);
					DecompileExpr( Class,iNode,Class, iNode, Out, Code, sizeof(DWORD) );
					Out.Logf(" (line %i);\r\n",Line);
					break;
				}
				case EX_GotoLabel:
				{
					Out.Logf("%sGoto(",spc(Indent));
					DecompileExpr(Class,iNode,Class,iNode,Out,Code,sizeof(FName));
					Out.Logf(");\r\n");
					break;
				}
				case EX_LabelTable:
				{
					checkState(((int)Code&3)==0);
					for( ; ; )
					{
						FLabelEntry *Entry = (FLabelEntry*)Code;
						Code += sizeof(*Entry);
						if( Entry->Name==NAME_None )
							break;
						else
							Out.Logf("%s Label %s.%X;\r\n",Entry->Name(),Entry->iCode);
					}
					break;
				}
				case EX_Broadcast:
				{
					UClass *ClassContext = new( "Actor", FIND_Existing )UClass;
					Out.Logf( "%sBroadcast(", spc(Indent) );
					DecompileExpr( Class, iNode, Class, iNode, Out, Code, sizeof(FName) );
					Out.Logf( "," );

					// If a class constant was specified, we can broadcast to its class context.
					if( *Code == EX_ObjectConst )
						ClassContext = *(UClass **)(Code+1);

					DecompileExpr( Class, iNode, Class, iNode, Out, Code, sizeof(UObject*) );
					Out.Logf( ")." );

					WORD wSkip = *(WORD*)Code; Code+=2;

					DecompileExpr( Class, iNode, ClassContext, 0, Out, Code, 0 );
					Out.Logf( ";\r\n" );
					break;
				}
				case EX_Iterator:
				{
					Out.Logf( "%sForEach ", spc(Indent) );
					DecompileExpr( Class, iNode, Class, iNode, Out, Code, 0 );
					Out.Logf(";\r\n");
					WORD W = *(WORD*)Code; Code+=2;
					break;
				}
				default:
				{
					Out.Logf( "%s", spc(Indent) );
					Code--;
					DecompileExpr( Class, iNode, Class, iNode, Out, Code, 0 );
					Out.Logf( ";\r\n" );
					break;
				}
			}
		} while( B != EX_EndCode );
		Indent -= 4;
	}

	if( Node.NestType==NEST_State && Node.CodeLabelOffset!=MAXWORD )
	{
		FLabelEntry *Entry = (FLabelEntry*)&Script->Element(Node.CodeLabelOffset);
		while( Entry->Name != NAME_None )
		{
			Out.Logf("%s     Label %s.%04X;\r\n",spc(Indent),Entry->Name(),Entry->iCode);
			Entry++;
		}
	}

	// Command that ended the nesting.
	Out.Logf( "    :%s};\r\n", spc(Indent) );
	unguardf(("(%s %i:%s)",Class->GetName(),iNode,Class->StackTree->Element(iNode).Name()));
}

//
// Decompile a script.
//
void FGlobalEditor::DecompileScript( UClass *Class, FOutputDevice &Out,int ParentLinks )
{
	guard(FGlobalEditor::DecompileScript);

	// Decompile.
	FScriptCompiler Compiler;
	Compiler.DecompileStackNode(Class,0,Out,0,ParentLinks);
	
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
