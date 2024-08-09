#include "stdafx.h"
#include "Boss/Bahamut/Weapon/Bahamut_Breath.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Effect_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CBahamut_Breath)
IMPLEMENT_CLONE(CBahamut_Breath, CGameObject)
CBahamut_Breath::CBahamut_Breath(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CBahamut_Breath::CBahamut_Breath(const CBahamut_Breath& rhs)
	: CBullet(rhs)
{
	m_TimeChecker = FTimeChecker(1.2f);
}

HRESULT CBahamut_Breath::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut_Breath::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBahamut_Breath::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	
	
	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 2.f,2.f,1.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	Set_StatusComByOwner("Bahamut_Breath");

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Bahamut_Breath_Fire", shared_from_this());
}

void CBahamut_Breath::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut_Breath::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Go_Straight(fTimeDelta * 1.5f);
}

void CBahamut_Breath::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_TimeChecker.Update(fTimeDelta))
	{
		Set_Dead();
	}
		
}

void CBahamut_Breath::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBahamut_Breath::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBahamut_Breath::Render()
{
	return S_OK;
}

HRESULT CBahamut_Breath::Ready_Components(void* pArg)
{
	return S_OK;
}

void CBahamut_Breath::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"075_SE_Bahamut (FX_DarkBreath_Hit_01).wav", ESoundGroup::Narration, 0.5f);
	}
}

void CBahamut_Breath::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_Breath::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CBahamut_Breath::Free()
{
	__super::Free();
}
