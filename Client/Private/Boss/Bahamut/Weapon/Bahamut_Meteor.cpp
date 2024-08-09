#include "stdafx.h"
#include "Boss/Bahamut/Weapon/Bahamut_Meteor.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Effect_Manager.h"

IMPLEMENT_CREATE(CBahamut_Meteor)
IMPLEMENT_CLONE(CBahamut_Meteor, CGameObject)
CBahamut_Meteor::CBahamut_Meteor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	: CBullet(pDevice, pDeviceContext)
{
}

CBahamut_Meteor::CBahamut_Meteor(const CBahamut_Meteor& rhs)
	: CBullet(rhs)
{
}

HRESULT CBahamut_Meteor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CBahamut_Meteor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components(pArg)))
		RETURN_EFAIL;

	m_pTransformCom->Set_Scaling(0.1f, 0.1f, 0.1f);

	m_TimeChecker_Tail = FTimeChecker(0.1f);

	return S_OK;
}

void CBahamut_Meteor::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);	
	
	m_pModelCom->Transform().Rotation(XMVectorSet(1.f,0.f,0.f,0.f),XMConvertToRadians(-90.f));
	PHYSXCOLLIDER_DESC ColliderDesc = {};
	PhysXColliderDesc::Setting_DynamicCollider_WithScale(ColliderDesc,
		PHYSXCOLLIDER_TYPE::BOX, CL_MONSTER_ATTACK, m_pTransformCom, { 10.f, 10.f, 30.f }, false, nullptr, true);
	m_vPhysXColliderLocalOffset = { 0.f,0.f,0.f };

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_PhysX_Collider"),
		TEXT("Com_PhysXColliderCom"), &(m_pPhysXColliderCom), &ColliderDesc)))
		return;

	GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Bahamut_MegaFlare_Meteor_Always", shared_from_this());

	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	float3 vOffset = vLook * 10.f;
	m_pLight = m_pGameInstance->Make_Light_On_Owner(weak_from_this(), _float4(1.f, 0.6f, 0.159f, 1.f), 30.f, vOffset);
}

void CBahamut_Meteor::Priority_Tick(_cref_time fTimeDelta)
{ 
	__super::Priority_Tick(fTimeDelta);
}

void CBahamut_Meteor::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_TimeChecker_Tail.Update(fTimeDelta))
	{
		shared_ptr<CParticle> pParticle[3] = {};

		pParticle[0] = GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Bahamut_MegaFlare_Meteor_Always_Tail4_Time", shared_from_this(),
			CEffect::USE_FOLLOW_NORMAL);
		pParticle[1] = GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Bahamut_MegaFlare_Meteor_Always_Tail3_Time", shared_from_this(),
			CEffect::USE_FOLLOW_NORMAL);
		pParticle[2] = GET_SINGLE(CEffect_Manager)->Create_Effect<CParticle>(L"Bahamut_MegaFlare_Meteor_Always_Tail5_Time", shared_from_this(),
			CEffect::USE_FOLLOW_NORMAL);

		for (_uint i = 0; i < 3; i++)
		{
			if(pParticle[i])
				pParticle[i]->Get_TransformCom().lock()->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
		}
	}

}

void CBahamut_Meteor::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_pGameInstance->Key_DownEx(DIK_F12, DIK_MD_LSHIFT))
	{
		if (m_bTest)
			m_bTest = false;
		else
			m_bTest = true;
	}

	if (!m_bTest)
		return;
	if (!m_pTransformCom->Go_Target(_float4(0.f, 0.f, 0.f, 1.f), fTimeDelta * 10.f, 1.f))
	{
		m_bFinished = true;
		Set_Dead();

		if (m_pLight)
			m_pGameInstance->Release_Light(m_pLight);
	}
}

void CBahamut_Meteor::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, shared_from_this())))
		return;
}

void CBahamut_Meteor::End_Play(_cref_time fTimeDelta)
{
	__super::End_Play(fTimeDelta);
}

HRESULT CBahamut_Meteor::Render()
{
	if (m_pModelCom)
	{
		if (FAILED(Bind_ShaderResources()))
			RETURN_EFAIL;

		auto ActiveMeshes = m_pModelCom->Get_ActiveMeshes();
		for (_uint i = 0; i < ActiveMeshes.size(); ++i)
		{

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
			if (FAILED(m_pModelCom->ShaderComp()->Begin(0)))
				RETURN_EFAIL;

			//버퍼 렌더
			if (FAILED(m_pModelCom->BindAndRender_Mesh(ActiveMeshes[i])))
				RETURN_EFAIL;
		}
	}
	return S_OK;
}

HRESULT CBahamut_Meteor::Ready_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_CommonModel"),
		TEXT("Com_ModelCom"), &(m_pModelCom), nullptr)))
		RETURN_EFAIL;

	if (m_pModelCom)
	{
		m_pModelCom->Link_Model(CCommonModelComp::TYPE_NONANIM, TEXT("FA0260_01"));
		if (FAILED(m_pModelCom->Link_Shader(L"Shader_Model", VTXMESH::Elements, VTXMESH::iNumElements)))
			RETURN_EFAIL;
	}

	return S_OK;
}

HRESULT CBahamut_Meteor::Bind_ShaderResources()
{
	_float4x4 matWorld = m_pModelCom->Calculate_TransformFloat4x4FromParent();

	// 월드 행렬 바인딩
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &matWorld)))
		RETURN_EFAIL;
	// 공통 행렬 바인딩
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;

	return S_OK;
}

void CBahamut_Meteor::PhysX_OnCollision_Enter(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Enter(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_Meteor::PhysX_OnCollision_Stay(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Stay(pThisCol, pOtherCol, ContactInfo);
}

void CBahamut_Meteor::PhysX_OnCollision_Exit(CPhysX_Collider* pThisCol, CPhysX_Collider* pOtherCol, const PxContactPair& ContactInfo)
{
	__super::PhysX_OnCollision_Exit(pThisCol, pOtherCol, ContactInfo);
}


void CBahamut_Meteor::Free()
{
	__super::Free();
}
