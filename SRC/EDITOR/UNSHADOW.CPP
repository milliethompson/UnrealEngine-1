/*=============================================================================
	UnLight.cpp: Bsp light mesh illumination builder code

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*---------------------------------------------------------------------------------------
   Globals
---------------------------------------------------------------------------------------*/

// The following causes moving brushes to be raytraced in their
// properly specified positions. Setting it to 1 currently exposes
// an internal bug that may cause crashing during raytracing.
#define RAYTRACE_BRUSHES_PROPERLY 1

enum {UNITS_PER_METER = 32}; /* Number of world units per meter. */
#define MESH_BORDER 1.0      /* Border surrounding mesh */

// Snap V to down to Grid.
inline FLOAT SnapTo( FLOAT V, FLOAT Grid )
{
	return floor( V / Grid ) * Grid;
}

//
// Class used for storing all globally-accessible light generation parameters:
//
class FMeshIlluminator
{
public:

	// Variables.
	UCamera			*Camera;
	ULevel::Ptr		Level;
	ULightMesh::Ptr	LightMesh;
	UVectors::Ptr	Vectors;
	UVectors::Ptr	Points;
	int				NumLights,PolysLit,ActivePolys,RaysTraced,Pairs,Oversample;

	// Functions.
	void	AllocateLightCamera( ULevel *Level );
	void	FreeLightCamera();
	void	SetCameraView( int ViewNum, FVector *Location );
	void	ComputeLightVisibility( AActor *Actor );
	int		ComputeAllLightVisibility(int Selected);
	void	LightBspSurf( INDEX iSurf );
	void	LightAllSurfs();
	void	BuildSurfList( INDEX iNode );
	void	InitLightMeshIndices();
};

/*---------------------------------------------------------------------------------------
   Functions for managing temporary (non-windowed) cameras
---------------------------------------------------------------------------------------*/

//
// Allocate a temporary camera for lighting purposes,
//
void FMeshIlluminator::AllocateLightCamera(ULevel *ThisLevel)
{
	guard(FMeshIlluminator::AllocateLightCamera);

	Camera = new("Raytracer",CREATE_Replace)UCamera(ThisLevel);
	Camera->X					= 128;
	Camera->Y					= 128;
	Camera->Actor->ShowFlags	= 0;
	Camera->Actor->RendMap		= REN_PlainTex;

	Camera->OpenWindow(0,1); // Open a temporary (non-visible) camera
	Camera->Lock(LOCK_ReadWrite);
	Level = Camera->Level;

	unguard;
}

void FMeshIlluminator::FreeLightCamera()
{				
	guard(FMeshIlluminator::FreeLightCamera);

	Camera->Unlock(LOCK_ReadWrite,0);
	Camera->Kill();

	unguard;
}

void FMeshIlluminator::SetCameraView(int ViewNum,FVector *Location)
{
	guard(FMeshIlluminator::SetCameraView);
	Camera->Actor->ViewRotation = *GMath.Views[ViewNum]; // Up/down/north/south/east/west.
	Camera->Actor->Location = *Location;
	Camera->Unlock(LOCK_ReadWrite,0);
	Camera->Lock(LOCK_ReadWrite);
	unguard;
}

/*---------------------------------------------------------------------------------------
   Light visibility computation
---------------------------------------------------------------------------------------*/

