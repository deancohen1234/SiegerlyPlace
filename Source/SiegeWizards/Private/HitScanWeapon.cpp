// Fill out your copyright notice in the Description page of Project Settings.

#include "HitScanWeapon.h"
#include "SCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SiegeWizards.h"


void AHitScanWeapon::Fire()
{
	AActor* Owner = GetOwner();

	if (!Owner) return;

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

	FVector EyesLocation;
	FRotator EyesRotation;

	Owner->GetActorEyesViewPoint(EyesLocation, EyesRotation);

	FVector ShotDirection = EyesRotation.Vector();

	FVector TraceEnd = EyesLocation + (ShotDirection * 10000);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	//Particle "Target" parameter
	FVector TracerEndPoint = TraceEnd;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
	{
		AActor* ActorHit = Hit.GetActor();

		EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

		//if we hit vulnerable location (head)

		float ActualDamage = BaseDamage;
		if (SurfaceType == SURFACE_FLESHVULNERABLE)
		{
			ActualDamage *= HeadshotMultiplier;
		}

		TracerEndPoint = Hit.ImpactPoint;

		FActorSpawnParameters Parameters;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FRotator TentacleRotation = Hit.Normal.Rotation();
		TentacleRotation.Add(270.0f, 0.0f, 0.0f);

		AActor* Tentacles = GetWorld()->SpawnActor<AActor>(SpawningActor, Hit.ImpactPoint, TentacleRotation, Parameters);
	}

	PlayFireEffects(TracerEndPoint);

	LastTimeFired = GetWorld()->TimeSeconds;
}