/*=============================================================================
	UnMath.cpp: Unreal math routines, implementation of FGlobalMath class

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "Float.h"

/*-----------------------------------------------------------------------------
	FGlobalMath class functions.
-----------------------------------------------------------------------------*/

//
// Init math subsystem.
//
void FGlobalMath::Init()
{
	guard(FGlobalMath::Init);

	double	Angle		= 0.0;
	double	AngleInc	= 2.0*PI / (FLOAT)NUM_ANGLES;
	int		i;

	// Init angular sine and cosine tables.
	for( i=0; i<NUM_ANGLES; i++ )
	{
		SinFLOAT [i]=sin(Angle);
		CosFLOAT [i]=cos(Angle);
		Angle+=AngleInc;
	}
	for( i=0; i<NUM_SQRTS; i++ )
	{
		FLOAT S				= sqrt((FLOAT)(i+1) * (1.0/(FLOAT)NUM_SQRTS));
		FLOAT Temp			= (1.0-S);// Was (2*S*S*S-3*S*S+1);
		LightSqrtFLOAT[i]	= Temp/S;
		SqrtFLOAT[i]		= sqrt((FLOAT)i / 16384.0);
	}

	// Init all constants.
	UnitVector.Make				(1.0,1.0,1.0);
	ZeroVector.Make				(0.0,0.0,0.0);
	XAxisVector.Make			(1.0,0.0,0.0);
	YAxisVector.Make			(0.0,1.0,0.0);
	ZAxisVector.Make			(0.0,0.0,1.0);
	WorldMin.Make				(-32700.0,-32700.0,-32700.0);
	WorldMax.Make				(32700.0,32700.0,32700.0);
	VectorMax.Make				(+(FLOAT)MAXSWORD,+(FLOAT)MAXSWORD,+(FLOAT)MAXSWORD);
	VectorMin.Make				(-(FLOAT)MAXSWORD,-(FLOAT)MAXSWORD,-(FLOAT)MAXSWORD);
	UnitScaleVect.Make			(1.0,1.0,1.0);
	UnitCoords.Make				(ZeroVector,XAxisVector,YAxisVector,ZAxisVector);
	UnitScale.Make				(UnitVector,0.0,SHEER_ZX);
	ZeroRotation.Make			(0,0,0);
	CameraViewCoords.Make 		(ZeroVector,YAxisVector,-ZAxisVector,XAxisVector);
	UncameraViewCoords.Make		(ZeroVector,ZAxisVector,XAxisVector,YAxisVector);
	SixViewRotations[0].Make	(0x4000,0     ,0); // Up (pitch, yaw, roll)
	SixViewRotations[1].Make	(0xC000,0     ,0); // Down
	SixViewRotations[2].Make	(0     ,0     ,0); // North
	SixViewRotations[3].Make	(0     ,0x8000,0); // South
	SixViewRotations[4].Make	(0     ,0xC000,0); // East
	SixViewRotations[5].Make	(0     ,0x4000,0); // West

	unguard;
}

//
// Shut down math subsystem.
//
void FGlobalMath::Exit()
{
	guard(FGlobalMath::Exit);
	unguard;
}

/*-----------------------------------------------------------------------------
	Conversion functions.
-----------------------------------------------------------------------------*/

//
// Return the FRotation corresponding to the direction that the vector
// is pointing in.  Sets Yaw and Pitch to the proper numbers, and sets
// roll to zero because the roll can't be determined from a vector.
//
FRotation FVector::Rotation()
{
	FRotation R;

	// Find yaw.
	R.Yaw = atan2(Y,X) * (FLOAT)MAXWORD / (2.0*PI);

	// Find pitch.
	R.Pitch = atan2(Z,sqrt(X*X+Y*Y)) * (FLOAT)MAXWORD / (2.0*PI);

	// Find roll.
	R.Roll = 0;

	return R;
}

//
// Convert a rotation to a normal vector.
//
FVector FRotation::Vector()
{
	FCoords Coords = GMath.UnitCoords;
	Coords.DeTransformByRotation(*this);

	return Coords.XAxis;
}

