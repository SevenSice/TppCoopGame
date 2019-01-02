// Fill out your copyright notice in the Description page of Project Settings.

#include "Sweapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
// Sets default values
ASweapon::ASweapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	//����ģ���µĵĲ����"MuzzleSocket"����Ȼ�޷�ʶ��
	MuzzleSocketName = "MuzzleSocket";
	//������Ч�µ����ֽ�Target�����½�parameter name��Ϊ"Target"
	TraceTargetName = "Target";
}

// Called when the game starts or when spawned
void ASweapon::BeginPlay()
{
	Super::BeginPlay();

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

		//�С�Target�����ӵĲ�����
		FVector TraceEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			//���е�ʱ������˺���
			AActor *HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			if (ImpactEffect != nullptr)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
			//���������ʱ���е㸲�ǵ������յ㡣
			TraceEndPoint = Hit.ImpactPoint;
		}
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

		//�����Ч
		if (MuzzuleEffect != nullptr)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzuleEffect, MeshComp, MuzzleSocketName);
		}
		if (TraceEffect!=nullptr)
		{
			FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

			UParticleSystemComponent *TraceComp= UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TraceEffect, MuzzleLocation);

			if (TraceComp)
			{
				TraceComp->SetVectorParameter(TraceTargetName, TraceEndPoint);
			}
		}
	}
}

// Called every frame
void ASweapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

