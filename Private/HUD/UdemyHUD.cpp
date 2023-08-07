// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/UdemyHUD.h"
#include "HUD/HUDOverlay.h"

void AUdemyHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* pWorld = GetWorld();
	if (pWorld)
	{
		APlayerController* pController = pWorld->GetFirstPlayerController();
		if (pController && OverlayClass)
		{
			HUDOverlay = CreateWidget<UHUDOverlay>(pController, OverlayClass);
			HUDOverlay->AddToViewport();
		}
	}	
}
