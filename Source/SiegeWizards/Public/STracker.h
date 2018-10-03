// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STracker.generated.h"

class UStaticMeshComponent;

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

	FVector GetNextPathPoint();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
};
