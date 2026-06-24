// SXPlayerPawn.cpp


#include "Character/SXPlayerPawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "ShooterX.h"

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
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassInfo(TEXT("/Script/Engine.AnimBlueprint'/Game/ShooterX/Animation/AnimationBlueprint/ABP_PlayerPawn.ABP_PlayerPawn_C'"));
	// 위 오브젝트 패스 대신, 작은 따옴표 안의 내용만으로도 가능함.
	//static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassInfo(TEXT("'/Game/ShooterX/Animation/AnimationBlueprint/ABP_PlayerPawn.ABP_PlayerPawn_C'")); 
	if (AnimInstanceClassInfo.Succeeded() == true)
	{
		SkeletalMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		SkeletalMeshComponent->SetAnimInstanceClass(AnimInstanceClassInfo.Class);
	}

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

void ASXPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = ShooterXFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("BeginPlay() %s [%s]"), *ShooterXFunctionLibrary::GetNetModeString(this), *NetRoleString);
	ShooterXFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ASXPlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = ShooterXFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("PossessedBy() %s [%s]"), *ShooterXFunctionLibrary::GetNetModeString(this), *NetRoleString);
	ShooterXFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ASXPlayerPawn::FrontRear(float InAxisValue)
{
	AddMovementInput(GetActorForwardVector(), InAxisValue);
}

void ASXPlayerPawn::LeftRight(float InAxisValue)
{
	AddMovementInput(GetActorRightVector(), InAxisValue);
}
