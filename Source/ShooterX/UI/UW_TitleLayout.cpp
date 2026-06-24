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
	PlayButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	ExitButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
	HostButton.Get()->OnClicked.AddDynamic(this, &ThisClass::OnHostButtonClicked);
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
	UWorld* World = GetWorld();
	if (IsValid(World) == false || World->GetNetMode() != NM_Standalone)
	{
		return;
	}

	const FName LevelName(TEXT("Lobby"));
	const FString Options(TEXT("listen"));

	UGameplayStatics::OpenLevel(World, LevelName, true, Options);
}
