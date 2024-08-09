#include "stdafx.h"
#include "Trail_Buffer.h"

#include "GameInstance.h"
#include "PartObject.h"

#include "BoneContainer.h"

IMPLEMENT_CLONE(CTrail_Buffer, CGameObject)
IMPLEMENT_CREATE_EX1(CTrail_Buffer, string, strFilePath)
IMPLEMENT_CREATE(CTrail_Buffer)


CTrail_Buffer::CTrail_Buffer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CEffect(pDevice, pContext)
{

}

CTrail_Buffer::CTrail_Buffer(const CTrail_Buffer& rhs)
	: CEffect(rhs),m_tTrailBuffer_Desc(rhs.m_tTrailBuffer_Desc)
{
}

HRESULT CTrail_Buffer::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CTrail_Buffer::Initialize_Prototype(string strFilePath)
{
	json In_Json;
	CJson_Utility::Load_Json(strFilePath.c_str(), In_Json);

	Load_Json(In_Json["GameObject"]);

	m_bLoad = true;

	return S_OK;
}


HRESULT CTrail_Buffer::Initialize(void* pArg)
{
	if (!m_bLoad)
	{
		m_tTrailBuffer_Desc = (*(TRAILBUFFER_DESC*)pArg);

		if (pArg != nullptr)
			m_pOwner = (*(TRAILBUFFER_DESC*)pArg).pOwner;
	}

	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components()))
		RETURN_EFAIL;



	if (m_bLoad)
	{
		m_pTransformCom->Set_WorldFloat4x4(m_matJsonWorld);
		// 월드행렬 로드
	}
	
	m_matWorld = m_pTransformCom->Get_WorldMatrix();
	// 초기 저장

	return S_OK;
}

void CTrail_Buffer::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	if (m_bLoad)
	{
		if (!m_pOwner.expired())
		{
			auto pModelCom = m_pOwner.lock()->Get_ModelCom().lock();
			if (pModelCom)
			{
				auto pBoneGroup = pModelCom->Get_BoneGroup();
				if (pBoneGroup)
					m_tTrailBuffer_Desc.pBoneGroup = pBoneGroup;
			}
			// 파싱으로 불러오면 오너의 뼈 그룹을 가져와서 사용한다.
			// begineplay에 있어야함. -> 오너를 Initialize 이후에 채워 넣는 경우가 많기 때문.
		}
	}

	if (m_tTrailBuffer_Desc.pBoneGroup)
		m_iBoneIndex = m_tTrailBuffer_Desc.pBoneGroup->Find_BoneData(m_tTrailBuffer_Desc.strBoneName)->iID;
}

void CTrail_Buffer::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_fTimeDelta += fTimeDelta;

	Update_WorldMatrix();
}

void CTrail_Buffer::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CTrail_Buffer::Late_Tick(_cref_time fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Judge_Dead(m_tTrailBuffer_Desc);
}

void CTrail_Buffer::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (m_bParticleDead)
		return;

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND_EFFECT, shared_from_this())))
		return;
}

HRESULT CTrail_Buffer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	m_pShaderCom->Begin(m_tTrailBuffer_Desc.iShaderPassType);

	m_pVIBufferCom->Bind_VIBuffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CTrail_Buffer::Update(_fmatrix matOwner)
{
	if (!m_tTrailBuffer_Desc.bUpdate)
		return;

	if (m_pVIBufferCom != nullptr)
		static_pointer_cast<CVIBuffer_Trail>(m_pVIBufferCom)->Update(matOwner);
}

void CTrail_Buffer::Reset(_fmatrix matOwner)
{
	if (m_pVIBufferCom != nullptr)
		static_pointer_cast<CVIBuffer_Trail>(m_pVIBufferCom)->Reset(matOwner);
}

void CTrail_Buffer::Reset_Effect(_bool bActivate)
{
	// 툴에서만 사용함.
	// 그룹, 툴, 인게임 상관 없음.

	if (!m_bIsInGroupEffect && !IsState(OBJSTATE::Active))
		TurnOn_State(OBJSTATE::Active);
}

