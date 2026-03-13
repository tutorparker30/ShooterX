#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SXPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxKillCountChangedDelegate, int32, InMaxKillCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentKillCountChangedDelegate, int32, InCurrentKillCount);

UCLASS()
class SHOOTERX_API ASXPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ASXPlayerState();
	int32 GetMaxKillCount() const { return MaxKillCount; }
	void SetMaxKillCount(int32 InMaxKillCount) { MaxKillCount = InMaxKillCount; }
	int32 GetCurrentKillCount() const { return CurrentKillCount; }
	void AddCurrentKillCount(int32 InCurrentKillCount);

public:
	FOnMaxKillCountChangedDelegate OnMaxKillCountChanged;
	FOnCurrentKillCountChangedDelegate OnCurrentKillCountChanged;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 MaxKillCount = 99;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 CurrentKillCount = 0;

};
