// SXGameInstance.cpp


#include "Game/SXGameInstance.h"

#include "Game/SXOnlineSessionSubsystem.h"


void USXGameInstance::Init()
{
	Super::Init();

	if (IsValid(MaxHPDataTable) == false || MaxHPDataTable->GetRowMap().Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enuough data in MaxHPDataTable."));
	}
	else
	{
		for (int32 i = 1; i <= MaxHPDataTable->GetRowMap().Num(); ++i)
		{
			check(nullptr != GetMaxHPDataTableRow(i));
		}
	}
}

void USXGameInstance::Shutdown()
{
	USXOnlineSessionSubsystem* Subsystem = GetSubsystem<USXOnlineSessionSubsystem>();
	if (IsValid(Subsystem) == true)
	{
		Subsystem->DestroySession();
	}

	Super::Shutdown();
}

FSXMaxHPTableRow* USXGameInstance::GetMaxHPDataTableRow(int32 InLevel)
{
	if (IsValid(MaxHPDataTable) == true)
	{
		return MaxHPDataTable->FindRow<FSXMaxHPTableRow>(*FString::FromInt(InLevel), TEXT(""));
	}

	return nullptr;
}
