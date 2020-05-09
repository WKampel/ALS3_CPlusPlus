


#include "BaseChar.h"
#include "UnrealNetwork.h"

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

