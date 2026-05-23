/*=============================================================================
    UnPhys.cpp: Simple physics and occlusion testing for editor

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*---------------------------------------------------------------------------------------
   Primitive PointCheck.
---------------------------------------------------------------------------------------*/

//
// Classify a point as inside (0) or outside (1), and also set its node location
// or INDEX_NONE if Bsp is empty.
//
INT UModel::PointCheck
(
	FCheckResult	&Hit,
	AActor			*Owner,
	const FVector	&Location,
	DWORD			ExtraNodeFlags
)
{
	guard(UModel::PointClass);
	if( Nodes->Num )
	{
		INDEX iNode=0, iTempNode, Class=RootOutside;
		do	
		{
			iTempNode				= iNode;
			const FBspNode	&Node	= Nodes(iNode);
			INT				CSG  	= Node.IsCsg(ExtraNodeFlags);

			if( Node.Plane.PlaneDot(Location) > 0.0 )
			{
				Class |= CSG;
				iNode = Node.iFront;
			}
			else
			{
				Class &= ~CSG;
				iNode = Node.iBack;
			}
		} while( iNode != INDEX_NONE );
		if( Class == 0 )
		{
			// Hit something.
			Hit.Location    = Location;
			Hit.Normal      = GMath.ZeroVector;
			Hit.Actor       = NULL;
			Hit.Primitive   = this;
			Hit.Item        = iTempNode;
			Hit.Time        = 0.0;
		}
		return Class;
	}
	else
	{
		Hit.Item = INDEX_NONE;
   		return RootOutside;
	}
	unguard;
}

/*---------------------------------------------------------------------------------------
   Primitive LineCheck.
---------------------------------------------------------------------------------------*/

//
// Recursive minion of UModel::LineCheck.
//
int LineCheck
(
	FCheckResult	&Hit,
	UModel			&Model,
	INDEX			iHit,
	INDEX			iParent,
	INDEX			iNode,
	const FVector	&Start,
	const FVector	&End, 
	INT				Outside,
	DWORD			ExtraNodeFlags
)
{
	while( iNode != INDEX_NONE )
	{
		const FBspNode*	Node	= &Model.Nodes(iNode);
		INT				CSG		= Node->IsCsg(ExtraNodeFlags);

		// Check side-of-plane for both points.
		FLOAT		Dist1	= Node->Plane.PlaneDot(Start);
		FLOAT		Dist2	= Node->Plane.PlaneDot(End);

		// Classify line based on both distances.
		if( Dist1 > -0.001 && Dist2 > -0.001 )
		{
			// Both points are in front.
			Outside |= CSG;
			iParent  = iNode;
			iNode    = Node->iFront;
		}
		else if( Dist1 < 0.001 && Dist2 < 0.001 )
		{
			// Both points are in back.
			Outside &= !CSG;
			iParent  = iNode;
			iNode    = Node->iBack;
		}
		else
		{
			// Line is split (guranteed to be non-parallel to plane, so TimeDenominator != 0).
			FVector Middle = Start + (Start-End) * (Dist1/(Dist2-Dist1));
			if( Dist1 > 0.0 )
			{
				// Dist2 < 0.
				if( !LineCheck( Hit, Model, iHit, iNode, Node->iFront, Start, Middle, Outside || CSG, ExtraNodeFlags ) )
					return 0;
				return LineCheck( Hit, Model, iNode, iNode, Node->iBack, Middle, End, Outside && !CSG, ExtraNodeFlags );
			}
			else
			{
				// Dist1 < 0, Dist2 > 0.
				if( !LineCheck( Hit, Model, iHit, iNode, Node->iBack, Start, Middle, Outside && !CSG, ExtraNodeFlags ) )
					return 0;
				return LineCheck( Hit, Model, iNode, iNode, Node->iFront, Middle, End, Outside || CSG, ExtraNodeFlags );
			}
		}
	}
	if( !Outside )
	{
		// We have encountered the first collision.
		Hit.Location  = Start;
		Hit.Normal    = Model.Nodes(iParent).Plane;
		Hit.Actor     = NULL;
		Hit.Primitive = &Model;
		Hit.Item      = iHit;		
	}
	return Outside;
}

