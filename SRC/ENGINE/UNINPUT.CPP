/*=============================================================================
	UnInput.cpp: Player input

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Definitions:

    combo: A set of inputs which together define a player action. The simplest
        combo can be a single input, which is called a combo even though it
        has only one input. For example, the CursorUp key might be the combo 
        which performs the MoveForward action. A combo can have one or two  
        inputs, as listed below. Note that a combo has at most one 
        movement. A combo with one input has no modifiers and a combo with
        two inputs has one modifier.
          1. input                      (input with no modifier)
          2. switch + input             (input with 1 modifier switch)
        When there is more than one input, the combo has 1 modifier
        switch and a main input. Here are some examples that you may be
        familiar with from other 3D action games:
          1. CursorLeft                 (turns the player to the left)
          2. Shift+CursorLeft           (turns the player faster to the left)
    
    input: A switch or a movement.

    modifier: A switch which forms part of a multiple-input combo. The switch
        is called a modifier because it is typically used to change the meaning
        of an input. For example, CursorLeft might mean TurnLeft, but 
        Alt+CursorLeft might mean MoveLeft. In this case, Alt is a modifier
        which changes the meaning of CursorLeft from TurnLeft to MoveLeft.
        Modifiers are always switches and never movements.

    movement: a mouse movement or a joystick movement. Each joystick or mouse
        axis of movement is separated into two distinct (but related) movements:
        the "positive" movement and the "negative" movement. For example, the
        mouse horizontal axis is separated into "mouse left" and "mouse right".
        The joystick's X-axis is separated into "positive X" and "negative X"
        movements. This lets actions or sensitivities be assigned to the 
        directions independently.

    movement combo: A combo where the main input is a movement.
        Example: Shift + MouseForward
   
    switch: a mouse button, a keyboard key, or a joystick button.

    switch combo: A combo where the main input is a switch.

Input combos:

    Input combos are briefly explained above under "combo". Here we provide
    more detail. Below are the most complex forms of a switch or modifier combo.

    Switch combo   : Modifier*2(T) + Switch*2(T)
    Movement combo : Modifier1*2(T) + Movement(Kind)
       
    Notes:
      1. Remember that the modifiers are optional and are always switches.
      2. "*2" is an optional indicator which means the switch is activated
         only when double-pressed (pressed twice quickly).
      3. "(T)" is an optional indicator which means the switch is toggled
         on or off each time it is activated.
      4. (Kind) is an optional definition of the kind of movement. This is
         described below.
     
    The movement kind defines how mouse or joystick movements are to be treated.
     (Digital)
         The movement is either on or off, depending on how far away the
         movement is from the neutral or center position. For example,
         moving the joystick forward might turn on the MoveForward action.
         Once you start moving, pushing the joystick further forward doesn't
         make you move any faster (this is the "digital" nature of the movement).
 
     (Analog)
         The position of the movement translates to the size or position of
         the player action. For example, if moving a joystick forward causes
         the player to move forward, then moving the joystick forward some
         more causes the player to move forward faster. 
     
     (Differential)
         The *change* in the movement translates to a change in the player's
         action. For example, moving the mouse forward a little moves the
         player forward a little. Note that it doesn't matter where the mouse
         is - it is the change in movement that results in the action.
         Differential movements are typically used for mouse movements.

When input combos are active:

    A switch combo is active if...
        A switch combo is active if its modifier (if any) is active and
        if the main switch input is active.

    A movement combo is active if ...
        A movement combo is active if its modifier (if any) is active and
        if the movement input is active.

    A switch (or modifier) is active if...
        Whether a switch is considered active depends on whether or not it is
        doubled (*2) or toggled (T). The cases are described below.
           Switch: (neither doubled nor toggled)
              Such a switch is active while it is held down.
           Switch*2: (doubled but not toggled)
              Such a switch is active while held down after being pressed
              twice quickly.
           Switch(T): (toggled but not doubled)
              Such a switch is active the first time it is pressed.
              Thereafter, the switch changes from active to inactive or
              inactive to active each time it is pressed again.
              Note that when the switch is active, it remains active even
              after the switch is released.
           Switch*2(T): (both doubled and toggled)
              Such a switch is active the first time it is double-pressed.
              Thereafter, the switch changes from active to inactive or
              inactive to active each time it is double-pressed again.

    For the purpose of explaining the activation of movements, consider
    the following definition:
        The relative position of a device in a particular direction is
        the distance from the center (neutral) position divided by the
        maximum distance from the center in that direction. This relative
        position is expressed as a percentage. If the device is not moved
        in the direction, or is moved in the opposite direction, the relative
        position is 0. The relative position is always non-negative.
    For example, if the joystick is pushed forward half-way between its
    rest position and its maximum position, then the relative position
    of the joystick in the "joystick forward" direction is 50%, and the
    relative position of the joystick in the "joystick backward" direction
    is 0%. The mouse is assigned maximum movements based on the screen size.

    Similarly, consider the following definition for movement changes:
        The relative change of a device in a particular direction is
        the change in that direction (from the previous position) divided by the
        maximum distance from the center in that direction. This relative
        position is expressed as a percentage. If the device is not moved
        in the direction, or is moved in the opposite direction, the relative
        change is 0. The relative change is always non-negative.
    For example, if the joystick is moved forward an extra 5 units, where
    the maximum joystick position in that direction is 50 units, then the
    relative change in the "joystick forward" direction is 10%, and the
    relative change of the joystick in the "joystick backward" direction
    is 0%.
    
    A Digital movement is active if...
        A Digital movement is active if the relative position of the device
        in the movement direction is not less than the digital threshold
        for the movement. Each movement is assigned a configurable digital 
        threshold.
        For example, if the "joystick left" movement has a digital threshold
        of 33%, then moving the joystick left within 1/3 of its "leftward"
        range won't active the digital movement. As soon as the joystick 
        moves to 1/3 of the range, the digital movement is active and it 
        remains active until the joystick is moved back to within 1/3 of 
        the leftward range. In this case, there is a 33% "dead zone"
        to the left of the center of the joystick.
    
    An Analog movement is active if...
        An analog movement is active if the relative position of the device in
        the movement direction is not less than the analog threshold for the
        movement. Each movement is assigned a configurable analog threshold.
        In addition, the position of the device is used to determine the
        size of the corresponding action.
    
    A Differential movement is active if...
        A differential movement is active if the relative change of the device in
        the movement direction is not less than the differential threshold for the
        movement. In addition, the size of the change is used to determine the
        size of the corresponding action.

=============================================================================*/

