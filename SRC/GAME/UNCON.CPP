/*=============================================================================
	UnCon.cpp: Implementation of FCameraConsole class

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Contains routines for: Messages, menus, status bar
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "UnGame.h"
#include "UnCon.h"
#include "UnRender.h"
#include "UnAction.h"
#include "UnCheat.h"
#include "UnFActor.h"

/*------------------------------------------------------------------------------
	Globals
------------------------------------------------------------------------------*/

static FAction GAction  ;

//
// These defines give the size of the displayable status bar graphics, because
// the status bar textures are forced to power-of-two sizes.
//
#define STATUSBAR_U  640.0
#define STATUSBAR_V  64.0

// Specification of locations of parts of the status bars:
static const FLOAT ArmorValueX  =  90.0; // Relative to left of status bar.
static const FLOAT HealthValueX = 281.0; // Relative to left of status bar.
static const FLOAT AmmoValueX   = 472.0; // Relative to left of status bar.

static const FLOAT HealthValueY =  31.0; // Relative to top of status bar.
static const FLOAT ArmorValueY  =  31.0; // Relative to top of status bar.
static const FLOAT AmmoValueY   =  31.0; // Relative to top of status bar.

static const FLOAT ArmorImageX  =   4.0; // Relative to left of status bar.
static const FLOAT ArmorImageY  =   4.0; // Relative to top of status bar.
static const FLOAT AmmoImageX   = 560.0; // Relative to left of status bar.
static const FLOAT AmmoImageY   =   0.0; // Relative to top of status bar.

// Location of lights:
struct TLightPosition
    {
        FLOAT X;
        FLOAT Y;
    };
static const TLightPosition StatusSmallLightPositions[] =
{
    {   43.0    , 4.0   }
,   {   64.0    , 4.0   }
,   {   87.0    , 4.0   }
,   {  108.0    , 4.0   }
,   {  130.0    , 4.0   }
,   {  175.0    , 4.0   }
,   {  197.0    , 4.0   }
,   {  218.0    , 4.0   }
,   {  241.0    , 4.0   }
,   {  263.0    , 4.0   }
};
static const TLightPosition StatusLightPositions[] =
{
    {   84.0    , 6.0   }
,   {  128.0    , 6.0   }
,   {  173.0    , 6.0   }
,   {  216.0    , 6.0   }
,   {  259.0    , 6.0   }
,   {  349.0    , 6.0   }
,   {  392.0    , 6.0   }
,   {  436.0    , 6.0   }
,   {  481.0    , 6.0   }
,   {  525.0    , 6.0   }
};

// Inventory icons appear in the thin status bar:
static const FLOAT InventoryIconsX = 2.0; // Relative to left of status bar.
static const FLOAT InventoryIconsY = 2.0; // Relative to top of status bar.

/*------------------------------------------------------------------------------
	Initializing and exiting a console
------------------------------------------------------------------------------*/

//
// Init console for a particular camera.  Should be called immediately
// when a new camera is created and recognized.
//
void FCameraConsole::Init(UCamera *ThisCamera)
	{
	GUARD;
	//
	Camera			= ThisCamera;
	//
	KeyState		= CK_None;
	HistoryTop		= 0;
	HistoryCur		= 0;
	for (int i=0; i<MAX_HISTORY; i++) *History[i]=0;
	//
	NumLines		= 0;
	TopLine			= MAX_LINES-1;
	MsgStart		= 0;
	MsgDuration 	= 0;
	for (i=0; i<MAX_LINES; i++) *MsgText[i]=0;
	//
	ConsolePos		= 0.0;
	ConsoleDest		= 0.0;
	//
	LastTickTime	= 0;
	StatusRefreshPages = 2;
	//
	BorderSize		= 1; 
	Redraw			= 0;
	LogoUp			= 0;
	Scrollback		= 0;
	//
	// Find all graphics resources.  These are imported
	// into the engine via the Graphics.mac macro and are
	// stored in the game's .gfx file in the System directory.
	//
	StatusBar		= new("StatusBar",		FIND_Existing)UTexture;
	StatusSmallBar	= new("StatusBarS",		FIND_Existing)UTexture;
	StatusWeaponLight		= new("StatusLight",		FIND_Existing)UTexture;
	StatusSmallWeaponLight	= new("StatusLightS",		FIND_Existing)UTexture;
	ConBackground	= new("ConBackground",	FIND_Existing)UTexture;
	Border			= new("Border",			FIND_Existing)UTexture;
	ArmorImages[DMT_Basic] = new("Armor1"       , FIND_Existing)UTexture;
    StatusDigits[0]        = new("Status0"      , FIND_Existing)UTexture;
    StatusDigits[1]        = new("Status1"      , FIND_Existing)UTexture;
    StatusDigits[2]        = new("Status2"      , FIND_Existing)UTexture;
    StatusDigits[3]        = new("Status3"      , FIND_Existing)UTexture;
    StatusDigits[4]        = new("Status4"      , FIND_Existing)UTexture;
    StatusDigits[5]        = new("Status5"      , FIND_Existing)UTexture;
    StatusDigits[6]        = new("Status6"      , FIND_Existing)UTexture;
    StatusDigits[7]        = new("Status7"      , FIND_Existing)UTexture;
    StatusDigits[8]        = new("Status8"      , FIND_Existing)UTexture;
    StatusDigits[9]        = new("Status9"      , FIND_Existing)UTexture;
    StatusSmallDigits[0]   = new("Status0S"     , FIND_Existing)UTexture;
    StatusSmallDigits[1]   = new("Status1S"     , FIND_Existing)UTexture;
    StatusSmallDigits[2]   = new("Status2S"     , FIND_Existing)UTexture;
    StatusSmallDigits[3]   = new("Status3S"     , FIND_Existing)UTexture;
    StatusSmallDigits[4]   = new("Status4S"     , FIND_Existing)UTexture;
    StatusSmallDigits[5]   = new("Status5S"     , FIND_Existing)UTexture;
    StatusSmallDigits[6]   = new("Status6S"     , FIND_Existing)UTexture;
    StatusSmallDigits[7]   = new("Status7S"     , FIND_Existing)UTexture;
    StatusSmallDigits[8]   = new("Status8S"     , FIND_Existing)UTexture;
    StatusSmallDigits[9]   = new("Status9S"     , FIND_Existing)UTexture;
    
	//
	// Start console log:
	//
	SpawnConsoleMessage();
	Logf("Console ready for %s",Camera->Level->ActorList->Element(Camera->iActor).Name.Name());
	Logf(" ");

    // Action and input handling:
    // Note:
    //   For now, we use the same global objects for all cameras. If we want to
    //   extend input/action handling to be camera-dependent, we would change
    //   this to create new objects for each camera.
    Actions     = &GAction  ;
    Actions->Initialize()   ;
    Input       = &GInput   ;
    Input->Initialize(GApp->Input);

	UNGUARD("FCameraConsole::Init");
	};