void CTrail_Buffer::Reset_Prototype_Data()
{
	json In_Json;
	string strFilePath = WstrToStr(CUtility_File::Get_FilePath(CPath_Mgr::FILE_TYPE::DATA_FILE, Get_PrototypeTag()));

	if (Compare_Str(strFilePath, "Not_Find"))
		return;

	CJson_Utility::Load_Json(strFilePath.c_str(), In_Json);

	Load_Json(In_Json["GameObject"]);

	m_bLoad = true;

	// 모든 데이터를 다시 받아오고 나서 기존에 남아있는 클론들은 삭제 예정.
}

void CTrail_Buffer::Write_Json(json& Out_Json)
{
	Out_Json["bUpdate"] = m_tTrailBuffer_Desc.bUpdate;

	CJson_Utility::Write_Float3(Out_Json["vPos_0"], m_tTrailBuffer_Desc.vPos_0);
	CJson_Utility::Write_Float3(Out_Json["vPos_1"], m_tTrailBuffer_Desc.vPos_1);
	Out_Json["iMaxCnt"] = m_tTrailBuffer_Desc.iMaxCnt;
	Out_Json["iLerpPointNum"] = m_tTrailBuffer_Desc.iLerpPointNum;

	Out_Json["bMaskInverse"] = m_tTrailBuffer_Desc.bMaskInverse;
	Out_Json["bDiffuseClamp"] = m_tTrailBuffer_Desc.bDiffuseClamp;
	Out_Json["bMaskClamp"] = m_tTrailBuffer_Desc.bMaskClamp;
	Out_Json["bNoiseClamp"] = m_tTrailBuffer_Desc.bNoiseClamp;

	// ----- socket -------
	Out_Json["eUseType"] = (_int)m_tTrailBuffer_Desc.eUseType; // 오너 타입
	Out_Json["bSocket"] = m_tTrailBuffer_Desc.bSocket; // 뼈 사용 유무
	Out_Json["strBoneName"] = WstrToStr(m_tTrailBuffer_Desc.strBoneName); // 뼈 이름

	// ------ Diffuse -------
	Out_Json["bDiffuseUse"] = m_tTrailBuffer_Desc.bDiffuseUse; // 재질을 사용할 것인지.

	Effect_Write_Json(Out_Json, m_tTrailBuffer_Desc);
	//CGameObject::Write_Json(Out_Json);

	_matrix matWorld = m_pTransformCom->Get_WorldFloat4x4();
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][0], matWorld.r[0]);
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][1], matWorld.r[1]);
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][2], matWorld.r[2]);
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][3], matWorld.r[3]);
	// 월드 행렬 저장
}

void CTrail_Buffer::Load_Json(const json& In_Json)
{
	m_tTrailBuffer_Desc.bUpdate = In_Json["bUpdate"];

	CJson_Utility::Load_Float3(In_Json["vPos_0"], m_tTrailBuffer_Desc.vPos_0);
	CJson_Utility::Load_Float3(In_Json["vPos_1"], m_tTrailBuffer_Desc.vPos_1);
	m_tTrailBuffer_Desc.iMaxCnt = In_Json["iMaxCnt"];
	m_tTrailBuffer_Desc.iLerpPointNum = In_Json["iLerpPointNum"];

	m_tTrailBuffer_Desc.bMaskInverse = In_Json["bMaskInverse"];
	m_tTrailBuffer_Desc.bDiffuseClamp = In_Json["bDiffuseClamp"];
	m_tTrailBuffer_Desc.bMaskClamp = In_Json["bMaskClamp"];
	m_tTrailBuffer_Desc.bNoiseClamp = In_Json["bNoiseClamp"];

	// ----- socket -------
	m_tTrailBuffer_Desc.eUseType = (CEffect::USE_TYPE)In_Json["eUseType"]; // 오너 타입
	m_tTrailBuffer_Desc.bSocket = In_Json["bSocket"]; // 뼈 사용 유무
	m_tTrailBuffer_Desc.strBoneName = StrToWstr(In_Json["strBoneName"]); // 뼈 이름

	// ------ Diffuse -------
	if (In_Json.contains("bDiffuseUse"))
	{
		m_tTrailBuffer_Desc.bDiffuseUse = In_Json["bDiffuseUse"]; // 재질을 사용할 것인지.
	}

	Effect_Load_Json(In_Json, &m_tTrailBuffer_Desc);

	_float4 vTemp; // 월드 행렬 로드
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][0], vTemp);
	memcpy(&m_matJsonWorld.m[0], &vTemp, sizeof(_float4));
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][1], vTemp);
	memcpy(&m_matJsonWorld.m[1], &vTemp, sizeof(_float4));
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][2], vTemp);
	memcpy(&m_matJsonWorld.m[2], &vTemp, sizeof(_float4));
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][3], vTemp);
	memcpy(&m_matJsonWorld.m[3], &vTemp, sizeof(_float4));
}

