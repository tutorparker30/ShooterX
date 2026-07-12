// SXGASNonPlayerCharacter.cpp

#include "Character/SXGASNonPlayerCharacter.h"

#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySystem/AS/SX_AS_Character.h"

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
	}
}

UAbilitySystemComponent* ASXGASNonPlayerCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}