//
// Shut down a particular camera's console. Should be called before
// closing a camera.
//
void FCameraConsole::Exit()
	{
	GUARD;
    if( Actions != 0 ) 
    { 
        // Actions->Exit(); // We can't do this currently because Actions is shared among cameras.
        Actions = 0; 
    }
    if( Input != 0 ) 
    { 
        // Input->Exit(); // We can't do this currently because Input is shared among cameras.
        Input = 0; 
    }
	UNGUARD("FCameraConsole::Exit");
	};

/*------------------------------------------------------------------------------
	Utility functions
------------------------------------------------------------------------------*/

void FCameraConsole::NoteResize()
	{
	GUARD;
	Redraw++;
	UNGUARD("FCameraConsole::NoteResize");
	};

void FCameraConsole::ClearInput()
	{
	GUARD;
	//
	HistoryCur = HistoryTop;
	memset(TypedStr,0,sizeof(TypedStr));
	//
	UNGUARD("FCameraConsole::ClearInput");
	};


/*------------------------------------------------------------------------------
	Console command-line
------------------------------------------------------------------------------*/

int FCameraConsole::Exec(const char *Cmd,FOutputDevice *Out)
	{
	GUARD;
	const char *Str = Cmd;
	//
	if (GetCMD(&Str,"STATUS") && (GetCMD(&Str,"CONSOLE") || !Str[0]))
		{
		Out->Logf("   CONSOLE - Owned by %s",Camera->Level->ActorList->Element(Camera->iActor).Name.Name());
		return Str[0]!=0;
		}
	else if (GetCMD(&Str,"CONSOLE"))
		{
		if (GetCMD(&Str,"HIDE"))
			{
			Out->Log("Console hidden");
			ConsoleDest = 0.0;
			return 1;
			}
		else if (GetCMD(&Str,"SHOW"))
			{
			Out->Log("Console shown");
			ConsoleDest = CON_SHOW;
			return 1;
			}
		else if (GetCMD(&Str,"FULL"))
			{
			Out->Log("Full console view");
			ConsoleDest = 1.0;
			return 1;
			}
		else return 0;
		}
	else if (GetCMD(&Str,"FPS"))
		{
		GRend->QuickStats ^= 1; 
		Out->Logf("FPS display is %s",GRend->QuickStats?"on":"off"); 
		return 1;
		}
	else if (PeekCMD(Str,"PN") || PeekCMD(Str,"PP"))
		{
		ICamera CameraInfo;
		APawn *Actor = &Camera->GetActor();
		//
		int d=+1;
		if		(GetCMD(&Str,"PP")) d=-1;
		else if (GetCMD(&Str,"PN")) d=+1;
		//
		UClass *DestClass;
		//
		if (Actor->ShowFlags & SHOW_StandardView)
			{
			Out->Log("Must open a free camera to possess");
			return 1;
			};
		if (!Camera->Lock(&CameraInfo)) return 1;
		//
		// Try to possess an available actor
		//
		if (GetCMD(&Str,"ALIKE")) DestClass = Actor->Class;
		else DestClass = NULL;
		//
		int n = CameraInfo.iActor;
		int Failed = 1;
		//
		for(;;)
			{
			n += d;
			if      (n >= CameraInfo.Level.Actors->Max)	n=0;
			else if (n < 0)								n=CameraInfo.Level.Actors->Max-1;
			//
			if (n == CameraInfo.iActor) break;
			//
			AActor *TestActor = &CameraInfo.Level.Actors->Element(n);
			if (TestActor->IsKindOf("Pawn") && 
				!TestActor->GetPlayer() &&
				((APawn*)TestActor)->bCanPossess && !TestActor->bHiddenEd)
				{
				if ((TestActor->Class == DestClass) || (DestClass == NULL))
					{
					CameraInfo.Level.UnpossessActor(CameraInfo.iActor);
					CameraInfo.Level.PossessActor(n,Camera);
					//
					CameraInfo.Camera->iActor = n;
					CameraInfo.iActor = n;
					CameraInfo.Actor = (APawn*)TestActor;
					//
					Failed=0;
					break;
					};
				};
			};
		Camera->Unlock(&CameraInfo,0);
		GCameraManager->UpdateCameraWindow(Camera);
		//
		// Update player console window:
		//
		if (Failed)
			{
			Out->Log(LOG_Info,"Couldn't possess");
			}
		else
			{
			char Who[256];
			strcpy( Who,"Possessed a " );
			strcat( Who,Camera->GetActor().Class->Name );
			Out->Log(Who);
			};
		return 1;
		}
	else if (GetCMD(&Str,"CHECK"))
		{
        // Add any checks here...
        debugf( LOG_Info, "Checking actor lists..." );
        Camera->Level->ActorList->CheckLists();
        return 1;
        }
	else if (GetCMD(&Str,"ACTORSIZES"))
		{
        Out->Logf( "sizeof(AI)=%i", sizeof(AActorAI) );
        Out->Logf( "sizeof(AActor)=%i", sizeof(AActor) );
        Out->Logf( "sizeof(APawn)=%i+Root", sizeof(APawn)-sizeof(AActor) );
        Out->Logf( "sizeof(ASkaarj)=%i+Root", sizeof(ASkaarj)-sizeof(AActor) );
        Out->Logf( "sizeof(ABigMan)=%i+Root", sizeof(ABigMan)-sizeof(AActor) );
        Out->Logf( "sizeof(AInventory)=%i+Root", sizeof(AInventory)-sizeof(AActor) );
    	return 1;
        }
	else if (GetCMD(&Str,"ACTORINFO")) // Summarize essential actor info?
		{
        for( int WhichActor = 0; WhichActor < Camera->Level->ActorList->Num; ++WhichActor )
            {
            AActor & Actor = Camera->Level->ActorList->Element(WhichActor);
            if( Actor.Class != 0 )
                {
                char Info[100];
                char * Text = Info;
                const char * Name = Actor.Name.Name();
                Text += sprintf( Text, "[%2i] %s %s", WhichActor, (Name!=0?Name:""), Actor.Class->Name );
                Text += sprintf( Text, " %s", Actor.bStatic ? "s" : "!s" );
                Text += sprintf( Text, ",%s", Actor.bCollideActors ? "c" : "!c" );
                Text += sprintf( Text, ",%s", Actor.bWasTouchedByPlayer ? "t" : "!t" );
                Text += sprintf( Text, " L:[%5.0f %5.0f %5.0f]", Actor.Location.X, Actor.Location.Y, Actor.Location.Z );
                Text += sprintf( Text, " V:[%5.0f %5.0f %5.0f]", Actor.Velocity.X, Actor.Velocity.Y, Actor.Velocity.Z );
                debug( LOG_Info, Info );
                }
            }
    	return 1;
        }
	else if (GetCMD(&Str,"LEVELSTATS")) // Show level stats?
		{
        APawn & Pawn = (APawn&)this->Camera->GetActor(); // Camera actor is always a pawn.
        const ALevelDescriptor & Descriptor = (const ALevelDescriptor &)
            Camera->Level->ActorList->Element(0)
            ;
        Out->Logf
        ( 
            "Kills: %i/%i  Items: %i/%i Secrets: %i/%i"
        ,   int(Pawn.KillCount), int(Descriptor.KillGoals)
        ,   int(Pawn.ItemsFound), int(Descriptor.ItemGoals)
        ,   int(Pawn.SecretsFound), int(Descriptor.SecretGoals)
        );
        return 1;
        }
    else if
    ( 
        // Check individual characters so complete strings won't appear in the executable code.
        // This makes it harder for hackers to figure out cheats by scanning for strings.
        // Allow password "CHEAT" and some abbreviations thereof...
            toupper(Str[0])=='C' 
        &&  toupper(Str[1])=='H' 
        &&  toupper(Str[2])=='E' 
        &&  toupper(Str[3])=='A'
        &&  toupper(Str[4])=='T'
    )
        {
        while( !isspace(*Str) ) { Str++; }
        AActor & Actor = (AActor&)Camera->GetActor();
        GCheat->DoCheatCommands( Str, &Actor, Camera->Level, Out );
        return 1;
        }

	else return 0;
	//
	UNGUARD("FCameraConsole::Exec");
	};

