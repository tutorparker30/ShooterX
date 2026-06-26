// SXHealthPack.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SXHealthPack.generated.h"

class USceneComponent;
class UBoxComponent;
class UStaticMeshComponent;
class URotatingMovementComponent;
class UNiagaraComponent;

UCLASS()
class SHOOTERX_API ASXHealthPack : public AActor
{
	GENERATED_BODY()

public:
	ASXHealthPack();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION()
	void OnEffectFinish(UNiagaraComponent* NiagaraComponent);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

private:
	UFUNCTION()
	void OnRep_ServerRotationYaw();

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

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	//UPROPERTY(Replicated)
	UPROPERTY(ReplicatedUsing = OnRep_ServerRotationYaw)
	float ServerRotationYaw;

	float NetUpdatePeriod;

	float AccDeltaSecondSinceReplicated = 0.f;

	float NetCullDistance = 1000.f;

};
