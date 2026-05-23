/*=============================================================================
	UnTask.cpp: Unreal task manager for Windpws

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#include "StdAfx.h"
#include "UnWn.h"
#include "Unreal.h"
#include "UnWnCam.h"

extern FWindowsCameraManager CameraManager;

/*-----------------------------------------------------------------------------
	Globals.
-----------------------------------------------------------------------------*/

//
// Unreal task manager for windows.  Manages all tasks.  Tasks are
// updated via their Tick routines in CUnrealWnApp.MessagePump().
//
class FWindowsTaskManager : public FTaskManagerBase
{
public:

	// FTaskManagerBase interface.
	virtual void Init();
	virtual void Exit();
	virtual int Exec(const char *Cmd,FOutputDevice *Out=GApp);

	virtual FTask *GetTaskByID(int TaskID);
	virtual int AddTask(FTask *Task,FTask *Owner,FOutputDevice *Out,ETaskPriority Priority,int Flags);
	virtual void KillTask(FTask *Task);
	virtual void KillTasksUsing(FOutputDevice *Out);

	// Private implementation.
	int TopTask,Initialized;
	enum {MAX_TASKS=64};
	FTask *Tasks[MAX_TASKS];

	FWindowsTaskManager() {Initialized=0;};
	void AssertInitialized() {if (!Initialized) appError("Not initialized");};
};

FWindowsTaskManager WindowsTaskManager;
FTaskManagerBase *TaskManager = &WindowsTaskManager;

/*-----------------------------------------------------------------------------
	Init & Exit.
-----------------------------------------------------------------------------*/

//
// Initialize the task manager.
//
void FWindowsTaskManager::Init()
{
	guard(FWindowsTaskManager::Init);

	if( Initialized )
		appError("Already initialized");

	for (int i=0; i<MAX_TASKS; i++)
		Tasks[i]=NULL;
	
	Initialized=1;
	TopTask=1;

	unguard;
}

//
// Shut down the task manager.  Does not kill any yet-unkilled tasks.
// You are responsible for killing all tasks before shutting down the
// task manager.
//
void FWindowsTaskManager::Exit()
{
	guard(FWindowsTaskManager::Exit);
	AssertInitialized();

	for( int i=0; i<MAX_TASKS; i++ )
	{
		if( Tasks[i] )
		{
			char Name[256],Descr[256];
			debugf(LOG_Exit,"Task is still running: %s",Tasks[i]->TaskStatus(Name,Descr));
		}
	}
	Initialized=0;

	unguard;
}

/*-----------------------------------------------------------------------------
	Command line.
-----------------------------------------------------------------------------*/

//
// Execute a command line.
//
int FWindowsTaskManager::Exec(const char *Cmd,FOutputDevice *Out)
{
	guard(FWindowsTaskManager::Exec);

	const char *Str = Cmd;
	AssertInitialized();

	if( GetCMD(&Str,"TASKS") )
	{
		Out->Log("Unreal tasks:");
		for( int i=0; i<MAX_TASKS; i++ )
		{
			if( Tasks[i] )
			{
				char Name[256],Descr[256];
				Tasks[i]->TaskStatus(Name,Descr);
				Out->Logf(" %i. %s (%s) %3.1f ms",Tasks[i]->TaskID,Name,Descr[0] ? Descr : "Ok",GApp->CpuToMilliseconds(Tasks[i]->TaskCpuTime));
			}
		}
		return 1;
	}
	else if( GetCMD(&Str,"KILL") )
	{
		int TaskID = atoi(Str);
		if( TaskID )
		{
			FTask *Task = GetTaskByID(TaskID);
			if( Task )
			{
				if( Task->TaskFlags & TASK_NoUserKill )
				{
					Out->Log("Task is not killable");
				}
				else
				{
					char Name[256],Descr[256];
					Out->Logf("Killing task: %s",Task->TaskStatus(Name,Descr));
					KillTask(Task);
				}
			}
		}
		else Out->Log("Missing or invalid task number");
		return 1;
	}
	else return 0;

	return 0;
	unguard;
}

/*-----------------------------------------------------------------------------
	Task management.
-----------------------------------------------------------------------------*/

