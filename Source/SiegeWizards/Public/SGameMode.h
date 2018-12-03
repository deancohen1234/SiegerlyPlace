// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SIEGEWIZARDS_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	TArray<int> TeamSpots;

	int GetRandomTeamIndex();
	
public:
	ASGameMode();

	virtual void StartPlay() override;
	
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	virtual void InitSeamlessTravelPlayer(AController* NewController) override;

	virtual void PostSeamlessTravel() override;
};
