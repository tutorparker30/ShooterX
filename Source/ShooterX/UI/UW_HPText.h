#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UW_HPText.generated.h"

class USXStatusComponent;
class UTextBlock;

UCLASS()
class SHOOTERX_API UUW_HPText : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUW_HPText(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	void InitializeHPTextWidget(USXStatusComponent* InStatusComponent);
	AActor* GetOwningActor() const { return OwningActor; }
	void SetOwningActor(AActor* InOwningActor) { OwningActor = InOwningActor; }
	UFUNCTION()
	void OnMaxHPChange(float InMaxHP);
	UFUNCTION()
	void OnCurrentHPChange(float InCurrentHP);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentHPText;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxHPText;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> OwningActor;

};
