// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/SProjectileLauncher.h"
#include "Kismet/GameplayStatics.h"
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

	if (!Owner || !ProjectileBlueprint) return;

	FVector EyesLocation;
	FRotator EyesRotation;

	Owner->GetActorEyesViewPoint(EyesLocation, EyesRotation);

	FVector SpawnLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
	FRotator SpawnRotation = MeshComponent->GetSocketRotation(MuzzleSocketName);

	FActorSpawnParameters Parameters;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AAProjectile* Projectile = GetWorld()->SpawnActor<AAProjectile>(ProjectileBlueprint, SpawnLocation, EyesRotation, Parameters);

	Projectile->Launch(1000);
}

