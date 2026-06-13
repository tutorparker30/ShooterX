// SXEagle.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SXFlyable.h"
#include "SXEagle.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERX_API USXEagle 
	: public UObject
	, public ISXFlyable
{
	GENERATED_BODY()

public:
	USXEagle();

	virtual void Fly() override;

private:
	UPROPERTY()
	FString Name;

};

