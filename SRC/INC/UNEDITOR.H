/*=============================================================================
	UnEditor.h: Main classes used by the Unreal editor

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNEDITOR
#define _INC_UNEDITOR
#if EDITOR /* Only include if editor subsystem is to be compiled */

#ifdef COMPILING_EDITOR
	UNEDITOR_API extern class FGlobalEditor GUnrealEditor;
#endif

/*-----------------------------------------------------------------------------
	FScan.
-----------------------------------------------------------------------------*/

//
// Scanner types, for mouse click hit checking.
//
enum EEdScan
{
	EDSCAN_None,			// Nothing found yet.
	EDSCAN_BspNodePoly, 	// Index = iNode, A = unused.
	EDSCAN_BspNodeSide,     // Index = iNode, A = side index.
	EDSCAN_BspNodeVertex, 	// Index = iNode, A = vertex index.
	EDSCAN_BrushPoly,		// Index = Brush ID, A = poly index.
	EDSCAN_BrushSide,       // Index = Brush ID, A = poly index, B = side index.
	EDSCAN_BrushVertex,     // Index = Brush ID, A = poly index, B = vertex index.
	EDSCAN_Actor,			// Index = actor pointer.
	EDSCAN_UIElement,		// Misc user interface elements, Index=element number.
	EDSCAN_BrowserTex,		// Texture in texture browser.
	EDSCAN_BrowserMesh,		// Mesh in mesh browser.
};

//
// Scanner parameters.
//
enum {EDSCAN_RADIUS=1  }; // Search center plus this many pixels on all sides.
enum {EDSCAN_IGNORE=254}; // Ignores color 254 while scanning.

//
// Scanner class, for mouse click hit testing.  Init() is called before
// a frame is rendered.  Before each object is drawn, PreScan() is called
// to remember the state of the screen under the mouse pointer.  After
// the object is drawn, PostScan() is called to see if the area under the
// mouse was changed.  By the time the frame is finished rendering from front
// to back, FScan knows what the user clicked on, if anything.
//
class UNEDITOR_API FScan
{
public:
	UCamera	*Camera;	// Remembered camera.
	int		Active;  	// 1 = actively scanning, 0=we are not scanning.
	int		X;			// Scan X location in screenspace (mouse click location).
	int		Y;			// Scan Y location in screenspace.
	int		Type;		// Type of most recent hit, or SCAN_NONE if nothing found yet.
	int  	Index;		// Index of most recent hit.
	int  	A;			// PostScan A value of most recent hit.
	int  	B;			// PostScan B value of most recent hit.
	FVector	V;			// Any floating point vector that you want.
	DWORD	Pixels [1 + 2 * EDSCAN_RADIUS][1 + 2 * EDSCAN_RADIUS];  // PreScan pixel values.

	// Functions.
	virtual void Init		(UCamera *NewCamera);
	virtual void Exit		();
	virtual void PreScan  	();
	virtual void PostScan 	(EEdScan ScanType, int Index, int A, int B, const FVector *V);
};

/*-----------------------------------------------------------------------------
	FConstraints.
-----------------------------------------------------------------------------*/

//
// General purpose movement/rotation constraints.
//
class UNEDITOR_API FConstraints
{
public:

	// Toggles.
	BOOL		GridEnabled;		// Grid on/off.
	BOOL		RotGridEnabled;		// Rotation grid on/off.
	BOOL		SnapVertex;			// Snap to nearest vertex within SnapDist, if any.
	DWORD		Flags;				// Movement constraint bit flags.

	FVector		Grid;				// Movement grid.
	FVector		GridBase;			// Base (origin) of movement grid.
	FRotation	RotGrid;			// Rotation grid.

	FLOAT		SnapDist;			// Distance to check for snapping.
};

/*-----------------------------------------------------------------------------
	Enums.
-----------------------------------------------------------------------------*/

//
// Quality level for rebuilding Bsp.
//
enum EBspOptimization
{
	BSP_Lame,
	BSP_Good,
	BSP_Optimal
};

