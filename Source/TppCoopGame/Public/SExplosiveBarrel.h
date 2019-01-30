// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class URadialForceComponent;
class USHealthComponent;
class UParticleSystem;

UCLASS()
class TPPCOOPGAME_API ASExplosiveBarrel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASExplosiveBarrel();
protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UStaticMeshComponent *MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		URadialForceComponent *RadialForceComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		USHealthComponent *HealthComp;

	/*当它爆炸时施加在桶网上的冲量，使它上升一点*/
	UPROPERTY(EditDefaultsOnly, Category = "FX")
		float ExplosionImpulse;

	//当生命值为0时，粒子开始玩耍
	UPROPERTY(EditDefaultsOnly, Category = "FX")
		UParticleSystem *ExplosionEffect;

	/*网格上替换原材质的材料一旦爆炸(一个变黑的版本)*/
	UPROPERTY(EditDefaultsOnly, Category = "FX")
		UMaterialInterface *ExplodedMaterial;

	//是否已经爆炸
	//ReplicatedUsing调用时进行复制，然后添加新函数OnRep_Exploded(),所以当bool值发生改变时，客户端会有反馈。
	UPROPERTY(ReplicatedUsing=OnRep_Exploded)
	bool bExploded;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnRep_Exploded();

	UFUNCTION()
		void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
			class AController* InstigatedBy, AActor* DamageCauser);

public:

};
