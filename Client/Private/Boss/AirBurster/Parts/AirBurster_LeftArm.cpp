#include "stdafx.h"
#include "../Public/Boss/AirBurster/Parts/AirBurster_LeftArm.h"
#include "GameInstance.h"

#include "../Public/Boss/AirBurster/State_List_AirBurster.h"

IMPLEMENT_CREATE(CAirBurster_LeftArm)
IMPLEMENT_CLONE(CAirBurster_LeftArm, CGameObject)

CAirBurster_LeftArm::CAirBurster_LeftArm(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CAirBurster_Parts(pDevice, pContext)
{
}

CAirBurster_LeftArm::CAirBurster_LeftArm(const CAirBurster_LeftArm& rhs)
    : CAirBurster_Parts(rhs)
{
}

HRESULT CAirBurster_LeftArm::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAirBurster_LeftArm::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        RETURN_EFAIL;

    if (FAILED(Ready_Components(pArg)))
        RETURN_EFAIL;

    if (FAILED(Ready_State()))
        RETURN_EFAIL;

    m_iShaderPassIndex = 0;

    return S_OK;
}

void CAirBurster_LeftArm::Begin_Play(_cref_time fTimeDelta)
{
    __super::Begin_Play(fTimeDelta);

    // 컨트롤러

    //Transform에 기록된 크기, 위치, 회전값으로 BOX 생성
    PHYSXCOLLIDER_DESC ColliderDesc = {};
    PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,
        PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_BODY, m_pTransformCom, _float3(1.5f, 1.5f, 2.f), false, nullptr, true);
    m_vPhysXColliderLocalOffset = { 0.f,0.0f,0.0f };

    m_vPhysXColliderLocalOffset += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
    
    if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_PhysX_Collider"),
        TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
        return;

    PhysXColliderDesc::Setting_StaticCollider_WithScale(ColliderDesc, PHYSXCOLLIDER_TYPE::BOX, CL_MAP_STATIC, m_pTransformCom, _float3(5.f, 2.f, 2.f));
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom_Block"), &(m_pPhysXColliderCom_Block), &ColliderDesc)))
		return;

    m_pTransformCom->Rotation(_float4(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

// 스탯 설정
	m_pStatusCom->Init_StatsByTable(CStatusComp::ETableType::Boss, "AirBurrrrster_LeftArm");
    // 물리 타입

    m_pGameInstance->Add_Object(m_pGameInstance->Get_CurrentLevelIndex(), L_MONSTER, shared_from_this());
    // 몬스터 레이어 추가
}

void CAirBurster_LeftArm::Priority_Tick(_cref_time fTimeDelta)
{
    __super::Priority_Tick(fTimeDelta);
	
}

void CAirBurster_LeftArm::Tick(_cref_time fTimeDelta)
{
    __super::Tick(fTimeDelta);

    cout << "에어버스터 팔 현재 체력 : " + to_string(m_pStatusCom->Get_CurHP()) << endl;
}

void CAirBurster_LeftArm::Late_Tick(_cref_time fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

void CAirBurster_LeftArm::Before_Render(_cref_time fTimeDelta)
{
    __super::Before_Render(fTimeDelta);

}

void CAirBurster_LeftArm::End_Play(_cref_time fTimeDelta)
{
    __super::End_Play(fTimeDelta);
}

HRESULT CAirBurster_LeftArm::Render()
{
    if (!m_pModelCom)
        return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        RETURN_EFAIL;

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
        if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_DIFFUSE_ROUGHNESS, "g_MRVTexture")))
            RETURN_EFAIL;
        if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_EMISSION_COLOR, "g_EmissiveTexture")))
            RETURN_EFAIL;
        if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_AMBIENT_OCCLUSION, "g_AOTexture")))
            RETURN_EFAIL;
        if (FAILED(m_pModelCom->Bind_MeshMaterialToShader(ActiveMeshes[i], TextureType_OPACITY, "g_AlphaTexture")))
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

HRESULT CAirBurster_LeftArm::Ready_Components(void* pArg)
{

    if (m_pModelCom)
    {
        m_pModelCom->Link_Model(CCommonModelComp::TYPE_ANIM, TEXT("EB0002_02_AirBuster_LeftArm"));
        if (FAILED(m_pModelCom->Link_Shader(L"Shader_AnimModel", VTXMESHANIM::Elements, VTXMESHANIM::iNumElements)))
            RETURN_EFAIL;

        m_pModelCom->Set_PreRotate(
            XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(90.f)));
    }

	// 스테이터스 컴포넌트
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"),
		TEXT("Com_StatusCom"), &(m_pStatusCom), nullptr)))
		RETURN_EFAIL;

    return S_OK;
}

HRESULT CAirBurster_LeftArm::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        RETURN_EFAIL;

    return S_OK;
}

HRESULT CAirBurster_LeftArm::Ready_State()
{
    m_pStateMachineCom->Add_State<CState_AirBurster_Arm_IDLE>();
    m_pStateMachineCom->Add_State<CState_AirBurster_Arm_Separate>();
    m_pStateMachineCom->Add_State<CState_AirBurster_Arm_Docking>();
    m_pStateMachineCom->Add_State<CState_AirBurster_Arm_FingerBeam>();
    m_pStateMachineCom->Add_State<CState_AirBurster_Arm_Dead>();
    m_pStateMachineCom->Add_State<CState_AirBurster_Arm_Term>();
    m_pStateMachineCom->Add_State<CState_AirBurster_Arm_Push>();
    m_pStateMachineCom->Add_State<CState_AirBurster_Arm_Push_Back>();
    m_pStateMachineCom->Add_State<CState_AirBurster_Arm_Return>();

    m_pStateMachineCom->Set_State<CState_AirBurster_Arm_IDLE>();

    return S_OK;
}

void CAirBurster_LeftArm::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
    __super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CAirBurster_LeftArm::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
    __super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CAirBurster_LeftArm::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
    __super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CAirBurster_LeftArm::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);

	if (pOtherCol->Get_ColliderDesc().iFilterType == COLLIDER_LAYER::CL_PLAYER_ATTACK)
	{
		
	}
}

void CAirBurster_LeftArm::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
    __super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_LeftArm::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CAirBurster_LeftArm::onControllerHit(CPhysX_Controller* pOtherController, const PxControllersHit& hit)
{
    __super::onControllerHit(pOtherController, hit);
}

void CAirBurster_LeftArm::Free()
{
    __super::Free();
}
