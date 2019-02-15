// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "Net/UnrealNetWork.h"
#include "TimerManager.h"
// Sets default values
ASPowerupActor::ASPowerupActor()
{

	PowerupInterval = 0.0f;
	TotalNrOfTicks = 0;

	TicksProcessed = 0;

	SetReplicates(true);
}


void ASPowerupActor::OnTickPowerup()
{
	//���¼���+1
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNrOfTicks)
	{

		OnExpired();

		//�����ʱ��
		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

//������ͼ����ʵ�ֵĹ���
void ASPowerupActor::OnRep_PowerupActive()
{
	
	OnPowerupStateChange(bIsPowerupActive);
}

void ASPowerupActor::ActivatePowerup()
{	
	OnActivated();


	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
}


void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
