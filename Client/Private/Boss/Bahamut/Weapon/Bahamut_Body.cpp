#include "stdafx.h"
#include "Boss/Bahamut/Weapon/Bahamut_Body.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Effect_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CBahamut_Body)
IMPLEMENT_CLONE(CBahamut_Body, CGameObject)
CBahamut_Body::CBahamut_Body(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{	
}

CBahamut_Body::CBahamut_Body(const CBahamut_Body& rhs)
	: CBullet(rhs)
{
	m_LifeTimeChecker = FTimeChecker(5.f);
}

HRESULT CBahamut_Body::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut_Body::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBahamut_Body::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	
	

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,
		PHYSXCOLLIDER_TYPE::SPHERE, CL_MONSTER_ATTACK, m_pTransformCom, { 4.f, 4.f, 4.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	Set_StatusComByOwner("Bahamut_Body");

	// [여기에] 트레일 이펙트 생성 넣어주세요.
}

void CBahamut_Body::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut_Body::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pOwner.lock())
	{
		_matrix MuzzleMatrix = m_pOwner.lock()->Get_ModelCom().lock()->
			Get_BoneTransformMatrixWithParents(m_strBoneName);
		m_pTransformCom->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
		// 위치가 항상 뼈의 영향을 받는다.
	}
}

void CBahamut_Body::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_LifeTimeChecker.Update(fTimeDelta))
		Set_Dead();
}

void CBahamut_Body::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBahamut_Body::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBahamut_Body::Render()
{
	return S_OK;
}

HRESULT CBahamut_Body::Ready_Components(void* pArg)
{
	return S_OK;
}

void CBahamut_Body::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"067_SE_Bahamut (FX_Aura_Hit_01).wav", ESoundGroup::Narration, 0.5f);
	}
}

void CBahamut_Body::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_Body::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CBahamut_Body::Free()
{
	__super::Free();
}