//
// Classify a line as unobstructed (1) or obstructed (0).
//
int UModel::LineCheck
(
	FCheckResult	&Hit,
	AActor			*Owner,
	const FVector	&Start,
	const FVector	&End,
	DWORD			ExtraNodeFlags
)
{
	guard(UModel::LineClass);
	int Outside = RootOutside;

	if( Nodes->Num )
	{
		Outside = ::LineCheck( Hit, *this, 0, 0, 0, Start, End, Outside, ExtraNodeFlags );
		if( !Outside )
			Hit.Time = sqrt( FDistSquared(Hit.Location,Start) / FDistSquared(End,Start) );
	}
	return Outside;	
	unguard;
}

/*---------------------------------------------------------------------------------------
   Primitive BoxCheck support.
---------------------------------------------------------------------------------------*/

//
// Info used by all hull box collision routines.
//
struct FBoxCheckInfo
{
	// Hull flags.
	enum EConvolveFlags
	{
		CV_XM = 0x01,
		CV_XP = 0x02,
		CV_YM = 0x04,
		CV_YP = 0x08,
		CV_ZM = 0x10,
		CV_ZP = 0x20,
	};

	// Hull variables.
	FCheckResult	&Hit;
	UModel			&Model;
	FLOAT			Radius;
	FLOAT			Height;
	DWORD			ExtraFlags;
	INT				NumHulls;
	FVector			Min,Max,LocalHit;
	FLOAT			T0,T1;

	// Hull info.
	FPlane			Hulls[64];
	INT				Flags[64];
	const INT		*HullNodes;

	// Constructor.
	FBoxCheckInfo
	(
		FCheckResult	&InHit,
		UModel			&InModel,
		FLOAT			InRadius,
		FLOAT			InHeight,
		DWORD			InExtraFlags
	)
	:	Hit				(InHit)
	,	Model			(InModel)
	,	Radius			(InRadius)
	,	Height			(InHeight)
	,	ExtraFlags		(InExtraFlags)
	{}

	// Functions
	void SetupHulls( const FBspNode &Node )
	{
		// Get nodes on this leaf's collision hull.
		HullNodes = &Model.LeafHulls( Node.iBound );
		for( NumHulls=0; HullNodes[NumHulls]!=INDEX_NONE && NumHulls<ARRAY_COUNT(Hulls); NumHulls++ )
		{
			Hulls[NumHulls] = Model.Nodes(HullNodes[NumHulls] & ~0x40000000).Plane;
			if( HullNodes[NumHulls] & 0x40000000 )
			{
				Hulls[NumHulls].X *= -1;
				Hulls[NumHulls].Y *= -1;
				Hulls[NumHulls].Z *= -1;
				Hulls[NumHulls].W *= -1;
			}
			Flags[NumHulls]
			=	((Hulls[NumHulls].X < 0.0) ? CV_XM : (Hulls[NumHulls].X > 0.0) ? CV_XP : 0)
			|	((Hulls[NumHulls].Y < 0.0) ? CV_YM : (Hulls[NumHulls].Y > 0.0) ? CV_YP : 0)
			|	((Hulls[NumHulls].Z < 0.0) ? CV_ZM : (Hulls[NumHulls].Z > 0.0) ? CV_ZP : 0);
		}

		// Get precomputed maxima.
		const FLOAT *Temp = (FLOAT*)&Model.LeafHulls( Node.iBound + NumHulls + 1);
		Min.X = Temp[0]; Min.Y = Temp[1]; Min.Z = Temp[2];
		Max.X = Temp[3]; Max.Y = Temp[4]; Max.Z = Temp[5];
	}
};

// Hull edge convolution macro.
#define CONVOLVE_EDGE(a,b,c,mask) \
{ \
	if( (Or2 & (mask)) == (mask) ) \
	{ \
		FVector C1 = FVector(a,b,c)^Hulls[i]; \
		FVector C2 = FVector(a,b,c)^Hulls[j]; \
		if( (C1 | C2) > 0.001 ) \
		{ \
			FVector I,D;\
			FIntersectPlanes2( I, D, Hulls[i], Hulls[j] ); \
			FVector A = (FVector(a,b,c) ^ D).Normal(); \
			if( (Hulls[i] | A) < 0.0 ) \
				A *= -1.0; \
			if( !ClipTo( FPlane(I,A), INDEX_NONE ) ) \
				goto NoBlock; \
		} \
	} \
}

