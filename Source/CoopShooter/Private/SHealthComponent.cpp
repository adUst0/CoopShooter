// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values
USHealthComponent::USHealthComponent()
{
	SetIsReplicatedByDefault(true);
}

// Called when the game starts or when spawned
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = StartingHealth;

	// Only hook if we are server so all the gameplay logic for health will be executed on the server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeDamage);
		}
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

	OnHealthChanged.Broadcast(CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	const float Damage = OldHealth - CurrentHealth;
	OnHealthChanged.Broadcast(CurrentHealth, Damage, nullptr, nullptr, nullptr);
}

void USHealthComponent::Heal(float HealAmount)
{
	if (HealAmount < 0 || CurrentHealth <= 0.f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, StartingHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(CurrentHealth), *FString::SanitizeFloat(StartingHealth))

	OnHealthChanged.Broadcast(CurrentHealth, -HealAmount, nullptr, nullptr, nullptr);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to every client that is connected to us
	DOREPLIFETIME(USHealthComponent, CurrentHealth);
}
