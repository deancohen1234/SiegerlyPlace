// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STracker.generated.h"

class UStaticMeshComponent;
class USHealthComponent;

UCLASS()
class SIEGEWIZARDS_API ASTracker : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTracker();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RequiredDistanceToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	bool bUseVelocityChange;

	FVector NextPathPoint;

	FVector GetNextPathPoint();

	//dynamic material to pulse
	UMaterialInstanceDynamic* MatInst;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
};
