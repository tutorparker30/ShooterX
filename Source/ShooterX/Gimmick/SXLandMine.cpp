// SXLandMine.cpp

#include "Gimmick/SXLandMine.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "ShooterX.h"
#include "Net/UnrealNetwork.h"

ASXLandMine::ASXLandMine()
	: bIsExploded(false)
	, NetCullDistance(1000.f)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->SetRelativeScale3D(FVector(0.15f, 0.15f, 0.35f));
	BoxComponent->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
	BoxComponent->SetCollisionProfileName(FName(TEXT("SXGimmick")));

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	StaticMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(GetRootComponent());
	NiagaraComponent->SetAutoActivate(false);

	SetNetCullDistanceSquared(NetCullDistance * NetCullDistance);
}

void ASXLandMine::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == true)
	{
		ShooterXFunctionLibrary::MyPrintString(this, FString::Printf(TEXT("Run on server.")), 5.f, FColor::Green);
	}
	else
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (IsValid(OwnerPawn) == true)
		{
			if (OwnerPawn->IsLocallyControlled() == true)
			{
				ShooterXFunctionLibrary::MyPrintString(this, FString::Printf(TEXT("Run on owning client.")), 5.f, FColor::Green);
			}
			else
			{
				ShooterXFunctionLibrary::MyPrintString(this, FString::Printf(TEXT("Run on other client.")), 5.f, FColor::Green);
			}
		}
	}
}

void ASXLandMine::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsExploded);
}

void ASXLandMine::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	NiagaraComponent->OnSystemFinished.AddDynamic(this, &ThisClass::OnEffectFinish);

	SetActorEnableCollision(false);

	if (bIsExploded == false && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		NiagaraComponent->Activate(true);
	}

	if (HasAuthority() == true)
	{
		ShooterXFunctionLibrary::MyPrintString(this, FString::Printf(TEXT("Run on server.")), 5.f, FColor::Green);

		MulticastRPCSpawnEffect();

		if (bIsExploded == false)
		{
			bIsExploded = true;
		}

		OnRep_IsExploded();
	}
	else
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if (IsValid(OwnerPawn) == true)
		{
			if (OwnerPawn->IsLocallyControlled() == true)
			{
				ShooterXFunctionLibrary::MyPrintString(this, FString::Printf(TEXT("Run on owning client.")), 5.f, FColor::Green);
			}
			else
			{
				ShooterXFunctionLibrary::MyPrintString(this, FString::Printf(TEXT("Run on other client.")), 5.f, FColor::Green);
			}
		}
	}
}

void ASXLandMine::OnEffectFinish(UNiagaraComponent* FinishedNiagaraComponent)
{
	//Destroy();
	if (HasAuthority() == true)
	{
		Destroy();
	}
}

void ASXLandMine::MulticastRPCSpawnEffect_Implementation()
{
	/*
	if (IsValid(ExplodedMaterial) == true)
	{
		StaticMeshComponent->SetMaterial(0, ExplodedMaterial);
	}
	*/
}

void ASXLandMine::OnRep_IsExploded()
{
	if (true == bIsExploded && IsValid(ExplodedMaterial) == true)
	{
		StaticMeshComponent->SetMaterial(0, ExplodedMaterial);
	}
}