#include "Unreal.h"
#include "UnInput.h"
#include "UnKeyVal.h"
#include "UnParse.h"

#define DebugParsing 0 // 1 to debug parsing, 0 otherwise.

#define InputDebugging (DebugParsing)

static const char * ToggleText = "(T)";
static const char * DoubleText = "*2" ;

static const char * AnalogThresholdSuffix       = "ThresholdA"  ; // Identifies movement thresholds.
static const char * DifferentialThresholdSuffix = "ThresholdC"  ; // Identifies movement thresholds.
static const char * DigitalThresholdSuffix      = "ThresholdD"  ; // Identifies movement thresholds.
static const char * SpeedSuffix                 = "Speed"       ; // Identifies movement speed.
static const char * UseKey                      = "Use"         ;

static char TextBuffer[200]; // For returning short-lived strings.

static const char * InputSectionName = "Input";

FInput GInput;

//
// This is still somewhat hacked.
//
// To do:
//
// Move this over to MFC and Windows.  Input packets should be platform
// independent.  All input packet building routines should be entierly
// reliant on the platform's keyboard, joystick, and mouse functions.
//
// Integrate mouse movement far more closely with Windows in order
// to get _great_ responsiveness and logarythmic mouse movement, in all
// Windows modes (in a window, DirectDraw).

void pGetStoredMove(UCamera *Camera);

#define arrayCount_(Array) ( sizeof(Array) / sizeof((Array)[0]) )


#if InputDebugging
    static void CDECL Debug(const char * Message, ...)
    {
        char Text[1000];
        va_list ArgumentList;
        va_start(ArgumentList,Message);
        vsprintf(Text,Message,ArgumentList);
        va_end(ArgumentList);
        debugf(LOG_Info,Text);
    }
#else
    static inline void CDECL Debug(const char * Message, ...)
    {
    }
#endif

//----------------------------------------------------------------------------
//                 Parse any doubler DoubleText out of Text
//----------------------------------------------------------------------------
// Strip any leading whitespace off Text.
// If Text then starts with DoubleText:
//   - move Text beyond the DoubleText, and then past any trailing whitespace
//   - return TRUE
// Otherwise:
//   - return FALSE
static BOOL ParseDoubler( const char * & Text )
{   
    BOOL Found = FALSE;
    FParse::SkipWhiteSpace(Text);
    const int DoublerLength = strlen(DoubleText);
    if( strnicmp( Text, DoubleText, DoublerLength ) == 0 )
    {
        Text += DoublerLength; // Skip over doubler.
        FParse::SkipWhiteSpace(Text);
        Found = TRUE;
    }
    return Found;
}

//----------------------------------------------------------------------------
//                 Parse any toggler ToggleText out of Text
//----------------------------------------------------------------------------
// Strip any leading whitespace off Text.
// If Text then starts with ToggleText:
//   - move Text beyond the ToggleText, and then past any trailing whitespace
//   - return TRUE
// Otherwise:
//   - return FALSE
static BOOL ParseToggler( const char * & Text )
{   
    BOOL Found = FALSE;
    FParse::SkipWhiteSpace(Text);
    const int TogglerLength = strlen(ToggleText);
    if( strnicmp( Text, ToggleText, TogglerLength ) == 0 )
    {
        Text += TogglerLength; // Skip over Toggler.
        FParse::SkipWhiteSpace(Text);
        Found = TRUE;
    }
    return Found;
}