//
// Editor mode settings.
//
// These are also referenced by help files and by the editor client, so
// they shouldn't be changed.
//
enum EEditorMode
{
	EM_None 			= 0,	// Gameplay, editor disabled.
	EM_CameraMove		= 1,	// Move camera normally.
	EM_CameraZoom		= 2,	// Move camera with acceleration.
	EM_BrushFree		= 3,	// Move brush free-form.
	EM_BrushMove		= 4,	// Move brush along one axis at a time.
	EM_BrushRotate		= 5,	// Rotate brush.
	EM_BrushSheer		= 6,	// Sheer brush.
	EM_BrushScale		= 7,	// Scale brush.
	EM_BrushStretch		= 8,	// Stretch brush.
	EM_AddActor			= 9,	// Add actor/light.
	EM_MoveActor		= 10,	// Move actor/light.
	EM_TexturePan		= 11,	// Pan textures.
	EM_TextureSet		= 12,	// Set textures.
	EM_TextureRotate	= 13,	// Rotate textures.
	EM_TextureScale		= 14,	// Scale textures.
	EM_BrushWarp		= 16,	// Warp brush verts.
	EM_Terraform		= 17,	// Terrain edit.
	EM_BrushSnap		= 18,	// Brush snap-scale.
	EM_TexView			= 19,	// Viewing textures.
	EM_TexBrowser		= 20,	// Browsing textures.
	EM_MeshView			= 21,	// Viewing mesh.
	EM_MeshBrowser		= 22,	// Browsing mesh.
};

//
// Editor mode classes.
//
// These are also referenced by help files and by the editor client, so
// they shouldn't be changed.
//
enum EEditorModeClass
{
	EMC_None		= 0,	// Editor disabled.
	EMC_Camera		= 1,	// Moving the camera.
	EMC_Brush		= 2,	// Affecting the brush.
	EMC_Actor		= 3,	// Affecting actors.
	EMC_Texture		= 4,	// Affecting textures.
	EMC_Player		= 5,	// Player movement.
	EMC_Terrain		= 6,	// Terrain editing.
};

//
// Bsp poly alignment types for polyTexAlign.
//
enum ETexAlign						
{
	TEXALIGN_Default		= 0,	// No special alignment (just derive from UV vectors).
	TEXALIGN_Floor			= 1,	// Regular floor (U,V not necessarily axis-aligned).
	TEXALIGN_WallDir		= 2,	// Grade (approximate floor), U,V X-Y axis aligned.
	TEXALIGN_WallPan		= 3,	// Align as wall (V vertical, U horizontal).
	TEXALIGN_OneTile		= 4,	// Align one tile.
	TEXALIGN_WallColumn		= 5,	// Align as wall on column.
};

//
// Things to set in mapSetBrush.
//
enum EMapSetBrushFlags				
{
	MSB_BrushColor	= 1,			// Set brush color.
	MSB_Group		= 2,			// Set group.
	MSB_PolyFlags	= 4,			// Set poly flags.
};

//
// Possible positions of a child Bsp node relative to its parent (for BspAddToNode).
//
enum ENodePlace 
{
	NODE_Back		= 0, // Node is in back of parent              -> Bsp[iParent].iBack.
	NODE_Front		= 1, // Node is in front of parent             -> Bsp[iParent].iFront.
	NODE_Plane		= 2, // Node is coplanar with parent           -> Bsp[iParent].iPlane.
	NODE_Root		= 3, // Node is the Bsp root and has no parent -> Bsp[0].
};

/*-----------------------------------------------------------------------------
	FGlobalEditor definition.
-----------------------------------------------------------------------------*/

typedef void (*POLY_CALLBACK)(UModel *Model, INDEX iSurf);

//
// The global Unreal editor.
//
class UNEDITOR_API FGlobalEditor
{
public:

	// Default macro record-buffer size.
	enum {MACRO_TEXT_REC_SIZE=20000};

	// Object array, containing held texture 
	// sets, classes, and meshmaps.
	UArray::Ptr			EditorArray;

	// Objects.
	UModel				*TempModel;
	UTexture			*CurrentTexture;
	UClass				*CurrentClass;
	TArray<UTextureSet*>*TextureSets;
	ULinkerLoad			*ClassLinker;

	// Toggles.
	int 			Mode;
	int 			ShowVertices;
	int 			MapEdit;
	int 			Show2DGrid;
	int			    Show3DGrid;
	int				FastRebuild;
	int				Bootstrapping;
	int				Pad[4];

	FLOAT			MovementSpeed;
	FConstraints	Constraints;
	UTextBuffer		*MacroRecBuffer;

	// Editor camera scanner.
	FScan			Scan;

	// Functions.
	virtual void	Init();
	virtual void	Exit();
	virtual int		Exec(const char *Cmd,FOutputDevice *Out=GApp);
	virtual void	NoteMacroCommand(const char *Cmd);
	virtual void	LoadClasses();
	virtual void	Cleanse(FOutputDevice& Out,int Redraw,const char *TransReset=NULL);

