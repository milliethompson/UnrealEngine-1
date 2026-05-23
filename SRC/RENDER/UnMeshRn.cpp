/*=============================================================================
	UnMeshRn.cpp: Unreal mesh rendering.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"
#include "UnRaster.h"
#include "UnRenDev.h"

/*------------------------------------------------------------------------------
	Triangle rendering.
------------------------------------------------------------------------------*/

void RenderTriangle
(
	UCamera*		Camera,
	UTexture*		Texture,
	FSpanBuffer*	Span,
	AZoneInfo*		Zone,
	FTransTexture*	Pts,
	INT				NumPts,
	DWORD			PolyFlags
)
{
	guard(RenderTriangle);

	// Draw it.
	if( GCameraManager->RenDev )
	{
		// Hardware render it.
		GCameraManager->RenDev->DrawPolyC( Camera, Texture, Pts, NumPts, PolyFlags );
	}
	else
	{
		// Allocate memory.
		FMemMark Mark(GMem);
		FRasterTexSetup	RasterTexSetup;
		FRasterTexPoly *RasterTexPoly = (FRasterTexPoly*)new( GMem, sizeof(FRasterTexPoly)+FGlobalRaster::MAX_RASTER_LINES*sizeof(FRasterTexLine) )BYTE;

		// Render it.
		RasterTexSetup.Setup( Camera, Pts, NumPts, &GMem );
		RasterTexSetup.Generate( RasterTexPoly );
		if( PolyFlags & PF_TwoSided )
		{
			RasterTexPoly->ForceForwardFace();
		}
		if 
		(	(Camera->Actor->RendMap != REN_Polys   )
		&&	(Camera->Actor->RendMap != REN_PolyCuts)
		&&	(Camera->Actor->RendMap != REN_Zones   )
		&&	(Camera->Actor->RendMap != REN_Wire    ) )
		{
			FMemMark Mark(GMem);
			FPolySpanTextureInfoBase *Info
			= GSpanTextureMapper->SetupForPoly
			(
				Camera,
				Texture,
				Zone,
				PolyFlags,
				PF_Masked
			);
			RasterTexPoly->Draw( Camera, *Info, Span );
			GSpanTextureMapper->FinishPoly(Info);
			Mark.Pop();
		}
		else 
		{
			RasterTexPoly->DrawFlat
			(
				Camera,
				COLOR(P_BROWN,4+(rand()&15)),
				Span
			);
		}
		Mark.Pop();
	}

	unguard;
}

/*------------------------------------------------------------------------------
	Shading calculation.
------------------------------------------------------------------------------*/

//
// Shade a point.
//
void Shade
(
	FVector			&Color,
	FTransSample&	Point,
	UCamera*		Camera,
	AActor*			Owner
)
{
	guardSlow(Shade);

	// Regular shading.
	if( !Owner->bUnlit )
	{
		// Lit.
		//FLOAT G = 0.5 - Min(0.5,0.00035*Location.Z);
		FLOAT G = Square(Square(Point | Point.Normal)/Point.SizeSquared());
		Color.R = Color.G = Color.B = Min
		(
			256.0 * 61.0, 256.0 + 60.0 * 256.0 * G + (Owner->AmbientGlow<<6)
		);
	}
	else
	{
		// Unlit.
		FLOAT G = 0.5 - Min(0.5,0.00035*Point.Z);
		Color.R = Color.G = Color.B = Min
		(
			256.0 * 61.0,256.0 + 60.0 * 256.0 * G + (Owner->AmbientGlow<<6)
		);
	}

	// Editor highlighting.
	if( Owner->bSelected  && Camera->Level->GetState()!=LEVEL_UpPlay )
	{
		Color.R = Min( 255.0*61.0, 128.0*61.0 + 0.5 * Color.R );
		Color.G = Min( 255.0*61.0, 128.0*61.0 + 0.5 * Color.G );
		Color.B = Min( 255.0*61.0, 128.0*61.0 + 0.5 * Color.B );
	}
	unguardSlow;
}

