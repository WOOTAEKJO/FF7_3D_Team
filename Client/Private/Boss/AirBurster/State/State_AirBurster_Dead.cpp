#include "stdafx.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_Dead.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"

#include "GameInstance.h"
#include "UI_Manager.h"

#include "Camera_Manager.h"

CState_AirBurster_Dead::CState_AirBurster_Dead(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Dead::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Die01_0", 1.f, false);

	if (m_bCheck)
	{
		m_bCheck = false;
		m_pActor.lock()->Get_PhysXColliderCom().lock()->PutToSleep();
		// 콜라이더 off
	}

	m_pActor.lock()->TurnOn_State(OBJSTATE::DeadAnim);

	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(0, 0, "AirBurster");
	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(1, 27);
	GET_SINGLE(CUI_Manager)->Make_EventLogPopUp(2, 44);

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Airburster_Dead_Following", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE, m_pActor.lock());

	m_pGameInstance->Play_BGM(TEXT("FF7_BGM"), TEXT("bgm_ungd6_03_field#1 (Music-Section0).mp3"), 1.f);

	m_pActor.lock()->TurnOn_State(OBJSTATE::DeadAnim);
	return S_OK;
}

void CState_AirBurster_Dead::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

}

void CState_AirBurster_Dead::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_Dead::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Dead::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_AirBurster_Dead::isValid_NextState(CState* state)
{
	return true;

}

void CState_AirBurster_Dead::Free()
{
}