/*------------------------------------------------------------------------------
	Camera console output
------------------------------------------------------------------------------*/

//

// Print a message on the playing screen.
// Time = time to keep message going, or 0=until next message arrives, in 60ths sec
//
void FCameraConsole::Log(ELogType ThisType, const char *Text)
	{
	GUARD;
	//
	TopLine		= (TopLine+1) % MAX_LINES;
	NumLines	= Min(NumLines+1,(int)(MAX_LINES-1));
	//
	strncpy (MsgText[TopLine],Text,TEXTMSG_LENGTH);
	MsgText[TopLine][TEXTMSG_LENGTH-1] = 0;
	//
	MsgType  	= ThisType;
	MsgStart 	= GServer.Ticks;
	MsgDuration	= MESSAGE_TIME;
	//
	UNGUARD("FCameraConsole::Message");
	};

/*------------------------------------------------------------------------------
	Rendering
------------------------------------------------------------------------------*/

//
// Called before rendering the world view.  Here, the
// camera console code can affect the screen's camera,
// for example by shrinking the view according to the
// size of the status bar.
//
ICamera *GSavedCamera;
void FCameraConsole::PreRender(ICamera *Camera)
	{
	GUARD;

    // Process any console-related actions:
    if( Camera->Camera->Current )
    {
        const BOOL FullConsole = GAction.Do(FAction::ConsoleFull);
        const BOOL HalfConsole = GAction.Do(FAction::ConsoleHalf);
        if( FullConsole || HalfConsole )
        {
            if( ConsoleDest==0 )
            {
                ConsoleDest = FullConsole ? 1.0 : CON_SHOW;
                if (KeyState!=CK_Type) ClearInput();
                KeyState = CK_Type;
            }
            else
            {
                ClearInput();
                ConsoleDest = 0;
                KeyState = CK_None;
            }
        }
        if( GAction.Do(FAction::ConsoleType) )
            {
                ClearInput();
                KeyState = CK_Type;
            }
        else if( GAction.Do(FAction::Chat) )
            {
                ClearInput();
                strcpy (TypedStr,"Say ");
                KeyState = CK_Type;
            }
        if( GAction.Do(FAction::ScreenEnlarge) )
            {
                if (--BorderSize<0) BorderSize=0;
            }
        else if( GAction.Do(FAction::ScreenShrink) )
            {
                if (++BorderSize>=MAX_BORDER) BorderSize=MAX_BORDER-1;
            }
        if( GAction.Do(FAction::Pause) )
            {
                GServer.IsPaused = !GServer.IsPaused;
                if( GServer.IsPaused )
                {
                    // Game was just paused...
                    GAudio.Pause();
                    Log( LOG_Play, "Paused" );
                    //todo? Should we save any existing message and display it when unpaused?
                }
                else
                {
                    // Game was just unpaused...
                    GAudio.UnPause();
                    MsgType = LOG_None; // Clear the "paused" message
                }
            }
    }

	//
	// Update the camera console:
	//
	if (LastTickTime) while (LastTickTime++ < GServer.Ticks) Tick(Camera,1);
	else Tick(Camera,1);
	LastTickTime      = GServer.Ticks;
	Old->LastTickTime = GServer.Ticks; // Prevent status redraw due to LastTickTime changing
	//
	// Save the camera
	//
	GSavedCamera=(ICamera *)GMem.Get(sizeof(ICamera));
	memcpy(GSavedCamera,Camera,sizeof(ICamera));
	//
	// Compute new status info:
	//
	SXR				= Camera->SXR;
	SYR				= Camera->SYR;
	BorderLines		= 0;
	BorderPixels	= 0;
	StatusBarLines	= 0;
	ConsoleLines	= 0;
	//
	//if (Camera->ColorBytes!=1)
	//	{
	//	BorderSize=0;
	//	ConsolePos=0.0;
	//	};
	//
	// Compute sizing of all visible status bar components:
	//
	if (Camera->Camera->IsGame())
		{
		if (BorderSize>=1) // Status bar
			{
			StatusBarLines = Min((FLOAT)(Camera->FSXR * STATUSBAR_V/STATUSBAR_U),(FLOAT)SYR);
			SYR -= StatusBarLines;
			};
		if (ConsolePos > 0.0) // Show console
			{
			ConsoleLines = Min(ConsolePos * (FLOAT)SYR,(FLOAT)SYR);
			SYR -= ConsoleLines;
			};
		if (BorderSize>=2) // Encroach on screen area
			{
			FLOAT Fraction = (FLOAT)(BorderSize-1) / (FLOAT)(MAX_BORDER-1);
			//
			BorderLines = (int)Min((FLOAT)SYR * 0.25f * Fraction,(FLOAT)SYR);
			BorderLines = Max(0,BorderLines - ConsoleLines);
			SYR -= 2 * BorderLines;
			//
			BorderPixels = (int)Min((FLOAT)SXR * 0.25f * Fraction,(FLOAT)SXR) & ~3;
			SXR -= 2 * BorderPixels;
			};
		Camera->Screen = &Camera->Screen
			[
			Camera->ColorBytes * 
			(BorderPixels + (ConsoleLines + BorderLines) * Camera->Stride)
			];
		Camera->PrecomputeRenderInfo(SXR,SYR);
		};
	//
	// Whether to show logo:
	//
	//if ((GServer.Ticks < 35*8) && Camera->Camera->IsGame()) LogoUp=1;
	//else LogoUp=0;
	LogoUp=0; // Logo is annoying
	//
	UNGUARD("FCameraConsole::PreRender");
	};

