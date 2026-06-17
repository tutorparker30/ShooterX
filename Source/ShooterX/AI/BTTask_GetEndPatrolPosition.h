// BTTask_GetEndPatrolPosition.h

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BTTask_GetEndPatrolPosition.generated.h"

/**
 *
 */
UCLASS()
class SHOOTERX_API UBTTask_GetEndPatrolPosition : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_GetEndPatrolPosition();

private:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
    UPROPERTY(EditInstanceOnly, Category = "Blackboard")
    FBlackboardKeySelector StartPatrolPositionKey;

    UPROPERTY(EditInstanceOnly, Category = "Blackboard")
    FBlackboardKeySelector EndPatrolPositionKey;

};
