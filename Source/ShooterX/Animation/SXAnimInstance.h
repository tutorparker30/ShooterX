// SXAnimInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SXAnimInstance.generated.h"

class ASXCharacterBase;
class UCharacterMovementComponent;

/**
 *
 */
UCLASS()
class SHOOTERX_API USXAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

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

};
