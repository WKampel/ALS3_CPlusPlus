
#include "CoreMinimal.h"
#include "ViewMode.generated.h"

UENUM(BlueprintType)
enum EViewMode
{
	FirstPerson     UMETA(DisplayName = "FirstPerson"),
	ThirdPerson     UMETA(DisplayName = "ThirdPerson")
};