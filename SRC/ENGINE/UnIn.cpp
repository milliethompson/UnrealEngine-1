/*=============================================================================
	UnIn.cpp: Unreal input system.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Description:
	This subsystem contains all input for one camera.  The input is generated 
	by the platform-specific code (mainly UnWnCam.cpp), which feeds it into the
	subsystem for processing via FInput::Process.  The input subsystem then 
	translates the input into command strings which are Exec'd.  This enables 
	input to be mapped onto buttons, axis movements, and Unreal engine commands
	equally well.

Related files:
	UnIn.h:	  The engine input system definition.
	UnIn.cpp: The engine input system implementation.
	Bind.mac: Key bindings.

Revision history:
	* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	FInput.
-----------------------------------------------------------------------------*/

//
// The input system implementation.
//
class FInput : public FInputBase
{
public:
	// FInputBase interface.
	void Init( UCamera *Camera, FGlobalPlatform *App );
	void Exit();
	int Exec( const char *Cmd,FOutputDevice *Out );
	void SaveBindings( FOutputDevice &Out );
	void ReadInput( PPlayerTick &Move, FLOAT DeltaSeconds, FOutputDevice *Out );
	const char *GetButtonName( EButtons Button ) const;
	const char *GetAxisName( EAxis Axis ) const;
	const char *GetKeyName( EInputKey Key ) const;
	int FindKeyName( const char *KeyName, EInputKey &iKey ) const;
	int FindButtonName( const char *ButtonName, EButtons &iButton) const;
	int FindAxisName( const char *AxisName, EAxis &iAxis) const;
	void Bind( EInputKey iKey, const char *Command );
	int Process( FOutputDevice &Out, EInputKey iKey, EInputState State, FLOAT Delta=0.0 );
	void ResetInput();

	// Button constants that accumulate to indicate button state.
	enum {BUTTON_NORMAL  = 0x00001};
	enum {BUTTON_TOGGLE  = 0x10000};

	// Constructors.
	FInput()
		{Initialized=0;}

private:
	// Bindings functions.
	void InitBindings();

	// Variables.
	UCamera			*Camera;
	FGlobalPlatform	*GApp;
	char			*Bindings[IK_MAX];
	int				Initialized;
	UEnumDef::Ptr	InputButtons;
	UEnumDef::Ptr	InputAxes;

	// Stored movements.
	DWORD			Buttons[BUT_MAX];
	FLOAT			Axes[AXIS_MAX];

	// Statics.
	static const char *InputKeyNames[IK_MAX];
};

/*-----------------------------------------------------------------------------
	Init & exit.
-----------------------------------------------------------------------------*/

//
// Init.
//
void FInput::Init( UCamera *InCamera, FGlobalPlatform *InApp )
{
	guard(FInput::Init);
	checkState(Initialized==0);
	Initialized=1;

	// Set objects.
	GApp   = InApp;
	Camera = InCamera;

	// Find the enums.
	InputButtons = new("EButton",  FIND_Existing)UEnumDef;
	InputAxes    = new("EAxis",    FIND_Existing)UEnumDef;

	// Init the bindings.
	for( int i=0; i<ARRAY_COUNT(Bindings); i++ )
		Bindings[i] = NULL;

	// Load the bindings into this camera.
	Camera->ExecMacro(DEFAULT_BINDINGS_FNAME);

	// Set the state.
	SetInputState(IST_None);

	// Reset.
	ResetInput();

	debugf(LOG_Init,"Input system initialized for %s",Camera->GetName());
	unguard;
}

