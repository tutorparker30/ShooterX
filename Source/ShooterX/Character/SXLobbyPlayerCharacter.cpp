// SXLobbyPlayerCharacter.cpp


#include "SXLobbyPlayerCharacter.h"
#include "ShooterXPlayGround/SXCharacterMaterialManager.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

ASXLobbyPlayerCharacter::ASXLobbyPlayerCharacter()
{
}

void ASXLobbyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterSkeletalMeshComponent = GetMesh();

	const USXCharacterMaterialManager* CharacterMaterialManager = GetDefault<USXCharacterMaterialManager>();
	if (IsValid(CharacterMaterialManager) == false)
	{
		return;
	}

	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

	for (int32 i = 0; i < CharacterMaterialManager->PlayerCharacterMeshMaterialPaths.Num(); ++i)
	{
		const FSoftObjectPath& MaterialPath = CharacterMaterialManager->PlayerCharacterMeshMaterialPaths[i];

		TSharedPtr<FStreamableHandle> Handle =
			StreamableManager.RequestSyncLoad(MaterialPath, false);

		if (Handle.IsValid() == true)
		{
			StreamableHandles.Add(Handle);
		}

		TObjectPtr<UMaterialInstance> LoadedMaterial = Cast<UMaterialInstance>(MaterialPath.ResolveObject());

		if (IsValid(LoadedMaterial) == false)
		{
			continue;
		}

		if (i % 2 == 0)
		{
			LoadedMaterialInstance0Assets.Add(LoadedMaterial);
		}
		else
		{
			LoadedMaterialInstance1Assets.Add(LoadedMaterial);
		}
	}

	if (CharacterSkeletalMeshComponent.IsValid() == true &&
		LoadedMaterialInstance0Assets.Num() > 0 &&
		LoadedMaterialInstance1Assets.Num() > 0)
	{
		CharacterSkeletalMeshComponent->SetMaterial(1, LoadedMaterialInstance0Assets[SelectedMeshMaterialIndex].Get());
		CharacterSkeletalMeshComponent->SetMaterial(0, LoadedMaterialInstance1Assets[SelectedMeshMaterialIndex].Get());
	}
}

void ASXLobbyPlayerCharacter::SetLocalCharacterSkeletalMesh(int32 InPrevOrNext)
{
	if (CharacterSkeletalMeshComponent.IsValid() == false)
	{
		return;
	}

	const int32 MaterialCount = FMath::Min(
		LoadedMaterialInstance0Assets.Num(),
		LoadedMaterialInstance1Assets.Num()
	);

	if (MaterialCount <= 0)
	{
		return;
	}

	SelectedMeshMaterialIndex += InPrevOrNext;

	if (SelectedMeshMaterialIndex < 0)
	{
		SelectedMeshMaterialIndex = MaterialCount - 1;
	}

	if (MaterialCount <= SelectedMeshMaterialIndex)
	{
		SelectedMeshMaterialIndex = 0;
	}

	CharacterSkeletalMeshComponent->SetMaterial(1, LoadedMaterialInstance0Assets[SelectedMeshMaterialIndex].Get());
	CharacterSkeletalMeshComponent->SetMaterial(0, LoadedMaterialInstance1Assets[SelectedMeshMaterialIndex].Get());
		// 꽂아줄때는 반대로 꽂아야함.
}
