// UW_TitleLayout.cpp

#include "UI/UW_TitleLayout.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/EditableText.h"

UUW_TitleLayout::UUW_TitleLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UUW_TitleLayout::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(PlayButton) &&
		PlayButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnPlayButtonClicked) == false)
	{
		PlayButton->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	}

	if (IsValid(ExitButton) &&
		ExitButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnExitButtonClicked) == false)
	{
		ExitButton->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
	}

	if (IsValid(HostButton) &&
		HostButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnHostButtonClicked) == false)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::OnHostButtonClicked);
	}
}

void UUW_TitleLayout::OnPlayButtonClicked()
{
	//UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString(TEXT("NextLevel=Lobby")));
	FText ServerIP = ServerIPEditableText->GetText();
	UGameplayStatics::OpenLevel(GetWorld(), FName(TEXT("Loading")), true, FString::Printf(TEXT("NextLevel=%s"), *ServerIP.ToString()));
}

void UUW_TitleLayout::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UUW_TitleLayout::OnHostButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OnHostButtonClicked called. this=%s"), *GetName());

	UWorld* World = GetWorld();
	if (IsValid(World) == false || World->GetNetMode() != NM_Standalone)
	{
		return;
	}

	HostButton->SetIsEnabled(false);

	const FName LevelName(TEXT("Lobby"));
	const FString Options(TEXT("listen"));

	UGameplayStatics::OpenLevel(World, LevelName, true, Options);
}
