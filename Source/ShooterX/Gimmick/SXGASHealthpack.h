// SXGASHealthpack.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "SXGASHealthpack.generated.h"

class USceneComponent;
class UBoxComponent;
class UStaticMeshComponent;
class UAbilitySystemComponent;
class URotatingMovementComponent;
class UGameplayAbility;

UCLASS()
class SHOOTERX_API ASXGASHealthpack 
	: public AActor
	, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	ASXGASHealthpack();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;

private:
	void OnTimerElapsed();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SXGASHealthpack|Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SXGASHealthpack|Components")
	TObjectPtr<UBoxComponent> Body;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SXGASHealthpack|Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SXGASHealthpack|Components")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SXGASHealthpack|Components")
	TObjectPtr<URotatingMovementComponent> RotatingMovement;

	FTimerHandle ToggleTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SXGASHealthpack|GAS")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

};
