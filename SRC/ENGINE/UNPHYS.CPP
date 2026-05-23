/*=============================================================================
    UnPhys.cpp: Simple physics and occlusion testing for editor

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"

/*---------------------------------------------------------------------------------------
   Parameters
---------------------------------------------------------------------------------------*/

#define COLLISION_FUDGE 0.03 /* Threshold for pushing points back extra behind planes in SphereFilter*/

/*---------------------------------------------------------------------------------------
   Point classification (for particles/projectiles)
---------------------------------------------------------------------------------------*/

//
// Recursive minion of IModel::PointClass.
//
int TestPointClass 
(
	const IModel	&ModelInfo, 
	const FVector	&Location, 
	INDEX			*iDestNode, 
	INT				Class,
	DWORD			ExtraNodeFlags
)
{
	INDEX iNode = 0, iTempNode;
	do	
	{
		iTempNode			= iNode;
		FBspNode	*Node	= &ModelInfo.BspNodes [iNode];
		INT			CSG  	= Node->IsCsg(ExtraNodeFlags);

		if( Node->Plane.PlaneDot(Location) > 0.0 )
		{
			Class |= CSG;
			iNode = Node->iFront;
		}
		else
		{
			Class &= ~CSG;
			iNode = Node->iBack;
		}
	} while (iNode != INDEX_NONE);

	if (iDestNode != NULL) *iDestNode = iTempNode;
	return Class;
}

//
// Classify a point as inside (0) or outside (1), and also set its node location
// (INDEX_NONE if Bsp is empty).
//
int IModel::PointClass (const FVector &Location, INDEX *iDestNode, DWORD ExtraNodeFlags) const
{
	GUARD;
	if (NumBspNodes)
	{
   		return TestPointClass(*this,Location,iDestNode,FBspNode::ROOT_OUTSIDE,ExtraNodeFlags);
	}
	else
	{
		if (iDestNode != NULL) *iDestNode = INDEX_NONE;
   		return FBspNode::ROOT_OUTSIDE;
	}
	UNGUARD("IModel::PointClass");
}

/*---------------------------------------------------------------------------------------
   Zone determination
---------------------------------------------------------------------------------------*/

