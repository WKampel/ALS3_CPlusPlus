


#include "BaseChar.h"
#include "UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Components/CapsuleComponent.h"
#include "Locomotion_Interface.h"
#include "Engine/LatentActionManager.h"

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

	FirstPersonCameraSocket = FName("head");

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	Camera->FieldOfView = 100.0f;
	Camera->bUsePawnControlRotation = true;
	Camera->AttachTo(GetMesh(), FirstPersonCameraSocket);


	JumpMaxHoldTime = 0.5f;
	BaseEyeHeight = 0.0f;
	bUseControllerRotationYaw = false;

	GetCapsuleComponent()->SetCapsuleHalfHeight(90.0f);
	GetCapsuleComponent()->SetCapsuleRadius(30.0f);

	GetMesh()->bUpdateJointsFromAnimation = true;

	GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
	GetCharacterMovement()->CrouchedHalfHeight = 60.0f;
	GetCharacterMovement()->SetWalkableFloorAngle(50.0f);
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->JumpZVelocity = 350.0f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->RotationRate = FRotator(0, 0, 0);
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->NavAgentProps.bCanFly = true;

}

// Called when the game starts or when spawned
void ABaseChar::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->AddTickPrerequisiteActor(this);

	LastVelocityRotation = GetActorRotation();
	LookingRotation = GetActorRotation();
	LastMovementInputRotation = GetActorRotation();
	TargetRotation = GetActorRotation();
	CharacterRotation = GetActorRotation();
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

	CalculateEssentialVariables();

	switch(MovementMode){
		case ECharMovementMode::Grounded:
		TickGrounded(DeltaTime);
		break;
		case ECharMovementMode::Ragdoll:
		TickRagdoll(DeltaTime);
	}

	//Sprint check
	if(IsLocallyControlled()){
		
		if(ShouldSprint){
			
			if(Gait != EGait::Sprinting){
				
				if(CanSprint()){
					SetGait(EGait::Sprinting);	
				} else{
					SetGait(EGait::Running);
				}

			}

		}else{
			if(Gait == EGait::Sprinting){
				SetGait(EGait::Running);
			}
		}

	}

	TickManageCharacterRotation(DeltaTime);
}

void ABaseChar::TickGrounded(float DeltaTime){
	
	if(Stance == EStance::Standing){
		switch(Gait){
			case EGait::Running:
			case EGait::Sprinting:
			CustomAcceleration();
		}
	}

}

void ABaseChar::TickRagdoll(float DeltaTime){
	
	//Set "stiffness" of ragdoll based on velocity. Faster ragdoll moves, more stiff it's joints become.
	float InSpring = UKismetMathLibrary::MapRangeClamped(UKismetMathLibrary::VSize(ChooseVelocity()), 0.0f, 1000.0f, 0.0f, 25000.0f);
	GetMesh()->SetAllMotorsAngularDriveParams(InSpring, 0.0f, 0.0f, false);

	if(!IsLocallyControlled()){
		//Ragdolls not locally controlled on the client will be pushed toward the replicated 'Ragdoll Location' vector.
		//They will simulate separately, but will end up in the same location. :)
		FVector Force = UKismetMathLibrary::Multiply_VectorFloat(UKismetMathLibrary::Subtract_VectorVector(RagdollLocation, GetMesh()->GetSocketLocation(PelvisBone)), 200.0f);
		GetMesh()->AddForce(Force, PelvisBone, true);

		return;
	} 

	//Disable gravity if falling too fast. This prevents ragdoll from continuously accelerating. 
	//This helps to keep ragdoll movement stable and will prevent it from falling through floor. :)
	if(ChooseVelocity().Z < -4000){
		GetMesh()->SetEnableGravity(false);
	}else{
		GetMesh()->SetEnableGravity(true);
	}

	RagdollVelocity = ChooseVelocity();
	RagdollLocation = GetMesh()->GetSocketLocation(PelvisBone);

	FVector LocInRagdoll = CalculateActorLocationInRagdoll(RagdollLocation);
	FRotator RotInRagdoll = CalculateActorRotationInRagdoll(GetMesh()->GetSocketRotation(PelvisBone));

	SetActorLocation(LocInRagdoll, false, false);
	TargetRotation = RotInRagdoll;

	TargetCharacterRotationDifference = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CharacterRotation).Yaw;

	CharacterRotation = RotInRagdoll;
	SetActorRotation(CharacterRotation, ETeleportType::ResetPhysics);


	SR_Update_Ragdoll(RagdollVelocity, RagdollLocation, RotInRagdoll, LocInRagdoll);

}

