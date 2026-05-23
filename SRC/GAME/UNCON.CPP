/*=============================================================================
	UnCon.cpp: Implementation of FCameraConsole class

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.
=============================================================================*/

#include "Unreal.h"
#include "UnGame.h"
#include "UnCon.h"
#include "UnRender.h"

/*------------------------------------------------------------------------------
	Globals.
------------------------------------------------------------------------------*/

// These defines give the size of the displayable status bar graphics, because
// the status bar textures are forced to power-of-two sizes.
#define STATUSBAR_U  640.0
#define STATUSBAR_V  64.0

/*------------------------------------------------------------------------------
	Initializing and exiting a console.
------------------------------------------------------------------------------*/

//
// Init console for a particular camera.  Should be called immediately
// when a new camera is created and recognized.
//
void FCameraConsole::Init( UCamera *ThisCamera )
{
	guard(FCameraConsole::Init);

	Camera			= ThisCamera;

	KeyState		= CK_None;
	HistoryTop		= 0;
	HistoryBot		= 0;
	HistoryCur		= 0;
	for( int i=0; i<MAX_HISTORY; i++ )
		*History[i]=0;

	NumLines		= 0;
	TopLine			= MAX_LINES-1;
	MsgStart		= 0.0;
	MsgDuration 	= 0.0;
	for( i=0; i<MAX_LINES; i++ )
		*MsgText[i]=0;

	ConsolePos		= 0.0;
	ConsoleDest		= 0.0;

	LastTimeSeconds	= 0.0;
	StatusRefreshPages = 2;

	BorderSize		= 1; 
	Redraw			= 0;
	Scrollback		= 0;

	// Find all graphics objects.  These are imported
	// into the engine via the Graphics.mac macro and are
	// stored in the game's .gfx file in the System directory.
	StatusBar				= new("StatusBar",		FIND_Existing)UTexture;
	StatusSmallBar			= new("StatusBarS",		FIND_Existing)UTexture;
	ConBackground			= new("Console",		FIND_Existing)UTexture;
	Border					= new("Border",			FIND_Existing)UTexture;
	Hud						= new("Hud",			FIND_Existing)UTexture;
    
	// Start console log.
	Logf(CONSOLE_SPAWN_1);
	Logf(CONSOLE_SPAWN_2);
	if( Camera->Actor ) Logf("Console ready for %s",Camera->Actor->GetName());
	Logf(" ");

	unguard;
}

//
// Shut down a particular camera's console. Should be called before
// closing a camera.
//
void FCameraConsole::Exit()
{
	guard(FCameraConsole::Exit);
	unguard;
}

/*------------------------------------------------------------------------------
	Utility functions.
------------------------------------------------------------------------------*/

void FCameraConsole::NoteResize()
{
	guard(FCameraConsole::NoteResize);
	Redraw++;
	unguard;
}

/*------------------------------------------------------------------------------
	Console command-line.
------------------------------------------------------------------------------*/

