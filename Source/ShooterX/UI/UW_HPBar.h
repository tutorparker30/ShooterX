#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_HPBar.generated.h"

class USXStatusComponent;
class UProgressBar;
class UTextBlock;

UCLASS()
class SHOOTERX_API UUW_HPBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUW_HPBar(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	void InitializerHPBarWidget(USXStatusComponent* InStatusComponent);
	void UpdateProgressBar();
	UFUNCTION()
	void OnMaxHPChange(float InMaxHP);
	UFUNCTION()
	void OnCurrentHPChange(float InCurrentHP);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> HPProgressBar;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentHPText;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHPText;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> OwningActor;
	float LastUpdatedMaxHP = 0.f;
	float LastUpdatedCurrentHP = 0.f;

};
