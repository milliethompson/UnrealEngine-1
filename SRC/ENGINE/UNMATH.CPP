/*=============================================================================
	UnMath.cpp: Unreal math routines, implementation of FGlobalMath class

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "Float.h"

/*-----------------------------------------------------------------------------
	FGlobalMath constructor.
-----------------------------------------------------------------------------*/

// Constructor.
FGlobalMath::FGlobalMath()
:	// Initialize FVectors.
	UnitVector			(1.0,1.0,1.0),
	ZeroVector			(0.0,0.0,0.0),
	XAxisVector			(1.0,0.0,0.0),
	YAxisVector			(0.0,1.0,0.0),
	ZAxisVector			(0.0,0.0,1.0),
	WorldMin			(-32700.0,-32700.0,-32700.0),
	WorldMax			(32700.0,32700.0,32700.0),
	VectorMax			(+(FLOAT)MAXSWORD,+(FLOAT)MAXSWORD,+(FLOAT)MAXSWORD),
	VectorMin			(-(FLOAT)MAXSWORD,-(FLOAT)MAXSWORD,-(FLOAT)MAXSWORD),
	UnitScaleVect		(1.0,1.0,1.0),
	// Initialize FRotations.
	ZeroRotation		(0,0,0),
	View0				(0x4000,0     ,0), // Up (pitch, yaw, roll).
	View1				(0xC000,0     ,0), // Down.
	View2				(0     ,0     ,0), // North.
	View3				(0     ,0x8000,0), // South.
	View4				(0     ,0xC000,0), // East.
	View5				(0     ,0x4000,0), // West.
	// Initialize FCoords.
	UnitCoords			(ZeroVector,XAxisVector,YAxisVector,ZAxisVector),
	CameraViewCoords	(ZeroVector,YAxisVector,-ZAxisVector,XAxisVector),
	UncameraViewCoords	(ZeroVector,ZAxisVector,XAxisVector,YAxisVector),
	// Initialize FScales.
	UnitScale			(UnitVector,0.0,SHEER_ZX)
{
	// Init view rotation array.
	Views[0]=&View0; Views[1]=&View1; Views[2]=&View2;
	Views[3]=&View3; Views[4]=&View4; Views[5]=&View5;

	// Init sin/cos signs.
	SinSIGN[0] = SinSIGN[1] = CosSIGN[0] = CosSIGN[3] = +1.0; 
	SinSIGN[2] = SinSIGN[3] = CosSIGN[1] = CosSIGN[2] = -1.0;

	// Init sin/cos masks.
	SinMASK[0] = SinMASK[2] = CosMASK[1] = CosMASK[3] = 0;
	SinMASK[1] = SinMASK[3] = CosMASK[0] = CosMASK[2] = NUM_ANGLES-1;

	// Init base angle table.
	for( int i=0; i<NUM_ANGLES; i++ )
		TrigFLOAT[i] = sin((FLOAT)i * 0.5 * PI / (FLOAT)NUM_ANGLES);

	// Init square root table.
	for( i=0; i<NUM_SQRTS; i++ )
	{
		FLOAT S				= sqrt((FLOAT)(i+1) * (1.0/(FLOAT)NUM_SQRTS));
		FLOAT Temp			= (1.0-S);// Was (2*S*S*S-3*S*S+1);
		SqrtFLOAT[i]		= sqrt((FLOAT)i / 16384.0);
	}
}

/*-----------------------------------------------------------------------------
	Conversion functions.
-----------------------------------------------------------------------------*/

// Return the FRotation corresponding to the direction that the vector
// is pointing in.  Sets Yaw and Pitch to the proper numbers, and sets
// roll to zero because the roll can't be determined from a vector.
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

#if CHECK_ALL
	// Check results.
	if (
		(Abs(Axis1 | *this)>0.0001) ||
		(Abs(Axis2 | *this)>0.0001) ||
		(Abs(Axis1 | Axis2 )>0.0001)
		) appError ("FindBestAxisVectors failed");
#endif
	unguard;
}

//
// Convert byte hue-saturation-brightness to floating point red-green-blue.
//
void FVector::GetHSV( BYTE H, BYTE S, BYTE V, int ColorBytes )
{
	FLOAT	Brightness	= (FLOAT)V;
	FLOAT	Alpha		= (FLOAT)S / 255.0;

	if( ColorBytes==1 )
	{
		Brightness *= (0.5/255.0) * (Alpha + 1.00);
		Brightness *= 0.70/(0.01 + sqrt(Brightness));
		Brightness  = Clamp(Brightness,0.f,1.f);

		R = G = B = Brightness;
	}
	else
	{
		Brightness *= (1.4/255.0);
		Brightness *= 0.70/(0.01 + sqrt(Brightness));
		Brightness  = Clamp(Brightness,(FLOAT)0.0,(FLOAT)1.0);

		GGfx.HueTable->Lock(LOCK_Read);
		FVector &Hue = GGfx.HueTable(H);
		*this = (Hue + Alpha * (FVector(1,1,1) - Hue)) * Brightness;
		GGfx.HueTable->Unlock(LOCK_Read);
	}
}

