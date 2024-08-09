#include "stdafx.h"
#include "Boss/AirBurster/Weapon/AirBurster_MachineGun.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"

#include "Effect_Manager.h"

#include "Boss/AirBurster/State_List_AirBurster.h"
#include "State_List_Cloud.h"
#include "Aerith/State/State_Aerith_Abnormal.h"

#include "Player.h"

IMPLEMENT_CREATE(CAirBurster_MachineGun)
IMPLEMENT_CLONE(CAirBurster_MachineGun, CGameObject)
CAirBurster_MachineGun::CAirBurster_MachineGun(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CAirBurster_MachineGun::CAirBurster_MachineGun(const CAirBurster_MachineGun& rhs)
	: CBullet(rhs)
{
}

HRESULT CAirBurster_MachineGun::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster_MachineGun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAirBurster_MachineGun::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	

	auto pCloudInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner,PLAYER_TYPE::CLOUD);
	auto pAerithInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner, PLAYER_TYPE::AERITH);

	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vCloudLook = pCloudInfo.vDirToTarget;

	if (XMVectorGetX(XMVector3Dot(XMVector3Normalize(vCloudLook), XMVector3Normalize(vLook))) > 0.f)
	{
		//클라우드가 보스 방향에 있다.
		if (pCloudInfo.pTarget.lock() != nullptr)
		{
			_vector vResultPos = pCloudInfo.vTargetPos;
			vResultPos.m128_f32[1] += 1.f;
			_vector vResultLook = XMVector3Normalize(vResultPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pTransformCom->Set_Look_Manual(vResultLook);
		}
	}
	else {
		//에어리스가 보스 방향에 있다.
		if (pAerithInfo.pTarget.lock() != nullptr)
		{
			_vector vResultPos = pAerithInfo.vTargetPos;
			vResultPos.m128_f32[1] += 1.f;
			_vector vResultLook = XMVector3Normalize(vResultPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pTransformCom->Set_Look_Manual(vResultLook);
		}
	}
	

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 0.2f, 0.2f, 0.5f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	shared_ptr<CEffect> pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Effect>(TEXT("SecuritySoldierBullet3"), shared_from_this());
}

void CAirBurster_MachineGun::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);

}

void CAirBurster_MachineGun::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;

	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
}

void CAirBurster_MachineGun::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (5.f <= m_fAccTime)
		Set_Dead();
}

void CAirBurster_MachineGun::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAirBurster_MachineGun::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAirBurster_MachineGun::Render()
{
	return S_OK;
}

HRESULT CAirBurster_MachineGun::Ready_Components(void* pArg)
{
	return S_OK;
}

void CAirBurster_MachineGun::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = false;
	if ((bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY))
		|| pOtherCol->Get_ColliderDesc().iFilterType == CL_MAP_STATIC)
	{
		if (bUseDamage)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AirBursterSoulderBeamHit"), shared_from_this());


		auto pState = m_pOwner.lock()->Get_StateMachineCom().lock()->Get_CurState();

		Set_Dead();
	}
}

void CAirBurster_MachineGun::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_MachineGun::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAirBurster_MachineGun::Free()
{
	__super::Free();
}
