#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Push.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_Parts.h"
#include "Player.h"

#include "Client_Manager.h"

#include "GameInstance.h"

CState_AirBurster_Arm_Push::CState_AirBurster_Arm_Push(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Arm_Push::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle01_1", 1.f, false);

	SetUp_AI_Action_Num(AI_ACTION_NON_TARGET);

	if (!m_pBehaviorTree)
	{
		m_vOriginPos = DynPtrCast<CAirBurster>(m_pActor.lock()->Get_Owner().lock())->Get_OriginPos();

		FUNCTION_NODE Call_IDLE
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Arm_IDLE>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
			.leaf<FunctionNode>(Call_IDLE)
			.end()
			.build();
	}

	m_vStartPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
	//시작 위치 기억

	_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
	_vector vOriginTemp = m_vOriginPos;

	vOriginTemp.m128_f32[1] = vPos.m128_f32[1] - 0.5f;

	_vector vLook = XMVector3Normalize(vOriginTemp - vPos);
	m_pActor_TransformCom.lock()->Set_Look_Manual(vLook);

	return S_OK;
}

void CState_AirBurster_Arm_Push::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Arm_Push::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
	_vector vLook = XMVector3Normalize(m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK));
	_vector vTargetPos = m_vOriginPos + (-vLook * 3.f);

	_float fSpeed = fTimeDelta * 1.5f; // 타겟에 가까울 수록 스피드가 올라감

	_float fSpeedDelta = m_fAcceleration * fTimeDelta; // 속도 변화량

	if (!XMVector3NearEqual(vPos, vTargetPos, XMVectorSet(0.2f, 100.f, 0.2f, 0.f)))
	{
		_float3 vAddPos;

		fSpeed += fSpeedDelta;
		fSpeed = min(max(fSpeed, m_fMinSpeed), m_fMaxSpeed); //최소 속도와 최고 속도를 정함.

		m_pActor_TransformCom.lock()->Go_Straight(fTimeDelta * fSpeed,&vAddPos);
		DynPtrCast<CAirBurster_Parts>(m_pActor.lock())->Set_Movement_Amount(vAddPos);
		// 이동량 기록
	}
	else {
		m_bArrive = true;
	}
}

void CState_AirBurster_Arm_Push::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Arm_Push::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	DynPtrCast<CAirBurster_Parts>(m_pActor.lock())->Set_Movement_Amount(_float3(0.f,0.f,0.f));
}

bool CState_AirBurster_Arm_Push::isValid_NextState(CState* state)
{
	if (m_bArrive)
		return true;
	else
		return false;
}

void CState_AirBurster_Arm_Push::Free()
{
}
