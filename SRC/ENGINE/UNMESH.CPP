/*=============================================================================
	UnMesh.cpp: Unreal mesh animation functions

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Note: See DRAGON.MAC for a sample import macro

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"

/*-----------------------------------------------------------------------------
	Mesh info (only used in this file, not globally).
-----------------------------------------------------------------------------*/

static void GetMeshResInfo( IMesh *MeshInfo, UMesh *Mesh, BYTE *MeshData )
{
	guard(GetMeshResInfo);

	// Get maximums.
	MeshInfo->TriMax      		= (int)Mesh->MaxTriangles  * sizeof (FMeshTriangle);
	MeshInfo->VertexMax   		= (int)Mesh->MaxAnimFrames * (int)Mesh->MaxVertices * sizeof (FMeshVertex);
	MeshInfo->VertLinkIndexMax	= (int)Mesh->MaxVertices   * sizeof (FMeshVertLinkIndex);
	MeshInfo->VertLinkMax		= (int)Mesh->MaxVertLinks  * sizeof (WORD);
	MeshInfo->BoundMax      	= (int)Mesh->MaxAnimFrames * sizeof (FBoundingRect);
	MeshInfo->AnimMax     		= (int)Mesh->MaxAnimSeqs   * sizeof (FMeshAnimSeq);

	// Calc total.
	MeshInfo->TotalMax			= 	MeshInfo->TriMax + MeshInfo->VertexMax +
									MeshInfo->VertLinkIndexMax + MeshInfo->VertLinkMax +
									MeshInfo->BoundMax + MeshInfo->AnimMax;

	// Get pointers.
	MeshInfo->Triangles			= (FMeshTriangle		*) (MeshData += 0);
	MeshInfo->Vertex			= (FMeshVertex			*) (MeshData += MeshInfo->TriMax);
	MeshInfo->VertLinkIndex		= (FMeshVertLinkIndex	*) (MeshData += MeshInfo->VertexMax);
	MeshInfo->VertLinks			= (WORD 				*) (MeshData += MeshInfo->VertLinkIndexMax);
	MeshInfo->Bound        		= (FBoundingRect		*) (MeshData += MeshInfo->VertLinkMax);
	MeshInfo->AnimSeqs			= (FMeshAnimSeq			*) (MeshData += MeshInfo->BoundMax);

	unguard;
}

//
// Lock a mesh for reading (doesn't handle writing).
// This just fills in the MeshMapInfo structure for easy manipulation.
//
void UMesh::GetInfo(IMesh *MeshInfo)
{
	guard(UMesh::GetInfo);
	GetMeshResInfo (MeshInfo,this,GetData());
	unguard;
}

/*-----------------------------------------------------------------------------
	UMesh resource implementation.
-----------------------------------------------------------------------------*/

void UMesh::Register(FResourceType *Type)
{
	guard(UMesh::Register);
	
	Type->HeaderSize = sizeof (UMesh);
	Type->RecordSize = 0;
	Type->Version    = 1;
	strcpy (Type->Descr,"Mesh");
	
	unguard;
}
void UMesh::InitHeader()
{
	guard(UMesh::InitHeader);

	// Init resource header to defaults.
	NumAnimFrames	= 0;	MaxAnimFrames = 0;
	NumVertices		= 0;	MaxVertices   = 0;
	NumVertLinks	= 0;	MaxVertLinks  = 0;
	NumTriangles	= 0;	MaxTriangles  = 0;

	Origin			= GMath.ZeroVector;
	RotOrigin		= GMath.ZeroRotation;
	
	unguard;
}
void UMesh::InitData()
{
	guard(UMesh::InitData);

	NumAnimFrames	= 0;
	NumVertices		= 0;
	NumVertLinks	= 0;
	NumTriangles	= 0;
	
	unguard;
}
int UMesh::QuerySize()
{
	guard(UMesh::QuerySize);

	IMesh MeshInfo;
	GetMeshResInfo (&MeshInfo,this,GetData());
	return MeshInfo.TotalMax;

	unguard;
}
int UMesh::QueryMinSize()
{
	guard(UMesh::QueryMinSize);
	return QuerySize();
	unguard;
}
void UMesh::QueryDataReferences(FResourceCallback &Callback)
{
	guard(UMesh::QueryDataReferences);

	IMesh		MeshInfo;
	GetMeshResInfo (&MeshInfo,this,GetData());

	for( int i=0; i<NumAnimSeqs; i++ )
	{
		Callback.Name (this,&MeshInfo.AnimSeqs[i].SeqName,0);
	}
	unguard;
}
void UMesh::Flip()
{
	guard(UMesh::Flip);

	UResource::Flip();
	//todo:

	unguard;
}
AUTOREGISTER_RESOURCE(RES_Mesh,UMesh,0xB2D90858,0xCCD211cf,0x91360000,0xC028B992);

/*-----------------------------------------------------------------------------
	UMeshMap resource implementation.
-----------------------------------------------------------------------------*/

