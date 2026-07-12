// SXGASHPBarWidget.h

#pragma once

#include "SXGASUserWidget.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "SXGASHPBarWidget.generated.h"

class UProgressBar;
class UTextBlock;

/**
 *
 */
UCLASS()
class SHOOTERX_API USXGASHPBarWidget : public USXGASUserWidget
{
	GENERATED_BODY()

protected:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);

	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);

	void UpdateHpBar();

	UFUNCTION()
	void OnInvincibleTagGranted(const FGameplayTag CallbackTag, int32 NewCount);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HPText;

	float CurrentHealth = 0.0f;

	float CurrentMaxHealth = 0.1f;

	FLinearColor NormalHPBarColor = FLinearColor::Red;

	FLinearColor InvincibleHPBarColor = FLinearColor::Yellow;

};
