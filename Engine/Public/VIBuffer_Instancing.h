#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
public:
	enum VALUE_TYPE {MOVE,SCALE,ROTATION,COLOR,VALUE_END};
	enum MOVE_CONTROL_TYPE {NORMAL,CENTER,EASING,MESH,ZERO_VEC_DIR,TYPE_END};
	enum DEATH_TYPE {DEATH_TIME,DEATH_SCALE,DEATH_COLOR,DEATH_RANGE,DEATH_OWNER,DEATH_TYPE_END};
	enum MESH_START_TYPE {ZERO_POS,MESH_POS,RANDOM_POS,START_END};

	typedef struct tag_Instnacing_Desc
	{
		_int		iNumInstance = {100};
		_float3		vCenter = {0.f,0.f,0.f};

		_float3		vStartRange[2] = { {0.f,0.f,0.f} ,{0.f,0.f,0.f} };
		_float		fCutRange = { 0.f };
		_float3		vOffsetRange[2] = { {-1.f,-1.f,-1.f} ,{1.f,1.f,1.f} };

		_float3		vMove_Speed[2] = { {-0.1f,0.1f,-0.1f} ,{0.1f,0.1f,0.1f} };
		_float3		vMove_Force = { 0.f,0.f,0.f };
		_float3		vMove_MinSpeed = { -100.f,-100.f,-100.f };
		_float3		vMove_MaxSpeed = { 100.f,100.f,100.f };
		_float		fMoveAdjustSpeed = { 1.f };

		_float3		vScale[2] = { {1.f,1.f,1.f} ,{1.f,1.f,1.f} };
		_float3		vScale_Speed = { 0.f,0.f,0.f };
		_float3		vScale_Force = { 0.f,0.f,0.f };
		_float3		vScale_Min = { 0.f,0.f,0.f };
		_float3		vScale_Max = { 100.f,100.f,100.f };

		_float3		vRotation[2] = { {0.f,0.f,0.f} ,{0.f,0.f,0.f} };
		_float3		vRotation_Speed = { 0.f,0.f,0.f };
		_float3		vRotation_Force = { 0.f,0.f,0.f };
		_float3		vRotation_Min = { -100.f,-100.f,-100.f };
		_float3		vRotation_Max = { 100.f,100.f,100.f };

		_float4		vColor[2] = { {1.f,1.f,1.f,1.f} ,{1.f,1.f,1.f,1.f} };
		_float4		vColor_Speed = { 0.f,0.f,0.f,0.f };
		_float4		vColor_Force = { 0.f,0.f,0.f,0.f };
		_float4		vColor_Max = { 1.f,1.f,1.f,1.f };

		_float2		vLifeTime = {0.f,10.f};
		_bool		bLoop = { true };
		_bool		bCube = { true };
		_bool		bBurst = { false };
		_float		fSpawnTime = { 0.5f };

		// ----- Easing -------
		_bool		bEasing[VALUE_END] = { false,false,false,false };
		_uint		iEasingType[VALUE_END] = {0,0,0,0}; // easing 추가

		// ---- Sprite ---------
		_bool		bSprite = { false };
		_bool		bSprtieLoop = { false };
		_float2		vSpriteColRow = { 1.f,1.f };
		_float		fSpriteDuration = { 0.f };

		_float2		vSprite_Speed = { 1.f,1.f };

		// ------ MaskArray --------
		_uint		iMaskArrayMaxIndex = { 0 };

		// ------ TurnDirection --------
		_bool	bDirTurn = { false };
		_float	fDirTurnLerp = { 1.f };
		_float	fDirTurnAngle = { 0.f };
		_float	fDirTurnWaitTime = { 0.f };
		_float2 vDirTurnTime = { 0.f,0.f };

		// ------ MeshParticle -----------
		_bool	bMeshVTXParticle = { false };
		MESH_START_TYPE eMeshStartType = { MESH_START_TYPE::ZERO_POS};
		_float3 vMeshPointOffset[2] = {}; // 메시 정점을 기준으로 오프셋 만큼 위치를 더해준다.
		_float	fMeshSize = { 1.f };
		vector<_float3> vecMeshVTX;

		// ----- MOVE_CONTROL_TYPE -------
		MOVE_CONTROL_TYPE eMoveControl = { MOVE_CONTROL_TYPE::TYPE_END };

		// ------ Death Condition --------
		DEATH_TYPE eDeathType = { DEATH_TYPE::DEATH_TYPE_END };

		// ------ Force --------
		_bool	bForce = { false };
		_float3 vForceDir = { 0.f,-1.f,0.f };
		_float	fForce = { 9.8f };
		_float  fForceSpeed = { 0.0341f };

	}INSTANCING_DESC;