HRESULT CTrail_Buffer::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Shader_Effect_Trail"),
		TEXT("Com_Shader"), & (m_pShaderCom))))
		RETURN_EFAIL;

	/* For.Com_VIBuffer */
	CVIBuffer_Trail::VIBUFFER_TRAIL_DESC		Trail_BufferDesc = {};
	Trail_BufferDesc.vPos_0 = m_tTrailBuffer_Desc.vPos_0;
	Trail_BufferDesc.vPos_1 = m_tTrailBuffer_Desc.vPos_1;
	Trail_BufferDesc.iMaxCnt = m_tTrailBuffer_Desc.iMaxCnt;
	Trail_BufferDesc.iLerpPointNum = m_tTrailBuffer_Desc.iLerpPointNum;

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), & (m_pVIBufferCom), &Trail_BufferDesc)))
		RETURN_EFAIL;

	/* For.Com_Material */
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), &(m_pMaterialCom))))
		RETURN_EFAIL;

	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_DIFFUSE,
		m_tTrailBuffer_Desc.strDiffuse, 1))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_MASK,
		m_tTrailBuffer_Desc.strMask, 1))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_NOISE,
		m_tTrailBuffer_Desc.strNoise, 1))) RETURN_EFAIL;

	return S_OK;
}

HRESULT CTrail_Buffer::Bind_ShaderResources()
{
	// ---- Matrix ------
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;

	// ----- Texture ------
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pShaderCom.get(), "g_DiffuseTexture", MATERIALTYPE::MATERIATYPE_DIFFUSE, 0,true))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pShaderCom.get(), "g_MaskTexture", MATERIALTYPE::MATERIATYPE_MASK, 0, true))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pShaderCom.get(), "g_NoiseTexture", MATERIALTYPE::MATERIATYPE_NOISE, 0, true))) RETURN_EFAIL;
	if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom.get(), "g_DepthTexture")))
		RETURN_EFAIL;

	// -------- UV --------
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_vDiffuseUV", &m_tTrailBuffer_Desc.vDiffuseUV, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_vMaskUV", &m_tTrailBuffer_Desc.vMaskUV, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_vNoiseUV", &m_tTrailBuffer_Desc.vNoiseUV, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_vWeight", &m_tTrailBuffer_Desc.vWeight, sizeof(_float3)))) RETURN_EFAIL;

	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_bDiffuseClamp", &m_tTrailBuffer_Desc.bDiffuseClamp, sizeof(_bool)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_bMaskClamp", &m_tTrailBuffer_Desc.bMaskClamp, sizeof(_bool)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_bNoiseClamp", &m_tTrailBuffer_Desc.bNoiseClamp, sizeof(_bool)))) RETURN_EFAIL;

	//---- Distortion --------
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_vDistortionValue", &m_tTrailBuffer_Desc.vDistortionValue, sizeof(_float3)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_fDistortionScale", &m_tTrailBuffer_Desc.fDistortionScale, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_fDistortionBias", &m_tTrailBuffer_Desc.fDistortionBias, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_fDistortionWeight", &m_tTrailBuffer_Desc.fNoiseWeight, sizeof(_float)))) RETURN_EFAIL;

	// Etc
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_fAlpha", &m_tTrailBuffer_Desc.fDiscard_Alpha, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_vColor", &m_tTrailBuffer_Desc.vSolid_Color,sizeof(_float4)))) RETURN_EFAIL; 
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_fTimeDelta", &m_fTimeDelta, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_bMaskInverse", &m_tTrailBuffer_Desc.bMaskInverse, sizeof(_bool)))) RETURN_EFAIL;
	if (FAILED(m_pShaderCom.get()->Bind_RawValue("g_bDiffuseUse", &m_tTrailBuffer_Desc.bDiffuseUse, sizeof(_bool)))) RETURN_EFAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iColorBlendType", &m_tTrailBuffer_Desc.eColorBlendType, sizeof(_int)))) RETURN_EFAIL;
	// 컬러 타입

	return S_OK;
}

