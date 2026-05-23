//=============================================================================
// DAmmo4.
//=============================================================================
class DAmmo4 expands DispersionAmmo;

#exec MESH IMPORT MESH=DispM3 ANIVFILE=MODELS\cros_t_a.3D DATAFILE=MODELS\cros_t_d.3D X=0 Y=0 Z=0 
#exec MESH ORIGIN MESH=DispM3 X=0 Y=-500 Z=0 YAW=-64
#exec MESH SEQUENCE MESH=DispM3 SEQ=All STARTFRAME=0  NUMFRAMES=1
#exec MESH SEQUENCE MESH=DispM3 SEQ=Still  STARTFRAME=0 NUMFRAMES=1
#exec MESHMAP SCALE MESHMAP=DispM3 X=0.09 Y=0.15 Z=0.08
#exec  OBJ LOAD FILE=Textures\fireeffect1.utx PACKAGE=UNREALI.Effect1
#exec MESHMAP SETTEXTURE MESHMAP=DispM3 NUM=0 TEXTURE=Unreali.Effect1.FireEffect1pb
#exec MESHMAP SETTEXTURE MESHMAP=DispM3 NUM=1 TEXTURE=Unreali.Effect1.FireEffect1o

defaultproperties
{
     ParticleType=Class'UnrealI.Spark34'
     SparkModifier=2.500000
     ExpType=Class'UnrealI.SpriteOrangeE'
     Damage=55.000000
     Mesh=Mesh'UnrealI.DispM3'
     LightBrightness=170
     LightHue=30
}
