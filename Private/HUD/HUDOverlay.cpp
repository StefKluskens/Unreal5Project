// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HUDOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHUDOverlay::SetHealthBarPercent(float percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(percent);
	}
}

void UHUDOverlay::SetStaminaBarPercent(float percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(percent);
	}
}

void UHUDOverlay::SetGold(int32 gold)
{
	if (GoldText)
	{		
		const FText goldText = FText::FromString(FString::Printf(TEXT("%d"), gold));
		GoldText->SetText(goldText);
	}
}

void UHUDOverlay::SetSouls(int32 souls)
{
	if (SoulsText)
	{
		const FText soulsText = FText::FromString(FString::Printf(TEXT("%d"), souls));
		SoulsText->SetText(soulsText);
	}
}
