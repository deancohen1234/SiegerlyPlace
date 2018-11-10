// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

UENUM()
enum class ETeamStatus : uint8
{
	AttackingTeamDead,
	DefendingTeamDead,
	BothTeamsDead,
	NoTeamsDead
};
/**
 * 
 */
UCLASS()
class SIEGEWIZARDS_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	float TimeLeftInRound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Rounds")
	float DefaultRoundLength;

public:
	//virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Rounds")
	FString GetTimeLeftInRound();

	UFUNCTION(BlueprintCallable, Category = "Rounds")
	ETeamStatus GetTeamsStatus();

	UFUNCTION()
	void OnPlayerHealthChanged(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser);

	void EndGame();
};
