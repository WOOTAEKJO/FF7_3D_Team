#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_IDLE.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"

CState_Bahamut_IDLE::CState_Bahamut_IDLE(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
}

HRESULT CState_Bahamut_IDLE::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1", 1.f, true,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_SKILL);

	auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);

	if (pPlayerInfo.pTarget.lock() != nullptr)
		cout << "바하무트 타겟과의 거리 : " + to_string(pPlayerInfo.fDistance) << endl;
	CBahamut::PHASE eCurPhase = static_pointer_cast<CBahamut>(m_pActor.lock())->Get_CurPhase();
	cout << "바하무트 " + to_string((_int)eCurPhase) + "페이스" << endl;
	cout << "바하무트 카운트" + to_string(m_iUltimateCount) << endl;
	cout << "바하무트 패턴 갯수" + to_string(m_iPatternCount) << endl;

	if (!m_pBehaviorTree)
	{
		FUNCTION_NODE Condition_Dead
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CBahamut>(m_pActor.lock())->Judge_Dead())
			{
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 죽음 판단

		FUNCTION_NODE Call_Dead
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_Dead>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_MegaFlare
			= FUNCTION_NODE_MAKE
		{
			if (m_iUltimateCount == ULTIMATECOUNT)
			{
				m_iUltimateCount = 0;
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 궁극기 판단

		FUNCTION_NODE Call_MegaFlare
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_MegaFlare>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Count
			= FUNCTION_NODE_MAKE
		{
			if (m_iPatternCount == COUNTNUM)
			{ 
				m_iPatternCount = 0; // 카운트까지 패턴 갯수 초기화
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 카운트 상태 들어가는지 판단

		FUNCTION_NODE Call_Count
			= FUNCTION_NODE_MAKE
		{
			m_iUltimateCount += 1; // 카운트 갯수 업
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_Count>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Special
			= FUNCTION_NODE_MAKE
		{
			if (m_iPatternCount == SPECIALNUM)
			{
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 스페셜 공격 판단

		FUNCTION_NODE Condition_Grab
			= FUNCTION_NODE_MAKE
		{
			CBahamut::PHASE eCurPhase = static_pointer_cast<CBahamut>(m_pActor.lock())->Get_CurPhase();
			if ((eCurPhase == CBahamut::PHASE::PHASE0)||(eCurPhase == CBahamut::PHASE::PHASE1) || (eCurPhase == CBahamut::PHASE::PHASE2))
			{
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 스페셜 공격 판단에서 성공했을 시, 페이스가 조건에 맞으면 잡기 공격

		FUNCTION_NODE Call_Grab
			= FUNCTION_NODE_MAKE
		{
			m_iPatternCount += 1; // 카운트까지 패턴 갯수 업
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_Grab>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_AerialRave
			= FUNCTION_NODE_MAKE
		{
			CBahamut::PHASE eCurPhase = static_pointer_cast<CBahamut>(m_pActor.lock())->Get_CurPhase();
			if ((eCurPhase == CBahamut::PHASE::PHASE3) || (eCurPhase == CBahamut::PHASE::PHASE4) ||
			(eCurPhase == CBahamut::PHASE::PHASE5))
				return BT_STATUS::Success;
		}; // 스페셜 공격 판단 성공시, 페이스 조건에 맞으면 상태 들어감

		FUNCTION_NODE Call_AerialRave
			= FUNCTION_NODE_MAKE
		{
			m_iPatternCount += 1; // 카운트까지 패턴 갯수 업
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_AerialRave>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Middle_Distance
			= FUNCTION_NODE_MAKE
		{
			auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);

			if (pPlayerInfo.fDistance < MIDDLEDISTANCE)
			{
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 중간거리 이내에 있는지 판단

		FUNCTION_NODE Condition_Close_Distance
			= FUNCTION_NODE_MAKE
		{
			auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);

			if (pPlayerInfo.fDistance < CLOSEDISTANCE)
			{
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 근접거리 이내에 있는지 판단

		FUNCTION_NODE Condition_CountMelee
			= FUNCTION_NODE_MAKE
		{
			if (m_iMelee_Count == MELEEATTACKCOUNT)
			{
				m_iMelee_Count = 0;
				return BT_STATUS::Failure;
			}

			return BT_STATUS::Success;
		}; // 근접거리 이내에 있는지 판단

		FUNCTION_NODE Condition_Call_Clorush
			= FUNCTION_NODE_MAKE
		{
			if (m_iRandomNum == 0)
			{
				m_iMelee_Count += 1; // 귽접 갯수 업
				m_iPatternCount += 1; // 카운트까지 패턴 갯수 업
				m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_Clorush>();
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 랜덤 숫자가 맞으면 상태를 부르고 성공

		FUNCTION_NODE Condition_Call_FlareBreath
			= FUNCTION_NODE_MAKE
		{
			if (m_iRandomNum == 1)
			{
				m_iMelee_Count += 1; // 근접 갯수 업
				m_iPatternCount += 1; // 카운트까지 패턴 갯수 업
				m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_FlareBreath>();
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		};// 랜덤 숫자가 맞으면 상태를 부르고 성공

		FUNCTION_NODE Condition_CountSpin
			= FUNCTION_NODE_MAKE
		{
			if (m_iSpinRush_Count == SPINATTACKCOUNT)
			{
				m_iSpinRush_Count = 0;
				return BT_STATUS::Failure;
			}

			return BT_STATUS::Success;
		}; // 스핀 3번 사용하면 원거리 대체

		FUNCTION_NODE Call_SpinRush
			= FUNCTION_NODE_MAKE
		{
			m_iSpinRush_Count += 1; // 스팬 갯수 업
			m_iPatternCount += 1; // 카운트까지 패턴 갯수 업
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_SpinRush>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Coutinuity
			= FUNCTION_NODE_MAKE
		{
			if (m_iLong_Distance_Attack_Count == LONGATTACKCOUNT)
			{
				m_iLong_Distance_Attack_Count = 0;
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		};

		FUNCTION_NODE Call_Walk
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_Walk>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Inferno
			= FUNCTION_NODE_MAKE
		{
			CBahamut::PHASE eCurPhase = static_pointer_cast<CBahamut>(m_pActor.lock())->Get_CurPhase();
			if ((eCurPhase == CBahamut::PHASE::PHASE3) || (eCurPhase == CBahamut::PHASE::PHASE4) ||
			(eCurPhase == CBahamut::PHASE::PHASE5))
			{
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 인페르노는 조건 페이스가 맞아야 사용가능

		FUNCTION_NODE Condition_Call_Inferno
			= FUNCTION_NODE_MAKE
		{
			if (m_iRandomNum == 0)
			{
				m_iLong_Distance_Attack_Count += 1; // 원거리 갯수 업
				m_iPatternCount += 1; // 카운트까지 패턴 갯수 업
				m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_Inferno>();
				return BT_STATUS::Success;
			}
			
			return BT_STATUS::Failure;
		}; // 랜덤 숫자가 맞으면 상태를 부르면서 성공

		FUNCTION_NODE Condition_Call_DarkClaw
			= FUNCTION_NODE_MAKE
		{
			if (m_iRandomNum == 0)
			{
				m_iLong_Distance_Attack_Count += 1; // 원거리 갯수 업
				m_iPatternCount += 1; // 카운트까지 패턴 갯수 업
				m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_DarkClaw>();
				return BT_STATUS::Success;
			}
			
			return BT_STATUS::Failure;
		};// 랜덤 숫자가 맞으면 상태를 부르면서 성공

		FUNCTION_NODE Condition_Call_HeavyStrike
			= FUNCTION_NODE_MAKE
		{
			if (m_iRandomNum == 1)
			{
				m_iLong_Distance_Attack_Count += 1; // 원거리 갯수 업
				m_iPatternCount += 1; // 카운트까지 패턴 갯수 업
				m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_HeavyStrike>();
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // 랜덤 숫자가 맞으면 상태를 부르면서 성공

		FUNCTION_NODE Random
			= FUNCTION_NODE_MAKE
		{
			m_iRandomNum = m_pGameInstance->RandomInt(0,1);

			return BT_STATUS::Success;
		}; // 랜덤 숫자만 돌림 무조건 성공

		m_pBehaviorTree = Builder()
			.composite<Selector>()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Dead) // 죽음 판단
					.leaf<FunctionNode>(Call_Dead)
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_MegaFlare) // 궁극기 판단
					.leaf<FunctionNode>(Call_MegaFlare)
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Count) // 카운트 상태 들입 판단
					.leaf<FunctionNode>(Call_Count)
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Special) // 준궁극기 판단
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Grab) // 그랩 공격 판단
							.leaf<FunctionNode>(Call_Grab)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_AerialRave) // 돌진 공격 판단
							.leaf<FunctionNode>(Call_AerialRave)
						.end()
					.end()
				.end()
				.composite<Selector>()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_Middle_Distance) // 중거리 판단
						.composite<Selector>()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Close_Distance) // 근거리 판단
								.leaf<FunctionNode>(Condition_CountMelee) // 근거리 공격 횟수 판단.
								.leaf<FunctionNode>(Random) // 랜덤 숫자 돌리기
								.composite<Selector>()
									.leaf<FunctionNode>(Condition_Call_Clorush) // 랜덤 숫자에 따른 판단
									.leaf<FunctionNode>(Condition_Call_FlareBreath) // 랜덤 숫자에 따른 판단
								.end()
							.end()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_CountSpin) // 스핀 횟수가 3회를 넘었는지 판단
								.leaf<FunctionNode>(Call_SpinRush)
							.end()
						.end()
					.end()
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Coutinuity) // 원거리 연속 횟수 판단
							.leaf<FunctionNode>(Call_Walk)
						.end()
						.composite<Selector>()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Inferno) // 페이즈에 따른 인페르노 판단
								.leaf<FunctionNode>(Random) // 랜덤 숫자 돌리기
								.composite<Selector>()
									.leaf<FunctionNode>(Condition_Call_Inferno) // 랜덤 숫자에 따른 판단
									.leaf<FunctionNode>(Condition_Call_HeavyStrike) // 랜덤 숫자에 따른 판단
								.end()
							.end()
							.composite<Sequence>()
								.leaf<FunctionNode>(Random) // 랜덤 숫자 돌리기
								.composite<Selector>() 
									.leaf<FunctionNode>(Condition_Call_DarkClaw)// 랜덤 숫자에 따른 판단
									.leaf<FunctionNode>(Condition_Call_HeavyStrike) // 랜덤 숫자에 따른 판단
								.end()
							.end()
						.end()
					.end()
				.end()
			.end()
		.build();
	}
	return S_OK;
}

void CState_Bahamut_IDLE::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Bahamut_IDLE::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

}

void CState_Bahamut_IDLE::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_IDLE::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
}

bool CState_Bahamut_IDLE::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_IDLE::Free()
{
}
