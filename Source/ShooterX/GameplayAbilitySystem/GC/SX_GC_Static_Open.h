// SX_GC_Static_Open.h

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "SX_GC_Static_Open.generated.h"

class UNiagaraSystem;

UCLASS()
class SHOOTERX_API USX_GC_Static_Open : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	USX_GC_Static_Open();

	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

};
