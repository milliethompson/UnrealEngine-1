/*=============================================================================
	UnActLst.h: Actor list resource class definition

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
        * Aug 30, 1996: Mark added PLevel
		* Oct 19, 1996: Tim redesigned to eliminate redundency
=============================================================================*/

#ifndef _INC_UNACTLST
#define _INC_UNACTLST

/*-----------------------------------------------------------------------------
	UActorList.
-----------------------------------------------------------------------------*/

//
// A list of actors associated with a level.  The actor list is sparse, in that
// actors with Class==NULL are treated as empty/nonexistant.
//
class UNREAL_API UActorList : public UDatabase
{
	RESOURCE_DB_CLASS(UActorList,AActor,RES_ActorList)

	// Variables.
	DWORD		LockType;		// ELockType (None, read, write, notrans)
	BOOL		Trans;			// Whether transaction tracking is enabled

	// Resource functions.
	void Register				(FResourceType *Type);
	void InitHeader				();
	void InitData				();
	const char *Import			(const char *Buffer, const char *BufferEnd,const char *FileType);
	char *Export				(char *Buffer,const char *FileType,int Indent);
	void QueryHeaderReferences	(FResourceCallback &Callback);
	void QueryDataReferences	(FResourceCallback &Callback);
	void PostLoad				();
	void PreKill				();
	void Flip					();

	// Custom functions.
	void Lock(int NewLockType);
	void Unlock();

    // Redundant compact actor arrays provided for performance.
    struct CompactList : public DArray<AActor*,200,100> 
    {
        BOOL NeedsCompressing; // TRUE when the list might have elements with value 0.
        CompactList() { NeedsCompressing = FALSE; }

        // Remove an actor from the list: The actor is not removed immediately but is
        // instead replaced with the value 0. This allows uninterrupted iteration over
        // the list. It is assumed the actor is on the list at most once.
        void RemoveActor(AActor * Actor)
        {
            ChangeElement(Actor,0);
            NeedsCompressing = TRUE;
        }
        void Compress() // Compress the list if NeedsCompressing==TRUE.
        {
            if( NeedsCompressing )
            {
                RemoveElements(0); // Clear out the null entries.
                NeedsCompressing = FALSE;
            }
        }
    };
    // These are the main lists.
    CompactList * StaticActors        ; // List of all actors with Class != 0 && bStaticActor==TRUE.
    CompactList * DynamicActors       ; // List of all actors with Class != 0 && bStaticActor==FALSE.
    CompactList * CollidingActors     ; // List of all actors with Class != 0 && bCollideActors==TRUE.
    // These are the administrative lists.
    CompactList * ActiveActors        ; // List of all actors with Class != 0.
    CompactList * UnusedActors        ; // List of all actors with Class == 0 && bJustDeleted==FALSE.
    CompactList * JustDeletedActors   ; // List of all actors with Class == 0 && bJustDeleted==TRUE.
    void RelistActors(); // Rebuild the above redundant lists from scratch by scanning the actor list.
    void UnlistActor(AActor * Actor); // Remove an actor from the appropriate main lists (based on its properties).
        // Note: UnlistActor does *not* remove the actor from the administrative lists.
    void ListActor(AActor * Actor); // Add an actor to appropriate main lists (based on its properties).
        // Note: ListActor does *not* remove the actor from the administrative lists.
    void CheckLists(); // Debug: Check the redundant lists for correctness and completeness.
};

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
#endif // _INC_UNACTLST
