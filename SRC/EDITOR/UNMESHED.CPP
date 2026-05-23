/*=============================================================================
	UnMeshEd.cpp: Unreal editor mesh code

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Note: See DRAGON.MAC for a sample import macro.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Data types for importing James' creature meshes.
-----------------------------------------------------------------------------*/

// James mesh info.
struct FJSDataHeader
{
	WORD	NumPolys;
	WORD	NumVertices;
	WORD	BogusRot;
	WORD	BogusFrame;
	DWORD	BogusNormX,BogusNormY,BogusNormZ;
	DWORD	FixScale;
	DWORD	Unused1,Unused2,Unused3;
};

// James animation info.
struct FJSAnivHeader
{
	WORD	NumFrames;		// Number of animation frames.
	WORD	FrameSize;		// Size of one frame of animation.
};

/*-----------------------------------------------------------------------------
	Import functions.
-----------------------------------------------------------------------------*/

//
// Import a mesh from James' editor.  Uses file commands instead of object
// manager.  Slow but works fine.
//
void FGlobalEditor::meshImport
(
	const char *MeshName, 
	const char *AnivFname, 
	const char *DataFname 
)
{
	guard(FGlobalEditor::meshImport);

	UMesh			*Mesh;
	FILE			*AnivFile,*DataFile;
	FJSDataHeader	JSDataHdr;
	FJSAnivHeader	JSAnivHdr;
	int				i;
	int				Ok = 0;

	debugf              ( LOG_Info, "Importing %s", MeshName );
	GApp->BeginSlowTask ( "Importing mesh", 1, 0 );
	GApp->StatusUpdate  ( "Reading files", 0, 0 );

	// Open James' animation vertex file and read header.
	AnivFile = fopen( AnivFname, "rb" );
	if( AnivFile == NULL )
	{
		debugf( LOG_Info, "Error opening file %s", AnivFname );
		goto Out1;
	}
	if( fread( &JSAnivHdr, sizeof(FJSAnivHeader), 1, AnivFile ) !=1 )
	{
		debugf( LOG_Info, "Error reading %s", AnivFname );
		goto Out2;
	}

	// Open James' mesh data file and read header.
	DataFile = fopen (DataFname,"rb");
	if( DataFile == NULL )
	{
		debugf( LOG_Info, "Error opening file %s", DataFname );
		goto Out2;
	}
	if( fread( &JSDataHdr, sizeof( FJSDataHeader ), 1, DataFile ) !=1 )
	{
		debugf( LOG_Info, "Error reading %s", DataFile );
		goto Out3;
	}

	// Allocate mesh object.
	Mesh = new(MeshName,CREATE_Replace)UMesh
	(
		JSDataHdr.NumPolys,
		JSDataHdr.NumVertices,
		JSAnivHdr.NumFrames,
		UMesh::NUM_TEXTURES
	);
	Mesh->Lock(LOCK_ReadWrite);

	// Display summary info.
	debugf(LOG_Info," * Triangles  %i",Mesh->Tris->Max);
	debugf(LOG_Info," * Vertex     %i",Mesh->Verts->Max);
	debugf(LOG_Info," * AnimFrames %i",Mesh->AnimFrames);
	debugf(LOG_Info," * FrameSize  %i",JSAnivHdr.FrameSize);
	debugf(LOG_Info," * AnimSeqs   %i",Mesh->AnimSeqs->Max);

	// Import mesh triangles.
	debugf( LOG_Info, "Importing triangles" );
	GApp->StatusUpdate( "Importing Triangles", 0, 0 );
	fseek( DataFile, 12, SEEK_CUR );
	for( i=0; i<Mesh->Tris->Max; i++ )
	{
		guard(Importing triangles);

		FMeshTri &Tri = Mesh->Tris(i);
		if( fread( &Tri, sizeof(FMeshTri), 1, DataFile ) != 1 )
		{
			debugf( LOG_Info, "Error processing %s", DataFile );
			goto Out4;
		}
		Tri.Flags = 0;
		unguard;
	}

	// Import mesh vertices.
	debugf( LOG_Info, "Importing vertices" );
	GApp->StatusUpdate( "Importing Vertices", 0, 0 );
	for( i=0; i<Mesh->AnimFrames; i++ )
	{
		guard(Importing animation frames);
		FMeshVert *TempVert = &Mesh->Verts(i * Mesh->FrameVerts);
		if( fread( TempVert, sizeof(FMeshVert), Mesh->FrameVerts, AnivFile ) != (size_t)Mesh->FrameVerts )
		{
			debugf( LOG_Info, "Vertex error in %s", AnivFname );
			goto Out4;
		}
		fseek( AnivFile, JSAnivHdr.FrameSize - Mesh->FrameVerts * sizeof(FMeshVert), SEEK_CUR );
		unguard;
	}

	// Build list of triangles per vertex.
	GApp->StatusUpdate( "Linking mesh", i, Mesh->FrameVerts );
	for( i=0; i<Mesh->FrameVerts; i++ )
	{
		guard(Importing vertices);
		
		Mesh->Connects(i).NumVertTriangles = 0;
		Mesh->Connects(i).TriangleListOffset = Mesh->VertLinks->Num;
		for( int j=0; j<Mesh->Tris->Max; j++ )
		{
			for( int k=0; k<3; k++ )
			{
				if( Mesh->Tris(j).iVertex[k] == i )
				{
					Mesh->VertLinks->AddItem(j);
					Mesh->Connects(i).NumVertTriangles++;
				}
			}
		}
		unguard;
	}
	debugf( LOG_Info, "Made %i links", Mesh->VertLinks->Num );
	Mesh->Unlock(LOCK_ReadWrite);

	// Compute per-frame bounding volumes plus overall bounding volume.
	meshBuildBounds(Mesh);

	// Exit labels.
	Ok = 1;
	Out4: if (!Ok) {Mesh->Unlock(LOCK_ReadWrite); Mesh->Kill();}
	Out3: fclose  (DataFile);
	Out2: fclose  (AnivFile);
	Out1: GApp->EndSlowTask ();
	unguard;
}

