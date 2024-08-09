#include "..\Public\VIBuffer_Instancing.h"
#include "GameInstance.h"
#include "../Json/Json_Utility.h"
#include "Easing.h"

#include "BlendObject.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_InstancingDesc(rhs.m_InstancingDesc)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
{
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void* pArg)
{
	m_InstancingDesc = *(INSTANCING_DESC*)pArg;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	VTXINSTANCE* pVertices = new VTXINSTANCE[m_iNumInstance];

	m_pMoveSpeeds = new _float3[m_iNumInstance];
	m_pScaleSpeeds = new _float3[m_iNumInstance];
	m_pRotationSpeeds = new _float3[m_iNumInstance];
	m_pColorSpeeds = new _float4[m_iNumInstance];
	m_pScales = new _float3[m_iNumInstance];
	m_pRotations = new _float3[m_iNumInstance];
	m_pColors = new _float4[m_iNumInstance];
	m_pLifeTimes = new _float[m_iNumInstance];

	// --- Sprite ---------

	m_pSprtieUV = new _float2[m_iNumInstance];
	m_pSpriteFrame = new _float[m_iNumInstance];
	m_pSpriteSpeed = new _float[m_iNumInstance];
	m_pSpriteFinished = new _bool[m_iNumInstance];

	/* For. RandomDirection*/
	m_pDirTurnTimeAcc = new _float[m_iNumInstance];
	m_pDirTurnTime = new _float[m_iNumInstance];

	// ------ MeshParticle -----------
	m_pMeshCurIndex = new _uint[m_iNumInstance];

	// ------ Death Condition --------
	m_pLifeTimeAcc = new _float[m_iNumInstance];

	/* For. Direction*/
	m_vRangeCenter = (m_InstancingDesc.vStartRange[0] + m_InstancingDesc.vStartRange[1]) / 2.f;

	if (m_InstancingDesc.bMeshVTXParticle)
	{
		/* For. MeshPosIndex*/
		m_pGameInstance->RandomInt_Vector(m_vecMeshPosIndex, m_iNumInstance,0, m_InstancingDesc.vecMeshVTX.size()-1);
	}

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		m_pMoveSpeeds[i] = m_pGameInstance->RandomFloat3(m_InstancingDesc.vMove_Speed[0],m_InstancingDesc.vMove_Speed[1]);
		m_pRotations[i] = m_pGameInstance->RandomFloat3(m_InstancingDesc.vRotation[0], m_InstancingDesc.vRotation[1]);
		m_pColors[i] = m_pGameInstance->RandomFloat4(m_InstancingDesc.vColor[0], m_InstancingDesc.vColor[1]);
		m_pLifeTimes[i] = m_pGameInstance->Random(m_InstancingDesc.vLifeTime.x, m_InstancingDesc.vLifeTime.y);

		// -- Sprite ----
		m_pSpriteSpeed[i] = m_pGameInstance->Random(m_InstancingDesc.vSprite_Speed.x, m_InstancingDesc.vSprite_Speed.y);
		m_pSpriteFinished[i] = false;
		m_pSpriteFrame[i] = 0.f;

		// --- MaskArray -----
		pVertices[i].iMaskIndex = m_pGameInstance->RandomInt(0, m_InstancingDesc.iMaskArrayMaxIndex <= 0 ? 0 : (int)m_InstancingDesc.iMaskArrayMaxIndex-1);
		// 마스크 인덱스

		// ------ Death Condition --------
		m_pLifeTimeAcc[i] = 0.f;

		/* For. RandomDirection*/
		m_pDirTurnTime[i] = m_pGameInstance->Random(m_InstancingDesc.vDirTurnTime.x, m_InstancingDesc.vDirTurnTime.y);

		if (m_InstancingDesc.bCube)
		{
			m_pScales[i].x = m_pScales[i].y = m_pScales[i].z = m_pGameInstance->Random(m_InstancingDesc.vScale[0].x, m_InstancingDesc.vScale[1].x);
		}
		else
			m_pScales[i] = m_pGameInstance->RandomFloat3(m_InstancingDesc.vScale[0], m_InstancingDesc.vScale[1]);

		m_pScaleSpeeds[i] = m_InstancingDesc.vScale_Speed;
		m_pRotationSpeeds[i] = m_InstancingDesc.vRotation_Speed;
		m_pColorSpeeds[i] = m_InstancingDesc.vColor_Speed;

		pVertices[i].vRight = _float4(m_pScales[i].x, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, m_pScales[i].y, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, m_pScales[i].z, 0.f);
		pVertices[i].vColor = m_pColors[i];

		
		_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_pRotations[i].x, m_pRotations[i].y, m_pRotations[i].z);
		_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);
		XMStoreFloat4(&pVertices[i].vRight, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vRight), RotationMatrix));
		XMStoreFloat4(&pVertices[i].vUp, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vUp), RotationMatrix));
		XMStoreFloat4(&pVertices[i].vLook, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vLook), RotationMatrix));

		_float3 vStartRange;

		if (!m_InstancingDesc.bMeshVTXParticle)
		{
			vStartRange = m_pGameInstance->RandomFloat3(m_InstancingDesc.vStartRange[0], m_InstancingDesc.vStartRange[1]);
			CutRange(&vStartRange);
		}
		else {

			_float3 vOffset = m_pGameInstance->RandomFloat3(m_InstancingDesc.vMeshPointOffset[0],
				m_InstancingDesc.vMeshPointOffset[1]);

			switch (m_InstancingDesc.eMeshStartType)
			{
			case MESH_START_TYPE::ZERO_POS:
				{
					vStartRange = (m_InstancingDesc.vecMeshVTX[0] * m_InstancingDesc.fMeshSize) + vOffset;
					m_pMeshCurIndex[i] = 0;
				} // 원점에서 시작
				
				break;
			case MESH_START_TYPE::MESH_POS:
				{
					_uint iSize = (_uint)m_InstancingDesc.vecMeshVTX.size();
					_uint iIndex = (_uint)i;
					if (iSize <= iIndex)
					{
						iIndex -= iSize * (_int)(iIndex / iSize);
					}
					m_pMeshCurIndex[i] = iIndex;
					vStartRange = (m_InstancingDesc.vecMeshVTX[iIndex] * m_InstancingDesc.fMeshSize) + vOffset;
				} // 인덱스 순서대로
				break;

			case MESH_START_TYPE::RANDOM_POS:
				{
					_int iSize = (_int)m_InstancingDesc.vecMeshVTX.size();
					vStartRange = (m_InstancingDesc.vecMeshVTX[m_vecMeshPosIndex[i]] * m_InstancingDesc.fMeshSize) + vOffset;
				} // 정점 위치에서 랜덤
				break;
			}
		}

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat3(&m_InstancingDesc.vCenter) + XMLoadFloat3(&vStartRange));
		pVertices[i].vPosition.w = 1.f;
		

		pVertices[i].vTextureUV = _float2(0.f, 0.f);
	}

	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_ModelBuffer(&m_pVBInstance)))
		RETURN_EFAIL;

	Safe_Delete_Array(pVertices);


	return S_OK;
}

