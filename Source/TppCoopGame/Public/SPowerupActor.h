// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"






UCLASS()
class TPPCOOPGAME_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();
protected:
	//ʱ����--���������͵��ߵ�ʱ����
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	//��������--���µ��ܴ���
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TotalNrOfTicks;

	FTimerHandle TimerHandle_PowerupTick;

	//��ǰ���µĴ���
	int32 TicksProcessed;

	//Keeps state of the power-up
	UPROPERTY(ReplicatedUsing = OnRep_PowerupActive)
	bool bIsPowerupActive;

protected:

	UFUNCTION()
	void OnTickPowerup();

	UFUNCTION()
	void OnRep_PowerupActive();

	//ϣ����������ͼ����ʵ��
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupStateChange(bool bNewIsActive);
public:	

	void ActivatePowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();
};
