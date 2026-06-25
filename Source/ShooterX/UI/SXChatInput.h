// SXChatInput.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "SXChatInput.generated.h"

class UEditableText;

/**
 * 
 */
UCLASS()
class SHOOTERX_API USXChatInput : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> EditableTextChatInput;
	
};
