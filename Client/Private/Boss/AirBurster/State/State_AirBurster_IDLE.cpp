#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_IDLE.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "Boss/AirBurster/Weapon/AirBurster_ShoulderBeam.h"

#include "GameInstance.h"
#include "Utility/ActionKey.h"

CState_AirBurster_IDLE::CState_AirBurster_IDLE(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_IDLE::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle02_1", 1.5f * AIRBURSTERANIMSPEED, false,
		dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_SKILL);

	if (!m_pBehaviorTree)
	{
		

		FUNCTION_NODE Call_Transform
			= FUNCTION_NODE_MAKE
		{
			if(static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_CurPhase() == CAirBurster::PHASE1)
				m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Control_Phase1>();
			else if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_CurPhase() == CAirBurster::PHASE2)
				m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Control_Phase2>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Call_Transform)
			.end()
			.build();
	}
	return S_OK;
}

void CState_AirBurster_IDLE::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_IDLE::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_IDLE::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_IDLE::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Set_Transition(true);
}

bool CState_AirBurster_IDLE::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_AirBurster_IDLE::Free()
{
}
