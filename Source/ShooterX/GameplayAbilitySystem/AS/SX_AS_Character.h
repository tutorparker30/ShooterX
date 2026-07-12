// SX_AS_Character.h

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SX_AS_Character.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
		// 게터/세터를 매번 손으로 만들지 않도록 어트리뷰트 셋 전용 매크로가 엔진에서 제공됨.

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOutOfHealthDelegate);

UCLASS()
class SHOOTERX_API USX_AS_Character : public UAttributeSet
{
	GENERATED_BODY()

public:
	USX_AS_Character();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ATTRIBUTE_ACCESSORS(ThisClass, AttackRange);

	ATTRIBUTE_ACCESSORS(ThisClass, AttackRadius);

	ATTRIBUTE_ACCESSORS(ThisClass, AttackDamage);

	ATTRIBUTE_ACCESSORS(ThisClass, Health);
	
	ATTRIBUTE_ACCESSORS(ThisClass, MaxHealth);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	ATTRIBUTE_ACCESSORS(ThisClass, MetaDamage);

	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

public:
	mutable FOnOutOfHealthDelegate OnOutOfHealth;
		// const USX_AS_Character* CurrentAttributeSet = GetAbilitySystemComponent()->GetSet<USX_AS_Character>();
		// GetSet() 함수의 반환 자료형이 const이기 때문에 OnOutOfHealth 속성의 자료형은 mutable로 지정.

protected:
	UPROPERTY(BlueprintReadOnly, Category = "USX_AS_Character|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "USX_AS_Character|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRadius;

	UPROPERTY(BlueprintReadOnly, Category = "USX_AS_Character|Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackDamage;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "USX_AS_Character|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth, BlueprintReadOnly, Category = "USX_AS_Character|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "USX_AS_Character|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MetaDamage;

	bool bOutOfHealth = false;

};