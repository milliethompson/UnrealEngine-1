/*=============================================================================
	UnFPoly.cpp: FPoly implementation (Editor polygons).

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "Unreal.h"

/*---------------------------------------------------------------------------------------
	FPoly class implementation.
---------------------------------------------------------------------------------------*/

//
// Initialize everything in an  editor polygon structure to defaults.
//
void FPoly::Init()
{
	guard(FPoly::Init);

	Base			= FVector(0,0,0);
	Normal			= FVector(0,0,0);
	TextureU		= FVector(0,0,0);
	TextureV		= FVector(0,0,0);
	PolyFlags       = 0;
	Brush			= NULL;
	Texture         = NULL;
	GroupName       = NAME_None;
	ItemName        = NAME_None;
	NumVertices     = 0;
	iLink           = INDEX_NONE;
	iBrushPoly		= INDEX_NONE;
	PanU			= 0;
	PanV			= 0;
	iZone[0]		= 0;
	iZone[1]		= 0;

	unguard;
}

//
// Reverse an FPoly by revesing the normal and reversing the order of its
// vertices.
//
void FPoly::Reverse()
{
	guard(FPoly::Reverse);

	FVector Temp;
	int i,c;

	Normal *= -1;

	c=NumVertices/2;
	for( i=0; i<c; i++ )
	{
		// Flip all points except middle if odd number of points.
		Temp      = Vertex[i];
		Vertex[i] = Vertex[(NumVertices-1)-i];
		Vertex[(NumVertices-1)-i] = Temp;
	}
	unguard;
}

//
// Fix up an editor poly by deleting vertices that are identical.  Sets
// vertex count to zero if it collapses.  Returns number of vertices, 0 or >=3.
//
int FPoly::Fix()
{
	guard(FPoly::Fix);
	int i,j,prev;

	j=0; prev=NumVertices-1;
	for( i=0; i<NumVertices; i++ )
	{
		if( !FPointsAreSame( Vertex[i], Vertex[prev] ) )
		{
			if( j != i )
				Vertex[j] = Vertex[i];
			prev = j;
			j    ++;
		}
		else debug( LOG_Problem, "FPoly::Fix: Collapsed a point" );
	}
	if (j>=3) NumVertices = j;
	else      NumVertices = 0;
	return NumVertices;
	unguard;
}

//
// Compute the 2D area.
//
FLOAT FPoly::Area()
{
	guard(FPoly::Area);

	FVector Side1,Side2;
	FLOAT Area;
	int i;

	Area  = 0.0;
	Side1 = Vertex[1] - Vertex[0];
	for( i=2; i<NumVertices; i++ )
	{
		Side2 = Vertex[i] - Vertex[0];
		Area += (Side1 ^ Side2).Size();
		Side1 = Side2;
	}
	return Area;
	unguard;
}

