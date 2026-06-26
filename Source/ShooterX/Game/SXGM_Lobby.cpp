// SXGM_Lobby.cpp


#include "Game/SXGM_Lobby.h"

#include "SXGS_Lobby.h"
#include "Controller/SXUIPC_Lobby.h"
#include "Game/SXPS_Lobby.h"
#include "ShooterX.h"

ASXGM_Lobby::ASXGM_Lobby()
{
	SX_LOG_NET(LogSXNet, Log, TEXT(""));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bUseSeamlessTravel = false;

	RemainTimeForPlaying = InitialRemainTimeForPlaying;
}

void ASXGM_Lobby::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	// ErrorMessage = TEXT("The server is currently full. Please try again later.");

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));
}

APlayerController* ASXGM_Lobby::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	APlayerController* LoginPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));

	return LoginPlayerController;
}

void ASXGM_Lobby::OnPostLogin(AController* NewPlayer)
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	Super::OnPostLogin(NewPlayer);

	UNetDriver* ServerNetDriver = GetNetDriver();
	if (IsValid(ServerNetDriver) == true)
	{
		if (ServerNetDriver->ClientConnections.Num() == 0)
		{
			SX_LOG_NET(LogSXNet, Log, TEXT("There is no client connection."));
		}
		else
		{
			for (const auto& ClientConnection : ServerNetDriver->ClientConnections)
			{
				if (IsValid(ClientConnection) == true)
				{
					SX_LOG_NET(LogSXNet, Log, TEXT("Client Connection: %s"), *ClientConnection->GetName());
				}
			}
		}
	}
	else
	{
		SX_LOG_NET(LogSXNet, Log, TEXT("ServerNetDriver is invalid."));
	}

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));

	ASXUIPC_Lobby* LobbyPC = Cast<ASXUIPC_Lobby>(NewPlayer);
	if (IsValid(LobbyPC) == true)
	{
		AllPlayerControllers.Add(LobbyPC);

		static int32 PlayerNumber = 0;
		ASXPS_Lobby* SXPS = LobbyPC->GetPlayerState<ASXPS_Lobby>();
		if (IsValid(SXPS) == true)
		{
			SXPS->PlayerLobbyNameString = TEXT("Player") + FString::FromInt(++PlayerNumber);
		}

		ASXGS_Lobby* SXGS_Lobby = GetGameState<ASXGS_Lobby>();
		if (IsValid(SXGS_Lobby) == true)
		{
			SXGS_Lobby->MulticastRPCBroadcastLoginMessage(SXPS->PlayerLobbyNameString);
		}
	}
}

void ASXGM_Lobby::StartPlay()
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	Super::StartPlay();

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));
}

void ASXGM_Lobby::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority() == false)
	{
		return;
	}

	if (bIsTravelling == true)
	{
		return;
	}

	if (AllPlayerControllers.Num() < RequiredPlayerCount)
	{
		RemainTimeForPlaying = InitialRemainTimeForPlaying;
		return;
	}

	RemainTimeForPlaying -= DeltaSeconds;

	SX_LOG_NET(LogSXNet, Log, TEXT("RemainTimeForPlaying: %.2f"), RemainTimeForPlaying);

	if (RemainTimeForPlaying <= 0.f)
	{
		bIsTravelling = true;

		UWorld* World = GetWorld();
		if (IsValid(World) == true)
		{
			World->ServerTravel(TEXT("L_Expanse"));
		}
	}
}

void ASXGM_Lobby::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ASXUIPC_Lobby* LobbyPC = Cast<ASXUIPC_Lobby>(Exiting);
	if (IsValid(LobbyPC) == true)
	{
		AllPlayerControllers.Remove(LobbyPC);
	}
}