// Show a numeric string with (optional) leading blanks and digits '0'-'9'.
// Updates the X position to beyond the last character shown.
static void ShowValue
(
    const char  * Value
,   ICamera     * Camera
,   int         & X             // Updated on output
,   int           Y
,   FLOAT         XScale
,   FLOAT         YScale
,   BOOL          UseSmallImages
,   UTexture  * * DigitImages   // An array of 10 textures for digits 0-9
)
    {
    while( *Value != 0 )
        {
        const int Spacing = XScale * (UseSmallImages ? 13.0 : 26.0 );
        if( *Value == ' ' )
            {
                // We can't use the image sizes for spacing, since they have 
                // a lot of extra empty space.
                X += Spacing;
            }
        else if( *Value >= '0' && *Value <= '9' )
            {
            UTexture * Image = DigitImages[*Value - '0'];
            //debugf( LOG_Info, "(%3.2f %3.2f) %i %i '%c' %i %i", XScale, YScale, X, Y, *Value, int(Image->USize), int(Image->VSize) );
			GRend->DrawScaledSprite
				(
				    Camera
                ,   Image
                ,   X , Y
                ,   FLOAT(Image->USize) * XScale 
                ,   FLOAT(Image->VSize) * YScale
                ,   BT_None,NULL,0,0
				);
            // We can't use the image sizes for spacing, since they have 
            // a lot of extra empty space.
            X += Spacing;
            }
        Value++;
        }
    }

