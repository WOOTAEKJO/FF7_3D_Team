#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut.h"

#include "StatusComp.h"

#include "Client_Manager.h"

CState_Bahamut::CState_Bahamut(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Boss(pActor, pStatemachine)
{
}

HRESULT CState_Bahamut::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	return S_OK;
}

void CState_Bahamut::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CState_Bahamut::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Bahamut::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Bahamut::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Bahamut::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut::SetUp_AI_Action_Num(_int iNum)
{
	DynPtrCast<IStatusInterface>(m_pActor.lock())->Get_StatusCom().lock()->Update_ActionPower(iNum);
}

void CState_Bahamut::Free()
{
}
