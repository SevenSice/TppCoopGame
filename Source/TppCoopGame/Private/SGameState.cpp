// Fill out your copyright notice in the Description page of Project Settings.

#include "SGameState.h"
#include "Net/UnrealNetwork.h"




void ASGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}

void ASGameState::SetWaveState(EWaveState NewState)
{
	if (Role==ROLE_Authority)
	{
		EWaveState oldState=WaveState;
		WaveState = NewState;

		//在服务器上面调用
		OnRep_WaveState(oldState);
	}
}


void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameState, WaveState);
}