// Show a numeric value 0-999 at the given position, padded on the left
// with empty space.
static void ShowValue
(
    int           Value         // 0-999
,   ICamera     * Camera
,   int           X
,   int           Y
,   FLOAT         XScale
,   FLOAT         YScale
,   BOOL          UseSmallImages
,   UTexture  * * DigitImages   // An array of 10 textures for digits 0-9
)
    {
        if( Value < 0 ) 
            { 
                Value = 0;  
            }
        else if( Value > 999 ) 
            { 
                Value = 999;  
            }
        char Text[5];
        sprintf( Text, "%3i", Value );
        ShowValue( Text, Camera, X, Y, XScale, YScale, UseSmallImages, DigitImages );
    }

//
// Refresh the player console on the specified camera.  This is called after
// all in-game graphics are drawn in the rendering loop, and it overdraws stuff
// with the status bar, menus, and chat text.
//
void FCameraConsole::PostRender(ICamera *Camera,int XLeft)
	{
	GUARD;
	//
	// Restore the previously-saved camera:
	//
	memcpy(Camera,GSavedCamera,sizeof(ICamera));
	GMem.Release(GSavedCamera);
	//
	// If the console has changed since the previous frame, draw it:
	//
    //todo: Right now, changes in the status of the pawn are detected
    // by having the pawn set bStatusChanged. This is error-prone, and
    // we need to find a better way. Its not enough just to 
    // save ammo, health, and weapon information, we need to note
    // changes in the pawn's inventory.
    APawn & Pawn = (APawn&)this->Camera->GetActor(); // Camera actor is always a pawn.
    const BOOL PawnStatusChanged = Pawn.bStatusChanged;
	int Changed		= !mymemeq(this,Old,sizeof(*this)) || PawnStatusChanged;
	int DrawStatus  = Changed || (StatusRefreshPages>0);
	int YStart		= BorderLines;
	int YEnd		= Camera->SYR-BorderLines-StatusBarLines;
	int DrawBorder	= DrawStatus || (MsgType != LOG_None) || (KeyState==CK_Type);
	//
	if (DrawStatus)
		{
		//
		// Output a debugging string if we're updating the status bar.  This is useful
		// when expanding the status bar logic, because you need to make sure the status
		// bar is only being redrawn when necessary, or else the game will run slowly.
		//
		//bug("Updating status bar"); /* For debugging */
		//
		if (Changed) StatusRefreshPages = 2; // Handle proper refresh in triple-buffered mode
		else StatusRefreshPages--;
		//
		// Draw status bar:
		//
		if (StatusBarLines>0)
			{
            const FLOAT XScale = FLOAT(Camera->FSXR) / STATUSBAR_U;
            const FLOAT YScale = FLOAT(StatusBarLines) / STATUSBAR_V;
            const FLOAT StatusBarX = 0;
            const FLOAT StatusBarY = Camera->FSYR-StatusBarLines;
            const BOOL UseSmallBar = XScale <= 0.5;
            UTexture * Bar = UseSmallBar ? StatusSmallBar : StatusBar;
            const FLOAT AdditionalScale = UseSmallBar ? 2.0 : 1.0;
			GRend->DrawScaledSprite
				(
				Camera,Bar,
				0,Camera->FSYR-StatusBarLines,
				Camera->FSXR          * (FLOAT)Bar->USize / STATUSBAR_U * AdditionalScale + 0.5,
				(FLOAT)StatusBarLines * (FLOAT)Bar->VSize / STATUSBAR_V * AdditionalScale + 0.5,
				BT_None,NULL,0,0
				);

	            // Show the health value:
                ShowValue
                    ( 
                        Pawn.Health
                    ,   Camera
                    ,   StatusBarX + HealthValueX * XScale
                    ,   StatusBarY + HealthValueY * YScale
                    ,   XScale * AdditionalScale
                    ,   YScale * AdditionalScale
                    ,   UseSmallBar
                    ,   UseSmallBar ? StatusSmallDigits : StatusDigits
                    );

                // Show the armor (image and value):
                {
                UTexture * ArmorImage = ArmorImages[DMT_Basic]; // todo: Should depend on kind of armour
			    GRend->DrawScaledSprite // Draw the armor
				    (
				        Camera
                    ,   ArmorImage
                    ,   StatusBarX + ArmorImageX * XScale
                    ,   StatusBarY + ArmorImageY * YScale
                    ,   FLOAT(ArmorImage->USize) * XScale
                    ,   FLOAT(ArmorImage->VSize) * YScale
                    ,   BT_None,NULL,0,0
				    );
                    ShowValue
                    ( 
                        Pawn.Armor[DMT_Basic]
                    ,   Camera
                    ,   StatusBarX + ArmorValueX * XScale
                    ,   StatusBarY + ArmorValueY * YScale
                    ,   XScale * AdditionalScale
                    ,   YScale * AdditionalScale
                    ,   UseSmallBar
                    ,   UseSmallBar ? StatusSmallDigits : StatusDigits
                    );
                }

            // Light up the lights for weapons in our inventory...
            INDEX iInventory = Pawn.iInventory;
            const TLightPosition * Positions = UseSmallBar ? StatusSmallLightPositions : StatusLightPositions;
            UTexture * Light = UseSmallBar ? StatusSmallWeaponLight : StatusWeaponLight;
            while( iInventory != INDEX_NONE )
                {
                AInventory & Inventory = (AInventory &)this->Camera->Level->ActorList->Element(iInventory);
                if( Inventory.OwningSet > 0 )
                    {
                        const int Index = Inventory.OwningSet-1;
			            GRend->DrawScaledSprite // Draw the armor
				            (
				                Camera
                            ,   Light
                            ,   StatusBarX + Positions[Index].X * XScale * AdditionalScale
                            ,   StatusBarY + Positions[Index].Y * YScale * AdditionalScale
                            ,   FLOAT(Light->USize) * XScale * AdditionalScale
                            ,   FLOAT(Light->VSize) * YScale * AdditionalScale
                            ,   BT_None,NULL,0,0
				            );
                        iInventory = Inventory.iInventory;
                    }
                }

            // Show the ammo (image and value):
            if( Pawn.iWeapon != INDEX_NONE )
                {
                AWeapon & Weapon = (AWeapon&)this->Camera->Level->ActorList->Element(Pawn.iWeapon);
                UTexture * AmmoImage = Weapon.AmmoStatusIcon;
                const int Ammo = Pawn.AmmoCount[Weapon.AmmoType];
                if( AmmoImage != 0 )
                    {
			        GRend->DrawScaledSprite // Draw the ammo
				        (
				            Camera
                        ,   AmmoImage
                        ,   StatusBarX + AmmoImageX * XScale
                        ,   StatusBarY + AmmoImageY * YScale
                        ,   FLOAT(AmmoImage->USize) * XScale
                        ,   FLOAT(AmmoImage->VSize) * YScale
                        ,   BT_None,NULL,0,0
				        );
                    }
                ShowValue
                    ( 
                        Ammo
                    ,   Camera
                    ,   StatusBarX + AmmoValueX * XScale
                    ,   StatusBarY + AmmoValueY * YScale
                    ,   XScale * AdditionalScale
                    ,   YScale * AdditionalScale
                    ,   UseSmallBar
                    ,   UseSmallBar ? StatusSmallDigits : StatusDigits
                    );
                }
			};
		//
		// Draw console:
		//
		if (ConsoleLines>0) GRend->DrawTiledTextureBlock
			(
			Camera,ConBackground,0,Camera->SXR,0,ConsoleLines,0,-ConsoleLines
			);
		}
	//
	// Draw border:
	//
	if (DrawBorder && ((BorderLines>0)||(BorderPixels>0)))
		{
		YStart += ConsoleLines;
		int V = ConsoleLines>>1;
		if (BorderLines>0)
			{
			GRend->DrawTiledTextureBlock(Camera,Border,0,Camera->SXR,0,BorderLines,0,-V);
			GRend->DrawTiledTextureBlock(Camera,Border,0,Camera->SXR,YEnd,BorderLines,0,-V);
			};
		if (BorderPixels>0)
			{
			GRend->DrawTiledTextureBlock(Camera,Border,0,BorderPixels,YStart,YEnd-YStart,0,-V);
			GRend->DrawTiledTextureBlock(Camera,Border,Camera->SXR-BorderPixels,BorderPixels,YStart,YEnd-YStart,0,-V);
			};
		};
	//
	// Draw logo:
	//
	if( LogoUp )
		{
		LogoUp=1;
		int USize,VSize, MipLevel=0;
		ITexture TextureInfo;
		BYTE *Data = GGfx.Logo->GetData(TextureInfo,&MipLevel,Camera->ColorBytes,&USize,&VSize);
		FLOAT XSize = Camera->FSXR * 0.7;
		FLOAT YSize = XSize * ((FLOAT)VSize/(FLOAT)USize);
		GRend->DrawScaledSprite(Camera,GGfx.Logo,0.5*(Camera->FSXR - XSize),0,XSize,YSize,BT_None,NULL,0,0);
		GGfx.Logo->Unlock(TextureInfo);
		};
	if (ConsoleLines)
		{
		//
		// Console is visible; display console view
		//
		int Y = ConsoleLines-1;
		sprintf(MsgText[(TopLine + 1 + MAX_LINES) % MAX_LINES],"(> %s_",TypedStr);
		for (int i=Scrollback; i<(NumLines+1); i++) // Display all text in the buffer
			{
			int Line = (TopLine + MAX_LINES*2 - (i-1)) % MAX_LINES;
			//
			int XL,YL;
			GGfx.WrappedStrLen
				(
				&XL,&YL,-1,0,
				GGfx.MedFont,Camera->SXR-8,MsgText[Line]
				);
			if (YL==0) YL=3; // Half-space blank lines
			//
			Y -= YL;
			if ((Y+YL)<0) break;
			//
			GGfx.WrappedPrintf
				(
				Camera->Texture,4,Y,
				-1,0,
				GGfx.MedFont,NormalFontColor,
				Camera->SXR-8,0,"%s",MsgText[Line]
				);
			};
		}
	else
		{
		//
		// Console is hidden; display single-line view
		//
		if ((NumLines>0) && (MsgType!=LOG_None)) // Display a status message
			{
			int iLine=TopLine;
			for (int i=0; i<NumLines; i++)
				{
				if (*MsgText[iLine]) break;
				iLine = (iLine-1+MAX_LINES)%MAX_LINES;
				};
			GGfx.WrappedPrintf
				(
				Camera->Texture,4,2,-1,0,
				GGfx.MedFont,NormalFontColor,
				Camera->SXR-8,1,
				"%s",MsgText[iLine]
				);
			};
		if (KeyState==CK_Type) // Draw stuff being typed:
			{
			int XL,YL;
			char S[256]; sprintf(S,"(> %s_",TypedStr);
			GGfx.WrappedStrLen
				(
				&XL,&YL,-1,0,
				GGfx.MedFont,Camera->SXR-8,S
				);
			GGfx.WrappedPrintf
				(
				Camera->Texture,2,Camera->SYR - ConsoleLines - StatusBarLines - YL - 1,
				-1,0,
				GGfx.MedFont,NormalFontColor,
				Camera->SXR-4,0,S
				);
			};
		};
	//
	// Remember old status info for later comparison:
	//
	memcpy(Old,this,sizeof(*this));
    Pawn.bStatusChanged = FALSE; // Indicates no change since last time status was drawn.
	//
	UNGUARD("FCameraConsole::PostRender");
	};

