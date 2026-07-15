// SXGASGrenade.cpp

#include "Item/SXGASGrenade.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "GameplayAbilitySystem/AS/SX_AS_Grenade.h"

ASXGASGrenade::ASXGASGrenade()
	: GrenadeState(ESXGASGrenadeState::Pickup)
	, bPickupProcessed(false)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);
		// 서버에서 이동한 수류탄의 위치와 이동 상태를 클라에 전달하기 위함.

	PickupCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));

	SetRootComponent(PickupCollision);

	PickupCollision->InitSphereRadius(50.0f);
	PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupCollision->SetCollisionObjectType(ECC_WorldDynamic);
	PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupCollision->SetGenerateOverlapEvents(true);

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));

	GrenadeMesh->SetupAttachment(PickupCollision);
	GrenadeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileMovement->SetUpdatedComponent(PickupCollision);

	ProjectileMovement->InitialSpeed = 1200.0f;
	ProjectileMovement->MaxSpeed = 1200.0f;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.35f;
	ProjectileMovement->Friction = 0.2f;
	ProjectileMovement->bAutoActivate = false;
}

void ASXGASGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, GrenadeState);	
		// GrenadeState는 모든 클라이언트가 알아야 하므로 별도의 복제 조건을 지정하지 않는다.
}

void ASXGASGrenade::BeginPlay()
{
	Super::BeginPlay();

	ApplyGrenadeState();
}

void ASXGASGrenade::SetGrenadeState(ESXGASGrenadeState NewGrenadeState)
{
	if (HasAuthority() == false)
	{
		return;
	}
		// 클라가 임의로 수류탄 액터의 상태를 변경하더라도 서버의 수류탄 상태는 변경되지 않음.

	GrenadeState = NewGrenadeState;

	ApplyGrenadeState();
	ForceNetUpdate();
}

float ASXGASGrenade::GetGrenadeCollisionRadius() const
{
	if (IsValid(PickupCollision) == false)
	{
		return 0.0f;
	}

	return PickupCollision->GetUnscaledSphereRadius();
}

float ASXGASGrenade::GetGrenadeInitialSpeed() const
{
	if (IsValid(ProjectileMovement) == false)
	{
		return 0.0f;
	}

	return ProjectileMovement->InitialSpeed;
}

float ASXGASGrenade::GetGrenadeGravityScale() const
{
	if (IsValid(ProjectileMovement) == false)
	{
		return 0.0f;
	}

	return ProjectileMovement->ProjectileGravityScale;
}

float ASXGASGrenade::GetGrenadeMaxSpeed() const
{
	if (IsValid(ProjectileMovement) == false)
	{
		return 0.0f;
	}

	return ProjectileMovement->MaxSpeed;
}

bool ASXGASGrenade::InitializeThrownGrenade(UAbilitySystemComponent* InSourceAbilitySystemComponent, AActor* InSourceActor, const FVector& InLaunchVelocity, float InExplosionRadius, float InExplosionDamage)
{
	if (HasAuthority() == false ||
		IsValid(InSourceAbilitySystemComponent) == false ||
		IsValid(InSourceActor) == false ||
		InLaunchVelocity.ContainsNaN() ||
		InLaunchVelocity.IsNearlyZero() ||
		FMath::IsFinite(InExplosionRadius) == false ||
		FMath::IsFinite(InExplosionDamage) == false ||
		InExplosionRadius <= 0.0f ||
		InExplosionDamage < 0.0f)
	{
		return false;
	}

	GrenadeState = ESXGASGrenadeState::Thrown;

	SourceAbilitySystemComponent = InSourceAbilitySystemComponent;

	SourceActor = InSourceActor;

	LaunchVelocity = InLaunchVelocity;

	ExplosionRadius = InExplosionRadius;

	ExplosionDamage = InExplosionDamage;

	bThrownGrenadeInitialized = true;

	return true;
}

void ASXGASGrenade::OnRep_GrenadeState()
{
	ApplyGrenadeState();
}

void ASXGASGrenade::ApplyGrenadeState()
{
	if (IsValid(PickupCollision) == false || IsValid(ProjectileMovement) == false)
	{
		return;
	}

	switch (GrenadeState)
	{
	case ESXGASGrenadeState::Pickup:
		PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		PickupCollision->SetCollisionObjectType(ECC_WorldDynamic);
		PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		PickupCollision->SetGenerateOverlapEvents(true);

		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();

		break;

	case ESXGASGrenadeState::Thrown:
		PickupCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		PickupCollision->SetCollisionObjectType(ECC_WorldDynamic);
		PickupCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		PickupCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		PickupCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		PickupCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		PickupCollision->SetGenerateOverlapEvents(false);

		break;

	default:
		break;

	}
}

void ASXGASGrenade::NotifyActorBeginOverlap(
	AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (HasAuthority() == false)
	{
		return;
	}

	if (GrenadeState != ESXGASGrenadeState::Pickup)
	{
		return;
	}

	if (bPickupProcessed)
	{
		return;
	}

	TryPickupGrenade(OtherActor);
}

bool ASXGASGrenade::TryPickupGrenade(AActor* OtherActor)
{
	if (IsValid(OtherActor) == false || OtherActor == this)
	{
		return false;
	}

	if (IsValid(PickupEffectClass) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] PickupEffectClass is invalid."), *GetNameSafe(this));

		return false;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);

	if (IsValid(TargetASC) == false)
	{
		return false;
	}

	const USX_AS_Grenade* GrenadeAttributeSet = TargetASC->GetSet<USX_AS_Grenade>();

	if (IsValid(GrenadeAttributeSet) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Grenade AttributeSet is invalid."), *GetNameSafe(OtherActor));

		return false;
	}

	const float PreviousGrenadeCount = GrenadeAttributeSet->GetCurrentGrenadeCount();

	const float MaxGrenadeCount = GrenadeAttributeSet->GetMaxGrenadeCount();

	if (MaxGrenadeCount <= PreviousGrenadeCount)
	{
		return false;
	}
		// 이미 수류탄 최대 수량을 쥐고 있으면 오버랩해도 획득되지 않음.

	bPickupProcessed = true;

	PickupCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
		// SourceObject에 수류탄 액터 객체를 등록함으로써
		// 추후 GameplayEffect Execution이나 AttributeSet에서 
		// 어떤 아이템으로부터 효과가 발생했는지, 해당 아이템의 추가 데이터는 무엇인지 등을 알 수 있게 됨.

	const UGameplayEffect* PickupEffect = PickupEffectClass->GetDefaultObject<UGameplayEffect>();
	if (IsValid(PickupEffect) == false)
	{
		bPickupProcessed = false;
		ApplyGrenadeState();
		return false;
	}

	TargetASC->ApplyGameplayEffectToSelf(PickupEffect, 1.0f, EffectContext);

	const float NewGrenadeCount = GrenadeAttributeSet->GetCurrentGrenadeCount();
	if (NewGrenadeCount <= PreviousGrenadeCount)
	{
		bPickupProcessed = false;
		ApplyGrenadeState();
		return false;
	}
		// PickupEffectClass를 설정하지 않았거나,
		// GameplayEffect 적용 요구 조건에 실패했거나,
		// 모종의 이유로 수류탄 수량이 실제로 증가하지 않았을 때 수류탄이 사라지는 것을 방지.

	UE_LOG(LogTemp, Log, TEXT("[%s] picked up [%s]. " "GrenadeCount: %.0f -> %.0f"), *GetNameSafe(OtherActor), *GetNameSafe(this), PreviousGrenadeCount, NewGrenadeCount);

	Destroy();

	return true;
}
