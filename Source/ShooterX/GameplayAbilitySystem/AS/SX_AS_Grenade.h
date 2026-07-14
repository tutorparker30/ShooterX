// SX_AS_Grenade.h

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SX_AS_Grenade.generated.h"

struct FGameplayEffectModCallbackData;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class SHOOTERX_API USX_AS_Grenade : public UAttributeSet
{
	GENERATED_BODY()

public:
	USX_AS_Grenade();

public:
	ATTRIBUTE_ACCESSORS(ThisClass, ExplosionRadius);
	ATTRIBUTE_ACCESSORS(ThisClass, ExplosionDamage);
	ATTRIBUTE_ACCESSORS(ThisClass, CurrentGrenadeCount);
	ATTRIBUTE_ACCESSORS(ThisClass, MaxGrenadeCount);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	UFUNCTION()
	void OnRep_ExplosionRadius(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ExplosionDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_CurrentGrenadeCount(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxGrenadeCount(const FGameplayAttributeData& OldValue);

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExplosionRadius, Category = "SX|Grenade")
	FGameplayAttributeData ExplosionRadius;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExplosionDamage, Category = "SX|Grenade")
	FGameplayAttributeData ExplosionDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentGrenadeCount, Category = "SX|Grenade")
	FGameplayAttributeData CurrentGrenadeCount;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxGrenadeCount, Category = "SX|Grenade")
	FGameplayAttributeData MaxGrenadeCount;

};