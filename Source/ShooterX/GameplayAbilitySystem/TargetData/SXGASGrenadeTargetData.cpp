// SXGASGrenadeTargetData.cpp

#include "GameplayAbilitySystem/TargetData/SXGASGrenadeTargetData.h"

bool FSXGameplayAbilityTargetData_GrenadeThrow::NetSerialize(FArchive& Archive, UPackageMap* Map, bool& bOutSuccess)
{
	bool bStartLocationSuccess = true;
	bool bLaunchVelocitySuccess = true;

	StartLocation.NetSerialize(Archive, Map, bStartLocationSuccess);
	LaunchVelocity.NetSerialize(Archive, Map, bLaunchVelocitySuccess);

	bOutSuccess = bStartLocationSuccess && bLaunchVelocitySuccess;

	return true;
}

bool FSXGameplayAbilityTargetData_GrenadeExplosion::NetSerialize(FArchive& Archive, UPackageMap* Map, bool& bOutSuccess)
{
	bool bLocationSuccess = true;

	ExplosionLocation.NetSerialize(Archive, Map, bLocationSuccess);

	Archive << ExplosionRadius;
	Archive << ExplosionDamage;

	bOutSuccess = bLocationSuccess;

	return true;
}
