#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_Control_Phase2.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "GameInstance.h"

CState_AirBurster_Control_Phase2::CState_AirBurster_Control_Phase2(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Control_Phase2::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle02_1", 1.f * AIRBURSTERANIMSPEED, false,
		dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_NOR_ATTACK);

	if (!m_pBehaviorTree)
	{

		FUNCTION_NODE Condition_Intro
			= FUNCTION_NODE_MAKE
		{
			if (!m_bIntro)
			{
				m_bIntro = true;
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		};

		FUNCTION_NODE Call_Phase12Intro
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_2PhaseEnter>();
		// 1Phase Intro ��ȯ
		return BT_STATUS::Success;
		};

		FUNCTION_NODE Judge_Phase
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_ChangePhase())
				{
					if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_CurPhase() ==
						CAirBurster::PHASE3)
					{
						return BT_STATUS::Success;
					}
				}
				return BT_STATUS::Failure;
		};

		FUNCTION_NODE Call_Dead
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Set_State<CState_AirBurster_Dead>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Separating
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_SeparateArm(L"Part_LeftArm") &&
			static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_SeparateArm(L"Part_RightArm") &&
				m_bSeparated)
			{
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // �и� ���� ������ �Ǵ� ����

		FUNCTION_NODE Condition_Cannon
			= FUNCTION_NODE_MAKE
		{
			if (m_iTankCannonCount == TANKCANNONCOUNT)
			{
				m_iTankCannonCount = 0;
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // ĳ�� �������� �Ǵ�.

		FUNCTION_NODE Call_Cannon
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_TankBurster>();
			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Docking
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Judge_Docking())
			{
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // ��ŷ�� �� �� �ִ��� �Ǵ� ����

		FUNCTION_NODE Call_Docking
			= FUNCTION_NODE_MAKE
		{
			m_bSeparated = false; // �и����� ����
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Docking>();
			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Separate
			= FUNCTION_NODE_MAKE
		{
			if ((m_iRocketCount == ROCKETCONDITIONNUM1) && !m_bVisitRocket)
			{
				m_bVisitRocket = true; // �� �и��� �� ���� ����
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // �и��� �� �ִ��� �Ǵ� ����

		FUNCTION_NODE Call_Separate
			= FUNCTION_NODE_MAKE
		{
			m_bSeparated = true; // �и� ���� on
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_RocketArm>();
			return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_ShoulderBeam
			= FUNCTION_NODE_MAKE
		{
			if (m_bSeparated)
				m_iTankCannonCount += 1;
			else
				m_iRocketCount += 1;

			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_ShoulderBeam>();
			return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_Burner
			= FUNCTION_NODE_MAKE
		{
			m_iRocketCount += 1;
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Burner>();
			return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_EnergyBall
			= FUNCTION_NODE_MAKE
		{
			if (m_bSeparated)
				m_iTankCannonCount += 1;
			else
				m_iRocketCount += 1;

			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_EnergyBall>();
			return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_None
			= FUNCTION_NODE_MAKE
		{
			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Selector>()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Intro)// ��Ʈ�� �Ǵ�
					.leaf<FunctionNode>(Call_Phase12Intro)
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Judge_Phase)// ���� �Ǵ�
					.leaf<FunctionNode>(Call_Dead)
				.end()
				.composite<Selector>()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_Separating) // ���� ������ �Ǵ�
						.composite<Selector>()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Cannon) // ĳ�� �Ǵ�
								.leaf<FunctionNode>(Call_Cannon)
							.end()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Docking) // ��ŷ�� ���� �Ǵ�
								.leaf<FunctionNode>(Call_Docking)
							.end()
							.composite<StatefulSelector>() // �и� ���� �� ���� �������
								.leaf<FunctionNode>(Call_ShoulderBeam)
								.decorator<Repeater>(50)
									.leaf<FunctionNode>(Call_None)
								.end()
								.leaf<FunctionNode>(Call_EnergyBall)
								.decorator<Repeater>(50)
									.leaf<FunctionNode>(Call_None)
								.end()
							.end()
						.end()
					.end()
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Separate) // �и����� �Ǵ�
							.leaf<FunctionNode>(Call_Separate)
						.end()
						.composite<StatefulSelector>()	// �и����� ���� ���¿��� ���� �������
							.leaf<FunctionNode>(Call_EnergyBall)
							.decorator<Repeater>(50)
								.leaf<FunctionNode>(Call_None)
							.end()
							.leaf<FunctionNode>(Call_ShoulderBeam)
							.decorator<Repeater>(50)
								.leaf<FunctionNode>(Call_None)
							.end()
							.leaf<FunctionNode>(Call_Burner)
							.decorator<Repeater>(50)
								.leaf<FunctionNode>(Call_None)
							.end()
						.end()
					.end()
				.end()
			.end()
		.build();
	}
	return S_OK;
}

void CState_AirBurster_Control_Phase2::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Control_Phase2::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CState_AirBurster_Control_Phase2::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Control_Phase2::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Set_Transition(true);
}

bool CState_AirBurster_Control_Phase2::isValid_NextState(CState* state)
{
	return true;
}

void CState_AirBurster_Control_Phase2::Free()
{
}
