#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_Walk.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"

CState_Bahamut_Walk::CState_Bahamut_Walk(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
}

HRESULT CState_Bahamut_Walk::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_0", 1.f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_NOR_ATTACK);

	if (!m_pBehaviorTree)
	{

		FUNCTION_NODE Call_Spine
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_SpinRush>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Selector>()
				.leaf<FunctionNode>(Call_Spine)
			.end()
		.build();
	}
	return S_OK;
}

void CState_Bahamut_Walk::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if ((m_iFrame == 0) && m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_iFrame = 1;
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_1", 1.f * BAHAMUTANIMSPEED, true);
	}

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Bahamut_Walk::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if (m_iFrame == 1)
	{
		auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);

		if (!m_pActor_TransformCom.lock()->Rotate_On_BaseDir(fTimeDelta, Get_CurPlayer_Y_Mediate_Dir()))
		{
			if (!m_pActor_TransformCom.lock()->Go_Target(pPlayerInfo.vTargetPos, fTimeDelta * 1.5f, MIDDLEDISTANCE))
			{
				m_iFrame = 2;
				m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_2", 1.f * BAHAMUTANIMSPEED, false);
			}
		}
	}
}

void CState_Bahamut_Walk::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_Walk::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);

	m_iFrame = 0;
}

bool CState_Bahamut_Walk::isValid_NextState(CState* state)
{
	if ((m_iFrame == 2) && m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_Bahamut_Walk::Free()
{
}
