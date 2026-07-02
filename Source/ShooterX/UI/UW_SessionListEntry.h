// UW_SessionListEntry.h

#pragma once
#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UW_SessionListEntry.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class SHOOTERX_API USXSessionSearchResultObject : public UObject
{
	GENERATED_BODY()

public:
	FOnlineSessionSearchResult SearchResult;

};

/**
 *
 */
UCLASS()
class SHOOTERX_API UUW_SessionListEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	void RefreshUI();

	UFUNCTION()
	void OnJoinButtonClicked();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UTextBlock> SessionNameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerCountText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UTextBlock> MapNameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;

private:
	FOnlineSessionSearchResult SessionResult;

};