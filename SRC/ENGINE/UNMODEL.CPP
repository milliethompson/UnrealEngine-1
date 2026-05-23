/*=============================================================================
	UnModel.cpp: Unreal model functions

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*---------------------------------------------------------------------------------------
	UPrimitive object implementation.
---------------------------------------------------------------------------------------*/

IMPLEMENT_CLASS(UPrimitive);

/*---------------------------------------------------------------------------------------
	UBspNodes object implementation.
---------------------------------------------------------------------------------------*/

void UBspNodes::InitHeader()
{
	guard(UBspNodes::InitHeader);

	// Call parent.
	UDatabase::InitHeader();

	// Init zone info.
	NumZones = 0;	
	for (int i=0; i<MAX_ZONES; i++)
	{
		Zones[i].ZoneActor    = NULL;
		Zones[i].Connectivity = ((QWORD)1)<<i;
		Zones[i].Visibility   = ~(QWORD)0;
	}
	
	unguard;
}
void UBspNodes::PostLoadItem( int Index, DWORD PostFlags )
{
	guard(UBspNodes::PostLoadItem);

	// Reset Bsp flags.
	Element(Index).NodeFlags   &= ~(NF_IsNew | NF_TagForEmpty);
	Element(Index).iDynamic[0]  = Element(Index).iDynamic[1] = INDEX_NONE;

	unguard;
}
IMPLEMENT_DB_CLASS(UBspNodes);

/*---------------------------------------------------------------------------------------
	UBspSurfs object implementation.
---------------------------------------------------------------------------------------*/

void UBspSurfs::ModifyItem(int Index, int UpdateMaster)
{
	guard(UBspSurfs::Modify);

	UDatabase::ModifyItem(Index);

	if( UpdateMaster && Element(Index).Brush )
		Element(Index).Brush->Polys->ModifyItem(Element(Index).iBrushPoly);

	unguard;
}
void UBspSurfs::ModifyAllItems(int UpdateMaster)
{
	guard(UBspSurfs::ModifyAllItems);

	for( int i=0; i<Num; i++ )
		ModifyItem(i,UpdateMaster);

	unguard;
}
void UBspSurfs::ModifySelected(int UpdateMaster)
{
	guard(UBspSurfs::ModifySelected);

	for( int i=0; i<Num; i++ )
		if( Element(i).PolyFlags & PF_Selected )
			ModifyItem(i,UpdateMaster);

	unguard;
}
IMPLEMENT_DB_CLASS(UBspSurfs);

/*---------------------------------------------------------------------------------------
	ULightMesh object implementation.
---------------------------------------------------------------------------------------*/

IMPLEMENT_DB_CLASS(ULightMesh);

/*---------------------------------------------------------------------------------------
	UVectors object implementation.
---------------------------------------------------------------------------------------*/

IMPLEMENT_DB_CLASS(UVectors);

/*---------------------------------------------------------------------------------------
	UFloats object implementation.
---------------------------------------------------------------------------------------*/

IMPLEMENT_DB_CLASS(UFloats);

/*---------------------------------------------------------------------------------------
	UVerts object implementation.
---------------------------------------------------------------------------------------*/

void UVerts::InitHeader()
{
	guard(UVerts::InitHeader);

	// Init parent.
	UDatabase::InitHeader();

	// Init UVerts info.
	NumSharedSides  = 4; // First 4 shared sides are view frustrum sides.

	unguard;
}
IMPLEMENT_DB_CLASS(UVerts);

/*---------------------------------------------------------------------------------------
	UBound object implementation.
---------------------------------------------------------------------------------------*/

IMPLEMENT_DB_CLASS(UBounds);

/*---------------------------------------------------------------------------------------
	UModel object implementation.
---------------------------------------------------------------------------------------*/