void ABaseChar::TickManageCharacterRotation(float DeltaTime) {
	if(IsLocallyControlled()){
		if(MovementMode == ECharMovementMode::Grounded){
			
			if(!IsMoving){
				LimitRotation(90.0f, 15.0f);
				return;
			}

			FRotator TargetRot = LookingDirectionWithOffset(5.0f, 60.0f, -60.0f, 120.0f, -120.0f, 5.0f);

			if(IsAiming){
				SetCharacterRotation(TargetRot, true, CalculateRotationRate(165.0f, 15.0f, 375.0f, 15.0f));
			}else{
				SetCharacterRotation(TargetRot, true, CalculateRotationRate(165.0f, 10.0f, 375.0f, 15.0f));
			}
			
		} else if (MovementMode == ECharMovementMode::Falling) {
			JumpRotation = LookingRotation;

			SetCharacterRotation(FRotator(0.0f, JumpRotation.Yaw, 0.0f), true, 10.0f);
		}
	}
}

void ABaseChar::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SetStance(EStance::Crouching);	
}

void ABaseChar::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SetStance(EStance::Standing);
}

void ABaseChar::Landed(const FHitResult & Hit)
{

	Super::Landed(Hit);

	if(HasMovementInput){
		GetCharacterMovement()->BrakingFrictionFactor = 0.5f;
	} else{
		GetCharacterMovement()->BrakingFrictionFactor = 3.0f;
	}

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([&]
	{
		GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
	});

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, 0.2f, false);
}

void ABaseChar::BPI_AddCharacterRotation_Implementation(FRotator _AddAmount)
{
	if(IsLocallyControlled()){
	
		//If you are having problems with rotation in the future, this might be why:
		//The original ALS code used two DELAY nodes with 0 second delays in order to force
		//AddCharacterRotation(_AddAmount); to run on the next frame.
		//As you can see, I am just straight up adding the rotation this frame without that frame delay.
		AddCharacterRotation(_AddAmount);
	}
}

