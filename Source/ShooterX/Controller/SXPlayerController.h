#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SXPlayerController.generated.h"

class USXHUD;
class UUserWidget;

UCLASS()
class SHOOTERX_API ASXPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASXPlayerController();
	USXHUD* GetHUDWidget() const { return HUDWidget; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<USXHUD> HUDWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess))
	TSubclassOf<USXHUD> HUDWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ASPlayerController, Meta = (AllowPrivateAccess))
	TSubclassOf<UUserWidget> CrosshairWidgetClass;
};