void UModel::InitHeader()
{
	guard(UModel::InitHeader);

	// Init parent class.
	UPrimitive::InitHeader();
	bCheckCollision = 1;

	// Init object header to defaults.
	Vectors    		= (UVectors  *)NULL;
	Points     		= (UVectors  *)NULL;
	Nodes    		= (UBspNodes *)NULL;
	Surfs			= (UBspSurfs *)NULL;
	Verts			= (UVerts    *)NULL;
	Polys	     	= (UPolys    *)NULL;
	Bounds			= (UBounds   *)NULL;
	LeafHulls		= (UInts     *)NULL;
	LightMesh   	= (ULightMesh*)NULL;
	Location      	= GMath.ZeroVector;
	Rotation		= GMath.ZeroRotation;
	PrePivot		= GMath.ZeroVector;
	PostPivot		= GMath.ZeroVector;
	Scale			= GMath.UnitScale;
	PostScale		= GMath.UnitScale;
	CsgOper			= CSG_Active;
	ModelFlags		= 0;

	TransformedBound.Init();

	unguard;
}
const char *UModel::Import(const char *Buffer, const char *BufferEnd,const char *FileType)
{
	guard(UModel::Import);
	const char		*StrPtr;
	char			StrLine[256];

	Init(1);

	while( GetLINE (&Buffer,StrLine,256)==0 )
	{
		StrPtr = StrLine;

		if( GetEND(&StrPtr,"BRUSH") )
		{
			// End of brush polys.
			break;
		}
		else if( GetBEGIN (&StrPtr,"POLYLIST") )
		{
			if (Polys==NULL)
				Polys = new(GetName(),CREATE_Replace,GetContextFlags())UPolys;
			
			Buffer = Polys->Import(Buffer,BufferEnd,FileType);
			
			if (!Buffer)
				return NULL;
		}
		else if( GetCMD(&StrPtr,"SETTINGS") )
		{
			GetDWORD(StrPtr,"CSG=",			(DWORD *)&CsgOper);
			GetDWORD(StrPtr,"FLAGS=",		&ModelFlags);
			GetDWORD(StrPtr,"POLYFLAGS=",	&PolyFlags);
			GetDWORD(StrPtr,"COLOR=",		&Color);

			ModelFlags &= ~(MF_NoImport);
		}
		else if (GetCMD(&StrPtr,"LOCATION"))		GetFVECTOR 	(StrPtr,&Location);
		else if (GetCMD(&StrPtr,"PREPIVOT"))		GetFVECTOR 	(StrPtr,&PrePivot);
		else if (GetCMD(&StrPtr,"POSTPIVOT"))		GetFVECTOR 	(StrPtr,&PostPivot);
		else if (GetCMD(&StrPtr,"SCALE"))			GetFSCALE 	(StrPtr,&Scale);
		else if (GetCMD(&StrPtr,"POSTSCALE"))		GetFSCALE 	(StrPtr,&PostScale);
		else if (GetCMD(&StrPtr,"ROTATION"))		GetFROTATION(StrPtr,&Rotation,1);
	}
	
	if (GEditor)
		GEditor->bspValidateBrush(this,0,0);

	return Buffer;
	unguard;
}
void UModel::Export( FOutputDevice &Out, const char *FileType, int Indent )
{
	guard(UModel::Export);
	char TempStr[256];

	Out.Logf("%sBegin Brush Name=%s\r\n",spc(Indent),GetName());

	// Save all brush properties.
	Out.Logf("%s   Settings  CSG=%i Flags=%u PolyFlags=%u Color=%i\r\n",spc(Indent),CsgOper,ModelFlags,PolyFlags,Color);
	Out.Logf("%s   Location  %s\r\n",spc(Indent),SetFVECTOR   (TempStr,&Location));
	Out.Logf("%s   PrePivot  %s\r\n",spc(Indent),SetFVECTOR   (TempStr,&PrePivot));
	Out.Logf("%s   PostPivot %s\r\n",spc(Indent),SetFVECTOR   (TempStr,&PostPivot));
	Out.Logf("%s   Scale     %s\r\n",spc(Indent),SetFSCALE    (TempStr,&Scale));
	Out.Logf("%s   PostScale %s\r\n",spc(Indent),SetFSCALE    (TempStr,&PostScale));
	Out.Logf("%s   Rotation  %s\r\n",spc(Indent),SetROTATION  (TempStr,&Rotation));

	// Export FPolys.
	Polys->Export(Out,FileType,Indent+3);

	Out.Logf("%sEnd Brush\r\n",spc(Indent));

	unguard;
}
void UModel::PostLoadHeader( DWORD PostFlags )
{
	guard(UModel::PostLoadHeader);

	// Postload parent.
	UPrimitive::PostLoadHeader( PostFlags );

	// Regenerate bounds if transacting.
	if( PostFlags & POSTLOAD_Trans )
	{
		if
		(	Nodes && Surfs
		&&	((Nodes->GetFlags() & RF_TransData) || Surfs->GetFlags() & RF_TransData) )
		{
			Lock(LOCK_ReadWrite);
			GEditor->bspBuildBounds(this);
			Unlock(LOCK_ReadWrite);
		}
	}

	unguard;
}
IMPLEMENT_CLASS(UModel);

/*---------------------------------------------------------------------------------------
	UModel implementation.
---------------------------------------------------------------------------------------*/

//
// Lock a model.
//
INT UModel::Lock( DWORD NewLockType )
{
	guard(UModel::Lock);

	// Lock all child objects.
	if( Nodes     ) Nodes    ->Lock(NewLockType);
	if( Surfs     ) Surfs    ->Lock(NewLockType);
	if( Polys     ) Polys    ->Lock(NewLockType);
	if( Points    ) Points   ->Lock(NewLockType);
	if( Vectors   ) Vectors  ->Lock(NewLockType);
	if( Verts     ) Verts    ->Lock(NewLockType);
	if( LightMesh ) LightMesh->Lock(NewLockType);

	return UPrimitive::Lock(NewLockType);
	unguard;
}

