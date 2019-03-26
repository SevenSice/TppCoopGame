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

	// �������ɻ����ˣ��ȴ���һ�ɱʣ�µĻ����ˡ�
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

	//����Ϊ�¼�������ͼ��ʵ��
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
		void WaveStateChanged(EWaveState NewState, EWaveState OldState);

	//RepNotifyֻ����C++�������У����Դ���һ������SetWaveState()�����ȡ�
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
		EWaveState WaveState;
public:


	void SetWaveState(EWaveState NewState);
};
