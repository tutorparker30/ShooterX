// SXPlayerCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "Character/SXCharacterBase.h"
#include "InputActionValue.h"
#include "Game/SXPlayerState.h"
#include "SXPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USXInputConfig;
class UInputMappingContext;
class UNiagaraComponent;
struct FStreamableHandle;
class ASXLandMine;

/**
 *
 */
UCLASS()
class SHOOTERX_API ASXPlayerCharacter : public ASXCharacterBase
{
	GENERATED_BODY()

#pragma region Override ACharacter

public:
	ASXPlayerCharacter();

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UCameraComponent> CameraComponent;

#pragma endregion

#pragma region Input

public:
	float GetCurrentAimPitch() const { return CurrentAimPitch; }

private:
	void InputMove(const FInputActionValue& InValue);

	void InputLook(const FInputActionValue& InValue);

	void InputAttackMelee(const FInputActionValue& InValue);

	void InputAttackRanged(const FInputActionValue& InValue);

	void TryFire();

	void InputMenu(const FInputActionValue& InValue);

	void InputSpawnLandMine(const FInputActionValue& InValue);

	UFUNCTION(Server, Unreliable) // 한 두번 정도는 씹혀도 되기 때문.
	void ServerRPCUpdateAimValue(const float& InAimPitchValue);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<USXInputConfig> PlayerCharacterInputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> PlayerCharacterInputMappingContext;

	UPROPERTY(Replicated)
	float CurrentAimPitch = 0.f;

	float PreviousAimPitch = 0.f;

#pragma endregion

#pragma region KillCount

public:
	UNiagaraComponent* GetNiagaraComponent() const { return NiagaraComponent; }

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

#pragma endregion

#pragma region MeshMaterial

public:
	void InitializeCharacterMeshMaterial(const EPlayerColor InPlayerColor);

private:
	void OnMeshMaterialLoadCompleted(FSoftObjectPath Path01, FSoftObjectPath Path02);

protected:
	FSoftObjectPath CurrentPlayerCharacterMeshMaterialPath01 = FSoftObjectPath();

	FSoftObjectPath CurrentPlayerCharacterMeshMaterialPath02 = FSoftObjectPath();

	TSharedPtr<FStreamableHandle> AssetStreamableHandle = nullptr;

#pragma endregion

#pragma region RangedAttack

private:
	bool CanFire() const;

	void Fire();

	bool GetAimLocation(FVector& OutAimLocation) const;

	bool TraceBullet(const FVector& InMuzzleLocation, const FVector& InAimLocation, FHitResult& OutHitResult) const;

	void ApplyHitDamage(const FHitResult& InHitResult);

	void PlayFireAnimation();

	void DrawFire(const FVector& InMuzzleLocation, const FHitResult& InHitResult, bool bHit);

	UFUNCTION(Server, Reliable)
	void ServerRPCFire(const FVector& InMuzzleLocation, const FHitResult& InHitResult, bool bHit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCFire(const FVector& InMuzzleLocation, const FHitResult& InHitResult, bool bHit);

#pragma endregion

#pragma region LandMine

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCSpawnLandMine();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ASXLandMine> LandMineClass;

#pragma endregion

#pragma region MeleeAttack

public:
	UFUNCTION()
	void OnCharacterDead();

private:
	UFUNCTION(Server, Reliable)
	void ServerRPCMeleeAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCMeleeAttack();

	void PlayMeleeAttackMontage();

#pragma endregion

};
