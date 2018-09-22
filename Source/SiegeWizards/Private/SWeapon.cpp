// Fill out your copyright notice in the Description page of Project Settings.

//#include "SWeapon.h"
#include "../Public/SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "SiegeWizards.h"

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

//TODO make this class not hitscan, rather a blueprint to override for launcher and other kinds of weapons
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
	QueryParams.bReturnPhysicalMaterial = true;

	//Particle "Target" parameter
	FVector TracerEndPoint = TraceEnd;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation, TraceEnd, COLLISION_WEAPON, QueryParams)) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Something!!"));
		
		AActor* ActorHit = Hit.GetActor();

		EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

		//if we hit vulnerable location (head)

		float ActualDamage = BaseDamage;
		if (SurfaceType == SURFACE_FLESHVULNERABLE) 
		{
			ActualDamage *= HeadshotMultiplier;
		}
		
		UGameplayStatics::ApplyPointDamage(ActorHit, ActualDamage, ShotDirection, Hit, Owner->GetInstigatorController(), this, DamageType);


		UParticleSystem* SelectedEffect = nullptr;
		switch (SurfaceType) 
		{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
		}

		if (SelectedEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect , Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}

		TracerEndPoint = Hit.ImpactPoint;
	}

	if (DebugWeaponDrawing > 0) 
	{
		DrawDebugLine(GetWorld(), EyesLocation, TraceEnd, FColor::Emerald, false, 1.0f, 0, 1.0f);
	}

	PlayFireEffects(TracerEndPoint);

	LastTimeFired = GetWorld()->TimeSeconds;
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

void ASWeapon::StartFire()
{
	float TimeBetweenShots = 60.0f / RateOfFire; //RPM converted into Rounds per Second

	//check to make sure you can't just spam faster than the weapon can shoot
	if (GetWorld()->TimeSeconds - LastTimeFired >= TimeBetweenShots) 
	{
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, 0.0f);

	}
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

