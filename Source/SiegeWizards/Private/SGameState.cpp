// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SPlayerController.h"

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
	TSubclassOf<ASPlayerController> PlayerControllersType;
	PlayerControllersType = ASPlayerController::StaticClass();
	TArray<AActor*> AllPlayerControllers;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerControllersType, AllPlayerControllers);

	int AttackersCount = 0;
	int DefendersCount = 0;

	//assumes all players are alive, if it can't find any attacker/defenders it returns proper enum
	for (int i = 0; i < AllPlayerControllers.Num(); i++)
	{
		ASPlayerController* PlayerController = Cast<ASPlayerController>(AllPlayerControllers[i]);

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