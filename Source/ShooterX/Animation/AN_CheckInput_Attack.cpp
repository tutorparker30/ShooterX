#include "Animation/AN_CheckInput_Attack.h"
#include "Character/SXCharacterBase.h"

void UAN_CheckInput_Attack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp))
	{
		ASXCharacterBase* AttackingCharacter = Cast<ASXCharacterBase>(MeshComp->GetOwner());
		if (IsValid(AttackingCharacter))
		{
			AttackingCharacter->HandleOnCheckInputAttack();
		}
	}

}
