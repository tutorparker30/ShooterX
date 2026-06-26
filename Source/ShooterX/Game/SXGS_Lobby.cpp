// SXGS_Lobby.cpp


#include "SXGS_Lobby.h"

#include "Kismet/GameplayStatics.h"
#include "Controller/SXUIPC_Lobby.h"
#include "ShooterX.h"

ASXGS_Lobby::ASXGS_Lobby()
{
	SX_LOG_NET(LogSXNet, Log, TEXT(""));
}

void ASXGS_Lobby::HandleBeginPlay()
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	Super::HandleBeginPlay();
		// 서버 로직. 여기서 월드의 모든 액터들에게 BeginPlay() 함수 호출 지시.
		// 이를 통해 ASXGS_Lobby::OnRep_ReplicatedHasBegunPlay() 함수가 호출됨.

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));
}

void ASXGS_Lobby::OnRep_ReplicatedHasBegunPlay()
{
	SX_LOG_NET(LogSXNet, Log, TEXT("Begin"));

	Super::OnRep_ReplicatedHasBegunPlay();

	SX_LOG_NET(LogSXNet, Log, TEXT("End"));
}

void ASXGS_Lobby::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ASXUIPC_Lobby* LobbyPC = Cast<ASXUIPC_Lobby>(PC);
			if (IsValid(LobbyPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				LobbyPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