/*------------------------------------------------------------------------------
	Subsurface rendering.
------------------------------------------------------------------------------*/

void RenderSubsurface
(
	UCamera*		Camera,
	AActor*			Owner,
	UTexture*		Texture,
	FSpanBuffer*	Span,
	AZoneInfo*		Zone,
	FTransTexture*	Pts,
	DWORD			PolyFlags,
	INT				SubCount
)
{
	guard(RenderSubsurface);

	// If outcoded, skip it.
	if( Pts[0].Flags & Pts[1].Flags & Pts[2].Flags )
		return;

	INT CutSide[3], Cuts=0;
	FLOAT Alpha[3];
	if( SubCount > 0 )
	{
		// Compute side distances.
		for( int i=0,j=2; i<3; j=i++ )
		{
			FLOAT Dist  = Square(Pts[j].ScreenX-Pts[i].ScreenX) + Square(Pts[j].ScreenY-Pts[i].ScreenY);
			CutSide[j]  = Dist > Square(32.f);
			Alpha[j]    = Min( Dist / Square(32.f) - 1.f, 1.f );
			Cuts       += (CutSide[j]<<j);
		}
	}

	// Triangle subdivision table.
	static const CutTable[8][4][3] =
	{
		{{0,1,2},{9,9,9},{9,9,9},{9,9,9}},
		{{0,3,2},{2,3,1},{9,9,9},{9,9,9}},
		{{0,1,4},{4,2,0},{9,9,9},{9,9,9}},
		{{0,3,2},{2,3,4},{4,3,1},{9,9,9}},
		{{0,1,5},{5,1,2},{9,9,9},{9,9,9}},
		{{0,3,5},{5,3,1},{1,2,5},{9,9,9}},
		{{0,1,4},{4,2,5},{5,0,4},{9,9,9}},
		{{0,3,5},{3,1,4},{5,4,2},{3,4,5}}
	};

	// See if it should be subdivided.
	if( Cuts && SubCount>0 && Owner->bMeshCurvy )
	{
		for( int i=0,j=2; i<3; j=i++ ) if( CutSide[j] )
		{
			// Compute midpoint.
			FTransTexture &MidPt = Pts[j+3];
			MidPt = (Pts[j]+Pts[i])*0.5;

			// Compute midpoint normal.
			MidPt.Normal  = Pts[j].Normal+Pts[i].Normal;
			MidPt.Normal *= DivSqrtApprox( MidPt.Normal.SizeSquared() );

			// Shade the midpoint.
			FVector MidShade;
			Shade( MidShade, MidPt, Camera, Owner );
			MidPt.Color = MidPt.Color + (MidShade - MidPt.Color) * Alpha[j];
			FLOAT Dist  = Square(Pts[j].ScreenX-Pts[i].ScreenX) + Square(Pts[j].ScreenY-Pts[i].ScreenY);

			// Curve the midpoint.
			(FVector&)MidPt
			+=	0.15
			*	Alpha[j]
			*	(FVector&)MidPt.Normal
			*	SqrtApprox
				(
					(Pts[j]-Pts[i]).SizeSquared()
				*	(Pts[i].Normal^Pts[j].Normal).SizeSquared()
				);

			// Outcode and optionally transform midpoint.
			MidPt.ComputeOutcode( Camera );
			MidPt.Transform( Camera, 0 );
		}
		FTransTexture NewPts[6];
		for( i=0; i<4 && CutTable[Cuts][i][0]!=9; i++ )
		{
			for( int j=0; j<3; j++ )
				NewPts[j] = Pts[CutTable[Cuts][i][j]];
			RenderSubsurface( Camera, Owner, Texture, Span, Zone, NewPts, PolyFlags, SubCount-1 );
		}
	}
	else
	{
		// Backface reject it.
		if( !(PolyFlags & PF_TwoSided) )
			if( (Pts[0] | (Pts[1] ^ Pts[2])) < 0.0 )
				return;

		// Clip and render it.
		FTransTexture NewPts[16];
		INT NumPts = GRender.ClipTexPoints( Camera, Pts, &NewPts[0], 3 );
		if( NumPts > 0 )
			RenderTriangle( Camera, Texture, Span, Zone, NewPts, NumPts, PolyFlags );
	}
	unguard;
}

