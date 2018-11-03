// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "Engine/World.h"
#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"


void ASGameMode::StartPlay() 
{
	Super::StartPlay();

}

FString ASGameMode::InitNewPlayer(class APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	ASCharacter* Character = Cast<ASCharacter>(NewPlayerController->GetPawn());

	if (NewPlayerController->GetPawn()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Pawn Name is %s"), *NewPlayerController->GetPawn()->GetName());
	}

	FString Result = "";

	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is alive"));
		if (Character->GetIsAttackingTeam())
		{
			//player is on attack
			Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, "Attackers");
		}
		else
		{
			//player is on defense
			Result = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, "Defenders");
		}
	}

	return Result;
}