//
// Compute per-polygon visibility of one light:
//
void FMeshIlluminator::ComputeLightVisibility( AActor *Actor )
{
	guard(FMeshIlluminator::ComputeLightVisibility);

	// Mark this actor as undeletable, so it can't be deleted at playtime, causing a
	// dangling light pointer.
	Actor->bNoDelete = 1;

	// Render six span occlusion frames looking up/down/n/s/e/w and tag all
	// visibly polygons:
	for( int i=0; i<6; i++ )
	{
		FMemMark MemMark(GMem);
		FMemMark DynMemMark(GDynMem);

		SetCameraView( i, &Actor->Location );
		GEdRend->InitTransforms( Level->Model );
		for( FBspDrawList *DrawList = GEdRend->OccludeBsp(Camera,NULL); DrawList != NULL; DrawList = DrawList->Next )
		{
			FBspSurf &Poly = Level->Model->Surfs(DrawList->iSurf);
			FBspNode &Node = Level->Model->Nodes(DrawList->iNode);
			if( Poly.iLightMesh!=INDEX_NONE && 
				(Actor->bSpecialLit ? (Poly.PolyFlags&PF_SpecialLit) : !(Poly.PolyFlags&PF_SpecialLit)))
			{
				if( Actor->LightRadius==0 || Abs(Node.Plane.PlaneDot(Actor->Location))<= Actor->WorldLightRadius() )
				{
					FLightMeshIndex* Index = &LightMesh(Poly.iLightMesh);
					if( Index->NumStaticLights < Index->MAX_POLY_LIGHTS )
					{
						int Duplicate=0;
						for( INDEX j=0; j<Index->NumStaticLights; j++ )
						{
							if( Index->LightActor[j] == Actor )
								Duplicate=1;
						}
						if( !Duplicate )
						{
							Pairs++;
							Index->LightActor[Index->NumStaticLights++] = Actor;
						}
					}
				}
			}
		}
		GEdRend->ExitTransforms();
		MemMark.Pop();
		DynMemMark.Pop();
	}
	unguard;
}

//
// Compute visibility between each light in the world and each polygon.
// Returns number of lights to be applied.
//
int FMeshIlluminator::ComputeAllLightVisibility( int Selected )
{
	guard(FMeshIlluminator::ComputeAllLightVisibility);
	int n=0;
	
	SQWORD Time = GApp->MicrosecondTime();
	for( int i=0; i<Level->Num; i++ )
	{
		if( (i&15)==0 )
			GApp->StatusUpdate( "Computing visibility", i, Level->Num );

		AActor *Actor = Level(i);
		if( Actor && Actor->LightType!=LT_None && Actor->bStatic )
		{
			int DoLight = (Actor->bSelected || !Selected);
			if( !DoLight && Actor->Zone!=NULL )
			{
				DoLight = Actor->Zone->bSelected;
			}
			if( DoLight )
			{
				n++;
				ComputeLightVisibility( Actor );
			}
		}
	}
	Time = GApp->MicrosecondTime() - Time;
	debugf( "Occluded %i views in %f sec (%f msec per light)", n*6, (float)Time/1000000.0, (float)Time/1000.0/n/6 );
	return n;
	unguard;
}

/*---------------------------------------------------------------------------------------
   Polygon lighting
---------------------------------------------------------------------------------------*/

