#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_CheckInput_Attack.generated.h"

UCLASS()
class SHOOTERX_API UAN_CheckInput_Attack : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

};
