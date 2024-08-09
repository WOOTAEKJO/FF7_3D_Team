#include "stdafx.h"
#include "Boss/AirBurster/Weapon/AirBurster_EMField.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"

IMPLEMENT_CREATE(CAirBurster_EMField)
IMPLEMENT_CLONE(CAirBurster_EMField, CGameObject)
CAirBurster_EMField::CAirBurster_EMField(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CAirBurster_EMField::CAirBurster_EMField(const CAirBurster_EMField& rhs)
	: CBullet(rhs)
{
}

HRESULT CAirBurster_EMField::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster_EMField::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAirBurster_EMField::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 8.0f, 3.f, 8.0f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.

	// 스킬 설정
	Set_StatusComByOwner("AirBurster_EMField");

	m_fLifeTime = 1.0f;
}

void CAirBurster_EMField::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAirBurster_EMField::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fDamageTick.Update(fTimeDelta);
	 
	if (m_pOwner.lock())
	{
		_vector vOwnerPos = m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Set_Position(fTimeDelta,vOwnerPos);
		// 항상 보스를 따라가게 하자
	}

	if (m_fLifeTime.Increase(fTimeDelta))
	{
		Set_Dead();
	}
}

void CAirBurster_EMField::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CAirBurster_EMField::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAirBurster_EMField::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAirBurster_EMField::Render()
{
	return S_OK;
}

HRESULT CAirBurster_EMField::Ready_Components(void* pArg)
{

	return S_OK;
}

void CAirBurster_EMField::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

}

void CAirBurster_EMField::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = false;
	if (m_fDamageTick.IsTicked() && (bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)))
	{
		if (bUseDamage)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

	}
}

void CAirBurster_EMField::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAirBurster_EMField::Free()
{
	__super::Free();
}
