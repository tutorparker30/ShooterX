// UW_SessionList.cpp

#include "UI/UW_SessionList.h"

#include "Components/ListView.h"
#include "Components/Button.h"

void UUW_SessionList::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(RefreshButton) &&
		RefreshButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnRefreshButtonClicked) == false)
	{
		RefreshButton->OnClicked.AddDynamic(this, &ThisClass::OnRefreshButtonClicked);
	}

	OnRefreshButtonClicked();
}

void UUW_SessionList::OnRefreshButtonClicked()
{
}

void UUW_SessionList::RefreshUI()
{
}