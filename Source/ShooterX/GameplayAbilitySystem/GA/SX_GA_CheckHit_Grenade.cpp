// SX_GA_CheckHit_Grenade.cpp

#include "GameplayAbilitySystem/GA/SX_GA_CheckHit_Grenade.h"

#include "GameplayAbilitySystem/TargetData/SXGASGrenadeTargetData.h"
#include "GameplayAbilitySystem/AT/SX_AT_OverlapMultiSphere.h"
#include "GameplayAbilitySystem/TA/SX_TA_OverlapMultiSphere.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "SXGameplayTags.h"

USX_GA_CheckHit_Grenade::USX_GA_CheckHit_Grenade()
	: bIgnoreSourceActor(true)
	, ExplosionLocation(FVector::ZeroVector)
	, ExplosionRadius(0.0f)
	, ExplosionDamage(0.0f)
	, bFinishRequested(false)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
		// 여러 수류탄이 비슷한 시점에 폭발하더라도 각 폭발이 별도의 어빌리티 객체로 처리되게끔 함.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
		// 폭발 피격 관련 로직은 서버에서만 실행되게끔 함.
	NetSecurityPolicy =	EGameplayAbilityNetSecurityPolicy::ServerOnly;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
}

void USX_GA_CheckHit_Grenade::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bFinishRequested = false;

	if (ActorInfo == nullptr ||
		ActorInfo->IsNetAuthority() == false ||
		IsValid(OverlapTargetActorClass) == false ||
		IsValid(GrenadeDamageEffectClass) == false ||
		ExtractExplosionData(TriggerEventData) == false)
	{
		FinishCheckHit(true);
		return;
	}

	AActor* IgnoredActor = bIgnoreSourceActor ? GetAvatarActorFromActorInfo() : nullptr;

	USX_AT_OverlapMultiSphere* OverlapTask = USX_AT_OverlapMultiSphere::OverlapMultiSphere(
			this,
			TEXT("GrenadeOverlapMultiSphere"),
			OverlapTargetActorClass,
			ExplosionLocation,
			ExplosionRadius,
			IgnoredActor);

	if (IsValid(OverlapTask) == false)
	{
		FinishCheckHit(true);
		return;
	}

	OverlapTask->ValidData.AddDynamic(this, &ThisClass::OnOverlapTargetDataReady);
	OverlapTask->Cancelled.AddDynamic(this,	&ThisClass::OnOverlapTargetDataCancelled);
	OverlapTask->ReadyForActivation();
}

void USX_GA_CheckHit_Grenade::OnOverlapTargetDataReady(FGameplayAbilityTargetDataHandle TargetDataHandle)
{
	if (bFinishRequested ||	IsActive() == false)
	{
		return;
	}

	FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(
			GrenadeDamageEffectClass,
			GetAbilityLevel());

	if (EffectSpecHandle.IsValid() == false ||
		EffectSpecHandle.Data.IsValid() == false)
	{
		FinishCheckHit(true);
		return;
	}

	EffectSpecHandle.Data->SetSetByCallerMagnitude(SXGameplayTags::SetByCaller_Damage_Grenade, ExplosionDamage);
	
	FGameplayEffectContextHandle EffectContext = EffectSpecHandle.Data->GetContext();
	EffectContext.AddOrigin(ExplosionLocation);

	ApplyGameplayEffectSpecToTarget(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		EffectSpecHandle,
		TargetDataHandle);

	FinishCheckHit(false);
}

void USX_GA_CheckHit_Grenade::OnOverlapTargetDataCancelled(FGameplayAbilityTargetDataHandle TargetDataHandle)
{
	FinishCheckHit(true);
}

bool USX_GA_CheckHit_Grenade::ExtractExplosionData(	const FGameplayEventData* TriggerEventData)
{
	if (TriggerEventData == nullptr || TriggerEventData->TargetData.Num() != 1)
	{
		return false;
	}

	const FGameplayAbilityTargetData* RawData =	TriggerEventData->TargetData.Get(0);

	if (RawData == nullptr ||
		RawData->GetScriptStruct() != FSXGameplayAbilityTargetData_GrenadeExplosion::StaticStruct())
	{
		return false;
	}

	const auto* ExplosionTargetData = static_cast<const FSXGameplayAbilityTargetData_GrenadeExplosion*>(RawData);
	if (ExplosionTargetData->IsValidData() == false)
	{
		return false;
	}

	ExplosionLocation =	ExplosionTargetData->ExplosionLocation;
	ExplosionRadius = ExplosionTargetData->ExplosionRadius;
	ExplosionDamage = ExplosionTargetData->ExplosionDamage;

	return true;
}

void USX_GA_CheckHit_Grenade::FinishCheckHit(bool bWasCancelled)
{
	if (bFinishRequested)
	{
		return;
	}

	bFinishRequested = true;

	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
	}
}
