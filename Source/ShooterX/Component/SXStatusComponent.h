// SXStatusComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SXStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHPChangedDelegate, float /*InMaxHP*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentHPChangedDelegate, float /*InCurrentHP*/);
DECLARE_MULTICAST_DELEGATE(FOnOutOfCurrentHPDelegate);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTERX_API USXStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USXStatusComponent();

	float ApplyDamage(float InDamage);

	float GetMaxHP() const { return MaxHP; }
	void SetMaxHP(float InMaxHP);

	float GetCurrentHP() const { return CurrentHP; }
	void SetCurrentHP(float InCurrentHP);

	bool IsDead() const { return bIsDead; }

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void TakeBuff(float InBuffValue);

protected:
	UFUNCTION()
	void OnRep_MaxHP();

	UFUNCTION()
	void OnRep_CurrentHP();

public:
	//UPROPERTY()
	UPROPERTY(ReplicatedUsing = OnRep_MaxHP)
	float MaxHP;

	//UPROPERTY(Transient)
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP)
	float CurrentHP;

	UPROPERTY(Transient)
	uint8 bIsDead : 1;

	FOnMaxHPChangedDelegate OnMaxHPChanged;

	FOnCurrentHPChangedDelegate OnCurrentHPChanged;

	FOnOutOfCurrentHPDelegate OnOutOfCurrentHP;

};
