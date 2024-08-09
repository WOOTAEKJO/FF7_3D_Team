#include "stdafx.h"
#include "../Public/Boss/Boss_Parts.h"
#include "GameInstance.h"
#include "Common_Shield.h"

CBoss_Parts::CBoss_Parts(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CPartObject(pDevice, pContext)
{
}

CBoss_Parts::CBoss_Parts(const CBoss_Parts& rhs)
    : CPartObject(rhs)
{
}

HRESULT CBoss_Parts::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoss_Parts::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    return S_OK;
}

void CBoss_Parts::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

}

void CBoss_Parts::Priority_Tick(_cref_time fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);

    m_pStateMachineCom->Priority_Tick(fTimeDelta);

}

void CBoss_Parts::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);

    m_pStateMachineCom->Tick(fTimeDelta);

}

void CBoss_Parts::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pStateMachineCom->Late_Tick(fTimeDelta);

    m_pModelCom->Tick_AnimTime(fTimeDelta);
    m_pTransformCom->Move_To_AnimationPos(m_pModelCom, fTimeDelta);
}

void CBoss_Parts::Before_Render(_cref_time fTimeDelta)
{
    __super::Before_Render(fTimeDelta);
}

void CBoss_Parts::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);
}

HRESULT CBoss_Parts::Render()
{
 
    return S_OK;
}

HRESULT CBoss_Parts::Ready_Components(void* pArg)
{
  
    return S_OK;
}

HRESULT CBoss_Parts::Bind_ShaderResources()
{

    return S_OK;
}

void CBoss_Parts::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CBoss_Parts::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CBoss_Parts::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CBoss_Parts::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CBoss_Parts::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CBoss_Parts::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
}

void CBoss_Parts::Status_Damaged(_uint iStatusDamaged, _uint iHitPower, _uint iAddHitPower)
{
    if (iStatusDamaged & ECast(EStatusModified::Block))
    {
        shared_ptr<CCommon_Shield> pBullet = { nullptr };
        m_pGameInstance->Add_CloneObject(m_pGameInstance->Get_CreateLevelIndex(), L_OBJECT,
            TEXT("Prototype_GameObject_Common_Shield"), nullptr, &pBullet);
        pBullet->Set_Owner(shared_from_this());
        pBullet->Get_TransformCom().lock()->Set_Scaling(3.f, 3.f, 3.f);
        DynPtrCast<CCommon_Shield>(pBullet)->Set_Offset({ 0.f, 1.f, 0.f });
    }
    else if (iStatusDamaged & ECast(EStatusModified::Damage))
    {
        if (iAddHitPower == 1)
        {
            auto pAnimComp = m_pModelCom->Get_AnimationComponent();

            pAnimComp->Set_ADD_Animation(0, TEXT("Main|B_Dmg01_Add"), TEXT("C_Hip_a"), {}, 1, (_float)iAddHitPower * 2.f, 4);
        }
    }
}

void CBoss_Parts::Free()
{
    __super::Free();
}
