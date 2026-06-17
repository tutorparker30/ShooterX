// SXHUD.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SXHUD.generated.h"

class USXStatusComponent;
class ASXPlayerState;
class UTextBlock;
class UUW_HPBar;

/**
 *
 */
UCLASS()
class SHOOTERX_API USXHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindStatusComponent(USXStatusComponent* InStatusComponent);

	void BindPlayerState(ASXPlayerState* InPlayerState);

protected:
	UFUNCTION()
	void OnKillCountChanged(int32 InKillCount);

protected:
	TObjectPtr<USXStatusComponent> StatusComponent;

	TObjectPtr<ASXPlayerState> PlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> KillCountText;
	// BindWidget 키워드를 통해서 하드코딩으로 속성과 위젯을 바인드 하지 않아도 됨.
	// 대신 속성의 이름과 위젯 블루프린트 속 위젯의 이름이 같이야함.

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UUW_HPBar> HPBar;

};
