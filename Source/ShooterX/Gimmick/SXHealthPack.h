// SXHealthPack.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SXHealthPack.generated.h"

class USceneComponent;
class UBoxComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;

UCLASS()
class SHOOTERX_API ASXHealthPack : public AActor
{
	GENERATED_BODY()

public:
	ASXHealthPack();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "ShooterX|SXHealthPack")
	float HealAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "ShooterX|SXHealthPack")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "ShooterX|SXHealthPack")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "ShooterX|SXHealthPack")
	TObjectPtr<UStaticMeshComponent> BodyStaticMeshComponent;

	FVector StartLocation;

	float ElapsedTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "ShooterX|SXHealthPack")
	float Amplitude = 10.f;

	UPROPERTY(EditAnywhere, Category = "ShooterX|SXHealthPack")
	float MovementSpeed = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "ShooterX|SXHealthPack")
	TObjectPtr<URotatingMovementComponent> RotatingMovementComponent;

	UPROPERTY(EditAnywhere, Category = "ShooterX|SXHealthPack")
	float RotationSpeed = 300.f;

};