//----------------------------------------------------------------------------
//                 Parse a movement suffix out of Text
//----------------------------------------------------------------------------
// Remove any leading whitespace from Text.
// Look for a movement suffix: "(X)", where X is a movement kind.
// If found:
//   - move Text beyond the suffix, and beyond any trailing whitespace.
//   - return the appropriate movement kind.
// Otherwise:
//   - return FInput::NoMovementKind
static FInput::TMovementKind ParseMovementSuffix
(
    const char * & Text
,   const FInput & Input
)
{   
    const char * CheckText = Text;
    FInput::TMovementKind Kind = FPlatformInput::NoMovementKind;
    char Suffix[50];
    if
    ( 
            FParse::StartsWith(CheckText,'(') 
        &&  FParse::GetWord(CheckText,Suffix,sizeof(Suffix)-1) > 0
        &&  FParse::StartsWith(CheckText,')')
    )
    {
        for( int Check_ = 1; Kind==0 && Check_ < FPlatformInput::MovementKindCount; Check_++ )
        {
            const FInput::TMovementKind Check = FInput::TMovementKind(Check_);
            if
            ( 
                    stricmp( Suffix, Input.PlatformInput->Abbreviation(Check) ) == 0 
                ||  stricmp( Suffix, Input.PlatformInput->Description(Check) ) == 0 
            )
            {
                Kind = Check;
            }
        }
    }
    if( Kind != 0 )
    {
        FParse::SkipWhiteSpace(CheckText);
        Text = CheckText;
    }        
    return Kind;
}


/*------------------------------------------------------------------------------
	Input packet building
------------------------------------------------------------------------------*/

//
// Build a player movement packet based on keyboard, mouse, and joystick
// movement.  Buffer must have plenty of free space.  Returns length of packet.
//
int PPlayerTick::BuildAllMovement(UCamera *Camera)
	{
	GUARD;
	int Length = sizeof (PPlayerTick);
	//
	// Process buttons/keys:
	//
	if (Camera && Camera->Current)
    {
        Camera->Console->SetActionStatus(*this);
        #if 0 //todo: [Delete] (obsolete)
        FVector Move;
        FFloatRotation Rotation;
		GCameraManager->GetStoredMove(Camera,&Move,&Rotation); // Called only to get dmTick (direct mouse support) called
        memmove( &Movements, GAction.Movements, sizeof(Movements) );
        memmove( &Actions, GAction.Status, sizeof(GAction.Status) );
        GAction.EmptyMovements(); // Reset all movements since they might be accumulated later
        #endif
    }
	else
    {
        // No input source - we build a null movement packet
        memset( this, 0, Length ); // Assumes zero-ness of components is desired.
    }
	return Length;
	UNGUARD("PPlayerTick::BuildAllMovement");
	};

//----------------------------------------------------------------------------
//             Reset the state of all switches.
//----------------------------------------------------------------------------
void FInput::Reset()
{
    PlatformInput->Reset();
}

//----------------------------------------------------------------------------
//                    Initialize
//----------------------------------------------------------------------------
// Note: Caller is expected to set PlatformInput before initialization.
void FInput::Initialize(FPlatformInput * PlatformInput)
{
    GUARD;
    this->PlatformInput = PlatformInput;
    PlatformInput->Initialize();
    Switches = &PlatformInput->Switches[0];
    MovementsInfo = &PlatformInput->MovementsInfo[0];
    LoadValues();
    UNGUARD("FInput::Initialize");
}

//----------------------------------------------------------------------------
//                    Exit
//----------------------------------------------------------------------------
void FInput::Exit()
{
    PlatformInput->Exit();
    PlatformInput = 0;
}


//----------------------------------------------------------------------------
//              What are all the keys?
//----------------------------------------------------------------------------
FInput::TStringList FInput::Keys() const
{
    TStringList Keys;
    char Key[FKeyValues::MaxKeyLength];

    Keys.Add( UseKey );
    for( int Movement_ = 1; Movement_ < MovementCount; ++Movement_ )
    {
        const TMovement Movement = TMovement(Movement_);
        sprintf( Key, "%s%s", PlatformInput->Abbreviation(Movement), AnalogThresholdSuffix );
        Keys.Add( Key );
    }
    for( Movement_ = 1; Movement_ < MovementCount; ++Movement_ )
    {
        const TMovement Movement = TMovement(Movement_);
        sprintf( Key, "%s%s", PlatformInput->Abbreviation(Movement), DifferentialThresholdSuffix );
        Keys.Add( Key );
    }
    for( Movement_ = 1; Movement_ < MovementCount; ++Movement_ )
    {
        const TMovement Movement = TMovement(Movement_);
        sprintf( Key, "%s%s", PlatformInput->Abbreviation(Movement), DigitalThresholdSuffix );
        Keys.Add( Key );
    }
    for( Movement_ = 1; Movement_ < MovementCount; ++Movement_ )
    {
        const TMovement Movement = TMovement(Movement_);
        sprintf( Key, "%s%s", PlatformInput->Abbreviation(Movement), SpeedSuffix );
        Keys.Add( Key );
    }
    return Keys;
}