void UMeshMap::Register(FResourceType *Type)
{
	guard(UMeshMap::Register);

	Type->HeaderSize = sizeof (UMeshMap);
	Type->RecordSize = 0;
	Type->Version    = 1;
	Type->TypeFlags  = RTF_ScriptReferencable;
	strcpy (Type->Descr,"MeshMap");
	
	unguard;
}
void UMeshMap::InitHeader()
{
	guard(UMeshMap::InitHeader);

	Mesh			= NULL;
	MaxTextures		= 0;
	AndFlags		= MAXWORD;
	OrFlags			= 0;
	Scale			= GMath.UnitScaleVect;

	unguard;
}
void UMeshMap::InitData()
{
	guard(UMeshMap::InitData);

	for (DWORD i=0; i<MaxTextures; i++) 
		GetData()[i] = NULL;
	
	unguard;
}
int UMeshMap::QuerySize()
{
	guard(UMeshMap::QuerySize);
	return MaxTextures * sizeof (UTexture *);
	unguard;
}
int UMeshMap::QueryMinSize()
{
	guard(UMeshMap::QueryMinSize);
	return QuerySize();
	unguard;
}
void UMeshMap::QueryHeaderReferences(FResourceCallback &Callback)
{
	guard(UMeshMap::QueryHeaderReferences);
	Callback.Resource (this,(UResource **)&Mesh,0);
	unguard;
}
void UMeshMap::QueryDataReferences(FResourceCallback &Callback)
{
	guard(UMeshMap::QueryDataReferences);

	for (DWORD i=0; i<MaxTextures; i++)
	{
		Callback.Resource (this,(UResource **)&GetData()[i],0);
	}
	unguard;
}
void UMeshMap::Flip()
{
	guard(UMeshMap::Flip);

	UResource::Flip();
	//todo:

	unguard;
}
AUTOREGISTER_RESOURCE(RES_MeshMap,UMeshMap,0xB2D90859,0xCCD211cf,0x91360000,0xC028B992);

/*-----------------------------------------------------------------------------
	Mesh sprite functions.
-----------------------------------------------------------------------------*/

//
// Lock a sprite for reading (doesn't handle writing).
// This just fills in the MeshMapInfo structure for easy manipulation.
//
void UMeshMap::Lock(IMeshMap *MeshMapInfo)
{
	guard(UMeshMap::Lock);
	IMesh MeshInfo;

	// Get mesh info.
	Mesh->GetInfo(&MeshInfo);

	// Set all MeshMapInfo properties.
	MeshMapInfo->MeshMap		= this;
	MeshMapInfo->Mesh			= Mesh;

	MeshMapInfo->NumTriangles	= Mesh->NumTriangles;
	MeshMapInfo->NumVertices	= Mesh->NumVertices;
	MeshMapInfo->NumVertLinks	= Mesh->NumVertLinks;
	MeshMapInfo->NumAnimFrames	= Mesh->NumAnimFrames;
	MeshMapInfo->NumAnimSeqs	= Mesh->NumAnimSeqs;
	MeshMapInfo->MaxTextures	= MaxTextures;

	MeshMapInfo->Triangles		= MeshInfo.Triangles;
	MeshMapInfo->Vertex			= MeshInfo.Vertex;
	MeshMapInfo->VertLinkIndex  = MeshInfo.VertLinkIndex;
	MeshMapInfo->VertLinks      = MeshInfo.VertLinks;
	MeshMapInfo->Bound          = MeshInfo.Bound;
	MeshMapInfo->AnimSeqs		= MeshInfo.AnimSeqs;
	MeshMapInfo->Textures		= (UTexture **)GetData();

	MeshMapInfo->Origin			= Mesh->Origin;
	MeshMapInfo->RotOrigin      = Mesh->RotOrigin;
	MeshMapInfo->Scale			= Scale;

	unguard;
}

//
// Unlock a mesh map.
//
void UMeshMap::Unlock(IMeshMap *MeshMapInfo)
{
	guard(UMeshMap::Unlock);
	unguard;
}

void UMesh::SetSequence(const char *SeqName, int StartFrame, int NumFrames)
{
	guard(UMesh::SetSequence);

	FMeshAnimSeq 	*AnimSeq;
	IMesh			MeshInfo;

	if( (NumAnimSeqs+1) >= MaxAnimSeqs )
	{
		debugf (LOG_Info,"Animation sequence table full");
	}
	else
	{
		GetInfo (&MeshInfo);

		AnimSeq = &MeshInfo.AnimSeqs [NumAnimSeqs++];

		AnimSeq->SeqName.Add(SeqName);			// Sequence's name.
		AnimSeq->SeqStartFrame 	= StartFrame;	// Starting animation frame.
		AnimSeq->SeqNumFrames  	= NumFrames;	// Number of frames in sequence.
		AnimSeq->Rate          	= 0;			// Playback rate (scale still undefined).
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Mesh link topic function.
-----------------------------------------------------------------------------*/

AUTOREGISTER_TOPIC("Mesh",MeshTopicHandler);
void MeshTopicHandler::Get(ULevel *Level, const char *Topic, const char *Item, char *Data)
{
	guard(MeshTopicHandler::Get);

	UMeshMap		*MeshMap;
	IMesh			MeshInfo;
	FMeshAnimSeq 	*AnimSeq;
	WORD			SeqNum;

	if( !_strnicmp(Item,"NUMANIMSEQS",11) )
	{
		if( GetUMeshMap(Item,"NAME=",&MeshMap) )
		{
			itoa(MeshMap->Mesh->NumAnimSeqs,Data,10);
		}
	}
	else if( !_strnicmp(Item,"ANIMSEQ",7) )
	{
		if( GetUMeshMap(Item,"NAME=",&MeshMap) &&
			(GetWORD(Item,"NUM=",&SeqNum)) )
		{
			MeshMap->Mesh->GetInfo(&MeshInfo);
			AnimSeq = &MeshInfo.AnimSeqs [SeqNum];

			if( !AnimSeq->SeqName.IsNone() )
			{
				sprintf(Data,"%s                                        %03i %03i",
					AnimSeq->SeqName.Name(),SeqNum,AnimSeq->SeqNumFrames);
			}
		}
	}
	unguard;
}
void MeshTopicHandler::Set(ULevel *Level, const char *Topic, const char *Item, const char *Data)
{
	guard(MeshTopicHandler::Set);
	unguard;
}

/*-----------------------------------------------------------------------------
	The end.
-----------------------------------------------------------------------------*/
