// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASProjectile::ASProjectile()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetSimulatePhysics(true);
	RootComponent = MeshComponent;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ExplosionParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("ExplosionParticle"));
}

// Called when the game starts or when spawned
void ASProjectile::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &ASProjectile::Explode, 1.f);
}

void ASProjectile::Explode()
{
	if (ExplosionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionParticle, GetActorLocation());	    
	}

	UGameplayStatics::ApplyRadialDamage(this, ProjectileDamage, GetActorLocation(), ProjectileDamageRadius, DamageTypeClass, {}, this);

    DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileDamageRadius, 12, FColor::Red, false, 1.f, 0, 2);

    Destroy();
}

