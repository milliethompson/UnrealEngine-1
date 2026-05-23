/*=============================================================================
	UnMesh.cpp: Unreal mesh animation functions

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Note: See DRAGON.MAC for a sample import macro

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Mesh related object implementation.
-----------------------------------------------------------------------------*/

IMPLEMENT_DB_CLASS(UMeshVerts);
IMPLEMENT_DB_CLASS(UMeshTris);
IMPLEMENT_DB_CLASS(UMeshAnimSeqs);
IMPLEMENT_DB_CLASS(UMeshAnimNotifys);
IMPLEMENT_DB_CLASS(UMeshVertConnects);

/*-----------------------------------------------------------------------------
	UMesh object implementation.
-----------------------------------------------------------------------------*/

void UMesh::InitHeader()
{
	guard(UMesh::InitHeader);

	// Init parent.
	UPrimitive::InitHeader();
	bCheckCollision = 0;

	// Init objects.
	Verts			= (UMeshVerts		*)NULL;
	Tris			= (UMeshTris		*)NULL;
	AnimSeqs		= (UMeshAnimSeqs	*)NULL;
	Connects		= (UMeshVertConnects*)NULL;
	Bounds			= (UBounds			*)NULL;
	VertLinks		= (UWords			*)NULL;
	Textures		= (TArray<UTexture*>*)NULL;
	Notifys			= (UMeshAnimNotifys *)NULL;

	// Counts.
	FrameVerts		= 0;
	AnimFrames		= 0;

	// Scaling.
	Scale			= GMath.UnitScaleVect;
	Origin			= GMath.ZeroVector;
	RotOrigin		= GMath.ZeroRotation;

	// Other info.
	AndFlags		= ~(DWORD)0;
	OrFlags			= 0;

	// Editing info.
	CurPoly			= 0;
	CurVertex		= 0;

	unguard;
}
void UMesh::PostLoadHeader( DWORD PostFlags )
{
	guard(UMesh::PostLoad);

	// Postload parent.
	UPrimitive::PostLoadHeader( PostFlags );

	// Check that all objects are in place.
	checkState(Tris!=NULL);
	checkState(AnimSeqs!=NULL);
	checkState(Verts!=NULL);
	checkState(Connects!=NULL);
	checkState(Bounds!=NULL);
	checkState(VertLinks!=NULL);
	checkState(Textures!=NULL);
	checkState(Notifys!=NULL);

	// Check all sizes.
	checkState(Verts->Num==FrameVerts*AnimFrames);
	checkState(Connects->Num==FrameVerts);
	checkState(Bounds->Num==AnimFrames);

	unguard;
}
IMPLEMENT_CLASS(UMesh);

/*-----------------------------------------------------------------------------
	UMesh collision interface.
-----------------------------------------------------------------------------*/

//
// Get the world bounding volume for this primitive, as owned by Owner.
//
FBoundingRect UMesh::GetBoundingRect( AActor *Owner )
{
	guard(UMesh::GetBoundingRect);
	FBoundingRect Bound;

	// Get frame indices.
	INDEX iFrame1 = 0, iFrame2 = 0;
	FMeshAnimSeq *Seq = GetAnimSeq( Owner->AnimSequence );
	if( Seq && Owner->AnimFrame >= 0.0 )
	{
		// Animating, so use bound enclosing two frames' bounds.
		INT iFrame = floor((Owner->AnimFrame+1.0) * Seq->NumFrames);
		iFrame1    = Seq->StartFrame + ((iFrame + 0) % Seq->NumFrames);
		iFrame2    = Seq->StartFrame + ((iFrame + 1) % Seq->NumFrames);
		Bound      = Bounds(iFrame1) + Bounds(iFrame2);
	}
	else
	{
		// Interpolating, so use pessimistic mesh bound.
		Bound = LocalBound;
	}

	// Transform Bound by owner's scale and origin.
	Bound.Min = FVector(-1,-1,-1) + Owner->Location + Scale * (Bound.Min - Origin) * Owner->DrawScale;
	Bound.Max = FVector(+1,+1,+1) + Owner->Location + Scale * (Bound.Max - Origin) * Owner->DrawScale;

	return Bound;
	unguard;
}

