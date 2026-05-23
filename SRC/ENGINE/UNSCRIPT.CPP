/*=============================================================================
	UnScript.cpp: UnrealScript execution and support code.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	UnrealScript execution and support code.

Revision history:
	* Created by Tim Sweeney 

=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

UNENGINE_API void (*GIntrinsics[EX_Max])( FExecStack &Stack, UObject *Context, BYTE *&Result );
int GIntrinsicDuplicate=0;

/*-----------------------------------------------------------------------------
	Script warning.
-----------------------------------------------------------------------------*/

void ScriptWarn( FExecStack &Stack, char *Fmt, ... )
{
	char TempStr[4096];
	va_list  ArgPtr;

	va_start (ArgPtr,Fmt);
	vsprintf (TempStr,Fmt,ArgPtr);
	va_end   (ArgPtr);

	guard(ScriptWarn);
#if 0 /* Use this to cause a critical error for any script warnings */
	appErrorf
	(
		"%s %s (%s) Warning: %s",
		Stack.Object->GetClassName(),
		Stack.Object->GetName(),
		Stack.Link.Node().Name(),
		TempStr
	);
#else /* Use this to displace a log message for any script warnings */
	debugf
	(
		LOG_ScriptWarn,
		"%s %s (%s) Warning: %s",
		Stack.Object->GetClassName(),
		Stack.Object->GetName(),
		Stack.Link.Node().Name(),
		TempStr
	);
#endif
	unguard;
}

/*-----------------------------------------------------------------------------
	Unaligned code reading routines.
-----------------------------------------------------------------------------*/

//warning: Byte order dependencies.

static union
{
	struct			{BYTE B0,B1,B2,B3;};
	struct			{WORD W0,W1;};
	INT				I0;
	DWORD			D0;
	FLOAT			F0;
} scriptTemp;

inline INT scriptReadInt( BYTE *&Code )
{
	scriptTemp.B0 = Code[0];
	scriptTemp.B1 = Code[1];
	scriptTemp.B2 = Code[2];
	scriptTemp.B3 = Code[3];
	Code += sizeof(INT);
	return scriptTemp.I0;
}

inline FLOAT scriptReadFloat( BYTE *&Code )
{
	scriptTemp.B0 = Code[0];
	scriptTemp.B1 = Code[1];
	scriptTemp.B2 = Code[2];
	scriptTemp.B3 = Code[3];
	Code += sizeof(FLOAT);
	return scriptTemp.F0;
}

inline INT scriptReadWord( BYTE *&Code )
{
	scriptTemp.I0 = 0;
	scriptTemp.B0 = Code[0];
	scriptTemp.B1 = Code[1];
	Code += sizeof(WORD);
	return scriptTemp.I0;
}

inline FName scriptReadName( BYTE *&Code )
{
	scriptTemp.B0 = Code[0];
	scriptTemp.B1 = Code[1];
	Code += sizeof(FName);
	return *(FName*)&scriptTemp.W0;
}

inline FStackNodeLink scriptReadStackNodeLink( BYTE *&Code )
{
	FStackNodeLink Temp = *(FStackNodeLink*)Code;
	Code += sizeof(FStackNodeLink);
	return Temp;
}

/*-----------------------------------------------------------------------------
	Global script execution functions.
-----------------------------------------------------------------------------*/

//
// Information remembered about an OutParm.
//
struct FOutParmRec
{
	BYTE *Dest;
	BYTE *Src;
	INT  Size;
};

//
// Find a label within a stack node link.
// If found, sets DestLink and returns 1.
// If not found, returns 0.
//
static int FindLabelLink( UScript *&DestScript, BYTE *&DestCode, FStackNodeLink *SourceLink, FName FindLabel )
{
	guard(FindLabelLink);
	//debugf("FindLabelLink '%s' in state '%s'",FindLabel(),SourceLink->Node().Name());

	if( FindLabel != NAME_None )
	{
		// Check this state and all its parent states for the label.
		while( SourceLink->Class )
		{
			FStackNode &Node = SourceLink->Node();
			if( Node.CodeLabelOffset != MAXWORD )
			{
				FLabelEntry *Label = (FLabelEntry *)&SourceLink->Class->Script->Element(Node.CodeLabelOffset);
				while( Label->Name != NAME_None )
				{
					if( Label->Name == FindLabel )
					{
						// Found it.
						DestScript = SourceLink->Class->Script;
						DestCode   = &DestScript->Element(Label->iCode);
						return 1;
					}
					Label++;
				}
			}

			// Check parent state for this label.
			SourceLink = &Node.Parent;
		}
	}

	// Didn't find it.
	DestCode = NULL;
	return 0;
	unguard;
}

//
// Have an object go to a named state, and idle at no label.
// If state is NAME_None or was not found, goes to no state.
// Returns 1 if we went to a state, 0 if went to no state.
//
int AActor::GotoState( FName NewState )
{
	guard(AActor::GotoState);
	debugState(GetClass()!=NULL);
	debugState(GetClass()->Script!=NULL);
	debugState(GetClass()->StackTree!=NULL);
	debugState(GetClass()->StackTree->Num>0);

	FStackNodeLink Link(NULL,0);
	if( NewState != NAME_Auto )
	{
		// Find regular state.
		for( Link = GetClass()->StackTree->Element(0).ChildStates; Link.Class; Link=Link.Node().Next )
			if( Link.Node().Name == NewState )
				break;
	}
	else
	{
		// Find auto state.
		for( Link = GetClass()->StackTree->Element(0).ChildStates; Link.Class; Link=Link.Node().Next )
			if( Link.Node().StackNodeFlags & SNODE_AutoState )
				break;
	}

	if( !Link.Class )
	{
		// Going nowhere.
		NewState = NAME_None;
		Link	 = FStackNodeLink( GetClass(), 0 );
	}

	// Go there.
	State			 = NewState;
	MainStack.Link	 = Link;
	MainStack.Script = Link.Class->Script;
	MainStack.Code	 = NULL;
	MainStack.InitVfCache();

	// Compute probes.
	MainStack.ProbeMask = 
		(Link.Node().ProbeMask | GetClass()->StackTree->Element(0).ProbeMask) & Link.Node().IgnoreMask;

	// End any latent action in progress.
	LatentAction = 0;

	return State!=NAME_None;
	unguard;
}

//
// Goto a label in the current state.
// Returns 1 if went, 0 if not found.
//
int AActor::GotoLabel( FName Label )
{
	guard(AActor::GotoLabel);

	debugState(GetClass()!=NULL);
	debugState(GetClass()->Script!=NULL);
	debugState(GetClass()->StackTree!=NULL);
	debugState(GetClass()->StackTree->Num>0);

	return FindLabelLink( MainStack.Script, MainStack.Code, &MainStack.Link, Label );
	unguard;
}


//
// Begin execution, by setting the state.
//
void AActor::BeginExecution()
{
	guard(UObject::BeginExecution);

	// Init major info.
	MainStack.InitVfCache();
	MainStack.Locals = NULL;

	checkState(GetClass()!=NULL);
	checkState(GetClass()->Script!=NULL);
	checkState(GetClass()->StackTree!=NULL);
	checkState(GetClass()->StackTree->Num>0);
	checkState(MainStack.Object==this);
	checkState(Level!=NULL);
	checkState(XLevel!=NULL);
	checkState(XLevel->Element(0)!=NULL);
	checkState(XLevel->Element(0)==Level);

	// Init the state to None and then set the desired state, if any.
	FName InitialState = State!=NAME_None ? State : NAME_Auto;
	State              = NAME_None;


	// Set the state.
	GotoState( InitialState );
	if( State != NAME_None )
		GotoLabel( NAME_Begin );

	unguardf(("(%s %s)",GetClassName(),GetName()));
}
/*-----------------------------------------------------------------------------
	Intrinsics.
-----------------------------------------------------------------------------*/

DWORD  *GBoolAddr;
DWORD  GBoolMask;

/////////////////////////////////
// Undefined intrinsic handler //
/////////////////////////////////

void UNENGINE_API execUndefined( FExecStack &Stack, UObject *Context, BYTE *&Result  )
{
	// Execute an undefined opcode.
	guardSlow(execUndefined);
	appErrorf("%s (%s): Unknown code token %02x", Stack.Script->GetName(), Stack.Link.Node().Name(), Stack.Code[-1] );
	unguardexecSlow;
}

///////////////
// Variables //
///////////////

static void execLocalVariable( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute a local variable.
	guardSlow(execLocalVariable);
	debugState(Stack.Object==Context);
	debugState(Stack.Locals!=NULL);
	Result = Stack.Locals + scriptReadWord(Stack.Code);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_LocalVariable, execLocalVariable );

static void execObjectVariable( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute an object variable.
	guardSlow(execObjectVariable);
	Result = (BYTE*)Context + scriptReadWord(Stack.Code);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ObjectVariable, execObjectVariable );

static void execStaticVariable( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute a static variable.
	guardSlow(execStaticVariable);
	Result = &Context->GetClass()->Bins[PROPBIN_PerClass]->Element(0) + scriptReadWord(Stack.Code);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_StaticVariable, execStaticVariable );

static void execDefaultVariable( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute a default variable.
	guardSlow(execDefaultVariable);
	Result = &Context->GetClass()->Bins[PROPBIN_PerObject]->Element(0) + scriptReadWord(Stack.Code);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_DefaultVariable, execDefaultVariable );

static void execArrayElement( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execArrayElement);

	// Get base pointer.
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Result );

	// Get array offset.
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );

	// Add scaled offset to base pointer.
	Result += *(INT*)Addr * *Stack.Code++;

	// Bounds check.
	if( *(INT*)Addr>=*Stack.Code++ || *(INT*)Addr<0 )
	{
		appErrorf
		(
			"Array bounds error in %s (%s): %i/%i",
			Stack.Script->GetName(),
			Stack.Link.Node().Name(),
			*(INT*)Addr,Stack.Code[-1]
		);
		//!! try to recover somehow?
	}
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ArrayElement, execArrayElement );

static void execBoolVariable( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolVariable);

	// Get bool variable.
	BYTE bCode  = *Stack.Code++;
	(*GIntrinsics[bCode >> 5])( Stack, Context, *(BYTE**)&GBoolAddr );
	GBoolMask   = (DWORD)1 << (bCode&31);

	// Note that we're not returning an in-place pointer to to the bool, so EX_Let does
	// not work here. Instead, we use EX_LetBool which accesses GBoolAddr and GBoolMask.
	*(DWORD*)Result = (*GBoolAddr & GBoolMask) ? 1 : 0;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_BoolVariable, execBoolVariable );

/////////////
// Nothing //
/////////////

static void execNothing( FExecStack &Stack, UObject *Context, BYTE *&Result  )
{
}
AUTOREGISTER_INTRINSIC( EX_Nothing, execNothing );

//////////////
// Commands //
//////////////

static void execStop( FExecStack &Stack, UObject *Context, BYTE *&Result  )
{
	// Exec EX_Stop.
	guardSlow(execStop);
	Stack.Code = NULL;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Stop, execStop );

static void execEndCode( FExecStack &Stack, UObject *Context, BYTE *&Result  )
{
	// Exec EX_EndCode.
	guardSlow(execEndCode);
	Stack.Code = NULL;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_EndCode, execEndCode );

