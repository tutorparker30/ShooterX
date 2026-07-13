// SXGASHealthpack.cpp


#include "Gimmick/SXGASHealthpack.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"

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
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0.f, 0.f, 0.f);
	SetReplicateMovement(true);

	GameplayCueTag = SXGameplayTags::GameplayCue_Action_Interation_Open;
}

UAbilitySystemComponent* ASXGASHealthpack::GetAbilitySystemComponent() const
{
	return ASC;
}

void ASXGASHealthpack::BeginPlay()
{
	Super::BeginPlay();

	ASC->InitAbilityActorInfo(this, this);

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

void ASXGASHealthpack::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	if (HasAuthority() == false || bIsUsed == true)
	{
		return;
	}

	InvokeGameplayCue(Other);
	ApplyEffectToTarget(Other);

	bIsUsed = true;
	UpdateUsedVisual();

	SetLifeSpan(2.f);
}

void ASXGASHealthpack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsUsed);
}

void ASXGASHealthpack::ApplyEffectToTarget(AActor* Target)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (IsValid(TargetASC) == false)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContext);
	if (EffectSpecHandle.IsValid() == true)
	{
		TargetASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
	}
}

void ASXGASHealthpack::InvokeGameplayCue(AActor* Target)
{
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = Target;
	Param.Location = GetActorLocation();
	ASC->ExecuteGameplayCue(GameplayCueTag, Param);
}

void ASXGASHealthpack::OnRep_IsUsed()
{
	UpdateUsedVisual();
}

void ASXGASHealthpack::UpdateUsedVisual()
{
	Mesh->SetHiddenInGame(bIsUsed);
	SetActorEnableCollision(bIsUsed == false);
}

void ASXGASHealthpack::OnTimerElapsed()
{
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
