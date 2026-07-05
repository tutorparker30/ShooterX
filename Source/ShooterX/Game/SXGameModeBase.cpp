// SXGameModeBase.cpp


#include "Game/SXGameModeBase.h"
#include "Controller/SXPlayerController.h"
#include "Character/SXPlayerPawn.h"
#include "Game/SXGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterX.h"
#include "Game/SXOnlineSessionSubsystem.h"

ASXGameModeBase::ASXGameModeBase()
{
	PlayerControllerClass = ASXPlayerController::StaticClass();
	DefaultPawnClass = ASXPlayerPawn::StaticClass();

	RemainWaitingTimeForPlaying = WaitingTime;
}

void ASXGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	RefreshPlayerControllers();

	ASXPlayerController* NewPlayerController = Cast<ASXPlayerController>(NewPlayer);
	if (IsValid(NewPlayerController) == false)
	{
		return;
	}

	if (AlivePlayerControllers.Find(NewPlayerController) != INDEX_NONE ||
		DeadPlayerControllers.Find(NewPlayerController) != INDEX_NONE)
	{
		return;
	}

	if (IsValid(NewPlayerController) == true)
	{
		AlivePlayerControllers.Add(NewPlayerController);

		NewPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
	}
}

void ASXGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	RefreshPlayerControllers();

	ASXPlayerController* ExitingPlayerController = Cast<ASXPlayerController>(Exiting);
	if (IsValid(ExitingPlayerController) == false)
	{
		return;
	}

	if (AlivePlayerControllers.Find(ExitingPlayerController) == INDEX_NONE ||
		DeadPlayerControllers.Find(ExitingPlayerController) == INDEX_NONE)
	{
		return;
	}

	AlivePlayerControllers.Remove(ExitingPlayerController);
	DeadPlayerControllers.Remove(ExitingPlayerController);
}

void ASXGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(MainTimerHandle, this, &ThisClass::OnMainTimerElapsed, 0.1f, true);
	RemainWaitingTimeForEnding = EndingTime;
}

void ASXGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	ASXGameStateBase* SXGameState = GetGameState<ASXGameStateBase>();
	if (IsValid(SXGameState) == false)
	{
		return;
	}

	if (SXGameState->MatchState != EMatchState::Waiting)
	{
		ErrorMessage = FString(TEXT("DXGameState->MatchState != EMatchState::Waiting"));
		return;
	}
}

void ASXGameModeBase::OnCharacterDead(ASXPlayerController* InController)
{
	RefreshPlayerControllers();

	if (IsValid(InController) == false)
	{
		return;
	}

	if (AlivePlayerControllers.Find(InController) == INDEX_NONE ||
		DeadPlayerControllers.Find(InController) != INDEX_NONE)
	{
		return;
	}

	InController->ClientRPCShowGameResultWidget(AlivePlayerControllers.Num());

	AlivePlayerControllers.Remove(InController);
	DeadPlayerControllers.Add(InController);
}

void ASXGameModeBase::RefreshPlayerControllers()
{
	for (int32 i = AlivePlayerControllers.Num() - 1; i >= 0; --i)
	{
		ASXPlayerController* SXPlayerController = AlivePlayerControllers[i].Get();
		if (IsValid(SXPlayerController) == false)
		{
			AlivePlayerControllers.RemoveAt(i);
			continue;
		}
	}

	for (int32 i = DeadPlayerControllers.Num() - 1; i >= 0; --i)
	{
		ASXPlayerController* SXPlayerController = DeadPlayerControllers[i].Get();
		if (IsValid(SXPlayerController) == false)
		{
			DeadPlayerControllers.RemoveAt(i);
			continue;
		}
	}
}

