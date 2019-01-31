// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem/Public/NavigationPath.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "SHealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SphereComponent.h"
// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	MeshComp->SetCanEverAffectNavigation(false);
	RootComponent = MeshComp;
	MeshComp->SetSimulatePhysics(true);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComponent"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComp->SetSphereRadius(200.0f);
	SphereComp->SetupAttachment(MeshComp);

	bUseVelocityChange = false;
	MovementForce = 1000.0f;
	RequireDistanceToTarget = 100.0f;

	ExplosionDamage = 40.0f;
	ExplosionRadius = 200.0f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
	//�ҵ��ƶ����λ��
	NextPathPoint = GetNextPathPoint();

}



FVector ASTrackerBot::GetNextPathPoint()
{
	//���Ի�ȡ���λ��
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	if (NavPath==nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NavPath is Null !!"));
		return PlayerPawn->GetActorLocation();
	}
	if (NavPath->PathPoints.Num()>1)
	{
		//�����¸����·��
		return NavPath->PathPoints[1];
	}
	return GetActorLocation();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent * OwningHealthComp, float Health, float HealthDelta,
	const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	//����ֵΪ0ʱ��ը

	//���к��������ʽ��˸
	if (MatInst==nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst!=nullptr)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0)
	{
		SelfDestruct();
	}
	UE_LOG(LogTemp, Warning, TEXT("Health �� %s"),*FString::SanitizeFloat(Health));
}


void ASTrackerBot::SelfDestruct()
{
	//������ը��Ч
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	//��ɷ�Χ�˺�
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius,nullptr, IgnoredActors,this,GetInstigatorController(),true);
	//����
	Destroy();
}


void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//��ɫ����һ����ľ���
	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequireDistanceToTarget)
	{
		//�����������
		NextPathPoint = GetNextPathPoint();
	}
	else
	{
		//����������һ��Ŀ��
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;

		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
	}
}
