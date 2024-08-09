#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_RocketArm.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "UI_Manager.h"

#include "GameInstance.h"
#include "StatusComp.h"

#include "Camera_Manager.h"

CState_AirBurster_RocketArm::CState_AirBurster_RocketArm(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_RocketArm::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	SetUp_AI_Action_Num(AI_ACTION_NON_TARGET);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkRocket01", 1.f * AIRBURSTERANIMSPEED, false);

	if (!m_pBehaviorTree)
	{
		m_pActor.lock()->Get_PhysXColliderCom().lock()->PutToSleep();
		// 콜라이더 off

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Call_IDLE)
			.end()
			.build();
	}

	GET_SINGLE(CUI_Manager)->Call_Dialog("U_Dialog_S3_D1_");

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Airburster_Separate_Following", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE, m_pActor.lock());

	return S_OK;
}

void CState_AirBurster_RocketArm::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_RocketArm::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pActor_ModelCom.lock()->IsAnimation_UpTo(50.f))
	{
		// 팔들 분리
		if (!static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_SeparateArm(L"Part_LeftArm"))
			static_pointer_cast<CAirBurster>(m_pActor.lock())->Set_SeparateArm(L"Part_LeftArm", true);
		if (!static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_SeparateArm(L"Part_RightArm"))
			static_pointer_cast<CAirBurster>(m_pActor.lock())->Set_SeparateArm(L"Part_RightArm", true);
		
		
	}
}

void CState_AirBurster_RocketArm::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_RocketArm::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_Weapon(L"Part_LeftArm");
	static_pointer_cast<CCharacter>(m_pActor.lock())->TurnOn_Weapon(L"Part_RightArm");
	// 팔들 콜라이더 on
	
	// 무적 상태
	auto pStatusCom = DynPtrCast<IStatusInterface>(m_pActor.lock())->Get_StatusCom().lock();
	pStatusCom->Set_IsInvincible(true);
}

bool CState_AirBurster_RocketArm::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_AirBurster_RocketArm::Free()
{
}
