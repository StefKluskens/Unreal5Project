// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();


}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::ReceiveDamage(float damage)
{
	Health = FMath::Clamp(Health - damage, 0.0f, MaxHealth);
}

void UAttributeComponent::UseStamina(float staminaCost)
{
	Stamina = FMath::Clamp(Stamina - staminaCost, 0.0f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent() const
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent() const
{
	return Stamina / MaxStamina;
}

bool UAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

void UAttributeComponent::AddGold(int32 amountOfGold)
{
	Gold += amountOfGold;
}

void UAttributeComponent::AddSouls(int32 amountOfSouls)
{
	Souls += amountOfSouls;
}

void UAttributeComponent::RegenStamina(float deltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * deltaTime, 0.0f, MaxStamina);
}
