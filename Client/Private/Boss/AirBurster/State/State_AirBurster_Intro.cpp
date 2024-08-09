#include "stdafx.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_Intro.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"

#include "GameInstance.h"
#include "Camera_ThirdPerson.h"

#include "Camera_Manager.h"

CState_AirBurster_Intro::CState_AirBurster_Intro(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{

}

HRESULT CState_AirBurster_Intro::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_DmgDown02_2", 1.f * AIRBURSTERANIMSPEED, false);

	if (!m_pBehaviorTree)
	{
		m_vTargetPos = static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_OriginPos();

		m_pActor.lock()->Get_PhysXColliderCom().lock()->PutToSleep();
		// 콜라이더 off

		m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(false); //		중력 관리

		FUNCTION_NODE Call_Phase1
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Wait_State<CState_AirBurster_Control_Phase1>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Selector>()
				.leaf<FunctionNode>(Call_Phase1)
			.end()
			.build();
	}


	m_pGameInstance->Play_BGM(TEXT("FF7_BGM"), TEXT("bgm_mako5_06#4 (Music-Section3).mp3"), 1.f);

	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Airburster_Intro", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE);



	return S_OK;
}

void CState_AirBurster_Intro::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Intro::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);

	if (!XMVector3NearEqual(vPos, m_vTargetPos, XMVectorSet(0.15f, 100.f, 0.15f, 0.f)))
	{
		m_pActor_TransformCom.lock()->Look_At_OnLand(m_vTargetPos);
		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * 15.f);
	}
	else {
		if (!m_pActor.lock()->Get_PhysXControllerCom().lock()->Is_Gravity())
		 {
			vPos.m128_f32[0] = m_vTargetPos.x;
			vPos.m128_f32[2] = m_vTargetPos.z;
			m_pActor.lock()->Get_PhysXControllerCom().lock()->Enable_Gravity(true);

		 }
			
	}
}

void CState_AirBurster_Intro::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Intro::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Set_Transition(false);

	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(CLOUD).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_AIRBURSTER);
	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(AERITH).lock()->Set_OffsetSetType(CCamera_ThirdPerson::OFFSET_AIRBURSTER);
}

bool CState_AirBurster_Intro::isValid_NextState(CState* state)
{
	if (m_pActor.lock()->Get_PhysXControllerCom().lock()->Is_Ground())
	{

		GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(TEXT("AirBurster_Fall1"), m_pActor.lock());

		Camera_Shaking("Airburster_Fall_After", true);

		return true;
	}
	else
		return false;
}

void CState_AirBurster_Intro::Free()
{
}
