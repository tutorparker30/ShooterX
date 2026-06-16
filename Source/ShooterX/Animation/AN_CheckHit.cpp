// AN_CheckHit.cpp


#include "Animation/AN_CheckHit.h"

#include "Character/SXCharacterBase.h"

void UAN_CheckHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp) == true)
	{
		ASXCharacterBase* AttackingCharacter = Cast<ASXCharacterBase>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter) == true)
		{
			AttackingCharacter->HandleOnCheckHit();
		}
	}
}
