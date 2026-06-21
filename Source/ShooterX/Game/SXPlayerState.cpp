// SXPlayerState.cpp


#include "Game/SXPlayerState.h"

#include "Character/SXPlayerCharacter.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

ASXPlayerState::ASXPlayerState()
{
}

void ASXPlayerState::AddCurrentKillCount(int32 InCurrentKillCount)
{
	CurrentKillCount = FMath::Clamp(CurrentKillCount + InCurrentKillCount, 0, MaxKillCount);

	OnCurrentKillCountChanged.Broadcast(CurrentKillCount);

	ASXPlayerCharacter* PlayerCharacter = Cast<ASXPlayerCharacter>(GetPawn());
	checkf(IsValid(PlayerCharacter) == true, TEXT("Invalid PlayerCharacter"));
	PlayerCharacter->GetNiagaraComponent()->Activate(true);
}

void ASXPlayerState::BeginPlay()
{
	Super::BeginPlay();

	const FString SavedDirectoryPath = FPaths::Combine(FPlatformMisc::ProjectDir(), TEXT("Saved"));
	const FString SavedFileName(TEXT("PlayerInfo.txt"));
	FString AbsoluteFilePath = FPaths::Combine(*SavedDirectoryPath, *SavedFileName);
	FPaths::MakeStandardFilename(AbsoluteFilePath);

	FString PlayerInfoJsonString;
	FFileHelper::LoadFileToString(PlayerInfoJsonString, *AbsoluteFilePath);
	TSharedRef<TJsonReader<TCHAR>> JsonReaderArchive = TJsonReaderFactory<TCHAR>::Create(PlayerInfoJsonString);

	TSharedPtr<FJsonObject> PlayerInfoJsonObject = nullptr;
	if (FJsonSerializer::Deserialize(JsonReaderArchive, PlayerInfoJsonObject) == true)
	{
		FString PlayerNameString = PlayerInfoJsonObject->GetStringField(TEXT("playername"));
		SetPlayerName(PlayerNameString);

		uint8 PlayerColorNumber = PlayerInfoJsonObject->GetIntegerField(TEXT("color"));
		PlayerColor = static_cast<EPlayerColor>(PlayerColorNumber);
		
		TryInitializeCharacterMeshMaterial();
	}
}

void ASXPlayerState::TryInitializeCharacterMeshMaterial()
{
	static int32 MaxRetryCount = 30;
	static int32 CurrentRetryCount = 0;
	if (MaxRetryCount < ++CurrentRetryCount)
	{
		return;
	}

	ASXPlayerCharacter* PlayerCharacter = Cast<ASXPlayerCharacter>(GetPawn());
	if (IsValid(PlayerCharacter) == false)
	{
		GetWorldTimerManager().SetTimerForNextTick(
			this,
			&ThisClass::TryInitializeCharacterMeshMaterial
		);

		return;
	}

	PlayerCharacter->InitializeCharacterMeshMaterial(PlayerColor);
}
