/*=============================================================================
	UnRender.cpp: Main Unreal rendering functions and pipe

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "Unreal.h"
#include "UnRender.h"
#include "UnRaster.h"
#include "UnRenDev.h"
#include "UnFireEn.h"

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

#if STATS
FRenderStats GStat;
#endif

extern FGlobalRaster GRaster;

//#define ASM_LATTICE ASM

class FMipTable
{
	public:
	BYTE MipLevel; 
	BYTE RoutineOfs;
} GSmallMipTable[512*4], GLargeMipTable[512*4], GOtherMipTable[512*4], GNoMipTable[512*4];

extern "C"
{
	ASMVAR FMipTable	*TRL_MipTable;
	ASMVAR FLOAT		TRL_FBaseU;
	ASMVAR FLOAT		TRL_FBaseV;
	ASMVAR DWORD		TRL_TexBaseU;
	ASMVAR DWORD		TRL_TexBaseV;
	ASMVAR DWORD		TRL_LightBaseU;
	ASMVAR DWORD		TRL_LightBaseV;
	ASMVAR DWORD		TRL_LightVMask;
	ASMVAR DWORD		TRO_DitherMask;
	ASMVAR BYTE			*TRL_MipRef;
	ASMVAR BYTE			TRL_LightMeshShift;
	ASMVAR BYTE			TRL_LightMeshUShift;
	ASMVAR BYTE			TRL_RoutineOfsEffectBase;
	void CDECL			TRL_RectLoop();
	void CDECL			TRL_SelfModRect();
};

/*-----------------------------------------------------------------------------
	FRender statics.
-----------------------------------------------------------------------------*/

DWORD 			FRender::Stamp;

FMemStack		FRender::PointMem;
FMemStack		FRender::VectorMem;

FMemMark		FRender::PointMark;
FMemMark		FRender::VectorMark;

FRender::FStampedPoint  *FRender::PointCache;
FRender::FStampedVector *FRender::VectorCache;

FTexLattice		*FRender::LatticePtr[MAX_YR][MAX_XR];

FRenderStats	*FRender::Stat;

/*-----------------------------------------------------------------------------
	FRender init & exit.
-----------------------------------------------------------------------------*/

//
// Initialize the rendering engine and allocate all temporary buffers.
// Calls appError if failure.
//
void FRender::Init()
{
	guard(FRender::Init);

	// Validate stuff.
	checkState(FBspNode::MAX_NODE_VERTICES<=FPoly::MAX_VERTICES);
	checkState(sizeof(FVector)==12);
	checkState(sizeof(FRotation)==12);
	checkState(sizeof(FCoords)==48);

	GBlitPtr			= &GBlit;
	MaxDynamics			= 1024;
	MaxPostDynamics		= 768;

	// Allocate rendering stuff.
	PointCache		= appMallocArray(MAX_POINTS,FStampedPoint,		"PointCache");
	VectorCache		= appMallocArray(MAX_VECTORS,FStampedVector,	"VectorCache");
	DynamicsIndex   = appMallocArray(MaxDynamics,FDynamicsIndex,	"DynamicsCache");
	PostDynamics    = appMallocArray(MaxPostDynamics,INDEX,			"PostDynCache");

	memset( LatticePtr, 0, sizeof(LatticePtr) );

	InitDither();
	GCache.Flush();

	// Initialize dynamics info.
	DynamicsLocked  = 0;

	// Misc params.
	TRO_DitherMask	= 3;
	Toggle			= 0;
	RendIter		= 0;
	ShowLattice		= 0;
	DoDither		= 1;
	ShowChunks		= 0;
	LeakCheck		= 0;
	Temporal		= 0;
	Antialias		= 0;
	Extra1			= 0;
	Extra2			= 0;
	Extra3			= 0;
	Extra4			= 0;
	AllStats		= 0;
	MipMultiplier	= 1.0;

	for(int i=0; i<MAX_POINTS; i++ ) PointCache [i].Stamp = Stamp;
	for(    i=0; i<MAX_VECTORS; i++) VectorCache[i].Stamp = Stamp;

	// Init stats.
	STAT(memset(&GStat,0,sizeof(GStat));)
	STAT(Stat = &GStat;)

	GRaster.Init();

	for( i=0; i<MAX_MIPS; i++ )
		GBlit.MipRef[i] = GBlit.PrevMipRef[i] = 0;

	for( int n=0; n<2048; n++ )
	{
		int i   = (n<1024) ? (n) : (1024-n);
		int Mip = (i/4) - 127;
		int Ofs = (i&3);
		if( Mip<0 )
		{
			GSmallMipTable[n].MipLevel    = 0;
			GSmallMipTable[n].RoutineOfs  = 1;
		}
		else if( Mip<7 )
		{
			GSmallMipTable[n].MipLevel    = Mip;
			GSmallMipTable[n].RoutineOfs  = Mip*16 + (i&3)*4 + 1;
		}
		else
		{
			GSmallMipTable[n].MipLevel    = 7;
			GSmallMipTable[n].RoutineOfs  = 7*16 + 1;
		}
		GOtherMipTable[n] = GSmallMipTable[n>0 ? n-1 : 0];

		GLargeMipTable[n].MipLevel   = Clamp(Mip,0,7);
		GLargeMipTable[n].RoutineOfs = Clamp(Mip,0,7)*16 + 1;

		GNoMipTable[n].MipLevel      = 0;
		GNoMipTable[n].RoutineOfs    = 1;
	}
	GLightManager->Init();

	// Allocate memory stacks.
	PointMem.Init (GCache,16384,65536);
	VectorMem.Init(GCache,2048, 8192);

	debug(LOG_Init,"Rendering initialized");
	unguard;
}

//
// Shut down the rendering engine
//
void FRender::Exit()
{
	guard(FRender::Exit);

	GRaster.Exit();

	appFree(PointCache);
	appFree(VectorCache);
	appFree(DynamicsIndex);
	appFree(PostDynamics);

	debug(LOG_Exit,"Rendering closed");

	// Shut down memory stacks.
	PointMem.Exit();

	unguard;
}

/*-----------------------------------------------------------------------------
	FRender Stats display.
-----------------------------------------------------------------------------*/

enum {STAT_Y = 16};

void FRender::DrawStats( UCamera *Camera )
{
	guard(FRender::DrawStats);
	char TempStr[256];

	ThisEndTime			= GApp->MicrosecondTime()/1000;
	FLOAT FrameTime		= ((FLOAT)ThisEndTime - (FLOAT)LastEndTime  );
	FLOAT RenderTime	= ((FLOAT)ThisEndTime - (FLOAT)ThisStartTime);

	if( QuickStats )
	{
		int XL,YL;
		sprintf
		(
			TempStr,
			"Fps=%05.1f (%05.1f) Nodes=%03i Polys=%03i",
			1000.0/FrameTime,
			1000.0/RenderTime,
			NodesDraw,
			PolysDraw
		);
		GGfx.SmallFont->StrLen(XL,YL,0,0,TempStr);

		int Y=Camera->Y;
		Camera->Texture->BurnRect(0,Camera->X,Y-YL-3,Y,0);
		GGfx.SmallFont->Printf(Camera->Texture,(Camera->X-XL)/2,Y-YL-2,0,TempStr);
	}
#if STATS
	if (AllStats)
	{
		int	 StatYL=0;

		sprintf(TempStr,"  TIME REALFPS=%05.1f (%05.1f MSEC) RENDFPS=%05.1f (%05.1f MSEC)",
			1000.0/FrameTime,
			FrameTime,
			1000.0/RenderTime,
			RenderTime);
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  MSEC Tmap=%05.1f Blit=%05.1f Illum=%05.1f Occ=%05.1f Pal=%05.1f",
			GApp->CpuToMilliseconds(GStat.TextureMap),
			GApp->CpuToMilliseconds(GCameraManager->DrawTime),
			GApp->CpuToMilliseconds(GStat.IllumTime),
			GApp->CpuToMilliseconds(GStat.OcclusionTime),
			GApp->CpuToMilliseconds(GStat.PalTime));
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  TICK Script=%05.1f Level=%05.1f Actor=%05.1f Audio=%05.1f Frags=%i",
			GApp->CpuToMilliseconds(GServer.ScriptExecTime),
			GApp->CpuToMilliseconds(GServer.LevelTickTime),
			GApp->CpuToMilliseconds(GServer.ActorTickTime),
			GApp->CpuToMilliseconds(GServer.AudioTickTime),
			GServer.ActorCollisionFrags);
		ShowStat	(Camera,&StatYL,TempStr);

		sprintf		(TempStr,"  CACH ");
		GCache.Status(TempStr+strlen(TempStr));
		ShowStat	(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  LITE PTS=%05i MESHES=%04i LITAGE=%04i LITMEM=%04iK",
			GStat.MeshPtsGen,
			GStat.MeshesGen,
			GStat.Lightage,
			GStat.LightMem>>10);
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  TEX  UNQTEX=%i TEXMEM=%iK, MODS=%i",
			GStat.UniqueTextures,
			GStat.UniqueTextureMem>>10,
			GStat.CodePatches);
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  BSP  VISIT=%05i/%05i BEHIND=%05i",
			GStat.NodesDone,
			GStat.NodesTotal,
			GStat.NodesBehind);
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  EXTR %05i %05i %05i %05i Invert=%05.1f",
			GStat.Extra1,
			GStat.Extra2,
			GStat.Extra3,
			GStat.Extra4,
			GApp->CpuToMilliseconds(GStat.InversionTime));
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  OCCL Checks=%04i Back=%03i In=%03i PyrOut=%03i SpanOcc=%03i",
			GStat.BoxChecks,
			GStat.BoxBacks,
			GStat.BoxIn,
			GStat.BoxOutOfPyramid,
			GStat.BoxSpanOccluded);
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  MEM  GMem=%iK GDynMem=%iK",
			GStat.GMem>>10,
			GStat.GDynMem>>10);
		ShowStat	(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  CLIP Accept=%05i Reject=%05i Nil=%05i",
			GStat.ClipAccept,
			GStat.ClipOutcodeReject,
			GStat.ClipNil);
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  ZONE Cur=%02i Visible=%02i/%02i Reject=%i",
			GStat.CurZone,
			GStat.VisibleZones,
			GStat.NumZones,
			GStat.MaskRejectZones);
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  RAST BoxRej=%04i Rasterized=%04i Nodes=%04i Polys=%04i",
			GStat.NumRasterBoxReject,
			GStat.NumRasterPolys,
			NodesDraw,
			PolysDraw);
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  SIDE Setup=%04i Cached=%04i",
			GStat.NumSides,
			GStat.NumSidesCached);
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  SPRT NUM=%i MESH=%i CHNKS=%i FINAL=%i DRAWN=%i",
			GStat.NumSprites,
			GStat.MeshesDrawn,
			GStat.NumChunks,
			GStat.NumFinalChunks,
			GStat.ChunksDrawn);
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  SPAN CHURN=%i REJIG=%03i",
			GStat.SpanTotalChurn,
			GStat.SpanRejig);
		ShowStat(Camera,&StatYL,TempStr);

#if DO_SLOW_CLOCK
		sprintf(TempStr,"  MSEC Gen=%05.1f Lat=%05.1f TMap=%05.1f Asm=%05.1f",
			GApp->CpuToMilliseconds(GStat.Generate),
			GApp->CpuToMilliseconds(GStat.CalcLattice),
			GApp->CpuToMilliseconds(GStat.Tmap),
			GApp->CpuToMilliseconds(GStat.Asm));
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  SPAN XV=%02.2f OV=%02.2f CRU=%02.2f CR=%02.2f CIF=%02.2f (MSEC)",
			GApp->CpuToMilliseconds(GStat.BoxIsVisible),
			GApp->CpuToMilliseconds(GStat.BoundIsVisible),
			GApp->CpuToMilliseconds(GStat.CopyFromRasterUpdate),
			GApp->CpuToMilliseconds(GStat.CopyFromRaster),
			GApp->CpuToMilliseconds(GStat.CopyIndexFrom));
		ShowStat(Camera,&StatYL,TempStr);

		sprintf(TempStr,"  SPAN CF=%02.2f MW=%02.2f MF=%02.2f CRF=%02.2f CLF=%02.2f",
			GApp->CpuToMilliseconds(GStat.CopyFromRange),
			GApp->CpuToMilliseconds(GStat.MergeWith),
			GApp->CpuToMilliseconds(GStat.MergeFrom),
			GApp->CpuToMilliseconds(GStat.CalcRectFrom),
			GApp->CpuToMilliseconds(GStat.CalcLatticeFrom));
		ShowStat(Camera,&StatYL,TempStr);
#endif
		sprintf(TempStr,"  ILLU VLITES=%03i LATS=%05i LATVLIGHTS=%05i",
			GStat.DynLightActors,
			GStat.LatsMade,
			GStat.LatLightsCalc);
		ShowStat(Camera,&StatYL,TempStr);

		// Show cache stats.
		GCache.DrawCache(Camera->Screen,Camera->X,Camera->Y,Camera->ColorBytes);
	}
#endif // STATS
	unguard;
}

//
// Show one statistic and update the pointer.
//
void FRender::ShowStat( UCamera *Camera, int *StatYL,const char *Str )
{
	guard(FRender::ShowStat);

	int	XL,YL;
	if( *StatYL==0 )
	{
		GGfx.SmallFont->Printf(Camera->Texture,16,STAT_Y + *StatYL,0,"Statistics:");
		GGfx.SmallFont->StrLen(XL,YL,0,1,"Statistics:");
		*StatYL += YL;
	}
	GGfx.SmallFont->Printf(Camera->Texture,16,STAT_Y + *StatYL,0,"%s",Str);
    GGfx.SmallFont->StrLen (XL,YL,0,1,Str);
	*StatYL += YL;
	unguard;
}

/*-----------------------------------------------------------------------------
	FRender PreRender & PostRender.
-----------------------------------------------------------------------------*/

//
// Set up for rendering a frame.
//
void FRender::PreRender( UCamera *Camera )
{
	guard(FRender::PreRender);

	// Set math to low precision.
	GApp->EnableFastMath(1);

	// Init stats.
	STAT(memset(Stat,0,sizeof(FRenderStats)));
	LastEndTime   = ThisEndTime;
	ThisStartTime = GApp->MicrosecondTime()/1000;

	// Init counts.
	NodesDraw		= 0;
	PolysDraw		= 0;

	// Bump the iteration count.
	RendIter++;

	// Reset all stats to zero.
	STAT(memset(&GStat,0,sizeof(GStat)));

	unguard;
}

//
// Clean up after rendering a frame.
//
void FRender::PostRender( UCamera *Camera )
{
	// Draw whatever stats were requested.
	if( Camera->Actor->RendMap==REN_Polys || Camera->Actor->RendMap==REN_PolyCuts || Camera->Actor->RendMap==REN_DynLight || Camera->Actor->RendMap==REN_PlainTex )
		DrawStats( Camera );

	// Restore default precision.
	GApp->EnableFastMath(0);
}

/*-----------------------------------------------------------------------------
	FRender command line.
-----------------------------------------------------------------------------*/

//
// Execute a command line.
//
int FRender::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(FRender::Exec);
	const char *Str = Cmd;

	if( GetCMD(&Str,"STATS") )
	{
		AllStats ^= 1; 
		Out->Logf("Stats are %s",AllStats?"on":"off"); 
		return 1;
	}
	else if( GetCMD(&Str,"FPS") )
	{
		QuickStats ^= 1; 
		Out->Logf("FPS display is %s",QuickStats?"on":"off"); 
		return 1;
	}
	else if( GetCMD(&Str,"DETAIL") )
	{
		if (GetCMD(&Str,"LOW"))
		{
			// Low detail.
			TRO_DitherMask = 0;
			Out->Logf("Low detail");
			return 1;
		}
		else if (GetCMD(&Str,"MEDIUM"))
		{
			// Low detail.
			TRO_DitherMask = 1;
			Out->Logf("Medium detail");
			return 1;
		}
		else if (GetCMD(&Str,"HIGH"))
		{
			// Low detail.
			TRO_DitherMask = 3;
			Out->Logf("High detail");
			return 1;
		}
		else
		{
			Out->Logf("Bad detail setting");
			return 1;
		}
	}
	else if( GetCMD(&Str,"REND") )
	{
		if 		(GetCMD(&Str,"SMOOTH"))		GGfx.Smooth		^= 1;
		else if (GetCMD(&Str,"STRETCH"))	GGfx.Stretch	^= 1;
		else if (GetCMD(&Str,"LEAK"))		LeakCheck		^= 1;
		else if (GetCMD(&Str,"BILINEAR"))	DoDither		^= 1;
		else if (GetCMD(&Str,"CUTS"))		ShowChunks		^= 1;
		else if (GetCMD(&Str,"LATTICE"))	ShowLattice		^= 1;
		else if (GetCMD(&Str,"EXTRA1"))		Extra1			^= 1;
		else if (GetCMD(&Str,"EXTRA2"))		Extra2			^= 1;
		else if (GetCMD(&Str,"EXTRA3"))		Extra3			^= 1;
		else if (GetCMD(&Str,"EXTRA4"))		Extra4			^= 1;
		else if (GetCMD(&Str,"T"))			Toggle			^= 1;
		else if (GetCMD(&Str,"TEMPORAL"))	Temporal		^= 1;
		else if (GetCMD(&Str,"ANTIALIAS"))	Antialias		^= 1;
		else if (GetCMD(&Str,"SET"))
		{
			GetFLOAT(Str,"MIPMULT=",&MipMultiplier);
		}
		else return 0;
		Out->Log("Rendering option recognized");
		return 1;
	}
	else return 0; // Not executed
	unguard;
}

