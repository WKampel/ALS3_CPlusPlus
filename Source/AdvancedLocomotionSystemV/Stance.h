#pragma once

#include "CoreMinimal.h"
#include "Stance.generated.h"

UENUM(BlueprintType)
enum EStance
{
	Standing     UMETA(DisplayName = "Standing"),
	Crouching     UMETA(DisplayName = "Crouching")
};