// Collision primitive clipper.
#define CLIP_COLLISION_PRIMITIVE \
{ \
	/* Check collision against hull planes. */ \
	FVector Hit=GMath.ZeroVector; \
	for( int i=0; i<NumHulls; i++ ) \
		if( !ClipTo( Hulls[i], HullNodes[i] & ~0x40000000) ) \
			goto NoBlock; \
	\
	/* Check collision against hull extent, minus small epsilon so flat hit nodes are identified. */ \
	if \
	(   !ClipTo( FPlane( 0.0, 0.0,-1.0,0.1-Min.Z), INDEX_NONE ) \
	||	!ClipTo( FPlane( 0.0, 0.0,+1.0,Max.Z+0.1), INDEX_NONE ) \
	||	!ClipTo( FPlane(-1.0, 0.0, 0.0,0.1-Min.X), INDEX_NONE ) \
	||	!ClipTo( FPlane(+1.0, 0.0, 0.0,Max.X-0.1), INDEX_NONE ) \
	||  !ClipTo( FPlane( 0.0,-1.0, 0.0,0.1-Min.Y), INDEX_NONE ) \
	||	!ClipTo( FPlane( 0.0,+1.0, 0.0,Max.Y-0.1), INDEX_NONE ) ) \
		goto NoBlock; \
	\
	/* Check collision against hull edges. */ \
	for( i=0; i<NumHulls; i++ ) \
	{ \
		for( int j=0; j<i; j++ ) \
		{ \
			/* Convolve with edge. */ \
			INT Or2 = Flags[i] | Flags[j]; \
			CONVOLVE_EDGE(1,0,0,CV_XM|CV_XP); \
			CONVOLVE_EDGE(0,1,0,CV_YM|CV_YP); \
			CONVOLVE_EDGE(0,0,1,CV_ZM|CV_ZP); \
		} \
	} \
}

/*---------------------------------------------------------------------------------------
   Primitive BoxPointCheck.
---------------------------------------------------------------------------------------*/

//
// Box check worker class.
//
struct FBoxPointCheckInfo : public FBoxCheckInfo
{
	// Variables.
	const FVector		Point;
	FLOAT				BestDist;

	// Constructor.
	FBoxPointCheckInfo
	(
		FCheckResult		&InHit,
		UModel				&InModel,
		const FVector		&InPoint,
		FLOAT				InRadius,
		FLOAT				InHeight,
		FLOAT				InExtraFlags
	)
	:	FBoxCheckInfo	    (InHit, InModel, InRadius, InHeight, InExtraFlags)
	,	Point				(InPoint)
	{}

	// Functions.
	int ClipTo( const FPlane &Hull, INDEX Item )
	{
		FLOAT Dist = Hull.PlaneDot( Point );
		if( Dist < BestDist )
		{
			BestDist      = Dist;
			Hit.Location  = Point + Hull * Dist;
			Hit.Normal    = Hull;
			Hit.Actor     = NULL;
			Hit.Primitive = &Model;
			Hit.Item      = Item;
			Hit.Time      = 0.0;
		}
		return Dist > FBoxPushOut( Hull, Radius, Height );
	}
	BOOL BoxPointCheck( INDEX iParent, INDEX iNode, INT Outside )
	{
		while( iNode != INDEX_NONE )
		{
			// Compute distance between start and end points and this node's plane.
			const FBspNode &Node = Model.Nodes(iNode);
			FLOAT PushOut        = FBoxPushOut(Node.Plane,Radius*1.1,Height*1.1);
			FLOAT Dist           = Node.Plane.PlaneDot(Point);

			// Recurse with front.
			if( Dist > -PushOut )
				if( BoxPointCheck( iNode, Node.iFront, Outside || Node.IsCsg(ExtraFlags) ) )
					return 1;

			// Loop with back.
			if( Dist > PushOut )
				return 0;
			iParent = iNode;
			iNode   = Node.iBack;
			Outside = Outside && !Node.IsCsg(ExtraFlags);
		}
		const FBspNode &Parent = Model.Nodes(iParent);
		if( Outside==0 && (Parent.NodeFlags & NF_SolidLeaf) )
		{
			// Reached a solid leaf, so setup hulls.
			debugState(Parent.iBound!=INDEX_NONE);
			SetupHulls(Parent);
			BestDist = 0.0;

			// Clip it.
			CLIP_COLLISION_PRIMITIVE;

			// We are embedded.
			return 1;
			NoBlock:;
		}
		return 0;
	}
};