/*-----------------------------------------------------------------------------
	UMesh animation interface.
-----------------------------------------------------------------------------*/

//
// Get the transformed point set corresponding to the animation frame 
// of this primitive owned by Owner. Returns the total outcode of the points.
//
BYTE UMesh::GetFrame
(
	FTransSample*	ResultVerts,
	UCamera*		Camera,
	AActor*			Owner
)
{
	guard(UMesh::GetFrame);

	// Create or get cache memory.
	FCacheItem *Item;
	BOOL  WasCached = 1;
	DWORD CacheID   = MakeCacheID( CID_TweenAnim, Owner->GetIndex() );
	BYTE  *Mem      = GCache.Get( CacheID, Item );
	if( Mem==NULL || *(UMesh**)Mem!=this )
	{
		if( Mem != NULL )
		{
			// Actor's mesh changed.
			Item->Unlock();
			GCache.Flush( CacheID );
		}
		Mem       = GCache.Create( CacheID, Item, sizeof(UMesh*) + sizeof(FLOAT) + sizeof(FName) + FrameVerts * sizeof(FVector) );
		WasCached = 0;
	}
	UMesh *&CachedMesh = *(UMesh**)Mem; Mem += sizeof(UMesh*);
	FLOAT &CachedFrame = *(FLOAT *)Mem; Mem += sizeof(FLOAT );
	FName &CachedSeq   = *(FName *)Mem; Mem += sizeof(FName );
	if( !WasCached )
	{
		CachedMesh  = this;
		CachedSeq   = NAME_None;
		CachedFrame = 0.0;
	}

	// Get vertices pointer and count.
	FVector *CachedVerts = (FVector*)Mem;

	// Setup coordinate system.
	FCoords Coords;
	FVector TempVector;
	if( Camera->IsWire() || Camera->IsOrtho() )
	{
		Coords		= GMath.UnitCoords * Owner->Rotation * RotOrigin;
		TempVector  = Owner->Location;
	}
	else
	{
		Coords		= Camera->Coords * Owner->Rotation * RotOrigin;
		TempVector	= (Owner->Location - Camera->Coords.Origin).TransformVectorBy( Camera->Coords );
	}
	FVector NewScale  = Scale * Owner->DrawScale;
	Coords.XAxis     *= NewScale;
	Coords.YAxis     *= NewScale;
	Coords.ZAxis     *= NewScale;

	// Get animation sequence.
	FMeshAnimSeq *Seq=GetAnimSeq( Owner->AnimSequence );

	// Transform all points into screenspace.
	BYTE Outcode = FVF_OutReject;
	if( Owner->AnimFrame>=0.0 || !WasCached )
	{
		// Compute interpolation numbers.
		FLOAT Alpha=0.0;
		INT iFrameOffset1=0, iFrameOffset2=0;
		if( Seq )
		{
			FLOAT Frame   = Max(Owner->AnimFrame,0.f) * Seq->NumFrames;
			INT iFrame    = floor(Frame);
			Alpha         = Frame - iFrame;
			iFrameOffset1 = (Seq->StartFrame + ((iFrame + 0) % Seq->NumFrames)) * FrameVerts;
			iFrameOffset2 = (Seq->StartFrame + ((iFrame + 1) % Seq->NumFrames)) * FrameVerts;
		}

		// Interpolate two frames.
		FMeshVert *MeshVertex1 = &Verts( iFrameOffset1 );
		FMeshVert *MeshVertex2 = &Verts( iFrameOffset2 );
		for( int i=0; i<FrameVerts; i++ )
		{
			// Convert packed vectors to float.
			FVector V1( MeshVertex1[i].X, MeshVertex1[i].Y, MeshVertex1[i].Z );
			FVector V2( MeshVertex2[i].X, MeshVertex2[i].Y, MeshVertex2[i].Z );

			// Interpolate vertices.
			CachedVerts[i] = V1 + (V2-V1)*Alpha;

			// Transform it.
			(FVector&)ResultVerts[i] = (CachedVerts[i] - Origin).TransformVectorBy(Coords) + TempVector;
			ResultVerts[i].Color.R   = -1;
			ResultVerts[i].ComputeOutcode( Camera );
			Outcode &= ResultVerts[i].Flags;
		}
	}
	else
	{
		// Compute tweening numbers.
		FLOAT StartFrame = Seq ? (-1.0 / Seq->NumFrames) : 0.0;
		INT iFrameOffset = Seq ? Seq->StartFrame * FrameVerts : 0;
		FLOAT Alpha = 1.0 - Owner->AnimFrame / CachedFrame;
		if( CachedSeq!=Owner->AnimSequence || Alpha<0.0 || Alpha>1.0)
		{
			CachedSeq   = Owner->AnimSequence;
			CachedFrame = StartFrame;
			Alpha       = 0.0;
		}

		// Tween all points.
		FMeshVert *MeshVertex = &Verts( iFrameOffset );
		for( int i=0; i<FrameVerts; i++ )
		{
			// Convert packed vector to float.
			FVector V2( MeshVertex[i].X, MeshVertex[i].Y, MeshVertex[i].Z );

			// Interpolate vertices.
			CachedVerts[i] += (V2 - CachedVerts[i])*Alpha;

			// Transform it.
			(FVector&)ResultVerts[i] = (CachedVerts[i] - Origin).TransformVectorBy(Coords) + TempVector;
			ResultVerts[i].Color.R   = -1;
			ResultVerts[i].ComputeOutcode( Camera );
			Outcode &= ResultVerts[i].Flags;
		}

		// Update cached frame.
		CachedFrame = Owner->AnimFrame;
	}
	Item->Unlock();
	return Outcode;
	unguard;
}

