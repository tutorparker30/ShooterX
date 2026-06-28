// SXHealthPack.cpp


#include "Gimmick/SXHealthPack.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Net/UnrealNetwork.h"
#include "ShooterX.h"
#include "Character/SXPlayerCharacter.h"
#include "Component/SXStatusComponent.h"

ASXHealthPack::ASXHealthPack()
	: HealAmount(100.f)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

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

	const static float ActorNetUpdateFrequency = 1.f;
	SetNetUpdateFrequency(ActorNetUpdateFrequency);
		// 1초에 1번씩 액터 레플리케이션 시도. 즉, 서버 성능이 아무리 좋아도 1초에 1번씩만 레플리케이션함.
	NetUpdatePeriod = 1 / GetNetUpdateFrequency();
		// 주기 = 1 / 주파수

	SetNetCullDistanceSquared(NetCullDistance * NetCullDistance);
}

void ASXHealthPack::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	RotationSpeed = 300.f;
	//RotatingMovementComponent->RotationRate = FRotator(0.f, RotationSpeed, 0.f);
}

void ASXHealthPack::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ElapsedTime += DeltaSeconds;
	const float ZOffset = FMath::Sin(ElapsedTime * MovementSpeed) * Amplitude;
	FVector NewLocation = StartLocation;
	NewLocation.Z += ZOffset;
	SetActorLocation(NewLocation);

	if (HasAuthority() == true)
	{
		AddActorLocalRotation(FRotator(0.f, RotationSpeed * DeltaSeconds, 0.f));
		ServerRotationYaw = RootComponent->GetComponentRotation().Yaw;
	}
	else
	{
		if (NetUpdatePeriod < KINDA_SMALL_NUMBER)
		{
			return;
		}

		AccDeltaSecondSinceReplicated += DeltaSeconds;
		const float LerpRatio = FMath::Clamp(AccDeltaSecondSinceReplicated / NetUpdatePeriod, 0.f, 1.f);

		const float NextServerRotationYaw = ServerRotationYaw + RotationSpeed * NetUpdatePeriod;

		const float EstimatedClientRotationYaw = FMath::Lerp(ServerRotationYaw, NextServerRotationYaw, LerpRatio);
		SetActorRotation(FRotator(0.f, EstimatedClientRotationYaw, 0.f));
	}

	DrawDebugSphere(GetWorld(), GetActorLocation(), NetCullDistance / 2.f, 16, FColor::Green, false, -1.f);
		// NetCullDistanceSquared를 시각화 하기 위한 디버그 드로잉
}

void ASXHealthPack::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	NiagaraComponent->OnSystemFinished.AddDynamic(this, &ThisClass::OnEffectFinish);

	NiagaraComponent->Activate(true);
	BodyStaticMeshComponent->SetHiddenInGame(true);
	SetActorEnableCollision(false);

	ASXCharacterBase* OverlappingPC = Cast<ASXCharacterBase>(OtherActor);
	if (IsValid(OverlappingPC) == true)
	{
		if (USXStatusComponent* StatusComponent = OverlappingPC->GetStatusComponent())
		{
			StatusComponent->TakeBuff(50.f);
		}
	}
}

void ASXHealthPack::OnEffectFinish(UNiagaraComponent* FinishedNiagaraComponent)
{
	Destroy();
}

void ASXHealthPack::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerRotationYaw);
}

bool ASXHealthPack::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	bool bIsNetRelevant = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);

	if (false == bIsNetRelevant)
	{
		SX_LOG_NET(LogSXNet, Log, TEXT("%s is not relevant for(%s, %s)"), *GetName(), *RealViewer->GetName(), *ViewTarget->GetName());
	}

	return bIsNetRelevant;
}

void ASXHealthPack::OnRep_ServerRotationYaw()
{
	SX_LOG_NET(LogSXNet, Log, TEXT("OnRep_ServerRotationYaw(): %f"), ServerRotationYaw);

	SetActorRotation(FRotator(0.f, ServerRotationYaw, 0.f));
		// 지금은 큰 차이 없긴함. Tick() 함수에서 매 틱 마다 ServerRotationYaw 값이 수정되기 때문.
		// 하지만 추후에 매 틱마다 수정하지 않거나, 레플리케이션 주기를 낮춘다면 상당한 효과를 거둘 수 있음.

	AccDeltaSecondSinceReplicated = 0.f;
}
