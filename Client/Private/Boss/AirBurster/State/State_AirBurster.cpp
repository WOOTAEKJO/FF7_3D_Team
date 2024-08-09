#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster.h"

#include "Player.h"
#include "StatusComp.h"

#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

CState_AirBurster::CState_AirBurster(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Boss(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	return S_OK;
}

void CState_AirBurster::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CState_AirBurster::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CState_AirBurster::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_AirBurster::isValid_NextState(CState* state)
{
	return true;
}

void CState_AirBurster::SetUp_AI_Action_Num(_int iNum)
{
	_int iActionPower = iNum;

	auto pAirBurster = DynPtrCast<CAirBurster>(m_pActor.lock());
	// 에어버스터 본체만 상관 있음. 파츠 상관x

	if (pAirBurster)
	{
		if (pAirBurster->Is_All_SeparateArm())
			iNum = 20; // 분리 상태이면 본체는 무적상태.
	}

	DynPtrCast<IStatusInterface>(m_pActor.lock())->Get_StatusCom().lock()->Update_ActionPower(iNum);
}

void CState_AirBurster::Free()
{
}
