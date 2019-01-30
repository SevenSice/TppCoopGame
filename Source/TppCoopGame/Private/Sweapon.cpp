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
#include "Net/UnrealNetWork.h"

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

	SetReplicates(true);

	//默认刷新率为100，最小刷新率为2.可能导致不太同步的情况。
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}
void ASweapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void ASweapon::OnRep_HitScanTrace()
{
	//播放外观效果
	// Play cosmetic FX
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASweapon::Fire()
{
	if (Role < ROLE_Authority)
	{
		ServerFire();
	}
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

		// Particle "Target" parameter
		FVector TraceEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			//击中的时候造成伤害。
			AActor *HitActor = Hit.GetActor();

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);
			TraceEndPoint = Hit.ImpactPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects(TraceEndPoint);

		if (Role == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TraceEndPoint;

		}

		LastFireTime = GetWorld()->TimeSeconds;
	}
}

//_Implementation在服务器上运行
void ASweapon::ServerFire_Implementation()
{
	Fire();
}

//验证（一般为检测是否作弊）
bool ASweapon::ServerFire_Validate()
{
	return true;
}

//开始射击
void ASweapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetShots, this, &ASweapon::Fire, TimeBetweenShots, true, FirstDelay);
}

//停止射击
void ASweapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetShots);
}

//播放开火特效
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
	if (MyPawn != nullptr)
	{
		APlayerController *PC = Cast<APlayerController>(MyPawn->GetController());

		if (PC != nullptr)
		{
			//相机抖动
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}
}

//播放击中特效
void ASweapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
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
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint,ShotDirection.Rotation());
	}
}

void ASweapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASweapon, HitScanTrace, COND_SkipOwner);

}