//
// Apply all lights to one poly, generating its lighting mesh and updating
// the tables:
//
void FMeshIlluminator::LightBspSurf( INDEX iSurf )
{
	guard(FMeshIlluminator::LightBspSurf );

	FBspSurf &Surf = Level->Model->Surfs(iSurf);
	if( Surf.iLightMesh==INDEX_NONE )
		appError( "Invalid lightmesh" );

	FLightMeshIndex		*Index	= &LightMesh(Surf.iLightMesh);
	UModel				*Brush	= NULL;

	if( iSurf >= Level->Model->Surfs->Num )
	{
		checkState(Surf.Actor!=NULL);
		Brush = Surf.Actor->Brush;
		checkState(Brush!=NULL);

#if RAYTRACE_BRUSHES_PROPERLY
		PBoolean Result(0);
		Surf.Actor->Process( NAME_RaytraceBrush, &Result);
		if( !Result.bBoolean )
		{
			Surf.iLightMesh = INDEX_NONE;
			return;
		}
#endif
	}

	FVector		&Base     =  Points  (Surf.pBase);
	FVector		&Normal   =  Vectors (Surf.vNormal);
	FVector		TextureU  =  Vectors (Surf.vTextureU);
	FVector		TextureV  =  Vectors (Surf.vTextureV);

	FLOAT		MinU	  = +10000000.0;
	FLOAT		MinV	  = +10000000.0;
	FLOAT		MaxU	  = -10000000.0;
	FLOAT		MaxV	  = -10000000.0;

	if( iSurf < Level->Model->Surfs->Num )
	{
		// Find extent of static world surface from all of the Bsp polygons
		// that use the surface.
		for( INDEX i=0; i<Level->Model->Nodes->Num; i++ )
		{
			FBspNode &Node = Level->Model->Nodes(i);
			if( (Node.iSurf == iSurf) && (Node.NumVertices>0) )
			{
				FVert *VertPool = &Level->Model->Verts(Node.iVertPool);
				for( BYTE B=0; B < Node.NumVertices; B++ )
				{
					FVector Vertex	= Points(VertPool[B].pVertex) - Base;
					FLOAT	U		= (Vertex | TextureU) / 65536.0;
					FLOAT	V		= (Vertex | TextureV) / 65536.0;

					if (U < MinU) MinU = U; if (U > MaxU) MaxU = U;
					if (V < MinV) MinV = V; if (V > MaxV) MaxV = V;
				}
			}
		}
	}
	else
	{
		// Find extent of moving brush polygon from the original EdPoly that
		// generated the surface.
		Brush->Lock(LOCK_ReadWrite);
		for( int i=0; i<Brush->Polys->Num; i++ )
		{
			FPoly *Poly = &Brush->Polys(i);
			if( Poly->iLink == iSurf )
			{
				for( int j=0; j<Poly->NumVertices; j++ )
				{
					// Find extent in untransformed brush space.
					FVector Vertex	= Poly->Vertex[j] - Poly->Base;
					FLOAT	U		= (Vertex | Poly->TextureU) / 65536.0;
					FLOAT	V		= (Vertex | Poly->TextureV) / 65536.0;

					MinU=Min(U,MinU); MaxU=Max(U,MaxU);
					MinV=Min(V,MinV); MaxV=Max(V,MaxV);
				}
				Poly->iBrushPoly = Surf.iLightMesh;
				break;
			}
		}
		checkState(i<Brush->Polys->Num);
		Brush->Unlock(LOCK_ReadWrite);
	}

	// Compute mesh density.
	DWORD PolyFlags = Level->Model->Surfs(iSurf).PolyFlags;
	if( PolyFlags & PF_HighShadowDetail )
	{
		Index->MeshSpacing   = 16;
		Index->MeshShift     = 4;
	}
	else if( PolyFlags & PF_LowShadowDetail )
	{
		Index->MeshSpacing   = 64;
		Index->MeshShift     = 6;
		}
	else
	{
		Index->MeshSpacing   = 32;
		Index->MeshShift     = 5;
	}

	// Set light mesh index values, forcing to lattice for coplanar mesh alignment.
	for( ;; )
	{
		MinU = SnapTo( MinU, Index->MeshSpacing ) - MESH_BORDER*Index->MeshSpacing;
		MinV = SnapTo( MinV, Index->MeshSpacing ) - MESH_BORDER*Index->MeshSpacing;

		MaxU = SnapTo( MaxU, Index->MeshSpacing ) + MESH_BORDER*Index->MeshSpacing + 2*Index->MeshSpacing;
		MaxV = SnapTo( MaxV, Index->MeshSpacing ) + MESH_BORDER*Index->MeshSpacing + 2*Index->MeshSpacing;

		Index->TextureUStart = MinU * 65536.0;
		Index->TextureVStart = MinV * 65536.0;

		Index->MeshUSize = Max( (int)(MaxU-MinU)/Index->MeshSpacing, 3 );
		Index->MeshVSize = Max( (int)(MaxV-MinV)/Index->MeshSpacing, 2 );

		Index->MeshUBits = FLogTwo(Index->MeshUSize);
		Index->MeshVBits = FLogTwo(Index->MeshVSize);

		if
		(   (Index->MeshUSize <= 256) 
		&&	(Index->MeshVSize <= 256)
		&&	(Index->MeshUSize * Index->MeshVSize <= 128*128))
			break;
		Index->MeshShift++;
		Index->MeshSpacing *= 2;
	}

	// Get some space for this shadow map.
	Index->DataOffset = LightMesh->Bits->Add
	(
		Index->NumStaticLights * ((Index->MeshUSize+7)>>3) * Index->MeshVSize
	);

	// Calculate new base point by moving polygon's base point forward by 4 units.
	FVector		NewBase			= Points(Surf.pBase) + Normal * 4.0;
	FLOAT		FMeshSpacing	= (FLOAT)Index->MeshSpacing;
	int			ByteSize	    = ((Index->MeshUSize+7)>>3) * Index->MeshVSize;

	// Calculate inverse U & V axes which map texels onto pixels.
	FCoords TexCoords    = FCoords( FVector(0,0,0), TextureU, TextureV, Normal ).Inverse().Transpose();
	FVector	InverseUAxis = TexCoords.XAxis * 65536.0;
	FVector InverseVAxis = TexCoords.YAxis * 65536.0;

	// Raytrace each lightsource.
	for( int i=0; i<Index->NumStaticLights; i++ )
	{
		BYTE	*Data	= &LightMesh->Bits(Index->DataOffset + i * ByteSize);
		AActor  *Actor  = Index->LightActor[i];
		FVector *Light	= &Actor->Location;

		// Go through all lattice points and build lighting mesh.
		// U,V units are texels.
		FLOAT	U			= (FLOAT)Unfix(Index->TextureUStart);
		FLOAT	V			= (FLOAT)Unfix(Index->TextureVStart);
		FVector	Vertex0		= NewBase + InverseUAxis*U + InverseVAxis*V;
		FVector VertexDU	= InverseUAxis * FMeshSpacing;
		FVector VertexDV	= InverseVAxis * FMeshSpacing;

		FCheckResult Hit(0.0);
		for( int VCounter = 0; VCounter < Index->MeshVSize; VCounter++ )
		{
			FVector Vertex = Vertex0;
			for( int UCounter = 0; UCounter < Index->MeshUSize; UCounter+=8 )
			{
				BYTE B = 0;
				BYTE M = 1;
				for( int ByteUCounter=0; ByteUCounter < 8; ByteUCounter++ )
				{
					if( Level->Model->LineCheck( Hit, NULL, Vertex, *Light, FVector(0,0,0), NF_NotVisBlocking ) )
					//FCheckResult Hit; if( Level->Trace( Hit, Actor, Vertex, *Light, TRACE_All ) )
						B |= M;
					M = M << 1;
					RaysTraced++;
					Vertex += VertexDU;
				}
				*Data++ = B;
			}
			Vertex0 += VertexDV;
		}
	}
#if RAYTRACE_BRUSHES_PROPERLY
	if( iSurf >= Level->Model->Surfs->Num )
		Surf.Actor->Process( NAME_RaytraceWorld, NULL );
#endif
	unguard;
}