//
// Perform byte order conversion.
//
void FVector::Flip()
{
	ToHost(X);
	ToHost(Y);
	ToHost(Z);
	ToHost(W);
}

//
// Perform byte order conversion.
//
void FRotation::Flip()
{
	ToHost(Pitch);
	ToHost(Yaw);
	ToHost(Roll);
}

//
// Find good arbitrary axis vectors to represent U and V axes of a plane
// given just the normal.
//
void FVector::FindBestAxisVectors(FVector &Axis1,FVector &Axis2)
{
	guard(FindBestAxisVectors);

	FLOAT NX=Abs(X);
	FLOAT NY=Abs(Y);
	FLOAT NZ=Abs(Z);

	// Find best basis vectors.
	if ((NZ>NX)&&(NZ>NY))	Axis1 = GMath.XAxisVector;
	else					Axis1 = GMath.ZAxisVector;

	Axis1 = (Axis1 - *this * (Axis1 | *this)).Normal();
	Axis2 = Axis1 ^ *this;

#if PARANOID
	// Check results.
	if (
		(OurAbs(Axis1 | *this)>0.0001) ||
		(OurAbs(Axis2 | *this)>0.0001) ||
		(OurAbs(Axis1 | Axis2 )>0.0001)
		) appError ("FindBestAxisVectors failed");
#endif
	unguard;
}

/*-----------------------------------------------------------------------------
	FScale implementation.
-----------------------------------------------------------------------------*/

//
// Perform byte-order conversion.
//
void FScale::Flip()
{
	Scale.Flip();
	ToHost(SheerRate);
	ToHost(SheerAxis);
}

/*-----------------------------------------------------------------------------
	FCoords implementation.
-----------------------------------------------------------------------------*/

//
// Perform byte-order conversion.
//
void FCoords::Flip()
{
	Origin.Flip();
	XAxis.Flip();
	YAxis.Flip();
	ZAxis.Flip();
}

/*-----------------------------------------------------------------------------
	FModelCoords implementation.
-----------------------------------------------------------------------------*/

//
// Perform byte-order conversion.
//
void FModelCoords::Flip()
{
	PointXform.Flip();
	VectorXform.Flip();
}

/*-----------------------------------------------------------------------------
	Matrix inversion.
-----------------------------------------------------------------------------*/

//
// 4x4 matrix for internal use.
//
struct FMatrix4
{
	FLOAT element[4][4];
};

//
// 4x4 matrix inversion.
// From "Graphics Gems I", Academic Press, 1990
//
inline float Det2x2(FLOAT a, FLOAT b, FLOAT c, FLOAT d)
{
    return a * d - b * c;
}

//
// Determinant of 3x3 matrix.
//
inline float Det3x3
(
	FLOAT a1, FLOAT a2, FLOAT a3,
	FLOAT b1, FLOAT b2, FLOAT b3,
	FLOAT c1, FLOAT c2, FLOAT c3
)
{
    return
		+ a1 * Det2x2( b2, b3, c2, c3 )
        - b1 * Det2x2( a2, a3, c2, c3 )
        + c1 * Det2x2( a2, a3, b2, b3 );
}

//
// Determinant of 4x4 matrix.
//
FLOAT Det4x4(FMatrix4 *m)
{
	const FLOAT a1 = m->element[0][0]; 
	const FLOAT b1 = m->element[0][1]; 
	const FLOAT c1 = m->element[0][2]; 
	const FLOAT d1 = m->element[0][3];

	const FLOAT a2 = m->element[1][0]; 
	const FLOAT b2 = m->element[1][1]; 
	const FLOAT c2 = m->element[1][2]; 
	const FLOAT d2 = m->element[1][3];

	const FLOAT a3 = m->element[2][0]; 
	const FLOAT b3 = m->element[2][1]; 
	const FLOAT c3 = m->element[2][2]; 
	const FLOAT d3 = m->element[2][3];

	const FLOAT a4 = m->element[3][0]; 
	const FLOAT b4 = m->element[3][1]; 
	const FLOAT c4 = m->element[3][2]; 
	const FLOAT d4 = m->element[3][3];

    return
		+ a1 * Det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4)
        - b1 * Det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4)
        + c1 * Det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4)
        - d1 * Det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

