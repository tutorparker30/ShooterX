#include "AI/BTTask_TurnToTarget.h"
#include "Controller/SXAIController.h"
#include "Character/SXNonPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTargetActor");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ASXAIController* AIController = Cast<ASXAIController>(OwnerComp.GetAIOwner());
	checkf(IsValid(AIController) == true, TEXT("Invalid AIController."));

	ASXNonPlayerCharacter* NPC = Cast<ASXNonPlayerCharacter>(AIController->GetPawn());
	checkf(IsValid(NPC) == true, TEXT("Invalid NPC."));

	if (ASXCharacterBase* TargetPC = Cast<ASXCharacterBase>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AIController->TargetCharacterKey)))
	{
		FVector LookVector = TargetPC->GetActorLocation() - NPC->GetActorLocation();
		LookVector.Z = 0.f;
		FRotator TargetRotation = FRotationMatrix::MakeFromX(LookVector).Rotator();
		NPC->SetActorRotation(FMath::RInterpTo(NPC->GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 2.f));

		return Result = EBTNodeResult::Succeeded;
	}

	return Result = EBTNodeResult::Failed;


}
