// Fill out your copyright notice in the Description page of Project Settings.

#include "STracker.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "SHealthComponent.h"


// Sets default values
ASTracker::ASTracker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComponent;
	MeshComponent->SetSimulatePhysics(true); 

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("Health Component"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASTracker::HandleTakeDamage);

	bUseVelocityChange = false;
	MovementForce = 1000.0f;

	RequiredDistanceToTarget = 100.0f;

	BaseDamage = 100;
	DamageRadius = 200;
}

// Called when the game starts or when spawned
void ASTracker::BeginPlay()
{
	Super::BeginPlay();
	
	//find initial move to
	NextPathPoint = GetNextPathPoint();
}

// Called every frame
void ASTracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

FVector ASTracker::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), Cast<AActor>(PlayerPawn));

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

	//TODO pulse the material on hit

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

void ASTracker::SelfDestruct()
{

	if (bExploded) return;

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), DamageRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 16, FColor::Cyan, false, 1.0f, 0, 1.0f);

	//delete actor immediately
	Destroy();
}

