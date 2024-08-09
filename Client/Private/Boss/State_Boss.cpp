#include "stdafx.h"
#include "../Public/Boss/State_Boss.h"

#include "Client_Manager.h"
#include "GameInstance.h"

#include "Boss/Boss.h"

#include "Camera_Manager.h"

CState_Boss::CState_Boss(shared_ptr<class CGameObject> pActor, shared_ptr<class CStateMachine> pStatemachine)
	:CState(pActor, pStatemachine)
{
}

HRESULT CState_Boss::Initialize_State(CState* pPreviousState)
{
	__super::Initialize_State(pPreviousState);

	return S_OK;
}

void CState_Boss::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CState_Boss::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CState_Boss::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CState_Boss::Transition_State(CState* pNextState)
{
	__super::Transition_State(pNextState);
}

bool CState_Boss::isValid_NextState(CState* state)
{
	return true;
}

void CState_Boss::Cur_Player_Look(_cref_time fTimeDelta,_bool bYMediate)
{
	auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor.lock());
	m_pActor_TransformCom.lock()->Rotate_On_BaseDir(fTimeDelta,
		bYMediate == true ? Get_CurPlayer_Y_Mediate_Dir() : pPlayerInfo.vDirToTarget);
}

_vector CState_Boss::Get_CurPlayer_Y_Mediate_Dir()
{// y 값을 보스 기준으로 맞춘 look 방향을 사용
	auto pPlayerInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pActor);

	_vector vPos = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION);
	_vector vTempPos = pPlayerInfo.vTargetPos;
	vTempPos.m128_f32[1] = vPos.m128_f32[1];

	_vector vDir = XMVector3Normalize(vTempPos - vPos);

	return vDir;
}

void CState_Boss::TurnOnOff_MotionBlur(_float fStart, _float fEnd,
	_bool bActorCenter, _float fBlurScale, _float fBlurAmount, _float2 vCenter)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(fStart))
	{
		m_pGameInstance->Get_Renderer()->Set_MotionBlur(true);
		m_pGameInstance->Get_Renderer()->Set_MotionBlurScale(fBlurScale);
		m_pGameInstance->Get_Renderer()->Set_MotionBlurAmount(fBlurAmount);
		if (bActorCenter)
		{
			auto pBoss = DynPtrCast<CBoss>(m_pActor.lock());
			_float4 vActorCenter = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION) + pBoss->Get_MotionBlur_Offset();
			vActorCenter = XMVector3TransformCoord(vActorCenter, m_pGameInstance->Get_TransformMatrix(CPipeLine::TS_VIEW));
			vActorCenter = XMVector3TransformCoord(vActorCenter, m_pGameInstance->Get_TransformMatrix(CPipeLine::TS_PROJ));
			vActorCenter = (vActorCenter + _float2(1.f, 1.f)) / 2.f;

			m_pGameInstance->Get_Renderer()->Set_MotionBlurCenter(_float2(vActorCenter.x, vActorCenter.y));
		}
		else
			m_pGameInstance->Get_Renderer()->Set_MotionBlurCenter(vCenter);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(fEnd))
	{
		GET_SINGLE(CClient_Manager)->TurnOff_MotionBlur();
	}
}

void CState_Boss::MotionBlur_On(_bool bActorCenter, _float fBlurScale, _float fBlurAmount, _float2 vCenter)
{
	m_pGameInstance->Get_Renderer()->Set_MotionBlur(true);
	m_pGameInstance->Get_Renderer()->Set_MotionBlurScale(fBlurScale);
	m_pGameInstance->Get_Renderer()->Set_MotionBlurAmount(fBlurAmount);
	if (bActorCenter)
	{
		auto pBoss = DynPtrCast<CBoss>(m_pActor.lock());
		_float4 vActorCenter = m_pActor_TransformCom.lock()->Get_State(CTransform::STATE_POSITION) + pBoss->Get_MotionBlur_Offset();
		vActorCenter = XMVector3TransformCoord(vActorCenter, m_pGameInstance->Get_TransformMatrix(CPipeLine::TS_VIEW));
		vActorCenter = XMVector3TransformCoord(vActorCenter, m_pGameInstance->Get_TransformMatrix(CPipeLine::TS_PROJ));
		vActorCenter = (vActorCenter + _float2(1.f, 1.f)) / 2.f;

		m_pGameInstance->Get_Renderer()->Set_MotionBlurCenter(_float2(vActorCenter.x, vActorCenter.y));
	}
	else
		m_pGameInstance->Get_Renderer()->Set_MotionBlurCenter(vCenter);
}

void CState_Boss::Camera_Shaking(string strTag,_float fStart, _float fEnd)
{
	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(fStart))
	{
		auto pPlayerType = GET_SINGLE(CClient_Manager)->Get_PlayerType();

		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(pPlayerType).lock()->Set_CameraShake(strTag, true);
	}

	if (m_pActor_ModelCom.lock()->IsAnimation_Frame_Once(fEnd))
	{
		auto pPlayerType = GET_SINGLE(CClient_Manager)->Get_PlayerType();

		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(pPlayerType).lock()->Set_CameraShake(strTag, false);
	}
}

void CState_Boss::Camera_Shaking(string strTag,_bool bCheck)
{
	auto pPlayerType = GET_SINGLE(CClient_Manager)->Get_PlayerType();

	GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(pPlayerType).lock()->Set_CameraShake(strTag, bCheck);
}

void CState_Boss::Free()
{
}
