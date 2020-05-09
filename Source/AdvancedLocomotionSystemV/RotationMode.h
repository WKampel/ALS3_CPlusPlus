
#include "CoreMinimal.h"
#include "RotationMode.generated.h"

UENUM(BlueprintType)
enum ERotationMode
{
	VelocityDirection     UMETA(DisplayName = "VelocityDirection"),
	LookingDirection     UMETA(DisplayName = "LookingDirection")
};