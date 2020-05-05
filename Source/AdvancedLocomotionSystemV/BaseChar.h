

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
};