HRESULT CVIBuffer_Instancing::Bind_VIBuffers()
{
	if (nullptr == m_pVB ||
		nullptr == m_pIB)
		RETURN_EFAIL;

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint				iStrides[] = {
		m_iStride,
		m_iInstanceStride

	};

	_uint				iOffsets[] = {
		0,
		0,
	};


	/* 어떤 버텍스 버퍼들을 이용할거다. */
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 어떤 인덱스 버퍼를 이용할거다. */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	/* 정점을 어떤식으로 이어서 그릴거다. */
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	return S_OK;
}

void CVIBuffer_Instancing::Update(_cref_time fTimeDelta)
{
	_uint iFinishNum = 0;
	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

	m_fTimeAcc += fTimeDelta;
	_int iSpawnInstance = 0;
	if (m_fTimeAcc <= m_InstancingDesc.fSpawnTime)
	{
		iSpawnInstance = (_int)(m_iNumInstance * (m_fTimeAcc / m_InstancingDesc.fSpawnTime));
	}
	else
		iSpawnInstance = m_iNumInstance;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		if (i > iSpawnInstance)
		{
			pVertices[i].vColor.w = 0.f;
			continue;
		}

		//Dead Particle (Reset Or Discard)
		_uint iNumTemp = iFinishNum;
		if (Judge_Death(pVertices, (_uint)i, &iFinishNum))
			continue;

		//LifeTime
		m_pLifeTimeAcc[i] += fTimeDelta;

		//Scale
		if (!m_InstancingDesc.bEasing[VALUE_TYPE::SCALE])
		{
			XMStoreFloat3(&m_pScaleSpeeds[i], XMLoadFloat3(&m_pScaleSpeeds[i]) + XMLoadFloat3(&m_InstancingDesc.vScale_Force) * fTimeDelta);
			XMStoreFloat3(&m_pScales[i], XMLoadFloat3(&m_pScales[i]) + XMLoadFloat3(&m_pScaleSpeeds[i]) * fTimeDelta);
			m_pScales[i] = Clamp(m_InstancingDesc.vScale_Min, m_InstancingDesc.vScale_Max, m_pScales[i]);
		}
		else {
			m_pScaleSpeeds[i] = CEasing::Ease_Float3((CEasing::EASE)m_InstancingDesc.iEasingType[VALUE_TYPE::SCALE],
				m_fTimeAcc, m_pScaleSpeeds[i], XMLoadFloat3(&m_InstancingDesc.vScale_Force) * fTimeDelta, m_pLifeTimes[i]);
			XMStoreFloat3(&m_pScales[i], XMLoadFloat3(&m_pScales[i]) + XMLoadFloat3(&m_pScaleSpeeds[i]) * fTimeDelta);
			m_pScales[i] = Clamp(m_InstancingDesc.vScale_Min, m_InstancingDesc.vScale_Max, m_pScales[i]);
		}
				
		//Rotation
		if (!m_InstancingDesc.bEasing[VALUE_TYPE::ROTATION])
		{
			XMStoreFloat3(&m_pRotationSpeeds[i], XMLoadFloat3(&m_pRotationSpeeds[i]) + XMLoadFloat3(&m_InstancingDesc.vRotation_Force) * fTimeDelta);
			XMStoreFloat3(&m_pRotations[i], XMLoadFloat3(&m_pRotations[i]) + XMLoadFloat3(&m_pRotationSpeeds[i]) * fTimeDelta);
			m_pRotations[i] = Clamp(m_InstancingDesc.vRotation_Min, m_InstancingDesc.vRotation_Max, m_pRotations[i]);
		}
		else {
			m_pRotationSpeeds[i] = CEasing::Ease_Float3((CEasing::EASE)m_InstancingDesc.iEasingType[VALUE_TYPE::ROTATION],
				m_fTimeAcc, m_pRotationSpeeds[i], XMLoadFloat3(&m_InstancingDesc.vRotation_Force) * fTimeDelta, m_pLifeTimes[i]);
			XMStoreFloat3(&m_pRotations[i], XMLoadFloat3(&m_pRotations[i]) + XMLoadFloat3(&m_pRotationSpeeds[i]) * fTimeDelta);
			m_pRotations[i] = Clamp(m_InstancingDesc.vRotation_Min, m_InstancingDesc.vRotation_Max, m_pRotations[i]);
		}
		
		//Color
		if (!m_InstancingDesc.bEasing[VALUE_TYPE::COLOR])
		{
			XMStoreFloat4(&m_pColorSpeeds[i], XMLoadFloat4(&m_pColorSpeeds[i]) + XMLoadFloat4(&m_InstancingDesc.vColor_Force) * fTimeDelta);
			XMStoreFloat4(&m_pColors[i], XMLoadFloat4(&m_pColors[i]) + XMLoadFloat4(&m_pColorSpeeds[i]) * fTimeDelta);
			m_pColors[i] = Clamp(_float4(0.f, 0.f, 0.f, 0.f), m_InstancingDesc.vColor_Max, m_pColors[i]);
		}
		else {
			m_pColorSpeeds[i] = CEasing::Ease_Float4((CEasing::EASE)m_InstancingDesc.iEasingType[VALUE_TYPE::COLOR], m_fTimeAcc,
				m_pColorSpeeds[i], XMLoadFloat4(&m_InstancingDesc.vColor_Force) * fTimeDelta, m_pLifeTimes[i]);
			XMStoreFloat4(&m_pColors[i], XMLoadFloat4(&m_pColors[i]) + XMLoadFloat4(&m_pColorSpeeds[i]));
			m_pColors[i] = Clamp(_float4(0.f, 0.f, 0.f, 0.f), m_InstancingDesc.vColor_Max, m_pColors[i]);
		}

		/* For. Move*/

		_float3 vDir = {};
		_float vLength = {};

		switch (m_InstancingDesc.eMoveControl)
		{
		case MOVE_CONTROL_TYPE::NORMAL:

			XMStoreFloat3(&m_pMoveSpeeds[i], XMLoadFloat3(&m_pMoveSpeeds[i]) + XMLoadFloat3(&m_InstancingDesc.vMove_Force) * fTimeDelta);
			m_pMoveSpeeds[i] = Clamp(m_InstancingDesc.vMove_MinSpeed, m_InstancingDesc.vMove_MaxSpeed, m_pMoveSpeeds[i]);
			vLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pMoveSpeeds[i])));
			Add_Force(fTimeDelta, &m_pMoveSpeeds[i]);
			Apply_DirTurn(fTimeDelta, &m_pMoveSpeeds[i], (_uint)i);
			vDir = m_pMoveSpeeds[i];
			break;

		case MOVE_CONTROL_TYPE::CENTER:
			XMStoreFloat3(&m_pMoveSpeeds[i], XMLoadFloat3(&m_pMoveSpeeds[i]) + XMLoadFloat3(&m_InstancingDesc.vMove_Force) * fTimeDelta);
			m_pMoveSpeeds[i] = Clamp(m_InstancingDesc.vMove_MinSpeed, m_InstancingDesc.vMove_MaxSpeed, m_pMoveSpeeds[i]);
			vLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pMoveSpeeds[i])));

			XMStoreFloat3(&vDir, XMVector3Normalize((XMLoadFloat3(&m_InstancingDesc.vCenter) - XMLoadFloat4(&pVertices[i].vPosition))));
			Add_Force(fTimeDelta, &vDir);
			Apply_DirTurn(fTimeDelta, &vDir, (_uint)i);

			break;
		case MOVE_CONTROL_TYPE::EASING:
			m_pMoveSpeeds[i] = CEasing::Ease_Float3((CEasing::EASE)m_InstancingDesc.iEasingType[VALUE_TYPE::MOVE],
				m_fTimeAcc, m_pMoveSpeeds[i], XMLoadFloat3(&m_InstancingDesc.vMove_Force) * fTimeDelta, m_pLifeTimes[i]);
			m_pMoveSpeeds[i] = Clamp(m_InstancingDesc.vMove_MinSpeed, m_InstancingDesc.vMove_MaxSpeed, m_pMoveSpeeds[i]);
			vLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pMoveSpeeds[i])));
			Add_Force(fTimeDelta, &m_pMoveSpeeds[i]);
			Apply_DirTurn(fTimeDelta, &m_pMoveSpeeds[i], (_uint)i);
			vDir = m_pMoveSpeeds[i];
			break;
		case MOVE_CONTROL_TYPE::MESH:
		{
			if (m_InstancingDesc.bMeshVTXParticle)
			{
				XMStoreFloat3(&m_pMoveSpeeds[i], XMLoadFloat3(&m_pMoveSpeeds[i]) + XMLoadFloat3(&m_InstancingDesc.vMove_Force) * fTimeDelta);
				m_pMoveSpeeds[i] = Clamp(m_InstancingDesc.vMove_MinSpeed, m_InstancingDesc.vMove_MaxSpeed, m_pMoveSpeeds[i]);
				vLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pMoveSpeeds[i])));

				_uint iMeshNextIndex = m_pMeshCurIndex[i] + 1;
				_bool bFree = { false };

				if (iMeshNextIndex >= m_InstancingDesc.vecMeshVTX.size())
				{
					iMeshNextIndex = 0;
					m_pMeshCurIndex[i] = iMeshNextIndex;
					bFree = true;
				}

				if (!bFree)
				{
					_vector vTargetPos = XMLoadFloat3(&m_InstancingDesc.vecMeshVTX[iMeshNextIndex]) * m_InstancingDesc.fMeshSize;
					_float fStand = 0.0001f * m_InstancingDesc.fMeshSize * vLength;

					if (XMVector3NearEqual(vTargetPos, XMLoadFloat4(&pVertices[i].vPosition), XMVectorSet(fStand, fStand, fStand, 0.f)))
					{
						m_pMeshCurIndex[i] = iMeshNextIndex;
					}
					else {
						XMStoreFloat3(&vDir, XMVector3Normalize(vTargetPos - XMLoadFloat4(&pVertices[i].vPosition)));
						Add_Force(fTimeDelta, &vDir);
						Apply_DirTurn(fTimeDelta, &vDir, (_uint)i);
					}
				}
				else {
					Add_Force(fTimeDelta, &m_pMoveSpeeds[i]);
					Apply_DirTurn(fTimeDelta, &m_pMoveSpeeds[i], (_uint)i);
					vDir = m_pMoveSpeeds[i];
					// 최대 인덱스에 도달하면 일반 Nor로 움직이게 한다.
				}
			}
		}
			break;
		case MOVE_CONTROL_TYPE::ZERO_VEC_DIR:
			{

			_float3 vZeroToward = XMLoadFloat4(&pVertices[i].vPosition) - XMVectorSetW(m_InstancingDesc.vCenter, 1.f);
			// 원점에서 현재 위치로 향하는 방향 벡터
			XMStoreFloat3(&m_pMoveSpeeds[i], XMLoadFloat3(&m_pMoveSpeeds[i]) + XMLoadFloat3(&m_InstancingDesc.vMove_Force) * fTimeDelta);
			m_pMoveSpeeds[i] = Clamp(m_InstancingDesc.vMove_MinSpeed, m_InstancingDesc.vMove_MaxSpeed, m_pMoveSpeeds[i]);
			vLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_pMoveSpeeds[i])));
			Add_Force(fTimeDelta, &vZeroToward);
			Apply_DirTurn(fTimeDelta, &vZeroToward, (_uint)i);
			vDir = vZeroToward;
			}
			break;
		default:
			break;
		}

		XMStoreFloat4(&pVertices[i].vPosition, XMLoadFloat4(&pVertices[i].vPosition)
			+ XMVector3Normalize(XMLoadFloat3(&vDir)) * m_InstancingDesc.fMoveAdjustSpeed * vLength * fTimeDelta);

		// Sprite
		SpriteAnim(fTimeDelta, (_uint)i, pVertices);

		pVertices[i].vPosition.w = 1.f;

		pVertices[i].vRight = _float4(m_pScales[i].x, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, m_pScales[i].y, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, m_pScales[i].z, 0.f);
		pVertices[i].vColor = m_pColors[i];

		// 방향 벡터 추가
		XMStoreFloat4(&pVertices[i].vDir, XMVector3Normalize(XMLoadFloat3(&vDir)));
		pVertices[i].vDir.w = 0.f;

		if (!m_InstancingDesc.bBurst) // 빌보드를 하지 않는 상태에서 방향 벡터에 맞게 회전
		{
			_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_pRotations[i].x, m_pRotations[i].y, m_pRotations[i].z);
			_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);
			XMStoreFloat4(&pVertices[i].vRight, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vRight), RotationMatrix));
			XMStoreFloat4(&pVertices[i].vUp, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vUp), RotationMatrix));
			XMStoreFloat4(&pVertices[i].vLook, XMVector3TransformNormal(XMLoadFloat4(&pVertices[i].vLook), RotationMatrix));
		}
		else
		{
			_vector		vUp = XMVector3Normalize(XMLoadFloat3(&m_pMoveSpeeds[i])) * m_pScales[i].y;
			vUp = XMVectorSetW(vUp, 0.f);
			_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vUp)) * m_pScales[i].x;
			vRight = XMVectorSetW(vRight, 0.f);
			_vector		vLook = XMVector3Normalize(XMVector3Cross(vUp, vRight)) * m_pScales[i].z;
			vLook = XMVectorSetW(vLook, 0.f);
			XMStoreFloat4(&pVertices[i].vRight, vRight);
			XMStoreFloat4(&pVertices[i].vUp, vUp);
			XMStoreFloat4(&pVertices[i].vLook, vLook);
		}
		
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Instancing::Render()
{
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instancing::Reset_AllParticles()
{
	D3D11_MAPPED_SUBRESOURCE			SubResource = {};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE* pVertices = ((VTXINSTANCE*)SubResource.pData);

	m_vRangeCenter = (m_InstancingDesc.vStartRange[0] + m_InstancingDesc.vStartRange[1]) / 2.f;

	if(m_InstancingDesc.bMeshVTXParticle)
		m_pGameInstance->RandomInt_Vector(m_vecMeshPosIndex, m_iNumInstance, 0, m_InstancingDesc.vecMeshVTX.size() - 1);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		Reset_Particle((_uint)i, &pVertices[i]);
	}

	Reset_Sprite();
	Reset_DirTurnTime();

	m_pContext->Unmap(m_pVBInstance, 0);

	m_fTimeAcc = 0.f;

	m_bIsFinished = false;
}


