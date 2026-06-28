// SXCharacterBase.cpp


#include "Character/SXCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/SXAnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ShooterX.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Component/SXStatusComponent.h"
#include "Item/SXWeapon.h"
#include "Game/SXGameModeBase.h"
#include "Game/SXGameStateBase.h"
#include "Kismet/GameplayStatics.h"

int32 ASXCharacterBase::ShowAttackMeleeDebug = 0;

FAutoConsoleVariableRef CVarShowAttackMeleeDebug(
	TEXT("SX.ShowAttackMeleeDebug"),
	ASXCharacterBase::ShowAttackMeleeDebug,
	TEXT(""),
	ECVF_Cheat
);

int32 ASXCharacterBase::ShowAttackRangedDebug = 0;

FAutoConsoleVariableRef CVarShowAttackRangedDebug(
	TEXT("SX.ShowAttackRangedDebug"),
	ASXCharacterBase::ShowAttackRangedDebug,
	TEXT(""),
	ECVF_Cheat
);

ASXCharacterBase::ASXCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	float CharacterHalfHeight = 90.f;
	float CharacterRadius = 40.f;

	GetCapsuleComponent()->InitCapsuleSize(CharacterRadius, CharacterHalfHeight);

	FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);
	GetMesh()->SetCollisionProfileName(TEXT("SXCharacterMesh"));

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;

	StatusComponent = CreateDefaultSubobject<USXStatusComponent>(TEXT("StatusComponent"));
}

void ASXCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	USXAnimInstance* AnimInstance = Cast<USXAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance) == true)
	{
		AnimInstance->OnPostDead.AddDynamic(this, &ThisClass::HandleOnPostCharacterDead);
	}
}

void ASXCharacterBase::HandleOnCheckHit()
{
	UKismetSystemLibrary::PrintString(this, TEXT("HandleOnCheckHit()"));

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepMultiByChannel(
		HitResults,
		GetActorLocation(),
		GetActorLocation() + AttackMeleeRange * GetActorForwardVector(),
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackMeleeRadius),
		Params
	);

	if (true == bResult)
	{
		if (HitResults.IsEmpty() == false)
		{
			for (FHitResult HitResult : HitResults)
			{
				if (IsValid(HitResult.GetActor()) == true)
				{
					FDamageEvent DamageEvent;
					HitResult.GetActor()->TakeDamage(10.f, DamageEvent, GetController(), this);

					if (1 == ShowAttackMeleeDebug)
					{
						UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName()));
					}
				}
			}
		}
	}

	if (1 == ShowAttackMeleeDebug)
	{
		FVector TraceVector = AttackMeleeRange * GetActorForwardVector();
		FVector Center = GetActorLocation() + TraceVector + GetActorUpVector() * 40.f;
		float HalfHeight = AttackMeleeRange * 0.5f + AttackMeleeRadius;
		FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();
		FColor DrawColor = true == bResult ? FColor::Green : FColor::Red;
		float DebugLifeTime = 5.f;

		DrawDebugCapsule(
			GetWorld(),
			Center,
			HalfHeight,
			AttackMeleeRadius,
			CapsuleRot,
			DrawColor,
			false,
			DebugLifeTime
		);
	}
}

void ASXCharacterBase::HandleOnCheckInputAttack()
{
	//UKismetSystemLibrary::PrintString(this, TEXT("HandleOnCheckInputAttack()"));

	USXAnimInstance* AnimInstance = Cast<USXAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

	if (bIsAttackKeyPressed == true)
	{
		CurrentComboCount = FMath::Clamp(CurrentComboCount + 1, 1, MaxComboCount);

		FName NextSectionName = *FString::Printf(TEXT("%s%02d"), *AttackAnimMontageSectionPrefix, CurrentComboCount);
		AnimInstance->Montage_JumpToSection(NextSectionName, AttackMeleeMontage);
		bIsAttackKeyPressed = false;
	}
}

void ASXCharacterBase::BeginAttack()
{
	USXAnimInstance* AnimInstance = Cast<USXAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

	bIsNowAttacking = true;
	if (IsValid(AnimInstance) == true && IsValid(AttackMeleeMontage) == true && AnimInstance->Montage_IsPlaying(AttackMeleeMontage) == false)
	{
		AnimInstance->Montage_Play(AttackMeleeMontage);
	}

	CurrentComboCount = 1;

	if (OnMeleeAttackMontageEndedDelegate.IsBound() == false)
	{
		OnMeleeAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
		AnimInstance->Montage_SetEndDelegate(OnMeleeAttackMontageEndedDelegate, AttackMeleeMontage);
	}
}

void ASXCharacterBase::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	ensureMsgf(CurrentComboCount != 0, TEXT("CurrentComboCount == 0"));

	CurrentComboCount = 0;
	bIsAttackKeyPressed = false;
	bIsNowAttacking = false;

	if (OnMeleeAttackMontageEndedDelegate.IsBound() == true)
	{
		OnMeleeAttackMontageEndedDelegate.Unbind();
	}
}

float ASXCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//StatusComponent->ApplyDamage(FinalDamageAmount);
	ASXGameStateBase* SXGameState = Cast<ASXGameStateBase>(UGameplayStatics::GetGameState(this));
	if (IsValid(SXGameState) == true && SXGameState->MatchState == EMatchState::Playing)
	{
		StatusComponent->ApplyDamage(FinalDamageAmount);
	}

	if (StatusComponent->IsDead() == true)
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}

	if (1 == ShowAttackMeleeDebug)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s [%.1f / %.1f]"), *GetName(), StatusComponent->GetCurrentHP(), StatusComponent->GetMaxHP()));
	}

	return FinalDamageAmount;
}

UAnimMontage* ASXCharacterBase::GetCurrentWeaponAttackAnimMontage() const
{
	if (IsValid(CurrentWeapon) == true)
	{
		return CurrentWeapon->GetAttackMontage();
	}
	return nullptr;
}

void ASXCharacterBase::HandleOnPostCharacterDead()
{
	SetLifeSpan(0.1f);
}