void FMeshIlluminator::LightAllSurfs()
{
	guard(FMeshIlluminator::LightAllSurfs);

	int n=0,c=0;

	// Count raytraceable surfs.
	for( INDEX i=0; i<Level->Model->Surfs->Max; i++ )
		n += (Level->Model->Surfs(i).iLightMesh != INDEX_NONE);

	// Raytrace each surf.
	for( i=0; i<Level->Model->Surfs->Max; i++ )
	{
		if( Level->Model->Surfs(i).iLightMesh != INDEX_NONE )
		{
			GApp->StatusUpdate ("Raytracing",c++,n);
			LightBspSurf(i);
		}
	}
	unguard;
}

/*---------------------------------------------------------------------------------------
   Index building
---------------------------------------------------------------------------------------*/

//
// Recursively go through the Bsp nodes and build a list of active Bsp surfs,
// allocating their light mesh indices.
//
void FMeshIlluminator::BuildSurfList( INDEX iNode )
{
	guard(FMeshIlluminator::BuildSurfList);

	FBspNode	&Node = Level->Model->Nodes(iNode);
	INDEX		iSurf = Node.iSurf;
	FBspSurf	&Surf = Level->Model->Surfs(iSurf);

	if
	(	(
			(Node.NumVertices  >  0) && 
			(iSurf             != INDEX_NONE) &&
			(!(Surf.PolyFlags  &  PF_NoShadows) )
		)
	||	GBrushTracker.SurfIsDynamic(iSurf) )
	{
		if( Surf.iLightMesh == INDEX_NONE )
		{
			// Create a light mesh for this surface.
			Surf.iLightMesh = LightMesh->Add();
			PolysLit++;
		}
	}
	if( Node.iFront != INDEX_NONE ) BuildSurfList(Node.iFront);
	if( Node.iBack  != INDEX_NONE ) BuildSurfList(Node.iBack);
	if( Node.iPlane != INDEX_NONE ) BuildSurfList(Node.iPlane);
	unguard;
}

