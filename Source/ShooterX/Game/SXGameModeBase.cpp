// SXGameModeBase.cpp


#include "Game/SXGameModeBase.h"
#include "Controller/SXPlayerController.h"
#include "Character/SXPlayerPawn.h"

ASXGameModeBase::ASXGameModeBase()
{
	PlayerControllerClass = ASXPlayerController::StaticClass();
	DefaultPawnClass = ASXPlayerPawn::StaticClass();

	/*
	static ConstructorHelpers::FClassFinder<ASXPlayerCharacter> DefaultPlayerCharacterClassRef(TEXT("오브젝트패스"));
	if (DefaultPlayerCharacterClassRef.Class)
	{
		DefaultPawnClass = DefaultPlayerCharacterClassRef.Class;
	}
	*/
}
