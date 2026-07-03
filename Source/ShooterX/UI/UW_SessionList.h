// UW_SessionList.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SessionList.generated.h"

class UListView;
class UButton;

/**
 *
 */
UCLASS()
class SHOOTERX_API UUW_SessionList : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void RefreshUI();

private:
	UFUNCTION()
	void OnRefreshButtonClicked();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UListView> SessionListView;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UButton> RefreshButton;

private:
	// ListView::SetListItems()에 넘긴 UObject 데이터가 GC되지 않도록 여기서 소유권을 유지한다.
	// (엔트리 위젯은 SessionListView가 알아서 생성/재활용하므로 별도로 들고 있을 필요가 없다)
	UPROPERTY()
	TArray<TObjectPtr<UObject>> SessionListItems;

	FDelegateHandle SessionSearchCompleteHandle;

};