// SXAnimInstance.cpp


#include "Animation/SXAnimInstance.h"
#include "Character/SXCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/SXNonPlayerCharacter.h"
#include "Component/SXStatusComponent.h"
#include "Character/SXPlayerCharacter.h"

void USXAnimInstance::NativeInitializeAnimation()
{
	APawn* OwnerPawn = TryGetPawnOwner();
	if (IsValid(OwnerPawn) == true)
	{
		OwnerCharacter = Cast<ASXCharacterBase>(OwnerPawn);
		OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
	}

	bIsUnarmed = true;
}

void USXAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (IsValid(OwnerCharacter) == true && IsValid(OwnerCharacterMovement) == true)
	{
		Velocity = OwnerCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);

		float GroundAcceleration = UKismetMathLibrary::VSizeXY(OwnerCharacterMovement->GetCurrentAcceleration());
		bool bIsAccelerationNearlyZero = FMath::IsNearlyZero(GroundAcceleration);
		bShouldMove = (KINDA_SMALL_NUMBER < GroundSpeed) && (bIsAccelerationNearlyZero == false);
		bIsFalling = OwnerCharacterMovement->IsFalling();
		if (IsValid(OwnerCharacter->GetStatusComponent()) == true)
		{
			bIsDead = OwnerCharacter->GetStatusComponent()->IsDead();
		}

		if (ASXNonPlayerCharacter* OwnerNPC = Cast<ASXNonPlayerCharacter>(OwnerCharacter))
		{
			bShouldMove = KINDA_SMALL_NUMBER < GroundSpeed;
		}

		bIsUnarmed = OwnerCharacter->GetCurrentWeaponAttackAnimMontage() == nullptr ? true : false;
		
		if (ASXPlayerCharacter* OwnerPC = Cast<ASXPlayerCharacter>(OwnerCharacter))
		{
			NormalizedCurrentPitch = OwnerPC->GetCurrentAimPitch();
		}
	}
}

void USXAnimInstance::AnimNotify_CheckHit()
{
	if (OnCheckHit.IsBound() == true)
	{
		OnCheckHit.Broadcast();
	}
}

void USXAnimInstance::AnimNotify_PostDead()
{
	if (OnPostDead.IsBound() == true)
	{
		OnPostDead.Broadcast();
	}
}
