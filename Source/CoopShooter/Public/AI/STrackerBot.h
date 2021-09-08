 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

 class USphereComponent;
class USHealthComponent;

UCLASS()
class COOPSHOOTER_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector GetNextPathPoint();

	void SelfDestruct();

	UFUNCTION()
	void HandleTakeDamage(float Health, float HealthDelta, 
		const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void DamageSelf();


	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent* HealhtComponent;

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float MovementForce = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange = false;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplosionEffect;

	// Dynamic matarial to pulse on damage
	UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionDamage = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ExplosionRadius = 200.f;

	bool bHasExploded = false;

	FTimerHandle TimerHandle_SelfDamage;
};