/*------------------------------------------------------------------------------
	High level mesh rendering.
------------------------------------------------------------------------------*/

//
// Structure used by DrawMesh for sorting triangles.
//
struct FMeshTriSort
{
	FTransTexture*	FirstPoint;	// First point of clipped triangle point list.
	INT 			Index;		// Index of triangle in triangle table.
	INT				Key;		// Sort key.
	friend inline INT Compare( const FMeshTriSort &A, const FMeshTriSort &B )
		{ return B.Key - A.Key; }
};

//
// Draw a mesh map.
//
void FRender::DrawMesh
(
	UCamera		*Camera,
	AActor		*Owner,
	FSpanBuffer *SpanBuffer,
	FSprite		*Sprite
)
{
	guard(FRender::DrawMesh);
	UMesh				*Mesh = Owner->Mesh;
	FPoly				EdPoly;
	FCoords				Coords;
	FTransform			*V1,*V2,*V3;
	FMeshTriSort 		*TriTop;
	WORD				Color;
	INT 				i,j,k;

	// Lock the mesh map.
	Mesh->Lock( LOCK_Read );
	BOOL Fatten   = Owner->Fatness!=0;
	FLOAT Fatness = (Owner->Fatness/16.0)-8.0;

	// Process special poly flags.
	DWORD ExtraFlags = Owner->bNoSmooth ? PF_NoSmooth : 0;

	// Grab all memory needed for drawing the mesh.
	FMemMark Mark(GMem);
	FMeshTriSort *TriPool = new(GMem,Mesh->Tris->Num)FMeshTriSort;

	// Get transformed verts.
	FTransSample *Samples = new(GMem,Mesh->FrameVerts)FTransSample;
	BYTE Outcode = Mesh->GetFrame( Samples, Camera, Sprite->Actor );

	// Render a wireframe view or textured view.
	if( Camera->IsWire() || Camera->IsOrtho() )
	{
		// Set up color.
		if( Owner->bSelected )	Color = ActorHiWireColor;
		else					Color = ActorWireColor;

		// Render each wireframe triangle.
		for( i=0; i<Mesh->Tris->Max; i++ )
		{
			// Build an FPoly from this triangle.
			EdPoly.NumVertices = 3;
			EdPoly.PolyFlags   = 0;
			FMeshTri &Tri = Mesh->Tris(i);
			for( j=0; j<3; j++ )
				EdPoly.Vertex[j] = Samples[Tri.iVertex[j]];

			// Set FPoly's flags.
			if( Tri.Type==MT_Masked )
				EdPoly.PolyFlags |= (PF_Masked | PF_NotSolid | PF_TwoSided);
			else if( Tri.Type==MT_Transparent )
				EdPoly.PolyFlags |= (PF_Transparent | PF_NotSolid | PF_TwoSided);

			// Render it.
			GRender.DrawFPoly( Camera, &EdPoly, Color, 1, 0 );
		}
	}
	else
	{
		// Process triangles.
		INT VisibleTriangles = 0;
		if( Outcode == 0 )
		{
			// Set up list for triangle sorting, adding all non-backfaced triangles.
			TriTop = &TriPool[0];
			for( i=0; i<Mesh->Tris->Max; i++ )
			{
				FMeshTri &Tri = Mesh->Tris(i);
				V1            = &Samples[Tri.iVertex[0]];
				V2            = &Samples[Tri.iVertex[1]];
				V3            = &Samples[Tri.iVertex[2]];

				// Try to outcode-reject the triangle.
				if( !(V1->Flags & V2->Flags & V3->Flags) )
				{
					// This is visible.
					TriTop->Index = i;

					// Set the sort key.
					if( GCameraManager->RenDev )
					{
						UTexture *T = Mesh->Textures(Tri.TextureNum);
						TriTop->Key = T ? ((T->Palette->GetIndex() << 16) + T->GetIndex()) : 0;
					}
					else
					{
						TriTop->Key = ftoi
						(
							Samples[Tri.iVertex[0]].Z +
							Samples[Tri.iVertex[1]].Z +
							Samples[Tri.iVertex[2]].Z
						);
					}

					// Add to list.
					VisibleTriangles++;
					TriTop++;
				}
			}
		}

		// Render triangles.
		if( VisibleTriangles>0 )
		{
			QSort( TriPool, VisibleTriangles );

			// Build list of all incident lights on the creature.
			// Considers only the entire creature and does not
			// differentiate between individual creature polys.
			GLightManager->SetupForActor( Camera, Sprite->Actor );

			// Compute all triangle normals.
			FVector *TriNormals = new(GMem,Mesh->Tris->Num)FVector;
			for( int i=0; i<Mesh->Tris->Num; i++ )
			{
				FMeshTri &Tri  = Mesh->Tris(i);
				FVector Side1  = Samples[Tri.iVertex[1]] - Samples[Tri.iVertex[0]];
				FVector Side2  = Samples[Tri.iVertex[2]] - Samples[Tri.iVertex[0]];
				TriNormals[i]  = (Side2 ^ Side1);
				TriNormals[i] *= DivSqrtApprox(TriNormals[i].SizeSquared());
			}

			// Perform all vertex lighting.
			for( i=0; i<VisibleTriangles; i++ )
			{
				FMeshTri &Tri = Mesh->Tris(TriPool[i].Index);
				for( j=0; j<3; j++ )
				{
					INDEX        iVert = Tri.iVertex[j];
					FTransSample &Vert = Samples[iVert];
					if( Vert.Color.R == -1 )
					{
						// Compute vertex normal.
						Vert.Normal = GMath.ZeroVector;
						FMeshVertConnect &Connect = Mesh->Connects(iVert);
						for( k=0; k<Connect.NumVertTriangles; k++ )
							Vert.Normal += TriNormals[Mesh->VertLinks(Connect.TriangleListOffset + k)];
						Vert.Normal *= DivSqrtApprox(Vert.Normal.SizeSquared());

						// Fatten it if desired.
						if( Fatten )
						{
							(FVector&)Vert += Vert.Normal * Fatness;
							Vert.ComputeOutcode( Camera );
						}

						// Transform it.
						Vert.Transform( Camera, 0 );

						// Compute effect of each lightsource on this vertex.
						Shade( Vert.Color, Vert, Camera, Owner );
					}
				}
			}

			// Clip and draw the triangles.
			for( i=0; i<VisibleTriangles; i++ )
			{
				// Set up the triangle.
				FMeshTri &Tri = Mesh->Tris(TriPool[i].Index);
				FTransTexture Pts[6];
				for( j=0; j<3; j++ )
				{
					(FTransSample&)Pts[j] = Samples[Tri.iVertex[j]];
					Pts[j].U = Tri.Tex[j].U * 65536.0;
					Pts[j].V = Tri.Tex[j].V * 65536.0;
				}

				// Get texture.
				UTexture *Texture = Mesh->Textures( Tri.TextureNum );
				if( !Texture ) 
					Texture = GGfx.DefaultTexture;
				
				// Render triangle.
				RenderSubsurface
				(
					Camera,
					Owner,
					Texture, 
					SpanBuffer, 
					Sprite->Zone, 
					Pts, 
					ExtraFlags | (Tri.Type==MT_Masked ? (PF_Masked | PF_TwoSided) : 0),
					4
				);
			}
		}
	}
	Mark.Pop();
	Mesh->Unlock(LOCK_Read);

	STAT(GStat.MeshesDrawn++);
	unguardf(("(%s)",Owner->Mesh->GetName()));
}

/*------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------*/
