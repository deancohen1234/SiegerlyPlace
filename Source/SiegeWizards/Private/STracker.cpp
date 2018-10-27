// Fill out your copyright notice in the Description page of Project Settings.

#include "STracker.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "SHealthComponent.h"
#include "SCharacter.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"


// Sets default values
ASTracker::ASTracker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComponent;
	MeshComponent->SetSimulatePhysics(true); 

	//sphere component used for self destruct radius
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetSphereRadius(200);
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("Health Component"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASTracker::HandleTakeDamage);

	bUseVelocityChange = false;
	MovementForce = 1000.0f;

	RequiredDistanceToTarget = 100.0f;

	BaseDamage = 100;
	DamageRadius = 200;

	SelfDamageInterval = 0.2f;
}

// Called when the game starts or when spawned
void ASTracker::BeginPlay()
{
	Super::BeginPlay();
	
	//server is in charge of telling tracker who to follow, and how to do it
	if (Role == ROLE_Authority) 
	{
		//find initial move to
		NextPathPoint = GetNextPathPoint();
	}
	
}

// Called every frame
void ASTracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority && bExploded == false)
	{

		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
		}

		else
		{
			//keeping moving towards next target
			FVector DirectionToPoint = NextPathPoint - GetActorLocation();
			DirectionToPoint.Normalize();
			DirectionToPoint *= MovementForce;

			MeshComponent->AddForce(DirectionToPoint, NAME_None, bUseVelocityChange);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + DirectionToPoint, 32.0f, FColor::Yellow, false, 0.0f, 0, 1.0f);
		}

		DrawDebugSphere(GetWorld(), NextPathPoint, 20.0f, 12, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}
}

FVector ASTracker::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (!ensure(PlayerPawn)) return FVector::ZeroVector;

	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), Cast<AActor>(PlayerPawn));

	if (!NavPath) return FVector::ZeroVector;

	if (NavPath->PathPoints.Num() > 1) 
	{
		//return next point in path
		return NavPath->PathPoints[1];
	}

	//failed to find path
	return GetActorLocation();
}

void ASTracker::HandleTakeDamage(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//explode on hit point 0

	if (MatInst == nullptr) 
	{
		MatInst = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));
	}

	if (MatInst) 
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}


	FString Name = HealthComp->GetOwner()->GetName();

	UE_LOG(LogTemp, Warning, TEXT("Tracker has %f of %s"), Health, *Name);

	if (Health <= 0) 
	{
		SelfDestruct();
	}
}

void ASTracker::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASTracker::SelfDestruct()
{

	if (bExploded) return;

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	MeshComponent->SetVisibility(false, true);
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Role == ROLE_Authority) 
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), DamageRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 16, FColor::Cyan, false, 1.0f, 0, 1.0f);

		//delete actor, give 2 seconds for client to spawn explosion
		SetLifeSpan(2.0f);
	}


	
}

void ASTracker::NotifyActorBeginOverlap(AActor * OtherActor)
{
	if (bStartedSelfDestruct == false)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn)
		{
			if (Role == ROLE_Authority) 
			{
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTracker::DamageSelf, SelfDamageInterval, true, 0.0f);
			}
			//we overlapped with the player

			//start tick 20 damage every .5 seconds

			bStartedSelfDestruct = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}
}