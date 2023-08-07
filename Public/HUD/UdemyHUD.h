// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UdemyHUD.generated.h"

class UHUDOverlay;

UCLASS()
class UDEMYCLASS_API AUdemyHUD : public AHUD
{
	GENERATED_BODY()

public:
	FORCEINLINE UHUDOverlay* GetOverlay() const { return HUDOverlay; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Udemy)
	TSubclassOf<UHUDOverlay> OverlayClass;

	UPROPERTY()
	UHUDOverlay* HUDOverlay;
};