void ABaseChar::BPI_CameraShake_Implementation(TSubclassOf<UCameraShake> ShakeClass, float Scale)
{

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if(!PlayerController){
		return;
	}

	PlayerController->ClientPlayCameraShake(ShakeClass, Scale, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
}

void ABaseChar::ToggleStance()
{
	if(MovementMode == ECharMovementMode::Grounded){
		switch(Stance){
			case EStance::Standing:
				Crouch();
				break;
			case EStance::Crouching:
				UnCrouch();
		}
	}
}

void ABaseChar::MoveForwardsBackwards(float _AxisValue)
{
	ForwardAxisValue = _AxisValue;
	PlayerMovementInput(true);
}

void ABaseChar::MoveRightLeft(float _AxisValue)
{
	RightAxisValue = _AxisValue;
	PlayerMovementInput(false);
}

void ABaseChar::LookUpDown(float _AxisValue)
{
	AddControllerPitchInput(LookUpDownRate * _AxisValue * GetWorld()->GetDeltaSeconds());
}

void ABaseChar::LookLeftRight(float _AxisValue)
{
	AddControllerYawInput(LookLeftRightRate * _AxisValue * GetWorld()->GetDeltaSeconds());
}

void ABaseChar::Pressed_JumpAction()
{	
	if(Stance == EStance::Standing){
		if(!IsPlayingRootMotion()) Jump();
	}else if(Stance == EStance::Crouching){
		UnCrouch();
	}
}

void ABaseChar::Released_JumpAction()
{
	StopJumping();
}

void ABaseChar::Pressed_AimAction(){
	SetAiming(true);
}

void ABaseChar::Released_AimAction() {
	SetAiming(false);
}

void ABaseChar::Pressed_SprintAction()
{
	ShouldSprint = true;

	if(Gait == EGait::Walking){
		Gait = EGait::Running;
	}
}

void ABaseChar::Released_SprintAction()
{
	ShouldSprint = false;
}

void ABaseChar::Pressed_RagdollAction()
{
	switch(MovementMode){
		case ECharMovementMode::Grounded:
		case ECharMovementMode::Falling:
		To_Ragdoll();
		break;
		case ECharMovementMode::Ragdoll:
		Un_Ragdoll();
	}
}

// Called to bind functionality to input
void ABaseChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseChar::MoveForwardsBackwards);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseChar::MoveRightLeft);

	PlayerInputComponent->BindAxis("LookUp", this, &ABaseChar::LookUpDown);
	PlayerInputComponent->BindAxis("LookLeft", this, &ABaseChar::LookLeftRight);

	PlayerInputComponent->BindAction("StanceAction", EInputEvent::IE_Pressed, this, &ABaseChar::ToggleStance);

	PlayerInputComponent->BindAction("JumpAction", EInputEvent::IE_Pressed, this, &ABaseChar::Pressed_JumpAction);
	PlayerInputComponent->BindAction("JumpAction", EInputEvent::IE_Released, this, &ABaseChar::Released_JumpAction);

	PlayerInputComponent->BindAction("AimAction", EInputEvent::IE_Pressed, this, &ABaseChar::Pressed_AimAction);
	PlayerInputComponent->BindAction("AimAction", EInputEvent::IE_Released, this, &ABaseChar::Released_AimAction);

	PlayerInputComponent->BindAction("SprintAction", EInputEvent::IE_Pressed, this, &ABaseChar::Pressed_SprintAction);
	PlayerInputComponent->BindAction("SprintAction", EInputEvent::IE_Released, this, &ABaseChar::Released_SprintAction);

	PlayerInputComponent->BindAction("RagdollAction", EInputEvent::IE_Pressed, this, &ABaseChar::Pressed_RagdollAction);
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

float ABaseChar::CalculateRotationRate(float _SlowSpeed, float _SlowSpeedRate, float _FastSpeed, float _FastSpeedRate)
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

bool ABaseChar::CardinalDirectionAngles(float _Value, float _Min, float _Max, float _Buffer, ECardinalDirection _CardinalDirection)
{
	if(_CardinalDirection == CardinalDirection){
		return UKismetMathLibrary::InRange_FloatFloat(_Value, _Min - _Buffer, _Max + _Buffer);
	} else {
		return UKismetMathLibrary::InRange_FloatFloat(_Value, _Min + _Buffer, _Max - _Buffer);
	}
}

FRotator ABaseChar::LookingDirectionWithOffset(float _OffsetInterpSpeed, float _NEAngle, float _NWAngle, float _SEAngle, float _SWAngle, float _Buffer)
{
	float Value = UKismetMathLibrary::NormalizedDeltaRotator(HasMovementInput ? LastMovementInputRotation : LastVelocityRotation, LookingRotation).Yaw;
	float Target = 0;

	if(CardinalDirectionAngles(Value, _NWAngle, _NEAngle, _Buffer, ECardinalDirection::North)){
		CardinalDirection = ECardinalDirection::North;
		Target = Value;
	} else if(CardinalDirectionAngles(Value, _NEAngle, _SEAngle, _Buffer, ECardinalDirection::East)){
		CardinalDirection = ECardinalDirection::East;
		Target = Value - 90;
	} else if (CardinalDirectionAngles(Value, _SWAngle, _NWAngle, _Buffer, ECardinalDirection::West)) {
		CardinalDirection = ECardinalDirection::West;
		Target = Value + 90;
	} else{
		CardinalDirection = ECardinalDirection::South;
		Target = (Value > 0) ? Value - 180 : Value + 180;
	}

	if(!IsAiming && Gait == EGait::Walking) Target = 0.0f;

	RotationOffset = UKismetMathLibrary::FInterpTo(RotationOffset, Target, GetWorld()->GetDeltaSeconds(), _OffsetInterpSpeed);

	return FRotator(0, LookingRotation.Yaw + RotationOffset, 0);
}

