// SX_GC_Static_Hit.cpp

#include "GameplayAbilitySystem/GC/SX_GC_Static_Hit.h"

#include "NiagaraFunctionLibrary.h"

USX_GC_Static_Hit::USX_GC_Static_Hit()
{
}

bool USX_GC_Static_Hit::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();
	if (HitResult != nullptr && IsValid(NiagaraSystem) == true)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(Target, NiagaraSystem, HitResult->ImpactPoint);
	}

	return false;
}