/*-----------------------------------------------------------------------------
	FRender point and vector transformation cache.
-----------------------------------------------------------------------------*/

//
// Initialize the point and vector transform caches for a frame.
//
void FRender::InitTransforms( UModel *Model )
{
	guard(FRender::InitTransforms);

	// Push memory stacks.
	PointMark.Push(PointMem);
	VectorMark.Push(VectorMem);

	// Increment stamp.
	Stamp++;

	unguard;
}

//
// Free and empty the point and vector transform caches.
//
void FRender::ExitTransforms( void )
{
	guard(FRender::ExitTransforms);

	// Pop memory stacks.
	VectorMark.Pop();
	PointMark.Pop();

	unguard;
}

/*--------------------------------------------------------------------------
	Clippers.
--------------------------------------------------------------------------*/

//
// Clipping macro used by ClipBspSurf and ClipTexPts.
//
#define CLIPPER(DOT1EXPR,DOT2EXPR,CACHEDVIEWPLANE)\
{\
	Num1    = 0;\
	SrcPtr  = &SrcList  [0];\
	DestPtr = &DestList [0];\
	P1		= SrcList   [Num0-1];\
	Dot1	= DOT1EXPR;\
	while( Num0-- > 0 )\
	{\
		P2   = *SrcPtr++;\
		Dot2 = DOT2EXPR;\
		if( Dot1 >= 0.0 ) /* P1 is unclipped */ \
		{\
			*DestPtr++ = P1;\
			Num1++;\
			if( Dot2 < 0.0 ) /* P1 is unclipped, P2 is clipped. */ \
			{\
				*Top = *P2 + (*P1-*P2) * (Dot2/(Dot2-Dot1));\
				Top->iTransform = INDEX_NONE;\
				Top->iSide      = P2->iSide; /* Cache partially clipped side rasterization */ \
				*DestPtr++		= Top++;\
				Num1++;\
			}\
		}\
		else if( Dot2 > 0.0 ) /* P1 is clipped, P2 is unclipped. */ \
		{\
			*Top            = *P2 + (*P1-*P2) * (Dot2/(Dot2-Dot1));\
			Top->iTransform = INDEX_NONE;\
			Top->iSide      = CACHEDVIEWPLANE; /* Fully clipped side - use cache from view planes 0-3 */ \
			*DestPtr++      = Top++;\
			Num1++;\
		}\
		P1   = P2;\
		Dot1 = Dot2;\
	}\
	if( Num1<3 ) {STAT(GStat.ClipNil++; unclockSlow(GStat.Clip);); return 0;};\
	Num0     = Num1;\
	TempList = SrcList;\
	SrcList  = DestList;\
	DestList = TempList;\
}

//
// Transform a Bsp poly into a list of points, and return number of points or zero
// if outcode rejected.
//
int inline FRender::TransformBspSurf
(
	UModel		*Model,
	UCamera		*Camera,
	INDEX		iNode,
	FTransform	**Pts, 
	BYTE		&AllCodes
)
{
	STAT(clockSlow(GStat.Transform));

	FBspNode   		*Node		= &Model->Nodes(iNode);
	FVert			*VertPool	= &Model->Verts(Node->iVertPool);
	BYTE			Outcode		= FVF_OutReject;
	FTransform		**DestPtr	= &Pts[0];
	int				Num			= Node->NumVertices;;

	// Transform, outcode reject, and build initial point list.
	AllCodes = 0;
	for( int i=0; i<Num; i++ )
	{
		*DestPtr			 = &GetPoint(Camera,VertPool->pVertex);
		(*DestPtr)->iSide	 = VertPool->iSide;
		Outcode				&= (*DestPtr)->Flags;
		AllCodes			|= (*DestPtr)->Flags;

		debugState(VertPool->iSide==INDEX_NONE || VertPool->iSide>=4);

		VertPool++;
		DestPtr++;
	}
	STAT(unclockSlow(GStat.Transform));
	if (Outcode) return 0;
	else return Num;
}

//
// Transform and clip a Bsp poly.  Outputs the transformed points to OutPts and
// returns the number of points, which will be zero or >=3.
//
int FRender::ClipBspSurf
(
	UModel		*Model,
	UCamera		*Camera,
	INDEX		iNode,
	FTransform	*OutPts
)
{
	guard(FRender::ClipBspSurf);
	STAT(clockSlow(GStat.Clip));

	static FTransform WorkPts[FBspNode::MAX_FINAL_VERTICES],*List0[FBspNode::MAX_FINAL_VERTICES],*List1[FBspNode::MAX_FINAL_VERTICES];
	FTransform		*Top,*P1,*P2;
	FTransform		**SrcList,**DestList,**TempList,**SrcPtr,**DestPtr;
	FLOAT			Dot1,Dot2;
	BYTE			AllCodes;
	int				Num0,Num1;

	Num0 = TransformBspSurf(Model,Camera,iNode,List0,AllCodes);
	if( !Num0 )
	{
		STAT(GStat.ClipOutcodeReject++;)
		STAT(unclockSlow(GStat.Clip));
		return 0;
	}
	Top = &WorkPts[0];
	SrcList	 = List0;
	DestList = List1;

	// Clip point list by each view frustrum clipping plane.
	if( AllCodes & FVF_OutXMin ) CLIPPER
	(
		P1->X * Camera->ProjZRX2 + P1->Z,
		P2->X * Camera->ProjZRX2 + P2->Z,0
	); 
	if( AllCodes & FVF_OutXMax ) CLIPPER
	(
		P1->Z - P1->X * Camera->ProjZRX2,
		P2->Z - P2->X * Camera->ProjZRX2,1
	);
	if( AllCodes & FVF_OutYMin ) CLIPPER
	(
		P1->Y * Camera->ProjZRY2 + P1->Z,
		P2->Y * Camera->ProjZRY2 + P2->Z,2
	); 
	if( AllCodes & FVF_OutYMax ) CLIPPER
	(
		P1->Z - P1->Y * Camera->ProjZRY2,
		P2->Z - P2->Y * Camera->ProjZRY2,3
	);
	P2     = &OutPts  [0];
	SrcPtr = &SrcList [0];
	for( int i=0; i<Num0; i++ )
	{
		P1 = *SrcPtr++;
		if( P1->iTransform==INDEX_NONE || P1->ScreenX==-1 )
		{
			FLOAT Factor = Camera->ProjZ / P1->Z;
			P1->ScreenX  = 65536.0 * (P1->X * Factor + Camera->FX15);
			P1->ScreenY  = P1->Y * Factor + Camera->FY15;
		}
		*P2++ = *P1;
	}
	STAT(GStat.ClipAccept++;)
	STAT(unclockSlow(GStat.Clip));
	return Num0;

	unguard;
}

