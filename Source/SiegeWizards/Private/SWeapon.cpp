// Fill out your copyright notice in the Description page of Project Settings.

//#include "SWeapon.h"
#include "../Public/SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines for Weapons"), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComponent;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "BeamEnd";
}

void ASWeapon::Fire() 
{
	AActor* Owner = GetOwner();

	if (!Owner) return;

	FVector EyesLocation;
	FRotator EyesRotation;

	Owner->GetActorEyesViewPoint(EyesLocation, EyesRotation);

	FVector ShotDirection = EyesRotation.Vector();

	FVector TraceEnd = EyesLocation + (ShotDirection * 10000);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;

	//Particle "Target" parameter
	FVector TracerEndPoint = TraceEnd;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation, TraceEnd, ECC_Visibility, QueryParams)) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Something!!"));
		
		AActor* ActorHit = Hit.GetActor();
		
		UGameplayStatics::ApplyPointDamage(ActorHit, 15.0f, ShotDirection, Hit, Owner->GetInstigatorController(), this, DamageType);

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}

		TracerEndPoint = Hit.ImpactPoint;
	}

	if (DebugWeaponDrawing > 0) 
	{
		DrawDebugLine(GetWorld(), EyesLocation, TraceEnd, FColor::Emerald, false, 1.0f, 0, 1.0f);
	}

	PlayFireEffects(TracerEndPoint);
}

void ASWeapon::PlayFireEffects(FVector TracerEndPoint) 
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* SpawnedTracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);

		if (SpawnedTracerComp)
		{
			SpawnedTracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}
}