//
// Adjoint of 4x4 matrix.
//
void Adjoint( FMatrix4 *in, FMatrix4 *out )
{
    // Assign to individual variable names to aid selecting correct values.
	const FLOAT a1 = in->element[0][0]; 
	const FLOAT b1 = in->element[0][1]; 
	const FLOAT c1 = in->element[0][2]; 
	const FLOAT d1 = in->element[0][3];

	const FLOAT a2 = in->element[1][0]; 
	const FLOAT b2 = in->element[1][1]; 
	const FLOAT c2 = in->element[1][2]; 
	const FLOAT d2 = in->element[1][3];

	const FLOAT a3 = in->element[2][0]; 
	const FLOAT b3 = in->element[2][1];
	const FLOAT c3 = in->element[2][2]; 
	const FLOAT d3 = in->element[2][3];

	const FLOAT a4 = in->element[3][0]; 
	const FLOAT b4 = in->element[3][1]; 
	const FLOAT c4 = in->element[3][2]; 
	const FLOAT d4 = in->element[3][3];

    // row column labeling reversed since we transpose rows & columns.
    out->element[0][0]  =   Det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
    out->element[1][0]  = - Det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
    out->element[2][0]  =   Det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
    out->element[3][0]  = - Det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);

    out->element[0][1]  = - Det3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
    out->element[1][1]  =   Det3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
    out->element[2][1]  = - Det3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
    out->element[3][1]  =   Det3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4);

    out->element[0][2]  =   Det3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
    out->element[1][2]  = - Det3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
    out->element[2][2]  =   Det3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
    out->element[3][2]  = - Det3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4);

    out->element[0][3]  = - Det3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
    out->element[1][3]  =   Det3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
    out->element[2][3]  = - Det3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
    out->element[3][3]  =   Det3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

//
// Invert a 4x4 matrix.
//
void Inverse(FMatrix4 *in, FMatrix4 *out)
{
    int i, j;
    FLOAT det, det4x4();

    // calculate the adjoint matrix.
    Adjoint(in,out);

    //  Calculate the 4x4 determinant:
    //  If the determinant is zero,  then the inverse matrix is not unique.

    det = Det4x4(in);
    if (fabs(det) < SMALL_NUMBER) appErrorf("Non-singular matrix, no inverse!");

    // Scale the adjoint matrix to get the inverse.
    for (i=0; i<4; i++) for(j=0; j<4; j++) out->element[i][j] = out->element[i][j] / det;
}

//
// Compute the inverse transpose of the 3x3 matrix defined by (V1,V2,V3) and return it
// in (R1,R2,R3):
//
void UNREAL_API InvertVectors 
(
	FVector &V1,FVector &V2,FVector &V3,
	FVector &R1,FVector &R2,FVector &R3
)
{
	guard(InvertVectors);

	FMatrix4 In,Out;

	In.element[0][0] = V1.X; In.element[0][1]=V1.Y; In.element[0][2]=V1.Z; In.element[0][3]=0.0;
	In.element[1][0] = V2.X; In.element[1][1]=V2.Y; In.element[1][2]=V2.Z; In.element[1][3]=0.0;
	In.element[2][0] = V3.X; In.element[2][1]=V3.Y; In.element[2][2]=V3.Z; In.element[2][3]=0.0;
	In.element[3][0] = 0.0;  In.element[3][1]=0.0;  In.element[3][2]=0.0;  In.element[3][3]=1.0;

	Inverse(&In,&Out);

	R1.X = Out.element[0][0]; R1.Y = Out.element[1][0]; R1.Z=Out.element[2][0];
	R2.X = Out.element[0][1]; R2.Y = Out.element[1][1]; R2.Z=Out.element[2][1];
	R3.X = Out.element[0][2]; R3.Y = Out.element[1][2]; R3.Z=Out.element[2][2];

	unguard;
}