//
// Clip a set of points with vertex texture coordinates and vertex lighting values.
// Outputs the points to OutPts and returns the number of poiints, which will be
// 0 or >=3.
//
int FRender::ClipTexPoints( UCamera *Camera, FTransTexture *InPts, FTransTexture *OutPts, int Num0 )
{
	guard(FRender::ClipTexPoints);
	STAT(clockSlow(GStat.Clip));

	static FTransTexture WorkPts[FBspNode::MAX_FINAL_VERTICES],*List0[FBspNode::MAX_FINAL_VERTICES],*List1[FBspNode::MAX_FINAL_VERTICES];
	FTransTexture	*Top,*P1,*P2;
	FTransTexture	**SrcList,**DestList,**TempList,**SrcPtr,**DestPtr;
	FLOAT			Dot1,Dot2;
	BYTE			AllCodes,Outcode;
	int				Num1;

	DestPtr	 = &List0[0];
	Outcode  = FVF_OutReject;
	AllCodes = 0;
	for( int i=0; i<Num0; i++ )
	{
		*DestPtr     = &InPts[i];
		Outcode		&= (*DestPtr)->Flags;
		AllCodes	|= (*DestPtr)->Flags;
		DestPtr++;
	}
	if( Outcode ) return 0;

	Top      = &WorkPts[0];
	SrcList	 = List0;
	DestList = List1;

	// Clip point list by each view frustrum clipping plane.
	if( AllCodes & FVF_OutXMin ) CLIPPER
	(
		P1->X * Camera->ProjZRX2 + P1->Z,
		P2->X * Camera->ProjZRX2 + P2->Z,INDEX_NONE
	); 
	if( AllCodes & FVF_OutXMax ) CLIPPER
	(
		P1->Z - P1->X * Camera->ProjZRX2,
		P2->Z - P2->X * Camera->ProjZRX2,INDEX_NONE
	);
	if( AllCodes & FVF_OutYMin ) CLIPPER
	(
		P1->Y * Camera->ProjZRY2 + P1->Z,
		P2->Y * Camera->ProjZRY2 + P2->Z,INDEX_NONE
	); 
	if( AllCodes & FVF_OutYMax ) CLIPPER
	(
		P1->Z - P1->Y * Camera->ProjZRY2,
		P2->Z - P2->Y * Camera->ProjZRY2,INDEX_NONE
	);
	P2     = &OutPts  [0];
	SrcPtr = &SrcList [0];
	for( i=0; i<Num0; i++ )
	{
		P1 = *SrcPtr++;
		if( P1->iTransform == INDEX_NONE )
			P1->Transform( Camera, 0 );
		*P2++ = *P1;
	}
	STAT(unclockSlow(GStat.Clip));
	return Num0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Bsp occlusion functions.
-----------------------------------------------------------------------------*/

//
// Checks whether the node's bouding box is totally occluded.  Returns 0 if
// total occlusion, 1 if all or partial visibility.
//
int FRender::BoundVisible
(
	UCamera			*Camera,
	FBoundingRect	*Bound,
	FSpanBuffer		*SpanBuffer,
	FScreenBounds	*Results,
	FCoords			*Rotation
)
{
	guard(FRender::BoundVisible);
	STAT(GStat.BoxChecks++);

	FCoords			*CameraCoords;
	FCoords			TempCoords;
	FCoords       	BoxDot[2];
	FTransform		Pts[8],*Pt;
	FVector 		CameraLoc;
	FVector       	NewMin,NewMax;
	FLOAT         	Persp1,Persp2,Temp,BoxMinZ,BoxMaxZ;
	int         	BoxX,BoxY;
	int 			BoxMinX,BoxMaxX,BoxMinY,BoxMaxY;
	int				i,OutCode;

	if( !Rotation )
	{
		// Box is axis-aligned; use camera coordinate system as-is.
		CameraCoords = &Camera->Coords;
	}
	else
	{
		// Generate a new, rotated camera coordinate system to make up for the box's rotation:
		TempCoords        = Camera->Coords * (*Rotation);
		TempCoords.Origin = (Camera->Coords.Origin - Rotation->Origin).TransformVectorBy(*Rotation) + Rotation->Origin;
		CameraCoords      = &TempCoords;
	}

	// Handle rejection in orthogonal views.
	if( Camera->IsOrtho() )
	{
		FLOAT SX1,SY1,SX2,SY2;
		Project (Camera,&Bound->Min,&SX1,&SY1,NULL);
		Project (Camera,&Bound->Max,&SX2,&SY2,NULL);

		return
		(
			!(
				((SX1<0.0       )&&(SX2<0.0       )) ||
				((SY1<0.0       )&&(SY2<0.0       )) ||
				((SX1>Camera->FX)&&(SX2>Camera->FX)) ||
				((SY1>Camera->FY)&&(SY2>Camera->FY))
			)
		);
	}

	// Trivial accept: If camera is within bouding box, skip calculations because
	// the bounding box is definitely in view.
	CameraLoc = CameraCoords->Origin;
	NewMin    = Bound->Min - CameraLoc;
	NewMax    = Bound->Max - CameraLoc;
	if(
		(NewMin.X < 0.0) && (NewMax.X > 0.0) &&
		(NewMin.Y < 0.0) && (NewMax.Y > 0.0) &&
		(NewMin.Z < 0.0) && (NewMax.Z > 0.0)
	)
	{
		if (Results) Results->Valid = 0;
		STAT(GStat.BoxIn++);
		return 1; // Visible
	}

	// Test bounding-box side-of-camera rejection.  Since box is axis-aligned,
	// this can be optimized: Box can only be rejected if all 8 dot products of the
	// 8 box sides are less than zero.  This is the case iff each dot product
	// component is less than zero.
	BoxDot[0].ZAxis.X = NewMin.X * CameraCoords->ZAxis.X; BoxDot[1].ZAxis.X = NewMax.X * CameraCoords->ZAxis.X;
	BoxDot[0].ZAxis.Y = NewMin.Y * CameraCoords->ZAxis.Y; BoxDot[1].ZAxis.Y = NewMax.Y * CameraCoords->ZAxis.Y;
	BoxDot[0].ZAxis.Z = NewMin.Z * CameraCoords->ZAxis.Z; BoxDot[1].ZAxis.Z = NewMax.Z * CameraCoords->ZAxis.Z;

	if(
		(BoxDot[0].ZAxis.X<=0.0) && (BoxDot[0].ZAxis.Y<=0.0) && (BoxDot[0].ZAxis.Z<=0.0) &&
		(BoxDot[1].ZAxis.X<=0.0) && (BoxDot[1].ZAxis.Y<=0.0) && (BoxDot[1].ZAxis.Z<=0.0)
	)
	{
		if (Results) Results->Valid = 0;
		STAT(GStat.BoxBacks++);
		return 0; // Totally behind camera - invisible
	}

	// Transform bounding box min and max coords into screenspace.
	BoxDot[0].XAxis.X = NewMin.X * CameraCoords->XAxis.X; BoxDot[1].XAxis.X = NewMax.X * CameraCoords->XAxis.X;
	BoxDot[0].XAxis.Y = NewMin.Y * CameraCoords->XAxis.Y; BoxDot[1].XAxis.Y = NewMax.Y * CameraCoords->XAxis.Y;
	BoxDot[0].XAxis.Z = NewMin.Z * CameraCoords->XAxis.Z; BoxDot[1].XAxis.Z = NewMax.Z * CameraCoords->XAxis.Z;

	BoxDot[0].YAxis.X = NewMin.X * CameraCoords->YAxis.X; BoxDot[1].YAxis.X = NewMax.X * CameraCoords->YAxis.X;
	BoxDot[0].YAxis.Y = NewMin.Y * CameraCoords->YAxis.Y; BoxDot[1].YAxis.Y = NewMax.Y * CameraCoords->YAxis.Y;
	BoxDot[0].YAxis.Z = NewMin.Z * CameraCoords->YAxis.Z; BoxDot[1].YAxis.Z = NewMax.Z * CameraCoords->YAxis.Z;

	// View-pyramid reject (outcode test).
	int ThisCode,AllCodes;

	BoxMinZ=Pts[0].Z;
	BoxMaxZ=Pts[0].Z;
	OutCode  = 1|2|4|8;
	AllCodes = 0;
	#define CMD(i,j,k,First,P)\
		\
		ThisCode = 0;\
		\
		P.Z = BoxDot[i].ZAxis.X + BoxDot[j].ZAxis.Y + BoxDot[k].ZAxis.Z;\
		if (First || (P.Z < BoxMinZ)) BoxMinZ = P.Z;\
		if (First || (P.Z > BoxMaxZ)) BoxMaxZ = P.Z;\
		\
		P.X = BoxDot[i].XAxis.X + BoxDot[j].XAxis.Y + BoxDot[k].XAxis.Z;\
		Temp = P.X * Camera->ProjZRX2;\
		if (Temp < -P.Z) ThisCode |= 1;\
		if (Temp >= P.Z) ThisCode |= 2;\
		\
		P.Y = BoxDot[i].YAxis.X + BoxDot[j].YAxis.Y + BoxDot[k].YAxis.Z;\
		Temp = P.Y * Camera->ProjZRY2;\
		if (Temp <  -P.Z) ThisCode |= 4;\
		if (Temp >=  P.Z) ThisCode |= 8;\
		\
		OutCode  &= ThisCode;\
		AllCodes |= ThisCode;
	CMD(0,0,0,1,Pts[0]); CMD(1,0,0,0,Pts[1]); CMD(0,1,0,0,Pts[2]); CMD(1,1,0,0,Pts[3]);
	CMD(0,0,1,0,Pts[4]); CMD(1,0,1,0,Pts[5]); CMD(0,1,1,0,Pts[6]); CMD(1,1,1,0,Pts[7]);
	#undef CMD

	if( OutCode )
	{
		STAT(GStat.BoxOutOfPyramid++;);
		return 0; // Invisible - pyramid reject
	}

	// Calculate projections of 8 points and take X,Y min/max bounded to Span X,Y window.
	Persp1 = Camera->ProjZ / BoxMinZ;
	Persp2 = Camera->ProjZ / BoxMaxZ;

	Pt = &Pts[0];
	BoxMinX = BoxMaxX = Camera->X2 + ftoi(Pt->X * Persp1);
	BoxMinY = BoxMaxY = Camera->Y2 + ftoi(Pt->Y * Persp1);

	if (AllCodes & 1) BoxMinX = 0;
	if (AllCodes & 2) BoxMaxX = Camera->X;
	if (AllCodes & 4) BoxMinY = 0;
	if (AllCodes & 8) BoxMaxY = Camera->Y;

	for( i=0; i<8; i++ )
	{
		// Check with MinZ.
		BoxX = Camera->X2 + ftoi(Pt->X * Persp1);
		BoxY = Camera->Y2 + ftoi(Pt->Y * Persp1);
		if (BoxX < BoxMinX) BoxMinX = BoxX; else if (BoxX > BoxMaxX) BoxMaxX = BoxX;
		if (BoxY < BoxMinY) BoxMinY = BoxY; else if (BoxY > BoxMaxY) BoxMaxY = BoxY;

		// Check with MaxZ.
		BoxX = Camera->X2 + ftoi(Pt->X * Persp2);
		BoxY = Camera->Y2 + ftoi(Pt->Y * Persp2);
		if (BoxX < BoxMinX) BoxMinX = BoxX; else if (BoxX > BoxMaxX) BoxMaxX = BoxX;
		if (BoxY < BoxMinY) BoxMinY = BoxY; else if (BoxY > BoxMaxY) BoxMaxY = BoxY;

		Pt++;
	}
	if( Results )
	{
		// Set bounding box size.
		Results->Valid = 1;
		Results->MinZ  = BoxMinZ;
		Results->MaxZ  = BoxMaxZ;
		Results->MinX  = BoxMinX;
		Results->MinY  = BoxMinY;
		Results->MaxX  = BoxMaxX;
		Results->MaxY  = BoxMaxY;
	}
	if( (!SpanBuffer) || SpanBuffer->BoxIsVisible(BoxMinX,BoxMinY,BoxMaxX,BoxMaxY) )
	{
		return 1; // Visible.
	}
	else
	{
		STAT(GStat.BoxSpanOccluded++);
		return 0; // Invisible.
	}
	unguard;
}

enum ENodePass
{
	PASS_Front=0,
	PASS_Plane=1,
	PASS_Back=2,
};

class FNodeStack
{
public:
	int			iNode;
	int			iFarNode;
	int			FarOutside;
	int			Outside;
	int			DrewStuff;
	ENodePass	Pass;
	FNodeStack	*Prev;
};

FBspDrawList *FRender::OccludeBsp( UCamera *Camera, FSpanBuffer *Backdrop )
{
	static UModel				*Model;
	static FSpanBuffer			ZoneSpanBuffer[UBspNodes::MAX_ZONES],*SpanBuffer;
	static FBspDrawList			*TempDrawList,*DrawList,**AllPolyDrawLists;
	static FRasterSetup			WorkingRaster;
	static FBspNode      		*Node;
	static UBspNodes::Ptr		Nodes;
	static UBspSurfs::Ptr		Surfs;
	static FBspSurf      		*Poly;
	static FNodeStack			*Stack;
	static FTransform 			Pts[FBspNode::MAX_FINAL_VERTICES];
	static FRasterSideSetup		**SideCache;
	static FVector				Origin;
	static DWORD				PolyFlags;
	static FLOAT				MaxZ,MinZ;
	static QWORD				ActiveZoneMask;
	static INDEX          		iNode,iSurf,iOriginalNode,iThingZone;
	static BYTE					iViewZone,iZone,iOppositeZone,ViewZoneMask;
	static int           		Visible,Merging,Mergeable,Outside,Pass,DrewStuff,NumPts;
	static int					CoplanarPass;

	guard(FRender::OccludeBsp);

	Model = Camera->Level->Model;
	if ( Model->Nodes->Num==0 )
		return 0;
	
	STAT(clock(GStat.OcclusionTime));

	DrawList			= NULL;
	TempDrawList		= new(GMem)FBspDrawList;

	AllPolyDrawLists    = new(GMem,MEM_Zeroed,Model->Surfs->Max)FBspDrawList*;
	SideCache	        = new(GMem,MEM_Zeroed,Model->Verts->NumSharedSides)FRasterSideSetup*;
	Nodes				= Model->Nodes;
	Surfs				= Model->Surfs;
	Origin				= Camera->Coords.Origin;
	iViewZone			= Model->PointZone(Origin);
	ViewZoneMask		= (iViewZone!=0) ? ~0 : 0;

	/* For debugging
	bug	("%i",
		Level->MaxBspSurfs   *sizeof(INDEX) +
		Level->NumSharedSides*sizeof(FRasterSideSetup *) +
		Level->MaxBspSurfs   *sizeof(BYTE));
	*/

	// Init first four units of the rasterization side setup cache so that they
	// represent the setups for the four view frustrum clipping planes.
	for ( int i=0; i<4; i++ )
		SideCache[i] = new(GDynMem)FRasterSideSetup;

	SideCache[0]->P.X = 0;				SideCache[0]->DP.X = 0; // X min clipping plane
	SideCache[1]->P.X = Fix(Camera->X);	SideCache[1]->DP.X = 0; // X max clipping plane
	SideCache[2]->P.X = 0;				SideCache[2]->DP.X = 0; // Y min clipping plane
	SideCache[3]->P.X = Fix(Camera->Y);	SideCache[3]->DP.X = 0; // Y max clipping plane

	// Init zone span buffers.
	for ( i=0; i<UBspNodes::MAX_ZONES; i++ )
		ZoneSpanBuffer[i].AllocIndex(0,0,&GDynMem);
	
	ZoneSpanBuffer[iViewZone].AllocIndexForScreen( Camera->X,Camera->Y,&GDynMem );
	ActiveZoneMask = ((QWORD)1) << iViewZone;

	// Init unrolled recursion stack.
	Stack				= new(GMem)FNodeStack;
	Stack->Prev			= NULL;
	iNode				= 0;
	Outside				= Model->RootOutside;
	DrewStuff			= 0;
	Pass				= PASS_Front;

	for( ;; )
	{
		Node = &Nodes(iNode);

		// Pass 1: Process node for the first time and optionally recurse with front node.
		if( Pass==PASS_Front )
		{
			// Zone mask rejection.
			if( iViewZone && !(Node->ZoneMask & ActiveZoneMask))
			{
				// Use pure zone rejection.
				STAT(GStat.MaskRejectZones++);
				goto PopStack;
			}

			// Bound rejection.
			if( (Node->NodeFlags & (NF_AllOccluded|NF_Bounded))==(NF_AllOccluded|NF_Bounded) )
			{
				// Use bounding box rejection if occluded on the previous frame.
				FScreenBounds Results;
				if( !BoundVisible(Camera,&Model->Bounds(Node->iBound),iViewZone?NULL:&ZoneSpanBuffer[0],&Results,NULL) )
				{
					goto PopStack;
				}
				if( Results.Valid && iViewZone )
				{
					QWORD		ZoneMask	= 1;
					FSpanBuffer *ZoneSpan	= &ZoneSpanBuffer[0];

					for( int iZone=0; iZone<64; iZone++)
					{
						if( ZoneSpan->ValidLines && (Node->ZoneMask & ZoneMask))
						{
							if( ZoneSpan->BoundIsVisible(Results) ) goto Visible;
						}
						ZoneMask += ZoneMask;
						ZoneSpan++;
					}
					STAT( GStat.BoxSpanOccluded++; );
					goto PopStack;
					Visible:;
				}
			}
			if( Node->iDynamic[0]!=INDEX_NONE )
			{
				dynamicsFilter( Camera,iNode,1,Outside );
			}
			if( Node->Plane.PlaneDot(Origin) > 0.0 )
			{
				Stack->iFarNode   = Node->iBack;
				Stack->FarOutside = Outside && !Node->IsCsg();

				if( Node->iFront != INDEX_NONE )
				{
					Stack->iNode		= iNode;
					Stack->Outside		= Outside;
					Stack->DrewStuff	= 0;
					Stack->Pass  		= PASS_Plane;
					
					FNodeStack *Prev	= Stack;
					Stack				= new(GMem)FNodeStack;
					Stack->Prev			= Prev;

					iNode				= Node->iFront;
					Outside				= Outside || Node->IsCsg();
					Pass				= PASS_Front;

					continue;
				}
			}
			else
			{
				Stack->iFarNode   = Node->iFront;
				Stack->FarOutside = Outside || Node->IsCsg();

				if( Node->iBack  != INDEX_NONE )
				{
					Stack->iNode		= iNode;
					Stack->Outside		= Outside;
					Stack->DrewStuff	= 0;
					Stack->Pass			= PASS_Plane;

					FNodeStack *Prev	= Stack;
					Stack				= new(GMem)FNodeStack;
					Stack->Prev			= Prev;

					iNode				= Node->iBack;
					Outside				= Outside && !Node->IsCsg();
					Pass				= PASS_Front;

					continue;
				}
			}
			Pass = PASS_Plane;
		}

		// Pass 2: Process polys within this node and optionally recurse with back.
		if (Pass==PASS_Plane)
		{
			// Zone mask rejection.
			if( iViewZone && !(Node->ZoneMask & ActiveZoneMask))
			{
				STAT(GStat.MaskRejectZones++);
				goto PopStack;
			}
			iOriginalNode	= iNode;
			FLOAT Dot		= Node->Plane.PlaneDot(Origin);
			INT IsFront		= Dot>0.0;
			iThingZone      = Node->iZone[IsFront] & ViewZoneMask;
			
			// Render dynamic stuff.
			if( (Node->iDynamic[1-IsFront]!=INDEX_NONE) && ZoneSpanBuffer[iThingZone].ValidLines )
			{
				dynamicsPreRender
				(
					Camera,
					&ZoneSpanBuffer[iThingZone],
					iNode,
					1-IsFront
				);
			}

			// View frustrum rejection.
			if( IsFront )
			{
				if (((Node->Plane | Camera->ViewSides[0])>0.0) &&
					((Node->Plane | Camera->ViewSides[1])>0.0) &&
					((Node->Plane | Camera->ViewSides[2])>0.0) &&
					((Node->Plane | Camera->ViewSides[3])>0.0))
					{
					goto PrePopStack;
					};
			}
			else
			{
				if (((Node->Plane | Camera->ViewSides[0])<0.0) &&
					((Node->Plane | Camera->ViewSides[1])<0.0) &&
					((Node->Plane | Camera->ViewSides[2])<0.0) &&
					((Node->Plane | Camera->ViewSides[3])<0.0))
					{
					goto PrePopStack;
					};
			}

			// Make two passes through this list of coplanars.  Draw regular (solid) polys on
			// first pass, semisolids and nonsolids on second pass.
			for( CoplanarPass=0; CoplanarPass<2; CoplanarPass++ )
			{
				// Process node and all of its coplanars.
				for( ;; )
				{
					// Note: Can't zone mask reject coplanars due to moving brush rules.
					iSurf		= Node->iSurf;
					Poly		= &Surfs(iSurf);
					PolyFlags	= Poly->PolyFlags | Camera->ExtraPolyFlags;

					// Skip this polygon if we're not processing it on this pass.
					if( CoplanarPass ^ !((PolyFlags^PF_Portal) & (PF_NotSolid|PF_Semisolid|PF_Portal)) ) 
						goto NextCoplanar;

					// Get zones.
					iZone         = Node->iZone[IsFront  ] & ViewZoneMask;
					iOppositeZone = Node->iZone[1-IsFront] & ViewZoneMask;

					// Get this zone's span buffer.
					SpanBuffer = &ZoneSpanBuffer[iZone];
					if( SpanBuffer->ValidLines <= 0 )
						goto NextCoplanar;

					// Skip if backfaced.
					if( !IsFront && Dot<-1.0 && !(Poly->PolyFlags & (PF_TwoSided|PF_Portal)) )
						goto NextCoplanar;

					STAT(GStat.NodesDone++);

					// Clip it.
					NumPts = ClipBspSurf( Model, Camera, iNode, Pts );
					if( !NumPts )
						goto NextCoplanar;

					// Box reject this poly if it was entirely occluded last frame.
					if( Node->NodeFlags & NF_PolyOccluded )
					{
						FScreenBounds Box;
						WorkingRaster.CalcBound( Pts, NumPts, Box );
						if( !SpanBuffer->BoundIsVisible(Box) )
						{
							STAT(GStat.NumRasterBoxReject++);
							goto NextCoplanar;
						}
					}

					// If this is a portal and we're not portal rendering, skip it.
					if( (Poly->PolyFlags & PF_Portal) && iViewZone==0 ) 
						goto NextCoplanar;

					// Rasterize it.
					WorkingRaster.SetupCached( Camera, Pts, NumPts, &GMem, &GDynMem, SideCache );
					WorkingRaster.Generate( GRaster.Raster );

					// Assimilate the texture's flags.
					if( Poly->Texture )
						PolyFlags |= Poly->Texture->PolyFlags;

					// Make sure this is facing forward
					if( (PolyFlags & (PF_TwoSided | PF_Portal)) && !IsFront )
						GRaster.Raster->ForceForwardFace();

					Mergeable = !
					(
						(PolyFlags & (PF_NoOcclude|PF_NoMerge) )
					||	(Node->NodeFlags & NF_NoMerge)
					||	GCameraManager->RenDev
					);
					Merging = 
					(
						(Mergeable && AllPolyDrawLists[iSurf])
					||	((PolyFlags&PF_Portal) && (Camera->Actor->RendMap!=REN_Zones))
					);
					if( !Merging )
					{
						if( Mergeable )
						{
							// Look up cached bounds to exploit frame-to-frame coherence.
							int GuessStartY = Clamp(Poly->LastStartY-4,0,GRaster.Raster->StartY);
							int GuessEndY	= Clamp(Poly->LastEndY  +4,GRaster.Raster->EndY,Camera->Y);
							TempDrawList->Span.AllocIndex(GuessStartY,GuessEndY,&GDynMem);
						}
						else if( GCameraManager->RenDev )
						{
							TempDrawList->Span.AllocIndex(GRaster.Raster->StartY,GRaster.Raster->EndY,&GMem);	
						}
						else TempDrawList->Span.AllocIndex(GRaster.Raster->StartY,GRaster.Raster->EndY,&GDynMem);
					}
					else
					{
						TempDrawList->Span.AllocIndex(GRaster.Raster->StartY,GRaster.Raster->EndY,&GMem);
					}

					if( !(PolyFlags&PF_NoOcclude) )	
						Visible = TempDrawList->Span.CopyFromRasterUpdate(*SpanBuffer,*GRaster.Raster);
					else							
						Visible = TempDrawList->Span.CopyFromRaster(*SpanBuffer,*GRaster.Raster);

					if( Visible && (PolyFlags & PF_Portal) )
					{
						if( iOppositeZone !=0 )
						{
							ActiveZoneMask |= ((QWORD)1)<<iOppositeZone;
							ZoneSpanBuffer[iOppositeZone].MergeWith(TempDrawList->Span);
						}
						if( Camera->Actor->RendMap==REN_Zones )
						{
							// Actually display zone portals.
							goto DrawNoMerge;
						}
					}
					else if( Visible && ((!(Camera->Actor->ShowFlags&SHOW_PlayerCtrl)) || !(PolyFlags & PF_Invisible)) )
					{
						// Compute Z range.
						MaxZ = MinZ = Pts[0].Z;
						for( i=1; i<NumPts; i++ )
						{
							if(			Pts[i].Z > MaxZ ) MaxZ = Pts[i].Z;
							else if(	Pts[i].Z < MinZ ) MinZ = Pts[i].Z;
						}
						if( ( PolyFlags & PF_FakeBackdrop ) && (Camera->Actor->ShowFlags&SHOW_Backdrop) )
						{
							if( Backdrop ) Backdrop->MergeWith(TempDrawList->Span);
						}
						else if( !Merging )
						{
							// Create new draw-list entry.
							DrawNoMerge:
							AllPolyDrawLists[Node->iSurf] = TempDrawList;
							TempDrawList->iNode		= iNode;
							TempDrawList->iZone		= Node->iZone[IsFront];
							TempDrawList->iSurf		= iSurf;
							TempDrawList->PolyFlags	= PolyFlags;
							TempDrawList->MaxZ		= MaxZ;
							TempDrawList->MinZ		= MinZ;
							TempDrawList->Texture	= Poly->Texture;
							TempDrawList->Next      = DrawList;
						
							if( GCameraManager->RenDev )
							{
								// Save stuff out for hardware rendering.
								TempDrawList->NumPts = NumPts;
								TempDrawList->Pts    = new(GDynMem,NumPts)FTransform;
								memcpy(TempDrawList->Pts,Pts,NumPts * sizeof(FTransform));
								TempDrawList->Span.Release();

								// Hardware rendering sort key.
								TempDrawList->Key = TempDrawList->iSurf;
								if( Poly->Texture )
								{
									TempDrawList->Key += (Poly->Texture->GetIndex() << 12);
									if( Poly->Texture->Palette )	
										TempDrawList->Key += (Poly->Texture->Palette->GetIndex() << 24);
								}
							}
							else
							{
								// Software rendering sort key.
								TempDrawList->Key = TempDrawList->iZone << (32-6);
								if( Poly->Texture )
								{
									TempDrawList->Key += Poly->Texture->GetIndex();
									if( Poly->Texture->Palette )	
										TempDrawList->Key += Poly->Texture->Palette->GetIndex() << 12;
								}
							}
							DrawList     = TempDrawList;
							TempDrawList = new(GDynMem)FBspDrawList;
							PolysDraw++;
						}
						else
						{
							// Add to existing draw-list entry:
							FBspDrawList *Existing = AllPolyDrawLists[iSurf];
							Existing->MaxZ = Max(MaxZ,Existing->MaxZ);
							Existing->MinZ = Min(MinZ,Existing->MinZ);
							Existing->Span.MergeWith(TempDrawList->Span);
							TempDrawList->Span.Release();
						}
						DrewStuff = 1;
						Node->NodeFlags &= ~NF_PolyOccluded;
						NodesDraw++;

						if( SpanBuffer->ValidLines <= 0 )
						{
							ActiveZoneMask &= ~(((QWORD)1)<<iZone);
							if (!ActiveZoneMask)
							{
								// Screen is now completely filled.
								Nodes(iOriginalNode).NodeFlags &= ~NF_AllOccluded;
								Stack = Stack->Prev;
								while( Stack != NULL )
								{
									Nodes(Stack->iNode).NodeFlags &= ~NF_AllOccluded;
									Stack = Stack->Prev;
								}
								goto DoneRendering;
							}
						}
					}
					else
					{
						// Rejected, span buffer wasn't affected.
						Node->NodeFlags |= NF_PolyOccluded;
						TempDrawList->Span.Release();
					}
					WorkingRaster.Release(); // Releases unlinked raster setups in GMem, not linked raster setups in GDynMem

					NextCoplanar:
					if( Node->iPlane==INDEX_NONE ) break;

					iNode	= Node->iPlane;
					Node	= &Nodes(iNode);
					Dot		= Node->Plane.PlaneDot(Origin);
					IsFront	= Dot>0.0;
				}
				iNode  = iOriginalNode;
				Node   = &Nodes(iNode);
				Dot		= Node->Plane.PlaneDot(Origin);
				IsFront	= Dot>0.0;
			}
			iThingZone = Node->iZone[1-IsFront] & ViewZoneMask;
			if( (Node->iDynamic[IsFront]!=INDEX_NONE)
				&& ZoneSpanBuffer[iThingZone].ValidLines )
			{
				dynamicsPreRender
				(
					Camera,
					&ZoneSpanBuffer[iThingZone],
					iNode,
					IsFront
				);
			}
			iNode = iOriginalNode;
			if( Stack->iFarNode != INDEX_NONE )
			{
				Stack->iNode		= iNode;
				Stack->Outside		= Outside;
				Stack->Pass			= PASS_Back;
				Stack->DrewStuff    = DrewStuff;

				iNode				= Stack->iFarNode;
				Outside				= Stack->FarOutside;
				Pass				= PASS_Front;
				DrewStuff			= 0;

				FNodeStack *Prev	= Stack;
				Stack				= new(GMem)FNodeStack;
				Stack->Prev			= Prev;

				continue;
			}
			Pass  = PASS_Back;
		}

		// Pass 3: Done processing all stuff at and below iNode in the tree, now update visibility information.
		// Assert: Pass==PASS_Back.

		PrePopStack:
		Node = &Nodes(iNode);
		if( !DrewStuff )	Node->NodeFlags |=  NF_AllOccluded;
		else				Node->NodeFlags &= ~NF_AllOccluded;

		// Return from recursion, noting that the node we're returning to is guaranteed visible if the
		// child we're processing now is visible.

		PopStack:
		Stack = Stack->Prev;
		if ( !Stack ) break;

		iNode		= Stack->iNode;
		Outside		= Stack->Outside;
		Pass		= Stack->Pass;
		DrewStuff  |= Stack->DrewStuff;
		}

	DoneRendering:

	// Build backdrop by merging all of the non-empty parts of the remaining span buffers
	// together if in the editor.  In the game, if there are cracks, hide them by leaving
	// them undrawn so the previous frame shows through.
	if( Backdrop && GEditor )
	{
		for (i=0; i<UBspNodes::MAX_ZONES; i++) if (ZoneSpanBuffer[i].ValidLines)
		{
			Backdrop->MergeWith(ZoneSpanBuffer[i]);
		}
	}

	// Update stats:
	STAT(GStat.NumZones=Model->Nodes->NumZones);
	STAT(GStat.CurZone =iViewZone);
	for (i=0; i<UBspNodes::MAX_ZONES; i++) if (ZoneSpanBuffer[i].EndY) STAT(GStat.VisibleZones++);

	STAT(unclock(GStat.OcclusionTime));
	return DrawList;

	unguard;
}

/*-----------------------------------------------------------------------------
	Lattice builders.
-----------------------------------------------------------------------------*/

//
// Lattice setup loop globals:
//
FLOAT	LSL_RZ,		LSL_RZGradSX,	LSL_RZGradSY;
FLOAT	LSL_URZ,	LSL_URZGradSX,	LSL_URZGradSY;
FLOAT	LSL_VRZ,	LSL_VRZGradSX,	LSL_VRZGradSY;
FLOAT	LSL_XRZ,	LSL_XRZInc;
FLOAT	LSL_YRZ,	LSL_YRZInc;
FLOAT	LSL_BaseU,	LSL_BaseV;
FLOAT	LSL_MipMult, LSL_RBaseNormal;
FVector	LSL_Normal,LSL_TextureU,LSL_TextureV,LSL_Base;

inline void LatticeSetupLoop( FTexLattice** LatticeBase,FTexLattice* TopLattice,int Start,int End )
{
	guard(LatticeSetupLoop);
	debugInput(End>Start);

	static const int ConstLight = ToLatticeLight(UNLIT_LIGHT_VALUE);
	FTexLattice *Original    = TopLattice;

#if ASM_LATTICE
	static FLOAT StepIn;
	__asm
	{
		///////////////////////////////////////////////
		// Assembly language lattice span setup loop //
		///////////////////////////////////////////////

		mov ecx,[Start]
		mov ebx,[LatticeBase]

		mov eax,[End]
		mov edi,[TopLattice]

		lea ebx,[ebx + ecx*4]
		sub eax,ecx

		mov ecx,0

		// Load & perform step-in:
		fild [Start]			; Start
		fld  [LSL_YRZ]			; YRZ Start
		fxch					; Start YRZ
		fstp [StepIn]			; YRZ

		fld  [LSL_VRZGradSX]	; VRZGradSX YRZ
		fmul [StepIn]			; VRZGradSX' YRZ

		fld  [LSL_URZGradSX]	; URZGradSX VRZGradSX' YRZ
		fmul [StepIn]			; URZGradSX' VRZGradSX' YRZ

		fld  [LSL_RZGradSX]		; RZGradSX URZGradSX' VRZGradSX' YRZ
		fmul [StepIn]			; RZGradSX' URZGradSX' VRZGradSX' YRZ

		fld  [LSL_XRZInc]		; XRZInc RZGradSX' URZGradSX' VRZGradSX' YRZ
		fmul [StepIn]			; XRZInc' RZGradSX' URZGradSX' VRZGradSX' YRZ

		fxch st(3)				; VRZGradSX' RZGradSX' URZGradSX' XRZInc' YRZ
		fadd [LSL_VRZ]			; VRZ+VRZGradSX' RZGradSX' URZGradSX' XRZInc' YRZ

		fxch st(2)				; URZGradSX' RZGradSX' VRZ+VRZGradSX' XRZInc' YRZ
		fadd [LSL_URZ]			; URZ+URZGradSX' RZGradSX' VRZ+VRZGradSX' XRZInc' YRZ

		fxch st(1)				; RZGradSX' URZ+URZGradSX' VRZ+VRZGradSX' XRZInc' YRZ
		fadd [LSL_RZ]			; RZ+RZGradSX' URZ+URZGradSX' VRZ+VRZGradSX' XRZInc' YRZ

		fxch st(3)				; XRZInc' URZ+URZGradSX' VRZ+VRZGradSX' RZ+RZGradSX' YRZ
		fadd [LSL_XRZ]			; X+XRZInc' URZ+URZGradSX' VRZ+VRZGradSX' RZ+RZGradSX' YRZ

		fxch st(3)				; RZ+RZGradSX' URZ+URZGradSX' VRZ+VRZGradSX' XRZ+XRZInc' YRZ

		fld1					; 1 RZ+RZGradSX' URZ+URZGradSX' VRZ+VRZGradSX' XRZ+XRZInc' YRZ
		fdiv st,st(1)			; 1/RZ+RZGradSX' RZ+RZGradSX' URZ+URZGradSX' VRZ+VRZGradSX' XRZ+XRZInc' YRZ

		// Loop:
		ALIGN 16
		MainLoop:

		; TopLattice->U = URZ * Z + BaseU;
		; TopLattice->V = VRZ * Z + BaseV;
		; URZ += URZGradSX;
		; VRZ += VRZGradSX;
		; RZ  += RZGradSX;
		
		;							;   st(0)   st(1)   st(2)   st(3)   st(4)   st(5)   st(6)   st(7)
		;							;   ------- ------- ------- ------- ------- ------- ------- -------
		fst [edi]TopLattice.Loc.Z	;   Z       RZ      URZ     VRZ		XRZ		YRZ
		fld st(0)					;   Z		Z       RZ      URZ     VRZ		XRZ		YRZ
		fmul [LSL_MipMult]			;   Z*Mip	Z       RZ      URZ     VRZ		XRZ		YRZ
		fld  st(3)					;-  URZ     Z*MipZ	Z       RZ      URZ     VRZ		XRZ		YRZ
		fmul st,st(2)				;-  URZ*Z   Z*MipZ	Z       RZ      URZ     VRZ		XRZ		YRZ
		fxch st(1)					;   Z*MipZ	URZ*Z   Z       RZ      URZ     VRZ		XRZ		YRZ
		fstp [edi]TopLattice.Loc.W	;   URZ*Z   Z       RZ      URZ     VRZ		XRZ		YRZ

		fld  st(4)					;-  VRZ     URZ*Z   Z       RZ      URZ     VRZ		XRZ		YRZ
		fmul st,st(2)				;-  VRZ*Z   URZ*Z   Z       RZ      URZ     VRZ		XRZ		YRZ
		fxch st(3)					;   RZ      URZ*Z   Z       VRZ*Z   URZ     VRZ		XRZ		YRZ
		fadd [LSL_RZGradSX]			;-  RZ'     URZ*Z   Z       VRZ*Z   URZ     VRZ		XRZ		YRZ
		fxch st(1)					;   URZ*Z   RZ      Z       VRZ*Z   URZ     VRZ		XRZ		YRZ
		fadd [LSL_BaseU]			;-  URZ*Z+B RZ      Z       VRZ*Z   URZ	    VRZ		XRZ		YRZ
		fxch st(3)					;   VRZ*Z   RZ      Z       URZ*Z+B URZ     VRZ		XRZ		YRZ
		fadd [LSL_BaseV]			;-  VRZ*Z+B RZ      Z       URZ*Z+B URZ     VRZ		XRZ		YRZ
		fxch st(5)					;   VRZ     RZ      Z       URZ*Z+B URZ     VRZ*Z+B	XRZ		YRZ
		fadd [LSL_VRZGradSX]		;-  VRZ'    RZ      Z       URZ*Z+B URZ     VRZ*Z+B	XRZ		YRZ
		fxch st(4)					;   URZ     RZ      Z       URZ*Z+B VRZ     VRZ*Z+B	XRZ		YRZ
		fadd [LSL_URZGradSX]		;-  URZ'    RZ      Z       URZ*Z+B VRZ     VRZ*Z+B	XRZ		YRZ
		fxch st(3)					;   URZ*Z+B RZ      Z       URZ     VRZ     VRZ*Z+B	XRZ		YRZ
		fstp [edi]FTexLattice.U		;-  RZ      Z       URZ     VRZ     VRZ*Z+B	XRZ		YRZ
		fxch st(4)					;   VRZ*Z+B Z       URZ     VRZ     RZ		XRZ		YRZ
		fstp [edi]FTexLattice.V		;-  Z       URZ     VRZ     RZ		XRZ		YRZ
		fxch st(3)					;   RZ      URZ     VRZ		Z		XRZ		YRZ
		fld  st(4)					;-  XRZ		RZ      URZ     VRZ		Z		XRZ		YRZ
		fadd [LSL_XRZInc]			;-  XRZ'	RZ      URZ     VRZ		Z		XRZ		YRZ

		fxch st(4)					;   Z		RZ      URZ     VRZ		XRZ'	XRZ		YRZ
		fmul st(5),st				;-	Z		RZ      URZ     VRZ		XRZ'	XRZ*Z	YRZ
		fld  st(6)					;-	YRZ		Z		RZ      URZ     VRZ		XRZ'	XRZ*Z	YRZ
		fxch st(1)					;	Z		YRZ		RZ      URZ     VRZ		XRZ'	XRZ*Z	YRZ
		fmul st,st(7)				;-	YRZ*Z	YRZ		RZ      URZ     VRZ		XRZ'	XRZ*Z	YRZ
		fxch st(6)					;	XRZ*Z	YRZ		RZ      URZ     VRZ		XRZ'	YRZ*Z	YRZ
		fstp [edi]TopLattice.Loc.X	;-	YRZ		RZ      URZ     VRZ		XRZ'	YRZ*Z	YRZ
		fxch st(5)					;	YRZ*Z	RZ      URZ     VRZ		XRZ'	YRZ		YRZ
		fstp [edi]TopLattice.Loc.Y	;-	RZ      URZ     VRZ		XRZ'	YRZ		YRZ

		fld1						;-  1       RZ      URZ     VRZ		XRZ		YRZ		YRZ
		ffree st(6)					; Discard
		fdiv st,st(1)				;-  Z'      RZ      URZ     VRZ		XRZ		YRZ

		// Fix up lattice values:
		mov edx,[edi]FTexLattice.U
		mov esi,[ConstLight]

		mov [edi]FTexLattice.G,esi
		mov esi,[edi]FTexLattice.V

		shl edx,8
		mov [edi]FTexLattice.RoutineOfs,ecx ; ecx=0

		shl esi,8
		mov [edi]FTexLattice.U,edx

		mov [edi]FTexLattice.V,esi
		mov [ebx],edi

		// End of loop:
		add ebx,4
		add edi,SIZE FTexLattice

		dec eax
		jg  MainLoop

		// Done:
		fcompp	; Pop 6 registers in 3 cycles
		fcompp
		fcompp
	}
#else
	FTexLattice **LatticePtr = &LatticeBase[Start];

	FLOAT StepIn	= (FLOAT)Start;
	FLOAT RZ		= LSL_RZ  + StepIn * LSL_RZGradSX;
	FLOAT URZ		= LSL_URZ + StepIn * LSL_URZGradSX;
	FLOAT VRZ		= LSL_VRZ + StepIn * LSL_VRZGradSX;
	FLOAT XRZ		= LSL_XRZ + StepIn * LSL_XRZInc;

	int n = End - Start;
	while( n-- > 0 )
	{
		FLOAT Z						= 1.0/RZ;
		TopLattice->Loc.Z			= Z;
		TopLattice->Loc.X			= Z * XRZ;
		TopLattice->Loc.Y			= Z * LSL_YRZ;
		TopLattice->Loc.W			= Z * LSL_MipMult;
		*(FLOAT *)&TopLattice->U	= Z * URZ + LSL_BaseU;
		*(FLOAT *)&TopLattice->V	= Z * VRZ + LSL_BaseV;
		TopLattice->RoutineOfs		= 0;

		TopLattice->U				= TopLattice->U << 8;
		TopLattice->V				= TopLattice->V << 8;

		TopLattice->G				= GLightManager->Mesh.PtrVOID ? ConstLight : Clamp(ftoi(0x2800 * 768.0 * RZ),0x100,0x3000);

		RZ                         += LSL_RZGradSX;
		URZ                        += LSL_URZGradSX;
		VRZ                        += LSL_VRZGradSX;
		XRZ                        += LSL_XRZInc;

		*LatticePtr++ = TopLattice++;
	}
#endif
	unguard;
}

/*-----------------------------------------------------------------------------
	Texture rectangle setup loop.
-----------------------------------------------------------------------------*/

void inline RectLoop( FTexLattice **LatticeBase,int Start, int End )
{
#if ASM_LATTICE
	__asm
	{
		pushad					; Save regs
		mov edi,[LatticeBase]	; Get lattice base pointer
		mov esi,[Start]			; Get start
		mov ebp,[End]			; Get end

		call TRL_RectLoop		; Setup

		popad					; Restore regs
	}
#else
	guard(RectLoop);

	int MeshBaseU = GLightManager->TextureUStart;
	int MeshBaseV = GLightManager->TextureVStart;

	FTexLattice **LatticePtr = &LatticeBase[Start];
	while( Start++ < End )
	{
		FTexLattice *T0	= LatticePtr[0];
		FTexLattice *T1	= LatticePtr[1];
		FTexLattice *B0	= LatticePtr[MAX_XR];
		FTexLattice *B1	= LatticePtr[MAX_XR+1];

		FMipTable *T	= &TRL_MipTable[*(DWORD *)&T0->Loc.W >> 21];

		T0->RoutineOfs	= T->RoutineOfs + TRL_RoutineOfsEffectBase;

		GBlit.MipRef[T->MipLevel  ] = 1;
		GBlit.MipRef[T->MipLevel+1] = 1;

		FMipInfo &Mip	= GBlit.Texture->Mips[T->MipLevel];
		INT  VMask		= Mip.VMask;
		INT  NotVMask	= ~VMask;

		BYTE GP			= GBlit.LatticeXBits;
		BYTE GL			= GBlit.LatticeYBits;
		BYTE GM			= T->MipLevel;
		BYTE GMU		= GM + Mip.UBits;

		// Lattice setup.
		int B_IU		= (T0->U + TRL_TexBaseU         ) >> GMU;
		int B_IUX		= (T1->U - T0->U                ) >> GMU;
		int B_IUY		= (B0->U - T0->U                ) >> GMU;
		int B_IUXY		= (T0->U - T1->U - B0->U + B1->U) >> GMU;

		int B_IV		= (T0->V + TRL_TexBaseV         ) >> GM;
		int B_IVX		= (T1->V - T0->V                ) >> GM;
		int B_IVY		= (B0->V - T0->V                ) >> GM;
		int B_IVXY		= (T0->V - T1->V - B0->V + B1->V) >> GM;

		T0->L			= (B_IV   << (16      )&0xffff0000);
		T0->LY			= (B_IVY  << (16-GL   )&0xffff0000);
		T0->LX			= (B_IVX  << (16-GP   )&0xffff0000);
		T0->LXY			= (B_IVXY << (16-GP-GL)&0xffff0000);
	
		T0->H			= ((B_IU   << (16      ))&NotVMask  )+(((B_IV   >> (16      )))&VMask);
		T0->HY			= ((B_IUY  << (16-GL   ))&NotVMask  )+(((B_IVY  >> (16+   GL)))&VMask);
		T0->HX			= ((B_IUX  << (16-GP   ))&NotVMask  )+(((B_IVX  >> (16+GP   )))&VMask);
		T0->HXY			= ((B_IUXY << (16-GP-GL))&NotVMask  )+(((B_IVXY >> (16+GP+GL)))&VMask);

		// Sublattice setup.
		if( GLightManager->iLightMesh != INDEX_NONE )
		{
			VMask		= (1 << GLightManager->MeshVBits) - 1;
			NotVMask	= ~VMask;

			BYTE GP		= GBlit.InterXBits;
			BYTE GL		= GBlit.InterYBits;
			BYTE GM		= GLightManager->MeshShift;
			BYTE GMU	= GM + GLightManager->MeshUBits;

			int B_IU	= (T0->U - MeshBaseU             ) >> GMU;
			int B_IUX	= (T1->U - T0->U                 ) >> GMU;
			int B_IUY	= (B0->U - T0->U                 ) >> GMU;
			int B_IUXY	= (T0->U - T1->U - B0->U + B1->U ) >> GMU;

			int B_IV	= (T0->V - MeshBaseV             ) >> GM;
			int B_IVX	= (T1->V - T0->V                 ) >> GM;
			int B_IVY	= (B0->V - T0->V                 ) >> GM;
			int B_IVXY	= (T0->V - T1->V - B0->V + B1->V ) >> GM;

			int B_IG	= (T0->G                         );
			int B_IGX	= (T1->G - T0->G                 );
			int B_IGY	= (B0->G - T0->G                 );
			int B_IGXY	= (T0->G - T1->G - B0->G + B1->G );

			T0->SubL	= (((B_IG                ))&0x0000ffff)+((B_IV   << (16      ))&0xffff0000);
			T0->SubLY	= (((B_IGY  >> (      GL)))&0x0000ffff)+((B_IVY  << (16-GL   ))&0xffff0000);
			T0->SubLX	= (((B_IGX  >> (   GP   )))&0x0000ffff)+((B_IVX  << (16-GP   ))&0xffff0000);
			T0->SubLXY	= (((B_IGXY >> (   GP+GL)))&0x0000ffff)+((B_IVXY << (16-GP-GL))&0xffff0000);

			T0->SubH	= (((B_IV   >> (16      )))&VMask     )+((B_IU   << (16      ))&NotVMask  );
			T0->SubHY	= (((B_IVY  >> (16+   GL)))&VMask     )+((B_IUY  << (16-GL   ))&NotVMask  );
			T0->SubHX	= (((B_IVX  >> (16+GP   )))&VMask     )+((B_IUX  << (16-GP   ))&NotVMask  );
			T0->SubHXY	= (((B_IVXY >> (16+GP+GL)))&VMask     )+((B_IUXY << (16-GP-GL))&NotVMask  );
		}
		else
		{
			int B_IG	= (T0->G                         );
			int B_IGX	= (T1->G - T0->G                 );
			int B_IGY	= (B0->G - T0->G                 );
			int B_IGXY	= (T0->G - T1->G - B0->G + B1->G );

			T0->SubL	= ((B_IG                ))&0x0000ffff;
			T0->SubLY	= ((B_IGY  >> (      GL)))&0x0000ffff;
			T0->SubLX	= ((B_IGX  >> (   GP   )))&0x0000ffff;
			T0->SubLXY	= ((B_IGXY >> (   GP+GL)))&0x0000ffff;

			T0->SubH	= 0;
			T0->SubHY	= 0;
			T0->SubHX	= 0;
			T0->SubHXY	= 0;
		}
		LatticePtr++;
	}
	unguard;
#endif
}

/*-----------------------------------------------------------------------------
	Software textured Bsp surface rendering.
-----------------------------------------------------------------------------*/

//
// Draw a software-textured Bsp surface.
//
void DrawSoftwareTexturedBspSurf( UCamera* Camera, FBspDrawList* Draw )
{
	guard(DrawSoftwareTexturedBspSurf);

	FCoords				&Coords		= Camera->Coords;
	ULevel				*Level		= Camera->Level;
	ALevelInfo			*LevelInfo	= Level->Info;

	FTexLattice	**LatticeBase;
	FSpanBuffer	SubRectSpan,SubLatticeSpan,RectSpan,LatticeSpan;
	FSpan		*Span,**SpanIndex;
	FLOAT		UPanRate,VPanRate;
	INT			iLightMesh,PolyFlags,YR;

	for ( int i=0; i<MAX_MIPS; i++ ) 
		GBlit.MipRef[i]=0;

	if ( (Draw->iSurf != INDEX_NONE) && !(Draw->PolyFlags & PF_Environment) )
	{
		FBspSurf *Surf	= &Level->Model->Surfs(Draw->iSurf);
		GBlit.Texture	= Surf->Texture ? Surf->Texture : GGfx.DefaultTexture;
		GBlit.Palette	= GBlit.Texture->Palette;
		GBlit.iZone		= Draw->iZone;
		GBlit.Zone		= Camera->Level->GetZoneActor(Draw->iZone);

		PolyFlags		= Surf->PolyFlags;
		iLightMesh		= (PolyFlags & PF_Unlit) ? INDEX_NONE : Surf->iLightMesh;

		// Compute largest lattice size required to capture the essence of this Bsp surface's
		// perspective correction and special effects.

		GBlit.LatticeXBits	= 7;
		GBlit.LatticeYBits	= 6;

		LSL_Normal			= GRender.GetVector(Level->Model,Coords,Surf->vNormal);
		LSL_Base     		= GRender.GetPoint (Camera,Surf->pBase);
		LSL_TextureU 		= GRender.GetVector(Level->Model,Coords,Surf->vTextureU);
		LSL_TextureV 		= GRender.GetVector(Level->Model,Coords,Surf->vTextureV);
		LSL_XRZInc			= Camera->RProjZ * (1 << GBlit.LatticeXBits);
		LSL_YRZInc			= Camera->RProjZ * (1 << GBlit.LatticeYBits);
		LSL_RBaseNormal		= 1.0/(LSL_Normal | LSL_Base);
		LSL_RZGradSX		= LSL_XRZInc * LSL_Normal.X * LSL_RBaseNormal;
		LSL_RZGradSY   		= LSL_YRZInc * LSL_Normal.Y * LSL_RBaseNormal;

		FLOAT RZGradSX		= Abs(LSL_RZGradSX);
		FLOAT RZGradSY		= Abs(LSL_RZGradSY);

		// Shrink lattice vertically as necessary to mainain the illusion of perspective correctness.
		static const FLOAT LineThresh[8] = { 0,1.80,1.50,1.00,0.60,0.36,0.20,0.08 };
		while ( GBlit.LatticeYBits>0 )
		{
			FLOAT Factor1   = Draw->MaxZ * RZGradSY;
			FLOAT Factor2   = Draw->MinZ * RZGradSY;
			GBlit.LatticeY	= 1<<GBlit.LatticeYBits;

			if( (GBlit.LatticeY*Factor1/(4.0+2.0/Factor1)<LineThresh[GBlit.LatticeYBits])
			&&	(GBlit.LatticeY*Factor2/(4.0+2.0/Factor2)<LineThresh[GBlit.LatticeYBits]) )
				break;
			RZGradSY *= 0.5;
			GBlit.LatticeYBits--;
		}

		// Shrink lattice horizontally as necessary to mainain the illusion of perspective correctness.
		static const FLOAT PixelThresh[8]={0,0,0,1.5,0.80,0.45,0.28,0.06}; // 0.50
		while ( GBlit.LatticeXBits>2 )
		{
			FLOAT Factor1      = Draw->MaxZ * RZGradSX;
			FLOAT Factor2      = Draw->MinZ * RZGradSX;
			GBlit.LatticeX	   = 1<<GBlit.LatticeXBits;

			if ( (GBlit.LatticeX*Factor1/(4.0+2.0/Factor1)<PixelThresh[GBlit.LatticeXBits])
			&&	(GBlit.LatticeX*Factor2/(4.0+2.0/Factor2)<PixelThresh[GBlit.LatticeXBits]) )
				break;
			RZGradSX *= 0.5;
			GBlit.LatticeXBits--;
		}
		TRL_TexBaseU	= Fix(Surf->PanU);
		TRL_TexBaseV	= Fix(Surf->PanV);

		UPanRate		= LevelInfo->TexUPanSpeed;
		VPanRate		= LevelInfo->TexVPanSpeed;

		LSL_MipMult		= 1.0;

		if ( GBlit.Zone && GBlit.Zone->bWaterZone )
			PolyFlags |= PF_WaterWavy;
	}
	else
	{
		GBlit.Zone		 = NULL;

		GBlit.LatticeXBits = 5;
		GBlit.LatticeYBits = 3;

		PolyFlags		= PF_AutoUPan | PF_AutoVPan | PF_CloudWavy;
		iLightMesh		= INDEX_NONE;

		LSL_TextureU 	= GMath.XAxisVector.TransformVectorBy(Coords)*65536;//!!inefficient
		LSL_TextureV 	= GMath.YAxisVector.TransformVectorBy(Coords)*65536;
		LSL_Normal   	= GMath.ZAxisVector.TransformVectorBy(Coords);
		LSL_Base     	= GMath.ZAxisVector.TransformVectorBy(Coords)*(90.0*LevelInfo->SkyScale);
		LSL_RBaseNormal	= 1.0/(LSL_Normal | LSL_Base);

		TRL_TexBaseU	= 0;
		TRL_TexBaseV	= 0;

		GBlit.Texture	= LevelInfo->SkyTexture ? LevelInfo->SkyTexture : GGfx.DefaultTexture;
		GBlit.Palette	= LevelInfo->SkyTexture ? LevelInfo->SkyTexture->Palette : GBlit.Texture->Palette;
		GBlit.iZone		= 0;

		UPanRate		= LevelInfo->SkyUPanSpeed;
		VPanRate		= LevelInfo->SkyVPanSpeed;

		LSL_MipMult		= 2.0;

		if( Draw->iSurf != INDEX_NONE )
		{
			// Handle environment mapping.
			//LSL_TextureU *= -1;
			//LSL_TextureV *= -1;
			LSL_Normal   *= -1;

			// Shrink the lattice.
			GBlit.LatticeXBits = 4;
			GBlit.LatticeYBits = 3;

			// Disable regular surface drawing.
			Draw->iSurf = INDEX_NONE;
		}
	}

	// Disable currently-unsupported PolyFlags.
	PolyFlags &= ~(PF_Transparent | PF_Masked);

	// Setup dynamic lighting.
	GLightManager->SetupForSurf(Camera,LSL_Normal,LSL_Base,Draw->iSurf,iLightMesh,PolyFlags,GBlit.Zone,GBlit.Texture);

	// Clamp lattice size to the min/max specified by lighting code:
	GBlit.LatticeXBits = Clamp((int)GBlit.LatticeXBits,GLightManager->MinXBits,GLightManager->MaxXBits);
	GBlit.LatticeYBits = Clamp((int)GBlit.LatticeYBits,GLightManager->MinYBits,GLightManager->MaxYBits);

	// Keep precision under control.
	while ( GBlit.LatticeXBits + GBlit.LatticeYBits > 12 )
	{
		if ( GBlit.LatticeXBits >= GBlit.LatticeYBits )	GBlit.LatticeXBits--;
		else											GBlit.LatticeYBits--;
	}
	if ( Camera->X<420 && GBlit.LatticeYBits>0 ) GBlit.LatticeYBits--;

	// Prevent from being larger than storage will allow.
	while ( (Camera->X >> GBlit.LatticeXBits) > (MAX_XR-2) )	GBlit.LatticeXBits++;
	while ( (Camera->Y >> GBlit.LatticeYBits) > (MAX_YR-2) )	GBlit.LatticeYBits++;

	// Prevent from being larger than precision will reasonably allow.
	if ( GBlit.LatticeXBits>7 ) GBlit.LatticeXBits=7;
	if ( GBlit.LatticeYBits>7 ) GBlit.LatticeYBits=7;

	// For testing: Hardcode lattice size.
	//GBlit.LatticeXBits	= 4;
	//GBlit.LatticeYBits	= 4;

	GBlit.LatticeX			= 1 << GBlit.LatticeXBits;
	GBlit.LatticeY			= 1 << GBlit.LatticeYBits;
	GBlit.LatticeXMask		= (GBlit.LatticeX-1);
	GBlit.LatticeXMask4		= (GBlit.LatticeX-1) & ~3;
	GBlit.LatticeXNotMask	= ~(GBlit.LatticeX-1);

	// Compute sublattice size needed for mesh lighting.
	GBlit.SubXBits = Min(GBlit.LatticeXBits,(BYTE)2);
	GBlit.SubYBits = Clamp(GBlit.LatticeYBits-2,0,(Camera->X>400) ? 2 : 1);

	GBlit.SubX = 1 << GBlit.SubXBits;
	GBlit.SubY = 1 << GBlit.SubYBits;

	// Compute interlattice.
	GBlit.InterXBits		= GBlit.LatticeXBits - GBlit.SubXBits;
	GBlit.InterYBits		= GBlit.LatticeYBits - GBlit.SubYBits;
	GBlit.InterX			= 1 << GBlit.InterXBits;
	GBlit.InterY			= 1 << GBlit.InterYBits;
	GBlit.InterXMask		= (GBlit.InterX-1);
	GBlit.InterXNotMask		= ~(GBlit.InterX-1);

	// Set up texture mapping.
	rendDrawAcrossSetup(Camera,GBlit.Texture,GBlit.Palette,PolyFlags,0);

	// Compute lattice hierarchy.
	if (!SubRectSpan.CalcRectFrom	(Draw->Span,GBlit.SubXBits,GBlit.SubYBits,&GMem)) return;
	SubLatticeSpan.CalcLatticeFrom	(SubRectSpan,&GMem);

	if (!RectSpan.CalcRectFrom		(SubRectSpan,GBlit.InterXBits,GBlit.InterYBits,&GMem)) return;
	LatticeSpan.CalcLatticeFrom		(RectSpan,&GMem);

	// Setup magic numbers.
	LSL_XRZ    		= (1.0 - Camera->FX2) * Camera->RProjZ; // 1.0 is subpixel adjustment based on rasterizer convention
	LSL_YRZ 		= (LatticeSpan.StartY * GBlit.LatticeY + 1.0 - Camera->FY2) * Camera->RProjZ;

	LSL_XRZInc		= Camera->RProjZ * GBlit.LatticeX;
	LSL_YRZInc		= Camera->RProjZ * GBlit.LatticeY;

	LSL_MipMult *= GRender.MipMultiplier * (2.8/Camera->X) * (0.5 / 65536.0) * sqrt
		(
		(Square(0.75) + Square(Camera->RProjZ * Camera->FX2 * LSL_RBaseNormal * 75.0)) *
		(LSL_TextureU.SizeSquared() + LSL_TextureV.SizeSquared())
		) * Camera->Actor->FovAngle / 90.0;
	//LSL_MipMult = 1.0/Camera->X; // To disable angular mip adjustment.

	if ( PolyFlags & PF_AutoUPan ) TRL_TexBaseU += LevelInfo->TimeSeconds*35.0 * 65536.0 * UPanRate;
	if ( PolyFlags & PF_AutoVPan ) TRL_TexBaseV += LevelInfo->TimeSeconds*35.0 * 65536.0 * VPanRate;

	LSL_RZGradSX   	= LSL_XRZInc * LSL_Normal.X * LSL_RBaseNormal;	
	LSL_RZGradSY	= LSL_YRZInc * LSL_Normal.Y * LSL_RBaseNormal;
	LSL_URZGradSX  	= LSL_XRZInc * LSL_TextureU.X;	
	LSL_URZGradSY	= LSL_YRZInc * LSL_TextureU.Y;
	LSL_VRZGradSX  	= LSL_XRZInc * LSL_TextureV.X;	
	LSL_VRZGradSY	= LSL_YRZInc * LSL_TextureV.Y;

	LSL_RZ     		= (LSL_Normal  .X * LSL_XRZ + LSL_Normal  .Y * LSL_YRZ + LSL_Normal  .Z) * LSL_RBaseNormal;
	LSL_URZ    		= (LSL_TextureU.X * LSL_XRZ + LSL_TextureU.Y * LSL_YRZ + LSL_TextureU.Z);
	LSL_VRZ    		= (LSL_TextureV.X * LSL_XRZ + LSL_TextureV.Y * LSL_YRZ + LSL_TextureV.Z);

	LSL_BaseU		= (FLOAT)0xC0000000 - (LSL_TextureU | LSL_Base);
	LSL_BaseV		= (FLOAT)0xC0000000 - (LSL_TextureV | LSL_Base);

#if ASM_LATTICE
	TRL_RoutineOfsEffectBase = 0;
	if ( GLightManager->Mesh.PtrVOID )
	{
		TRL_LightVMask      = (DWORD)0xffffffff >> (32-GLightManager->MeshVBits);
		TRL_LightBaseU		= -GLightManager->TextureUStart;
		TRL_LightBaseV		= -GLightManager->TextureVStart;
		TRL_LightMeshShift	= GLightManager->MeshShift;
		TRL_LightMeshUShift	= GLightManager->MeshShift + GLightManager->MeshUBits;
	}
	TRL_MipRef = &GBlit.MipRef[0];
	__asm
	{
		mov al,[GBlit]GBlit.LatticeXBits
		mov bl,[GBlit]GBlit.LatticeYBits
		mov cl,[GBlit]GBlit.InterXBits
		mov dl,[GBlit]GBlit.InterYBits
		call TRL_SelfModRect
	}
#endif

	// Interpolate affine values RZ, URZ, VRZ, XRZ, YRZ.
	STAT(clockSlow(GStat.CalcLattice));
	if		(GBlit.Texture->Mips[1].Offset==MAXDWORD)	TRL_MipTable = GNoMipTable;
	else if (GBlit.Texture->VBits<=8)					TRL_MipTable = (GRender.GetTemporalIter()&1) ? GSmallMipTable:GOtherMipTable;
	else												TRL_MipTable = GLargeMipTable;

	SpanIndex = &LatticeSpan.Index [0];
	LatticeBase	= &GRender.LatticePtr[LatticeSpan.StartY+1][1];
	for ( YR=LatticeSpan.StartY; YR<LatticeSpan.EndY; YR++ )
	{
		Span = *SpanIndex++;
		while ( Span )
		{
			FTexLattice *TopLattice = new(GMem,Span->End - Span->Start)FTexLattice;
			LatticeSetupLoop( LatticeBase, TopLattice, Span->Start, Span->End );
			if( GLightManager->LatticeEffects )
			{
				GLightManager->ApplyLatticeEffects
					(
					&TopLattice[0],
					&TopLattice[Span->End - Span->Start] 
					);
			}

			STAT(GStat.LatsMade += Span->End - Span->Start);
			Span        = Span->Next;
		}
		LSL_RZ 		+= LSL_RZGradSY;
		LSL_URZ		+= LSL_URZGradSY;
		LSL_VRZ		+= LSL_VRZGradSY;
		LSL_YRZ		+= LSL_YRZInc;
		LatticeBase	+= MAX_XR;
	}
	SpanIndex = &RectSpan.Index[0];
	LatticeBase	= &GRender.LatticePtr[RectSpan.StartY+1][1];
	for ( YR=RectSpan.StartY; YR<RectSpan.EndY; YR++ )
	{
		Span = *SpanIndex++;
		while ( Span )
		{
			RectLoop(LatticeBase,Span->Start,Span->End);
			Span = Span->Next;
		}
		LatticeBase += MAX_XR;
	}
	STAT(unclockSlow( GStat.CalcLattice ));

	// Draw it.
	rendDrawAcross
	(
		Camera,
		&Draw->Span,&RectSpan,&LatticeSpan,&SubRectSpan,&SubLatticeSpan,
		GLightManager->Mesh.PtrVOID != NULL
	);

	// Finish up.
	if ( GRender.ShowLattice )
		GRender.DrawLatticeGrid( Camera,&LatticeSpan );

	// Clean up.
	GRender.CleanupLattice( LatticeSpan );
	GLightManager->ReleaseLightBlock();
	rendDrawAcrossExit();

	unguardf(("(%s %ix%i)",GBlit.Texture->GetName(),GBlit.Texture->USize,GBlit.Texture->VSize));
}

/*-----------------------------------------------------------------------------
	Drawers.
-----------------------------------------------------------------------------*/

//
// Draw the lattice grid, for lattice debugging.
//
void FRender::DrawLatticeGrid( UCamera *Camera,FSpanBuffer *LatticeSpan )
{
	guard(FRender::DrawLatticeGrid);

	int Y         = LatticeSpan->StartY * GBlit.LatticeY;
	BYTE *Dest1   = &Camera->Screen[Y * Camera->Stride];
	FSpan **Index = &LatticeSpan->Index[0];
	for( int i=LatticeSpan->StartY; i<LatticeSpan->EndY; i++ )
	{
		if (Y>=Camera->Y) break;
		FSpan *Span = *Index++;
		while( Span )
		{
			BYTE *Dest = Dest1 + Span->Start * GBlit.LatticeX;
			for( int j=Span->Start; j<Span->End; j++ )
			{
				*Dest  = BrushWireColor;
				Dest  += GBlit.LatticeX;
			}
			Span = Span->Next;
		}
		Y     += GBlit.LatticeY;
		Dest1 += Camera->Stride * GBlit.LatticeY;
	}
	unguard;
}

//
// Draw a flatshaded poly.
//
void FRender::DrawFlatPoly( UCamera *Camera,FSpanBuffer *SpanBuffer, BYTE Color )
{
	guard(FRender::DrawFlatPoly);

	FSpan			*Span,**Index;
	int				m,n;

	m     = SpanBuffer->EndY - SpanBuffer->StartY;
	Index = &SpanBuffer->Index [0];

	if( Camera->ColorBytes==1 )
	{
		BYTE *Line = &Camera->Screen [SpanBuffer->StartY * Camera->Stride];
		while (m-- > 0)
			{
			Span = *Index++;
			while (Span)
				{
				memset(&Line[Span->Start],Color,Span->End - Span->Start);
				Span   = Span->Next;
				};
			Line += Camera->Stride;
			};
		}
	else if (Camera->ColorBytes==2)
		{
		WORD *Screen,*Line,HiColor;
		//
		GGfx.DefaultPalette->Lock(LOCK_Read);
		if (Camera->Caps & CC_RGB565)	HiColor = GGfx.DefaultPalette(Color).HiColor565();
		else							HiColor = GGfx.DefaultPalette(Color).HiColor555();
		GGfx.DefaultPalette->Unlock(LOCK_Read);
		//
		Line = &((WORD *)Camera->Screen)[SpanBuffer->StartY * Camera->Stride];
		while (m-- > 0)
			{
			Span = *Index++;
			while (Span)
				{
				Screen = &Line[Span->Start];
				n      = Span->End - Span->Start;
				while (n-- > 0) *Screen++ = HiColor;
				Span = Span->Next;
				};
			Line += Camera->Stride;
			};
		}
	else
		{
		GGfx.DefaultPalette->Lock(LOCK_Read);
		DWORD TrueColor = GGfx.DefaultPalette(Color).TrueColor();
		GGfx.DefaultPalette->Unlock(LOCK_Read);
		//
		DWORD *Screen,*Line;
		//
		Line = &((DWORD *)Camera->Screen)[SpanBuffer->StartY * Camera->Stride];
		while (m-- > 0)
			{
			Span = *Index++;
			while (Span)
				{
				Screen = &Line[Span->Start];
				n      = Span->End - Span->Start;
				while (n-- > 0) *Screen++ = TrueColor;
				Span = Span->Next;
				};
			Line += Camera->Stride;
			};
		};
	unguard;
	};

//
// Draw a highlighted overlay polygon.
//
void FRender::DrawHighlight( UCamera *Camera,FSpanBuffer *SpanBuffer,BYTE Color )
{
	guard(FRender::DrawHighlight);
	FSpan	*Span,**Index;
	BYTE	*Screen,*Line;
	int		X,Y,XOfs;

	Y     = (SpanBuffer->StartY+1)&~1;
	Index = &SpanBuffer->Index [0];

	if ( Camera->ColorBytes==1 )
	{
		Line  = &Camera->Screen [Y * Camera->Stride];
		while( Y < SpanBuffer->EndY )
		{
			Span = *Index;
			while ( Span )
			{
				XOfs   = (Y & 2) * 2;
				X      = (((int)Span->Start + XOfs + 7) & ~7) - XOfs;
				Screen = &Line[X];
				while ( X < Span->End )
				{
					*Screen = Color;
					Screen += 8;
					X      += 8;
				}
				Span = Span->Next;
			}
			Line  += Camera->Stride * 2;
			Y     +=2;
			Index +=2;
		}
	}
	else if( Camera->ColorBytes==2 )
	{
		WORD HiColor;
		GGfx.DefaultPalette->Lock(LOCK_Read);
		if( Camera->Caps & CC_RGB565 ) HiColor = GGfx.DefaultPalette(SelectColor).HiColor565();
		else HiColor = GGfx.DefaultPalette(SelectColor).HiColor555();
		GGfx.DefaultPalette->Unlock(LOCK_Read);
		//
		Line = &Camera->Screen [Y * Camera->Stride * 2];
		while ( Y < SpanBuffer->EndY )
		{
			Span = *Index;
			while ( Span )
			{
				XOfs   = (Y & 2) * 2;
				X      = (((int)Span->Start + XOfs + 7) & ~7) - XOfs;
				Screen = &Line[X*2];
				while ( X < Span->End )
				{
					*(WORD *)Screen = HiColor;
					Screen += 16;
					X      += 8;
				}
				Span = Span->Next;
			}
			Line  += Camera->Stride * 4;
			Y     += 2;
			Index += 2;
		}
	}
	else
	{
		GGfx.DefaultPalette->Lock(LOCK_Read);
		DWORD TrueColor	= GGfx.DefaultPalette(SelectColor).TrueColor();
		GGfx.DefaultPalette->Unlock(LOCK_Read);
		//
		Line = &Camera->Screen [Y * Camera->Stride * 4];
		while ( Y < SpanBuffer->EndY )
		{
			Span = *Index;
			while ( Span )
			{
				XOfs   = (Y & 2) * 2;
				X      = (((int)Span->Start + XOfs + 7) & ~7) - XOfs;
				Screen = &Line[X*4];
				while ( X < Span->End )
				{
					*(DWORD *)Screen = TrueColor;
					Screen += 32;
					X      += 8;
				}
				Span = Span->Next;
			}
			Line  += Camera->Stride * 8;
			Y     += 2;
			Index += 2;
		}
	}
	unguard;
}

//
// Draw a raster side.
//
void FRender::DrawRasterSide (BYTE *Line,int SXStride,FRasterSideSetup *Side,BYTE Color)
	{
	BYTE *Temp;
	INT  FixX,FixDX,YL,L;
	//
	while (Side)
		{
		FixX  = Side->P.X;
		FixDX = Side->DP.X;
		YL    = Side->DY;
		//
		if ((FixDX>=-65535) && (FixDX<=65535)) // Vertical major
			{
			while (YL-- > 0)
				{
				Line[Unfix(FixX)]  = Color;
				Line              += SXStride;
				FixX              += FixDX;
				}
			}
		else if (FixDX>0) // Horizontal major, positive slope
			{
			while (YL-- > 0)
				{
				L    = Unfix(FixX+FixDX) - Unfix(FixX);
				Temp = &Line[Unfix(FixX)];
				while (L-- > 0) *Temp++ = Color;
				FixX += FixDX;
				Line += SXStride;
				};
			}
		else // Horizontal major, negative slope
			{
			while (YL-- > 0)
				{
				L    = Unfix(FixX) - Unfix(FixX+FixDX);
				Temp = &Line[Unfix(FixX+FixDX+0xffff)];
				while (L-- > 0) *Temp++ = Color;
				FixX += FixDX;
				Line += SXStride;
				};
			};
		Side = Side->Next;
		};
	};

//
// Draw a raster outline.
//
void FRender::DrawRasterOutline( UCamera *Camera,FRasterSetup *Raster, BYTE Color )
{
	guard(FRender::DrawRasterOutline);

	BYTE *StartScreenLine = &Camera->Screen[Raster->StartY * Camera->Stride];

	DrawRasterSide(StartScreenLine,Camera->Stride,Raster->LeftSide, Color);
	DrawRasterSide(StartScreenLine,Camera->Stride,Raster->RightSide,Color);

	unguard;
}

//
// Clean up LatticePtr by zeroing out all pointers
// specified by LatticeSpan.  This is needed because we assume that
// all NULL LatticeSpan pointers don't fall into the currently generated
// lattice.
//
void FRender::CleanupLattice( FSpanBuffer &LatticeSpan )
{
	guard(FRender::CleanupLattice);

	FSpan		**Index			= &LatticeSpan.Index [0];
	FTexLattice **LatticePtr1	= &LatticePtr  [LatticeSpan.StartY+1][1];

	for( int i=LatticeSpan.StartY; i<LatticeSpan.EndY; i++ )
	{
		FSpan *Span = *Index++;
		while( Span )
		{
			FTexLattice **Lattice = &LatticePtr1[Span->Start];
			for (int j=Span->Start; j<Span->End; j++) *Lattice++ = NULL;
			Span = Span->Next;
		}
		LatticePtr1 += MAX_XR;
	}
#if 0
	for( i=0; i<MAX_YR; i++ )
		for( int j=0; j<MAX_XR; j++ )
			if( LatticePtr[i][j] )
				appErrorf( "Failed %i,%i",i,j );
#endif
	unguard;
}

/*-----------------------------------------------------------------------------
	General Bsp surface rendering.
-----------------------------------------------------------------------------*/

// Draw a Bsp surface.
void FRender::DrawBspSurf( UCamera *Camera, FBspDrawList *Draw )
{
	guard(FRender::DrawBspSurf);
	FMemMark Mark(GMem);

#if 0
	//!! FireEngine test!
	static int Inited=0;
	static FireEngineParams Parms;
	static FLOAT F=0.0;
	if( Inited )
	{
		guard(FireUpdate);
		/*
		TempSpark
		(	
			64+32.0*cos(F),
			64+32.0*sin(F),
            127,
            &Parms
		);
		*/
		F += 0.1;
		CausticsUpdate( &Parms );
		unguard;
	}
	else
	{
		guard(EngineTileInit);
		Inited=1;
		memset(&GGfx.DefaultTexture->Element(GGfx.DefaultTexture->Mips[0].Offset),0,GGfx.DefaultTexture->USize*GGfx.DefaultTexture->VSize);
		CausticsEngineInit
		(
			GGfx.DefaultTexture->USize,
			GGfx.DefaultTexture->VSize,
			&GGfx.DefaultTexture->Element(GGfx.DefaultTexture->Mips[0].Offset),
			&Parms
		);
		for( int i=0; i<20; i++ )
		{
			/*
			Parms.DrawSparkType = 5;
			FirePaint
			(
				frand()*127.0,
				frand()*127.0,
                1,
                0,
                &Parms
			);
			*/
		}
		unguard;
	}
#endif

	UModel 		*Model 		= Camera->Level->Model;
	FBspNode	*Node 		= &Model->Nodes (Draw->iNode);
	FBspSurf	*Poly 		= &Model->Surfs (Node->iSurf);
	DWORD		PolyFlags	= Draw->PolyFlags;
	FSpanBuffer	TempLinearSpan;

	UTexture *Texture = Model->Surfs(Draw->iSurf).Texture;
	if( !Texture ) Texture=GGfx.DefaultTexture;

	if( GEditor  && GEditor->Scan.Active ) GEditor->Scan.PreScan ();
	if( PolyFlags & PF_Selected ) TempLinearSpan.CopyIndexFrom(Draw->Span,&GMem);

	if ((Camera->Actor->RendMap!=REN_Polys)&&(Camera->Actor->RendMap!=REN_PolyCuts)&&(Camera->Actor->RendMap!=REN_Zones))
	{
		DrawSoftwareTexturedBspSurf(Camera,Draw);
	}
	else
	{
		BYTE Color = Texture->MipZero.RemapIndex;
		int Index;
		if( (Camera->Actor->RendMap==REN_Zones)&&(Model->Nodes->NumZones>0) )
		{
			if (Node->iZone[1]==0) Color = 0x67 + ((Draw->iNode&3)<<3);
			else Color = 0x28 + (Node->iZone[1]&0x07) + ((Draw->iNode&3)<<3) + ((Node->iZone[1]&0x28)<<2);

			if (PolyFlags & PF_Portal) DrawHighlight (Camera,&Draw->Span,BrushSnapColor);
			else DrawFlatPoly (Camera,&Draw->Span,Color);
		}
		else
		{
			if (Camera->Actor->RendMap==REN_Polys)	Index=Draw->iSurf;
			else Index=Draw->iNode;

			if (Color>0x80) Color -= (Index%6) << 3;
			else			Color += (Index%6) << 3;

			DrawFlatPoly (Camera,&Draw->Span,Color);
		}
	}
	if( GEditor && (PolyFlags & PF_Selected) )
	{
		DrawHighlight( Camera, &TempLinearSpan, SelectColor );
	}
	if( GEditor && GEditor->Scan.Active )
	{
		GEditor->Scan.PostScan( EDSCAN_BspNodePoly, Draw->iNode, 0, 0, NULL );
	}
	Mark.Pop();
	unguard;
}

/*-----------------------------------------------------------------------------
	Edge postprocess antialiasing.
-----------------------------------------------------------------------------*/

// Antialias an edge whose two adjoining surfaces have already been drawn.
#define IFTEST(cmd) ;
void FRender::AntialiasEdge( UCamera *Camera, FLOAT X1, FLOAT Y1, FLOAT X2, FLOAT Y2 )
{
	guard(FRender::Antialias);

	FLOAT FDX = (X2 - X1) / (Y2 - Y1);
	if( Abs(FDX)<65536.0 )
	{
		// Vertical major.
		if( Y1 > Y2 ) 
		{
			Exchange( X1, X2 );
			Exchange( Y1, Y2 );
		}

		INT   Y   = (INT)Y1;
		FLOAT Adj = (FLOAT)(Y+1) - Y1;
		INT   X   = ftoi(X1 + Adj * FDX);
		INT   DY  = (INT)Y2 - Y;
		INT   DX  = (INT)FDX;

		RAINBOW_PTR Line = &Camera->Screen[Y * Camera->ByteStride];
		if( Camera->ColorBytes == 4 ) while( DY-- > 0 )
		{
			DWORD *A = &Line.PtrDWORD[Unfix(X)-1];
			if( X>=65536 ) switch( X & 0xc000 )
			{
				case 0x0000:
					IFTEST(A[1] = 0xff3f3f;) break;
				case 0x4000: A[1]
					=	((A[0]&0xfcfcfc)>>2)
					+	((A[1]&0xfcfcfc)>>2) + ((A[1]&0xfefefe)>>1);
					IFTEST(A[1] = 0xff7f7f;) break;
				case 0x8000: A[1]
					=	((A[0]&0xfefefe)>>1)
					+	((A[1]&0xfefefe)>>1); 
					IFTEST(A[1] = 0xffcfcf;) break;
				case 0xc000: A[1]
					=	((A[0]&0xfcfcfc)>>2) + ((A[0]&0xfefefe)>>1)
					+	((A[1]&0xfcfcfc)>>2);
					IFTEST(A[1] = 0xffffff;) break;
			}
			Line.PtrDWORD += Camera->Stride; X += DX;
		}
		else if( Camera->ColorBytes == 2 && (Camera->Caps & CC_RGB565) ) while( DY-- > 0 )
		{
			WORD *A = &Line.PtrWORD[Unfix(X)-1];
			if( X>=65536 ) switch( X & 0xc000 )
			{
				case 0x0000:
					break;
				case 0x4000: A[1]
					=	((A[0] & (0xe000 + 0x0780 + 0x001C))>>2) + ((A[1] & (0x0800 + 0x0020 + 0x0001))   )
					+	((A[1] & (0xe000 + 0x0780 + 0x001C))>>2) + ((A[1] & (0xf000 + 0x07C0 + 0x001e))>>1);
					break;
				case 0x8000: A[1]
					=	((A[0] & (0xf000 + 0x07C0 + 0x001e))>>1) + ((A[1] & (0x0800 + 0x0020 + 0x0001))   )
					+	((A[1] & (0xf000 + 0x07C0 + 0x001e))>>1); 
					break;
				case 0xc000: A[1]
					=	((A[0] & (0xe000 + 0x0780 + 0x001C))>>2) + ((A[0] & (0x0800 + 0x0020 + 0x0001))   )
					+	((A[1] & (0xe000 + 0x0780 + 0x001C))>>2) + ((A[0] & (0xf000 + 0x07C0 + 0x001e))>>1);
					break;
			}
			Line.PtrWORD += Camera->Stride; X += DX;
		}
		else if( Camera->ColorBytes == 2 ) while( DY-- > 0 )
		{
			WORD *A = &Line.PtrWORD[Unfix(X)-1];
			if( X>=65536 ) switch( X & 0xc000 )
			{
				case 0x0000:
					break;
				case 0x4000: A[1]
					=	((A[0] & (0x7000 + 0x0380 + 0x001C))>>2) + ((A[1] & (0x0400 + 0x0020 + 0x0001))   )
					+	((A[1] & (0x7000 + 0x0380 + 0x001C))>>2) + ((A[1] & (0x7800 + 0x03C0 + 0x001e))>>1);
					break;
				case 0x8000: A[1]
					=	((A[0] & (0x7800 + 0x03C0 + 0x001e))>>1) + ((A[1] & (0x0400 + 0x0020 + 0x0001))   )
					+	((A[1] & (0x7800 + 0x03C0 + 0x001e))>>1); 
					break;
				case 0xc000: A[1]
					=	((A[0] & (0x7000 + 0x0380 + 0x001C))>>2) + ((A[0] & (0x0400 + 0x0020 + 0x0001))   )
					+	((A[1] & (0x7000 + 0x0380 + 0x001C))>>2) + ((A[0] & (0x7800 + 0x03C0 + 0x001e))>>1);
					break;
			}
			Line.PtrWORD += Camera->Stride; X += DX;
		}
		else while( DY-- > 0 )
		{
			BYTE *A = &Line.PtrBYTE[Unfix(X)-1];
			if( X>=65536 ) switch( X & 0xc000 )
			{
				case 0x0000: break;
				case 0x4000: A[1] = GGfx.Blend256(GGfx.Blend256(A[0],A[1]),A[1]); break;
				case 0x8000: A[1] = GGfx.Blend256(A[0],A[1]); break;
				case 0xc000: A[1] = GGfx.Blend256(A[0],GGfx.Blend256(A[0],A[1])); break;
			}
			Line.PtrBYTE += Camera->Stride; X += DX;
		}
	}
	else
	{
		// Horizontal major.
		if( X2 < X1 )
		{
			Exchange(X1, X2);
			Exchange(Y1, Y2);
		}

		INT YInc, LineInc, DX, DY;
		INT Y = (INT)Y1;
		if( Y2 > Y1 )
		{
			// Sloping downward.
			YInc    = 1;
			LineInc = Camera->Stride;
			DY      = (INT)Y2 - Y;
			DX		= (INT)FDX;
		}
		else
		{
			// Sloping upward.
			YInc    = -1;
			LineInc = -Camera->Stride;
			DY		= Y - (INT)Y2;
			DX		= -(INT)FDX;
		}
		FLOAT Adj = (FLOAT)(Y+1) - Y1;
		INT   X   = ftoi(X1 + Adj * FDX);

		RAINBOW_PTR Line = &Camera->Screen[Y * Camera->ByteStride];

		INT   EndX  = Min(ftoi(X2),Fix(Camera->X));
		FLOAT YAdj  = (FLOAT)Unfix(ftoi(X1)) + 1.0 - X1/65536.0;
		INT   FixDY = 65536.0 * 65536.0 / FDX;
		INT   FixY  = 65536.0 * Y1 + YAdj * FixDY;
		INT   MinX  = Unfix(ftoi(X1));
		INT   MaxX  = Unfix(Min(EndX,X));

		if( Y2 < Y1 )
		{
			X += DX;
			MaxX  = Unfix(Min(EndX,X));
		}

// Helper macro for setting up antialiasing.
#define PRE_AA(type) \
	while( DY-- >= 0 ) { \
		type *A = &Line.Ptr##type[MinX - Camera->Stride]; \
		type *B = &Line.Ptr##type[MinX]; \
		type *E = &Line.Ptr##type[MaxX]; \
		if( Y>0 && Y<Camera->Y ) while( B < E ) { \
			switch( FixY & 0xc000 ) {

// Helper macro for finishing antialiasing.
#define POST_AA(type) \
			} \
		FixY += FixDY; A++; B++; \
		} \
	else FixY += FixDY * (E-B); \
	MinX = Unfix(X); \
	MaxX = Unfix(Min(EndX,X += DX)); \
	Line.Ptr##type += LineInc; Y += YInc; \
	} \

		if( Camera->ColorBytes==4 ) 
		{
			PRE_AA(DWORD)
			case 0x0000:
				IFTEST(B[0] = 0xff3f3f;) break;
			case 0x4000: B[0]
				=	((A[0]&0xfcfcfc)>>2) 
				+	((B[0]&0xfcfcfc)>>2) + ((B[0]&0xfefefe)>>1);
				IFTEST(B[0] = 0xff7f7f;) break;
			case 0x8000: B[0]
				=	((A[0]&0xfefefe)>>1)
				+	((B[0]&0xfefefe)>>1); 
				IFTEST(B[0] = 0xffcfcf;) break;
			case 0xc000: B[0]
				=	((A[0]&0xfcfcfc)>>2) + ((A[0]&0xfefefe)>>1)
				+	((B[0]&0xfcfcfc)>>2);
				IFTEST(B[0] = 0xffffff;) break;
			POST_AA(DWORD)
		}
		else if( Camera->ColorBytes==2 && (Camera->Caps & CC_RGB565 ) )
		{
			PRE_AA(WORD);
			case 0x0000:
				break;
			case 0x4000: B[0]
				=	((A[0] & (0xe000 + 0x0780 + 0x001C))>>2) + ((B[0] & (0x0800 + 0x0020 + 0x0001))   )
				+	((B[0] & (0xe000 + 0x0780 + 0x001C))>>2) + ((B[0] & (0xf000 + 0x07C0 + 0x001e))>>1);
				break;
			case 0x8000: B[0]
				=	((A[0] & (0xf000 + 0x07C0 + 0x001e))>>1) + ((B[0] & (0x0800 + 0x0020 + 0x0001))   )
				+	((B[0] & (0xf000 + 0x07C0 + 0x001e))>>1); 
				break;
			case 0xc000: B[0]
				=	((A[0] & (0xe000 + 0x0780 + 0x001C))>>2) + ((A[0] & (0x0800 + 0x0020 + 0x0001))   )
				+	((B[0] & (0xe000 + 0x0780 + 0x001C))>>2) + ((A[0] & (0xf000 + 0x07C0 + 0x001e))>>1);
				break;
			POST_AA(WORD);
		}
		else if( Camera->ColorBytes==2 )
		{
			PRE_AA(WORD);
			case 0x0000:
				break;
			case 0x4000: B[0]
				=	((A[0] & (0x7000 + 0x0380 + 0x001C))>>2) + ((B[0] & (0x0400 + 0x0020 + 0x0001))   )
				+	((B[0] & (0x7000 + 0x0380 + 0x001C))>>2) + ((B[0] & (0x7800 + 0x03C0 + 0x001e))>>1);
				break;
			case 0x8000: B[0]
				=	((A[0] & (0x7800 + 0x03C0 + 0x001e))>>1) + ((B[0] & (0x0400 + 0x0020 + 0x0001))   )
				+	((B[0] & (0x7800 + 0x03C0 + 0x001e))>>1); 
				break;
			case 0xc000: B[0]
				=	((A[0] & (0x7000 + 0x0380 + 0x001C))>>2) + ((A[0] & (0x0400 + 0x0020 + 0x0001))   )
				+	((B[0] & (0x7000 + 0x0380 + 0x001C))>>2) + ((A[0] & (0x7800 + 0x03C0 + 0x001e))>>1);
				break;
			POST_AA(WORD);
		}
		else
		{
			PRE_AA(BYTE);
			case 0x0000: break;
			case 0x4000: B[0] = GGfx.Blend256(GGfx.Blend256(A[0],B[0]),B[0]); break;
			case 0x8000: B[0] = GGfx.Blend256(A[0],B[0]); break;
			case 0xc000: B[0] = GGfx.Blend256(A[0],GGfx.Blend256(A[0],B[0])); break;
			POST_AA(BYTE);
		}
#undef PRE_AA
#undef POST_AA
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Span buffer Bsp rendering.
-----------------------------------------------------------------------------*/

//
// BSP draw list pointer for sorting.
//
struct FBspDrawListPtr
{
	FBspDrawList *Ptr;
	friend inline INT Compare( const FBspDrawListPtr &A, const FBspDrawListPtr &B )
		{ return A.Ptr->Key - B.Ptr->Key; }
};

//
// Draw the entire world.
//
#if 0
///////////////////////////// !!BEGIN TEMPORARY TERRAIN HACK!! ////////////////////////////
void FRender::DrawWorld( UCamera *Camera )
{
	guard(FRender::DrawWorld);

	// Clear the screen.
	memset(Camera->Screen,0,Camera->ByteStride*Camera->Y);

	// Get the height map texture.
	UTexture *HeightMap = new("CracksX",FIND_Optional)UTexture;
	if( !HeightMap ) return;

	UTexture *Surface = new("Default",FIND_Optional)UTexture;
	if( !Surface ) return;

	FMemMark MemMark(GMem);

	FTextureInfo HInfo;
	HeightMap->Lock(HInfo,Camera->Texture,TL_Render|TL_RenderPalette);
	FMipInfo &HMip = *HInfo.Mips[0];

	static FLOAT *Shades;
	static int First=0;
	if( !First )
	{
		First=1;
		Shades = new FLOAT[HMip.USize * HMip.VSize];
		for( DWORD u=0; u<HMip.USize-1; u++ )
		{
			for( DWORD v=0; v<HMip.VSize-1; v++ )
			{
				FLOAT A  = HMip.Data[(u+0) + (v+0)*HMip.USize]/256.0;
				FLOAT B  = HMip.Data[(u+1) + (v+0)*HMip.USize]/256.0;
				FLOAT C  = HMip.Data[(u+0) + (v+1)*HMip.USize]/256.0;
				FLOAT DX = B-A; DX = Sgn(DX) * sqrt(sqrt(Abs(DX)));
				FLOAT DY = C-A; DY = Sgn(DY) * sqrt(sqrt(Abs(DY)));
				Shades[u+v*HMip.USize] = 0.501 + 0.25 * DX + 0.25 * DY;
			}
		}
	}

	FTextureInfo TInfo;
	Surface->Lock(TInfo,Camera->Texture,TL_Render|TL_RenderPalette,(AZoneInfo*)&Camera->Level->Actors(0));
	FMipInfo TMip = *TInfo.Mips[2];

	FLOAT HScale = 128.0;
	FLOAT VScale = -16.0;

	INT Step =
		Camera->Actor->RendMap==REN_DynLight?1:
		Camera->Actor->RendMap==REN_PlainTex?2:
		Camera->Actor->RendMap==REN_Polys   ?4:
		Camera->Actor->RendMap==REN_Zones   ?8:16;

	// Draw backdrop.
	if( Camera->Actor->ShowFlags & SHOW_Backdrop )
	{
		FSpanBuffer Backdrop;
		Backdrop.AllocIndexForScreen(Camera->X,Camera->Y,&GMem);
		DrawBackdrop(Camera,&Backdrop);
	}
	Surface->Unlock(TInfo);

	// Draw the height map as polys.
	static FTransTexture P[5];
	static FTransTexture *T[4] = {&P[0],&P[1],&P[3],&P[2]};

	FLOAT Alpha = 0.5 + 0.5 * cos(RendIter * 0.05);

	FLOAT ViewDist=2048.0; // Figure out the proper math for:
	INT MinU = ((INT)Max((Camera->Coords.Origin.X-ViewDist-2.0*Step*HScale)/HScale,-(int)HMip.USize/2)) & ~(Step-1);
	INT MinV = ((INT)Max((Camera->Coords.Origin.Y-ViewDist-2.0*Step*HScale)/HScale,-(int)HMip.USize/2)) & ~(Step-1);
	INT MaxU = ((INT)Min((Camera->Coords.Origin.X+ViewDist+2.0*Step*HScale)/HScale,+(int)HMip.VSize/2)) & ~(Step-1);
	INT MaxV = ((INT)Min((Camera->Coords.Origin.Y+ViewDist+2.0*Step*HScale)/HScale,+(int)HMip.VSize/2)) & ~(Step-1);

	/*
	MinU = -(int)HMip.USize/2;
	MaxU = +(int)HMip.USize/2;
	MinV = -(int)HMip.VSize/2;
	MaxV = +(int)HMip.VSize/2;
	*/

	for( INT v=MinV; v<MaxV-Step; v+=Step )
	{
		for( INT u=MinU; u<MaxU-Step; u+=Step )
		{
			// Get points.
			int i=0;
			for( int vv=0; vv<2; vv++ )
			{
				for( int uu=0; uu<2; uu++ )
				{
					// Setup.
					int U = u + uu*Step;
					int V = v + vv*Step;

					// Get coords.
					*(FVector*)T[i] = FVector( HScale*U, HScale*V, VScale*HMip.Data[U + V*HMip.USize]).TransformPointBy(Camera->Coords);
					FLOAT Dist      = T[i]->Size();
					FLOAT Darken    = Max(0.05,1.0 - Dist/ViewDist);

					// Setup texture/shading.
					T[i]->U = U * HScale * 65536.0 * 0.5; // Last#=sizing relative to norm
					T[i]->V = V * HScale * 65536.0 * 0.5;
					T[i]->G = 256.0 * 63.0 * Shades[(U&(HMip.USize-1)) + (V&(HMip.VSize-1))*HMip.USize] * Darken;

					// Outcode it.
					T[i]->ComputeOutcode(*Camera);

					// Project.
					FLOAT Factor  = Camera->ProjZ / T[i]->Z;
					T[i]->ScreenX = T[i]->X * Factor + Camera->FX15;
					T[i]->ScreenY = T[i]->Y * Factor + Camera->FY15;

					i++;
				}
			}
			// Draw textured surfaces.
			if( 1 )
			{
				// Clip and draw as 2 triangles.
				FTransTexture NewPts[16];
				P[4]=P[0];
				for( int TriOfs=0; TriOfs<=2; TriOfs+=2 )
				{
					int NumPts = GRender.ClipTexPoints(Camera,&P[TriOfs],NewPts,3);
					if( NumPts > 0 )
					{
						// Draw tex.
						FMemMark Mark(GMem);
						FRasterTexSetup RasterTexSetup;
						FRasterTexPoly *RasterTexPoly = (FRasterTexPoly	*)new(GMem,sizeof(FRasterTexPoly)+FGlobalRaster::MAX_RASTER_LINES*sizeof(FRasterTexLine))BYTE;

						// Setup.
						RasterTexSetup.Setup(*Camera,NewPts,NumPts,&GMem);
						RasterTexSetup.Generate(RasterTexPoly);

						// Draw it.
						FPolySpanTextureInfoBase *Info = GSpanTextureMapper->SetupForPoly( Camera, Surface, 0, 0, 0 );
						RasterTexPoly->Draw(*Camera,*Info,NULL);

						// Finish up.
						GSpanTextureMapper->FinishPoly(Info);
						RasterTexSetup.Release();
						Mark.Pop();
					}
				}
			}
			// Draw lines.
			if( 0 )
			{
				BYTE Color = u>=MinU && v>=MinV && u<MaxU && v<MaxV ? AddWireColor : BrushWireColor;
				Draw3DLine(Camera,&P[0],&P[1],0,Color,1,0);
				Draw3DLine(Camera,&P[0],&P[3],0,Color,1,0);
			}
		}
	}
	HeightMap->Unlock(HInfo);
	MemMark.Pop();
	unguard;
}
///////////////////////////// END TEMPORARY TERRAIN HACK ////////////////////////////
#else
void FRender::DrawWorld( UCamera *Camera )
{
	guard(FRender::DrawWorld);
	checkState(Camera->Level->Info!=NULL);
	FMemMark MemMark(GMem);
	FMemMark DynMark(GDynMem);

	UModel			*Model = Camera->Level->Model;
	FSpanBuffer		Backdrop;
	FBspDrawList	*DrawList;
	AActor			*Exclude;

	// Antialiasing.
	if( Antialias )
		Camera->ExtraPolyFlags |= PF_NoMerge;

	// Fill the screen if desired.
	if( LeakCheck )
		memset( Camera->Screen, BrushWireColor, Camera->ByteStride*Camera->Y );

	// Tick stuff.
	GRandoms->Tick( Camera->Level->Info->TimeSeconds );
	TemporalIter = Temporal ? TemporalIter+1 : 0;

	// Init stuff.
	dynamicsLock		(Camera->Level->Model);
	InitTransforms		(Camera->Level->Model);

	// Set up dynamic object rendering.
	if( Camera->Actor->ShowFlags & SHOW_Actors )
	{
		Exclude = (AActor*)Camera->Actor;
		if( Camera->Actor->bBehindView ) Exclude = NULL;
		dynamicsSetup( Camera, Exclude );
	}

	// Prerender the world.
	if( Model->Nodes->Num )
	{
		Backdrop.AllocIndex(0,0,&GDynMem);
		DrawList = OccludeBsp( Camera, &Backdrop );
	}
	else
	{
		Backdrop.AllocIndexForScreen(Camera->X,Camera->Y,&GDynMem);
		DrawList = NULL;
	}

	// Perform dynamic lighting only on visible parts of the bsp.
	GLightManager->DoDynamicLighting( Camera->Level );

	// Count surfaces to draw.
	int Num[2]={0,0};
	for( FBspDrawList *Draw = DrawList; Draw!=NULL; Draw = Draw->Next )
		Num[(Draw->PolyFlags & PF_NoOcclude)!=0]++;

	// Group surfaces into solid (draw-order irrelevant) and transparent.
	FBspDrawListPtr *FirstDraw [2] = {new(GMem,Num[0])FBspDrawListPtr,new(GMem,Num[1])FBspDrawListPtr};
	FBspDrawListPtr *LastDraw  [2] = {FirstDraw[0],FirstDraw[1]};
	for( Draw = DrawList; Draw!=NULL; Draw = Draw->Next )
		(LastDraw[(Draw->PolyFlags & PF_NoOcclude)!=0]++)->Ptr = Draw;

	// Sort solid surfaces by texture and then by palette for cache coherence.
	QSort( FirstDraw[0], Num[0] );

	if( GCameraManager->RenDev )
	{
		// Draw everything in the world from front to back to hardware.
		for( int Pass=0; Pass<2; Pass++ )
		{
			FBspDrawListPtr *DrawPtr = FirstDraw[Pass];
			while( DrawPtr < LastDraw[Pass] )
			{
				// Setup for this surface.
				FBspDrawList *Draw      = DrawPtr->Ptr;
				UModel 		 *Model 	= Camera->Level->Model;
				FBspSurf	 *Surf 		= &Model->Surfs( Draw->iSurf );
				UTexture     *Texture   = Surf->Texture ? Surf->Texture : GGfx.DefaultTexture;
				FVector	     TextureU	= GetVector( Model,  Camera->Coords, Surf->vTextureU ) / 65536.0;
				FVector	     TextureV 	= GetVector( Model,  Camera->Coords, Surf->vTextureV ) / 65536.0;
				FVector	     &Normal   	= GetVector( Model,  Camera->Coords, Surf->vNormal   );
				FVector	     &Base     	= GetPoint ( Camera, Surf->pBase);

				// Setup dynamic lighting for this surface.
				GLightManager->SetupForSurf
				(
					Camera,
					Normal,
					Base,
					Draw->iSurf,
					Surf->iLightMesh,
					Draw->PolyFlags,
					Camera->Level->GetZoneActor(GBlit.iZone),
					Texture
				);

				// Draw all polys sharing this surface.
				INDEX iCurrentSurf = Draw->iSurf;
				do
				{
					Draw = DrawPtr->Ptr;
					GCameraManager->RenDev->DrawPolyV
					(
						Camera, 
						Texture,
						Draw->Pts,
						Draw->NumPts,
						Base,
						Normal,
						TextureU,
						TextureV,
						Surf->PanU,
						Surf->PanV,
						Draw->PolyFlags
					);
					DrawPtr++;
				} while( DrawPtr<LastDraw[Pass] && DrawPtr->Ptr->iSurf==iCurrentSurf );
				GLightManager->ReleaseLightBlock();
			}
		}
	}
	else
	{
		// Show backdrop if desired.
		if( (Camera->Actor->ShowFlags & SHOW_Backdrop) && (Backdrop.ValidLines>0) )
			DrawBackdrop(Camera,&Backdrop);

		// Draw everything.
		UTexture *PrevTex = NULL;
		for( int Pass=0; Pass<2; Pass++ )
		{
			for( FBspDrawListPtr *DrawPtr = FirstDraw[Pass]; DrawPtr < LastDraw[Pass]; DrawPtr++ )
			{
				FBspDrawList *Draw = DrawPtr->Ptr;
				if( !(Draw->PolyFlags & PF_Portal) )
				{
					// Handle texture.
					if( Draw->Texture!=PrevTex || !PrevTex )
					{
						STAT(GStat.UniqueTextures++);
						if( Draw->Texture ) STAT(GStat.UniqueTextureMem+=Draw->Texture->USize*Draw->Texture->VSize);
						for( int j=0; j<MAX_MIPS; j++ ) GBlit.PrevMipRef[j]=0;
						PrevTex = Draw->Texture;
					}

					// Update span bounds.
					if( Pass == 0 )
					{
						FBspSurf *Poly = &Model->Surfs(Draw->iSurf);
						Draw->Span.GetValidRange(&Poly->LastStartY,&Poly->LastEndY);
					}

					// Draw it.
					DrawBspSurf( Camera, Draw );
				}
			}
		}
		if( Antialias )
		{	
			// Antialias in depth order.
			for( FBspDrawList *Draw = DrawList; Draw!=NULL; Draw = Draw->Next )
			{
				// Edge antialiasing test.
				FTransform Pts[32];
				int NumPts = ClipBspSurf( Model, Camera, Draw->iNode, Pts );
				for( int i=0; i<NumPts; i++ )
				{
					// Get points.
					FTransform *V1   = &Pts[i];
					FTransform *V2   = &Pts[i?i-1:NumPts-1];
					AntialiasEdge( Camera, V1->ScreenX, V1->ScreenY, V2->ScreenX, V2->ScreenY );
				}
			}
		}
	}

	if( Camera->Actor->ShowFlags & SHOW_Actors )
		dynamicsFinalize( Camera, 1 );

	// Draw all moving brushes as wireframes.
	if( Camera->Actor->ShowFlags & SHOW_MovingBrushes )
		DrawMovingBrushWires(Camera);

	// Finish up.
	STAT(GStat.GMem       = GMem.GetByteCount());
	STAT(GStat.GDynMem    = GDynMem.GetByteCount());
	STAT(GStat.NodesTotal = Model->Nodes->Num);

	GLightManager->UndoDynamicLighting( Camera->Level );

	ExitTransforms();
	dynamicsUnlock(Camera->Level->Model);
	MemMark.Pop();
	DynMark.Pop();

	unguard;
}
#endif

void FRender::DrawBackdrop( UCamera *Camera, FSpanBuffer *SpanBuffer )
{
	guard(FRender::DrawBackdrop);
	FMemMark Mark(GMem);

	for( int j=0; j<MAX_MIPS; j++ )
		GBlit.PrevMipRef[j]=0;

	FBspDrawList Draw;
	Draw.iNode		= INDEX_NONE;
	Draw.iSurf		= INDEX_NONE;
	Draw.iZone		= 0;
	Draw.PolyFlags	= 0;
	Draw.NumPts		= 0;
	Draw.Span		= *SpanBuffer;
	Draw.Texture	= Camera->Level->Info->SkyTexture;

	DrawSoftwareTexturedBspSurf(Camera,&Draw);

	Mark.Pop();
	unguard;
}

/*-----------------------------------------------------------------------------
	Texture locking/unlocking functions.
-----------------------------------------------------------------------------*/

// Structure containing all platform-specific locked texture info.
struct FTextureInfoPlatform
{
	FCacheItem *Item;
};

// Called after the engine locks a texture, so that we can perform any
// rendering engine specific locking work.
void FRender::PostLockTexture( FTextureInfo &TextureInfo, UTexture *Camera )
{
	guard(FRender::PostLockTexture);
	FTextureInfoPlatform &Plat = *(FTextureInfoPlatform *)TextureInfo.Platform;

	// Init Plat.
	Plat.Item = NULL;

	if( TextureInfo.Flags & TL_RenderPalette )
	{
		// Get a Camera-compatible palette.
		checkState(TextureInfo.Palette!=NULL);
		TextureInfo.Colors = TextureInfo.Palette->GetColorDepthPalette(Plat.Item,Camera);
	}
	else if( TextureInfo.Flags & TL_RenderRamp )
	{
		// Get a Camera-compatible ramp.
		TextureInfo.Colors = GetPaletteLightingTable
		(
			Camera,
			TextureInfo.Palette,
			TextureInfo.Zone,
			NULL,
			Plat.Item
		);
	}
	unguard;
}

// Called before the engine unlocks a texture, so that we can perform any
// rendering engine specific unlocking work.
void FRender::PreUnlockTexture( FTextureInfo &TextureInfo )
{
	guard(FRender::PreUnlockTexture);
	FTextureInfoPlatform &Plat = *(FTextureInfoPlatform *)TextureInfo.Platform;

	// Unlock cache items.
	if( Plat.Item ) Plat.Item->Unlock();

	unguard;
}

// Called after the engine loads a texture, so that we can perform any
// rendering engine specific fixup work.
void FRender::PostLoadTexture( UTexture *Texture, DWORD PostFlags )
{
	guard(FRender::PostLoadTexture);
	unguard;
}

// Called before the engine kills a texture, so that we can perform any
// rendering engine specific texture killing work.
void FRender::PreKillTexture( UTexture *Texture )
{
	guard(FRender::PreKillTexture);
	unguard;
}

/*-----------------------------------------------------------------------------
	Global subsystem instantiation.
-----------------------------------------------------------------------------*/

FGlobalRaster					GRaster;
FGlobalBlit						GBlit;
FRender							GRender;
UNRENDER_API FRenderBase		*GRendPtr = &GRender;
UNRENDER_API FEditorRenderBase	*GEditorRendPtr = &GRender;

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
