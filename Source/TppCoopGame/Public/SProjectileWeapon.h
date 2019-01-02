// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sweapon.h"
#include "SProjectileWeapon.generated.h"




/**
 *
 */
UCLASS()
class TPPCOOPGAME_API ASProjectileWeapon : public ASweapon
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
		TSubclassOf<AActor> ProjectileClass;

protected:
	virtual void Fire()override;
};
