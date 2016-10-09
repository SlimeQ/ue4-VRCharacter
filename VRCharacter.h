// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

UCLASS()
class MYGAME_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

	virtual FVector ClampVectorToVector(FVector vector, FVector target, float deviance);

public:
	// Sets default values for this character's properties
	AVRCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(VisibleDefaultsOnly, Category = VR)
		USceneComponent* VROrigin;

	UPROPERTY(VisibleDefaultsOnly, Category = VR)
		UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly, Category = VR)
		USceneComponent* NeckMarker;

	UFUNCTION(BlueprintCallable, Category = VR)
		virtual void UpdateVROrigin(float DeltaSeconds, FVector OldLocation, FVector OldVelocity);

	UFUNCTION(BlueprintCallable, Category = VR)
		virtual void ThumbstickMovement(FVector direction);

private:
	float FloorOffset_cm;
	FVector WalkingDirection;
	float WalkStep;
	float WalkSpeed;
};
