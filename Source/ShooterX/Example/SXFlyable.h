// SXFlyable.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SXFlyable.generated.h"

UINTERFACE(MinimalAPI)
class USXFlyable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHOOTERX_API ISXFlyable
{
	GENERATED_BODY()

public:
	virtual void Fly() = 0;
		// ISFlyable 인터페이스에서 구현 해도되고 안해도됨. 보통은 안함.

};
