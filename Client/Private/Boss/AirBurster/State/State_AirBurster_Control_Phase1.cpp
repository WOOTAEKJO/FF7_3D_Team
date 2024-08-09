#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_Control_Phase1.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "GameInstance.h"

CState_AirBurster_Control_Phase1::CState_AirBurster_Control_Phase1(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Control_Phase1::Initialize_State(CState* pPreviousState)
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

		FUNCTION_NODE Call_Phase1Intro
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_1PhaseEnter>();
		// 1Phase Intro ��ȯ
		return BT_STATUS::Success;
		};

		FUNCTION_NODE Judge_Phase
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_ChangePhase())
			{
				if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_CurPhase() ==
					CAirBurster::PHASE2)
				{
					return BT_STATUS::Success;
				}
			}

			return BT_STATUS::Failure;
		};

		FUNCTION_NODE Call_Phase2
			= FUNCTION_NODE_MAKE
		{

			m_pStateMachineCom.lock()->Set_State<CState_AirBurster_Control_Phase2>();
			// 2Phase ���� ��ȯ

			if (m_bEMField && (m_pEMField != nullptr))
			{
				m_bEMField = false;
				m_pEMField->Set_Dead();
			}// 2������� ��ȯ�� ��, ���ڱ��İ� �����ִ� ��� ����ó��

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_Turn
			= FUNCTION_NODE_MAKE
		{
			if (m_iTurnCount == TURNCOUNT)
			{
				m_iTurnCount = 0;
				return BT_STATUS::Success;
			}

			return BT_STATUS::Failure;
		};

		FUNCTION_NODE Call_Turn
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Turn180>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Condition_EMField
			= FUNCTION_NODE_MAKE
		{
			if (!m_bEMField && (m_pEMField==nullptr) && (m_iPatternCount == EMFIELDCOUNT))
			{
				m_bEMField = true;
				m_iPatternCount = 0;
				m_iTurnCount += 1;	// turn ī��Ʈ ����
				return BT_STATUS::Success;
				// ���ڱ��İ� ������ ����
			}
			else if (m_bEMField && (m_pEMField != nullptr) && (m_iPatternCount == EMFIELDCOUNT))
			{
				m_bEMField = false;
				m_iPatternCount = 0;

				m_pEMField->Set_Dead();
				m_pEMField = nullptr;
				// ���ڱ��İ� ������ ����
			}

			return BT_STATUS::Failure;
		};

		FUNCTION_NODE Call_EMField
			= FUNCTION_NODE_MAKE
		{

			m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
				TEXT("Prototype_GameObject_AirBurster_EMField"), nullptr, &m_pEMField);
			m_pEMField->Set_Owner(m_pActor);
			m_pEMField->Get_TransformCom().lock()->Set_Position(fTimeDelta,
			m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION));
			// ���ڱ��� ����

			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_EMField>();
			// ���ڱ��� ��ȯ
			return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_FrontMachineGun
			= FUNCTION_NODE_MAKE
		{
			m_iTurnCount += 1;	// turn ī��Ʈ ����
			m_iPatternCount += 1; // ī��Ʈ ����
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_FrontMachineGun>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_FingerBeam
			= FUNCTION_NODE_MAKE
		{
			m_iTurnCount += 1;	// turn ī��Ʈ ����
			m_iPatternCount += 1; // ī��Ʈ ����
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_FingerBeam>();

			return BT_STATUS::Success;
		};

		FUNCTION_NODE Call_EnergyBall
			= FUNCTION_NODE_MAKE
		{
			m_iTurnCount += 1;	// turn ī��Ʈ ����
			m_iPatternCount += 1; // ī��Ʈ ����
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
					.leaf<FunctionNode>(Condition_Intro)
					.leaf<FunctionNode>(Call_Phase1Intro) // ��Ʈ�� ����
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Judge_Phase)
					.leaf<FunctionNode>(Call_Phase2) // ������ ��ȯ ����
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_Turn)
					.leaf<FunctionNode>(Call_Turn) // ��
				.end()
				.composite<Sequence>()
					.leaf<FunctionNode>(Condition_EMField)
					.leaf<FunctionNode>(Call_EMField) // ���ڱ���
				.end()
				.composite<StatefulSelector>()
					.leaf<FunctionNode>(Call_FrontMachineGun)
					.decorator<Repeater>(50)
						.leaf<FunctionNode>(Call_None) // ������
					.end()
					.leaf<FunctionNode>(Call_FingerBeam)
					.decorator<Repeater>(50)
						.leaf<FunctionNode>(Call_None) // ������
					.end()
					.leaf<FunctionNode>(Call_EnergyBall)
					.decorator<Repeater>(50)
						.leaf<FunctionNode>(Call_None) // ������
					.end()
				.end()
			.end()
		.build();
	}
	return S_OK;
}

void CState_AirBurster_Control_Phase1::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Control_Phase1::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CState_AirBurster_Control_Phase1::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Control_Phase1::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Set_Transition(true);
}

bool CState_AirBurster_Control_Phase1::isValid_NextState(CState* state)
{
	return true;
}

void CState_AirBurster_Control_Phase1::Free()
{
}
