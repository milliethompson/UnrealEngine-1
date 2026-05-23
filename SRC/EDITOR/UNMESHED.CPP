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

/*
James' notes:

Here is the format for the 3d animation:
data**.3d
HEADER
  word #polygons, word #verticies
  word rotation about direction vector (0-3600)
  word current animation #
  dword 0,0,0  direction normal (16bit fixed int)
  dword 10000h,0,0,0    Scale(fixed int), unused,unused,unused
 
(note:  the directional vector and rotation about it was for compatibility
with 3d studio.  I will change this in the future)
 
MAIN DATA (poly info list)
  dw 0,1,2   ;offset in vertex list of 3 verticies of current frame
             ; of animation
  db 0       ;poly type 0=textured, 1=flat, 2 translucent, 3=0masked
                        (only textured and masked used)
  db 0       ;polygon color (only for flat and gouraud poly)
  db 1,1, 2,2, 3,3   ;texture map coords
  db 0               ;#of 256x256 source texture
  db 0               ;unused
 
at end of poly list is a bunch of bytes for each vertex indicating the
gouraud value for that vertex.  (this list may or may not exist on the
current meshes cuz I haven't used gouraud much yet)
 
VERTEX DATA
word number of frames of animation
word side of a frame animation
 
each vertex is stored in 4bytes
x=first 11 bits
y=next 11 bits
z=last 10 bits
*/

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
	FJSDataHeader	JSDataHdr;			// James' DATA*.3D file header.
	FJSAnivHeader	JSAnivHdr;			// James' ANIV*.3D file header.
	int				i,k;
	int				Ok = 0;

	debugf(LOG_Info,"Importing %s",MeshName);
	GApp->BeginSlowTask ("Importing mesh",1,0);
	GApp->StatusUpdate  ("Reading files",0,0);

	// Open James' animation vertex file and read header.
	AnivFile = fopen( AnivFname, "rb" );
	if (AnivFile==NULL) {debugf (LOG_Info,"Error opening file %s",AnivFname); goto Out1;};
	if (fread (&JSAnivHdr,sizeof(FJSAnivHeader),1,AnivFile)!=1) {debugf (LOG_Info,"Error reading %s",AnivFname); goto Out2;};

	// Open James' mesh data file and read header.
	DataFile = fopen (DataFname,"rb");
	if (DataFile==NULL) {debugf (LOG_Info,"Error opening file %s",DataFname); goto Out2;};
	if (fread (&JSDataHdr,sizeof(FJSDataHeader),1,DataFile)!=1) {debugf (LOG_Info,"Error reading %s",DataFile); goto Out3;};

	//	Allocate mesh object and set its header.
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
	debugf (LOG_Info,"Importing triangles");
	fseek (DataFile,12,SEEK_CUR); // Skip empty stuff.

	// Import all triangles.
	for( i=0; i<Mesh->Tris->Max; i++ )
	{
		guard(Importing triangles);
		FMeshTri &Tri = Mesh->Tris(i);

		if (!(i&15)) GApp->StatusUpdate( "Importing Triangles",i,Mesh->Tris->Max );
		if (fread (&Tri,sizeof(FMeshTri),1,DataFile)!=1) {debugf (LOG_Info,"Error processing %s",DataFile); goto Out4;};

		Tri.Flags = 0;
		unguard;
	}

	// Import mesh vertices.
	debugf (LOG_Info,"Importing vertices");
	for( i=0; i<Mesh->AnimFrames; i++ )
	{
		guard(Importing animation frames);
		FMeshVert *TempVert = &Mesh->Verts(i * Mesh->FrameVerts);
		if( !(i&3) ) GApp->StatusUpdate( "Importing Vertices", i, Mesh->AnimFrames );
		if( fread( TempVert, sizeof(FMeshVert), Mesh->FrameVerts, AnivFile ) != (size_t)Mesh->FrameVerts )
		{
			debugf (LOG_Info,"Vertex error in %s",AnivFname);
			goto Out4;
		}
		fseek(AnivFile,JSAnivHdr.FrameSize - Mesh->FrameVerts*sizeof(FMeshVert),SEEK_CUR); // Skip empty stuff
		unguard;
	}

	// Build list of triangles per vertex.
	for( i=0; i<Mesh->FrameVerts; i++ )
	{
		guard(Importing vertices);
		if (i&1) GApp->StatusUpdate("Linking mesh",i,Mesh->FrameVerts);
		
		Mesh->Connects(i).NumVertTriangles = 0;
		Mesh->Connects(i).TriangleListOffset = Mesh->VertLinks->Num;
		for( WORD j=0; j<Mesh->Tris->Max; j++ )
		{
			for( k=0; k<3; k++ )
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
	debugf (LOG_Info,"Made %i links",Mesh->VertLinks->Num);
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

#define UPDATE_PACKED_MIN(min,v)\
{\
	if (((FLOAT)(v)->X) < ((min)->X)) (min)->X = (FLOAT)(v)->X;\
	if (((FLOAT)(v)->Y) < ((min)->Y)) (min)->Y = (FLOAT)(v)->Y;\
	if (((FLOAT)(v)->Z) < ((min)->Z)) (min)->Z = (FLOAT)(v)->Z;\
};

#define UPDATE_PACKED_MAX(max,v)\
{\
	if (((FLOAT)(v)->X) > ((max)->X)) (max)->X = (FLOAT)(v)->X;\
	if (((FLOAT)(v)->Y) > ((max)->Y)) (max)->Y = (FLOAT)(v)->Y;\
	if (((FLOAT)(v)->Z) > ((max)->Z)) (max)->Z = (FLOAT)(v)->Z;\
};

//
// Build bounding boxes for each animation frame of the mesh,
// and one bounding box enclosing all animation frames.
//
void FGlobalEditor::meshBuildBounds( UMesh *Mesh )
{
	guard(FGlobalEditor::meshBuildBounds);

	// Create bounds.
	Mesh->Bounds = new(Mesh->GetName(),CREATE_Replace)UBounds(Mesh->AnimFrames,1);

	// Lock the mesh.
	Mesh->Lock(LOCK_ReadWrite);

	Mesh->LocalBound.Min = GMath.VectorMax;
	Mesh->LocalBound.Max = GMath.VectorMin;
	for( int i=0; i<Mesh->AnimFrames; i++ )
	{
		FBoundingRect& MeshBound = Mesh->Bounds(i);

		if( i & 1 ) GApp->StatusUpdate("Bounding mesh",i,Mesh->FrameVerts);

		MeshBound.Min = GMath.VectorMax;
		MeshBound.Max = GMath.VectorMin;

		int Base = i * Mesh->FrameVerts;
		for( int j=0; j<Mesh->FrameVerts; j++ )
		{
			UPDATE_PACKED_MIN(&MeshBound.Min,&Mesh->Verts(Base + j));
			UPDATE_PACKED_MAX(&MeshBound.Max,&Mesh->Verts(Base + j));
		}
		UPDATE_PACKED_MIN(&Mesh->LocalBound.Min,&MeshBound.Min);
		UPDATE_PACKED_MAX(&Mesh->LocalBound.Max,&MeshBound.Max);
	}
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
