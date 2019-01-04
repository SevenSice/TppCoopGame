// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplosiveBarrel.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComp;
	//设置为物理体，让径向分量影响我们,当附近的桶爆炸时.
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComp->SetSimulatePhysics(true);

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;//防止tick自动更新，只有调用FireImpulse()的时候才更新。
	RadialForceComp->bIgnoreOwningActor = true;//忽略自身。

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

	ExplosionImpulse = 400;
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
		
		//把物体往上推
		FVector BoostIntensity = FVector::UpVector*ExplosionImpulse;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

		//产生粒子特效
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

		//改变材质
		MeshComp->SetMaterial(0, ExplodedMaterial);

		//产生爆炸脉冲
		RadialForceComp->FireImpulse();
	}

}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

