#include "stdafx.h"
#include "../Public/Boss/AirBurster/State/State_AirBurster_Docking.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"

#include "GameInstance.h"
#include "StatusComp.h"

#include "Camera_Manager.h"

CState_AirBurster_Docking::CState_AirBurster_Docking(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Docking::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_strCurAnimTag = "Main|B_AtkRocket02";
	m_pActor_ModelCom.lock()->Set_Animation(m_strCurAnimTag, 1.f, false);

	SetUp_AI_Action_Num(AI_ACTION_NON_TARGET);

	if (!m_pBehaviorTree)
	{
		static_pointer_cast<CAirBurster>(m_pActor.lock())->Ready_Docking();
		// 도킹 준비 완료

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Call_IDLE)
			.end()
			.build();
	}
	
	GET_SINGLE(CCamera_Manager)->Call_CutSceneCamera("Airburster_Docking_Following", (_int)CCamera::tagCameraMotionData::INTERPOLATION_TYPE::INTER_NONE, m_pActor.lock());

	return S_OK;
}

void CState_AirBurster_Docking::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if ((m_iFrame == 0) && m_pActor_ModelCom.lock()->IsAnimation_Finished(m_strCurAnimTag))
	{
		m_iFrame += 1;
		m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkRocket02_1", 1.f * AIRBURSTERANIMSPEED, true);
	}

	else if (m_iFrame == 1)
	{
		_int iNumType = -1;
		if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Judge_Combination(&iNumType))
		{
			if (iNumType == 0)
			{
				if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_SeparateArm(TEXT("Part_LeftArm")))
				{
					m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkRocketL02_2", 1.f * AIRBURSTERANIMSPEED, false);
					static_pointer_cast<CAirBurster>(m_pActor.lock())->Set_SeparateArm(TEXT("Part_LeftArm"), false);
					// 왼팔 도킹 -> 분리 false
				}
			}

			if (iNumType == 1)
			{
				if (static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_SeparateArm(TEXT("Part_RightArm")))
				{
					m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkRocketR02_2", 1.f * AIRBURSTERANIMSPEED, false);
					static_pointer_cast<CAirBurster>(m_pActor.lock())->Set_SeparateArm(TEXT("Part_RightArm"), false);
					// 오른팔 도킹 -> 분리 false
				}
			}
		}
	}

	if (!static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_SeparateArm(TEXT("Part_RightArm"))
		&& !static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_SeparateArm(TEXT("Part_LeftArm"))
		&& m_pActor_ModelCom.lock()->IsAnimation_Finished())
	{
		m_bFinished = true;
		// 두 팔이 모두 결합되고 애니메이션이 끝나면 다음 상태로

		auto pStatusCom = DynPtrCast<IStatusInterface>(m_pActor.lock())->Get_StatusCom().lock();
		pStatusCom->Set_IsInvincible(false);
	}

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Docking::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_Docking::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Docking::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	m_pActor.lock()->Get_PhysXColliderCom().lock()->WakeUp();
	// 콜라이더 On
}

bool CState_AirBurster_Docking::isValid_NextState(CState* state)
{
	if (m_bFinished)
		return true;
	else
		return false;
}

void CState_AirBurster_Docking::Free()
{
}
