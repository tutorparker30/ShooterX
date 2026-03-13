#include "Game/SXGameModeBase.h"
#include "Controller/SXPlayerController.h"
#include "Character/SXPlayerPawn.h"

ASXGameModeBase::ASXGameModeBase()
{
	PlayerControllerClass = ASXPlayerController::StaticClass();
	DefaultPawnClass = ASXPlayerPawn::StaticClass();

}
