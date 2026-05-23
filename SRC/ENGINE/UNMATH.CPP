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
	WorldMin			(-32700.0,-32700.0,-32700.0),
	WorldMax			(32700.0,32700.0,32700.0),
	// Initialize FRotations.
	View0				(0x4000,0     ,0), // Up (pitch, yaw, roll).
	View1				(0xC000,0     ,0), // Down.
	View2				(0     ,0     ,0), // North.
	View3				(0     ,0x8000,0), // South.
	View4				(0     ,0xC000,0), // East.
	View5				(0     ,0x4000,0), // West.
	// Initialize FCoords.
	UnitCoords			(FVector(0,0,0),FVector(1,0,0),FVector(0,1,0),FVector(0,0,1)),
	CameraViewCoords	(FVector(0,0,0),FVector(0,1,0),FVector(0,0,-1),FVector(1,0,0)),
	// Initialize FScales.
	UnitScale			(FVector(1,1,1),0.0,SHEER_ZX)
{
	// Init view rotation array.
	Views[0]=&View0; Views[1]=&View1; Views[2]=&View2;
	Views[3]=&View3; Views[4]=&View4; Views[5]=&View5;

	// Init base angle table.
	for( int i=0; i<NUM_ANGLES; i++ )
		TrigFLOAT[i] = sin((FLOAT)i * 2.0 * PI / (FLOAT)NUM_ANGLES);

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
void FVector::FindBestAxisVectors( FVector &Axis1, FVector &Axis2 )
{
	guard(FindBestAxisVectors);

	FLOAT NX=Abs(X);
	FLOAT NY=Abs(Y);
	FLOAT NZ=Abs(Z);

	// Find best basis vectors.
	if ((NZ>NX)&&(NZ>NY))	Axis1 = FVector(1,0,0);
	else					Axis1 = FVector(0,0,1);

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

#if 0 /* Outdated 4x4 matrix inversion */
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

FCoords FCoords::Inverse() const
{
	guard(FCoords::Inverse);

	FMatrix4 In, Out;

	In.E[0][0] = XAxis.X; In.E[0][1]=XAxis.Y; In.E[0][2]=XAxis.Z; In.E[0][3]=0.0;
	In.E[1][0] = YAxis.X; In.E[1][1]=YAxis.Y; In.E[1][2]=YAxis.Z; In.E[1][3]=0.0;
	In.E[2][0] = ZAxis.X; In.E[2][1]=ZAxis.Y; In.E[2][2]=ZAxis.Z; In.E[2][3]=0.0;
	In.E[3][0] = 0.0;     In.E[3][1]=0.0;     In.E[3][2]=0.0;     In.E[3][3]=1.0;

	In.Inverse( Out );

	return FCoords
	(
		FVector( 0, 0, 0 ),
		FVector( Out.E[0][0], Out.E[1][0], Out.E[2][0] ),
		FVector( Out.E[0][1], Out.E[1][1], Out.E[2][1] ),
		FVector( Out.E[0][2], Out.E[1][2], Out.E[2][2] )
	).Transpose();
	unguard;
}
#endif

//
// Coordinate system inverse.
//
FCoords FCoords::Inverse() const
{
	FLOAT RDet = 1.0 / 
	(	(XAxis.X * (YAxis.Y * ZAxis.Z - YAxis.Z * ZAxis.Y))
	+	(XAxis.Y * (YAxis.Z * ZAxis.X - YAxis.X * ZAxis.Z))
	+	(XAxis.Z * (YAxis.X * ZAxis.Y - YAxis.Y * ZAxis.X)) );
	return FCoords
	(	-Origin.TransformVectorBy(*this)
	,	RDet * FVector
		(	+(YAxis.Y * ZAxis.Z - YAxis.Z * ZAxis.Y)
		,	+(ZAxis.Y * XAxis.Z - ZAxis.Z * XAxis.Y)
		,	+(XAxis.Y * YAxis.Z - XAxis.Z * YAxis.Y) )
	,	RDet * FVector
		(	+(YAxis.Z * ZAxis.X - ZAxis.Z * YAxis.X)
		,	+(ZAxis.Z * XAxis.X - XAxis.Z * ZAxis.X)
		,	+(XAxis.Z * YAxis.X - XAxis.X * YAxis.Z))
	,	RDet * FVector
		(	+(YAxis.X * ZAxis.Y - YAxis.Y * ZAxis.X)
		,	+(ZAxis.X * XAxis.Y - ZAxis.Y * XAxis.X)
		,	+(XAxis.X * YAxis.Y - XAxis.Y * YAxis.X) )
	);
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
