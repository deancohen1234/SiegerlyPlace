// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/SProjectileLauncher.h"
#include "Kismet/GameplayStatics.h"
#include "SCharacter.h"
#include "../Public/AProjectile.h"


// Sets default values
ASProjectileLauncher::ASProjectileLauncher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASProjectileLauncher::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASProjectileLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASProjectileLauncher::Fire() 
{
	AActor* Owner = GetOwner();

	ASCharacter* Player = Cast<ASCharacter>(Owner);

	if (Player)
	{
		//if player is out of mana
		if (Player->GetMana() <= 0)
		{
			return;
		}

		Player->UseMana(ManaUsedPerShot);
	}

	if (!Owner || !ProjectileBlueprint) return;

	FVector EyesLocation;
	FRotator EyesRotation;

	Owner->GetActorEyesViewPoint(EyesLocation, EyesRotation);

	FVector SpawnLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
	FRotator SpawnRotation = MeshComponent->GetSocketRotation(MuzzleSocketName);

	FActorSpawnParameters Parameters;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AAProjectile* Projectile = GetWorld()->SpawnActor<AAProjectile>(ProjectileBlueprint, SpawnLocation, EyesRotation, Parameters);

	Projectile->Launch(EyesRotation.Vector());

	//eventually factor this out
	//myowner is the player pawn
	APawn* MyOwner = Cast<APawn>(GetOwner());

	if (MyOwner)
	{
		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		if (PlayerController)
		{
			PlayerController->ClientPlayCameraShake(FireCameraShake);
		}
	}

	
}

