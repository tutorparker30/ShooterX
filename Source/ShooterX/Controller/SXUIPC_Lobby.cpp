// SXUIPC_Lobby.cpp


#include "Controller/SXUIPC_Lobby.h"

#include "UI/SXChatInput.h"
#include "ShooterX.h"
#include "EngineUtils.h"

void ASXUIPC_Lobby::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		ChatInputWidgetInstance = CreateWidget<USXChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			ChatInputWidgetInstance->AddToViewport();
		}
	}
}

void ASXUIPC_Lobby::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	//PrintChatMessageString(InChatMessageString);
	if (IsLocalController() == true)
	{
		ServerRPCPrintChatMessageString(InChatMessageString);
	}
}

void ASXUIPC_Lobby::PrintChatMessageString(const FString& InChatMessageString)
{
	ShooterXFunctionLibrary::MyPrintString(this, InChatMessageString, 5.0f, FColor::Red);
}

void ASXUIPC_Lobby::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void ASXUIPC_Lobby::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	for (TActorIterator<ASXUIPC_Lobby> It(GetWorld()); It; ++It)
	{
		ASXUIPC_Lobby* PlayerController = *It;
		if (IsValid(PlayerController) == true)
		{
			PlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
		}
	}
}
