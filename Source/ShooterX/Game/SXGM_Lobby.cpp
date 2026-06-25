// SXGM_Lobby.cpp


#include "Game/SXGM_Lobby.h"

#include "SXGS_Lobby.h"

void ASXGM_Lobby::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ASXGS_Lobby* SXGS_Lobby =  GetGameState<ASXGS_Lobby>();
	if (IsValid(SXGS_Lobby) == true)
	{
		SXGS_Lobby->MulticastRPCBroadcastLoginMessage(TEXT("XXXXXXX"));
	}
}
