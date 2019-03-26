// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"


UENUM(BlueprintType)
enum class EWaveState:uint8
{
	WaitingToStart,

	WaveInProgress,

	// 不再生成机器人，等待玩家击杀剩下的机器人。
	WaitingToComplete,

	WaveComplete,

	GameOver,
};

/**
 * 
 */
UCLASS()
class TPPCOOPGAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:

	UFUNCTION()
		void OnRep_WaveState(EWaveState OldState);

	//申明为事件，在蓝图中实现
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
		void WaveStateChanged(EWaveState NewState, EWaveState OldState);

	//RepNotify只能在C++里面运行，所以创建一个函数SetWaveState()来过度。
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
		EWaveState WaveState;
public:


	void SetWaveState(EWaveState NewState);
};
