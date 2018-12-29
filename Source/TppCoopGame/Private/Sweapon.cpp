// Fill out your copyright notice in the Description page of Project Settings.

#include "Sweapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ASweapon::ASweapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
}

// Called when the game starts or when spawned
void ASweapon::BeginPlay()
{
	Super::BeginPlay();

}

void ASweapon::Fire()
{
	//追踪角色双眼到屏幕准星的位置的方向。
	AActor *MyOwner = GetOwner();
	if (MyOwner != nullptr)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		//射击方位
		FVector ShotDirection = EyeRotation.Vector();

		//射线终点位置
		FVector TraceEnd = EyeLocation + (ShotDirection * 1000.0f);

		//射线碰撞设置。
		FCollisionQueryParams  QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true; /*Thur 为精准碰撞，false为简单碰撞*/


		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd,ECC_Visibility, QueryParams))
		{
			//击中的时候造成伤害。
			AActor *HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			if (ImpactEffect != nullptr)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
		}
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

		//射击特效
		if (MuzzuleEffect!=nullptr)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzuleEffect, MeshComp, MuzzleSocketName);
		}
		
	}
}

// Called every frame
void ASweapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

