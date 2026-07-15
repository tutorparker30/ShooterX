// SX_TA_OverlapMultiSphere.cpp

#include "GameplayAbilitySystem/TA/SX_TA_OverlapMultiSphere.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

ASX_TA_OverlapMultiSphere::ASX_TA_OverlapMultiSphere()
	: bShowDebug(false)
	, Origin(FVector::ZeroVector)
	, Radius(0.0f)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;
}

void ASX_TA_OverlapMultiSphere::ConfigureOverlap(const FVector& InOrigin, float InRadius, AActor* InIgnoredActor)
{
	Origin = InOrigin;
	Radius = InRadius;
	IgnoredActor = InIgnoredActor;
}

void ASX_TA_OverlapMultiSphere::ConfirmTargetingAndContinue()
{
	if (Radius <= 0.0f || Origin.ContainsNaN())
	{
		CanceledDelegate.Broadcast(FGameplayAbilityTargetDataHandle());

		return;
	}

	TargetDataReadyDelegate.Broadcast(MakeTargetData());
}

FGameplayAbilityTargetDataHandle ASX_TA_OverlapMultiSphere::MakeTargetData() const
{
	UWorld* World = GetWorld();

	if (IsValid(World) == false)
	{
		return FGameplayAbilityTargetDataHandle();
	}

	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(GrenadeOverlapMultiSphere), false);

	if (IsValid(IgnoredActor))
	{
		QueryParams.AddIgnoredActor(IgnoredActor);
	}

	World->OverlapMultiByObjectType(
		OverlapResults,
		Origin,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(Radius),
		QueryParams);

	TArray<TWeakObjectPtr<AActor>> HitActors;

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		AActor* OverlapActor = OverlapResult.GetActor();

		if (IsValid(OverlapActor) == false ||
			OverlapActor == IgnoredActor ||
			HitActors.Contains(OverlapActor))
		{
			continue;
		}

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OverlapActor);
		if (IsValid(TargetASC) == false)
		{
			continue;
				// ASC가 없으면 제외.
		}

		HitActors.Add(OverlapActor);
	}

	auto* ActorArrayData = new FGameplayAbilityTargetData_ActorArray();

	ActorArrayData->SetActors(HitActors);

#if ENABLE_DRAW_DEBUG

	if (bShowDebug)
	{
		const FColor DebugColor = HitActors.IsEmpty() ? FColor::Red : FColor::Green;

		DrawDebugSphere(
			World,
			Origin,
			Radius,
			24,
			DebugColor,
			false,
			3.0f);
	}

#endif

	return FGameplayAbilityTargetDataHandle(ActorArrayData);
}
