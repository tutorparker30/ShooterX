// SXGameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SXGameModeBase.generated.h"

class ASXPlayerController;

/**
 *
 */
UCLASS()
class SHOOTERX_API ASXGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASXGameModeBase();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	virtual void BeginPlay() override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	void OnCharacterDead(ASXPlayerController* InController);

private:
	void RefreshPlayerControllers();

	UFUNCTION()
	void OnMainTimerElapsed();

	void NotifyToAllPlayer(const FString& NotificationString);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WaitingTime = 15.f;

	float RemainWaitingTimeForPlaying = 15.f;

	int MinimumPlayerCountForPlaying = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EndingTime = 15;

	float RemainWaitingTimeForEnding = 15;

protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<ASXPlayerController>> AlivePlayerControllers;

	UPROPERTY()
	TArray<TWeakObjectPtr<ASXPlayerController>> DeadPlayerControllers;

	FTimerHandle MainTimerHandle;

};