bool ABaseChar::CanSprint()
{
	if(MovementMode == ECharMovementMode::Ragdoll) return true;
	if(!HasMovementInput) return true;
	if(IsAiming) return true;

	return (UKismetMathLibrary::Abs(UKismetMathLibrary::NormalizedDeltaRotator(LastMovementInputRotation, LookingRotation).Yaw) < 50.0f);
}

void ABaseChar::CustomAcceleration()
{
	float Value = UKismetMathLibrary::Abs(MovementInputVelocityDifference);

	float MaxAcceleration = RunningAcceleration * UKismetMathLibrary::MapRangeClamped(Value, 45.0f, 130.0f, 1.0f, 0.2f);
	float GroundFriction = RunningGroundFriction * UKismetMathLibrary::MapRangeClamped(Value, 45.0f, 130.0f, 1.0f, 0.4f);

	GetCharacterMovement()->MaxAcceleration = MaxAcceleration;
	GetCharacterMovement()->GroundFriction = GroundFriction;
}

float ABaseChar::ChooseGroundFriction()
{
	if (Gait == EGait::Walking) return WalkingGroundFriction;

	return RunningGroundFriction;
}


float ABaseChar::ChooseMaxAcceleration()
{
	if (Gait == EGait::Walking) return WalkingAcceleration;

	return RunningAcceleration;
}


float ABaseChar::ChooseBrakingDeceleration()
{
	if (Gait == EGait::Walking) return WalkingDeceleration;

	return RunningDeceleration;
}

float ABaseChar::ChooseMaxWalkSpeed()
{
	if(Stance == EStance::Standing){
		if(IsAiming){
			if(Gait == EGait::Sprinting) return RunningSpeed;

			return WalkingSpeed;
		}else{
			if (Gait == EGait::Walking) return WalkingSpeed;
			if (Gait == EGait::Running) return RunningSpeed;
			if (Gait == EGait::Sprinting) return SprintingSpeed;
		}
	}else if(Stance == EStance::Crouching){
		if (Gait == EGait::Walking) return CrouchingSpeed - 50;
		if (Gait == EGait::Running) return CrouchingSpeed;
		if (Gait == EGait::Sprinting) return CrouchingSpeed + 50;
	}

	return 0.0f;
}

void ABaseChar::UpdateCharacterMovementSettings()
{
	UCharacterMovementComponent* CharMovement = GetCharacterMovement();

	CharMovement->MaxWalkSpeed = ChooseMaxWalkSpeed();
	CharMovement->MaxWalkSpeedCrouched = CharMovement->MaxWalkSpeed;
	CharMovement->MaxAcceleration = ChooseMaxAcceleration();
	CharMovement->BrakingDecelerationWalking = ChooseBrakingDeceleration();
	CharMovement->GroundFriction = ChooseGroundFriction();
}

void ABaseChar::SetMovementMode(TEnumAsByte<ECharMovementMode> _NewMovementMode)
{

	if(_NewMovementMode != MovementMode){
		PrevMovementMode = MovementMode;
		MovementMode = _NewMovementMode;


		//on change
		ILocomotion_Interface::Execute_BPI_SetMovementMode(GetMesh()->GetAnimInstance(), MovementMode);
		ILocomotion_Interface::Execute_BPI_SetMovementMode(GetMesh()->GetPostProcessInstance(), MovementMode);

		UpdateCharacterMovementSettings();

		if (IsLocallyControlled()) {
			switch (PrevMovementMode) {
			case ECharMovementMode::Grounded:
				JumpRotation = IsMoving ? LastVelocityRotation : CharacterRotation;
				RotationOffset = 0.0f;
				break;
			case ECharMovementMode::Ragdoll:
				JumpRotation = CharacterRotation;
				break;
			}
		}
	}
}

