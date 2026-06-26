// SXUIPC_Lobby.cpp


#include "Controller/SXUIPC_Lobby.h"

#include "UI/SXChatInput.h"
#include "ShooterX.h"
#include "EngineUtils.h"
#include "Game/SXPS_Lobby.h"
#include "Character/SXLobbyPlayerCharacter.h"
#include "ShooterX.h"

ASXUIPC_Lobby::ASXUIPC_Lobby()
{
	SX_LOG_NET(LogSXNet, Log, TEXT(""));
}

void ASXUIPC_Lobby::PostInitializeComponents()
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	Super::PostInitializeComponents();

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));
}

void ASXUIPC_Lobby::BeginPlay()
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	Super::BeginPlay();

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));

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

	if (IsLocalController() == true)
	{
		//ServerRPCPrintChatMessageString(InChatMessageString);

		ASXPS_Lobby* SXPS = GetPlayerState<ASXPS_Lobby>();
		if (IsValid(SXPS) == true)
		{
			FString CombinedMessageString = SXPS->PlayerLobbyNameString + TEXT(": ") + InChatMessageString;

			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ASXUIPC_Lobby::PrintChatMessageString(const FString& InChatMessageString)
{
	ShooterXFunctionLibrary::MyPrintString(this, InChatMessageString, 5.0f, FColor::Red);
}

void ASXUIPC_Lobby::PostNetInit()
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	Super::PostNetInit();

	if (IsLocalController() == true)
	{
		UNetDriver* ClientNetDriver = GetNetDriver();
		if (IsValid(ClientNetDriver) == true)
		{
			UNetConnection* ServerConnection = ClientNetDriver->ServerConnection;
			if (IsValid(ServerConnection) == true)
			{
				SX_LOG_NET(LogSXNet, Log, TEXT("Server Connection: %s"), *ClientNetDriver->ServerConnection->GetName());
			}
			else
			{
				SX_LOG_NET(LogSXNet, Log, TEXT("There is no server connection."));
			}
		}
		else
		{
			SX_LOG_NET(LogSXNet, Log, TEXT("ClientNetDriver is invalid."));
		}
	}

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));
}

void ASXUIPC_Lobby::OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection)
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	Super::OnActorChannelOpen(InBunch, Connection);

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));
}

void ASXUIPC_Lobby::OnPossess(APawn* InPawn)
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	Super::OnPossess(InPawn);
		// 클라이언트에서는 OnPossess() 함수가 호출되지 않음에 주의.
		// 그렇다면 클라이언트에서 Owner는 어떻게 초기화 되는 걸까. 
		// AActor::Owner 속성은 ReplicatedUsing 키워드가 달린 속성임. 
		// Onwer가 초기화 되면 OnRep_Owner() 함수가 클라이언트에서 호출됨.

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));
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
