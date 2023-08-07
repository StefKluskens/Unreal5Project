// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickUpInterface.h"
#include "Kismet/KismetSystemLibrary.h" 

void ASoul::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (GetActorLocation().Z > DesiredZ)
	{
		AddActorWorldOffset(FVector(0.0f, 0.0f, DriftRate * deltaTime));
	}
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	const FVector start = GetActorLocation();
	const FVector end = start - FVector(0.0f, 0.0f, 2000.0f);

	TArray<TEnumAsByte<EObjectTypeQuery>> objectType;
	objectType.Add(EObjectTypeQuery::ObjectTypeQuery1);

	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(GetOwner());

	FHitResult hit;

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		start,
		end,
		objectType,
		false,
		actorsToIgnore,
		EDrawDebugTrace::None,
		hit,
		true
	);

	DesiredZ = hit.ImpactPoint.Z + 50.0f;
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickUpInterface* pPickUpInterface = Cast<IPickUpInterface>(OtherActor);
	if (pPickUpInterface)
	{
		pPickUpInterface->AddSouls(this);

		SpawnPickUpSystem();
		SpawnPickUpSound();
		Destroy();
	}
}