

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gait.h"
#include "CardinalDirection.h"
#include "Stance.h"
#include "MovementMode.h"
#include "Camera/CameraComponent.h"
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

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void Landed(const FHitResult& Hit) override;

public:


	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

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
	float PrevAimYaw;

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
	TEnumAsByte<ECharMovementMode> MovementMode;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "State Values")
	TEnumAsByte<ECharMovementMode> PrevMovementMode;


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

	//DEBUG
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Debug")
	bool ShowTraces;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Debug")
	bool ShowSettings;

	//CAMERA SYSTEM
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Camera System")
	bool RightShoulder;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Camera System")
	UCurveFloat* CameraLerpCurve;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Camera System")
	FName FirstPersonCameraSocket;

public:
	UFUNCTION(BlueprintCallable)
	FVector ChooseVelocity();

	UFUNCTION(BlueprintCallable)
	void CalculateEssentialVariables();

	UFUNCTION(BlueprintCallable)
	void PlayerMovementInput(bool IsForwardAxis);

	UFUNCTION(BlueprintCallable)
	FVector GetForwardVector();

	UFUNCTION(BlueprintCallable)
	FRotator CalculateActorRotationInRagdoll(FRotator _RagdollRotation);

	UFUNCTION(BlueprintCallable)
	FVector CalculateActorLocationInRagdoll(FVector _RagdollLocation);

	UFUNCTION(BlueprintCallable)
		void SetCharacterRotation(FRotator _TargetRotation, bool _InterpRotation, float _InterpSpeed);

	UFUNCTION(BlueprintCallable)
	void AddCharacterRotation(FRotator _AddAmount);

	UFUNCTION(BlueprintCallable)
	void LimitRotation(float _AimYawLimit, float _InterpSpeed);

	UFUNCTION(BlueprintCallable)
	float CalculateRotationRate(float _SlowSpeed, float _SlowSpeedRate, float _FastSpeed, float _FastSpeedRate);

	UFUNCTION(BlueprintCallable)
	bool CardinalDirectionAngles(float _Value, float _Min, float _Max, float _Buffer, ECardinalDirection _CardinalDirection);

	UFUNCTION(BlueprintCallable)
	FRotator LookingDirectionWithOffset(float _OffsetInterpSpeed, float _NEAngle, float _NWAngle, float _SEAngle, float _SWAngle, float _Buffer);

	UFUNCTION(BlueprintCallable)
	bool CanSprint();

	UFUNCTION(BlueprintCallable)
	void CustomAcceleration();

	UFUNCTION(BlueprintCallable)
	float ChooseGroundFriction();

	UFUNCTION(BlueprintCallable)
	float ChooseMaxAcceleration();

	UFUNCTION(BlueprintCallable)
		float ChooseBrakingDeceleration();

	UFUNCTION(BlueprintCallable)
		float ChooseMaxWalkSpeed();

	UFUNCTION(BlueprintCallable)
	void UpdateCharacterMovementSettings();

	//Setters
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetMovementMode(TEnumAsByte<ECharMovementMode> _NewMovementMode);

	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetGait(TEnumAsByte<EGait> _NewGait);

	UFUNCTION(BlueprintCallable, Category = "Setters")
		void SetStance(TEnumAsByte<EStance> _NewStance);

	UFUNCTION(BlueprintCallable, Category = "Setters")
		void SetAiming(bool _NewAiming);

private:
	UFUNCTION(Server, Unreliable)
	void SR_SetMovementInput(FVector _MovementInput);

	UFUNCTION(Server, Unreliable)
	void SR_SetLookingRotation(FRotator _LookingRotation);

	UFUNCTION(Server, Unreliable)
	void SR_SetCharacterRotation(FRotator _TargetRotation, FRotator _CharacterRotation);

	UFUNCTION(Server, Unreliable)
		void SR_SetGait(const EGait _NewGait);

	UFUNCTION(NetMulticast, Unreliable)
		void MC_SetGait(const EGait _NewGait);

	UFUNCTION(Server, Unreliable)
		void SR_SetAiming(bool _NewAiming);

	UFUNCTION(NetMulticast, Unreliable)
		void MC_SetAiming(bool _NewAiming);

	FVector GetRightVector();

	void OnMovementModeChanged(EMovementMode PrevMovementMode,  uint8 PrevCustomMode) override;



};
