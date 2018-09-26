// Fill out your copyright notice in the Description page of Project Settings.

//#include "SCharacter.h"
#include "../Public/SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeapon.h"
#include "SiegeWizards.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Component"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentProperties()->bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	WeaponAttachSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComponent->FieldOfView;

	SetCurrentWeapon(PrimaryWeapon);

	CurrentMana = MaxMana;
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{

	//float CurrentFOV = bWantsToZoom ? ZoomedInFOV : DefaultFOV;
	float GoalFOV = bWantsToZoom ? ZoomedInFOV : DefaultFOV;

	float DesiredFOV = FMath::Lerp(CameraComponent->FieldOfView, GoalFOV, DeltaTime * ZoomSpeed);

	CameraComponent->SetFieldOfView(DesiredFOV);

	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::WizardJump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::ReloadWeapon);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComponent) 
	{
		return CameraComponent->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::WizardJump()
{
	Jump();
}

void ASCharacter::BeginCrouch() 
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::BeginZoom() 
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::SwapWeapon() 
{
	bIsUsingPrimary = !bIsUsingPrimary;

	if (bIsUsingPrimary) 
	{
		SetCurrentWeapon(PrimaryWeapon);
	}
	else 
	{
		SetCurrentWeapon(SecondaryWeapon);
	}
}

void ASCharacter::SetCurrentWeapon(TSubclassOf<ASWeapon> NewWeapon) 
{
	if (CurrentWeapon) 
	{
		CurrentWeapon->Destroy();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(NewWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		if (CurrentMana <= 0) 
		{
			CurrentWeapon->StopFire();
			return;
		}

		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::ReloadWeapon() 
{
	CurrentMana = MaxMana;
}

void ASCharacter::UseMana(float ManaAmount) 
{
	CurrentMana = FMath::Clamp(CurrentMana - ManaAmount, 0.0f, MaxMana);
}

float ASCharacter::GetMana() 
{
	return CurrentMana;
}

FString ASCharacter::GetFormattedAmmoString() const
{
	FString Text = FString::SanitizeFloat(CurrentMana);
	Text += "/";
	Text += FString::SanitizeFloat(MaxMana);

	return Text;
}

