// SXPlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SXPlayerController.generated.h"

class USXHUD;
class UUW_GameResult;

/**
 *
 */
UCLASS()
class SHOOTERX_API ASXPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASXPlayerController();

	USXHUD* GetHUDWidget() const { return HUDWidget; };

	void ToggleInGameMenu();

	void OnCharacterDead();

	UFUNCTION(Client, Reliable)
	void ClientRPCShowGameResultWidget(int32 InRanking);

	UFUNCTION(Client, Reliable)
	void ClientRPCReturnToTitle();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FText NotificationText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ASXPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> NotificationTextUIClass;

private:
	UPROPERTY();
	TObjectPtr<USXHUD> HUDWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess));
	TSubclassOf<USXHUD> HUDWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ASPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> CrosshairWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ASXPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> InGameMenuClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ASXPlayerController, Meta = (AllowPrivateAccess))
	TObjectPtr<UUserWidget> InGameMenuInstance;

	bool bIsInGameMenuOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ASXPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUW_GameResult> GameResultUIClass;

};
