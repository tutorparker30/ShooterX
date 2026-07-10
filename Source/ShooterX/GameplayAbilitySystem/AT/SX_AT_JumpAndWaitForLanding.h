// SX_AT_JumpAndWaitForLanding.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SX_AT_JumpAndWaitForLanding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSXJumpAndWaitForLandingDelegate);

UCLASS()
class SHOOTERX_API USX_AT_JumpAndWaitForLanding : public UAbilityTask
{
	GENERATED_BODY()

public:
	USX_AT_JumpAndWaitForLanding();

	UFUNCTION(BlueprintCallable, Category = "Ability|Task", meta = (DisplayName = "JumpAndWaitForLanding", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USX_AT_JumpAndWaitForLanding* CreateTask(UGameplayAbility* OwningAbility);
		// 태스크 생성 함수는 static 멤버함수로 구현. BlueprintCallable로 열어두면
		// 블루프린트 스크립팅으로도 태스크를 생성할 수 있음.

	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY(BlueprintAssignable)
	FSXJumpAndWaitForLandingDelegate OnComplete;

protected:
	UFUNCTION()
	void OnLanded(const FHitResult& Hit);

};