//
// Lookup a task by its ID and return a pointer to the task object.
// Returns 0 if the task ID doesn't correspond to a running task.
//
FTask *FWindowsTaskManager::GetTaskByID(int TaskID)
{
	guard(FWindowsTaskManager::GetTaskByID);
	AssertInitialized();

	for( int i=0; i<MAX_TASKS; i++ )
	{
		if (Tasks[i] && (Tasks[i]->TaskID==TaskID))
			return Tasks[i];
	}
	return NULL; // Not found

	unguard;
}

//
// Add a newly-created task to the task list.  Assumes that you have already
// initialized the task and that it's ready to receive Tick() calls.
// This doesn't directly call any task functions.  Returns task ID, or 0 if 
// task couldn't be created, which happens only if there are more than 
// MAX_TASKS tasks active.
//
int FWindowsTaskManager::AddTask(FTask *Task,FTask *Owner,FOutputDevice *Out,ETaskPriority Priority,int Flags)
{
	guard(FWindowsTaskManager::AddTask);
	AssertInitialized();

	int TaskID = TopTask++;

	for (int i=0; i<MAX_TASKS; i++) 
		if (!Tasks[i]) 
			break;
	if (i>=MAX_TASKS) 
		return 0; 

	Tasks[i]=Task;
	Task->Owner			= Owner;
	Task->Out			= Out;
	Task->Priority		= Priority;
	Task->TaskFlags		= Flags;
	Task->TaskID		= TaskID;
	Task->LastTickTime	= App.Platform.MicrosecondTime();

	char Name[256],Descr[256];
	debugf(LOG_Info,"Created task: %s",Task->TaskStatus(Name,Descr));
	return TaskID;

	unguard;
}

//
// Kill a task.  Calls the task's TaskExit() function.  This may be
// called at any time, such as if someone kills the task manually
// at a console, so the task's TaskExit() function must be able
// to perform all necessary cleanup.
//
void FWindowsTaskManager::KillTask(FTask *Task)
{
	guard(FWindowsTaskManager::KillTask);
	AssertInitialized();

	char Name[256],Descr[256];
	debugf(LOG_Info,"Destroyed task: %s",Task->TaskStatus(Name,Descr));

	for( int i=0; i<MAX_TASKS; i++ )
	{
		if( Tasks[i]==Task )
		{
			Task->TaskExit();
			Tasks[i]=NULL;
			return;
		}
	}
	appError("Task not found");
	unguard;
}

void FWindowsTaskManager::KillTasksUsing(FOutputDevice *Out)
{
	guard(FWindowsTaskManager::KillTasksUsing);
	AssertInitialized();
	for( int i=0; i<MAX_TASKS; i++ )
	{
		if (Tasks[i] && (Tasks[i]->Out==Out)) 
			KillTask(Tasks[i]);
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	Windows Message Pump.
-----------------------------------------------------------------------------*/

//
// Unreal's main message pump.  All windows in Unreal receive messages
// somewhere below this function on the stack.
//
void CUnrealWnApp::MessagePump()
{
	guard(CUnrealWnApp::MessagePump);
	MSG Msg;

	for( ; ; )
	{
		// Handle all incoming messages.
		while( PeekMessage( &Msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if( Msg.message == WM_QUIT )
				return;

			RouteMessage(&Msg);
			if( Msg.message == WM_MOUSEMOVE ) 
				break;
		}

		// Update all stuff.
		for( int i=0; i<WindowsTaskManager.MAX_TASKS; i++ )
		{
			FTask *Task = WindowsTaskManager.Tasks[i];
			if( Task && (Task->Priority==PRIORITY_Realtime || Task->Priority==PRIORITY_Camera) )
			{
				QWORD NewTime = App.Platform.MicrosecondTime();
				Task->TaskCpuTime = 0;
				clock(Task->TaskCpuTime);
				Task->TaskTick((FLOAT)(SQWORD)(NewTime - Task->LastTickTime) / 1000000.0);
				unclock(Task->TaskCpuTime);
				Task->LastTickTime = NewTime;
			}
		}
		GObj.Tick();				
		GAudio.Tick();
	}
	unguard;
}

/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/