// FCameraConsole command line.
int FCameraConsole::Exec( const char *Cmd, FOutputDevice *Out )
{
	guard(FCameraConsole::Exec);
	const char *Str = Cmd;

	if( GetCMD(&Str,"ENDFULLSCREEN") )
	{
		// Return from full-screen mode.
		if( GCameraManager->FullscreenCamera && !IsTyping() )
		{
			debug(LOG_Info,"Esc pressed: Ending fullscreen mode");
			GCameraManager->EndFullscreen();
		}
		return 1;
	}
	if( GetCMD(&Str,"CONSOLE") )
	{
		int IsToggle = GetCMD(&Str,"TOGGLE");
		if( GetCMD(&Str,"HIDE") || (IsToggle && ConsoleDest!=0.0) )
		{
			ConsoleDest = 0.0;
			KeyState	= CK_None;
			return 1;
		}
		else if( GetCMD(&Str,"SHOW") || IsToggle )
		{
			KeyState	= CK_Type;
			ConsoleDest = CON_SHOW;
			return 1;
		}
		else if( GetCMD(&Str,"FULL") )
		{
			KeyState	= CK_Type;
			ConsoleDest = 1.0;
			return 1;
		}
		else return 0;
	}
	else if( GetCMD(&Str,"BRIGHTNESS") )
	{
		if( ++GGfx.GammaLevel >= GGfx.NumGammaLevels )
			GGfx.GammaLevel = 0;
		GCameraManager->SetPalette(GGfx.DefaultPalette);
		GCache.Flush();
		Logf(LOG_Info,"Brightness level %i/%i",GGfx.GammaLevel+1,GGfx.NumGammaLevels);
		return 1;
	}
	else if( GetCMD(&Str,"TYPE") )
	{
		Camera->Input->ResetInput();
        strcpy( TypedStr, "" );
		KeyState = (KeyState==CK_Type) ? CK_None : CK_Type;
		return 1;
    }
	else if( GetCMD(&Str,"CHAT") )
	{
		Camera->Input->ResetInput();
		KeyState = CK_Type;
        strcpy( TypedStr, "Say " );
		return 1;
    }
	else if( GetCMD(&Str,"VIEWUP") )
	{
		if( --BorderSize < 0 ) BorderSize=0;
		return 1;
	}
    else if( GetCMD(&Str,"VIEWDOWN") )
    {
		if( ++BorderSize >= MAX_BORDER ) BorderSize=MAX_BORDER-1;
		return 1;
    }
	else if( PeekCMD(Str,"PN") || PeekCMD(Str,"PP") )
	{
		int d=+1;
		if		(GetCMD(&Str,"PP")) d=-1;
		else if (GetCMD(&Str,"PN")) d=+1;

		UClass *DestClass;

		if( Camera->Actor->ShowFlags & SHOW_StandardView )
		{
			Out->Log("Must open a free camera to possess");
			return 1;
		}
		if( !Camera->Lock(LOCK_ReadWrite|LOCK_CanFail) )
			return 1;

		// Try to possess an available actor.
		DestClass = GetCMD(&Str,"ALIKE") ? Camera->Actor->GetClass() : NULL;

		int n=Camera->Level->GetActorIndex(Camera->Actor), Original=n, Failed=1;
		for( ;; )
		{
			n += d;
			if      (n >= Camera->Level->Num)	n=0;
			else if (n < 0)					    n=Camera->Level->Num-1;

			if( n == Original )
				break;

			AActor *TestActor = Camera->Level->Element(n);
			if
			(	TestActor
			&&	TestActor->IsA("Pawn")
			&&	!TestActor->GetPlayer()
			&&	((APawn*)TestActor)->bCanPossess && !TestActor->bHiddenEd
			)
			{
				if( TestActor->GetClass()==DestClass || DestClass==NULL )
				{
					//Camera->Level->UnpossessActor(*Camera->Actor);
					Camera->Level->PossessActor( (APawn*)TestActor, Camera );

					Failed=0;
					break;
				}
			}
		}
		Camera->Unlock(LOCK_ReadWrite,0);
		GCameraManager->UpdateCameraWindow(Camera);

		// Update player console window.
		if( Failed )
		{
			Out->Log(LOG_Info,"Couldn't possess");
		}
		else
		{
			char Who[256];
			strcpy( Who,"Possessed a " );
			strcat( Who, Camera->Actor->GetClassName() );
			Out->Log(Who);
		}
		return 1;
	}
	else if( GetCMD(&Str,"LEVELSTATS") )
	{
		// Show level stats.
        const ALevelInfo &Info = *(const ALevelInfo *)Camera->Level->Element(0);
        Out->Logf
        ( 
            "Kills: %i/%i  Items: %i/%i Secrets: %i/%i"
        ,   int(Camera->Actor->KillCount),   int(Info.KillGoals)
        ,   int(Camera->Actor->ItemCount),   int(Info.ItemGoals)
        ,   int(Camera->Actor->SecretCount), int(Info.SecretGoals)
        );
        return 1;
    }
	else
	{
		// Not handled.
		return 0;
	}
	unguard;
}

/*------------------------------------------------------------------------------
	Camera console output.
------------------------------------------------------------------------------*/

//
// Print a message on the playing screen.
// Time = time to keep message going, or 0=until next message arrives, in 60ths sec
//
void FCameraConsole::Write( const void *Data, int Length, ELogType ThisType )
{
	guard(FCameraConsole::Log);

	TopLine		= (TopLine+1) % MAX_LINES;
	NumLines	= Min(NumLines+1,(int)(MAX_LINES-1));

	strncpy(MsgText[TopLine],(char*)Data,TEXTMSG_LENGTH);
	MsgText[TopLine][TEXTMSG_LENGTH-1] = 0;

	MsgType  	= ThisType;
	MsgStart 	= GServer.TimeSeconds;
	MsgDuration	= MESSAGE_TIME;

	unguard;
}

