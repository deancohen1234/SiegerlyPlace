// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Public/SWeapon.h"
#include "SProjectileLauncher.generated.h"

class AAProjectile;

UCLASS()
class SIEGEWIZARDS_API ASProjectileLauncher : public ASWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectileLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	TSubclassOf<AAProjectile> ProjectileBlueprint = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float LaunchSpeed = 1000.0f;

	virtual void Fire() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
