#include "stdafx.h"
#include "Boss/AirBurster/Weapon/AirBurster_EnergyBall.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"
#include "Trail_Effect.h"

#include "Effect_Group.h"
#include "Trail_Effect.h"

#include "Effect_Manager.h"
#include "Light.h"

IMPLEMENT_CREATE(CAirBurster_EnergyBall)
IMPLEMENT_CLONE(CAirBurster_EnergyBall, CGameObject)
CAirBurster_EnergyBall::CAirBurster_EnergyBall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CAirBurster_EnergyBall::CAirBurster_EnergyBall(const CAirBurster_EnergyBall& rhs)
	: CBullet(rhs)
{
}

HRESULT CAirBurster_EnergyBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster_EnergyBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAirBurster_EnergyBall::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);
	m_vTargetPos = pTarget.vTargetPos;

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::SPHERE,
		CL_MONSTER_ATTACK, m_pTransformCom, { 0.5f, 0.5f, 0.5f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AirBursterEnergyBall_Main"), shared_from_this());
	GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AirBursterEnergyBall_Shoot"), shared_from_this());

	// 스킬 설정
	Set_StatusComByOwner("AirBurster_EnergyBall");

	m_fLifeTime = 5.f; 
}

void CAirBurster_EnergyBall::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAirBurster_EnergyBall::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_fLifeTime.Increase(fTimeDelta))
	{
		Set_Dead();
		if (m_pEffect)
			m_pEffect->Set_Dead();
	}

	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
}

void CAirBurster_EnergyBall::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CAirBurster_EnergyBall::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAirBurster_EnergyBall::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAirBurster_EnergyBall::Render()
{
	return S_OK;
}

HRESULT CAirBurster_EnergyBall::Ready_Components(void* pArg)
{

	return S_OK;
}

void CAirBurster_EnergyBall::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = false;
	if ((bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY))
		|| pOtherCol->Get_ColliderDesc().iFilterType == CL_MAP_STATIC)
	{
		if (bUseDamage)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AirBursterEnergyBall_Hit"), shared_from_this());
		Set_Dead();
		m_pEffect->Set_Dead();

		shared_ptr<PxContactPairPoint> pContactPoint(new PxContactPairPoint[ContactInfo.contactCount]);
		PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint.get(), ContactInfo.contactCount);

		if (nbContacts >= 1)
		{
			_float3 vPos = Convert_Vector(pContactPoint->position);
			LIGHT_DESC LightDesc = {};
			LightDesc.bUseVolumetric = false;
			LightDesc.eType = LIGHT_DESC::TYPE_POINT;
			LightDesc.fRange = 3.5f;
			LightDesc.strLightName = "LightEnergyBall" + to_string(m_pGameInstance->RandomInt(0, 10000000));
			LightDesc.vDiffuse = { 2.f, 2.f, 2.2f, 1.f };
			LightDesc.fSpotPower = 10.f;
			LightDesc.vPosition = { vPos.x, vPos.y, vPos.z, 1.f };
			LightDesc.vAmbient = { 0.2f, 0.2f, 0.2f, 1.f };
			LightDesc.vEmissive = { 0.2f, 0.2f, 0.16f, 1.f };
			LightDesc.vSpecular = { 0.2f, 0.2f, 0.16f, 1.f };

			shared_ptr<CLight>    pLight = nullptr;
			m_pGameInstance->Add_Light(LightDesc, &pLight);
			pLight->Set_RangeLinearDecrease(3.f);
			pLight->Set_RangeQuadDecrease(3.f);
			pLight->Set_LightDamping(3.f);
			pLight->Set_LightVolumeQuadDamping(3.f);
			pLight->Set_Dead();
		}
	}
}

void CAirBurster_EnergyBall::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_EnergyBall::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAirBurster_EnergyBall::Free()
{
	__super::Free();
}
