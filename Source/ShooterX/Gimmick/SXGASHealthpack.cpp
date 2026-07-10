// SXGASHealthpack.cpp


#include "Gimmick/SXGASHealthpack.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
//#include "GameplayAbilitySystem/GA/SX_GA_Rotate.h"
	// 태그를 활용한 의존성 제거.
#include "SXGameplayTags.h"

ASXGASHealthpack::ASXGASHealthpack()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Body = CreateDefaultSubobject<UBoxComponent>(TEXT("Body"));
	Body->SetupAttachment(Root);
	Body->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	Body->SetRelativeScale3D(FVector(0.6f, 0.8f, 0.8f));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	Mesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.f, 0.f, 0.f);
	SetReplicateMovement(true);
}

UAbilitySystemComponent* ASXGASHealthpack::GetAbilitySystemComponent() const
{
	return ASC;
}

void ASXGASHealthpack::BeginPlay()
{
	Super::BeginPlay();

	ASC->InitAbilityActorInfo(this, this);

	/*
	if (HasAuthority() == true)
	{
		FGameplayAbilitySpec AbilitySpec(USX_GA_Rotate::StaticClass());
		ASC->GiveAbility(AbilitySpec);

		GetWorldTimerManager().SetTimer(ToggleTimerHandle, this, &ThisClass::OnTimerElapsed, 3.f, true);
	}
	*/

	if (HasAuthority() == true)
	{
		for (const auto& GrantedAbility : GrantedAbilities)
		{
			FGameplayAbilitySpec GrantedAbilitySpec(GrantedAbility);
			ASC->GiveAbility(GrantedAbilitySpec);
		}

		GetWorldTimerManager().SetTimer(ToggleTimerHandle, this, &ThisClass::OnTimerElapsed, 3.f, true);
	}	
}

void ASXGASHealthpack::OnTimerElapsed()
{
	/*
	FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(USX_GA_Rotate::StaticClass());
	if (nullptr == AbilitySpec)
	{
		return;
	}

	if (AbilitySpec->IsActive() == false)
	{
		ASC->TryActivateAbility(AbilitySpec->Handle);
	}
	else
	{
		ASC->CancelAbilityHandle(AbilitySpec->Handle);
	}
	*/

	FGameplayTagContainer TargetTag(SXGameplayTags::Ability_ID_Action_Movement_Rotate);

	if (ASC->HasMatchingGameplayTag(SXGameplayTags::State_Action_Movement_Rotating) == false)
	{
		ASC->TryActivateAbilitiesByTag(TargetTag);
	}
	else
	{
		ASC->CancelAbilities(&TargetTag);
	}
}
