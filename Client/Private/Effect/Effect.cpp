#include "stdafx.h"
#include "Effect.h"

#include "GameInstance.h"

#include "PartObject.h"

#include "BoneContainer.h"

CEffect::CEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CBlendObject(pDevice, pContext)
{// ����̽��� ����̽� ���ؽ�Ʈ�� �޾� �ʱ�ȭ
	
}

CEffect::CEffect(const CEffect& rhs)
	: CBlendObject(rhs),m_bLoad(rhs.m_bLoad), m_matJsonWorld(rhs.m_matJsonWorld),
	m_iObjPool_MaxNum(rhs.m_iObjPool_MaxNum)
{// ���� ������
}

HRESULT CEffect::Initialize_Prototype()
{// ���� ��ü �ʱ�ȭ

	return S_OK;
}

HRESULT CEffect::Initialize_Prototype(string strFilePath)
{// ���ڷ� ���� �ּҸ� �޴� ���� ��ü �ʱ�ȭ

	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{// �纻 ��ü �ʱ�ȭ
	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	m_matWorld = m_pTransformCom->Get_WorldFloat4x4(); // ���� ����� ���� ��ķ� �ʱ�ȭ �۾�
	// ���� ��� �������� ��� ����.
	// �׳� �׵���ķ� �ʱ�ȭ �Ѵٰ� ����.

	m_pOffsetTransform = CTransform::Create(m_pDevice, m_pContext);
	if (nullptr == m_pOffsetTransform)
		RETURN_EFAIL;
	// offset ��� ����

	return S_OK;
}

void CEffect::Begin_Play(_cref_time fTimeDelta)
{// Ȱ�� ���� �� �� �� ȣ��
	__super::Begin_Play(fTimeDelta);

	if (m_pOwner.lock())
	{
		m_pOwnerTransformCom = m_pOwner.lock()->Get_TransformCom().lock();
		// ������ ���ʰ� ������ �ڱ� Ʈ������ ����� �������� �ʴ´�.
		// �� ���� ������ �ƴ� ������ ��������� ������ ���� ����� ���
	}	
}

void CEffect::Priority_Tick(_cref_time fTimeDelta)
{// �켱���� ƽ
	__super::Priority_Tick(fTimeDelta);
}

void CEffect::Tick(_cref_time fTimeDelta)
{// �Ϲ����� ƽ
	__super::Tick(fTimeDelta);
}

void CEffect::Late_Tick(_cref_time fTimeDelta)
{// ���� ƽ
	__super::Late_Tick(fTimeDelta);
}

void CEffect::Before_Render(_cref_time fTimeDelta)
{// ���� �Լ� ȣ�� ��
	__super::Before_Render(fTimeDelta);
}

HRESULT CEffect::Render()
{
	return S_OK;
}

void CEffect::Write_Json(json& Out_Json)
{

}

void CEffect::Load_Json(const json& In_Json)
{
}

void CEffect::Effect_Write_Json(json& Out_Json, EFFECT_DESC eDesc)
{// ����Ʈ ������ ���̽� ����
	Out_Json["ShaderType"] = eDesc.iShaderPassType;

	Out_Json["Discard_Alpha"] = eDesc.fDiscard_Alpha;

	Out_Json["DiffuseTexture"] = WstrToStr( eDesc.strDiffuse);
	Out_Json["MaskTexture"] = WstrToStr(eDesc.strMask);
	Out_Json["NoiseTexture"] = WstrToStr(eDesc.strNoise);

	CJson_Utility::Write_Float2(Out_Json["DiffuseUV"], eDesc.vDiffuseUV);
	CJson_Utility::Write_Float2(Out_Json["MaskUV"], eDesc.vMaskUV);
	CJson_Utility::Write_Float2(Out_Json["NoiseUV"], eDesc.vNoiseUV);

	CJson_Utility::Write_Float2(Out_Json["Size"], eDesc.vSize);
	CJson_Utility::Write_Float3(Out_Json["Rotation"], eDesc.vRotation);

	CJson_Utility::Write_Float4(Out_Json["Solid_Color"], eDesc.vSolid_Color);

	Out_Json["bSpriteAnim"] = eDesc.bSpriteAnim;
	Out_Json["bLoop"] = eDesc.bLoop;
	Out_Json["SpriteSpeed"] = eDesc.fSpeed;
	Out_Json["SpriteDuration"] = eDesc.fDuration;
	Out_Json["SpriteType"] = eDesc.iSpriteType;
	CJson_Utility::Write_Float2(Out_Json["SpriteColRow"], eDesc.vSpriteColRow);

	// --------Distortion ---------
	CJson_Utility::Write_Float3(Out_Json["DistortionValue"], eDesc.vDistortionValue);
	Out_Json["DistortionScale"] = eDesc.fDistortionScale;
	Out_Json["DistortionBias"] = eDesc.fDistortionBias;
	Out_Json["DistortionNoiseWeight"] = eDesc.fNoiseWeight;
	
	//----------Easing ----------
	Out_Json["bEasing"] = eDesc.bEasing;
	Out_Json["EasingType"] = eDesc.eEasingType;

	// -------- DeadType ------
	Out_Json["eDeadType"] = (_int)eDesc.eDeadType;

	// ------ Owner -------
	Out_Json["eUseType"] = (_int)eDesc.eUseType;

	// ------ ColorBlendType --------
	Out_Json["eColorBlendType"] = (_int)eDesc.eColorBlendType;

	// ------ SolidColor -------- 
	Out_Json["bSolidColor"] = eDesc.bSolidColor;

	// ------ ObjPool ----------
	Out_Json["iObjPool_MaxNum"] = m_iObjPool_MaxNum;
}

void CEffect::Effect_Load_Json(const json& In_Json, EFFECT_DESC* eDesc)
{// ����Ʈ ������ ���̽� �ε�
	eDesc->iShaderPassType = In_Json["ShaderType"];

	eDesc->fDiscard_Alpha = In_Json["Discard_Alpha"];

	eDesc->strDiffuse = StrToWstr(In_Json["DiffuseTexture"]);
	eDesc->strMask = StrToWstr(In_Json["MaskTexture"]);
	eDesc->strNoise = StrToWstr(In_Json["NoiseTexture"]);

	eDesc->vDiffuseUV = _float2(In_Json["DiffuseUV"][0], In_Json["DiffuseUV"][1]);
	eDesc->vMaskUV = _float2(In_Json["MaskUV"][0], In_Json["MaskUV"][1]);
	eDesc->vNoiseUV = _float2(In_Json["NoiseUV"][0], In_Json["NoiseUV"][1]);

	eDesc->vSize = _float2(In_Json["Size"][0], In_Json["Size"][1]);
	eDesc->vRotation = _float3(In_Json["Rotation"][0], In_Json["Rotation"][1], In_Json["Rotation"][2]);

	eDesc->vSolid_Color = _float4(In_Json["Solid_Color"][0], In_Json["Solid_Color"][1], In_Json["Solid_Color"][2], In_Json["Solid_Color"][3]);

	eDesc->bSpriteAnim = In_Json["bSpriteAnim"];
	eDesc->bLoop = In_Json["bLoop"];
	eDesc->fSpeed = In_Json["SpriteSpeed"];
	eDesc->fDuration = In_Json["SpriteDuration"];
	eDesc->iSpriteType = In_Json["SpriteType"];
	eDesc->vSpriteColRow = _float2(In_Json["SpriteColRow"][0], In_Json["SpriteColRow"][1]);

	// --------Distortion ---------
	eDesc->vDistortionValue = _float3(In_Json["DistortionValue"][0], In_Json["DistortionValue"][1], In_Json["DistortionValue"][2]);
	eDesc->fDistortionScale = In_Json["DistortionScale"];
	eDesc->fDistortionBias = In_Json["DistortionBias"];
	eDesc->fNoiseWeight = In_Json["DistortionNoiseWeight"];

	//----------Easing ----------
	eDesc->bEasing = In_Json["bEasing"];
	eDesc->eEasingType = In_Json["EasingType"];

	// -------- DeadType ------
	eDesc->eDeadType = (DEADTYPE)In_Json["eDeadType"];

	// ------ Owner -------
	eDesc->eUseType = (USE_TYPE)In_Json["eUseType"];

	// ------ ColorBlendType --------
	if (In_Json.contains("eColorBlendType"))
	{
		eDesc->eColorBlendType = (COLORBLEND)In_Json["eColorBlendType"];
	}

	// ------ SolidColor -------- 
	if (In_Json.contains("bSolidColor"))
	{
		eDesc->bSolidColor = In_Json["bSolidColor"];
	}

	// ------ ObjPool ----------
	if (In_Json.contains("iObjPool_MaxNum"))
		m_iObjPool_MaxNum = In_Json["iObjPool_MaxNum"];
}

HRESULT CEffect::Ready_Components()
{

	return S_OK;
}

HRESULT CEffect::Bind_ShaderResources(EFFECT_DESC EffectDesc)
{// ���̴� ���ҽ� ���ε�

	// For.Matrix
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom.get(), "g_WorldMatrix")))
		RETURN_EFAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;

	// For.TexTure
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pShaderCom.get(), "g_DiffuseTexture", MATERIALTYPE::MATERIATYPE_DIFFUSE,0))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pShaderCom.get(), "g_MaskTexture", MATERIALTYPE::MATERIATYPE_MASK, 0))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pShaderCom.get(), "g_NoiseTexture", MATERIALTYPE::MATERIATYPE_NOISE, 0))) RETURN_EFAIL;

	// For.UV
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDiffuseUV", &EffectDesc.vDiffuseUV, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vMaskUV", &EffectDesc.vMaskUV, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNoiseUV", &EffectDesc.vNoiseUV, sizeof(_float2)))) RETURN_EFAIL;

	// For.Distirtion
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vDistortionValue", &EffectDesc.vDistortionValue, sizeof(_float3)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionScale", &EffectDesc.fDistortionScale, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDistortionBias", &EffectDesc.fDistortionBias, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNoiseWeight", &EffectDesc.fNoiseWeight, sizeof(_float)))) RETURN_EFAIL;

	// For.Sprite
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bSprite", &EffectDesc.bSpriteAnim, sizeof(_bool)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iSpriteType", &EffectDesc.iSpriteType, sizeof(_int)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSpriteColRow", &EffectDesc.vSpriteColRow, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSpriteIndex", &m_vSpriteIndex, sizeof(_float2)))) RETURN_EFAIL;

	// Etc
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &EffectDesc.fDiscard_Alpha, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTimeDelta", &m_fTimeDelta, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &EffectDesc.vSolid_Color, sizeof(_float4)))) RETURN_EFAIL;

	return S_OK;
}