void CTrail_Buffer::Judge_Dead(EFFECT_DESC EffectDesc)
{
	if (m_pOwner.lock() == nullptr || m_pOwner.lock()->IsState(OBJSTATE::WillRemoved))
	{
		Set_Dead();

		// 이곳은 그룹과 단일 상관 없음
		// 무조건 오너 위주로 돌아가기 때문.
		// 툴에서도 마찬가지.
	}
}

void CTrail_Buffer::Update_WorldMatrix()
{
	if (m_pOwner.lock() != nullptr)
	{
		_matrix matOwnerWorld;

		if (m_pGameInstance->Get_CreateLevelIndex() == LEVEL_TOOL)
		{
			if (m_tTrailBuffer_Desc.pBoneGroup)
				m_iBoneIndex = m_tTrailBuffer_Desc.pBoneGroup->Find_BoneData(m_tTrailBuffer_Desc.strBoneName)->iID;
		} // 툴 상에서는 뼈 업데이트 가능

		if (m_tTrailBuffer_Desc.eUseType == USE_TYPE::USE_FOLLOW_NORMAL)
		{// 논파트 오브젝트 오너의 일반적인 월드행렬을 가져와서 계산한다.

			if (m_tTrailBuffer_Desc.bSocket)
			{
				if (m_tTrailBuffer_Desc.pBoneGroup == nullptr)
					return;

				matOwnerWorld = m_pTransformCom->Get_WorldFloat4x4()
					* Get_MatrixNormalize(m_tTrailBuffer_Desc.pBoneGroup->CRef_BoneCombinedTransforms()[m_iBoneIndex])
					* m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix();
			}
			else
			{
				matOwnerWorld = m_pTransformCom->Get_WorldFloat4x4()
					* m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrix();
			}
		}
		else if (m_tTrailBuffer_Desc.eUseType == USE_TYPE::USE_FOLLOW_PARTS)
		{// 파트 오브젝트 오너의 오너 행렬과 계산된 월드행렬을 가져와서 계산한다.

			if (m_tTrailBuffer_Desc.bSocket)
			{
				if (m_tTrailBuffer_Desc.pBoneGroup == nullptr)
					return;

				matOwnerWorld = m_pTransformCom->Get_WorldFloat4x4()
					* Get_MatrixNormalize(m_tTrailBuffer_Desc.pBoneGroup->CRef_BoneCombinedTransforms()[m_iBoneIndex])
					* static_pointer_cast<CPartObject>(m_pOwner.lock())->Get_WorldMatrix();
			}
			else
			{
				matOwnerWorld = m_pTransformCom->Get_WorldFloat4x4()
					* static_pointer_cast<CPartObject>(m_pOwner.lock())->Get_WorldMatrix();
			}
		}
		else if (m_tTrailBuffer_Desc.eUseType == USE_TYPE::USE_FOLLOW_EFFECT)
		{// 이펙트 오너는 따로 계산된 행렬을 가져와서 사용한다.

			matOwnerWorld = m_pTransformCom->Get_WorldFloat4x4()
				* static_pointer_cast<CBlendObject>(m_pOwner.lock())->Get_matWorld();
			// 이펙트는 뼈가 없음
		}

		if (m_bIsReset)
		{
			Reset(matOwnerWorld);
			m_bIsReset = false;
		}
		
		Update(matOwnerWorld);

		m_matWorld = m_pTransformCom->Get_WorldFloat4x4();
	}
}

void CTrail_Buffer::Free()
{
	__super::Free();

}