//
// Randomly perturb a vector using a coordinate system for scaling.
//
void UNREAL_API PerturbNormalVector(FVector &V,const FCoords &C,FLOAT Ratio)
{
	Ratio *= 2.0;
	V += C.XAxis * (Ratio * (0.5 - (FLOAT)rand() / (FLOAT)RAND_MAX)) +
		 C.YAxis * (Ratio * (0.5 - (FLOAT)rand() / (FLOAT)RAND_MAX)) +
		 C.ZAxis * (Ratio * (0.5 - (FLOAT)rand() / (FLOAT)RAND_MAX));
	V.Normalize();
}

/*-----------------------------------------------------------------------------
	FBoundingRect implementation.
-----------------------------------------------------------------------------*/

//
// Initialize to an empty bound centered at 0,0,0.
//
void FBoundingRect::Init()
{
	guard(FBoundingRect::Init);

	Min      = GMath.ZeroVector;
	Max      = GMath.ZeroVector;

	// Note that it's invalid.
	Min.iTransform = 0;

	unguard;
}

//
// Set the bounding volume with a list of points.
// Graphics Gems I, Jack Ritter p. 301
//
void FBoundingRect::Init(FVector *Points,int NumPts)
{
	guard(FBoundingRect::Init);

	if( NumPts == 0 )
	{
		Init();
		return;
	}
	Min = Points[0];
	Max = Points[0];
	for( int i=1; i<NumPts; i++ )
	{
		FVector *P = &Points[i];

		UpdateMin(&Min,P);
		UpdateMax(&Max,P);
	}

	// Note that it's now valid.
	Min.iTransform = MAXWORD;

	unguard;
}

//
// Perform byte-order conversion.
//
void FBoundingRect::Flip()
{
	Min.Flip();
	Max.Flip();
}

/*-----------------------------------------------------------------------------
	FBoundingVolume implementation.
-----------------------------------------------------------------------------*/

//
// Initialize to an empty bound centered at 0,0,0.
//
void FBoundingVolume::Init()
{
	guard(FBoundingVolume::Init);

	FBoundingRect::Init();

	Sphere   = GMath.ZeroVector;
	Sphere.W = 0.0;

	unguard;
}

//
// Set the bounding volume with a list of points.
// Graphics Gems I, Jack Ritter p. 301
//
void FBoundingVolume::Init(FVector *Points,int NumPts)
{
	guard(FBoundingVolume::Init);

	FBoundingRect::Init(Points,NumPts);

	FVector MinX = Points[0];
	FVector MaxX = Points[0];
	FVector MinY = Points[0];
	FVector MaxY = Points[0];
	FVector MinZ = Points[0];
	FVector MaxZ = Points[0];

	for( int i=1; i<NumPts; i++ )
	{
		FVector *P = &Points[i];

		if (P->X < MinX.X) MinX = *P;
		if (P->X > MaxX.X) MaxX = *P;
		if (P->Y < MinY.Y) MinY = *P;
		if (P->Y > MaxY.Y) MaxY = *P;
		if (P->Z < MinZ.Z) MinZ = *P;
		if (P->Z > MaxZ.Z) MaxZ = *P;
	}
	FLOAT DX = FDist(MinX,MaxX);
	FLOAT DY = FDist(MinY,MaxY);
	FLOAT DZ = FDist(MinZ,MaxZ);

	FLOAT RSquared;
	if		((DX>DY) && (DX>DZ))	{Sphere=FMidpoint(MinX,MaxX); RSquared=DX*DX/4.0;}
	else if (DY>DZ)					{Sphere=FMidpoint(MinY,MaxY); RSquared=DY*DY/4.0;}
	else							{Sphere=FMidpoint(MinZ,MaxZ); RSquared=DZ*DZ/4.0;};

	for( i=0; i<NumPts; i++ )
	{
		FLOAT DistSquared = FDistSquared(Points[i],Sphere);
		if (DistSquared > RSquared) 
			RSquared = DistSquared;
	}

	// For guaranteed safety.
	Sphere.W = sqrt(RSquared) * 1.01;

	// Note that it's now valid.
	Min.iTransform = MAXWORD;

	unguard;
}

//
// Perform byte-order conversion.
//
void FBoundingVolume::Flip()
{
	FBoundingRect::Flip();
	Sphere.Flip();
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
