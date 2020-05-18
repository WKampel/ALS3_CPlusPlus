
#pragma once

#include "Engine.h"
#include "CoreMinimal.h"
#include "MovementDirection.h"
#include "PivotParams.generated.h"

USTRUCT(BlueprintType)
struct FPivotParams
{
    GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PivotDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EMovementDirection> CompletedMovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CompletedStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EMovementDirection> InterruptedMovementDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InterruptedStartTime;

};