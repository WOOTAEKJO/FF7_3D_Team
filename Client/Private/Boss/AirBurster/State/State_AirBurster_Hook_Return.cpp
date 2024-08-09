#include "stdafx.h"
#include "Boss/AirBurster/State/State_AirBurster_Hook_Return.h"
#include "Boss/AirBurster/State_List_AirBurster.h"
#include "Boss/AirBurster/AirBurster.h"

#include "Boss/AirBurster/Weapon/AirBurster_ShoulderBeam.h"

#include "GameInstance.h"

#include "Player.h"
#include "State_List_Cloud.h"
#include "Aerith/State/State_Aerith_Abnormal.h"
#include "Aerith/State/State_Aerith_Idle.h"

CState_AirBurster_Hook_Return::CState_AirBurster_Hook_Return(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Hook_Return::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_Idle02_1", 1.5f * AIRBURSTERANIMSPEED, false,
		dynamic_pointer_cast<CAirBurster>(m_pActor.lock())->Get_Transition());

	SetUp_AI_Action_Num(AI_ACTION_NOR_ATTACK);

	if (!m_pBehaviorTree)
	{
		m_vTargetPos = static_pointer_cast<CAirBurster>(m_pActor.lock())->Get_OriginPos();
		// 오리진 포스 등록

		FUNCTION_NODE Call_Phase1
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Control_Phase1>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
				.leaf<FunctionNode>(Call_Phase1)
			.end()
			.build();
	}

	_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
	m_fOriginDistance = XMVectorGetX(XMVector3Length(m_vTargetPos - vPos));
	// 이 상태로 들어왔을 때 처음 위치에서 오리진 위치까지의 총 거리

	return S_OK;
}

void CState_AirBurster_Hook_Return::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Hook_Return::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
	_vector vTemPos = XMVectorSetW(m_vTargetPos, 1.f);

	vPos.m128_f32[1] = 0.f;
	vTemPos.m128_f32[1] = 0.f;
	_float fLength = XMVectorGetX(XMVector3Length(vTemPos - vPos));
	// 현재 위치에서 오리진 위치까지의 거리
	_float fSpeed = pow((fLength / m_fOriginDistance), 2) * m_fSpeed;
	// 오리진 위치에 가까워질 수록 속도가 줄어든다.

	_float fSpeedDelta = m_fAcceleration * fTimeDelta; // 속도 변화량

	if (!m_bArrive)
	{
		if (fLength > 0.3f)
		{
			fSpeed += fSpeedDelta;
			fSpeed = min(max(fSpeed, m_fMinSpeed), m_fMaxSpeed); //최소 속도와 최고 속도를 정함.

			if (fLength <= 1.f)
				fSpeed = 1.f;

			_float3 vAddPos;
			m_pActor_TransformCom.lock()->Go_Backward(fTimeDelta * fSpeed,&vAddPos);
			DynPtrCast<CAirBurster>(m_pActor.lock())->Set_Movement_Amount(vAddPos);
			// 이동량을 확보.
		}
		else
		{
			m_bArrive = true;
			m_pActor_TransformCom.lock()->Set_Position(fTimeDelta, m_vTargetPos);
		}
	}
}

void CState_AirBurster_Hook_Return::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Hook_Return::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
	m_bArrive = false;
	m_bMediate = false;

	DynPtrCast<CAirBurster>(m_pActor.lock())->Set_Movement_Amount(_float3(0.f,0.f,0.f));
	// 이동량 초기화.

	for (_uint i = 0; i < 2; i++)
	{
		shared_ptr<CPlayer> pPlayer = DynPtrCast<CPlayer>(GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor.lock(), (PLAYER_TYPE)i).pTarget.lock());
		
		auto pCurState = pPlayer->Get_StateMachineCom().lock()->Get_CurState();

		if (pPlayer->Get_PlayerType() == CLOUD)
		{
			if (typeid(*pCurState) == typeid(CState_Cloud_Abnormal))
				pPlayer->Get_StateMachineCom().lock()->Set_State<CState_Cloud_Idle>();
		}
		else
		{
			if (typeid(*pCurState) == typeid(CState_Aerith_Abnormal))
				pPlayer->Get_StateMachineCom().lock()->Set_State<CState_Aerith_Idle>();
		}
	}

	
}

bool CState_AirBurster_Hook_Return::isValid_NextState(CState* state)
{
	if (m_bArrive)
		return true;
	else
		return false;
}

void CState_AirBurster_Hook_Return::Free()
{
}