/*------------------------------------------------------------------------------
	Rendering.
------------------------------------------------------------------------------*/

//
// Called before rendering the world view.  Here, the
// camera console code can affect the screen's camera,
// for example by shrinking the view according to the
// size of the status bar.
//
FMemMark Mark;
UCamera *GSavedCamera;
void FCameraConsole::PreRender( UCamera *Camera )
{
	guard(FCameraConsole::PreRender);

	// Prevent status redraw due to changing.
	LastTimeSeconds      = GServer.TimeSeconds;
	Old->LastTimeSeconds = GServer.TimeSeconds;
	Tick( Camera, 1 );

	// Save the camera.
	Mark.Push(GMem);
	GSavedCamera=(UCamera *)new(GMem,sizeof(UCamera))BYTE;
	memcpy(GSavedCamera,Camera,sizeof(UCamera));

	// Compute new status info.
	SXR				= Camera->X;
	SYR				= Camera->Y;
	BorderLines		= 0;
	BorderPixels	= 0;
	StatusBarLines	= 0;
	ConsoleLines	= 0;

	// Compute sizing of all visible status bar components.
	if( Camera->IsGame() )
	{
		if( BorderSize>=1 )
		{
			// Status bar.
			//!!StatusBarLines = Min((FLOAT)(Camera->FX * STATUSBAR_V/STATUSBAR_U),(FLOAT)SYR);
			StatusBarLines=0;
			SYR -= StatusBarLines;
		}
		if( ConsolePos > 0.0 )
		{
			// Show console.
			ConsoleLines = Min(ConsolePos * (FLOAT)SYR,(FLOAT)SYR);
			SYR -= ConsoleLines;
		}
		if( BorderSize>=2 )
		{
			// Encroach on screen area.
			FLOAT Fraction = (FLOAT)(BorderSize-1) / (FLOAT)(MAX_BORDER-1);

			BorderLines = (int)Min((FLOAT)SYR * 0.25f * Fraction,(FLOAT)SYR);
			BorderLines = Max(0,BorderLines - ConsoleLines);
			SYR -= 2 * BorderLines;

			BorderPixels = (int)Min((FLOAT)SXR * 0.25f * Fraction,(FLOAT)SXR) & ~3;
			SXR -= 2 * BorderPixels;
		}
		Camera->FXB   += BorderPixels;
		Camera->FYB   += ConsoleLines + BorderLines;
		Camera->Screen = &Camera->Screen
		[
			Camera->ColorBytes * 
			(BorderPixels + (ConsoleLines + BorderLines) * Camera->Stride)
		];
		Camera->PrecomputeRenderInfo(SXR,SYR);
	}
	unguard;
}

