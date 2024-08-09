#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Dead.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_Parts.h"
#include "Player.h"

#include "GameInstance.h"

CState_AirBurster_Arm_Dead::CState_AirBurster_Arm_Dead(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Arm_Dead::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_DmgBurst01_0", 1.f, false);

	if (!m_pBehaviorTree)
	{
		m_pActor.lock()->Get_PhysXColliderCom().lock()->PutToSleep();
		// 콜라이더 off

		if (Compare_Wstr(m_pActor.lock()->Get_PrototypeTag(), TEXT("Prototype_GameObject_AirBurster_LeftArm")))
		{
			m_pActor_ModelCom.lock()->Set_PreRotate(
				XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));

			m_pActor_ModelCom.lock()->Set_PreRotate(
				XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)));
		}
		else if (Compare_Wstr(m_pActor.lock()->Get_PrototypeTag(), TEXT("Prototype_GameObject_AirBurster_RightArm")))
		{
			m_pActor_ModelCom.lock()->Set_PreRotate(
				XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f)));
			m_pActor_ModelCom.lock()->Set_PreRotate(
				XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.f)));
			m_pActor_ModelCom.lock()->Set_PreRotate(
				XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
		}

		FUNCTION_NODE Condition_Docking
			= FUNCTION_NODE_MAKE
		{
			if (static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Get_ReadyDocking())
			{
				return BT_STATUS::Success;
			}

		return BT_STATUS::Failure;
		}; // 도킹 준비 상태인지

		FUNCTION_NODE Call_Docking
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Arm_Docking>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Condition_Docking)
				.leaf<FunctionNode>(Call_Docking)
			.end()
		.build();
	}

	GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"AirBurster_Dead_Explosion", m_pActor.lock(),CEffect::USE_FOLLOW_PARTS);

	m_pActor.lock()->TurnOn_State(OBJSTATE::DeadAnim);
	return S_OK;
}

void CState_AirBurster_Arm_Dead::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Arm_Dead::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_AirBurster_Arm_Dead::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Arm_Dead::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);

	if (Compare_Wstr(m_pActor.lock()->Get_PrototypeTag(), TEXT("Prototype_GameObject_AirBurster_LeftArm")))
	{
		m_pActor_ModelCom.lock()->Set_PreRotate(
			XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
		m_pActor_ModelCom.lock()->Set_PreRotate(
			XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)));
	}
	else if (Compare_Wstr(m_pActor.lock()->Get_PrototypeTag(), TEXT("Prototype_GameObject_AirBurster_RightArm")))
	{

		m_pActor_ModelCom.lock()->Set_PreRotate(
			XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f)));
		m_pActor_ModelCom.lock()->Set_PreRotate(
			XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-180.f)));
		m_pActor_ModelCom.lock()->Set_PreRotate(
			XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f)));
	}

}

bool CState_AirBurster_Arm_Dead::isValid_NextState(CState* state)
{
	return true;
}

void CState_AirBurster_Arm_Dead::Free()
{
}
