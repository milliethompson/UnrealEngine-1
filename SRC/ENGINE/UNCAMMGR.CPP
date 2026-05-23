/*=============================================================================
	UnCamMgr.cpp: Unreal camera manager, generic implementation

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*-----------------------------------------------------------------------------
	FCameraManagerBase generic implementation.
-----------------------------------------------------------------------------*/

//
// Redraw all cameras looking at the level.
//
void FCameraManagerBase::RedrawLevel( ULevel *Level )
{
	guard(FCameraManagerBase::RedrawLevel);

	for( int i=0; i<CameraArray->Num; i++ )
	{
		UCamera *Camera = CameraArray->Element(i);
	    if
		(	(Camera->Level == Level)
	    &&	(!Camera->OnHold)
		&&	(Camera->X)
		&&	(Camera->Y) )
		{
			Camera->Draw( 0 );
		}
	}
	unguard;
}

//
// Close all cameras that are child windows of a specified window.
//
void FCameraManagerBase::CloseWindowChildren( DWORD ParentWindow )
{
	guard(FCameraManagerBase::CloseWindowChildren);

	char TempName[NAME_SIZE];
	for( int i=CameraArray->Num-1; i>=0; i-- )
	{
		UCamera *Camera = CameraArray->Element(i);
		strcpy (TempName,Camera->GetName()); strupr(TempName);

		if( (ParentWindow==0) ||
			(Camera->ParentWindow==ParentWindow) ||
			((ParentWindow==MAXDWORD)&&!strstr(TempName,"STANDARD") )
			)
		{
			Camera->Kill();
		}
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Reconciling actors and cameras after loading or creating a new level.
-----------------------------------------------------------------------------*/

//
// Update all actor Camera references in the world based on the Cameras
// controlling them.
//
void FCameraManagerBase::UpdateActorUsers()
{
	guard(FCameraManagerBase::UpdateActorUsers);
	int i;

	// Dissociate all Cameras and cameras from all actors.
	if( GServer.GetLevel() )
		GServer.GetLevel()->DissociateActors();

	// Hook all cameras up to their corresponding actors.
	for( i=0; i<GCameraManager->CameraArray->Num; i++ )
	{
		UCamera *Camera = GCameraManager->CameraArray->Element(i);		
		if( !Camera->Actor || !Camera->Actor->GetClass() || !Camera->Actor->IsA("Pawn") )
		{
			debugf( LOG_Problem, "Bad actor association" );
			Camera->Kill();
		}
		else Camera->Actor->Camera = Camera;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
