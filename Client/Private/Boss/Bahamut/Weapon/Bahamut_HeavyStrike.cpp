#include "stdafx.h"
#include "Boss/Bahamut/Weapon/Bahamut_HeavyStrike.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Effect_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CBahamut_HeavyStrike)
IMPLEMENT_CLONE(CBahamut_HeavyStrike, CGameObject)
CBahamut_HeavyStrike::CBahamut_HeavyStrike(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CBahamut_HeavyStrike::CBahamut_HeavyStrike(const CBahamut_HeavyStrike& rhs)
	: CBullet(rhs)
{
}

HRESULT CBahamut_HeavyStrike::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut_HeavyStrike::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBahamut_HeavyStrike::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,
		PHYSXCOLLIDER_TYPE::SPHERE, CL_MONSTER_ATTACK, m_pTransformCom, { 1.f, 1.f, 1.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	Set_StatusComByOwner("Bahamut_HeavyStrike");

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	shared_ptr<CEffect> pEffect1 = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(L"Bahamut_HeavyStrike", shared_from_this());

	m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"093_SE_Bahamut (FX_ImpuluseZero_Exp_01).wav", ESoundGroup::Narration, 0.5f);
}

void CBahamut_HeavyStrike::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut_HeavyStrike::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fAccTime += fTimeDelta;

	if (!m_bIndependent)
	{
		if (m_pOwner.lock())
		{
			_matrix MuzzleMatrix = m_pOwner.lock()->Get_ModelCom().lock()->
				Get_BoneTransformMatrixWithParents(m_strBoneName);
			m_pTransformCom->Set_Position(fTimeDelta, MuzzleMatrix.r[3]);
			// 위치가 항상 뼈의 영향을 받는다.
		}
	}
	else {
		auto pTarget = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(m_pOwner);

		_float3 vMediatePos = pTarget.vTargetPos;
		vMediatePos.y += 1.f;

		_vector vOwnerPos = m_pOwner.lock()->Get_TransformCom().lock()->Get_State(CTransform::STATE_POSITION);
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float fMaxLength = XMVector3Length(vOwnerPos - vMediatePos).m128_f32[0];
		// 오너와 타겟과의 거리
		_float fCurLength = XMVector3Length(vOwnerPos - vPos).m128_f32[0];
		// 오너와 총알과의 거리

		_float fChaseValueSpeed = (fCurLength / fMaxLength) * 10.f; // 거리에 따른 비율

		m_fAccChaseValue += fTimeDelta * 0.018f * 10.f * fChaseValueSpeed; // 회전속도
		m_fSpeed += fTimeDelta * fChaseValueSpeed * 2.f; // 가속도++

		m_pTransformCom->Look_At(vMediatePos, m_fAccChaseValue);
		// 점점 타겟을 바라보게 됨

		m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
	}
}

void CBahamut_HeavyStrike::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CBahamut_HeavyStrike::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBahamut_HeavyStrike::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBahamut_HeavyStrike::Render()
{
	return S_OK;
}

HRESULT CBahamut_HeavyStrike::Ready_Components(void* pArg)
{
	return S_OK;
}

void CBahamut_HeavyStrike::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());
		
		GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("Bahamut_FireBall_Hit"), shared_from_this());
		
		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"090_SE_Bahamut (FX_Impulse_Hit_01).wav", ESoundGroup::Narration, 0.5f);

		Set_Dead();
	}
}

void CBahamut_HeavyStrike::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_HeavyStrike::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CBahamut_HeavyStrike::Free()
{
	__super::Free();
}
