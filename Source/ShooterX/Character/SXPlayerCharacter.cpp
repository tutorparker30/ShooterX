// SXPlayerCharacter.cpp


#include "Character/SXPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/SXInputConfig.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/SXAnimInstance.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "ShooterXPlayGround/SXCharacterMaterialManager.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Item/SXWeapon.h"
#include "SXPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Component/SXPickupComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "ShooterX.h"
#include "Controller/SXPlayerController.h"
#include "Gimmick/SXLandMine.h"
#include "Net/UnrealNetwork.h"

ASXPlayerCharacter::ASXPlayerCharacter()
{
	//PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.f;
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 50.f, 100.f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);
		// ControlRotation이 Pawn의 회전과 동기화되고,
		// Pawn의 회전이 SpringArm의 회전 동기화. 이로 인해 SetRelativeRotation()이 무의미.

	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	SpringArmComponent->bDoCollisionTest = true;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetAutoActivate(false);
}

void ASXPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController) == true)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (IsValid(Subsystem) == true)
		{
			Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
		}
	}
}

void ASXPlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentAimPitch);
}

void ASXPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(GetController()) == true)
	{
		PreviousAimPitch = CurrentAimPitch;

		FRotator ControlRotation = GetController()->GetControlRotation();
		float NormalizedPitch = FRotator::NormalizeAxis(ControlRotation.Pitch);
		CurrentAimPitch = FMath::Clamp(NormalizedPitch, -90.0f, 90.0f);
	}

	if (IsLocallyControlled() == true && PreviousAimPitch != CurrentAimPitch)
	{
		ServerRPCUpdateAimValue(CurrentAimPitch);
	}
}

void ASXPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent) == true)
	{
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Look, ETriggerEvent::Triggered, this, &ThisClass::InputLook);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->AttackMelee, ETriggerEvent::Started, this, &ThisClass::InputAttackMelee);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->AttackRanged, ETriggerEvent::Started, this, &ThisClass::InputAttackRanged);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->Menu, ETriggerEvent::Started, this, &ThisClass::InputMenu);
		EnhancedInputComponent->BindAction(PlayerCharacterInputConfig->SpawnLandMine, ETriggerEvent::Started, this, &ThisClass::InputSpawnLandMine);
	}
}

void ASXPlayerCharacter::InputMove(const FInputActionValue& InValue)
{
	FVector2D MovementVector = InValue.Get<FVector2D>();

	const FRotator ControlRotation = GetController()->GetControlRotation();
	const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);

	const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVector, MovementVector.X);
	AddMovementInput(RightVector, MovementVector.Y);
}