void ABaseChar::SetGait(TEnumAsByte<EGait> _NewGait)
{
	if(_NewGait != Gait){
		Gait = _NewGait;

		SR_SetGait(_NewGait);

		//on change
		ILocomotion_Interface::Execute_BPI_SetGait(GetMesh()->GetAnimInstance(), Gait);

		UpdateCharacterMovementSettings();
	}
}

void ABaseChar::SetStance(TEnumAsByte<EStance> _NewStance)
{
	if(_NewStance != Stance){
		Stance = _NewStance;

		//on change
		ILocomotion_Interface::Execute_BPI_SetStance(GetMesh()->GetAnimInstance(), Stance);
		ILocomotion_Interface::Execute_BPI_SetStance(GetMesh()->GetPostProcessInstance(), Stance);

		UpdateCharacterMovementSettings();


	}
}

void ABaseChar::SetAiming(bool _NewAiming)
{
	if(_NewAiming != IsAiming){
		IsAiming = _NewAiming;

		SR_SetAiming(IsAiming);

		//on change
		ILocomotion_Interface::Execute_BPI_SetAiming(GetMesh()->GetAnimInstance(), IsAiming);

		UpdateCharacterMovementSettings();
	}
}

void ABaseChar::SetWalkingSpeed(float _Speed)
{
	if(_Speed != WalkingSpeed){
		WalkingSpeed = _Speed;

		ILocomotion_Interface::Execute_BPI_SetWalkingSpeed(GetMesh()->GetAnimInstance(), WalkingSpeed);
		UpdateCharacterMovementSettings();
	}	
}

void ABaseChar::SetRunningSpeed(float _Speed)
{
	if (_Speed != RunningSpeed) {
		RunningSpeed = _Speed;

		ILocomotion_Interface::Execute_BPI_SetRunningSpeed(GetMesh()->GetAnimInstance(), RunningSpeed);
		UpdateCharacterMovementSettings();
	}
}

void ABaseChar::SetSprintingSpeed(float _Speed)
{
	if (_Speed != SprintingSpeed) {
		SprintingSpeed = _Speed;

		ILocomotion_Interface::Execute_BPI_SetSprintingSpeed(GetMesh()->GetAnimInstance(), SprintingSpeed);
		UpdateCharacterMovementSettings();
	}
}

void ABaseChar::SetCrouchingSpeed(float _Speed)
{
	if (_Speed != CrouchingSpeed) {
		CrouchingSpeed = _Speed;

		ILocomotion_Interface::Execute_BPI_SetCrouchingSpeed(GetMesh()->GetAnimInstance(), CrouchingSpeed);
		UpdateCharacterMovementSettings();
	}
}

void ABaseChar::SR_SetAiming_Implementation(bool _NewAiming)
{
	MC_SetAiming(_NewAiming);
}

void ABaseChar::MC_SetAiming_Implementation(bool _NewAiming)
{
	if(!IsLocallyControlled()){
		IsAiming = _NewAiming;

		ILocomotion_Interface::Execute_BPI_SetAiming(GetMesh()->GetAnimInstance(), IsAiming);

		UpdateCharacterMovementSettings();
	}
}

void ABaseChar::SR_SetCharacterRotation_Implementation(FRotator _TargetRotation, FRotator _CharacterRotation)
{
	TargetRotation = _TargetRotation;
	CharacterRotation = _CharacterRotation;

	SetActorRotation(CharacterRotation, ETeleportType::ResetPhysics);
}

void ABaseChar::SR_SetGait_Implementation(const EGait _NewGait)
{
	MC_SetGait(_NewGait);
}

void ABaseChar::MC_SetGait_Implementation(const EGait _NewGait)
{
	if(!IsLocallyControlled()){
		Gait = _NewGait;

		//on change
		ILocomotion_Interface::Execute_BPI_SetGait(GetMesh()->GetAnimInstance(), Gait);

		UpdateCharacterMovementSettings();
	}
}

void ABaseChar::SR_PlayNetworkedMontage_Implementation(UAnimMontage* MontageToPlay, float InPlayRate, float InTimeToStartMontageAt, bool StopAllMontages)
{
	MC_PlayNetworkedMontage(MontageToPlay, InPlayRate, InTimeToStartMontageAt, StopAllMontages);
}

