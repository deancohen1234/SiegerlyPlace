// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SPlayerController.h"

void ASGameState::StartPlay()
{
	//sets match to be as long as round length, from moment game starts
	DefaultRoundLength = GetServerWorldTimeSeconds() + DefaultRoundLength;
}

//called on UI for player on changed value
FString ASGameState::GetTimeLeftInRound()
{
	int seconds = (int)(DefaultRoundLength - GetServerWorldTimeSeconds());
	int minutes = FMath::FloorToInt(seconds / 60);

	int secondsFormatted = seconds % 60;

	FString formattedString = "TimeLeft: ";
	formattedString.AppendInt(minutes);
	formattedString += ":";
	formattedString.AppendInt(secondsFormatted);

	TimeLeftInRound = DefaultRoundLength - GetServerWorldTimeSeconds();

	if (seconds <= 0) 
	{
		EndGame();
	}

	return formattedString;
}

//TODO this function makes assumption there are only 4 players, 2 and 2. Its bad and should be made more dynamic
ETeamStatus ASGameState::GetTeamsStatus()
{
	//don't do logic unless you are server
	if (Role != ROLE_Authority) 
	{
		return ETeamStatus::NoTeamsDead;
	}

	int AttackersCount = 0;
	int DefendersCount = 0;
	UE_LOG(LogTemp, Warning, TEXT("Num Players: %d"), AllPlayers.Num());

	if (AllPlayers.Num() <= 2) 
	{
		return ETeamStatus::NoTeamsDead;
	}

	//assumes all players are alive, if it can't find any attacker/defenders it returns proper enum
	for (int i = 0; i < AllPlayers.Num(); i++)
	{
		ASPlayerController* PlayerController = Cast<ASPlayerController>(AllPlayers[i]);

		if (PlayerController)
		{
			FString TeamName = PlayerController->GetTeamName();

			if (TeamName == "Attackers")
			{
				if (!PlayerController->IsControlledPlayerDead()) 
				{
					AttackersCount++;
				}
			}
			else if (TeamName == "Defenders")
			{
				if (!PlayerController->IsControlledPlayerDead())
				{
					DefendersCount++;
				}			
			}
		}
	}

	if (AttackersCount == 0 && DefendersCount == 0)
	{
		return ETeamStatus::BothTeamsDead;
	}

	else if (AttackersCount == 0)
	{
		return ETeamStatus::AttackingTeamDead;
	}

	else if (DefendersCount == 0)
	{
		return ETeamStatus::DefendingTeamDead;
	}

	else
	{
		return ETeamStatus::NoTeamsDead;
	}


	//TODO Should return an error because should have returned previously
	return ETeamStatus::NoTeamsDead;
}

void ASGameState::OnPlayerHealthChanged(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Player Health Changed"));
	if (Role == ROLE_Authority)
	{
		//this should not need to be called on every frame
		ETeamStatus TeamStatus = GetTeamsStatus();
		//if one or both teams are dead
		if (TeamStatus < ETeamStatus::NoTeamsDead)
		{
			//set round to five seconds left
			DefaultRoundLength = GetServerWorldTimeSeconds() + 5;
		}

	}
}

void ASGameState::EndGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Ending Game"));
	UGameplayStatics::OpenLevel(GetWorld(), FName("Main"));
}