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

	//�Ƿ��Ѿ���ը
	bool bExploded;

	/*������ըʱʩ����Ͱ���ϵĳ�����ʹ������һ��*/
	UPROPERTY(EditDefaultsOnly, Category = "FX")
		float ExplosionImpulse;

	//������ֵΪ0ʱ�����ӿ�ʼ��ˣ
	UPROPERTY(EditDefaultsOnly, Category = "FX")
		UParticleSystem *ExplosionEffect;

	/*�������滻ԭ���ʵĲ���һ����ը(һ����ڵİ汾)*/
	UPROPERTY(EditDefaultsOnly, Category = "FX")
		UMaterialInterface *ExplodedMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
			class AController* InstigatedBy, AActor* DamageCauser);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
