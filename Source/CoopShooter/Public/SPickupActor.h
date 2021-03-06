// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

class ASPowerupActor;
class USphereComponent;

UCLASS()
class COOPSHOOTER_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Respawn();


	UPROPERTY(VisibleAnywhere, Category = "Componentts")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Componentts")
	UDecalComponent* DecalComponent;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	TSubclassOf<ASPowerupActor> PowerupClass;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	float CooldownDuration = 10.f;

	ASPowerupActor* PowerupInstance;

	FTimerHandle TimerHandle_RespawnTimer;
};
