// SX_GC_Static_Hit.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "SX_GC_Static_Hit.generated.h"

class UNiagaraSystem;

UCLASS()
class SHOOTERX_API USX_GC_Static_Hit : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	USX_GC_Static_Hit();

	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

};
