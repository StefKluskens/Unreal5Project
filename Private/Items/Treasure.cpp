// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Interfaces/PickUpInterface.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickUpInterface* pPickUpInterface = Cast<IPickUpInterface>(OtherActor);
	if (pPickUpInterface)
	{
		pPickUpInterface->AddGold(this);

		SpawnPickUpSound();
		Destroy();
	}
}