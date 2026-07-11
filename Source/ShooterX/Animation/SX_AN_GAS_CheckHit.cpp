// SX_AN_GAS_CheckHit.cpp


#include "Animation/SX_AN_GAS_CheckHit.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

USX_AN_GAS_CheckHit::USX_AN_GAS_CheckHit()
{
}

FString USX_AN_GAS_CheckHit::GetNotifyName_Implementation() const
{
	return TEXT("GASCheckHit");
}

void USX_AN_GAS_CheckHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp) == true)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
		if (IsValid(OwnerActor) == true && IsValid(AbilitySystemComponent) == true)
		{
			FGameplayEventData PayloadData;
			PayloadData.EventTag = TriggerGameplayTag;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, PayloadData);
		}
	}
}
