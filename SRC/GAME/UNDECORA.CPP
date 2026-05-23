/*
==============================================================================
UnDecora.cpp: Unreal decoration actors

Copyright 1996 Epic MegaGames, Inc. This software is a trade secret.
Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

Revision history:
    * 08/12/96: Created by Mark
==============================================================================
*/

#include "UnGame.h"
#include "UnFActor.h"
#include "UnInput.h"

//----------------------------------------------------------------------------
//                    Decoration processing
//----------------------------------------------------------------------------
int ADecoration::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    FActor & Actor = FActor::Actor(*this);
    return ProcessParent; 
    UNGUARD( "ADecoration::Process" );
}

#if 0 //todo: Delete
//----------------------------------------------------------------------------
//                    Vase processing
//----------------------------------------------------------------------------
int AVase::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    return ProcessParent; 
    UNGUARD( "AVase::Process" );
}

//----------------------------------------------------------------------------
//                    Chandelier processing
//----------------------------------------------------------------------------
int AChandelier::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    return ProcessParent; 
    UNGUARD( "AChandelier::Process" );
}

//----------------------------------------------------------------------------
//                    Hammok processing
//----------------------------------------------------------------------------
int AHammok::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    return ProcessParent; 
    UNGUARD( "AHammok::Process" );
}
#endif

#if 1 //todo: Add
//----------------------------------------------------------------------------
//                    Flame processing
//----------------------------------------------------------------------------
int ATorchFlame::Process(ILevel *Level, FName Message, void *Params)
{
    GUARD;
    switch( Message.Index )
    {
        case ACTOR_Tick:
        {
            static int dYaw = 180;
            DrawRot.Yaw += dYaw;
            break;
        }
    }   
    return ProcessParent; 
    UNGUARD( "AFlame::Process" );
}
#endif