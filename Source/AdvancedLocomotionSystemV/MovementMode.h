
#include "CoreMinimal.h"
#include "MovementMode.generated.h"

UENUM(BlueprintType)
enum ECharMovementMode
{
	None     UMETA(DisplayName = "None"),
	Grounded     UMETA(DisplayName = "Grounded"),
	Falling   UMETA(DisplayName = "Falling"),
	Ragdoll   UMETA(DisplayName = "Ragdoll")
};