void CVIBuffer_Instancing::Reset_Particle(_uint i, VTXINSTANCE* pVertex)
{
	m_pMoveSpeeds[i] = m_pGameInstance->RandomFloat3(m_InstancingDesc.vMove_Speed[0], m_InstancingDesc.vMove_Speed[1]);
	m_pRotations[i] = m_pGameInstance->RandomFloat3(m_InstancingDesc.vRotation[0], m_InstancingDesc.vRotation[1]);
	m_pColors[i] = m_pGameInstance->RandomFloat4(m_InstancingDesc.vColor[0], m_InstancingDesc.vColor[1]);
	m_pLifeTimes[i] = m_pGameInstance->Random(m_InstancingDesc.vLifeTime.x, m_InstancingDesc.vLifeTime.y);

	// ------ Death Condition --------
	m_pLifeTimeAcc[i] = 0.f;

	if (m_InstancingDesc.bCube)
	{
		m_pScales[i].x = m_pScales[i].y = m_pScales[i].z = m_pGameInstance->Random(m_InstancingDesc.vScale[0].x, m_InstancingDesc.vScale[1].x);
	}
	else
		m_pScales[i] = m_pGameInstance->RandomFloat3(m_InstancingDesc.vScale[0], m_InstancingDesc.vScale[1]);

	m_pScaleSpeeds[i] = m_InstancingDesc.vScale_Speed;
	m_pRotationSpeeds[i] = m_InstancingDesc.vRotation_Speed;
	m_pColorSpeeds[i] = m_InstancingDesc.vColor_Speed;

	// -- Sprite ----
	m_pSpriteFinished[i] = false;
	m_pSpriteFrame[i] = 0.f;

	pVertex->vRight = _float4(m_pScales[i].x, 0.f, 0.f, 0.f);
	pVertex->vUp = _float4(0.f, m_pScales[i].y, 0.f, 0.f);
	pVertex->vLook = _float4(0.f, 0.f, m_pScales[i].z, 0.f);
	pVertex->vColor = m_pColors[i];

	_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_pRotations[i].x, m_pRotations[i].y, m_pRotations[i].z);
	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);
	XMVector3TransformNormal(XMLoadFloat4(&pVertex->vRight), RotationMatrix);
	XMVector3TransformNormal(XMLoadFloat4(&pVertex->vUp), RotationMatrix);
	XMVector3TransformNormal(XMLoadFloat4(&pVertex->vLook), RotationMatrix);

	_float3 vStartRange;

	if (!m_InstancingDesc.bMeshVTXParticle)
	{
		vStartRange = m_pGameInstance->RandomFloat3(m_InstancingDesc.vStartRange[0], m_InstancingDesc.vStartRange[1]);
		CutRange(&vStartRange);
	}
	else {
		_float3 vOffset = m_pGameInstance->RandomFloat3(m_InstancingDesc.vMeshPointOffset[0],
			m_InstancingDesc.vMeshPointOffset[1]);

		switch (m_InstancingDesc.eMeshStartType)
		{
		case MESH_START_TYPE::ZERO_POS:
		{
			vStartRange = (m_InstancingDesc.vecMeshVTX[0] * m_InstancingDesc.fMeshSize) + vOffset;
			m_pMeshCurIndex[i] = 0;
		}

		break;
		case MESH_START_TYPE::MESH_POS:
		{
			_uint iSize = (_uint)m_InstancingDesc.vecMeshVTX.size();
			_uint iIndex = i;
			if (iSize <= iIndex)
			{
				iIndex -= iSize * (_int)(iIndex / iSize);
			}
			m_pMeshCurIndex[i] = iIndex;
			vStartRange = (m_InstancingDesc.vecMeshVTX[iIndex] * m_InstancingDesc.fMeshSize) + vOffset;
		}
		break;

		case MESH_START_TYPE::RANDOM_POS:
		{
			_int iSize = (_int)m_InstancingDesc.vecMeshVTX.size();
			vStartRange = (m_InstancingDesc.vecMeshVTX[m_vecMeshPosIndex[i]] * m_InstancingDesc.fMeshSize) + vOffset;
		}
		break;
		}
	}

	XMStoreFloat4(&pVertex->vPosition, XMLoadFloat3(&m_InstancingDesc.vCenter) + XMLoadFloat3(&vStartRange));
	pVertex->vPosition.w = 1.f;

	pVertex->vTextureUV = _float2(0.f, 0.f);

}

