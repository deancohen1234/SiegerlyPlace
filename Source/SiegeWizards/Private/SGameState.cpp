// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameState.h"

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
	UE_LOG(LogTemp, Warning, TEXT("Time Left: %d:%d"), minutes, secondsFormatted);

	return formattedString;
}