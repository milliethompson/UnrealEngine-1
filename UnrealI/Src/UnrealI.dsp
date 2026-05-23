# Microsoft Developer Studio Project File - Name="UnrealI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=UnrealI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UnrealI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UnrealI.mak" CFG="UnrealI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UnrealI - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "UnrealI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/Unreal", QBCAAAAA"
# PROP Scc_LocalPath "..\.."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UnrealI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp4 /MD /W4 /WX /vd0 /GX /O2 /Ob2 /I "..\..\Core\Inc" /I "..\..\Engine\Inc" /I "..\Inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /D /Zm256 PACKAGE="\"UnrealI"\" /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\..\Core\Lib\Core.lib ..\..\Engine\Lib\Engine.lib /nologo /base:"0x10700000" /subsystem:windows /dll /incremental:yes /machine:I386

!ELSEIF  "$(CFG)" == "UnrealI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp4 /MDd /W4 /WX /Gm /vd0 /GX /Zi /Od /I "..\..\Core\Inc" /I "..\..\Engine\Inc" /I "..\Inc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /D /Zm256 PACKAGE="\"UnrealI"\" /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\Core\Lib\Core.lib ..\..\Engine\Lib\Engine.lib /nologo /base:"0x10900000" /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "UnrealI - Win32 Release"
# Name "UnrealI - Win32 Debug"
# Begin Group "Classes"

# PROP Default_Filter "*.uc"
# Begin Source File

SOURCE=..\Classes\AlarmPoint.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\AltarTrigger.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Amplifier.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Arc.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Arm1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Armor.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Arrow.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ArrowSpawner.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\AsbestosSuit.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ASMD.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ASMDAmmo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\AutoMag.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BabyCow.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BallExplosion.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Bandages.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Barrel.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BarrelSludge.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Behemoth.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\bigbiogel.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\bigblacksmoke.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BigRock.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\biodrop.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\biogel.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\bird1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BiterFish.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BiterFishSchool.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Bloblet.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Blood2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BloodBurst.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BloodPool.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BloodPuff.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BloodSpurt.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BloodTrail.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BlueBook.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Book.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BotInfo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Bots.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Boulder.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Boulder1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BreakingGlass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Brute.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\brutecarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BruteProjectile.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Bubble.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Bubble1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BubbleGenerator.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\BulletHit.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Burned.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Candle.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Candle2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Cannon.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\CannonBolt.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\CaveManta.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Chair.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Chest.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Chip.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Chunk.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Chunk1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Chunk2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Chunk3.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Chunk4.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Clip.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\CloudZone.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\CoopGame.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Corroded.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Cow.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\cowcarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\CreatureCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\CreatureChunks.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\CreatureFactory.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\crucifiednali.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Dampener.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DarkMatch.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DeadBodySwarm.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DeadMales.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DeathMatchGame.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Decapitated.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DefaultAmmo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DEProj.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\devilfish.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\devilfishcarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Dice.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DispersionAmmo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DispersionPistol.uc
# End Source File
# Begin Source File

