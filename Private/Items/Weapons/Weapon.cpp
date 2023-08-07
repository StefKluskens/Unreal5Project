// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/UdemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	//Create WeaponBox
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());

	//Set collision and physics settings
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxBeginOverlap);
}

void AWeapon::Equip(USceneComponent* inParent, FName inSocketName, AActor* pNewOwner, APawn* pNewInstigator)
{
	ItemState = EItemState::EIS_Equipped;

	SetOwner(pNewOwner);
	SetInstigator(pNewInstigator);
	AttachMeshToSocket(inParent, inSocketName);
	DisableSphereCollision();

	PlayEquipSound();
	DeactivateEmbers();
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* inParent, const FName& inSocketName)
{
	FAttachmentTransformRules transformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(inParent, transformRules, inSocketName);
}

void AWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor))
	{
		return;
	}

	FHitResult hit;
	BoxTrace(hit);

	if (hit.GetActor())
	{
		if (ActorIsSameType(OtherActor))
		{
			return;
		}

		UGameplayStatics::ApplyDamage(hit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(hit);
		CreateFields(hit.ImpactPoint);
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::BoxTrace(FHitResult& hit)
{
	const FVector start = BoxTraceStart->GetComponentLocation();
	const FVector end = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);
	actorsToIgnore.AddUnique(GetOwner());

	for (AActor* actor : IgnoreActors)
	{
		actorsToIgnore.AddUnique(actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		start,
		end,
		BoxTraceExtend,
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		actorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		hit,
		true
	);
	IgnoreActors.AddUnique(hit.GetActor());
}

void AWeapon::ExecuteGetHit(FHitResult& hit)
{
	IHitInterface* pHitInterface = Cast<IHitInterface>(hit.GetActor());
	if (pHitInterface)
	{
		pHitInterface->Execute_GetHit(hit.GetActor(), hit.ImpactPoint, GetOwner());
	}
}