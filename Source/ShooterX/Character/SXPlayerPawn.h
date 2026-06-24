// SXPlayerPawn.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SXPlayerPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class SHOOTERX_API ASXPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	ASXPlayerPawn();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

private:
	void FrontRear(float InAxisValue);

	void LeftRight(float InAxisValue);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ShooterX|ASXPlayerPawn")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, Category = "ShooterX|ASXPlayerPawn")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "ShooterX|ASXPlayerPawn")
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "ShooterX|ASXPlayerPawn")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, Category = "ShooterX|ASXPlayerPawn")
	TObjectPtr<UCameraComponent> CameraComponent;

};