//
// Refresh the player console on the specified camera.  This is called after
// all in-game graphics are drawn in the rendering loop, and it overdraws stuff
// with the status bar, menus, and chat text.
//
void FCameraConsole::PostRender(UCamera *Camera,int XLeft)
{
	guard(FCameraConsole::PostRender);

	// Restore the previously-saved camera.
	memcpy(Camera,GSavedCamera,sizeof(UCamera));
	Mark.Pop();

	// If the console has changed since the previous frame, draw it.
    APawn &Pawn = *this->Camera->Actor; // Camera actor is always a pawn.
    const BOOL PawnStatusChanged = 0;//Pawn.bStatusChanged;
	int Changed		= memcmp(this,Old,sizeof(*this)) || PawnStatusChanged;
	int DrawStatus  = Changed || (StatusRefreshPages>0);
	int YStart		= BorderLines;
	int YEnd		= Camera->Y - BorderLines-StatusBarLines;
	int DrawBorder	= DrawStatus || (MsgType != LOG_None) || (KeyState==CK_Type);

	if( DrawStatus )
	{
		if (Changed) StatusRefreshPages = 2; // Handle proper refresh in triple-buffered mode
		else StatusRefreshPages--;

		// Draw status bar.
		/*
		if( StatusBarLines > 0 )
		{
            const FLOAT XScale = FLOAT(Camera->FX) / STATUSBAR_U;
            const FLOAT YScale = FLOAT(StatusBarLines) / STATUSBAR_V;
            const FLOAT StatusBarX = 0;
            const FLOAT StatusBarY = Camera->FY-StatusBarLines;
            const BOOL UseSmallBar = XScale <= 0.5;
            UTexture * Bar = UseSmallBar ? StatusSmallBar : StatusBar;
            const FLOAT AdditionalScale = UseSmallBar ? 2.0 : 1.0;
			
			GRend->DrawScaledSprite
			(
				Camera,
				Bar,
				0,
				Camera->FY-StatusBarLines,
				Camera->FX            * (FLOAT)Bar->USize / STATUSBAR_U * AdditionalScale + 0.5,
				(FLOAT)StatusBarLines * (FLOAT)Bar->VSize / STATUSBAR_V * AdditionalScale + 0.5,
				BT_None,
				NULL,
				0,
				0,
				1000.0
			);
		}*/

		// Draw console.
		if( ConsoleLines > 0 ) GRend->DrawTiledTextureBlock
		(
			Camera,ConBackground,0,Camera->X,0,ConsoleLines,0,-ConsoleLines
		);
	}

	// Draw border.
	if( DrawBorder && ((BorderLines>0)||(BorderPixels>0)) )
	{
		YStart += ConsoleLines;
		int V = ConsoleLines>>1;
		if( BorderLines > 0 )
		{
			GRend->DrawTiledTextureBlock(Camera,Border,0,Camera->X,0,BorderLines,0,-V);
			GRend->DrawTiledTextureBlock(Camera,Border,0,Camera->X,YEnd,BorderLines,0,-V);
		}
		if( BorderPixels > 0 )
		{
			GRend->DrawTiledTextureBlock(Camera,Border,0,BorderPixels,YStart,YEnd-YStart,0,-V);
			GRend->DrawTiledTextureBlock(Camera,Border,Camera->X-BorderPixels,BorderPixels,YStart,YEnd-YStart,0,-V);
		}
	}

	if( !GEditor && BorderSize >= 1 && Camera->X > 64+8 && Camera->Y > 64+8) GRend->DrawScaledSprite
	(
		Camera,
		Hud,
		Camera->FX - (64+4),
		Camera->FY - (64+3),
		64 + 0.5,
		64 + 0.5,
		GCameraManager->RenDev ? BT_Transparent : BT_None,
		NULL,
		0,
		0,
		1.0
	);

	// Draw console text.
	if( ConsoleLines )
	{
		// Console is visible; display console view.
		int Y = ConsoleLines-1;
		sprintf(MsgText[(TopLine + 1 + MAX_LINES) % MAX_LINES],"(> %s_",TypedStr);
		for( int i=Scrollback; i<(NumLines+1); i++ )
		{
			// Display all text in the buffer.
			int Line = (TopLine + MAX_LINES*2 - (i-1)) % MAX_LINES;

			int XL,YL;
			GGfx.MedFont->WrappedStrLen
			(
				XL,YL,-1,0,
				Camera->X-8,MsgText[Line]
			);
			if (YL==0) YL=3; // Half-space blank lines.

			Y -= YL;
			if ((Y+YL)<0) break;
			GGfx.MedFont->WrappedPrintf
			(
				Camera->Texture,
				4,
				Y,
				-1,
				0,
				Camera->X-8,
				0,
				"%s",
				MsgText[Line]
			);
		}
	}
	else
	{
		// Console is hidden; display single-line view.
		if ( NumLines>0 && MsgType!=LOG_None )
		{
			int iLine=TopLine;
			for( int i=0; i<NumLines; i++ )
			{
				if( *MsgText[iLine] )
					break;
				iLine = (iLine-1+MAX_LINES)%MAX_LINES;
			}
			GGfx.MedFont->WrappedPrintf
			(
				Camera->Texture,
				4,
				2,
				-1,
				0,
				Camera->X-8,
				1,
				"%s",
				MsgText[iLine]
			);
		}
		if( KeyState == CK_Type ) // Draw stuff being typed.
		{
			int XL,YL;
			char S[256]; sprintf(S,"(> %s_",TypedStr);
			GGfx.MedFont->WrappedStrLen
			(
				XL,YL,-1,0,
				Camera->X-8,S
			);
			GGfx.MedFont->WrappedPrintf
			(
				Camera->Texture,
				2,
				Camera->Y - ConsoleLines - StatusBarLines - YL - 1,
				-1,
				0,
				Camera->X-4,
				0,
				S
			);
		}
	}

	// Remember old status info for later comparison.
	memcpy( Old, this, sizeof(*this) );

	unguard;
}

