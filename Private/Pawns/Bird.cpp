// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(20.0f);
	Capsule->SetCapsuleRadius(15.0f);
	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(GetRootComponent());

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(Capsule);
	SpringArm->TargetArmLength = 300.0f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* pPlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* pSubSystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(pPlayerController->GetLocalPlayer()))
		{
			pSubSystem->AddMappingContext(BirdMappingContext, 0);
		}
	}
}

void ABird::MoveForward(float value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Value: %f"), value);
	if (Controller && value != 0.0f)
	{
		FVector forward = GetActorForwardVector();
		AddMovementInput(forward, value);
	}
}

void ABird::Move(const FInputActionValue& value)
{
	const float currentValue = value.Get<float>();

	if (GetController() && currentValue != 0.0f)
	{
		FVector forward = GetActorForwardVector();
		AddMovementInput(forward, currentValue);
	}
}

void ABird::Look(const FInputActionValue& value)
{
	const FVector2D currentValue = value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerYawInput(currentValue.X);
		AddControllerPitchInput(currentValue.Y);
	}
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Needs the full name (ABird::MoveForward)
	//PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);

	if (UEnhancedInputComponent* pEnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		pEnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
		pEnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look);
	}
}

