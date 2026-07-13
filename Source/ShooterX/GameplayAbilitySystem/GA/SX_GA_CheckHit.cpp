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

	CurrentLevel = (TriggerEventData != nullptr) ? TriggerEventData->EventMagnitude : 1.f;

	USX_AT_SweepSingleCapsule* AttackTraceTask = USX_AT_SweepSingleCapsule::CreateTask(this, ASX_TA_SweepSingleCapsule::StaticClass());
	AttackTraceTask->OnComplete.AddDynamic(this, &ThisClass::OnSweepSingleCapsuleResultReady);
	AttackTraceTask->ReadyForActivation();
}

void USX_GA_CheckHit::OnSweepSingleCapsuleResultReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	const bool bHasHitResult = UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0);

	if (true == bHasHitResult && HasAuthority(&CurrentActivationInfo) == true)
	{
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Ensured();
		const USX_AS_Character* SourceAttributeSet = SourceASC->GetSet<USX_AS_Character>();

		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
		if (EffectSpecHandle.IsValid() == true)
		{
			EffectSpecHandle.Data->SetSetByCallerMagnitude(SXGameplayTags::SetByCaller_Damage, -SourceAttributeSet->GetAttackDamage());
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
		}

		FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackRadiusBuffEffect);
		if (BuffEffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
		}

		const FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());

		if (IsValid(TargetASC) == true && EffectSpecHandle.IsValid() == true)
		{
			FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
			CueContextHandle.AddHitResult(HitResult);

			FGameplayCueParameters CueParam;
			CueParam.EffectContext = CueContextHandle;

			TargetASC->ExecuteGameplayCue(SXGameplayTags::GameplayCue_Action_Combat_Hit, CueParam);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}