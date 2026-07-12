// SXGASHPBarWidget.cpp


#include "SXGASHPBarWidget.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySystem/AS/SX_AS_Character.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "SXGameplayTags.h"

void USXGASHPBarWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (IsValid(ASC) == true)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(USX_AS_Character::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(USX_AS_Character::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);

		const USX_AS_Character* CurrentAttributeSet = ASC->GetSet<USX_AS_Character>();
		if (CurrentAttributeSet)
		{
			CurrentHealth = CurrentAttributeSet->GetHealth();
			CurrentMaxHealth = CurrentAttributeSet->GetMaxHealth();

			if (0.0f < CurrentMaxHealth)
			{
				UpdateHpBar();
			}
		}

		ASC->RegisterGameplayTagEvent(SXGameplayTags::State_Condition_Combat_Invincible, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnInvincibleTagGranted);
		HPBar->SetFillColorAndOpacity(NormalHPBarColor);
	}
}

void USXGASHPBarWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealth = ChangeData.NewValue;
	UpdateHpBar();
}

void USXGASHPBarWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxHealth = ChangeData.NewValue;
	UpdateHpBar();
}

void USXGASHPBarWidget::UpdateHpBar()
{
	if (HPBar)
	{
		HPBar->SetPercent(CurrentHealth / CurrentMaxHealth);
	}

	if (HPText)
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %0.f"), CurrentHealth, CurrentMaxHealth)));
	}
}

void USXGASHPBarWidget::OnInvincibleTagGranted(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (0 < NewCount)
	{
		HPBar->SetFillColorAndOpacity(InvincibleHPBarColor);
		HPBar->SetPercent(1.0f);

		HPText->SetText(FText::FromString(FString::Printf(TEXT("Inf / Inf"))));
	}
	else
	{
		HPBar->SetFillColorAndOpacity(NormalHPBarColor);
		UpdateHpBar();
	}
}
