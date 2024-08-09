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
		cout << "���Ϲ�Ʈ Ÿ�ٰ��� �Ÿ� : " + to_string(pPlayerInfo.fDistance) << endl;
	CBahamut::PHASE eCurPhase = static_pointer_cast<CBahamut>(m_pActor.lock())->Get_CurPhase();
	cout << "���Ϲ�Ʈ " + to_string((_int)eCurPhase) + "���̽�" << endl;
	cout << "���Ϲ�Ʈ ī��Ʈ" + to_string(m_iUltimateCount) << endl;
	cout << "���Ϲ�Ʈ ���� ����" + to_string(m_iPatternCount) << endl;

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
		}; // ���� �Ǵ�

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
		}; // �ñر� �Ǵ�

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
				m_iPatternCount = 0; // ī��Ʈ���� ���� ���� �ʱ�ȭ
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // ī��Ʈ ���� ������ �Ǵ�

		FUNCTION_NODE Call_Count
			= FUNCTION_NODE_MAKE
		{
			m_iUltimateCount += 1; // ī��Ʈ ���� ��
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
		}; // ����� ���� �Ǵ�

		FUNCTION_NODE Condition_Grab
			= FUNCTION_NODE_MAKE
		{
			CBahamut::PHASE eCurPhase = static_pointer_cast<CBahamut>(m_pActor.lock())->Get_CurPhase();
			if ((eCurPhase == CBahamut::PHASE::PHASE0)||(eCurPhase == CBahamut::PHASE::PHASE1) || (eCurPhase == CBahamut::PHASE::PHASE2))
			{
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // ����� ���� �Ǵܿ��� �������� ��, ���̽��� ���ǿ� ������ ��� ����

		FUNCTION_NODE Call_Grab
			= FUNCTION_NODE_MAKE
		{
			m_iPatternCount += 1; // ī��Ʈ���� ���� ���� ��
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
		}; // ����� ���� �Ǵ� ������, ���̽� ���ǿ� ������ ���� ��

		FUNCTION_NODE Call_AerialRave
			= FUNCTION_NODE_MAKE
		{
			m_iPatternCount += 1; // ī��Ʈ���� ���� ���� ��
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
		}; // �߰��Ÿ� �̳��� �ִ��� �Ǵ�

		FUNCTION_NODE Condition_Close_Distance
			= FUNCTION_NODE_MAKE
		{
			auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);

			if (pPlayerInfo.fDistance < CLOSEDISTANCE)
			{
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // �����Ÿ� �̳��� �ִ��� �Ǵ�

		FUNCTION_NODE Condition_CountMelee
			= FUNCTION_NODE_MAKE
		{
			if (m_iMelee_Count == MELEEATTACKCOUNT)
			{
				m_iMelee_Count = 0;
				return BT_STATUS::Failure;
			}

			return BT_STATUS::Success;
		}; // �����Ÿ� �̳��� �ִ��� �Ǵ�

		FUNCTION_NODE Condition_Call_Clorush
			= FUNCTION_NODE_MAKE
		{
			if (m_iRandomNum == 0)
			{
				m_iMelee_Count += 1; // �C�� ���� ��
				m_iPatternCount += 1; // ī��Ʈ���� ���� ���� ��
				m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_Clorush>();
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // ���� ���ڰ� ������ ���¸� �θ��� ����

		FUNCTION_NODE Condition_Call_FlareBreath
			= FUNCTION_NODE_MAKE
		{
			if (m_iRandomNum == 1)
			{
				m_iMelee_Count += 1; // ���� ���� ��
				m_iPatternCount += 1; // ī��Ʈ���� ���� ���� ��
				m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_FlareBreath>();
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		};// ���� ���ڰ� ������ ���¸� �θ��� ����

		FUNCTION_NODE Condition_CountSpin
			= FUNCTION_NODE_MAKE
		{
			if (m_iSpinRush_Count == SPINATTACKCOUNT)
			{
				m_iSpinRush_Count = 0;
				return BT_STATUS::Failure;
			}

			return BT_STATUS::Success;
		}; // ���� 3�� ����ϸ� ���Ÿ� ��ü

		FUNCTION_NODE Call_SpinRush
			= FUNCTION_NODE_MAKE
		{
			m_iSpinRush_Count += 1; // ���� ���� ��
			m_iPatternCount += 1; // ī��Ʈ���� ���� ���� ��
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
		}; // ���丣��� ���� ���̽��� �¾ƾ� ��밡��

		FUNCTION_NODE Condition_Call_Inferno
			= FUNCTION_NODE_MAKE
		{
			if (m_iRandomNum == 0)
			{
				m_iLong_Distance_Attack_Count += 1; // ���Ÿ� ���� ��
				m_iPatternCount += 1; // ī��Ʈ���� ���� ���� ��
				m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_Inferno>();
				return BT_STATUS::Success;
			}
			
			return BT_STATUS::Failure;
		}; // ���� ���ڰ� ������ ���¸� �θ��鼭 ����

		FUNCTION_NODE Condition_Call_DarkClaw
			= FUNCTION_NODE_MAKE
		{
			if (m_iRandomNum == 0)
			{
				m_iLong_Distance_Attack_Count += 1; // ���Ÿ� ���� ��
				m_iPatternCount += 1; // ī��Ʈ���� ���� ���� ��
				m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_DarkClaw>();
				return BT_STATUS::Success;
			}
			
			return BT_STATUS::Failure;
		};// ���� ���ڰ� ������ ���¸� �θ��鼭 ����

		FUNCTION_NODE Condition_Call_HeavyStrike
			= FUNCTION_NODE_MAKE
		{
			if (m_iRandomNum == 1)
			{
				m_iLong_Distance_Attack_Count += 1; // ���Ÿ� ���� ��
				m_iPatternCount += 1; // ī��Ʈ���� ���� ���� ��
				m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_HeavyStrike>();
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		}; // ���� ���ڰ� ������ ���¸� �θ��鼭 ����

		FUNCTION_NODE Random
			= FUNCTION_NODE_MAKE
		{
			m_iRandomNum = m_pGameInstance->RandomInt(0,1);

			return BT_STATUS::Success;
		}; // ���� ���ڸ� ���� ������ ����

		m_pBehaviorTree = Builder()
			.composite<Selector>()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Dead) // ���� �Ǵ�
					.leaf<FunctionNode>(Call_Dead)
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_MegaFlare) // �ñر� �Ǵ�
					.leaf<FunctionNode>(Call_MegaFlare)
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Count) // ī��Ʈ ���� ���� �Ǵ�
					.leaf<FunctionNode>(Call_Count)
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Special) // �رñر� �Ǵ�
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Grab) // �׷� ���� �Ǵ�
							.leaf<FunctionNode>(Call_Grab)
						.end()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_AerialRave) // ���� ���� �Ǵ�
							.leaf<FunctionNode>(Call_AerialRave)
						.end()
					.end()
				.end()
				.composite<Selector>()
					.composite<Sequence>()
						.leaf<FunctionNode>(Condition_Middle_Distance) // �߰Ÿ� �Ǵ�
						.composite<Selector>()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Close_Distance) // �ٰŸ� �Ǵ�
								.leaf<FunctionNode>(Condition_CountMelee) // �ٰŸ� ���� Ƚ�� �Ǵ�.
								.leaf<FunctionNode>(Random) // ���� ���� ������
								.composite<Selector>()
									.leaf<FunctionNode>(Condition_Call_Clorush) // ���� ���ڿ� ���� �Ǵ�
									.leaf<FunctionNode>(Condition_Call_FlareBreath) // ���� ���ڿ� ���� �Ǵ�
								.end()
							.end()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_CountSpin) // ���� Ƚ���� 3ȸ�� �Ѿ����� �Ǵ�
								.leaf<FunctionNode>(Call_SpinRush)
							.end()
						.end()
					.end()
					.composite<Selector>()
						.composite<Sequence>()
							.leaf<FunctionNode>(Condition_Coutinuity) // ���Ÿ� ���� Ƚ�� �Ǵ�
							.leaf<FunctionNode>(Call_Walk)
						.end()
						.composite<Selector>()
							.composite<Sequence>()
								.leaf<FunctionNode>(Condition_Inferno) // ����� ���� ���丣�� �Ǵ�
								.leaf<FunctionNode>(Random) // ���� ���� ������
								.composite<Selector>()
									.leaf<FunctionNode>(Condition_Call_Inferno) // ���� ���ڿ� ���� �Ǵ�
									.leaf<FunctionNode>(Condition_Call_HeavyStrike) // ���� ���ڿ� ���� �Ǵ�
								.end()
							.end()
							.composite<Sequence>()
								.leaf<FunctionNode>(Random) // ���� ���� ������
								.composite<Selector>() 
									.leaf<FunctionNode>(Condition_Call_DarkClaw)// ���� ���ڿ� ���� �Ǵ�
									.leaf<FunctionNode>(Condition_Call_HeavyStrike) // ���� ���ڿ� ���� �Ǵ�
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
