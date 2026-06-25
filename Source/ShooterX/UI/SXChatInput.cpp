// SXChatInput.cpp


#include "UI/SXChatInput.h"

#include "Components/EditableText.h"
#include "Controller/SXUIPC_Lobby.h"

void USXChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void USXChatInput::NativeDestruct()
{
	if (EditableTextChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}

	Super::NativeDestruct();
}

void USXChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController) == true)
		{
			ASXUIPC_Lobby* OwningCXPlayerController = Cast<ASXUIPC_Lobby>(OwningPlayerController);
			if (IsValid(OwningCXPlayerController) == true)
			{
				OwningCXPlayerController->SetChatMessageString(Text.ToString());

				EditableTextChatInput->SetText(FText());
			}
		}
	}
}