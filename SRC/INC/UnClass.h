/*=============================================================================
	UnClass.h: Actor class list resource class definition

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * Aug 30, 1996: Mark added PLevel
		* Oct 19, 1996: Tim redesigned to eliminate redundency
=============================================================================*/

#ifndef _INC_UNCLASS
#define _INC_UNCLASS

/*-----------------------------------------------------------------------------
	UClass.
-----------------------------------------------------------------------------*/

//
// An actor class.  Describes all of the attributes of a kind of actor, but
// does not refer to an actual instance of an actor.
//
class UNREAL_API UClass : public UDatabase
{
	RESOURCE_DB_CLASS(UClass,FClassProperty,RES_Class)

	// Constants.
	enum {MAX_CLASS_PROPERTIES=384}; // Predefined limit of properties.

	// General variables.
	UClass		*ParentClass;		// Parent class of the actor, or NULL if this class is the root.
	UTextBuffer	*ScriptText;		// Optional script source code, NULL if none.
	UScript		*Script;			// Optional compiled script object code, NULL if not valid.
	BOOL		Intrinsic;			// Whether the class logic is scripted or intrinsic.

	// Pointer to actor's ::Process member function.
	AActorBase::ACTOR_PROCESS_FUNC ActorFunc; // Valid only in memory, associated in PostLoad

	// Dependence info for recompiling class scripts when needed.
	DWORD		ScriptTextCRC;		// CRC of ScriptText data after most recent successful compile.
	DWORD		ParentTextCRC;		// CRC of parent's ScriptText data after most recent successful compile.
	DWORD		Pad1,Pad2,Pad3;

	// Default actor properties.  These are copied to new actors when new actors of this 
	// class are spawned.
	AActor		DefaultActor;		// Default actor.

	// Resource functions.
	void Register				(FResourceType *Type);
	void InitHeader				();
	void InitData				();
	int  QuerySize				();
	int  QueryMinSize			();
	const char *Import			(const char *Buffer, const char *BufferEnd,const char *FileType);
	char *Export				(char *Buffer,const char *FileType,int Indent);
	void QueryHeaderReferences	(FResourceCallback &Callback);
	void QueryDataReferences	(FResourceCallback &Callback);
	void PostLoad				();
	void Flip					();

	// Inlines.
	inline int IsKindOf( const UClass *SomeParent ) const
	{
		for( const UClass *Class=this; Class; Class=Class->ParentClass )
			if (Class==SomeParent) 
				return 1;
		return 0;
	}
	inline int IsKindOf( const char *ClassName ) const
	{
		for( const UClass *Class=this; Class; Class=Class->ParentClass )
			if( stricmp(Class->Name,ClassName)==0 ) 
				return 1;
		return 0;
	}
	inline int IsNamed( const char *ClassName ) const
	{
		return stricmp(Name,ClassName)==0;
	}

	// Functions.
	UClass						(UClass *ParentClass);
	void AddParentProperties	();
	void Delete					();
	void FindActorFunc			();
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNCLASS
