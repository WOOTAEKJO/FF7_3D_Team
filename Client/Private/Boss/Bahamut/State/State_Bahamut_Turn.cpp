#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_Turn.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"

CState_Bahamut_Turn::CState_Bahamut_Turn(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
}

HRESULT CState_Bahamut_Turn::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_0", 1.f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	if (!m_pBehaviorTree)
	{

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_Bahamut_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Selector>()
				.leaf<FunctionNode>(Call_IDLE)
			.end()
		.build();
	}
	return S_OK;
}

void CState_Bahamut_Turn::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_Bahamut_Turn::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if ((m_iFrame == 0) && m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_iFrame = 1;
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_1", 1.f, true);
	}

	if (m_iFrame == 1)
	{
		auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);
		
		if (!m_pActor_TransformCom.lock()->Rotate_On_BaseDir(fTimeDelta, pPlayerInfo.vDirToTarget))
		{
			if (!m_pActor_TransformCom.lock()->Go_Target(pPlayerInfo.vTargetPos, fTimeDelta,3.f))
			{
				m_iFrame = 2;
				m_pActor_ModelCom.lock()->Set_Animation("Main|B_Walk01_2", 1.f, false);
			}
		}

	}
	
}

void CState_Bahamut_Turn::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut_Turn::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);

	m_iFrame = 0;
}

bool CState_Bahamut_Turn::isValid_NextState(CState* state)
{
	if ((m_iFrame == 2) && m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;
}

void CState_Bahamut_Turn::Free()
{
}
