// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Engine/Classes/Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "TppCoopGame/TppCoopGame.h"
#include "SHealthComponent.h"
#include "Sweapon.h"
#include "Net/UnrealNetWork.h"
#include "Camera/CameraShake.h"

// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	WeaponAttachSocketName = "WeaponSocket";

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20.0f;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	//写在beginPlay里面，是防止有人在蓝图里面修改默认值，我们仍然能获得有效值。
	DefaultFOV = CameraComp->FieldOfView;

	if (Role==ROLE_Authority)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//生成默认武器
		CurrentWeapon = GetWorld()->SpawnActor<ASweapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
		if (CurrentWeapon != nullptr)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}

}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector()*Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector()*Value);
}
//开始蹲伏
void ASCharacter::BeginCrouch()
{
	Crouch();
}
//结束蹲伏
void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon != nullptr)
	{
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::OnHealthChanged(USHealthComponent * OwningHealthComp, float Health, float HealthDelta,
	const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Health <= 0 && bDied == false)
	{
		//Die !
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		//设置生命周期10秒。
		SetLifeSpan(10.0f);
	}
}


// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	//更新视野范围
	CameraComp->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp != nullptr)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	//DOREPLIFETIME(ASCharacter, bDied);
}