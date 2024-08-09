#include "stdafx.h"
#include "Sprite.h"

#include "GameInstance.h"

IMPLEMENT_CLONE(CSprite, CGameObject)
IMPLEMENT_CREATE(CSprite)
IMPLEMENT_CREATE_EX1(CSprite,const string&,strFilePath)

CSprite::CSprite(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CEffect(pDevice, pContext)
{
	
}

CSprite::CSprite(const CSprite& rhs)
	: CEffect(rhs),m_tEffect_Desc(rhs.m_tEffect_Desc)
{
}

HRESULT CSprite::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSprite::Initialize_Prototype(string strFilePath)
{
	json In_Json;
	CJson_Utility::Load_Json(strFilePath.c_str(), In_Json);

	Load_Json(In_Json["GameObject"]);

	m_bLoad = true;

	return S_OK;
}

HRESULT CSprite::Initialize(void* pArg)
{
	if (!m_bLoad)
	{
		m_tEffect_Desc = *(EFFECT_RECT_DESC*)pArg;
	}

	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components()))
		RETURN_EFAIL;

	m_matWorld = m_pTransformCom->Get_WorldFloat4x4();

	return S_OK;
}

void CSprite::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	if (!m_bLoad)
	{
		m_pTransformCom->Set_Scaling(m_tEffect_Desc.vSize.x, m_tEffect_Desc.vSize.y, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE::STATE_POSITION,
			XMVectorSetW(XMLoadFloat3(&m_tEffect_Desc.vInitialPosition), 1.f));
		m_pTransformCom->Rotation(m_tEffect_Desc.vRotation.x, m_tEffect_Desc.vRotation.y, m_tEffect_Desc.vRotation.z);
		// 카메라 빌보드 때문에 적용이 안됨
	}

	m_pTransformCom->Look_At(XMLoadFloat4(&m_pGameInstance->Get_CamPosition()));

}

void CSprite::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

	SpriteAnim(fTimeDelta, m_tEffect_Desc);
}

void CSprite::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	UpdateWorldMatrix(m_tEffect_Desc);
}

void CSprite::Before_Render(_cref_time fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND_EFFECT, shared_from_this())))
		return;
}

HRESULT CSprite::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	m_pShaderCom->Begin(m_tEffect_Desc.iShaderPassType);

	m_pVIBufferCom->Bind_VIBuffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CSprite::Reset_Effect(_bool bGroup)
{
	if (!bGroup && !IsState(OBJSTATE::Active))
		TurnOn_State(OBJSTATE::Active);

	Reset_Frame();
}

void CSprite::Write_Json(json& Out_Json)
{
	Effect_Write_Json(Out_Json, m_tEffect_Desc);
	CGameObject::Write_Json(Out_Json);
}

void CSprite::Load_Json(const json& In_Json)
{
	Effect_Load_Json(In_Json, &m_tEffect_Desc);
	CGameObject::Load_Json(In_Json);
}

HRESULT CSprite::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Shader_Effect_Rect"),
		TEXT("Com_Shader"), & (m_pShaderCom))))
		RETURN_EFAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), & (m_pVIBufferCom))))
		RETURN_EFAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), &(m_pMaterialCom))))
		RETURN_EFAIL;

	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_DIFFUSE,
		m_tEffect_Desc.strDiffuse, 1))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_MASK,
		m_tEffect_Desc.strMask, 1))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_NOISE,
		m_tEffect_Desc.strNoise, 1))) RETURN_EFAIL;

	return S_OK;
}

HRESULT CSprite::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources(m_tEffect_Desc)))
		RETURN_EFAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom.get(), "g_DepthTexture")))
		RETURN_EFAIL;

	return S_OK;
}

void CSprite::Free()
{
	__super::Free();
}