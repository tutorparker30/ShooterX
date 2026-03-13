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

private:
	void UpDown(float InAxisValue);
	void LeftRight(float InAxisValue);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ASXPlayerPawn")
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
	UPROPERTY(EditDefaultsOnly, Category = "ASXPlayerPawn")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "ASXPlayerPawn")
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovementComponent;
	UPROPERTY(EditDefaultsOnly, Category = "ASXPlayerPawn")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditDefaultsOnly, Category = "ASXPlayerPawn")
	TObjectPtr<UCameraComponent> CameraComponent;

};