static void execSwitch( FExecStack &Stack, UObject *Context, BYTE *&Result  )
{
	// Execute EX_Switch.
	guardSlow(execSwitch);

	// Get switch size.
	BYTE bSize = *Stack.Code++;

	// Get switch expression.
	BYTE SwitchBuffer[MAX_CONST_SIZE], *SwitchVal=SwitchBuffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, SwitchVal );

	// Check each case clause till we find a match.
	for( ; ; )
	{
		// Skip over case token.
		debugState(*Stack.Code==EX_Case);
		Stack.Code++;

		// Get address of next handler.
		INT wNext = scriptReadWord( Stack.Code );
		if( wNext == MAXWORD ) // Default case or end of cases.
			break;

		// Get case expression.
		BYTE Buffer[MAX_STRING_CONST_SIZE], *Val=Buffer;
		(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Val );

		// Compare.
		if( (bSize ? memcmp(SwitchVal,Val,bSize) : strcmp((char*)SwitchVal,(char*)Val) )==0 )
			break;

		// Jump to next handler.
		Stack.Code = &Stack.Script->Element(wNext);
	}
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Switch, execSwitch );

static void execCase( FExecStack &Stack, UObject *Context, BYTE *&Result  )
{
	// Skip over EX_Case and its expression.
	guardSlow(execCase);

	// Get address of next handler.
	INT wNext = scriptReadWord( Stack.Code );
	if( wNext != MAXWORD )
	{
		BYTE Buffer[MAX_STRING_CONST_SIZE], *Val=Buffer;
		(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Val );
	}
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Case, execCase );

static void execJump( FExecStack &Stack, UObject *Context, BYTE *&Result  )
{
	// Execute EX_Jump.
	guardSlow(execJump);

	// Jump immediate.
	Stack.Code = &Stack.Script->Element( scriptReadWord( Stack.Code ) );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Jump, execJump );

static void execJumpIfNot( FExecStack &Stack, UObject *Context, BYTE *&Result  )
{
	// Execute EX_JumpIfNot.
	guardSlow(execJumpIfNot);

	// Get code offset.
	INT wOffset = scriptReadWord( Stack.Code );

	// Get boolean test value.
	BYTE Buffer[MAX_CONST_SIZE], *Val=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Val );

	// Jump if false.
	if( !*(DWORD*)Val )
		Stack.Code = &Stack.Script->Element(wOffset);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_JumpIfNot, execJumpIfNot );

static void execAssert( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute EX_Assert.
	guardSlow(execAssert);

	// Get line number.
	INT wLine = scriptReadWord( Stack.Code );

	// Get boolean assert value.
	BYTE Buffer[MAX_CONST_SIZE], *Val=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Val );

	// Check it.
	if( !*(DWORD*)Val )
	{
		appErrorf
		(
			"%s (%s): Assertion failed, line %i",
			Stack.Script->GetName(),
			Stack.Link.Node().Name(), 
			wLine
		);
	}

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Assert, execAssert );

static void execGotoLabel( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute EX_GotoLabel.
	guardSlow(execGotoLabel);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_NAME(N);
	if( !ActorContext->GotoLabel( N ) )
		ScriptWarn( Stack, "GotoLabel (%s): Label not found", N );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_GotoLabel, execGotoLabel );

static void execBroadcast( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute EX_Broadcast.
	guardSlow(execBroadcast);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	// Get optional name of broadcast destination.
	FName NameBuffer, *Name=&NameBuffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Name );

	// Get broadcast class mask.
	UClass *ClassBuffer, **Class=&ClassBuffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, *(BYTE**)&Class );

	// Get relative skip offset.
	INT wOffset = scriptReadWord( Stack.Code );

	// Broadcast to all matching actors.
	//debugf("BC:%s TO:%s",(*Name)(),*Class ? (*Class)->GetName() : "NULL" );
	ULevel *Level = ActorContext->GetLevel();
	for( int iActor=0; iActor<Level->Num; iActor++ )
	{
		AActor *Actor = Level->Element(iActor);
		if
		(	(Actor)
		&&	( *Name==NAME_None || *Name == Actor->GetFName() )
        &&  ( Actor->IsA(*Class) ) )
        {
			// Call the function through a temporary execution stack. This assumes and 
			// requires that the following command is a function call and will generate its
			// own locals frame.
			FExecStack NewStack = Stack;
			BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
			(*GIntrinsics[*NewStack.Code++])( NewStack, Actor, Addr );
        }
	}

	// Go to skip offset.
	Stack.Code += wOffset;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Broadcast, execBroadcast );

////////////////
// Assignment //
////////////////

static void execLet( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute EX_Let.
	guardSlow(execLet);

	// Get size.
	BYTE Size = *Stack.Code++;

	// Get variable address.
	BYTE *Var=NULL;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Var );

	// Get value.
	BYTE Buffer[MAX_CONST_SIZE], *Val=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Val );

	// Copy value to variable.
	if( Var )
		memcpy( Var, Val, Size );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Let, execLet );

static void execLet1( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute EX_Let1.
	guardSlow(execLet1);

	// Get variable address.
	BYTE *Var=NULL;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Var );

	// Get value.
	BYTE Buffer[1], *Val=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Val );

	// Copy value to variable.
	if( Var )
		*(BYTE*)Var = *(BYTE*)Val;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Let1, execLet1 );

static void execLet4( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute EX_Let4.
	guardSlow(execLet4);

	// Get variable address.
	BYTE *Var=NULL;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Var );

	// Get value.
	BYTE Buffer[4], *Val=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Val );

	// Copy value to variable.
	if( Var )
		*(DWORD*)Var = *(DWORD*)Val;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Let4, execLet4 );

static void execLetBool( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute EX_LetBool.
	guardSlow(execLetBool);

	// Get variable address.
	BYTE BoolHolder[4], *Var=BoolHolder;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Var );

	// Remember globals.
	DWORD *BoolAddr = GBoolAddr;
	DWORD  BoolMask = GBoolMask;

	// Get value.
	BYTE Buffer[MAX_CONST_SIZE], *Val=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Val );

	// Perform boolean assignment.
	if( BoolAddr )
	{
		if( *(DWORD*)Val ) *BoolAddr |=  BoolMask;
		else               *BoolAddr &= ~BoolMask;
	}

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_LetBool, execLetBool );

static void execLetString( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute EX_LetString.
	guardSlow(execLetString);

	// Get variable address.
	BYTE *Var=NULL;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Var );

	// Get value.
	BYTE Buffer[MAX_STRING_CONST_SIZE]; BYTE *Val=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Val );

	// Copy value to variable.
	if( Var )
		strcpy( (char*)Var, (char*)Val );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_LetString, execLetString );

/////////////////////////
// Context expressions //
/////////////////////////

static void execSelf( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Execute EX_Self.
	guardSlow(execSelf);

	// Get Self actor for this context.
	*(UObject**)Result = Context;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Self, execSelf );

static void execContext( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execContext);

	// Get actor variable.
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );

	// Execute or skip the following expression in the actor's context.
	AActor *NewContext = *(AActor**)Addr;
	if( NewContext != NULL )
	{
		debugState(NewContext->GetClass()!=NULL);

		// Skip the skip info.
		Stack.Code += 3;

		// Evaluate expression in that actor's context.
		(*GIntrinsics[*Stack.Code++])( Stack, NewContext, Result );
	}
	else
	{
		// Get the skipover info.
		INT  wSkip = scriptReadWord( Stack.Code );
		BYTE bSize = *Stack.Code++;

		// Fill the result, unless it's NULL which indicates the caller wants a left-value.
		if( Result )
			memset( Result, 0, bSize );
		GBoolAddr = NULL;

		// Skip the context expression.
		Stack.Code += wSkip;

		// Display debugging message.
		ScriptWarn( Stack, "Accessed None" );
	}
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_Context, execContext );

////////////////////
// Function calls //
////////////////////

static void execVirtualFunction( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVirtualFunction);
	debugState(FExecStackMain::NUM_CACHE_ENTRIES==4);
	debugInput(Context!=NULL);

	FStackNodeLink *Link;
	FStackNode     *Node;
	INT            IsState = Context->IsA("Actor") && ((AActor*)Context)->State!=NAME_None;

	// Get virtual function name.
	FName Message = scriptReadName(Stack.Code);

	// Get virtual function cache info.
	FExecStackMain::FVfCacheEntry *Cache = Context->MainStack.VfCache, *CacheStart=Cache;

	// Make all virtual function cache entries staler.
	CacheStart[0].Staleness++;
	CacheStart[1].Staleness++;
	CacheStart[2].Staleness++;
	CacheStart[3].Staleness++;

	// See if the virtual function is cached.
	for( int i=0; i<FExecStackMain::NUM_CACHE_ENTRIES && Cache->Name!=Message; i++,Cache++ );
	if( i < FExecStackMain::NUM_CACHE_ENTRIES )
	{
		// The virtual function is cached.
		//debugf("Cached: %s",Message());
		Cache->Staleness = 0;
		Link = &Cache->Link;
		Node = &Link->Node();
		goto Found;
	}

	// Find the virtual function in state scope or global scope.
	// Traverse the current stack node to find the specified function.
	Link = &Context->MainStack.Link.Node().ChildFunctions;
	Recheck:
	while( Link->Class != NULL )
	{
		Node = &Link->Node();
		if( Node->Name == Message )
		{
			// Update the cache.
			//debugf("Uncached: %s",Message());
			{
				int i1 = 0 + (CacheStart[0 ].Staleness < CacheStart[1 ].Staleness);
				int i2 = 2 + (CacheStart[2 ].Staleness < CacheStart[3 ].Staleness);
				int i  =     (CacheStart[i1].Staleness < CacheStart[i2].Staleness) ? i2 : i1;
				CacheStart[i].Name      = Message;
				CacheStart[i].Link      = *Link;
				CacheStart[i].Staleness = 0;
			}
			
			// Found it.
			Found:
			if( Node->iIntrinsic )
			{
				// Virtual intrinsic function.
				(*GIntrinsics[ Node->iIntrinsic ])(Stack, Context, Result);
			}
			else
			{
				// Virtual scripted function.
				FMemMark Mark(GMem);
				debugState(Node->iCode!=MAXWORD);
				FExecStack NewStack( Context, *Link, new(GMem,MEM_Zeroed,Node->LocalsSize)BYTE );

				// Form the parms.
				BYTE *Dest = NewStack.Locals;
				FOutParmRec Outs[MAX_FUNC_PARMS], *Out = Outs;
				while( (Out->Size = *NewStack.Code++) != 0 )
				{
					debugState(*NewStack.Code==0 || *NewStack.Code==1);
					Out->Src = Out->Dest = Dest;
					(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Out->Dest );
					if( Out->Dest != Dest )
						memcpy( Dest, Out->Dest, Out->Size );
					Dest += Out->Size;
					Out  += *NewStack.Code++;
				}
				debugState(*Stack.Code==EX_EndFunctionParms);
				Stack.Code++;

				// Execute the code.
				BYTE Buffer[MAX_STRING_CONST_SIZE], *Addr, B;
				while( (B = *NewStack.Code++) != EX_Return )
					(*GIntrinsics[B])( NewStack, NewStack.Object, Addr=Buffer );

				// Copy back outparms.
				while( --Out >= Outs )
					memcpy( Out->Dest, Out->Src, Out->Size );

				// Snag return offset and finish.
				Result = &NewStack.Locals[Node->CodeLabelOffset];

				// Release temp memory.
				Mark.Pop();
			}
			return;
		}
		Link = &Node->Next;
	}
	if( IsState )
	{
		// Failed to find a state version of the function, so check for a global version.
		IsState  = 0;
		Link     = &Context->GetClass()->StackTree->Element(0).ChildFunctions;
		goto Recheck;
	}
	appErrorf( "Function '%s' not found in %s (%s)", Message(), Stack.Object->GetClassName(), Stack.Link.Node().Name() );
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_VirtualFunction, execVirtualFunction );

