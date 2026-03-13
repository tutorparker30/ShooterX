#pragma once

#include "CoreMinimal.h"
#include "Character/SXCharacterBase.h"
#include "SXNonPlayerCharacter.generated.h"

class USXHPTextWidgetComponent;
class UUW_HPText;

DECLARE_DELEGATE_TwoParams(FOnAttackMontageEnded, UAnimMontage*, bool /*bInterrupted*/)

UCLASS()
class SHOOTERX_API ASXNonPlayerCharacter : public ASXCharacterBase
{
	GENERATED_BODY()

	friend class UBTTask_Attack;

public:
	ASXNonPlayerCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetHPTextWidget(UUW_HPText* InHPTextWidget);

protected:
	virtual void BeginAttack() override;
	virtual void EndAttack(UAnimMontage* InMontage, bool bInterruped) override;
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

public:
	bool bIsNowAttacking;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USXHPTextWidgetComponent> HPTextWidgetComponent;
	FOnAttackMontageEnded OnAttackMontageEndedDelegate;

};