//
// See if a box with the specified collision info fits at Point.
//
// If it doesn't fit, returns 0 and sets SuggestedOutVector to a suggested
// vector which might move the point out of the surface it's colliding with.
// SuggestedOutVector is a good guess but it's not guaranteed to be correct.
//
// If it fits, returns 1.
//
BOOL UModel::BoxPointCheck
(
	FCheckResult	&Hit,
	AActor			*Owner,
	const FVector   &Point,
	FLOAT           Radius,
	FLOAT           Height,
	DWORD           ExtraNodeFlags
)
{
	guard(UModel::BoxPointCheck);

	// Perform the check.
	BOOL Outside = RootOutside;
	if( Nodes->Num > 0 )
	{
		FBoxPointCheckInfo Check
		(
			Hit,
			*this,
			Point,
			Radius,
			Height,
			ExtraNodeFlags
		);
		Outside = Check.BoxPointCheck( 0, 0, Outside );
	}
	return Outside;
	unguard;
}

/*---------------------------------------------------------------------------------------
   Primitive BoxLineCheck.
---------------------------------------------------------------------------------------*/

//
// Tracing worker class.
//
struct FBoxLineCheckInfo : public FBoxCheckInfo
{
	// Variables.
	const FVector		Start;
	const FVector		End;
	FVector				Vector;
	FLOAT				Dist;

	// Constructor.
	FBoxLineCheckInfo
	(
		FCheckResult	&Hit,
		UModel			&InModel,
		const FVector	&InStart,
		const FVector	&InEnd,
		FLOAT			InRadius,
		FLOAT			InHeight,
		DWORD			InExtraFlags
	)
	:	FBoxCheckInfo	(Hit, InModel, InRadius, InHeight, InExtraFlags)
	,	Start			(InStart)
	,	End				(InEnd)
	,	Vector			(InEnd-InStart)
	,	Dist			(Vector.Size())
	{	
		Hit.Time = 2.0;
	}

	// Tracer.
	inline int ClipTo( const FPlane &Hull, INDEX Item )
	{
		FLOAT PushOut = FBoxPushOut(Hull,Radius,Height);
		FLOAT D0      = Hull.PlaneDot(Start);
		FLOAT D1      = Hull.PlaneDot(End);
		FLOAT T       = (D0-PushOut)/(D0-D1);

		if     ( (D0-D1) < -0.0001        )	{ if( T < T1 ) { T1 = T; }                  }
		else if( (D0-D1) > +0.0001        ) { if( T > T0 ) { T0 = T; LocalHit = Hull; } }
		else if( D0>PushOut && D1>PushOut ) { return 0;                                 }

		return T0 < T1;
	}
	void BoxLineCheck( INDEX iParent, INDEX iNode, INT Outside )
	{
		while( iNode != INDEX_NONE )
		{
			// Compute distance between start and end points and this node's plane.
			const FBspNode &Node      = Model.Nodes(iNode);
			FLOAT          D0         = Node.Plane.PlaneDot(Start);
			FLOAT          D1         = Node.Plane.PlaneDot(End);
			FLOAT          PushOut    = FBoxPushOut(Node.Plane,Radius*1.1,Height*1.1);
			INT            Use[2]     = {D0<=PushOut || D1<=PushOut, D0>=-PushOut || D1>=-PushOut};
			INT            FrontFirst = D0 >= D1;

			// Traverse down nearest side then furthest side.
			if( Use[FrontFirst] )
				BoxLineCheck( iNode, Node.iChild[FrontFirst], Node.ChildOutside(FrontFirst,Outside) );
			if( !Use[1-FrontFirst] )
				return;

			iParent = iNode;
			iNode   = Node.iChild[1-FrontFirst];
			Outside = Node.ChildOutside(1-FrontFirst,Outside);
		}
		const FBspNode &Parent = Model.Nodes(iParent);
		if( Outside==0 && (Parent.NodeFlags & NF_SolidLeaf) )
		{
			// Reached a solid leaf, so setup hulls.
			debugState(Parent.iBound!=INDEX_NONE);

			// Init.
			SetupHulls(Parent);
			T0       = 0.0; 
			T1       = Hit.Time;
			LocalHit = GMath.ZeroVector;

			// Perform collision clipping.
			CLIP_COLLISION_PRIMITIVE;

			// See if we hit.
			if( T0>0.0 && T0<T1 )
			{
				Hit.Time   = T0;
				Hit.Normal = LocalHit;
			}
			NoBlock:;
		}
	}
};

