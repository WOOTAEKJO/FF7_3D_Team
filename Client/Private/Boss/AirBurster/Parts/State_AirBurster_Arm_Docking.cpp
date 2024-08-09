#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Docking.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_Parts.h"

#include "GameInstance.h"

CState_AirBurster_Arm_Docking::CState_AirBurster_Arm_Docking(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Arm_Docking::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1", 1.f, false);

	SetUp_AI_Action_Num(AI_ACTION_NON_TARGET);

	if (!m_pBehaviorTree)
	{
		m_vTargetPos = _float3(99.f, 8.f, 89.f);

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
	// IDLE 상태로 돌아가지만 부모에 속함 -> 상태 비활성화
	return S_OK;
}

void CState_AirBurster_Arm_Docking::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Arm_Docking::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// 먼저 look을 변경 천천히
	// 그 다음 look 반대 방향으로 이동
	// 왼쪽과 오른쪽의 속도를 다르게 준다.
	// 그래야 서로 다른 시점에서 도킹을 하기 때문

	_vector vTargerLook = XMVector3Normalize(m_vTargetPos - m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION));
	m_pActor_TransformCom.lock()->Set_BaseDir(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
	if (!m_pActor_TransformCom.lock()->Rotate_On_BaseDir(fTimeDelta, -vTargerLook))
	{
		_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
		if (!XMVector3NearEqual(vPos, m_vTargetPos, XMVectorSet(2.5f, 2.5f, 2.5f, 0.f)))
		{
			_float fSpeed;

			if (Compare_Wstr(m_pActor.lock()->Get_PrototypeTag(), TEXT("Prototype_GameObject_AirBurster_LeftArm")))
				fSpeed = 6.f;
			else if (Compare_Wstr(m_pActor.lock()->Get_PrototypeTag(), TEXT("Prototype_GameObject_AirBurster_RightArm")))
				fSpeed = 4.f;

			m_pActor_TransformCom.lock()->Go_Backward(fTimeDelta * fSpeed);
		}
		else {
			m_bDockingFinish = true;
			static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Set_Docking(true);
		}
	}
	
}

void CState_AirBurster_Arm_Docking::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Arm_Docking::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	// 독립 판단, 도킹 준비 판단 변수들은 본체에서 제어

	m_pActor_TransformCom.lock()->Set_Position(1.f, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_bDockingFinish = false;
}

bool CState_AirBurster_Arm_Docking::isValid_NextState(CState* state)
{
	if (m_bDockingFinish)
		return true;
	else
		return false;
}

void CState_AirBurster_Arm_Docking::Free()
{
}
