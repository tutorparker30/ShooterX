// SX_TA_GrenadeTrajectory.cpp

#include "GameplayAbilitySystem/TA/SX_TA_GrenadeTrajectory.h"

#include "Abilities/GameplayAbility.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStaticsTypes.h"

#include "Item/SXGASGrenade.h"
#include "Item/SXGASGrenadeFunctionLibrary.h"

ASX_TA_GrenadeTrajectory::ASX_TA_GrenadeTrajectory()
	: AimTraceDistance(5000.0f)
	, StartForwardOffset(20.0f)
	, MaxSimTime(3.0f)
	, SimFrequency(15.0f)
	, bHasValidThrowData(false)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	TrajectorySpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrajectorySpline"));
	TrajectorySpline->SetupAttachment(SceneRoot);

	LandingMarker =	CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LandingMarker"));
	LandingMarker->SetupAttachment(SceneRoot);
	LandingMarker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LandingMarker->SetGenerateOverlapEvents(false);
	LandingMarker->SetCastShadow(false);
	LandingMarker->SetVisibility(false);
}

void ASX_TA_GrenadeTrajectory::ConfigureTrajectory(TSubclassOf<ASXGASGrenade> InGrenadeClass, FName InThrowSocketName)
{
	GrenadeClass = InGrenadeClass;
	ThrowSocketName = InThrowSocketName;
}

void ASX_TA_GrenadeTrajectory::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	if (IsValid(Ability) == false)
	{
		ClearTrajectory();
		SetActorTickEnabled(false);
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();

	if (ActorInfo == nullptr)
	{
		ClearTrajectory();
		SetActorTickEnabled(false);
		return;
	}

	SourceActor = ActorInfo->AvatarActor.Get();

	PrimaryPC = ActorInfo->PlayerController.Get();

	ACharacter* SourceCharacter = Cast<ACharacter>(SourceActor);

	if (IsValid(SourceCharacter) == false || SourceCharacter->IsLocallyControlled() == false)
	{
		ClearTrajectory();
		SetActorTickEnabled(false);
		return;
	}

	SetActorTickEnabled(true);
	RefreshTrajectory();
}

void ASX_TA_GrenadeTrajectory::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RefreshTrajectory();
}

bool ASX_TA_GrenadeTrajectory::RefreshTrajectory()
{
	ACharacter* SourceCharacter = Cast<ACharacter>(SourceActor);

	if (IsValid(SourceCharacter) == false ||
		SourceCharacter->IsLocallyControlled() == false ||
		IsValid(GrenadeClass) == false ||
		ThrowSocketName.IsNone())
	{
		ClearTrajectory();
		return false;
	}

	FSXGASGrenadeThrowData NewThrowData;

	const bool bThrowDataValid = USXGASGrenadeFunctionLibrary::CalculateGrenadeThrowData(
			SourceCharacter,
			ThrowSocketName,
			GrenadeClass,
			AimTraceDistance,
			StartForwardOffset,
			NewThrowData);

	if (bThrowDataValid == false)
	{
		ClearTrajectory();
		return false;
	}

	FPredictProjectilePathResult PathResult;

	const bool bPathValid = USXGASGrenadeFunctionLibrary::PredictGrenadePath(
			this,
			SourceCharacter,
			GrenadeClass,
			NewThrowData,
			MaxSimTime,
			SimFrequency,
			PathResult);

	if (bPathValid == false)
	{
		ClearTrajectory();
		return false;
	}

	CurrentThrowData = NewThrowData;
	bHasValidThrowData = true;

	UpdateSpline(PathResult);

	return true;
}

bool ASX_TA_GrenadeTrajectory::GetCurrentThrowData(FSXGASGrenadeThrowData& OutThrowData) const
{
	if (bHasValidThrowData == false || CurrentThrowData.IsValid() == false)
	{
		OutThrowData = FSXGASGrenadeThrowData();

		return false;
	}

	OutThrowData = CurrentThrowData;
	return true;
}

