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

ASXPlayerCharacter::ASXPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.f;
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 50.f, 100.f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
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

	const USXCharacterMaterialManager* CDO = GetDefault<USXCharacterMaterialManager>();

	const int32 MaterialPathCount = CDO->PlayerCharacterMeshMaterialPaths.Num();

	if (MaterialPathCount < 2 || MaterialPathCount % 2 != 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid PlayerCharacterMeshMaterialPaths count."));
		return;
	}

	const int32 PairCount = MaterialPathCount / 2;
	const int32 PairIndex = FMath::RandRange(0, PairCount - 1);
	const int32 MaterialIndex = PairIndex * 2;

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
