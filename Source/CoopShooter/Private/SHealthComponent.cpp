// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"

// Sets default values
USHealthComponent::USHealthComponent()
{

}

// Called when the game starts or when spawned
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = StartingHealth;

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeDamage);
	}
}

void USHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, StartingHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(CurrentHealth));

	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

