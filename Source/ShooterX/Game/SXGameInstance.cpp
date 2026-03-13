#include "Game/SXGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "JsonObjectConverter.h"
#include "UObject/SavePackage.h"

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
