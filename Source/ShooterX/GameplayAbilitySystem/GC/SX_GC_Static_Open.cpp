// SX_GC_Static_Open.cpp

#include "GameplayAbilitySystem/GC/SX_GC_Static_Open.h"

#include "SXGameplayTags.h"
#include "NiagaraFunctionLibrary.h"


USX_GC_Static_Open::USX_GC_Static_Open()
{
	GameplayCueTag = SXGameplayTags::GameplayCue_Action_Interation_Open;
}

bool USX_GC_Static_Open::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (IsValid(NiagaraSystem) == true)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(Target, NiagaraSystem, Parameters.Location);
	}

	return false;
}
