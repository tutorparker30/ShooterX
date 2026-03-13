#include "Character/SXNonPlayerCharacter.h"
#include "Controller/SXAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/SXAnimInstance.h"
#include "Component/SXStatusComponent.h"
#include "UI/UW_HPText.h"
#include "Component/SXHPTextWidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/SXPlayerCharacter.h"
#include "Game/SXPlayerState.h"

ASXNonPlayerCharacter::ASXNonPlayerCharacter()
	: bIsNowAttacking(false)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;

	AIControllerClass = ASXAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HPTextWidgetComponent = CreateDefaultSubobject<USXHPTextWidgetComponent>(TEXT("WidgetComponent"));
	HPTextWidgetComponent->SetupAttachment(GetRootComponent());
	HPTextWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

	HPTextWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	HPTextWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void ASXNonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (false == IsPlayerControlled())
	{
		bUseControllerRotationYaw = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);

		GetCharacterMovement()->MaxWalkSpeed = 300.f;

	}

}

void ASXNonPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsValid(HPTextWidgetComponent) == true)
	{
		FVector WidgetComponentLocation = HPTextWidgetComponent->GetComponentLocation();
		FVector LocalPlayerCameraLocation = UGameplayStatics::GetPlayerCameraManager(this, 0)->GetCameraLocation();
		HPTextWidgetComponent->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(WidgetComponentLocation, LocalPlayerCameraLocation));

	}

}

void ASXNonPlayerCharacter::SetHPTextWidget(UUW_HPText* InHPTextWidget)
{
	if (IsValid(InHPTextWidget) == true)
	{
		InHPTextWidget->InitializeHPTextWidget(StatusComponent);
		StatusComponent->OnCurrentHPChanged.AddUObject(InHPTextWidget, &UUW_HPText::OnCurrentHPChange);
		StatusComponent->OnMaxHPChanged.AddUObject(InHPTextWidget, &UUW_HPText::OnMaxHPChange);
	}
}

void ASXNonPlayerCharacter::BeginAttack()
{
	USXAnimInstance* AnimInstance = Cast<USXAnimInstance>(GetMesh()->GetAnimInstance());
	checkf(IsValid(AnimInstance) == true, TEXT("Invalid AnimInstance"));

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	if (IsValid(AnimInstance) && IsValid(AttackMeleeMontage) && AnimInstance->Montage_IsPlaying(AttackMeleeMontage) == false)
	{
		AnimInstance->Montage_Play(AttackMeleeMontage);

		bIsNowAttacking = true;

		if (OnAttackMontageEndedDelegate.IsBound() == false)
		{
			OnAttackMontageEndedDelegate.BindUObject(this, &ThisClass::EndAttack);
			AnimInstance->Montage_SetEndDelegate(OnAttackMontageEndedDelegate, AttackMeleeMontage);
		}

	}
}

void ASXNonPlayerCharacter::EndAttack(UAnimMontage* InMontage, bool bInterruped)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	bIsNowAttacking = false;

	if (OnAttackMontageEndedDelegate.IsBound() == true)
	{
		OnAttackMontageEndedDelegate.Unbind();
	}

}

float ASXNonPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//if (CurrentHP < KINDA_SMALL_NUMBER)
	if (StatusComponent->IsDead() == true)
	{
		ASXAIController* AIController = Cast<ASXAIController>(GetController());
		if (IsValid(AIController))
		{
			AIController->EndAI();
		}

		ASXPlayerCharacter* DamageCauserCharacter = Cast<ASXPlayerCharacter>(DamageCauser);
		if (IsValid(DamageCauserCharacter))
		{
			ASXPlayerState* SXPlayerState = Cast<ASXPlayerState>(DamageCauserCharacter->GetPlayerState());
			if (IsValid(SXPlayerState))
			{
				SXPlayerState->AddCurrentKillCount(1);
			}
		}

	}

	return FinalDamageAmount;

}