void ABaseChar::MC_PlayNetworkedMontage_Implementation(UAnimMontage* MontageToPlay, float InPlayRate, float InTimeToStartMontageAt, bool StopAllMontages)
{
	if(!IsLocallyControlled()){
		GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay, InPlayRate, EMontagePlayReturnType::MontageLength, InTimeToStartMontageAt, StopAllMontages);
	}
}

void ABaseChar::To_Ragdoll()
{
	SR_To_Ragdoll();

	Enable_Ragdoll();
}

void ABaseChar::Un_Ragdoll()
{
	SR_Un_Ragdoll(RagdollOnGround);

	Disable_Ragdoll();
}

void ABaseChar::SR_To_Ragdoll_Implementation()
{
	MC_To_Ragdoll();
}

void ABaseChar::SR_Un_Ragdoll_Implementation(bool OnGround)
{
	MC_Un_Ragdoll(OnGround);
}

void ABaseChar::MC_To_Ragdoll_Implementation()
{
	if (!IsLocallyControlled()) {
		Enable_Ragdoll();
	}
}

void ABaseChar::MC_Un_Ragdoll_Implementation(bool OnGround)
{
	if (!IsLocallyControlled()) {
		RagdollOnGround = OnGround;
		Disable_Ragdoll();
	}
}


void ABaseChar::Enable_Ragdoll()
{
	SetReplicateMovement(false);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None, 0);
	SetMovementMode(ECharMovementMode::Ragdoll);

	//Disable capsule collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	//Enable mesh collision
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);

	//Simulate physics on all bodies below pelvis
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBone, true, true);
}

void ABaseChar::Disable_Ragdoll()
{
	SetReplicateMovement(true);

	EMovementMode Mode = RagdollOnGround ? EMovementMode::MOVE_Walking : EMovementMode::MOVE_Falling;
	GetCharacterMovement()->SetMovementMode(Mode, 0);

	GetCharacterMovement()->Velocity = RagdollVelocity;

	//Save pose snapshot (used for blending) and play get up animation (if ragdoll on ground)
	ILocomotion_Interface::Execute_BPI_SavePoseSnapshot(GetMesh()->GetAnimInstance(), RagdollPoseSnapshot);

	if(RagdollOnGround){
		bool FaceDown = GetMesh()->GetSocketRotation(PelvisBone).Roll > 0.0f;
		ILocomotion_Interface::Execute_BPI_Play_GetUp_Anim(GetMesh()->GetAnimInstance(), FaceDown);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);

	//Unsimulate all bodies
	GetMesh()->SetAllBodiesSimulatePhysics(false);
}

void ABaseChar::SR_Update_Ragdoll_Implementation(FVector _RagdollVelocity, FVector _RagdollLocation, FRotator _ActorRotation, FVector _ActorLocation)
{
	RagdollVelocity = _RagdollVelocity;
	RagdollLocation = _RagdollLocation;
	CharacterRotation = _ActorRotation;
	TargetRotation = FRotator(0, 0, 0);

	MC_Update_Ragdoll(_ActorLocation);
}

void ABaseChar::MC_Update_Ragdoll_Implementation(FVector _ActorLocation)
{
	if(!IsLocallyControlled()){
		SetActorLocationAndRotation(_ActorLocation, CharacterRotation);
	}
}

FVector ABaseChar::GetRightVector()
{
	return UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0));
}

void ABaseChar::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PrevCustomMode);

	switch(GetCharacterMovement()->MovementMode){
		case EMovementMode::MOVE_Walking:
		case EMovementMode::MOVE_NavWalking:
			SetMovementMode(ECharMovementMode::Grounded);
			break;
		case EMovementMode::MOVE_Falling:
			SetMovementMode(ECharMovementMode::Falling);
			break;
	}

}

void ABaseChar::SR_SetMovementInput_Implementation(FVector _MovementInput)
{
	MovementInput = _MovementInput;
}

void ABaseChar::SR_SetLookingRotation_Implementation(FRotator _LookingRotation)
{
	LookingRotation = _LookingRotation;
}