/*-----------------------------------------------------------------------------
	Matrix inversion.
-----------------------------------------------------------------------------*/

// 4x4 matrix.
struct FMatrix4
{
	// Variables.
	FLOAT E[4][4];

	// 4x4 determinant.
	FLOAT Det4x4()
	{
		return
		+ E[0][0] * Det3x3( E[1][1], E[2][1], E[3][1], E[1][2], E[2][2], E[3][2], E[1][3], E[2][3], E[3][3] )
		- E[0][1] * Det3x3( E[1][0], E[2][0], E[3][0], E[1][2], E[2][2], E[3][2], E[1][3], E[2][3], E[3][3] )
		+ E[0][2] * Det3x3( E[1][0], E[2][0], E[3][0], E[1][1], E[2][1], E[3][1], E[1][3], E[2][3], E[3][3] )
		- E[0][3] * Det3x3( E[1][0], E[2][0], E[3][0], E[1][1], E[2][1], E[3][1], E[1][2], E[2][2], E[3][2]) ;
	}

	// 3x3 determinant.
	FLOAT Det3x3
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

	// 2x2 Determinant.
	FLOAT Det2x2( FLOAT a, FLOAT b, FLOAT c, FLOAT d )
	{
		return 
		a * d - b * c;
	}

	// Get the adjoint of this matrix.
	void ScaledAdjoint( FMatrix4 &Out, FLOAT Scale )
	{
		Out.E[0][0] = + Det3x3( E[1][1], E[2][1], E[3][1], E[1][2], E[2][2], E[3][2], E[1][3], E[2][3], E[3][3]) * Scale;
		Out.E[1][0] = - Det3x3( E[1][0], E[2][0], E[3][0], E[1][2], E[2][2], E[3][2], E[1][3], E[2][3], E[3][3]) * Scale;
		Out.E[2][0] = + Det3x3( E[1][0], E[2][0], E[3][0], E[1][1], E[2][1], E[3][1], E[1][3], E[2][3], E[3][3]) * Scale;
		Out.E[3][0] = - Det3x3( E[1][0], E[2][0], E[3][0], E[1][1], E[2][1], E[3][1], E[1][2], E[2][2], E[3][2]) * Scale;

		Out.E[0][1] = - Det3x3( E[0][1], E[2][1], E[3][1], E[0][2], E[2][2], E[3][2], E[0][3], E[2][3], E[3][3]) * Scale;
		Out.E[1][1] = + Det3x3( E[0][0], E[2][0], E[3][0], E[0][2], E[2][2], E[3][2], E[0][3], E[2][3], E[3][3]) * Scale;
		Out.E[2][1] = - Det3x3( E[0][0], E[2][0], E[3][0], E[0][1], E[2][1], E[3][1], E[0][3], E[2][3], E[3][3]) * Scale;
		Out.E[3][1] = + Det3x3( E[0][0], E[2][0], E[3][0], E[0][1], E[2][1], E[3][1], E[0][2], E[2][2], E[3][2]) * Scale;

		Out.E[0][2] = + Det3x3( E[0][1], E[1][1], E[3][1], E[0][2], E[1][2], E[3][2], E[0][3], E[1][3], E[3][3]) * Scale;
		Out.E[1][2] = - Det3x3( E[0][0], E[1][0], E[3][0], E[0][2], E[1][2], E[3][2], E[0][3], E[1][3], E[3][3]) * Scale;
		Out.E[2][2] = + Det3x3( E[0][0], E[1][0], E[3][0], E[0][1], E[1][1], E[3][1], E[0][3], E[1][3], E[3][3]) * Scale;
		Out.E[3][2] = - Det3x3( E[0][0], E[1][0], E[3][0], E[0][1], E[1][1], E[3][1], E[0][2], E[1][2], E[3][2]) * Scale;

		Out.E[0][3] = - Det3x3( E[0][1], E[1][1], E[2][1], E[0][2], E[1][2], E[2][2], E[0][3], E[1][3], E[2][3]) * Scale;
		Out.E[1][3] = + Det3x3( E[0][0], E[1][0], E[2][0], E[0][2], E[1][2], E[2][2], E[0][3], E[1][3], E[2][3]) * Scale;
		Out.E[2][3] = - Det3x3( E[0][0], E[1][0], E[2][0], E[0][1], E[1][1], E[2][1], E[0][3], E[1][3], E[2][3]) * Scale;
		Out.E[3][3] = + Det3x3( E[0][0], E[1][0], E[2][0], E[0][1], E[1][1], E[2][1], E[0][2], E[1][2], E[2][2]) * Scale;
	}