void ASX_TA_GrenadeTrajectory::EnsureSplineMeshPool(int32 RequiredSegmentCount)
{
	if (RequiredSegmentCount <=	SplineMeshPool.Num())
	{
		return;
	}

	for (int32 SegmentIndex = SplineMeshPool.Num();	SegmentIndex < RequiredSegmentCount; ++SegmentIndex)
	{
		USplineMeshComponent* NewSplineMesh = NewObject<USplineMeshComponent>(this);

		if (IsValid(NewSplineMesh) == false)
		{
			break;
		}

		AddInstanceComponent(NewSplineMesh);

		NewSplineMesh->SetupAttachment(TrajectorySpline);
		NewSplineMesh->SetMobility(EComponentMobility::Movable);
		NewSplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NewSplineMesh->SetGenerateOverlapEvents(false);
		NewSplineMesh->SetCastShadow(false);
		NewSplineMesh->SetStaticMesh(TrajectorySegmentMesh);
		if (IsValid(TrajectoryMaterial))
		{
			NewSplineMesh->SetMaterial(0, TrajectoryMaterial);
		}
		NewSplineMesh->SetForwardAxis(ESplineMeshAxis::X, false);
		NewSplineMesh->RegisterComponent();

		SplineMeshPool.Add(NewSplineMesh);
	}
}

void ASX_TA_GrenadeTrajectory::UpdateSpline(const FPredictProjectilePathResult& PathResult)
{
	if (IsValid(TrajectorySpline) == false ||
		IsValid(TrajectorySegmentMesh) == false ||
		PathResult.PathData.Num() < 2)
	{
		ClearTrajectory();
		return;
	}

	TrajectorySpline->ClearSplinePoints(false);

	for (const FPredictProjectilePathPointData& PathPoint : PathResult.PathData)
	{
		TrajectorySpline->AddSplinePoint(PathPoint.Location, ESplineCoordinateSpace::World, false);
	}

	const int32 SplinePointCount = TrajectorySpline->GetNumberOfSplinePoints();
	for (int32 PointIndex = 0; PointIndex < SplinePointCount; ++PointIndex)
	{
		TrajectorySpline->SetSplinePointType(PointIndex, ESplinePointType::Linear, false);
	}

	TrajectorySpline->UpdateSpline();

	const int32 RequiredSegmentCount = SplinePointCount - 1;
	EnsureSplineMeshPool(RequiredSegmentCount);

	const int32 UsableSegmentCount = FMath::Min(RequiredSegmentCount, SplineMeshPool.Num());
	for (int32 SegmentIndex = 0; SegmentIndex < UsableSegmentCount; ++SegmentIndex)
	{
		USplineMeshComponent* SplineMesh = SplineMeshPool[SegmentIndex];
		if (IsValid(SplineMesh) == false)
		{
			continue;
		}

		const FVector StartPosition = TrajectorySpline->GetLocationAtSplinePoint(SegmentIndex, ESplineCoordinateSpace::Local);
		const FVector EndPosition = TrajectorySpline->GetLocationAtSplinePoint(SegmentIndex + 1, ESplineCoordinateSpace::Local);
		const FVector SegmentTangent = EndPosition - StartPosition;

		SplineMesh->SetStartAndEnd(StartPosition, SegmentTangent, EndPosition, SegmentTangent, true);
		SplineMesh->SetVisibility(true, true);
	}

	HideUnusedSplineMeshes(UsableSegmentCount);

	if (IsValid(LandingMarker))
	{
		FVector LandingLocation = PathResult.PathData.Last().Location;

		if (PathResult.HitResult.bBlockingHit)
		{
			LandingLocation = PathResult.HitResult.ImpactPoint;
		}

		LandingMarker->SetWorldLocation(LandingLocation);

		LandingMarker->SetVisibility(true, true);
	}
}

void ASX_TA_GrenadeTrajectory::HideUnusedSplineMeshes(int32 UsedSegmentCount)
{
	for (int32 SegmentIndex = UsedSegmentCount; SegmentIndex < SplineMeshPool.Num(); ++SegmentIndex)
	{
		USplineMeshComponent* SplineMesh = SplineMeshPool[SegmentIndex];
		if (IsValid(SplineMesh))
		{
			SplineMesh->SetVisibility(false, true);
		}
	}
}

void ASX_TA_GrenadeTrajectory::ClearTrajectory()
{
	bHasValidThrowData = false;
	CurrentThrowData = FSXGASGrenadeThrowData();

	if (IsValid(TrajectorySpline))
	{
		TrajectorySpline->ClearSplinePoints(true);
	}

	HideUnusedSplineMeshes(0);

	if (IsValid(LandingMarker))
	{
		LandingMarker->SetVisibility(false, true);
	}
}