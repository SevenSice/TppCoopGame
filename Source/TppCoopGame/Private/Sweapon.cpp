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
//����һ������̨������
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

	//����ģ���µĵĲ����"MuzzleSocket"����Ȼ�޷�ʶ��
	MuzzleSocketName = "MuzzleSocket";

	//������Ч�µ����ֽ�Target�����½�parameter name��Ϊ"Target"
	TraceTargetName = "Target";

	//�����˺�
	BaseDamage = 20.0f;

	//ÿ���ӷ����ӵ�����
	RateOfFire = 600.0f;
}
void ASweapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
}

void ASweapon::Fire()
{
	//׷�ٽ�ɫ˫�۵���Ļ׼�ǵ�λ�õķ���
	AActor *MyOwner = GetOwner();
	if (MyOwner != nullptr)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		//�����λ
		FVector ShotDirection = EyeRotation.Vector();

		//�����յ�λ��
		FVector TraceEnd = EyeLocation + (ShotDirection * 1000.0f);

		//������ײ���á�
		FCollisionQueryParams  QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true; /*Thur Ϊ��׼��ײ��falseΪ����ײ*/
		QueryParams.bReturnPhysicalMaterial = true;//�Ƿ񷵻����������Ϣ



		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			//���е�ʱ������˺���
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
	//�С�Target�����ӵĲ�����
	FVector TraceEndPoint = TraceEnd;

	////���������ʱ���е㸲�ǵ������յ㡣
	//TraceEndPoint = Hit.ImpactPoint;

	//�����Ч
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
			//�������
			PC->ClientPlayCameraShake(FireCameraShake);
		}
	}
}