static void execFinalFunction( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFinalFunction);

	// Make new stack frame in the current actor context.
	FMemMark Mark(GMem);
	FStackNodeLink NewLink = scriptReadStackNodeLink(Stack.Code);
	FStackNode     &Node   = NewLink.Node();
	FExecStack NewStack( Context, NewLink, new(GMem,MEM_Zeroed,Node.LocalsSize)BYTE );
	debugState(Node.iCode!=MAXWORD);

	// Form the parms.
	BYTE *Dest = NewStack.Locals;
	FOutParmRec Outs[MAX_FUNC_PARMS], *Out = Outs;
	while( (Out->Size = *NewStack.Code++) != 0 )
	{
		debugState(*NewStack.Code==0 || *NewStack.Code==1);
		Out->Src = Out->Dest = Dest;
		(*GIntrinsics[*Stack.Code++])( Stack, Stack.Object, Out->Dest );
		if( Out->Dest != Dest )
			memcpy( Dest, Out->Dest, Out->Size );
		Dest += Out->Size;
		Out  += *NewStack.Code++;
	}
	debugState(*Stack.Code==EX_EndFunctionParms);
	Stack.Code++;

	// Execute the code.
	
	BYTE Buffer[MAX_STRING_CONST_SIZE], *Addr, B;
	while( (B = *NewStack.Code++) != EX_Return )
		(*GIntrinsics[B])( NewStack, NewStack.Object, Addr=Buffer );

	// Copy back outparms.
	while( --Out >= Outs )
		memcpy( Out->Dest, Out->Src, Out->Size );

	// Snag return offset and finish.
	Result = &NewStack.Locals[Node.CodeLabelOffset];
	Mark.Pop();
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_FinalFunction, execFinalFunction );

///////////////
// Constants //
///////////////

static void execIntConst( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntConst);
	*(INT*)Result = scriptReadInt(Stack.Code);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_IntConst, execIntConst );

static void execFloatConst( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatConst);
	*(FLOAT*)Result = scriptReadFloat(Stack.Code);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_FloatConst, execFloatConst );

static void execStringConst( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// If everything goes properly, the caller won't overwrite the code.
	guardSlow(execStringConst);
	Result      = Stack.Code;
	Stack.Code += strlen((char*)Stack.Code)+1;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_StringConst, execStringConst );

static void execObjectConst( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execObjectConst);
	*(UObject**)Result = (UObject*)scriptReadInt(Stack.Code);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ObjectConst, execObjectConst );

static void execNameConst( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execNameConst);
	*(FName*)Result = scriptReadName(Stack.Code);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_NameConst, execNameConst );

static void execRotationConst( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationConst);
	((FRotation*)Result)->Pitch = scriptReadWord( Stack.Code );
	((FRotation*)Result)->Yaw   = scriptReadWord( Stack.Code );
	((FRotation*)Result)->Roll  = scriptReadWord( Stack.Code );
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_RotationConst, execRotationConst );

static void execVectorConst( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorConst);
	*(FVector*)Result = *(FVector*)Stack.Code;
	Stack.Code += sizeof(FVector);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_VectorConst, execVectorConst );

static void execByteConst( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteConst);
	*(BYTE*)Result = *Stack.Code++;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ByteConst, execByteConst );

static void execIntZero( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntZero);
	*(INT*)Result = 0;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_IntZero, execIntZero );

static void execIntOne( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntOne);
	*(INT*)Result = 1;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_IntOne, execIntOne );

static void execTrue( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execTrue);
	*(INT*)Result = 1;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_True, execTrue );

static void execFalse( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFalse);
	*(DWORD*)Result = 0;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_False, execFalse );

static void execNoObject( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execNoObject);
	*(UObject**)Result = NULL;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_NoObject, execNoObject );

static void execIntConstByte( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntConstByte);
	*(INT*)Result = *Stack.Code++;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_IntConstByte, execIntConstByte );

/////////////////
// Conversions //
/////////////////

static void execResizeString( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execResizeString);

	// Get new length.
	BYTE NewLength = *Stack.Code++;
	debugState(NewLength>0);

	// Get copy of string expression to convert.
	BYTE *Addr = Result;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	if( Addr != Result )
		strncpy( (char*)Result, (char*)Addr, NewLength );
	Result[NewLength-1] = 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ResizeString, execResizeString );

static void execActorCast( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execActorCast);

	// Get destination class of dynamic actor class.
	UClass *Class = (UClass *)scriptReadInt(Stack.Code);

	// Compile actor expression.
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );

	AActor *Castee = *(AActor**)Addr;
	if( Castee && Castee->IsA(Class) )
	{
		// Cast succeeded.
		*(AActor**)Result = Castee;
	}
	else
	{
		// Cast failed.
		*(AActor**)Result = NULL;
	}
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ActorCast, execActorCast );

static void execByteToInt( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteToInt);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(INT*)Result = *(BYTE*)Addr;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ByteToInt, execByteToInt );

static void execByteToBool( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteToBool);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(DWORD*)Result = *(BYTE*)Addr ? 1 : 0;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ByteToBool, execByteToBool );

static void execByteToFloat( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteToFloat);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(FLOAT*)Result = *(BYTE*)Addr;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ByteToFloat, execByteToFloat );

static void execByteToString( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Bytes can't overflow maximum size.
	guardSlow(execByteToString);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	itoa(*(BYTE*)Addr,(char*)Result,10);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ByteToString, execByteToString );

static void execIntToByte( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntToByte);
	BYTE Buffer[MAX_CONST_SIZE];
	Result = Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Result );
	// Result is automatically truncated.
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_IntToByte, execIntToByte );

static void execIntToBool( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntToBool);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(INT*)Result = *(INT*)Addr ? 1 : 0;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_IntToBool, execIntToBool );

static void execIntToFloat( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntToFloat);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(FLOAT*)Result = *(INT*)Addr;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_IntToFloat, execIntToFloat );

static void execIntToString( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	// Integers can't overflow maximum size.
	guardSlow(execIntToString);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	itoa(*(INT*)Addr,(char*)Result,10);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_IntToString, execIntToString );

static void execBoolToByte( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolToByte);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(BYTE*)Result = *(DWORD*)Addr & 1;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_BoolToByte, execBoolToByte );

static void execBoolToInt( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolToInt);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(INT*)Result = *(DWORD*)Addr & 1;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_BoolToInt, execBoolToInt );

static void execBoolToFloat( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolToFloat);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(FLOAT*)Result = *(DWORD*)Addr & 1;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_BoolToFloat, execBoolToFloat );

static void execBoolToString( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolToString);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	strcpy( (char*)Result, (*(DWORD*)Addr&1) ? "True" : "False" );
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_BoolToString, execBoolToString );

static void execFloatToByte( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatToByte);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(BYTE*)Result = *(FLOAT*)Addr;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_FloatToByte, execFloatToByte );

static void execFloatToInt( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatToInt);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(INT*)Result = *(FLOAT*)Addr;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_FloatToInt, execFloatToInt );

static void execFloatToBool( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatToBool);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(DWORD*)Result = *(FLOAT*)Addr!=0.0 ? 1 : 0;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_FloatToBool, execFloatToBool );

static void execFloatToString( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatToString);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	sprintf((char*)Result,"%f",*(FLOAT*)Addr);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_FloatToString, execFloatToString );

static void execObjectToBool( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execObjectToBool);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(DWORD*)Result = *(UObject**)Addr!=NULL;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ObjectToBool, execObjectToBool );

static void execObjectToInt( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execObjectToInt);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	UObject *Test = *(UObject**)Addr;
	*(DWORD*)Result = Test ? Test->GetIndex() : -1;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ObjectToInt, execObjectToInt );

static void execObjectToString( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execObjectToString);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	strcpy((char*)Result, *(UObject**)Addr ? (*(UObject**)Addr)->GetName() : "None");
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_ObjectToString, execObjectToString );

static void execNameToBool( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execNameToBool);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(DWORD*)Result = *(FName*)Addr!=NAME_None ? 1 : 0;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_NameToBool, execNameToBool );

static void execNameToString( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execNameToString);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	strcpy((char*)Result,(*(FName*)Addr)());
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_NameToString, execNameToString );

static void execStringToByte( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringToByte);
	BYTE Buffer[MAX_STRING_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(BYTE*)Result = atoi((char*)Addr);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_StringToByte, execStringToByte );

static void execStringToInt( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringToInt);
	BYTE Buffer[MAX_STRING_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(INT*)Result = atoi((char*)Addr);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_StringToInt, execStringToInt );

static void execStringToBool( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringToBool);
	BYTE Buffer[MAX_STRING_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	if     ( stricmp((char*)Addr,"True" ) == 0 ) *(INT*)Result = 1;
	else if( stricmp((char*)Addr,"False") == 0 ) *(INT*)Result = 0;
	else                                         *(INT*)Result = atoi((char*)Addr) ? 1 : 0;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_StringToBool, execStringToBool );

static void execStringToFloat( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringToFloat);
	BYTE Buffer[MAX_STRING_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(FLOAT*)Result = atof((char*)Addr);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_StringToFloat, execStringToFloat );

static void execStringToVector( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringToVector);
	BYTE Buffer[MAX_STRING_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(FVector*)Result = GMath.ZeroVector;
	GetFVECTOR((char*)Addr,(FVector*)Result);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_StringToVector, execStringToVector );

static void execStringToRotation( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringToRotation);
	BYTE Buffer[MAX_STRING_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(FRotation*)Result = GMath.ZeroRotation;
	GetFROTATION((char*)Addr,(FRotation*)Result,1.0);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_StringToRotation, execStringToRotation );

static void execVectorToBool( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorToBool);
	BYTE Buffer[MAX_STRING_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(DWORD*)Result = ((FVector*)Addr)->IsZero() ? 0 : 1;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_VectorToBool, execVectorToBool );

static void execVectorToString( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorToString);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	sprintf((char*)Result,"%f,%f,%f",((FVector*)Addr)->X,((FVector*)Addr)->Y,((FVector*)Addr)->Z);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_VectorToString, execVectorToString );

static void execVectorToRotation( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorToRotation);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(FRotation*)Result = ((FVector*)Addr)->Rotation();
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_VectorToRotation, execVectorToRotation );

static void execRotationToBool( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationToBool);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(DWORD*)Result = ((FRotation*)Addr)->IsZero() ? 0 : 1;
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_RotationToBool, execRotationToBool );

static void execRotationToVector( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationToVector);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	*(FVector*)Result = ((FRotation*)Addr)->Vector();
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_RotationToVector, execRotationToVector );

static void execRotationToString( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationToString);
	BYTE Buffer[MAX_CONST_SIZE], *Addr=Buffer;
	(*GIntrinsics[*Stack.Code++])( Stack, Context, Addr );
	sprintf((char*)Result,"%i,%i,%i",((FRotation*)Addr)->Pitch&65535,((FRotation*)Addr)->Yaw&65535,((FRotation*)Addr)->Roll&65535);
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EX_RotationToString, execRotationToString );

////////////////////////////////////////////
// Intrinsic bool operators and functions //
////////////////////////////////////////////

static void execBoolUnaryNot( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolUnaryNot);

	P_GET_BOOL(A);
	P_FINISH;

	*(DWORD*)Result = !A;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 129, execBoolUnaryNot );

static void execBoolLogicEQ( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolLogicEQ);

	P_GET_BOOL(A);
	P_GET_BOOL(B);
	P_FINISH;

	*(DWORD*)Result = ((!A) == (!B));
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 242, execBoolLogicEQ );

