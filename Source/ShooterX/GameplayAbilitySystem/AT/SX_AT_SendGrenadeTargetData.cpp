// SX_AT_SendGrenadeTargetData.cpp

#include "GameplayAbilitySystem/AT/SX_AT_SendGrenadeTargetData.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

#include "GameplayAbilitySystem/TargetData/SXGASGrenadeTargetData.h"

USX_AT_SendGrenadeTargetData* USX_AT_SendGrenadeTargetData::SendGrenadeTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, const FSXGASGrenadeThrowData& LocalThrowData)
{
	USX_AT_SendGrenadeTargetData* Task = NewAbilityTask<USX_AT_SendGrenadeTargetData>(OwningAbility, TaskInstanceName);
	if (IsValid(Task))
	{
		Task->LocalThrowData = LocalThrowData;
	}

	return Task;
}

FGameplayAbilityTargetDataHandle USX_AT_SendGrenadeTargetData::MakeTargetDataHandle() const
{
	auto* NewTargetData = new FSXGameplayAbilityTargetData_GrenadeThrow();
	NewTargetData->StartLocation = LocalThrowData.StartLocation;
	NewTargetData->LaunchVelocity =	LocalThrowData.LaunchVelocity;

	return FGameplayAbilityTargetDataHandle(NewTargetData);
}

void USX_AT_SendGrenadeTargetData::Activate()
{
	Super::Activate();

	if (IsValid(Ability) == false)
	{
		EndTask();
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (IsValid(ASC) == false || ActorInfo == nullptr)
	{
		Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());

		EndTask();
		return;
	}
	
	const FGameplayAbilitySpecHandle SpecHandle = Ability->GetCurrentAbilitySpecHandle();
		// SpecHandle는 어떤 어빌리티인지 식별하기 위한 값.
	const FPredictionKey ActivationPredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();
		// ActivationPredictionKey를 매개로 로컬 클라의 예측 실행과 서버 실행을 상호 연결을 위한 예측 키.
		// Ex) 첫 번째 투척
		//     SpecHandle == 11
		//     ActivationPredictionKey == 101
		//     두 번째 투척
		//     SpecHandle == 11
		//     ActivationPredictionKey == 102
		// 즉, 같은 수류탄 투척 어빌리티가 여러 번 발동하면 SpecHandle만으로는 식별하기 어려움.
	
	const bool bLocallyControlled = ActorInfo->IsLocallyControlled();
	const bool bAuthority =	ActorInfo->IsNetAuthority();
	if (bLocallyControlled)
	{
		// 로컬 클라 또는 리슨 서버 호스트.

		if (LocalThrowData.IsValid() == false) // FSXGASGrenadeThrowData::IsValid() 호출.
		{
			Cancelled.Broadcast(FGameplayAbilityTargetDataHandle());

			EndTask();
			return;
		}

		FGameplayAbilityTargetDataHandle TargetData = MakeTargetDataHandle();
			// LocalThrowData를 GAS의 TargetData로 변환하는 함수.
		
		if (bAuthority == false)
		{
			// 로컬 클라때만 RPC로 서버에 전달.
			
			FScopedPredictionWindow ScopedPrediction(ASC, true);
				// TargetData를 서버로 전송하기 위한 예측 키 생성.
				// ActivationPredictionKey와 ScopedPredictionKey는 다르다.
				// ActivationPredictionKey는 Cost/Cooldown/Effect/Cue 예측 실행을 위해 만들어진 최초 예측키.
				// ScopedPredictionKey는 새로운 예측 실행(TargetData 서버 전송)을 위해 추가로 생성된 예측키.
				// 그럼 왜 ActivationPredictionKey가 있음에도 새로운 예측키를 만들었을까?
				// 수류탄 투척 로직의 흐름은 수류탄 투척 자세를 잡는 로직이 있고, 입력 해제 후 투척하는 로직이 있다.
				// 최초의 예측키인 ActivationPredictionKey는 여러 프레임 후 입력이 해제된 시점에는 유효하지 않을 수 있음.
				// 그래서 새로운 예측키를 만듬.

			ASC->ServerSetReplicatedTargetData(
				SpecHandle,
				ActivationPredictionKey,
				TargetData,
				FGameplayTag(),
				ASC->ScopedPredictionKey);
				// ServerSetReplicatedTargetData() 함수는 클라에서 서버로 TargetData를 보내주는 RPC.
		}

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			// ShouldBroadcastAbilityTaskDelegates() 함수는
			// 현재 태스크의 델리게이트를 broadcast 해도되는 상태인지 확인하는 함수.
			
			ValidData.Broadcast(TargetData);
		}

		EndTask();
		return;
	}

	if (bAuthority)
	{
		// bLocallyControlled == false && bAuthority == true인 상황.
		// 즉, 데디케이티드 서버에서 어빌리티 실행 중.
		// 이 시점의 서버는 TargetData가 없음. 그래서 로컬 클라가 보낼 예정인 TargetData를 대기.

		TargetDataDelegateHandle = ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
			.AddUObject(this, &ThisClass::OnTargetDataReplicated);
			// ActivationPredictionKey에 해당하는 TargetData 델리게이트를 가져와서 OnTargetDataReplicated() 함수 바인딩.
			// 즉, 로컬 클라가 ServerSetReplicatedTargetData() RPC 함수를 호출하면 OnTargetDataReplicated() 함수도 호출될 예정.

		// 이 시점에는 두 가지 경우의 수가 있음.
		// 1번 케이스: 서버에서 태스크 발동 -> 델리게이트 등록 -> 로컬 클라 TargetData 도착.
		// 2번 케이스: 로컬 클라 TargetData 도착 -> 서버에서 태스크 발동 -> 델리게이트 등록
		// 2번 케이스의 경우에는 이미 서버의 ASC 내부에 TargetData가 저장되어 있음.
		
		const bool bTargetDataAlreadyReceived =	ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
			// CallReplicatedTargetDataDelegatesIfSet() 함수는 저장된 TargetData가 있는지 확인하고,
			// 있다면 등록된 콜백함수를 즉시 호출되게끔 해줌.
		if (bTargetDataAlreadyReceived == false)
		{
			SetWaitingOnRemotePlayerData();
				// 만약 TargetData가 아직 서버에 도착하지 않았다면 도착할 때까지 대기.
				// 즉, 여기서 EndTask() 함수를 호출하면 안됨. 나중에 TargetData가 도착하면 
				// OnTargetDataReplicated() 함수에서 EndTask() 함수 호출 예정.
		}

		return;
	}

	EndTask();
}