void ASXGameModeBase::OnMainTimerElapsed()
{
	ASXGameStateBase* SXGameState = GetGameState<ASXGameStateBase>();
	if (IsValid(SXGameState) == false)
	{
		return;
	}

	switch (SXGameState->MatchState)
	{
	case EMatchState::None:
		break;
	case EMatchState::Waiting:
	{
		FString NotificationString = FString::Printf(TEXT(""));

		if (AlivePlayerControllers.Num() < MinimumPlayerCountForPlaying)
		{
			NotificationString = FString::Printf(TEXT("Wait another players for playing."));

			RemainWaitingTimeForPlaying = WaitingTime;
		}
		else
		{
			NotificationString = FString::Printf(TEXT("Wait %.0f seconds for playing."), RemainWaitingTimeForPlaying);

			RemainWaitingTimeForPlaying -= 0.1f;
		}

		if (RemainWaitingTimeForPlaying <= 0)
		{
			NotificationString = FString::Printf(TEXT(""));

			SXGameState->MatchState = EMatchState::Playing;

			RemainWaitingTimeForPlaying = 0.f;
		}

		NotifyToAllPlayer(NotificationString);

		break;
	}
	case EMatchState::Playing:
	{
		SXGameState->AlivePlayerControllerCount = AlivePlayerControllers.Num();

		FString NotificationString = FString::Printf(TEXT("%d / %d"), SXGameState->AlivePlayerControllerCount, SXGameState->AlivePlayerControllerCount + DeadPlayerControllers.Num());

		NotifyToAllPlayer(NotificationString);

		if (SXGameState->AlivePlayerControllerCount <= 1)
		{
			SXGameState->MatchState = EMatchState::Ending;

			AlivePlayerControllers[0]->ClientRPCShowGameResultWidget(1);
		}

		break;
	}
	case EMatchState::Ending:
	{
		FString NotificationString = FString::Printf(TEXT("Waiting %.0f for returning to title."), RemainWaitingTimeForEnding);

		NotifyToAllPlayer(NotificationString);

		RemainWaitingTimeForEnding -= 0.1f;

		if (RemainWaitingTimeForEnding <= 0)
		{
			USXOnlineSessionSubsystem* Subsystem = GetGameInstance()->GetSubsystem<USXOnlineSessionSubsystem>();
			if (IsValid(Subsystem) == true)
			{
				Subsystem->EndSession();
			}

			UE_LOG(LogTemp, Warning, TEXT("[RoundEnd] NetMode=%s, Alive=%d, Dead=%d"),
				*ShooterXFunctionLibrary::GetNetModeString(this), AlivePlayerControllers.Num(), DeadPlayerControllers.Num());
			for (auto AliveController : AlivePlayerControllers)
			{
				if (AliveController->IsLocalController() == true)
				{
					LocalHostController = AliveController;
					continue;
				}
				AliveController->ClientRPCReturnToTitle();
			}
			for (auto DeadController : DeadPlayerControllers)
			{
				if (DeadController->IsLocalController() == true)
				{
					LocalHostController = DeadController;
					continue;
				}
				DeadController->ClientRPCReturnToTitle();
			}

			if (GetNetMode() == NM_DedicatedServer)
			{
				UGameplayStatics::OpenLevel(this, *FString(TEXT("Lobby")), true, FString(TEXT("listen")));
			}
			if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_Standalone)
			{
				if (LocalHostController.IsValid() == true)
				{
					LocalHostController->ClientRPCReturnToTitle();
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("[RoundEnd] Branch done. LocalHostControllerValid=%d"), LocalHostController.IsValid());

			RemainWaitingTimeForEnding = 0.f;

			return;
		}

		break;
	}
	case EMatchState::End:
		break;
	default:
		break;
	}
}

void ASXGameModeBase::NotifyToAllPlayer(const FString& NotificationString)
{
	RefreshPlayerControllers();

	for (auto AlivePlayerController : AlivePlayerControllers)
	{
		AlivePlayerController->NotificationText = FText::FromString(NotificationString);
	}

	for (auto DeadPlayerController : DeadPlayerControllers)
	{
		DeadPlayerController->NotificationText = FText::FromString(NotificationString);
	}
}