void CEffect::SpriteAnim(_cref_time fTimeDelta, EFFECT_DESC EffectDesc)
{// ��������Ʈ �ִϸ��̼�
	if (EffectDesc.bSpriteAnim && !m_bIsFinished)
	{
		m_fFrame += fTimeDelta * EffectDesc.fSpeed;

		if (m_fFrame >= EffectDesc.fDuration)
		{
			m_vSpriteIndex.x += 1.f; // x �ε��� ����

			if (m_vSpriteIndex.x >= EffectDesc.vSpriteColRow.x)
			{
				m_vSpriteIndex.x = 0.f;
				m_vSpriteIndex.y += 1.f; // y �ε��� ����
				if (m_vSpriteIndex.y >= EffectDesc.vSpriteColRow.y)
				{
					// �� �������� Ž
					if (EffectDesc.bLoop)
					{
						m_vSpriteIndex.y = 0.f;
					}
					else {
						m_bIsFinished = true;
						// ������ �ƴϸ� ����
					}
				}
			}
			m_fFrame = 0.f;
		}
	}

}

void CEffect::Judge_Dead(EFFECT_DESC EffectDesc)
{// ���� ���� �Ǵ�
	switch (EffectDesc.eDeadType)
	{
	case DEADTYPE::TIME:
		break;
	case DEADTYPE::SCALE:
		
		break;
	case DEADTYPE::COLOR:
		break;
	}
}

