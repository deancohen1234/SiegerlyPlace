// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STracker.generated.h"

class UStaticMeshComponent;
class USHealthComponent;
class USphereComponent;
class USoundCue;
class ASPlayerController;
class UProjectileMovementComponent;

UCLASS()
class SIEGEWIZARDS_API ASTracker : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTracker();

private:
	AActor* TargetingActor = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = Projectile)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "Self-Destruct")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Self-Destruct")
	float DamageRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Self-Destruct")
	float SelfDamageInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Self-Destruct")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* ExplosionSound;

	FVector NextPathPoint;

	FVector GetNextPathPoint();

	//dynamic material to pulse
	UMaterialInstanceDynamic* MatInst;

	FTimerHandle TimerHandle_SelfDamage;

	void DamageSelf();

	bool bExploded;
	bool bStartedSelfDestruct;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void SelfDestruct();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	//when player throws tracker into battlefield
	void SetTrackerTarget(ASPlayerController* Owner, FVector ThrowDirection);

	void Throw(FVector AimDirection);
};
