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
		// 1Phase Intro 전환
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
		}; // 분리 상태 중인지 판단 유무

		FUNCTION_NODE Condition_Cannon
			= FUNCTION_NODE_MAKE
		{
			if (m_iTankCannonCount == TANKCANNONCOUNT)
			{
				m_iTankCannonCount = 0;
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 캐논 상태인지 판단.

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
		}; // 도킹을 할 수 있는지 판단 유무

		FUNCTION_NODE Call_Docking
			= FUNCTION_NODE_MAKE
		{
			m_bSeparated = false; // 분리상태 해제
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Docking>();
			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Separate
			= FUNCTION_NODE_MAKE
		{
			if ((m_iRocketCount == ROCKETCONDITIONNUM1) && !m_bVisitRocket)
			{
				m_bVisitRocket = true; // 팔 분리는 한 번만 가능
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 분리할 수 있는지 판단 유무

		FUNCTION_NODE Call_Separate
			= FUNCTION_NODE_MAKE
		{
			m_bSeparated = true; // 분리 상태 on
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
					.leaf<FunctionNode>(Condition_Intro)// 인트로 판단
					.leaf<FunctionNode>(Call_Phase12Intro)
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Judge_Phase)// 죽음 판단
					.leaf<FunctionNode>(Call_Dead)
				.end()
				.composite<Selector>()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_Separating) // 결합 중인지 판단
						.composite<Selector>()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Cannon) // 캐논 판단
								.leaf<FunctionNode>(Call_Cannon)
							.end()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Docking) // 도킹을 할지 판단
								.leaf<FunctionNode>(Call_Docking)
							.end()
							.composite<StatefulSelector>() // 분리 중일 때 패턴 순서대로
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
							.leaf<FunctionNode>(Condition_Separate) // 분리할지 판단
							.leaf<FunctionNode>(Call_Separate)
						.end()
						.composite<StatefulSelector>()	// 분리하지 않은 상태에서 패턴 순서대로
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
