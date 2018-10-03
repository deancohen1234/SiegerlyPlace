// Fill out your copyright notice in the Description page of Project Settings.

#include "STracker.h"


// Sets default values
ASTracker::ASTracker()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void ASTracker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTracker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

