// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "CoopShooter/CoopShooter.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PhysicsCore\Public\Chaos\ChaosEngineInterface.h"

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
	MeshComponent->SetCollisionResponseToChannel(COLLISION_CHANNEL_WEAPON, ECollisionResponse::ECR_Block);
}

void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	SecondsBetweenShots = 60 / RateOfFire;
}

void ASWeapon::StartFire()
{
	// Prevent spamming the LClick to get faster shots
	const float FirstDelay = FMath::Max(LastFireTime + SecondsBetweenShots - GetWorld()->TimeSeconds, 0.f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, SecondsBetweenShots, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASWeapon::Fire()
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	const FVector TraceEndPoint = TraceWeaponFireAndApplyDamage();
	
	PlayFireEffects(TraceEndPoint);

	LastFireTime = GetWorld()->TimeSeconds;
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
	QueryParams.bReturnPhysicalMaterial = true;

	if (DebugWeaponDrawing > 0)
	{
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.f, 0, 1.f);
	}

	return GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, COLLISION_CHANNEL_WEAPON, QueryParams);
}

void ASWeapon::ApplyDamage(const FHitResult& HitResult, const FRotator& EyeRotation)
{
	const AActor& MyOwner = *GetOwner();
	AActor* HitActor = HitResult.GetActor();

	const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
	float ActualDamage = BaseDamage;
	if (SurfaceType == SURFACE_FLESH_VULNERABLE)
	{
		ActualDamage *= HeadshotDamageMultiplier;
	}

	const FVector ShotDirection = EyeRotation.Vector();

	UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, HitResult, MyOwner.GetInstigatorController(), this, DamageType);

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

	if (APawn* MyOwner = Cast<APawn>(GetOwner()))
	{
		if (APlayerController* PC = Cast<APlayerController>(MyOwner->GetController()))
		{
			PC->ClientStartCameraShake(FireCamShake);
		}
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FireSound);
	}
}

void ASWeapon::PlayImpactEffect(const FHitResult& HitResult) const
{
	UParticleSystem* SelectedEffect = nullptr;
	const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());

	switch (SurfaceType)
	{
	case SURFACE_FLESH_DEFAULT:
		SelectedEffect = FleshImpactEffect;
		break;
	case SURFACE_FLESH_VULNERABLE:
		SelectedEffect = FleshImpactEffect; // TODO: Implement another effect
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}
}