// UW_SessionList.cpp

#include "UI/UW_SessionList.h"

#include "Components/ListView.h"
#include "Components/Button.h"
#include "Game/SXOnlineSessionSubsystem.h"
#include "UI/UW_SessionListEntry.h"

void UUW_SessionList::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(RefreshButton) &&
		RefreshButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnRefreshButtonClicked) == false)
	{
		RefreshButton->OnClicked.AddDynamic(this, &ThisClass::OnRefreshButtonClicked);
	}

	USXOnlineSessionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USXOnlineSessionSubsystem>();
	if (IsValid(Subsystem) == true)
	{
		SessionSearchCompleteHandle =
			Subsystem->OnSessionSearchComplete.AddUObject(this, &ThisClass::RefreshUI);
	}

	OnRefreshButtonClicked();
}

void UUW_SessionList::OnRefreshButtonClicked()
{
	RefreshButton->SetIsEnabled(false);

	USXOnlineSessionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USXOnlineSessionSubsystem>();
	if (IsValid(Subsystem) == true)
	{
		Subsystem->FindSessions();
		// FindSessions 완료 후 RefreshUI() 호출은 서브시스템 콜백에서 처리
	}
}

void UUW_SessionList::RefreshUI()
{
	SessionListView->ClearListItems();
	SessionListItems.Empty();

	USXOnlineSessionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USXOnlineSessionSubsystem>();
	if (IsValid(Subsystem) == false || IsValid(SessionListView) == false)
	{
		return;
	}

	for (const FOnlineSessionSearchResult& SessionInfo : Subsystem->SessionResults)
	{
		// ListView는 UObject* 배열만 받으므로, 구조체 데이터를 UObject 래퍼에 담아 전달한다.
		USXSessionSearchResultObject* ResultObject = NewObject<USXSessionSearchResultObject>(this);
		ResultObject->SearchResult = SessionInfo;
		SessionListItems.Add(ResultObject);
	}

	// SetListItems가 내부적으로 엔트리 위젯(UUW_SessionListEntry)을 자동 생성/재활용하고
	// NativeOnListItemObjectSet()을 통해 각 데이터를 전달한다.
	SessionListView->SetListItems(SessionListItems);

	RefreshButton->SetIsEnabled(true); // 통신 완료 후 버튼 재활성화
}