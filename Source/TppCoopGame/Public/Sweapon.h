// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sweapon.generated.h"

class UDamageType;
class USkeletalMeshComponent;
class UParticleSystem;
class UCameraShake;

//包含扫描射击武器的轨迹信息。
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()
public:
	//不能直接传枚举，需TEnumAsByte转换为二进制
	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
		FVector_NetQuantize TraceTo;
};

UCLASS()
class TPPCOOPGAME_API ASweapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASweapon();

protected:
	virtual void BeginPlay()override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent *MeshComp = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem *MuzzuleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem *DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem *FleshImpactEffect;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName TraceTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem *TraceEffect;



	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<UCameraShake> FireCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float BaseDamage = 0;

	FTimerHandle TimerHandle_TimeBetShots;

	float LastFireTime = 0;

	//每分钟发射子弹数量
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float RateOfFire = 0;

	//射击速率
	float TimeBetweenShots = 0;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
		FHitScanTrace HitScanTrace;

protected:
	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UFUNCTION()
		void OnRep_HitScanTrace();
public:

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void Fire();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();

	void StartFire();

	void StopFire();
};
