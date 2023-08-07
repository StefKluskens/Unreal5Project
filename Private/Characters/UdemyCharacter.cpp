// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/UdemyCharacter.h"

#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"

#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/UdemyHUD.h"
#include "HUD/HUDOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"

AUdemyCharacter::AUdemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Disable controller rotation yaw/pitch/roll to stop weird rotation behaviour
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Orient with movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);

	//Set collision properties
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);


	//Create springArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.0f;

	//Create camera
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	//Create hair groom
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	//Create eyebrows groom
	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

void AUdemyCharacter::Tick(float deltaTime)
{
	if (Attributes && HUDOverlay)
	{
		Attributes->RegenStamina(deltaTime);
		HUDOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AUdemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Bind functions to actions
	if (UEnhancedInputComponent* pEnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		pEnhancedInput->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AUdemyCharacter::Move);
		pEnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUdemyCharacter::Look);
		pEnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AUdemyCharacter::Jump);
		pEnhancedInput->BindAction(EKeyAction, ETriggerEvent::Started, this, &AUdemyCharacter::EPressed);
		pEnhancedInput->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AUdemyCharacter::Attack);
		pEnhancedInput->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AUdemyCharacter::Dodge);
	}
}

void AUdemyCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}	
}

void AUdemyCharacter::GetHit_Implementation(const FVector& impactPoint, AActor* pHitter)
{
	Super::GetHit_Implementation(impactPoint, pHitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.0f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

float AUdemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void AUdemyCharacter::SetOverlappingItem(AItem* pItem)
{
	OverlappingItem = pItem;
}

void AUdemyCharacter::AddSouls(ASoul* pSoul)
{
	if (Attributes && HUDOverlay)
	{
		Attributes->AddSouls(pSoul->GetSouls());
		HUDOverlay->SetSouls(Attributes->GetSouls());
	}
}

void AUdemyCharacter::AddGold(ATreasure* pTreasure)
{
	if (Attributes && HUDOverlay)
	{		
		Attributes->AddGold(pTreasure->GetGold());
		HUDOverlay->SetGold(Attributes->GetGold());
	}
}

void AUdemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Add mapping context to controller
	if (APlayerController* pPlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* pSubSystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(pPlayerController->GetLocalPlayer()))
		{
			pSubSystem->AddMappingContext(UdemyContext, 0);
		}
	}

	Tags.Add(FName("EngageableTarget"));

	InitializeHUDOverlay();
}

void AUdemyCharacter::Move(const FInputActionValue& value)
{
	if (ActionState != EActionState::EAS_Unoccupied)
	{
		return;
	}

	//Get value of input
	const FVector2D currentValue = value.Get<FVector2D>();

	//Get yaw rotation of controller
	const FRotator rotation = GetControlRotation();
	const FRotator yawRotation(0.0f, rotation.Yaw, 0.0f);

	//Move forward/backwards
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(forwardDirection, currentValue.Y);

	//Move left/right
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(rightDirection, currentValue.X);
}

void AUdemyCharacter::Look(const FInputActionValue& value)
{
	//Get value of input
	const FVector2D currentValue = value.Get<FVector2D>();

	//Rotate up/down
	AddControllerPitchInput(currentValue.Y);
	
	//Rotate left/right
	AddControllerYawInput(currentValue.X);
}

void AUdemyCharacter::EPressed()
{
	AWeapon* pOverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	if (!EquippedWeapon && pOverlappingWeapon)
	{
		EquipWeapon(pOverlappingWeapon);
	}
	else
	{
		if (CanDisArm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void AUdemyCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AUdemyCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina())
	{
		return;
	}

	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;

	if (Attributes && HUDOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		HUDOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AUdemyCharacter::PlayEquipMontage(FName sectionName)
{
	ActionState = EActionState::EAS_EquippingWeapon;

	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();

	if (pAnimInstance && EquipMontage)
	{
		pAnimInstance->Montage_Play(EquipMontage);
		pAnimInstance->Montage_JumpToSection(sectionName, EquipMontage);
	}
}

void AUdemyCharacter::EquipWeapon(AWeapon* pWeapon)
{
	pWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = pWeapon;
}

void AUdemyCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AUdemyCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

bool AUdemyCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AUdemyCharacter::CanDisArm() const
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AUdemyCharacter::CanArm() const
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void AUdemyCharacter::Disarm()
{
	PlayEquipMontage(FName("UnEquip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AUdemyCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AUdemyCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

bool AUdemyCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() >= Attributes->GetDodgeCost();
}

bool AUdemyCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void AUdemyCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AUdemyCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AUdemyCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AUdemyCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool AUdemyCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void AUdemyCharacter::InitializeHUDOverlay()
{
	APlayerController* pPlayerController = Cast<APlayerController>(GetController());
	if (pPlayerController)
	{
		AUdemyHUD* pHUD = Cast<AUdemyHUD>(pPlayerController->GetHUD());
		if (pHUD)
		{
			HUDOverlay = pHUD->GetOverlay();
			if (HUDOverlay && Attributes)
			{
				HUDOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				HUDOverlay->SetStaminaBarPercent(1.0f);
				HUDOverlay->SetGold(0);
				HUDOverlay->SetSouls(0);
			}
		}
	}
}

void AUdemyCharacter::SetHUDHealth()
{
	if (HUDOverlay && Attributes)
	{
		HUDOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}