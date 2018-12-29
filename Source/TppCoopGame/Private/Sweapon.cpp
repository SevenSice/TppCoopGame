// Fill out your copyright notice in the Description page of Project Settings.

#include "Sweapon.h"
#include "DrawDebugHelpers.h"
// Sets default values
ASweapon::ASweapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
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

		FVector TraceEnd = EyeLocation + (EyeRotation.Vector()*1000.0f);

		FCollisionQueryParams  QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true; /*Trur 为精准碰撞，false为简单碰撞*/


		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd,ECC_Visibility, QueryParams))
		{
			//击中的时候造成伤害。

		}
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
	}
}

// Called every frame
void ASweapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