_bool CVIBuffer_Instancing::Is_InRange(_float4 vPosition)
{

	switch (m_InstancingDesc.eMoveControl)
	{
	case MOVE_CONTROL_TYPE::NORMAL:

		return InRange(vPosition.x, m_InstancingDesc.vCenter.x + m_InstancingDesc.vOffsetRange[0].x, m_InstancingDesc.vCenter.x + m_InstancingDesc.vOffsetRange[1].x, "[]")
			&& InRange(vPosition.y, m_InstancingDesc.vCenter.y + m_InstancingDesc.vOffsetRange[0].y, m_InstancingDesc.vCenter.y + m_InstancingDesc.vOffsetRange[1].y, "[]")
			&& InRange(vPosition.z, m_InstancingDesc.vCenter.z + m_InstancingDesc.vOffsetRange[0].z, m_InstancingDesc.vCenter.z + m_InstancingDesc.vOffsetRange[1].z, "[]");

	case MOVE_CONTROL_TYPE::CENTER:
		
	{
		_float fStand = 0.03f;// * m_InstancingDesc.fMeshSize;

		if (XMVector3NearEqual(XMLoadFloat3(&m_InstancingDesc.vCenter), XMLoadFloat4(&vPosition), XMVectorSet(fStand,
			fStand, fStand, 0.f)))
			return false;// 현재 위치가 중심 위치와 비슷하면 false를 반환
		else
			return true;
	}
		 
	case MOVE_CONTROL_TYPE::EASING:
		
		return InRange(vPosition.x, m_InstancingDesc.vCenter.x + m_InstancingDesc.vOffsetRange[0].x, m_InstancingDesc.vCenter.x + m_InstancingDesc.vOffsetRange[1].x, "[]")
			&& InRange(vPosition.y, m_InstancingDesc.vCenter.y + m_InstancingDesc.vOffsetRange[0].y, m_InstancingDesc.vCenter.y + m_InstancingDesc.vOffsetRange[1].y, "[]")
			&& InRange(vPosition.z, m_InstancingDesc.vCenter.z + m_InstancingDesc.vOffsetRange[0].z, m_InstancingDesc.vCenter.z + m_InstancingDesc.vOffsetRange[1].z, "[]");

	case MOVE_CONTROL_TYPE::MESH:
	
		return InRange(vPosition.x, m_InstancingDesc.vCenter.x + m_InstancingDesc.vOffsetRange[0].x, m_InstancingDesc.vCenter.x + m_InstancingDesc.vOffsetRange[1].x, "[]")
			&& InRange(vPosition.y, m_InstancingDesc.vCenter.y + m_InstancingDesc.vOffsetRange[0].y, m_InstancingDesc.vCenter.y + m_InstancingDesc.vOffsetRange[1].y, "[]")
			&& InRange(vPosition.z, m_InstancingDesc.vCenter.z + m_InstancingDesc.vOffsetRange[0].z, m_InstancingDesc.vCenter.z + m_InstancingDesc.vOffsetRange[1].z, "[]");

	default:
		return true;
	}
}

