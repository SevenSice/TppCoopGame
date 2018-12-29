// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sweapon.generated.h"


class USkeletalMeshComponent;

UCLASS()
class TPPCOOPGAME_API ASweapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASweapon();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent *MeshComp = nullptr;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Fire();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
