// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

namespace
{
	int32 DebugWeaponDrawing = 0;
	FAutoConsoleVariableRef CVarDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines for Weapons"), ECVF_Cheat);
}

// Sets default values
ASWeapon::ASWeapon()
{
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	const FVector TraceEndPoint = TraceWeaponFireAndApplyDamage();
	
	PlayFireEffects(TraceEndPoint);
}

// Trace the world, from pawn eyes to crosshair location and apply result
FVector ASWeapon::TraceWeaponFireAndApplyDamage()
{
	const AActor& MyOwner = *GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner.GetActorEyesViewPoint(EyeLocation, EyeRotation);

	const FVector DefaultTraceEnd = EyeLocation + (EyeRotation.Vector() * LineTraceDistance);
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

	if (DebugWeaponDrawing > 0)
	{
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.f, 0, 1.f);
	}

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

void ASWeapon::PlayFireEffects(const FVector& TraceEndPoint) const
{
	if (TracerEffect)
	{
		const FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		if (auto&& TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation))
		{
			TracerComponent->SetVectorParameter(TracerTargetName, TraceEndPoint);
		}
	}

	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);
	}
}

void ASWeapon::PlayImpactEffect(const FHitResult& HitResult) const
{
	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}
}