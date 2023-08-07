// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDOverlay.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class UDEMYCLASS_API UHUDOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBarPercent(float percent);
	void SetStaminaBarPercent(float percent);
	void SetGold(int32 gold);
	void SetSouls(int32 souls);

private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulsText;
	
};