//
// Unlock a model.
//
void UModel::Unlock( DWORD OldLockType )
{
	guard(UModel::Unlock);

	if( Nodes     ) Nodes    ->Unlock(OldLockType);
	if( Surfs     ) Surfs    ->Unlock(OldLockType);
	if( Polys     ) Polys    ->Unlock(OldLockType);
	if( Points    ) Points   ->Unlock(OldLockType);
	if( Vectors   ) Vectors  ->Unlock(OldLockType);
	if( Verts     ) Verts    ->Unlock(OldLockType);
	if( LightMesh ) LightMesh->Unlock(OldLockType);

	UPrimitive::Unlock(OldLockType);
	unguard;
}

//
// Init a model's parameters to defaults without affecting what's in it
//
void UModel::Init( int InitPositionRotScale )
{
	guard(UModel::Init);

	// Init basic info.
	CsgOper	    = CSG_Active;
	PolyFlags	= 0;
	Color		= 0;

	if( InitPositionRotScale )
	{
		// Init position, rotation, and scale only.
		Scale		= GMath.UnitScale;
		PostScale	= GMath.UnitScale;
		Location	= GMath.ZeroVector;
		Rotation	= GMath.ZeroRotation;
	}
	else
	{
		// Just update.
		if( GEditor )
			GEditor->constraintApply(NULL,NULL,&Location,&Rotation,&GEditor->Constraints);
		
		FModelCoords TempCoords;
		BuildCoords( &TempCoords, NULL );

		Location += PostPivot - PrePivot.TransformVectorBy(TempCoords.PointXform);
	}
	PrePivot	= GMath.ZeroVector;
	PostPivot	= GMath.ZeroVector;
	unguard;
}

//
// Allocate subobjects for a model.
//
void UModel::AllocDatabases( BOOL AllocPolys )
{
	guard(UModel::AllocDatabases);

	// Make vector table name.
	char VName[NAME_SIZE]="V";
	mystrncat( VName, GetName(), NAME_SIZE );

	// Make point table name.
	char PName[NAME_SIZE]="P";
	mystrncat( PName, GetName(), NAME_SIZE );

	// Allocate all objects for model (indentation shows hierarchy).
	// Doesn't allocate light mesh; that's allocated when built.
	Nodes		= new(GetName(), CREATE_Replace, GetContextFlags())UBspNodes(0);
	Surfs		= new(GetName(), CREATE_Replace, GetContextFlags())UBspSurfs(0);
	Verts   	= new(GetName(), CREATE_Replace, GetContextFlags())UVerts(0);
	Bounds	    = new(GetName(), CREATE_Replace, GetContextFlags())UBounds(0);
	LeafHulls   = new(GetName(), CREATE_Replace, GetContextFlags())UInts(0);
	Vectors		= new(VName,     CREATE_Replace, GetContextFlags())UVectors(0);
	Points		= new(PName,     CREATE_Replace, GetContextFlags())UVectors(0);

	if( AllocPolys )
		Polys	= new(GetName(), CREATE_Replace, GetContextFlags())UPolys(0);

	unguard;
}

//
// Create a new model and allocate all objects needed for it.
// Call with Editor=1 to allocate editor structures for it, also.  Returns
// model's ID if ok, NULL if error.
//
UModel::UModel( int InEditable, int InRootOutside )
:	RootOutside( InRootOutside )
{
	guard(UModel::UModel);

	// Allocate tables.
	AllocDatabases(1);

	Init(1);
	unguard;
}

//
// Build an optional coordinate system and anticoordinate system
// for a model.  Returns orientation, 1.0 if scaling is normal,
// -1.0 if scaling mirrors the brush.
//
FLOAT UModel::BuildCoords( FModelCoords *Coords, FModelCoords *Uncoords )
{
	guard(UModel::BuildCoords);
	if( Coords )
	{
		Coords->PointXform    = (GMath.UnitCoords * PostScale * Rotation * Scale);
		Coords->VectorXform   = (GMath.UnitCoords / Scale / Rotation / PostScale).Transpose();
	}
	if( Uncoords )
	{
		Uncoords->PointXform  = (GMath.UnitCoords / Scale / Rotation / PostScale);
		Uncoords->VectorXform = (GMath.UnitCoords * PostScale * Rotation * Scale).Transpose();
	}
	return Scale.Orientation();
	unguard;
}

