// SXCharacterBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SXCharacterBase.generated.h"

class UAnimMontage;

UCLASS()
class SHOOTERX_API ASXCharacterBase : public ACharacter
{
	GENERATED_BODY()

#pragma region Override ACharacter

public:
	ASXCharacterBase();

protected:
	virtual void BeginPlay() override;

#pragma endregion

#pragma region Attack

public:
	UFUNCTION()
	void HandleOnCheckHit();

	UFUNCTION()
	void HandleOnCheckInputAttack();

	virtual void BeginAttack();

	UFUNCTION()
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMeleeMontage;

	FString AttackAnimMontageSectionPrefix = FString(TEXT("Attack"));

	int32 MaxComboCount = 3;

	int32 CurrentComboCount = 0;

	bool bIsNowAttacking = false;

	bool bIsAttackKeyPressed = false;

	FOnMontageEnded OnMeleeAttackMontageEndedDelegate;

#pragma endregion

};
