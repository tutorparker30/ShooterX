// SXGASNonPlayerCharacter.cpp

#include "Character/SXGASNonPlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySystem/AS/SX_AS_Character.h"
#include "Component/SXGASWidgetComponent.h"
#include "UI/SXGASUserWidget.h"

ASXGASNonPlayerCharacter::ASXGASNonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	float CharacterHalfHeight = 90.f;
	float CharacterRadius = 40.f;

	GetCapsuleComponent()->InitCapsuleSize(CharacterRadius, CharacterHalfHeight);

	FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<USX_AS_Character>(TEXT("AttributeSet"));

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

void ASXGASNonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ASC->InitAbilityActorInfo(this, this);

	Level = 3;

	if (HasAuthority() == true)
	{
		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(InitStatEffect, Level, EffectContextHandle);
		if (EffectSpecHandle.IsValid())
		{
			ASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
		}

		FGameplayAbilitySpec InvincibilityAbilitySpec(InvincibilityAbilityClass);
		ASC->GiveAbility(InvincibilityAbilitySpec);
		ASC->TryActivateAbilityByClass(InvincibilityAbilityClass);
	}

	AttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
}

UAbilitySystemComponent* ASXGASNonPlayerCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void ASXGASNonPlayerCharacter::OnOutOfHealth()
{
	if (HasAuthority() == true)
	{
		SetLifeSpan(0.1f);
	}
}