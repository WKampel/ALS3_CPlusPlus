

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gait.h"
#include "CardinalDirection.h"
#include "Stance.h"
#include "MovementMode.h"
#include "Camera/CameraComponent.h"
#include "Locomotion_Interface.h"
#include "BaseChar.generated.h"

UCLASS()
class ADVANCEDLOCOMOTIONSYSTEMV_API ABaseChar : public ACharacter, public ILocomotion_Interface
{
	GENERATED_BODY()

public:
	ABaseChar();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void TickGrounded(float DeltaTime);

	void TickManageCharacterRotation(float DeltaTime);

private:
	/////////////////////////////////////////////////////////////////
	//
	//                    STATE VARIABLES
	//
	/////////////////////////////////////////////////////////////////

	bool RagdollOnGround;

	float PrevAimYaw;

	float RotationRateMultiplier;

	float LookUpDownRate;

	float LookLeftRightRate;

	bool ShouldSprint;

	float ForwardAxisValue;

	float RightAxisValue;

	UPROPERTY(Replicated)
	float RotationOffset;

	TEnumAsByte<ECharMovementMode> PrevMovementMode;

	TEnumAsByte<ECardinalDirection> CardinalDirection;

	FVector RagdollVelocity;

	UPROPERTY(Replicated)
	FVector RagdollLocation;

	UPROPERTY(Replicated)
	FVector MovementInput;

	FRotator JumpRotation;

	UPROPERTY(Replicated)
	FRotator TargetRotation;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HasMovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float MovementInputVelocityDifference;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float TargetCharacterRotationDifference;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float AimYawDelta;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float AimYawRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator LastVelocityRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator LastMovementInputRotation;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	FRotator LookingRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	FRotator CharacterRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TEnumAsByte<EGait> Gait;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TEnumAsByte<EStance> Stance;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECharMovementMode> MovementMode;

private:
	/////////////////////////////////////////////////////////////////
	//
	//                    CUSTOMIZABLE VARIABLES
	//
	/////////////////////////////////////////////////////////////////

	float WalkingAcceleration;

	float RunningAcceleration;

	float WalkingDeceleration;

	float RunningDeceleration;

	float WalkingGroundFriction;

	float RunningGroundFriction;

	FName FirstPersonCameraSocket;

	FName RagdollPoseSnapshot;

	FName PelvisBone;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float WalkingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float RunningSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float SprintingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CrouchingSpeed;

private:
	/////////////////////////////////////////////////////////////////
	//
	//                    NATIVE EVENT HANDLING FUNCTIONS
	//
	/////////////////////////////////////////////////////////////////
	
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	
	virtual void Landed(const FHitResult& Hit) override;
	
	void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode) override;

private:
	/////////////////////////////////////////////////////////////////
	//
	//                    PLAYER INPUT HANDLING FUNCTIONS
	//
	/////////////////////////////////////////////////////////////////

	void ToggleStance();

	void MoveForwardsBackwards(float _AxisValue);

	void MoveRightLeft(float _AxisValue);

	void LookUpDown(float _AxisValue);
	
	void LookLeftRight(float _AxisValue);

	void Pressed_JumpAction();
	
	void Released_JumpAction();

	void Pressed_AimAction();
	
	void Released_AimAction();

	void Pressed_SprintAction();

	void Released_SprintAction();

	void Pressed_RagdollAction();


public:
	/////////////////////////////////////////////////////////////////
	//
	//                    SETTER FUNCTIONS
	//
	/////////////////////////////////////////////////////////////////

	void SetMovementMode(TEnumAsByte<ECharMovementMode> _NewMovementMode);

	void SetGait(TEnumAsByte<EGait> _NewGait);

	void SetStance(TEnumAsByte<EStance> _NewStance);

	void SetAiming(bool _NewAiming);

	void SetWalkingSpeed(float _Speed);

	void SetRunningSpeed(float _Speed);

	void SetSprintingSpeed(float _Speed);

	void SetCrouchingSpeed(float _Speed);

	void SetCharacterRotation(FRotator _TargetRotation, bool _InterpRotation, float _InterpSpeed);

	UFUNCTION(Server, Unreliable)
	void SR_SetMovementInput(FVector _MovementInput);

	UFUNCTION(Server, Unreliable)
	void SR_SetLookingRotation(FRotator _LookingRotation);

	UFUNCTION(Server, Unreliable)
	void SR_SetCharacterRotation(FRotator _TargetRotation, FRotator _CharacterRotation);

	UFUNCTION(Server, Unreliable)
	void SR_SetGait(const EGait _NewGait);

	UFUNCTION(Server, Unreliable)
	void SR_SetAiming(bool _NewAiming);

	UFUNCTION(NetMulticast, Unreliable)
	void MC_SetGait(const EGait _NewGait);

	UFUNCTION(NetMulticast, Unreliable)
	void MC_SetAiming(bool _NewAiming);