	// Get the inverse of this matrix.
	void Inverse( FMatrix4 &Out )
	{
		// Calculate the 4x4 determinant.
		FLOAT Det = Det4x4();

		// If the determinant is near zero, then the inverse matrix is not unique.
		if( fabs(Det) < SMALL_NUMBER )
		{
			// Make up a fake solution.
			Out = *this;
			debugf( "Non-singular matrix, no inverse!" );
		}
		else
		{
			// Calculate the adjoint matrix.
			ScaledAdjoint(Out,1.0/Det);
		}
	}
};

// Compute the inverse transpose of the 3x3 matrix defined by (V1,V2,V3) and return it
// in (R1,R2,R3):
FCoords FCoords::Inverse() const
{
	guard(FCoords::Inverse);

	FMatrix4 In,Out;

	In.E[0][0] = XAxis.X; In.E[0][1]=XAxis.Y; In.E[0][2]=XAxis.Z; In.E[0][3]=0.0;
	In.E[1][0] = YAxis.X; In.E[1][1]=YAxis.Y; In.E[1][2]=YAxis.Z; In.E[1][3]=0.0;
	In.E[2][0] = ZAxis.X; In.E[2][1]=ZAxis.Y; In.E[2][2]=ZAxis.Z; In.E[2][3]=0.0;
	In.E[3][0] = 0.0;     In.E[3][1]=0.0;     In.E[3][2]=0.0;     In.E[3][3]=1.0;

	In.Inverse(Out);

	return FCoords
	(
		FVector(0,0,0),
		FVector( Out.E[0][0], Out.E[1][0], Out.E[2][0] ),
		FVector( Out.E[0][1], Out.E[1][1], Out.E[2][1] ),
		FVector( Out.E[0][2], Out.E[1][2], Out.E[2][2] )
	);
	unguard;
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

	IsValid  = 0;
	Min      = GMath.ZeroVector;
	Max      = GMath.ZeroVector;

	unguard;
}

//
// Set the bounding rectangle with a list of points.
//
void FBoundingRect::Init( FVector *Points, int NumPts )
{
	guard(FBoundingRect::Init);
	if( NumPts == 0 )
	{
		Init();
	}
	else
	{
		IsValid = 1;
		Min     = Points[0];
		Max     = Points[0];
		for( int i=1; i<NumPts; i++ )
		{
			Min.UpdateMinWith( Points[i] );
			Max.UpdateMaxWith( Points[i] );
		}
	}
	unguard;
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
	Sphere = FPlane(0,0,0,0);

	unguard;
}

//
// Set the bounding volume with a list of points.
// Graphics Gems I, Jack Ritter p. 301.
//
void FBoundingVolume::Init( FVector *Points, int NumPts )
{
	guard(FBoundingVolume::Init);

	if( NumPts == 0 )
	{
		Init();
	}
	else
	{
		// Compute bounding rectangle.
		FBoundingRect::Init( Points, NumPts );

		// Compute bounding sphere.
		FVector MinX = Points[0];
		FVector MaxX = Points[0];
		FVector MinY = Points[0];
		FVector MaxY = Points[0];
		FVector MinZ = Points[0];
		FVector MaxZ = Points[0];
		for( int i=1; i<NumPts; i++ )
		{
			FVector *P = &Points[i];

			if( P->X < MinX.X ) MinX = *P;
			if( P->X > MaxX.X ) MaxX = *P;
			if( P->Y < MinY.Y ) MinY = *P;
			if( P->Y > MaxY.Y ) MaxY = *P;
			if( P->Z < MinZ.Z ) MinZ = *P;
			if( P->Z > MaxZ.Z ) MaxZ = *P;
		}
		FLOAT DX = FDist( MinX, MaxX );
		FLOAT DY = FDist( MinY, MaxY );
		FLOAT DZ = FDist( MinZ, MaxZ );

		FLOAT RSquared;
		if	   ( DX>DY && DX>DZ )	{Sphere=FMidpoint(MinX,MaxX); RSquared=DX*DX/4.0;}
		else if( DY>DZ )			{Sphere=FMidpoint(MinY,MaxY); RSquared=DY*DY/4.0;}
		else						{Sphere=FMidpoint(MinZ,MaxZ); RSquared=DZ*DZ/4.0;}

		for( i=0; i<NumPts; i++ )
		{
			FLOAT DistSquared = FDistSquared( Points[i], Sphere );
			if( DistSquared > RSquared )
				RSquared = DistSquared;
		}

		// For guaranteed safety.
		Sphere.W = sqrt(RSquared) * 1.01;
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
