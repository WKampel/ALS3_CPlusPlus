

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "Gait.h"
#include "Stance.h"
#include "MovementMode.h"
#include "Animation/AnimMontage.h"
#include "PivotParams.h"
#include "IdleEntryState.h"
#include "Camera/CameraShake.h"

#include "Locomotion_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULocomotion_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ADVANCEDLOCOMOTIONSYSTEMV_API ILocomotion_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Values")
	void BPI_SetGait(EGait Gait);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Values")
	void BPI_SetStance(EStance Stance);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Values")
	void BPI_SetMovementMode(ECharMovementMode MovementMode);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Values")
	void BPI_SetAiming(bool Aiming);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character Movement Settings")
		void BPI_SetWalkingSpeed(float Speed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character Movement Settings")
		void BPI_SetRunningSpeed(float Speed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character Movement Settings")
		void BPI_SetSprintingSpeed(float Speed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character Movement Settings")
		void BPI_SetCrouchingSpeed(float Speed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Anim Notify")
		void BPI_AnimNotify_Pivot(FPivotParams PivotParams);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Anim Notify")
		void BPI_AnimNotify_TurningInPlace(UAnimMontage* TurnInPlaceMontage, bool ShouldTurnInPlace, bool TurningInPlace, bool TurningRight);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Anim Notify")
		void BPI_AnimNotify_IdleEntryState(EIdleEntryState IdleEntryState);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BPI_Play_GetUp_Anim(bool FaceDown);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BPI_SavePoseSnapshot(FName PoseName);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BPI_AddCharacterRotation(FRotator AddAmount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BPI_ShowTraces(bool ShowTraces);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BPI_CameraShake(TSubclassOf<UCameraShake> ShakeClass, float Scale);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BPI_SetRF(bool RF);
};
