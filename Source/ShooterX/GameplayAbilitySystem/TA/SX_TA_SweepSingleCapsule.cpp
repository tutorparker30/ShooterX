// SX_TA_SweepSingleCapsule.cpp

#include "GameplayAbilitySystem/TA/SX_TA_SweepSingleCapsule.h"

#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySystem/AS/SX_AS_Character.h"

static TAutoConsoleVariable<int32> CVarDrawSweepSingleSphere(
	TEXT("sx.DrawSweepSingleSphere"),
	0,
	TEXT("Draw ASX_TA_SweepSingleCapsule sphere sweep debug shapes.\n"),
	ECVF_Cheat
);

ASX_TA_SweepSingleCapsule::ASX_TA_SweepSingleCapsule()
{
}

void ASX_TA_SweepSingleCapsule::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	if (IsValid(Ability) == false)
	{
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (nullptr == ActorInfo)
	{
		return;
	}

	SourceActor = ActorInfo->AvatarActor.Get();
}

void ASX_TA_SweepSingleCapsule::ConfirmTargetingAndContinue()
{
	if (IsValid(SourceActor) == false)
	{
		return;
	}

	FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
	TargetDataReadyDelegate.Broadcast(DataHandle);
}

FGameplayAbilityTargetDataHandle ASX_TA_SweepSingleCapsule::MakeTargetData() const
{
	FGameplayAbilityTargetDataHandle DataHandle;

	ACharacter* SourceCharacter = CastChecked<ACharacter>(SourceActor);

	if (IsValid(SourceCharacter) == false)
	{
		return DataHandle;
	}

	UWorld* World = GetWorld();
	if (IsValid(World) == false)
	{
		return DataHandle;
	}

	const UCapsuleComponent* CapsuleComponent =	SourceCharacter->GetCapsuleComponent();
	if (IsValid(CapsuleComponent) == false)
	{
		return DataHandle;
	}

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceCharacter);
	if (IsValid(SourceASC) == false)
	{
		return FGameplayAbilityTargetDataHandle();
	}

	const USX_AS_Character* SourceAttributeSet = SourceASC->GetSet<USX_AS_Character>();
	if (IsValid(SourceAttributeSet) == false)
	{
		return FGameplayAbilityTargetDataHandle();
	}

	//const float AttackRange = 100.f;
	//const float AttackRadius = 50.f;
	const float AttackRange = SourceAttributeSet->GetAttackRange();
	const float AttackRadius = SourceAttributeSet->GetAttackRadius();

	const FVector Forward = SourceCharacter->GetActorForwardVector();
	const FVector Start = SourceCharacter->GetActorLocation() + Forward * SourceCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + Forward * AttackRange;

	FCollisionQueryParams Params(FName(), false, SourceCharacter);
	FHitResult OutHitResult;
	const FCollisionShape SweepShape = FCollisionShape::MakeSphere(AttackRadius);
	const bool bHitDetected = GetWorld()->SweepSingleByChannel(OutHitResult, Start, End, FQuat::Identity, ECC_Pawn, SweepShape, Params);

	if (bHitDetected == true)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
		DataHandle.Add(TargetData);
	}

	const bool bDrawSweepDebug = CVarDrawSweepSingleSphere.GetValueOnGameThread() != 0;

	if (true == bDrawSweepDebug)
	{
		const FColor DrawColor = bHitDetected == true ? FColor::Green : FColor::Red;
		constexpr int32 SphereSegments = 16;
		constexpr float DrawDuration = 3.0f;
		constexpr float LineThickness = 1.5f;
		constexpr uint8 DepthPriority = 0;

		DrawDebugSphere(World, Start, AttackRadius, SphereSegments,	DrawColor, false, DrawDuration, DepthPriority, LineThickness);

		DrawDebugSphere(World, End, AttackRadius, SphereSegments, DrawColor, false,	DrawDuration, DepthPriority, LineThickness);

		DrawDebugLine( World, Start, End, DrawColor, false, DrawDuration, DepthPriority, LineThickness);

		if (bHitDetected == true)
		{
			// Sweep가 실제로 멈춘 위치의 구체.
			DrawDebugSphere( World, OutHitResult.Location, AttackRadius, SphereSegments, FColor::Yellow, false, DrawDuration, DepthPriority, LineThickness);
			
			// 실제 표면 충돌점.
			DrawDebugPoint( World, OutHitResult.ImpactPoint, 15.0f, FColor::Cyan, false, DrawDuration, DepthPriority);
		}
	}

	return DataHandle;
}