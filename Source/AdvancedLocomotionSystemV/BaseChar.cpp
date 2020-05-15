


#include "BaseChar.h"
#include "UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABaseChar::ABaseChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Input
	LookUpDownRate = 150.0f;
	LookLeftRightRate = 150.0f;

	//Essential

	//Rotation System
	RotationRateMultiplier = 1.0f;

	//Movement System
	WalkingSpeed = 165.0f;
	RunningSpeed = 375.0f;
	SprintingSpeed = 600.0f;
	CrouchingSpeed = 150.0f;
	WalkingAcceleration = 800.0f;
	RunningAcceleration = 1000.0f;
	WalkingDeceleration = 800.0f;
	RunningDeceleration = 800.0f;
	WalkingGroundFriction = 8.0f;
	RunningGroundFriction = 6.0f;

	Gait = Running; 

	PelvisBone = FName("pelvis");
	RagdollPoseSnapshot = FName("RagdollPose");

	RightShoulder = true;

	FirstPersonCameraSocket = FName("head");

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->FieldOfView = 100.0f;
	Camera->bUsePawnControlRotation = true;
	Camera->AttachTo(GetMesh(), FirstPersonCameraSocket);

}

// Called when the game starts or when spawned
void ABaseChar::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseChar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseChar, CharacterRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABaseChar, LookingRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABaseChar, MovementInput, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABaseChar, TargetRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ABaseChar, RagdollLocation, COND_SkipOwner);

	DOREPLIFETIME(ABaseChar, RotationOffset);
}

// Called every frame
void ABaseChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//ALS FUNCTIONS
FVector ABaseChar::ChooseVelocity()
{
	if(MovementMode == ECharMovementMode::Ragdoll) return GetMesh()->GetPhysicsLinearVelocity(PelvisBone);
	return GetVelocity();
}

void ABaseChar::CalculateEssentialVariables()
{

	//IsMoving
	IsMoving = UKismetMathLibrary::NotEqual_VectorVector(FVector(ChooseVelocity().X, ChooseVelocity().Y, 0), FVector(0, 0, 0), 1.0f);

	if(IsMoving){
		LastVelocityRotation = UKismetMathLibrary::Conv_VectorToRotator(ChooseVelocity());
		Direction = UKismetMathLibrary::NormalizedDeltaRotator(LastVelocityRotation, CharacterRotation).Yaw;
	}

	if(IsLocallyControlled()){
		MovementInput = GetCharacterMovement()->GetLastInputVector();
		SR_SetMovementInput(MovementInput);
	}

	HasMovementInput = UKismetMathLibrary::NotEqual_VectorVector(MovementInput, FVector(0, 0, 0), .0001f);

	if(HasMovementInput){
		LastMovementInputRotation = UKismetMathLibrary::Conv_VectorToRotator(MovementInput);
		MovementInputVelocityDifference = UKismetMathLibrary::NormalizedDeltaRotator(LastMovementInputRotation, LastVelocityRotation).Yaw;
	}

	if(IsLocallyControlled()){
		PrevAimYaw = LookingRotation.Yaw;
		LookingRotation = GetControlRotation();
		SR_SetLookingRotation(LookingRotation);
		AimYawRate = (LookingRotation.Yaw - PrevAimYaw) / GetWorld()->GetDeltaSeconds();
	}

	AimYawDelta = UKismetMathLibrary::NormalizedDeltaRotator(LookingRotation, CharacterRotation).Yaw;

}

void ABaseChar::PlayerMovementInput(bool IsForwardAxis)
{
	
	if(MovementMode == ECharMovementMode::Ragdoll){
		FVector Value = UKismetMathLibrary::Add_VectorVector(UKismetMathLibrary::Multiply_VectorFloat(GetForwardVector(), ForwardAxisValue), UKismetMathLibrary::Multiply_VectorFloat(GetRightVector(), RightAxisValue));
		FVector NormalizedValue = UKismetMathLibrary::Normal(Value, .0001);

		float Strength = 0;

		switch (Gait) {
		case EGait::Walking:
		case EGait::Running:
			Strength = 500.0f;
			break;
		case EGait::Sprinting:
			Strength = 800.0f;
		}

		FVector Torque = UKismetMathLibrary::Multiply_VectorFloat(NormalizedValue, Strength);

		GetMesh()->AddTorqueInRadians(FVector(Torque.Y * -1, Torque.X, Torque.Z), PelvisBone, true);
		GetCharacterMovement()->AddInputVector(NormalizedValue, false);

	} else if(IsForwardAxis){
		AddMovementInput(GetForwardVector(), ForwardAxisValue, false);			 
	}else{
		AddMovementInput(GetRightVector(), RightAxisValue, false);
	}

}