static void execBoolLogicNE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolLogicNE);

	P_GET_BOOL(A);
	P_GET_BOOL(B);
	P_FINISH;

	*(DWORD*)Result = ((!A) != (!B));

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 243, execBoolLogicNE );

static void execBoolBinaryAnd( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolBinaryAnd);

	P_GET_BOOL(A);
	P_GET_SKIP_OFFSET(W);

	if( A )
	{
		P_GET_BOOL(B);
		*(DWORD*)Result = A && B;
		P_FINISH;
	}
	else
	{
		*(DWORD*)Result = 0;
		Stack.Code += W;
	}
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 130, execBoolBinaryAnd );

static void execBoolBinaryXor( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolBinaryXor);

	P_GET_BOOL(A);
	P_GET_BOOL(B);
	P_FINISH;

	*(DWORD*)Result = !A ^ !B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 131, execBoolBinaryXor );

static void execBoolBinaryOr( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execBoolBinaryOr);
	P_GET_BOOL(A);
	P_GET_SKIP_OFFSET(W);
	if( !A )
	{
		P_GET_BOOL(B);
		*(DWORD*)Result = A || B;
		P_FINISH;
	}
	else
	{
		*(DWORD*)Result = 1;
		Stack.Code += W;
	}
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 132, execBoolBinaryOr );

////////////////////////////////////////////
// Intrinsic byte operators and functions //
////////////////////////////////////////////

static void execByteBinaryMulAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteBinaryMulAssign);

	P_GET_BYTE_REF(A);
	P_GET_BYTE(B);
	P_FINISH;

	*(BYTE*)Result = (*A *= B);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 5, execByteBinaryMulAssign );

static void execByteBinaryDivAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteBinaryDivAssign);

	P_GET_BYTE_REF(A);
	P_GET_BYTE(B);
	P_FINISH;

	*(BYTE*)Result = B ? (*A /= B) : 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 6, execByteBinaryDivAssign );

static void execByteBinaryAddAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteBinaryAddAssign);

	P_GET_BYTE_REF(A);
	P_GET_BYTE(B);
	P_FINISH;

	*(BYTE*)Result = (*A += B);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 7, execByteBinaryAddAssign );

static void execByteBinarySubAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteBinarySubAssign);

	P_GET_BYTE_REF(A);
	P_GET_BYTE(B);
	P_FINISH;

	*(BYTE*)Result = (*A -= B);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 8, execByteBinarySubAssign );

static void execByteUnaryPreInc( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteUnaryPreInc);

	P_GET_BYTE_REF(A);
	P_FINISH;

	*(BYTE*)Result = ++(*A);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 9, execByteUnaryPreInc );

static void execByteUnaryPreDec( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteUnaryPreDec);

	P_GET_BYTE_REF(A);
	P_FINISH;

	*(BYTE*)Result = --(*A);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 10, execByteUnaryPreDec );

static void execByteUnaryPostInc( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteUnaryPostInc);

	P_GET_BYTE_REF(A);
	P_FINISH;

	*(BYTE*)Result = (*A)++;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 11, execByteUnaryPostInc );

static void execByteUnaryPostDec( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execByteUnaryPostDec);

	P_GET_BYTE_REF(A);
	P_FINISH;

	*(BYTE*)Result = (*A)--;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 12, execByteUnaryPostDec );

/////////////////////////////////
// Int operators and functions //
/////////////////////////////////

static void execIntUnaryBitNot( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntUnaryBitNot);

	P_GET_INT(A);
	P_FINISH;

	*(INT*)Result = ~A;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 13, execIntUnaryBitNot );

static void execIntUnaryNeg( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntUnaryNeg);

	P_GET_INT(A);
	P_FINISH;

	*(INT*)Result = -A;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 15, execIntUnaryNeg );

static void execIntBinaryMul( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryMul);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A * B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 16, execIntBinaryMul );

static void execIntBinaryDiv( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryDiv);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = B ? A / B : 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 17, execIntBinaryDiv );

static void execIntBinaryAdd( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryAdd);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A + B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 18, execIntBinaryAdd );

static void execIntBinarySub( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinarySub);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A - B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 19, execIntBinarySub );

static void execIntBinaryLeftShift( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryLeftShift);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A << B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 20, execIntBinaryLeftShift );

static void execIntBinaryRightShift( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryRightShift);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A >> B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 21, execIntBinaryRightShift );

static void execIntLogicLT( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntLogicLT);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A < B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 22, execIntLogicLT );

static void execIntLogicGT( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntLogicGT);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A > B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 23, execIntLogicGT );

static void execIntLogicLE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntLogicLE);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A <= B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 24, execIntLogicLE );

static void execIntLogicGE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntLogicGE);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A >= B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 25, execIntLogicGE );

static void execIntLogicEQ( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntLogicEQ);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A == B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 26, execIntLogicEQ );

static void execIntLogicNE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntLogicNE);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(DWORD*)Result = A != B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 27, execIntLogicNE );

static void execIntBinaryAnd( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryAnd);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A & B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 28, execIntBinaryAnd );

static void execIntBinaryXor( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryXor);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A ^ B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 29, execIntBinaryXor );

static void execIntBinaryOr( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryOr);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = A | B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 30, execIntBinaryOr );

static void execIntBinaryMulAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryMulAssign);

	P_GET_INT_REF(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = (*A *= B);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 31, execIntBinaryMulAssign );

static void execIntBinaryDivAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryDivAssign);

	P_GET_INT_REF(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = (B ? *A /= B : 0);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 32, execIntBinaryDivAssign );

static void execIntBinaryAddAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinaryAddAssign);

	P_GET_INT_REF(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = (*A += B);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 33, execIntBinaryAddAssign );

static void execIntBinarySubAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntBinarySubAssign);

	P_GET_INT_REF(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = (*A -= B);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 34, execIntBinarySubAssign );

static void execIntUnaryPreInc( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntUnaryPreInc);

	P_GET_INT_REF(A);
	P_FINISH;

	*(INT*)Result = ++(*A);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 35, execIntUnaryPreInc );

static void execIntUnaryPreDec( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntUnaryPreDec);

	P_GET_INT_REF(A);
	P_FINISH;

	*(INT*)Result = --(*A);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 36, execIntUnaryPreDec );

static void execIntUnaryPostInc( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntUnaryPostInc);

	P_GET_INT_REF(A);
	P_FINISH;

	*(INT*)Result = (*A)++;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 37, execIntUnaryPostInc );

static void execIntUnaryPostDec( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntUnaryPostDec);

	P_GET_INT_REF(A);
	P_FINISH;

	*(INT*)Result = (*A)--;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 38, execIntUnaryPostDec );

static void execIntFuncRand( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntFuncRand);

	P_GET_INT(A);
	P_FINISH;

	*(INT*)Result = A>0 ? (rand() % A) : 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 39, execIntFuncRand );

static void execIntFuncMin( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntFuncMin);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = Min(A,B);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 121, execIntFuncMin );

static void execIntFuncMax( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntFuncMax);

	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = Max(A,B);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 122, execIntFuncMax );

static void execIntFuncClamp( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIntFuncClamp);

	P_GET_INT(V);
	P_GET_INT(A);
	P_GET_INT(B);
	P_FINISH;

	*(INT*)Result = Clamp(V,A,B);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 123, execIntFuncClamp );

///////////////////////////////////
// Float operators and functions //
///////////////////////////////////

static void execFloatUnaryNeg( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatUnaryNeg);

	P_GET_FLOAT(A);
	P_FINISH;

	*(FLOAT*)Result = -A;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 41, execFloatUnaryNeg );

static void execFloatBinaryPow( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatBinaryPow);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = exp( B * log(A) );

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 42, execFloatBinaryPow );

static void execFloatBinaryMul( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatBinaryMul);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = A * B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 43, execFloatBinaryMul );

static void execFloatBinaryDiv( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatBinaryDiv);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = A / B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 44, execFloatBinaryDiv );

static void execFloatBinaryMod( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatBinaryMod);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = fmod(A,B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 45, execFloatBinaryMod );

static void execFloatBinaryAdd( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatBinaryAdd);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = A + B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 46, execFloatBinaryAdd );

static void execFloatBinarySub( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatBinarySub);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = A - B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 47, execFloatBinarySub );

static void execFloatLogicLT( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatLogicLT);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(DWORD*)Result = A < B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 48, execFloatLogicLT );

static void execFloatLogicGT( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatLogicGT);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(DWORD*)Result = A > B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 49, execFloatLogicGT );

static void execFloatLogicLE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatLogicLE);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(DWORD*)Result = A <= B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 50, execFloatLogicLE );

static void execFloatLogicGE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatLogicGE);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(DWORD*)Result = A >= B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 51, execFloatLogicGE );

static void execFloatLogicEQ( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatLogicEQ);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(DWORD*)Result = A == B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 52, execFloatLogicEQ );

static void execFloatLogicNE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatLogicNE);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(DWORD*)Result = A != B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 53, execFloatLogicNE );

static void execFloatLogicAE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatLogicAE);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(DWORD*)Result = Abs(A - B)<KINDA_SMALL_NUMBER;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 82, execFloatLogicAE );

static void execFloatBinaryMulAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatBinaryMulAssign);

	P_GET_FLOAT_REF(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = (*A *= B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 54, execFloatBinaryMulAssign );

static void execFloatBinaryDivAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatBinaryDivAssign);

	P_GET_FLOAT_REF(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = (*A /= B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 55, execFloatBinaryDivAssign );

static void execFloatBinaryAddAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatBinaryAddAssign);

	P_GET_FLOAT_REF(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = (*A += B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 56, execFloatBinaryAddAssign );

static void execFloatBinarySubAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatBinarySubAssign);

	P_GET_FLOAT_REF(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = (*A -= B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 57, execFloatBinarySubAssign );

static void execFloatFuncAbs( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncAbs);

	P_GET_FLOAT(A);
	P_FINISH;

	*(FLOAT*)Result = Abs(A);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 58, execFloatFuncAbs );

static void execFloatFuncSin( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncSin);

	P_GET_FLOAT(A);
	P_FINISH;

	*(FLOAT*)Result = sin(A);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 59, execFloatFuncSin );

static void execFloatFuncCos( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncCos);

	P_GET_FLOAT(A);
	P_FINISH;

	*(FLOAT*)Result = cos(A);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 60, execFloatFuncCos );

static void execFloatFuncTan( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncTan);

	P_GET_FLOAT(A);
	P_FINISH;

	*(FLOAT*)Result = tan(A);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 61, execFloatFuncTan );

static void execFloatFuncAtan( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncAtan);

	P_GET_FLOAT(A);
	P_FINISH;

	*(FLOAT*)Result = atan(A);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 62, execFloatFuncAtan );

static void execFloatFuncExp( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncExp);

	P_GET_FLOAT(A);
	P_FINISH;

	*(FLOAT*)Result = exp(A);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 63, execFloatFuncExp );

static void execFloatFuncLog( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncLog);

	P_GET_FLOAT(A);
	P_FINISH;

	*(FLOAT*)Result = log(A);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 64, execFloatFuncLog );

static void execFloatFuncSqrt( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncSqrt);

	P_GET_FLOAT(A);
	P_FINISH;

	*(FLOAT*)Result = sqrt(A);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 65, execFloatFuncSqrt );

static void execFloatFuncSquare( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncSquare);

	P_GET_FLOAT(A);
	P_FINISH;

	*(FLOAT*)Result = Square(A);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 66, execFloatFuncSquare );

static void execFloatFuncFRand( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncFRand);

	P_FINISH;

	*(FLOAT*)Result = frand();

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 67, execFloatFuncFRand );

