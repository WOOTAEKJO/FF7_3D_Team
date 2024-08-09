#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_Transform.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "GameInstance.h"

CState_AirBurster_Transform::CState_AirBurster_Transform(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Transform::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1", 1.f, true,
		static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_Transition());

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE Judge_Phase
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_ChangePhase())
			{
				m_quePreNum.pop();
				m_quePreNum.pop();
				m_quePreNum.push(-1);
				m_quePreNum.push(-1);

				m_iCurPhaseNum = static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_CurPhase();
			}

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Docking
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_SeparateArm(L"Part_LeftArm") &&
			static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_SeparateArm(L"Part_RightArm") &&
			static_pointer_cast<CAirBurster>(m_pActor.lock())->Judge_Docking())
			{// 팔이 분리된 상태에서 도킹을 판단
				if (m_iCurPhaseNum == CAirBurster::AIRBURSTER_PAHSE::PHASE2)
				{
					if (m_bCheckPattern)
					{
						m_eCurPattern = AIRBURSTER_PATTERN::P1_Docking;

						m_bCheckPattern = false; // 패턴 체크
						m_bRocketPattern = false; // 로켓 패턴 해제
					}
				}
			}

			return BT_STATUS::Success;
		};

		FunctionNode Random_Pattern
			= FUNCTION_NODE_MAKE
		{
			if (m_iCurPhaseNum == CAirBurster::AIRBURSTER_PAHSE::PHASE1)
			{
				if (m_bCheckPattern) {

					if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_ChangePhase())
					{
						m_eCurPattern = AIRBURSTER_PATTERN::P1_PHASE1ENTER;
					}
					else {
						m_eCurPattern = (AIRBURSTER_PATTERN)m_iPhase1Num[RandNum(5)];
					}

					m_bCheckPattern = false;
				}

			}
			else if (m_iCurPhaseNum == CAirBurster::AIRBURSTER_PAHSE::PHASE2)
			{
				if (m_bCheckPattern) {

					if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_ChangePhase())
					{
						m_eCurPattern = AIRBURSTER_PATTERN::P1_PHASE2ENTER;
					}
					else {

						if (!m_bRocketPattern && (m_iPhase2Count == ROCKETCONDITIONNUM1
							|| m_iPhase2Count == ROCKETCONDITIONNUM2))
						{
							m_bRocketPattern = true;
							m_eCurPattern = AIRBURSTER_PATTERN::P1_ROCKETARM;
							++m_iPhase2Count;
						}
						else {
							if (!m_bRocketPattern)
							{
								m_eCurPattern = (AIRBURSTER_PATTERN)m_iPhase2Num[RandNum(4)];
								++m_iPhase2Count;
							}
							else
								m_eCurPattern = (AIRBURSTER_PATTERN)m_iPhase2RocketArmNum[RandNum(3)];
						}
					}

					m_bCheckPattern = false;
				}
			}
			else if (m_iCurPhaseNum == CAirBurster::AIRBURSTER_PAHSE::PHASE3)
			{
				if (m_bCheckPattern)
				{
					m_eCurPattern = AIRBURSTER_PATTERN::P1_Dead;
					// 죽음 처리

				m_bCheckPattern = false;
			}
		}
		else
			return BT_STATUS::Failure;

		return BT_STATUS::Success;
		};

		FunctionNode Call_Phase1
			= FUNCTION_NODE_MAKE
		{
			if (m_iCurPhaseNum == CAirBurster::AIRBURSTER_PAHSE::PHASE1)
			{
				switch (m_eCurPattern)
				{
				case Client::CState_AirBurster_Transform::P1_PHASE1ENTER:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_1PhaseEnter>();
					break;
				case Client::CState_AirBurster_Transform::P1_REARMACHINEGUN:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_RearMachineGun>();
					break;
				case Client::CState_AirBurster_Transform::P1_FRONTMACHINEGUN:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_FrontMachineGun>();
					break;
				case Client::CState_AirBurster_Transform::P1_ENERGYBALL:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_EnergyBall>();
					break;
				case Client::CState_AirBurster_Transform::P1_FINGERBEAM:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_FingerBeam>();
					break;
				case Client::CState_AirBurster_Transform::P1_EMFIELD:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_EMField>();
					break;
#pragma region 나중에 빼야함
				case Client::CState_AirBurster_Transform::P1_SHOULDERBEAM:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_ShoulderBeam>();
					break;
#pragma endregion

				default:
					return BT_STATUS::Failure;

				}
			}
			else
				return BT_STATUS::Failure;

		return BT_STATUS::Success;
		};

		FunctionNode Call_Phase2
			= FUNCTION_NODE_MAKE
		{
			if (m_iCurPhaseNum == CAirBurster::AIRBURSTER_PAHSE::PHASE2)
			{
				switch (m_eCurPattern)
				{
				case Client::CState_AirBurster_Transform::P1_PHASE2ENTER:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_2PhaseEnter>();
					break;
				case Client::CState_AirBurster_Transform::P1_TANKBURSTER:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_TankBurster>();
					break;
				case Client::CState_AirBurster_Transform::P1_ENERGYBALL:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_EnergyBall>();
					break;
				case Client::CState_AirBurster_Transform::P1_SHOULDERBEAM:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_ShoulderBeam>();
					break;
				case Client::CState_AirBurster_Transform::P1_FINGERBEAM:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_FingerBeam>();
					break;
				case Client::CState_AirBurster_Transform::P1_BURNUR:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Burner>();
					break;
				case Client::CState_AirBurster_Transform::P1_ROCKETARM:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_RocketArm>();
					break;
				case Client::CState_AirBurster_Transform::P1_Docking:
					m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Docking>();
					break;
				default:
					return BT_STATUS::Failure;
				}
			}
			else
				return BT_STATUS::Failure;

		return BT_STATUS::Success;
		};

		FunctionNode Call_Phase3
			= FUNCTION_NODE_MAKE
		{
			if (m_iCurPhaseNum == CAirBurster::AIRBURSTER_PAHSE::PHASE3)
			{
				m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Dead>();
				// 임시로 죽음처리
			}
			else
				return BT_STATUS::Failure;

		return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
			.leaf<FunctionNode>(Judge_Phase)
			.leaf<FunctionNode>(Condition_Docking)
			.leaf<FunctionNode>(Random_Pattern)
			.composite<Selector>()
			.leaf<FunctionNode>(Call_Phase1)
			.leaf<FunctionNode>(Call_Phase2)
			.leaf<FunctionNode>(Call_Phase3)
			.end()
			.end()
			.build();
	}
	return S_OK;
}

void CState_AirBurster_Transform::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_fTimeAcc += fTimeDelta;

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Transform::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_Transform::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Transform::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	static_pointer_cast<CAirBurster>(m_pActor.lock())->Set_Transition(true);
	m_fTimeAcc = 0.f;

	m_bCheckPattern = true;

}

bool CState_AirBurster_Transform::isValid_NextState(CState* state)
{
	if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_ChangePhase())
	{
		return true;
	}
	else {
		if (m_fTimeAcc > 1.f)
			return true;
		else
			return false;
	}
	
}

_uint CState_AirBurster_Transform::RandNum(_uint iPatternNum)
{
	_uint iRandNum = rand() % iPatternNum;

	if (m_quePreNum.front() != -1)
	{
		if (m_quePreNum.front() == iRandNum || m_quePreNum.back() == iRandNum)
			iRandNum = RandNum(iPatternNum);
		else {
			m_quePreNum.pop();
			m_quePreNum.push(iRandNum);
		}
	}
	else {
		m_quePreNum.pop();
		m_quePreNum.push(iRandNum);
	}

	return iRandNum;
}

void CState_AirBurster_Transform::Free()
{
}
