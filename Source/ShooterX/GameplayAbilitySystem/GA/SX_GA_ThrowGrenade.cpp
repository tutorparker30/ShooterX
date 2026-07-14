// SX_GA_ThrowGrenade.cpp

#include "GameplayAbilitySystem/GA/SX_GA_ThrowGrenade.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Animation/AnimMontage.h"
#include "GameplayAbilitySystem/AS/SX_AS_Grenade.h"
#include "GameFramework/Character.h"
#include "Item/SXGASGrenade.h"
#include "GameplayAbilitySystem/TA/SX_TA_GrenadeTrajectory.h"


USX_GA_ThrowGrenade::USX_GA_ThrowGrenade()
	: AimStartSectionName(TEXT("AimStart"))
	, AimLoopSectionName(TEXT("AimLoop"))
	, ThrowSectionName(TEXT("Throw"))
	, bThrowConfirmed(false)
	, bFinishRequested(false)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy =	EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ReplicationPolicy =	EGameplayAbilityReplicationPolicy::ReplicateNo;

	bReplicateInputDirectly = false;
}

void USX_GA_ThrowGrenade::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bThrowConfirmed = false;
	bFinishRequested = false;

	if (ActorInfo == nullptr ||	ActorInfo->AvatarActor.IsValid() == false)
	{
		FinishAbility(true);
		return;
	}

	//if (ValidateMontage() == false)
	if (ValidateMontage() == false || ValidateThrowConfiguration() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Invalid grenade throw montage " "or montage section."), *GetNameSafe(this));

		FinishAbility(true);
		return;
	}

	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("GrenadeThrowMontage"), GrenadeThrowMontage, 1.0f,	AimStartSectionName);
	if (IsValid(MontageTask) == false)
	{
		FinishAbility(true);
		return;
	}
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this,	&ThisClass::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	MontageTask->ReadyForActivation();

	UAbilityTask_WaitInputRelease* InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	if (IsValid(InputReleaseTask) == false)
	{
		FinishAbility(true);
		return;
	}
	InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
	InputReleaseTask->ReadyForActivation();

	if (StartTrajectoryPreview() == false)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Failed to start " "grenade trajectory preview."), *GetNameSafe(GetAvatarActorFromActorInfo()));

		FinishAbility(true);
	}
}

bool USX_GA_ThrowGrenade::ValidateMontage() const
{
	// 애님몽타주 애셋 속 섹션 네임이 정상적인지 확인하는 함수.

	if (IsValid(GrenadeThrowMontage) == false)
	{
		return false;
	}

	if (GrenadeThrowMontage->GetSectionIndex(AimStartSectionName) == INDEX_NONE)
	{
		return false;
	}

	if (GrenadeThrowMontage->GetSectionIndex(AimLoopSectionName) == INDEX_NONE)
	{
		return false;
	}

	if (GrenadeThrowMontage->GetSectionIndex(ThrowSectionName) == INDEX_NONE)
	{
		return false;
	}

	return true;
}

void USX_GA_ThrowGrenade::OnInputReleased(float InTimeHeld)
{
	if (bThrowConfirmed || bFinishRequested || IsActive() == false)
	{
		return;
	}

	bThrowConfirmed = true;

	UE_LOG(LogTemp, Log, TEXT("[%s] Grenade input released. " "TimeHeld: %.2f"), *GetNameSafe(GetAvatarActorFromActorInfo()), InTimeHeld);

	UAbilitySystemComponent* CachedASC = GetAbilitySystemComponentFromActorInfo();
	if (IsValid(CachedASC) == false)
	{
		FinishAbility(true);
		return;
	}

	//CachedASC->CurrentMontageJumpToSection(ThrowSectionName);

	UE_LOG(LogTemp, Log, TEXT("[%s] Grenade input released. " "TimeHeld: %.2f"), *GetNameSafe(GetAvatarActorFromActorInfo()), InTimeHeld);

	if (TryCommitThrow() == false)
	{
		FinishAbility(true);
		return;
	}

	bThrowConfirmed = true;

	//CachedASC->CurrentMontageJumpToSection(ThrowSectionName);

	StopTrajectoryPreview();

	CachedASC->CurrentMontageJumpToSection(ThrowSectionName);
}

void USX_GA_ThrowGrenade::OnMontageCompleted()
{
	if (bThrowConfirmed == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Grenade montage completed "	"before input release."), *GetNameSafe(GetAvatarActorFromActorInfo()));

		FinishAbility(true);
		return;
	}
		// 정상적인 흐름에서는 AimLoop 섹션의 애님시퀀스가 반복되므로,
		// 입력 해제 전까지는 몽타주가 끝나지 않음. OnMontageCompleted() 함수 호출 안됨.
		// 만약 입력 해제 전에 OnMontageCompleted() 함수가 호출되었다면 어빌리티 취소함.

	FinishAbility(false);
}

void USX_GA_ThrowGrenade::OnMontageInterrupted()
{
	FinishAbility(true);
}

void USX_GA_ThrowGrenade::OnMontageCancelled()
{
	FinishAbility(true);
}