static void execFloatFuncFMin( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncFMin);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = Min(A,B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 116, execFloatFuncFMin );

static void execFloatFuncFMax( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncFMax);

	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = Max(A,B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 117, execFloatFuncFMax );

static void execFloatFuncFClamp( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncFClamp);

	P_GET_FLOAT(V);
	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = Clamp(V,A,B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 118, execFloatFuncFClamp );

static void execFloatFuncLerp( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncLerp);

	P_GET_FLOAT(V);
	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = A + V*(B-A);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 119, execFloatFuncLerp );

static void execFloatFuncSmerp( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFloatFuncSmerp);

	P_GET_FLOAT(V);
	P_GET_FLOAT(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FLOAT*)Result = A + (3.0*V*V - 2.0*V*V*V)*(B-A);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 120, execFloatFuncSmerp );

////////////////////////////////////
// String operators and functions //
////////////////////////////////////

static void execStringBinaryConcat( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringBinaryAdd);

	P_GET_STRING(A);
	P_GET_STRING(B);
	P_FINISH;

	INT Size = strlen(A);
	strcpy((char*)Result,A);
	strncpy((char*)Result + Size, B, MAX_STRING_CONST_SIZE - Size);
	Result[MAX_STRING_CONST_SIZE-1] = 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 100, execStringBinaryConcat );

static void execStringLogicLT( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringLogicLT);

	P_GET_STRING(A);
	P_GET_STRING(B);
	P_FINISH;

	*(DWORD*)Result = strcmp(A,B) < 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 69, execStringLogicLT );

static void execStringLogicGT( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringLogicGT);

	P_GET_STRING(A);
	P_GET_STRING(B);
	P_FINISH;

	*(DWORD*)Result = strcmp(A,B) > 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 70, execStringLogicGT );

static void execStringLogicLE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringLogicLE);

	P_GET_STRING(A);
	P_GET_STRING(B);
	P_FINISH;

	*(DWORD*)Result = strcmp(A,B) <= 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 71, execStringLogicLE );

static void execStringLogicGE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringLogicGE);

	P_GET_STRING(A);
	P_GET_STRING(B);
	P_FINISH;

	*(DWORD*)Result = strcmp(A,B) >= 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 72, execStringLogicGE );

static void execStringLogicEQ( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringLogicEQ);

	P_GET_STRING(A);
	P_GET_STRING(B);
	P_FINISH;

	*(DWORD*)Result = strcmp(A,B) == 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 73, execStringLogicEQ );

static void execStringLogicNE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringLogicNE);

	P_GET_STRING(A);
	P_GET_STRING(B);
	P_FINISH;

	*(DWORD*)Result = strcmp(A,B) != 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 74, execStringLogicNE );

static void execStringLogicAE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringLogicAE);

	P_GET_STRING(A);
	P_GET_STRING(B);
	P_FINISH;

	*(DWORD*)Result = stricmp(A,B) == 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 40, execStringLogicAE );

static void execStringFuncLen( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringFuncLen);

	P_GET_STRING(S);
	P_FINISH;

	*(INT*)Result = strlen(S);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 76, execStringFuncLen );

static void execStringFuncInStr( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringFuncInStr);

	P_GET_STRING(S);
	P_GET_STRING(A);
	P_FINISH;

	CHAR *Ptr = strstr(S,A);
	*(INT*)Result = Ptr ? Ptr - S : -1;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 77, execStringFuncInStr );

static void execStringFuncMid( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringFuncMid);

	P_GET_STRING(A);
	P_GET_INT(I);
	P_GET_INT_OPT(C,65535);
	P_FINISH;

	if( I < 0 ) C += I;
	I = Clamp( I, 0, (int)strlen(A) );
	C = Clamp( C, 0, (int)strlen(A)-I );
	strncpy( (char*)Result, A + I, C );
	Result[C]=0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 78, execStringFuncMid );

static void execStringFuncLeft( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringFuncLeft);

	P_GET_STRING(A);
	P_GET_INT(N);
	P_FINISH;

	N = Clamp( N, 0, (int)strlen(A) );
	strncpy( (char*)Result, A, N );
	Result[N]=0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 79, execStringFuncLeft );

static void execStringFuncRight( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringFuncRight);

	P_GET_STRING(A);
	P_GET_INT(N);
	P_FINISH;

	N = Clamp( (int)strlen(A) - N, 0, (int)strlen(A) );
	strcpy( (char*)Result, A + N );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 80, execStringFuncRight );

static void execStringFuncCaps( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStringFuncCaps);

	P_GET_STRING(A);
	P_FINISH;

	for( int i=0; A[i]; i++ )
		Result[i] = toupper(A[i]);
	Result[i]=0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 81, execStringFuncCaps );

////////////////////////////////////
// Vector operators and functions //
////////////////////////////////////

static void execVectorUnaryNeg( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorUnaryNeg);

	P_GET_VECTOR(A);
	P_FINISH;

	*(FVector*)Result = -A;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 83, execVectorUnaryNeg );

static void execVectorBinaryMulVF( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryMulVF);

	P_GET_VECTOR(A);
	P_GET_FLOAT (B);
	P_FINISH;

	*(FVector*)Result = A*B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 84, execVectorBinaryMulVF );

static void execVectorBinaryMulFV( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryMulFV);

	P_GET_FLOAT (A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(FVector*)Result = A*B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 85, execVectorBinaryMulFV );

static void execVectorBinaryMulVV( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryMulVV);

	P_GET_VECTOR(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(FVector*)Result = A*B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 296, execVectorBinaryMulVV );

static void execVectorBinaryDivVF( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryDivVF);

	P_GET_VECTOR(A);
	P_GET_FLOAT (B);
	P_FINISH;

	*(FVector*)Result = A/B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 86, execVectorBinaryDivVF );

static void execVectorBinaryAdd( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryAdd);

	P_GET_VECTOR(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(FVector*)Result = A+B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 87, execVectorBinaryAdd );

static void execVectorBinarySub( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinarySub);

	P_GET_VECTOR(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(FVector*)Result = A-B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 88, execVectorBinarySub );

static void execVectorDetransform( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorDetransform);

	P_GET_VECTOR(A);
	P_GET_ROTATION(B);
	P_FINISH;

	*(FVector*)Result = A.TransformVectorBy(GMath.UnitCoords / B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 275, execVectorDetransform );

static void execVectorTransform( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorTransform);

	P_GET_VECTOR(A);
	P_GET_ROTATION(B);
	P_FINISH;

	*(FVector*)Result = A.TransformVectorBy(GMath.UnitCoords * B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 276, execVectorTransform );

static void execVectorLogicEQ( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorLogicEQ);

	P_GET_VECTOR(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(DWORD*)Result = A.X==B.X && A.Y==B.Y && A.Z==B.Z;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 89, execVectorLogicEQ );

static void execVectorLogicNE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorLogicNE);

	P_GET_VECTOR(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(DWORD*)Result = A.X!=B.X || A.Y!=B.Y || A.Z!=B.Z;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 90, execVectorLogicNE );

static void execVectorBinaryDot( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryDot);

	P_GET_VECTOR(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(FLOAT*)Result = A|B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 91, execVectorBinaryDot );

static void execVectorBinaryCross( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryCross);

	P_GET_VECTOR(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(FVector*)Result = A^B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 92, execVectorBinaryCross );

static void execVectorBinaryMulVFAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryMulVFAssign);

	P_GET_VECTOR_REF(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FVector*)Result = (*A *= B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 93, execVectorBinaryMulVFAssign );

static void execVectorBinaryMulVVAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryMulVVAssign);

	P_GET_VECTOR_REF(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(FVector*)Result = (*A *= B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 297, execVectorBinaryMulVVAssign );

static void execVectorBinaryDivAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryDivAssign);

	P_GET_VECTOR_REF(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FVector*)Result = (*A /= B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 94, execVectorBinaryDivAssign );

static void execVectorBinaryAddAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinaryAddAssign);

	P_GET_VECTOR_REF(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(FVector*)Result = (*A += B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 95, execVectorBinaryAddAssign );

static void execVectorBinarySubAssign( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorBinarySubAssign);

	P_GET_VECTOR_REF(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(FVector*)Result = (*A -= B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 96, execVectorBinarySubAssign );

static void execVectorFuncSize( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorFuncSize);

	P_GET_VECTOR(A);
	P_FINISH;

	*(FLOAT*)Result = A.Size();

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 97, execVectorFuncSize );

static void execVectorFuncNormal( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorFuncNormal);

	P_GET_VECTOR(A);
	P_FINISH;

	*(FVector*)Result = A.Normal();

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 98, execVectorFuncNormal );

static void execVectorFuncInvert( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorFuncInvert);

	P_GET_VECTOR_REF(X);
	P_GET_VECTOR_REF(Y);
	P_GET_VECTOR_REF(Z);
	P_FINISH;

	FCoords Temp = FCoords( FVector(0,0,0), *X, *Y, *Z ).Inverse();
	*X           = Temp.XAxis;
	*Y           = Temp.YAxis;
	*Z           = Temp.ZAxis;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 99, execVectorFuncInvert );

static void execVectorFuncVRand( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorFuncVRand);
	P_FINISH;
	do
	{
		// Check random vectors in the unit sphere so result is statistically uniform.
		((FVector*)Result)->X = frand();
		((FVector*)Result)->Y = frand();
		((FVector*)Result)->Z = frand();
	} while( ((FVector*)Result)->SizeSquared() > 1.0 );
	((FVector*)Result)->Normalize();
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 124, execVectorFuncVRand );

static void execVectorFuncDist( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorFuncDist);

	P_GET_VECTOR(A);
	P_GET_VECTOR(B);
	P_FINISH;

	*(FLOAT*)Result = (B-A).Size();

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 125, execVectorFuncDist );

//////////////////////////////////////
// Rotation operators and functions //
//////////////////////////////////////

static void execRotationLogicEQ( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorLogicEQ);

	P_GET_ROTATION(A);
	P_GET_ROTATION(B);
	P_FINISH;

	*(DWORD*)Result = A.Pitch==B.Pitch && A.Yaw==B.Yaw && A.Roll==B.Roll;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 14, execRotationLogicEQ );

static void execRotationLogicNE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execVectorLogicNE);

	P_GET_ROTATION(A);
	P_GET_ROTATION(B);
	P_FINISH;

	*(DWORD*)Result = A.Pitch!=B.Pitch || A.Yaw!=B.Yaw || A.Roll!=B.Roll;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 0x80 + 75, execRotationLogicNE );

static void execRotationMul1( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationMul1);

	P_GET_ROTATION(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FRotation*)Result = A * B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 287, execRotationMul1 );

static void execRotationMul2( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationMul2);

	P_GET_FLOAT(A);
	P_GET_ROTATION(B);
	P_FINISH;

	*(FRotation*)Result = B * A;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 288, execRotationMul2 );

static void execRotationDiv1( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationDiv1);

	P_GET_ROTATION(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*(FRotation*)Result = A * (1.0/B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 289, execRotationDiv1 );

static void execRotationMulEq( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationMul1);

	P_GET_ROTATION_REF(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*A = *A * B;

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 290, execRotationMulEq );

static void execRotationDivEq( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationMul1);

	P_GET_ROTATION_REF(A);
	P_GET_FLOAT(B);
	P_FINISH;

	*A = *A * (1.0/B);

	unguardexecSlow;
}	
AUTOREGISTER_INTRINSIC( 291, execRotationDivEq );

