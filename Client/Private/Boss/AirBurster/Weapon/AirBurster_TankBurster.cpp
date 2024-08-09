#include "stdafx.h"
#include "Boss/AirBurster/Weapon/AirBurster_TankBurster.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"

#include "Trail_Effect.h"

IMPLEMENT_CREATE(CAirBurster_TankBurster)
IMPLEMENT_CLONE(CAirBurster_TankBurster, CGameObject)

CAirBurster_TankBurster::CAirBurster_TankBurster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CAirBurster_TankBurster::CAirBurster_TankBurster(const CAirBurster_TankBurster& rhs)
	: CBullet(rhs)
{
}

HRESULT CAirBurster_TankBurster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster_TankBurster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	m_TimeChecker = FTimeChecker(3.f);

	return S_OK;
}

void CAirBurster_TankBurster::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);
	m_vTargetPos = pTarget.vTargetPos;

	// 오너가 바라보는 방향대로 바라보게 한다.
	m_pTransformCom->Set_Look_Manual(m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_LOOK));

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 3.f, 3.f, 100.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,50.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.

	// 스킬 설정
	Set_StatusComByOwner("AirBurster_TankBurster");
}

void CAirBurster_TankBurster::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAirBurster_TankBurster::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
}

void CAirBurster_TankBurster::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if(m_TimeChecker.Update(fTimeDelta))
		Set_Dead();
}

void CAirBurster_TankBurster::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAirBurster_TankBurster::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAirBurster_TankBurster::Render()
{
	return S_OK;
}

HRESULT CAirBurster_TankBurster::Ready_Components(void* pArg)
{

	return S_OK;
}

void CAirBurster_TankBurster::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_TankBurster::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
	}
}

void CAirBurster_TankBurster::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAirBurster_TankBurster::Free()
{
	__super::Free();
}
