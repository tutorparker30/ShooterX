// UW_LobbyLayout.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_LobbyLayout.generated.h"

class ASXLobbyPlayerCharacter;
class UButton;
class UEditableText;
struct FStreamableHandle;
class UMaterialInstance;

/**
 *
 */
UCLASS()
class SHOOTERX_API UUW_LobbyLayout : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnPreviousButtonClicked();

	UFUNCTION()
	void OnNextButtonClicked();

	UFUNCTION()
	void OnPlayButtonClicked();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LobbyLayout)
	TObjectPtr<ASXLobbyPlayerCharacter> OwningPlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LobbyLayout, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> PreviousButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LobbyLayout, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> NextButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LobbyLayout, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UEditableText> EditPlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LobbyLayout, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> PlayButton;

};
