// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TPPCOOPGAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:

public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void addScore(float ScoreDelta);
};