static void execRotationFuncGetAxes( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationFuncGetAxes);

	P_GET_ROTATION(A);
	P_GET_VECTOR_REF(X);
	P_GET_VECTOR_REF(Y);
	P_GET_VECTOR_REF(Z);
	P_FINISH;

	FCoords Coords = GMath.UnitCoords;
	Coords /= A;
	*X = Coords.XAxis;
	*Y = Coords.YAxis;
	*Z = Coords.ZAxis;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 101, execRotationFuncGetAxes );

static void execRotationFuncGetUnAxes( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execRotationFuncGetUnAxes);

	P_GET_ROTATION(A);
	P_GET_VECTOR_REF(X);
	P_GET_VECTOR_REF(Y);
	P_GET_VECTOR_REF(Z);
	P_FINISH;

	FCoords Coords = GMath.UnitCoords * A;
	*X = Coords.XAxis;
	*Y = Coords.YAxis;
	*Z = Coords.ZAxis;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 102, execRotationFuncGetUnAxes );

////////////////////////////////////////////
// Intrinsic name operators and functions //
////////////////////////////////////////////

static void execNameLogicEQ( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execNameLogicEQ);

	P_GET_NAME(A);
	P_GET_NAME(B);
	P_FINISH;

	*(DWORD*)Result = A == B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 126, execNameLogicEQ );

static void execNameLogicNE( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execNameLogicNE);

	P_GET_NAME(A);
	P_GET_NAME(B);
	P_FINISH;

	*(DWORD*)Result = A != B;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 127, execNameLogicNE );

/////////////////////////////
// Log and error functions //
/////////////////////////////

static void execLog( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execLog);

	P_GET_STRING(S);
	P_FINISH;

	debug( LOG_ScriptLog, S );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 103, execLog );

static void execWarn( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execLog);

	P_GET_STRING(S);
	P_FINISH;

	ScriptWarn( Stack, "%s", S );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 104, execWarn );

static void execError( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execLog);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_STRING(S);
	P_FINISH;

	ScriptWarn( Stack, S );
	ActorContext->XLevel->DestroyActor( ActorContext );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 0x80 + 105, execError );

static void execMessage( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execMessage);
	debugInput(Context!=NULL);

	P_GET_STRING(S);
	P_FINISH;

	debug( LOG_Play, S );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 268, execMessage );

static void execPawnMessage( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPawnMessage);
	debugInput(Context!=NULL);
	debugInput(Context->IsA("Pawn"));
	APawn *PawnContext = (APawn*)Context;

	P_GET_STRING(S);
	P_FINISH;

	if( PawnContext->GetPlayer() ) PawnContext->Camera->Log( LOG_Play, S );
	else debug( LOG_Play, S );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 269, execPawnMessage );

/////////////////////
// High intrinsics //
/////////////////////

#define HIGH_INTRINSIC(n) \
	static void execHighIntrinsic##n( FExecStack &Stack, UObject *Context, BYTE *&Result ) \
	{ \
		guardSlow(execHighIntrinsic##n); \
		(*GIntrinsics[ n*0x100 + *Stack.Code++ ])(Stack, Context, Result); \
		unguardexecSlow; \
	} \
	AUTOREGISTER_INTRINSIC( 0x60 + n, execHighIntrinsic##n );

HIGH_INTRINSIC(0);
HIGH_INTRINSIC(1);
HIGH_INTRINSIC(2);
HIGH_INTRINSIC(3);
HIGH_INTRINSIC(4);
HIGH_INTRINSIC(5);
HIGH_INTRINSIC(6);
HIGH_INTRINSIC(7);
HIGH_INTRINSIC(8);
HIGH_INTRINSIC(9);
HIGH_INTRINSIC(10);
HIGH_INTRINSIC(11);
HIGH_INTRINSIC(12);
HIGH_INTRINSIC(13);
HIGH_INTRINSIC(14);
HIGH_INTRINSIC(15);

//////////////////////////////
// Slow function initiators //
//////////////////////////////

enum EPollSlowFuncs
{
	EPOLL_Sleep			= 384,
	EPOLL_FinishAnim	= 385,
};

static void execSleep( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSleep);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_FLOAT(Seconds);
	P_FINISH;

	ActorContext->LatentAction = EPOLL_Sleep;
	ActorContext->LatentFloat  = Seconds;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 256, execSleep );

static void execFinishAnim( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execFinishAnim);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_FINISH;

	if( ActorContext->bAnimLoop )
	{
		// We are looping, so finish at the next sequence end.
		ActorContext->bAnimLoop     = 0;
		ActorContext->bAnimFinished = 0;
	}
	ActorContext->LatentAction = EPOLL_FinishAnim;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 261, execFinishAnim );

///////////////////////////
// Slow function pollers //
///////////////////////////

static void execPollSleep( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPollSleep);
	debugState(Stack.Object->IsA("Actor"));
	AActor *StackActor = (AActor*)Stack.Object;

	FLOAT DeltaSeconds = *(FLOAT*)Result;
	if( (StackActor->LatentFloat -= DeltaSeconds) <= 0.0 )
	{
		// Awaken.
		StackActor->LatentAction = 0;
	}
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EPOLL_Sleep, execPollSleep );

static void execPollFinishAnim( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPollFinishAnim);
	debugState(Stack.Object->IsA("Actor"));
	AActor *StackActor = (AActor*)Stack.Object;

	if( StackActor->bAnimFinished )
		StackActor->LatentAction = 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( EPOLL_FinishAnim, execPollFinishAnim );

/////////////////////////
// Animation functions //
/////////////////////////

static void execPlayAnim( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPlayAnim);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;
	
	P_GET_NAME(SequenceName);
	P_GET_FLOAT_OPT(AnimRate,1.0);
	P_GET_BOOL_OPT(bTweenInto,ActorContext->AnimSequence!=NAME_None);
	P_FINISH;

	// Set one-shot animation.
	if( ActorContext->Mesh )
	{
		FMeshAnimSeq *Seq = ActorContext->Mesh->GetAnimSeq( SequenceName );
		if( Seq )
		{
			ActorContext->AnimSequence  = SequenceName;
			ActorContext->AnimRate      = AnimRate * (Seq->Rate / Seq->NumFrames);
			ActorContext->AnimFrame     = bTweenInto ? (-1.0/Seq->NumFrames) : (0.0);
			ActorContext->AnimEnd       = 1.0 - 1.0 / Seq->NumFrames;
			ActorContext->bAnimNotify   = (Seq->NumNotifys!=0);
			ActorContext->bAnimFinished = 0;
			ActorContext->bAnimLoop     = 0;
		}
		else ScriptWarn( Stack, "PlayAnim: Sequence '%s' not found in Mesh '%s'", SequenceName(), ActorContext->Mesh->GetName() );
	} else ScriptWarn( Stack, "PlayAnim: No mesh" );
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 259, execPlayAnim );

static void execLoopAnim( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execLoopAnim);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;
	
	P_GET_NAME(SequenceName);
	P_GET_FLOAT_OPT(AnimRate,1.0);
	P_GET_BOOL_OPT(bTweenInto,ActorContext->AnimSequence!=NAME_None);
	P_GET_FLOAT_OPT(MinRate,0.0);
	P_FINISH;

	// Set looping animation.
	if( ActorContext->Mesh )
	{
		FMeshAnimSeq *Seq = ActorContext->Mesh->GetAnimSeq( SequenceName );
		if( Seq )
		{
			ActorContext->AnimSequence  = SequenceName;
			ActorContext->AnimRate      = AnimRate * (Seq->Rate / Seq->NumFrames);
			ActorContext->AnimFrame     = bTweenInto ? (-1.0/Seq->NumFrames) : (0.0);
			ActorContext->AnimEnd       = 1.0 - 1.0 / Seq->NumFrames;
			ActorContext->AnimMinRate   = MinRate * (Seq->Rate / Seq->NumFrames);
			ActorContext->bAnimNotify   = (Seq->NumNotifys!=0);
			ActorContext->bAnimFinished = 0;
			ActorContext->bAnimLoop     = 1;
		}
		else ScriptWarn( Stack, "LoopAnim: Sequence '%s' not found in Mesh '%s'", SequenceName(), ActorContext->Mesh->GetName() );
	} else ScriptWarn( Stack, "LoopAnim: No mesh" );
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 260, execLoopAnim );

static void execTweenAnim( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execLoopAnim);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_NAME(SequenceName);
	P_GET_FLOAT(AnimTime);
	P_FINISH;

	// Tweening an animation from wherever it is, to the start of a specified sequence.
	if( ActorContext->Mesh )
	{
		FMeshAnimSeq *Seq = ActorContext->Mesh->GetAnimSeq( SequenceName );
		if( Seq )
		{
			ActorContext->AnimSequence  = SequenceName;
			ActorContext->AnimRate      = (AnimTime>0.0) ? (1.0 / (AnimTime * Seq->NumFrames)) : (0.0);
			ActorContext->AnimFrame     = -1.0/Seq->NumFrames;
			ActorContext->AnimEnd       = 0.0;
			ActorContext->bAnimNotify   = 0;
			ActorContext->bAnimFinished = 0;
			ActorContext->bAnimLoop     = 0;
		}
		else ScriptWarn( Stack, "TweenAnim: Sequence '%s' not found in Mesh '%s'", SequenceName(), ActorContext->Mesh->GetName() );
	} else ScriptWarn( Stack, "LoopAnim: No mesh" );
	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 294, execTweenAnim );

static void execStopAnim( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStopAnim);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_FINISH;

	// Set looping animation.
	ActorContext->AnimRate      = 0.0;
	ActorContext->bAnimFinished = 1;
	ActorContext->bAnimLoop     = 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 282, execStopAnim );

//////////////////////////////
// Force feedback functions //
//////////////////////////////

static void execPlayForce( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execStopAnim);
	debugState(Context!=NULL);
	debugState(Context->IsA("Pawn"));
	APawn *PawnContext = (APawn*)Context;

	P_GET_BYTE(PlayForceType);
	P_GET_VECTOR(Force);
	P_GET_FLOAT(Time);
	P_FINISH;

	//todo!!

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 295, execPlayForce );

/////////////////////////////
// Class related functions //
/////////////////////////////

static void execClassIsChildOf( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execClassIsChildOf);

	P_GET_OBJECT(UClass,K);
	P_GET_OBJECT(UClass,C);
	P_FINISH;

	*(DWORD*)Result = (C && K) ? K->IsChildOf(C) : 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 258, execClassIsChildOf );

///////////////////////////////
// State and label functions //
///////////////////////////////

static void execGoto( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execGoto);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_NAME(L);
	P_FINISH;

	ActorContext->GotoLabel( L );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 112, execGoto );

static void execGotoState( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execGotoState);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_NAME_OPT(S,NAME_None);
	P_GET_NAME_OPT(L,NAME_Begin);
	P_FINISH;

	if( ActorContext->GotoState( S ) )
	{
		if( L != NAME_None )
		{
			ActorContext->GotoLabel( L );
		}
	}
	else ScriptWarn( Stack, "GotoState (%s %s): State not found", S, L );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 113, execGotoState );

static void execIsProbing( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIsProbing);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_NAME(N);
	P_FINISH;

	*(DWORD*)Result = ActorContext->IsProbing(N);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 114, execIsProbing );

static void execIsLabel( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIsLabel);
	debugInput(Context!=NULL);

	P_GET_NAME(L);
	P_FINISH;

	UScript *DummyScript;
	BYTE    *DummyByte;
	*(DWORD*)Result = FindLabelLink(DummyScript,DummyByte,&Context->MainStack.Link,L);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 115, execIsLabel );

