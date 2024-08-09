#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/State_AirBurster_Arm_Separate.h"
#include "../Public/Boss/AirBurster/State_List_AirBurster.h"
#include "../Public/Boss/AirBurster/AirBurster.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_Parts.h"
#include "Player.h"
#include "Character.h"

#include "GameInstance.h"

#include "Camera_Manager.h"

CState_AirBurster_Arm_Separate::CState_AirBurster_Arm_Separate(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState_AirBurster(pActor, pStatemachine)
{
}

HRESULT CState_AirBurster_Arm_Separate::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	m_pActor_ModelCom.lock()->Set_Animation("Main|B_AtkRocket01", 1.3f, false);

	SetUp_AI_Action_Num(AI_ACTION_NON_TARGET);

	if (!m_pBehaviorTree)
	{
		m_pActor_ModelCom.lock()->Set_PreRotate(
			XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f)));

		m_fStartPosX = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION).m128_f32[0];

		FUNCTION_NODE Call_Push
			= FUNCTION_NODE_MAKE
		{
			m_pStateMachineCom.lock()->Enter_State<CState_AirBurster_Arm_Push>();

			return BT_STATUS::Success;
		};

		m_pBehaviorTree = Builder()
			.composite<Sequence>()
			.leaf<FunctionNode>(Call_Push)
			.end()
			.build();
	}

	return S_OK;
}

void CState_AirBurster_Arm_Separate::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	_vector vArmPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);

	if (m_pActor_ModelCom.lock()->IsAnimation_UpTo(30.f))
	{
		if (!m_bLerp)
		{
			if (m_fPosY < vArmPos.m128_f32[1])
				m_bLerp = true;
			else
				m_fPosY = vArmPos.m128_f32[1];
		}// y 값 맞춰주기

		_float4 fCurPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);

		if (m_fStartPosX - m_fMovement_amount >= fCurPos.x)
		{
			if (m_bMove)
			{
				m_pActor_TransformCom.lock()->Set_Move_AnimationPosition(false);
				m_bMove = false;
			}
		}// 특정 구간 루트애니메이션 이동량 빼기

		_float3	vScale = m_pActor_TransformCom.lock()->Get_Scaled();
		_vector vPrevUP = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_UP);
		_vector vDestUP = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;

		if (!XMVector3NearEqual(vPrevUP, vDestUP, XMVectorZero()))
		{
			_float		fTurnDir = XMVectorGetX(XMVector3Dot(XMVector3Cross(vPrevUP, vDestUP), vDestUP)) > 0.f ? 1.f : -1.f;
			_vector		vRight = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_RIGHT);
			_vector		vLook = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_LOOK);
			
			_vector vAxis = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vPrevUP));

			_matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fTurnDir * fTimeDelta);

			m_pActor_TransformCom.lock()->Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
			m_pActor_TransformCom.lock()->Set_State(CTransform::STATE_UP, XMVector3TransformNormal(vPrevUP, RotationMatrix));
			m_pActor_TransformCom.lock()->Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

			// Up 방향 위로 향하게
		}

	}

	if (m_bLerp)
	{
		_vector vMainPos = m_pActor.lock()->Get_Owner().lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
		_vector vTempPos = vArmPos;
		if (!InRange(vArmPos.m128_f32[1], vMainPos.m128_f32[1] + 1.9f, vMainPos.m128_f32[1] + 2.1f, "()"))
		{
			vTempPos.m128_f32[1] -= fTimeDelta * 10.f;
			m_pActor_TransformCom.lock()->Set_Position(fTimeDelta, XMVectorLerp(vArmPos, vTempPos, 0.5f));
		}
	}
	// 팔들 오너가 바라보는 look의 각각 90, -90 look 방향 주기
	m_pBehaviorTree->update(fTimeDelta);
}

void CState_AirBurster_Arm_Separate::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CState_AirBurster_Arm_Separate::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_AirBurster_Arm_Separate::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);	

	m_pActor_ModelCom.lock()->Set_PreRotate(
		XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(-90.f)));

	_vector vMainPos = m_pActor.lock()->Get_Owner().lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
	_vector vArmPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
	_vector vTempPos = vArmPos;

	vTempPos.m128_f32[1] = vMainPos.m128_f32[1] + 2.f;
	m_pActor_TransformCom.lock()->Set_Position(0.1f, vTempPos);

	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);

	if (pTarget.pTarget.lock())
	{
		_vector vPos = pTarget.vTargetPos;

		m_pActor_TransformCom.lock()->Look_At(vPos);
	}
		
	static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Set_Transition(false); // 다음 애니메이션 보간x

	m_bMove = true;

	static_pointer_cast<CAirBurster_Parts>(m_pActor.lock())->Set_ActivePos(m_pActor_TransformCom.lock()
	->Get_State(CTransform::STATE_POSITION));
	// 고정된 위치를 저장
}

bool CState_AirBurster_Arm_Separate::isValid_NextState(CState* state)
{
	if(m_pActor_ModelCom.lock()->IsAnimation_Finished())
		return true;
	else
		return false;

}

void CState_AirBurster_Arm_Separate::Free()
{
}