//
// Initialize all light mesh indices
//
void FMeshIlluminator::InitLightMeshIndices()
{
	guard(FMeshIlluminator::InitLightMeshIndices);

	for( int i=0; i<LightMesh->Num; i++ )
	{
		LightMesh(i).NumStaticLights  = 0;
		LightMesh(i).NumDynamicLights = 0;
	}
	unguard;
}

/*---------------------------------------------------------------------------------------
   High-level lighting routine
---------------------------------------------------------------------------------------*/

void FGlobalEditor::shadowIlluminateBsp(ULevel *Level, int Selected)
{
	guard(FGlobalEditor::shadowIlluminateBsp);
	FMeshIlluminator Illum;

	// Invalidate texture/illumination cache.
	GCache.Flush();
	GTrans->Reset("Rebuilding lighting");

	GApp->BeginSlowTask("Raytracing",1,0);

	guard(Init);
	GBrushTracker.Init(Level);
	Illum.AllocateLightCamera(Level);
	unguard;

	guard(Setup);
	Illum.Vectors	= Illum.Level->Model->Vectors;
	Illum.Points	= Illum.Level->Model->Points;
	Illum.LightMesh = Illum.Level->Model->LightMesh = (ULightMesh*)NULL;
	unguard;

	if( Illum.Level->Model->Nodes->Num != 0 )
	{
		// Allocate a new lighting mesh.
		guard(Alloc);
		Illum.LightMesh = new(Level->GetName(),CREATE_Replace)ULightMesh(0,0);
		Illum.LightMesh->Lock(LOCK_ReadWrite);
		unguard;

		Illum.PolysLit			= 0;
		Illum.RaysTraced		= 0;
		Illum.ActivePolys		= 0;
		Illum.Pairs				= 0;

		// Clear all surface light mesh indices.
		guard(ClearSurfs);
		for( INDEX i=0; i<Illum.Level->Model->Surfs->Max; i++ )
			Illum.Level->Model->Surfs(i).iLightMesh = INDEX_NONE;
		unguard;

		// Tell all actors that we're about to raytrace the world.
		// This enables movable brushes to set their positions for raytracing.
		guard(PreRaytrace);
		for( INDEX i=0; i<Illum.Level->Num; i++ )
		{
			if( (i&15)==0 )
				GApp->StatusUpdate( "Allocating meshes", i, Illum.Level->Num );
			
			AActor *Actor = Illum.Camera->Level(i);
			if( Actor )
			{
				Actor->Process( NAME_PreRaytrace, NULL );
				Actor->Process( NAME_RaytraceWorld, NULL );

				if( Actor->Brush && Actor->Brush->Polys )
				{
					Actor->Brush->Lock(LOCK_ReadWrite);
					// Clean out all brush attached lightmesh indices.
					for( int j=0; j<Actor->Brush->Polys->Num; j++ )
						Actor->Brush->Polys(j).iBrushPoly = INDEX_NONE;
					Actor->Brush->Unlock(LOCK_ReadWrite);
				}
			}
		}
		unguard;

		// Recursively update list of polys with min/max U,V's and visibility flags.
		Illum.BuildSurfList(0);
		Illum.InitLightMeshIndices();

		// Compute light visibility and update index with it.
		Illum.NumLights = Illum.ComputeAllLightVisibility(Selected);

		// Apply light to each polygon.
		Illum.LightAllSurfs();

		// Tell all actors that we're done raytracing the world.
		guard(PostRaytrace);
		for( INDEX i=0; i<Illum.Level->Num; i++ )
		{
			AActor *Actor = Illum.Level(i);
			if( Actor )
			{
				Actor->bDynamicLight = 0;
				Actor->Process( NAME_PostRaytrace, NULL );
			}
		}
		unguard;
		debugf (LOG_Info,"%i Lights, %i Polys, %i Pairs, %i Rays",
			Illum.NumLights,Illum.PolysLit,Illum.Pairs,Illum.RaysTraced);

		// Unlock the LightMesh.
		Illum.LightMesh->Unlock(LOCK_ReadWrite);
		Illum.LightMesh->Shrink();
		Illum.LightMesh->Bits->Shrink();
	}
	Illum.FreeLightCamera();
	Illum.Level->Model->LightMesh = Illum.LightMesh;

	GApp->EndSlowTask();
	GBrushTracker.Exit();
	GCache.Flush();

	unguard;
}

