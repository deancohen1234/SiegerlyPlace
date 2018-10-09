// Fill out your copyright notice in the Description page of Project Settings.

//#include "SWeapon.h"
#include "../Public/SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "SCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
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

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	bCanAutoFire = true;
}



//TODO make this class not hitscan, rather a blueprint to override for launcher and other kinds of weapons
void ASWeapon::Fire() 
{
	if (Role < ROLE_Authority) 
	{
		//client calls this, client does nothing, it just tells server what to do
		ServerFire();
	}

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

	EPhysicalSurface SurfaceType = SurfaceType_Default;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyesLocation, TraceEnd, COLLISION_WEAPON, QueryParams)) 
	{		
		AActor* ActorHit = Hit.GetActor();

		SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

		//if we hit vulnerable location (head)

		float ActualDamage = BaseDamage;
		if (SurfaceType == SURFACE_FLESHVULNERABLE) 
		{
			ActualDamage *= HeadshotMultiplier;
		}
		
		UGameplayStatics::ApplyPointDamage(ActorHit, ActualDamage, ShotDirection, Hit, Owner->GetInstigatorController(), this, DamageType);

		PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

		TracerEndPoint = Hit.ImpactPoint;
	}

	if (DebugWeaponDrawing > 0) 
	{
		DrawDebugLine(GetWorld(), EyesLocation, TraceEnd, FColor::Emerald, false, 1.0f, 0, 1.0f);
	}

	PlayFireEffects(TracerEndPoint);

	if (Role == ROLE_Authority) 
	{
		HitScanTrace.TraceTo = TracerEndPoint;
		HitScanTrace.SurfaceType = SurfaceType;
	}

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

void ASWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
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
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ASWeapon::ServerFire_Implementation() 
{
	Fire();
}

bool ASWeapon::ServerFire_Validate() 
{
	//used to prevent cheating in theory
	return true;
}

void ASWeapon::OnRep_HitScanTrace()
{
	//Play cosmetic effects
	PlayFireEffects(HitScanTrace.TraceTo);

	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}

void ASWeapon::StartFire()
{
	if (bCanAutoFire) 
	{
		float TimeBetweenShots = 60.0f / RateOfFire; //RPM converted into Rounds per Second

													 //check to make sure you can't just spam faster than the weapon can shoot
		if (GetWorld()->TimeSeconds - LastTimeFired >= TimeBetweenShots)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, TimeBetweenShots, true, 0.0f);

		}
	}

	else 
	{
		Fire();
	}
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

