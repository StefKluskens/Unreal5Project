// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickUpInterface.generated.h"

class AItem;
class ASoul;
class ATreasure;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickUpInterface : public UInterface
{
	GENERATED_BODY()
};

class UDEMYCLASS_API IPickUpInterface
{
	GENERATED_BODY()

public:
	virtual void SetOverlappingItem(AItem* pItem);
	virtual void AddSouls(ASoul* pSoul);
	virtual void AddGold(ATreasure* pTreasure);
};
