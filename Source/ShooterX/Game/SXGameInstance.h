#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "SXGameInstance.generated.h"

//class UDataTable;

USTRUCT(BlueprintType)
struct FSXMaxHPTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FSXMaxHPTableRow(){}

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MaxHP;

};

UCLASS()
class SHOOTERX_API USXGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;
	UDataTable* GetMaxHPDataTable() const { return MaxHPDataTable; }
	FSXMaxHPTableRow* GetMaxHPDataTableRow(int32 InLevel);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UDataTable> MaxHPDataTable;

};
