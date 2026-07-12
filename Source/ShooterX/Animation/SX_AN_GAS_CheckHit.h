// SX_AN_GAS_CheckHit.h

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "SX_AN_GAS_CheckHit.generated.h"

UCLASS()
class SHOOTERX_API USX_AN_GAS_CheckHit : public UAnimNotify
{
	GENERATED_BODY()

public:
	USX_AN_GAS_CheckHit();

protected:
	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere)
	FGameplayTag TriggerGameplayTag;

	UPROPERTY(EditAnywhere)
	float AttackLevel = 1.0f;

};