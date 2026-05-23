/*=============================================================================
	UnEditor.cpp: Unreal editor main file

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*---------------------------------------------------------------------------------------
	Globals.
---------------------------------------------------------------------------------------*/

UNEDITOR_API FGlobalEditor GUnrealEditor;

/*-----------------------------------------------------------------------------
	Init & Exit.
-----------------------------------------------------------------------------*/

//
// Init the editor:
//
void FGlobalEditor::Init()
{
	guard(FGlobalEditor::Init);
	
	// Make sure properties match up.
	VERIFY_CLASS_OFFSET(A,Actor,Owner);
	VERIFY_CLASS_OFFSET(A,Pawn,Player);

	// Init misc.
	GGfx.DefaultCameraFlags  = SHOW_Frame | SHOW_MovingBrushes | SHOW_Actors | SHOW_Brush | SHOW_Menu;
	GGfx.DefaultRendMap      = REN_PlainTex;
	MacroRecBuffer = NULL;

	// Allocate editor object array.
	EditorArray = new("Editor",CREATE_Unique)UArray(0);
	GObj.AddToRoot(EditorArray);

	// Transaction tracking system.
	GUndo = GTrans = new("Undo",CREATE_Unique)UTransBuffer
	(
		GDefaults.MaxTrans,
		GDefaults.MaxChanges,
		GDefaults.MaxDataOffset
	);
	EditorArray->AddItem(GTrans);

	// Allocate temporary model.
	TempModel = new("Temp",CREATE_Unique,RF_NotForClient|RF_NotForServer)UModel( 1, 1 );
	EditorArray->AddItem(TempModel);

	// Default actor class to add.
	CurrentClass = NULL;

	// Current texture.
	CurrentTexture = NULL;

	// Texture set list.
	TextureSets = new("TextureSets",CREATE_Unique)TArray<UTextureSet*>(0);
	EditorArray->AddItem(TextureSets);

	// Settings.
	Mode			= EM_None;
	MovementSpeed	= 4.0;
	MapEdit         = 0;
	ShowVertices	= 0;
	FastRebuild		= 0;
	Bootstrapping	= 0;

	// Constraints.
	constraintInit (&Constraints);

	// Set editor mode.
	edcamSetMode (EM_CameraMove);

	// Subsystem init messsage.	
	debug(LOG_Init,"Editor initialized");
	unguard;
};

void FGlobalEditor::Exit()
{
	guard(FGlobalEditor::Exit);

	// Shut down transaction tracking system.
	if( GTrans )
	{
		if( GTrans->IsLocked() )
			debug(LOG_Problem,"Warning: A transaction is active");

		// Purge any unused objects held in undo-limbo.
		GTrans->Reset ("shutdown");

		GUnrealEditor.EditorArray->RemoveItem(GTrans);
		GTrans->Kill();
		GTrans=NULL;

		debug (LOG_Exit,"Transaction tracking system closed");
	}

	// Remove editor array from root.
	GObj.RemoveFromRoot(EditorArray);
	debug(LOG_Exit,"Editor closed");

	unguard;
}

/*-----------------------------------------------------------------------------
	Class loading.
-----------------------------------------------------------------------------*/

void FGlobalEditor::LoadClasses()
{
	guard(FGlobalEditor::LoadClasses);

	// Load classes for UnrealEd.
	if( !GObj.AddFile(DEFAULT_CLASS_FNAME,&ClassLinker,1) )
	{
		// Import the classes from their source.
		Bootstrapping = 1;
		GApp->BeginSlowTask("Building " DEFAULT_CLASS_FNAME,1,0);
		Exec("MACRO PLAY NAME=Classes FILE=" CLASS_BOOTSTRAP_FNAME);
		GApp->EndSlowTask();
		Bootstrapping = 0;

		// Try to load the newly saved classes.
		if( !GObj.AddFile( DEFAULT_CLASS_FNAME, &ClassLinker ) )
			appError( "Failed loading classes after rebuild" );
	}
	GEditor->EditorArray->AddItem((UObject*)ClassLinker);
	unguard;
}

/*-----------------------------------------------------------------------------
	Garbage collection.
-----------------------------------------------------------------------------*/

//
// Clean up after a major event like loading a file.
//
void FGlobalEditor::Cleanse( FOutputDevice& Out, int Redraw, const char *TransReset )
{
	guard(FGlobalEditor::Cleanse);

	// Perform an object purge.
	if( GApp->ServerAlive && !Bootstrapping )
	{
		GObj.CollectGarbage(&Out);

		// Reset the transaction tracking system if desired.
		if( TransReset )
			GTrans->Reset( TransReset );

		// Flush the cache.
		GCache.Flush();

		// Redraw the level.
		if( Redraw )
			GCameraManager->RedrawLevel(GServer.GetLevel());
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Macro recording.
-----------------------------------------------------------------------------*/

void FGlobalEditor::NoteMacroCommand(const char *Cmd)
{
	guard(FGlobalEditor::NoteMacroCommand);
	if( MacroRecBuffer )
	{
		MacroRecBuffer->Log(Cmd);
		MacroRecBuffer->Log("\r\n");
	}
	unguard;
}

/*---------------------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------------------*/