/*-----------------------------------------------------------------------------
	Bounds.
-----------------------------------------------------------------------------*/

//
// Build bounding boxes for each animation frame of the mesh,
// and one bounding box enclosing all animation frames.
//
void FGlobalEditor::meshBuildBounds( UMesh *Mesh )
{
	guard(FGlobalEditor::meshBuildBounds);
	GApp->StatusUpdate( "Bounding mesh", 0, 0 );

	// Create bounds.
	Mesh->Bounds     = new(Mesh->GetName(),CREATE_Replace)UBounds(Mesh->AnimFrames,1);
	Mesh->LocalBound = FBoundingVolume(0);

	// Lock the mesh.
	Mesh->Lock(LOCK_ReadWrite);

	// Create per-frame bounds.
	for( int i=0; i<Mesh->AnimFrames; i++ )
	{
		FBoundingBox& MeshBound = Mesh->Bounds(i);
		MeshBound               = FBoundingBox(0);
		for( int j=0; j<Mesh->FrameVerts; j++ )
			MeshBound += Mesh->Verts( i * Mesh->FrameVerts + j ).Vector();
		Mesh->LocalBound += MeshBound;
	}

	// Display bounds.
	debugf
	(
		LOG_Info,
		"Bound %f,%f %f,%f %f,%f",
		Mesh->LocalBound.Min.X,
		Mesh->LocalBound.Max.X,
		Mesh->LocalBound.Max.Y,
		Mesh->LocalBound.Max.Y,
		Mesh->LocalBound.Min.Z,
		Mesh->LocalBound.Max.Z
	);
	Mesh->Unlock(LOCK_ReadWrite);
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