//
// Split with plane. Meant to be numerically stable.
//
int FPoly::SplitWithPlane
(
	const FVector	&PlaneBase,
	const FVector	&PlaneNormal,
	FPoly			*FrontPoly,
	FPoly			*BackPoly,
	int				VeryPrecise
) const
{
	guard(FPoly::SplitWithPlane);
	
	FVector 	Intersection;
	FLOAT   	Dist=0.0,MaxDist=0,MinDist=0;
	FLOAT		PrevDist,Thresh;
	enum 	  	{V_FRONT,V_BACK,V_EITHER} Status,PrevStatus=V_EITHER;
	int     	i,j;

	if (VeryPrecise)	Thresh = THRESH_SPLIT_POLY_PRECISELY;	
	else				Thresh = THRESH_SPLIT_POLY_WITH_PLANE;

	// Find number of vertices.
	checkState(NumVertices>=3);
	checkState(NumVertices<MAX_VERTICES);

	// See if the polygon is split by SplitPoly, or it's on either side, or the
	// polys are coplanar.  Go through all of the polygon points and
	// calculate the minimum and maximum signed distance (in the direction
	// of the normal) from each point to the plane of SplitPoly.
	for( i=0; i<NumVertices; i++ )
	{
		Dist = FPointPlaneDist( Vertex[i], PlaneBase, PlaneNormal );

		if( i==0 || Dist>MaxDist ) MaxDist=Dist;
		if( i==0 || Dist<MinDist ) MinDist=Dist;

		if      (Dist > +Thresh) PrevStatus = V_FRONT;
		else if (Dist < -Thresh) PrevStatus = V_BACK;
	}
	if( MaxDist<Thresh && MinDist>-Thresh )
	{
		return SP_Coplanar;
	}
	else if( MaxDist < Thresh )
	{
		return SP_Back;
	}
	else if( MinDist > -Thresh )
	{
		return SP_Front;
	}
	else
	{
		// Split.
		if (FrontPoly==NULL) return SP_Split; // Caller only wanted status.
		if (NumVertices >= MAX_VERTICES) appError ("FPoly::SplitWithPlane: Vertex overflow");

		*FrontPoly = *this; // Copy all info.
		FrontPoly->PolyFlags |= PF_EdCut; // Mark as cut.
		FrontPoly->NumVertices =  0;

		*BackPoly = *this; // Copy all info.
		BackPoly->PolyFlags |= PF_EdCut; // Mark as cut.
		BackPoly->NumVertices = 0;

		j = NumVertices-1; // Previous vertex; have PrevStatus already.

		for( i=0; i<NumVertices; i++ )
		{
			PrevDist	= Dist;
      		Dist		= FPointPlaneDist( Vertex[i], PlaneBase, PlaneNormal );

			if      (Dist > +Thresh)  	Status = V_FRONT;
			else if (Dist < -Thresh)  	Status = V_BACK;
			else						Status = PrevStatus;

			if( Status != PrevStatus )
	        {
				// Crossing.  Either Front-to-Back or Back-To-Front.
				// Intersection point is naturally on both front and back polys.
				if( (Dist >= -Thresh) && (Dist < +Thresh) )
				{
					// This point lies on plane.
					if( PrevStatus == V_FRONT )
					{
						FrontPoly->Vertex[FrontPoly->NumVertices++] = Vertex[i];
						BackPoly ->Vertex[BackPoly ->NumVertices++] = Vertex[i];
					}
					else
					{
						BackPoly ->Vertex[BackPoly ->NumVertices++] = Vertex[i];
						FrontPoly->Vertex[FrontPoly->NumVertices++] = Vertex[i];
					}
				}
				else if( (PrevDist >= -Thresh) && (PrevDist < +Thresh) )
				{
					// Previous point lies on plane.
					if (Status == V_FRONT)
					{
						FrontPoly->Vertex[FrontPoly->NumVertices++] = Vertex[j];
						FrontPoly->Vertex[FrontPoly->NumVertices++] = Vertex[i];
					}
					else
					{
						BackPoly ->Vertex[BackPoly ->NumVertices++] = Vertex[j];
						BackPoly ->Vertex[BackPoly ->NumVertices++] = Vertex[i];
					}
				}
				else
				{
					// Intersection point is in between.
					Intersection = FLinePlaneIntersection(Vertex[j],Vertex[i],PlaneBase,PlaneNormal);

					if( PrevStatus == V_FRONT )
					{
						FrontPoly->Vertex[FrontPoly->NumVertices++] = Intersection;
						BackPoly ->Vertex[BackPoly ->NumVertices++] = Intersection;
						BackPoly ->Vertex[BackPoly ->NumVertices++]	= Vertex[i];
					}
					else
					{
						BackPoly ->Vertex[BackPoly ->NumVertices++] = Intersection;
						FrontPoly->Vertex[FrontPoly->NumVertices++] = Intersection;
						FrontPoly->Vertex[FrontPoly->NumVertices++]	= Vertex[i];
					}
				}
			}
			else
			{
        		if (Status==V_FRONT) FrontPoly->Vertex[FrontPoly->NumVertices++] = Vertex[i];
        		else                 BackPoly ->Vertex[BackPoly ->NumVertices++] = Vertex[i];
			}
			j          = i;
			PrevStatus = Status;
		}

		// Handle possibility of sliver polys due to precision errors.
		if( FrontPoly->Fix()<3 )
		{
			debug (LOG_Problem,"FPoly::SplitWithPlane: Ignored front sliver");
			return SP_Back;
		}
		else if( BackPoly->Fix()<3 )
	    {
			debug (LOG_Problem,"FPoly::SplitWithPlane: Ignored back sliver");
			return SP_Front;
		}
		else return SP_Split;
	}
	unguard;
}