void CVIBuffer_Instancing::Set_InstancingDesc(INSTANCING_DESC& NewDesc)
{
	m_InstancingDesc = NewDesc;
}

void CVIBuffer_Instancing::SpriteAnim(_cref_time fTimeDelta, _uint iIndex, VTXINSTANCE* pVertex)
{
	if (m_InstancingDesc.bSprite && !m_pSpriteFinished[iIndex])
	{
		m_pSpriteFrame[iIndex] += fTimeDelta * m_pSpriteSpeed[iIndex];

		if (m_pSpriteFrame[iIndex] >= m_InstancingDesc.fSpriteDuration)
		{
			pVertex[iIndex].vTextureUV.x += 1.f;

			if (pVertex[iIndex].vTextureUV.x >= m_InstancingDesc.vSpriteColRow.x)
			{

				pVertex[iIndex].vTextureUV.x = 0.f;
				pVertex[iIndex].vTextureUV.y += 1.f;

				if (pVertex[iIndex].vTextureUV.y >= m_InstancingDesc.vSpriteColRow.y)
				{
					// 맨 마지막을 탐
					if (m_InstancingDesc.bSprtieLoop)
					{
						pVertex[iIndex].vTextureUV.y = 0.f;
					}
					else {
						m_pSpriteFinished[iIndex] = true;
						// 루프가 아니면 끝냄
					}
				}
			}
			m_pSpriteFrame[iIndex] = 0.f;
		}
	}
}

void CVIBuffer_Instancing::Reset_Sprite()
{
	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		m_pSpriteFrame[i] = 0;
		m_pSpriteFinished[i] = false;
		m_pSprtieUV[i] = _float2(0.f, 0.f);
		m_pSpriteSpeed[i] = m_pGameInstance->Random(m_InstancingDesc.vSprite_Speed.x, m_InstancingDesc.vSprite_Speed.y);
	}
}

void CVIBuffer_Instancing::Apply_DirTurn(_cref_time fTimeDelta, _float3* vDir, _uint iIndex)
{
	if (m_InstancingDesc.bDirTurn)
	{
		if (m_fTimeAcc > m_InstancingDesc.fDirTurnWaitTime)
		{
			m_pDirTurnTimeAcc[iIndex] += fTimeDelta;
			if (m_pDirTurnTimeAcc[iIndex] >= m_pDirTurnTime[iIndex])
			{
				_float	fAngle = {};
				_uint iRadomNum = m_pGameInstance->RandomInt(1, 2);
				if (iRadomNum == 1)
					fAngle = m_InstancingDesc.fDirTurnAngle;
				if (iRadomNum == 2)
					fAngle = -m_InstancingDesc.fDirTurnAngle;

				_matrix	 matRot = XMMatrixRotationY(XMConvertToRadians(fAngle));
				_vector  vecTmpMoveSpeeds = XMVector3TransformNormal(XMLoadFloat3(vDir), matRot);
				XMStoreFloat3(vDir, XMVectorLerp(XMLoadFloat3(vDir), vecTmpMoveSpeeds, m_InstancingDesc.fDirTurnLerp));

				m_pDirTurnTimeAcc[iIndex] = 0.f;
			}
		}
	}
}

void CVIBuffer_Instancing::Reset_DirTurnTime()
{
	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		m_pDirTurnTimeAcc[i] = 0.f;
		m_pDirTurnTime[i] = m_pGameInstance->Random(m_InstancingDesc.vDirTurnTime.x, m_InstancingDesc.vDirTurnTime.y);
	}
}

void CVIBuffer_Instancing::CutRange(_float3* vStartRagne)
{
	
	_float fStartLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&(*vStartRagne - m_vRangeCenter))));
	
	if (fStartLength <= m_InstancingDesc.fCutRange)
	{

		_float fResultLenght = (fStartLength + (m_InstancingDesc.fCutRange));

		_float fMaxLenght = XMVectorGetX(XMVector3Length(XMLoadFloat3(&(m_InstancingDesc.vStartRange[1] - m_vRangeCenter))));

		if (fResultLenght > fMaxLenght)
			fResultLenght = fMaxLenght;

		(*vStartRagne) = XMVector3Normalize(XMLoadFloat3(vStartRagne)) * fResultLenght;

	}
}

void CVIBuffer_Instancing::Add_Force(_cref_time fTimeDelta, _float3* vDir)
{
	if (m_InstancingDesc.bForce)
	{		
		_matrix matWorldInverse = XMMatrixInverse(nullptr, static_pointer_cast<CBlendObject>(m_pOwner.lock())->Get_matWorld());
		m_pOwner.lock()->Get_TransformCom().lock()->Get_WorldMatrixInverse();
		_vector vForce = XMLoadFloat3(&m_InstancingDesc.vForceDir) * m_InstancingDesc.fForce * fTimeDelta * m_InstancingDesc.fForceSpeed;
		vForce = XMVector3TransformNormal(vForce, matWorldInverse);
		XMStoreFloat3(vDir, XMLoadFloat3(vDir) + vForce);
		// 강제 힘 적용
	}
}

