
#include "CoreMinimal.h"
#include "MovementDirection.generated.h"

UENUM(BlueprintType)
enum EMovementDirection
{
	Forwards     UMETA(DisplayName = "Forwards"),
	Backwards     UMETA(DisplayName = "Backwards")
};