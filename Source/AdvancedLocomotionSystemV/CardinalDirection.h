
#include "CoreMinimal.h"
#include "CardinalDirection.generated.h"

UENUM(BlueprintType)
enum ECardinalDirection
{
	North     UMETA(DisplayName = "North"),
	East     UMETA(DisplayName = "East"),
	West   UMETA(DisplayName = "West"),
	South   UMETA(DisplayName = "South")
};