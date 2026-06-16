// SXAIController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SXAIController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERX_API ASXAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASXAIController();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void OnPatrolTimerElapsed();

public:
	FTimerHandle PatrolTimerHandle = FTimerHandle();

	static const float PatrolRepeatInterval;

	static const float PatrolRadius;

};
