// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	Health = 100.0f;
	DefaultHealth = 100.0f;
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AActor *MyOwner = GetOwner();
	if (MyOwner!=nullptr)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
	}

	DefaultHealth = Health;
}

void USHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage<=0)
	{
		return;
	}
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Warning, TEXT("Health:%s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}


//// Called every frame
//void USHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

