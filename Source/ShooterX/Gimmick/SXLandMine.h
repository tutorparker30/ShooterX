// SXLandMine.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SXLandMine.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class SHOOTERX_API ASXLandMine : public AActor
{
	GENERATED_BODY()

public:
	ASXLandMine();

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION()
	void OnEffectFinish(UNiagaraComponent* NiagaraComponent);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCSpawnEffect();

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

};