#include "stdafx.h"
#include "Boss/AirBurster/Weapon/AirBurster_ShoulderBeam.h"
#include "Client_Manager.h"
#include "GameInstance.h"
#include "PartObject.h"

#include "Trail_Effect.h"
#include "Effect_Group.h"

#include "Effect_Manager.h"
#include "Light.h"

IMPLEMENT_CREATE(CAirBurster_ShoulderBeam)
IMPLEMENT_CLONE(CAirBurster_ShoulderBeam, CGameObject)
CAirBurster_ShoulderBeam::CAirBurster_ShoulderBeam(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CAirBurster_ShoulderBeam::CAirBurster_ShoulderBeam(const CAirBurster_ShoulderBeam& rhs)
	: CBullet(rhs)
{
}

HRESULT CAirBurster_ShoulderBeam::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster_ShoulderBeam::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CAirBurster_ShoulderBeam::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);
	m_vTargetPos = pTarget.vTargetPos + pTarget.pTarget.lock()->Get_PhysXControllerLocalOffset();

	// 40퍼 확률로 빗나감
	if (m_pGameInstance->Random(0.f, 1.f) <= 0.3f)
	{
		_float3 vTargetRanPos = m_pGameInstance->RandomFloat3({-2.5f, -2.5f, -2.f}, { 2.5f, 2.5f, 2.f });
		_float fLength = XMVector3Length(vTargetRanPos).m128_f32[0];
		vTargetRanPos = XMVector3TransformNormal(vTargetRanPos, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix()) * fLength;
		m_vTargetPos += vTargetRanPos;
	}
	// 40퍼 확률로 정확도가 조금 낮아짐
	else if (m_pGameInstance->Random(0.f, 1.f) <= 0.3f)
	{
		_float3 vTargetRanPos = m_pGameInstance->RandomFloat3({ -1.f, -1.f, -0.75f }, { 1.f, 1.f, 0.75f });
		_float fLength = XMVector3Length(vTargetRanPos).m128_f32[0];
		vTargetRanPos = XMVector3TransformNormal(vTargetRanPos, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix()) * fLength;
		m_vTargetPos += vTargetRanPos;
	}


	_float3 vRanDir = XMVector3Normalize(XMLoadFloat3(&
		m_pGameInstance->RandomFloat3({ -9.f, 4.f, -4.f }, { 9.f, 20.f, 8.f })));
	vRanDir = XMVector3TransformNormal(vRanDir, m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix());
	m_pTransformCom->Set_Look_Manual(vRanDir);

	m_fCheckPosTime = m_pGameInstance->Random(0.5f, 1.f);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 0.2f, 0.2f, 0.2f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	shared_ptr<CEffect> pEffect1 = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Effect>(L"AirBurster_ShoulderBeam_Bullet_Mesh", shared_from_this());
	shared_ptr<CTrail_Buffer> pEffect2 = GET_SINGLE(CEffect_Manager)->Create_Effect<CTrail_Buffer>(L"ET_Bahamut_Inferno_Bullet_Trail", pEffect1, CEffect::USE_TYPE::USE_FOLLOW_EFFECT);

	pEffect2->Trail_Pos_Reset();

		// 스킬 설정
	Set_StatusComByOwner("AirBurster_ShoulderBeam");

	m_fLifeTime = { 3.f };
}

void CAirBurster_ShoulderBeam::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAirBurster_ShoulderBeam::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;
	m_fAccChaseValue += fTimeDelta * 0.018f * 10.f;
	m_fSpeed += fTimeDelta * 2.f;

	if (!m_bIsChaseEnd)
	{
		m_pTransformCom->Look_At(m_vTargetPos, m_fAccChaseValue);

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float fLength = XMVector3Length(m_vTargetPos - vPos).m128_f32[0];

		// 추적 중지 조건

		if (fLength <= m_fSpeed && m_fAccTime >= m_fCheckPosTime)
			m_bIsChaseEnd = true;
	}

	m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed, nullptr);

	if (m_fLifeTime.Increase(fTimeDelta))
	{
		Set_Dead();
		if (m_pEffect)
			m_pEffect->Set_Dead();
		if (m_pTrailEffect)
			m_pTrailEffect->Set_Dead();
	}
}

void CAirBurster_ShoulderBeam::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CAirBurster_ShoulderBeam::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAirBurster_ShoulderBeam::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAirBurster_ShoulderBeam::Render()
{
	return S_OK;
}

HRESULT CAirBurster_ShoulderBeam::Ready_Components(void* pArg)
{


	return S_OK;
}

void CAirBurster_ShoulderBeam::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	_bool bUseDamage = false;
	if ((bUseDamage = (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY))
		|| pOtherCol->Get_ColliderDesc().iFilterType == CL_MAP_STATIC)
	{
		if (bUseDamage)
			Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AirBursterSoulderBeamHit"), shared_from_this());
		Set_Dead();
		if (m_pEffect) m_pEffect->Set_Dead();
		if (m_pTrailEffect) m_pTrailEffect->Set_Dead();

		shared_ptr<PxContactPairPoint> pContactPoint(new PxContactPairPoint[ContactInfo.contactCount]);
		PxU32 nbContacts = ContactInfo.extractContacts(pContactPoint.get(), ContactInfo.contactCount);

		if (nbContacts >= 1)
		{
			_float3 vPos = Convert_Vector(pContactPoint->position);

			LIGHT_DESC LightDesc = {};
			LightDesc.bUseVolumetric = false;
			LightDesc.eType = LIGHT_DESC::TYPE_POINT;
			LightDesc.fRange = 9.5f;
			LightDesc.strLightName = "LightShouderBeam" + to_string(m_pGameInstance->RandomInt(0, 10000000));
			LightDesc.vDiffuse = { 1.f, 0.239f, 0.0f, 1.f };
			LightDesc.fSpotPower = 10.f;
			LightDesc.vPosition = { vPos.x, vPos.y, vPos.z, 1.f };
			LightDesc.vAmbient = { 0.2f, 0.2f, 0.2f, 1.f };
			LightDesc.vEmissive = { 0.2f, 0.2f, 0.16f, 1.f };
			LightDesc.vSpecular = { 0.2f, 0.2f, 0.16f, 1.f };

			shared_ptr<CLight>    pLight = nullptr;
			m_pGameInstance->Add_Light(LightDesc, &pLight);
			pLight->Set_RangeLinearDecrease(3.f);
			pLight->Set_RangeQuadDecrease(6.f);
			pLight->Set_LightDamping(3.f);
			pLight->Set_LightVolumeQuadDamping(3.f);
			pLight->Set_Dead();
		}
	}
}

void CAirBurster_ShoulderBeam::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_ShoulderBeam::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAirBurster_ShoulderBeam::Free()
{
	__super::Free();
}