void ASXPlayerCharacter::InputLook(const FInputActionValue& InValue)
{
	if (IsValid(GetController()) == true)
	{
		FVector2D LookVector = InValue.Get<FVector2D>();

		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void ASXPlayerCharacter::InputAttackMelee(const FInputActionValue& InValue)
{
	if (GetCharacterMovement()->IsFalling() == true)
	{
		return;
	}

	if (0 == CurrentComboCount)
	{
		BeginAttack();
	}
	else
	{
		ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
		bIsAttackKeyPressed = true;
	}

	ServerRPCMeleeAttack();
}

void ASXPlayerCharacter::InputAttackRanged(const FInputActionValue& InValue)
{
	if (CanFire() == false)
	{
		return;
	}

	Fire();
}

void ASXPlayerCharacter::InputMenu(const FInputActionValue& InValue)
{
	ASXPlayerController* PlayerController = GetController<ASXPlayerController>();
	if (true == IsValid(PlayerController))
	{
		PlayerController->ToggleInGameMenu();
	}
}

void ASXPlayerCharacter::InputSpawnLandMine(const FInputActionValue& InValue)
{
	/*
	if (IsValid(LandMineClass) == true)
	{
		FVector SpawnedLocation = (GetActorLocation() + GetActorForwardVector() * 300.f) - FVector(0.f, 0.f, 90.f);
		ASXLandMine* SpawnedLandMine = GetWorld()->SpawnActor<ASXLandMine>(LandMineClass, SpawnedLocation, FRotator::ZeroRotator);
	}
	*/

	if (IsLocallyControlled() == true)
	{
		ServerRPCSpawnLandMine();
	}
}

void ASXPlayerCharacter::ServerRPCUpdateAimValue_Implementation(const float& InAimPitchValue)
{
	CurrentAimPitch = InAimPitchValue;
}

void ASXPlayerCharacter::OnMeshMaterialLoadCompleted(FSoftObjectPath Path01, FSoftObjectPath Path02)
{
	TSoftObjectPtr<UMaterialInstance> LoadedMaterialInstance01(Path01);
	TSoftObjectPtr<UMaterialInstance> LoadedMaterialInstance02(Path02);

	if (LoadedMaterialInstance01.IsValid() &&
		LoadedMaterialInstance02.IsValid() &&
		IsValid(GetMesh()))
	{
		GetMesh()->SetMaterial(1, LoadedMaterialInstance01.Get());
		GetMesh()->SetMaterial(0, LoadedMaterialInstance02.Get());
	}

	if (AssetStreamableHandle.IsValid())
	{
		AssetStreamableHandle->ReleaseHandle();
		AssetStreamableHandle.Reset();
	}
}

void ASXPlayerCharacter::InitializeCharacterMeshMaterial(const EPlayerColor InPlayerColor)
{
	const USXCharacterMaterialManager* CDO = GetDefault<USXCharacterMaterialManager>();
	if (IsValid(CDO) == false)
	{
		return;
	}

	int32 MaterialIndex = static_cast<int32>(InPlayerColor);
	MaterialIndex *= 2;

	CurrentPlayerCharacterMeshMaterialPath01 = CDO->PlayerCharacterMeshMaterialPaths[MaterialIndex];
	CurrentPlayerCharacterMeshMaterialPath02 = CDO->PlayerCharacterMeshMaterialPaths[MaterialIndex + 1];

	const FSoftObjectPath Path01 = CurrentPlayerCharacterMeshMaterialPath01;
	const FSoftObjectPath Path02 = CurrentPlayerCharacterMeshMaterialPath02;

	AssetStreamableHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
		{ Path01, Path02 },
		FStreamableDelegate::CreateUObject(
			this,
			&ThisClass::OnMeshMaterialLoadCompleted,
			Path01,
			Path02
		)
	);
}

bool ASXPlayerCharacter::CanFire() const
{
	if (0.f < GetCharacterMovement()->Velocity.Size())
	{
		return false;
	}

	if (IsValid(CurrentWeapon) == false)
	{
		return false;
	}

	if (IsValid(GetCurrentWeaponAttackAnimMontage()) == false)
	{
		return false;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) == false)
	{
		return false;
	}

	if (AnimInstance->Montage_IsPlaying(GetCurrentWeaponAttackAnimMontage()) == true)
	{
		return false;
	}

	return true;
}

void ASXPlayerCharacter::Fire()
{
	FVector MuzzleLocation = CurrentWeapon->GetPickupComponent()->GetSocketLocation(TEXT("MuzzleFlash"));

	FVector AimLocation;
	if (GetAimLocation(AimLocation) == false)
	{
		return;
	}

	FHitResult HitResult;
	bool bHit = TraceBullet(MuzzleLocation, AimLocation, HitResult);

	/*
	if (bHit == true)
	{
		ApplyHitDamage(HitResult);
	}
	*/

	PlayFireAnimation();

	DrawFire(MuzzleLocation, HitResult, bHit);

	if (IsLocallyControlled() == true)
	{
		ServerRPCFire(MuzzleLocation, HitResult, bHit);
	}
}

bool ASXPlayerCharacter::GetAimLocation(FVector& OutAimLocation) const
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (IsValid(PlayerController) == false)
	{
		return false;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector CameraDirection = CameraRotation.Vector().GetSafeNormal();
	FVector CameraTraceEnd = CameraLocation + CameraDirection * CurrentWeapon->GetMaxAttackRange();

	FHitResult CameraHitResult;

	FCollisionQueryParams TraceParams(NAME_None, false, this);
	TraceParams.AddIgnoredActor(CurrentWeapon);

	bool bHit = GetWorld()->LineTraceSingleByChannel(CameraHitResult, CameraLocation, CameraTraceEnd, ECC_CAMERAAIM, TraceParams);

	if (true == bHit)
	{
		OutAimLocation = CameraHitResult.ImpactPoint;
	}
	else
	{
		OutAimLocation = CameraTraceEnd;
	}

	return true;
}