/*-----------------------------------------------------------------------------
	UMesh constructor.
-----------------------------------------------------------------------------*/

//
// UMesh constructor.
//
UMesh::UMesh( int NumPolys, int NumVerts, int NumFrames, int NumTex )
{
	guard(UMesh::UMesh);

	// Set counts.
	FrameVerts	= NumVerts;
	AnimFrames	= NumFrames;

	// Allocate all stuff.
	Tris		= new(GetName(),CREATE_Replace)UMeshTris(NumPolys,1);
	AnimSeqs	= new(GetName(),CREATE_Replace)UMeshAnimSeqs(0);
	Verts		= new(GetName(),CREATE_Replace)UMeshVerts(NumVerts * NumFrames,1);
	Connects	= new(GetName(),CREATE_Replace)UMeshVertConnects(NumVerts,1);
	Bounds		= new(GetName(),CREATE_Replace)UBounds(NumFrames,1);
	VertLinks	= new(GetName(),CREATE_Replace)UWords(0);
	Textures	= new(GetName(),CREATE_Replace)TArray<UTexture*>(NumTex,1);
	Notifys     = new(GetName(),CREATE_Replace)UMeshAnimNotifys(0);

	// Init textures.
	for( int i=0; i<Textures->Num; i++ )
		Textures(i) = NULL;

	unguard;
}

/*-----------------------------------------------------------------------------
	Mesh link topic function.
-----------------------------------------------------------------------------*/

AUTOREGISTER_TOPIC("Mesh",MeshTopicHandler);
void MeshTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(MeshTopicHandler::Get);

	if( !_strnicmp(Item,"NUMANIMSEQS",11) )
	{
		UMesh *Mesh;
		if( GetUMesh(Item,"NAME=",Mesh) )
		{
			Mesh->Lock(LOCK_Read);
			Out.Logf( "%i",Mesh->AnimSeqs->Num );
			Mesh->Unlock(LOCK_Read);
		}
	}
	else if( !_strnicmp(Item,"ANIMSEQ",7) )
	{
		UMesh *Mesh;
		INT   SeqNum;
		if( GetUMesh(Item,"NAME=",Mesh) &&
			(GetINT(Item,"NUM=",&SeqNum)) )
		{
			Mesh->Lock(LOCK_Read);
			FMeshAnimSeq &Seq = Mesh->AnimSeqs(SeqNum);
			if( Seq.Name!=NAME_None )
			{
				Out.Logf
				(
					"%s                                        %03i %03i",
					Seq.Name(),
					SeqNum,
					Seq.NumFrames
 				);
			}
			Mesh->Unlock(LOCK_Read);
		}
	}
	unguard;
}
void MeshTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(MeshTopicHandler::Set);
	unguard;
}

/*-----------------------------------------------------------------------------
	The end.
-----------------------------------------------------------------------------*/