private:
	/////////////////////////////////////////////////////////////////
	//
	//                    RAGDOLL MANAGEMENT FUNCTIONS
	//
	/////////////////////////////////////////////////////////////////

	void TickRagdoll(float DeltaTime);

	void Enable_Ragdoll();

	void Disable_Ragdoll();

	void To_Ragdoll();

	void Un_Ragdoll();

	FRotator CalculateActorRotationInRagdoll(FRotator _RagdollRotation);

	FVector CalculateActorLocationInRagdoll(FVector _RagdollLocation);

	UFUNCTION(Server, Unreliable)
	void SR_To_Ragdoll();

	UFUNCTION(Server, Unreliable)
	void SR_Un_Ragdoll(bool OnGround);

	UFUNCTION(Server, Unreliable)
	void SR_Update_Ragdoll(FVector _RagdollVelocity, FVector _RagdollLocation, FRotator _ActorRotation, FVector _ActorLocation);

	UFUNCTION(NetMulticast, Unreliable)
	void MC_To_Ragdoll();

	UFUNCTION(NetMulticast, Unreliable)
	void MC_Un_Ragdoll(bool OnGround);

	UFUNCTION(NetMulticast, Unreliable)
	void MC_Update_Ragdoll(FVector _ActorLocation);

private:
	/////////////////////////////////////////////////////////////////
	//
	//                    CHARACTER CALCULATION FUNCTIONS
	//
	/////////////////////////////////////////////////////////////////

	void CalculateEssentialVariables();

	void PlayerMovementInput(bool IsForwardAxis);

	FVector GetForwardVector();

	void AddCharacterRotation(FRotator _AddAmount);

	void LimitRotation(float _AimYawLimit, float _InterpSpeed);

	float CalculateRotationRate(float _SlowSpeed, float _SlowSpeedRate, float _FastSpeed, float _FastSpeedRate);

	bool CardinalDirectionAngles(float _Value, float _Min, float _Max, float _Buffer, ECardinalDirection _CardinalDirection);

	FRotator LookingDirectionWithOffset(float _OffsetInterpSpeed, float _NEAngle, float _NWAngle, float _SEAngle, float _SWAngle, float _Buffer);

	bool CanSprint();

	void CustomAcceleration();

	float ChooseGroundFriction();

	float ChooseMaxAcceleration();

	float ChooseBrakingDeceleration();

	float ChooseMaxWalkSpeed();

	void UpdateCharacterMovementSettings();

	FVector GetRightVector();

public:
	UFUNCTION(BlueprintCallable)
	FVector ChooseVelocity();


private:
	/////////////////////////////////////////////////////////////////
	//
	//                    INTERFACE FUNCTIONS
	//
	/////////////////////////////////////////////////////////////////

	void BPI_AddCharacterRotation(FRotator _AddAmount);

	void BPI_CameraShake(TSubclassOf<UCameraShake> ShakeClass, float Scale);

	virtual void BPI_AddCharacterRotation_Implementation(FRotator _AddAmount) override;

	virtual void BPI_CameraShake_Implementation(TSubclassOf<UCameraShake> ShakeClass, float Scale) override;

public:
	/////////////////////////////////////////////////////////////////
	//
	//                    CHARACTER COMPONENTS
	//
	/////////////////////////////////////////////////////////////////

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;


private:
	/////////////////////////////////////////////////////////////////
	//
	//                    ANIMATION FUNCTIONS
	//
	/////////////////////////////////////////////////////////////////

	UFUNCTION(NetMulticast, Unreliable)
	void MC_PlayNetworkedMontage(UAnimMontage* MontageToPlay, float InPlayRate, float InTimeToStartMontageAt, bool StopAllMontages);

public:
	UFUNCTION(Server, Unreliable, BlueprintCallable)
	void SR_PlayNetworkedMontage(UAnimMontage* MontageToPlay, float InPlayRate, float InTimeToStartMontageAt, bool StopAllMontages);

}; 
