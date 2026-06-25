// SXGM_Lobby.cpp


#include "Game/SXGM_Lobby.h"

#include "SXGS_Lobby.h"
#include "Controller/SXUIPC_Lobby.h"
#include "Game/SXPS_Lobby.h"

void ASXGM_Lobby::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	/*
	ASXGS_Lobby* SXGS_Lobby =  GetGameState<ASXGS_Lobby>();
	if (IsValid(SXGS_Lobby) == true)
	{
		SXGS_Lobby->MulticastRPCBroadcastLoginMessage(TEXT("XXXXXXX"));
	}
	*/

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
