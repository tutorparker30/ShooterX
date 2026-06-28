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

#pragma region NetLogging

SHOOTERX_API DECLARE_LOG_CATEGORY_EXTERN(LogSXNet, Log, All);

// this가 AActor 자식 클래스 객체일때만 사용 가능.
#if WITH_EDITOR
#define SX_NETMODE ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("Client%02d"), UE::GetPlayInEditorID()) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("StandAlone") : TEXT("Server")))
#else
#define SX_NETMODE ((GetNetMode() == ENetMode::NM_Client) ? TEXT("Client") : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("StandAlone") : TEXT("Server")))
#endif

#define SX_CURRENT_FUNCTION (ANSI_TO_TCHAR(__FUNCTION__))
#define SX_LOG_NET(LogCategory, Verbosity, Format, ...) UE_LOG(LogCategory, Verbosity, TEXT("[%s] %s %s"), SX_NETMODE, SX_CURRENT_FUNCTION, *FString::Printf(Format, ##__VA_ARGS__))

#define SX_LOCAL_ROLE *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
#define SX_REMOTE_ROLE *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))
#define SX_LOG_NET_ROLE(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), SX_NETMODE, SX_LOCAL_ROLE, SX_REMOTE_ROLE, SX_CURRENT_FUNCTION, *FString::Printf(Format, ##__VA_ARGS__))

#define SX_SUBOBJECT_LOCAL_ROLE *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
#define SX_SUBOBJECT_REMOTE_ROLE *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))
#define SX_LOG_SUBOBJECT_NET_ROLE(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s][%s/%s] %s %s"), SX_NETMODE, SX_SUBOBJECT_LOCAL_ROLE, SX_SUBOBJECT_REMOTE_ROLE, SX_CURRENT_FUNCTION, *FString::Printf(Format, ##__VA_ARGS__))

#pragma endregion
