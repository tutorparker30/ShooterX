#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SXCharacterBase.generated.h"

class UAnimMontage;
class USXStatusComponent;
class ASXWeapon;

UCLASS()
class SHOOTERX_API ASXCharacterBase : public ACharacter
{
	GENERATED_BODY()

#pragma region Override ACharacter

public:
	ASXCharacterBase();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

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
	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser
	) override;
	UAnimMontage* GetCurrentWeaponAttackAnimMontage() const;

public:
	static int32 ShowAttackMeleeDebug;
	static int32 ShowAttackRangedDebug;
	UPROPERTY()
	TObjectPtr<ASXWeapon> CurrentWeapon;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMeleeMontage;
	FString AttackAnimMontageSectionPrefix = FString(TEXT("Attack"));
	int32 MaxComboCount = 3;
	int32 CurrentComboCount = 0;
	bool bIsNowAttacking = false;
	bool bIsAttackKeyPressed = false;
	FOnMontageEnded OnMeleeAttackMontageEndedDelegate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackMeleeRange = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackMeleeRadius = 20.f;

#pragma endregion

#pragma region HP

public:
	USXStatusComponent* GetStatusComponent() const { return StatusComponent; }

protected:
	UFUNCTION()
	virtual void HandleOnPostCharacterDead();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<USXStatusComponent> StatusComponent;

#pragma endregion

#pragma region Ragdoll

private:
	UFUNCTION()
	void OnHittedRagdollRestoreTimerElapsed();

protected:
	FTimerHandle HittedRagdollRestoreTimer;
	FTimerDelegate HittedRagdollRestoreTimerDelegate;
	float TargetRagDollBlendWeight = 0.f;
	float CurrentRagDollBlendWeight = 0.f;
	bool bIsNowRagdollBlending = false;

#pragma endregion

};
