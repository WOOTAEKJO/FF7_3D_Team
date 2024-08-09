#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_Dead.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"
#include "UI_Manager.h"


CState_Bahamut_Dead::CState_Bahamut_Dead(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(1.f);
}

HRESULT CState_Bahamut_Dead::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Die01_0", 1.f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	if (!m_pBehaviorTree)
	{
		
	}


	m_pActor.lock()->TurnOn_State(OBJSTATE::DeadAnim);

	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(0, 0, "Bahamut");
	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(1, 34);
	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(2, 58);

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Bahamut_Dead_Following", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE, m_pActor.lock());

	return S_OK;
}

void CState_Bahamut_Dead::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if ((m_iFrame == 0) && m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_iFrame = 1;
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_Die01_1", 1.f, false);
	}
}

void CState_Bahamut_Dead::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if ((m_iFrame == 1) && m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		dynamic_pointer_cast<CCharacter>(m_pActor.lock())->Dead_DissolveType(L"EM_MonsterDead_Num5000");
	}

}

void CState_Bahamut_Dead::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CState_Bahamut_Dead::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);
}

bool CState_Bahamut_Dead::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_Dead::Free()
{
}
