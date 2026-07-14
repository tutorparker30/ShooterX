// SXGameplayTags.cpp

#include "SXGameplayTags.h"

namespace SXGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ID_Action_Movement_Rotate, "Ability.ID.Action.Movement.Rotate", "회전시키는 어빌리티 아이디 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ID_Action_Movement_Jumping, "Ability.ID.Action.Movement.Jump", "점프 어빌리티 아이디 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ID_Action_Combat_PlayingTestMontage, "Ability.ID.Action.Combat.PlayingTestMontage", "테스트 어빌리티 아이디 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ID_Action_Combat_Throw_Grenade, "Ability.ID.Action.Combat.Throw.Grenade", "수류탄 투척 어빌리티를 식별 태그");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Action_Movement_Rotate, "Ability.Type.Action.Movement.Rotate", "회전시키는 어빌리티 타입 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Action_Movement_Jumping, "Ability.Type.Action.Movement.Jump", "점프 어빌리티 타입 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Action_Combat_PlayingTestMontage, "Ability.Type.Action.Combat.PlayingTestMontage", "테스트 어빌리티 타입 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_TYPE_Action_Combat_Throw_Grenade, "Ability.Type.Action.Combat.Throw.Grenade", "수류탄 투척 타입 태그");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Action_Movement_Rotating, "State.Action.Movement.Rotating", "회전 중을 뜻하는 상태 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Action_Movement_Jumping, "State.Action.Movement.Jumping", "점프 중 상태 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Action_Combat_PlayingTestMontage, "State.Action.Combat.PlayingTestMontage", "테스트 상태 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Action_Combat_Aiming_Grenade, "State.Action.Combat.Aiming.Grenade", "수류탄을 들고 궤적을 조준 중 상태 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Condition_Combat_Dead, "State.Condition.Combat.Dead", "죽음 상태 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Condition_Combat_Invincible, "State.Condition.Combat.Invincible", "무적 상태 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Condition_Cooldown_Grenade, "State.Condition.Cooldown.Grenade", "수류탄 투척 쿨다운 적용 중 상태 태그");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Test, "InputTag.Test", "테스트 입력 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Jump, "InputTag.Jump", "점프 입력 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Play_TestMontage, "InputTag.Play.TestMontage", "테스트 입력 태그");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Action_Combat_Throw_Grenade, "InputTag.Action.Combat.Throw.Grenade", "수류탄 투척 입력 태그");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Action_Combat_Melee_CheckHit, "Event.Action.Combat.Melee.CheckHit", "근접 공격 히트 체크 이벤트 태그");
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "SetByCaller.Damage", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Action_Combat_Hit, "GameplayCue.Action.Combat.Hit", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Action_Interation_Open, "GameplayCue.Action.Interaction.Open", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Action_Combat_Hit_Periodic, "GameplayCue.Action.Combat.Hit.Periodic", "");

}