SOURCE=.\DisplayRotate.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DKmaplist.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DMmaplist.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Drip.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DripGenerator.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Drowned.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\DynamicAmbientSound.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Earthquake.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Eightball.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Electricity.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\EliteKrallBolt.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\EndGame.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\EnergyBolt.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\EntryGameInfo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ExplodingWall.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ExplosionChain.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Fan2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FatRing.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FearSpot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Fell.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Female.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Female2Body.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FemaleBody.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FemaleBot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FemaleOne.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FemaleOneBot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FemaleOneCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FemaleTorso.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FemaleTwo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FemaleTwoBot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FemaleTwoCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FireSlith.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Flag1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Flag2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Flag3.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FlakBox.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FlakCannon.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FlakShell.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FlameBall.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FlameExplosion.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Flare.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Flashlight.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FlashLightBeam.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FlockMasterPawn.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FlockPawn.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Fly.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\FlyCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ForceField.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ForceFieldProj.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Gasbag.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GasBagBelch.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GassiusCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GESBioRifle.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GiantGasbag.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GiantManta.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GlassFragments.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GradualMover.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GreenBlob.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GreenBloodPuff.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GreenBook.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GreenSmokePuff.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Grenade.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\GuardPoint.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Health.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Heart.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\HeavyWallHitEffect.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\HorseFly.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\HorseFlySwarm.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Human.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\HumanBot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\HumanCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\IceSkaarj.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ImpalerBolt.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\InfoMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\InterpolatingObject.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\IntroNullHud.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\invisibility.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\JumpBoots.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Jumper.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\KevlarSuit.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\KingOfTheHill.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\KraalBolt.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Krall.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\KrallCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\KrallElite.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Lamp1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Lamp4.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\LavaZone.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Leg1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Leg2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\LeglessKrall.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\LesserBrute.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\LesserBruteCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\LightWallHitEffect.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Liver.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Magma.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MagmaBurst.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Male.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleBody.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleBodyThree.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleBodyTwo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleBot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleOne.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleOneBot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleOneCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleThree.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleThreeBot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleThreeCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleTwo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleTwoBot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MaleTwoCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Manta.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MantaCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MasterChunk.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MedWoodBox.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MercCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Mercenary.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MercenaryElite.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MercFlare.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MercRocket.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Minigun.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MiniGunSentry.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\MonkStatue.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Moon.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Moon2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Moon3.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Nali.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\NaliCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\NaliFruit.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\NaliPriest.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\NaliRabbit.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\NaliStatue.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\NitrogenSlith.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\NitrogenZone.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\NullAmmo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\OKMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\parentBlob.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ParticleBurst.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ParticleBurst2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\PawnTeleportEffect.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\PeaceRocket.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\PHeart.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Plant1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Plant2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Plant3.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\PlayerChunks.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\PowerShield.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Pupae.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\PupaeCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\QuadShot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Queen.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\QueenCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\QueenDest.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\QueenShield.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\QueenTeleportEffect.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\RazorAmmo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\RazorBlade.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\RazorJack.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ReSpawn.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Rifle.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\RifleRound.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\RingExplosion.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\RingExplosion2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\RingExplosion3.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\RingExplosion4.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\rocket.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\RocketCan.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\RockSlide.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Sconce.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ScriptedPawn.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SCUBAGear.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Seeds.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SeekingRocket.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Shellbox.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ShellCase.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Shells.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Shield.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ShieldBelt.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ShortSmokeGen.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SightLight.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Sign1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SinglePlayer.Uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Skaarj.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\skaarjassassin.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjBerserker.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjGunner.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjInfantry.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjLord.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjOfficer.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\skaarjplayer.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjPlayerBot.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjProjectile.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\skaarjscout.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjSniper.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjTrooper.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SkaarjWarrior.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SlimeZone.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Slith.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SlithCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SlithProjectile.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SlotMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Sludge.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SludgeBarrel.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SmallSpark.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SmallSpark2.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SmallSteelBox.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SmallWire.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SmallWoodBox.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SmokeGenerator.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SmokePuff.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SmokeTrail.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SparkBit.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Sparks.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Spawnpoint.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\spectatorhud.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\splash.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SpriteBallExplosion.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SpriteExplosion.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SpriteSmokePuff.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Squid.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SquidCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\StealthCloak.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SteelBarrel.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SteelBox.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Stinger.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\StingerAmmo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\StingerProjectile.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\StochasticTrigger.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Stomach.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\StoneTitan.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Suits.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\SuperHealth.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Table.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Tapestry1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TarydiumBarrel.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TarZone.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TazerBeam.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Tazerexplosion.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TazerProj.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TeamGame.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TeamInfo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TeleportEffect.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TeleporterZone.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TeleportProj.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Tentacle.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TentacleCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TentacleProjectile.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Thigh.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ThingFactory.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\thrownbody.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ThrowStuff.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TinyBurst.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Titan.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TitanCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TorchFlame.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\ToxinSuit.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Tracer.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Translator.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TranslatorEvent.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Tree1.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TriggerLight.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\troopercarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\TSmoke.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealBotConfigMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealChooseGameMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealDamageType.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealDMGameOptionsMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealFavoritesMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealGameInfo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealGameMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealGameOptionsMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealHelpMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealHUD.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealIndivBotMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealIPlayer.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealJoinGameMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealKeyboardMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealListenMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealLoadMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealMainMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealMeshMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealMultiPlayerMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealNewGameMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealOptionsMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealPlayerMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealQuitMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealSaveMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealScoreBoard.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealServerMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\unrealslotmenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealSpectator.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealTeamGameOptionsMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealTestInfo.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealVideoMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\UnrealWeaponMenu.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Urn.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Vase.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\VoiceBox.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\VRikersGame.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WallFragments.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WallHitEffect.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Warlord.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WarlordCarcass.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WarlordRocket.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WaterImpact.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WaterRing.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WaterZone.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WeaponLight.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WeaponPowerUp.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\Wire.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WoodenBox.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\WoodFragments.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\YellowBook.uc
# End Source File
# Begin Source File

SOURCE=..\Classes\YesNoMenu.uc
# End Source File
# End Group
# End Target
# End Project
