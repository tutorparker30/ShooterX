// UW_SessionSetupMenu.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_SessionSetupMenu.generated.h"

class UEditableText;
class UComboBoxString;
class UButton;

/**
 *
 */
UCLASS()
class SHOOTERX_API UUW_SessionSetupMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnCreateSessionButtonClicked();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UEditableText> SessionNameEditableText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UComboBoxString> MapComboBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	TObjectPtr<UButton> CreateSessionButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxPlayers = 4;

};