//----------------------------------------------------------------------------
//           What is the current value for the given key? 
//----------------------------------------------------------------------------
char * FInput::Value(const char * Key) const
{
    char SubKey[FKeyValues::MaxKeyLength+1]; // For when we break up the subkey.
    const int KeyLength = strlen(Key);
    char Value[FKeyValues::MaxValueLength+1]; // +1 for trailing null.
    Value[0] = 0;
    char * Text = Value; // Place in Value where we are adding text.
    if( stricmp(Key,UseKey)==0 )
    {
        //----------------------------------------------
        //             Use = ...
        //----------------------------------------------
        for( int Device_ = 0; Device_ < DeviceCount; Device_++ )
        {
            const TDevice Device = TDevice(Device_);
            Text += sprintf
            ( 
                Text
            ,   "%c%s"
            ,   PlatformInput->UsingDevice(Device) ? '+' : '-'
            ,   PlatformInput->DeviceName(Device) 
            );
        }
    }
    else if( FParse::EndsWith( Key, AnalogThresholdSuffix ) )
    {
        //----------------------------------------------
        //     Analog threshold for a movement
        //----------------------------------------------
        strcpy( SubKey, Key );
        SubKey[ KeyLength - strlen(AnalogThresholdSuffix) ] = 0; // Strip off the suffix.
        const TMovement Movement = FindMovement(SubKey);
        if( Movement != 0 )
        {
            sprintf( Value, "%2.3f", float(MovementsInfo[Movement].Sensitivity.AnalogThreshold) );
        }
    }
    else if( FParse::EndsWith( Key, DifferentialThresholdSuffix ) )
    {
        //----------------------------------------------
        //     Analog threshold for a movement
        //----------------------------------------------
        strcpy( SubKey, Key );
        SubKey[ KeyLength - strlen(DifferentialThresholdSuffix) ] = 0; // Strip off the suffix.
        const TMovement Movement = FindMovement(SubKey);
        if( Movement != 0 )
        {
            sprintf( Value, "%2.3f", float(MovementsInfo[Movement].Sensitivity.DifferentialThreshold) );
        }
    }
    else if( FParse::EndsWith( Key, DigitalThresholdSuffix ) )
    {
        //----------------------------------------------
        //     Digital threshold for a movement
        //----------------------------------------------
        strcpy( SubKey, Key );
        SubKey[ KeyLength - strlen(DigitalThresholdSuffix) ] = 0; // Strip off the suffix.
        const TMovement Movement = FindMovement(SubKey);
        if( Movement != 0 )
        {
            sprintf( Value, "%2.3f", float(MovementsInfo[Movement].Sensitivity.DigitalThreshold) );
        }
    }
    else if( FParse::EndsWith( Key, SpeedSuffix ) )
    {
        //----------------------------------------------
        //     Speed for a movement
        //----------------------------------------------
        strcpy( SubKey, Key );
        SubKey[ KeyLength - strlen(SpeedSuffix) ] = 0; // Strip off the suffix.
        const TMovement Movement = FindMovement(SubKey);
        if( Movement != 0 )
        {
            sprintf( Value, "%2.3f", float(MovementsInfo[Movement].Sensitivity.Speed) );
        }
    }
    else
    {
        //----------------------------------------------
        //           Unrecognized key
        //----------------------------------------------
    }
    return Value[0] == 0 ? 0 : FParse::MakeString(Value);
}

//----------------------------------------------------------------------------
//                   Section name
//----------------------------------------------------------------------------
const char * FInput::SectionName() const
{
    return InputSectionName;
}

//----------------------------------------------------------------------------
//            Set the value associated with a text key.
//----------------------------------------------------------------------------
BOOL FInput::SetValue(const char * Key, const char * Value)
{   
    GUARD;
    Debug( "SetValue(%s,%s)", Key, Value );
    BOOL IsOkay = FALSE;
    char SubValue[FKeyValues::MaxValueLength+1]; // +1 for trailing null.
    const int KeyLength = strlen(Key);
    const char * Text = Value;
    FParse::SkipWhiteSpace(Text);
    BOOL CheckEnd = TRUE; // TRUE if stuff at the end of the value needs to be checked.
    if( stricmp(Key,UseKey)==0 )
    {
        //----------------------------------------------
        //             Use = ...
        //----------------------------------------------
        // We stop after an error, end of line, or start of comment (';')
        IsOkay = TRUE;
        while( IsOkay && Text[0] != 0 && Text[0] != ';' )
        {
            Debug( "Checking: \"%s\"", Text );
            BOOL IsUsed = TRUE;
            if( FParse::StartsWith(Text,'+') )
            {
                IsUsed = TRUE;
            }
            else if( FParse::StartsWith(Text,'-') )
            {
                IsUsed = FALSE;
            }
            if( FParse::GetWord(Text,SubValue,FKeyValues::MaxValueLength) )
            {
                TDevice Device;
                BOOL Found = FALSE;
                for( int CheckDevice_ = 0; !Found && CheckDevice_ < DeviceCount; CheckDevice_++ )
                {
                    const TDevice CheckDevice = TDevice(CheckDevice_);
                    if( stricmp(SubValue,PlatformInput->DeviceName(CheckDevice)) == 0 )
                    {
                        Device = CheckDevice;
                        Found = TRUE;
                    }
                }
                if( !Found )
                {
                    IsOkay = FALSE;
                }
                else
                {
                    Debug( "%s found (%i): isused: %i", SubValue, int(Device), int(IsUsed) );
                    PlatformInput->UseDevice(Device,IsUsed);
                }
            }
        }
    }
    else
    {
        IsOkay = SetValue( Key, Value, MovementsInfo );
        CheckEnd = FALSE; // The SetValue() call checks the ending for us.
    }
    // Check for extraneous stuff on the line: (comments are okay - they start with ';')
    if( CheckEnd && IsOkay && !FParse::StartsWith(Text,';') && Text[0] != 0 )
    {
        IsOkay = FALSE;
    }
    return IsOkay;
    UNGUARD( "FInput::SetValue" );
}

