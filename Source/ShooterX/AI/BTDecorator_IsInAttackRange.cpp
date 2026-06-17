// BTDecorator_IsInAttackRange.cpp


#include "AI/BTDecorator_IsInAttackRange.h"
#include "Character/SXPlayerCharacter.h"
#include "Controller/SXAIController.h"
#include "Character/SXNonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

const float UBTDecorator_IsInAttackRange::AttackRange(200.f);

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("IsInAttackRange");

	TargetCharacterKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, TargetCharacterKey), ASXPlayerCharacter::StaticClass());
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	checkf(bResult == true, TEXT("Super::CalculateRawConditionValue() function has returned false."));

	ASXAIController* AIController = Cast<ASXAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	ASXNonPlayerCharacter* NPC = Cast<ASXNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	ASXPlayerCharacter* TargetPlayerCharacter = Cast<ASXPlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetCharacterKey.SelectedKeyName));
	if (IsValid(TargetPlayerCharacter) == true)
	{
		return NPC->GetDistanceTo(TargetPlayerCharacter) <= AttackRange;
	}

	return false;
}

