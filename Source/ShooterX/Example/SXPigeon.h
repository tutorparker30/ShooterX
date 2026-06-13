// SXPigeon.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Example/SXFlyable.h" // 타 헤더 파일 인클루드 시에 반드시 generated.h 파일 위에 작성.
#include "SXPigeon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERX_API USXPigeon 
	: public UObject
	, public ISXFlyable
{
	GENERATED_BODY()
	
public:
	USXPigeon();

	virtual void Fly() override;

private:
	UPROPERTY()
	FString Name;

};
