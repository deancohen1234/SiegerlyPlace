// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerController.h"
#include "SCharacter.h"

FString ASPlayerController::GetTeamName() const
{
	return TeamName;
}

void ASPlayerController::SetTeamName(FString Name)
{
	TeamName = Name;
}

bool ASPlayerController::IsControlledPlayerDead()
{
	ASCharacter* Character = Cast<ASCharacter>(GetPawn());

	if (Character) 
	{
		bool bIsDead = Character->IsPlayerDead();
		return bIsDead;
	}

	//if player doesn't exist then assume they are dead
	return true;
}