static void execIsState( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execIsState);
	debugInput(Context!=NULL);

	P_GET_NAME(S);
	P_GET_NAME_OPT(L,NAME_None);
	P_FINISH;

	for( FStackNodeLink *Link = &Context->MainStack.Link.Node().ChildStates; Link->Class; Link=&Link->Node().Next )
		if( Link->Node().Name == S )
			break;
	UScript *DummyScript;
	BYTE    *DummyByte;
	*(DWORD*)Result = !Link->Class ? 0 : L==NAME_None ? 1 : FindLabelLink(DummyScript,DummyByte,Link,L);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 116, execIsState );

static void execEnable( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execEnable);
	debugInput(Context!=NULL);

	P_GET_NAME(N);
	if( N.GetIndex()>=PROBE_MIN && N.GetIndex()<PROBE_MAX )
		Context->MainStack.ProbeMask |= (Context->MainStack.Link.Node().ProbeMask & ((QWORD)1<<(N.GetIndex()-PROBE_MIN)));
	P_FINISH;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 117, execEnable );

static void execDisable( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execDiable);
	debugInput(Context!=NULL);

	P_GET_NAME(N);
	if( N.GetIndex()>=PROBE_MIN && N.GetIndex()<PROBE_MAX )
		Context->MainStack.ProbeMask &= ~((QWORD)1<<(N.GetIndex()-PROBE_MIN));
	P_FINISH;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 118, execDisable );

///////////////
// Collision //
///////////////

static void execSetCollision( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSetCollision);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_BOOL_OPT(NewCollideActors,ActorContext->bCollideActors);
	P_GET_BOOL_OPT(NewBlockActors,  ActorContext->bBlockActors  );
	P_GET_BOOL_OPT(NewBlockPlayers, ActorContext->bBlockPlayers );
	P_FINISH;

	ActorContext->SetCollision( NewCollideActors, NewBlockActors, NewBlockPlayers );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 262, execSetCollision );

static void execSetCollisionSize( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSetCollisionSize);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_FLOAT(NewRadius);
	P_GET_FLOAT(NewHeight);
	P_FINISH;

	// Untouch this actor.
	if( ActorContext->bCollideActors )
		ActorContext->GetLevel()->Hash.RemoveActor( ActorContext );

	// Set collision sizes.
	ActorContext->CollisionRadius = NewRadius;
	ActorContext->CollisionHeight = NewHeight;

	// Touch this actor.
	if( ActorContext->bCollideActors )
		ActorContext->GetLevel()->Hash.RemoveActor( ActorContext );

	// Return boolean success or failure.
	*(DWORD*)Result = 1;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 283, execSetCollisionSize );

static void execSetFloor( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSetFloor);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_OBJECT(AActor,NewFloor);
	P_FINISH;

	ActorContext->SetFloor( NewFloor );

	unguardSlow;
}
AUTOREGISTER_INTRINSIC( 298, execSetFloor );

static void execCollisionTrace( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execCollisionTrace);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_VECTOR_REF (HitLocation);
	P_GET_VECTOR_REF (HitNormal);
	P_GET_VECTOR     (TraceEnd);
	P_GET_VECTOR_OPT (TraceStart,ActorContext->Location);
	P_GET_FLOAT_OPT  (Radius,ActorContext->CollisionRadius);
	P_GET_FLOAT_OPT  (Height,ActorContext->CollisionHeight);
	P_GET_BOOL_OPT   (bReboundAdjust,1);
	P_FINISH;

	//todo!!

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 284, execCollisionTrace );

///////////
// Audio //
///////////

static void execAmbientSoundSet( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execAmbientSoundSet);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_OBJECT(USound,Sound);
	P_FINISH;

	//debugf("AmbientSoundSet %s",Sound ? Sound->GetName() : "NULL");
	ActorContext->SetAmbientSound(Sound);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 263, execAmbientSoundSet );

static void execPlaySound( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPlaySound);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_OBJECT(USound,Sound);
	P_GET_FLOAT_OPT(Volume,1.0);
	P_GET_FLOAT_OPT(Radius,0.0);
	P_GET_FLOAT_OPT(Pitch,1.0);
	P_FINISH;

	//debugf("PlaySound %s",Sound ? Sound->GetName() : "NULL");
	ActorContext->MakeSound( Sound, Radius, Volume, Pitch );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 264, execPlaySound );

static void execPlayPrimitiveSound( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execPlayPrimitiveSound);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_OBJECT(USound,Sound);
	P_GET_FLOAT_OPT(Volume,1.0);
	P_GET_FLOAT_OPT(Pitch,1.0);
	P_FINISH;

	//debugf("PlayPrimitiveSound %s",Sound ? Sound->GetName() : "NULL");
	ActorContext->PrimitiveSound( Sound, Volume, Pitch );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 265, execPlayPrimitiveSound );

//////////////
// Movement //
//////////////

static void execMove( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execMove);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_VECTOR(Delta);
	P_FINISH;

	FCheckResult Hit(1.0);
	*(DWORD*)Result = ActorContext->GetLevel()->MoveActor( ActorContext, &Delta, Hit );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 266, execMove );

static void execSetLocation( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSetLocation);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_VECTOR(NewLocation);
	P_FINISH;

	*(DWORD*)Result = ActorContext->GetLevel()->FarMoveActor( ActorContext, &NewLocation );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 267, execSetLocation );

static void execSetRotation( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSetRotation);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_ROTATION(NewRotation);
	P_FINISH;

	*(DWORD*)Result = ActorContext->GetLevel()->RotateActor( ActorContext, NewRotation );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 299, execSetRotation );

///////////////
// Reckoning //
///////////////

static void execDistanceTo( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execDistanceTo);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_ACTOR(Other);
	P_FINISH;

	*(FLOAT*)Result = Other ? (ActorContext->Location - Other->Location).Size() : 0.0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 270, execDistanceTo );

static void execCanSee( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execCanSee);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_ACTOR(Other);
	P_FINISH;

	FCheckResult Hit(0.0);
	*(DWORD*)Result = Other ? !ActorContext->GetLevel()->Model->LineCheck
	(
		Hit,
		NULL,
		ActorContext->Location,
		Other->Location,
		0
	) : 0;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 271, execCanSee );

///////////////
// Relations //
///////////////

static void execSetOwner( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSetOwner);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_ACTOR(NewOwner);
	P_FINISH;

	ActorContext->SetOwner( NewOwner );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 272, execSetOwner );

////////////////////////
// Internet functions //
////////////////////////

static void execInternetSend( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execInternetSend);

	P_FINISH;
	P_GET_STRING(ToURL);
	P_GET_STRING(Msg);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 274, execInternetSend );

//////////////////
// Line tracing //
//////////////////

static void execTrace( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execTrace);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_VECTOR_REF(HitLocation);
	P_GET_VECTOR_REF(HitNormal);
	P_GET_VECTOR(TraceEnd);
	P_GET_VECTOR_OPT(TraceStart,ActorContext->Location);
	P_GET_BOOL_OPT(bTraceActors,ActorContext->bCollideActors);
	P_FINISH;

	// Trace the line.
	FCheckResult Hit(1.0);
	ActorContext->XLevel->Trace
	(
		Hit,
		ActorContext,
		TraceEnd,
		TraceStart,
		bTraceActors
	);
	*(AActor**)Result = Hit.Actor;
	*HitLocation      = Hit.Location;
	*HitNormal        = Hit.Normal;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 277, execTrace );

///////////////////////
// Spawn and Destroy //
///////////////////////

static void execSpawn( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSpawn);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_OBJECT(UClass,SpawnClass);
	P_GET_OBJECT_OPT(SpawnOwner,NULL);
	P_GET_NAME_OPT(SpawnName,NAME_None);
	P_GET_VECTOR_OPT(SpawnLocation,ActorContext->Location);
	P_GET_ROTATION_OPT(SpawnRotation,ActorContext->Rotation);
	P_FINISH;

	// Spawn and return actor.
	*(AActor**)Result = ActorContext->GetLevel()->SpawnActor
	(
		SpawnClass,
		(AActor*)SpawnOwner,
		SpawnName,
		&SpawnLocation,
		&SpawnRotation
	);

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 278, execSpawn );

static void execDestroy( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execDestroy);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_FINISH;
	
	*(DWORD*)Result = ActorContext->GetLevel()->DestroyActor( ActorContext );

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 279, execDestroy );

////////////
// Timing //
////////////

static void execSetTimer( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSetTimer);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_FLOAT(NewTimerRate);
	P_GET_BOOL(bLoop);
	P_FINISH;

	ActorContext->TimerCounter = 0.0;
	ActorContext->TimerRate    = NewTimerRate;
	ActorContext->bTimerLoop   = bLoop;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 280, execSetTimer );