//
// Try moving a collision box from Start to End and see what it collides
// with. Returns 1 if unblocked, 0 if blocked.
//
// Note: This function assumes that Start is a valid starting point which
// is not stuck in solid space (i.e. CollisionPointCheck(Start)=1). If Start is
// stuck in solid space, the returned time will be meaningless.
//
int UModel::BoxLineCheck
(
	FCheckResult	&Hit,
	AActor			*Owner,
	const FVector   &Start,
	const FVector   &End,
	FLOAT           Radius,
	FLOAT           Height,
	DWORD           ExtraNodeFlags
)
{
	guard(UModel::BoxLineCheck);

	// Perform the trace.
	if( Nodes->Num>0 )
	{
		FBoxLineCheckInfo Trace
		(
			Hit,
			*this,
			Start,
			End,
			Radius,
			Height,
			ExtraNodeFlags
		);
		Trace.BoxLineCheck( 0, 0, RootOutside );

		// Truncate by the greater of 1% or 0.1 world units.
		//Hit.Time = Clamp( Hit.Time - Max(0.001f, 0.1f/Trace.Dist),0.f, 1.f );
		Hit.Time = Clamp( Hit.Time - Max(0.1f, 0.1f/Trace.Dist),0.f, 1.f );
		return Hit.Time==1.0;
	}
	return RootOutside ? 1.0 : 0.0;
	unguard;
}

/*---------------------------------------------------------------------------------------
   Sphere plane filtering.
---------------------------------------------------------------------------------------*/

//
// Recursive minion of UModel::PlaneFilter.
//
void PlaneFilter 
(
	UModel			&Model,
	INDEX			iNode, 
	const FVector	&Location,
	FLOAT			Radius, 
	UModel::PLANE_FILTER_CALLBACK Callback, 
	DWORD			SkipNodeFlags, 
	INT				Param
)
{
	FLOAT Dist;
	do  
	{
		FBspNode *Node	= &Model.Nodes(iNode);

		if (Node->NodeFlags & SkipNodeFlags) 
			return;

		Dist = Node->Plane.PlaneDot(Location);

		if (Dist >= -Radius)
		{
			if (Node->iFront!=INDEX_NONE)
			{
				// Recurse with front.
				PlaneFilter (Model,Node->iFront,Location,Radius,Callback,SkipNodeFlags,Param); 
			}
			if (Dist <= Radius)
			{
				// Within bounds: Do callback.
				Callback(&Model,iNode,Param);
			}
		}
		iNode = Node->iBack;
	} while ((Dist <= Radius) && (iNode != INDEX_NONE));
}

//
// Filter a sphere through the Bsp and call the specified callback for each Bsp node whose
// plane is touching the specified sphere.
//
// This only calls the callback for the first node in each coplanar chain; if the
// callback cares about coplanars, it must iterate through them itself.
//
// This is used for applying dynamic lights.
//
void UModel::PlaneFilter
(
	const FVector			&Location,
	FLOAT					Radius, 
	PLANE_FILTER_CALLBACK	Callback, 
	DWORD					SkipNodeFlags,
	INT						Param
)
{
	guard(UModel::PlaneFilter);

	if( Nodes->Num )
		::PlaneFilter(*this,0,Location,Radius,Callback,SkipNodeFlags,Param);

	unguard;
}

