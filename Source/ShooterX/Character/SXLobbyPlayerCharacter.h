// SXCharacterBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SXLobbyPlayerCharacter.generated.h"

class UAnimMontage;
class USXStatusComponent;
class ASXWeapon;
class UMaterialInstance;
struct FStreamableHandle;

UCLASS()
class SHOOTERX_API ASXLobbyPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Override ACharacter

public:
	ASXLobbyPlayerCharacter();

	virtual void BeginPlay() override;

#pragma endregion


#pragma region MeshMaterial

public:
	void SetLocalCharacterSkeletalMesh(int32 InPrevOrNext);

public:
	TWeakObjectPtr<USkeletalMeshComponent> CharacterSkeletalMeshComponent;

	TArray<TSharedPtr<FStreamableHandle>> StreamableHandles;

	TArray<TObjectPtr<UMaterialInstance>> LoadedMaterialInstance0Assets;

	TArray<TObjectPtr<UMaterialInstance>> LoadedMaterialInstance1Assets;

	int32 SelectedMeshMaterialIndex = 0;

#pragma endregion

};
