// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "Engine/World.h"
#include "SPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SHealthComponent.h"
#include "SGameState.h"
#include "Kismet/KismetSystemLibrary.h"


ASGameMode::ASGameMode()
{
	TeamSpots = { 0, 1, 2, 3 };
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();

}

//doesn't get called on build apparently
FString ASGameMode::InitNewPlayer(class APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	ASPlayerController* PlayerController = Cast<ASPlayerController>(NewPlayerController);

	UKismetSystemLibrary::PrintString(GetWorld(), "Initing New Player", true, true, FLinearColor((0.0), (0.66000000000000031), 1.0f), 15.0f);

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

		ASGameState* GameState = GetGameState<ASGameState>();
		GameState->AllPlayers.Add(PlayerController);
	}

	return Result;
}

void ASGameMode::InitSeamlessTravelPlayer(AController * NewController)
{
	UKismetSystemLibrary::PrintString(GetWorld(), "Seamless Travel Call", true, true, FLinearColor((0.0), (0.66000000000000031), 1.0f), 15.0f);

	ASPlayerController* PlayerController = Cast<ASPlayerController>(NewController);

	if (!PlayerController) return;

	if (PlayerController->GetTeamName() != "Defenders" || PlayerController->GetTeamName() != "Attackers") 
	{
		//greater than 2, you are a Defender
		//TODO turn this into a function

		if (GetRandomTeamIndex() >= 2)
		{
			PlayerController->SetTeamName("Defenders");
		}
		else
		{
			PlayerController->SetTeamName("Attackers");
		}

		ASGameState* GameState = GetGameState<ASGameState>();
		GameState->AllPlayers.Add(PlayerController);
	}
}

//returns value from TeamSpots and removes value so there are no duplicates
int ASGameMode::GetRandomTeamIndex() 
{
	int ArrayIndex = FMath::RandHelper(TeamSpots.Num());
	int TeamIndex = TeamSpots[ArrayIndex];

	TeamSpots.RemoveAt(ArrayIndex);

	return TeamIndex;
}