bool ASXPlayerCharacter::TraceBullet(const FVector& InMuzzleLocation, const FVector& InAimLocation, FHitResult& OutHitResult) const
{
	FVector FireDirection = (InAimLocation - InMuzzleLocation).GetSafeNormal();
	FVector EndLocation = InMuzzleLocation + FireDirection * CurrentWeapon->GetMaxAttackRange();

	FCollisionQueryParams TraceParams(NAME_None, false, this);
	TraceParams.AddIgnoredActor(CurrentWeapon);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHitResult, InMuzzleLocation, EndLocation, ECC_ATTACK, TraceParams);

	if (bHit == false)
	{
		OutHitResult.TraceStart = InMuzzleLocation;
		OutHitResult.TraceEnd = EndLocation;
	}

	return bHit;
}

void ASXPlayerCharacter::ApplyHitDamage(const FHitResult& InHitResult)
{
	ASXCharacterBase* HitCharacter = Cast<ASXCharacterBase>(InHitResult.GetActor());
	if (IsValid(HitCharacter) == false)
	{
		return;
	}

	float Damage = 10.f;

	FString BoneNameString = InHitResult.BoneName.ToString();

	if (InHitResult.BoneName.IsEqual(TEXT("HEAD"), ENameCase::IgnoreCase) == true)
	{
		Damage = 100.f;
	}

	FDamageEvent DamageEvent;
	HitCharacter->TakeDamage(Damage, DamageEvent, GetController(), this);
}

void ASXPlayerCharacter::PlayFireAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) == false)
	{
		return;
	}

	AnimInstance->Montage_Play(GetCurrentWeaponAttackAnimMontage());
}

void ASXPlayerCharacter::DrawFire(const FVector& InMuzzleLocation, const FHitResult& InHitResult, bool bHit)
{
	FVector EndLocation = bHit == true ? InHitResult.ImpactPoint : InHitResult.TraceEnd;

	DrawDebugLine(GetWorld(), InMuzzleLocation, EndLocation, FColor::White, false, 0.1f, 0, 2.f);
}

void ASXPlayerCharacter::ServerRPCFire_Implementation(const FVector& InMuzzleLocation, const FHitResult& InHitResult, bool bHit)
{
	if (bHit == true)
	{
		ApplyHitDamage(InHitResult);
	}

	MulticastRPCFire(InMuzzleLocation, InHitResult, bHit);
	DrawFire(InMuzzleLocation, InHitResult, bHit);
}

void ASXPlayerCharacter::MulticastRPCFire_Implementation(const FVector& InMuzzleLocation, const FHitResult& InHitResult, bool bHit)
{
	if (HasAuthority() == true)
	{
		return;
	}

	if (IsLocallyControlled() == true)
	{
		return;
	}

	PlayFireAnimation();
	DrawFire(InMuzzleLocation, InHitResult, bHit);
}

void ASXPlayerCharacter::ServerRPCSpawnLandMine_Implementation()
{
	if (IsValid(LandMineClass) == true)
	{
		FVector SpawnedLocation = (GetActorLocation() + GetActorForwardVector() * 300.f) - FVector(0.f, 0.f, 90.f);
		ASXLandMine* SpawnedLandMine = GetWorld()->SpawnActor<ASXLandMine>(LandMineClass, SpawnedLocation, FRotator::ZeroRotator);
		SpawnedLandMine->SetOwner(this);
			// 내 캐릭터는 서버와 내 컴퓨터 뿐만 아니라 다른 컴퓨터에도 존재함.
			// 만약 SpawnedLandMine->SetOwner(GetController())라고 작성한다면?
			// 내 컨트롤러는 서버 컴퓨터와 내 컴퓨터에만 존재함. 
			// 다른 컴퓨터에는 내 컨트롤러가 존재하지 않음에 주의. Owner가 제대로 설정되지 않을 수 있음.
	}
}

bool ASXPlayerCharacter::ServerRPCSpawnLandMine_Validate()
{
	return true;
}

void ASXPlayerCharacter::ServerRPCMeleeAttack_Implementation()
{
	if (IsLocallyControlled() == false)
	{
		if (0 == CurrentComboCount)
		{
			BeginAttack();
		}
		else
		{
			ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
			bIsAttackKeyPressed = true;
		}
	}

	MulticastRPCMeleeAttack();
}

void ASXPlayerCharacter::MulticastRPCMeleeAttack_Implementation()
{
	if (HasAuthority() == true)
	{
		return;
	}

	if (IsLocallyControlled() == true)
	{
		return;
	}

	if (0 == CurrentComboCount)
	{
		BeginAttack();
	}
	else
	{
		ensure(FMath::IsWithinInclusive<int32>(CurrentComboCount, 1, MaxComboCount));
		bIsAttackKeyPressed = true;
	}
}