	// Editor mode virtuals from UnEdCam.cpp.
	virtual void	edcamSetMode			(int Mode);
	virtual int		edcamMode				(UCamera *Camera);
	virtual int		edcamModeClass			(int Mode);

	// Editor CSG virtuals from UnEdCsg.cpp.
	virtual void    csgPrepMovingBrush      (UModel *Brush);
	virtual UModel	*csgDuplicateBrush		(ULevel *Level,UModel *Brush, DWORD PolyFlags, DWORD ModelFlags, DWORD ResFlags, BOOL IsMovingBrush);
	virtual UModel	*csgAddOperation		(UModel *Brush,ULevel *Level, DWORD PolyFlags, ECsgOper CSG, BYTE BrushFlags);
	virtual void	csgRebuild		 		(ULevel *Level);
	virtual void	csgInvalidateBsp		(ULevel *Level);
	virtual char	*csgGetName 			(ECsgOper CsgOper);

	// Editor EdPoly/BspSurf assocation virtuals from UnEdCsg.cpp.
	virtual int		polyFindMaster			(UModel *Model, INDEX iSurf, FPoly &Poly);
	virtual void    polyUpdateMaster		(UModel *Model, INDEX iSurf,int UpdateTexCoords,int UpdateBase);

	// Bsp Poly search virtuals from UnEdCsg.cpp.
	virtual void	polyFindByFlags 		(UModel *Model,DWORD SetBits, DWORD ClearBits, POLY_CALLBACK Callback);
	virtual void	polyFindByBrush 		(UModel *Model,UModel *Brush, INDEX BrushPoly, POLY_CALLBACK Callback);
	virtual void	polyFindByBrushGroupItem(UModel *Model,UModel *Brush, INDEX BrushPoly,FName Group, FName Item,POLY_CALLBACK Callback);
	virtual void	polySetAndClearPolyFlags(UModel *Model,DWORD SetBits, DWORD ClearBits,int SelectedOnly,int UpdateMaster);
	virtual void	polySetAndClearNodeFlags(UModel *Model,DWORD SetBits, DWORD ClearBits);

	// Bsp Poly selection virtuals from UnEdCsg.cpp.
	virtual void	polyResetSelection 		(UModel *Model);
	virtual void	polySelectAll 			(UModel *Model);
	virtual void	polySelectNone 			(UModel *Model);
	virtual void	polySelectMatchingGroups(UModel *Model);
	virtual void	polySelectMatchingItems	(UModel *Model);
	virtual void	polySelectCoplanars		(UModel *Model);
	virtual void	polySelectAdjacents		(UModel *Model);
	virtual void	polySelectAdjacentWalls	(UModel *Model);
	virtual void	polySelectAdjacentFloors(UModel *Model);
	virtual void	polySelectAdjacentSlants(UModel *Model);
	virtual void	polySelectMatchingBrush	(UModel *Model);
	virtual void	polySelectMatchingTexture(UModel *Model);
	virtual void	polySelectReverse 		(UModel *Model);
	virtual void	polyMemorizeSet 		(UModel *Model);
	virtual void	polyRememberSet 		(UModel *Model);
	virtual void	polyXorSet 				(UModel *Model);
	virtual void	polyUnionSet			(UModel *Model);
	virtual void	polyIntersectSet		(UModel *Model);

	// Poly texturing virtuals from UnEdCsg.cpp.
	virtual void	polyTexPan 				(UModel *Model,int PanU,int PanV,int Absolute);
	virtual void	polyTexScale			(UModel *Model,FLOAT UU,FLOAT UV, FLOAT VU, FLOAT VV,int Absolute);
	virtual void	polyTexAlign			(UModel *Model,ETexAlign TexAlignType,DWORD Texels);

