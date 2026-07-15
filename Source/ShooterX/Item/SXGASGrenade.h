// SXGASGrenade.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SXGASGrenade.generated.h"

class UGameplayEffect;
class UProjectileMovementComponent;
class USphereComponent;
class UStaticMeshComponent;
class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class ESXGASGrenadeState : uint8
{
	Pickup,
	Thrown,
};

UCLASS()
class SHOOTERX_API ASXGASGrenade : public AActor
{
	GENERATED_BODY()

public:
	ASXGASGrenade();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ESXGASGrenadeState GetGrenadeState() const
	{
		return GrenadeState;
	}

	void SetGrenadeState(ESXGASGrenadeState NewGrenadeState);

	float GetGrenadeCollisionRadius() const;

	float GetGrenadeInitialSpeed() const;

	float GetGrenadeGravityScale() const;

	float GetGrenadeMaxSpeed() const;

	bool InitializeThrownGrenade(
		UAbilitySystemComponent*
		InSourceAbilitySystemComponent,
		AActor* InSourceActor,
		const FVector& InLaunchVelocity,
		float InExplosionRadius,
		float InExplosionDamage);

protected:
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UFUNCTION()
	void OnRep_GrenadeState();

	void ApplyGrenadeState();

	bool TryPickupGrenade(AActor* OtherActor);
	
	void StartThrownGrenade();

	void Explode();

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

	UPROPERTY(Transient)
	TWeakObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> SourceActor;

	FVector LaunchVelocity;

	float ExplosionRadius;

	float ExplosionDamage;

	bool bThrownGrenadeInitialized;
	
	bool bExploded;
	
	float FuseTime;
	
	FTimerHandle ExplosionTimerHandle;

};