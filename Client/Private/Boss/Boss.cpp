#include "stdafx.h"
#include "../Public/Boss/Boss.h"

#include "GameInstance.h"

#include "../Public/Boss/Boss_Parts.h"

CBoss::CBoss(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonster(pDevice, pContext)
{
}

CBoss::CBoss(const CBoss& rhs)
	:CMonster(rhs)
{
}

HRESULT CBoss::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;
	return S_OK;
}

HRESULT CBoss::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	m_iShaderPassIndex = 0;
	m_strDissolveTextureTag = L"DissolveExplode_Fractal01_M";
	// 디졸브 텍스쳐 필요시 각 클래스에서 변경. 현재는 공용

	return S_OK;
}

void CBoss::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);
}

void CBoss::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBoss::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBoss::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CBoss::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);
}

void CBoss::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBoss::Render()
{
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

HRESULT CBoss::Ready_Components(void* pArg)
{
	return S_OK;
}

HRESULT CBoss::Bind_ShaderResources()
{
	// 공통 행렬 바인딩
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;

	// 월드 행렬 바인딩
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		RETURN_EFAIL;

	// Dissolve
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pModelCom->ShaderComp().get(), "g_DissolveTexture", MATERIALTYPE::MATERIATYPE_DISSOLVE, 0, true))) RETURN_EFAIL;

	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmout, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_fDissolveGradiationDistance", &m_fDissolveGradiationDistance, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vDissolveGradiationStartColor", &m_vDissolveGradiationStartColor, sizeof(_float3)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vDissolveGradiationEndColor", &m_vDissolveGradiationEndColor, sizeof(_float3)))) RETURN_EFAIL;

	return S_OK;
}

void CBoss::SetUp_Controller(_float fHeight, _float fRadius)
{
	if (m_pModelCom == nullptr)
		return;

	PHYSXCONTROLLER_DESC ControllerDesc = {};
	PhysXControllerDesc::Setting_Controller(ControllerDesc, m_pTransformCom, CL_MONSTER_CONTROLLER, fHeight, fRadius);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Controller"),
		TEXT("Com_PhysXControllerCom"), &(m_pPhysXControllerCom), &ControllerDesc)))
		return;

	m_pTransformCom->Set_PhysXControllerCom(m_pPhysXControllerCom);

}

void CBoss::Parts_Command(wstring wstrPartsTag, _uint iCommandIndex)
{
	shared_ptr<CPartObject> pParts = Find_PartObject(wstrPartsTag);
	if (pParts == nullptr)
		return;

	static_pointer_cast<CBoss_Parts>(pParts)->Set_Command(iCommandIndex);
}

void CBoss::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Enter(pThisCol, pOtherCol);
}

void CBoss::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Stay(pThisCol, pOtherCol);
}

void CBoss::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	__super::OnCollision_Exit(pThisCol, pOtherCol);
}

void CBoss::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CBoss::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBoss::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}

void CBoss::Free()
{
	__super::Free();
}