//
// Split with a Bsp node.
//
int FPoly::SplitWithNode
(
	const UModel	*Model,
	INDEX			iNode,
	FPoly			*FrontPoly,
	FPoly			*BackPoly,
	INT				VeryPrecise
) const
{
	guard(FPoly::SplitWithNode);
	const FBspNode &Node = Model->Nodes  (iNode       );
	const FBspSurf &Surf = Model->Surfs  (Node.iSurf  );

	return SplitWithPlane
	(
		Model->Points (Surf.pBase  ),
		Model->Vectors(Surf.vNormal),
		FrontPoly, 
		BackPoly, 
		VeryPrecise
	);
	unguard;
}

//
// Split with plane quickly for in-game geometry operations.
// Results are always valid. May return sliver polys.
//
int FPoly::SplitWithPlaneFast
(
	const FPlane	Plane,
	FPoly			*FrontPoly,
	FPoly			*BackPoly
) const
{
	guard(FPoly::SplitWithPlaneFast);

	enum {V_FRONT=0,V_BACK=1} Status,PrevStatus,VertStatus[MAX_VERTICES],*StatusPtr;
	int Front=0,Back=0;

	StatusPtr = &VertStatus[0];
	for( int i=0; i<NumVertices; i++ )
	{
		FLOAT Dist = Plane.PlaneDot(Vertex[i]);
		if( Dist >= 0.0 )
		{
			*StatusPtr++ = V_FRONT;
			if( Dist > +THRESH_SPLIT_POLY_WITH_PLANE )
				Front=1;
		}
		else
		{
			*StatusPtr++ = V_BACK;
			if( Dist < -THRESH_SPLIT_POLY_WITH_PLANE )
				Back=1;
		}
	}
	if( !Front )
	{
		if( Back ) return SP_Back;
		else       return SP_Coplanar;
	}
	if( !Back )
	{
		return SP_Front;
	}
	else
	{
		// Split.
		if( FrontPoly )
		{
			const FVector *V  = &Vertex            [0];
			const FVector *W  = &Vertex            [NumVertices-1];
			FVector *V1       = &FrontPoly->Vertex [0];
			FVector *V2       = &BackPoly ->Vertex [0];
			PrevStatus        = VertStatus         [NumVertices-1];
			StatusPtr         = &VertStatus        [0];

			int N1=0, N2=0;
			for( i=0; i<NumVertices; i++ )
			{
				Status = *StatusPtr++;
				if( Status != PrevStatus )
				{
					// Crossing.
					*V1++ = *V2++ = FLinePlaneIntersection( *W, *V, Plane );
					if( PrevStatus == V_FRONT )	{*V2++ = *V; N1++; N2+=2;}
					else {*V1++ = *V; N2++; N1+=2;};
				}
				else if( Status==V_FRONT ) {*V1++ = *V; N1++;}
				else {*V2++ = *V; N2++;};

				PrevStatus = Status;
				W          = V++;
			}
			FrontPoly->NumVertices	= N1;
			FrontPoly->Base			= Base;
			FrontPoly->Normal		= Normal;

			BackPoly->NumVertices	= N2;
			BackPoly->Base			= Base;
			BackPoly->Normal		= Normal;
		}
		return SP_Split;
	}
	unguard;
}