_bool CVIBuffer_Instancing::Judge_Death(VTXINSTANCE* pVertex, _uint iIndex, _uint* iFinishNum)
{
	_bool bCheck = false;

	switch (m_InstancingDesc.eDeathType)
	{
	case DEATH_TYPE::DEATH_TIME:
		
		if (m_pLifeTimes[iIndex] <= m_pLifeTimeAcc[iIndex])
		{
			if (m_InstancingDesc.bLoop)
			{
				Reset_Particle(iIndex, &pVertex[iIndex]);
			}
			else {
				pVertex[iIndex].vColor.w = 0.f;
				++(*iFinishNum);
				bCheck = true; // 기준 시간이 지나면 인덱스 카운트
			}
		}
		
		break;
	case DEATH_TYPE::DEATH_SCALE:
		if (!InRange(m_pScales[iIndex].x, m_InstancingDesc.vScale_Min.x, m_InstancingDesc.vScale_Max.x, "()") &&
			!InRange(m_pScales[iIndex].y, m_InstancingDesc.vScale_Min.y, m_InstancingDesc.vScale_Max.y, "()") &&
			!InRange(m_pScales[iIndex].z, m_InstancingDesc.vScale_Min.z, m_InstancingDesc.vScale_Max.z, "()"))
		{
			if (m_InstancingDesc.bLoop)
			{
				Reset_Particle(iIndex, &pVertex[iIndex]);
			}
			else {
				pVertex[iIndex].vColor.w = 0.f;
				++(*iFinishNum);
				bCheck = true; // 기준 크기를 지나면 인덱스 카운트
			}
		}
		
		break;
	case DEATH_TYPE::DEATH_COLOR:
		if (m_pColors[iIndex].w <= 0.f)
		{
			if (m_InstancingDesc.bLoop)
			{
				Reset_Particle(iIndex, &pVertex[iIndex]);
			}
			else {
				pVertex[iIndex].vColor.w = 0.f;
				++(*iFinishNum);
				bCheck = true; // 기준 색을 지나면 인덱스 카운트
			}
		}
		
		break;
	case DEATH_TYPE::DEATH_RANGE:
		if (!Is_InRange(pVertex[iIndex].vPosition))
		{
			if (m_InstancingDesc.bLoop)
			{
				Reset_Particle(iIndex, &pVertex[iIndex]);
			}
			else {
				pVertex[iIndex].vColor.w = 0.f;
				++(*iFinishNum);
				bCheck = true; // 기준 범위를 지나면 인덱스 카운트
			}
		}
		break;
	default:
		break;
	}

	if (*iFinishNum == m_iNumInstance)
		m_bIsFinished = true;
	
	return bCheck;
}

void CVIBuffer_Instancing::Write_Json(json& Out_Json)
{
	Out_Json.clear();
	Out_Json.emplace("iNumInstance", m_InstancingDesc.iNumInstance);
	CJson_Utility::Write_Float3(Out_Json["vCenter"], m_InstancingDesc.vCenter);
	CJson_Utility::Write_Float3(Out_Json["vStartRange"][0], m_InstancingDesc.vStartRange[0]);
	CJson_Utility::Write_Float3(Out_Json["vStartRange"][1], m_InstancingDesc.vStartRange[1]);
	CJson_Utility::Write_Float3(Out_Json["vOffsetRange"][0], m_InstancingDesc.vOffsetRange[0]);
	CJson_Utility::Write_Float3(Out_Json["vOffsetRange"][1], m_InstancingDesc.vOffsetRange[1]);

	CJson_Utility::Write_Float3(Out_Json["vMove_Speed"][0], m_InstancingDesc.vMove_Speed[0]);
	CJson_Utility::Write_Float3(Out_Json["vMove_Speed"][1], m_InstancingDesc.vMove_Speed[1]);
	CJson_Utility::Write_Float3(Out_Json["vScale"][0], m_InstancingDesc.vScale[0]);
	CJson_Utility::Write_Float3(Out_Json["vScale"][1], m_InstancingDesc.vScale[1]);

	CJson_Utility::Write_Float3(Out_Json["vRotation"][0], m_InstancingDesc.vRotation[0]);
	CJson_Utility::Write_Float3(Out_Json["vRotation"][1], m_InstancingDesc.vRotation[1]);
	CJson_Utility::Write_Float4(Out_Json["vColor"][0], m_InstancingDesc.vColor[0]);
	CJson_Utility::Write_Float4(Out_Json["vColor"][1], m_InstancingDesc.vColor[1]);

	CJson_Utility::Write_Float3(Out_Json["vMove_Force"], m_InstancingDesc.vMove_Force);
	CJson_Utility::Write_Float3(Out_Json["vMove_MinSpeed"], m_InstancingDesc.vMove_MinSpeed);
	CJson_Utility::Write_Float3(Out_Json["vMove_MaxSpeed"], m_InstancingDesc.vMove_MaxSpeed);

	CJson_Utility::Write_Float3(Out_Json["vScale_Speed"], m_InstancingDesc.vScale_Speed);
	CJson_Utility::Write_Float3(Out_Json["vScale_Force"], m_InstancingDesc.vScale_Force);
	CJson_Utility::Write_Float3(Out_Json["vScale_Min"], m_InstancingDesc.vScale_Min);
	CJson_Utility::Write_Float3(Out_Json["vScale_Max"], m_InstancingDesc.vScale_Max);

	CJson_Utility::Write_Float3(Out_Json["vRotation_Speed"], m_InstancingDesc.vRotation_Speed);
	CJson_Utility::Write_Float3(Out_Json["vRotation_Force"], m_InstancingDesc.vRotation_Force);
	CJson_Utility::Write_Float3(Out_Json["vRotation_Min"], m_InstancingDesc.vRotation_Min);
	CJson_Utility::Write_Float3(Out_Json["vRotation_Max"], m_InstancingDesc.vRotation_Max);

	CJson_Utility::Write_Float4(Out_Json["vColor_Speed"], m_InstancingDesc.vColor_Speed);
	CJson_Utility::Write_Float4(Out_Json["vColor_Force"], m_InstancingDesc.vColor_Force);
	CJson_Utility::Write_Float4(Out_Json["vColor_Max"], m_InstancingDesc.vColor_Max);

	CJson_Utility::Write_Float2(Out_Json["vLifeTime"], m_InstancingDesc.vLifeTime);
	
	Out_Json.emplace("bLoop", m_InstancingDesc.bLoop);
	Out_Json.emplace("bCube", m_InstancingDesc.bCube);
	Out_Json.emplace("fSpawnTime", m_InstancingDesc.fSpawnTime);
	Out_Json.emplace("fMoveAdjustSpeed", m_InstancingDesc.fMoveAdjustSpeed);
	Out_Json.emplace("bBurst", m_InstancingDesc.bBurst);

	Out_Json.emplace("fCutRange", m_InstancingDesc.fCutRange);

	// ------ Easing ---------

	Out_Json.emplace("bMoveEasing", m_InstancingDesc.bEasing[VALUE_TYPE::MOVE]);
	Out_Json.emplace("bScaleEasing", m_InstancingDesc.bEasing[VALUE_TYPE::SCALE]);
	Out_Json.emplace("bRotationEasing", m_InstancingDesc.bEasing[VALUE_TYPE::ROTATION]);
	Out_Json.emplace("bColorEasing", m_InstancingDesc.bEasing[VALUE_TYPE::COLOR]);

	Out_Json.emplace("iMoveEasingType", m_InstancingDesc.iEasingType[VALUE_TYPE::MOVE]);
	Out_Json.emplace("iScaleEasingType", m_InstancingDesc.iEasingType[VALUE_TYPE::SCALE]);
	Out_Json.emplace("iRotationEasingType", m_InstancingDesc.iEasingType[VALUE_TYPE::ROTATION]);
	Out_Json.emplace("iColorEasingType", m_InstancingDesc.iEasingType[VALUE_TYPE::COLOR]);

	// ------- Sprite ---------

	Out_Json.emplace("bSprite", m_InstancingDesc.bSprite);
	Out_Json.emplace("bSprtieLoop", m_InstancingDesc.bSprtieLoop);
	CJson_Utility::Write_Float2(Out_Json["vSpriteColRow"], m_InstancingDesc.vSpriteColRow);
	Out_Json.emplace("fSpriteDuration", m_InstancingDesc.fSpriteDuration);

	CJson_Utility::Write_Float2(Out_Json["fSprite_Speed"], m_InstancingDesc.vSprite_Speed);

	// ------ MaskArray --------

	Out_Json.emplace("iMaskArrayMaxIndex", m_InstancingDesc.iMaskArrayMaxIndex);

	// ------- DirTurn -----------

	Out_Json.emplace("bDirTurn", m_InstancingDesc.bDirTurn);
	Out_Json.emplace("fDirTurnLerp", m_InstancingDesc.fDirTurnLerp);
	Out_Json.emplace("fDirTurnAngle", m_InstancingDesc.fDirTurnAngle);
	Out_Json.emplace("fDirTurnWaitTime", m_InstancingDesc.fDirTurnWaitTime);
	CJson_Utility::Write_Float2(Out_Json["vDirTurnTime"], m_InstancingDesc.vDirTurnTime);

	// ------ MeshParticle -----------

	Out_Json.emplace("bMeshVTXParticle", m_InstancingDesc.bMeshVTXParticle);
	Out_Json.emplace("eMeshStartType", (_int)m_InstancingDesc.eMeshStartType);
	CJson_Utility::Write_Float3(Out_Json["vMeshPointOffset"][0], m_InstancingDesc.vMeshPointOffset[0]);
	CJson_Utility::Write_Float3(Out_Json["vMeshPointOffset"][1], m_InstancingDesc.vMeshPointOffset[1]);
	Out_Json.emplace("fMeshSize", m_InstancingDesc.fMeshSize);
	
	if (m_InstancingDesc.bMeshVTXParticle)
	{
		_uint iSize = (_uint)m_InstancingDesc.vecMeshVTX.size();

		for (_uint i = 0; i < iSize; i++)
		{
			CJson_Utility::Write_Float3(Out_Json["vecMeshVTX"][i], m_InstancingDesc.vecMeshVTX[i]);
		}
	}

	// ----- MOVE_CONTROL_TYPE -------

	Out_Json.emplace("eMoveControl", (_int)m_InstancingDesc.eMoveControl);

	// ------ Death Condition --------

	Out_Json.emplace("eDeathType", (_int)m_InstancingDesc.eDeathType);

	// ------ Force --------

	Out_Json.emplace("bForce", m_InstancingDesc.bForce);
	CJson_Utility::Write_Float3(Out_Json["vForceDir"], m_InstancingDesc.vForceDir);
	Out_Json.emplace("fForce", m_InstancingDesc.fForce);
	Out_Json.emplace("fForceSpeed", m_InstancingDesc.fForceSpeed);
	
}

