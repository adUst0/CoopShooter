// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

namespace
{
	float LINE_TRACE_DISTANCE = 10000;
}

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	const FVector TraceEndPoint = TraceWeaponFire();

	PlayMuzzleEffect();
	PlayTracerEffect(TraceEndPoint);
}

// Trace the world, from pawn eyes to crosshair location and apply result
FVector ASWeapon::TraceWeaponFire()
{
	const AActor& MyOwner = *GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner.GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const FVector DefaultTraceEnd = EyeLocation + (EyeRotation.Vector() * LINE_TRACE_DISTANCE);
	FVector ActualTraceEnd = DefaultTraceEnd;

	FHitResult HitResult;
	if (HadBlockingHit(HitResult, EyeLocation, DefaultTraceEnd))
	{
		ApplyDamage(HitResult, EyeRotation);

		ActualTraceEnd = HitResult.ImpactPoint;
	}

	return ActualTraceEnd;
}

bool ASWeapon::HadBlockingHit(FHitResult& HitResult, const FVector& EyeLocation, const FVector& TraceEnd) const
{
	const AActor& MyOwner = *GetOwner();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(&MyOwner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true; // Trace against each individual triangle of the hit mesh. This is more expensive but gives the exact location of the hit

	//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.f, 0, 1.f);

	return GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Visibility, QueryParams);
}

void ASWeapon::ApplyDamage(const FHitResult& HitResult, const FRotator& EyeRotation)
{
	const AActor& MyOwner = *GetOwner();
	AActor* HitActor = HitResult.GetActor();

	const FVector ShotDirection = EyeRotation.Vector();

	UGameplayStatics::ApplyPointDamage(HitActor, 20.f, ShotDirection, HitResult, MyOwner.GetInstigatorController(), this, DamageType);

	PlayImpactEffect(HitResult);
}

void ASWeapon::PlayMuzzleEffect() const
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);
	}
}

void ASWeapon::PlayTracerEffect(const FVector& TraceEndPoint) const
{
	if (TracerEffect)
	{
		const FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		if (auto&& TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation))
		{
			TracerComponent->SetVectorParameter(TracerTargetName, TraceEndPoint);
		}
	}
}

void ASWeapon::PlayImpactEffect(const FHitResult& HitResult) const
{
	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}
}

// Called every frame
void ASWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