/*------------------------------------------------------------------------------
	Keypress and input filtering.
------------------------------------------------------------------------------*/

// Filter a low level input system character.
// Returns 1 if intercepted, 0 if passed through.
int	FCameraConsole::Process(EInputKey iKey, EInputState State, FLOAT Delta )
{
	guard(FCameraConsole::Process);

	// Here we only care about presses.
	if( State != IST_Press )
		return 0;

	// Keys that always take effect.
	if( iKey==IK_Enter && Camera->Input->KeyDown(IK_Alt) )
	{	
		// Toggle fullscreen.
		if( !(Camera->Actor->ShowFlags & SHOW_ChildWindow) )
			GCameraManager->MakeFullscreen(Camera);
		return 1;
	}

	// Handle keystroke based on KeyState.
	if( KeyState == CK_None )
	{
		// No key state.
		if( iKey==IK_Escape )
		{
			if( TypedStr[0] )
				TypedStr[0]=0;
			else
				ConsoleDest=0.0;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if( KeyState==CK_Type )
	{
		// Typing a command.
		if( iKey==IK_Escape )
		{
			if( Scrollback )
			{
				Scrollback = 0;
			}
			else if( TypedStr[0] )
			{
				TypedStr[0]=0;
			}
			else
			{
				ConsoleDest=0.0;
				KeyState = CK_None;
			}
			Scrollback=0;
			return 1;
		}
		else if( iKey==IK_Enter )
		{
			if( Scrollback )
			{
				Scrollback = 0;
				return 1;
			}
			else
			{
				if( TypedStr[0] )
				{
					if( ConsoleLines )
						Logf( LOG_Console, "(> %s",TypedStr );

					// Update history buffer.
					strcpy(History[HistoryCur % MAX_HISTORY],TypedStr);
					HistoryCur++;
					if( HistoryCur > HistoryBot )
						HistoryBot++;
					if( HistoryCur - HistoryTop >= MAX_HISTORY)
						HistoryTop = HistoryCur - MAX_HISTORY + 1;

					// Make a local copy of the string, in case something
					// recursively affects the contents of the console, then
					// execute the typed string.
					char Temp[256]; strcpy(Temp,TypedStr);
					Camera->Exec( TypedStr, this );
					Log(LOG_Console,"");
				}
				TypedStr[0]=0;
				if( !ConsoleDest )
					KeyState = CK_None;
				Scrollback = 0;
				return 1;
			}
		}
		else if( iKey==IK_Up )
		{
			if( HistoryCur > HistoryTop )
			{
				strcpy(History[HistoryCur % MAX_HISTORY],TypedStr);
				strcpy(TypedStr,History[--HistoryCur % MAX_HISTORY]);
			}
			Scrollback=0;
			return 1;
		}
		else if( iKey==IK_Down )
		{
			strcpy(History[HistoryCur % MAX_HISTORY],TypedStr);
			if( HistoryCur < HistoryBot )
				strcpy(TypedStr,History[++HistoryCur % MAX_HISTORY]);
			else
				strcpy(TypedStr,"");
			Scrollback=0;
			return 1;
		}
		else if( iKey==IK_PageUp )
		{
			if( ++Scrollback >= MAX_LINES )
				Scrollback = MAX_LINES-1;
			return 1;
		}
		else if( iKey==IK_PageDown )
		{
			if( --Scrollback < 0 )
				Scrollback = 0;
			return 1;
		}
		else if( iKey==IK_Backspace || iKey==IK_Left )
		{
			int Len = strlen (TypedStr);
			if( Len>0 ) TypedStr [Len-1] = 0;
			Scrollback=0;
			return 1;
		}
		else
		{
			// Don't let commands get fed through to the input system while typing.
			return 1;
		}
	}
	else
	{
		// Other KeyState.
		return 0;
	}
	unguard;
}

// Console keypress handler.  Returns 1 if processed, 0 if not.
// Returns 1 if intercepted, 0 if passed through.
int FCameraConsole::Key( int Key )
{
	guard(FCameraConsole::Key);

	APawn *Actor = Camera->Actor;
    const BOOL Changed = 0;//!!GApp->Input->Press( GApp->Input->WindowsKeySwitches[Key&255] );

	// State-dependent keys.
	if( Key=='~' || Key=='`' )
	{
		// Console up/down.
		if      ( ConsoleDest != 0.0               ) Exec("Console Hide");
		else if ( Camera->Input->KeyDown(IK_Shift) ) Exec("Console Full");
		else										 Exec("Console Show");
		return 1;
	}
	if( KeyState == CK_Type )
	{
		// Typing a command.
		if( Key>=0x20 && Key <0x80 )
		{
			// Typing a command.
			int Len = strlen (TypedStr);
			if( Len<(TEXTMSG_LENGTH-1) )
			{
				TypedStr [Len]=Key; TypedStr [Len+1]=0;
			}
			Scrollback=0;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{	
		// Other state.
		return 0;
	}
	unguard;
}

/*------------------------------------------------------------------------------
	Tick.
------------------------------------------------------------------------------*/

//
// Console timer tick.  Should be called every timer tick for every active
// camera whether a game is running or not.
//
void FCameraConsole::Tick( UCamera *Camera, int TicksPassed )
{
	guard(FCameraConsole::Tick);

	// Slide console up or down.
	if( ConsolePos != ConsoleDest )
	{
		FLOAT Delta = 0.05;

		if( ConsolePos < ConsoleDest ) ConsolePos = Min(ConsolePos+Delta,ConsoleDest);
		else						   ConsolePos = Max(ConsolePos-Delta,ConsoleDest);
	}

	// Update status message.
	if( GServer.TimeSeconds - MsgStart > MsgDuration )
		MsgType = LOG_None;

	unguard;
}

/*------------------------------------------------------------------------------
	Status functions.
------------------------------------------------------------------------------*/

//
// See if the user is typing something on the console.
// This is used to block the actions of certain player inputs,
// such as the space bar.
//
int FCameraConsole::IsTyping()
{
	guard(FCameraConsole::IsTyping);
	return KeyState!=CK_None;
	unguard;
}

/*------------------------------------------------------------------------------
	Input translating.
------------------------------------------------------------------------------*/

//
// Perform any game-specific postprocessing on the camera input stream,
// after it has been read off the camera, and before it is passed to
// the PlayerTick message.
//
void FCameraConsole::PostReadInput( PPlayerTick &Move, FLOAT DeltaSeconds, FOutputDevice *Out )
{
	guard(FCameraConsole::PostReadInput);

	// Remap raw x-axis movement:
	if( Move.Buttons[BUT_Strafe] )
	{
		// Strafe.
		Move.Axis[AXIS_Strafe] += Move.Axis[AXIS_BaseX] + Move.Axis[AXIS_MouseX];
		Move.Axis[AXIS_BaseX] = Move.Axis[AXIS_MouseX] = 0;
	}
	else
	{
		// Forward.
		Move.Axis[AXIS_Turn] += Move.Axis[AXIS_BaseX] + Move.Axis[AXIS_MouseX];
		Move.Axis[AXIS_BaseX] = Move.Axis[AXIS_MouseX] = 0;
	}

	// Remap mouse y-axis movement.
	if( Move.Buttons[BUT_Look] )
	{
		// Look up/down.
		Move.Axis[AXIS_LookUp] += Move.Axis[AXIS_MouseY];
		Move.Axis[AXIS_MouseY]  = 0;
	}
	else
	{
		// Move forward/backward.
		Move.Axis[AXIS_Forward] += Move.Axis[AXIS_MouseY];
		Move.Axis[AXIS_MouseY] = 0;
	}

	// Remap other y-axis movement.
	Move.Axis[AXIS_Forward] += Move.Axis[AXIS_BaseY];
	Move.Axis[AXIS_BaseY] = 0;

	// Handle running.
	if( Move.Buttons[BUT_Run] )
	{
		Move.Axis[AXIS_Forward] *= 2.0;
		Move.Axis[AXIS_Strafe] *= 2.0;
		Move.Axis[AXIS_Up] *= 2.0;
	}

	unguard;
}

/*------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------*/
