// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

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

	MuzzleSocketName = "MuzzleSocket";
}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Trace the world, from pawn eyes to crosshair location
void ASWeapon::Fire()
{
	FHitResult HitResult;
	if (HadBlockingHit(HitResult))
	{
		ApplyDamage(HitResult);
	}

	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);
	}
}

bool ASWeapon::HadBlockingHit(FHitResult& HitResult) const
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner)
	{
		return false;
	}

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * LINE_TRACE_DISTANCE);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true; // Trace against each individual triangle of the hit mesh. This is more expensive but gives the exact location of the hit

	DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.f, 0, 1.f);

	return GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Visibility, QueryParams);
}

void ASWeapon::ApplyDamage(const FHitResult& HitResult)
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner)
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	const FVector ShotDirection = EyeRotation.Vector();

	UGameplayStatics::ApplyPointDamage(HitActor, 20.f, ShotDirection, HitResult, MyOwner->GetInstigatorController(), this, DamageType);

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

