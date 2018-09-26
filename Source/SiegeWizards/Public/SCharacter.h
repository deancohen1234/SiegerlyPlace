// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;

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

	////////////*Variables*//////////////
	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	float MaxMana = 100;

	float CurrentMana; 

	ASWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASWeapon> PrimaryWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapons")
	TSubclassOf<ASWeapon> SecondaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	bool bIsUsingPrimary = true;

	bool bWantsToZoom = false;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	float ZoomedInFOV = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming", meta = (ClampMin = 0.0, ClampMax = 100.0))
	float ZoomSpeed = 10.0f;

	float DefaultFOV;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapons")
	FName WeaponAttachSocketName;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	void UseMana(float ManaUsed);

	float GetMana();

	UFUNCTION(BlueprintCallable)
	FString GetFormattedAmmoString() const;
};
