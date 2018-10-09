// Fill out your copyright notice in the Description page of Project Settings.

//#include "SCharacter.h"
#include "../Public/SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "SHealthComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeapon.h"
#include "Net/UnrealNetwork.h"
#include "SiegeWizards.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Component"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("Health Component"));

	GetMovementComponent()->GetNavAgentProperties()->bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	WeaponAttachSocketName = "WeaponSocket";

	LeanTranslationAmount = 3.0f;
	LeanRotationAmount = 15.0f;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComponent->FieldOfView;
	DefaultCameraPosition = CameraComponent->RelativeLocation;
	DefaultCameraRotation = CameraComponent->RelativeRotation;

	HealthComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	//only sets up weapon when server
	if (Role == ROLE_Authority) 
	{
		SetCurrentWeapon(PrimaryWeapon);
	}

	CurrentMana = MaxMana;


	bDied = false;
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

	PlayerInputComponent->BindAction("LeanLeft", IE_Pressed, this, &ASCharacter::LeanLeft);
	PlayerInputComponent->BindAction("LeanRight", IE_Pressed, this, &ASCharacter::LeanRight);
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

void ASCharacter::LeanLeft() 
{

	if (CameraComponent->RelativeLocation.Equals(DefaultCameraPosition)) 
	{
		//lean camera left
		//CameraComponent->SetRelativeLocationAndRotation(FVector(-LeanTranslationAmount, 0, 0), FQuat(FRotator(0, 0, -LeanRotationAmount)), true);

		FVector CameraPosition = DefaultCameraPosition + FVector(0, -LeanTranslationAmount, 0);
		FQuat CameraRotation = FQuat(DefaultCameraRotation + FRotator(0, 0, -LeanRotationAmount));

		CameraComponent->SetRelativeLocationAndRotation(CameraPosition, CameraRotation, true);
	}

	else 
	{
		//lean camera back
		CameraComponent->SetRelativeLocationAndRotation(DefaultCameraPosition, FQuat(DefaultCameraRotation), true);

	}
}

void ASCharacter::LeanRight()
{
	if (CameraComponent->RelativeLocation.Equals(DefaultCameraPosition))
	{
		//lean camera right
		FVector CameraPosition = DefaultCameraPosition + FVector(0, LeanTranslationAmount, 0);
		FQuat CameraRotation = FQuat(DefaultCameraRotation + FRotator(0, 0, LeanRotationAmount));

		CameraComponent->SetRelativeLocationAndRotation(CameraPosition, CameraRotation, true);
	}

	else
	{
		//lean camera right
		CameraComponent->SetRelativeLocationAndRotation(DefaultCameraPosition, FQuat(DefaultCameraRotation), true);

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

void ASCharacter::OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (HealthComp) 
	{
		if (Health <= 0 && !bDied) 
		{
			//Die

			bDied = true;

			GetMovementComponent()->StopMovementImmediately();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			DetachFromControllerPendingDestroy();

			SetLifeSpan(5.0f);
		}
	}
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

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	//bDied needed to be replicated for the death animation
	DOREPLIFETIME(ASCharacter, bDied);
}