//
// Split an FPoly in half.
//
void FPoly::SplitInHalf( FPoly *OtherHalf )
{
	guard(FPoly::SplitInHalf);

	int m = NumVertices/2;
	int i;

	if( (NumVertices<=3) || (NumVertices>MAX_VERTICES) )
		appErrorf ("FPoly::SplitInHalf: %i Vertices",NumVertices);

	*OtherHalf = *this;

	OtherHalf->NumVertices = (NumVertices-m) + 1;
	NumVertices            = (m            ) + 1;

	for( i=0; i<(OtherHalf->NumVertices-1); i++ )
	{
		OtherHalf->Vertex[i] = Vertex[i+m];
	}
	OtherHalf->Vertex[OtherHalf->NumVertices-1] = Vertex[0];

	PolyFlags            |= PF_EdCut;
	OtherHalf->PolyFlags |= PF_EdCut;
	
	unguard;
}

//
// Compute normal of an FPoly.  Works even if FPoly has 180-degree-angled sides (which
// are often created during T-joint elimination).  Returns nonzero result (plus sets
// normal vector to zero) if a problem occurs.
//
int FPoly::CalcNormal()
{
	guard(FPoly::CalcNormal);

	Normal = FVector(0,0,0);
	for( int i=2; i<NumVertices; i++ )
		Normal += (Vertex[i-1] - Vertex[0]) ^ (Vertex[i] - Vertex[0]);

	if( Normal.SizeSquared() < (FLOAT)THRESH_ZERO_NORM_SQUARED )
	{
		debug( LOG_Problem, "FPoly::CalcNormal: Zero-area polygon" );
		return 1;
	}
	Normal.Normalize();
	return 0;
	unguard;
}

//
// Transform an editor polygon with a coordinate system, a pre-transformation
// addition, and a post-transformation addition:
//
void FPoly::Transform
(
	const FModelCoords	&Coords,
	const FVector		&PreSubtract,
	const FVector		&PostAdd,
	FLOAT				Orientation
)
{
	guard(FPoly::Transform);

	FVector 	Temp;
	int 		i,m;

	TextureU = TextureU.TransformVectorBy(Coords.VectorXform);
	TextureV = TextureV.TransformVectorBy(Coords.VectorXform);

	Base = (Base - PreSubtract).TransformVectorBy(Coords.PointXform) + PostAdd;
	for( i=0; i<NumVertices; i++ )
		Vertex[i]  = (Vertex[i] - PreSubtract).TransformVectorBy( Coords.PointXform ) + PostAdd;

	// Flip vertex order if orientation is negative.
	if( Orientation < 0.0 )
	{
		m = NumVertices/2;
		for( i=0; i<m; i++ )
		{
			Temp 					  = Vertex[i];
			Vertex[i] 		          = Vertex[(NumVertices-1)-i];
			Vertex[(NumVertices-1)-i] = Temp;
		}
	}

	// Transform normal.  Since the transformation coordinate system is
	// orthogonal but not orthonormal, it has to be renormalized here.
	Normal = Normal.TransformVectorBy(Coords.VectorXform).Normal();

	unguard;
}

