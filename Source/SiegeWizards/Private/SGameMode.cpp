// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "Engine/World.h"
#include "SCharacter.h"


void ASGameMode::StartPlay() 
{
	Super::StartPlay();


	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = *Iterator;
		if (PlayerController)
		{
			ASCharacter* Character = Cast<ASCharacter>(PlayerController->GetPawn());

			UE_LOG(LogTemp, Warning, TEXT("Game Mode Starting %d"), Character->GetIsAttackingTeam());

			if (Character->GetIsAttackingTeam()) 
			{
				//player is on attack
				PlayerController->StartSpot = FindPlayerStart(PlayerController, "Attackers");
			}
			else 
			{
				//player is on defense
				PlayerController->StartSpot = FindPlayerStart(PlayerController, "Defenders");
			}
		}
	}

}



