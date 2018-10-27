// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameState.h"
#include "Kismet/GameplayStatics.h"

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

void ASGameState::EndGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Ending Game"));
	UGameplayStatics::OpenLevel(GetWorld(), FName("Main"));
}
