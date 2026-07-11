// SX_GA_CheckHit.cpp

#include "GameplayAbilitySystem/GA/SX_GA_CheckHit.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilitySystem/AT/SX_AT_SweepSingleCapsule.h"
#include "GameplayAbilitySystem/TA/SX_TA_SweepSingleCapsule.h"
#include "SXGameplayTags.h"

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

	if (bHasHitResult == true && HasAuthority(&CurrentActivationInfo) == true)
	{
		// ### 서버 권위 검증 ###
		// 클라이언트(소유 클라이언트)가 스윕한 결과는 여기까지 오지도 않음 — 클라이언트에서
		// 실행된 이 함수는 그냥 자기 스윕 결과를 로컬에서 확인만 하고 끝(연출/디버그용).
		// 실제 대미지로 이어지는 GameplayEffect 적용은 반드시 서버에서 "서버 자신의" 스윕 결과로만 수행.
		// (HasAuthority(&CurrentActivationInfo)는 어빌리티가 서버 권위 하에 실행 중인지를 판별하는
		//  UGameplayAbility의 헬퍼 함수. GE 적용은 19.6에서 AttributeSet과 함께 이어서 구현.)
		const FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		UE_LOG(LogTemp, Log, TEXT("[Server] Target %s Detected"), *(HitResult.GetActor()->GetName()));
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}