//
// Figure out which zone a point is in, and return it.  A value of
// zero indicates that the point doesn't fall into any zone.
//
BYTE IModel::PointZone (const FVector &Location) const
{
	GUARD;

	if (!NumZones || !NumBspNodes) 
		return 0;

	INT			Class	= 1;
	INDEX		iNode	= 0;
	ENodePlace	Place	= NODE_Front;
	FBspNode	*Node	= &BspNodes[0];

	while (iNode!=INDEX_NONE)
	{
		Node	= &BspNodes [iNode];
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
	
	if (!Class) 
		return 0;

	return Node->iZone[Place==NODE_Front];

	UNGUARD("IModel::PointClassPointZone");
}

/*---------------------------------------------------------------------------------------
   Fuzzy point classification (for corner radiosity)
---------------------------------------------------------------------------------------*/

//
// Recusive minion of IModel::FuzzyPointClass.
//
FLOAT FuzzyPointClass 
(
	const IModel	&ModelInfo, 
	const FVector	&Location, 
	FLOAT			RRadius, 
	INT				Outside, 
	INDEX			iNode,
	DWORD			ExtraNodeFlags
)
{
	while (iNode!=INDEX_NONE)
	{
		FBspNode	*Node	= &ModelInfo.BspNodes[iNode];
		INT			CSG		= Node->IsCsg(ExtraNodeFlags);
		FLOAT		Dist	= Node->Plane.PlaneDot(Location) * RRadius;

		if (Dist > 1.0)
		{
			Outside |= CSG;
			iNode    = Node->iFront;
		}
		else if (Dist < -1.0)
	    {
			Outside &= ~CSG;
			iNode    = Node->iBack;
		}
		else
		{
			FLOAT FrontFrac = Square(0.5 + 0.5 * Dist);

			return
				(FrontFrac    )*FuzzyPointClass(ModelInfo,Location,RRadius,Outside| CSG,Node->iFront,ExtraNodeFlags) +
				(1.0-FrontFrac)*FuzzyPointClass(ModelInfo,Location,RRadius,Outside&~CSG,Node->iBack ,ExtraNodeFlags);
		}
	}
	return Outside;
}

//
// Classify a point as inside=0.0, outside=1.0, or kinda=inbetween.
//
FLOAT IModel::FuzzyPointClass (const FVector &Location, FLOAT Radius, DWORD ExtraNodeFlags) const
{
	GUARD;

	if (NumBspNodes) 
		return ::FuzzyPointClass
		(
			*this,
			Location,
			1.0/Radius,
			FBspNode::ROOT_OUTSIDE,
			0,
			ExtraNodeFlags
			);
	else
		return FBspNode::ROOT_OUTSIDE;

	UNGUARD("IModel::FuzzyPointClass");
}

/*---------------------------------------------------------------------------------------
   Fuzzy raytracing
---------------------------------------------------------------------------------------*/

//
// Line of sight globals.
//
struct FPhysGLOS
{
	const IModel	*ModelInfo;
	FBspNode		*BspNodes;
	FVector			*FVectors;
	FVector			*FPoints;
	FVector			V0;		// Starting point, where parameter = 0.0
	FVector			V1;		// Ending point, where parameter = 1.0
	FVector			RayVector;
	FLOAT			Length;
	FLOAT			Radius,RRadius;
} GLOS;

//
// Recursive minion of IModel::FuzzyLineClass.
//
FLOAT FuzzyLineOfSight (INDEX iNode,FLOAT T0,FLOAT T1,INT Class,DWORD ExtraNodeFlags)
{
	FLOAT Factor = 1.0;
	while (iNode!=INDEX_NONE)
	{
		FBspNode	*Node	= &GLOS.BspNodes [iNode];
		int			CSG		= Node->IsCsg(ExtraNodeFlags);

		FLOAT Dist0 = Node->Plane.PlaneDot(GLOS.V0);
		FLOAT Dist1 = Node->Plane.PlaneDot(GLOS.V1);

		FLOAT Dist  = Dist1-Dist0;
		FLOAT T     = -Dist0;

		if (Dist < 0)
		{
			FLOAT Tmin = T + GLOS.Radius;
			FLOAT Tmax = T - GLOS.Radius;

			if (Tmax > T0*Dist)
			{
				// Both points are on Dist1 side.
				if (Dist1 > 0.0)	
				{
					iNode  = Node->iFront; 
					Class |= CSG;
				}
				else
				{
					iNode  = Node->iBack;  
					Class &= ~CSG;
				}
			}
			else if (Tmin < T1*Dist)
			{
				// Both points are on Dist0 side.
				if (Dist0 > 0.0)
				{
					iNode  = Node->iFront; 
					Class |= CSG;
				}
				else
				{
					iNode  = Node->iBack;  
					Class &= ~CSG;
				}
			}
			else
			{
				// Line is split.
				FLOAT RDist = 1.0/Dist;
				Tmin *= RDist;
				Tmax *= RDist;

				Factor *= FuzzyLineOfSight (Node->iFront,T0,Min(Tmax,T1),Class|CSG,ExtraNodeFlags);

				if (Factor==0.0) 
					return 0.0;

				iNode  = Node->iBack;
				Class &= ~CSG;
				T0     = Max(Tmin,T0);
			}
		}
		else
		{
			FLOAT Tmin = T - GLOS.Radius;
			FLOAT Tmax = T + GLOS.Radius;

			if (Tmax<T0*Dist)
			{
				// Both points are on Dist1 side.
				if (Dist1 > 0.0)
				{
					iNode  = Node->iFront;
					Class |= CSG;
				}
				else
				{
					iNode  = Node->iBack;
					Class &= ~CSG;
				}
			}
			else if (Tmin>T1*Dist)
			{
				// Both points are on Dist0 side.
				if (Dist0 > 0.0)
				{
					iNode  = Node->iFront; 
					Class |= CSG;
				}
				else
				{
					iNode  = Node->iBack;  
					Class &= ~CSG;
				}
			}
			else
			{
				// Line is split.
				FLOAT RDist = 1.0/Dist;
				Tmin *= RDist;
				Tmax *= RDist;

				Factor *= FuzzyLineOfSight (Node->iBack,T0,Min(Tmax,T1),Class & ~CSG,ExtraNodeFlags);

				if (Factor==0.0) 
					return 0.0;

				iNode	= Node->iFront;
				Class  |= CSG;
				T0		= Max(Tmin,T0);
			}
		}
	}
	if (!Class)
	{
		Factor *= 1.0 - 0.5 * (T1-T0) * (T1-T0) * GLOS.Length * GLOS.RRadius;
		if (Factor<0.0) Factor = 0.0;
	}
	return Factor;
}

//
// Classify a line as unobstructed (1.0), obstructed (0.0), or somewhere
// in between, based on a radius.  Note that this function can penetrate
// walls of thickness less than Radius.
//
FLOAT IModel::FuzzyLineClass (const FVector &V1, const FVector &V2, FLOAT Radius, DWORD ExtraNodeFlags) const
{
	GUARD;

	GLOS.V0			= V1;
	GLOS.V1			= V2;
	GLOS.ModelInfo	= this;
	GLOS.FVectors	= FVectors;
	GLOS.FPoints	= FPoints;
	GLOS.BspNodes	= BspNodes;
	GLOS.Radius		= Radius;
	GLOS.RRadius	= 1.0/Radius;

	GLOS.RayVector = V2 - V1;
	GLOS.Length    = GLOS.RayVector.Size();

	if (NumBspNodes) 
		return FuzzyLineOfSight (0,0.0,1.0,FBspNode::ROOT_OUTSIDE,ExtraNodeFlags);
	else 
		return FBspNode::ROOT_OUTSIDE;

	UNGUARD("IModel::FuzzyLineClass");
}

/*---------------------------------------------------------------------------------------
   Line classification (line-of-sight)
---------------------------------------------------------------------------------------*/

//
// Recursive minion of IModel::LineClass.
//
int LineClass (const IModel &ModelInfo, INDEX iNode, const FVector &V1, const FVector &V2, 
	INT Outside, DWORD ExtraNodeFlags)
{
	while( iNode != INDEX_NONE )
	{
		FBspNode*	Node	= &ModelInfo.BspNodes [iNode];
		INT			CSG		= Node->IsCsg(ExtraNodeFlags);

		// Check side-of-plane for both points.
		FLOAT		Dist1	= Node->Plane.PlaneDot(V1);
		FLOAT		Dist2	= Node->Plane.PlaneDot(V2);

		// Classify line based on both distances.
		if ((Dist1 > -0.001) && (Dist2 > -0.001))
		{
			// Both points are in front.
			Outside |= CSG;
			iNode  = Node->iFront;
		}
		else if ((Dist1 < 0.001)&&(Dist2 < 0.001))
		{
			// Both points are in back.
			Outside &= !CSG;
			iNode  = Node->iBack;
		}
		else
		{
			// Line is split (guranteed to be non-parallel to plane, so TimeDenominator != 0).
			FVector MidVector = V1 + (V1-V2) * (Dist1/(Dist2-Dist1));

			if (Dist1 > 0.0) // Dist2 < 0.
			{
				if (!LineClass  (ModelInfo,Node->iFront,V1,MidVector,Outside || CSG,ExtraNodeFlags)) return 0;
				return LineClass(ModelInfo,Node->iBack,MidVector,V2,Outside && !CSG,ExtraNodeFlags);
			}
			else // Dist1 < 0, Dist2 > 0.
			{
				if (!LineClass  (ModelInfo,Node->iFront,V2,MidVector,Outside || CSG,ExtraNodeFlags)) return 0;
				return LineClass(ModelInfo,Node->iBack,MidVector,V1,Outside && !CSG,ExtraNodeFlags);
			}
		}
	}
	return Outside;
}

//
// Classify a line as unobstructed (1) or obstructed (0).
//
int IModel::LineClass( const FVector &V1, const FVector &V2, DWORD ExtraNodeFlags ) const
{
	guard(IModel::LineClass);

	if (NumBspNodes)
		return ::LineClass (*this,0,V1,V2,FBspNode::ROOT_OUTSIDE,ExtraNodeFlags);
	else
		return FBspNode::ROOT_OUTSIDE;
	
	unguard;
}

/*---------------------------------------------------------------------------------------
   Raytracing with an optional leaf callback
---------------------------------------------------------------------------------------*/

struct FRaytraceParams
{
	IModel::RAYTRACE_CALLBACK Callback;
	FVector				*HitLocation;
	FVector				*HitPlane;
	INDEX				*HitNode;
	FLOAT				ConeRadiusFactor;
	INT					Param;
	INT					Stop;
	DWORD				ExtraNodeFlags;
};

//
// Recursive minion of IModel::Raytrace
//
int Raytrace 
(
	IModel			&ModelInfo, 
	INDEX			iNode, 
	FVector			V1, 
	FVector			V2, 
	INT				Class,
	FRaytraceParams &Params
)
{
	int TestResult;

	while( !Params.Stop )
	{
		FBspNode	*Node	= &ModelInfo.BspNodes	[iNode];
		int			CSG		= Node->IsCsg(Params.ExtraNodeFlags);

		// Check side-of-plane for both points.
		FLOAT		Dist1	= Node->Plane.PlaneDot(V1);
		FLOAT		Dist2	= Node->Plane.PlaneDot(V2);

		// Classify line based on both distances.
		if ((Dist1 > -0.001) && (Dist2 > -0.001))
		{
			// Both points are in front.
			DoFront:
			Class = Class || CSG;
			if (Node->iFront==INDEX_NONE)
			{
				// Front leaf.
				if (Class) 
					// Outside leaf.
					Params.Stop = Params.Callback(&ModelInfo,iNode,0,Params.Param);
				else 
					// Inside leaf.
					Params.Stop = 1;
				return Class;
			}
			iNode = Node->iFront;
		}
		else if ( (Dist1 < 0.001) && (Dist2 < 0.001) )
		{
			// Both points are in back.
			DoBack:
			Class = Class && !CSG;
			if (Node->iBack==INDEX_NONE)
			{
				// Back leaf.
				if (Class)
					// Outside leaf.
					Params.Stop = Params.Callback(&ModelInfo,iNode,1,Params.Param);
				else
					// Inside leaf.
					Params.Stop = 1;
				return Class;
			}
			iNode = Node->iBack;
		}
		else
		{
			// Line is split.
			// It's guranteed to be non-parallel to plane, so TimeDenominator != 0.
			// Now we go and raytrace the nearest part then the furthest part.

			FVector MidVector = V1 + (V1 - V2) * (Dist1/(Dist2-Dist1));

			if (Dist1 > 0.0) // Dist2 < 0
			{
				// Process nearest part (front).
				int FrontClass = Class || CSG;
				if (Node->iFront!=INDEX_NONE)
				{
					// Recurse down front.
					TestResult = Raytrace (ModelInfo,Node->iFront,V1,MidVector,FrontClass,Params);

					if (Params.Stop) 
						return TestResult;
				}
				else
				{	
					// Back leaf.
					if (FrontClass) 
						// Outside leaf.
						Params.Stop = Params.Callback(&ModelInfo,iNode,0,Params.Param);
					else
						// Inside leaf.
						Params.Stop = 1;

					if (Params.Stop) 
						return FrontClass;
				}
				// Handle CSG outside->inside transition.
				if (CSG)
				{
					*Params.HitLocation	= MidVector;
					*Params.HitPlane	= Node->Plane;
					*Params.HitNode		= iNode;
				}
				// Process furthest part (back).
				V1 = MidVector;
				goto DoBack;
			}
			else // Dist1 < 0, Dist2 > 0.
			{
				// Process nearest part (back):
				int BackClass = Class && !CSG;
				if (Node->iBack!=INDEX_NONE)
				{
					// Recurse down back.
					TestResult = Raytrace (ModelInfo,Node->iBack,V1,MidVector,BackClass,Params);
					if (Params.Stop) return TestResult;
				}
				else
				{
					// Inside leaf.
					if (BackClass)
						Params.Stop = Params.Callback(&ModelInfo,iNode,1,Params.Param);
					else
						Params.Stop = 1; // Inside leaf

					if (Params.Stop)
						return BackClass;
				}

				// Process furthest part (front).
				V1 = MidVector;
				goto DoFront;
			}
		}
	}
	return Class;
}

//
// Filter a line through the Bsp, calling the specified callback for each leaf reached
// (in order, starting at V1's leaf and ending at V2's).  Continues until the entire
// line has been raytraced, or the callback returns a zero, signaling that tracing should
// stop.
//
// Returns 1 if unobstructed, 0 if obstructed.
//
// If obstructed, sets HitLocation, HitNormal, and HitNode according to the closest point
// that was hit.  If the ray is obstructed without hitting anything (i.e. if the source
// point is obstructed), sets HitLocation to the source location, HitNormal to the Z-Axis 
// Vector, and sets HitNode to INDEX_NONE.
//
// If ConeRadiusFactor is zero, does a normal raytrace operation.  If ConeRadiusFactor
// is positive, the raytrace is performed as a cone trace, where the cone's distance
// is equal to ConeRadiusFactor times the distance of the test point from V1 along the line
// from V1 to V2.
//
// Purpose: Actor/projectile collision, actor weapon targeting.
//
int IModel::Raytrace
(
	const FVector		&V1, 
	const FVector		&V2, 
	FLOAT				ConeRadiusFactor,
	RAYTRACE_CALLBACK	Callback,
	INT					Param,
	FVector				&HitLocation,
	FVector				&HitPlane,
	INDEX				&iHitNode,
	DWORD				ExtraNodeFlags
)
{
	GUARD;
	if (!NumBspNodes) 
		return FBspNode::ROOT_OUTSIDE;

	// Defaults.
	HitLocation = V1;
	HitPlane    = GMath.ZAxisVector;
	iHitNode    = INDEX_NONE;

	// Set up Params.
	FRaytraceParams Params;
	Params.Callback				= Callback;
	Params.HitLocation			= &HitLocation;
	Params.HitPlane				= &HitPlane;
	Params.HitNode				= &iHitNode;
	Params.ConeRadiusFactor		= ConeRadiusFactor;
	Params.Param				= Param;
	Params.Stop					= 0;
	Params.ExtraNodeFlags		= ExtraNodeFlags;

	// Perform raytrace.
	int Result = ::Raytrace (*this,0,V1,V2,FBspNode::ROOT_OUTSIDE,Params);

	// If raytracer ran into a fake backdrop surface, treat it as unoccluded.
	if ((Result==0) && (iHitNode != INDEX_NONE))
	{
		FBspNode *Node = &BspNodes[iHitNode];
		FBspSurf *Surf = &BspSurfs[Node->iSurf];
		if (Surf->PolyFlags & PF_FakeBackdrop) 
			// Unoccluded.
			return 1;
		else
			// Occluded.
			return 0;
	}
	else
		// Unoccluded.
		return 1;

	UNGUARD("IModel::Raytrace");
}

/*---------------------------------------------------------------------------------------
	Z axis-aligned ray filtering for player collision
---------------------------------------------------------------------------------------*/

//
// Recursive minion of IModel::ZCollision.
//
INDEX FilterZRay 
(
	const IModel	&ModelInfo, 
	INDEX			iNode, 
	INDEX			iHit, 
	FVector			V, 
	FLOAT			EndZ, 
	FVector			&Hit, 
	INDEX			&iActorHit, 
	INT				Outside,
	DWORD			ExtraNodeFlags)
{
	while( iNode != INDEX_NONE )
	{
		// Setup.
		FBspNode	*Node	= &ModelInfo.BspNodes [iNode];
		INT			CSG  	= Node->IsCsg(ExtraNodeFlags);
		INT			Split;

		// Check side-of-plane for point, then find Z intersection based on it.
		FLOAT Dist = Node->Plane.PlaneDot(V);
		FLOAT DZ;

		if( (Node->Plane.Z<-0.001) || (Node->Plane.Z>0.001) )
		{
			DZ		= -Dist / Node->Plane.Z;
			Split	= (DZ < 0.0) && ((V.Z + DZ) > EndZ);
		}
		else
		{
			Split	= 0;
			DZ		= 0.0;
		}

		// Classify line based on both distances.
		if (!Split)
		{
			if( Dist > 0.0 )
			{
				// Entire ray is in front of node, no intersection.
				Outside |= CSG;
				iNode    = Node->iFront;
			}
			else // Dist <= 0.0.
			{
				// Entire ray is in back of node, no intersection.
				Outside &= ~CSG;
				iNode    = Node->iBack;

				if (CSG)
					iActorHit = ModelInfo.BspSurfs[Node->iSurf].iActor;
			}
		}
		else
		{
			// Ray is split.  Process both halves in nearest-to-furthest order.
			if (Dist > 0.0)
			{
				// Front then back.
				INDEX Result = FilterZRay(ModelInfo,Node->iFront,iHit,V,V.Z + DZ,Hit,iActorHit,Outside | CSG,ExtraNodeFlags);
				if (Result != INDEX_NONE) return Result;

				V.Z      += DZ;
				if (CSG)
				{
					Hit = V; 
					iHit = iNode;
					iActorHit = ModelInfo.BspSurfs[Node->iSurf].iActor;
				};
				Outside  &= ~CSG;
				iNode     = Node->iBack;
			}
			else
			{
				// Back then front.
				INDEX iTemp = iActorHit;

				if (CSG) 
					iActorHit = ModelInfo.BspSurfs[Node->iSurf].iActor;

				INDEX Result = FilterZRay(ModelInfo,Node->iBack,iHit,V,V.Z + DZ,Hit,iActorHit,Outside & !CSG,ExtraNodeFlags);
				if (Result != INDEX_NONE) return Result;

				iActorHit = iTemp;

				V.Z      += DZ;
				if (CSG) 
				{
					Hit = V; 
					iHit = iNode;
				};
				Outside  |= CSG;
				iNode     = Node->iFront;
			}
		}
	}
	if (Outside)
		return INDEX_NONE;
	else
		return iHit;
}

//
// Filter a Z-aligned ray down the Bsp starting at a point, and find the first thing
// it hits.  Returns the Bsp node index for the collision, or INDEX_NONE of no collision.
// If collision, sets Hit vector to location of intersection.
//
INDEX IModel::ZCollision(const FVector &V, FVector &Hit, INDEX &iActorHit, DWORD ExtraNodeFlags) const
{
	GUARD;

	// Defaults.
	iActorHit	= INDEX_NONE;
	Hit			= V;

	if (NumBspNodes)
		return FilterZRay (*this,0,INDEX_NONE,V,V.Z - 100000.0,Hit,iActorHit,FBspNode::ROOT_OUTSIDE,ExtraNodeFlags);
	else
		return INDEX_NONE;

	UNGUARD("IModel::ZCollision");
}

/*---------------------------------------------------------------------------------------
   Sphere collision
---------------------------------------------------------------------------------------*/

class FCollisionLocals
{
	public:
	FLOAT 		BackAdjustmentSize;
	FVector 	BackAdjustment;
};

class FCollisionGlobals
{
	public:
	FLOAT		AdjustmentSize;
	FVector		Adjustment;
	FVector		MoveVector;
	DWORD		ExtraNodeFlags;
	INT			FavorSmoothness;
};

//
// Filter through all nodes that the sphere lies in, down to leaves.
// If within any inside leaf, classify as inside.
//
// Returns 1 if collision (sphere is inside or sphere has collided with a polygon),
// or 0 if no collison.  If collision, Globals.AdjustmentSize is a suggested adjustment
// vector to get the sphere out of the wall.
//
int SphereCollision 
(
	const IModel		&ModelInfo, 
	INDEX				iNode, 
	const FVector		&Dest,
	FCollisionLocals	&Locals, 
	FCollisionGlobals	&Globals, 
	FLOAT				Radius, 
	INT					Outside
)
{
	// Loop through convex volumes (backs) while recursing with fronts.
	int Collision = 0;

	BackLoop:
	FBspNode	*Node	= &ModelInfo.BspNodes [iNode];
	FBspSurf	*Surf	= &ModelInfo.BspSurfs [Node->iSurf];
	int			CSG		= Node->IsCsg(Globals.ExtraNodeFlags);

	FLOAT		Dist;
	if (!(Surf->PolyFlags & PF_HighLedge))
	{
		Dist = Node->Plane.PlaneDot(Dest);
	}
	else
	{
		FVector TempAdjustment = ModelInfo.FVectors[Surf->vNormal] * 56.0 + ModelInfo.FPoints[Surf->pBase];
		Dist = FPointPlaneDist(Dest,TempAdjustment,ModelInfo.FVectors[Surf->vNormal]);
	}

	// Calculate collision adjustment.  Will be replaced if sphere collides
	// with a poly further down the tree's back.

	// Check front (outside).
	if( Dist >= -Radius )
	{
		FCollisionLocals NewLocals = Locals; 

		if( CSG && (Dist <= Radius) )
		{
			FVector TempAdjustment = Node->Plane * (Radius + COLLISION_FUDGE + Dist);
			FLOAT TempAdjustmentSize;

			if (Globals.FavorSmoothness)
			{
				// Favor small movements (looking for minimum adjustment size).
				TempAdjustmentSize = TempAdjustment.Size();
				
				if (Globals.FavorSmoothness!=2) 
					TempAdjustmentSize /= Node->Plane | Globals.MoveVector;
				
				TempAdjustmentSize = Abs(TempAdjustmentSize);
			}
			else
			{
				// Favor large, non-floor movements.
				TempAdjustmentSize = -(1.0 + Abs(TempAdjustment.X) + Abs(TempAdjustment.Y));
			}

			// Fix adjustment so it doesn't try to move the player up/down.
			if ((Node->Plane.Z!=0.0) && (Abs(Node->Plane.Z)<0.85) && (Globals.FavorSmoothness != 2))
			{
				FLOAT Temp = 1.02 / Node->Plane.Size2D();
				TempAdjustment.X *= Temp;
				TempAdjustment.Y *= Temp;
				TempAdjustment.Z  = 0;
			}

			// Update new locals.
			if( (NewLocals.BackAdjustmentSize==0.0) || (TempAdjustmentSize <= NewLocals.BackAdjustmentSize) )
			{
				NewLocals.BackAdjustment		= TempAdjustment;
      			NewLocals.BackAdjustmentSize	= TempAdjustmentSize;
			}
		}
		
		if( Node->iFront != INDEX_NONE )
		{
			Collision |= SphereCollision( ModelInfo, Node->iFront, Dest, NewLocals, Globals, Radius, Outside | CSG );
		}
		else 
		{
			Collision = Collision || (!Outside && !CSG );
		}
	}

	if (CSG && (Dist >= -Radius) && (Dist <= Radius))
	{
		FVector TempAdjustment = Node->Plane * (Radius + COLLISION_FUDGE - Dist);
		FLOAT TempAdjustmentSize;

		if (Globals.FavorSmoothness)
		{
			// Favor small movements (looking for minimum adjustment size).
			TempAdjustmentSize = TempAdjustment.Size();
			
			if (Globals.FavorSmoothness!=2) 
				TempAdjustmentSize /= Node->Plane | Globals.MoveVector;
			
			TempAdjustmentSize = Abs(TempAdjustmentSize);
		}
		else
		{
			// Favor large, non-floor movements.
			TempAdjustmentSize = -(1.0 + Abs(TempAdjustment.X) + Abs(TempAdjustment.Y));
		}

		// Fix adjustment so it doesn't try to move the player up/down.
		if ((Node->Plane.Z!=0.0) && (Abs(Node->Plane.Z)<0.85) && (Globals.FavorSmoothness != 2))
		{
			FLOAT Temp = 1.02 / Node->Plane.Size2D();
			TempAdjustment.X *= Temp;
			TempAdjustment.Y *= Temp;
			TempAdjustment.Z  = 0;
		}

		// Update locals.
		if( (Locals.BackAdjustmentSize==0.0) || (TempAdjustmentSize <= Locals.BackAdjustmentSize) )
		{
			Locals.BackAdjustment		= TempAdjustment;
      		Locals.BackAdjustmentSize	= TempAdjustmentSize;
		}
	}
	
	// Loop with back.
	if (Dist <= Radius)
	{
		Outside &= !CSG;
		iNode    = Node->iBack;

		if (iNode!=INDEX_NONE)
			goto BackLoop;

	   	if (!Outside)
		{
      		if
			(
				(( Globals.FavorSmoothness) && (Locals.BackAdjustmentSize >= Globals.AdjustmentSize)) ||
				((!Globals.FavorSmoothness) && ((Globals.AdjustmentSize==0.0)||(Locals.BackAdjustmentSize < Globals.AdjustmentSize)))
			)
			{
         		Globals.Adjustment     = Locals.BackAdjustment;
         		Globals.AdjustmentSize = Locals.BackAdjustmentSize;
         	}
			return 1;
		}
	}
	return Collision;
}

//
// Find reflection vector of a sphere colliding with the world.
// Assumes that sphere has collided with a wall.
// Returns 1 if reflected successfully, 0 if unreflectable.
//
int IModel::SphereReflect
(
	FVector		&Location,
	FLOAT		Radius, 
	FVector		&ReflectionVector
) const 
{
	GUARD;

	FVector				TempLocation = Location;
	FCollisionGlobals	Globals;
	FCollisionLocals	Locals;

	if (NumBspNodes)
	{
		Globals.FavorSmoothness		= 0;
		Globals.AdjustmentSize 		= 0.0;
		Globals.ExtraNodeFlags		= 0;
		Globals.MoveVector			= GMath.ZeroVector;
		Locals.BackAdjustmentSize 	= 0.0;
		Locals.BackAdjustment 		= GMath.ZeroVector;

		if (!SphereCollision(*this,0,TempLocation,Locals,Globals,Radius,FBspNode::ROOT_OUTSIDE)) 
			return 0;

		ReflectionVector = Globals.Adjustment;
		return 1;
	}
	else return FBspNode::ROOT_OUTSIDE;

	UNGUARD("IModel::SphereReflect");
}

//
// Try to move a sphere by a movement vector, which must be less
// than the sphere's radius.  If fits, returns 1.  If blocked, returns
// 0, and sets Adjustment to a suggested adjustment which might move
// the sphere out of the wall.
//
int IModel::SphereTestMove
(
	const FVector	&Location, 
	const FVector	&Delta, 
	FVector			&ResultAdjustment,
	FLOAT			Radius, 
	INT				FavorSmoothness,
	DWORD			ExtraNodeFlags
) const
{
	if (NumBspNodes)
	{
		FCollisionGlobals Globals;
		Globals.FavorSmoothness		= FavorSmoothness;
		Globals.Adjustment			= GMath.ZeroVector;
		Globals.AdjustmentSize 		= 0.0;
		Globals.MoveVector			= Delta;
		Globals.ExtraNodeFlags		= ExtraNodeFlags;

		FCollisionLocals Locals;
		Locals.BackAdjustment 		= GMath.ZeroVector;
		Locals.BackAdjustmentSize 	= 0.0;
		int Result = !SphereCollision(*this,0,Location,Locals,Globals,Radius,FBspNode::ROOT_OUTSIDE);
		ResultAdjustment = Globals.Adjustment;
		return Result;
	}
	else return FBspNode::ROOT_OUTSIDE;
}

//
// Try to move a sphere by a movement vector, which must be less
// than the sphere's radius.  If it doesn't fit, retries a few times
// in order to properly collide with multiple simultaneous walls.
//
// Glides along walls.
//
// Returns 1 if full or partial movement occured, 0 if no movement.
//
int IModel::SphereNearMove
(
	FVector			&Location,
	const FVector	&Delta, 
	FLOAT			Radius, 
	INT				FavorSmoothness,
	DWORD			ExtraNodeFlags
) const
{
	GUARD;
	FVector	NewLocation	= Location + Delta;
	int		Iteration	= 0;
	int		Moved		= 0;

	while (Iteration++ < 4)
	{
		FVector Adjustment;
		if (SphereTestMove(NewLocation,Delta,Adjustment,Radius,FavorSmoothness,ExtraNodeFlags))
		{
			// Moved without collision on this iteration - we have a valid destination.
			Location = NewLocation;
			Moved    = 1; 
			break;
		}
		// Collided; adjust and try again.
		NewLocation += Adjustment;
	}
	return Moved;
	UNGUARD("IModel::SphereNearMove");
}

//
// Try to move a sphere by a movement vector, which is allowed to
// be larger than the sphere's radius.  This function works by
// subdividing the movement vector into intervals of size less than
// the radius, and calling IModel::SphereNearMove to do the world.
//
// Glides along walls.
// Returns 1 if full or partial movement occured, 0 if no movement.
//
int IModel::SphereMove 
(
	FVector			&Location,
	const FVector	&Delta, 
	FLOAT			Radius, 
	INT				FavorSmoothness,
	DWORD			ExtraNodeFlags
) const
{
	GUARD;

	if (NumBspNodes)
	{
		int Steps = (int)(Delta.Size() * 4.00 / Radius);

		if (Steps<1) Steps = 1;
		FVector NewDelta = Delta/(FLOAT)Steps;

		int Moved = 0;
		while (Steps-- > 0) Moved |= SphereNearMove (Location,NewDelta,Radius,FavorSmoothness,ExtraNodeFlags);

		return Moved;
	}
	else
	{
		Location += Delta;
		return 1;
	}
	UNGUARD("IModel::SphereMove");
}

/*---------------------------------------------------------------------------------------
   Sphere plane filtering
---------------------------------------------------------------------------------------*/

//
// Recursive minion of IModel::PlaneFilter
//
void PlaneFilter 
(
	IModel			&ModelInfo,
	INDEX			iNode, 
	const FVector	&Location,
	FLOAT			Radius, 
	IModel::PLANE_FILTER_CALLBACK Callback, 
	DWORD			SkipNodeFlags, 
	INT				Param
)
{
	FLOAT Dist;

	do  
	{
		FBspNode *Node	= &ModelInfo.BspNodes[iNode];

		if (Node->NodeFlags & SkipNodeFlags) 
			return;

		Dist = Node->Plane.PlaneDot(Location);

		if (Dist >= -Radius)
		{
			if (Node->iFront!=INDEX_NONE)
			{
				// Recurse with front.
				PlaneFilter (ModelInfo,Node->iFront,Location,Radius,Callback,SkipNodeFlags,Param); 
			}
			if (Dist <= Radius)
			{
				// Within bounds: Do callback.
				Callback(&ModelInfo,iNode,Param);
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
void IModel::PlaneFilter
(
	const FVector			&Location,
	FLOAT					Radius, 
	PLANE_FILTER_CALLBACK	Callback, 
	DWORD					SkipNodeFlags,
	INT						Param
)
{
	GUARD;

	if (NumBspNodes) 
		::PlaneFilter(*this,0,Location,Radius,Callback,SkipNodeFlags,Param);

	UNGUARD("IModel::PlaneFilter");
}

/*---------------------------------------------------------------------------------------
   Sphere leaf filtering
---------------------------------------------------------------------------------------*/

//
// Recursive minion of IModel::SphereLeafFilter
//
void SphereLeafFilter 
(
	IModel			&ModelInfo,
	INDEX			iNode, 
	const FVector	&Location,
	FLOAT			Radius, 
	IModel::SPHERE_FILTER_CALLBACK Callback, 
	INT				Outside, 
	DWORD			SkipNodeFlags,
	INT				Param
)
{
	for( ;; )
	{
		FBspNode *Node = &ModelInfo.BspNodes [iNode];
		FLOAT Dist = Node->Plane.PlaneDot(Location);

		if (Dist >= -Radius)
		{
			if (Node->iFront != INDEX_NONE)
				// Recurse with front.
				::SphereLeafFilter(ModelInfo,Node->iFront,Location,Radius,Callback,Outside || Node->IsCsg(),SkipNodeFlags,Param);
			else
				// Call callback with front.
				Callback(&ModelInfo,iNode,0,Outside || Node->IsCsg(), Param);
		}

		if (Dist>Radius) 
			break;

		Outside = Outside && !Node->IsCsg();

		if( Node->iBack == INDEX_NONE )
		{
			// Call callback with back.
			Callback(&ModelInfo,iNode,1,Outside,Param);
			break;
		}
		iNode = Node->iBack;
	}
}

//
// Filter a sphere through the Bsp and call the specified callback for each Bsp leaf that
// a part of the sphere falls in.  This uses inexact radius-based comparison, which
// can treat the sphere as in a wedge leaf that it's not actually in.
//
void IModel::SphereLeafFilter
(
	const FVector			&Location,
	FLOAT					Radius,
	SPHERE_FILTER_CALLBACK	Callback, 
	DWORD					SkipNodeFlags,
	INT						Param
)
{
	GUARD;

	if (NumBspNodes) 
		::SphereLeafFilter(*this,0,Location,Radius,Callback,FBspNode::ROOT_OUTSIDE,SkipNodeFlags,Param);

	UNGUARD("IModel::SphereLeafFilter");
}

/*---------------------------------------------------------------------------------------
   Point searching
---------------------------------------------------------------------------------------*/

//
// Find closest vertex to a point at or below a node in the Bsp.  If no vertices
// are closer than MinRadius, returns -1.
//
FLOAT FindNearestVertex
(
	const IModel	&ModelInfo, 
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
		FBspNode	*Node	= &ModelInfo.BspNodes [iNode];
		FBspSurf	*Surf	= &ModelInfo.BspSurfs [Node->iSurf];
		INDEX		iBack   = Node->iBack;

		FLOAT PlaneDist = FPointPlaneDist
		(
			SourcePoint,
			ModelInfo.FPoints [Surf->pBase],
			ModelInfo.FVectors[Surf->vNormal]
		);
		if ((PlaneDist >= -MinRadius) && (Node->iFront!=INDEX_NONE))
		{
			// Check front.
			FLOAT TempRadius = FindNearestVertex (ModelInfo,SourcePoint,DestPoint,MinRadius,Node->iFront,pVertex);
			if (TempRadius >= 0.0) {ResultRadius = TempRadius; MinRadius = TempRadius;};
		}
		if ((PlaneDist > -MinRadius) && (PlaneDist <= MinRadius))
		{
			// Check this node's poly's vertices.
			while (iNode != INDEX_NONE)
			{
				// Loop through all coplanars.
				Node				= &ModelInfo.BspNodes [iNode];
				Surf				= &ModelInfo.BspSurfs [Node->iSurf];

				FVector *Base		= &ModelInfo.FPoints[Surf->pBase];
				FLOAT   TempRadius	= FDistApprox (SourcePoint,*Base);

				if (TempRadius < MinRadius)
				{
					pVertex      = Surf->pBase;
					ResultRadius = TempRadius;
					MinRadius    = TempRadius;
					DestPoint    =	*Base;
				}
				FVertPool *VertPool = &ModelInfo.VertPool[Node->iVertPool];
				for (BYTE B=0; B<Node->NumVertices; B++)
				{
					FVector *Vertex		= &ModelInfo.FPoints [VertPool->pVertex];
					FLOAT   TempRadius	= FDistApprox (SourcePoint,*Vertex);
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
FLOAT IModel::FindNearestVertex
(
	const FVector	&SourcePoint,
	FVector			&DestPoint,
	FLOAT			MinRadius, 
	INDEX			&pVertex
) const
{
	GUARD;

	if (NumBspNodes) 
		return ::FindNearestVertex( *this,SourcePoint,DestPoint,MinRadius,0,pVertex );
	else 
		return -1.0;

	UNGUARD("IModel::FindNearestVertex");
}

/*---------------------------------------------------------------------------------------
   Bound filter precompute
---------------------------------------------------------------------------------------*/

//
// Recursive worker function for IModel::PrecomputeSphereFilter.
//
void PrecomputeFilter(IModel &ModelInfo,INDEX iNode,const FVector &Sphere)
{
	while (iNode!=INDEX_NONE)
	{
		FBspNode	*Node	= &ModelInfo.BspNodes[iNode];
		FBspSurf	*Surf	= &ModelInfo.BspSurfs[Node->iSurf];
		FLOAT		Dist	= Node->Plane.PlaneDot(Sphere);

		if (Dist < -Sphere.W)
		{
			// All back.
			Surf->PolyFlags = (Surf->PolyFlags & ~PF_IsFront) | PF_IsBack;
			iNode = Node->iBack;
		}
		else if (Dist > Sphere.W)
		{	
			// All front.
			Surf->PolyFlags = (Surf->PolyFlags & ~PF_IsBack) | PF_IsFront;
			iNode = Node->iFront;
		}
		else
		{
			// Both front and back.
			Surf->PolyFlags &= ~(PF_IsFront | PF_IsBack);

			if (Node->iBack!=INDEX_NONE) PrecomputeFilter(ModelInfo,Node->iBack,Sphere);
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
void IModel::PrecomputeSphereFilter(const FVector &Sphere)
{
	if (NumBspNodes) 
		PrecomputeFilter(*this,0,Sphere);
}

/*---------------------------------------------------------------------------------------
   The End
---------------------------------------------------------------------------------------*/
