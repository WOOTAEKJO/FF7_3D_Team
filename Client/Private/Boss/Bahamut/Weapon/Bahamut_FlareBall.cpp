#include "stdafx.h"
#include "Boss/Bahamut/Weapon/Bahamut_FlareBall.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Effect_Manager.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CBahamut_FlareBall)
IMPLEMENT_CLONE(CBahamut_FlareBall, CGameObject)
CBahamut_FlareBall::CBahamut_FlareBall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CBahamut_FlareBall::CBahamut_FlareBall(const CBahamut_FlareBall& rhs)
	: CBullet(rhs)
{
	m_TimeChecker = FTimeChecker(0.2f);
}

HRESULT CBahamut_FlareBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut_FlareBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBahamut_FlareBall::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::SPHERE, CL_MONSTER_ATTACK, m_pTransformCom, { 0.3f,0.3f,0.3f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(L"Bahamut_MegaFlare_Ball_Group", shared_from_this());

	m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"097_SE_Bahamut (FX_MegaFlare_Bullet_01).wav", ESoundGroup::Narration, 1.f);
}

void CBahamut_FlareBall::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut_FlareBall::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bActivateBullet)
	{
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
		else
		{
			if (!m_pTransformCom->Go_Target(XMVectorSet(0.f, 0.f, 0.f, 1.f), fTimeDelta * 10.f, 0.5f))
			{
				m_pHitEffect = GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("Bahamut_MegaFlare_Hit2"), shared_from_this());
				Set_Dead();
				m_bActivateBullet = false;

				auto pCloud = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(weak_from_this(), CLOUD).pTarget.lock();
				auto pAerith = GET_SINGLE(CClient_Manager)->Find_TargetPlayer(weak_from_this(), AERITH).pTarget.lock();

				if (pCloud)
				{
					
					Status_DamageTo(m_strSkillName, pCloud->Get_PhysXColliderCom().lock().get(), pCloud, Get_Owner());
				}
				if (pAerith)
				{
					Status_DamageTo(m_strSkillName, pAerith->Get_PhysXColliderCom().lock().get(), pCloud, Get_Owner());
				}

				m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"101_SE_Bahamut (FX_MegaFlare_Exp_01).wav", ESoundGroup::Narration, 0.5f);
				m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"104_SE_Bahamut(FX_MeteorDive_Meteo_01).wav", ESoundGroup::Narration, 0.5f);
				
			}
		}
	}
}

void CBahamut_FlareBall::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CBahamut_FlareBall::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBahamut_FlareBall::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBahamut_FlareBall::Render()
{
	return S_OK;
}

HRESULT CBahamut_FlareBall::Ready_Components(void* pArg)
{
	return S_OK;
}

void CBahamut_FlareBall::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		Set_Dead();
	}
}

void CBahamut_FlareBall::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_FlareBall::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CBahamut_FlareBall::Free()
{
	__super::Free();
}
