#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Return.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_Parts.h"
#include "Player.h"

#include "GameInstance.h"

CState_AirBurster_Arm_Return::CState_AirBurster_Arm_Return(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Arm_Return::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1", 1.f, false);

	SetUp_AI_Action_Num(AI_ACTION_NOR_ATTACK);

	if (!m_pBehaviorTree)
	{

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

void CState_AirBurster_Arm_Return::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Arm_Return::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
	_vector vOriginPos = static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Get_ActivePos();
	// 저장되어 있던 위치를 가져와서 비교하여 움직임 제어

	_vector vDir = vOriginPos - vPos;
	_vector vLook = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK);

	_float fLength = XMVectorGetX(XMVector3Length(vDir));

	if (XMVectorGetX(XMVector3Dot(vLook, vDir)) < 0.f)
	{
		if (fLength > 0.3f)
		{
			_float fSpeed = 3.f;

			if (fLength <= 0.7f)
				fSpeed = 1.f;

			m_pActor_TransformCom.lock()->Go_Backward(fTimeDelta * fSpeed);
		}
		else {
			m_bArrive = true;
			m_pActor_TransformCom.lock()->Set_Position(1.f, vOriginPos);
		}
	}
	else
	{
		if (fLength > 0.3f)
		{
			_float fSpeed = 3.f;

			if (fLength <= 0.7f)
				fSpeed = 1.f;

			m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * fSpeed);
		}
		else {
			m_bArrive = true;
			m_pActor_TransformCom.lock()->Set_Position(1.f, vOriginPos);
		}
	}

}

void CState_AirBurster_Arm_Return::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Arm_Return::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	m_bArrive = false;
}

bool CState_AirBurster_Arm_Return::isValid_NextState(CState* state)
{
	if (m_bArrive)
		return true;
	else
		return false;
}

void CState_AirBurster_Arm_Return::Free()
{
}
