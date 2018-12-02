// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USHealthComponent;
class ASWeapon;
class ASTracker;

UCLASS()
class SIEGEWIZARDS_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:

	//////////////*Functions*////////////////

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	void SwapWeapon();

	void SetCurrentWeapon(TSubclassOf<ASWeapon> NewWeapon);

	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();

	void WizardJump();

	void BeginZoom();
	void EndZoom();

	void StartFire();
	void StopFire();
	void ReloadWeapon();
	void ThrowSpecial();

	void LeanLeft();
	void LeanRight();

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	////////////*Variables*//////////////
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	float MaxMana = 100;

	UPROPERTY(Replicated)
	float CurrentMana; 

	UPROPERTY(Replicated)
	ASWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASWeapon> PrimaryWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASWeapon> SecondaryWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASTracker> SpecialProjectile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	bool bIsUsingPrimary = true;

	bool bWantsToZoom = false;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	float ZoomedInFOV = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming", meta = (ClampMin = 0.0, ClampMax = 100.0))
	float ZoomSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	float LeanRotationAmount; //in degrees

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	float LeanTranslationAmount; //in units in direction

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "TeamSetup")
	bool bIsAttackingTeam;

	float DefaultFOV;
	FVector DefaultCameraPosition;
	FRotator DefaultCameraRotation;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapons")
	FName WeaponAttachSocketName;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwapWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerThrowSpecial();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReload();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	virtual void Destroyed() override;

	void UseMana(float ManaUsed);

	float GetMana();

	bool GetIsAttackingTeam() const;

	bool IsPlayerDead() const;

	UFUNCTION(BlueprintCallable)
	FString GetFormattedAmmoString() const;
};
