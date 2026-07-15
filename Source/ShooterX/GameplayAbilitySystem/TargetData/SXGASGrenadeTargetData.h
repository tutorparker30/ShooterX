// SXGASGrenadeTargetData.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "SXGASGrenadeTargetData.generated.h"

/**
 * 소유 클라이언트가 서버에 전달하는 수류탄 투척 데이터.
 */
USTRUCT()
struct SHOOTERX_API FSXGameplayAbilityTargetData_GrenadeThrow : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("StartLocation=%s, LaunchVelocity=%s"), *StartLocation.ToString(), *LaunchVelocity.ToString());
	}

	bool IsValidData() const
	{
		return StartLocation.ContainsNaN() == false &&
			LaunchVelocity.ContainsNaN() == false &&
			LaunchVelocity.IsNearlyZero() == false;
	}

	bool NetSerialize(FArchive& Archive, UPackageMap* Map, bool& bOutSuccess);

public:
	UPROPERTY()
	FVector_NetQuantize100 StartLocation;
		// FVector를 그대로 송신 하는 것보다 네트워크 대역폭을 줄일 수 있음.

	UPROPERTY()
	FVector_NetQuantize100 LaunchVelocity;

};

template<>
struct TStructOpsTypeTraits<FSXGameplayAbilityTargetData_GrenadeThrow> 
	: public TStructOpsTypeTraitsBase2<FSXGameplayAbilityTargetData_GrenadeThrow>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true,
	};
};

/**
 * 수류탄 폭발 시 CheckHit 어빌리티에 전달하는 데이터.
 */
USTRUCT()
struct SHOOTERX_API FSXGameplayAbilityTargetData_GrenadeExplosion : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual FString ToString() const override
	{
		return FString::Printf(TEXT("Location=%s, Radius=%.1f, Damage=%.1f"), *ExplosionLocation.ToString(), ExplosionRadius, ExplosionDamage);
	}

	bool IsValidData() const
	{
		return ExplosionLocation.ContainsNaN() == false &&
			FMath::IsFinite(ExplosionRadius) &&
			FMath::IsFinite(ExplosionDamage) &&
			ExplosionRadius > 0.0f &&
			ExplosionDamage >= 0.0f;
	}

	bool NetSerialize(FArchive& Archive, UPackageMap* Map, bool& bOutSuccess);

public:
	UPROPERTY()
	FVector_NetQuantize100 ExplosionLocation;

	UPROPERTY()
	float ExplosionRadius = 0.0f;

	UPROPERTY()
	float ExplosionDamage = 0.0f;

};

template<>
struct TStructOpsTypeTraits<FSXGameplayAbilityTargetData_GrenadeExplosion> 
	: public TStructOpsTypeTraitsBase2<FSXGameplayAbilityTargetData_GrenadeExplosion>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true,
	};
};
