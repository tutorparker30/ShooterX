// UW_LobbyLayout.cpp


#include "UI/UW_LobbyLayout.h"
#include "ShooterXPlayGround/SXCharacterMaterialManager.h"
#include "Character/SXLobbyPlayerCharacter.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Kismet/GameplayStatics.h"

void UUW_LobbyLayout::NativeConstruct()
{
	Super::NativeConstruct();

	OwningPlayerCharacter = Cast<ASXLobbyPlayerCharacter>(GetOwningPlayerPawn());

	if (PreviousButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnPreviousButtonClicked) == false)
	{
		PreviousButton->OnClicked.AddDynamic(this, &ThisClass::OnPreviousButtonClicked);
	}

	if (NextButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnNextButtonClicked) == false)
	{
		NextButton->OnClicked.AddDynamic(this, &ThisClass::OnNextButtonClicked);
	}

	if (PlayButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnPlayButtonClicked) == false)
	{
		PlayButton->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
	}
}

void UUW_LobbyLayout::NativeDestruct()
{
	if (PreviousButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnPreviousButtonClicked) == true)
	{
		PreviousButton->OnClicked.RemoveDynamic(this, &ThisClass::OnPreviousButtonClicked);
	}

	if (NextButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnNextButtonClicked) == true)
	{
		NextButton->OnClicked.RemoveDynamic(this, &ThisClass::OnNextButtonClicked);
	}

	if (PlayButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnPlayButtonClicked) == true)
	{
		PlayButton->OnClicked.RemoveDynamic(this, &ThisClass::OnPlayButtonClicked);
	}

	Super::NativeDestruct();
}

void UUW_LobbyLayout::OnPreviousButtonClicked()
{
	OwningPlayerCharacter->SetLocalCharacterSkeletalMesh(-1);
}

void UUW_LobbyLayout::OnNextButtonClicked()
{
	OwningPlayerCharacter->SetLocalCharacterSkeletalMesh(1);
}

void UUW_LobbyLayout::OnPlayButtonClicked()
{
	FString PlayerName = EditPlayerName->GetText().ToString();

	// UGameplayStatics::OpenLevel(GetWorld(), TEXT("Loading"), true, FString(TEXT("NextLevel=L_Expanse")));

	const FString SavedDirectoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
	const FString SavedFileName(TEXT("PlayerInfo.txt"));
	FString AbsoluteFilePath = FPaths::Combine(*SavedDirectoryPath, *SavedFileName);
	FPaths::MakeStandardFilename(AbsoluteFilePath);

	TSharedRef<FJsonObject> PlayerInfoJsonObject = MakeShared<FJsonObject>();
	PlayerInfoJsonObject->SetStringField("playername", PlayerName);
	PlayerInfoJsonObject->SetNumberField("color", OwningPlayerCharacter->SelectedMeshMaterialIndex);

	FString PlayerInfoJsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriterArchive = TJsonWriterFactory<TCHAR>::Create(&PlayerInfoJsonString);
	if (FJsonSerializer::Serialize(PlayerInfoJsonObject, JsonWriterArchive) == true)
	{
		FFileHelper::SaveStringToFile(PlayerInfoJsonString, *AbsoluteFilePath);
	}

	UGameplayStatics::OpenLevel(GetWorld(), TEXT("Loading"), true, FString(TEXT("NextLevel=L_Expanse")));
}
