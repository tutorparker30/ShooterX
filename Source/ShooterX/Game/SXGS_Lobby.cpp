// SXGS_Lobby.cpp


#include "SXGS_Lobby.h"

#include "Kismet/GameplayStatics.h"
#include "Controller/SXUIPC_Lobby.h"

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
