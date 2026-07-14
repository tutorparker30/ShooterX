// SXGASGrenadeFunctionLibrary.cpp

#include "Item/SXGASGrenadeFunctionLibrary.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Item/SXGASGrenade.h"

bool USXGASGrenadeFunctionLibrary::CalculateGrenadeThrowData(
	ACharacter* SourceCharacter,
	FName ThrowSocketName,
	TSubclassOf<ASXGASGrenade> GrenadeClass,
	float AimTraceDistance,
	float StartForwardOffset,
	FSXGASGrenadeThrowData& OutThrowData)
{
	OutThrowData = FSXGASGrenadeThrowData();

	if (IsValid(SourceCharacter) == false || IsValid(GrenadeClass) == false || ThrowSocketName.IsNone() || AimTraceDistance <= 0.0f)
	{
		return false;
	}

	UWorld* World = SourceCharacter->GetWorld();
	if (IsValid(World) == false)
	{
		return false;
	}

	USkeletalMeshComponent* CharacterMesh = SourceCharacter->GetMesh();
	if (IsValid(CharacterMesh) == false || CharacterMesh->DoesSocketExist(ThrowSocketName) == false)
	{
		return false;
	}

	APlayerController* PlayerController = Cast<APlayerController>(SourceCharacter->GetController());
	if (IsValid(PlayerController) == false)
	{
		return false;
	}

	const ASXGASGrenade* GrenadeCDO = GrenadeClass->GetDefaultObject<ASXGASGrenade>();
	if (IsValid(GrenadeCDO) == false)
	{
		return false;
	}

	const float InitialSpeed = GrenadeCDO->GetGrenadeInitialSpeed();
	const float GravityScale = GrenadeCDO->GetGrenadeGravityScale();
	if (InitialSpeed <= 0.0f ||	GravityScale < 0.0f)
	{
		return false;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
	const FVector ViewDirection = ViewRotation.Vector().GetSafeNormal();
	if (ViewDirection.IsNearlyZero())
	{
		return false;
	}

	const FVector AimTraceEnd = ViewLocation + ViewDirection * AimTraceDistance;

	FCollisionQueryParams AimTraceParams(SCENE_QUERY_STAT(GrenadeAimTrace), false, SourceCharacter);
	FHitResult AimHitResult;

	const bool bAimHit = World->LineTraceSingleByChannel(AimHitResult, ViewLocation, AimTraceEnd, ECC_Visibility, AimTraceParams);
	const FVector AimPoint = bAimHit ? AimHitResult.ImpactPoint : AimTraceEnd;
	const FVector SocketLocation = CharacterMesh->GetSocketLocation(ThrowSocketName);

	FVector ThrowDirection = (AimPoint - SocketLocation).GetSafeNormal();
	if (ThrowDirection.IsNearlyZero())
	{
		ThrowDirection = ViewDirection;
	}

	const float SafeStartForwardOffset = FMath::Max(StartForwardOffset, 0.0f);
	const FVector StartLocation = SocketLocation + ThrowDirection * SafeStartForwardOffset;
	OutThrowData.StartLocation = StartLocation;
	OutThrowData.AimPoint = AimPoint;
	OutThrowData.LaunchVelocity = ThrowDirection * InitialSpeed;
	OutThrowData.GravityZ = World->GetGravityZ() * GravityScale;

	return OutThrowData.IsValid();
}

bool USXGASGrenadeFunctionLibrary::PredictGrenadePath(
	const UObject* WorldContextObject,
	AActor* SourceActor,
	TSubclassOf<ASXGASGrenade> GrenadeClass,
	const FSXGASGrenadeThrowData& ThrowData,
	float MaxSimTime,
	float SimFrequency,
	FPredictProjectilePathResult& OutPathResult)
{
	OutPathResult = FPredictProjectilePathResult();

	if (IsValid(WorldContextObject) == false ||
		IsValid(GrenadeClass) == false ||
		ThrowData.IsValid() == false ||
		MaxSimTime <= 0.0f ||
		SimFrequency <= 0.0f)
	{
		return false;
	}

	const ASXGASGrenade* GrenadeCDO = GrenadeClass->GetDefaultObject<ASXGASGrenade>();

	if (IsValid(GrenadeCDO) == false)
	{
		return false;
	}

	const float ProjectileRadius = GrenadeCDO->GetGrenadeCollisionRadius();
	if (ProjectileRadius <= 0.0f)
	{
		return false;
	}

	FPredictProjectilePathParams PredictParams;
	PredictParams.ProjectileRadius = ProjectileRadius;
	PredictParams.StartLocation = ThrowData.StartLocation;
	PredictParams.LaunchVelocity = ThrowData.LaunchVelocity;
	PredictParams.MaxSimTime = MaxSimTime;
	PredictParams.SimFrequency = FMath::Clamp(SimFrequency, 1.0f, 30.0f);
	PredictParams.OverrideGravityZ = ThrowData.GravityZ;
	PredictParams.bTraceWithCollision = true;
	PredictParams.bTraceWithChannel = false;
	PredictParams.bTraceComplex = false;
	PredictParams.DrawDebugType = EDrawDebugTrace::None;
	PredictParams.ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	PredictParams.ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	if (IsValid(SourceActor))
	{
		PredictParams.ActorsToIgnore.Add(SourceActor);
	}

	UGameplayStatics::PredictProjectilePath(WorldContextObject, PredictParams, OutPathResult);

	return OutPathResult.PathData.Num() >= 2;
}
