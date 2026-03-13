#include "Gimmick/SXBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"

ASXBox::ASXBox()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetBoxExtent(FVector(40.f, 42.f, 30.f));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	BoxComponent->SetCollisionProfileName(FName("SXBox"));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetRelativeLocation(FVector(0.f, -3.5f, -30.f));
	StaticMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(GetRootComponent());
	ParticleSystemComponent->SetAutoActivate(false);

}

void ASXBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	ParticleSystemComponent->Activate(true);
	StaticMeshComponent->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	ParticleSystemComponent->OnSystemFinished.AddDynamic(this, &ThisClass::OnEffectFinish);
}

void ASXBox::OnEffectFinish(UParticleSystemComponent* ParticleSystem)
{
	Destroy();
}

