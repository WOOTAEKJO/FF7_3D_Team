#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_Parts.h"
#include "GameInstance.h"

#include "Boss/AirBurster/State_List_AirBurster.h"

CAirBurster_Parts::CAirBurster_Parts(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CBoss_Parts(pDevice, pContext)
{
}

CAirBurster_Parts::CAirBurster_Parts(const CAirBurster_Parts& rhs)
    : CBoss_Parts(rhs)
{
}

HRESULT CAirBurster_Parts::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAirBurster_Parts::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    if (FAILED(Ready_Components(pArg)))
        RETURN_EFAIL;

    return S_OK;
}

void CAirBurster_Parts::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

}

void CAirBurster_Parts::Priority_Tick(_cref_time fTimeDelta)
{
    if (!m_bMediate && m_bIndependent && m_pPhysXControllerCom)
    {
        if (!m_bIndependentStart)
        {
            m_bIndependentStart = true;
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            m_pTransformCom->Set_Position(1.f, vPos);
		}

		m_pPhysXControllerCom->Synchronize_Transform(m_pTransformCom, m_vPhysXControllerLocalOffset, m_vPhysXControllerWorldOffset);
    }

   
    __super::Priority_Tick(fTimeDelta);

    Judge_Dead();
}

void CAirBurster_Parts::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);

}

void CAirBurster_Parts::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

	if (m_bIndependent && m_pPhysXControllerCom)
	{
        m_pPhysXControllerCom->Synchronize_Controller(m_pTransformCom, {0.f,-1.f,0.f});
	}
}

void CAirBurster_Parts::Before_Render(_cref_time fTimeDelta)
{
    if (!m_bIndependent)
    {
        if (!m_bMediate)
            m_bMediate = true;

        if (m_bIndependentStart)
            m_bIndependentStart = false;

        __super::Before_Render(fTimeDelta);
    }  
    else
    {
        if (m_bMediate)
        {
            m_bMediate = false;
            m_pTransformCom->Set_WorldFloat4x4(m_WorldMatrix);
        }
        else
            XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
        
        if (m_pPhysXColliderCom)
        {
            m_pPhysXColliderCom->Synchronize_Collider(m_WorldMatrix, m_vPhysXColliderLocalOffset, m_vPhysXColliderWorldOffset);
        }

		if (m_pPhysXColliderCom_Block)
			m_pPhysXColliderCom_Block->Synchronize_Collider(m_pTransformCom, m_vPhysXColliderLocalOffset, m_vPhysXColliderWorldOffset);

        if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
            return;

        //독립 파츠는 자기의 월드행렬을 사용한다.
        //독립 파츠일 때만 렌더 On
        //OBJSTATE로 렌더 상태를 제어하면 이 함수 자체를 타지 않음. BOOL 변수로 제어해주자.
    }

}

void CAirBurster_Parts::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);

    m_pTransformCom->Set_Scaling(0.1f, 0.1f, 0.1f);
}

HRESULT CAirBurster_Parts::Render()
{
    if (!m_pModelCom)
        return E_FAIL;

    auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
    for (_uint i = 0; i < ActiveMeshes.size(); ++i)
    {
        // 뼈 바인딩
        if (FAILED(m_pModelCom->Bind_BoneToShader(ActiveMeshes[i], "g_BoneMatrices")))
            RETURN_EFAIL;

        // 텍스처 바인딩
        if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE, "g_DiffuseTexture")))
            RETURN_EFAIL;
        if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_NORMALS, "g_NormalTexture")))
            RETURN_EFAIL;

        // 패스, 버퍼 바인딩
        if (FAILED(m_pModelCom->ShaderComp()->Begin(m_iShaderPassIndex)))
            RETURN_EFAIL;

        //버퍼 렌더
        if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
            RETURN_EFAIL;
    }

    return S_OK;
}

_bool CAirBurster_Parts::Judge_Dead()
{
	if (m_pStatusCom->Get_HP_Percent() <= 0.f)
	{
        return m_bPartsDead = true;
	}

    return false;
}

HRESULT CAirBurster_Parts::Ready_Components(void* pArg)
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
        TEXT("Com_StateMachineCom"), &m_pStateMachineCom)))
        RETURN_EFAIL;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_AnimCommonModel"),
        TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CAirBurster_Parts::Bind_ShaderResources()
{
    // 공통 행렬 바인딩
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
        RETURN_EFAIL;
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
        RETURN_EFAIL;

    // 월드 행렬 바인딩
    if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        RETURN_EFAIL;

    return S_OK;
}

void CAirBurster_Parts::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
    __super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CAirBurster_Parts::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
    __super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CAirBurster_Parts::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
    __super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CAirBurster_Parts::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == COLLIDER_LAYER::CL_PLAYER_ATTACK)
	{
        auto pCurState = m_pStateMachineCom->Get_CurState();

        m_pStateMachineCom->Set_State<CState_AirBurster_Arm_Push_Back>();
	}
}

void CAirBurster_Parts::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_Parts::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_Parts::onControllerHit(CPhysX_Controller* pOtherController, const PxControllersHit& hit)
{
	// 컨트롤끼리 충돌했을 경우
	if (pOtherController->Get_ControllerDesc().iFilterType == CL_PLAYER_CONTROLLER)
	{
		CState* pCurState = m_pStateMachineCom->Get_CurState();

		if (typeid(*pCurState) == typeid(CState_AirBurster_Arm_Push))
		{
			pOtherController->Get_Owner().lock()->Get_TransformCom().lock()->Add_Position(XMVectorSetW(m_vMovement_Amount, 1.f));
			// 이동량을 더함.
		}
	}
}

void CAirBurster_Parts::Free()
{
    __super::Free();
}
