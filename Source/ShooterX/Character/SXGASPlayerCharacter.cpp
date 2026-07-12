// SXGASPlayerCharacter.cpp


#include "Character/SXGASPlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Game/SXGASPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Component/SXGASWidgetComponent.h"
#include "UI/SXGASUserWidget.h"
#include "GameplayAbilitySystem/AS/SX_AS_Character.h"

ASXGASPlayerCharacter::ASXGASPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	float CharacterHalfHeight = 90.f;
	float CharacterRadius = 40.f;

	GetCapsuleComponent()->InitCapsuleSize(CharacterRadius, CharacterHalfHeight);

	FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	HPBar = CreateDefaultSubobject<USXGASWidgetComponent>(TEXT("HPBar"));
	HPBar->SetupAttachment(GetMesh());
	HPBar->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/ShooterX/UI/WBP_GASHPBar.WBP_GASHPBar_C'"));
	if (HpBarWidgetRef.Class)
	{
		HPBar->SetWidgetClass(HpBarWidgetRef.Class);
		HPBar->SetWidgetSpace(EWidgetSpace::Screen);
		HPBar->SetDrawSize(FVector2D(200.0f, 20.f));
		HPBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ASXGASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMoveInput);

	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLookInput);

	//EIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	//EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

	for (const auto& GrantedInputAbility : GrantedInputAbilities)
	{
		EIC->BindAction(
			GrantedInputAbility.InputAction,
			ETriggerEvent::Started,
			this,
			&ThisClass::HandleGameplayAbilityInputPressed,
			GrantedInputAbility.InputTag);

		EIC->BindAction(
			GrantedInputAbility.InputAction,
			ETriggerEvent::Completed,
			this,
			&ThisClass::HandleGameplayAbilityInputReleased,
			GrantedInputAbility.InputTag);
	}
}

void ASXGASPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled() == false)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	checkf(IsValid(PC) == true, TEXT("PlayerController is invalid."));

	UEnhancedInputLocalPlayerSubsystem* EILPS = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	checkf(IsValid(EILPS) == true, TEXT("EnhancedInputLocalPlayerSubsystem is invalid."));

	EILPS->AddMappingContext(InputMappingContext, 0);

	PC->ConsoleCommand(TEXT("ShowDebug AbilitySystem"));
}

void ASXGASPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority() == true)
	{
		InitializeGASCharacterInfo();

		const USX_AS_Character* CurrentAttributeSet = GetAbilitySystemComponent()->GetSet<USX_AS_Character>();
		if (IsValid(CurrentAttributeSet) == true)
		{
			CurrentAttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
		}
	}
}

void ASXGASPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (HasAuthority() == false)
	{
		InitializeGASCharacterInfo();
	}
}

void ASXGASPlayerCharacter::HandleMoveInput(const FInputActionValue& InValue)
{
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is invalid."));
		return;
	}

	const FVector2D InMovementVector = InValue.Get<FVector2D>();

	const FRotator ControlRotation = Controller->GetControlRotation();
	const FRotator ControlYawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(ControlYawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InMovementVector.X);
	AddMovementInput(RightDirection, InMovementVector.Y);
}

void ASXGASPlayerCharacter::HandleLookInput(const FInputActionValue& InValue)
{
	if (IsValid(Controller) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is invalid."));
		return;
	}

	const FVector2D InLookVector = InValue.Get<FVector2D>();

	AddControllerYawInput(InLookVector.X);
	AddControllerPitchInput(InLookVector.Y);
}

UAbilitySystemComponent* ASXGASPlayerCharacter::GetAbilitySystemComponent() const
{
	ASXGASPlayerState* SXGASPlayerState = GetPlayerState<ASXGASPlayerState>();
	if (IsValid(SXGASPlayerState) == false)
	{
		return nullptr;
	}

	UAbilitySystemComponent* ASC = SXGASPlayerState->GetAbilitySystemComponent();
	if (IsValid(ASC) == false)
	{
		return nullptr;
	}

	return ASC;
}

void ASXGASPlayerCharacter::InitializeGASCharacterInfo()
{
	ASXGASPlayerState* SXGASPlayerState = GetPlayerState<ASXGASPlayerState>();
	if (IsValid(SXGASPlayerState) == false)
	{
		return;
	}

	GetAbilitySystemComponent()->InitAbilityActorInfo(SXGASPlayerState, this);

	if (HasAuthority() == true)
	{
		for (const auto& GrantedAbility : GrantedAbilities)
		{
			FGameplayAbilitySpec GrantedAbilitySpec(GrantedAbility);
			GetAbilitySystemComponent()->GiveAbility(GrantedAbilitySpec);
		}

		for (const auto& GrantedInputAbility : GrantedInputAbilities)
		{
			FGameplayAbilitySpec GrantedAbilitySpec(GrantedInputAbility.AbilityClass);
			GrantedAbilitySpec.GetDynamicSpecSourceTags().AddTag(GrantedInputAbility.InputTag);
			GetAbilitySystemComponent()->GiveAbility(GrantedAbilitySpec);
		}
	}
}

void ASXGASPlayerCharacter::HandleGameplayAbilityInputPressed(FGameplayTag InputTag)
{
	UAbilitySystemComponent* CachedASC = GetAbilitySystemComponent();
	if (IsValid(CachedASC) == false)
	{
		return;
	}

	for (FGameplayAbilitySpec& AbilitySpec : CachedASC->GetActivatableAbilities())
	{
		if (IsValid(AbilitySpec.Ability) == false)
		{
			continue;
		}

		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpec.InputPressed = true;
			if (AbilitySpec.IsActive() == true)
			{
				CachedASC->AbilitySpecInputPressed(AbilitySpec);
			}
			else
			{
				CachedASC->TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}	
}

void ASXGASPlayerCharacter::HandleGameplayAbilityInputReleased(FGameplayTag InputTag)
{
	UAbilitySystemComponent* CachedASC = GetAbilitySystemComponent();
	if (IsValid(CachedASC) == false)
	{
		return;
	}

	for (FGameplayAbilitySpec& AbilitySpec : CachedASC->GetActivatableAbilities())
	{
		if (IsValid(AbilitySpec.Ability) == false)
		{
			continue;
		}

		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpec.InputPressed = false;
			if (AbilitySpec.IsActive() == true)
			{
				CachedASC->AbilitySpecInputReleased(AbilitySpec);
			}
		}
	}
}

void ASXGASPlayerCharacter::OnOutOfHealth()
{
	if (HasAuthority() == true)
	{
		SetLifeSpan(0.1f);
	}
}
