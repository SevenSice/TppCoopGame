// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

enum class EWaveState :uint8;


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

	//当前波次中要生成的机器人数量
	int32 NrOfBotsToSpawn;

	//波数，第几波
	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;
protected:
	//关联蓝图生成单个机器人
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	//开始产生机器人
	void StartWave();

	//停止产生机器人
	void EndWave();

	//为下次生成设置定时器
	void PreparaForNextWave();

	void CheckWaveState();

	//检测是否还有玩家存活
	void CheakAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);
public:
	virtual void Tick(float DeltaSecond) override;

	virtual void StartPlay() override;
};
