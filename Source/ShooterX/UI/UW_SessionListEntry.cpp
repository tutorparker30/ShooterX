// UW_SessionListEntry.cpp

#include "UI/UW_SessionListEntry.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Game/SXOnlineSessionSubsystem.h"

void UUW_SessionListEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(JoinButton) &&
		JoinButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnJoinButtonClicked) == false)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);
	}

	USXOnlineSessionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USXOnlineSessionSubsystem>();
	if (IsValid(Subsystem) == true)
	{
		JoinSessionResultHandle =
			Subsystem->OnJoinSessionResult.AddUObject(this, &ThisClass::OnJoinSessionResult);
	}
}

void UUW_SessionListEntry::NativeDestruct()
{
	if (IsValid(JoinButton) &&
		JoinButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnJoinButtonClicked) == true)
	{
		JoinButton->OnClicked.RemoveDynamic(this, &ThisClass::OnJoinButtonClicked);
	}

	USXOnlineSessionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USXOnlineSessionSubsystem>();
	if (IsValid(Subsystem) == true)
	{
		Subsystem->OnJoinSessionResult.Remove(JoinSessionResultHandle);
	}

	Super::NativeDestruct();
}

void UUW_SessionListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	USXSessionSearchResultObject* ResultObject = Cast<USXSessionSearchResultObject>(ListItemObject);
	if (IsValid(ResultObject) == false)
	{
		return;
	}

	SessionResult = ResultObject->SearchResult;
	RefreshUI();
}

void UUW_SessionListEntry::RefreshUI()
{
	// 방 이름 표시 (Key-Value에서 읽기)
	FString SessionName;
	SessionResult.Session.SessionSettings.Get(FName(TEXT("SessionName")), SessionName);
	SessionNameText->SetText(FText::FromString(SessionName));

	// 현재/최대 인원 계산 후 표시
	int32 MaxPlayers = SessionResult.Session.SessionSettings.NumPublicConnections;
	int32 OpenSlots = SessionResult.Session.NumOpenPublicConnections;
	int32 CurrentPlayers = MaxPlayers - OpenSlots;

	FString PlayerCountString = FString::Printf(TEXT("%d / %d"), CurrentPlayers, MaxPlayers);
	PlayerCountText->SetText(FText::FromString(PlayerCountString));

	// 맵 이름 표시
	FString MapName;
	SessionResult.Session.SessionSettings.Get(FName(TEXT("MapName")), MapName);
	MapNameText->SetText(FText::FromString(MapName));
}

void UUW_SessionListEntry::OnJoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);

	USXOnlineSessionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USXOnlineSessionSubsystem>();
	if (IsValid(Subsystem) == true)
	{
		Subsystem->JoinSession(SessionResult);
	}
}

void UUW_SessionListEntry::OnJoinSessionResult(bool bWasSuccessful)
{
	if (bWasSuccessful == false && IsValid(JoinButton) == true)
	{
		JoinButton->SetIsEnabled(true);
	}
}