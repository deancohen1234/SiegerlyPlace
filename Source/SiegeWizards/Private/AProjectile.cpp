// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/AProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AAProjectile::AAProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement Component"));
	ProjectileMovementComponent->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AAProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAProjectile::Launch(float Speed)
{
	ProjectileMovementComponent->Activate();
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);

	UE_LOG(LogTemp, Warning, TEXT("Launching"));
}

