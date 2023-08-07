// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/UdemyAnimInstance.h"
#include "Characters/UdemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UUdemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//Need to cast to custom character, custom Unreal casting
	Character = Cast<AUdemyCharacter>(TryGetPawnOwner());
	if (Character)
	{
		CharacterMovement = Character->GetCharacterMovement();
	}
}

void UUdemyAnimInstance::NativeUpdateAnimation(float deltaTime)
{
	Super::NativeUpdateAnimation(deltaTime);

	if (CharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovement->Velocity);

		IsFalling = CharacterMovement->IsFalling();
		CharacterState = Character->GetCharacterState();
		ActionState = Character->GetActionState();
		DeathPose = Character->GetDeathPose();
	}
}
