#include "stdafx.h"
#include "Boss/AirBurster/Weapon/AirBurster_Burner.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"
#include "Particle.h"

IMPLEMENT_CREATE(CAirBurster_Burner)
IMPLEMENT_CLONE(CAirBurster_Burner, CGameObject)
CAirBurster_Burner::CAirBurster_Burner(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CAirBurster_Burner::CAirBurster_Burner(const CAirBurster_Burner& rhs)
	: CBullet(rhs)
{
}

HRESULT CAirBurster_Burner::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster_Burner::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAirBurster_Burner::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);
	m_vTargetPos = pTarget.vTargetPos;

	// 오차 세팅
	_float3 vTargetRanPos = m_pGameInstance->RandomFloat3({ -0.25f, 0.25f, -0.25f }, { 0.25f, 0.25f, 0.25f });
	vTargetRanPos = XMVector3TransformNormal(vTargetRanPos, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix());
	m_vTargetPos.x += vTargetRanPos.x;
	m_vTargetPos.z += vTargetRanPos.z;

	m_pTransformCom->Set_Look_Manual(m_vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::SPHERE, CL_MONSTER_ATTACK, m_pTransformCom, { 0.3f, 0.3f, 0.3f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(TEXT("AirBurster_Fire2"), shared_from_this());

	// 스킬 설정
	Set_StatusComByOwner("AirBurster_Burner");
}

void CAirBurster_Burner::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAirBurster_Burner::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
	m_fAccChaseValue += fTimeDelta * 0.017f;
	m_fSpeed -= fTimeDelta * 2.f;

	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
}

void CAirBurster_Burner::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pEffect)
	{
		if (m_pEffect->Get_IsEffectDead())
			Set_Dead();
	}

}

void CAirBurster_Burner::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAirBurster_Burner::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAirBurster_Burner::Render()
{
	return S_OK;
}

HRESULT CAirBurster_Burner::Ready_Components(void* pArg)
{
	return S_OK;
}

void CAirBurster_Burner::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
	}
}

void CAirBurster_Burner::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_Burner::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAirBurster_Burner::Free()
{
	__super::Free();
}