	// Map brush selection virtuals from UnEdCsg.cpp.
	virtual void	mapSelectAll			(ULevel *Level);
	virtual void	mapSelectNone			(ULevel *Level);
	virtual void	mapSelectOperation		(ULevel *Level,ECsgOper CSGOper);
	virtual void	mapSelectFlags			(ULevel *Level,DWORD Flags);
	virtual void	mapSelectPrevious		(ULevel *Level);
	virtual void	mapSelectNext			(ULevel *Level);
	virtual void	mapSelectFirst 			(ULevel *Level);
	virtual void	mapSelectLast 			(ULevel *Level);
	virtual void	mapBrushGet				(ULevel *Level);
	virtual void	mapBrushPut				(ULevel *Level);
	virtual void	mapDelete				(ULevel *Level);
	virtual void	mapDuplicate			(ULevel *Level);
	virtual void	mapSendToFirst			(ULevel *Level);
	virtual void	mapSendToLast			(ULevel *Level);
	virtual void	mapSetBrush				(ULevel *Level,EMapSetBrushFlags PropertiesMask,WORD BrushColor,
		FName Group,DWORD SetPolyFlags,DWORD ClearPolyFlags);

	// Editor actor virtuals from UnEdAct.cpp.
	virtual void	edactMoveSelected 		(ULevel *Level, FVector &Delta, FRotation &DeltaRot);
	virtual void	edactSelectAll 			(ULevel *Level);
	virtual void	edactSelectNone 		(ULevel *Level);
	virtual void	edactSelectOfClass		(ULevel *Level,UClass *Class);
	virtual void	edactDeleteSelected 	(ULevel *Level);
	virtual void	edactDuplicateSelected 	(ULevel *Level);
	virtual void	edactDeleteDependentsOf	(ULevel *Level,UClass *Class);

	// Bsp virtuals from UnBsp.cpp.
	virtual INDEX	bspAddVector		(UModel *Model, FVector *V, int Exact);
	virtual INDEX	bspAddPoint			(UModel *Model, FVector *V, int Exact);
	virtual int		bspNodeToFPoly		(UModel *Model, INDEX iNode, FPoly *EdPoly);
	virtual void	bspBuild			(UModel *Model, EBspOptimization Opt, int Balance, int RebuildSimplePolys);
	virtual void	bspRefresh			(UModel *Model,int NoRemapSurfs);
	virtual void	bspCleanup 			(UModel *Model);
	virtual void	bspBuildBounds		(UModel *Model);
	virtual void	bspBuildFPolys		(UModel *Model,int iSurfLinks);
	virtual void	bspMergeCoplanars	(UModel *Model,int RemapLinks,int MergeDisparateTextures);
	virtual int		bspBrushCSG 		(UModel *Brush, UModel *Model, DWORD PolyFlags, ECsgOper CSGOper,int RebuildBounds);
	virtual void	bspOptGeom			(UModel *Model);
	virtual void	bspValidateBrush	(UModel *Brush,int ForceValidate,int DoStatusUpdate);
	virtual INDEX	bspAddNode			(UModel *Model, INDEX iParent, ENodePlace ENodePlace, DWORD NodeFlags, FPoly *EdPoly);

	// Shadow virtuals (UnShadow.cpp).
	virtual void	shadowIlluminateBsp (ULevel *Level, int Selected);

	// Constraints (UnEdCnst.cpp).
	virtual void	constraintInit				(FConstraints *Constraints);
	virtual int		constraintApply 			(UModel *LevelModel, UModel *BrushInfo, FVector *Location, FRotation *Rotation,FConstraints *Constraints);
	virtual void	constraintFinishSnap 		(ULevel *Level,UModel *Brush);
	virtual void	constraintFinishAllSnaps	(ULevel *Level);

	// Camera functions (UnEdCam.cpp).
	virtual void	edcamDraw		(UCamera *Camera, int Scan);
	virtual void	edcamMove		(UCamera *Camera, BYTE Buttons, FLOAT MouseX, FLOAT MouseY, int Shift, int Ctrl);
	virtual int		edcamKey		(UCamera *Camera, int Key);
	virtual void	edcamClick		(UCamera *Camera, BYTE Buttons, SWORD MouseX, SWORD MouseY,int Shift, int Ctrl);
	
	// Mesh functions (UnMeshEd.cpp).
	virtual void meshImport(const char *MeshName, const char *AnivFname, const char *DataFname);
	virtual void meshBuildBounds(UMesh *Mesh);

	// Visibility.
	virtual void TestVisibility(ULevel *Level,UModel *Model,int A, int B);

	// Scripts.
	virtual int MakeScripts(int MakeAll);
	virtual int CheckScripts(UClass *Class,FOutputDevice &Out);
	virtual int CompileScript(UClass *Class,BOOL ObjectPropertiesAreValid,BOOL Booting);
	virtual void DecompileScript(UClass *Class,FOutputDevice &Out,int ParentLinks);
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // EDITOR
#endif // _INC_UNEDITOR
