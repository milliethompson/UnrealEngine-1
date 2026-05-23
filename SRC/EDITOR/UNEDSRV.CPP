/*=============================================================================
	UnEdSrv.cpp: FGlobalEditor implementation, the Unreal editing server

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	What's happening: When the Visual Basic level editor is being used,
	this code exchanges messages with Visual Basic.  This lets Visual Basic
	affect the world, and it gives us a way of sending world information back
	to Visual Basic.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

#pragma DISABLE_OPTIMIZATION /* Not performance-critical */

/*-----------------------------------------------------------------------------
	UnrealEd command line.
-----------------------------------------------------------------------------*/

//
// Process an incoming network message meant for the editor server
//
int FGlobalEditor::Exec
(
	const char		*Stream,
	FOutputDevice	*Out
)
{
	char ErrorTemp[256]="Setup: ";
	guard(FGlobalEditor::Exec);

	ULevel			*Level		= GServer.GetLevel();
	INT				ModeClass	= edcamModeClass(Mode);
	FVector 		TempVector;
	FRotation		TempRotation;
	UModel			*Brush;
	UClass			*Class;
	WORD	 		Word1,Word2,Word3,Word4;
	INDEX			Index1;
	ETexAlign		TexAlign;
	ECsgOper		CsgType;
	char	 		TempStr[256],TempStr1[256],TempFname[256],TempName[256],Temp[256];
	int				SaveAsMacro,DWord1,DWord2;
	int				Processed=0;

	if( strlen(Stream)<200 ) strcat( ErrorTemp, Stream );

	Brush = Level ? Level->Brush() : NULL;

	SaveAsMacro = (MacroRecBuffer != NULL);

	mystrncpy(Temp,Stream,256);
	const char *Str = &Temp[0];
	if (strchr(Str,';')) *strchr(Str,';') = 0; // Kill comments

	strncpy(ErrorTemp,Str,79);
	ErrorTemp[79]=0;

	//------------------------------------------------------------------------------------
	// BRUSH
	//
	if( GetCMD(&Str,"BRUSH") )
	{
		if( GetCMD(&Str,"SET") )
		{
			GTrans->Begin(Level,"Brush Set");
			Brush->Lock(LOCK_Trans);
			Brush->Polys->ModifyAllItems();
			Brush->Unlock(LOCK_Trans);
			Brush->Init(0);
			NoteMacroCommand(Stream);
			Brush->Polys->ParseFPolys(&Stream,0,1);
			bspValidateBrush(Brush,1,1);
			GTrans->End	();
			GCameraManager->RedrawLevel(Level);
			SaveAsMacro = 0;
			Processed = 1;
		}
		else if( GetCMD(&Str,"MORE") )
		{
			GTrans->Continue();
			Brush->Lock(LOCK_Trans);
			Brush->Polys->ModifyAllItems();
			Brush->Unlock(LOCK_Trans);
			NoteMacroCommand(Stream);
			Brush->Polys->ParseFPolys(&Stream,1,1);
			bspValidateBrush(Brush,1,1);
			GTrans->End();	
			GCameraManager->RedrawLevel(Level);
			SaveAsMacro = 0;
			Processed = 1;
		}
		else if( GetCMD(&Str,"RESET") )
		{
			GTrans->Begin(Level,"Brush Reset");
			GTrans->NoteResHeader(Brush);
			Brush->Init(1);
			Brush->BuildBound(1);
			GTrans->End();
			GCameraManager->RedrawLevel(Level);
			Processed = 1;
		}
		else if( GetCMD(&Str,"MIRROR") )
		{
			GTrans->Begin		(Level,"Brush Mirror");
			Brush->Lock			(LOCK_Trans);
			if (GetCMD(&Str,"X")) Brush->Scale.Scale.X *= -1.0;
			if (GetCMD(&Str,"Y")) Brush->Scale.Scale.Y *= -1.0;
			if (GetCMD(&Str,"Z")) Brush->Scale.Scale.Z *= -1.0;
			Brush->Unlock		(LOCK_Trans);
			Brush->BuildBound	(1);
			GTrans->End			();
			GCameraManager->RedrawLevel(Level);
			Processed = 1;
		}
		else if( GetCMD(&Str,"SCALE") )
		{
			GTrans->Begin(Level,"Brush Scale");
			Brush->Lock(LOCK_Trans);
			if( GetCMD(&Str,"RESET") )
			{
				Brush->Scale     = GMath.UnitScale;
				Brush->PostScale = GMath.UnitScale;
			}
			else
			{
				GetFVECTOR(Str,&Brush->Scale.Scale);
				GetFLOAT(Str,"SHEER=",&Brush->Scale.SheerRate);
				if( GetSTRING (Str,"SHEERAXIS=",TempStr,255) )
				{
					if      (stricmp(TempStr,"XY")==0)	Brush->Scale.SheerAxis = SHEER_XY;
					else if (stricmp(TempStr,"XZ")==0)	Brush->Scale.SheerAxis = SHEER_XZ;
					else if (stricmp(TempStr,"YX")==0)	Brush->Scale.SheerAxis = SHEER_YX;
					else if (stricmp(TempStr,"YZ")==0)	Brush->Scale.SheerAxis = SHEER_YZ;
					else if (stricmp(TempStr,"ZX")==0)	Brush->Scale.SheerAxis = SHEER_ZX;
					else if (stricmp(TempStr,"ZY")==0)	Brush->Scale.SheerAxis = SHEER_ZY;
					else								Brush->Scale.SheerAxis = SHEER_None;
				}
			}
			Brush->Unlock(LOCK_Trans);
			Brush->BuildBound(1);
			GTrans->End();
			GCameraManager->RedrawLevel(Level);
			Processed = 1;
		}
		else if( GetCMD(&Str,"APPLYTRANSFORM") )
		{
			GTrans->Begin(Level,"Brush ApplyTransform");
			Brush->Transform();
			Brush->BuildBound(1);
			GTrans->End();
			GCameraManager->RedrawLevel(Level);
			Processed = 1;
		}
		else if( GetCMD(&Str,"ROTATETO") )
		{
			GTrans->Begin(Level,"Brush RotateTo");
			Brush->Lock(LOCK_Trans);
			GetFROTATION(Str,&Brush->Rotation,256);
			Brush->Unlock(LOCK_Trans);
			Brush->BuildBound(1);
			GTrans->End();
			GCameraManager->RedrawLevel(Level);
			Processed = 1;
		}
		else if( GetCMD(&Str,"ROTATEREL") )
		{
			GTrans->Begin(Level,"Brush RotateRel");
			Brush->Lock(LOCK_Trans);
			TempRotation = FRotation(0,0,0);
			GetFROTATION(Str,&TempRotation,256);
			Brush->Rotation.Pitch += TempRotation.Pitch;
			Brush->Rotation.Yaw	+= TempRotation.Yaw;
			Brush->Rotation.Roll += TempRotation.Roll;
			Brush->Unlock(LOCK_Trans);
			Brush->BuildBound(1);
			GTrans->End();
			GCameraManager->RedrawLevel(Level);
			Processed = 1;
		}
		else if( GetCMD(&Str,"MOVETO") )
		{
			GTrans->Begin(Level,"Brush MoveTo");
			Brush->Lock(LOCK_Trans);
			GetFVECTOR(Str,&Brush->Location);
			Brush->Unlock(LOCK_Trans);
			Brush->BuildBound(1);
			GTrans->End();
			GCameraManager->RedrawLevel(Level);
			Processed = 1;
		}
		else if( GetCMD(&Str,"MOVEREL") )
		{
			GTrans->Begin(Level,"Brush MoveRel");
			Brush->Lock(LOCK_Trans);
			TempVector = FVector(0,0,0);
			GetFVECTOR(Str,&TempVector);
			Brush->Location.AddBounded(TempVector);
			Brush->Unlock(LOCK_Trans);
			Brush->BuildBound(1);
			GTrans->End();
			GCameraManager->RedrawLevel(Level);
			Processed = 1;
		}
		else if (GetCMD(&Str,"ADD"))
		{
			GTrans->Begin(Level,"Brush Add");
			constraintFinishAllSnaps(Level);
			DWord1=0; GetINT(Str,"FLAGS=",&DWord1);
			CsgType = CSG_Add;
			UModel *TempModel = csgAddOperation (Brush,Level,DWord1,CsgType,0);
			if( TempModel && !MapEdit ) bspBrushCSG (TempModel,Level->Model,DWord1,CsgType,1);
			GTrans->End();
			GCameraManager->RedrawLevel(Level);
			GApp->EdCallback(EDC_MapChange,0);
			Processed = 1;
		}
		else if (GetCMD(&Str,"ADDMOVER")) // BRUSH ADDMOVER
		{
			GTrans->Begin           	(Level,"Brush AddMover");
			GTrans->NoteResHeader		(Level);
			constraintFinishAllSnaps	(Level);
			Level->Lock					(LOCK_Trans);

			UClass *MoverClass	= new("Mover",FIND_Existing)UClass;
			AActor *Actor		= Level->SpawnActor(MoverClass,NULL,NAME_None,Brush->Location);
			if( Actor )
			{
				Actor->Rotation = Brush->Rotation;
				Actor->Brush	= csgDuplicateBrush( Level, Brush, 0, 0, 0, 1 );

				Actor->Process( NAME_PreEditChange, NULL );
				Actor->Process( NAME_PostEditChange, NULL );
			}
			Level->Unlock				(LOCK_Trans);
			GTrans->End					();
			GCameraManager->RedrawLevel	(Level);
			Processed = 1;
		}
		else if (GetCMD(&Str,"SUBTRACT")) // BRUSH SUBTRACT
			{
			GTrans->Begin				(Level,"Brush Subtract");
			constraintFinishAllSnaps(Level);
			//
			UModel *TempModel = csgAddOperation(Brush,Level,0,CSG_Subtract,0); // Layer
			if (TempModel && !MapEdit)
				{
				bspBrushCSG (TempModel,Level->Model,0,CSG_Subtract,1);
				};
			GTrans->End();
			GCameraManager->RedrawLevel(Level);
			GApp->EdCallback(EDC_MapChange,0);
			Processed = 1;
			}
		else if (GetCMD(&Str,"FROM")) // BRUSH FROM ACTOR/INTERSECTION/DEINTERSECTION
			{
			if (GetCMD(&Str,"INTERSECTION"))
				{
				Out->Log		("Brush from intersection");
				//
				GTrans->Begin	(Level,"Brush From Intersection");
				Brush->Lock		(LOCK_Trans);
				Brush->Polys->ModifyAllItems();
				Brush->Unlock	(LOCK_Trans);
				//
				constraintFinishAllSnaps (Level);
				//
				if (!MapEdit) bspBrushCSG (Brush,Level->Model,0,CSG_Intersect,0);
				//
				GTrans->End			();
				GCameraManager->RedrawLevel(Level);
				Processed = 1;
				}
			else if (GetCMD(&Str,"DEINTERSECTION"))
				{
				Out->Log		("Brush from deintersection");
				//
				GTrans->Begin	(Level,"Brush From Deintersection");
				Brush->Lock		(LOCK_Trans);
				Brush->Polys->ModifyAllItems();
				Brush->Unlock	(LOCK_Trans);
				//
				constraintFinishAllSnaps (Level);
				//
				if (!MapEdit) bspBrushCSG (Brush,Level->Model,0,CSG_Deintersect,0);
				GTrans->End				();
				GCameraManager->RedrawLevel(Level);
				Processed = 1;
				};
			}
		else if (GetCMD (&Str,"NEW"))
			{
			GTrans->Begin		(Level,"Brush New");
			Brush->Lock			(LOCK_Trans);
			Brush->Polys->ModifyAllItems();
			//
			Brush->Polys->Num=0;
			//
			Brush->Unlock		(LOCK_Trans);
			GTrans->End			();
			GCameraManager->RedrawLevel(Level);
			//
			Processed = 1;
			}
		else if (GetCMD (&Str,"LOAD"))
			{
			if (GetSTRING(Str,"FILE=",TempFname,79))
				{
				GTrans->Begin			(Level,"Brush Load");
				//
				Brush->Lock				(LOCK_Trans);
				TempVector   = Brush->Location;
				TempRotation = Brush->Rotation;
				Brush->Polys->ModifyAllItems();
				Brush->Unlock			(LOCK_Trans);
				//
				GObj.AddFile 			(TempFname,NULL);
				//
				Brush->Lock				(LOCK_Trans);
				Brush->Location = TempVector;
				Brush->Rotation = TempRotation;
				Brush->Unlock			(LOCK_Trans);
				//
				bspValidateBrush		(Brush,0,1);
				GTrans->End				();
				Cleanse(*Out,1);
				Processed = 1;
				};
			}
		else if (GetCMD (&Str,"SAVE"))
			{
			if (GetSTRING(Str,"FILE=",TempFname,79))
				{
				Out->Logf("Saving %s",TempFname);
				GObj.SaveDependent( Brush, TempFname );
				}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed = 1;
			}
		else if (GetCMD (&Str,"IMPORT"))
			{
			if (GetSTRING(Str,"FILE=",TempFname,79))
				{
				GApp->BeginSlowTask	("Importing brush",1,0);
				GTrans->Begin		(Level,"Brush Import");
				//
				if (!GetSTRING(Str,"NAME=", TempName,NAME_SIZE)) strcpy(TempName,"Brush");
				if (!GetONOFF (Str, "MERGE=",&DWord2)) DWord2=1;
				if (!GetINT   (Str, "FLAGS=",&DWord1)) DWord1=0;
				//
				UModel *TempModel = new(TempName,FIND_Optional)UModel;
				if (!TempModel)
					{
					TempModel = new(TempName,CREATE_Unique,RF_NotForClient|RF_NotForServer)UModel;
					TempModel->ModelFlags &= ~MF_Linked;
					TempModel->Polys = new(TempName,TempFname,IMPORT_Replace)UPolys;
					if (!TempModel->Polys)
						{
						Out->Log(LOG_ExecError,"Brush import failed");
						TempModel->Kill();
						goto ImportBrushError;
						};
					}
				else
					{
					TempModel->ModelFlags &= ~MF_Linked;
					TempModel->Lock		(LOCK_Trans);
					Brush->Polys->ModifyAllItems();
					TempModel->Unlock	(LOCK_Trans);
					//
					if (!TempModel->Polys->ImportFromFile(TempFname))
						{
						GTrans->Rollback(Level);
						goto ImportBrushError;
						};
					};
				if (DWord1) // Set flags for all imported EdPolys
					{
					for (Word2=0; Word2<TempModel->Polys->Num; Word2++)
						{
						TempModel->Polys->Element(Word2).PolyFlags |= DWord1;
						};
					};
				if( DWord2 ) bspMergeCoplanars (TempModel,0,1);
				bspValidateBrush( TempModel, 0, 1 );
				GTrans->End		 	();
				//
				ImportBrushError:
				//
				Cleanse(*Out,1);
				GApp->EndSlowTask();
				}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
			}
		else if (GetCMD (&Str,"EXPORT"))
			{
			if (GetSTRING(Str,"FILE=",TempFname,79))
				{
				GApp->BeginSlowTask	("Exporting brush",1,0);
				Brush->Polys->ExportToFile(TempFname); // Only exports polys
				GApp->EndSlowTask();
				}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
			};
		}

	//----------------------------------------------------------------------------------
	// PATHS
	//
	else if (GetCMD(&Str,"PATHS"))
	{
		if (GetCMD(&Str,"BUILD"))
		{
			int opt = 1; //assume medium
			if (GetCMD(&Str,"LOWOPT"))
				opt = 0;
			else if (GetCMD(&Str,"HIGHOPT"))
				opt = 2;

			FPathBuilder builder;

			GTrans->Begin			(Level,"Remove Paths");
			Level->Lock				(LOCK_Trans);
			int numpaths = builder.removePaths		(Level);
			Level->Unlock			(LOCK_Trans);
			GTrans->End				();

			GTrans->Begin			(Level,"Build Paths");
			Level->Lock				(LOCK_Trans);
			numpaths = builder.buildPaths		(Level, opt);
			Level->Unlock			(LOCK_Trans);
			GTrans->End				();
			GCameraManager->RedrawLevel		(Level);
			Out->Logf("Built Paths: %d", numpaths);
			Processed=1;
		}
		else if (GetCMD(&Str,"SHOW"))
		{
			FPathBuilder builder;
			GTrans->Begin			(Level,"Show Paths");
			Level->Lock				(LOCK_Trans);
			int numpaths = builder.showPaths(Level);
			Level->Unlock			(LOCK_Trans);
			GTrans->End				();
			GCameraManager->RedrawLevel		(Level);
			Out->Logf(" %d Paths are visible!", numpaths);
			Processed=1;
		}
		else if (GetCMD(&Str,"HIDE"))
		{
			FPathBuilder builder;
			GTrans->Begin			(Level,"Hide Paths");
			Level->Lock				(LOCK_Trans);
			int numpaths = builder.hidePaths(Level);
			Level->Unlock			(LOCK_Trans);
			GTrans->End				();
			GCameraManager->RedrawLevel		(Level);
			Out->Logf(" %d Paths are hidden!", numpaths);
			Processed=1;
		}
		else if (GetCMD(&Str,"REMOVE"))
		{
			FPathBuilder builder;
			GTrans->Begin			(Level,"Remove Paths");
			Level->Lock				(LOCK_Trans);
			int numpaths = builder.removePaths		(Level);
			Level->Unlock			(LOCK_Trans);
			GTrans->End				();
			GCameraManager->RedrawLevel		(Level);
			Out->Logf("Removed %d Paths", numpaths);
			Processed=1;
		}
		else if (GetCMD(&Str,"DEFINE"))
		{
			FPathBuilder builder;
			GTrans->Begin			(Level,"UnDefine old Paths");
			Level->Lock				(LOCK_Trans);
			builder.undefinePaths	(Level);
			Level->Unlock			(LOCK_Trans);
			GTrans->End				();

			GTrans->Begin			(Level,"Define Paths");
			Level->Lock				(LOCK_Trans);
			builder.definePaths		(Level);
			Level->Unlock			(LOCK_Trans);
			GTrans->End				();

			GCameraManager->RedrawLevel		(Level);
			Processed=1;
		}
	}

	//------------------------------------------------------------------------------------
	// Bsp
	//
	else if( GetCMD( &Str, "BSP" ) )
	{
		if( GetCMD( &Str, "REBUILD") ) // Bsp REBUILD [LAME/GOOD/OPTIMAL] [BALANCE=0-100] [LIGHTS] [MAPS] [REJECT]
		{
			GTrans->Reset("rebuilding Bsp"); // Not tracked transactionally
			Out->Log("Bsp Rebuild");
			EBspOptimization BspOpt;

			if      (GetCMD(&Str,"LAME")) 		BspOpt=BSP_Lame;
			else if (GetCMD(&Str,"GOOD"))		BspOpt=BSP_Good;
			else if (GetCMD(&Str,"OPTIMAL"))	BspOpt=BSP_Optimal;
			else								BspOpt=BSP_Good;

			if( !GetWORD( Str, "BALANCE=", &Word2 ) )
				Word2=50;

			GApp->BeginSlowTask( "Rebuilding Bsp", 1, 0 );

			GApp->StatusUpdate( "Building polygons", 0, 0 );
			bspBuildFPolys( Level->Model, 1 );

			GApp->StatusUpdate( "Merging planars", 0, 0 );
			bspMergeCoplanars( Level->Model, 0, 0 );

			GApp->StatusUpdate( "Partitioning", 0, 0 );
			bspBuild( Level->Model, BspOpt, Word2, 0 );

			if( GetSTRING( Str, "ZONES", TempStr, 1 ) )
			{
				GApp->StatusUpdate( "Building visibility zones", 0, 0 );
				TestVisibility( Level, Level->Model, 0, 0 );
			}
			if( GetSTRING( Str, "OPTGEOM", TempStr, 1 ) )
			{
				GApp->StatusUpdate( "Optimizing geometry", 0, 0 );
				bspOptGeom( Level->Model );
			}

			// Empty EdPolys.
			Level->Lock( LOCK_ReadWrite );
			Level->Model->Polys->Num = 0;
			Level->Unlock( LOCK_ReadWrite );

			GApp->EndSlowTask();
			GCameraManager->RedrawLevel( Level );
			GApp->EdCallback( EDC_MapChange, 0 );

			Processed=1;
		}
	}
	//------------------------------------------------------------------------------------
	// LIGHT
	//
	else if (GetCMD(&Str,"LIGHT"))
		{
		if (GetCMD(&Str,"APPLY")) // LIGHT APPLY [MESH=..] [SELECTED=..] [SMOOTH=..] [RADIOSITY=..]
			{
			DWord1 = 0; GetONOFF (Str,"SELECTED=",  &DWord1); // Light selected lights only
			//
			shadowIlluminateBsp (Level,DWord1);
			GCameraManager->RedrawLevel (Level);
			//
			Processed=1;
			};
		}
	//------------------------------------------------------------------------------------
	// MAP
	//
	else if (GetCMD(&Str,"MAP"))
		{
		//
		// Parameters:
		//
		if (GetONOFF (Str,"EDIT=", &MapEdit))
			{
			constraintFinishAllSnaps (Level);
			if (MapEdit)
				{
				GTrans->Reset ("map editing"); // Can't be transaction-tracked
				csgInvalidateBsp (Level);
				};
			GCameraManager->RedrawLevel(Level);
			//
			Processed=1;
			};
		//
		// Commands:
		//
		if (GetCMD(&Str,"GRID")) // MAP GRID [SHOW3D=ON/OFF] [SHOW2D=ON/OFF] [X=..] [Y=..] [Z=..]
			{
			//
			// Before changing grid, force editor to current grid position to avoid jerking:
			//
			constraintFinishAllSnaps (Level);
			//
			Word1  = GetONOFF   (Str,"SHOW2D=",&Show2DGrid);
			Word1 |= GetONOFF   (Str,"SHOW3D=",&Show3DGrid);
			Word1 |= GetFVECTOR (Str,&Constraints.Grid);
			//
			if (Word1) GCameraManager->RedrawLevel(Level);
			Processed=1;
			}
		else if (GetCMD(&Str,"ROTGRID")) // MAP ROTGRID [PITCH=..] [YAW=..] [ROLL=..]
			{
			constraintFinishAllSnaps (Level);
			if (GetFROTATION (Str,&Constraints.RotGrid,256)) GCameraManager->RedrawLevel(Level);
			Processed=1;
			}
		else if (GetCMD(&Str,"SELECT")) // MAP SELECT ALL/NONE/INVERSE/NAMEstr
			{
			GTrans->Begin (Level,"Select");
			//
			if 		(GetCMD(&Str,"ALL"))		mapSelectAll		(Level);
			else if (GetCMD(&Str,"NONE"))		mapSelectNone		(Level);
			else if (GetCMD(&Str,"ADDS"))		mapSelectOperation	(Level,CSG_Add);
			else if (GetCMD(&Str,"SUBTRACTS"))	mapSelectOperation	(Level,CSG_Subtract);
			else if (GetCMD(&Str,"SEMISOLIDS"))	mapSelectFlags		(Level,PF_Semisolid);
			else if (GetCMD(&Str,"NONSOLIDS"))	mapSelectFlags		(Level,PF_NotSolid);
			else if (GetCMD(&Str,"PREVIOUS"))	mapSelectPrevious	(Level);
			else if (GetCMD(&Str,"NEXT"))		mapSelectNext		(Level);
			else if (GetCMD(&Str,"FIRST"))		mapSelectFirst		(Level);
			else if (GetCMD(&Str,"LAST"))		mapSelectLast		(Level);
			//
			GTrans->End 			();
			GCameraManager->RedrawLevel	(Level);
			Processed=1;
			}
		else if (GetCMD(&Str,"DELETE")) // MAP DELETE
			{
			GTrans->Begin		(Level,"Map Delete");
			mapDelete 			(Level);
			GTrans->End			();
			GCameraManager->RedrawLevel	(Level);
			Processed=1;
			}
		else if (GetCMD(&Str,"DUPLICATE")) // MAP DUPLICATE
			{
			GTrans->Begin		(Level,"Map Duplicate");
			mapDuplicate		(Level);
			GTrans->End			();
			GCameraManager->RedrawLevel	(Level);
			Processed=1;
			}
		else if (GetCMD(&Str,"BRUSH")) // MAP BRUSH GET/PUT
			{
			if (GetCMD (&Str,"GET"))
				{
				GTrans->Begin		(Level,"Brush Get");
				mapBrushGet			(Level);
				GTrans->End			();
				GCameraManager->RedrawLevel	(Level);
				Processed=1;
				}
			else if (GetCMD (&Str,"PUT"))
				{
				GTrans->Begin		(Level,"Brush Put");
				mapBrushPut			(Level);
				GTrans->End			();
				GCameraManager->RedrawLevel	(Level);
				Processed=1;
				};
			}
		else if (GetCMD(&Str,"SENDTO")) // MAP SENDTO FRONT/BACK
			{
			if (GetCMD(&Str,"FIRST"))
				{
				GTrans->Begin		(Level,"Map SendTo Front");
				mapSendToFirst		(Level);
				GTrans->End			();
				GCameraManager->RedrawLevel	(Level);
				Processed=1;
				}
			else if (GetCMD(&Str,"LAST"))
				{
				GTrans->Begin		(Level,"Map SendTo Back");
				mapSendToLast		(Level);
				GTrans->End			();
				GCameraManager->RedrawLevel	(Level);
				Processed=1;
				};
			}
		else if (GetCMD(&Str,"REBUILD")) // MAP REBUILD
			{
			GTrans->Reset		("rebuilding map"); 	// Can't be transaction-tracked
			csgRebuild			(Level);				// Revalidates the Bsp
			GCameraManager->RedrawLevel	(Level);
			GApp->EdCallback	(EDC_MapChange,0);
			Processed=1;
			}
		else if (GetCMD (&Str,"NEW")) // MAP NEW
			{
			GTrans->Reset			("clearing map");
			Level->RememberActors	();
			Level->EmptyLevel		();
			Level->SetState 		(LEVEL_UpEdit);
			Level->ReconcileActors	();
			//
			GApp->EdCallback		(EDC_MapChange,0);
			GApp->EdCallback		(EDC_SelPolyChange,0);
			GApp->EdCallback		(EDC_SelActorChange,0);
			//
			Cleanse(*Out,1);
			//
			Processed=1;
			}
		else if (GetCMD (&Str,"LOAD")) // MAP LOAD
			{
			if (GetSTRING(Str,"FILE=",TempFname,79))
				{
				GTrans->Reset			("loading map"); // Can't be transaction tracked
				//
				GApp->BeginSlowTask		("Loading map",1,0);
				Level->RememberActors	();
				GObj.AddFile 			(TempFname,NULL);
				Level->SetState			(LEVEL_UpEdit);
				Level->ReconcileActors	();
				bspValidateBrush		(Brush,0,1);
				GApp->EndSlowTask   	();
				//
				GApp->EdCallback		(EDC_MapChange,0);
				GApp->EdCallback		(EDC_SelPolyChange,0);
				GApp->EdCallback		(EDC_SelActorChange,0);
				//
				Cleanse(*Out,0);
				}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
			}
		else if (GetCMD (&Str,"SAVE")) // MAP SAVE
			{
			if (GetSTRING(Str,"FILE=",TempFname,79))
				{
				Level->ShrinkLevel	();
				GApp->BeginSlowTask	("Saving map",1,0);
				GObj.SaveDependent	( Level, TempFname );
				GApp->EndSlowTask	();
				}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
			}
		else if( GetCMD( &Str, "IMPORT" ) )
		{
			Word1=1;
			DoImportMap:
			if( GetSTRING( Str, "FILE=", TempFname, 79 ) )
			{
				GTrans->Reset( "importing map" );
				GApp->BeginSlowTask( "Importing map", 1, 0 );
				Level->RememberActors();
				if( Word1 )
					Level->EmptyLevel();
				Level->ImportFromFile( TempFname );
				GCache.Flush();
				csgInvalidateBsp( Level );
				Level->SetState( LEVEL_UpEdit );
				Level->ReconcileActors();
				if (Word1)
				{
					mapSelectNone( Level );
					edactSelectNone( Level );
				}
				GApp->EndSlowTask();
				GApp->EdCallback( EDC_MapChange, 0 );
				GApp->EdCallback( EDC_SelPolyChange, 0 );
				GApp->EdCallback( EDC_SelActorChange, 0 );
				Cleanse( *Out, 1 );
			}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
		}
		else if( GetCMD( &Str, "IMPORTADD" ) )
		{
			Word1=0;
			mapSelectNone( Level );
			edactSelectNone( Level );
			goto DoImportMap;
		}
		else if (GetCMD (&Str,"EXPORT"))
			{
			if (GetSTRING(Str,"FILE=",TempFname,79))
				{
				GApp->BeginSlowTask	("Exporting map",1,0);
				GObj.UntagAll();
				Level->ExportToFile(TempFname);
				GApp->EndSlowTask();
				}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
			}
		else if (GetCMD (&Str,"SETBRUSH")) // MAP SETBRUSH (set properties of all selected brushes)
			{
			GTrans->Begin		(Level,"Set Brush Properties");
			//
			Word1  = 0;  // Properties mask
			DWord1 = 0;  // Set flags
			DWord2 = 0;  // Clear flags
			//
			FName GroupName=NAME_None;
			if (GetWORD (Str,"COLOR=",&Word2))			Word1 |= MSB_BrushColor;
			if (GetNAME (Str,"GROUP=",&GroupName))		Word1 |= MSB_Group;
			if (GetINT  (Str,"SETFLAGS=",&DWord1))		Word1 |= MSB_PolyFlags;
			if (GetINT  (Str,"CLEARFLAGS=",&DWord2))	Word1 |= MSB_PolyFlags;
			//
			mapSetBrush(Level,(EMapSetBrushFlags)Word1,Word2,GroupName,DWord1,DWord2);
			//
			GTrans->End			();
			GCameraManager->RedrawLevel	(Level);
			//
			Processed=1;
			}
		else if (GetCMD (&Str,"SAVEPOLYS"))
			{
			if (GetSTRING(Str,"FILE=",TempFname,79))
				{
				if (!GetONOFF (Str, "MERGE=",&DWord2)) DWord2=1;
				//
				GApp->BeginSlowTask	("Exporting map polys",1,0);
				GApp->StatusUpdate  ("Building polygons",0,0);
				bspBuildFPolys		(Level->Model,0);
				//
				if (DWord2)
					{
					GApp->StatusUpdate  ("Merging planars",0,0);
					bspMergeCoplanars	(Level->Model,0,1);
					};
				Level->Lock(LOCK_ReadWrite); 
				Level->Model->Polys->ExportToFile(TempFname);
				Level->Model->Polys->Num = 0; // Empty edpolys
				Level->Unlock	(LOCK_ReadWrite);
				//
				GApp->EndSlowTask 	();
				GCameraManager->RedrawLevel	(Level);
				}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
			};
		}
	//------------------------------------------------------------------------------------
	// SELECT: Rerouted to mode-specific command
	//
	else if (GetCMD(&Str,"SELECT"))
		{
		strcpy (TempStr,"SELECT");
		//
		ModeSpecificReroute:
		//
		switch( ModeClass )
		{
			case EMC_Camera:
			case EMC_Player:
			case EMC_Brush:
			case EMC_Texture:
				if( MapEdit )
				{
					sprintf (TempStr1,"MAP %s %s ",TempStr,Str);
					Exec(TempStr1,Out);
				}
				else
				{
					sprintf (TempStr1,"POLY %s %s ",TempStr,Str);
					Exec(TempStr1,Out);
				}
				break;
			case EMC_Actor:
				sprintf (TempStr1,"ACTOR %s %s ",TempStr,Str);
				Exec(TempStr1,Out);
				break;
		}
		Processed=1;
	}
	//------------------------------------------------------------------------------------
	// DELETE: Rerouted to mode-specific command
	//
	else if (GetCMD(&Str,"DELETE"))
		{
		strcpy (TempStr,"DELETE");
		goto ModeSpecificReroute;
		}
	//------------------------------------------------------------------------------------
	// DUPLICATE: Rerouted to mode-specific command
	//
	else if (GetCMD(&Str,"DUPLICATE"))
		{
		strcpy (TempStr,"DUPLICATE");
		goto ModeSpecificReroute;
		}
	//------------------------------------------------------------------------------------
	// ACTOR: Actor-related functions
	//
	else if (GetCMD(&Str,"ACTOR"))
		{
		if (GetCMD(&Str,"SELECT")) // ACTOR SELECT
			{
			if (GetCMD(&Str,"NONE")) // ACTOR SELECT NONE
				{
				GTrans->Begin			(Level,"Select None");
				//
				Level->Lock				(LOCK_Trans);
				edactSelectNone 		(Level);
				Level->Unlock			(LOCK_Trans);
				//
				GTrans->End				();
				GCameraManager->RedrawLevel		(Level);
				//
				GApp->EdCallback(EDC_SelActorChange,0);
				Processed=1;
				}
			else if (GetCMD(&Str,"ALL")) // ACTOR SELECT ALL
				{
				GTrans->Begin		(Level,"Select All");
				//
				Level->Lock			(LOCK_Trans);
				edactSelectAll 		(Level);
				Level->Unlock		(LOCK_Trans);
				//
				GTrans->End			();
				GCameraManager->RedrawLevel	(Level);
				//
				GApp->EdCallback(EDC_SelActorChange,0);
				Processed=1;
				}
			else if (GetCMD(&Str,"OFCLASS")) // ACTOR SELECT OFCLASS CLASS=..
				{
				UClass *Class;
				if (GetUClass(Str,"CLASS=",Class))
					{
					GTrans->Begin		(Level,"Select of class");
					//
					Level->Lock			(LOCK_Trans);
					edactSelectOfClass	(Level,Class);
					Level->Unlock		(LOCK_Trans);
					//
					GTrans->End			();
					GCameraManager->RedrawLevel	(Level);
					//
					GApp->EdCallback(EDC_SelActorChange,0);
					}
				else Out->Log(LOG_ExecError,"Missing class");
				Processed=1;
				};
			}
		else if (GetCMD(&Str,"SET")) // ACTOR SET [ADDCLASS=class]
			{
			GetUClass(Str,"ADDCLASS=",CurrentClass);
			Processed=1;
			}
		else if (GetCMD(&Str,"DELETE")) // ACTOR DELETE (selected)
		{
			GTrans->Begin			(Level,"Delete Actors");
			//
			Level->Lock				(LOCK_Trans);
			edactDeleteSelected		(Level);
			Level->Unlock			(LOCK_Trans);
			//
			GTrans->End				();
			GCameraManager->RedrawLevel		(Level);
			//
			GApp->EdCallback(EDC_SelActorChange,0);
			Processed=1;
		}
		else if (GetCMD(&Str,"DUPLICATE")) // ACTOR DUPLICATE (selected)
		{
			GTrans->Begin				(Level,"Duplicate Actors");

			Level->Lock					(LOCK_Trans);
			edactDuplicateSelected		(Level);
			Level->Unlock				(LOCK_Trans);

			GTrans->End					();
			GCameraManager->RedrawLevel			(Level);

			GApp->EdCallback(EDC_SelActorChange,0);
			Processed=1;
		}
		else if( GetCMD(&Str,"SUBCLASS") ) // ACTOR SUBCLASS (selected)
		{
			UClass *NewClass;
			debugf("Actor subclass");
			if( GetUClass(Str,"NEWCLASS=",NewClass) )
			{
				GTrans->Begin( Level, "subclassing actors" );
				Level->Lock( LOCK_Trans );
				for( int i=0; i<Level->Num; i++ )
				{
					AActor *Actor = Level->Element(i);
					if( Actor && Actor->bSelected )
					{
						checkState(Actor->GetClass()->ParentClass!=NULL);
						if( NewClass->IsChildOf(Actor->GetClass()) )
						{
							// Note change.
							debugf("Subclassing...");
							Actor->Lock(LOCK_Trans);

							// Export properties.
							BYTE WhichBins[PROPBIN_MAX]; memset(WhichBins,0,sizeof(WhichBins)); WhichBins[PROPBIN_PerObject] = 1;
							UTextBuffer *ActorPropertiesBuffer = new("Properties",CREATE_Replace)UTextBuffer(1);
							BYTE *ObjectBins[PROPBIN_MAX]; Actor->GetObjectBins(ObjectBins);
							ExportActor
							(
								Actor->GetClass(),
								ObjectBins,
								*ActorPropertiesBuffer,
								NAME_None,
								0,
								0,
								CPF_Edit,
								Actor->GetClass()->ParentClass,
								0,
								-1,
								NAME_None,
								WhichBins
							);

							// Copy info from parent class.
							UBuffer *NewBin = NewClass->Bins[PROPBIN_PerObject];
							memcpy
							(
								(BYTE*)Actor               + sizeof(UObject),
								(BYTE*)&NewBin->Element(0) + sizeof(UObject),
								NewBin->Num                - sizeof(UObject)
							);
							Actor->SetClass( NewClass );

							// Reimport.
							ActorPropertiesBuffer->Lock(LOCK_Read);
							ImportActorProperties
							(
								Level,
								Actor->GetClass(),
								ObjectBins,
								(char*)ActorPropertiesBuffer->GetData(),
								WhichBins,
								0
							);
							ActorPropertiesBuffer->Unlock(LOCK_Read);
							ActorPropertiesBuffer->Kill();
							Actor->Unlock(LOCK_Trans);

							checkState(Actor->GetClass()==NewClass);
						}
						else debugf( "%s is not a subclass of %s", NewClass->GetName(), Actor->GetClassName() );
					}
				}
				Level->Unlock( LOCK_Trans );
				GTrans->End();
				GCameraManager->RedrawLevel( Level );
			}
			Processed=1;
		}
	}
	//------------------------------------------------------------------------------------
	// POLY: Polygon adjustment and mapping
	//
	else if (GetCMD(&Str,"POLY"))
		{
		if (GetCMD(&Str,"SELECT")) // POLY SELECT [ALL/NONE/INVERSE] FROM [LEVEL/SOLID/GROUP/ITEM/ADJACENT/MATCHING]
			{
			sprintf    (TempStr,"POLY SELECT %s",Str);
			GTrans->Begin (Level,TempStr);
			Level->Lock  (LOCK_Trans);
			//
			if (GetCMD(&Str,"ALL"))
				{
				polySelectAll (Level->Model);
				GApp->EdCallback(EDC_SelPolyChange,0);
				Processed=1;
				}
			else if (GetCMD(&Str,"NONE"))
				{
				polySelectNone (Level->Model);
				GApp->EdCallback(EDC_SelPolyChange,0);
				Processed=1;
				}
			else if (GetCMD(&Str,"REVERSE"))
				{
				polySelectReverse (Level->Model);
				GApp->EdCallback(EDC_SelPolyChange,0);
				Processed=1;
				}
			else if (GetCMD(&Str,"MATCHING"))
				{
				if 		(GetCMD(&Str,"GROUPS"))		polySelectMatchingGroups 	(Level->Model);
				else if (GetCMD(&Str,"ITEMS"))		polySelectMatchingItems 	(Level->Model);
				else if (GetCMD(&Str,"BRUSH"))		polySelectMatchingBrush 	(Level->Model);
				else if (GetCMD(&Str,"TEXTURE"))	polySelectMatchingTexture 	(Level->Model);
				GApp->EdCallback(EDC_SelPolyChange,0);
				Processed=1;
				}
			else if (GetCMD(&Str,"ADJACENT"))
				{
				if 	  (GetCMD(&Str,"ALL"))			polySelectAdjacents 		(Level->Model);
				else if (GetCMD(&Str,"COPLANARS"))	polySelectCoplanars 		(Level->Model);
				else if (GetCMD(&Str,"WALLS"))		polySelectAdjacentWalls 	(Level->Model);
				else if (GetCMD(&Str,"FLOORS"))		polySelectAdjacentFloors 	(Level->Model);
				else if (GetCMD(&Str,"CEILINGS"))	polySelectAdjacentFloors 	(Level->Model);
				else if (GetCMD(&Str,"SLANTS"))		polySelectAdjacentSlants 	(Level->Model);
				GApp->EdCallback(EDC_SelPolyChange,0);
				Processed=1;
				}
			else if (GetCMD(&Str,"MEMORY"))
				{
				if 		(GetCMD(&Str,"SET"))		polyMemorizeSet 			(Level->Model);
				else if (GetCMD(&Str,"RECALL"))		polyRememberSet 			(Level->Model);
				else if (GetCMD(&Str,"UNION"))		polyUnionSet 				(Level->Model);
				else if (GetCMD(&Str,"INTERSECT"))	polyIntersectSet 			(Level->Model);
				else if (GetCMD(&Str,"XOR"))		polyXorSet 					(Level->Model);
				GApp->EdCallback(EDC_SelPolyChange,0);
				Processed=1;
				};
			Level->Unlock	(LOCK_Trans);
			GTrans->End			();
			GCameraManager->RedrawLevel	(Level);
			}
		else if (GetCMD(&Str,"DEFAULT")) // POLY DEFAULT <variable>=<value>...
			{
			if (!GetUTexture(Str,"TEXTURE=",CurrentTexture))
				{
				Out->Log(LOG_ExecError,"Missing texture");
				};
			Processed=1;
			}
		else if (GetCMD(&Str,"SET")) // POLY SET <variable>=<value>...
			{
			//
			// Options: TEXTURE=name SETFLAGS=value CLEARFLAGS=value
			//          UPAN=value VPAN=value ROTATION=value XSCALE=value YSCALE=value
			//
			GTrans->Begin				(Level,"Poly Set");
			Level->Lock			(LOCK_Trans);
			Level->Model->Surfs->ModifySelected(1);
			//
			UTexture *Texture;
			if (GetUTexture(Str,"TEXTURE=",Texture))
				{
				for (Index1=0; Index1<Level->Model->Surfs->Num; Index1++)
					{
					if (Level->Model->Surfs(Index1).PolyFlags & PF_Selected)
						{
						Level->Model->Surfs(Index1).Texture  = Texture;
						polyUpdateMaster( Level->Model, Index1, 0, 0 );
						};
					};
				};
			Word4  = 0;
			DWord1 = 0;
			DWord2 = 0;
			if (GetINT(Str,"SETFLAGS=",&DWord1))   Word4=1;
			if (GetINT(Str,"CLEARFLAGS=",&DWord2)) Word4=1;
			if (Word4)  polySetAndClearPolyFlags (Level->Model,DWord1,DWord2,1,1); // Update selected polys' flags
			//
			Level->Unlock(LOCK_Trans);
			GTrans->End();
			GCameraManager->RedrawLevel(Level);			
			Processed=1;
			}
		else if (GetCMD(&Str,"TEXSCALE")) // POLY TEXSCALE [U=..] [V=..] [UV=..] [VU=..]
			{
			GTrans->Begin 				(Level,"Poly Texscale");
			Level->Lock			(LOCK_Trans);
			Level->Model->Surfs->ModifySelected(1);
			//
			Word2 = 1; // Scale absolute
			//
			TexScale:
			//
			FLOAT UU,UV,VU,VV;
			UU=1.0; GetFLOAT (Str,"UU=",&UU);
			UV=0.0; GetFLOAT (Str,"UV=",&UV);
			VU=0.0; GetFLOAT (Str,"VU=",&VU);
			VV=1.0; GetFLOAT (Str,"VV=",&VV);
			//
			polyTexScale( Level->Model, UU, UV, VU, VV, Word2 );
			//
			Level->Unlock	(LOCK_Trans);
			GTrans->End			();
			GCameraManager->RedrawLevel	(Level);
			Processed=1;
			}
		else if (GetCMD(&Str,"TEXMULT")) // POLY TEXMULT [U=..] [V=..]
			{
			GTrans->Begin 				(Level,"Poly Texmult");
			Level->Lock			(LOCK_Trans);
			Level->Model->Surfs->ModifySelected(1);
			//
			Word2 = 0; // Scale relative;
			//
			goto TexScale;
			}
		else if (GetCMD(&Str,"TEXPAN")) // POLY TEXPAN [RESET] [U=..] [V=..]
			{
			GTrans->Begin 				(Level,"Poly Texpan");
			Level->Lock			(LOCK_Trans);
			Level->Model->Surfs->ModifySelected(1);
			//
			if (GetCMD (&Str,"RESET")) polyTexPan  (Level->Model,0,0,1);
			//
			Word1 = 0; GetWORD (Str,"U=",&Word1);
			Word2 = 0; GetWORD (Str,"V=",&Word2);
			polyTexPan (Level->Model,Word1,Word2,0);
			//
			Level->Unlock	(LOCK_Trans);
			GTrans->End			();
			GCameraManager->RedrawLevel	(Level);
			Processed=1;
			}
		else if (GetCMD(&Str,"TEXALIGN")) // POLY TEXALIGN [FLOOR/GRADE/WALL/NONE]
			{
			if		(GetCMD (&Str,"DEFAULT"))	TexAlign = TEXALIGN_Default;
			else if (GetCMD (&Str,"FLOOR"))		TexAlign = TEXALIGN_Floor;
			else if (GetCMD (&Str,"WALLDIR"))	TexAlign = TEXALIGN_WallDir;
			else if (GetCMD (&Str,"WALLPAN"))	TexAlign = TEXALIGN_WallPan;
			else if (GetCMD (&Str,"WALLCOLUMN"))TexAlign = TEXALIGN_WallColumn;
			else if (GetCMD (&Str,"ONETILE"))	TexAlign = TEXALIGN_OneTile;
			else								goto Skipt;
			//
			if (!GetINT(Str,"TEXELS=",&DWord1)) DWord1=0;
			//
			GTrans->Begin				(Level,"Poly Texalign");
			Level->Lock			(LOCK_Trans);
			Level->Model->Surfs->ModifySelected(1);
			//
			polyTexAlign			(Level->Model,TexAlign,DWord1);
			//
			Level->Unlock			(LOCK_Trans);
			GTrans->End					();
			GCameraManager->RedrawLevel	(Level);
			Processed=1;
			//
			Skipt:;
			};
		}
	//------------------------------------------------------------------------------------
	// PALETTE management:
	//
	else if (GetCMD(&Str,"PALETTE"))
		{
		if (GetCMD(&Str,"IMPORT")) // PALETTE IMPORT FILE=.. NAME=.. SMOOTH=on/off
			{
			if ((GetSTRING (Str, "FILE=",  TempFname, 128)) &&
				(GetSTRING (Str, "NAME=",  TempName,  NAME_SIZE)))
				{
				GApp->BeginSlowTask ("Importing palette",1,0);
				//
				DWord1=0; GetONOFF (Str,"SMOOTH=",&DWord1);
				UPalette *Palette = new(TempName,TempFname,IMPORT_Replace)UPalette;
				if (Palette)
					{
					if (DWord1) Palette->Smooth();
					GCache.Flush();
					GCameraManager->RedrawLevel(Level);
					};
				GApp->EndSlowTask();
				}
			else Out->Log(LOG_ExecError,"Missing file or name");
			Processed=1;
			};
		}
	//------------------------------------------------------------------------------------
	// TEXTURE management (not mapping):
	//
	else if( GetCMD(&Str,"Texture") )
	{
		if (GetCMD(&Str,"Import"))
		{
			char SetName[NAME_SIZE]="";
			if
			(
				(GetSTRING (Str, "File=",  TempFname, 128)) 
			&&	(GetSTRING (Str, "Name=",  TempName,  NAME_SIZE))
			)
			{
				GApp->BeginSlowTask( "Importing texture", 1, 0 );
				GetSTRING (Str,"Family=", SetName, NAME_SIZE);
				GetSTRING (Str,"Set=", SetName, NAME_SIZE);
				
				DWord1=1; GetONOFF(Str,"Mips=", &DWord1);
				DWord2=1; GetONOFF(Str,"Remap=",&DWord2);

				UTexture *Texture = new( TempName, TempFname, IMPORT_Replace )UTexture;
				if( Texture )
				{
					DWORD TexFlags=0; GetDWORD(Str,"TexFlags=",&TexFlags);
					GetDWORD(Str,"FLAGS=",&Texture->PolyFlags);
					GetUPalette(Str,"PALETTE=",*(UPalette**)&Texture->Palette);
					GetUTexture(Str,"BUMP=",Texture->BumpMap);
					GetUTexture(Str,"DETAIL=",Texture->DetailTexture);
					GetUTexture(Str,"MTEX=",Texture->MacroTexture);
					GetUTexture(Str,"NEXT=",Texture->AnimNext);

					if( !Texture->Palette && DWord2 )
					{
						// Import palette.
						Texture->Palette = new(TempName,TempFname,IMPORT_Replace)UPalette;
						if( Texture->Palette )
						{
							Texture->Palette->BuildPaletteRemapIndex(Texture->PolyFlags & PF_Masked);
							Texture->Palette = Texture->Palette->ReplaceWithExisting();
						}
					}
					else if( Texture->Palette )
					{
						// Remap to someone else's palette.
						UPalette *TempPalette = new("Temp",TempFname,IMPORT_Replace)UPalette;
						if( TempPalette )
						{
							Texture->Remap(TempPalette,Texture->Palette);
							TempPalette->Kill();
						}
					}
					else Texture->Fixup();

					Texture->CreateMips(DWord1);
					if( SetName[0] )
					{
						// Find existing set or create new one.
						UTextureSet *Set = new(SetName,FIND_Optional)UTextureSet;
						if( !Set ) Set = new(SetName,CREATE_Unique)UTextureSet;
						TextureSets->AddUniqueItem(Set);

						// Add texture to set.
						Set->AddUniqueItem(Texture);
					}

					if( TexFlags & TF_BumpMap )
					{
						// Turn it into a bump map.
						Texture->CreateBumpMap();
					}
					Texture->CreateColorRange();
				}
				else Out->Logf(LOG_ExecError,"Import texture %s from %s failed",TempName,TempFname);
				GApp->EndSlowTask();
			}
			else Out->Log( LOG_ExecError, "Missing file or name" );
			Processed=1;
		}
		else if( GetCMD(&Str,"Kill") )
		{
			UTexture *Texture;
			UTextureSet *Set;
			if( GetUTexture(Str,"Name=",Texture) )
			{
				FOR_ALL_TYPED_OBJECTS(Set,UTextureSet)
				{
					Set->RemoveItem(Texture);
					if( Set->Num == 0 )
						TextureSets->RemoveItem(Set);
				}	
				END_FOR_ALL_TYPED_OBJECTS;
				Cleanse(*Out,1);
			}
			else if( GetUTextureSet(Str,"Set=",Set) )
			{
				TextureSets->RemoveItem(Set);
				Cleanse(*Out,1);
			}
			Processed=1;
		}
		else if( GetCMD(&Str,"LoadSet") )
		{
			if( GetSTRING(Str,"File=",TempFname,79) )
			{
				GApp->BeginSlowTask("Loading textures",1,0);
				GObj.AddFile(TempFname,NULL);
				UTextureSet *Set;
				FOR_ALL_TYPED_OBJECTS(Set,UTextureSet)
				{
					TextureSets->AddUniqueItem(Set);
				}
				END_FOR_ALL_TYPED_OBJECTS;
				Cleanse(*Out,1);
				GApp->EndSlowTask();
			}
			else Out->Log( LOG_ExecError, "Missing filename" );
			Processed=1;
		}
		else if( GetCMD(&Str,"SaveSet") )
		{
			UTextureSet *Set = NULL;
			if
			(
				GetSTRING(Str,"File=",TempFname,79) 
			&&	(GetCMD(&Str,"All") || GetUTextureSet(Str,"SET=",Set))
			)
			{
				// Tag the appropriate textures.
				GObj.UntagAll();
				if( Set )
				{
					Set->SetFlags(RF_TagExp);
				}
				else
				{
					for( int i=0; i<TextureSets->Num; i++ )
						TextureSets->Element(i)->SetFlags(RF_TagExp);
				}
				GApp->BeginSlowTask("Saving textures",1,0);
				GObj.SaveDependentTagged( TempFname );
				GApp->EndSlowTask();
			}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
		}
	}
	//------------------------------------------------------------------------------------
	// FONT management
	//
	else if( GetCMD(&Str,"FONT") )
	{
		if( GetCMD(&Str,"BUILD") )
		{
			UTexture *Texture;
			if (GetUTexture(Str,"TEXTURE=",Texture))
				UFont *Font = new(Texture->GetName(),CREATE_Replace)UFont(Texture);
			else Out->Log(LOG_ExecError,"Missing texture");
			Processed=1;
		}
	}
	//------------------------------------------------------------------------------------
	// MODE management (Global EDITOR mode):
	//
	else if( GetCMD(&Str,"MODE") )
		{
		Word1 = GUnrealEditor.Mode;  // To see if we should redraw
		Word2 = GUnrealEditor.Mode;  // Destination mode to set
		//
		if (GetONOFF (Str,"GRID=", &DWord1))
			{
			//
			// Before changing grid, force editor to current grid position to avoid jerking:
			//
			constraintFinishAllSnaps (Level);
			Constraints.GridEnabled = DWord1;
			Word1=MAXWORD;
			};
		if (GetONOFF (Str,"ROTGRID=", &DWord1))
			{
			constraintFinishAllSnaps (Level);
			Constraints.RotGridEnabled=DWord1;
			Word1=MAXWORD;
			};
		if (GetONOFF (Str,"SNAPVERTEX=", &DWord1))
			{
			constraintFinishAllSnaps (Level);
			Constraints.SnapVertex=DWord1;
			Word1=MAXWORD;
			};
		if (GetONOFF (Str,"SHOWVERTICES=", &ShowVertices))
			{
			Word1=MAXWORD;
			};
		GetFLOAT (Str,"SPEED=",    &MovementSpeed);
		GetFLOAT (Str,"SNAPDIST=",	&Constraints.SnapDist);
		//
		// Major modes:
		//
		if 		(GetCMD(&Str,"CAMERAMOVE"))		Word2 = EM_CameraMove;
		else if	(GetCMD(&Str,"CAMERAZOOM"))		Word2 = EM_CameraZoom;
		else if	(GetCMD(&Str,"BRUSHFREE"))		Word2 = EM_BrushFree;
		else if	(GetCMD(&Str,"BRUSHMOVE"))		Word2 = EM_BrushMove;
		else if	(GetCMD(&Str,"BRUSHROTATE"))	Word2 = EM_BrushRotate;
		else if	(GetCMD(&Str,"BRUSHSHEER"))		Word2 = EM_BrushSheer;
		else if	(GetCMD(&Str,"BRUSHSCALE"))		Word2 = EM_BrushScale;
		else if	(GetCMD(&Str,"BRUSHSTRETCH"))	Word2 = EM_BrushStretch;
		else if	(GetCMD(&Str,"BRUSHSNAP")) 		Word2 = EM_BrushSnap;
		else if	(GetCMD(&Str,"ADDACTOR"))		Word2 = EM_AddActor;
		else if	(GetCMD(&Str,"MOVEACTOR"))		Word2 = EM_MoveActor;
		else if	(GetCMD(&Str,"TEXTUREPAN"))		Word2 = EM_TexturePan;
		else if	(GetCMD(&Str,"TEXTURESET"))		Word2 = EM_TextureSet;
		else if	(GetCMD(&Str,"TEXTUREROTATE"))	Word2 = EM_TextureRotate;
		else if	(GetCMD(&Str,"TEXTURESCALE")) 	Word2 = EM_TextureScale;
		else if	(GetCMD(&Str,"BRUSHWARP")) 		Word2 = EM_BrushWarp;
		else if	(GetCMD(&Str,"TERRAFORM")) 		Word2 = EM_Terraform;
		//
		if (Word2 != Word1)
			{
			edcamSetMode(Word2);
			GCameraManager->RedrawLevel(Level);
			};
		Processed=1;
		}
	//------------------------------------------------------------------------------------
	// Transaction tracking and control
	//
	else if (GetCMD(&Str,"TRANSACTION"))
		{
		if (GetCMD(&Str,"UNDO"))
			{
			if (GTrans->Undo (Level)) GCameraManager->RedrawLevel (Level);
			Processed=1;
			}
		else if (GetCMD(&Str,"REDO"))
			{
			if (GTrans->Redo(Level)) GCameraManager->RedrawLevel (Level);
			Processed=1;
			}
		GApp->EdCallback(EDC_SelActorChange,0);
		GApp->EdCallback(EDC_SelPolyChange,0);
		GApp->EdCallback(EDC_MapChange,0);
		}
	//------------------------------------------------------------------------------------
	// RES (General objects)
	//
	else if (GetCMD(&Str,"RES"))
		{
		if (GetCMD(&Str,"IMPORT")) // RES IMPORT TYPE=.. NAME=.. FILE=..
			{
			UClass *Class;
			if (GetUClass(Str,"TYPE=",Class) &&
				GetSTRING(Str,"FILE=",TempFname,80) &&
				GetSTRING(Str,"NAME=",TempName,NAME_SIZE))
				{
				//GObj.Import (ResType,TempFname,TempName,1);
				}
			else Out->Log(LOG_ExecError,"Missing file, name, or type");
			Processed=1;
			}
		else if (GetCMD(&Str,"EXPORT")) // RES EXPORT TYPE=.. NAME=.. FILE=..
			{
			UClass *Type;
			UObject *Res;
			if (GetUClass(Str,"TYPE=",Type) &&
				GetSTRING(Str,"FILE=",TempFname,80) &&
				GetOBJ   (Str,"NAME=",Type,&Res))
				{
				GObj.UntagAll();
				Res->ExportToFile(TempFname);
				}
			else Out->Log(LOG_ExecError,"Missing file, name, or type");
			Processed=1;
			}
		else if (GetCMD(&Str,"LOAD")) // RES LOAD FILE=..
			{
			if (GetSTRING (Str,"FILE=",TempFname,80))
				{
				Level->RememberActors	();
				GObj.AddFile			(TempFname,NULL);
				GCache.Flush			();
				Level->ReconcileActors	();
				GCameraManager->RedrawLevel(Level);
				}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
			}
		else if (GetCMD(&Str,"SAVE")) // RES SAVE TYPE=.. NAME=.. FILE=..
			{
			UObject *Res;
			UClass *Type;
			if (GetUClass(Str,"TYPE=",Type) &&
				GetSTRING(Str,"FILE=",TempFname,80) &&
				GetOBJ   (Str,"NAME=",Type,&Res))
				{
				GObj.Save( Res, TempFname );
				}
			else Out->Log(LOG_ExecError,"Missing file, name, or type");
			Processed=1;
			}
		else if (GetCMD(&Str,"ARRAYADD")) // RES ARRAYADD TYPE=.. NAME=.. ARRAY=..
			{
			UClass      *Type;
			UObject	*Res;
			UArray		*Array;
			if (GetUClass(Str,"TYPE=",Type) &&
				GetOBJ   (Str,"NAME=",Type,&Res) &&
				GetUArray(Str,"ARRAY=",Array))
				{
				Array->AddItem(Res);
				}
			else Out->Log(LOG_ExecError,"Missing file, name, or type");
			Processed=1;
			}
		}
	//------------------------------------------------------------------------------------
	// TEXT objects
	//
	else if (GetCMD(&Str,"TEXT"))
		{
		if (GetCMD(&Str,"IMPORT")) // TEXT IMPORT NAME=.. FILE=..
			{
			if (GetSTRING(Str,"FILE=",TempFname,79) && GetSTRING(Str,"NAME=",TempName,79))
				{
				new(TempName,TempFname,IMPORT_Replace)UTextBuffer;
				}
			else Out->Log(LOG_ExecError,"Missing file or name");
			Processed=1;
			}
		else if (GetCMD(&Str,"EXPORT")) // TEXT EXPORT NAME=.. FILE=..
			{
			UTextBuffer *Text;
			if (GetSTRING(Str,"FILE=",TempFname,79) && GetUTextBuffer(Str,"NAME=",Text))
				{
				Text->ExportToFile(TempFname);
				}
			else Out->Log(LOG_ExecError,"Missing file or name");
			Processed=1;
			}
		}
	//------------------------------------------------------------------------------------
	// CLASS functions
	//
	else if( GetCMD(&Str,"CLASS") )
	{
		SaveAsMacro = 0;
		if( GetCMD(&Str,"SAVEBELOW") )
		{
			if( GetSTRING (Str,"FILE=",TempFname,80) &&
				GetUClass (Str,"NAME=",Class) )
			{
				GObj.UntagAll();
				if( mystrstr(TempFname,"UCX") )
				{
					// Save as Unreal object.
					Class->SetFlags(RF_TagExp);
					GObj.TagReferencingTagged( UClass::GetBaseClass() );

					// Snub out ParentClass to prevent tagging all parent classes above.
					UClass *Parent = Class->ParentClass;
					Class->ParentClass = NULL;
					GObj.SaveTagAllDependents();
					Class->ParentClass = Parent;
					GObj.TagImports(0);
					GObj.SaveTagged( TempFname);
				}
				else if( mystrstr(TempFname,"H") || mystrstr(TempFname,"U") )
				{
					// Save as C++ header.
					UClass *TempClass;
					FOR_ALL_TYPED_OBJECTS(TempClass,UClass)
					{
						if( TempClass->IsChildOf( Class ) )
						{
							TempClass->SetFlags( RF_TagExp );
							if( TempClass->Script )
								TempClass->Script->SetFlags( RF_TagExp );
						}
					}
					END_FOR_ALL_TYPED_OBJECTS;

					// Tag all imports for DeclareClass.
					GObj.TagImports(0);

					// Untag parent class.
					if( Class->ParentClass )
						Class->ParentClass->ClearFlags( RF_TagImp | RF_TagExp );

					// Export.
					Class->ExportToFile( TempFname );
				}
			}
			else Out->Log( LOG_ExecError, "Missing file or name" );
			Processed=1;
		}
		else if (GetCMD(&Str,"SET")) // CLASS SET
			{
			UClass *Class;
			if (GetUClass(Str,"CLASS=",Class) &&
				GetUMesh(Str,"MESH=",Class->GetDefaultActor().Mesh))
				{
				Class->GetDefaultActor().DrawType = DT_Mesh;
				}
			else Out->Log(LOG_ExecError,"Missing class or meshmap");
			Processed=1;
			}
		else if (GetCMD(&Str,"LOAD")) // CLASS LOAD FILE=..
		{
			if( GetSTRING( Str, "FILE=", TempFname, 80 ))
			{
				Out->Logf("Loading classes from %s",TempFname);
				if( mystrstr( TempFname, "UCX") )
				{
					// Load from Unrealfile.
					GObj.AddFile( TempFname, NULL );
					Cleanse( *Out, 1 );
				}
				else if( mystrstr(TempFname,"U") )
				{
					// TempClass is a dummy class; it's import routine imports multiple, named classes
					UClass *TempClass = new("Temp",TempFname,IMPORT_Replace)UClass;
					if( TempClass )
						TempClass->Kill();
				}
				else Out->Log(LOG_ExecError,"Unrecognized file type");
			}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
		}
		else if( GetCMD(&Str,"NEW") ) // CLASS NEW
		{
			UClass *Parent;
			if (GetUClass(Str,"PARENT=",Parent) && 
				GetSTRING(Str,"NAME=",TempStr,NAME_SIZE))
			{
				UClass *Class = new(TempStr,CREATE_Replace)UClass(Parent);
				if( Class )
					Class->ScriptText = new(TempStr,CREATE_Replace,RF_NotForClient|RF_NotForServer)UTextBuffer(1);
				else Out->Log( LOG_ExecError, "Class not found" );
			}
			Processed=1;
		}
		else if (GetCMD(&Str,"DELETE"))
			{
			UClass *Class;
			if (GetUClass(Str,"NAME=",Class))
				{
				GTrans->Reset			("deleting actor class"); // Not tracked transactionally
				Level->Lock				(LOCK_Trans);
				edactDeleteDependentsOf (Level,Class);
				Class->DeleteClass		();
				Level->Unlock			(LOCK_Trans);
				//
				GApp->EdCallback(EDC_SelActorChange,0);
				}
			else Out->Log(LOG_ExecError,"Missing name");
			Processed=1;
			};
		}
	//------------------------------------------------------------------------------------
	// MACRO functions
	//
	else if (GetCMD(&Str,"MACRO"))
	{
		if( GetCMD(&Str,"PLAY") ) // MACRO PLAY [NAME=..] [FILE=..]
		{
			Word1 = GetSTRING( Str,"FILE=",TempFname,79 );
			Word2 = GetSTRING( Str,"NAME=",TempName,79  );

			UTextBuffer *Text;
			if     ( Word1 && Word2 ) Text = new( TempName, TempFname, IMPORT_Replace    )UTextBuffer;
			else if( Word1          ) Text = new( TempName, TempFname, IMPORT_MakeUnique )UTextBuffer;
			else                      Text = new( TempName, FIND_Optional                )UTextBuffer;

			if( Text )
			{
				GObj.AddToRoot(Text);
				Text->Lock( LOCK_Read );
				char Temp[256];
				const char *Data = &Text->Element(0);
				while( GetLINE (&Data,Temp,256)==0 )
					Exec( Temp );
				Text->Unlock( LOCK_Read );
				GObj.RemoveFromRoot(Text);
				Text->Kill();
			}
			else Out->Log( LOG_ExecError, "Macro not found for playing" );
			Processed=1;
		}
		else if (GetCMD(&Str,"RECORD")) // MACRO RECORD NAME=..
			{
			if (!GetSTRING (Str,"NAME=",TempName,79)) strcpy (TempName,"MACRO");
			//
			MacroRecBuffer = new(TempName,CREATE_Replace)UTextBuffer(MACRO_TEXT_REC_SIZE);
			SaveAsMacro = 0;
			//
			Out->Log(LOG_ExecError,"Macro record begin");
			Processed=1;
			}
		else if (GetCMD(&Str,"ENDRECORD")) // MACRO ENDRECORD
			{
			MacroRecBuffer = NULL;
			SaveAsMacro = 0;
			//
			Out->Log(LOG_ExecError,"Macro record ended");
			Processed=1;
			}
		else if (GetCMD(&Str,"LOAD")) // MACRO LOAD FILE=..
			{
			if (!GetSTRING (Str,"NAME=",TempName,79)) strcpy (TempName,"MACRO");
			if (GetSTRING(Str,"FILE=",TempFname,79))
				{
				new(TempName,TempFname,IMPORT_Replace)UTextBuffer;
				}
			else Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
			}
		else if (GetCMD(&Str,"SAVE")) // MACRO SAVE [NAME=..] FILE=..
			{
			if (!GetSTRING (Str,"NAME=",TempName,79)) strcpy (TempName,"MACRO");
			UTextBuffer *Text = new(TempName,FIND_Optional)UTextBuffer;
			//
			if (!Text) 										Out->Log(LOG_ExecError,"Macro not found for saving");
			else if (GetSTRING(Str,"FILE=",TempFname,79))	Text->ExportToFile(TempFname);
			else											Out->Log(LOG_ExecError,"Missing filename");
			Processed=1;
			}
		}
	//------------------------------------------------------------------------------------
	// MESH functions
	//
	else if (GetCMD(&Str,"MESH"))
	{
		if( GetCMD(&Str,"IMPORT") ) // MESH IMPORT MESH=.. ANIVFILE=.. DATAFILE=..
		{
			if
			(	GetSTRING(Str,   "MESH=",TempName,79)
			&&	GetSTRING(Str,"ANIVFILE=",TempStr,79)
			&&	GetSTRING(Str,"DATAFILE=",TempStr1,79) )
			{
				meshImport (TempName,TempStr,TempStr1);
			}
			else Out->Log(LOG_ExecError,"Bad MESH IMPORT");
			Processed=1;
		}
		else if( GetCMD(&Str,"ORIGIN") ) // MESH ORIGIN X=.. Y=.. Z=..
		{
			UMesh *Mesh;
			if( GetUMesh(Str,"MESH=",Mesh) )
			{
				Mesh->Origin    = FVector(0,0,0);
				Mesh->RotOrigin = FRotation(0,0,0);
				GetFVECTOR  ( Str, &Mesh->Origin );
				GetFROTATION( Str, &Mesh->RotOrigin, 256 );
			}
			else Out->Log( LOG_ExecError, "Bad MESH ORIGIN" );
			Processed=1;
		}
		else if( GetCMD(&Str,"SEQUENCE") ) // MESH SEQUENCE MESH=.. SEQ=.. STARTFRAME=.. NUMFRAMES=..
		{
			UMesh *Mesh;
			FMeshAnimSeq Seq;
			Seq.Init();
			if
			(	GetUMesh	(Str,      "MESH=",Mesh)
			&&	GetNAME     (Str,       "SEQ=",&Seq.Name)
			&&	GetWORD		(Str,"STARTFRAME=",&Seq.StartFrame)
			&&	GetWORD		(Str, "NUMFRAMES=",&Seq.NumFrames) )
			{
				GetFLOAT( Str, "RATE=", &Seq.Rate );
				Mesh->AnimSeqs->AddItem( Seq );
			}
			else Out->Log(LOG_ExecError,"Bad MESH SEQUENCE");
			Processed=1;
		}
		else if( GetCMD(&Str,"NOTIFY") ) // MESH NOTIFY MESH=.. SEQ=.. TIME=.. FUNCTION=..
		{
			UMesh *Mesh;
			FName SeqName;
			FMeshAnimNotify Notify;
			Notify.Init();
			if
			(	GetUMesh	(Str,      "MESH=",Mesh)
			&&	GetNAME     (Str,       "SEQ=",&SeqName)
			&&	GetFLOAT    (Str,      "TIME=",&Notify.Time)
			&&	GetNAME     (Str,  "FUNCTION=",&Notify.Function) )
			{
				FMeshAnimSeq* Seq = Mesh->GetAnimSeq( SeqName );
				if( Seq )
				{
					if( Seq->StartNotify + Seq->NumNotifys != Mesh->Notifys->Num )
					{
						// Move to end.
						int NewStartNotify = Mesh->Notifys->Num;
						for( int i=0; i<Seq->NumNotifys; i++ )
							Mesh->Notifys->AddItem( Mesh->Notifys(i+Seq->StartNotify) );
						Seq->StartNotify = NewStartNotify;
					}
					Mesh->Notifys->AddItem( Notify );
					Seq->NumNotifys++;
				}
				else Out->Log( LOG_ExecError, "Unknown sequence in MESH NOTIFY" );
			}
			else Out->Log( LOG_ExecError, "Bad MESH NOTIFY" );
			Processed=1;
		}
		else if( GetCMD(&Str,"SAVE") ) // MESH SAVE MESH=.. FILE=..
		{
			UMesh *Mesh;
			if
			(	GetUMesh   (Str,"MESH=",Mesh)
			&&	GetSTRING  (Str,"FILE=",TempFname,79) )
			{
				Out->Log("Saving mesh");
				GObj.Save( Mesh, TempFname );
			}
			else Out->Log(LOG_ExecError,"Missing mesh or filename");
			Processed=1;
		}
	}
	//------------------------------------------------------------------------------------
	// MESHMAP functions
	//
	else if (GetCMD(&Str,"MESHMAP"))
		{
		if (GetCMD(&Str,"NEW")) // MESHMAP NEW MESHMAP=.. MESH=..
			{
			UMesh *Mesh;
			if (GetSTRING (Str,"MESHMAP=",TempName,NAME_SIZE) &&
				GetUMesh  (Str,  "MESH=",Mesh))
				{
				UMesh *NewMesh = new(TempName,FIND_Optional)UMesh;
				if (!NewMesh)
					{
					// Create it now.
					NewMesh = new(TempName,Mesh,DUPLICATE_Unique)UMesh;
					};
				// Set flags.
				Word2 = MAXWORD; GetWORD (Str,"AND=",&Word2);
				Word3 = 0;       GetWORD (Str, "OR=",&Word3);
				//
				NewMesh->AndFlags    = Word2;
				NewMesh->OrFlags     = Word3;
				}
			else Out->Log(LOG_ExecError,"Missing meshmap or mesh");
			Processed=1;
			}
		else if (GetCMD(&Str,"SCALE")) // MESHMAP SCALE X=.. Y=.. Z=..
			{
			UMesh *Mesh;
			if (GetUMesh(Str,"MESHMAP=",Mesh))
				{
				GetFVECTOR(Str,&Mesh->Scale);
				}
			else Out->Log(LOG_ExecError,"Missing meshmap");
			Processed=1;
			}
		else if (GetCMD(&Str,"SETTEXTURE")) // MESHMAP SETTEXTURE MESHMAP=.. NUM=.. TEXTURE=..
			{
			UMesh *Mesh;
			UTexture *Texture;
			if (GetUMesh(Str,"MESHMAP=",Mesh) &&
			    GetUTexture(Str,"TEXTURE=",Texture) &&
				GetWORD    (Str,    "NUM=",&Word2))
				{
				if (Word2 >= Mesh->Textures->Max) Out->Log(LOG_ExecError,"Texture number exceeds maximum");
				else Mesh->Textures(Word2) = Texture;
				}
			else Out->Logf(LOG_ExecError,"Missing meshmap, texture, or num (%s)",Str);
			Processed=1;
			}
		else if (GetCMD(&Str,"SAVE")) // MESHMAP SAVE MESHMAP=.. FILE=..
			{
			UMesh *Mesh;
			if (GetUMesh(Str,"MESHMAP=",Mesh) &&
				GetSTRING(Str,"FILE=",TempFname,79    ))
				{
				GObj.SaveDependent( Mesh, TempFname );
				}
			else Out->Log(LOG_ExecError,"Missing meshmap or file");
			Processed=1;
			}
		}
	//------------------------------------------------------------------------------------
	// SCRIPT: script compiler
	//
	else if( GetCMD(&Str,"SCRIPT") )
	{
		if( GetCMD(&Str,"MAKE") )
		{
			GApp->BeginSlowTask("Compiling scripts",0,0);
			MakeScripts(GetCMD(&Str,"ALL"));
			GApp->EndSlowTask();
			Processed=1;
		}
		else if( GetCMD(&Str,"DECOMPILE") )
		{
			UClass *Class;
			if( GetUClass(Str,"CLASS=",Class) ) DecompileScript(Class,*Out,1);
			else Out->Log(LOG_ExecError,"Missing class");
			Processed=1;
		}
	}
	//------------------------------------------------------------------------------------
	// CUTAWAY: cut-away areas for overhead view
	//
	else if( GetCMD(&Str,"CUTAWAY") )
	{
		if( GetCMD(&Str,"SHOW") )
		{
			Word1 = 1; // Show
			ShowOrHideCutaway:;

			Word2=0;
			if (GetCMD(&Str,"ALL")) Word2=1;
			else if (GetCMD(&Str,"SELECTED")) Word2=2;

			if( Word2 )
			{
				// Cutaway logic goes here
			}
			Processed=1;
		}
		else if( GetCMD(&Str,"HIDE") )
		{
			Word1 = 0; // Hide
			goto ShowOrHideCutaway;
		}
	}
	//------------------------------------------------------------------------------------
	// Other handlers.
	//
	else if( GetCMD(&Str,"MUSIC") )
	{
		MusicCmdLine(Str,Out);
		return 1;
	}
	else if( GetCMD(&Str,"AUDIO") )
	{
		AudioCmdLine(Str,Out);
		return 1;
	}
	//------------------------------------------------------------------------------------
	// Done with this command.  Now note it (if we're recording a macro) and go to
	// next command:
	//
	if (SaveAsMacro) NoteMacroCommand (Stream);
	return Processed;

	unguardf(("(%s)%s",ErrorTemp,(strlen(ErrorTemp)>=69)?"..":""));
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
