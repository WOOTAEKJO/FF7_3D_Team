#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Term.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_Parts.h"

#include "GameInstance.h"

CState_AirBurster_Arm_Term::CState_AirBurster_Arm_Term(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(3.f);
}

HRESULT CState_AirBurster_Arm_Term::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
	m_pActor_ModelCom.lock()->Set_Animation("Main|B_DmgB03_2", 1.f, false,
		static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Get_Transition());
	
	SetUp_AI_Action_Num(AI_ACTION_SKILL);
	
	if (!m_pBehaviorTree)
	{

	FUNCTION_NODE Call_IDLE
		= FUNCTION_NODE_MAKE
	{
		m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Arm_IDLE>();

		return BT_STATUS::Success;
	};

	m_pBehaviorTree = Builder()
		.composite<Selector>()
			.leaf<FunctionNode>(Call_IDLE)
		.end()
		.build();

	}

	return S_OK;
}

void CState_AirBurster_Arm_Term::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Arm_Term::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_TimeChecker.Update(fTimeDelta))
		m_bNextState = true;
}

void CState_AirBurster_Arm_Term::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Arm_Term::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
	static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Set_Transition(false);

	m_bNextState = false;
}

bool CState_AirBurster_Arm_Term::isValid_NextState(CState* state)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Finished() && m_bNextState)
		return true;
	else
		return false;
}

void CState_AirBurster_Arm_Term::Free()
{
}
