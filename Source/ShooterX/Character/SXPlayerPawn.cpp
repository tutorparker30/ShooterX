// SXPlayerPawn.cpp


#include "Character/SXPlayerPawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

ASXPlayerPawn::ASXPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	float CharacterHalfHeight = 90.f;
	float CharacterRadius = 40.f;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
	CapsuleComponent->SetCapsuleHalfHeight(CharacterHalfHeight);
	CapsuleComponent->SetCapsuleRadius(CharacterRadius);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	SkeletalMeshComponent->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMeshAsset(TEXT("오브젝트 패스"));
	//if (true == SkeletalMeshAsset.Succeeded())
	//{
	//    SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshAsset.Object);
	//}

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	FloatingPawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovementComponent"));

}

void ASXPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("FrontRear"), this, &ThisClass::FrontRear);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &ThisClass::LeftRight);
}

void ASXPlayerPawn::FrontRear(float InAxisValue)
{
	AddMovementInput(GetActorForwardVector(), InAxisValue);
}

void ASXPlayerPawn::LeftRight(float InAxisValue)
{
	AddMovementInput(GetActorRightVector(), InAxisValue);
}
