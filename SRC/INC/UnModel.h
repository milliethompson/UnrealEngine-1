/*=============================================================================
	UnModel.h: Unreal UModel definition.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNMODEL
#define _INC_UNMODEL

/*-----------------------------------------------------------------------------
	UModel.
-----------------------------------------------------------------------------*/

//
// Flags associated with models.
//
enum EModelFlags
{
	// Basic flags.
	MF_Selected			= 0x00000001,	// Brush is selected.
	MF_Temp				= 0x00000002,	// Temporary work flag (editor).
	MF_InvalidBsp		= 0x00000004,	// Model's Bsp has been invalidated by map operations.
	MF_ShouldSnap		= 0x00000008,	// Editor: Should grid snap this world brush.
	MF_Color			= 0x00000010,	// Color contains a valid brush color.
	MF_Linked			= 0x00000020,	// Model's iLinks have been set.

	// Combinations of flags.
	MF_NoImport			= MF_Selected | MF_Temp | MF_ShouldSnap, // Clear these flags upon importing a map.
};

//
// A Csg brush operation.
// Must agree with definitions in editorGetCsgName (unedcsg.cpp)
//
enum ECsgOper
{
	CSG_Active      	= 0,	// Active brush (always brush 0), no effect on world.
	CSG_Add         	= 1, 	// Add brush to world.
	CSG_Subtract    	= 2, 	// Subtract brush from world.
	CSG_Intersect   	= 3, 	// Rebuild brush from brush/world intersection.
	CSG_Deintersect 	= 4, 	// Rebuild brush from brush/inverse world intersection.
};

//
// Model objects are used for brushes and for the level itself.
//
class UNENGINE_API UModel : public UPrimitive
{
	DECLARE_CLASS(UModel,UPrimitive,NAME_Model,NAME_UnEngine)

	// Identification.
	enum {BaseFlags = CLASS_Intrinsic};
	enum {GUID1=0,GUID2=0,GUID3=0,GUID4=0};

	// References.
	UVectors::Ptr	Vectors;	// Vector table for normals and texture axis vectors.
	UVectors::Ptr	Points;		// Point table for base points and vertices.
	UBspNodes::Ptr	Nodes;		// Bsp nodes.
	UBspSurfs::Ptr	Surfs;		// Bsp surfaces.
	UVerts::Ptr		Verts;		// Vertex table for Bsp nodes.
	UPolys::Ptr		Polys;		// FPolys.
	ULightMesh::Ptr	LightMesh;	// Lighting mesh.
	UBounds::Ptr	Bounds;		// Bsp node bounds.
	UInts::Ptr		LeafHulls;	// Bsp leaf solid hulls.

	// The following are only used by brush models, not by level models.
	FVector		Location;		// Location of origin within level.
	FRotation	Rotation;       // Rotation about (0,0,0).
	FVector		PrePivot;       // Pivot point for editing.
	FScale		Scale;			// Scaling and sheering.
	ECsgOper	CsgOper; 		// Csg operation from ECsgOper (CSG_Add,etc).
	DWORD		Color;			// Base color of brush (0-7) if MF_Color is set.
	DWORD		PolyFlags;		// Poly flags for Csg operation.
	DWORD		ModelFlags;		// Model bit flags.
	INT         RootOutside;    // Whether root node is outside.
	FVector		PostPivot;		// Post-rotation pivot.
	FScale		PostScale;		// Post-rotation scaling.
	FScale		TempScale;		// Working scale value during MF_Scaling.
	FBoundingVolume TransformedBound; // Transformed bounding volume.

	// UObject interface.
	void InitHeader();
	const char *Import(const char *Buffer, const char *BufferEnd,const char *FileType);
	void Export(FOutputDevice &Out,const char *FileType,int Indent);
	void PostLoadHeader(DWORD PostFlags);
	void SerializeHeader(FArchive &Ar)
	{
		guard(UModel::SerializeHeader);

		// UPrimitive references.
		UPrimitive::SerializeHeader(Ar);

		// UModel references.
		Ar << Vectors << Points << Nodes << Surfs << Verts << Polys << LightMesh << Bounds << LeafHulls;
		Ar << Location << Rotation << PrePivot << Scale;
		Ar << CsgOper << Color << PolyFlags << ModelFlags;
		Ar << RootOutside;
		Ar << PostPivot << PostScale << TempScale;
		Ar << TransformedBound;
		unguard;
	}

	// UPrimitive interface.
	INT PointCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		const FVector	&Location,
		DWORD			ExtraNodeFlags
	);
	INT LineCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		const FVector	&Start,
		const FVector	&End,
		DWORD ExtraNodeFlags
	);
	INT BoxPointCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		const FVector   &Point,
		FLOAT           Radius,
		FLOAT           Height,
		DWORD           ExtraNodeFlags
	);
	INT BoxLineCheck
	(
		FCheckResult	&Result,
		AActor			*Owner,
		const FVector   &Start,
		const FVector   &End,
		FLOAT           Radius,
		FLOAT           Height,
		DWORD           ExtraNodeFlags
	);

	// UModel interface.
	UModel						(int Editable, int RootOutside);
	void    AllocDatabases		(BOOL AllocPolys);
	INT     Lock				(DWORD LockType);
	void    Unlock				(DWORD OldLockType);
	void	Init				(int InitPositionRotScale);
	FLOAT	BuildCoords			(FModelCoords *Coords,FModelCoords *Uncoords);
	void	BuildBound			(int Transformed);
	void	Transform			();
	void	SetPivotPoint		(FVector *PivotLocation,int SnapPivotToGrid);
	void	CopyPosRotScaleFrom	(UModel *OtherModel);
	void	EmptyModel			(int EmptySurfInfo,int EmptyPolys);
	void	ShrinkModel			();

	// UModel collision functions.
	typedef void (*PLANE_FILTER_CALLBACK )(UModel *Model, INDEX iNode, int Param);
	typedef void (*SPHERE_FILTER_CALLBACK)(UModel *Model, INDEX iNode, int IsBack, int Outside, int Param);
	INT PointLeaf
	(
		const FVector	&Location,
		DWORD			ExtraNodeFlags
	) const;
	BYTE PointZone
	(
		const FVector	&Location
	)  const;
	FLOAT FindNearestVertex
	(
		const FVector	&SourcePoint,
		FVector			&DestPoint,
		FLOAT			MinRadius,
		INDEX			&pVertex
	) const;
	void PlaneFilter
	(
		const FVector	&Location,
		FLOAT			Radius, 
		PLANE_FILTER_CALLBACK Callback, 
		DWORD			SkipNodeFlags,
		int				Param
	);
	void PrecomputeSphereFilter
	(
		const FVector	&Sphere
	);
	FLightMeshIndex *GetLightMeshIndex( INDEX iSurf )
	{
		guard(UModel::GetLightMeshIndex);
		if( iSurf == INDEX_NONE ) return NULL;
		FBspSurf &Surf = Surfs(iSurf);
		if( Surf.iLightMesh==INDEX_NONE || !LightMesh ) return NULL;
		return &LightMesh->Element(Surf.iLightMesh);
		unguard;
	}
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNMODEL
