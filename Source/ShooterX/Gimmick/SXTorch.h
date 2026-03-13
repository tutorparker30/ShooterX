#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SXTorch.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPointLightComponent;
class UParticleSystemComponent;
class URotatingMovementComponent;

UCLASS()
class SHOOTERX_API ASXTorch : public AActor
{
	GENERATED_BODY()
	
public:	
	ASXTorch();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SXTorch", meta = (AllowPrivateAccess))
	TObjectPtr<UBoxComponent> BoxComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SXTorch", meta = (AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> BodyStaticMeshComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SXTorch", meta = (AllowPrivateAccess))
	TObjectPtr<UPointLightComponent> PointLightComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SXTorch", meta = (AllowPrivateAccess))
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "SXTorch", meta = (AllowPrivateAccess))
	int32 ID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SXTorch", meta = (AllowPrivateAccess))
	float RotationSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SXTorch", meta = (AllowPrivateAccess))
	TObjectPtr<URotatingMovementComponent> RotatingMovementComponent;

};
