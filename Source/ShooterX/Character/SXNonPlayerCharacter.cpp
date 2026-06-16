// SXNonPlayerCharacter.cpp


#include "Character/SXNonPlayerCharacter.h"

#include "Controller/SXAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

ASXNonPlayerCharacter::ASXNonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 50.f;

	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	AIControllerClass = ASXAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
		// ASXNonPlayerCharacter는 레벨에 배치되거나 새롭게 생성되면 SXAIController의 빙의가 자동으로 진행됨.
}
