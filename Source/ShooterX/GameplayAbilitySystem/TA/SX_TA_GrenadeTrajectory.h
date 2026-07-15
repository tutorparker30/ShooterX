// SX_TA_GrenadeTrajectory.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Item/SXGASGrenadeTypes.h"
#include "SX_TA_GrenadeTrajectory.generated.h"

class ASXGASGrenade;
class UMaterialInterface;
class USceneComponent;
class USplineComponent;
class USplineMeshComponent;
class UStaticMesh;
class UStaticMeshComponent;
struct FPredictProjectilePathResult;

/**
 * 로컬 플레이어에게 수류탄 예상 경로를 표시하는
 * TargetActor.
 */
UCLASS()
class SHOOTERX_API ASX_TA_GrenadeTrajectory
	: public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ASX_TA_GrenadeTrajectory();

	virtual void Tick(float DeltaSeconds) override;

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	void ConfigureTrajectory(TSubclassOf<ASXGASGrenade> InGrenadeClass,	FName InThrowSocketName);

	bool RefreshTrajectory();

	bool GetCurrentThrowData(FSXGASGrenadeThrowData& OutThrowData) const;

	float GetStartForwardOffset() const
	{
		return StartForwardOffset;
	}

private:
	void UpdateSpline(const FPredictProjectilePathResult& PathResult);

	void EnsureSplineMeshPool(int32 RequiredSegmentCount);

	void HideUnusedSplineMeshes(int32 UsedSegmentCount);

	void ClearTrajectory();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USplineComponent> TrajectorySpline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> LandingMarker;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh>	TrajectorySegmentMesh;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> TrajectoryMaterial;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "100.0"))
	float AimTraceDistance;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0.0"))
	float StartForwardOffset;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0.1"))
	float MaxSimTime;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "1.0", ClampMax = "30.0"))
	float SimFrequency;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshPool;

	UPROPERTY(Transient)
	TSubclassOf<ASXGASGrenade> GrenadeClass;

	FName ThrowSocketName;

	FSXGASGrenadeThrowData CurrentThrowData;

	bool bHasValidThrowData;
	
};