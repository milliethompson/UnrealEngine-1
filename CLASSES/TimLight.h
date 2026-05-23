/*===========================================================================
	C++ "TimLight" actor class definitions exported from UnrealEd
===========================================================================*/
#pragma pack (push,4) /* 4-byte alignment */

///////////////////////////////////////////////////////
// Actor class ATimLight:ALight:AActor
///////////////////////////////////////////////////////

class IMPLEMENTATION_API ATimLightBase : public ALightBase {
public:
};
class IMPLEMENTATION_API ATimLight : public ATimLightBase {
public:
    BYTE PropertyPad[1024];
    FActorPrivate Internal;
};

#pragma pack (pop) /* Restore alignment to previous setting */