//----------------------------------------------------------------------------
// Fill in Info (an array of MovementCount items) with the value interpreted from Key=Value.
//----------------------------------------------------------------------------
BOOL FInput::SetValue(const char * Key, const char * Value, TMovementInfo * Info )
{
    GUARD;
    //Debug( "SetValue(%s,%s)", Key, Value );
    BOOL IsOkay = FALSE;
    const int KeyLength = strlen(Key);
    char SubKey[FKeyValues::MaxKeyLength+1]; // For when we break up the subkey.
    const char * Text = Value;
    FParse::SkipWhiteSpace(Text);
    //tbi: Similarity of code for 3 different thresholds...
    if( FParse::EndsWith( Key, AnalogThresholdSuffix ) )
    {
        //----------------------------------------------
        //     Analog threshold (for a movement)
        //----------------------------------------------
        IsOkay = TRUE;
        strcpy( SubKey, Key );
        SubKey[ KeyLength - strlen(AnalogThresholdSuffix) ] = 0; // Strip off the suffix.
        const TMovement Movement = FindMovement(SubKey);
        float Threshold;
        if( Movement != 0 && FParse::GetFloat(Text,Threshold) )
        {
            if( Threshold > 0.0 && Threshold < 1.0 )
            {
                Info[Movement].Sensitivity.AnalogThreshold = Threshold;
            }
            else
            {
                IsOkay = FALSE;
            }
        }
    }
    else if( FParse::EndsWith( Key, DifferentialThresholdSuffix ) )
    {
        //-------------------------------------------------
        //   Differential Threshold (for some movement)
        //-------------------------------------------------
        IsOkay = TRUE;
        strcpy( SubKey, Key );
        SubKey[ KeyLength - strlen(DifferentialThresholdSuffix) ] = 0; // Strip off the suffix.
        const TMovement Movement = FindMovement(SubKey);
        float Threshold;
        if( Movement != 0 && FParse::GetFloat(Text,Threshold) )
        {
            if( Threshold > 0.0 && Threshold < 1.0 )
            {
                Info[Movement].Sensitivity.DifferentialThreshold = Threshold;
            }
            else
            {
                IsOkay = FALSE;
            }
        }
    }
    else if( FParse::EndsWith( Key, DigitalThresholdSuffix ) )
    {
        //----------------------------------------------
        //     Digital Threshold for some movement)
        //----------------------------------------------
        IsOkay = TRUE;
        strcpy( SubKey, Key );
        SubKey[ KeyLength - strlen(DigitalThresholdSuffix) ] = 0; // Strip off the suffix.
        const TMovement Movement = FindMovement(SubKey);
        float Threshold;
        if( Movement != 0 && FParse::GetFloat(Text,Threshold) )
        {
            if( Threshold > 0.0 && Threshold < 1.0 )
            {
                Info[Movement].Sensitivity.DigitalThreshold = Threshold;
            }
            else
            {
                IsOkay = FALSE;
            }
        }
    }
    else if( FParse::EndsWith( Key, SpeedSuffix ) )
    {
        //----------------------------------------------
        //     Movement speed
        //----------------------------------------------
        IsOkay = TRUE;
        strcpy( SubKey, Key );
        SubKey[ KeyLength - strlen(SpeedSuffix) ] = 0; // Strip off the suffix.
        const TMovement Movement = FindMovement(SubKey);
        float Speed;
        if( Movement != 0 && FParse::GetFloat(Text,Speed) )
        {
            if( Speed > 0.0 && Speed <= 3.0 )
            {
                Info[Movement].Sensitivity.Speed = Speed;
            }
            else
            {
                IsOkay = FALSE;
            }
        }
    }
    // Check for extraneous stuff on the line: (comments are okay - they start with ';')
    if( IsOkay && !FParse::StartsWith(Text,';') && Text[0] != 0 )
    {
        IsOkay = FALSE;
    }
    return IsOkay;
    UNGUARD( "FInput::SetValue" );
}

