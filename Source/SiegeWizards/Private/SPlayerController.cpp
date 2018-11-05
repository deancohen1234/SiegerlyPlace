// Fill out your copyright notice in the Description page of Project Settings.

#include "SPlayerController.h"

FString ASPlayerController::GetTeamName() const
{
	return TeamName;
}

void ASPlayerController::SetTeamName(FString Name)
{
	TeamName = Name;
}