protected:
	CVIBuffer_Instancing(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_VIBuffers() override;
	virtual void Update(_cref_time fTimeDelta);
	virtual HRESULT Render();

	void Reset_Particle(_uint i, VTXINSTANCE* pVertex);
	void Reset_AllParticles();
	_bool Is_InRange(_float4 vPosition);
	void Set_InstancingDesc(INSTANCING_DESC& NewDesc);
	INSTANCING_DESC* Get_InstancingDesc() { return &m_InstancingDesc; }

	void	SpriteAnim(_cref_time fTimeDelta,_uint iIndex, VTXINSTANCE* pVertex);
	void	Reset_Sprite();

	void	Apply_DirTurn(_cref_time fTimeDelta,_float3* vDir, _uint iIndex);
	void	Reset_DirTurnTime();

	void	CutRange(_float3* vStartRagne);

	void	Add_Force(_cref_time fTimeDelta, _float3* vDir);

	_bool	Judge_Death(VTXINSTANCE* pVertex, _uint iIndex,_uint* iFinishNum);

protected:
	ID3D11Buffer* m_pVBInstance = { nullptr };
	_uint						m_iInstanceStride = { 0 };
	_uint						m_iNumInstance = { 0 };
	_uint						m_iIndexCountPerInstance = { 0 };

protected:
	_float3* m_pMoveSpeeds = { nullptr };
	_float3* m_pScaleSpeeds = { nullptr };
	_float3* m_pRotationSpeeds = { nullptr };
	_float4* m_pColorSpeeds = { nullptr };
	_float3* m_pScales = { nullptr };
	_float3* m_pRotations = { nullptr };
	_float4* m_pColors = { nullptr };
	_float*	 m_pLifeTimes = { nullptr };
	weak_ptr<class CTransform> m_pOwnerTransformCom;

	INSTANCING_DESC				m_InstancingDesc;
	_float						m_fTimeAcc = { 0.0f };

protected:
	/* For. Sprite*/
	_float2*	m_pSprtieUV = { nullptr };
	_float*		m_pSpriteFrame = { nullptr };
	_float*		m_pSpriteSpeed = { nullptr };
	_bool*		m_pSpriteFinished = { nullptr };

	/* For. RandomDirection*/
	_float*		m_pDirTurnTimeAcc = { nullptr };
	_float*		m_pDirTurnTime = { nullptr };

	/* For. Direction*/
	_float3		m_vRangeCenter = { 0.f,0.f,0.f };

	// ------ MeshParticle -----------
	_uint*		m_pMeshCurIndex = {nullptr}; // 현재 인스턴스가 위치하는 메시 정점의 인덱스

	// ------ Death Condition --------
	_float*						m_pLifeTimeAcc = { nullptr };
	_bool						m_bIsFinished = { false };// 파티클 죽음처리

private:
	vector<_int>				m_vecMeshPosIndex;	// 메쉬 위치 인덱스를 저장

public:
	_bool Is_Finished() { return m_bIsFinished; }
public:
	virtual void Write_Json(json& Out_Json);
	virtual void Load_Json(const json& In_Json);

public:
	virtual shared_ptr<CComponent> Clone(void* pArg) = 0;
	virtual void Free() override;
};

END