

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gait.h"
#include "CardinalDirection.h"
#include "Stance.h"
#include "RotationMode.h"
#include "MovementMode.h"
#include "ViewMode.h"
#include "BaseChar.generated.h"

UCLASS()
class ADVANCEDLOCOMOTIONSYSTEMV_API ABaseChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseChar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	//Input
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Input")
	bool ShouldSprint;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Input")
	float LookUpDownRate;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Input")
	float LookLeftRightRate;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Input")
	float ForwardAxisValue;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Input")
	float RightAxisValue;

	//Essential
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential", Replicated)
	FVector MovementInput;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential")
	bool IsMoving;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential")
	bool HasMovementInput;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential")
	float AimYawDelta;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential")
	float AimYawRate;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential")
	float MovementInputVelocityDifference;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential")
	float TargetCharacterRotationDifference;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential")
	float Direction;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential", Replicated)
	FRotator CharacterRotation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential", Replicated)
	FRotator LookingRotation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential")
	FRotator LastVelocityRotation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Essential")
	FRotator LastMovementInputRotation;

	//ROTATION SYSTEM
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Rotation System", Replicated)
	FRotator TargetRotation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Rotation System")
	FRotator JumpRotation;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Rotation System", Replicated)
	float RotationOffset;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Rotation System")
	float RotationRateMultiplier;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "State Values")
	TEnumAsByte<ECardinalDirection> CardinalDirection;

	//MOVEMENT SYSTEM
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement System")
	float WalkingSpeed;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement System")
	float RunningSpeed;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement System")
	float SprintingSpeed;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement System")
	float CrouchingSpeed;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement System")
	float WalkingAcceleration;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement System")
	float RunningAcceleration;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement System")
	float WalkingDeceleration;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement System")
	float RunningDeceleration;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement System")
	float WalkingGroundFriction;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement System")
	float RunningGroundFriction;

	//STATE VALUES	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "State Values")
	TEnumAsByte<EGait> Gait;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "State Values")
	TEnumAsByte<EStance> Stance;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "State Values")
	TEnumAsByte<ERotationMode> RotationMode;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "State Values")
	TEnumAsByte<ECharMovementMode> MovementMode;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "State Values")
	TEnumAsByte<ECharMovementMode> PrevMovementMode;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "State Values")
	TEnumAsByte<EViewMode> ViewMode;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "State Values")
	bool IsAiming;

	//BONE NAMES
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Bone Names")
	FName PelvisBone;

	//RAGDOLL SYSTEM
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Ragdoll System")
	FName RagdollPoseSnapshot;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Ragdoll System")
	bool RagdollOnGround;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Ragdoll System", Replicated)
	FVector RagdollLocation;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Ragdoll System")
	FVector RagdollVelocity;


};
