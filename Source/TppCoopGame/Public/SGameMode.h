// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TPPCOOPGAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
		
		ASGameMode();
protected:

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	//��ǰ������Ҫ���ɵĻ���������
	int32 NrOfBotsToSpawn;

	//�������ڼ���
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;
protected:
	//������ͼ���ɵ���������
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();



	//��ʼ����������
	void StartWave();

	//ֹͣ����������
	void EndWave();

	//Ϊ�´��������ö�ʱ��
	void PreparaForNextWave();

	void CheckWaveState();
public:
	virtual void Tick(float DeltaSecond) override;

	virtual void StartPlay() override;
};