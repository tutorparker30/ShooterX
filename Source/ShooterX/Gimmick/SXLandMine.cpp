// SXLandMine.cpp

#include "Gimmick/SXLandMine.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

ASXLandMine::ASXLandMine()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.35f));
	BoxComponent->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	//BoxComponent->SetCollisionProfileName(FName("Trigger"));	
	BoxComponent->SetCollisionProfileName(FName(TEXT("SXGimmick")));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	StaticMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(GetRootComponent());
	NiagaraComponent->SetAutoActivate(false);

	/*
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> EffectTemplate(TEXT("오브젝트 패스"));
	if (EffectTemplate.Object)
	{
		NiagaraComponent->SetAsset(EffectTemplate.Object);
		NiagaraComponent->SetAutoActivate(false);
	}
	*/
}

void ASXLandMine::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	NiagaraComponent->OnSystemFinished.AddDynamic(this, &ThisClass::OnEffectFinish);

	NiagaraComponent->Activate(true);
	StaticMeshComponent->SetHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ASXLandMine::OnEffectFinish(UNiagaraComponent* FinishedNiagaraComponent)
{
	Destroy();
}