void CEffect::UpdateWorldMatrix(EFFECT_DESC EffectDesc)
{
	// ������ �⺻������ ������� ����.
	if (m_pGameInstance->Get_CreateLevelIndex() == LEVEL_TOOL) // �� ����
	{
		if(m_bApplyTransform)
			m_matWorld = m_pTransformCom->Get_WorldFloat4x4(); // ������ Ʈ������ ������������
		return;
	}

	if (m_bUseOffsetTransform)
		m_matWorld = m_pTransformCom->Get_WorldFloat4x4() * m_pOffsetTransform->Get_WorldFloat4x4(); // ������ ��� ����
	else
		m_matWorld = m_pTransformCom->Get_WorldFloat4x4();

	if (EffectDesc.eUseType == USE_TYPE::USE_NONE)
		m_matWorld *= Get_MatrixNormalize(m_matOneTImeWorld);
	else if (EffectDesc.eUseType == USE_TYPE::USE_FOLLOW_NORMAL)
	{// �Ϲ����� ��ü�� ������ ���
		if (m_pOwner.lock() == nullptr)
			return;

		if (m_pSocketBoneGroup)
		{
			m_matWorld *= Get_MatrixNormalize(m_pSocketBoneGroup->CRef_BoneCombinedTransforms()[m_iSocketBoneIndex])
				* Get_MatrixNormalize(m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix());
		}
		else
			m_matWorld *= Get_MatrixNormalize(m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix());
	}
	else if (EffectDesc.eUseType == USE_TYPE::USE_FOLLOW_PARTS)
	{// ����������Ʈ�� ������ ���
		if (m_pOwner.lock() == nullptr)
			return;

		if (m_pSocketBoneGroup)
		{
			m_matWorld *= Get_MatrixNormalize(m_pSocketBoneGroup->CRef_BoneCombinedTransforms()[m_iSocketBoneIndex])
				* Get_MatrixNormalize(static_pointer_cast<CPartObject>(m_pOwner.lock())->Get_WorldMatrix());
		}
		else
			m_matWorld *= Get_MatrixNormalize(static_pointer_cast<CPartObject>(m_pOwner.lock())->Get_WorldMatrix());
		// ������ ���� ��İ� ���� ����� �����ͼ� ������ش�.
	}
	else if (EffectDesc.eUseType == USE_TYPE::USE_FOLLOW_EFFECT)
	{// ����Ʈ�� ������ ���
		if (m_pOwner.lock() == nullptr)
			return;

		m_matWorld *= Get_MatrixNormalize(static_pointer_cast<CBlendObject>(m_pOwner.lock())->Get_matWorld());
		// ����Ʈ�� ���� ��İ� ���� ����� �����ͼ� ������ش�.
	}
}

void CEffect::Free()
{
	__super::Free();

}