// SXGASGrenade.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SXGASGrenade.generated.h"

class UGameplayEffect;
class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class ESXGASGrenadeState : uint8
{
	Pickup,
	Thrown,
};

/**
 * 필드에 배치된 수류탄 아이템과
 * 플레이어가 투척한 수류탄을 표현하는 액터.
 */
UCLASS()
class SHOOTERX_API ASXGASGrenade : public AActor
{
	GENERATED_BODY()

public:
	ASXGASGrenade();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ESXGASGrenadeState GetGrenadeState() const
	{
		return GrenadeState;
	}

	void SetGrenadeState(ESXGASGrenadeState NewGrenadeState);

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	UFUNCTION()
	void OnRep_GrenadeState();
		// 서버에서 수류탄 액터의 상태가 변경되면 클라쪽에도 OnRep 함수 호출되면서
		// 충돌 설정과 ProjectileMovement 상태 업데이트.

	void ApplyGrenadeState();

	bool TryPickupGrenade(AActor* OtherActor);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SXGASGrenade|Component")
	TObjectPtr<USphereComponent> PickupCollision;

	UPROPERTY(EditDefaultsOnly, Category = "SXGASGrenade|Component")
	TObjectPtr<UStaticMeshComponent> GrenadeMesh;

	UPROPERTY(EditDefaultsOnly, Category = "SXGASGrenade|Component")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "SXGASGrenade|Pickup")
	TSubclassOf<UGameplayEffect> PickupEffectClass;

	UPROPERTY(ReplicatedUsing = OnRep_GrenadeState)
	ESXGASGrenadeState GrenadeState;

private:
	bool bPickupProcessed;

};