//
// Remove colinear vertices and check convexity.  Returns 1 if convex, 0 if
// nonconvex or collapsed.
//
int FPoly::RemoveColinears()
{
	guard(FPoly::RemoveColinears);

	FVector  SidePlaneNormal[MAX_VERTICES];
	FVector  Side;
	int      i,j;

	for( i=0; i<NumVertices; i++ )
	{
		j=i-1; if (j<0) j=NumVertices-1;

		// Create cutting plane perpendicular to both this side and the polygon's normal.
		Side = Vertex[i] - Vertex[j];
		SidePlaneNormal[i] = Side ^ Normal;

		if( !SidePlaneNormal[i].Normalize() )
		{
			// Eliminate these nearly identical points.
			memcpy (&Vertex[i],&Vertex[i+1],(NumVertices-(i+1)) * sizeof (FVector));
			if (--NumVertices<3) {NumVertices = 0; return 0;}; // Collapsed.
			i--;
		}
	}
	for( i=0; i<NumVertices; i++ )
	{
		j=i+1; if (j>=NumVertices) j=0;

		if( FPointsAreNear(SidePlaneNormal[i],SidePlaneNormal[j],FLOAT_NORMAL_THRESH) )
	    {
			// Eliminate colinear points.
			memcpy (&Vertex[i],&Vertex[i+1],(NumVertices-(i+1)) * sizeof (FVector));
			memcpy (&SidePlaneNormal[i],&SidePlaneNormal[i+1],(NumVertices-(i+1)) * sizeof (FVector));
			if (--NumVertices<3) {NumVertices = 0; return 0;}; // Collapsed.
			i--;
		}
		else
		{
			for( j=0; j<NumVertices; j++ )
	        {
				if (j != i)
				{
					switch( SplitWithPlane (Vertex[i],SidePlaneNormal[i],NULL,NULL,0) )
					{
						case SP_Front: return 0; // Nonconvex + Numerical precision error
						case SP_Split: return 0; // Nonconvex
						// SP_BACK: Means it's convex
						// SP_COPLANAR: Means it's probably convex (numerical precision)
					}
				}
			}
		}
	}
	return 1; // Ok.
	unguard;
}

//
// Split a poly and keep only the front half. Returns number of vertices,
// 0 if clipped away.
//
int FPoly::Split( const FVector &Normal, const FVector &Base, int NoOverflow )
{
	guard(FPoly::Split);
	if( NoOverflow && NumVertices>=FPoly::VERTEX_THRESHOLD )
	{
		// Don't split it, just reject it.
		if( SplitWithPlaneFast( FPlane(Base,Normal), NULL, NULL )==SP_Back )
			return 0;
		else
			return NumVertices;
	}
	else
	{
		// Split it.
		FPoly Front, Back;
		switch( SplitWithPlaneFast( FPlane(Base,Normal), &Front, &Back ))
		{
			case SP_Back:
				return 0;
			case SP_Split:
				*this = Front;
				return NumVertices;
			default:
				return NumVertices;
		}
	}
	unguard;
}

//
// Compute all remaining polygon parameters (normal, etc) that are blank.
// Returns 0 if ok, nonzero if problem.
//
int FPoly::Finalize( int NoError )
{
	guard(FPoly::Finalize);

	// Check for problems.
	Fix();
	if( NumVertices<3 )
	{
		debugf( LOG_Problem, "FPoly::Finalize: Not enough vertices (%i)", NumVertices );
		if( NoError )
			return -1;
		else
			appErrorf( "FPoly::Finalize: Not enough vertices (%i)", NumVertices );
	}

	// If no normal, compute from cross-product and normalize it.
	if( Normal.IsZero() && NumVertices>=3 )
	{
		if( CalcNormal() )
		{
			debugf( LOG_Problem, "FPoly::Finalize: Normalization failed, verts=%i, size=%f", NumVertices, Normal.Size() );
			if( NoError ) return -1;
			else appErrorf( "FPoly::Finalize: Normalization failed, verts=%i, size=%f", NumVertices, Normal.Size() );
		}
	}

	// If texture U and V coordinates weren't specified, generate them.
	if( TextureU.IsZero() && TextureV.IsZero() )
	{
		for( int i=1; i<NumVertices; i++ )
		{
			TextureU = ((Vertex[0] - Vertex[i]) ^ Normal).Normal() * +65536.0;
			TextureV = (TextureU                ^ Normal).Normal() * -65536.0;
			if( TextureU.SizeSquared()!=0 && TextureV.SizeSquared()!=0 )
				break;
		}
	}
	return 0;
	unguard;
}

/*---------------------------------------------------------------------------------------
	FPolys object implementation.
---------------------------------------------------------------------------------------*/