/*---------------------------------------------------------------------------------------
   Light link topic handler
---------------------------------------------------------------------------------------*/

AUTOREGISTER_TOPIC("Light",LightTopicHandler);
void LightTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(LightTopicHandler::Get);
	ULightMesh::Ptr	LightMesh	= Level->Model->LightMesh;
	int Meshes=0,MeshPts=0,Size=0,MaxSize=0,CacheSize=0,Meters=0,LightCount=0,SelCount=0;

	for( int i=0; i<Level->Num; i++ )
	{
		AActor *Actor = Level->Element(i);
		if( Actor && Actor->IsA("Light") )
		{
			LightCount++;
			if( Actor->bSelected )
				SelCount++;
		}
	}

	if( !Level || !LightMesh )
	{
		Meshes = 0;
	}
	else
	{
		for( int i=0; i<LightMesh->Num; i++ )
		{
			Size       = (int)LightMesh(i).MeshUSize * (int)LightMesh(i).MeshVSize;
			MeshPts   += Size;
	  		CacheSize += Size * (int)LightMesh(i).MeshSpacing * (int)LightMesh(i).MeshSpacing;
			if (Size>MaxSize) MaxSize = Size;
		}
		Meters = CacheSize / (UNITS_PER_METER * UNITS_PER_METER);
	}
    if      (stricmp(Item,"Meshes")==0) 	Out.Logf("%i",Meshes);
    else if (stricmp(Item,"MeshPts")==0) 	Out.Logf("%i",MeshPts);
    else if (stricmp(Item,"MaxSize")==0) 	Out.Logf("%i",MaxSize);
    else if (stricmp(Item,"Meters")==0) 	Out.Logf("%i",Meters);
    else if (stricmp(Item,"Count")==0) 		Out.Logf("%i (%i)",LightCount,SelCount);
    else if (stricmp(Item,"AvgSize")==0) 	Out.Logf("%i",MeshPts/Max(1,Meshes));
    else if (stricmp(Item,"CacheSize")==0)  Out.Logf("%iK",CacheSize/1000);

	unguard;
}
void LightTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(LightTopicHandler::Set);
	unguard;
};

/*---------------------------------------------------------------------------------------
   The End
---------------------------------------------------------------------------------------*/
