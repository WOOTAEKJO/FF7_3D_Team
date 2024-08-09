#include "stdafx.h"
#include "Boss/Bahamut/Weapon/Bahamut_Aura.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Effect_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CBahamut_Aura)
IMPLEMENT_CLONE(CBahamut_Aura, CGameObject)
CBahamut_Aura::CBahamut_Aura(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CBahamut_Aura::CBahamut_Aura(const CBahamut_Aura& rhs)
	: CBullet(rhs)
{
	m_LifeTimeChecker = FTimeChecker(5.f);
}

HRESULT CBahamut_Aura::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut_Aura::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBahamut_Aura::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::SPHERE, CL_MONSTER_ATTACK, m_pTransformCom, { 20.f,20.f,20.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// 스킬 설정
	Set_StatusComByOwner("Bahamut_Aura");

	// [여기에] 트레일 이펙트 생성 넣어주세요.
}

void CBahamut_Aura::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut_Aura::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pOwner.lock())
	{
		_matrix MuzzleMatrix = m_pOwner.lock()->Get_ModelCom().lock()->
			Get_BoneTransformMatrixWithParents(m_strBoneName);
		m_pTransformCom->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
		// 위치가 항상 뼈의 영향을 받는다.
	}

	// 대미지 틱
	m_fDamageTick.Update(fTimeDelta);
}

void CBahamut_Aura::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_LifeTimeChecker.Update(fTimeDelta))
		Set_Dead();

}

void CBahamut_Aura::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBahamut_Aura::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBahamut_Aura::Render()
{
	return S_OK;
}

HRESULT CBahamut_Aura::Ready_Components(void* pArg)
{
	return S_OK;
}

void CBahamut_Aura::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_Aura::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

	// 대미지 틱이 일어나는 상황일 때 충돌 판정
	if (m_fDamageTick.IsTicked()
		&& (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY))
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		
		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"066_SE_Bahamut(FX_Atk_Aura_02).wav", ESoundGroup::Narration, 0.5f);
	}
}

void CBahamut_Aura::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CBahamut_Aura::Free()
{
	__super::Free();
}