/*------------------------------------------------------------------------------
	Key
------------------------------------------------------------------------------*/

//
// Console keypress handler.  Returns 1 if processed, 0 if not.
//
int FCameraConsole::Key( int Key )
	{
	GUARD;
	APawn *Actor = &Camera->GetActor();
    const BOOL Changed = GApp->Input->Press( GApp->Input->WindowsKeySwitches[Key] );
	//
	// Keys recognized regardless of state:
	//
	switch(toupper(Key))
		{
		case 27: // Escape
			if (GCameraManager->FullscreenCamera && !IsTyping())
				{
				// Return from full-screen mode
				debug(LOG_Win,"Esc pressed: Ending fullscreen mode");
				GCameraManager->EndFullscreen();
				return 1;
				};
			break;
		case '`':
		case '~':
            return 1;
            break;
		};
	//
	// State-dependent keys:
	//
	switch (KeyState)
		{
		case CK_None:
			//
			// Normal input state - no typing, no menu.
			//
            switch(Key)
				{
				case 27:
					if (ConsoleDest!=0.0) ConsoleDest=0.0;
					break;
				case K_ENTER: // Toggle fullscreen mode
					if (GApp->KeyDown[K_ALT])
						{
						if(!(Camera->GetActor().ShowFlags & SHOW_ChildWindow))
							{
							GCameraManager->MakeFullscreen(Camera);
							};
						};
					break;
				case K_F11+256: // Brightness level
					char Temp[80];
					if (++GGfx.GammaLevel >= GGfx.NumGammaLevels) GGfx.GammaLevel = 0;
					GGfx.SetPalette();
					GCache.Flush();
					sprintf(Temp,"Brightness level %i/%i",GGfx.GammaLevel+1,GGfx.NumGammaLevels);
					Log(LOG_Info,Temp);
					return 1;
	            };
			return 0;
		case CK_Type:
			//
			// Typing a command:
			//
			int Len = strlen (TypedStr);
			switch(Key)
				{
				case 27:
					if (Scrollback)
						{
						Scrollback = 0;
						}
					else if (TypedStr[0])
						{
						ClearInput();
						}
					else
						{
						ConsoleDest=0.0;
						KeyState = CK_None;
						};
					Scrollback=0;
					break;
				case 13:
					if (Scrollback)
						{
						Scrollback = 0;
						}
					else
						{
						if (TypedStr[0])
							{
							Logf(LOG_Console,"(> %s",TypedStr);
							//
							// Update history buffer:
							//
							strcpy(History[HistoryTop],TypedStr);
							HistoryTop = (HistoryTop+1) % MAX_HISTORY;
							HistoryCur = (HistoryTop+1) % MAX_HISTORY;
							//
							// Make a local copy of the string, in case something
							// recursively affects the contents of the console, then
							// execute the typed string:
							//
							char Temp[256]; strcpy(Temp,TypedStr);
							//
							if		(Exec				(TypedStr,this)) {}
							else if	(Camera->Exec		(TypedStr,this)) {}
							else if (Camera->Level->Exec(TypedStr,this)) {}
							else if (GUnreal.Exec		(TypedStr,this)) {}
							};
						if (TypedStr[0]) Log(LOG_Console,"");
						//
						ClearInput();
						if (!ConsoleDest) KeyState = CK_None;
						Scrollback=0;
						};
					break;
				case K_UP+256:
					{
					int HistoryTemp = (HistoryCur - 1 + MAX_HISTORY) % MAX_HISTORY;
					if (HistoryTemp!=HistoryTop) HistoryCur = HistoryTemp;
					//
					strcpy(TypedStr,History[HistoryCur]);
					};
					Scrollback=0;
					break;
				case K_DOWN+256:
					if (HistoryCur==HistoryTop)
						{
						ClearInput();
						}
					else
						{
						HistoryCur = (HistoryCur+1) % MAX_HISTORY;
						strcpy(TypedStr,History[HistoryCur]);
						};
					Scrollback=0;
					break;
				case K_PAGEUP+256:
					if (++Scrollback >= MAX_LINES) Scrollback = MAX_LINES-1;
					break;
				case K_PAGEDOWN+256:
					if (--Scrollback < 0) Scrollback = 0;
					break;
				case K_RIGHT+256:
					break;
				case K_BACKSPACE:
				case K_LEFT+256:
					if (Len>0) TypedStr [Len-1] = 0;
					Scrollback=0;
					break;
				default:
					if (Len<(TEXTMSG_LENGTH-1))
						{
						TypedStr [Len]=Key; TypedStr [Len+1]=0;
						};
					Scrollback=0;
					break;
				};				
			return 1;
		};
	return 0; // Key was not processed
	UNGUARD("FCameraConsole::Key");
	};

