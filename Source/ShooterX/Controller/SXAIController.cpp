#include "Controller/SXAIController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

const float ASXAIController::PatrolRadius(500.f);
int32 ASXAIController::ShowAIDebug(0);
const FName ASXAIController::StartPatrolPositionKey(TEXT("StartPatrolPosition"));
const FName ASXAIController::EndPatrolPositionKey(TEXT("EndPatrolPosition"));
const FName ASXAIController::TargetCharacterKey(TEXT("TargetCharacter"));

FAutoConsoleVariableRef CVarShowAIDebug(
	TEXT("SXProject.ShowAIDebug"),
	ASXAIController::ShowAIDebug,
	TEXT(""),
	ECVF_Cheat
);

ASXAIController::ASXAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BrainComponent"));
}

void ASXAIController::BeginPlay()
{
	Super::BeginPlay();

	APawn* ControlledPawn = GetPawn();
	if (IsValid(ControlledPawn))
	{
		BeginAI(ControlledPawn);
	}

}

void ASXAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	EndAI();

	Super::EndPlay(EndPlayReason);
}

void ASXAIController::BeginAI(APawn* InPawn)
{
	UBlackboardComponent* BlackboardComponent = Cast<UBlackboardComponent>(Blackboard);
	if (IsValid(BlackboardComponent))
	{
		if (UseBlackboard(BlackboardDataAsset, BlackboardComponent) == true)
		{
			bool bRunSucceeded = RunBehaviorTree(BehaviorTree);
			checkf(bRunSucceeded == true, TEXT("Fail to run behavior tree."));

			BlackboardComponent->SetValueAsVector(StartPatrolPositionKey, InPawn->GetActorLocation());

			if (ShowAIDebug == 1)
			{
				UKismetSystemLibrary::PrintString(
					this,
					FString(TEXT("BeginAI()"))
				);
			}

		}
	}
}

void ASXAIController::EndAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (IsValid(BehaviorTreeComponent))
	{
		BehaviorTreeComponent->StopTree();

		if (ShowAIDebug == 1)
		{
			UKismetSystemLibrary::PrintString(
				this,
				FString::Printf(TEXT("EndAI()"))
			);
		}
	}

}

