// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"


void ASGameMode::StartPlay() 
{
	Super::StartPlay();

	UE_LOG(LogTemp, Warning, TEXT("Game Mode Starting"));
}

