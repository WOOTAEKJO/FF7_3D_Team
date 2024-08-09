#include "stdafx.h"
#include "Boss/Bahamut/Weapon/Bahamut_Inferno.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Effect_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CBahamut_Inferno)
IMPLEMENT_CLONE(CBahamut_Inferno, CGameObject)
CBahamut_Inferno::CBahamut_Inferno(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CBahamut_Inferno::CBahamut_Inferno(const CBahamut_Inferno& rhs)
	: CBullet(rhs)
{
}

HRESULT CBahamut_Inferno::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut_Inferno::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBahamut_Inferno::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	
	
	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);
	m_fTargetPos = pTarget.vTargetPos;
	m_fTargetPos.y += 1.f;
	// 타겟 위치 저장

	m_fRenewPosTime = m_pGameInstance->Random(0.05f, 0.1f);
	m_TimeChecker = FTimeChecker(m_fRenewPosTime);
	// 갱신되는 시간 랜덤

	// 40퍼 확률로 빗나감
	if (m_pGameInstance->Random(0.f, 1.f) <= 0.3f)
	{
		_float3 vTargetRanPos = m_pGameInstance->RandomFloat3({ -1.5f, -1.5f, -1.f }, { 1.5f, 1.5f, 1.f });
		_float fLength = XMVector3Length(vTargetRanPos).m128_f32[0];
		vTargetRanPos = XMVector3TransformNormal(vTargetRanPos, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix()) * fLength;
		m_fTargetPos += vTargetRanPos;
	}

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,
		PHYSXCOLLIDER_TYPE::SPHERE, CL_MONSTER_ATTACK, m_pTransformCom, { 0.3f,0.3f,0.3f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	Set_StatusComByOwner("Bahamut_Inferno");

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	shared_ptr<CEffect> pEffect1 = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Effect>(L"Bahamut_Inferno_Bullet", shared_from_this());
	GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Buffer>(L"ET_Bahamut_Inferno_Bullet_Trail", pEffect1, CEffect::USE_TYPE::USE_FOLLOW_EFFECT);
}

void CBahamut_Inferno::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut_Inferno::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);
	_vector vOwnerPos = m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vMediate = pTarget.vTargetPos;
	vMediate.y += 1.f;

	_float fMaxLength = XMVectorGetX(XMVector3Length(vMediate - vOwnerPos));
	_float fCurLength = XMVectorGetX(XMVector3Length(vPos - vOwnerPos));

	if (fMaxLength > fCurLength)
	{
		m_fStartTime += fTimeDelta;

		if (m_fStartTime >= 0.1f)
		{
			if (m_iRenewCount != 2)
			{
				if (m_TimeChecker.Update(fTimeDelta))
				{
					m_fTargetPos = vMediate;

					if (m_pGameInstance->Random(0.f, 1.f) <= 0.3f)
					{
						_float3 vTargetRanPos = m_pGameInstance->RandomFloat3({ -1.5f, -1.5f, -1.f }, { 1.5f, 1.5f, 1.f });
						_float fLength = XMVector3Length(vTargetRanPos).m128_f32[0];
						vTargetRanPos = XMVector3TransformNormal(vTargetRanPos, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix()) * fLength;
						m_fTargetPos += vTargetRanPos;
					}

					m_iRenewCount += 1;
				} // 시간에 따라 위치 갱신
			}
			else {
				m_fAccTime += fTimeDelta;
			}

			m_fAccChaseValue += fTimeDelta * 0.018f * 10.f * 10.f; // 회전속도
			m_fSpeed += fTimeDelta * 1.f; // 가속도++

			m_pTransformCom->Look_At(m_fTargetPos, m_fAccChaseValue);
		}
	}
	else {
		m_fAccTime += fTimeDelta;
	}

	// 점점 타겟을 바라보게 됨
	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
}

void CBahamut_Inferno::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_fAccTime >= 1.5f)
		Set_Dead();
}

void CBahamut_Inferno::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBahamut_Inferno::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBahamut_Inferno::Render()
{
	return S_OK;
}

HRESULT CBahamut_Inferno::Ready_Components(void* pArg)
{
	return S_OK;
}

void CBahamut_Inferno::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"083_SE_Bahamut (FX_HomingLaser_Hit_01; FX_HomingLaser_Hit_01_Material).wav", ESoundGroup::Narration, 0.15f);

		Set_Dead();
	}
}

void CBahamut_Inferno::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_Inferno::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CBahamut_Inferno::Free()
{
	__super::Free();
}
