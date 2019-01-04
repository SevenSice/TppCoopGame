// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class ASweapon;
class UCameraComponent;
class USpringArmComponent;
class USHealthComponent;
UCLASS()
class TPPCOOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void MoveForward(float Value);

	void MoveRight(float Value);

	//��ʼ�׷�
	void BeginCrouch();
	//�����׷�
	void EndCrouch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USpringArmComponent *SpringArmComp = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent *CameraComp = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp = nullptr;

	bool bWantsToZoom = false;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
		float ZoomInterpSpeed;

	//��Ϸ��ʼʱ��Ĭ�ϵ��ӽǷ�Χ
	float DefaultFOV;

	ASweapon *CurrentWeapon = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TSubclassOf<ASweapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
		FName WeaponAttachSocketName = "";

	//����Ƿ�����
	UPROPERTY(BlueprintReadOnly, Category = "Player")
		bool bDied;

protected:

	void StartFire();

	void StopFire();

	void BeginZoom();

	void EndZoom();

	UFUNCTION()
		void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
			class AController* InstigatedBy, AActor* DamageCauser);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;
};
