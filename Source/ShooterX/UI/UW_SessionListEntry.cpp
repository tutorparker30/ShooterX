// UW_SessionListEntry.cpp

#include "UI/UW_SessionListEntry.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

void UUW_SessionListEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(JoinButton) &&
		JoinButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnJoinButtonClicked) == false)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::OnJoinButtonClicked);
	}
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
}

void UUW_SessionListEntry::OnJoinButtonClicked()
{
}