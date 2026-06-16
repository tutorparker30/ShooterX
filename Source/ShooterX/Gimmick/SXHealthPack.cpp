// SXHealthPack.cpp


#include "Gimmick/SXHealthPack.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

ASXHealthPack::ASXHealthPack()
	: HealAmount(100.f)
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	BoxComponent->SetRelativeScale3D(FVector(0.6f, 0.8f, 0.8f));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	BoxComponent->SetCollisionProfileName(FName(TEXT("SXGimmick")));

	BodyStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMeshComponent"));
	BodyStaticMeshComponent->SetupAttachment(GetRootComponent());
	BodyStaticMeshComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	BodyStaticMeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyStaticMesh(TEXT("/Script/Engine.StaticMesh'/Game/LyraResource/Weapons/Healthpack/Mesh/SM_healthpackFull.SM_healthpackFull'"));
	if (true == BodyStaticMesh.Succeeded())
	{
		BodyStaticMeshComponent->SetStaticMesh(BodyStaticMesh.Object);
	}

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(GetRootComponent());
	NiagaraComponent->SetAutoActivate(false);
}

void ASXHealthPack::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	RotationSpeed = 300.f;
	RotatingMovementComponent->RotationRate = FRotator(0.f, RotationSpeed, 0.f);
}

void ASXHealthPack::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ElapsedTime += DeltaSeconds;
	const float ZOffset = FMath::Sin(ElapsedTime * MovementSpeed) * Amplitude;
	FVector NewLocation = StartLocation;
	NewLocation.Z += ZOffset;
	SetActorLocation(NewLocation);
}

void ASXHealthPack::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	NiagaraComponent->OnSystemFinished.AddDynamic(this, &ThisClass::OnEffectFinish);

	NiagaraComponent->Activate(true);
	BodyStaticMeshComponent->SetHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ASXHealthPack::OnEffectFinish(UNiagaraComponent* FinishedNiagaraComponent)
{
	Destroy();
}
