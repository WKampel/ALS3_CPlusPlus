
#include "CoreMinimal.h"
#include "Gait.generated.h"

UENUM(BlueprintType)
enum EGait
{
	Walking     UMETA(DisplayName = "Walking"),
	Running     UMETA(DisplayName = "Running"),
	Sprinting   UMETA(DisplayName = "Sprinting")
};