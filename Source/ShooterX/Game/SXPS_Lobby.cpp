// SXPS_Lobby.cpp


#include "Game/SXPS_Lobby.h"

#include "Net/UnrealNetwork.h"

ASXPS_Lobby::ASXPS_Lobby()
	: PlayerLobbyNameString(TEXT("None"))
{
	bReplicates = true;
}

void ASXPS_Lobby::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerLobbyNameString);
}
