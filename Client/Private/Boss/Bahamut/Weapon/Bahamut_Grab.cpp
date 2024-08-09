#include "stdafx.h"
#include "Boss/Bahamut/Weapon/Bahamut_Grab.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Effect_Manager.h"

#include "Boss/Bahamut/State_List_Bahamut.h"
#include "State_List_Cloud.h"
#include "Aerith/State/State_List_Aerith.h"
#include "Player.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CBahamut_Grab)
IMPLEMENT_CLONE(CBahamut_Grab, CGameObject)
CBahamut_Grab::CBahamut_Grab(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CBahamut_Grab::CBahamut_Grab(const CBahamut_Grab& rhs)
	: CBullet(rhs)
{
}

HRESULT CBahamut_Grab::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut_Grab::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBahamut_Grab::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::SPHERE, CL_MONSTER_ATTACK, m_pTransformCom, { 2.f,2.f,2.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	Set_StatusComByOwner("Bahamut_Grab");

	// [여기에] 트레일 이펙트 생성 넣어주세요.
}

void CBahamut_Grab::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut_Grab::Tick(_cref_time fTimeDelta)
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

void CBahamut_Grab::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

}

void CBahamut_Grab::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBahamut_Grab::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBahamut_Grab::Render()
{
	return S_OK;
}

HRESULT CBahamut_Grab::Ready_Components(void* pArg)
{
	return S_OK;
}

void CBahamut_Grab::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		m_bGrabCol = true; // 그랩 성공시 다음 애니메이션 재생을 위함

		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"020_SE_Bahamut (B_AtkKama01_hand).wav", ESoundGroup::Narration, 0.5f);

		m_eCol_PlayerType = DynPtrCast<CPlayer>(pOtherCol->Get_Owner().lock())->Get_PlayerType();

	}
}

void CBahamut_Grab::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_Grab::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CBahamut_Grab::Free()
{
	__super::Free();
}