static void execSetTickRate( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(execSetTickRate);
	debugState(Context!=NULL);
	debugState(Context->IsA("Actor"));
	AActor *ActorContext = (AActor*)Context;

	P_GET_FLOAT(NewTickRate);
	P_FINISH;

	ActorContext->TickCounter = 0.0;
	ActorContext->TickRate    = NewTickRate;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( 281, execSetTickRate );

/* Prototype code for an intrinsic function.
static void exec( FExecStack &Stack, UObject *Context, BYTE *&Result )
{
	guardSlow(exec);

	P_FINISH;

	unguardexecSlow;
}
AUTOREGISTER_INTRINSIC( ?, exec );
*/

/*-----------------------------------------------------------------------------
	Intrinsic registry.
-----------------------------------------------------------------------------*/

//
// Register an intrinsic function.
// Called at startup time, before engine initialization.
//
int UNENGINE_API RegisterIntrinsic
(
	int iIntrinsic,
	void (*Func)( FExecStack &Stack, UObject *Context, BYTE *&Result )
)
{
	static int Initialized = 0;
	if( !Initialized )
	{
		Initialized = 1;
		for( int i=0; i<EX_Max; i++ )
			GIntrinsics[i] = execUndefined;
	}
	if( iIntrinsic<0 || iIntrinsic>ARRAY_COUNT(GIntrinsics) || GIntrinsics[iIntrinsic]!=execUndefined) 
		GIntrinsicDuplicate = 1;
	GIntrinsics[iIntrinsic] = Func;
	return 0;
}

/*-----------------------------------------------------------------------------
	Script processing function.
-----------------------------------------------------------------------------*/

//
// Script processing function.
// Should return 1 if processed, 0 if not.
//
void UObject::Process( FName Message, PMessageParms *Parms )
{
	guard(UObject::Process);
	AActor *Actor = (AActor*)this;

	if( !IsA("Actor") || Actor->bDeleteMe )
		return;
	if( Actor->GetLevel()->GetState() != LEVEL_UpPlay )
		return;

	// Validate.
	debugState(Class!=NULL);
	debugState(Class->Script!=NULL);
	debugState(Class->StackTree!=NULL);
	debugState(Class->StackTree->Num>0);
	debugState(MainStack.Link.Class!=NULL);
	debugState(MainStack.Link.iNode!=MAXWORD);
	debugState(MainStack.Object==this);

	// Make sure we're probing for this message.
	if( IsProbing( Message ) )
	{
		// Traverse the current stack node to find the specified function.
		clock(GServer.ScriptExecTime);
		INT            IsState = IsA("Actor") && (((AActor*)this)->State!=NAME_None);
		FStackNodeLink *Link   = &MainStack.Link.Node().ChildFunctions;
		FStackNode     *Node;
		Recheck:
		while( Link->Class != NULL )
		{
			Node = &Link->Node();
			if( Node->Name == Message )
			{
				//debugf("Found %i %s - %s",iMe,Class->GetName(),Message());
				//debugf("%s LocalsSize=%i",Message(),Node->LocalsSize);

				// Calling an intrinsic function from the engine is illegal, because the
				// intrinsics call script parsing functions.
				checkState(Node->iIntrinsic==0);
				checkState(Node->iCode!=MAXWORD);
				debugState(Node->ParmsSize==0 || Parms!=NULL);

				// If an undefined engine function, skip it.
				if( !(Node->StackNodeFlags & SNODE_DefinedFunc) )
					goto Unfound;

				// Create a new local execution stack.
				FMemMark Mark(GMem);
				FExecStack NewStack( this, *Link, new(GMem,MEM_Zeroed,Node->LocalsSize)BYTE );
				memcpy( NewStack.Locals, Parms, Node->ParmsSize );

				// Parse the outparms.
				BYTE *Dest = (BYTE*)Parms;
				BYTE *Src  = NewStack.Locals;
				FOutParmRec Outs[MAX_FUNC_PARMS], *Out = Outs;
				while( (Out->Size = *NewStack.Code++) != 0 )
				{
					debugState(*NewStack.Code==0 || *NewStack.Code==1);
					Out->Src  = Src;
					Out->Dest = Dest;
					Src      += Out->Size;
					Dest     += Out->Size;
					Out      += *NewStack.Code++;
				}

				// Execute the script code.
				BYTE Buffer[MAX_CONST_SIZE], *Addr, B;
				while( (B = *NewStack.Code++) != EX_Return )
					(*GIntrinsics[B])( NewStack, this, Addr=Buffer );

				// Copy outparms and return value back.
				while( --Out >= Outs )
					memcpy( Out->Dest, Out->Src, Out->Size );

				// Restore locals bin.
				Mark.Pop();

				unclock(GServer.ScriptExecTime);
				return;
			}
			Link = &Node->Next;
		}
		if( IsState )
		{
			// Failed to find a state version of the function, so check for a global version.
			IsState  = 0;
			Link     = &GetClass()->StackTree->Element(0).ChildFunctions;
			goto Recheck;
		}
		unclock(GServer.ScriptExecTime);
	}

	Unfound:;
	unguardf(("(class %s, func %s)",GetClassName(),Message()));
}

/*-----------------------------------------------------------------------------
	UScript resource implementation.
-----------------------------------------------------------------------------*/

//
// Serialize an expression to an archive.
// Returns expression token.
//
static EExprToken SerializeExpr( UScript *Script, INT &iCode, FArchive &Ar )
{
	guard(SerializeExpr);

	// Get expr token.
	Ar << Script->Element(iCode);
	EExprToken Expr = (EExprToken)Script->Element(iCode++);
	if( Expr >= EX_MinConversion && Expr < EX_MaxConversion )
	{
		// A type conversion.
		SerializeExpr( Script, iCode, Ar );
	}
	else if( Expr >= EX_FirstIntrinsic )
	{
		// Intrinsic final function with id 1-127.
		while( SerializeExpr( Script, iCode, Ar ) != EX_EndFunctionParms );
	}
	else if( Expr >= EX_ExtendedIntrinsic )
	{
		// Intrinsic final function with id 128-16383.
		Ar << Script->Element(iCode++);
		while( SerializeExpr( Script, iCode, Ar ) != EX_EndFunctionParms );
	}
	else switch( Expr )
	{
		case EX_LocalVariable:
		case EX_ObjectVariable:
		case EX_StaticVariable:
		case EX_UnusedVariable:
		case EX_DefaultVariable:
		{
			Ar << *(WORD*)&Script->Element(iCode); iCode+=sizeof(WORD);
			break;
		}
		case EX_BoolVariable:
		{
			Ar << Script->Element(iCode);
			Expr = (EExprToken)(Script->Element(iCode++) >> 5);
			checkState( Expr==EX_LocalVariable || Expr==EX_ObjectVariable || Expr==EX_StaticVariable || Expr==EX_DefaultVariable );
			Ar << *(WORD*)&Script->Element(iCode); iCode+=sizeof(WORD);
			break;
		}
		case EX_Nothing:
		case EX_EndFunctionParms:
		case EX_IntZero:
		case EX_IntOne:
		case EX_True:
		case EX_False:
		case EX_NoObject:
		case EX_Self:
		{
			break;
		}
		case EX_Context:
		{
			SerializeExpr( Script, iCode, Ar );              // Actor expression.
			Ar << *(WORD*)&Script->Element(iCode); iCode+=2; // Skip offset.
			Ar << *(BYTE*)&Script->Element(iCode++);         // Skip size.
			SerializeExpr( Script, iCode, Ar );              // Context expression.
			break;
		}
		case EX_ArrayElement:
		{
			SerializeExpr( Script, iCode, Ar );
			SerializeExpr( Script, iCode, Ar );
			Ar << Script->Element(iCode++);
			Ar << Script->Element(iCode++);
			break;
		}
		case EX_VirtualFunction:
		{
			Ar << *(FName *)&Script->Element(iCode); iCode+=sizeof(FName);
			while( SerializeExpr( Script, iCode, Ar ) != EX_EndFunctionParms );
			break;
		}
		case EX_FinalFunction:
		{
			Ar << *(FStackNodeLink*)&Script->Element(iCode); iCode+=sizeof(FStackNodeLink);
			while( SerializeExpr( Script, iCode, Ar ) != EX_EndFunctionParms );
			break;
		}
		case EX_IntConst:
		{
			Ar << *(INT *)&Script->Element(iCode); iCode+=sizeof(INT);
			break;
		}
		case EX_FloatConst:
		{
			Ar << *(FLOAT *)&Script->Element(iCode); iCode+=sizeof(FLOAT);
			break;
		}
		case EX_StringConst:
		{
			do Ar << Script->Element(iCode++); while( Script->Element(iCode-1) != 0);
			break;
		}
		case EX_ObjectConst:
		{
			Ar << *(UObject **)&Script->Element(iCode); iCode+=sizeof(UObject*);
			break;
		}
		case EX_NameConst:
		{
			Ar << *(FName *)&Script->Element(iCode); iCode+=sizeof(FName);
			break;
		}
		case EX_RotationConst:
		{
			Ar << *(WORD*)&Script->Element(iCode+0);
			Ar << *(WORD*)&Script->Element(iCode+2);
			Ar << *(WORD*)&Script->Element(iCode+4);
			iCode+=6;
			break;
		}
		case EX_VectorConst:
		{
			Ar << *(FVector *)&Script->Element(iCode); iCode+=sizeof(FVector);
			break;
		}
		case EX_ByteConst:
		{
			Ar << Script->Element(iCode++);
			break;
		}
		case EX_ResizeString:
		{
			Ar << Script->Element(iCode++);
			SerializeExpr( Script, iCode, Ar );
			break;
		}
		case EX_IntConstByte:
		{
			Ar << Script->Element(iCode++);
			break;
		}
		case EX_ActorCast:
		{
			Ar << *(UClass **)&Script->Element(iCode); iCode+=sizeof(UClass*);
			SerializeExpr( Script, iCode, Ar );
			break;
		}
		case EX_EndCode:
		case EX_Stop:
		case EX_Return:
		{
			break;
		}
		case EX_JumpIfNot:
		{
			Ar << *(WORD*)&Script->Element(iCode); iCode+=2; // Code offset.
			SerializeExpr( Script, iCode, Ar );              // Boolean expr.
			break;
		}
		case EX_Switch:
		{
			Ar << Script->Element(iCode++);     // Size.
			SerializeExpr( Script, iCode, Ar ); // Switch expr.
			break;
		}
		case EX_Jump:
		{
			Ar << *(WORD*)&Script->Element(iCode); iCode+=2; // Code offset.
			break;
		}
		case EX_Assert:
		{
			Ar << *(WORD*)&Script->Element(iCode); iCode += 2; // Line number.
			SerializeExpr( Script, iCode, Ar );                // Assert expr.
			break;
		}
		case EX_Case:
		{
			WORD *W=(WORD*)&Script->Element(iCode); Ar << *W; iCode+=2; // Code offset.
			if( *W != MAXWORD ) SerializeExpr( Script, iCode, Ar );     // Boolean expr.
			break;
		}
		case EX_LabelTable:
		{
			checkState((iCode&3)==0);
			for( ; ; )
			{
				FLabelEntry *E = (FLabelEntry*)&Script->Element(iCode);
				Ar << *E; iCode+=sizeof(FLabelEntry);
				if( E->Name == NAME_None ) break;
			}
			break;
		}
		case EX_GotoLabel:
		{
			SerializeExpr( Script, iCode, Ar ); // Label name expr.
			break;
		}
		case EX_Broadcast:
		{
			SerializeExpr( Script, iCode, Ar );              // Name expr.
			SerializeExpr( Script, iCode, Ar );              // Class expr.
			Ar << *(WORD*)&Script->Element(iCode); iCode+=2; // Skip offset;
			SerializeExpr( Script, iCode, Ar );              // Function call expr.
			break;
		}
		case EX_Let:
		{
			Ar << Script->Element(iCode++);     // Size.
			SerializeExpr( Script, iCode, Ar ); // Variable expr.
			SerializeExpr( Script, iCode, Ar ); // Assignment expr.
			break;
		}
		case EX_Let1:
		case EX_Let4:
		case EX_LetBool:
		case EX_LetString:
		{
			SerializeExpr( Script, iCode, Ar ); // Variable expr.
			SerializeExpr( Script, iCode, Ar ); // Assignment expr.
			break;
		}
		case EX_Skip:
		{
			Ar << *(WORD*)&Script->Element(iCode); iCode+=2; // Skip size.
			SerializeExpr( Script, iCode, Ar );              // Expression to possibly skip.
			break;
		}
		case EX_BeginFunction:
		{
			for( ; ; )
			{
				Ar << Script->Element(iCode);		// Parm size.
				if( Script->Element(iCode++) == 0 )
					break;
				Ar << Script->Element(iCode++);		// OutParm flag.
			}
			break;
		}
		default:
		{
			appErrorf( "Bad expr token %02x", Expr );
			break;
		}
	}
	return Expr;
	unguard;
}

//
// Serialize a script's data to an archive.
//
void UScript::SerializeData( FArchive &Ar )
{
	guard(UScript::SerializeData);
	INT iCode=0;

	// Serialize all code.
	while( iCode < Num )
		SerializeExpr( this, iCode, Ar );

	checkState(iCode==Num);
	unguard;
}
IMPLEMENT_DB_CLASS(UScript);

/*-----------------------------------------------------------------------------
	UStackTree.
-----------------------------------------------------------------------------*/

//
// FStackNode serializer.
//
FArchive& operator<<( FArchive& Ar, FStackNode &Node )
{
	guard(FStackNode::<<);

	// Archive the stuff.
	Ar << Node.Parent << Node.ChildFunctions << Node.ChildStates << Node.Next;
	Ar << Node.Name << Node.iCode << Node.CodeLabelOffset << Node.Line << Node.Pos;
	Ar << Node.NestType << Node.StackNodeFlags << Node.NumParms << Node.OperPrecedence;

	if( Node.NestType==NEST_Class || Node.NestType==NEST_State )
	{
		// Class/State-specific union info.
		Ar << Node.ProbeMask << Node.IgnoreMask;
	}
	else
	{
		// Function info.
		Ar << Node.LocalsSize;
		Ar << Node.ParmsSize << Node.iIntrinsic;
		Ar << Node.iFirstProperty << Node.NumProperties;

		// Set intrinsic function.
		if( Node.iIntrinsic != 0 )
		{
			checkState(Node.iIntrinsic<EX_Max);
			checkState(GIntrinsics[Node.iIntrinsic] != NULL);
		}
	}

	return Ar;
	unguard;
}

IMPLEMENT_DB_CLASS(UStackTree);

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
