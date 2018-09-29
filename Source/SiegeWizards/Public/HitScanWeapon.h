// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SIEGEWIZARDS_API AHitScanWeapon : public ASWeapon
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<AActor> SpawningActor = nullptr;

	virtual void Fire() override;
};
