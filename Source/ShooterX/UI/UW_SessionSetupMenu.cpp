// UW_SessionSetupMenu.cpp

#include "UI/UW_SessionSetupMenu.h"

#include "Components/EditableText.h"
#include "Components/ComboBoxString.h"
#include "Components/Button.h"
#include "Game/SXOnlineSessionSubsystem.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"

void UUW_SessionSetupMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(MapComboBox) == true)
	{
		MapComboBox->ClearOptions();
		
		MapComboBox->AddOption(TEXT("L_Convolution_Blockout"));
		MapComboBox->AddOption(TEXT("L_Expanse"));
			// Package.bat 파일에서 패키징 되는 레벨 중 Title과 Lobby를 제외한 레벨을 선택해야함.

		MapComboBox->SetSelectedOption(TEXT("Lobby"));
	}

	if (IsValid(CreateSessionButton) &&
		CreateSessionButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnCreateSessionButtonClicked) == false)
	{
		CreateSessionButton->OnClicked.AddDynamic(this, &ThisClass::OnCreateSessionButtonClicked);
	}
}

void UUW_SessionSetupMenu::OnCreateSessionButtonClicked()
{
    UWorld* World = GetWorld();
    if (IsValid(World) == false || World->GetNetMode() != NM_Standalone)
    {
        return;
    }

    CreateSessionButton->SetIsEnabled(false);

    const FString SessionName = SessionNameEditableText->GetText().ToString();
    const FString MapName = MapComboBox->GetSelectedOption();

    const FString SavedDirectoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
    const FString SavedFileName(TEXT("SessionSetup.json"));
    FString AbsoluteFilePath = FPaths::Combine(*SavedDirectoryPath, *SavedFileName);
    FPaths::MakeStandardFilename(AbsoluteFilePath);

    TSharedRef<FJsonObject> SessionSetupJsonObject = MakeShared<FJsonObject>();
    SessionSetupJsonObject->SetStringField(TEXT("sessionname"), SessionName);
    SessionSetupJsonObject->SetStringField(TEXT("mapname"), MapName);

    FString SessionSetupJsonString;
    TSharedRef<TJsonWriter<TCHAR>> JsonWriterArchive = TJsonWriterFactory<TCHAR>::Create(&SessionSetupJsonString);
    if (FJsonSerializer::Serialize(SessionSetupJsonObject, JsonWriterArchive) == true)
    {
        FFileHelper::SaveStringToFile(SessionSetupJsonString, *AbsoluteFilePath);
    }

    const FName LevelName(TEXT("Lobby"));
    const FString Options(TEXT("listen"));
    UGameplayStatics::OpenLevel(World, LevelName, true, Options);
}
