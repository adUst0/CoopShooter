// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USoundCue;
class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UMatineeCameraShake;

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	
	UPROPERTY()
	FVector_NetQuantize TraceTo;

	UPROPERTY()
	uint8 ForceUpdate = 1;
};

UCLASS()
class COOPSHOOTER_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();
	virtual void BeginPlay() override;

	void StartFire();
	void StopFire();
protected:
	virtual void Fire();

	// When this is called on the client, will not run on the client but will push the request to the server
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UFUNCTION()
	void OnRep_HitScanTrace();


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components");
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float HeadshotDamageMultiplier = 4.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName = "MuzzleSocket"; // Check this on the Riffle Mesh

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName = "BeamEnd"; // Check this in P_SmokeTrail -> Emitters -> Target -> Details -> Target -> Target -> Distribution -> Parameter Name

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float LineTraceDistance = 10000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UMatineeCameraShake> FireCamShake;

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastFireTime;

	// RPM - Bullets per minute fired by the weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float RateOfFire = 600.f;

	// Derived from RateOfFire
	float SecondsBetweenShots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundCue* FireSound;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;
private:
	FVector TraceWeaponFireAndApplyDamage();
	bool HadBlockingHit(FHitResult& HitResult, const FVector& EyeLocation, const FVector& TraceEnd) const;
	void ApplyDamage(const FHitResult& HitResult, const FRotator& EyeRotation);
	void PlayFireEffects(const FVector& TraceEndPoint) const;
	void PlayImpactEffect(EPhysicalSurface SurfaceType, FVector ImpactPoint) const;
};
