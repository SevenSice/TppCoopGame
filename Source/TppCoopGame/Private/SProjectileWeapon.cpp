// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileWeapon.h"
#include "Components/SkeletalMeshComponent.h"


void ASProjectileWeapon::Fire()
{
	//׷�ٽ�ɫ˫�۵���Ļ׼�ǵ�λ�õķ���
	AActor *MyOwner = GetOwner();
	if (MyOwner != nullptr && ProjectileClass != nullptr)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		//FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//��Ϊ��û����ǹ�����ĳ�����������ĳ���Ϊ˫�۵ĳ���
		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParameters);
	}
}