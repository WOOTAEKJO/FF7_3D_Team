#include "stdafx.h"
#include "Boss/Bahamut/Weapon/Bahamut_FireBall.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Effect_Manager.h"

#include "Boss/Bahamut/State_List_Bahamut.h"
#include "State_List_Cloud.h"
#include "Aerith/State/State_List_Aerith.h"
#include "Player.h"
#include "Sound_Manager.h"

IMPLEMENT_CREATE(CBahamut_FireBall)
IMPLEMENT_CLONE(CBahamut_FireBall, CGameObject)
CBahamut_FireBall::CBahamut_FireBall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CBahamut_FireBall::CBahamut_FireBall(const CBahamut_FireBall& rhs)
	: CBullet(rhs)
{
	m_TimeChecker = FTimeChecker(0.2f);
}

HRESULT CBahamut_FireBall::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut_FireBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	return S_OK;
}

void CBahamut_FireBall::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	

	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,
		PHYSXCOLLIDER_TYPE::SPHERE, CL_MONSTER_ATTACK, m_pTransformCom, { 1.f,1.f,1.0f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	if (m_pPhysXColliderCom)
		m_pPhysXColliderCom->PutToSleep();

	Set_StatusComByOwner("Bahamut_FireBall");

	// [여기에] 트레일 이펙트 생성 넣어주세요.
	GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(L"Bahamut_FireBall", shared_from_this());
}

void CBahamut_FireBall::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut_FireBall::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

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
		m_pTransformCom->Go_Straight(fTimeDelta * 10.f);
}

void CBahamut_FireBall::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_bIndependent)
	{
		if (m_TimeChecker.Update(fTimeDelta))
			Set_Dead();
	}
}

void CBahamut_FireBall::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBahamut_FireBall::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBahamut_FireBall::Render()
{
	return S_OK;
}

HRESULT CBahamut_FireBall::Ready_Components(void* pArg)
{
	return S_OK;
}

void CBahamut_FireBall::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == CL_PLAYER_BODY)
	{
		Status_DamageTo(m_strSkillName, pOtherCol, pOtherCol->Get_Owner(), pThisCol->Get_Owner());

		GET_SINGLE(CEffect_Manager)->Create_Effect<CEffect_Group>(TEXT("Bahamut_FireBall_Hit"), shared_from_this());

		m_pGameInstance->Play_Sound(L"FF7_Bahamut_Effect", L"093_SE_Bahamut (FX_ImpuluseZero_Exp_01).wav", ESoundGroup::Narration, 0.5f);

		auto pState = m_pOwner.lock()->Get_StateMachineCom().lock()->Get_CurState();

		if (typeid(*pState) == typeid(CState_Bahamut_Grab))
		{
			if (DynPtrCast<CPlayer>(pOtherCol->Get_Owner().lock())->Get_PlayerType() == Client::CLOUD)
				pOtherCol->Get_Owner().lock()->Get_StateMachineCom().lock()->Set_State<CState_Cloud_Idle>();
			else
				pOtherCol->Get_Owner().lock()->Get_StateMachineCom().lock()->Set_State<CState_Aerith_Idle>();

			pOtherCol->Get_Owner().lock()->Get_PhysXControllerCom().lock()->Set_Gravity(true);
		} // 강제로 상태이상.

		Set_Dead();
	}
}

void CBahamut_FireBall::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_FireBall::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CBahamut_FireBall::Free()
{
	__super::Free();
}
