// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

#include "SHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent Component"));
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody); // Let radial component affect us
	RootComponent = MeshComponent;

	HealthComponent = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(MeshComponent);
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->bAutoActivate = false; // Prevent component from ticking, and only use FireImpulse() instead
	RadialForceComponent->bIgnoreOwningActor = true; // ignore self
	RadialForceComponent->Radius = 250;

	SetReplicates(true);
	AActor::SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
}

void ASExplosiveBarrel::OnHealthChanged(float Health, float HealthDelta,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bHasExploded)
	{
		bHasExploded = true;
		OnRep_Exploded(); // Call this function also on the server to play the local UI effects

		// This will be replicated to the clients because of AActor::SetReplicateMovement(true) from the constructor
		// If it has no gameplay importance, we can disable it for optimization and apply the impulse on the client but positions may not be exactly the same on all machines
		{
			// Boost the barrel upwards
			FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
			MeshComponent->AddImpulse(BoostIntensity, NAME_None, true);

			// Blast away nearby physics actors
			RadialForceComponent->FireImpulse();

			TArray<AActor*> IgnoredActors;
			IgnoredActors.Add(this);
			UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);
		}
	}
}

void ASExplosiveBarrel::OnRep_Exploded()
{
	MeshComponent->SetMaterial(0, ExplodedMaterial);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to every client that is connected to us
	DOREPLIFETIME(ASExplosiveBarrel, bHasExploded);
}