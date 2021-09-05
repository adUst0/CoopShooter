// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

UCLASS()
class COOPSHOOTER_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components");
	USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName MuzzleSocketName = "MuzzleSocket"; // Check this on the Riffle Mesh

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName TracerTargetName = "BeamEnd"; // Check this in P_SmokeTrail -> Emitters -> Target -> Details -> Target -> Target -> Distribution -> Parameter Name

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    UParticleSystem* TracerEffect;

private:
	FVector TraceWeaponFire();
	bool HadBlockingHit(FHitResult& HitResult, const FVector& EyeLocation, const FVector& TraceEnd) const;
    void ApplyDamage(const FHitResult& HitResult, const FRotator& EyeRotation);
	void PlayMuzzleEffect() const;
	void PlayTracerEffect(const FVector& TraceEndPoint) const;
	void PlayImpactEffect(const FHitResult& HitResult) const;
};
