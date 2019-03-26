// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameMode.h"
#include "SGameState.h"
#include "TimerManager.h"
#include "SHealthComponent.h"
#include "Engine/World.h "

ASGameMode::ASGameMode()
{
	TimeBetweenWaves = 2.0f;

	GameStateClass = ASGameState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

}




void ASGameMode::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = WaveCount * 2;
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
	
	SetWaveState(EWaveState::WaveInProgress);
}


void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;
	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}


void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	SetWaveState(EWaveState::WaitingToComplete);
}


void ASGameMode::PreparaForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);
	
	SetWaveState(EWaveState::WaitingToStart);
}


void ASGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}

	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn==nullptr||TestPawn->IsPlayerControlled())
		{
			continue;
		}

		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}
	if (bIsAnyBotAlive == false)
	{
		SetWaveState(EWaveState::WaveComplete);
		PreparaForNextWave();
	}
}

void ASGameMode::CheakAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		APlayerController* PC = It->Get();

		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();

			USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
			{
				/*
				*  ensure()��ʾ������ִ�����ָ���������ͻ��ϣ��������ϵ㡣
				*/
				// ����һ��һ���
				return;
			}
		}
	}

	//����Ҵ��
	GameOver();
}

void ASGameMode::GameOver()
{
	EndWave();
	//@TODO:��ʾ�����Ϸ������

	SetWaveState(EWaveState::GameOver);
	UE_LOG(LogTemp, Warning, TEXT(" Game Over! Player Dead ! "));
}

void ASGameMode::SetWaveState(EWaveState NewState)
{
	ASGameState* GS = GetGameState<ASGameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}

void ASGameMode::Tick(float DeltaSecond)
{
	Super::Tick(DeltaSecond);

	CheckWaveState();

	CheakAnyPlayerAlive();
}


void ASGameMode::StartPlay()
{
	Super::StartPlay();

	PreparaForNextWave();
}