//
// Exit.
//
void FInput::Exit()
{
	guard(FInput::Exit);
	checkState(Initialized==1);

	// Free the bindings.
	for( int i=0; i<ARRAY_COUNT(Bindings); i++ )
		if( Bindings[i] )
			appFree( Bindings[i] );

	debugf(LOG_Exit,"Input system shut down for %s",Camera->GetName());
	Initialized=0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/

//
// Execute a command.
//
int FInput::Exec( const char *Str, FOutputDevice *Out )
{
	guard(FInput::Exec);
	char Temp[256];

	if( GetCMD(&Str,"BIND") )
	{
		// Bind an input key to a command string.
		char KeyName[64];
		EInputKey iKey;
		if( !GrabSTRING(Str,KeyName,ARRAY_COUNT(KeyName)) )
		{
			Out->Logf( "Missing key name" );
			return 1;
		}
		else if( !FindKeyName( KeyName, iKey ) )
		{
			Out->Logf("Unrecognized key name <%s>",KeyName);
			return 1;
		}
		else
		{
			// Get possibly-blank key binding.
			GrabSTRING( Str, Temp, ARRAY_COUNT(Temp) );
			Out->Logf("Binding <%s> to <%s>",GetKeyName(iKey),Temp);
			Bind( iKey, Temp );
			return 1;
		}
	}
	else if( GetCMD(&Str,"RESETBINDINGS") )
	{
		ResetInput();
		Out->Log("Bindings reset");
		return 1;
	}
	else if( GetCMD(&Str,"SHOWBINDINGS") )
	{
		SaveBindings(*Out);
		return 1;
	}
	else if( GetCMD(&Str,"BUTTON") )
	{
		// Normal button.
		EButtons iButton;
		if( GrabSTRING(Str,Temp,ARRAY_COUNT(Temp)) && FindButtonName(Temp,iButton) )
		{
			if( GetInputState() == IST_Press )
				Buttons[iButton] += BUTTON_NORMAL;
			else if( GetInputState() == IST_Release )
				Buttons[iButton] -= BUTTON_NORMAL;
		}
		else Out->Log( "Bad Button command" );
		return 1;
	}
	else if( GetCMD(&Str,"TOGGLE") )
	{
		// Toggle button.
		EButtons iButton;
		if( GrabSTRING(Str,Temp,ARRAY_COUNT(Temp)) && FindButtonName(Temp,iButton) )
		{
			if( GetInputState() == IST_Press )
				Buttons[iButton] ^= BUTTON_TOGGLE;
		}
		else Out->Log("Bad Toggle command");
		return 1;
	}
	else if( PeekCMD(Str,"AXIS") || PeekCMD(Str,"DELTA") )
	{
		// Axis movement.
		int IsAxis  = GetCMD(&Str,"AXIS");
		int IsDelta = GetCMD(&Str,"DELTA");
		EAxis iAxis;
		if( GrabSTRING( Str, Temp, ARRAY_COUNT(Temp) ) && FindAxisName( Temp, iAxis ) )
		{
			FLOAT Speed=1.0;
			GetFLOAT(Str,"SPEED=",&Speed);
			if( GetInputState() == IST_Axis )
			{
				Axes[iAxis] += 0.01 * GetInputDelta() * Speed;
			}
			else if( GetInputState() == IST_Hold )
			{
				Axes[iAxis] += GetInputDelta() * Speed;
			}
		}
		else Out->Logf("Bad %s command",IsAxis ? "Axis" : "Delta");
		return 1;
	}
	else
	{
		return 0;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Bindings.
-----------------------------------------------------------------------------*/

//
// Bind an input key to a command.
//
void FInput::Bind( EInputKey iKey, const char *Command )
{
	guard(FInput::Bind);

	// Free existing binding.
	if( Bindings[iKey] != NULL )
		appFree( Bindings[iKey] );

	if( Command==NULL || Command[0]==0 )
	{
		// Null binding.
		Bindings[iKey] = NULL;
	}
	else
	{
		// Actual binding.
		Bindings[iKey] = (char *)appMalloc(strlen(Command)+1,"Binding");
		strcpy(Bindings[iKey],Command);
	}
	unguard;
}

//
// Save bindings to an output device.
//
void FInput::SaveBindings( FOutputDevice &Out )
{
	guard(FInput::SaveBindings);
	for( int i=0; i<ARRAY_COUNT(Bindings); i++ )
	{
		if( Bindings[i] )
			Out.Logf("Bind \"%s\" \"%s\"",GetKeyName((EInputKey)i),Bindings[i]);
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Key and axis movement processing.
-----------------------------------------------------------------------------*/

//
// Process input. Returns 1 if handled, 0 if not.
//
int FInput::Process( FOutputDevice &Out, EInputKey iKey, EInputState State, FLOAT Delta )
{
	guard(FInput::Process);
	int Result = 0;

	if( iKey>=0 && iKey<IK_MAX )
	{
		// Update key flags.
		switch( State )
		{
			case IST_Press:
				if( KeyDownTable[iKey] ) return 0;
				KeyDownTable[iKey] = 1;
				break;
			case IST_Release:
				KeyDownTable[iKey] = 0;
				break;
		}

		// Handle binding.
		if( Bindings[iKey]!=NULL )
		{
			// Process the key or axis movement.
			const char *Action = Bindings[iKey];
			char Line[256];
			SetInputState(State,Delta);

			// Process each line of the binding string.
			while( GetLINE( &Action, Line, ARRAY_COUNT(Line))==0 )
			{
				int DoExec = 0;
				switch( State )
				{
					case IST_Press:
						// Unconditionally execute the command.
						DoExec = 1;
						break;
					case IST_Hold:
						// Execute only Axis.
						DoExec = PeekCMD( Line, "AXIS" );
						break;
					case IST_Release:
						// Execute only Button.
						DoExec = PeekCMD( Line, "BUTTON" );
						break;
					case IST_Axis:
						// Execute only Axis.
						DoExec = PeekCMD( Line, "AXIS" );
						break;
					default:
						appError( "Unrecognized input state" );
						break;
				}
				if( DoExec )
				{
					//debugf("Processing <%s>",Bindings[iKey]);
					Camera->Exec(Line,&Out);
				}
			}
			SetInputState(IST_None);
			Result = 1;
		}
	}
	return Result;
	unguard;
}

/*-----------------------------------------------------------------------------
	Input reading.
-----------------------------------------------------------------------------*/

//
// Read input for the camera.
//
void FInput::ReadInput( PPlayerTick &PlayerTick, FLOAT DeltaSeconds, FOutputDevice *Out )
{
	guard(FInput::ReadInput);

	// Antidiscretization coefficient.
	const FLOAT k = 100.0;

	// Update everything with IST_Hold.
	for( int i=0; i<IK_MAX; i++ )
		if( KeyDownTable[i] )
			Process( *GApp, (EInputKey)i, IST_Hold, DeltaSeconds );

	// Get axis velocities.
	for( i=0; i<AXIS_MAX; i++ )
	{
		FLOAT Velocity      = 20.0 * Axes[i] / DeltaSeconds;
		PlayerTick.Axis[i]  = Velocity;
		//Axes[i]            *= (1.0 - exp(-k * DeltaSeconds)) / (k * DeltaSeconds);
		Axes[i] = 0.0;
	}

	// Get buttons.
	for( i=0; i<BUT_MAX; i++ )
		PlayerTick.Buttons[i]  = Buttons[i] ? 1 : 0;

	unguard;
}

/*-----------------------------------------------------------------------------
	Input resetting.
-----------------------------------------------------------------------------*/

//
// Reset the input system's state.
//
void FInput::ResetInput()
{
	guard(FInput::ResetInput);

	// Reset all keys.
	for( int i=0; i<IK_MAX; i++ ) KeyDownTable[i]=0;

	// Reset all buttons.
	for( i=0; i<BUT_MAX; i++ ) Buttons[i]=0;

	// Reset all axes.
	for( i=0; i<AXIS_MAX; i++ ) Axes[i]=0.0;

	unguard;
}

/*-----------------------------------------------------------------------------
	Utility functions.
-----------------------------------------------------------------------------*/

//
// Return the name of a button.
//
const char *FInput::GetButtonName( EButtons Button ) const
{
	guard(FInput::GetButtonName);
	checkInput(Button>=0 && Button<BUT_MAX);
	return InputButtons(Button)();
	unguard;
}

//
// Return the name of an axis.
//
const char *FInput::GetAxisName( EAxis Axis ) const
{
	guard(FInput::GetAxisName);
	checkInput(Axis>=0 && Axis<AXIS_MAX);
	return InputAxes(Axis)();
	unguard;
}

//
// Return the name of a key.
//
const char *FInput::GetKeyName( EInputKey Key ) const
{
	guard(FInput::GetKeyName);
	if( Key>=0 && Key<IK_MAX )
		return InputKeyNames[Key];
	else
		return "Unknown";
	unguard;
}

//
// Find the index of a named key.
//
int FInput::FindKeyName( const char *KeyName, EInputKey &iKey ) const
{
	guard(FInput::FindKeyName);
	if( *KeyName )
	{
		for( int i=0; i<IK_MAX; i++ )
		{
			if( stricmp(KeyName,InputKeyNames[i]) == 0 )
			{
				// Found it.
				iKey = (EInputKey)i;
				return 1;
			}
		}
	}

	// Didn't find a match.
	return 0;
	unguard;
}

//
// Find the index of a named button.
//
int FInput::FindButtonName( const char *ButtonName, EButtons &iButton) const
{
	guard(FInput::FindButtonName);
	return InputButtons->FindNameIndex(ButtonName,*(INDEX*)&iButton);
	unguard;
}

//
// Find the index of a named axis.
//
int FInput::FindAxisName( const char *AxisName, EAxis &iAxis) const
{
	guard(FInput::FindAxisName);
	return InputAxes->FindNameIndex(AxisName,*(INDEX*)&iAxis);
	unguard;
}

/*-----------------------------------------------------------------------------
	Statics.
-----------------------------------------------------------------------------*/

// Names of all keys.
const char *FInput::InputKeyNames[IK_MAX] =
{
/*00*/	"None"		,"LButton"		,"RButton"		,"Cancel"		,
/*04*/	"MButton"	,"Unknown05"	,"Unknown06"	,"Unknown07"	,
/*08*/	"Backspace"	,"Tab"			,"Unknown0A"	,"Unknown0B"	,
/*0C*/	"Unknown0C"	,"Enter"		,"Unknown0E"	,"Unknown0F"	,
/*10*/	"Shift"		,"Ctrl"			,"Alt"			,"Pause"		,
/*14*/	"CapsLock"	,"Unknown15"	,"Unknown16"	,"Unknown17"	,
/*18*/	"Unknown18"	,"Unknown19"	,"Unknown1A"	,"Escape"		,
/*1C*/	"Unknown1C"	,"Unknown1D"	,"Unknown1E"	,"Unknown1F"	,
/*20*/	"Space"		,"PageUp"		,"PageDown"		,"End"			,
/*24*/	"Home"		,"Left"			,"Up"			,"Right"		,
/*28*/	"Down"		,"Select"		,"Print"		,"Execute"		,
/*2C*/	"PrintScrn"	,"Insert"		,"Delete"		,"Help"			,
/*30*/	"0"			,"1"			,"2"			,"3"			,
/*34*/	"4"			,"5"			,"6"			,"7"			,
/*38*/	"8"			,"9"			,"Unknown3A"	,"Unknown3A"	,
/*3C*/	"Unknown3C"	,"Unknown3D"	,"Unknown3E"	,"Unknown3F"	,
/*40*/	"Unknown40"	,"A"			,"B"			,"C"			,
/*44*/	"D"			,"E"			,"F"			,"G"			,
/*48*/	"H"			,"I"			,"J"			,"K"			,
/*4C*/	"L"			,"M"			,"N"			,"O"			,
/*50*/	"P"			,"Q"			,"R"			,"S"			,
/*54*/	"T"			,"U"			,"V"			,"W"			,
/*58*/	"X"			,"Y"			,"Z"			,"Unknown5B"	,
/*5C*/	"Unknown5C"	,"Unknown5D"	,"Unknown5E"	,"Unknown5F"	,
/*60*/	"NumPad0"	,"NumPad1"		,"NumPad2"		,"NumPad3"		,
/*64*/	"NumPad4"	,"NumPad5"		,"NumPad6"		,"NumPad7"		,
/*68*/	"NumPad8"	,"NumPad9"		,"GreyStar"		,"GreyPlus"		,
/*6C*/	"Separator"	,"GreyMinus"	,"NumPadPeriod"	,"GreySlash"	,
/*70*/	"F1"		,"F2"			,"F3"			,"F4"			,
/*74*/	"F5"		,"F6"			,"F7"			,"F8"			,
/*78*/	"F9"		,"F10"			,"F11"			,"F12"			,
/*7C*/	"F13"		,"F14"			,"F15"			,"F16"			,
/*80*/	"F17"		,"F18"			,"F19"			,"F20"			,
/*84*/	"F21"		,"F22"			,"F23"			,"F24"			,
/*88*/	"Unknown88"	,"Unknown89"	,"Unknown8A"	,"Unknown8B"	,
/*8C*/	"Unknown8C"	,"Unknown8D"	,"Unknown8E"	,"Unknown8F"	,
/*90*/	"NumLock"	,"ScrollLock"	,"Unknown92"	,"Unknown93"	,
/*94*/	"Unknown94"	,"Unknown95"	,"Unknown96"	,"Unknown97"	,
/*98*/	"Unknown98"	,"Unknown99"	,"Unknown9A"	,"Unknown9B"	,
/*9C*/	"Unknown9C"	,"Unknown9D"	,"Unknown9E"	,"Unknown9F"	,
/*A0*/	"LShift"	,"RShift"		,"LControl"		,"RControl"		,
/*A4*/	"UnknownA4"	,"UnknownA5"	,"UnknownA6"	,"UnknownA7"	,
/*A8*/	"UnknownA8"	,"UnknownA9"	,"UnknownAA"	,"UnknownAB"	,
/*AC*/	"UnknownAC"	,"UnknownAD"	,"UnknownAE"	,"UnknownAF"	,
/*B0*/	"UnknownB0"	,"UnknownB1"	,"UnknownB2"	,"UnknownB3"	,
/*B4*/	"UnknownB4"	,"UnknownB5"	,"UnknownB6"	,"UnknownB7"	,
/*B8*/	"UnknownB8"	,"UnknownB9"	,";"			,"="			,
/*BC*/	","			,"-"			,"."			,"/"			,
/*C0*/	"`"			,"UnknownC1"	,"UnknownC2"	,"UnknownC3"	,
/*C4*/	"UnknownC4"	,"UnknownC5"	,"UnknownC6"	,"UnknownC7"	,
/*C8*/	"UnknownC8"	,"UnknownC9"	,"UnknownCA"	,"UnknownCB"	,
/*CC*/	"UnknownCC"	,"UnknownCD"	,"UnknownCE"	,"UnknownCF"	,
/*D0*/	"UnknownD0"	,"UnknownD1"	,"UnknownD2"	,"UnknownD3"	,
/*D4*/	"UnknownD4"	,"UnknownD5"	,"UnknownD6"	,"UnknownD7"	,
/*D8*/	"UnknownD8"	,"UnknownD9"	,"UnknownDA"	,"["			,
/*DC*/	"\\"		,"]"			,"'"			,"UnknownDF"	,
/*E0*/	"UnknownE0"	,"UnknownE1"	,"UnknownE2"	,"UnknownE3"	,
/*E4*/	"UnknownE4"	,"UnknownE5"	,"UnknownE6"	,"UnknownE7"	,
/*E8*/	"UnknownE8"	,"UnknownE9"	,"UnknownEA"	,"UnknownEB"	,
/*EC*/	"UnknownEC"	,"UnknownED"	,"UnknownEE"	,"UnknownEF"	,
/*F0*/	"UnknownF0"	,"UnknownF1"	,"UnknownF2"	,"UnknownF3"	,
/*F4*/	"UnknownF4"	,"UnknownF5"	,"Attn"			,"CrSel"		,
/*F8*/	"ExSel"		,"ErEof"		,"Play"			,"Zoom"			,
/*FC*/	"NoName"	,"PA1"			,"OEMClear"		,"UnknownFF"	,
/*100*/ "JoyX"		,"JoyY"			,"JoyZ"			,"JoyW"			,
/*104*/	"MouseX"	,"MouseY"		,"MouseZ"		,"MouseW"		,
/*108*/	"Joy1"		,"Joy2"			,"Joy3"			,"Joy4"			,
};

/*-----------------------------------------------------------------------------
	Instantiator.
-----------------------------------------------------------------------------*/

//
// Create a new input subsystem.
//
FInputBase *NewInput()
{
	guard(NewInput);
	return new FInput;
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
