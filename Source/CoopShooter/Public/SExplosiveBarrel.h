// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class URadialForceComponent;
class USHealthComponent;

UCLASS()
class COOPSHOOTER_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASExplosiveBarrel();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHealthChanged(float Health, float HealthDelta,
		const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);


	UFUNCTION()
	void OnRep_Exploded();


	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	URadialForceComponent* RadialForceComponent;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UMaterialInterface* ExplodedMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	float ExplosionImpulse = 400.f;

	UPROPERTY(ReplicatedUsing=OnRep_Exploded)
	bool bHasExploded = false;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionDamage = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionRadius = 200.f;
};
