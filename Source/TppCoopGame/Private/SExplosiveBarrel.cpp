// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplosiveBarrel.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComp;
	//����Ϊ�����壬�þ������Ӱ������,��������Ͱ��ըʱ.
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComp->SetSimulatePhysics(true);

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;//��ֹtick�Զ����£�ֻ�е���FireImpulse()��ʱ��Ÿ��¡�
	RadialForceComp->bIgnoreOwningActor = true;//��������

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

	ExplosionImpulse = 400;

	//�������Ժ�λ��ͬ������.
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}


void ASExplosiveBarrel::OnHealthChanged(USHealthComponent * OwningHealthComp, float Health, float HealthDelta,
	const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (bExploded==true)
	{
		return;
	}
	if (Health <= 0.0f)
	{
		bExploded = true;
		OnRep_Exploded();

		//������������
		FVector BoostIntensity = FVector::UpVector*ExplosionImpulse;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

		//������ը����
		RadialForceComp->FireImpulse();
	}

}


void ASExplosiveBarrel::OnRep_Exploded()
{
	//����������Ч
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	//�ı����
	MeshComp->SetMaterial(0, ExplodedMaterial);
}


void ASExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASExplosiveBarrel, bExploded);
}