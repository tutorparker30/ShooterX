// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SXGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class SHOOTERX_API USXGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	USXGameInstance();

	virtual void Init() override;

	virtual void Shutdown() override;

protected:
	UPROPERTY()
	FString Name;

};
