// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "Engine/World.h"
#include "SPlayerController.h"
#include "Kismet/GameplayStatics.h"


ASGameMode::ASGameMode()
{
	TeamSpots = { 0, 1, 2, 3 };
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();
}

FString ASGameMode::InitNewPlayer(class APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	ASPlayerController* PlayerController = Cast<ASPlayerController>(NewPlayerController);

	FString Result = "";
	if (PlayerController)
	{
		//greater than 2, you are a Defender
		if (GetRandomTeamIndex() >= 2) 
		{
			PlayerController->SetTeamName("Defenders"); 
			Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, PlayerController->GetTeamName());
		}
		else 
		{
			PlayerController->SetTeamName("Attackers");
			Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, PlayerController->GetTeamName());
		}
	}

	return Result;
}

//returns value from TeamSpots and removes value so there are no duplicates
int ASGameMode::GetRandomTeamIndex() 
{
	int ArrayIndex = FMath::RandHelper(TeamSpots.Num());
	int TeamIndex = TeamSpots[ArrayIndex];

	TeamSpots.RemoveAt(ArrayIndex);

	return TeamIndex;
}