/*---------------------------------------------------------------------------------------
   Leaf determination.
---------------------------------------------------------------------------------------*/

//
// Return the leaf a point is in, or INDEX_NONE if none.
//
int UModel::PointLeaf
(
	const FVector	&Location,
	DWORD			ExtraNodeFlags
) const
{
	guard(UModel::PointLeaf);
	if( Nodes->Num )
	{
		INDEX iNode=0, iTempNode, Class=RootOutside, IsFront;
		do	
		{
			iTempNode = iNode;
			const FBspNode &Node = Nodes(iNode);
			if( Node.Plane.PlaneDot(Location) > 0.0 )
			{
				Class  |= Node.IsCsg(ExtraNodeFlags);
				iNode   = Node.iFront;
				IsFront = 1;
			}
			else
			{
				Class  &= ~Node.IsCsg(ExtraNodeFlags);
				iNode   = Node.iBack;
				IsFront = 0;
			}
		} while( iNode != INDEX_NONE );
		if		( !Class  )	return INDEX_NONE;
		else if ( IsFront ) return (INDEX)(Nodes(iTempNode).ZoneMask >> 32);
		else				return (INDEX)(Nodes(iTempNode).ZoneMask >>  0);
	}
	else return INDEX_NONE;
	unguard;
}

/*---------------------------------------------------------------------------------------
   Zone determination.
---------------------------------------------------------------------------------------*/

//
// Figure out which zone a point is in, and return it.  A value of
// zero indicates that the point doesn't fall into any zone.
//
BYTE UModel::PointZone( const FVector &Location ) const
{
	guard(UModel::PointZone);

	if(! Nodes->NumZones || !Nodes->Num ) 
		return 0;

	INT				Class	= 1;
	INDEX			iNode	= 0;
	ENodePlace		Place	= NODE_Front;
	const FBspNode	*Node	= &Nodes(0);

	while( iNode != INDEX_NONE )
	{
		Node	= &Nodes(iNode);
		INT CSG = Node->IsCsg();

		if( Node->Plane.PlaneDot(Location) >= 0.0 )
		{
			Class |= CSG;
			iNode = Node->iFront;
			Place = NODE_Front;
		}
		else
		{
			Class &= ~CSG;
			iNode = Node->iBack;
			Place = NODE_Back;
		}
	}
	
	if( !Class )
		return 0;

	return Node->iZone[Place==NODE_Front];
	unguard;
}

/*---------------------------------------------------------------------------------------
   Point searching.
---------------------------------------------------------------------------------------*/