void USX_GA_ThrowGrenade::FinishAbility(bool bWasCancelled)
{
	if (bFinishRequested)
	{
		return;
	}

	bFinishRequested = true;
		// 여러 몽타주 콜백이 연속으로 발생하더라도 EndAbility() 함수가 중복 호출되지 않도록 방지함.

	if (IsActive() == false)
	{
		return;
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, bWasCancelled);
}

bool USX_GA_ThrowGrenade::TryCommitThrow()
{
	if (IsActive() == false || CurrentActorInfo == nullptr)
	{
		return false;
	}

	UAbilitySystemComponent* CachedASC = GetAbilitySystemComponentFromActorInfo();

	if (IsValid(CachedASC) == false)
	{
		return false;
	}

	const USX_AS_Grenade* GrenadeAttributeSet =	CachedASC->GetSet<USX_AS_Grenade>();
	if (IsValid(GrenadeAttributeSet) == false)
	{
		UE_LOG(LogTemp,	Error, TEXT("[%s] Grenade AttributeSet " "is invalid."), *GetNameSafe(GetAvatarActorFromActorInfo()));
		return false;
	}

	const float PreviousGrenadeCount = GrenadeAttributeSet->GetCurrentGrenadeCount();

	const bool bCommitSucceeded = CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	if (bCommitSucceeded == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Failed to commit " "grenade throw ability. " "GrenadeCount: %.0f"), *GetNameSafe(GetAvatarActorFromActorInfo()), PreviousGrenadeCount);
		return false;
	}

	const float CurrentGrenadeCount = GrenadeAttributeSet->GetCurrentGrenadeCount();

	UE_LOG(LogTemp, Log, TEXT("[%s] Grenade throw committed. " "GrenadeCount: %.0f -> %.0f"), *GetNameSafe(GetAvatarActorFromActorInfo()),PreviousGrenadeCount,CurrentGrenadeCount);

	return true;
}

bool USX_GA_ThrowGrenade::ValidateThrowConfiguration()
{
	// 투척 설정 검사는 로컬 클라와 서버 모두에서 수행됨. 
	// 따라서 설정이 누락된 상태에서는 실패할 수 있음.

	if (CurrentActorInfo == nullptr || IsValid(GrenadeClass) == false || GrenadeThrowSocketName.IsNone())
	{
		return false;
	}

	ACharacter* AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (IsValid(AvatarCharacter) == false || IsValid(AvatarCharacter->GetMesh()) == false)
	{
		return false;
	}

	if (AvatarCharacter->GetMesh()->DoesSocketExist(GrenadeThrowSocketName) == false)
	{
		return false;
	}

	if (CurrentActorInfo->IsLocallyControlled() && IsValid(TrajectoryTargetActorClass) == false)
	{
		return false;
	}

	const ASXGASGrenade* GrenadeCDO = GrenadeClass->GetDefaultObject<ASXGASGrenade>();
	if (IsValid(GrenadeCDO) == false ||
		GrenadeCDO->GetGrenadeCollisionRadius() <= 0.0f ||
		GrenadeCDO->GetGrenadeInitialSpeed() <= 0.0f ||
		GrenadeCDO->GetGrenadeMaxSpeed() <= 0.0f)
	{
		return false;
	}

	return true;
}

bool USX_GA_ThrowGrenade::StartTrajectoryPreview()
{
	if (CurrentActorInfo == nullptr)
	{
		return false;
	}

	if (CurrentActorInfo->IsLocallyControlled() == false)
	{
		return true;
			// true를 반환하는 이유는 서버에서는 궤적을 만들지 않는 것이 정상적인 동작이기 때문.
	}

	if (TrajectoryTargetActor)
	{
		return true;
	}

	UWorld* World = GetWorld();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (IsValid(World) == false ||
		IsValid(AvatarActor) == false ||
		IsValid(TrajectoryTargetActorClass) ==false)
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = AvatarActor;
	SpawnParameters.Instigator = Cast<APawn>(AvatarActor);
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASX_TA_GrenadeTrajectory* SpawnedTargetActor = World->SpawnActor<ASX_TA_GrenadeTrajectory>(
			TrajectoryTargetActorClass,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParameters);

	if (IsValid(SpawnedTargetActor) == false)
	{
		return false;
	}

	SpawnedTargetActor->ConfigureTrajectory(GrenadeClass, GrenadeThrowSocketName);
	SpawnedTargetActor->StartTargeting(this);
	if (SpawnedTargetActor->RefreshTrajectory() == false)
	{
		SpawnedTargetActor->Destroy();
		return false;
	}

	TrajectoryTargetActor = SpawnedTargetActor;

	return true;
}

void USX_GA_ThrowGrenade::StopTrajectoryPreview()
{
	if (TrajectoryTargetActor)
	{
		TrajectoryTargetActor->Destroy();
	}

	TrajectoryTargetActor = nullptr;
}

void USX_GA_ThrowGrenade::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	StopTrajectoryPreview();

	bThrowConfirmed = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}