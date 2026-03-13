#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SXBox.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UParticleSystemComponent;

UCLASS()
class SHOOTERX_API ASXBox : public AActor
{
	GENERATED_BODY()
	
public:	
	ASXBox();

private:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepHitResult);
	UFUNCTION()
	void OnEffectFinish(UParticleSystemComponent* ParticleSystem);

private:
	UPROPERTY(VisibleAnywhere, Category = "ASXItemBox")
	TObjectPtr<UBoxComponent> BoxComponent;
	UPROPERTY(EditAnywhere, Category = "ASXItemBox")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(EditAnywhere, Category = "ASXItemBox")
	TObjectPtr<UParticleSystemComponent> ParticleSystemComponent;

};
