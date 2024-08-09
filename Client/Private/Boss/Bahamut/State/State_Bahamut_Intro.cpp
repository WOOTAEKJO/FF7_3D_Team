#include "stdafx.h"
#include "Boss/Bahamut/State/State_Bahamut_Intro.h"
#include "Boss/Bahamut/State_List_Bahamut.h"
#include "Boss/Bahamut/Bahamut.h"

#include "GameInstance.h"
#include "UI_Manager.h"
#include "Sound_Manager.h"


CState_Bahamut_Intro::CState_Bahamut_Intro(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_Bahamut(pActor, pStatemachine)
{
	m_TimeChecker = FTimeChecker(3.f);
}

HRESULT CState_Bahamut_Intro::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1", 1.f, false,
		dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Get_Transition());

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Bahamut_Intro_Following", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE,m_pActor.lock());

	m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"001_SE_Bahamut (B_Appear01_001).wav", ESoundGroup::Narration, 1.f);

	return S_OK;
}

void CState_Bahamut_Intro::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

}

void CState_Bahamut_Intro::Tick(_cref_time fTimeDelta)
{	
	__super::Tick(fTimeDelta);

	if (m_TimeChecker.Update(fTimeDelta))
		m_pStateMachineCom.lock()->Set_State<CState_Bahamut_IDLE>();
}

void CState_Bahamut_Intro::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CState_Bahamut_Intro::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CBahamut>(m_pActor.lock())->Set_Transition(true);
	m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(true);

	GET_SINGLE(CUI_Manager)->Make_GamePlay_UI_Object(TEXT("Prototype_GameObject_UI_MapTitle"), nullptr, nullptr, nullptr, "MapTitle_B2");
	GET_SINGLE(CUI_Manager)->Call_Dialog("U_Dialog_BS_D0_");
}

bool CState_Bahamut_Intro::isValid_NextState(CState* state)
{
	return true;
}

void CState_Bahamut_Intro::Free()
{
}
