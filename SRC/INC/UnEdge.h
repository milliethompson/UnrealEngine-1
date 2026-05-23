/*=============================================================================
	UnEdge.h: Unreal edge code.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNEDGE
#define _INC_UNEDGE

/*-----------------------------------------------------------------------------
	Edges.
-----------------------------------------------------------------------------*/

// An edge.  A list of these will be stored for each leaf in the Bsp.
struct FBspEdge
{
	INDEX			iSurf[2];		// 1 or 2 Bsp surfaces containing the edge.
	INDEX			pVertex[2];		// Vertices of the edge.
	INDEX			iFrame;
};

// A Bsp leaf.
struct FBspLeaf
{
	// Index into EdgePool of first edge.
	INT	iLeafEdgePool;

	// Number of edges in this leaf.
	INT NumLeafEdges;

	// Logical leaf number, if leaves have been merged.
	INT iLogicalLeaf;

	// Zone this leaf belongs in.
	BYTE iZone;
};

// Class to store temporary edge info.
struct FGlobalEdgeHack
{
	// Edges.
	FBspEdge			*Edges;

	// The visibility table.
	UBitMatrix			*Visibility;

	// List of all Bsp leaves.
	INT					NumLeaves;
	FBspLeaf			*Leaves;

	// Edge pool indexed into by FBspLeaf.
	INT					*EdgePool;
};

extern UNRENDER_API FGlobalEdgeHack GEdgeHack;

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/
#endif // _INC_UNEDGE
