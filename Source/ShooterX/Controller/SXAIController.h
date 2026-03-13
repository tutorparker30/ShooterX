#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SXAIController.generated.h"

class UBlackboardData;
class UBehaviorTree;

UCLASS()
class SHOOTERX_API ASXAIController : public AAIController
{
	GENERATED_BODY()
	
	friend class ASXNonPlayerCharacter;

public:
	ASXAIController();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void BeginAI(APawn* InPawn);
	void EndAI();

public:
	static const float PatrolRadius;
	static int32 ShowAIDebug;
	static const FName StartPatrolPositionKey;
	static const FName EndPatrolPositionKey;
	static const FName TargetCharacterKey;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UBlackboardData> BlackboardDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UBehaviorTree> BehaviorTree;

};
