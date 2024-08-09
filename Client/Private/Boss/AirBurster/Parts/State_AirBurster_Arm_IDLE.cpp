#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_IDLE.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_Parts.h"
#include "Player.h"

#include "State.h"

#include "GameInstance.h"

CState_AirBurster_Arm_IDLE::CState_AirBurster_Arm_IDLE(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Arm_IDLE::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1", 1.f, false,
		static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_SKILL);

	if (!m_pBehaviorTree)
	{

		FUNCTION_NODE Condition_Activate
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Get_Independent())
			{
				return BT_STATUS::Success;

			}
			// �����̸� �� ���µ� Ȱ��ȭ
			m_bVisit_SeparateState = false; // ��Ȱ�� �� �湮 �ʱ�ȭ
			return BT_STATUS::Failure;
		};

		FUNCTION_NODE Condition_Separate
			= FUNCTION_NODE_MAKE
		{
			if (!m_bVisit_SeparateState)
			{
				m_bVisit_SeparateState = true;
				return BT_STATUS::Success;
				// �� �̵��ؾ� ��
			}

			return BT_STATUS::Failure;
		};

		FUNCTION_NODE Call_Separate
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Arm_Separate>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Dead
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Get_PartsDead())
				return BT_STATUS::Success;

			return BT_STATUS::Failure;
		}; // ���� ��������

		FUNCTION_NODE Call_Dead
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Arm_Dead>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Cannon
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Get_Cannon())
				return BT_STATUS::Success;

			return BT_STATUS::Failure;
		}; // ĳ�� ��������

		FUNCTION_NODE Call_Cannon
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Arm_Push>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_FingerBeam
			= FUNCTION_NODE_MAKE
		{
			auto pPlayer = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);

			if (!pPlayer.pTarget.lock())
				return BT_STATUS::Failure;

			_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
			_vector vPlayerPos = pPlayer.vTargetPos;

			if (XMVector3NearEqual(vPos, vPlayerPos,XMVectorSet(10.f,10.f,10.f,0.f)))
				return BT_STATUS::Success;

			return BT_STATUS::Failure;
		}; // �����Ÿ� �ȿ� ������ ������ ��

		FUNCTION_NODE Call_Beam
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Arm_FingerBeam>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Condition_Activate)
				.composite<Selector>()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_Separate) // �и����¸� �湮�ߴ���
						.leaf<FunctionNode>(Call_Separate)
					.end()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_Dead) // ���� ó���Ǿ�����
						.leaf<FunctionNode>(Call_Dead)
					.end()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_FingerBeam) // �� �����Ÿ� �ȿ� ���Դ���
						.leaf<FunctionNode>(Call_Beam)
					.end()
				.end()
			.end()
		.build();
	}
	
	m_pBehaviorTree->getBlackboard()->m_pPreviousState = pPreviousState;
		
	return S_OK;
}

void CState_AirBurster_Arm_IDLE::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Arm_IDLE::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_Arm_IDLE::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Arm_IDLE::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Set_Transition(true);
}

bool CState_AirBurster_Arm_IDLE::isValid_NextState(CState* state)
{
	return true;

}

void CState_AirBurster_Arm_IDLE::Free()
{
}
