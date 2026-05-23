/*=============================================================================
	UnTopics.h: Unreal information topics for Unreal/UnrealEd communication

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#ifndef _INC_UNTOPICS
#define _INC_UNTOPICS

/*-------------------------------------------------------------------------------
	FTopicHandler.
-------------------------------------------------------------------------------*/

//
// The topic handler base class.  All specific topic handlers are derived from this.
//
class FTopicHandler
{
public:
	// Name of topic handler, set by FTopicTable::Register.
	char TopicName[NAME_SIZE];

	// Next topic handler in linked list of global topic list.
	FTopicHandler *Next;

	// Public functions.
	virtual void Set (ULevel *Level, const char *Item, const char *Data)=0;
	virtual void Get (ULevel *Level, const char *Item, FOutputDevice &Out)=0;
};

/*-------------------------------------------------------------------------------
	FGlobalTopicTable.
-------------------------------------------------------------------------------*/

//
// Global topic table class.  Contains all available topics.
//
class UNENGINE_API FGlobalTopicTable
{
public:

	// Variables.
	FTopicHandler *FirstHandler; // Linked list; last entry is NULL.

	// Functions.
	void	Register(const char *TopicName,FTopicHandler *Handler);
	void	Init	();
	void 	Exit	();
	void	Get		(ULevel *Level, const char *Topic, const char *Item, FOutputDevice &Out);
	void	Set		(ULevel *Level, const char *Topic, const char *Item, const char *Value);

	private:
	FTopicHandler *Find (const char *TopicName);
};

/*-------------------------------------------------------------------------------
	Autoregistration macro.
-------------------------------------------------------------------------------*/

//
// Register a topic with Unreal's global topic manager (GTopics).  This
// macro should be used exactly once at global scope for every unique topic
// that must be made available.
//
// The macro creates a bogus global variable and assigns it a value returned from
// the topic class's *Name constructor, which has the effect of
// registering the topic primordially, before main() has been called.
//
#define AUTOREGISTER_TOPIC(NAME,HANDLERCLASS) \
	class HANDLERCLASS : public FTopicHandler \
		{\
		public:\
		HANDLERCLASS(char *Name){GTopics.Register(Name,this);};\
		void Get (ULevel *Level, const char *Item, FOutputDevice &Out);\
		void Set (ULevel *Level, const char *Item, const char *Value);\
		};\
	HANDLERCLASS autoregister##HANDLERCLASS(NAME);

/*-------------------------------------------------------------------------------
	Generic topic handler sample.
-------------------------------------------------------------------------------*/

//
// Generic version of a topic handler for cutting and pasting into code:
//
#if 0
AUTOREGISTER_TOPIC("YourTopicName",YourTopicHandler);
void YourTopicHandler::Get(ULevel *Level, const char *Item, FOutputDevice &Out)
{
	guard(YourTopicHandler::Get);
	unguard;
};
void YourTopicHandler::Set(ULevel *Level, const char *Item, const char *Data)
{
	guard(YourTopicHandler::Set);
	unguard;
};
#endif

/*-------------------------------------------------------------------------------
	The End.
-------------------------------------------------------------------------------*/
#endif // _INC_UNTOPICS
