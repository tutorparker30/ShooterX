// SXUIPC_Lobby.h

#pragma once

#include "CoreMinimal.h"
#include "Controller/SXUIPlayerControllerBase.h"
#include "SXUIPC_Lobby.generated.h"

class USXChatInput;

/**
 * 
 */
UCLASS()
class SHOOTERX_API ASXUIPC_Lobby : public ASXUIPlayerControllerBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USXChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<USXChatInput> ChatInputWidgetInstance;

	FString ChatMessageString;

};
