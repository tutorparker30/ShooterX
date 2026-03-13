#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SXAnimInstance.generated.h"

class ASXCharacterBase;
class UCharacterMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCheckHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPostDead);

UCLASS()
class SHOOTERX_API USXAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UFUNCTION()
	void AnimNotify_CheckHit();
	UFUNCTION()
	void AnimNotify_PostDead();

public:
	FOnCheckHit OnCheckHit;
	FOnPostDead OnPostDead;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ASXCharacterBase> OwnerCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Velocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GroundSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bShouldMove : 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bIsFalling : 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 bIsDead : 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bIsUnarmed : 1;

	UPROPERTY(BlueprintReadOnly)
	float NormalizedCurrentPitch;

};