//----------------------------------------------------------------------------
// Fill in Info (an array of MovementCount items) with the value interpreted from Key=Value in Pair.
//----------------------------------------------------------------------------
BOOL FInput::SetValue(const char * Pair, TMovementInfo * Info )
{
    const char * Text = Pair;
    BOOL IsOkay = FALSE;
    if( FParse::StartsWith(Text,';') )
    {
        // Comment line - ignore it.
        IsOkay = TRUE;
    }
    else
    {
        char Key[FKeyValues::MaxKeyLength+1]; // +1 for trailing null.
        if( FParse::GetWord(Text,Key,MaxKeyLength) && FParse::StartsWith(Text,'=') )
        {
            FParse::SkipWhiteSpace(Text);
            IsOkay = SetValue( Key, Text, Info );
        }
    }
    return IsOkay;
}

//----------------------------------------------------------------------------
// Fills in Info (an array of MovementCount items) with parsed values from PairList.
//----------------------------------------------------------------------------
BOOL FInput::SetValues(TStringList PairList, TMovementInfo * Info )
{
    TStringList Pairs = PairList; // Pairs will iterate over each pair.
    BOOL IsOkay = TRUE;
    while( !Pairs.IsEmpty() )
    {
        if( !SetValue(Pairs.First(),Info) )
        {
            IsOkay = FALSE;
        }
        Pairs = Pairs.Next();
    }
    return IsOkay;
}

//----------------------------------------------------------------------------
//                Find a movement from its name.
//----------------------------------------------------------------------------
// Returning the matching switch or FInput::M_None if not found.
static FInput::TMovement MovementFromName( const char * Text)
{
    FInput::TMovement Movement = FInput::TMovement(0);
    if( Text != 0 && Text[0] != 0 )
    {
        for( int Check = 1; Movement==0 && Check < FInput::MovementCount; Check++ )
        {
            if
            ( 
                    stricmp( Text, GInput.PlatformInput->Abbreviation( FInput::TMovement(Check) ) ) == 0 
                ||  stricmp( Text, GInput.PlatformInput->Description( FInput::TMovement(Check) ) ) == 0 
            )
            {
                Movement = FInput::TMovement(Check);
            }
        }
    }
    return Movement;
}

//----------------------------------------------------------------------------
//                Find a switch from its name.
//----------------------------------------------------------------------------
// Returning the matching switch or FInput::S_None if not found.
static FInput::TSwitch SwitchFromName(const char * Text)
{
    FInput::TSwitch Switch = FInput::TSwitch(0);
    char AdjustedText[100]; // For when we want to try an altered name.
    if( Text != 0 && Text[0] != 0 )
    {
        for( int Check = 1; Switch==0 && Check < FInput::SwitchCount; Check++ )
        {
            if
            ( 
                    stricmp( Text, GInput.PlatformInput->Abbreviation( FInput::TSwitch(Check) ) ) == 0
                ||  stricmp( Text, GInput.PlatformInput->Description( FInput::TSwitch(Check) ) ) == 0 
            )
            {
                Switch = FInput::TSwitch(Check);
            }
        }
        if( Switch==0 )
        {
            const int Length = strlen(Text);
            if( Length >= 3 && Length < 100 && Text[0]=='\'' && Text[Length-1]=='\'' ) // Quoted text?
            {
                // Try again with leading and trailing '\'' removed:
                memmove( AdjustedText, &Text[1], Length-2 );
                AdjustedText[Length-2] = 0;
                Switch = SwitchFromName(AdjustedText);
            }
            if( Length >= 3 && Length < 100 && Text[0]=='\"' && Text[Length-1]=='\"' ) // Quoted text?
            {
                // Try again with leading and trailing '\"' removed:
                memmove( AdjustedText, &Text[1], Length-2 );
                AdjustedText[Length-2] = 0;
                Switch = SwitchFromName(AdjustedText);
            }
        }
    }
    return Switch;
}

//----------------------------------------------------------------------------
//              Parse a movement name out of some text.
//----------------------------------------------------------------------------
FInput::TMovement FInput::ParseMovement
(
    const char * & Text
)
{   
    FInput::TMovement Movement = FInput::TMovement(0);
    const char * CheckText = Text;
    char Name[50]; // Name of the movement
    if( FParse::GetWord(CheckText,Name,arrayCount_(Name)-1) > 0 )
    {
        Movement = MovementFromName(Name);
        if( Movement != 0 )
        {
            Text = CheckText;
        }       
    }
    FParse::SkipWhiteSpace(Text);
    return Movement;
}

//----------------------------------------------------------------------------
//     Find a movement name at the beginning of some text.
//----------------------------------------------------------------------------
FInput::TMovement FInput::FindMovement( const char * Text )
{
    return ParseMovement(Text);
}

//----------------------------------------------------------------------------
//              Parse a switch name out of some text.
//----------------------------------------------------------------------------
FInput::TSwitch FInput::ParseSwitch
(
    const char * & Text
)
{   
    FInput::TSwitch Switch = FInput::TSwitch(0);
    FParse::SkipWhiteSpace(Text);
    const char * CheckText = Text;
    char Name[50]; // Name of the switch
    const char C = FParse::GetQuotedCharacter(CheckText);
    if( C != 0 )
    {
        Name[0] = C;
        Name[1] = 0;
        Switch = SwitchFromName(Name);
    }
    else if( FParse::GetWord(CheckText,Name,arrayCount_(Name)-1) > 0 )
    {
        Switch = SwitchFromName(Name);
        if( DebugParsing ) { Debug( "FInput::ParseSwitch::Parse(\"%s\")=>%i %i", Text, int(Switch), int(FInput::SwitchCount) ); }
    }
    if( Switch != 0 )
    {
        Text = CheckText;
    }       
    FParse::SkipWhiteSpace(Text);
    return Switch;
}