/*------------------------------------------------------------------------------
	Tick
------------------------------------------------------------------------------*/

//
// Console timer tick.  Should be called every timer tick for every active
// camera whether a game is running or not.
//
void FCameraConsole::Tick (ICamera *Camera,int TicksPassed)
	{
	GUARD;
	//
	// Slide console up or down:
	//
	if (ConsolePos!=ConsoleDest)
		{
		FLOAT Delta = 0.05;
		//
		if (ConsolePos<ConsoleDest) ConsolePos = Min(ConsolePos+Delta,ConsoleDest);
		else						ConsolePos = Max(ConsolePos-Delta,ConsoleDest);
		};
	//
	// Update status message:
	//
	if ((GServer.Ticks - MsgStart) > MsgDuration) MsgType = LOG_None;
	//
	UNGUARD("FCameraConsole::Tick");
	};

/*------------------------------------------------------------------------------
	Status functions
------------------------------------------------------------------------------*/

//
// See if the user is typing something on the console.
// This is used to block the actions of certain player inputs,
// such as the space bar.
//
int FCameraConsole::IsTyping()
	{
	GUARD;
	return KeyState!=CK_None;
	UNGUARD("FCameraConsole::IsTyping");
	};

//----------------------------------------------------------------------------
//              Get the actions associated with the camera
//----------------------------------------------------------------------------
//todo: Can we eliminate this function?
class FAction * FCameraConsole::GetActions()
{
    return Actions;
}

