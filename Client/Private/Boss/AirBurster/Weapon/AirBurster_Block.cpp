#include "stdafx.h"
#include "Boss/AirBurster/Weapon/AirBurster_Block.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"

#include "Effect_Manager.h"

IMPLEMENT_CREATE(CAirBurster_Block)
IMPLEMENT_CLONE(CAirBurster_Block, CGameObject)
CAirBurster_Block::CAirBurster_Block(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CAirBurster_Block::CAirBurster_Block(const CAirBurster_Block& rhs)
	: CBullet(rhs)
{
}

HRESULT CAirBurster_Block::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster_Block::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAirBurster_Block::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_StaticCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MAP_STATIC,
		m_pTransformCom, _float3(1.f, 3.f, 5.f));
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom_Block"), &(m_pPhysXColliderCom_Block), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.

	// 스킬 설정

	m_pTransformCom->Set_Position(1.f,_float3(87.011, 2.568, 89.373));
}

void CAirBurster_Block::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CAirBurster_Block::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CAirBurster_Block::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);


	if (m_pPhysXColliderCom_Block)
		m_pPhysXColliderCom_Block->Synchronize_Collider(m_pTransformCom, m_vPhysXColliderLocalOffset, m_vPhysXColliderWorldOffset);
}

void CAirBurster_Block::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAirBurster_Block::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAirBurster_Block::Render()
{
	return S_OK;
}

HRESULT CAirBurster_Block::Ready_Components(void* pArg)
{
	return S_OK;
}

void CAirBurster_Block::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
	}
}

void CAirBurster_Block::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_Block::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAirBurster_Block::Free()
{
	__super::Free();
}