//----------------------------------------------------------------------------
//     Find a switch name at the beginning of some text.
//----------------------------------------------------------------------------
FInput::TSwitch FInput::FindSwitch( const char * Text )
{
    return ParseSwitch(Text);
}

//----------------------------------------------------------------------------
//              Textual description of the combo.
//----------------------------------------------------------------------------
const char * FInput::TCombo::Text(BOOL Descriptive) const
{
    char Description[200];
    char * Text = Description;
    Text[0] = 0;
    if( !Modifiers.IsEmpty() )
    {
        Text += sprintf( Text, "%s+", Modifiers.Text(Descriptive) );
    }
    if( IsSwitch() )
    {
        Text += sprintf( Text, "%s", MetaSwitch.Text(Descriptive) );
    }
    else
    {
        Text += sprintf( Text, "%s", MetaMovement.Text(Descriptive) );
    }
    strcpy( TextBuffer, Description );
    return TextBuffer;
}

//----------------------------------------------------------------------------
//              Textual description of the main input of the combo.
//----------------------------------------------------------------------------
const char * FInput::TCombo::MainInputText(BOOL Descriptive) const
{
    const char * Text = 0;
    if( IsSwitch() )
    {
        Text = MetaSwitch.Text(Descriptive);
    }
    else
    {
        Text = MetaMovement.Text(Descriptive);
    }
    return Text;
}

//----------------------------------------------------------------------------
//              Textual description of the modifiers.
//----------------------------------------------------------------------------
const char * FInput::TModifiers::Text(BOOL Descriptive) const 
{
    char Description[100];
    char * Text = Description;
    Text[0] = 0;
    if( Has1() )
    {
        Text += sprintf( Text, "%s", MetaSwitch1.Text(Descriptive) );
    }
    strcpy( TextBuffer, Description );
    return TextBuffer;
}

//----------------------------------------------------------------------------
//              Textual description of the meta-movement.
//----------------------------------------------------------------------------
// If !IsEmpty(), the returned text will look like this:
//      MovementName(MovementKind)
const char * FInput::TMetaMovement::Text(BOOL Descriptive) const
{
    char Description[100];
    char * Text = Description;
    Text[0] = 0;
    if( Movement != 0 )
    {
        Text += sprintf( Text, "%s", GInput.Name(Movement,Descriptive) );
        Text += sprintf( Text, "(%s)", GInput.Name(Kind,Descriptive) );
    }    
    strcpy( TextBuffer, Description );
    return TextBuffer;
}

//----------------------------------------------------------------------------
//              Meta-switch comparison.
//----------------------------------------------------------------------------
BOOL FInput::TMetaSwitch::operator == (const TMetaSwitch & Value) const 
{
    return 
        (this->Switch==0 && Value.Switch ==0 )
    ||  
        (
            this->Switch   == Value.Switch 
        &&  this->IsDouble == Value.IsDouble 
        &&  this->IsToggle == Value.IsToggle 
        )
    ;
}

//----------------------------------------------------------------------------
//              textual description of a meta-switch.
//----------------------------------------------------------------------------
// If !IsEmpty(), the returned text will look like this:
//      SwitchName DoubleText ToggleText
// There will be no spaces as implied above.
// The DoubleText will appear only if IsDouble.
// The ToggleText will appear only if IsToggle.
const char * FInput::TMetaSwitch::Text(BOOL Descriptive) const
{
    char Description[100];
    char * Text = Description;
    Text[0] = 0;
    if( Switch != 0 )
    {
        Text += sprintf( Text, "%s", GInput.Name(Switch,Descriptive) );
        if( Descriptive )
        {
            if( IsDouble && IsToggle )
            {
                Text += sprintf( Text, "%s", "(T2)" );
            }
            else if( IsDouble )
            {
                Text += sprintf( Text, "%s", "(2)" );
            }
            else if( IsToggle )
            {
                Text += sprintf( Text, "%s", "(T)" );
            }
        }
        else 
        {
            if( IsDouble )
            {
                Text += sprintf( Text, "%s", DoubleText );
            }
            if( IsToggle )
            {
                Text += sprintf( Text, "%s", ToggleText );
            }
        }
    }    
    strcpy( TextBuffer, Description );
    return TextBuffer;
}

//----------------------------------------------------------------------------
//              Normalize a meta-switch by clearing useless info.
//----------------------------------------------------------------------------
void  FInput::TMetaSwitch::Normalize()
{
    if( IsEmpty() )
    {   
        Empty();
    }
}


