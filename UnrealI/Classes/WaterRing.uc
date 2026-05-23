//=============================================================================
// WaterRing.
//=============================================================================
class WaterRing expands RingExplosion;

#exec OBJ LOAD FILE=Textures\fireeffect56.utx  PACKAGE=UNREALI.Effect56

simulated function SpawnEffects()
{
}

defaultproperties
{
     Skin=UnrealI.Effect56.fireeffect56
     Class=UnrealI.WaterRing
	 bNetOptional=True
}
