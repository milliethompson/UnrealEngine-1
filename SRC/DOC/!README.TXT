/*=============================================================================
	!ReadMe.txt: Unreal source distribution notes
	By: Tim Sweeney, Epic MegaGames, Inc.
=============================================================================*/

This is a confidential, internal source code distribution of Unreal, and is
a trade secret of Epic MegaGames, Inc.  It is intended only for people who have
signed the appropriate non-disclosure agreements.

--------------------------------------
For the latest development information
--------------------------------------

Visit http://www.epicgames.com/UnrealDev
Please read all of the latest info there before asking a question.
Check out the list of contacts there to find the approprite person to question.

------------
This package
------------

Version:		Unreal 0.86.  See UnBuild.h for detailed version information.
Included:		Server, Engine, Render, Game, Networking, Launcher, Editor.

--------------------
Development Platform
--------------------

Unreal source, Windows 95/NT, requires:
   * Visual C++ 5.0 (also works with 4.0, 4.1, or 4.2 if you create a new .mak).
   * MASM 6.11d (from \Unreal\Src\Tools), ML.EXE must be in path

UnrealEd, Windows 95/NT, requires:
   * Visual Basic Professional or Enterprise 5.0

--------------------
Install instructions
--------------------

1. Install the corresponding game/demo version of Unreal into \Unreal.
   Run Unreal.exe and verify that it works properly.

2. Optionally install UnrealEd and any auxillary distributions into \Unreal.

3. Install this source distribution into \Unreal.
   To verify that it works, delete the .exe and .dll files in \Unreal and \Unreal\System,
   then build all of the source files here and run Unreal.exe again.

-------------------
Documentation Index
-------------------

	File			Contents
	--------------- -------------------------------------------------------
	!ReadMe.txt		This index
	ProjMan.txt		Important source code guidelines to be aware of
	T3d.txt			Unreal brush .T3D file format docs
	TimLog.txt		Tim Sweeney work log
	TimNotes.txt	Tim Sweeney misc notes (confidential)
	TimOld.txt		Tim Sweeney old work logs

----------------
Unreal.mak Index
----------------

	Project		   Description             Target			            Compatibility
	-------------  ----------------------  ---------------------------  ----------------------------
	Documentation  Documentation files	   (none)                       (none)
	Editor		   UnrealEd support files  \Unreal\System\UnEditor.dll	Ansi C++
	Engine		   All engine source code  \Unreal\System\UnEngine.dll	Ansi C++
	Game		   Game & AI routines      \Unreal\System\UnGame.dll	Ansi C++
	Network		   All network code		   \Unreal\System\UnNet.dll     Ansi C++ & Windows API
	Render		   Rendering subsystem	   \Unreal\System\UnRender.dll	Ansi C++
	Windows		   Windows-specific code   \Unreal\System\UnServer.exe	Ansi C++ & Windows API & MFC

---------------
Utils.mak Index
---------------

	Project		   Description             Target			            Compatibility
	-------------  ----------------------  ---------------------------  --------------------
	Launcher	   Unreal launcher stub	   \Unreal\Unreal.exe           Ansi C & Windows API

---------------
Directory Index
---------------

Assuming you installed into the \Unreal\ directory, here are the 
subdirectories you'll find the Unreal source code in:

From the Unreal source distribution:

Unreal\					Root Unreal directory
	Classes\			Actor class descriptions
		*.u				Actor class scripts
		Classes.mac		UnrealEd macro to import all actor classes
	Doc\				Documentation files directory
		!ReadMe.txt		This index file
	EdSrc\				Root UnrealEd source code directory
		UnrealEd.vbp	Visual Basic 4.0 project file for UnrealEd.exe
	GateSrc\            Gatekeeper source code
	    Client			Gatekeeper client and user interface
		Common			Gatekeeper common files
		Server			Gatekeeper server
	Src\				Root Unreal source code directory
		Game\			Game code for UnActors.dll
			Unreal.mak	Visual C++ project file for Unreal
		Engine\			Game engine (client & server) for UnEngine.dll
			Unreal.cpp	Main startup file
		Inc\			Include files directory
			Unreal.h	Main Unreal include
		Network\		Windows networking code
		Render\			Rendering code
		Editor\			Unreal editor support files
		Windows\		All windows-specific code for Unreal
		Launcher\		Unreal launcher stub
		Listing\		VC++ generated assembly listing files

From auxillary Unreal distributions:

	\Unreal\Graphics\			Ancillary game and editor graphics, compiled to \Unreal\System\Unreal.gfx
		Graphics.mac			UnrealEd macro that imports all graphics resources

	\Unreal\Models\				All 3D models and their associated texture maps referenced by scripts.

----------
File types
----------

Industry standard filetypes supported by Unreal:

	*.pcx: 256-color PCX graphics files.
	*.bmp: 256-color Windows bitmap.
	*.wav: 8-bit and 16-bit mono audio samples.
	*.asc: 3D Studio Release 4 exported brushes.
	*.dxf: Standard AutoCad model format (only certain varieties are supported).

Text files native to Unreal:

	*.t3d: Level files (exported levels) and brush files (exported brushes).
	*.u:   Unreal class definitions (exported actor classes).

Binary files native to Unreal:

	*.unr: An Unreal level file, containing everything the level needs.
	*.ucx: An Unreal class file, containing actor class definitions.
	*.utx: An Unreal texture family.

	These files are all stored in the Unreal resource file format.
	This format is a delinked dump of Unreal's internal structures and
	the format changes every time the internal formats change.

-----------------------
VC++ Build instructions
-----------------------

You need to build the Unreal files in this order due to dependencies:

1. Build "Engine Files".
2. Build "Render Files".
3. Build "Editor Files", "Network Files" and "Game Files".
4. Build "Windows Files".

----------------------
Project configurations
----------------------

Release: Optimized version of Unreal for general development use.

Debug: Debug version of Unreal, for use in the VC++ debugger.  Note that the debug version is
too damn slow to be usable for non-debugging purposes.

We hardly ever use the debugger while working on Unreal.  The guard/unguard mechanism we use
shows the calling history whenever a crash or critical error occurs, so we track down errors
from the release version.

-----------------
Completion status
-----------------

This is very much a work in progress.
See TimLog.txt and TimOld.txt for a day-by-day account of the details.

------------
Legal notice
------------

	Copyright 1996 Epic MegaGames, Inc. This software is a trade secret, and
	all distribution is prohibited.

	All source code, programming techniques, content, and algorithms incorporated
	herein are trade secrets of Epic MegaGames, Inc., including, but not limited to
	the following:

    * Curved surface rendering techniques.
	* All specific texture mapping concept and algorithms
	* All texture and light filtering/dithering concepts and algoritms
	* Lattice-based rendering concept
	* Lattice light generation concept and algorithm
	* guard/unguard call history error logging facility
	* UnrealEd interactive constructive solid geometry paradigm and algorithms
	* Realtime BSP maintenance algorithm
	* Zone and portal based occlusion algorithms
	* Unreal resource manager object-oriented load/save/link/delink algorithms

Enjoy.

-Tim
