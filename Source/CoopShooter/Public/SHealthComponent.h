// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnHealthChangedSignature, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, class AActor*, DamageCauser);

UCLASS(ClassGroup=(COOP), meta=(BlueprintSpawnableComponent))
class COOPSHOOTER_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	USHealthComponent();

	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, 
		AController* InstigatedBy, AActor* DamageCauser);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float StartingHealth = 100.f;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "HealthComponent")
	float CurrentHealth;

};
