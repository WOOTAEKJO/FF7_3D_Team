#include "stdafx.h"
#include "Boss/Bahamut/Weapon/Bahamut_DarkClaw.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Effect_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CBahamut_DarkClaw)
IMPLEMENT_CLONE(CBahamut_DarkClaw, CGameObject)
CBahamut_DarkClaw::CBahamut_DarkClaw(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CBahamut_DarkClaw::CBahamut_DarkClaw(const CBahamut_DarkClaw& rhs)
	: CBullet(rhs)
{
}

HRESULT CBahamut_DarkClaw::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut_DarkClaw::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBahamut_DarkClaw::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	
	

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,
		PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 0.5f, 4.5f, 0.8f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	Set_StatusComByOwner("Bahamut_HeavyStrike");

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(L"Bahamut_DarClaw_AttackSlash", shared_from_this());
}

void CBahamut_DarkClaw::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut_DarkClaw::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;

	m_pTransformCom->Go_Straight(fTimeDelta * 5.f);
}

void CBahamut_DarkClaw::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_fAccTime > 3.f)
		Set_Dead();
}

void CBahamut_DarkClaw::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBahamut_DarkClaw::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBahamut_DarkClaw::Render()
{
	return S_OK;
}

HRESULT CBahamut_DarkClaw::Ready_Components(void* pArg)
{
	return S_OK;
}

void CBahamut_DarkClaw::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"106_SE_Bahamut (FX_Scartch_Hit_01).wav", ESoundGroup::Narration, 0.5f);

		GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(L"Bahamut_Clo_Hit",
			pOtherCol->Get_Owner().lock(), CEffect::USE_TYPE::USE_FOLLOW_NORMAL, _float3(0.f, 1.f, 0.f));
	}
}

void CBahamut_DarkClaw::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_DarkClaw::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CBahamut_DarkClaw::Free()
{
	__super::Free();
}
