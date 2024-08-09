#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Push_Back.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_Parts.h"
#include "Player.h"

#include "Client_Manager.h"

#include "GameInstance.h"

CState_AirBurster_Arm_Push_Back::CState_AirBurster_Arm_Push_Back(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(0.3);
}

HRESULT CState_AirBurster_Arm_Push_Back::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1", 1.f, false);

	SetUp_AI_Action_Num(AI_ACTION_NOR_ATTACK);

	if (!m_pBehaviorTree)
	{
		m_vOriginPos = DynPtrCast<CAirBurster>(m_pActor.lock()->Get_Owner().lock())->Get_OriginPos();

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Arm_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
			.leaf<FunctionNode>(Call_IDLE)
			.end()
			.build();
	}

	return S_OK;
}

void CState_AirBurster_Arm_Push_Back::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Arm_Push_Back::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_TimeChecker.Update(fTimeDelta))	
		m_bFinished = true;
	else
	{
		_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);

		_vector vStartPos = static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Get_ActivePos();

		if (!XMVector3NearEqual(vStartPos, vPos, XMVectorSet(1.f, 1.f, 1.f, 0.f)))
		{
			_vector vLook = XMVector3Normalize(vStartPos - vPos);

			m_pActor_TransformCom.lock()->Add_Position(fTimeDelta * vLook * 5.f);
		}

	}
}

void CState_AirBurster_Arm_Push_Back::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Arm_Push_Back::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_bFinished = false;
}

bool CState_AirBurster_Arm_Push_Back::isValid_NextState(CState* state)
{
	if (m_bFinished)
		return true;
	else
		return false;

}

void CState_AirBurster_Arm_Push_Back::Free()
{
}