FVector ABaseChar::GetForwardVector()
{
	return UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0));
}

FRotator ABaseChar::CalculateActorRotationInRagdoll(FRotator _RagdollRotation)
{

	float Yaw = (_RagdollRotation.Roll > 0) ? _RagdollRotation.Yaw : _RagdollRotation.Yaw - 180;
	return FRotator(0, Yaw, 0);
}

FVector ABaseChar::CalculateActorLocationInRagdoll(FVector _RagdollLocation)
{
	FHitResult HitResult;

	UKismetSystemLibrary::LineTraceSingle(GetWorld(), _RagdollLocation, FVector(_RagdollLocation.X, _RagdollLocation.Y, _RagdollLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, TArray<AActor*>(), EDrawDebugTrace::Type::ForOneFrame, HitResult, true, FLinearColor::Black, FLinearColor::Black, 5);

	RagdollOnGround = HitResult.bBlockingHit;

	float Z = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - UKismetMathLibrary::Abs(HitResult.ImpactPoint.Z - HitResult.TraceStart.Z) + 2.0f;

	if(!RagdollOnGround) Z = 0;

	return FVector(_RagdollLocation.X, _RagdollLocation.Y, _RagdollLocation.Z + Z);
}

void ABaseChar::SetCharacterRotation(FRotator _TargetRotation, bool _InterpRotation, float _InterpSpeed)
{
	TargetRotation = _TargetRotation;
	TargetCharacterRotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation).Yaw;

	if(_InterpRotation){
	
		if(_InterpSpeed != 0.0f){
			CharacterRotation = UKismetMathLibrary::RInterpTo(CharacterRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), _InterpSpeed);
		} else{
			return;
		}
		
	}else{
		CharacterRotation = TargetRotation;
	}

	SetActorRotation(CharacterRotation, ETeleportType::ResetPhysics);
	SR_SetCharacterRotation(TargetRotation, CharacterRotation);

}

void ABaseChar::AddCharacterRotation(FRotator _AddAmount)
{
	TargetRotation = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, UKismetMathLibrary::NegateRotator(_AddAmount));

	TargetCharacterRotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation).Yaw;

	CharacterRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, UKismetMathLibrary::NegateRotator(_AddAmount));

	SetActorRotation(CharacterRotation, ETeleportType::ResetPhysics);
	SR_SetCharacterRotation(TargetRotation, CharacterRotation);
}

void ABaseChar::LimitRotation(float _AimYawLimit, float _InterpSpeed)
{
	if(UKismetMathLibrary::Abs(AimYawDelta) > _AimYawLimit){
		float Yaw = LookingRotation.Yaw + _AimYawLimit;

		if(AimYawDelta > 0) Yaw = LookingRotation.Yaw - _AimYawLimit;

		SetCharacterRotation(FRotator(0, Yaw, 0), true, _InterpSpeed);
	}
}

float ABaseChar::CalculateRotationRateNew(float _SlowSpeed, float _SlowSpeedRate, float _FastSpeed, float _FastSpeedRate)
{
	if(RotationRateMultiplier != 1.0f){
		RotationRateMultiplier = UKismetMathLibrary::FClamp(RotationRateMultiplier + GetWorld()->GetDeltaSeconds(), 0, 1);
	}

	float Value = 0.0f;
	float XYSpeed = UKismetMathLibrary::VSize(FVector(ChooseVelocity().X, ChooseVelocity().Y, 0.0f));

	if(XYSpeed > _SlowSpeed){
		Value = UKismetMathLibrary::MapRangeUnclamped(XYSpeed, _SlowSpeed, _FastSpeed, _SlowSpeedRate, _FastSpeedRate);
	} else{
		Value = UKismetMathLibrary::MapRangeUnclamped(XYSpeed, 0, _SlowSpeed, 0, _SlowSpeedRate);
	}

	return UKismetMathLibrary::FClamp(Value * RotationRateMultiplier, 0.1f, 15.0f);
}

void ABaseChar::SR_SetCharacterRotation_Implementation(FRotator _TargetRotation, FRotator _CharacterRotation)
{
	TargetRotation = _TargetRotation;
	CharacterRotation = _CharacterRotation;

	SetActorRotation(CharacterRotation, ETeleportType::ResetPhysics);
}

FVector ABaseChar::GetRightVector()
{
	return UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0));
}

void ABaseChar::SR_SetMovementInput_Implementation(FVector _MovementInput)
{
	MovementInput = _MovementInput;
}

void ABaseChar::SR_SetLookingRotation_Implementation(FRotator _LookingRotation)
{
	LookingRotation = _LookingRotation;
}
