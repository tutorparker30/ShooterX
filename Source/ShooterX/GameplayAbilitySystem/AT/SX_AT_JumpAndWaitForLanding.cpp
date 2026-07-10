// SX_AT_JumpAndWaitForLanding.cpp

#include "GameplayAbilitySystem/AT/SX_AT_JumpAndWaitForLanding.h"

#include "GameFramework/Character.h"

USX_AT_JumpAndWaitForLanding::USX_AT_JumpAndWaitForLanding()
{
}

USX_AT_JumpAndWaitForLanding* USX_AT_JumpAndWaitForLanding::CreateTask(UGameplayAbility* OwningAbility)
{
	return NewAbilityTask<USX_AT_JumpAndWaitForLanding>(OwningAbility);
}

void USX_AT_JumpAndWaitForLanding::Activate()
{
	Super::Activate();

	if (ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor()))
	{
		Character->LandedDelegate.AddDynamic(this, &ThisClass::OnLanded);
			// LandedDelegate는 각 머신의 CharacterMovementComponent가 판정하는 이벤트라서
			// NetExecutionPolicy=LocalPredicted인 어빌리티라면
			// 로컬/서버 양쪽에서 이 태스크가 각자 알아서 착지를 감지함. 별도의 복제 로직이 필요 없음.
		Character->Jump();

		SetWaitingOnAvatar();
			// 어빌리티 태스크 종료를 미루고 Waiting 상태로 진입. (Tick이 필요하면 bTickingTask=true)
	}	
}

void USX_AT_JumpAndWaitForLanding::OnDestroy(bool AbilityEnded)
{
	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
	Character->LandedDelegate.RemoveDynamic(this, &ThisClass::OnLanded);

	Super::OnDestroy(AbilityEnded);
}

void USX_AT_JumpAndWaitForLanding::OnLanded(const FHitResult& Hit)
{
	if (ShouldBroadcastAbilityTaskDelegates() == true)
	{
		OnComplete.Broadcast();
	}
}