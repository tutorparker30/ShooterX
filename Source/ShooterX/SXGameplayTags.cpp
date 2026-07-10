// SXGameplayTags.cpp

#include "SXGameplayTags.h"

namespace SXGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ID_Action_Movement_Rotate, "Ability.ID.Action.Movement.Rotate", "회전시키는 어빌리티 아이디 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ID_Action_Movement_Jumping, "Ability.ID.Action.Movement.Jump", "점프 어빌리티 아이디 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ID_Action_Combat_PlayingTestMontage, "Ability.ID.Action.Combat.PlayingTestMontage", "테스트 어빌리티 아이디 태그");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Action_Movement_Rotate, "Ability.Type.Action.Movement.Rotate", "회전시키는 어빌리티 타입 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Action_Movement_Jumping, "Ability.Type.Action.Movement.Jump", "점프 어빌리티 타입 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Action_Combat_PlayingTestMontage, "Ability.Type.Action.Combat.PlayingTestMontage", "테스트 어빌리티 타입 태그");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Action_Movement_Rotating, "State.Action.Movement.Rotating", "회전 중을 뜻하는 상태 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Action_Movement_Jumping, "State.Action.Movement.Jumping", "점프 중 상태 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Action_Combat_PlayingTestMontage, "State.Action.Combat.PlayingTestMontage", "테스트 상태 태그");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Test, "InputTag.Test", "테스트 입력 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Jump, "InputTag.Jump", "점프 입력 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Play_TestMontage, "InputTag.Play.TestMontage", "테스트 입력 태그");

}