//
// Build the model's bounds (min and max):
//
void UModel::BuildBound( int Transformed )
{
	guard(UModel::BuildBound);
	Lock(LOCK_ReadWrite);

	if( Polys && Polys->Num>0 )
	{
		int Total=0;
		for( int i=0; i<Polys->Num; i++ )
			Total += Polys(i).NumVertices;

		FModelCoords Coords;
		FMemMark	Mark(GMem);
		FVector		*Points = new(GMem,Total)FVector;
		FLOAT		Orientation = 0.0;

		if( Transformed )
			Orientation = BuildCoords(&Coords,NULL);

		int Count = 0;
		for( i=0; i<Polys->Num; i++ )
		{
			FPoly Temp = Polys(i);
			
			if( Transformed )
				Temp.Transform( Coords, &PrePivot, &Location, Orientation );

			for( int j=0; j<Temp.NumVertices; j++ )
				Points[Count++] = Temp.Vertex[j];
		}
		checkLogic(Count==Total);
		if( Transformed )
			TransformedBound.Init(Points,Total);
		else
			LocalBound.Init(Points,Total);
		Mark.Pop();
	}
	else
	{
		if( Transformed )
			TransformedBound.Init();
		else
			LocalBound.Init();
	}

	Unlock(LOCK_ReadWrite);
	unguard;
}

//
// Transform this model by its coordinate system.
//
void UModel::Transform()
{
	guard(UModel::Transform);
	Lock(LOCK_Trans);

	FModelCoords	Coords,Uncoords;
	FLOAT			Orientation;

	Orientation = BuildCoords(&Coords,&Uncoords);

	Location += PostPivot;
	for( INDEX i=0; i<Polys->Num; i++ )
		Polys(i).Transform( Coords, &PrePivot, &Location, Orientation	);

	Location -= PostPivot;
	Unlock(LOCK_Trans);
	unguard;
}

//
// Set a brush's absolute pivot location, without affecting the brush's
// post-transformation location.  Usually called in map edit mode to force
// all brushes to share a common pivot.
//
// Assumes that brush location and rotation are in their desired positions
// and that any desired snaps have already been applied to it.
//
void UModel::SetPivotPoint( FVector *PivotLocation, int SnapPivotToGrid )
{
	guard(UModel::SetPivotPoint);

	FModelCoords Coords,Uncoords;
	BuildCoords(&Coords,&Uncoords);

	if( GTrans )
		GTrans->NoteResHeader (this);

	PrePivot += (*PivotLocation - Location - PostPivot).TransformVectorBy(Uncoords.PointXform);
	Location  = *PivotLocation;

	if( GEditor )
		GEditor->constraintApply(NULL,NULL,&Location,&Rotation,&GEditor->Constraints);

	if( SnapPivotToGrid )
		PostPivot = GMath.ZeroVector;
	else
		PostPivot = *PivotLocation - Location;
	
	unguard;
}

//
// Copy position, rotation, and scale from another model.
//
void UModel::CopyPosRotScaleFrom( UModel *OtherModel )
{
	guard(UModel::CopyPosRotScaleFrom);

	Location	= OtherModel->Location;
	Rotation	= OtherModel->Rotation;
	PrePivot	= OtherModel->PrePivot;
	Scale		= OtherModel->Scale;
	PostPivot	= OtherModel->PostPivot;
	PostScale	= OtherModel->PostScale;

	BuildBound(0);
	BuildBound(1);

	unguard;
}

/*---------------------------------------------------------------------------------------
	UModel basic implementation (not including physics).
---------------------------------------------------------------------------------------*/

//
// Empty the contents of a model.
//
void UModel::EmptyModel( int EmptySurfInfo, int EmptyPolys )
{
	guard(UModel::Empty);

	Nodes->Empty();
	Nodes->NumZones	= 0;
	
	Verts->Empty(); // First 4 shared sides are view frustrum edges.
	Verts->NumSharedSides = 4;

	Bounds->Empty();
	LeafHulls->Empty();

	if( EmptyPolys )
		Polys->Empty();

	if( EmptySurfInfo )
	{
		Vectors->Empty();
		Points->Empty();
		Surfs->Empty();
	}
	unguard;
}

//
// Shrink all stuff to its minimum size.
//
void UModel::ShrinkModel()
{
	guard(UModel::ShrinkModel);

	if( Vectors   ) Vectors  ->Shrink();
	if( Points    ) Points   ->Shrink();
	if( Nodes     ) Nodes    ->Shrink();
	if( Surfs     ) Surfs    ->Shrink();
	if( Verts     ) Verts    ->Shrink();
	if( Polys     ) Polys    ->Shrink();
	if( Bounds    ) Bounds   ->Shrink();
	if( LeafHulls ) LeafHulls->Shrink();

	unguard;
}

/*---------------------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------------------*/
