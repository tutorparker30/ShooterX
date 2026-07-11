// SX_GA_CheckHit.cpp

#include "GameplayAbilitySystem/GA/SX_GA_CheckHit.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilitySystem/AT/SX_AT_SweepSingleCapsule.h"
#include "GameplayAbilitySystem/TA/SX_TA_SweepSingleCapsule.h"
#include "SXGameplayTags.h"
#include "GameplayAbilitySystem/AS/SX_AS_Character.h"

USX_GA_CheckHit::USX_GA_CheckHit()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
		// 공격 어빌리티(19.2)의 몽타주 노티파이로부터 트리거되므로, 몽타주가 재생되는
		// 소유 클라이언트/서버 양쪽에서 각각 독립적으로 이 어빌리티도 실행됨.

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SXGameplayTags::Event_Action_Combat_Melee_CheckHit;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;

	AbilityTriggers.Add(TriggerData);
}

void USX_GA_CheckHit::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	USX_AT_SweepSingleCapsule* AttackTraceTask = USX_AT_SweepSingleCapsule::CreateTask(this, ASX_TA_SweepSingleCapsule::StaticClass());
	AttackTraceTask->OnComplete.AddDynamic(this, &ThisClass::OnSweepSingleCapsuleResultReady);
	AttackTraceTask->ReadyForActivation();
}

void USX_GA_CheckHit::OnSweepSingleCapsuleResultReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	const bool bHasHitResult = UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0);

	if (true == bHasHitResult && HasAuthority(&CurrentActivationInfo) == true)
	{
		const FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		//UE_LOG(LogTemp, Log, TEXT("[Server] Target %s Detected"), *(HitResult.GetActor()->GetName()));

		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Ensured();
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
		if (IsValid(SourceASC) == false || IsValid(TargetASC) == false)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		const USX_AS_Character* SourceAttributeSet = SourceASC->GetSet<USX_AS_Character>();
		USX_AS_Character* TargetAttributeSet = const_cast<USX_AS_Character*>(TargetASC->GetSet<USX_AS_Character>());
		if (IsValid(SourceAttributeSet) == false || IsValid(TargetAttributeSet) == false)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			return;
		}

		const float AttackDamage = SourceAttributeSet->GetAttackDamage();
		TargetAttributeSet->SetHealth(TargetAttributeSet->GetHealth() - AttackDamage);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}