//
// Find closest vertex to a point at or below a node in the Bsp.  If no vertices
// are closer than MinRadius, returns -1.
//
FLOAT FindNearestVertex
(
	const UModel	&Model, 
	const FVector	&SourcePoint,
	FVector			&DestPoint, 
	FLOAT			MinRadius, 
	INDEX			iNode, 
	INDEX			&pVertex
)
{
	FLOAT ResultRadius = -1.0;
	while (iNode != INDEX_NONE)
	{
		const FBspNode	*Node	= &Model.Nodes(iNode);
		const FBspSurf	*Surf	= &Model.Surfs(Node->iSurf);
		INDEX			iBack   = Node->iBack;

		FLOAT PlaneDist = FPointPlaneDist
		(
			SourcePoint,
			Model.Points (Surf->pBase),
			Model.Vectors(Surf->vNormal)
		);
		if ((PlaneDist >= -MinRadius) && (Node->iFront!=INDEX_NONE))
		{
			// Check front.
			FLOAT TempRadius = FindNearestVertex (Model,SourcePoint,DestPoint,MinRadius,Node->iFront,pVertex);
			if (TempRadius >= 0.0) {ResultRadius = TempRadius; MinRadius = TempRadius;};
		}
		if ((PlaneDist > -MinRadius) && (PlaneDist <= MinRadius))
		{
			// Check this node's poly's vertices.
			while (iNode != INDEX_NONE)
			{
				// Loop through all coplanars.
				Node	= &Model.Nodes(iNode);
				Surf	= &Model.Surfs(Node->iSurf);

				const FVector *Base	= &Model.Points(Surf->pBase);
				FLOAT   TempRadius	= FDistApprox (SourcePoint,*Base);

				if (TempRadius < MinRadius)
				{
					pVertex      = Surf->pBase;
					ResultRadius = TempRadius;
					MinRadius    = TempRadius;
					DestPoint    =	*Base;
				}

				const FVert *VertPool = &Model.Verts(Node->iVertPool);
				for (BYTE B=0; B<Node->NumVertices; B++)
				{
					const FVector	*Vertex		= &Model.Points(VertPool->pVertex);
					FLOAT			TempRadius	= FDistApprox (SourcePoint,*Vertex);
					if (TempRadius < MinRadius)
					{
						pVertex      = VertPool->pVertex;
						ResultRadius = TempRadius;
						MinRadius    = TempRadius;
						DestPoint    =	*Vertex;
					}
					VertPool++;
				}
				iNode = Node->iPlane;
			}
		}
		if (PlaneDist > MinRadius) 
			// Don't go down back.
			break;
		iNode = iBack;
	}
	return ResultRadius;
}

//
// Find Bsp node vertex nearest to a point (within a certain radius) and
// set the location.  Returns distance, or -1.0 if no point was found.
//
FLOAT UModel::FindNearestVertex
(
	const FVector	&SourcePoint,
	FVector			&DestPoint,
	FLOAT			MinRadius, 
	INDEX			&pVertex
) const
{
	guard(UModel::FindNearestVertex);

	if (Nodes->Num) 
		return ::FindNearestVertex( *this,SourcePoint,DestPoint,MinRadius,0,pVertex );
	else 
		return -1.0;

	unguard;
}

/*---------------------------------------------------------------------------------------
   Bound filter precompute.
---------------------------------------------------------------------------------------*/

//
// Recursive worker function for UModel::PrecomputeSphereFilter.
//
void PrecomputeSphereFilter( UModel &Model, INDEX iNode, const FPlane &Sphere )
{
	while( iNode != INDEX_NONE )
	{
		FBspNode	*Node	= &Model.Nodes(iNode);
		FBspSurf	*Surf	= &Model.Surfs(Node->iSurf);
		FLOAT		Dist	= Node->Plane.PlaneDot(Sphere);

		if( Dist < -Sphere.W )
		{
			// All back.
			Surf->PolyFlags = (Surf->PolyFlags & ~PF_IsFront) | PF_IsBack;
			iNode = Node->iBack;
		}
		else if( Dist > Sphere.W )
		{	
			// All front.
			Surf->PolyFlags = (Surf->PolyFlags & ~PF_IsBack) | PF_IsFront;
			iNode = Node->iFront;
		}
		else
		{
			// Both front and back.
			Surf->PolyFlags &= ~(PF_IsFront | PF_IsBack);

			if( Node->iBack != INDEX_NONE )
				PrecomputeSphereFilter( Model, Node->iBack, Sphere );
			iNode = Node->iFront;
		}
	}
}

//
// Precompute the front/back test for a bounding sphere.  Tags all nodes that
// the sphere falls into with a PF_IsBack tag (if the sphere is entirely in back
// of the node), a PF_IsFront tag (if the sphere is entirely in front of the node),
// or neither (if the sphere is split by the node).  This only affects nodes
// that the sphere falls in.  Thus, it is not necessary to perform any cleanup
// after precomputing the filter as long as you're sure the sphere completely
// encloses the object whose filter you're precomputing.
//
void UModel::PrecomputeSphereFilter( const FVector &Sphere )
{
	guard(UModel::PrecomputeSphereFilter);

	if( Nodes->Num )
		::PrecomputeSphereFilter( *this, 0, Sphere );

	unguard;
}

/*---------------------------------------------------------------------------------------
   The End.
---------------------------------------------------------------------------------------*/
