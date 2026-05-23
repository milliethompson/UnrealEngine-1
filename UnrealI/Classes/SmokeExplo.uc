//=============================================================================
// SmokeExplo.
//=============================================================================
class SmokeExplo expands AnimSpriteEffect;

#exec TEXTURE IMPORT NAME=SmokeE1 FILE=MODELS\f201.pcx GROUP=Effects
#exec TEXTURE IMPORT NAME=SmokeE2 FILE=MODELS\f202.pcx GROUP=Effects
#exec TEXTURE IMPORT NAME=SmokeE3 FILE=MODELS\f203.pcx GROUP=Effects
#exec TEXTURE IMPORT NAME=SmokeE4 FILE=MODELS\f204.pcx GROUP=Effects
#exec TEXTURE IMPORT NAME=SmokeE5 FILE=MODELS\f205.pcx GROUP=Effects
#exec TEXTURE IMPORT NAME=SmokeE6 FILE=MODELS\f206.pcx GROUP=Effects
#exec TEXTURE IMPORT NAME=SmokeE7 FILE=MODELS\f207.pcx GROUP=Effects
#exec TEXTURE IMPORT NAME=SmokeE8 FILE=MODELS\f208.pcx GROUP=Effects
#exec TEXTURE IMPORT NAME=SmokeE9 FILE=MODELS\f209.pcx GROUP=Effects

defaultproperties
{
     SpriteAnim(0)=UnrealI.SmokeE1
     SpriteAnim(1)=UnrealI.SmokeE2
     SpriteAnim(2)=UnrealI.SmokeE3
     SpriteAnim(3)=UnrealI.SmokeE4
     SpriteAnim(4)=UnrealI.SmokeE5
     SpriteAnim(5)=UnrealI.SmokeE6
     SpriteAnim(6)=UnrealI.SmokeE7
     SpriteAnim(7)=UnrealI.SmokeE8
     SpriteAnim(8)=UnrealI.SmokeE9
     NumFrames=9
     Pause=+00000.050000
     DrawScale=+00000.200000
     LightType=LT_None
     LightBrightness=68
     LightHue=0
     LightSaturation=255
     LightRadius=3
     LifeSpan=+00001.000000
}
