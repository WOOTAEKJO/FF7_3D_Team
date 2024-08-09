#include "stdafx.h"
#include "Boss/AirBurster/Weapon/AirBurster_FingerBeam.h"
#include "GameInstance.h"
#include "PartObject.h"
#include "Client_Manager.h"

#include "CommonModelComp.h"
#include "PhysX_Manager.h"

#include "Effect_Group.h"
#include "Effect_Manager.h"

IMPLEMENT_CREATE(CAirBurster_FingerBeam)
IMPLEMENT_CLONE(CAirBurster_FingerBeam, CGameObject)
CAirBurster_FingerBeam::CAirBurster_FingerBeam(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CAirBurster_FingerBeam::CAirBurster_FingerBeam(const CAirBurster_FingerBeam& rhs)
	: CBullet(rhs)
{
}

HRESULT CAirBurster_FingerBeam::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CAirBurster_FingerBeam::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	m_TimeChecker = FTimeChecker(2.f);

	return S_OK;
}

void CAirBurster_FingerBeam::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	auto pCloudInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner, PLAYER_TYPE::CLOUD);
	auto pAerithInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner, PLAYER_TYPE::AERITH);

	_vector vLook = m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_LOOK);
	_vector vCloudLook = pCloudInfo.vDirToTarget;
	_vector vAerithLook = pAerithInfo.vDirToTarget;

	if (XMVectorGetX(XMVector3Dot(XMVector3Normalize(vCloudLook), XMVector3Normalize(vLook))) > 0.f)
	{
		//클라우드가 보스 방향에 있다.
		if (pCloudInfo.pTarget.lock() != nullptr)
		{
			_vector vDir = (XMVector3Normalize(pCloudInfo.vDirToTarget));
			_vector vResultPos = m_pOwner.lock()->Get_TransformCom().lock()
				->Get_State(CTransform::STATE_POSITION)+ vDir;
			vResultPos.m128_f32[3] = 1.f;
			_vector vResultLook = XMVector3Normalize(vResultPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pTransformCom->Set_Look_Manual(vResultLook);

			m_eTargetType = PLAYER_TYPE::CLOUD;
		}	
	}
	else {
		//에어리스가 보스 방향에 있다.
		if (pAerithInfo.pTarget.lock() != nullptr)
		{
			_vector vDir = (-XMVector3Normalize(pAerithInfo.vDirToTarget)) * 5.f;
			_vector vResultPos = pAerithInfo.vTargetPos + vDir;// + vTargetRanPos;
			vResultPos.m128_f32[3] = 1.f;
			_vector vResultLook = XMVector3Normalize(vResultPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pTransformCom->Set_Look_Manual(vResultLook);

			m_eTargetType = PLAYER_TYPE::AERITH;
		}
	}

	m_pTransformCom->Set_Speed(1.f);

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 0.2f, 0.2f, 0.2f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	m_pEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("GRP_AirBursterFingerBeam"), shared_from_this());

	m_fLifeTime = { 0.7f };
}

void CAirBurster_FingerBeam::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAirBurster_FingerBeam::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_pOwner.lock())
	{
		_matrix MuzzleMatrix = m_pOwner.lock()->Get_ModelCom().lock()->
			Get_BoneTransformMatrixWithParents(m_strBoneName);
		m_pTransformCom->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);

		if (m_eTargetType != PLAYER_TYPE::PLAYER_END)
		{
			auto pTargetInfo = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(weak_from_this(), m_eTargetType);
			m_pTransformCom->Look_At(pTargetInfo.vTargetPos, 0.2f);
		}
	}
}

void CAirBurster_FingerBeam::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	PxRaycastHit hit;
	if (GET_SINGLE(CPhysX_Manager)->RaycastSingle(shared_from_this(),CL_MONSTER_ATTACK, m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 1000.f, hit))
	{
		if (hit.actor->userData != nullptr)
		{
			CPhysX_Collider* pCollider = (CPhysX_Collider*)hit.actor->userData;
			int a = 0;

			Status_DamageTo(m_strSkillName, pCollider, pCollider->Get_Owner(), Get_Owner());

		}
	} // 레이 충돌

	if (m_pEffect.lock() == nullptr || m_pEffect.lock()->Get_IsEffectDead() || m_fLifeTime.Increase(fTimeDelta))
		Set_Dead();
}

void CAirBurster_FingerBeam::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CAirBurster_FingerBeam::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CAirBurster_FingerBeam::Render()
{
	return S_OK;
}

HRESULT CAirBurster_FingerBeam::Ready_Components(void* pArg)
{
	return S_OK;
}

void CAirBurster_FingerBeam::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_FingerBeam::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_FingerBeam::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CAirBurster_FingerBeam::Free()
{
	__super::Free();
}