//----------------------------------------------------------------------------
//              Get the input associated with the camera
//----------------------------------------------------------------------------
//todo: Can we eliminate this function?
class FInput * FCameraConsole::GetInput()
{
    return Input;
}

//----------------------------------------------------------------------------
//              Gather polled input and update the status of all actions
//----------------------------------------------------------------------------
void FCameraConsole::UpdateActionStatus()
{
    GAction.UpdateStatus( *Input, IsTyping() );
}

//----------------------------------------------------------------------------
//              Build a player action packet 
//----------------------------------------------------------------------------
void FCameraConsole::SetActionStatus( PPlayerTick & PlayerActions )
{
    //todo: We don't need the Camera parameter, since we have a member Camera.
    FVector         Move        ;
    FFloatRotation  Rotation    ;
    GCameraManager->GetStoredMove(Camera,&Move,&Rotation); // Called only to get dmTick (direct mouse support) called
    memmove( &PlayerActions.Movements, Actions->Movements, sizeof(PlayerActions.Movements) );
    memmove( &PlayerActions.Actions, Actions->Status, sizeof(Actions->Status) );
    Actions->EmptyMovements(); // Reset all movements since they might be accumulated later
}

//----------------------------------------------------------------------------
//             A switch was pressed
//----------------------------------------------------------------------------
void FCameraConsole::PressSwitch( int Switch_ )
{
    const FInput::TSwitch Switch = FInput::TSwitch(Switch_);
    //todo: Code
    Actions->TriggerActions(*Input,Switch,Camera);
}

//----------------------------------------------------------------------------
//             A switch was released
//----------------------------------------------------------------------------
void FCameraConsole::ReleaseSwitch( int Switch_ )
{
    const FInput::TSwitch Switch = FInput::TSwitch(Switch_);
    //todo: Code
}

/*------------------------------------------------------------------------------
	The End
------------------------------------------------------------------------------*/
