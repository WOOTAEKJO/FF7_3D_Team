#include "stdafx.h"
#include "Boss/AirBurster/Weapon/AirBurster_Hook.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"

#include "Effect_Manager.h"

#include "Camera_Manager.h"

#include "Player.h"

IMPLEMENT_CREATE(CAirBurster_Hook)
IMPLEMENT_CLONE(CAirBurster_Hook, CGameObject)
CAirBurster_Hook::CAirBurster_Hook(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CAirBurster_Hook::CAirBurster_Hook(const CAirBurster_Hook& rhs)
	: CBullet(rhs)
{
}

HRESULT CAirBurster_Hook::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster_Hook::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAirBurster_Hook::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	
	

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,
		PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 3.f, 3.f, 3.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.

	// 스킬 설정
	Set_StatusComByOwner("AirBurster_Hook");
}

void CAirBurster_Hook::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CAirBurster_Hook::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;

	if (m_pOwner.lock())
	{
		_matrix MuzzleMatrix = m_pOwner.lock()->Get_ModelCom().lock()->
			Get_BoneTransformMatrixWithParents(m_strBoneName);
		m_pTransformCom->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
		// 위치가 항상 뼈의 영향을 받는다.
	}
}

void CAirBurster_Hook::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (1.f <= m_fAccTime)
		Set_Dead();
}

void CAirBurster_Hook::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAirBurster_Hook::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAirBurster_Hook::Render()
{
	return S_OK;
}

HRESULT CAirBurster_Hook::Ready_Components(void* pArg)
{
	return S_OK;
}

void CAirBurster_Hook::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AirBursterSoulderBeamHit"), shared_from_this());
		Set_Dead();

		auto pPlayerType = DynPtrCast<CPlayer>(pOtherCol->Get_Owner().lock())->Get_PlayerType();

		GET_SINGLE(CCamera_Manager)->Get_ThirdPersonCamera(pPlayerType).lock()->Set_CameraShake("Airburster_Hook_Hit", true);
	}
}

void CAirBurster_Hook::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_Hook::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAirBurster_Hook::Free()
{
	__super::Free();
}
