/*=============================================================================
	UnMesh.h: Unreal mesh objects.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNMESH
#define _INC_UNMESH

/*-----------------------------------------------------------------------------
	UMeshVert.
-----------------------------------------------------------------------------*/

// Packed mesh vertex point for skinned meshes.
struct FMeshVert
{
	union
	{
		struct {INT X:11; INT Y:11; INT Z:10;};
		struct {DWORD D;};
	};

	// Serializer.
	friend FArchive &operator<< (FArchive &Ar, FMeshVert &V)
		{return Ar << V.D;}
};

// A database of mesh vertex points.
class UNENGINE_API UMeshVerts : public UDatabase
{
	DECLARE_DB_CLASS(UMeshVerts,UDatabase,FMeshVert,NAME_MeshVerts,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Swappable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UMeshVerts(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UMeshTris.
-----------------------------------------------------------------------------*/

// Texture coordinates associated with a vertex and one or more mesh triangles.
// All triangles sharing a vertex do not necessarily have the same texture
// coordinates at the vertex.
struct FMeshUV
{
	BYTE U;
	BYTE V;
	friend FArchive &operator<< (FArchive &Ar, FMeshUV &M)
		{return Ar << M.U << M.V;}
};

// Flags associated with a mesh triangle.
enum EMeshTriFlags
{
	MT_Textured		= 0,		// Texture map the triangle.
	MT_Flat			= 1,		// Draw it black and flat-shaded.
	MT_Transparent	= 2,		// Draw it transparently.
	MT_Masked		= 3,		// Draw it with masking.
};

// One triangular polygon in a mesh, which references three vertices,
// and various drawing/texturing information.
struct FMeshTri
{
	WORD		iVertex[3];		// Vertex indices.
	BYTE		Type;			// James' mesh type.
	BYTE		Color;			// Color for flat and Gouraud shaded.
	FMeshUV		Tex[3];			// Texture UV coordinates.
	BYTE		TextureNum;		// Source texture offset.
	BYTE		Flags;			// Unreal mesh flags (currently unused).
	friend FArchive &operator<< (FArchive &Ar, FMeshTri &T)
	{
		Ar << T.iVertex[0] << T.iVertex[1] << T.iVertex[2];
		Ar << T.Type;
		Ar << T.Color;
		Ar << T.Tex[0] << T.Tex[1] << T.Tex[2];
		Ar << T.TextureNum;
		Ar << T.Flags;
		return Ar;
	}
};

// A database of mesh triangles.
class UNENGINE_API UMeshTris : public UDatabase
{
	DECLARE_DB_CLASS(UMeshTris,UDatabase,FMeshTri,NAME_MeshTris,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Swappable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UMeshTris(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UMeshAnimSeqs.
-----------------------------------------------------------------------------*/

// Information about one animation sequence associated with a mesh,
// a group of contiguous frames.
struct FMeshAnimSeq
{
	FName	Name;			// Sequence's name.
	WORD	StartFrame;		// Starting frame number.
	WORD	NumFrames;		// Number of frames in sequence.
	WORD    StartNotify;    // Starting notify number.
	WORD    NumNotifys;     // Number of notifys.
	FLOAT	Rate;			// Playback rate in frames per second.
	friend FArchive &operator<< (FArchive &Ar, FMeshAnimSeq &A)
		{return Ar << A.Name << A.StartFrame << A.NumFrames << A.StartNotify << A.NumNotifys << A.Rate;}
	void Init()
		{Name=NAME_None; StartFrame=0; NumFrames=0; StartNotify=0; NumNotifys=0; Rate=30.0;}
};

// A database of mesh animation sequences.
class UNENGINE_API UMeshAnimSeqs : public UDatabase
{
	DECLARE_DB_CLASS(UMeshAnimSeqs,UDatabase,FMeshAnimSeq,NAME_MeshAnimSeqs,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UMeshAnimSeqs(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UMeshAnimNotifys.
-----------------------------------------------------------------------------*/

// An actor notification event associated with an animation sequence.
struct FMeshAnimNotify
{
	FLOAT	Time;			// Time to occur, 0.0-1.0.
	FName	Function;		// Name of the actor function to call.
	friend FArchive &operator<< (FArchive &Ar, FMeshAnimNotify &N)
		{return Ar << N.Time << N.Function;}
	void Init()
		{Time=0.0; Function=NAME_None;}
};

// A database of animation sequences notifications.
class UNENGINE_API UMeshAnimNotifys : public UDatabase
{
	DECLARE_DB_CLASS(UMeshAnimNotifys,UDatabase,FMeshAnimNotify,NAME_MeshAnimNotifys,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UMeshAnimNotifys(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UMeshVertConnect
-----------------------------------------------------------------------------*/

// Says which triangles a particular mesh vertex is associated with.
// Precomputed so that mesh triangles can be shaded with Gouraud-style
// shared, interpolated normal shading.
struct FMeshVertConnect
{
	INT	NumVertTriangles;
	INT	TriangleListOffset;
	friend FArchive &operator<< (FArchive &Ar, FMeshVertConnect &C)
		{return Ar << C.NumVertTriangles << C.TriangleListOffset;}
};

// A database of mesh vertex connectivity information.
class UNENGINE_API UMeshVertConnects : public UDatabase
{
	DECLARE_DB_CLASS(UMeshVertConnects,UDatabase,FMeshVertConnect,NAME_MeshVertConnects,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_Swappable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constructors.
	UMeshVertConnects(int InNum, int InOccupy=0) : UDatabase(InNum,InOccupy) {}
};

/*-----------------------------------------------------------------------------
	UMesh object class.
-----------------------------------------------------------------------------*/

//
// A mesh, completely describing a 3D object (creature, weapon, etc) and
// its animation sequences.  Does not reference textures.
//
class UNENGINE_API UMesh : public UPrimitive
{
	DECLARE_CLASS(UMesh,UPrimitive,NAME_Mesh,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic | CLASS_ScriptWritable};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// Constants.
	enum {NUM_TEXTURES=16};

	// Objects.
	UMeshVerts::Ptr			Verts;
	UMeshTris::Ptr			Tris;
	UMeshAnimSeqs::Ptr		AnimSeqs;
	UMeshVertConnects::Ptr	Connects;
	UBounds::Ptr			Bounds;
	UWords::Ptr				VertLinks;
	TArray<UTexture *>::Ptr Textures;
	UMeshAnimNotifys::Ptr	Notifys;

	// Counts.
	INT						FrameVerts;
	INT						AnimFrames;

	// Render info.
	DWORD					AndFlags;
	DWORD					OrFlags;

	// Scaling.
	FVector					Scale;		// Mesh scaling.
	FVector 				Origin;		// Origin in original coordinate system.
	FRotation				RotOrigin;	// Amount to rotate when importing (mostly for yawing).

	// Editing info.
	INT						CurPoly;	// Index of selected polygon.
	INT						CurVertex;	// Index of selected vertex.

	// UObject interface.
	void InitHeader();
	void PostLoadHeader(DWORD PostFlags);
	void SerializeHeader(FArchive &Ar)
	{
		guard(UMesh::SerializeHeader);

		// Serialize parent.
		UPrimitive::SerializeHeader(Ar);

		// Serialize this.
		Ar << AR_OBJECT(Verts) << AR_OBJECT(Tris) << AR_OBJECT(AnimSeqs);
		Ar << AR_OBJECT(Connects) << AR_OBJECT(Bounds) << AR_OBJECT(VertLinks) << AR_OBJECT(Textures) << AR_OBJECT(Notifys);
		Ar << FrameVerts << AnimFrames;
		Ar << AndFlags << OrFlags;
		Ar << Scale << Origin << RotOrigin;
		Ar << CurPoly << CurVertex;
		
		unguard;
	}
	INT Lock(DWORD New)
	{
		guard(UMesh::Lock);
		Verts     ->Lock(New);
		Tris      ->Lock(New);
		AnimSeqs  ->Lock(New);
		Connects  ->Lock(New);
		Bounds    ->Lock(New);
		VertLinks ->Lock(New);
		Textures  ->Lock(New);
		Notifys   ->Lock(New);
		return UPrimitive::Lock(New);
		unguard;
	}
	void Unlock(DWORD OldLockType)
	{
		guard(UMesh::Unlock);
		UPrimitive::Unlock(OldLockType);
		Verts     ->Unlock(OldLockType);
		Tris      ->Unlock(OldLockType);
		AnimSeqs  ->Unlock(OldLockType);
		Connects  ->Unlock(OldLockType);
		Bounds    ->Unlock(OldLockType);
		VertLinks ->Unlock(OldLockType);
		Textures  ->Unlock(OldLockType);
		Notifys   ->Unlock(OldLockType);
		unguard;
	}

	// UPrimitive interface.
	FBoundingRect GetBoundingRect( AActor *Owner );

	// UMesh interface.
	UMesh( int NumPolys, int NumVerts, int NumFrames, int NumTex );
	FMeshAnimSeq* GetAnimSeq( FName SeqName )
	{
		guardSlow(UMesh::GetAnimSeq);
		FMeshAnimSeq* Result = NULL;
		Lock(LOCK_Read);
		if( AnimSeqs )
		{
			for( int i=0; i<AnimSeqs->Num; i++ )
			{
				if( SeqName == AnimSeqs(i).Name )
				{
					Result = &AnimSeqs(i);
					break;
				}
			}
		}
		Unlock(LOCK_Read);
		return Result;
		unguardSlow;
	}
	BYTE GetFrame( class FTransSample *Verts, UCamera *Camera, AActor *Owner );
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNMESH
