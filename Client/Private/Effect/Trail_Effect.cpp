#include "stdafx.h"
#include "Trail_Effect.h"

#include "GameInstance.h"
#include "Effect_Manager.h"

#include "Client_Manager.h"

IMPLEMENT_CLONE(CTrail_Effect, CGameObject)
IMPLEMENT_CREATE_EX1(CTrail_Effect, string, strFilePath)
IMPLEMENT_CREATE(CTrail_Effect)


CTrail_Effect::CTrail_Effect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CEffect(pDevice, pContext)
{

}

CTrail_Effect::CTrail_Effect(const CTrail_Effect& rhs)
	: CEffect(rhs)
	, m_TrailDesc(rhs.m_TrailDesc)
{
}

HRESULT CTrail_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail_Effect::Initialize_Prototype(string strFilePath)
{
	json In_Json;
	CJson_Utility::Load_Json(strFilePath.c_str(), In_Json);

	Load_Json(In_Json["GameObject"]);

	m_bLoad = true;

	return S_OK;
}

HRESULT CTrail_Effect::Initialize(void* pArg)
{
	if (!m_bLoad)
		m_TrailDesc = *((TRAIL_DESC*)pArg);

	if (FAILED(__super::Initialize(pArg)))
		RETURN_EFAIL;

	if (FAILED(Ready_Components()))
		RETURN_EFAIL;

	if (!m_bLoad)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f));
		// 기본적으로 해줘야 축이 원본과 맞게 된다.
	}
	else {
		m_pTransformCom->Set_WorldFloat4x4(m_matJsonWorld);
		// 트랜스폼 행렬을 툴에서 맞춰준 로컬 행렬로 변경
	}
	
	return S_OK;
}

void CTrail_Effect::Begin_Play(_cref_time fTimeDelta)
{
	__super::Begin_Play(fTimeDelta);

	Reset();
}

void CTrail_Effect::Priority_Tick(_cref_time fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
	
	m_fTimeDelta += fTimeDelta;

	if (m_bPlay)
		Play(fTimeDelta);

	UpdateWorldMatrix(m_TrailDesc); // 월드행렬 계산

	if (m_TrailDesc.iShaderPassType == 4)
	{
		_float3 vWorldPos = _float3(m_matWorld.m[3][0], m_matWorld.m[3][1], m_matWorld.m[3][2]);
		GET_SINGLE(CClient_Manager)->TurnOn_RadialBlur(vWorldPos, 0.5f);
		// 쉐이더 패스 4번이면 레디어블러에 정보를 계속 넘겨줌. -> 위치등등
	}
}

void CTrail_Effect::Tick(_cref_time fTimeDelta)
{
	__super::Tick(fTimeDelta);	
}

void CTrail_Effect::Late_Tick(_cref_time fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);

	Judge_Dead(m_TrailDesc); // 죽음 판단
}
	
void CTrail_Effect::Before_Render(_cref_time fTimeDelta)
{
	__super::Before_Render(fTimeDelta);

	if (m_bParticleDead)
		return;

	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND_EFFECT, shared_from_this())))
		return;
}

HRESULT CTrail_Effect::Ready_Components()
{
	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_Material"),
		TEXT("Com_Material"), &(m_pMaterialCom))))
		RETURN_EFAIL;

	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_DIFFUSE,
		m_TrailDesc.strDiffuse, 1))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_MASK,
		m_TrailDesc.strMask, 1))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_NOISE,
		m_TrailDesc.strNoise, 1))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Ready_CustomSingleMaterial(MATERIALTYPE::MATERIATYPE_DISSOLVE,
		m_TrailDesc.strDissolve, 1))) RETURN_EFAIL;

	if (FAILED(__super::Add_Component(m_pGameInstance->Get_CreateLevelIndex(), TEXT("Prototype_Component_CommonModel"),
		TEXT("Com_Model"), &(m_pModelCom))))
		RETURN_EFAIL;

	m_pModelCom->Link_Model(CCommonModelComp::TYPE::TYPE_NONANIM, m_TrailDesc.strModelTag);
	if (FAILED(m_pModelCom->Link_Shader(TEXT("Shader_Effect_Mesh"))))
		RETURN_EFAIL;

	return S_OK;
}

HRESULT CTrail_Effect::Render()
{
	if (FAILED(Bind_ShaderResources()))
		RETURN_EFAIL;

	auto Meshes = m_pModelCom->Get_ActiveMeshes();
	if (Meshes.empty())
		RETURN_EFAIL;
	for (auto& iter : Meshes)
	{
		m_pModelCom->Begin_Shader(m_TrailDesc.iShaderPassType);
		m_pModelCom->BindAndRender_Mesh(iter);
	}

	return S_OK;
}

void CTrail_Effect::Play(_cref_time fTimeDelta)
{
	_vector vAddPosition;

	if (!m_bDissovleStart)
	{
		//TIme
		m_fLifeTimeAcc += fTimeDelta;

		//UV
		m_fUVSpeeds += fTimeDelta * m_TrailDesc.fUVForce;
		m_fTimeDelta += m_fUVSpeeds * fTimeDelta;

		//Move
		XMStoreFloat3(&m_pMoveSpeeds, XMLoadFloat3(&m_pMoveSpeeds) + XMLoadFloat3(&m_TrailDesc.vMove_Force) * fTimeDelta);
		m_pMoveSpeeds = Clamp(m_TrailDesc.vMove_MinSpeed, m_TrailDesc.vMove_MaxSpeed, m_pMoveSpeeds);
		XMStoreFloat3(&m_vAccPosition, XMLoadFloat3(&m_vAccPosition) + XMLoadFloat3(&m_pMoveSpeeds) * fTimeDelta);
		vAddPosition = XMLoadFloat3(&m_pMoveSpeeds) * fTimeDelta;

		//Scale
		XMStoreFloat3(&m_pScaleSpeeds, XMLoadFloat3(&m_pScaleSpeeds) + XMLoadFloat3(&m_TrailDesc.vScale_Force) * fTimeDelta);
		XMStoreFloat3(&m_pScales, XMLoadFloat3(&m_pScales) + XMLoadFloat3(&m_pScaleSpeeds) * fTimeDelta);
		m_pScales = Clamp(m_TrailDesc.vScale_Min, m_TrailDesc.vScale_Max, m_pScales);

		//Rotation
		XMStoreFloat3(&m_pRotationSpeeds, XMLoadFloat3(&m_pRotationSpeeds) + XMLoadFloat3(&m_TrailDesc.vRotation_Force) * fTimeDelta);
		XMStoreFloat3(&m_pRotations, XMLoadFloat3(&m_pRotations) + XMLoadFloat3(&m_pRotationSpeeds) * fTimeDelta);
		m_pRotations = Clamp(m_TrailDesc.vRotation_Min, m_TrailDesc.vRotation_Max, m_pRotations);

		//Color
		XMStoreFloat4(&m_pColorSpeeds, XMLoadFloat4(&m_pColorSpeeds) + XMLoadFloat4(&m_TrailDesc.vColor_Force) * fTimeDelta);
		XMStoreFloat4(&m_pColors, XMLoadFloat4(&m_pColors) + XMLoadFloat4(&m_pColorSpeeds) * fTimeDelta);
		m_pColors = Clamp(_float4(0.f, 0.f, 0.f, 0.f), m_TrailDesc.vColor_Max, m_pColors);


		m_pTransformCom->Set_Scaling(m_pScales.x, m_pScales.y, m_pScales.z);
		m_pTransformCom->Add_Position(vAddPosition);

		if (m_TrailDesc.bRotation)
		{
			if (!m_bAxisFinishRot)
			{
				
			}

			m_pTransformCom->Rotation(XMConvertToRadians(m_pRotations.x), XMConvertToRadians(m_pRotations.y),
				XMConvertToRadians(m_pRotations.z));

			AxisRot(fTimeDelta);
		}	
	}
	else {

		m_fDissolveAmout += fTimeDelta * m_TrailDesc.fDissolveAmountSpeed;
		m_fDissolveGradiationDistance += fTimeDelta * m_TrailDesc.fDissolveGradiationDistanceSpeed;
		// 디졸브 조절
	}

}

void CTrail_Effect::Reset()
{
	m_pScales = m_TrailDesc.vScale;
	m_pRotations = m_TrailDesc.vRotation;
	m_pColors = m_TrailDesc.vColor;
	m_pMoveSpeeds = m_TrailDesc.vMove_Speed;
	m_pScaleSpeeds = m_TrailDesc.vScale_Speed;
	m_pRotationSpeeds = m_TrailDesc.vRotation_Speed;
	m_pColorSpeeds = m_TrailDesc.vColor_Speed;
	m_fUVSpeeds = m_TrailDesc.fUVSpeed;
	m_fTimeDelta = 0.f;
	m_fDissolveAmout = 0.f;
	m_fDissolveGradiationDistance = 0.f;
	m_bDissovleStart = false;
	m_fLifeTimeAcc = 0.f;

	m_fAxisAngleAcc = 0.f;
	m_fAxisAngleSpeedAcc = m_TrailDesc.fAxiAngleSpeed;
	m_bAxisFinishRot = false;
}

void CTrail_Effect::Reset_Effect(_bool bActivate)
{
	if (bActivate && !m_bIsInGroupEffect && m_pGameInstance->Get_CreateLevelIndex() == LEVEL_TOOL)
		TurnOn_State(OBJSTATE::Active); // 툴에서 비활성화 시킨 이펙트를 다시 활성화 시킴.
	// 그룹은 알아서 리셋 시킴.

	Reset();
	m_bParticleDead = false; // 죽음 처리x
}

void CTrail_Effect::Reset_Prototype_Data()
{
	json In_Json;
	string strFilePath =WstrToStr(CUtility_File::Get_FilePath(CPath_Mgr::FILE_TYPE::DATA_FILE, Get_PrototypeTag()));

	if (Compare_Str(strFilePath, "Not_Find"))
		return;

	CJson_Utility::Load_Json(strFilePath.c_str(), In_Json);

	Load_Json(In_Json["GameObject"]);

	m_bLoad = true;

	// 모든 데이터를 다시 받아오고 나서 기존에 남아있는 클론들은 삭제 예정.
}

void CTrail_Effect::OnCollision_Enter(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CTrail_Effect::OnCollision_Stay(CCollider* pThisCol, CCollider* pOtherCol)
{
}

void CTrail_Effect::OnCollision_Exit(CCollider* pThisCol, CCollider* pOtherCol)
{
	
}

HRESULT CTrail_Effect::Bind_ShaderResources()
{
	/* For.Matrix*/
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_WorldMatrix", &m_matWorld)))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_VIEW))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::TS_PROJ))))
		RETURN_EFAIL;

	/* For.TextureUV*/
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vDiffuseUV", &m_TrailDesc.vDiffuseUV, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vMaskUV", &m_TrailDesc.vMaskUV, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vNoiseUV", &m_TrailDesc.vNoiseUV, sizeof(_float2)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_bMoveUV_Y", &m_TrailDesc.bMoveUV_Y, sizeof(m_TrailDesc.bMoveUV_Y))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_fUVStart", &m_TrailDesc.fUVStart, sizeof(m_TrailDesc.fUVStart))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vWeight", &m_TrailDesc.vWeight, sizeof(_float3)))) RETURN_EFAIL;

	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_bDiffuseClamp", &m_TrailDesc.bDiffuseClamp, sizeof(_bool)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_bMaskClamp", &m_TrailDesc.bMaskClamp, sizeof(_bool)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_bNoiseClamp", &m_TrailDesc.bNoiseClamp, sizeof(_bool)))) RETURN_EFAIL;

	/* For.Texture*/
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pModelCom->ShaderComp().get(), "g_DiffuseTexture", MATERIALTYPE::MATERIATYPE_DIFFUSE, 0,true))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pModelCom->ShaderComp().get(), "g_MaskTexture", MATERIALTYPE::MATERIATYPE_MASK, 0, true))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pModelCom->ShaderComp().get(), "g_NoiseTexture", MATERIALTYPE::MATERIATYPE_NOISE, 0, true))) RETURN_EFAIL;
	if (FAILED(m_pMaterialCom->Bind_SRVToShader(m_pModelCom->ShaderComp().get(), "g_DissolveTexture", MATERIALTYPE::MATERIATYPE_DISSOLVE, 0, true))) RETURN_EFAIL;

	// For. Dissolve
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_fDissolveAmount", &m_fDissolveAmout, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_fDissolveGradiationDistance", &m_fDissolveGradiationDistance, sizeof(_float)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vDissolveGradiationStartColor", &m_TrailDesc.vDissolveGradiationStartColor, sizeof(_float3)))) RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vDissolveGradiationEndColor", &m_TrailDesc.vDissolveGradiationEndColor, sizeof(_float3)))) RETURN_EFAIL;

	/* ETC */
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_fTimeDelta", &m_fTimeDelta, sizeof(m_fTimeDelta))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_vColor", &m_pColors, sizeof(m_pColors))))
		RETURN_EFAIL;
	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_bMaskInverse", &m_TrailDesc.bMaskInverse, sizeof(m_TrailDesc.bMaskInverse))))
		RETURN_EFAIL;

	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_fAlpha", &m_TrailDesc.fDiscard_Alpha, sizeof(m_TrailDesc.fDiscard_Alpha))))
		RETURN_EFAIL;

	if (FAILED(m_pModelCom->ShaderComp()->Bind_RawValue("g_iColorBlendType", &m_TrailDesc.eColorBlendType, sizeof(_int)))) RETURN_EFAIL;
	// 컬러 타입

	return S_OK;
}

void CTrail_Effect::Judge_Dead(EFFECT_DESC EffectDesc)
{
	_bool bCheck = false;

	switch (EffectDesc.eDeadType)
	{
	case DEADTYPE::TIME:
		if (m_fLifeTimeAcc >= m_TrailDesc.fLifeTime)
		{
			if (m_TrailDesc.iShaderPassType == 3) // dissolve
			{
				if (!m_bDissovleStart)
				{
					m_bDissovleStart = true;
					DissolveParticle();	
				}

				if ((m_fDissolveAmout >= 1.f) && (m_fDissolveGradiationDistance >= 1.f))
					bCheck = true;

			}
			else
				bCheck = true;
		}
		break;
	case DEADTYPE::SCALE:
		if (!InRange(m_pScales.x, m_TrailDesc.vScale_Min.x, m_TrailDesc.vScale_Max.x, "()") ||
			!InRange(m_pScales.y, m_TrailDesc.vScale_Min.y, m_TrailDesc.vScale_Max.y, "()") ||
			!InRange(m_pScales.z, m_TrailDesc.vScale_Min.z, m_TrailDesc.vScale_Max.z, "()"))
		{
			if ( m_TrailDesc.iShaderPassType == 3) // dissolve
			{
				if (!m_bDissovleStart)
				{
					m_bDissovleStart = true;
					DissolveParticle();
				}

				if ((m_fDissolveAmout >= 1.f) && (m_fDissolveGradiationDistance >= 1.f))
					bCheck = true;

			}else
				bCheck = true;
		}
		break;
	case DEADTYPE::COLOR:
		if (m_pColors.w <= 0.f)
		{
			if (m_TrailDesc.iShaderPassType == 3) // dissolve
			{
				if (!m_bDissovleStart)
				{
					m_bDissovleStart = true;
					DissolveParticle();
				}

				if ((m_fDissolveAmout >= 1.f) && (m_fDissolveGradiationDistance >= 1.f))
					bCheck = true;
			}
			else
				bCheck = true;
		}
		break;
	case DEADTYPE::OWNER:
		if ( (m_pOwner.lock() == nullptr) || m_pOwner.lock()->IsState(OBJSTATE::WillRemoved))
		{ // 툴 레벨에서는 계속 진행
			if ((m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL))
			{
				if (m_TrailDesc.iShaderPassType == 3) // dissolve
				{
					if (!m_bDissovleStart)
					{
						m_bDissovleStart = true;
						DissolveParticle();
					}

					if ((m_fDissolveAmout >= 1.f) && (m_fDissolveGradiationDistance >= 1.f))
						bCheck = true;
				}
				else
					bCheck = true;
			}
		}
		break;
	}

	if (bCheck && !m_bParticleDead)
	{
		if (m_bLoad && m_pGameInstance->Get_CreateLevelIndex() != LEVEL_TOOL)
		{// 게임에서 사용

			m_bParticleDead = true;

			if (m_bIsInGroupEffect)
				TurnOff_State(OBJSTATE::Active); // 그룹 이펙트
			else
				Set_Dead(); // 단일 이펙트 -> 오브젝트 풀로 리턴
		}
		else
		{// 툴에서 사용
			m_bParticleDead = true;
			TurnOff_State(OBJSTATE::Active); // 비활성화
		}
			
	}

	if ((m_TrailDesc.iShaderPassType == 4) && m_bParticleDead)
	{
		GET_SINGLE(CClient_Manager)->TurnOff_RadialBlur();
	}
}

void CTrail_Effect::DissolveParticle()
{
	if (!m_TrailDesc.bDissolveParticle)
		return;
	
	GET_SINGLE(CEffect_Manager)->Create_EffectMesh_VTX_Particle(m_TrailDesc.strDissolveParticleTag,
		shared_from_this(), m_TrailDesc.eDissolveParticleStartType);
}

void CTrail_Effect::AxisRot(_cref_time fTimeDelta)
{// 임의의 축을 이용해서 회전
	if (!m_TrailDesc.bCurAxisUse)
		return;

	_vector	vAxis = XMVectorSetW(m_TrailDesc.vAxisDir, 0.f);

	if (XMVector3Equal(vAxis, XMVectorZero()))
		return;

	_vector vRot = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_pRotations.x), XMConvertToRadians(m_pRotations.y),
		XMConvertToRadians(m_pRotations.z));
	_matrix matRot = XMMatrixRotationQuaternion(vRot);

	vAxis = XMVector3TransformNormal(vAxis, matRot);

	//_bool	bIsRot = true;

	m_fAxisAngleSpeedAcc += fTimeDelta * m_TrailDesc.fAxiAngleForce;
	m_fAxisAngleAcc += m_fAxisAngleSpeedAcc * fTimeDelta;
	m_fAxisAngleAcc = Clamp<_float>(m_TrailDesc.vAxisAngleMinMax.x, m_TrailDesc.vAxisAngleMinMax.y, m_fAxisAngleAcc);

	if ((m_fAxisAngleAcc == m_TrailDesc.vAxisAngleMinMax.x) || (m_fAxisAngleAcc == m_TrailDesc.vAxisAngleMinMax.y))
		m_bAxisFinishRot = true;

	if (!m_bAxisFinishRot)
		m_pTransformCom->Rotation(vAxis, XMConvertToRadians(m_fAxisAngleAcc));	
}

void CTrail_Effect::Write_Json(json& Out_Json)
{
	Out_Json["strModelTag"] = WstrToStr(m_TrailDesc.strModelTag);

	CJson_Utility::Write_Float3(Out_Json["vInitialPosition"], m_TrailDesc.vInitialPosition);
	CJson_Utility::Write_Float3(Out_Json["vAdjustPosition"], m_TrailDesc.vAdjustPosition);
	CJson_Utility::Write_Float3(Out_Json["vAdjustRotation"], m_TrailDesc.vAdjustRotation);

	CJson_Utility::Write_Float3(Out_Json["vMove_Speed"], m_TrailDesc.vMove_Speed);
	CJson_Utility::Write_Float3(Out_Json["vScale"], m_TrailDesc.vScale);
	CJson_Utility::Write_Float3(Out_Json["vRotation"], m_TrailDesc.vRotation);
	CJson_Utility::Write_Float4(Out_Json["vColor"], m_TrailDesc.vColor);

	CJson_Utility::Write_Float3(Out_Json["vMove_Force"], m_TrailDesc.vMove_Force);
	CJson_Utility::Write_Float3(Out_Json["vMove_MinSpeed"], m_TrailDesc.vMove_MinSpeed);
	CJson_Utility::Write_Float3(Out_Json["vMove_MaxSpeed"], m_TrailDesc.vMove_MaxSpeed);

	CJson_Utility::Write_Float3(Out_Json["vScale_Speed"], m_TrailDesc.vScale_Speed);
	CJson_Utility::Write_Float3(Out_Json["vScale_Force"], m_TrailDesc.vScale_Force);
	CJson_Utility::Write_Float3(Out_Json["vScale_Min"], m_TrailDesc.vScale_Min);
	CJson_Utility::Write_Float3(Out_Json["vScale_Max"], m_TrailDesc.vScale_Max);

	CJson_Utility::Write_Float3(Out_Json["vRotation_Speed"], m_TrailDesc.vRotation_Speed);
	CJson_Utility::Write_Float3(Out_Json["vRotation_Force"], m_TrailDesc.vRotation_Force);
	CJson_Utility::Write_Float3(Out_Json["vRotation_Min"], m_TrailDesc.vRotation_Min);
	CJson_Utility::Write_Float3(Out_Json["vRotation_Max"], m_TrailDesc.vRotation_Max);

	CJson_Utility::Write_Float4(Out_Json["vColor_Speed"], m_TrailDesc.vColor_Speed);
	CJson_Utility::Write_Float4(Out_Json["vColor_Force"], m_TrailDesc.vColor_Force);
	CJson_Utility::Write_Float4(Out_Json["vColor_Max"], m_TrailDesc.vColor_Max);

	Out_Json.emplace("bFollowOwner", m_TrailDesc.bFollowOwner);
	Out_Json.emplace("bClamp", m_TrailDesc.bClamp);
	Out_Json.emplace("bMaskInverse", m_TrailDesc.bMaskInverse);
	Out_Json.emplace("fUVSpeed", m_TrailDesc.fUVSpeed);
	Out_Json.emplace("fUVForce", m_TrailDesc.fUVForce);
	Out_Json.emplace("fUVStart", m_TrailDesc.fUVStart);
	Out_Json.emplace("bMoveUV_Y", m_TrailDesc.bMoveUV_Y);
	Out_Json.emplace("bPlayAnimation", m_TrailDesc.bPlayAnimation);

	CJson_Utility::Write_Float3(Out_Json["vWeight"], m_TrailDesc.vWeight);

	// ------ Dissolve -------
	Out_Json["strDissolve"] = WstrToStr(m_TrailDesc.strDissolve);

	Out_Json.emplace("fDissolveAmountSpeed", m_TrailDesc.fDissolveAmountSpeed);
	Out_Json.emplace("fDissolveGradiationDistanceSpeed", m_TrailDesc.fDissolveGradiationDistanceSpeed);

	CJson_Utility::Write_Float3(Out_Json["vDissolveGradiationStartColor"], m_TrailDesc.vDissolveGradiationStartColor);
	CJson_Utility::Write_Float3(Out_Json["vDissolveGradiationEndColor"], m_TrailDesc.vDissolveGradiationEndColor);

	// ------ TIme --------
	Out_Json.emplace("fLifeTime", m_TrailDesc.fLifeTime);

	// ------ DissolveParticle -------
	Out_Json.emplace("bDissolveParticle", m_TrailDesc.bDissolveParticle);
	Out_Json["strDissolveParticleTag"] = WstrToStr(m_TrailDesc.strDissolveParticleTag);
	Out_Json["eDissolveParticleStartType"] = (_int)m_TrailDesc.eDissolveParticleStartType;

	// ----- Clam ------
	Out_Json.emplace("bDiffuseClamp", m_TrailDesc.bDiffuseClamp);
	Out_Json.emplace("bMaskClamp", m_TrailDesc.bMaskClamp);
	Out_Json.emplace("bNoiseClamp", m_TrailDesc.bNoiseClamp);
	
	// ------ Rotation --------
	Out_Json.emplace("bRotation", m_TrailDesc.bRotation);
	Out_Json.emplace("bCurAxisUse", m_TrailDesc.bCurAxisUse);
	CJson_Utility::Write_Float3(Out_Json["vAxisDir"], m_TrailDesc.vAxisDir);
	Out_Json.emplace("fAxiAngleSpeed", m_TrailDesc.fAxiAngleSpeed);
	Out_Json.emplace("fAxiAngleForce", m_TrailDesc.fAxiAngleForce);
	CJson_Utility::Write_Float2(Out_Json["vAxisAngleMinMax"], m_TrailDesc.vAxisAngleMinMax);

	Effect_Write_Json(Out_Json, m_TrailDesc);

	_matrix matWorld = m_pTransformCom->Get_WorldFloat4x4();
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][0], matWorld.r[0]);
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][1], matWorld.r[1]);
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][2], matWorld.r[2]);
	CJson_Utility::Write_Float4(Out_Json["WorldMatrix"][3], matWorld.r[3]);
	// 월드 행렬 저장
}

void CTrail_Effect::Load_Json(const json& In_Json)
{
	m_TrailDesc.strModelTag = StrToWstr(In_Json["strModelTag"]);
	
	CJson_Utility::Load_Float3(In_Json["vInitialPosition"], m_TrailDesc.vInitialPosition);
	CJson_Utility::Load_Float3(In_Json["vAdjustPosition"], m_TrailDesc.vAdjustPosition);
	CJson_Utility::Load_Float3(In_Json["vAdjustRotation"], m_TrailDesc.vAdjustRotation);

	CJson_Utility::Load_Float3(In_Json["vMove_Speed"], m_TrailDesc.vMove_Speed);
	CJson_Utility::Load_Float3(In_Json["vScale"], m_TrailDesc.vScale);
	CJson_Utility::Load_Float3(In_Json["vRotation"], m_TrailDesc.vRotation);
	CJson_Utility::Load_Float4(In_Json["vColor"], m_TrailDesc.vColor);


	CJson_Utility::Load_Float3(In_Json["vMove_Force"], m_TrailDesc.vMove_Force);
	CJson_Utility::Load_Float3(In_Json["vMove_MinSpeed"], m_TrailDesc.vMove_MinSpeed);
	CJson_Utility::Load_Float3(In_Json["vMove_MaxSpeed"], m_TrailDesc.vMove_MaxSpeed);

	CJson_Utility::Load_Float3(In_Json["vScale_Speed"], m_TrailDesc.vScale_Speed);
	CJson_Utility::Load_Float3(In_Json["vScale_Force"], m_TrailDesc.vScale_Force);
	CJson_Utility::Load_Float3(In_Json["vScale_Min"], m_TrailDesc.vScale_Min);
	CJson_Utility::Load_Float3(In_Json["vScale_Max"], m_TrailDesc.vScale_Max);

	CJson_Utility::Load_Float3(In_Json["vRotation_Speed"], m_TrailDesc.vRotation_Speed);
	CJson_Utility::Load_Float3(In_Json["vRotation_Force"], m_TrailDesc.vRotation_Force);
	CJson_Utility::Load_Float3(In_Json["vRotation_Min"], m_TrailDesc.vRotation_Min);
	CJson_Utility::Load_Float3(In_Json["vRotation_Max"], m_TrailDesc.vRotation_Max);

	CJson_Utility::Load_Float4(In_Json["vColor_Speed"], m_TrailDesc.vColor_Speed);
	CJson_Utility::Load_Float4(In_Json["vColor_Force"], m_TrailDesc.vColor_Force);
	CJson_Utility::Load_Float4(In_Json["vColor_Max"], m_TrailDesc.vColor_Max);

	m_TrailDesc.bFollowOwner = In_Json["bFollowOwner"];
	m_TrailDesc.bClamp = In_Json["bClamp"];
	m_TrailDesc.bMaskInverse = In_Json["bMaskInverse"];
	m_TrailDesc.fUVSpeed = In_Json["fUVSpeed"];
	m_TrailDesc.fUVForce = In_Json["fUVForce"];
	m_TrailDesc.fUVStart = In_Json["fUVStart"];
	m_TrailDesc.bMoveUV_Y = In_Json["bMoveUV_Y"];
	m_TrailDesc.bPlayAnimation = In_Json["bPlayAnimation"];

	CJson_Utility::Load_Float3(In_Json["vWeight"], m_TrailDesc.vWeight);

	// ------ Dissolve -------
	m_TrailDesc.strDissolve = StrToWstr(In_Json["strDissolve"]);

	m_TrailDesc.fDissolveAmountSpeed = In_Json["fDissolveAmountSpeed"];
	m_TrailDesc.fDissolveGradiationDistanceSpeed = In_Json["fDissolveGradiationDistanceSpeed"];

	CJson_Utility::Load_Float3(In_Json["vDissolveGradiationStartColor"], m_TrailDesc.vDissolveGradiationStartColor);
	CJson_Utility::Load_Float3(In_Json["vDissolveGradiationEndColor"], m_TrailDesc.vDissolveGradiationEndColor);

	// ------ TIme --------
	m_TrailDesc.fLifeTime = In_Json["fLifeTime"];

	// ------ DissolveParticle -------
	m_TrailDesc.bDissolveParticle = In_Json["bDissolveParticle"];
	m_TrailDesc.strDissolveParticleTag = StrToWstr(In_Json["strDissolveParticleTag"]);
	m_TrailDesc.eDissolveParticleStartType = (CVIBuffer_Instancing::MESH_START_TYPE)In_Json["eDissolveParticleStartType"];

	// ----- Clamp ------
	if (In_Json.contains("bDiffuseClamp"))
	{
		m_TrailDesc.bDiffuseClamp = In_Json["bDiffuseClamp"];
	}
	
	if (In_Json.contains("bMaskClamp"))
	{
		m_TrailDesc.bMaskClamp = In_Json["bMaskClamp"];
	}

	if (In_Json.contains("bNoiseClamp"))
	{
		m_TrailDesc.bNoiseClamp = In_Json["bNoiseClamp"];
	}

	// ------ Rotation --------
	if (In_Json.contains("bRotation"))
		m_TrailDesc.bRotation = In_Json["bRotation"];

	if (In_Json.contains("bCurAxisUse"))
		m_TrailDesc.bCurAxisUse = In_Json["bCurAxisUse"];

	if (In_Json.contains("vAxisDir"))
		CJson_Utility::Load_Float3(In_Json["vAxisDir"], m_TrailDesc.vAxisDir);

	if (In_Json.contains("fAxiAngleSpeed"))
		m_TrailDesc.fAxiAngleSpeed = In_Json["fAxiAngleSpeed"];

	if (In_Json.contains("fAxiAngleForce"))
		m_TrailDesc.fAxiAngleForce = In_Json["fAxiAngleForce"];

	if (In_Json.contains("vAxisAngleMinMax"))
		CJson_Utility::Load_Float2(In_Json["vAxisAngleMinMax"], m_TrailDesc.vAxisAngleMinMax);

	Effect_Load_Json(In_Json, &m_TrailDesc);

	if (m_TrailDesc.iShaderPassType == 1)
	{
		m_TrailDesc.bDiffuseClamp = true;
	}

	if (m_TrailDesc.bClamp)
	{

		m_TrailDesc.bMaskClamp = true;
		m_TrailDesc.bNoiseClamp = true;
	}

	_float4 vTemp;
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][0], vTemp);
	memcpy(&m_matJsonWorld.m[0], &vTemp, sizeof(_float4));
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][1], vTemp);
	memcpy(&m_matJsonWorld.m[1], &vTemp, sizeof(_float4));
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][2], vTemp);
	memcpy(&m_matJsonWorld.m[2], &vTemp, sizeof(_float4));
	CJson_Utility::Load_Float4(In_Json["WorldMatrix"][3], vTemp);
	vTemp = _float4(0.f, 0.f, 0.f, 1.f);
	memcpy(&m_matJsonWorld.m[3], &vTemp, sizeof(_float4));
	// 월드 행렬 저장	
}

void CTrail_Effect::Free()
{
	__super::Free();

}
