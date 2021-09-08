// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USHealthComponent;
class ASWeapon;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class COOPSHOOTER_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Override the Pawn View location to return the Camera location for the line tracing of the Weapon
	virtual FVector GetPawnViewLocation() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();
	void BeginZoom();
	void EndZoom();

	void StartFire();
	void StopFire();

	UFUNCTION()
	void OnHealthChanged(float Health, float HealthDelta, 
		const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV = 65.f;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClamMin = 0.1, ClampMax = 100.f))
	float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StartingWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName = "WeaponSocket";

	float DefaultFOV;

	UPROPERTY(Replicated)
	ASWeapon* CurrentWeapon;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bIsDead;

private:
	void SetupThirdPersonCamera();
	void SpawnDefaultWeapon();
};
