// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "UdemyClass/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Interfaces/PickUpInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AItem::AItem()
	: m_Amplitude(0.25f)
	, m_TimeConstant(5.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create a new static mesh component
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	ItemEffect->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	//Bind callback function to the overlap of the sphere component
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

float AItem::TransformedSin()
{
	return m_Amplitude * FMath::Sin(m_RunningTime * m_TimeConstant);
}

float AItem::TransformedCos()
{
	return m_Amplitude * FMath::Cos(m_RunningTime * m_TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickUpInterface* pPickUpInterface = Cast<IPickUpInterface>(OtherActor);
	if (pPickUpInterface)
	{
		pPickUpInterface->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickUpInterface* pPickUpInterface = Cast<IPickUpInterface>(OtherActor);
	if (pPickUpInterface)
	{
		pPickUpInterface->SetOverlappingItem(nullptr);
	}
}

void AItem::SpawnPickUpSystem()
{
	if (PickUpEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickUpEffect,
			GetActorLocation()
		);
	}
}

void AItem::SpawnPickUpSound()
{
	if (PickUpSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			PickUpSound,
			GetActorLocation()
		);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.0f, 0.0f, TransformedSin()));
	}
}

