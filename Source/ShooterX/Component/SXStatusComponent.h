#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SXStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMaxHPChangedDelegate, float /*InMaxHP*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentHPChangedDelegate, float /*InCurrentHP*/);
DECLARE_MULTICAST_DELEGATE(FOnOutOfCurrentHPDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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


public:
	UPROPERTY()
	float MaxHP;
	UPROPERTY(Transient)
	float CurrentHP;
	UPROPERTY(Transient)
	uint8 bIsDead : 1;

	FOnMaxHPChangedDelegate OnMaxHPChanged;
	FOnCurrentHPChangedDelegate OnCurrentHPChanged;
	FOnOutOfCurrentHPDelegate OnOutOfCurrentHP;
};
