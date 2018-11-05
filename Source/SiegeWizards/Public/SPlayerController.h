// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SIEGEWIZARDS_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	
protected:


	UPROPERTY(EditDefaultsOnly, Category = "Teams")
	FString TeamName;

public:

	UFUNCTION(BlueprintCallable, Category = "Teams")
	FString GetTeamName() const;

	UFUNCTION(BlueprintCallable, Category = "Teams")
	void SetTeamName(FString Name);
};
