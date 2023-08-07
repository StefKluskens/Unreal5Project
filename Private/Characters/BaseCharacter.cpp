// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"

#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"

#include "UdemyClass/DebugMacros.h"

ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create attribute component
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& impactPoint, AActor* pHitter)
{
	if (IsAlive() && pHitter)
	{
		DirectionalHitReact(pHitter->GetActorLocation());
	}
	else
	{
		Die();
	}

	PlayHitSound(impactPoint);
	SpawnHitParticles(impactPoint);
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::Die_Implementation()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::DodgeEnd()
{
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);

	TEnumAsByte<EDeathPose> pose(selection);
	if (pose < EDeathPose::EDP_Max)
	{
		DeathPose = pose;
	}

	return selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Dodge"));
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (pAnimInstance)
	{
		pAnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (!CombatTarget)
	{
		return FVector();
	}
	
	const FVector combatTargetLocation = CombatTarget->GetActorLocation();
	const FVector location = GetActorLocation();

	FVector targetToMe = (location - combatTargetLocation).GetSafeNormal();
	targetToMe *= WarpTargetDistance;

	return combatTargetLocation + targetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* pMontage, const TArray<FName>& sectionNames)
{
	if (sectionNames.Num() <= 0)
	{
		return -1;
	}

	const int32 maxSectionIndex = sectionNames.Num() - 1;
	const int32 selection = FMath::RandRange(0, maxSectionIndex);
	PlayMontageSection(pMontage, sectionNames[selection]);
	return selection;
}

void ABaseCharacter::PlayHitReactMontage(const FName& sectionName)
{
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();

	if (pAnimInstance && HitReactMontage)
	{
		pAnimInstance->Montage_Play(HitReactMontage);
		pAnimInstance->Montage_JumpToSection(sectionName, HitReactMontage);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& impactPoint)
{
	const FVector forward = GetActorForwardVector();
	const FVector impactLowered(impactPoint.X, impactPoint.Y, GetActorLocation().Z);
	const FVector toHit = (impactLowered - GetActorLocation()).GetSafeNormal();

	const double cosTheta = FVector::DotProduct(forward, toHit);
	double theta = FMath::Acos(cosTheta);
	theta = FMath::RadiansToDegrees(theta);

	//If cross product is pointing down, theta should be negative
	const FVector crossProduct = FVector::CrossProduct(forward, toHit);
	if (crossProduct.Z < 0)
	{
		theta *= -1.0f;
	}

	FName section("FromBack");

	if (theta >= -45.0f && theta < 45.0f)
	{
		section = FName("FromFront");
	}
	else if (theta >= -135.0f && theta < -45.0f)
	{
		section = FName("FromLeft");
	}
	else if (theta >= 45.0f && theta < 135.0f)
	{
		section = FName("FromRight");
	}

	PlayHitReactMontage(section);
}

void ABaseCharacter::PlayHitSound(const FVector& impactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			impactPoint
		);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& impactPoint)
{
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			impactPoint
		);
	}
}

void ABaseCharacter::HandleDamage(float damageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(damageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* pMontage, const FName& sectionName)
{
	UAnimInstance* pAnimInstance = GetMesh()->GetAnimInstance();
	if (pAnimInstance && pMontage)
	{
		pAnimInstance->Montage_Play(pMontage);
		pAnimInstance->Montage_JumpToSection(sectionName, pMontage);
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type collisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(collisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}
