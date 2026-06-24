// ShooterX.h

#pragma once

#include "CoreMinimal.h"

#pragma region Custom Collision Channel

#define ECC_SX_CHARACTER_MESH ECollisionChannel::ECC_GameTraceChannel1
#define ECC_ATTACK ECollisionChannel::ECC_GameTraceChannel2
#define ECC_CAMERAAIM ECollisionChannel::ECC_GameTraceChannel4

#pragma endregion

class ShooterXFunctionLibrary
{
public:
	static void MyPrintString(const AActor* InWorldContextActor, const FString& InString, float InTimeToDisplay = 1.f, FColor InColor = FColor::Cyan)
	{
		if (IsValid(GEngine) == true && IsValid(InWorldContextActor) == true)
		{
			if (InWorldContextActor->GetNetMode() == NM_Client || InWorldContextActor->GetNetMode() == NM_ListenServer)
			{
				GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, InString);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("%s"), *InString);
			}
		}
	}

	static FString GetNetModeString(const AActor* InWorldContextActor)
	{
		FString NetModeString = TEXT("None");

		if (IsValid(InWorldContextActor) == true)
		{
			ENetMode NetMode = InWorldContextActor->GetNetMode();
			if (NetMode == NM_Client)
			{
				NetModeString = TEXT("Client");
			}
			else
			{
				if (NetMode == NM_Standalone)
				{
					NetModeString = TEXT("StandAlone");
				}
				else
				{
					NetModeString = TEXT("Server");
				}
			}
		}

		return NetModeString;
	}

	static FString GetRoleString(const AActor* InActor)
	{
		FString RoleString = TEXT("None");

		if (IsValid(InActor) == true)
		{
			FString LocalRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InActor->GetLocalRole());
			FString RemoteRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InActor->GetRemoteRole());

			RoleString = FString::Printf(TEXT("%s / %s"), *LocalRoleString, *RemoteRoleString);
		}

		return RoleString;
	}

};