// Fill out your copyright notice in the Description page of Project Settings.

#include "Sweapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TppCoopGame.h"
#include "Public/TimerManager.h"
//定义一个控制台变量。
static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVarDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
ASweapon::ASweapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	//骨骼模型下的的插槽名"MuzzleSocket"，不然无法识别。
	MuzzleSocketName = "MuzzleSocket";

	//粒子特效下的名字叫Target属性下叫parameter name的为"Target"
	TraceTargetName = "Target";

	//基础伤害
	BaseDamage = 20.0f;

	//每分钟发射子弹数量
	RateOfFire = 600.0f;
}
void ASweapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
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
		QueryParams.bReturnPhysicalMaterial = true;//是否返回物理材质信息



		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			//击中的时候造成伤害。
			AActor *HitActor = Hit.GetActor();

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			float ActualDamage = BaseDamage;
			if (SurfaceType==SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			UParticleSystem *SelectedEffect = nullptr;
			//UE_LOG(LogTemp, Warning, TEXT("shake shake shake !%s"), &SurfaceType);
			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
				SelectedEffect = FleshImpactEffect;
				break;
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}
			if (SelectedEffect != nullptr)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

		}

		if (DebugWeaponDrawing > 0)
		{
				DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}
		PlayFireEffects(TraceEnd);

		LastFireTime = GetWorld()->TimeSeconds;
	}
}


void ASweapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds,0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetShots, this, &ASweapon::Fire,TimeBetweenShots, true, FirstDelay);
}

void ASweapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetShots);
}




void ASweapon::PlayFireEffects(FVector TraceEnd)
{
	//叫“Target”粒子的参数。
	FVector TraceEndPoint = TraceEnd;

	////当射击受阻时击中点覆盖掉射线终点。
	//TraceEndPoint = Hit.ImpactPoint;

	//射击特效
	if (MuzzuleEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzuleEffect, MeshComp, MuzzleSocketName);
	}

	if (TraceEffect != nullptr)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent *TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);

		if (TraceComp)
		{
			TraceComp->SetVectorParameter(TraceTargetName, TraceEndPoint);
		}
	}

	APawn *MyPawn = Cast<APawn>(GetOwner());
	if (MyPawn!=nullptr)
	{
		APlayerController *PC= Cast<APlayerController>(MyPawn->GetController());

		if (PC != nullptr)
		{
			//相机抖动
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}
}
