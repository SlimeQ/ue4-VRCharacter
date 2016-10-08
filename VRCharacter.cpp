// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterGame.h"
#include "VRCharacter.h"


// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set constants
	// ------------------------------------------
	FloorOffset_cm = 2;
	WalkSpeed = 0.05f;
	WalkStep = 10;
	// ------------------------------------------


	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VR Origin"));
	VROrigin->SetupAttachment(GetCapsuleComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("HMD"));
	Camera->SetupAttachment(VROrigin);
	Camera->bLockToHmd = true;
	//Camera->SetIsReplicated(true);

	NeckMarker = CreateDefaultSubobject<USceneComponent>(TEXT("Neck Marker"));
	NeckMarker->SetupAttachment(Camera);

	OnCharacterMovementUpdated.AddDynamic(this, &AVRCharacter::UpdateVROrigin);

}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	NeckMarker->SetRelativeLocation(FVector(-20, 0, -40));
}

// Called every frame
void AVRCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// calculate direction to move capsule towards camera
	FVector direction_to_camera = NeckMarker->GetComponentLocation() - GetCapsuleComponent()->GetComponentLocation();
	direction_to_camera.Z = 0;

	// move capsule towards camera
	float vector_length = direction_to_camera.Size();
	if (vector_length > 1) {
		AddMovementInput(direction_to_camera, vector_length);
	}
}

void AVRCharacter::UpdateVROrigin(float DeltaSeconds, FVector OldLocation, FVector OldVelocity) {
	// keep VROrigin from rotating
	VROrigin->SetWorldRotation(FRotator(0, 0, 0));

	// get true relative location of scene
	FVector new_scene_location = VROrigin->GetComponentLocation() - GetCapsuleComponent()->GetComponentLocation();

	//add it to old_location
	new_scene_location += OldLocation;

	// set Z to bottom of capsule
	new_scene_location.Z = GetCapsuleComponent()->GetComponentLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - FloorOffset_cm;

	// update VROrigin location
	VROrigin->SetWorldLocation(new_scene_location);
}

void AVRCharacter::ThumbstickMovement(FVector direction) {
	// get ideal scene location to force camera to capsule
	FVector ideal_scene_location = GetCapsuleComponent()->GetComponentLocation() - (NeckMarker->GetComponentLocation() - VROrigin->GetComponentLocation());
	ideal_scene_location.Z = GetCapsuleComponent()->GetComponentLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - FloorOffset_cm;

	// only walk while touching the ground
	if (GetMovementComponent()->IsMovingOnGround()) {
		// save walking direction for jumping momentum
		WalkingDirection = direction;

		// calculate desired VROrigin
		FVector destination = VROrigin->GetComponentLocation() + (direction * WalkStep);
		destination = ClampVectorToVector(destination, ideal_scene_location, 10);

		// convert to relative space
		destination = GetCapsuleComponent()->GetComponentTransform().InverseTransformPosition(destination);

		// move VROrigin
		FLatentActionInfo latent_info;
		latent_info.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(VROrigin, destination, VROrigin->RelativeRotation, false, false, WalkSpeed, false, EMoveComponentAction::Type::Move, latent_info);
	}
	else {
		// calculate desired VROrigin
		FVector destination = VROrigin->GetComponentLocation() + (WalkingDirection * WalkStep);
		destination = ClampVectorToVector(destination, ideal_scene_location, 10);

		// convert to relative space
		destination = GetCapsuleComponent()->GetComponentTransform().InverseTransformPosition(destination);

		// move VROrigin
		FLatentActionInfo latent_info;
		latent_info.CallbackTarget = this;
		UKismetSystemLibrary::MoveComponentTo(VROrigin, destination, VROrigin->RelativeRotation, false, false, WalkSpeed, false, EMoveComponentAction::Type::Move, latent_info);
	}
}

FVector AVRCharacter::ClampVectorToVector(FVector vector, FVector target, float deviance)
{
	FVector clamped = vector;
	clamped.X = FMath::Clamp(vector.X, target.X - deviance, target.X + deviance);
	clamped.Y = FMath::Clamp(vector.Y, target.Y - deviance, target.Y + deviance);
	return FVector(
		FMath::Clamp(vector.X, target.X - deviance, target.X + deviance),
		FMath::Clamp(vector.Y, target.Y - deviance, target.Y + deviance),
		vector.Z
	);
}
//
//// Called to bind functionality to input
//void AVRCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
//{
//	Super::SetupPlayerInputComponent(InputComponent);
//
//}