void CVIBuffer_Instancing::Load_Json(const json& In_Json)
{
	m_InstancingDesc.iNumInstance = In_Json["iNumInstance"];
	for(_int i = 0; i < 2; i++)
	{
		m_InstancingDesc.vStartRange[i] = _float3(In_Json["vStartRange"][i][0], In_Json["vStartRange"][i][1], In_Json["vStartRange"][i][2]);

		m_InstancingDesc.vOffsetRange[i] = _float3(In_Json["vOffsetRange"][i][0], In_Json["vOffsetRange"][i][1], In_Json["vOffsetRange"][i][2]);

		m_InstancingDesc.vMove_Speed[i] = _float3(In_Json["vMove_Speed"][i][0], In_Json["vMove_Speed"][i][1], In_Json["vMove_Speed"][i][2]);

		m_InstancingDesc.vScale[i] = _float3(In_Json["vScale"][i][0], In_Json["vScale"][i][1], In_Json["vScale"][i][2]);

		m_InstancingDesc.vOffsetRange[i] = _float3(In_Json["vOffsetRange"][i][0], In_Json["vOffsetRange"][i][1], In_Json["vOffsetRange"][i][2]);

		m_InstancingDesc.vRotation[i] = _float3(In_Json["vRotation"][i][0], In_Json["vRotation"][i][1], In_Json["vRotation"][i][2]);

		m_InstancingDesc.vColor[i] = _float4(In_Json["vColor"][i][0], In_Json["vColor"][i][1], In_Json["vColor"][i][2], In_Json["vColor"][i][3]);

	}
	m_InstancingDesc.vCenter = _float3(In_Json["vCenter"][0], In_Json["vCenter"][1], In_Json["vCenter"][2]);
	m_InstancingDesc.vMove_Force = _float3(In_Json["vMove_Force"][0], In_Json["vMove_Force"][1], In_Json["vMove_Force"][2]);	
	m_InstancingDesc.vMove_MinSpeed = _float3(In_Json["vMove_MinSpeed"][0], In_Json["vMove_MinSpeed"][1], In_Json["vMove_MinSpeed"][2]);
	m_InstancingDesc.vMove_MaxSpeed = _float3(In_Json["vMove_MaxSpeed"][0], In_Json["vMove_MaxSpeed"][1], In_Json["vMove_MaxSpeed"][2]);

	m_InstancingDesc.vScale_Speed = _float3(In_Json["vScale_Speed"][0], In_Json["vScale_Speed"][1], In_Json["vScale_Speed"][2]);
	m_InstancingDesc.vScale_Force = _float3(In_Json["vScale_Force"][0], In_Json["vScale_Force"][1], In_Json["vScale_Force"][2]);
	m_InstancingDesc.vScale_Min = _float3(In_Json["vScale_Min"][0], In_Json["vScale_Min"][1], In_Json["vScale_Min"][2]);
	m_InstancingDesc.vScale_Max = _float3(In_Json["vScale_Max"][0], In_Json["vScale_Max"][1], In_Json["vScale_Max"][2]);

	m_InstancingDesc.vRotation_Speed = _float3(In_Json["vRotation_Speed"][0], In_Json["vRotation_Speed"][1], In_Json["vRotation_Speed"][2]);
	m_InstancingDesc.vRotation_Force = _float3(In_Json["vRotation_Force"][0], In_Json["vRotation_Force"][1], In_Json["vRotation_Force"][2]);
	m_InstancingDesc.vRotation_Min = _float3(In_Json["vRotation_Min"][0], In_Json["vRotation_Min"][1], In_Json["vRotation_Min"][2]);
	m_InstancingDesc.vRotation_Max = _float3(In_Json["vRotation_Max"][0], In_Json["vRotation_Max"][1], In_Json["vRotation_Max"][2]);

	m_InstancingDesc.vColor_Speed = _float4(In_Json["vColor_Speed"][0], In_Json["vColor_Speed"][1], In_Json["vColor_Speed"][2], In_Json["vColor_Speed"][3]);
	m_InstancingDesc.vColor_Force = _float4(In_Json["vColor_Force"][0], In_Json["vColor_Force"][1], In_Json["vColor_Force"][2], In_Json["vColor_Force"][3]);
	m_InstancingDesc.vColor_Max = _float4(In_Json["vColor_Max"][0], In_Json["vColor_Max"][1], In_Json["vColor_Max"][2], In_Json["vColor_Max"][3]);

	m_InstancingDesc.vLifeTime = _float2(In_Json["vLifeTime"][0], In_Json["vLifeTime"][1]);
	m_InstancingDesc.bLoop = In_Json["bLoop"];
	m_InstancingDesc.bCube = In_Json["bCube"];
	m_InstancingDesc.fSpawnTime = In_Json["fSpawnTime"];
	m_InstancingDesc.fMoveAdjustSpeed = In_Json["fMoveAdjustSpeed"];
	m_InstancingDesc.bBurst = In_Json["bBurst"];

	m_InstancingDesc.fCutRange = In_Json["fCutRange"];

	// ------ Easing ---------

	m_InstancingDesc.bEasing[VALUE_TYPE::MOVE] = In_Json["bMoveEasing"];
	m_InstancingDesc.bEasing[VALUE_TYPE::SCALE] = In_Json["bScaleEasing"];
	m_InstancingDesc.bEasing[VALUE_TYPE::ROTATION] = In_Json["bRotationEasing"];
	m_InstancingDesc.bEasing[VALUE_TYPE::COLOR] = In_Json["bColorEasing"];

	m_InstancingDesc.iEasingType[VALUE_TYPE::MOVE] = In_Json["iMoveEasingType"];
	m_InstancingDesc.iEasingType[VALUE_TYPE::SCALE] = In_Json["iScaleEasingType"];
	m_InstancingDesc.iEasingType[VALUE_TYPE::ROTATION] = In_Json["iRotationEasingType"];
	m_InstancingDesc.iEasingType[VALUE_TYPE::COLOR] = In_Json["iColorEasingType"];

	// ------- Sprite ---------

	m_InstancingDesc.bSprite = In_Json["bSprite"];
	m_InstancingDesc.bSprtieLoop = In_Json["bSprtieLoop"];
	CJson_Utility::Load_Float2(In_Json["vSpriteColRow"], m_InstancingDesc.vSpriteColRow);
	m_InstancingDesc.fSpriteDuration = In_Json["fSpriteDuration"];
	CJson_Utility::Load_Float2(In_Json["fSprite_Speed"], m_InstancingDesc.vSprite_Speed);

	// ------ MaskArray --------

	m_InstancingDesc.iMaskArrayMaxIndex = In_Json["iMaskArrayMaxIndex"];

	// ------- DirTurn -----------

	m_InstancingDesc.bDirTurn = In_Json["bDirTurn"];
	m_InstancingDesc.fDirTurnLerp = In_Json["fDirTurnLerp"];
	m_InstancingDesc.fDirTurnAngle = In_Json["fDirTurnAngle"];
	m_InstancingDesc.fDirTurnWaitTime = In_Json["fDirTurnWaitTime"];
	CJson_Utility::Load_Float2(In_Json["vDirTurnTime"], m_InstancingDesc.vDirTurnTime);

	// ------ MeshParticle -----------

	m_InstancingDesc.bMeshVTXParticle = In_Json["bMeshVTXParticle"];
	m_InstancingDesc.eMeshStartType = (MESH_START_TYPE)In_Json["eMeshStartType"];
	CJson_Utility::Load_Float3(In_Json["vMeshPointOffset"][0], m_InstancingDesc.vMeshPointOffset[0]);
	CJson_Utility::Load_Float3(In_Json["vMeshPointOffset"][1], m_InstancingDesc.vMeshPointOffset[1]);
	m_InstancingDesc.fMeshSize = In_Json["fMeshSize"];
	
	if (m_InstancingDesc.bMeshVTXParticle)
	{
		_uint iIndex = 0;

		for (auto& iter : In_Json["vecMeshVTX"])
		{
			m_InstancingDesc.vecMeshVTX.push_back(_float3(In_Json["vecMeshVTX"][iIndex][0], In_Json["vecMeshVTX"][iIndex][1],
				In_Json["vecMeshVTX"][iIndex][2]));
			++iIndex;
		}
	}

	// ----- MOVE_CONTROL_TYPE -------

	m_InstancingDesc.eMoveControl = (MOVE_CONTROL_TYPE)In_Json["eMoveControl"];

	// ------ Death Condition --------
	  
	m_InstancingDesc.eDeathType = (DEATH_TYPE)In_Json["eDeathType"];

	// ------ Force --------

	m_InstancingDesc.bForce = In_Json["bForce"];
	CJson_Utility::Load_Float3(In_Json["vForceDir"], m_InstancingDesc.vForceDir);
	m_InstancingDesc.fForce = In_Json["fForce"];
	m_InstancingDesc.fForceSpeed = In_Json["fForceSpeed"];
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();
	
	Safe_Delete_Array(m_pMoveSpeeds);
	Safe_Delete_Array(m_pScaleSpeeds);
	Safe_Delete_Array(m_pRotationSpeeds);
	Safe_Delete_Array(m_pColorSpeeds);
	Safe_Delete_Array(m_pScales);
	Safe_Delete_Array(m_pRotations);
	Safe_Delete_Array(m_pColors);
	Safe_Delete_Array(m_pLifeTimes);

	Safe_Delete_Array(m_pSprtieUV);
	Safe_Delete_Array(m_pSpriteFrame);
	Safe_Delete_Array(m_pSpriteSpeed);
	Safe_Delete_Array(m_pSpriteFinished);

	/* For. RandomDirection*/
	Safe_Delete_Array(m_pDirTurnTimeAcc);
	Safe_Delete_Array(m_pDirTurnTime);

	// ------ MeshParticle -----------
	Safe_Delete_Array(m_pMeshCurIndex);

	// ------ Death Condition --------
	Safe_Delete_Array(m_pLifeTimeAcc);

	Safe_Release(m_pVBInstance);
}
