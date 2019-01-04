// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sweapon.generated.h"

class UDamageType;
class USkeletalMeshComponent;
class UParticleSystem;



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

	void PlayFireEffects(FVector TraceEnd);

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
public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void Fire();

	void StartFire();

	void StopFire();
};
