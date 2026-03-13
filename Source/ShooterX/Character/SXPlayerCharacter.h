#pragma once

#include "CoreMinimal.h"
#include "Character/SXCharacterBase.h"
#include "InputActionValue.h"
#include "SXPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USXInputConfig;
class UInputMappingContext;
class UCameraShakeBase;
struct FStreamableHandle;

UCLASS()
class SHOOTERX_API ASXPlayerCharacter : public ASXCharacterBase
{
	GENERATED_BODY()
	
#pragma region Override ACharacter

public:
	ASXPlayerCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UCameraComponent> CameraComponent;

#pragma endregion

#pragma region Input

private:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);
	void InputAttackMelee(const FInputActionValue& InValue);
	void InputAttackRanged(const FInputActionValue& InValue);
	void TryFire();
	void InputToggleSelector(const FInputActionValue& InValue);
	void InputStartFullAutoFire(const FInputActionValue& InValue);
	void InputStopFullAutoFire(const FInputActionValue& InValue);
	void InputStartIronSight(const FInputActionValue& InValue);
	void InputEndIronSight(const FInputActionValue& InValue);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<USXInputConfig> PlayerCharacterInputConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> PlayerCharacterInputMappingContext;

#pragma endregion

#pragma region KillCount

public:
	UParticleSystemComponent* GetParticleSystem() const { return ParticleSystemComponent; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;

#pragma endregion

#pragma region MeshMaterial

protected:
	FSoftObjectPath CurrentPlayerCharacterMeshMaterialPath01 = FSoftObjectPath();
	FSoftObjectPath CurrentPlayerCharacterMeshMaterialPath02 = FSoftObjectPath();
	TSharedPtr<FStreamableHandle> AssetStreamableHandle = nullptr;

#pragma endregion

#pragma region Effect

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCameraShakeBase> AttackRangedCameraShake;

#pragma endregion 

#pragma region Selector

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FirePerMinute = 600;
	bool bIsFullAutoFire = false;
	FTimerHandle FullAutoTimerHandle;
	float TimeBetweenFire;

#pragma endregion

#pragma region IronSight

protected:
	float TargetFOV = 70.f;
	float CurrentFOV = 70.f;

#pragma endregion


};
