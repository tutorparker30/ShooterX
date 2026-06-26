// SXLobbyPlayerCharacter.h

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

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_Owner() override;

	virtual void PostNetInit() override;

#pragma endregion


#pragma region MeshMaterial

public:
	void SetLocalCharacterSkeletalMesh(int32 InPrevOrNext);

private:
	UFUNCTION(Server, Reliable)
	void ServerRPCSetCharacterSkeletalMesh(int32 InPrevOrNext);

	UFUNCTION()
	void OnRep_SelectedMeshMaterialIndex();

public:
	TWeakObjectPtr<USkeletalMeshComponent> CharacterSkeletalMeshComponent;

	TArray<TSharedPtr<FStreamableHandle>> StreamableHandles;

	TArray<TObjectPtr<UMaterialInstance>> LoadedMaterialInstance0Assets;

	TArray<TObjectPtr<UMaterialInstance>> LoadedMaterialInstance1Assets;

	UPROPERTY(ReplicatedUsing = OnRep_SelectedMeshMaterialIndex)
	int32 SelectedMeshMaterialIndex = 0;

#pragma endregion

};