const char *UPolys::Import(const char *Buffer, const char *BufferEnd,const char *FileType)
{
	guard(UPolys::Import);
	if( Max==0 )
	{
		// We're importing a new set of polys.
		Num = 0;
	}
	ParseFPolys(&Buffer,0,0);
	Shrink();
	return Buffer;
	unguard;
}
void UPolys::Export( FOutputDevice &Out, const char *FileType, int Indent )
{
	guard(UPolys::Export);

	char	TempStr[256];
	int		i,j;

	Out.Logf("%sBegin PolyList Num=%i Max=%i\r\n",spc(Indent),Num,Max);

	for( i=0; i<Num; i++ )
	{
		FPoly *Poly = &Element(i);

		// Start of polygon plus group/item name if applicable.
		Out.Logf("%s   Begin Polygon",spc(Indent));
		if( Poly->GroupName != NAME_None )
		{
			Out.Logf(" Group=%s",Poly->GroupName());
		}
		if( Poly->ItemName != NAME_None )
		{
			Out.Logf(" Item=%s",Poly->ItemName());
		}
		if( Poly->Texture )
		{
			Out.Logf(" Texture=%s",Poly->Texture->GetName());
		}
		if( Poly->PolyFlags != 0 )
		{
			Out.Logf(" Flags=%i",Poly->PolyFlags);
		}
		if( Poly->iLink != INDEX_NONE )
		{
			Out.Logf(" Link=%i",Poly->iLink);
		}
		Out.Logf("\r\n");

		// All coordinates.
		Out.Logf("%s      Origin   %s\r\n",spc(Indent),SetFVECTOR(TempStr,&Poly->Base));
		Out.Logf("%s      Normal   %s\r\n",spc(Indent),SetFVECTOR(TempStr,&Poly->Normal));

		if( Poly->PanU!=0 || Poly->PanV!=0 )
		{
			Out.Logf("%s      Pan      U=%i V=%i\r\n",spc(Indent),Poly->PanU,Poly->PanV);
		}
		Out.Logf("%s      TextureU %s\r\n",spc(Indent),SetFIXFVECTOR(TempStr,&Poly->TextureU));
		Out.Logf("%s      TextureV %s\r\n",spc(Indent),SetFIXFVECTOR(TempStr,&Poly->TextureV));
		for( j=0; j<Poly->NumVertices; j++ )
		{
			Out.Logf("%s      Vertex   %s\r\n",spc(Indent),SetFVECTOR(TempStr,&Poly->Vertex[j]));
		}
		Out.Logf("%s   End Polygon\r\n",spc(Indent));
	}
	Out.Logf("%sEnd PolyList\r\n",spc(Indent));
	unguard;
}
IMPLEMENT_DB_CLASS(UPolys);

/*---------------------------------------------------------------------------------------
	UPoly custom functions.
---------------------------------------------------------------------------------------*/