//----------------------------------------------------------------------------
//          Parse a meta-switch out of Text.
//----------------------------------------------------------------------------
BOOL FInput::TMetaSwitch::Parse(const char * & Text)
{
    FParse::SkipWhiteSpace(Text);
    const char * CheckText = Text;
    Switch = GInput.ParseSwitch(CheckText);
    if( Switch != 0 ) // Did we find a switch?
    {
        IsDouble = ParseDoubler(CheckText);
        IsToggle = ParseToggler(CheckText);
        FParse::SkipWhiteSpace(CheckText);
        if( DebugParsing ) { Debug( "TMetaSwitch::Parse(\"%s\")=>\"%s\"", Text, this->Text(FALSE) ); }
        Text = CheckText;
    }
    else
    {
        Empty();
    }
    return Switch != 0;
}

//----------------------------------------------------------------------------
//              Meta-Movement comparison.
//----------------------------------------------------------------------------
BOOL FInput::TMetaMovement::operator == (const TMetaMovement & Value) const 
{
    return 
        (this->Movement==0 && Value.Movement ==0 )
    ||  
        (
            this->Movement   == Value.Movement 
        &&  this->Kind       == Value.Kind
        )
    ;
}

//----------------------------------------------------------------------------
//          Parse a meta-movement out of Text.
//----------------------------------------------------------------------------
BOOL FInput::TMetaMovement::Parse(const char * & Text)
{
    FParse::SkipWhiteSpace(Text);
    const char * CheckText = Text;
    Movement = GInput.ParseMovement(CheckText);
    if( Movement != 0 ) // Did we find a Movement?
    {
        Kind = ParseMovementSuffix(CheckText,GInput);
        if( Kind == 0 )
        {
            Kind = GInput.PlatformInput->DefaultMovementKind(Movement);
        }
        if( DebugParsing ) { Debug( "TMetaMovement::Parse(\"%s\")=>\"%s\"", Text, this->Text(FALSE) ); }
        FParse::SkipWhiteSpace(CheckText);
        Text = CheckText;
    }
    else
    {
        Empty();
    }
    return Movement != 0;
}

//----------------------------------------------------------------------------
//              TModifiers comparison.
//----------------------------------------------------------------------------
BOOL FInput::TModifiers::operator == (const TModifiers & Value) const 
{
    return Switch1() == Value.Switch1();
}

//----------------------------------------------------------------------------
//              Normalize a set of modifiers by clearing useless info.
//----------------------------------------------------------------------------
void  FInput::TModifiers::Normalize()
{
    //todo: We don't really need the normalize functions - remove them up (carefully).
    MetaSwitch1.Normalize();
}

//----------------------------------------------------------------------------
//          Parse any modifiers out of Text.
//----------------------------------------------------------------------------
BOOL FInput::TModifiers::Parse(const char * & Text)
{
    FParse::SkipWhiteSpace(Text);
    const char * CheckText = Text;
    if( MetaSwitch1.Parse(CheckText) && FParse::StartsWith(CheckText,'+') )
    {
        // There is a meta-switch, and it is a modifier.
        FParse::SkipWhiteSpace(CheckText);
        if( DebugParsing ) { Debug( "TModifiers::Parse(\"%s\")=>\"%s\"", Text, this->Text(FALSE) ); }
        Text = CheckText;
    }
    else
    {   
        Empty();
    }
    return Has1();
}

//----------------------------------------------------------------------------
//              TCombo comparison.
//----------------------------------------------------------------------------
BOOL FInput::TCombo::operator == (const TCombo & Value) const 
{
    return 
        this->IsASwitch == Value.IsASwitch
    &&  this->Modifiers == Value.Modifiers
    &&  (!this->IsASwitch || this->MetaSwitch   == Value.MetaSwitch   )
    &&  (this->IsASwitch  || this->MetaMovement == Value.MetaMovement )
    ;
}

//----------------------------------------------------------------------------
//              Normalize a combo by clearing useless info.
//----------------------------------------------------------------------------
void  FInput::TCombo::Normalize()
{
    Modifiers.Normalize();
    if( IsASwitch )
    {
        MetaSwitch.Normalize();
    }
    else
    {
        MetaMovement.Normalize();
    }
}


//----------------------------------------------------------------------------
//          Parse any combo definition out of Text.
//----------------------------------------------------------------------------
BOOL FInput::TCombo::Parse(const char * & Text)
{
    FParse::SkipWhiteSpace(Text);
    const char * CheckText = Text;
    BOOL IsOkay = FALSE;
    Modifiers.Parse(CheckText);
    const BOOL SwitchFound = MetaSwitch.Parse(CheckText);
    if( SwitchFound || MetaMovement.Parse(CheckText) )
    {
        IsOkay = TRUE;
        IsASwitch = SwitchFound;
        if( DebugParsing ) { Debug( "TCombo::Parse(\"%s\")=>\"%s\"", Text, this->Text(FALSE) ); }
        Text = CheckText;
    }
    else
    {
        Empty();
    }
    return IsOkay;
}

/*------------------------------------------------------------------------------
	The End
------------------------------------------------------------------------------*/
