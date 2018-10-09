// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkelatalMeshComponent;
class UDamageType;
class UParticleSystem;

//contains information of a single hitscan linetrace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class SIEGEWIZARDS_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent * MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage = 15.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float HeadshotMultiplier = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire = 600.0f; //bullets per minute

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ManaUsedPerShot = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool bCanAutoFire;

	/****************Particle Effects*******************/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastTimeFired;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	void PlayFireEffects(FVector TracerEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UFUNCTION()
	void OnRep_HitScanTrace();

public:	
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	virtual void StartFire();
	virtual void StopFire();
};