//
// Parse a list of polygons.  Returns the number parsed.
//
void UPolys::ParseFPolys( const char **Stream, int More, int CmdLine )
{
	guard(UPolys::ParseFPolys);
	Lock(LOCK_ReadWrite);

	// Eat up if present.
	GetBEGIN( Stream, "POLYLIST" );

	if( !More )
		Empty();

	// Parse all stuff.
	int First=1, GotBase=0;
	FPoly Poly;
	for( ; ; )
	{
		char StrLine[256], ExtraLine[256];
		if (GetLINE (Stream,StrLine,256)!=0)	break;
		if (CmdLine && GEditor) 				GEditor->NoteMacroCommand (StrLine);
		if (strlen(StrLine)==0) 				continue;

		const char *Str = StrLine;
		if (GetEND(&Str,"POLYLIST")) break; // End of brush polys
		else if( strstr(Str,"ENTITIES") && First )
		{
			// Autocad .DXF file.
			//debugf(LOG_Info,"Reading Autocad DXF file");
			INT Started=0,NumPts=0,IsFace=0;
			FVector PointPool[4096];
			FPoly NewPoly; NewPoly.Init();

			while( GetLINE(Stream,StrLine,256,1)==0 && GetLINE(Stream,ExtraLine,256,1)==0 )
			{
				// Handle the line.
				Str = ExtraLine;
				INT Code = atoi(StrLine);
				//debugf("DXF: %i: %s",Code,ExtraLine);
				if( Code==0 )
				{
					// Finish up current poly.
					if( Started )
					{
						if( NewPoly.NumVertices == 0 )
						{
							// Got a vertex definition.
							NumPts++;
							//debugf("DXF: Added vertex %i",NewPoly.NumVertices);
						}
						else if( NewPoly.NumVertices>=3 && NewPoly.NumVertices<FPoly::MAX_VERTICES )
						{
							// Got a poly definition.
							if( IsFace ) NewPoly.Reverse();
							NewPoly.Base = NewPoly.Vertex[0];
							NewPoly.Finalize(0);
							AddItem(NewPoly);
							//debugf("DXF: Added poly %i",Num);
						}
						else
						{
							// Bad.
							debugf( "DXF: Bad vertex count %i", NewPoly.NumVertices );
						}
						
						// Prepare for next.
						NewPoly.Init();
					}
					Started=0;

					if( GetCMD(&Str,"VERTEX") )
					{
						// Start of new vertex.
						//debugf("DXF: Vertex");
						PointPool[NumPts] = FVector(0,0,0);
						Started = 1;
						IsFace  = 0;
					}
					else if( GetCMD(&Str,"3DFACE") )
					{
						// Start of 3d face definition.
						//debugf("DXF: 3DFace");
						Started = 1;
						IsFace  = 1;
					}
					else if( GetCMD(&Str,"SEQEND") )
					{
						// End of sequence.
						//debugf("DXF: SEQEND");
						NumPts=0;
					}
					else if( GetCMD(&Str,"EOF") )
					{
						// End of file.
						//debugf("DXF: End");
						break;
					}
				}
				else if( Started )
				{
					// Replace commas with periods to handle european dxf's.
					for( char *Stupid = strchr(ExtraLine,','); Stupid; Stupid=strchr(Stupid,',') )
						*Stupid = '.';

					// Handle codes.
					if( Code>=10 && Code<=19 )
					{
						// X coordinate.
						if( IsFace && Code-10==NewPoly.NumVertices )
						{
							//debugf("DXF: NewVertex %i",NewPoly.NumVertices);
							NewPoly.Vertex[NewPoly.NumVertices++] = FVector(0,0,0);
						}
						NewPoly.Vertex[Code-10].X = PointPool[NumPts].X = atof(ExtraLine);
					}
					else if( Code>=20 && Code<=29 )
					{
						// Y coordinate.
						NewPoly.Vertex[Code-20].Y = PointPool[NumPts].Y = atof(ExtraLine);
					}
					else if( Code>=30 && Code<=39 )
					{
						// Z coordinate.
						NewPoly.Vertex[Code-30].Z = PointPool[NumPts].Z = atof(ExtraLine);
					}
					else if( Code>=71 && Code<=79 && (Code-71)==NewPoly.NumVertices )
					{
						INT iPoint = abs(atoi(ExtraLine));
						if( iPoint>0 && iPoint<=NumPts )
							NewPoly.Vertex[NewPoly.NumVertices++] = PointPool[iPoint-1];
						else debugf("DXF: Invalid point index %i/%i",iPoint,NumPts);
					}
				}
			}
		}
		else if( strstr(Str,"Tri-mesh,") && First )
		{
			// 3DS .ASC file.
			debug(LOG_Info,"Reading 3D Studio ASC file");
			FVector PointPool[4096];

			AscReloop:
			int NumVerts = 0, TempNumPolys=0, TempVerts=0;
			while( GetLINE(Stream,StrLine,256)==0 )
			{
				Str=StrLine;

				char VertText[256],FaceText[256];
				sprintf(VertText,"Vertex %i:",NumVerts);
				sprintf(FaceText,"Face %i:",TempNumPolys);

				if( strstr(Str,VertText) )
				{
					PointPool[NumVerts].X = atof(strstr(Str,"X:")+2);
					PointPool[NumVerts].Y = atof(strstr(Str,"Y:")+2);
					PointPool[NumVerts].Z = atof(strstr(Str,"Z:")+2);
					NumVerts++;
					TempVerts++;
				}
				else if( strstr(Str,FaceText) )
				{
					Poly.Init();
					Poly.NumVertices=3;
					Poly.Vertex[0]=PointPool[atoi(strstr(Str,"A:")+2)];
					Poly.Vertex[1]=PointPool[atoi(strstr(Str,"B:")+2)];
					Poly.Vertex[2]=PointPool[atoi(strstr(Str,"C:")+2)];
					Poly.Base = Poly.Vertex[0];
					Poly.Finalize(0);
					AddItem(Poly);
					TempNumPolys++;
				}
				else if( strstr(Str,"Tri-mesh,") ) goto AscReloop;
			}
			debugf(LOG_Info,"Imported %i vertices, %i faces",TempVerts,Num);
		}
		else if( GetBEGIN(&Str,"POLYGON") ) // Unreal .T3D file.
		{
			// Init to defaults and get group/item and texture.
			Poly.Init();
			GetINDEX	(Str,"LINK=", &Poly.iLink);
			GetNAME		(Str,"GROUP=",&Poly.GroupName);
			GetNAME		(Str,"ITEM=", &Poly.ItemName);
			GetUTexture	(Str,"TEXTURE=",Poly.Texture);
			GetDWORD	(Str,"FLAGS=",&Poly.PolyFlags);
			Poly.PolyFlags &= ~PF_NoImport;
		}
		else if( GetCMD(&Str,"PAN") )
		{
			GetSWORD(Str,"U=",&Poly.PanU);
			GetSWORD(Str,"V=",&Poly.PanV);
		}
		else if( GetCMD(&Str,"ORIGIN") )
		{
			GotBase=1;
			GetFVECTOR(Str,&Poly.Base);
		}
		else if( GetCMD(&Str,"NORMAL") )
		{
			// Ignore it - we compute normals internally for better accuracy.
		}
		else if( GetCMD(&Str,"VERTEX") )
		{
			if( Poly.NumVertices < FPoly::MAX_VERTICES )
			{
				GetFVECTOR(Str,&Poly.Vertex[Poly.NumVertices]);
				Poly.NumVertices++;
			}
		}
		else if( GetCMD(&Str,"TEXTUREU") )
		{
			GetFIXFVECTOR (Str,&Poly.TextureU);
		}
		else if( GetCMD(&Str,"TEXTUREV") )
		{
			GetFIXFVECTOR(Str,&Poly.TextureV);
		}
		else if( GetEND(&Str,"POLYGON") )
		{
			if( !GotBase )
				Poly.Base = Poly.Vertex[0];
			if( Poly.Finalize(1)==0 )
				AddItem(Poly);
			GotBase=0;
		}
	}
	Unlock(LOCK_ReadWrite);
	unguard;
}

/*---------------------------------------------------------------------------------------
	Backfacing.
---------------------------------------------------------------------------------------*/

//
// Return whether this poly and Test are facing each other.
// The polys are facing if they are noncoplanar, one or more of Test's points is in 
// front of this poly, and one or more of this poly's points are behind Test.
//
int FPoly::Faces( const FPoly &Test ) const
{
	guard(FPoly::Faces);

	// Coplanar implies not facing.
	if( IsCoplanar( Test ) )
		return 0;

	// If this poly is frontfaced relative to all of Test's points, they're not facing.
	for( int i=0; i<Test.NumVertices; i++ )
	{
		if( !IsBackfaced( Test.Vertex[i] ) )
		{
			// If Test is frontfaced relative to on or more of this poly's points, they're facing.
			for( i=0; i<NumVertices; i++ )
				if( Test.IsBackfaced( Vertex[i] ) )
					return 1;
			return 0;
		}
	}
	return 0;
	unguard;
}

/*---------------------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------------------*/
