// SX_GC_Static_Hit_Periodic.cpp

#include "GameplayAbilitySystem/GC/SX_GC_Static_Hit_Periodic.h"

#include "SXGameplayTags.h"
#include "NiagaraFunctionLibrary.h"


USX_GC_Static_Hit_Periodic::USX_GC_Static_Hit_Periodic()
{
	GameplayCueTag = SXGameplayTags::GameplayCue_Action_Combat_Hit_Periodic;
}

bool USX_GC_Static_Hit_Periodic::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (IsValid(NiagaraSystem) == true)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(Target, NiagaraSystem, Target->GetActorLocation());
	}

	return false;
}