void USX_AT_SendGrenadeTargetData::OnTargetDataReplicated(const FGameplayAbilityTargetDataHandle& TargetData, FGameplayTag ActivationTag)
{
	if (IsValid(Ability) == false)
	{
		EndTask();
		return;
	}

	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (IsValid(ASC) == false)
	{
		EndTask();
		return;
	}

	const FGameplayAbilitySpecHandle SpecHandle = Ability->GetCurrentAbilitySpecHandle();
	const FPredictionKey ActivationPredictionKey = Ability->GetCurrentActivationInfo().GetActivationPredictionKey();

	const FGameplayAbilityTargetDataHandle CopiedTargetData = TargetData;
		// Consume 전에 복사해 Task 콜백에서 사용한다.

	ASC->ConsumeClientReplicatedTargetData(SpecHandle, ActivationPredictionKey);
		// 서버 ASC가 저장하고 있던 로컬 클라의 TargetData를 제거(소비)
		// 여기서 소비하지 않으면 다음 수류탄 투척시 CallReplicatedTargetDataDelegatesIfSet() 함수가
		// 과거 TargetData를 다시 발견할 수도 있음.
		// 또한 불필요한 캐시 유지, 중복 처리 위험도 있음. 

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(CopiedTargetData);
	}

	EndTask();
}

void USX_AT_SendGrenadeTargetData::OnDestroy(bool bAbilityEnded)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();

	if (IsValid(ASC) &&	IsValid(Ability) &&	TargetDataDelegateHandle.IsValid())
	{
		ASC->AbilityTargetDataSetDelegate(
			Ability->GetCurrentAbilitySpecHandle(),
			Ability
			->GetCurrentActivationInfo()
			.GetActivationPredictionKey())
		.Remove(TargetDataDelegateHandle);
	}

	Super::OnDestroy(bAbilityEnded);
}
